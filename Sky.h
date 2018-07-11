#pragma once

class Sun;
class DirectionLight;

class Sky
{
private:
	LPD3DXMESH skyDome;
	LPDIRECT3DTEXTURE9 tNightTexture;
	Effect* pEffect;
	Sun* pSun;

public:
	Sky();
	~Sky();

	void LoadAssets();
	void Update(DirectionLight* light);
	void Render(DirectionLight* light,Camera* pCamera);
	void ReverseRender(DirectionLight* light, Camera* pCamera);
	void Render(DirectionLight* light, Camera* pCamera, D3DXPLANE* reflectedW);
	Sun* GetSun() { return pSun; }
};

