float pixelSizeU;
float pixelSizeV;
float pixelHalfSizeU;
float pixelHalfSizeV;


//////////////////////////////////////정점 쉐이더 기본////////////////////////////////////////

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 UV : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 UV : TEXCOORD0;
};

VS_OUTPUT VS_Base(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.Position = Input.Position;
	Output.UV = Input.UV;

	return Output;
}
///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////픽셸쉐이더 기본//////////////////////////////////////////////

texture screenTex; //스크린 텍스쳐
sampler2D screenTexSampler = sampler_state
{
	Texture = <screenTex>;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
};

float4 PS_Base(VS_OUTPUT Input) : COLOR
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	return tex2D(screenTexSampler, Input.UV);
}

///////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////픽셸쉐이더 아웃라인//////////////////////////////////////////////
texture normalTex;
sampler2D normalTexSampler = sampler_state
{
	Texture = <normalTex>;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
};

float mask[9] =
{
	-1,-1,-1,
	-1, 8,-1,
	-1,-1,-1 //라플라스 필터
};

float coordX[3] = { -1,0,1 };
float coordY[3] = { -1,0,1 };

float4 outLineColor = float4(1, 1, 0, 1);

float4 PS_Outline(VS_OUTPUT Input) : COLOR0
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	float outlineStrength = 0.0f;

	float pX = pixelSizeU;
	float pY = pixelSizeV;

	//자신을 포함한 주변픽셀 9개의 정보
	for (int i = 0; i < 9; ++i)
	{
		float2 uv = Input.UV + float2(coordX[i % 3] * pX, coordY[i / 3] * pY);

		float3 normal = tex2D(normalTexSampler, uv).rgb*mask[i];

		outlineStrength += normal.x;
		outlineStrength += normal.y;
		outlineStrength += normal.z;
	}

	outlineStrength = saturate(abs(outlineStrength));

	float4 baseColor = tex2D(screenTexSampler, Input.UV);
	float4 finalColor = lerp(baseColor, outLineColor, outlineStrength);

	return finalColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////픽셸쉐이더 블러X//////////////////////////////////////////////

float blurWeights[13] =
{
	0.002216,		// -6
	0.008764,       // -5
	0.026995,       // -4
	0.064759,       // -3
	0.120985,       // -2
	0.176033,       // -1
	0.199471,		//Center
	0.176033,		//	1
	0.120985,		//	2
	0.064759,		//	3
	0.026995,		//	4
	0.008764,		//	5
	0.002216		//	6
};

float blurPos[13] = 
{
	-6,
	-5,
	-4,
	-3,
	-2,
	-1,
	0,
	1,
	2,
	3,
	4,
	5,
	6
};

float blurScale = 1.0f;

float4 PS_BlurX(VS_OUTPUT Input) : COLOR0
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	float4 finalColor = float4(0, 0, 0, 1);

	for (int i = 0; i < 13; ++i)
	{
		float2 uv = Input.UV + float2(blurPos[i] * pixelSizeU, 0)*blurScale;

		uv.x = max(pixelHalfSizeU, uv.x);
		uv.x = min(1 - pixelHalfSizeU, uv.x);

		uv.y = max(pixelHalfSizeV, uv.y);
		uv.y = min(1 - pixelHalfSizeV, uv.y);

		finalColor += tex2D(screenTexSampler, uv)* blurWeights[i];
	}

	return finalColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////픽셸쉐이더 블러Y//////////////////////////////////////////////

float4 PS_BlurY(VS_OUTPUT Input) : COLOR0
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	float4 finalColor = float4(0, 0, 0, 1);

	for (int i = 0; i < 13; ++i)
	{
		float2 uv = Input.UV + float2(0, blurPos[i] * pixelSizeV)*blurScale;

		uv.x = max(pixelHalfSizeU, uv.x);
		uv.x = min(1 - pixelHalfSizeU, uv.x);

		uv.y = max(pixelHalfSizeV, uv.y);
		uv.y = min(1 - pixelHalfSizeV, uv.y);

		finalColor += tex2D(screenTexSampler, uv)*blurWeights[i];
	}

	return finalColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////픽셸쉐이더 BBO//////////////////////////////////////////////

texture blurTex;
sampler2D blurTexSampler = sampler_state
{
	Texture = <blurTex>;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

float blurAmount = 3.0f;

float4 PS_BBO(VS_OUTPUT Input) : COLOR0
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	float4 originalTex = tex2D(screenTexSampler, Input.UV);
	float4 blurTex = tex2D(blurTexSampler, Input.UV);

	return originalTex + blurTex * blurAmount;
}
///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////픽셸쉐이더 블룸///////////////////////////////////////////
float2 rcpres = { 0.0025, 0.0033333333333333333333333333333333 };
float BloomIntensity = 32;

float4 PS_Bloom(VS_OUTPUT Input) : COLOR0
{
	Input.UV.x += pixelHalfSizeU;
	Input.UV.y += pixelHalfSizeV;

	float4 Color = tex2D(screenTexSampler, Input.UV);
	Color = pow(Color, BloomIntensity);

	float4 Color2 = -0.84;

	for (int i = 0; i < 13; ++i)
	{
		Color2 += tex2D(screenTexSampler, Input.UV + float2(blurPos[i], 0)*rcpres)*blurWeights[i];
		Color2 += tex2D(screenTexSampler, Input.UV + float2(0, blurPos[i])*rcpres)*blurWeights[i];
	}

	Color2 *= 0.15f;

	float4 Color3 = tex2D(blurTexSampler, Input.UV);

	return Color + Color2 + Color3;
}




technique PostEffect
{
	//테크니크를 바꾸는것보다 패스를 바꾸는것이 비용이 덜든다.

	pass Base //기본
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_Base();
	}

	pass Outline // 아웃라인
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_Outline();
	}

	pass BlurX //블러 X
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_BlurX();
	}

	pass BlurY //블러 Y
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_BlurY();
	}

	pass BBO 
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_BBO();
	}

	pass Bloom
	{
		ZWRITEENABLE = false;
		VertexShader = compile vs_3_0 VS_Base();
		PixelShader = compile ps_3_0 PS_Bloom();
	}
}