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
	LPDIRECT3DTEXTURE9 pReflectMap; //�ݻ�� 
	LPDIRECT3DTEXTURE9 pRefractMap; //������
	LPDIRECT3DSURFACE9 pReflectSurface;
	LPDIRECT3DSURFACE9 pRefractSurface;
private:
	//������Ʈ
	Effect * pEffect; 

	//�ΰ��� �븻���ؽ���
	LPDIRECT3DTEXTURE9 pWaveMap0; 
	LPDIRECT3DTEXTURE9 pWaveMap1;

	//�ΰ��� �븻�� ������ 
	D3DXVECTOR2 vWaveMapOffset0;
	D3DXVECTOR2 vWaveMapOffset1;

	//���� ���� ����
	WaterInfo stWaterInfo; 
	float fWidth;
	float fDepth;
	DWORD numTris;
	DWORD numVerts;

	//���ؽ� �ε��� ���� ����
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

