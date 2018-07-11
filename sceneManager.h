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
	static SceneBase* _currentScene; //���� ��
	static SceneBase* _readyScene; //��ü�غ����� ��

	mapSceneList _mSceneList; //������Ʈ

public:
	HRESULT init();
	void release();
	void update();
	void render();

	//�� �߰��Ѵ�
	SceneBase* addScene(string sceneName, SceneBase* scene);

	//�� ü����
	HRESULT changeScene(string sceneName);
};