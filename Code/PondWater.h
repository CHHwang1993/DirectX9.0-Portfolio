#pragma once

class PondWater
{
public:
	struct WaterInfo
	{
		DirLight dirLight;
		Mtrl mtrl;
		int vertX; 
		int vertZ;
		float dX;
		float dZ;
		D3DXVECTOR2 waveMapVelocity0;
		D3DXVECTOR2 waveMapVelocity1;
		float texScale;
		float refractBias;
		float refractPower;
		D3DXVECTOR2 rippleScale;
		D3DXMATRIXA16 matWorld;
	};
public:
	LPDIRECT3DTEXTURE9 pReflectMap; //반사맵 
	LPDIRECT3DTEXTURE9 pRefractMap; //굴절맵
	LPDIRECT3DSURFACE9 pReflectSurface;
	LPDIRECT3DSURFACE9 pRefractSurface;
private:
	//물이펙트
	Effect * pEffect; 

	//두개의 노말맵텍스쳐
	LPDIRECT3DTEXTURE9 pWaveMap0; 
	LPDIRECT3DTEXTURE9 pWaveMap1;

	//두개의 노말맵 오프셋 
	D3DXVECTOR2 vWaveMapOffset0;
	D3DXVECTOR2 vWaveMapOffset1;

	//워터 관련 정보
	WaterInfo stWaterInfo; 
	float fWidth;
	float fDepth;
	DWORD numTris;
	DWORD numVerts;

	//버텍스 인덱스 버퍼 정보
	VertexBuffer* pVB;
	IndexBuffer* pIB;
	LPDIRECT3DVERTEXDECLARATION9 pDecl;

public:
	PondWater(WaterInfo& initInfo);
	~PondWater();
	void InitVIB();
	void BuildFX();
	void Update(DirectionLight* pLight);
	void Draw(Camera* pCamera,D3DXVECTOR4 fog, D3DXVECTOR3 fogColor);

	void ReflectMapBegin();
	void ReflectMapEnd();
	void RefractMapBegin();
	void RefractMapEnd();

	D3DXMATRIXA16 GetWorld() { return stWaterInfo.matWorld; }
};

