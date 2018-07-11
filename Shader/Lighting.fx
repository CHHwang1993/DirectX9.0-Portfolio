//////////////////////////////////////////////////////////////////////////
// 포인트 라이트 구조체
//////////////////////////////////////////////////////////////////////////
struct PointLight 
{
    float3 vPosition;//위치
	float3 vColor;//색상
    float fFalloff;//강도
    float fRange;//반경
};
//////////////////////////////////////////////////////////////////////////


//포인트 라이트
shared PointLight lights[8];//포인트 라이트 배열
shared int iNumLights = 0;//라이트 갯수

//태양
shared float3 vSunColor;//태양 색
shared float3 vSunVector;//광원 방향
shared float fSunIntensity;//강도
shared float fDepthBias = 0.001f;//깊이


//지형 그림자 맵
shared bool bShadowsEnabled = false;//그림자 출력
shared float4x4 matSunWVP;//태양 월드뷰프로젝션 매트릭스

shared float overcast = 1;//날씨의 흐림 정도

shared float3 vAmbientColor;//엠비언트 색상
shared float fAmbient;//엠비언트 강도



//////////////////////////////////////////////////////////////////////////
// 그림자맵 샘플러
//////////////////////////////////////////////////////////////////////////
shared texture tShadowmap;//그림자 텍스처
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
// 태양의 디퓨즈 색상 계산
// vPosLightView : 라이트 뷰
// vNormal : 노멀벡터
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
// 다이렉트 광원 방향 계산
// vNormal : 노멀 벡터
//////////////////////////////////////////////////////////////////////////
float3 CalculateDirectionalLighting(float3 vNormal)
{
	float fDiffuse = saturate(dot(vSunVector, vNormal));
	return fAmbient * vAmbientColor + fDiffuse * vSunColor * fSunIntensity / overcast;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 포인트 라이트 계산
// light : 포인트 라이트 객체
// vPosition : 위치
// vNormal : 노멀 벡터
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