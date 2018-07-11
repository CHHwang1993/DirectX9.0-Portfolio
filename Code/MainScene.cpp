#include "stdafx.h"
#include "MainScene.h"
#include "waterShader.h"
#include "SpecularMapping.h"
#include "Environment.h"
#include "cGrid.h"
#include "LensFlare.h"
#include "Sun.h"
#include "Sky.h"
#include "Terrain.h"
#include "PondWater.h"
#include "Fog.h"
#include "Tree.h"


MainScene::MainScene()
	:pPondWater(NULL), IsLensVisible(false), IsRTVisible(false)
{
	TwBar* bar = TweakBar::Get()->GetBar();
	TwAddVarRW(bar, "CamSpeed", TW_TYPE_FLOAT, &this->pCamera->camSpeed, "group='Camera'");
	TwAddVarRW(bar, "CamRotSpeed", TW_TYPE_FLOAT, &this->pCamera->camRotSpeed, "group='Camera'");
	TwAddVarRW(bar, "IsLensVisible", TW_TYPE_BOOL32, &IsLensVisible, "group='LensFlare'");
	TwAddVarRW(bar, "IsRTVisible", TW_TYPE_BOOL32, &IsRTVisible, "group='Render Target'");
}


MainScene::~MainScene()
{
}

HRESULT MainScene::Scene_Init(void)
{

	Lens = new LensFlare();
	Lens->LoadAssets();

	m_pSky = new Sky();
	m_pSky->LoadAssets();

	pTerrain = new Terrain();
	pTerrain->init(
		"Texture/MyHeight512.bmp",
		"Texture/terrain1.jpg",
		"Texture/terrain2.png",
		"Texture/terrain3.png",
		"Texture/terrain4.png",
		"Texture/Splat.png",
		1.0f,
		200.0f,
		3,
		100);

	PondWaterInit();
	return S_OK;
}

void MainScene::Scene_Release(void)
{
	SAFE_DELETE(Lens);
	SAFE_DELETE(m_pSky);
	SAFE_DELETE(pTerrain);
	SAFE_DELETE(pPondWater);
}

void MainScene::Scene_Update(void)
{
	if (Keyboard::Get()->Press(VK_UP))pDirLight->RotateX(-2.0f*TIMEMANAGER->getElapsedTime());
	else if (Keyboard::Get()->Press(VK_DOWN))pDirLight->RotateX(2.0f*TIMEMANAGER->getElapsedTime());

	if (m_pSky) m_pSky->Update(pDirLight);
	
	if (Lens) Lens->Update(&m_pSky->GetSun()->SunPosition(),pCamera);

	if (pPondWater) pPondWater->Update(this->pDirLight);

	CreateTerrainShadow();
}

void MainScene::Scene_Render(void)
{
	DrawScene();
}

void MainScene::PondWaterInit()
{
	DirLight light;
	D3DXMATRIXA16 matWorld;

	light.dirW = pDirLight->GetLightDirection();
	light.ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	light.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.spec = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	D3DXMatrixTranslation(&matWorld, 0.0f, 15.0f, 0.0f);

	Mtrl waterMtrl;
	waterMtrl.ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.00f);
	waterMtrl.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.00f);
	waterMtrl.spec = 0.6f*WHITE;
	waterMtrl.specPower = 200.0f;

	PondWater::WaterInfo waterInitInfo;
	waterInitInfo.dirLight = light;
	waterInitInfo.mtrl = waterMtrl;
	waterInitInfo.vertX = 128;
	waterInitInfo.vertZ = 128;
	waterInitInfo.dX = 8.0f;
	waterInitInfo.dZ = 8.0f;
	waterInitInfo.waveMapVelocity0 = D3DXVECTOR2(0.09f, 0.06f);
	waterInitInfo.waveMapVelocity1 = D3DXVECTOR2(-0.05f, 0.08f);
	waterInitInfo.texScale = 100.0f;
	waterInitInfo.refractBias = 0.1f;
	waterInitInfo.refractPower = 1.0f;
	waterInitInfo.rippleScale = D3DXVECTOR2(0.006f, 0.003f);
	waterInitInfo.matWorld = matWorld;

	pPondWater = new PondWater(waterInitInfo);
}

void MainScene::CreateTerrainShadow()
{
	this->pDirLightCamera->SetWorldPosition(m_pSky->GetSun()->SunPosition()*0.1f);
	this->pDirLightCamera->LookDirection(pDirLight->pTransform->GetForward());

	this->pDirLightCamera->UpdateMatrix();

	this->pDirLightCamera->RenderTextureBegin(0xffffffff); //렌더타겟 텍스쳐 그리기(그림자맵)

	pTerrain->CreateShadowRender(this->pDirLightCamera); //쉐도우맵 그리기

	this->pDirLightCamera->RenderTextureEnd(); //렌더타겟 텍스쳐 종료
}

