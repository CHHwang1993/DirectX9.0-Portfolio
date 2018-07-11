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
	//해당키 검색하고..
	mapSceneIter iter = _mSceneList.begin();

	//맵 전체를 돌면서 지워준다
	for (; iter != _mSceneList.end();)
	{
		//지워지면 반복자 증가시키지 않는다
		if (iter->second != NULL)
		{
			SAFE_RELEASE(iter->second);
			iter = _mSceneList.erase(iter);
		}
		else
		{
			//반복자 증가..
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


//씬 추가한다
SceneBase* sceneManager::addScene(string sceneName, SceneBase* scene)
{
	if (!scene) return NULL;

	_mSceneList.insert(make_pair(sceneName, scene));

	return scene;
}

//씬 체인지
HRESULT sceneManager::changeScene(string sceneName)
{
	//바꿀 씬 찾고...
	mapSceneIter find = _mSceneList.find(sceneName);

	//못찾으면 펄스
	if (find == _mSceneList.end()) return E_FAIL;

	//바꿀 씬 초기화
	if (SUCCEEDED(find->second->Init()))
	{
		//기존 씬이 있으면 릴리즈
		if (_currentScene) SAFE_RELEASE(_currentScene);
		_currentScene = find->second;

		return S_OK;
	}

	return E_FAIL;
}

