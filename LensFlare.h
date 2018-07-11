#pragma once

#define TOTALFLARE 9

class LensFlare
{
public:
	struct FlareElement
	{
		float linePos;		// position on line (-1 to 1) //투영좌표
		float scale;		// screen-based scale//화면 스케일
		float brightness;	// emissive property (0-1) //빛남의 정도
		float texU;		// x location in texture//텍스쳐내의 x좌표
		float texV;		// y location in texture//텍스쳐내의 y좌표
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

