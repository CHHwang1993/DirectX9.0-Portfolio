#pragma once

class TerrainCell;
class Vegetation;

class Terrain
{
public:
	

private:
	Transform * pTransform; //������ Ʈ������ ����
	TERRAININFO* pTerrainInfo; //���� ����

	//�ͷ��� ����
	int nWidth; //����ũ��
	int nHeight; //����ũ��
	int nTotalVertex; //�� ���� ũ��
	int nCellX; //���μ� ũ��
	int nCellZ; //���μ� ũ��
	int	nTotalCell; //�� ���� ũ��
	int nTotalTri; //�� �ﰢ���� ũ��
	float fCellScale; //���ϳ��� ũ��
	float fHeightScale; //���� ������

	//�ؽ��� �� ����Ʈ
	Effect* pEffect;
	LPDIRECT3DTEXTURE9 tHeightMap; //���̸�
	LPDIRECT3DTEXTURE9 tTileTex00; //�ؽ���1
	LPDIRECT3DTEXTURE9 tTileTex01; //�ؽ���2
	LPDIRECT3DTEXTURE9 tTileTex02; //�ؽ���3
	LPDIRECT3DTEXTURE9 tTileTex03; //�ؽ���4
	LPDIRECT3DTEXTURE9 tSplatTex; //���÷��ÿ� ������


	//�ͷ��μ�
	int nCellCount;
	int nCellRowCount;
	int TotalCellSize;
	bool CellLineVisible;
	TerrainCell* pTerrainCells;
	
	//�ܵ�
	Vegetation* pVegetation;

	//�ݻ�
	D3DXMATRIXA16 RefMtx;
public:
	Terrain();
	~Terrain();

	void init(const char* heightName, const char* tile00, const char* tile01, const char* tile02, const char* tile03,
		const char* splat, float cellScale, float heightScale, int smooths, int tileNum);

	void CreateTerrain(int smooths, int tileNum);
	void ApplySmoothing(int smooths);
	void LoadTerrainCells();

	void Render(DirectionLight* light,Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor, D3DXPLANE* reflectedW = NULL);
	void CreateShadowRender(Camera* pLightCamera,bool use=false); //�׸��ڸ��� �׸���.
	void ReciveShadowRender(DirectionLight* light, Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog,D3DXVECTOR3 fogColor); //�׸��ڸ��� �޾Ƽ� �׸���.
	
};

