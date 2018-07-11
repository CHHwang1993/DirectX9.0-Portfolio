#include "stdafx.h"
#include "Sky.h"
#include "Sun.h"
#include "DirectionLight.h"

Sky::Sky()
	:skyDome(NULL), tNightTexture(NULL), pEffect(NULL), pSun(NULL)
{
}


Sky::~Sky()
{
	SAFE_DELETE(pSun);
}

void Sky::LoadAssets()
{
	skyDome = MODEL->GetResource("Model/unit_dome.x");
	tNightTexture = TEXTURE2D->GetResource("Texture/Stars.jpg");
	pEffect = SHADER->GetResource("fx/Sky.fx");

	pSun = new Sun();
}

void Sky::Update(DirectionLight* light)
{
	pSun->Update(light);
}

void Sky::Render(DirectionLight* light, Camera* pCamera)
{
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	D3DXVECTOR3 cameraPosition = pCamera->GetWorldPosition();

	D3DXMATRIXA16 matWorld, matView, matProjection, matViewProjection;
	D3DXMATRIXA16 matT1, matS, matT2;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matT1, 0, -0.15f, 0);
	D3DXMatrixScaling(&matS, 100, 100, 100);
	D3DXMatrixTranslation(&matT2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	matWorld = matT1 * matS*matT2;
	matView = pCamera->GetViewMatrix();
	matProjection = pCamera->GetProjectionMatrix();

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matView", &matView);
		pEffect->SetMatrix("matProjection", &matProjection);
		pEffect->SetTexture("tNight", tNightTexture);
		pEffect->SetFloat("overcast", 1.0f);

		pEffect->SetVector3("vSunVector", light->GetLightDirection());
		pEffect->SetFloat("fSunIntensity", pSun->SunIntensity());
		pEffect->SetFloat("fTime", (float)TIMEMANAGER->getWorldTime() / 30);

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			skyDome->DrawSubset(0);
		}
		pEffect->EndPass();
		pEffect->End();
	}


	pSun->Render(light, pCamera);

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

}

void Sky::Render(DirectionLight* light, Camera* pCamera, D3DXPLANE* reflectedW)
{
	D3DXMATRIXA16 RefMtx;
	D3DXMatrixIdentity(&RefMtx);
	if (reflectedW)
		D3DXMatrixReflect(&RefMtx, reflectedW);

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	D3DXVECTOR3 cameraPosition = pCamera->GetWorldPosition();

	D3DXMATRIXA16 matWorld, matView, matProjection, matViewProjection;
	D3DXMATRIXA16 matT1, matS, matT2;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matT1, 0, -0.15f, 0);
	D3DXMatrixScaling(&matS, 1000, 1000, 1000);
	D3DXMatrixTranslation(&matT2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	matWorld = matT1 * matS* matT2*RefMtx;
	matView = pCamera->GetViewMatrix();
	matProjection = pCamera->GetProjectionMatrix();

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matView", &matView);
		pEffect->SetMatrix("matProjection", &matProjection);
		pEffect->SetTexture("tNight", tNightTexture);
		pEffect->SetFloat("overcast", 1.0f);

		pEffect->SetVector3("vSunVector", light->GetLightDirection());
		pEffect->SetFloat("fSunIntensity", pSun->SunIntensity());
		pEffect->SetFloat("fTime", (float)TIMEMANAGER->getWorldTime() / 30);

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			skyDome->DrawSubset(0);
		}
		pEffect->EndPass();
		pEffect->End();
	}


	pSun->Render(light, pCamera);

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

}

void Sky::ReverseRender(DirectionLight * light, Camera * pCamera)
{
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	D3DXVECTOR3 cameraPosition = pCamera->GetWorldPosition();

	D3DXMATRIXA16 matWorld, matView, matProjection, matViewProjection;
	D3DXMATRIXA16 matT1, matS, matT2, matR;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matT1, 0, 0.15f, 0);
	D3DXMatrixScaling(&matS, 100, 100, 100);
	D3DXMatrixRotationZ(&matR, D3DX_PI);
	D3DXMatrixTranslation(&matT2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	matWorld = matT1 * matS*matR*matT2;
	matView = pCamera->GetViewMatrix();
	matProjection = pCamera->GetProjectionMatrix();

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matView", &matView);
		pEffect->SetMatrix("matProjection", &matProjection);
		pEffect->SetTexture("tNight", tNightTexture);
		pEffect->SetFloat("overcast", 1.0f);

		pEffect->SetVector3("vSunVector", light->GetLightDirection());
		pEffect->SetFloat("fSunIntensity", pSun->SunIntensity());
		pEffect->SetFloat("fTime", (float)TIMEMANAGER->getWorldTime() / 30);

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			skyDome->DrawSubset(0);
		}
		pEffect->EndPass();
		pEffect->End();
	}

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

}
