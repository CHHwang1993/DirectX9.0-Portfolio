//////////////////////////////////////////////////////////////////////////
// ����Ʈ ����Ʈ ����ü
//////////////////////////////////////////////////////////////////////////
struct PointLight 
{
    float3 vPosition;//��ġ
	float3 vColor;//����
    float fFalloff;//����
    float fRange;//�ݰ�
};
//////////////////////////////////////////////////////////////////////////


//����Ʈ ����Ʈ
shared PointLight lights[8];//����Ʈ ����Ʈ �迭
shared int iNumLights = 0;//����Ʈ ����

//�¾�
shared float3 vSunColor;//�¾� ��
shared float3 vSunVector;//���� ����
shared float fSunIntensity;//����
shared float fDepthBias = 0.001f;//����


//���� �׸��� ��
shared bool bShadowsEnabled = false;//�׸��� ���
shared float4x4 matSunWVP;//�¾� ������������� ��Ʈ����

shared float overcast = 1;//������ �帲 ����

shared float3 vAmbientColor;//�����Ʈ ����
shared float fAmbient;//�����Ʈ ����



//////////////////////////////////////////////////////////////////////////
// �׸��ڸ� ���÷�
//////////////////////////////////////////////////////////////////////////
shared texture tShadowmap;//�׸��� �ؽ�ó
sampler sShadowmap = sampler_state { 
	Texture		= (tShadowmap);
	MinFilter	= Linear;
	MagFilter	= Linear;
	MipFilter	= Linear;
	AddressU	= Clamp;
	AddressV	= Clamp;
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// �¾��� ��ǻ�� ���� ���
// vPosLightView : ����Ʈ ��
// vNormal : ��ֺ���
//////////////////////////////////////////////////////////////////////////
float CalculateSunDiffuse(float4 vPosLightView, float3 vNormal)
{
	// map coordinates from [-1,1] to [0,1]
	float2 vSMTexCoords;
	vSMTexCoords.x = vPosLightView.x / 2.0f + 0.5f;
	vSMTexCoords.y = -vPosLightView.y / 2.0f + 0.5f;
	
	// sample the shadowmap and calculate the pixel depth
	float fDiffuse = 0;
	float fSMDepth = tex2D(sShadowmap, vSMTexCoords).r;
	float fPixelDepth = vPosLightView.z;

	// compare the depth stored in the shadowmap with the pixel's actual depth
	if (fPixelDepth - fDepthBias <= fSMDepth) {
		// pixel is lit, so apply normal diffuse lighting and cloud shadowing
		fDiffuse =  saturate(dot(vSunVector, vNormal)) / overcast;
	}
       
    return fDiffuse;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Calculates specular highlight using the direction of a light, a surface's normal, and the camera eye vector
//////////////////////////////////////////////////////////////////////////
float CalculateSpecular(float3 vLightDir, float3 vNormal, float3 vEye, float fPower)
{
	float3 vReflect = -reflect(vLightDir, vNormal);
	return pow(abs(dot(vReflect, vEye)), fPower);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ���̷�Ʈ ���� ���� ���
// vNormal : ��� ����
//////////////////////////////////////////////////////////////////////////
float3 CalculateDirectionalLighting(float3 vNormal)
{
	float fDiffuse = saturate(dot(vSunVector, vNormal));
	return fAmbient * vAmbientColor + fDiffuse * vSunColor * fSunIntensity / overcast;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ����Ʈ ����Ʈ ���
// light : ����Ʈ ����Ʈ ��ü
// vPosition : ��ġ
// vNormal : ��� ����
//////////////////////////////////////////////////////////////////////////
float3 CalculatePointLighting(PointLight light, float3 vPosition, float3 vNormal)
{
	float3 vLightDir = light.vPosition - vPosition;
	float fDistance = length(vLightDir);
	float fTotalIntensity = pow(saturate((light.fRange - fDistance) / light.fRange), light.fFalloff);
	
	// diffuse
	float fDiffuse = saturate(dot(vNormal, vLightDir));
	float3 vDiffuseColor = fDiffuse * light.vColor;
	
	return vDiffuseColor * fTotalIntensity;
}
//////////////////////////////////////////////////////////////////////////