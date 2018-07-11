float4x4 matWVP; //로컬에서 투영공간으로 이동
float4x4 matLP; //로컬에서 투영공간으로 이동
float4x4 matLPB; //텍스쳐 좌표계로 투영
float4 vColor; //메쉬색
float4 vLightDirection; //라이트 방향

////////////////////////텍스쳐///////////////////////////////////
texture ShadowMap; //그림자 맵
sampler ShadowMapSampler = sampler_state
{
	Texture = <ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	mipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};

texture DecaleMap; //메쉬 맵
sampler DecaleMapSampler = sampler_state
{
	Texture = <DecaleMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position :POSITION;//위치
	float4 Diffuse : COLOR0; //색상
	float4 Ambient : COLOR1;//색상
	float4 ShadowMapUV : TEXCOORD0; //그림자맵 좌표
	float4 Depth : TEXCOORD1;//그림자맵 깊이
	float2 TexDecale : TEXCOORD2; //메쉬 텍스쳐 좌표
};

////////////////////1패스 : 정점쉐이더 //////////////////////////////////////
VS_OUTPUT VS_Pass0(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 pos = mul(Input.Position, matLP);

	Output.Position = pos;

	Output.ShadowMapUV = pos;

	return Output;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////1패스 : 픽셸쉐이더 //////////////////////////////////////
float4 PS_Pass0(VS_OUTPUT Input) : COLOR0
{
	float4 Color;
	
	Color = Input.ShadowMapUV.z / Input.ShadowMapUV.w;

	return Color;
}
//////////////////////////////////////////////////////////////////////////////

struct VS_INPUT2
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

/////////////////////////////정점 쉐이더 /////////////////////////////////////////
VS_OUTPUT VS_Pass1(VS_INPUT2 Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	float4 uv;

	Output.Position = mul(Input.Position, matWVP);

	Output.Diffuse = vColor * max(dot(vLightDirection, Input.Normal), 0);
	Output.Ambient = vColor * 0.3f;

	//그림자맵
	Output.ShadowMapUV = mul(Input.Position, matLPB);

	Output.Depth = mul(Input.Position, matLP);

	Output.TexDecale = Input.Tex;

	return Output;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////2패스 픽셸쉐이더(텍스쳐있음)//////////////////////////////
float4 PS_Pass1(VS_OUTPUT Input) : COLOR
{
	float4 Color;
	float shadow = tex2Dproj(ShadowMapSampler, Input.ShadowMapUV).x;
	float4 decale = tex2D(DecaleMapSampler, Input.TexDecale);

	Color = Input.Ambient + ((shadow*Input.Depth.w < Input.Depth.z - 0.03f) ? 0.0f : Input.Diffuse);

	return Color * decale;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////2패스 픽셸쉐이더(텍스쳐없음)//////////////////////////////
float4 PS_Pass2(VS_OUTPUT Input) : COLOR
{
	float4 Color;
	float shadow = tex2Dproj(ShadowMapSampler, Input.ShadowMapUV).x;
	
	Color = Input.Ambient + ((shadow*Input.Depth.w < Input.Depth.z - 0.03f) ? 0.0f : Input.Diffuse);

	return Color;
}
//////////////////////////////////////////////////////////////////////////////

technique TShader
{
	pass P0 //그림자맵 생성
	{
		VertexShader = compile vs_2_0 VS_Pass0();
		PixelShader = compile ps_2_0 PS_Pass0();
	}

	pass P1 //텍스쳐 있음
	{
		VertexShader = compile vs_2_0 VS_Pass1();
		PixelShader = compile ps_2_0 PS_Pass1();
	}

	pass P2 //텍스쳐 없음
	{
		VertexShader = compile vs_2_0 VS_Pass1();
		PixelShader = compile ps_2_0 PS_Pass2();
	}
}