float4x4 matWorld;//월드 행렬
float4x4 matView;//뷰 행렬
float4x4 matProjection;//프로젝션 행렬
float3 vSunVector;//광원 방향
float overcast = 1;//날씨의 흐림 정도

texture tSun;
sampler sSun = sampler_state { 
	texture = <tSun>; 
	magfilter = LINEAR; 
	minfilter = LINEAR; 
	mipfilter = LINEAR; 
	AddressU = clamp; 
	AddressV = clamp;
};

struct VS_INPUT //버텍스 쉐이더 입력 구조체
{
    float4 vPosition	: POSITION0;
    float4 vColor		: COLOR0;
    float1 fSize		: PSIZE;
};

struct VS_OUTPUT //버텍스 쉐이더 출력 구조체
{
	float4 vPosition	: POSITION0;
	float1 fSize		: PSIZE;
	float3 vWPosition	: TEXCOORD0;
};

VS_OUTPUT VS_Sun(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
     
    float4x4 matVP = mul (matView, matProjection);
	float4x4 matWVP = mul (matWorld, matVP); 
	Output.vPosition = mul(Input.vPosition, matWVP);
	Output.fSize = Input.fSize;
	Output.vWPosition = Input.vPosition.xxx;
    
    return Output;
}

struct PS_INPUT //픽셀 쉐이더 입력 구조체
{
	float2 vTexCoords	: TEXCOORD0;
};

float4 PS_Sun(PS_INPUT PsInput) : COLOR0 
{
	float3 sunVector = -vSunVector;
	float4 vColor = tex2D(sSun, PsInput.vTexCoords);
	vColor.a *= saturate(2-overcast);
	vColor.a *= lerp(0,1, sunVector.y);
	return vColor;
}

technique Sun
{
    pass Pass1
    {
        VertexShader = compile vs_2_0 VS_Sun();
        PixelShader = compile ps_2_0 PS_Sun();
    }
}