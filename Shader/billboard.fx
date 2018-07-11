float4x4 matWorld;
float4x4 matView;
float4x4 matProjection;
float3 vCameraPos;
float3 vAllowedRotDir;
float3 vWorldLightDir;
float fTime;
float4 vFog;
float3 vFogColor;

texture BillboardTexture;

sampler textureSampler = sampler_state
{
	texture = <BillboardTexture>;
	magFilter = LINEAR;
	minFilter = LINEAR;
	mipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VS_INPUT
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float2 Scale : TEXCOORD1;
};

struct VS_OUTPUT
{
	float4 Position : POSITION0;
	float3 Normal : TEXCOORD1;
	float2 TexCoord : TEXCOORD0;
	float Fog : FOG;
};

VS_OUTPUT VS_main(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float fWind = (1 - Input.TexCoord.y) * sin(fTime + Input.Position.x + Input.Position.y);

	Input.Position.x += fWind;

	float3 worldPos = mul(Input.Position, matWorld);
	float3 eyeVector = worldPos - vCameraPos;
	eyeVector = normalize(eyeVector);

	float3 upVector = vAllowedRotDir;
	upVector = normalize(upVector);

	float3 sideVector = cross(eyeVector, upVector);
	sideVector = normalize(sideVector);

	float3 finalPosition = worldPos;
	finalPosition += (Input.TexCoord.x - 0.5f)*sideVector*Input.Scale.x;
	finalPosition += (1.5f - Input.TexCoord.y*1.5f)*upVector*Input.Scale.y;

	float4 finalPosition4 = float4(finalPosition, 1);
	float4x4 prevViewProjection = mul(matView, matProjection);
	Output.Position = mul(finalPosition4, prevViewProjection);

	Output.Normal = mul(Input.Normal, (float3x3)matWorld);

	Output.TexCoord = Input.TexCoord;

	Output.Fog = vFog.x + Output.Position.z*vFog.y; //선형포그

	if (Output.Fog < 0.f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.f;

	return Output;
}

float4 PS_main(VS_OUTPUT Input) : COLOR0
{
	float4 Color = tex2D(textureSampler,Input.TexCoord);
	float Alpha = Color.a;

	float3 worldNormal = normalize(Input.Normal);
	float diff = dot(worldNormal, -normalize(vWorldLightDir.xyz));

	diff = saturate(diff);
	diff = max(0.2f, diff);

	float3 FinalColor = Input.Fog* Color.rgb*diff + (1 - Input.Fog)*vFogColor;
	
	return float4(FinalColor,Alpha);
}

technique Billboard
{
	pass Pass0
	{
		AlphaTestEnable = true;
		AlphaFunc = GreaterEqual;
		AlphaRef = 200;
		VertexShader = compile vs_2_0 VS_main();
		PixelShader = compile ps_3_0 PS_main();
	}

	pass Pass1
	{
		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		AlphaFunc = less;
		VertexShader = compile vs_2_0 VS_main();
		PixelShader = compile ps_3_0 PS_main();
	}
}