#include "stdafx.h"
#include "sceneManager.h"
#include "SceneBase.h"

SceneBase* sceneManager::_currentScene = NULL;
SceneBase* sceneManager::_readyScene = NULL;

sceneManager::sceneManager(void)
{
}

sceneManager::~sceneManager(void)
{ 
}

HRESULT sceneManager::init(void)
{
	_currentScene = NULL;
	_readyScene = NULL;

	return S_OK;
}

void sceneManager::release(void)
{
	//�ش�Ű �˻��ϰ�..
	mapSceneIter iter = _mSceneList.begin();

	//�� ��ü�� ���鼭 �����ش�
	for (; iter != _mSceneList.end();)
	{
		//�������� �ݺ��� ������Ű�� �ʴ´�
		if (iter->second != NULL)
		{
			SAFE_RELEASE(iter->second);
			iter = _mSceneList.erase(iter);
		}
		else
		{
			//�ݺ��� ����..
			++iter;
		}
	}

	_mSceneList.clear();
}

void sceneManager::update(void)
{
	if (_currentScene)
	{
		_currentScene->Update();
	}
}

void sceneManager::render(void)
{
	if (_currentScene)
	{
		_currentScene->Render();
	}
}


//�� �߰��Ѵ�
SceneBase* sceneManager::addScene(string sceneName, SceneBase* scene)
{
	if (!scene) return NULL;

	_mSceneList.insert(make_pair(sceneName, scene));

	return scene;
}

//�� ü����
HRESULT sceneManager::changeScene(string sceneName)
{
	//�ٲ� �� ã��...
	mapSceneIter find = _mSceneList.find(sceneName);

	//��ã���� �޽�
	if (find == _mSceneList.end()) return E_FAIL;

	//�ٲ� �� �ʱ�ȭ
	if (SUCCEEDED(find->second->Init()))
	{
		//���� ���� ������ ������
		if (_currentScene) SAFE_RELEASE(_currentScene);
		_currentScene = find->second;

		return S_OK;
	}

	return E_FAIL;
}

