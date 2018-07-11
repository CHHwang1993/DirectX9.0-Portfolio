#include "stdafx.h"
#include "Sun.h"
#include "DirectionLight.h"

LPDIRECT3DVERTEXDECLARATION9 VertexSun::Decl = NULL;

Sun::Sun() : pEffect(NULL), sunTexture(NULL), sunPosition(0,50,0)
{
	sunTexture = TEXTURE2D->GetResource("Texture/Sun.jpg");
	pEffect = SHADER->GetResource("fx/Sun.fx");

	//선형포그값
	fFar = 600.0f;
	fNear = 0.0f;


	D3DVERTEXELEMENT9 tempVertex[] =
	{
		{0,0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,sizeof(float)*3, D3DDECLTYPE_FLOAT1,D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE,0},
		D3DDECL_END()
	};

	HRESULT hr;

	hr = g_pD3DDevice->CreateVertexDeclaration(tempVertex, &VertexSun::Decl);
	assert(hr == S_OK);
}


Sun::~Sun()
{
	SAFE_RELEASE(VertexSun::Decl);
}

void Sun::Update(DirectionLight * pLight)
{
	sunIntensity = Clamp01(D3DXVec3Dot(&-pLight->GetLightDirection(), &D3DXVECTOR3(0, 1, 0)));
	sunColor = D3DXVECTOR3(1, 0.5f + sunIntensity / 2.0f, sunIntensity);
	ambientColor = D3DXVECTOR3(1, 1 - sunIntensity / 10, 1 - sunIntensity / 5);
	ambientIntensity = 0.2f;

	fogColor = sunColor * sunIntensity + ambientColor * ambientIntensity;

	//포그공식
	vFog.x = fFar / (fFar - fNear);
	vFog.y = -1.0f / (fFar - fNear);
}

void Sun::Render(DirectionLight* pLight, Camera* pCamera)
{
	g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);

	D3DXVECTOR3 cameraPosition = pCamera->GetWorldPosition();

	float sunHeight = pLight->GetLightMatrix()._22;

	sunPosition = D3DXVECTOR3(cameraPosition.x, cameraPosition.y-sunHeight*1000, cameraPosition.z - pLight->GetLightMatrix()._23* 1000);

	VertexSun spriteArray[1];
	spriteArray[0] = VertexSun(sunPosition, 50);

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matView", &pCamera->GetViewMatrix());
		pEffect->SetMatrix("matProjection", &pCamera->GetProjectionMatrix());
		pEffect->SetTexture("tSun", sunTexture);
		pEffect->SetFloat("overcast", 1.0f);
		pEffect->SetVector3("vSunVector", pLight->GetLightDirection());

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			g_pD3DDevice->SetVertexDeclaration(VertexSun::Decl);
			g_pD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, spriteArray, sizeof(VertexSun));
		}
		pEffect->EndPass();
		pEffect->End();
	}

	g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
