#include "stdafx.h"
#include "cMainGame.h"
#include "Camera.h"

cMainGame::cMainGame()
{
	bgColor = D3DCOLOR_ARGB(255, 128, 196, 196);
}


cMainGame::~cMainGame()
{
}

HRESULT cMainGame::init()
{
	SingleToneSetting(); //싱글톤을 생성한다.

	SCENE->addScene("BaseScene", new BaseScene);
	SCENE->addScene("BaseScene2", new BaseScene2);
	SCENE->addScene("MainScene", new MainScene);

	SCENE->changeScene("MainScene");

	return S_OK;
}

void cMainGame::release()
{
	SingleToneRelease(); //싱글톤을 해제한다.
}

void cMainGame::Update()
{
	SCENE->update();
}
	

void cMainGame::Render()
{
	g_pD3DDevice->Clear(NULL,NULL,	D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bgColor,1.0f, 0);

	g_pD3DDevice->BeginScene();

	SCENE->render(); //씬을 렌더한다.

	TIMEMANAGER->render(); //타임매니저 렌더

	TweakBar::Get()->Render(); //트위크바 렌더

	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void cMainGame::SingleToneRelease()
{
	TweakBar::Delete();
	Random::Delete();
	Mouse::Delete();
	Keyboard::Delete();

	SCENE->release();
	//타임 매니져 싱글톤 해제
	TIMEMANAGER->release();
	//쉐이더 매니저 해제
	SHADER->ClearResource();
	Resource_Shader::ReleaseInstance();
	//텍스처 매니저 해제
	TEXTURE2D->ClearResource();
	Resource_Texture::ReleaseInstance();
	//모델 매니저 해제
	MODEL->ClearResource();
	Resource_Model::ReleaseInstance();

	g_pDeviceManager->Destroy();
}

void cMainGame::SingleToneSetting()
{
	TweakBar::Get()->Start();
	Random::Create();
	Mouse::Create();
	Keyboard::Create();
	SCENE->init();		//신매니저 셋팅
	TIMEMANAGER->init();		//타임매니져 셋팅
}

