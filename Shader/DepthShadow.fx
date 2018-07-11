float4x4 matWVP; //���ÿ��� ������������ �̵�
float4x4 matLP; //���ÿ��� ������������ �̵�
float4x4 matLPB; //�ؽ��� ��ǥ��� ����
float4 vColor; //�޽���
float4 vLightDirection; //����Ʈ ����

////////////////////////�ؽ���///////////////////////////////////
texture ShadowMap; //�׸��� ��
sampler ShadowMapSampler = sampler_state
{
	Texture = <ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	mipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};

texture DecaleMap; //�޽� ��
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
	float4 Position :POSITION;//��ġ
	float4 Diffuse : COLOR0; //����
	float4 Ambient : COLOR1;//����
	float4 ShadowMapUV : TEXCOORD0; //�׸��ڸ� ��ǥ
	float4 Depth : TEXCOORD1;//�׸��ڸ� ����
	float2 TexDecale : TEXCOORD2; //�޽� �ؽ��� ��ǥ
};

////////////////////1�н� : �������̴� //////////////////////////////////////
VS_OUTPUT VS_Pass0(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 pos = mul(Input.Position, matLP);

	Output.Position = pos;

	Output.ShadowMapUV = pos;

	return Output;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////1�н� : �ȼн��̴� //////////////////////////////////////
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

/////////////////////////////���� ���̴� /////////////////////////////////////////
VS_OUTPUT VS_Pass1(VS_INPUT2 Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	float4 uv;

	Output.Position = mul(Input.Position, matWVP);

	Output.Diffuse = vColor * max(dot(vLightDirection, Input.Normal), 0);
	Output.Ambient = vColor * 0.3f;

	//�׸��ڸ�
	Output.ShadowMapUV = mul(Input.Position, matLPB);

	Output.Depth = mul(Input.Position, matLP);

	Output.TexDecale = Input.Tex;

	return Output;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////2�н� �ȼн��̴�(�ؽ�������)//////////////////////////////
float4 PS_Pass1(VS_OUTPUT Input) : COLOR
{
	float4 Color;
	float shadow = tex2Dproj(ShadowMapSampler, Input.ShadowMapUV).x;
	float4 decale = tex2D(DecaleMapSampler, Input.TexDecale);

	Color = Input.Ambient + ((shadow*Input.Depth.w < Input.Depth.z - 0.03f) ? 0.0f : Input.Diffuse);

	return Color * decale;
}
//////////////////////////////////////////////////////////////////////////////

////////////////////2�н� �ȼн��̴�(�ؽ��ľ���)//////////////////////////////
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
	pass P0 //�׸��ڸ� ����
	{
		VertexShader = compile vs_2_0 VS_Pass0();
		PixelShader = compile ps_2_0 PS_Pass0();
	}

	pass P1 //�ؽ��� ����
	{
		VertexShader = compile vs_2_0 VS_Pass1();
		PixelShader = compile ps_2_0 PS_Pass1();
	}

	pass P2 //�ؽ��� ����
	{
		VertexShader = compile vs_2_0 VS_Pass1();
		PixelShader = compile ps_2_0 PS_Pass2();
	}
}