float4x4 matWorld; //월드행렬
float4x4 matViewProjection; //뷰 투영행렬
float4x4 matLightViewProjection; //광원에서 바라보면 뷰 투영행렬

float3 vCameraPos; //카메라의 위치
float3 vLightDir; //빛의 방향
float3 vLightColor; //빛의 색상
float bias = 0.01f; //그림자맵 바이어스값
float fSpecPower;
float camNear; //근거리 카메라
float camFar; //원거리 카메라
float4 vFog;
float4 vFogColor;

texture Diffuse_Tex;
sampler2D DiffuseSampler = sampler_state
{
	texture = <Diffuse_Tex>;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
};

texture Normal_Tex;
sampler2D NormalSampler = sampler_state
{
	texture = <Normal_Tex>;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
};

texture Specular_Tex;
sampler2D SpecularSampler = sampler_state
{
	texture = <Specular_Tex>;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
};

texture Shadow_Tex;
sampler2D ShadowSampler = sampler_state
{
	texture = <Shadow_Tex>;
	ADDRESSU = BORDER;
	ADDRESSV = BORDER;
	BORDERCOLOR = 0xFFFFFFFF;
};

struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 Binormal : BINORMAL0;
	float3 Tangent : TANGENT0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal	: TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 ViewDir : TEXCOORD4;
	float4 FinalPosition : TEXCOORD5;
	float Fog : FOG;
};


VS_OUTPUT VS_Base(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 worldPos = mul(Input.Position, matWorld);
	Output.Position = mul(worldPos, matViewProjection);

	Output.TexCoord = Input.TexCoord;

	Output.Normal = mul(Input.Normal, (float3x3)matWorld);
	Output.Binormal = mul(Input.Binormal, (float3x3)matWorld);
	Output.Tangent = mul(Input.Tangent, (float3x3)matWorld);

	Output.ViewDir = vCameraPos - worldPos.xyz;

	Output.FinalPosition = Output.Position;

	Output.Fog = vFog.x + Output.Position.z*vFog.y; //선형포그

	if (Output.Fog < 0.f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.f;

	return Output;
}

//픽셀셰이더 출력 구조체
struct PS_OUTPUT
{
	float4 baseColor : COLOR0;			//0번 스테이지 컬러
	float4 normalDepth : COLOR1;		//1번 스테이지 컬러 ( RGB 노말, A 뎁스 )
};

PS_OUTPUT PS_Base(VS_OUTPUT Input) : COLOR0
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 diffTex = tex2D(DiffuseSampler, Input.TexCoord);
	clip(diffTex.a - 0.1f);
	
	float3x3 TBN = float3x3(
		normalize(Input.Tangent),
		normalize(Input.Binormal),
		normalize(Input.Normal));
	
	//WorldNormal
	float3 normalColor = tex2D(NormalSampler, Input.TexCoord).rgb;
	
	//Tangent Space Normal
	float3 spaceNormal = (normalColor*2.0f) - 1.0f;
	
	float3 worldNormal = mul(spaceNormal, TBN);
	worldNormal = normalize(worldNormal);
	
	float3 viewDir = normalize(Input.ViewDir);
	float3 diffuseColor = float3(0, 0, 0);
	float3 specularColor = float3(0, 0, 0);
	
	float3 dir = normalize(vLightDir);
	float3 lightDir = -dir;
	
	float NdotL = dot(lightDir, worldNormal); //음영
	
	float diffuse = NdotL;
	
	diffuse = max(0.2f, diffuse);		//Ambient 는 0.2f
	
	float3 lightReflection = normalize(dir + 2.0f*NdotL*worldNormal);
	float specular = saturate(dot(lightReflection, viewDir));
	specular = pow(specular, fSpecPower);
	
	diffuseColor = vLightColor * diffuse;
	specularColor = vLightColor * specular * diffuse;
	
	float3 FinalDiffuseColor = diffTex.rgb*diffuseColor;
	float3 FinalSpecularColor = tex2D(SpecularSampler, Input.TexCoord).rgb*specularColor;
	
	float3 FinalColor = FinalDiffuseColor + FinalSpecularColor;
	
	//행렬변환을 거친 값 z 에 행렬변환에서 얻는 가중치 w 를 나누면 0 ~ 1 사이의 깊이 값이 된다.
	float depth = Input.FinalPosition.z / Input.FinalPosition.w;
	
	
	//위의 depth 값을 카메라의 near 와 far 를 이용하여 선형으로 펴준다....
	//Perspective Projection Linear Depth
	float z = depth;
	float a = camFar / (camFar - camNear);
	float b = -camNear / (camFar - camNear);
	depth = b / (z - a);
	
	float4 Color = float4(FinalColor, 1);
	
	Output.baseColor = Input.Fog* Color + (1 - Input.Fog)*vFogColor;
	Output.normalDepth = float4(worldNormal, depth);		//alpha 값에 뎁스를 썼다.

	return Output;
}



struct VS_INPUT_SHADOW //그림자 생성 버텍스 입력 구조체
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT_SHADOW //그림자 생성 버텍스 출력 구조체
{
	float4 Position : POSITION0;
	float4 FinalPosition : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};

////////////////////////////////CreateShadow/////////////////////////////////////////////////

VS_OUTPUT_SHADOW VS_CreateShadow(VS_INPUT_SHADOW Input) //그림자맵 생성 버텍스 쉐이더
{
	VS_OUTPUT_SHADOW Output = (VS_OUTPUT_SHADOW)0;

	float4 worldPos = mul(Input.Position, matWorld);
	Output.Position = mul(worldPos, matViewProjection);

	Output.FinalPosition = Output.Position;

	Output.TexCoord = Input.TexCoord;

	return Output;
}

