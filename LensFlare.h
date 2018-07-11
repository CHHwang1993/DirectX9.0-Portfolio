#pragma once

#define TOTALFLARE 9

class LensFlare
{
public:
	struct FlareElement
	{
		float linePos;		// position on line (-1 to 1) //������ǥ
		float scale;		// screen-based scale//ȭ�� ������
		float brightness;	// emissive property (0-1) //������ ����
		float texU;		// x location in texture//�ؽ��ĳ��� x��ǥ
		float texV;		// y location in texture//�ؽ��ĳ��� y��ǥ
	};


private:
	VertexBuffer*  pVB;
	IndexBuffer* pIB;
	Effect* pEffect;
	LPDIRECT3DTEXTURE9 LensTexture;
	FlareElement stFlareElements[TOTALFLARE];
	bool IsVisible;
	D3DXVECTOR3 vScreenPos;
	D3DXVECTOR4 vFlareColor;
	D3DXVECTOR3 vSunPos;
	D3DXVECTOR3 vCamPos;
	LPDIRECT3DVERTEXDECLARATION9 pVertexDeclaration;

public:
	LensFlare();
	~LensFlare();

	void LoadAssets();
	void VertexSetting();
	void Update(D3DXVECTOR3* sunPos,Camera* pCamera);
	void Render();
};

