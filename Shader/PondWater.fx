struct Mtrl //물의 표면 재질
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct DirLight //방향성 광원 구조체
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;  
};

uniform extern float4x4			gWorld; //월드 행렬
uniform extern float4x4			gWorldInv; //월드 역행렬
uniform extern float4x4			gWVP; //월드뷰프로젝션 행렬
uniform extern Mtrl				gMtrl; //물의 표면 재질
uniform extern DirLight			gLight; //방향성 광원
uniform extern float3			gEyePosW; //카메라 위치
uniform extern float2			gWaveMapOffset0; //물의 노말맵 오프셋1
uniform extern float2			gWaveMapOffset1; // 물의 노말맵 오프셋2
uniform extern texture			gWaveMap0; //물 노말맵1 
uniform extern texture			gWaveMap1; //물 노말맵2
uniform extern float			gRefractBias; //굴절 편향(??)
uniform extern float			gRefractPower; //굴절 세기
uniform extern float2			gRippleScale; //잔물결 세기
uniform extern texture			gReflectMap; //반사맵
uniform extern texture			gRefractMap; //굴절맵
uniform extern float4			vFog;
uniform extern float4			vFogColor;


sampler ReflectMapS = sampler_state //반사맵 샘플러
{
	Texture = <gReflectMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler RefractMapS = sampler_state //굴절맵 샘플러
{
	Texture = <gRefractMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler WaveMapS0 = sampler_state //웨이브맵1 샘플러
{
	Texture = <gWaveMap0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler WaveMapS1 = sampler_state //웨이브맵2 샘플러
{
	Texture = <gWaveMap1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 TangentEye : TEXCOORD0;
	float3 TangentLightDir : TEXCOORD1;
	float2 Tex0 : TEXCOORD2;
	float2 Tex1 : TEXCOORD3;
	float4 ProjTex : TEXCOORD4;
	float EyeDist : TEXCOORD5;
	float Fog : TEXCOORD6;
};
VS_OUTPUT WaterVS(VS_INPUT Input)
{
    // Zero out our output.
	//반환 구조체를 초기화 한다.
	VS_OUTPUT Output = (VS_OUTPUT)0;
	
	// Build TBN-basis.  For flat water grid in the xz-plane in 
	// object space, the TBN-basis has a very simple form.
	//TBN 행렬을 기준으로 하기 위해 TBN 행렬을 만들어라
	float3x3 TBN;
	TBN[0] = float3(1.0f, 0.0f, 0.0f); // Tangent goes along object space x-axis. //접선은 객체공간 X축으로
	TBN[1] = float3(0.0f, 0.0f, -1.0f);// Binormal goes along object space -z-axis //종법선은 객체공간 -Z축으로
	TBN[2] = float3(0.0f, 1.0f, 0.0f); // Normal goes along object space y-axis //법선은 객체공간 y축으로
	
	// Matrix transforms from object space to tangent space.
	// 객체공간에서 접선공간으로 행렬을 변환해라.
	float3x3 toTangentSpace = transpose(TBN);
	
	// Transform eye position to local space.
	// 카메라 포지션을 로컬포지션으로 변환해라
	float3 localEyePosition = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;
	Output.EyeDist = distance(localEyePosition, Input.Position); //로컬 카메라 포지션과 물의 포지션의 거리값을 구한다.
	
	// Transform to-eye vector to tangent space.
	// 카메라 벡터를 탄젠트공간으로 변환해라
	float3 toEyeL = localEyePosition - Input.Position;
	Output.TangentEye = mul(toEyeL, toTangentSpace);
	
	// Transform light direction to tangent space.
	//빛 방향값을 탄젠트 공간으로 변환해라
	float3 localLightDirection = mul(float4(gLight.dirW, 0.0f), gWorldInv).xyz;
	Output.TangentLightDir = mul(localLightDirection, toTangentSpace);
	
	// Transform to homogeneous clip space.
	//월드뷰프로젝션 행렬로 최종 위치를 구한다.
	Output.Position = mul(float4(Input.Position, 1.0f), gWVP);
	
	// Scroll texture coordinates.
	//텍스쳐의 좌표를 오프셋만큼 스크롤해라
	Output.Tex0 = Input.Tex + gWaveMapOffset0;
	Output.Tex1 = Input.Tex + gWaveMapOffset1;
	
	// Generate projective texture coordinates from camera's perspective.
	// 카메라의 관점에서 투영텍스쳐 좌표를 생성한다.
	Output.ProjTex = Output.Position;

	Output.Fog = vFog.x + Output.Position.z*vFog.y; //선형포그

	if (Output.Fog < 0.f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.f;
		
	// Done--return the output.
	//구조체를 반환해라~
    return Output;
}

float4 WaterPS(VS_OUTPUT Input) : COLOR
{
	//탄젠트공간에 있는 카메라, 빛의 방향을 정규화한다.
	float3 toEyeT    = normalize(Input.TangentEye);
	float3 lightDirT = normalize(Input.TangentLightDir);

	// Light vector is opposite the direction of the light.
	float3 lightVecT = -lightDirT;
	
	// Sample normal map.
	//노말맵을 샘플링해 색을 추출한다.
	float3 normalT0 = tex2D(WaveMapS0, Input.Tex0);
	float3 normalT1 = tex2D(WaveMapS1, Input.Tex1);
	
	// Expand from [0, 1] compressed interval to true [-1, 1] interval.
	//[0,1] 텍스쳐값을 [-1, 1] 투영값으로 옮긴다.
    normalT0 = 2.0f*normalT0 - 1.0f;
    normalT1 = 2.0f*normalT1 - 1.0f;
    
	// Average the two vectors.
	//두 벡터의 평균값을 구한다(정규화 포함)
	float3 normalT = normalize(0.5f*(normalT0 + normalT1));
	
	// Compute the reflection vector.
	//반사벡터를 계산한다.(반사벡터를 구할때는 정상적인 빛 벡터를 사용해야 되지만 위에서 -값을 줬기때문에 다시 -값을 준다)
	float3 r = reflect(-lightVecT, normalT);
	
	// Determine how much (if any) specular light makes it into the eye.
	//반사조명이 얼마나 들어올지 결정해라! (스페큘러세기에 따라 달라짐)
	float t  = pow(max(dot(r, toEyeT), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	// 정점에 부딪칠 확산광의 세기를 조절한다.
	float s = max(dot(lightVecT, normalT), 0.0f);
	
	// If the diffuse light intensity is low, kill the specular lighting term.
	// It doesn't look right to add specular light when the surface receives 
	// little diffuse light.
	//만약 확산광의 세기가 0보다 낮을때는 스페큘러가 반사되지 않으므로 스페큘러도 0으로 만든다.
	if(s <= 0.0f)
	     t = 0.0f;
	     
	// Project the texture coordinates and scale/offset to [0,1].
	// 텍스쳐 좌표와 스케일/오프셋을 [0,1]로 투영한다.
	Input.ProjTex.xy /= Input.ProjTex.w;
	Input.ProjTex.x =  0.5f*Input.ProjTex.x + 0.5f;
	Input.ProjTex.y = -0.5f*Input.ProjTex.y + 0.5f;

	// To avoid clamping artifacts near the bottom screen edge, we 
	// scale the perturbation magnitude of the v-coordinate so that
	// when v is near the bottom edge of the texture (i.e., v near 1.0),
	// it doesn't cause much distortion.  The following power function
	// scales v very little until it gets near 1.0.
	// (Plot this function to see how it looks.)
	float vPerturbMod = -pow(Input.ProjTex.y, 10.0f) + 1.0f;
	
	// Sample reflect/refract maps and perturb texture coordinates.
	// 반사/굴절맵 및 교란텍스쳐 샘플링
	float2 perturbVec = normalT.xz*gRippleScale;

	perturbVec.y *= vPerturbMod;
	float3 reflectCol = tex2D(ReflectMapS, Input.ProjTex.xy+perturbVec).rgb;
	float3 refractCol = tex2D(RefractMapS, Input.ProjTex.xy+perturbVec).rgb;
	
	// Refract based on view angle.
	// 시야각을 기준으로 굴절
	float refractWt = saturate(gRefractBias+pow(max(dot(toEyeT, normalT), 0.0f), gRefractPower));
	
	// Weighted average between the reflected color and refracted color, modulated
	// with the material.
	// 반사된 색상과 굴절된 색상 사이의 가중치 평균값을 변환
	float3 ambientMtrl = gMtrl.ambient*lerp(reflectCol, refractCol, refractWt);
	float3 diffuseMtrl = gMtrl.diffuse*lerp(reflectCol, refractCol, refractWt);
	
	// Compute the ambient, diffuse and specular terms separatly.
	//주변광, 확산광, 정반사광 계산
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = saturate(diffuseMtrl*gLight.diffuse.rgb);
	float3 ambient = ambientMtrl*gLight.ambient;
	
	float3 final = diffuse + spec;

	//포그가 적용되기전 최종 칼라
	float4 Color = float4(final, gMtrl.diffuse.a);
	//포그가 적용된 최종 칼라
	float4 finalColor = Input.Fog*Color + (1 - Input.Fog)*vFogColor;
	
	// Output the color and the alpha.
    return finalColor;

	
}

technique WaterTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 WaterVS();
        pixelShader  = compile ps_2_0 WaterPS();
    }    
}
