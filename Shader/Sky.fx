float4x4 matWorld;//���� ���
float4x4 matView;//�� ���
float4x4 matProjection;//�������� ���
float3 vSunVector;//���� ����
float fSunIntensity;//���� ����
float overcast = 1;//������ �帲 ����
float fTime;//�ð�

//�ϴ� ����
float4 vHorizonColorTwilight = float4(0.4f, 0.16f, 0, 1) * 1.5f;//Ȳȥ �����÷�
float4 vHorizonColorDay = float4(1, 1, 1, 1);//�ְ� �����÷�
float4 vHorizonColorNight = float4(0.2f, 0.2f, 0.2f, 1);//�߰� �����÷�
float4 vCeilingColorTwilight = float4(0.17f, 0.15f, 0.15f, 1);//Ȳȥ �����÷�
float4 vCeilingColorDay = float4(0.72f, 0.75f, 0.98f, 1);//�ְ� �����÷�
float4 vCeilingColorNight = float4(0.1f, 0.1f, 0.15f, 1);//�߰� �����÷�


texture tNight; //�� �ؽ�ó
sampler sNight = sampler_state {
	texture = <tNight>;
	MagFilter = linear;
	MinFilter = linear;
	MipFilter = linear;
	AddressU  = wrap;
	AddressV  = wrap;
};

struct VS_INPUT //���ؽ� ���̴� �Է� ����ü
{
    float4 vPosition	: POSITION0;
    float2 vTexCoords	: TEXCOORD0;
};

struct VS_OUTPUT //���ؽ� ���̴� ��� ����ü
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
    float4 vHorizonColor;	// color at the base of the skydome �ϴ��� �⺻ ����
    float4 vCeilingColor;	// color at the top of the skydome �ϴ��� õ�� ����

	float3 sunVector = -vSunVector;
    
    // interpolate the horizon/ceiling colors based on the time of day
	//�ð��� �������� horizon�� ceiling ���� �����Ѵ�.
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
	// ��ī�̵��� ���̷� �ȼ��� ���� �����Ѵ�.
    float4 vPixelColor = lerp(vHorizonColor, vCeilingColor, saturate(Input.vWPosition.y / 0.4f));
    
    // darken sky when overcast
	//�帱���� ��ο� �ϴ÷�
    vPixelColor.rgb /= overcast;
    
    // sunrises/sunsets should glow around the horizon where the sun is rising/setting
	//�¾��� �߰ų� �������� ���� ������ �������Ѵ�.
    vPixelColor.rgb += float3(1.2f,0.8f,0) * saturate(1 - distance(Input.vWPosition, sunVector)) / 2 * (1-fSunIntensity) / overcast;
    
    // stars should be gradually more visible as they are closer to the ceiling and invisible when
    // closer to the horizon.  stars should also fade in/out during transitions between night/day
	//���� ceiling�� ��������� �ߺ��̰� �ǰ� ���򼱿� ��������� �߾Ⱥ��̰� �˴ϴ�.
	//���� ������ ���ϰ� ������ ���̵� ��/�ƿ��� �˴ϴ�.
    float fHorizonLerp = saturate(lerp(0,1, Input.vWPosition.y * 1.5f));
    float fStarVisibility = saturate(max(fHorizonLerp * lerp(0,1,max(-sunVector.y,0)),0));
    
    // display stars according to visibility, scaling the texture coordinates for smaller stars
	//���� ���̴µ��� ���� ������ �ؽ��� ��ǥ���� �����˴ϴ�.
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
