#pragma once
class TerrainCell
{
private:
	struct CellDimension
	{
		float maximumWidth;
		float maximumHeight;
		float maximumDepth;
		float minimumWidth;
		float minimumHeight;
		float minimumDepth;
	};
private:
	VertexBuffer * pVB;
	VertexBuffer* pLineVB;
	IndexBuffer* pIB;
	CellDimension stCellDimensions;
	D3DXVECTOR3 vPosition;

	int nVertexCount, nIndexCount, nLineIndexCount;

	LPDIRECT3DVERTEXDECLARATION9 pDecl;
	LPDIRECT3DVERTEXDECLARATION9 pLineDecl;

	Effect* pEffect;

	vector<VertexPositionColor> pVertex;

public:
	D3DXVECTOR3 * vertexList;

public:
	TerrainCell();
	~TerrainCell();

	void Init(void* terrainVertexInfo,int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
	void LineRender(D3DXMATRIXA16* matWorld, Camera* pCamera);
	void Render();

	CellDimension GetCellDimensions() { return stCellDimensions; }

private:
	void IniteBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, TERRAININFO* terrainVertexInfo);
	void CalculateCellDimensions();
	void BuildLine();
	
};

