#pragma once

class Tree;

class Vegetation
{
private:
	VertexBuffer * pVB;
	LPDIRECT3DVERTEXDECLARATION9 pDecl;
	Effect* pEffect;
	LPDIRECT3DTEXTURE9 tGrassTexture;
	Tree* pTree;
	vector<Tree*> vecTree;
public:
	Vegetation();
	~Vegetation();

	void VegetationSetting(TERRAININFO* terrainVertices, int totalVertex);
	void Render(Camera* pCamera, Camera* pLightCamera, DirectionLight* pLight, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor, D3DXPLANE* reflectPlane = NULL);
	void CreateShadowRender(Camera* pLightCamera);
};

