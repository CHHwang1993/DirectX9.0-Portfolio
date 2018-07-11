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
	SingleToneSetting(); //�̱����� �����Ѵ�.

	SCENE->addScene("BaseScene", new BaseScene);
	SCENE->addScene("BaseScene2", new BaseScene2);
	SCENE->addScene("MainScene", new MainScene);

	SCENE->changeScene("MainScene");

	return S_OK;
}

void cMainGame::release()
{
	SingleToneRelease(); //�̱����� �����Ѵ�.
}

void cMainGame::Update()
{
	SCENE->update();
}
	

void cMainGame::Render()
{
	g_pD3DDevice->Clear(NULL,NULL,	D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bgColor,1.0f, 0);

	g_pD3DDevice->BeginScene();

	SCENE->render(); //���� �����Ѵ�.

	TIMEMANAGER->render(); //Ÿ�ӸŴ��� ����

	TweakBar::Get()->Render(); //Ʈ��ũ�� ����

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
	//Ÿ�� �Ŵ��� �̱��� ����
	TIMEMANAGER->release();
	//���̴� �Ŵ��� ����
	SHADER->ClearResource();
	Resource_Shader::ReleaseInstance();
	//�ؽ�ó �Ŵ��� ����
	TEXTURE2D->ClearResource();
	Resource_Texture::ReleaseInstance();
	//�� �Ŵ��� ����
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
	SCENE->init();		//�ŸŴ��� ����
	TIMEMANAGER->init();		//Ÿ�ӸŴ��� ����
}

