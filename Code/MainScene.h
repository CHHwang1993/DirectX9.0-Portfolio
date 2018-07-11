#pragma once
#include "SceneBase.h"

class LensFlare;
class Sky;
class Terrain;
class PondWater;

class MainScene : public SceneBase
{
private:
	LensFlare* Lens;
	PondWater* pPondWater;
	Sky* m_pSky;
	Terrain* pTerrain;
	D3DXPLANE reflectPlane;

	bool IsLensVisible;
	bool IsRTVisible;

protected:
	virtual HRESULT Scene_Init(void);
	virtual void Scene_Release(void);
	virtual void Scene_Update(void);
	virtual void Scene_Render(void);
	
public:
	MainScene();
	virtual ~MainScene();

	void PondWaterInit();
	void CreateTerrainShadow();
	void DrawScene();
};

