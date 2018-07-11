#pragma once

#define SCENE sceneManager::GetInstance()

class SceneBase;

using namespace std;

class sceneManager : public SingletonBase<sceneManager>
{
	SINGLETONE(sceneManager)
public:
	typedef map<string, SceneBase*> mapSceneList;
	typedef map<string, SceneBase*>::iterator mapSceneIter;

private:
	static SceneBase* _currentScene; //«ˆ¿Á æ¿
	static SceneBase* _readyScene; //±≥√º¡ÿ∫Ò¡ﬂ¿Œ æ¿

	mapSceneList _mSceneList; //æ¿∏ÆΩ∫∆Æ

public:
	HRESULT init();
	void release();
	void update();
	void render();

	//æ¿ √ﬂ∞°«—¥Ÿ
	SceneBase* addScene(string sceneName, SceneBase* scene);

	//æ¿ √º¿Œ¡ˆ
	HRESULT changeScene(string sceneName);
};