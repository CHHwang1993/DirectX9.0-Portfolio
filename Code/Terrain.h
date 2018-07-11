#pragma once

class TerrainCell;
class Vegetation;

class Terrain
{
public:
	

private:
	Transform * pTransform; //지형의 트랜스폼 정보
	TERRAININFO* pTerrainInfo; //정점 정보

	//터레인 정보
	int nWidth; //가로크기
	int nHeight; //세로크기
	int nTotalVertex; //총 정점 크기
	int nCellX; //가로셀 크기
	int nCellZ; //세로셀 크기
	int	nTotalCell; //총 셀의 크기
	int nTotalTri; //총 삼각형의 크기
	float fCellScale; //셀하나의 크기
	float fHeightScale; //높이 스케일

	//텍스쳐 및 이펙트
	Effect* pEffect;
	LPDIRECT3DTEXTURE9 tHeightMap; //높이맵
	LPDIRECT3DTEXTURE9 tTileTex00; //텍스쳐1
	LPDIRECT3DTEXTURE9 tTileTex01; //텍스쳐2
	LPDIRECT3DTEXTURE9 tTileTex02; //텍스쳐3
	LPDIRECT3DTEXTURE9 tTileTex03; //텍스쳐4
	LPDIRECT3DTEXTURE9 tSplatTex; //스플래팅용 텏스쳐


	//터레인셀
	int nCellCount;
	int nCellRowCount;
	int TotalCellSize;
	bool CellLineVisible;
	TerrainCell* pTerrainCells;
	
	//잔디
	Vegetation* pVegetation;

	//반사
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
	void CreateShadowRender(Camera* pLightCamera,bool use=false); //그림자맵을 그린다.
	void ReciveShadowRender(DirectionLight* light, Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog,D3DXVECTOR3 fogColor); //그림자맵을 받아서 그린다.
	
};

