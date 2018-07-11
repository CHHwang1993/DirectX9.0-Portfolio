struct Mtrl //���� ǥ�� ����
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct DirLight //���⼺ ���� ����ü
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;  
};

uniform extern float4x4			gWorld; //���� ���
uniform extern float4x4			gWorldInv; //���� �����
uniform extern float4x4			gWVP; //������������� ���
uniform extern Mtrl				gMtrl; //���� ǥ�� ����
uniform extern DirLight			gLight; //���⼺ ����
uniform extern float3			gEyePosW; //ī�޶� ��ġ
uniform extern float2			gWaveMapOffset0; //���� �븻�� ������1
uniform extern float2			gWaveMapOffset1; // ���� �븻�� ������2
uniform extern texture			gWaveMap0; //�� �븻��1 
uniform extern texture			gWaveMap1; //�� �븻��2
uniform extern float			gRefractBias; //���� ����(??)
uniform extern float			gRefractPower; //���� ����
uniform extern float2			gRippleScale; //�ܹ��� ����
uniform extern texture			gReflectMap; //�ݻ��
uniform extern texture			gRefractMap; //������
uniform extern float4			vFog;
uniform extern float4			vFogColor;


sampler ReflectMapS = sampler_state //�ݻ�� ���÷�
{
	Texture = <gReflectMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler RefractMapS = sampler_state //������ ���÷�
{
	Texture = <gRefractMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler WaveMapS0 = sampler_state //���̺��1 ���÷�
{
	Texture = <gWaveMap0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler WaveMapS1 = sampler_state //���̺��2 ���÷�
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
	//��ȯ ����ü�� �ʱ�ȭ �Ѵ�.
	VS_OUTPUT Output = (VS_OUTPUT)0;
	
	// Build TBN-basis.  For flat water grid in the xz-plane in 
	// object space, the TBN-basis has a very simple form.
	//TBN ����� �������� �ϱ� ���� TBN ����� ������
	float3x3 TBN;
	TBN[0] = float3(1.0f, 0.0f, 0.0f); // Tangent goes along object space x-axis. //������ ��ü���� X������
	TBN[1] = float3(0.0f, 0.0f, -1.0f);// Binormal goes along object space -z-axis //�������� ��ü���� -Z������
	TBN[2] = float3(0.0f, 1.0f, 0.0f); // Normal goes along object space y-axis //������ ��ü���� y������
	
	// Matrix transforms from object space to tangent space.
	// ��ü�������� ������������ ����� ��ȯ�ض�.
	float3x3 toTangentSpace = transpose(TBN);
	
	// Transform eye position to local space.
	// ī�޶� �������� �������������� ��ȯ�ض�
	float3 localEyePosition = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;
	Output.EyeDist = distance(localEyePosition, Input.Position); //���� ī�޶� �����ǰ� ���� �������� �Ÿ����� ���Ѵ�.
	
	// Transform to-eye vector to tangent space.
	// ī�޶� ���͸� ź��Ʈ�������� ��ȯ�ض�
	float3 toEyeL = localEyePosition - Input.Position;
	Output.TangentEye = mul(toEyeL, toTangentSpace);
	
	// Transform light direction to tangent space.
	//�� ���Ⱚ�� ź��Ʈ �������� ��ȯ�ض�
	float3 localLightDirection = mul(float4(gLight.dirW, 0.0f), gWorldInv).xyz;
	Output.TangentLightDir = mul(localLightDirection, toTangentSpace);
	
	// Transform to homogeneous clip space.
	//������������� ��ķ� ���� ��ġ�� ���Ѵ�.
	Output.Position = mul(float4(Input.Position, 1.0f), gWVP);
	
	// Scroll texture coordinates.
	//�ؽ����� ��ǥ�� �����¸�ŭ ��ũ���ض�
	Output.Tex0 = Input.Tex + gWaveMapOffset0;
	Output.Tex1 = Input.Tex + gWaveMapOffset1;
	
	// Generate projective texture coordinates from camera's perspective.
	// ī�޶��� �������� �����ؽ��� ��ǥ�� �����Ѵ�.
	Output.ProjTex = Output.Position;

	Output.Fog = vFog.x + Output.Position.z*vFog.y; //��������

	if (Output.Fog < 0.f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.f;
		
	// Done--return the output.
	//����ü�� ��ȯ�ض�~
    return Output;
}

float4 WaterPS(VS_OUTPUT Input) : COLOR
{
	//ź��Ʈ������ �ִ� ī�޶�, ���� ������ ����ȭ�Ѵ�.
	float3 toEyeT    = normalize(Input.TangentEye);
	float3 lightDirT = normalize(Input.TangentLightDir);

	// Light vector is opposite the direction of the light.
	float3 lightVecT = -lightDirT;
	
	// Sample normal map.
	//�븻���� ���ø��� ���� �����Ѵ�.
	float3 normalT0 = tex2D(WaveMapS0, Input.Tex0);
	float3 normalT1 = tex2D(WaveMapS1, Input.Tex1);
	
	// Expand from [0, 1] compressed interval to true [-1, 1] interval.
	//[0,1] �ؽ��İ��� [-1, 1] ���������� �ű��.
    normalT0 = 2.0f*normalT0 - 1.0f;
    normalT1 = 2.0f*normalT1 - 1.0f;
    
	// Average the two vectors.
	//�� ������ ��հ��� ���Ѵ�(����ȭ ����)
	float3 normalT = normalize(0.5f*(normalT0 + normalT1));
	
	// Compute the reflection vector.
	//�ݻ纤�͸� ����Ѵ�.(�ݻ纤�͸� ���Ҷ��� �������� �� ���͸� ����ؾ� ������ ������ -���� ��⶧���� �ٽ� -���� �ش�)
	float3 r = reflect(-lightVecT, normalT);
	
	// Determine how much (if any) specular light makes it into the eye.
	//�ݻ������� �󸶳� ������ �����ض�! (����ŧ�����⿡ ���� �޶���)
	float t  = pow(max(dot(r, toEyeT), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	// ������ �ε�ĥ Ȯ�걤�� ���⸦ �����Ѵ�.
	float s = max(dot(lightVecT, normalT), 0.0f);
	
	// If the diffuse light intensity is low, kill the specular lighting term.
	// It doesn't look right to add specular light when the surface receives 
	// little diffuse light.
	//���� Ȯ�걤�� ���Ⱑ 0���� �������� ����ŧ���� �ݻ���� �����Ƿ� ����ŧ���� 0���� �����.
	if(s <= 0.0f)
	     t = 0.0f;
	     
	// Project the texture coordinates and scale/offset to [0,1].
	// �ؽ��� ��ǥ�� ������/�������� [0,1]�� �����Ѵ�.
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
	// �ݻ�/������ �� �����ؽ��� ���ø�
	float2 perturbVec = normalT.xz*gRippleScale;

	perturbVec.y *= vPerturbMod;
	float3 reflectCol = tex2D(ReflectMapS, Input.ProjTex.xy+perturbVec).rgb;
	float3 refractCol = tex2D(RefractMapS, Input.ProjTex.xy+perturbVec).rgb;
	
	// Refract based on view angle.
	// �þ߰��� �������� ����
	float refractWt = saturate(gRefractBias+pow(max(dot(toEyeT, normalT), 0.0f), gRefractPower));
	
	// Weighted average between the reflected color and refracted color, modulated
	// with the material.
	// �ݻ�� ����� ������ ���� ������ ����ġ ��հ��� ��ȯ
	float3 ambientMtrl = gMtrl.ambient*lerp(reflectCol, refractCol, refractWt);
	float3 diffuseMtrl = gMtrl.diffuse*lerp(reflectCol, refractCol, refractWt);
	
	// Compute the ambient, diffuse and specular terms separatly.
	//�ֺ���, Ȯ�걤, ���ݻ籤 ���
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = saturate(diffuseMtrl*gLight.diffuse.rgb);
	float3 ambient = ambientMtrl*gLight.ambient;
	
	float3 final = diffuse + spec;

	//���װ� ����Ǳ��� ���� Į��
	float4 Color = float4(final, gMtrl.diffuse.a);
	//���װ� ����� ���� Į��
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
