#include "stdafx.h"
#include "TerrainCell.h"


TerrainCell::TerrainCell()
	:vertexList(NULL), pVB(NULL), pLineVB(NULL), pIB(NULL),
	pLineDecl(NULL), pDecl(NULL), pEffect(NULL)
{
}


TerrainCell::~TerrainCell()
{
	SAFE_DELETE(pVB);
	SAFE_DELETE(pIB);
	SAFE_DELETE(pLineVB);
	SAFE_DELETE_ARRAY(vertexList);
	SAFE_RELEASE(pLineDecl);
	SAFE_RELEASE(pDecl);
}

void TerrainCell::Init(void * terrainVertexInfo,int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth)
{
	TERRAININFO* terrainInfo;

	pEffect = SHADER->GetResource("fx/ColorShader.fx");

	terrainInfo = (TERRAININFO*)terrainVertexInfo;

	IniteBuffers(nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainInfo);

	terrainInfo = NULL;

	CalculateCellDimensions();

	BuildLine();
}

void TerrainCell::IniteBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, TERRAININFO* terrainVertexInfo)
{
	TERRAININFO* vertices;
	TERRAINTRI* indices;
	int terrainIndex, index;
	HRESULT hr;

	nVertexCount = cellHeight*cellWidth;

	nIndexCount = (cellHeight-1)*(cellWidth-1)*2;

	vertices = new TERRAININFO[nVertexCount];
	assert(vertices != NULL);

	indices = new TERRAINTRI[nIndexCount];
	assert(indices != NULL);

	terrainIndex = (nodeIndexX*(cellWidth-1) + nodeIndexY* (cellHeight-1)*terrainWidth);
	index = 0;

	for (int z = 0; z < cellHeight; ++z)
	{
		for (int x = 0; x< cellWidth; ++x)
		{
			index = z * cellHeight + x;

			vertices[index].position = terrainVertexInfo[terrainIndex].position;
			vertices[index].baseUV = terrainVertexInfo[terrainIndex].baseUV;
			vertices[index].tileUV = terrainVertexInfo[terrainIndex].tileUV;
			vertices[index].normal = terrainVertexInfo[terrainIndex].normal;
			vertices[index].binormal = terrainVertexInfo[terrainIndex].binormal;
			vertices[index].tangent = terrainVertexInfo[terrainIndex].tangent;
			terrainIndex++;
		}
		terrainIndex += (terrainWidth - cellWidth);
	}

	index = 0;

	for (int z = 0; z < cellHeight - 1; ++z)
	{
		for (int x = 0; x < cellWidth-1; ++x)
		{
			DWORD lt = z * cellWidth + x;
			DWORD rt = z * cellWidth + x + 1;
			DWORD lb = ((z + 1) * cellWidth)+ x;
			DWORD rb = ((z + 1) * cellWidth)+ x + 1;

			indices[index].dw0 = lt;
			indices[index].dw1 = rt;
			indices[index].dw2 = lb;
			index++;

			indices[index].dw0 = lb;
			indices[index].dw1 = rt;
			indices[index].dw2 = rb;
			index++;
		}
	}


	pVB = new VertexBuffer();
	pVB->CreateVertexBuffer(nVertexCount * sizeof(TERRAININFO));
	pVB->LockAndCopyVertexBuffer(vertices);

	pIB = new IndexBuffer();
	pIB->CreateIndexBuffer(nIndexCount * sizeof(TERRAINTRI), D3DFMT_INDEX32);
	pIB->LockAndCopyIndexBuffer(indices);

	D3DVERTEXELEMENT9 vertElement[7] = {
		{ 0, 0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //위치 정보
	{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 }, //노말 정보
	{ 0, 24,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 }, //바이노말 정보
	{ 0, 36,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 }, //탄젠트 정보 
	{ 0, 48,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //Base UV
	{ 0, 56,D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 }  //Tile UV
	,D3DDECL_END() }; //정점정보 갯수 +1

	hr = g_pD3DDevice->CreateVertexDeclaration(vertElement, &pDecl); //LPDIRECT3DVERTEXDECLARATION9 생성
	assert(hr == S_OK);


	vertexList = new D3DXVECTOR3[nVertexCount];
	assert(vertexList != NULL);

	for (int i = 0; i < nVertexCount; ++i)
	{
		vertexList[i].x = vertices[i].position.x;
		vertexList[i].y = vertices[i].position.y;
		vertexList[i].z = vertices[i].position.z;
	}

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void TerrainCell::CalculateCellDimensions()
{
	float width, height, depth;

	stCellDimensions.maximumWidth = -100000.0f;
	stCellDimensions.maximumDepth = -100000.0f;
	stCellDimensions.maximumHeight = -100000.0f;

	stCellDimensions.minimumWidth = 100000.0f;
	stCellDimensions.minimumDepth = 100000.0f;
	stCellDimensions.minimumHeight = 100000.0f;

	for (int i = 0; i < nVertexCount; ++i)
	{
		width = vertexList[i].x;
		height = vertexList[i].y;
		depth = vertexList[i].z;

		if (width > stCellDimensions.maximumWidth) stCellDimensions.maximumWidth = width;
		if (width <= stCellDimensions.minimumWidth) stCellDimensions.minimumWidth = width;

		if (height > stCellDimensions.maximumHeight) stCellDimensions.maximumHeight = height;
		if (height <= stCellDimensions.minimumHeight) stCellDimensions.minimumHeight = height;

		if (depth > stCellDimensions.maximumDepth) stCellDimensions.maximumDepth = depth;
		if (depth <= stCellDimensions.minimumDepth) stCellDimensions.minimumDepth = depth;
	}

	vPosition.x = (stCellDimensions.maximumWidth - stCellDimensions.minimumWidth) * 0.5 + stCellDimensions.minimumWidth;
	vPosition.y = (stCellDimensions.maximumHeight - stCellDimensions.minimumHeight) * 0.5 + stCellDimensions.minimumHeight;
	vPosition.z = (stCellDimensions.maximumDepth - stCellDimensions.minimumDepth) * 0.5 + stCellDimensions.minimumDepth;
}

void TerrainCell::BuildLine()
{
	VertexPositionColor* vertices;

	D3DXVECTOR4 lineColor;
	int index, temporaryVertexCount, temporaryIndexCount;

	lineColor = D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f);

	temporaryVertexCount = 24;
	temporaryIndexCount = 24;

	vertices = new VertexPositionColor[temporaryVertexCount];
	assert(vertices != NULL);

	index = 0;

	//8개 가로 직선
	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;
	
	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;


	//4개 세로 직선
	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.maximumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.maximumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.maximumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;

	vertices[index].Position = D3DXVECTOR3(stCellDimensions.minimumWidth, stCellDimensions.minimumHeight, stCellDimensions.minimumDepth);
	vertices[index].Color = lineColor;
	index++;


	pLineVB = new VertexBuffer();
	pLineVB->CreateVertexBuffer(temporaryVertexCount * sizeof(VertexPositionColor));
	pLineVB->LockAndCopyVertexBuffer(vertices);
	
	HRESULT hr;

	D3DVERTEXELEMENT9 vertElement[3] = {
		{ 0, 0,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //위치 정보
		{ 0, 12,D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 }, //칼라 정보
		D3DDECL_END() }; //정점정보 갯수 +1

	hr = g_pD3DDevice->CreateVertexDeclaration(vertElement, &pLineDecl); //LPDIRECT3DVERTEXDECLARATION9 생성
	assert(hr == S_OK);

	nLineIndexCount = temporaryIndexCount;

	SAFE_DELETE_ARRAY(vertices);
}

void TerrainCell::LineRender(D3DXMATRIXA16* matWorld, Camera* pCamera)
{
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("gWorldMatrix", matWorld);
		pEffect->SetMatrix("gViewMatrix", &pCamera->GetViewMatrix());
		pEffect->SetMatrix("gProjectionMatrix", &pCamera->GetProjectionMatrix());

		pEffect->Begin();
		pEffect->BeginPass(0);

		g_pD3DDevice->SetStreamSource(0, pLineVB->Buffer(), 0, sizeof(VertexPositionColor));
		g_pD3DDevice->SetVertexDeclaration(pLineDecl);
		g_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 24);

		pEffect->EndPass();
		pEffect->End();
	}

	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void TerrainCell::Render()
{
	g_pD3DDevice->SetStreamSource(0, pVB->Buffer(), 0, sizeof(TERRAININFO));
	g_pD3DDevice->SetVertexDeclaration(this->pDecl);
	g_pD3DDevice->SetIndices(pIB->Buffer());
	g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertexCount,0, nIndexCount);
}
