#pragma once
class Tree
{
private:
	LPD3DXMESH pTree;
	DWORD dwMaterialsNum;
	vector<LPDIRECT3DTEXTURE9> vecDiffuseTex;
	vector<LPDIRECT3DTEXTURE9> vecNormalTex;
	vector<LPDIRECT3DTEXTURE9> vecSpecularTex;
	std::vector<D3DMATERIAL9> vecMaterials;

	Effect* pEffect;
	Transform* pTransform;

public:
	Tree();
	virtual ~Tree();
	
	void LoadAsset();
	Transform* GetTransform() { return pTransform; }
	void CreateShadowRender(Camera* pLightCamera);
	void ReciveShadowRender(DirectionLight* light, Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog,D3DXVECTOR3 fogColor, D3DXPLANE* reflectedW=NULL);
};

