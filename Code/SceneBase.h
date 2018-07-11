#pragma once

class Camera;
class DirectionLight;


class SceneBase
{
private:
	struct SCENEVERTEX //����Ʈ����Ʈ ó���� �� ����ϴ� ���� ���ؽ�����!
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 UV;
		enum {FVF = D3DFVF_XYZ | D3DFVF_TEX1};
	};
protected:
	Camera * pCamera; //���� ī�޶�
	SCENEVERTEX stSceneVertex[4]; //�� �÷��� ���ؽ����� ����ü
	DWORD dwSceneIndex[6]; // �� �÷��� �ε���

	Effect* pPostEffect; //����Ʈ ����Ʈ!

	DirectionLight* pDirLight; //�⺻ ���⼺ ����
	Camera* pDirLightCamera; //���⼺ ������ ���󰡴� ī�޶�

	bool isWireFrame; //���̾� ������ Ȯ���Ұų�? 

	int nPassNumber; //����Ʈ����Ʈ �н��ѹ�

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

