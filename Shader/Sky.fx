float4x4 matWorld;//월드 행렬
float4x4 matView;//뷰 행렬
float4x4 matProjection;//프로젝션 행렬
float3 vSunVector;//광원 방향
float fSunIntensity;//빛의 강도
float overcast = 1;//날씨의 흐림 정도
float fTime;//시간

//하늘 색상
float4 vHorizonColorTwilight = float4(0.4f, 0.16f, 0, 1) * 1.5f;//황혼 수직컬러
float4 vHorizonColorDay = float4(1, 1, 1, 1);//주간 수직컬러
float4 vHorizonColorNight = float4(0.2f, 0.2f, 0.2f, 1);//야간 수직컬러
float4 vCeilingColorTwilight = float4(0.17f, 0.15f, 0.15f, 1);//황혼 윗면컬러
float4 vCeilingColorDay = float4(0.72f, 0.75f, 0.98f, 1);//주간 윗면컬러
float4 vCeilingColorNight = float4(0.1f, 0.1f, 0.15f, 1);//야간 윗면컬러


texture tNight; //밤 텍스처
sampler sNight = sampler_state {
	texture = <tNight>;
	MagFilter = linear;
	MinFilter = linear;
	MipFilter = linear;
	AddressU  = wrap;
	AddressV  = wrap;
};

struct VS_INPUT //버텍스 쉐이더 입력 구조체
{
    float4 vPosition	: POSITION0;
    float2 vTexCoords	: TEXCOORD0;
};

struct VS_OUTPUT //버텍스 쉐이더 출력 구조체
{
    float4 vPosition	: POSITION0;
    float2 vTexCoords	: TEXCOORD0;
    float4 vWPosition	: TEXCOORD3;
	
};

VS_OUTPUT VS_Sky(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4x4 matVP = mul(matView, matProjection);
	float4x4 matWVP = mul(matWorld, matVP);
    
	Output.vPosition = mul(Input.vPosition, matWVP);
	Output.vTexCoords = Input.vTexCoords;
	Output.vWPosition = Input.vPosition;

    return Output;
}

float4 PS_Sky(VS_OUTPUT Input) : COLOR0
{
    float4 vHorizonColor;	// color at the base of the skydome 하늘의 기본 색상
    float4 vCeilingColor;	// color at the top of the skydome 하늘의 천장 색상

	float3 sunVector = -vSunVector;
    
    // interpolate the horizon/ceiling colors based on the time of day
	//시간을 기준으로 horizon과 ceiling 색을 보간한다.
    if (sunVector.y > 0)
    {
		float amount = min(sunVector.y * 1.5f, 1);
		vHorizonColor = lerp(vHorizonColorTwilight, vHorizonColorDay, amount);
		vCeilingColor = lerp(vCeilingColorTwilight, vCeilingColorDay, amount);
    }
    else
    {
		float amount = min(-sunVector.y * 1.5f, 1);
    	vHorizonColor = lerp(vHorizonColorTwilight, vHorizonColorNight, amount);
		vCeilingColor = lerp(vCeilingColorTwilight, vCeilingColorNight, amount);
    }
    
    // interpolate the color of the pixel by its height in the skydome
	// 스카이돔의 높이로 픽셀의 색을 보간한다.
    float4 vPixelColor = lerp(vHorizonColor, vCeilingColor, saturate(Input.vWPosition.y / 0.4f));
    
    // darken sky when overcast
	//흐릴때는 어두운 하늘로
    vPixelColor.rgb /= overcast;
    
    // sunrises/sunsets should glow around the horizon where the sun is rising/setting
	//태양이 뜨거나 질때에는 수평선 주위가 빛나야한다.
    vPixelColor.rgb += float3(1.2f,0.8f,0) * saturate(1 - distance(Input.vWPosition, sunVector)) / 2 * (1-fSunIntensity) / overcast;
    
    // stars should be gradually more visible as they are closer to the ceiling and invisible when
    // closer to the horizon.  stars should also fade in/out during transitions between night/day
	//별은 ceiling에 가까워지면 잘보이게 되고 수평선에 가까워지면 잘안보이게 됩니다.
	//별은 낮밤이 변하고 있을때 페이드 인/아웃이 됩니다.
    float fHorizonLerp = saturate(lerp(0,1, Input.vWPosition.y * 1.5f));
    float fStarVisibility = saturate(max(fHorizonLerp * lerp(0,1,max(-sunVector.y,0)),0));
    
    // display stars according to visibility, scaling the texture coordinates for smaller stars
	//별이 보이는동안 작은 별들의 텍스쳐 좌표들이 조절됩니다.
    float2 vStarTexCoords = Input.vTexCoords * 8;
    vStarTexCoords.y += fTime / 2;
    vPixelColor += tex2D(sNight, vStarTexCoords) * fStarVisibility * 0.6f / (overcast * 2);
   
    return vPixelColor;
}

technique Sky
{
    pass Pass1
    {
        VertexShader = compile vs_1_1 VS_Sky();
        PixelShader = compile ps_2_0 PS_Sky();
    }
}