struct PS_INPUT_SHADOW //그림자맵 생성 픽셀 입력 구조체
{
	float4 FinalPosition : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};

float4 PS_CreateShadow(PS_INPUT_SHADOW Input) : COLOR0 //그림자맵 생성 픽셀 쉐이더
{
	float depth = Input.FinalPosition.z / Input.FinalPosition.w;

	float4 diffTex = tex2D(DiffuseSampler, Input.TexCoord);
	clip(diffTex.a - 0.1f); //나뭇잎 텍스처에서 주변 초록텍스처 제외

	return float4(depth.xxx, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////ReciveShadow/////////////////////////////////////////////////

struct VS_INPUT_RECIVESHADOW //그림자 렌더 버텍스 입력 구조체
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 Binormal : BINORMAL0;
	float3 Tangent : TANGENT0;
};

struct VS_OUTPUT_RECIVESHADOW //그림자 렌더 버텍스 출력 구조체
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 ViewDir : TEXCOORD4;
	float4 FinalPosition : TEXCOORD5;
	float4 LightClipPosition : TEXCOORD6;
	float Fog : FOG;
};

VS_OUTPUT_RECIVESHADOW VS_ReciveShadow(VS_INPUT_RECIVESHADOW Input)
{
	VS_OUTPUT_RECIVESHADOW Output = (VS_OUTPUT_RECIVESHADOW)0;

	float4 worldPos = mul(Input.Position, matWorld);
	Output.Position = mul(worldPos, matViewProjection);

	Output.TexCoord = Input.TexCoord;

	Output.Normal = mul(Input.Normal, (float3x3)matWorld);
	Output.Binormal = mul(Input.Binormal, (float3x3)matWorld);
	Output.Tangent = mul(Input.Tangent, (float3x3)matWorld);

	Output.ViewDir = vCameraPos - worldPos.xyz;

	Output.FinalPosition = Output.Position;
	Output.LightClipPosition = mul(worldPos, matLightViewProjection);

	Output.Fog = vFog.x + Output.Position.z*vFog.y; //선형포그

	if (Output.Fog < 0.f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.f;

	return Output;
}

PS_OUTPUT PS_ReciveShadow(VS_OUTPUT_RECIVESHADOW Input) : COLOR0
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 diffTex = tex2D(DiffuseSampler, Input.TexCoord);
	clip(diffTex.a - 0.1f);

	float lightDepth = Input.LightClipPosition.z / Input.LightClipPosition.w;

	//(-1 ~ 1)의 범위로 맞춘다.
	float2 shadowUV = Input.LightClipPosition.xy / Input.LightClipPosition.w;
	shadowUV.y = -shadowUV.y; //y값 반전

	//(0 ~ 1)의 범위로 맞춘다.
	shadowUV = (shadowUV*0.5f) + 0.5f;

	float shadowDepth = tex2D(ShadowSampler, shadowUV).r; //깊이값

	//TBN Matrix(텍스처 공간)
	float3x3 TBN = float3x3(
		normalize(Input.Tangent),
		normalize(Input.Binormal),
		normalize(Input.Normal));

	//WorldNormal
	float3 normalColor = tex2D(NormalSampler, Input.TexCoord).rgb;

	//Tangent Space Normal
	float3 spaceNormal = (normalColor*2.0f) - 1.0f;

	float3 worldNormal = mul(spaceNormal, TBN);
	worldNormal = normalize(worldNormal);

	float3 viewDir = normalize(Input.ViewDir);
	float3 diffuseColor = float3(0, 0, 0);
	float3 specularColor = float3(0, 0, 0);

	float3 dir = normalize(vLightDir);
	float3 lightDir = -dir;

	float NdotL = dot(lightDir, worldNormal); //음영

	float diffuse = NdotL;

	if (shadowDepth + bias < lightDepth && lightDepth < 1.0f)
		diffuse = abs(diffuse)* -1.0f;

	diffuse = max(0.2f, diffuse);		//Ambient 는 0.2f

	float3 lightReflection = normalize(dir + 2.0f*NdotL*worldNormal);
	float specular = saturate(dot(lightReflection, viewDir));
	specular = pow(specular, fSpecPower);

	diffuseColor = vLightColor * diffuse;
	specularColor = vLightColor * specular * diffuse;

	float3 FinalDiffuseColor = diffTex.rgb*diffuseColor;
	float3 FinalSpecularColor = tex2D(SpecularSampler, Input.TexCoord).rgb*specularColor;

	float3 FinalColor = FinalDiffuseColor + FinalSpecularColor;

	//행렬변환을 거친 값 z 에 행렬변환에서 얻는 가중치 w 를 나누면 0 ~ 1 사이의 깊이 값이 된다.
	float depth = Input.FinalPosition.z / Input.FinalPosition.w;


	//위의 depth 값을 카메라의 near 와 far 를 이용하여 선형으로 펴준다....
	//Perspective Projection Linear Depth
	float z = depth;
	float a = camFar / (camFar - camNear);
	float b = -camNear / (camFar - camNear);
	depth = b / (z - a);


	float4 Color = float4(FinalColor, 1);

	Output.baseColor = Input.Fog* Color + (1 - Input.Fog)*vFogColor;
	Output.normalDepth = float4(worldNormal, depth);		//alpha 값에 뎁스를 썼다.

	return Output;
}

technique Shadow
{
	pass Base
	{
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_Base();
	}

	pass CreateShadow
	{
		VertexShader = compile vs_3_0 VS_CreateShadow();
		PixelShader = compile ps_3_0 PS_CreateShadow();
	}

	pass ReciveShadow
	{
		VertexShader = compile vs_3_0 VS_ReciveShadow();
		PixelShader = compile ps_3_0 PS_ReciveShadow();
	}
}