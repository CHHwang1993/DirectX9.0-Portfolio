#pragma once

class Camera;
class DirectionLight;


class SceneBase
{
private:
	struct SCENEVERTEX //포스트이펙트 처리할 때 사용하는 씬의 버텍스정보!
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 UV;
		enum {FVF = D3DFVF_XYZ | D3DFVF_TEX1};
	};
protected:
	Camera * pCamera; //메인 카메라
	SCENEVERTEX stSceneVertex[4]; //씬 플레인 버텍스정보 구조체
	DWORD dwSceneIndex[6]; // 씬 플레인 인덱스

	Effect* pPostEffect; //포스트 이펙트!

	DirectionLight* pDirLight; //기본 방향성 광원
	Camera* pDirLightCamera; //방향성 광원을 따라가는 카메라

	bool isWireFrame; //와이어 프레임 확인할거냐? 

	int nPassNumber; //포스트이펙트 패스넘버

	float fBloomIntensity;
	float shadowDistance;

public:
	SceneBase();
	virtual ~SceneBase();

	HRESULT Init();
	void Release();
	void Update();
	void Render();

protected:
	virtual HRESULT Scene_Init() = 0;
	virtual void Scene_Release() = 0;
	virtual void Scene_Update() = 0;
	virtual void Scene_Render() = 0;
};