void MainScene::DrawScene()
{
	// Reflection plane in local space.
	// 로컬스페이스 안 반사평면
	D3DXPLANE waterPlaneL(0.0f, -1.0f, 0.0f, 0.0f);
	
	// Reflection plane in world space.
	//월드스페이스 안 반사평면
	D3DXMATRIX WInvTrans;
	D3DXMatrixInverse(&WInvTrans, 0, &pPondWater->GetWorld());
	D3DXMatrixTranspose(&WInvTrans, &WInvTrans);
	D3DXPlaneTransform(&reflectPlane, &waterPlaneL, &WInvTrans);
	
	// Reflection plane in homogeneous clip space.
	//동일한 클립 스페이스 안 반사평면
	D3DXMATRIX WVPInvTrans;
	D3DXMatrixInverse(&WVPInvTrans, 0, &(pPondWater->GetWorld()*pCamera->GetViewProjectionMatrix()));
	D3DXMatrixTranspose(&WVPInvTrans, &WVPInvTrans);
	D3DXPLANE waterPlaneH;
	D3DXPlaneTransform(&waterPlaneH, &waterPlaneL, &WVPInvTrans);
	
	float f[4] = { waterPlaneH.a, waterPlaneH.b, waterPlaneH.c, waterPlaneH.d };
	
	//클립평면을 세팅해서 굴절에 불필요한 렌더를 제거
	g_pD3DDevice->SetClipPlane(0, (float*)f);
	g_pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, true);
	
	pPondWater->RefractMapBegin();
	
	//굴절용 렌더링
	if (m_pSky) m_pSky->Render(pDirLight, pCamera);
	if (pTerrain) pTerrain->ReciveShadowRender(pDirLight, pCamera, pDirLightCamera, m_pSky->GetSun()->Fog(), m_pSky->GetSun()->FogColor()); //그려진 쉐도우맵과 기존 맵 2패스!
	
	pPondWater->RefractMapEnd();

	//클립평면을 세팅해서 반사에 불필요한 렌더를 제거
	g_pD3DDevice->SetClipPlane(0, (float*)f);
	g_pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, true);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	pPondWater->ReflectMapBegin();

	if (m_pSky) m_pSky->Render(pDirLight, pCamera,&reflectPlane);
	if (pTerrain) pTerrain->Render(pDirLight, pCamera, pDirLightCamera,m_pSky->GetSun()->Fog(), m_pSky->GetSun()->FogColor(), &reflectPlane); //그려진 쉐도우맵과 기존 맵 2패스!

	pPondWater->ReflectMapEnd();
	//클립평면 렌더 방식을 끈다.
	g_pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//화면 한번 깨끗하게 밀고~
	g_pD3DDevice->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1,1,1,1), 1.0f, 0);

	//메인 렌더 실시!
	if (m_pSky) m_pSky->Render(pDirLight, pCamera);
	if (m_pSky) m_pSky->ReverseRender(pDirLight, pCamera);
	if (pTerrain) pTerrain->ReciveShadowRender(pDirLight, pCamera, pDirLightCamera, m_pSky->GetSun()->Fog(),m_pSky->GetSun()->FogColor()); //그려진 쉐도우맵과 기존 맵 2패스!
	if (pPondWater) pPondWater->Draw(pCamera, m_pSky->GetSun()->Fog(),m_pSky->GetSun()->FogColor());
	if (Lens && IsLensVisible) Lens->Render();

	if (IsRTVisible)
	{
		////디버그용 텍스쳐 출력
		float scale = 256;
		typedef struct
		{
			D3DXVECTOR4 Position;
			D3DXVECTOR2 uv;
		}TVERTEX;

		TVERTEX Vertex[4] =
		{
			{ D3DXVECTOR4(0,0,0,1),D3DXVECTOR2(0,0) },
		{ D3DXVECTOR4(scale,0,0,1),D3DXVECTOR2(1,0) },
		{ D3DXVECTOR4(scale,scale,0,1),D3DXVECTOR2(1,1) },
		{ D3DXVECTOR4(0,scale,0,1),D3DXVECTOR2(0,1) }
		};

		g_pD3DDevice->SetTexture(0, this->pDirLightCamera->GetRenderTexture());
		g_pD3DDevice->SetVertexShader(NULL);
		g_pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pD3DDevice->SetPixelShader(NULL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(TVERTEX));
	}
}
