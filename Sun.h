#pragma once

class DirectionLight;

struct VertexSun
{
	D3DXVECTOR3 position;//위치
	float size;//크기

	static LPDIRECT3DVERTEXDECLARATION9 Decl;

	VertexSun() : position(0,0,0), size(0.0f) {}

	VertexSun(D3DXVECTOR3 pos, float size) :position(pos), size(size) {}
};

class Sun
{
private:
	Effect * pEffect;
	LPDIRECT3DTEXTURE9 sunTexture;
	D3DXVECTOR3 sunPosition;

	float sunIntensity;
	float ambientIntensity;
	D3DXVECTOR3 ambientColor;
	D3DXVECTOR3 sunColor;
	D3DXVECTOR3 fogColor;

	//포그
	D3DXVECTOR4 vFog;
	float fFar;
	float fNear;

public:
	Sun();
	~Sun();
	
	void Update(DirectionLight* pLight);
	void Render(DirectionLight* pLight,Camera* pCamera);

	float SunIntensity() { return sunIntensity; }
	float AmbientIntensity() { return ambientIntensity; }
	D3DXVECTOR3 SunColor() { return sunColor; }
	D3DXVECTOR3 AmbientColor() { return ambientColor; }
	D3DXVECTOR3 FogColor() { return fogColor; }
	D3DXVECTOR3 SunPosition() { return sunPosition; }
	D3DXVECTOR4 Fog() { return vFog; }
};

