#include "stdafx.h"
#include "Terrain.h"
#include "TerrainCell.h"
#include "Vegetation.h"

Terrain::Terrain()
	:pTransform(NULL), pTerrainInfo(NULL)
	, pEffect(NULL), tHeightMap(NULL), tTileTex00(NULL), tTileTex01(NULL), tTileTex02(NULL), tTileTex03(NULL), tSplatTex(NULL), TotalCellSize(0)
	, CellLineVisible(false)
{
	pTransform = new Transform();

	D3DXMatrixIdentity(&RefMtx);

	TwBar* bar = TweakBar::Get()->GetBar();
	TwAddVarRW(bar, "CellLineVisible", TW_TYPE_BOOL32, &CellLineVisible, "group='Terrain'");
	TwAddVarRO(bar, "TerrainCellSize", TW_TYPE_INT32, &TotalCellSize, "group='Terrain'");
}


Terrain::~Terrain()
{
	SAFE_DELETE(pTransform);
	SAFE_DELETE(pVegetation);
	SAFE_DELETE_ARRAY(pTerrainInfo);
	SAFE_DELETE_ARRAY(pTerrainCells);
}

void Terrain::init(const char * heightName, const char * tile00, const char * tile01, const char * tile02, const char * tile03, 
	const char * splat, float cellScale, float heightScale, int smooths, int tileNum)
{
	fCellScale = cellScale;
	fHeightScale = heightScale;

	tHeightMap = TEXTURE2D->GetResource(heightName);

	D3DSURFACE_DESC desc; //이미지 정보
	tHeightMap->GetLevelDesc(0, &desc);

	nWidth = desc.Width;//가로크기
	nHeight = desc.Height;//세로크기
	nTotalVertex = nWidth * nHeight; //총정점 크기

	nCellX = nWidth - 1;
	nCellZ = nHeight - 1;

	nTotalCell = nCellX * nCellZ;
	nTotalTri = nTotalCell * 2;

	this->CreateTerrain(smooths, tileNum);

	this->LoadTerrainCells();
	
	tTileTex00 = TEXTURE2D->GetResource(tile00);
	tTileTex01 = TEXTURE2D->GetResource(tile01);
	tTileTex02 = TEXTURE2D->GetResource(tile02);
	tTileTex03 = TEXTURE2D->GetResource(tile03);
	tSplatTex = TEXTURE2D->GetResource(splat);

	pEffect = SHADER->GetResource("fx/TerrainBase.fx");

	pVegetation = new Vegetation();
	pVegetation->VegetationSetting(pTerrainInfo, nTotalVertex);
}

void Terrain::CreateTerrain(int smooths, int tileNum)
{
	float tileIntervalX = static_cast<float>(tileNum) / nCellX;
	float tileIntervalY = static_cast<float>(tileNum) / nCellZ;

	pTerrainInfo = new TERRAININFO[nTotalVertex];

	HRESULT hr;
	D3DLOCKED_RECT lockRect;
	hr = tHeightMap->LockRect(0, &lockRect, 0, 0);
	assert(hr == S_OK);


	for (int z = 0; z < nHeight; ++z)
	{
		for (int x = 0; x < nWidth; ++x)
		{
			//정점 포지션 계산

			D3DXVECTOR3 pos;

			pos.x = (x - (nWidth*0.5f))*fCellScale;
			pos.z = (-z + (nHeight*0.5f))*fCellScale;

			if (x == nWidth - 1) //가로 마지막 라인이라면 이전 왼쪽정점과 같은 높이를 맞춘다.
			{
				int idx = z * nWidth + x - 1;
				pos.y = pTerrainInfo[idx].position.y;
			}
			else if (z == nHeight - 1) //세로 마지막 라인이라면 이전 위쪽정점과 같은 높이를 맞춘다.
			{
				int idx = (z - 1)*nHeight + x;
				pos.y = pTerrainInfo[idx].position.y;
			}
			else
			{
				DWORD* pStart = (DWORD*)lockRect.pBits; //DWORD로 형변환 된 이미지 시작 주소
				DWORD dwColor = *(pStart + (z*(lockRect.Pitch / 4) + x));

				float inv = 1.0f / 255.0f;
				float r = ((dwColor & 0x00ff0000 >> 16))*inv;
				float g = ((dwColor & 0x0000ff00 >> 8))*inv;
				float b = ((dwColor & 0x000000ff))*inv;

				//높이맵값
				float factor = (r + g + b) / 3.0f;

				pos.y = factor * fHeightScale;
			}


			//정점 UV 계산
			D3DXVECTOR2 baseUV;
			baseUV.x = x / static_cast<float>(nWidth - 1);
			baseUV.y = z / static_cast<float>(nHeight - 1);

			D3DXVECTOR2 tileUV;
			tileUV.x = x * tileIntervalX;
			tileUV.y = z * tileIntervalY;

			//배열 인덱스계산
			int idx = z * nWidth + x;

			pTerrainInfo[idx].position = pos;
			pTerrainInfo[idx].normal = D3DXVECTOR3(0,0,0);
			pTerrainInfo[idx].baseUV = baseUV;
			pTerrainInfo[idx].tileUV = tileUV;
		}
	}

	hr = tHeightMap->UnlockRect(0);
	assert(hr == S_OK);

	ApplySmoothing(smooths);

	TERRAINTRI* pTerrainTri = new TERRAINTRI[nTotalTri];

	//배열 인덱스
	int idx = 0;

	for (DWORD z = 0; z < nCellZ; ++z)
	{
		for (DWORD x = 0; x < nCellX; ++x)
		{
			// lt-----rt
			//  |    /|
			//  |   / |
			//  |  /  |
			//  | /   |
			//  |/    |
			// lb-----rb

			DWORD lt = z * nWidth + x;
			DWORD rt = z * nWidth + x+1;
			DWORD lb = ((z+1) * nWidth) + x;
			DWORD rb = ((z+1) * nWidth) + x+1;

			pTerrainTri[idx].dw0 = lt;
			pTerrainTri[idx].dw1 = rt;
			pTerrainTri[idx].dw2 = lb;
			idx++;

			pTerrainTri[idx].dw0 = lb;
			pTerrainTri[idx].dw1 = rt;
			pTerrainTri[idx].dw2 = rb;
			idx++;
		}
		int a = 0;
	}

	//노말 , 바이노말, 탄젠트 계산
	D3DXVECTOR3* poses = new D3DXVECTOR3[nTotalVertex];
	D3DXVECTOR3* normals = new D3DXVECTOR3[nTotalVertex];
	D3DXVECTOR3* biNormals = new D3DXVECTOR3[nTotalVertex];
	D3DXVECTOR3* tangents = new D3DXVECTOR3[nTotalVertex];
	D3DXVECTOR2* uvs = new D3DXVECTOR2[nTotalVertex];
	DWORD* indices = (DWORD*)pTerrainTri;

	for (int i = 0; i < nTotalVertex; ++i)
	{
		poses[i] = this->pTerrainInfo[i].position;
		uvs[i] = this->pTerrainInfo[i].baseUV;
	}

	CalculateNormal(normals, poses, nTotalVertex, indices, nTotalTri*3);

	CalculateTangentBinormal(tangents, biNormals, poses, normals, uvs, indices, nTotalTri, nTotalVertex);

	for (int i = 0; i < nTotalVertex; ++i)
	{
		this->pTerrainInfo[i].normal = normals[i];
		this->pTerrainInfo[i].binormal = biNormals[i];
		this->pTerrainInfo[i].tangent = tangents[i];
	}

	//위에서 썼던 임시배열 해제

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(poses);
	SAFE_DELETE_ARRAY(normals);
	SAFE_DELETE_ARRAY(biNormals);
	SAFE_DELETE_ARRAY(tangents);
	SAFE_DELETE_ARRAY(uvs);
}

void Terrain::ApplySmoothing(int smooths)
{
	if (smooths <= 0)return;

	float* smooth = new float[nTotalVertex];

	while (smooths > 0)
	{
		smooths--;

		for (int z = 0; z < nHeight; z++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				int adjacentSections = 0;		//몇개의 정점과 평균값을 내니?
				float totalSections = 0.0f;		//주변의 정점 높이 총합은 얼마니?

												//왼쪽체크
				if ((x - 1) > 0) {
					totalSections += pTerrainInfo[(z * nWidth) + (x - 1)].position.y;
					adjacentSections++;

					//왼쪽 상단
					if ((z - 1) > 0) {
						totalSections += pTerrainInfo[((z - 1) * nWidth) + (x - 1)].position.y;
						adjacentSections++;
					}

					//왼쪽 하단
					if ((z + 1) < nHeight) {
						totalSections += pTerrainInfo[((z + 1) * nWidth) + (x - 1)].position.y;
						adjacentSections++;
					}
				}

				//오른쪽 체크
				if ((x + 1) < nWidth) {
					totalSections += pTerrainInfo[(z * nWidth) + (x + 1)].position.y;
					adjacentSections++;

					//오른쪽 상단
					if ((z - 1) > 0) {
						totalSections += pTerrainInfo[((z - 1) * nWidth) + (x + 1)].position.y;
						adjacentSections++;
					}

					//오른쪽 하단 
					if ((z + 1) < nHeight) {
						totalSections += pTerrainInfo[((z + 1) * nWidth) + (x + 1)].position.y;
						adjacentSections++;
					}
				}


				//상단
				if ((z - 1) > 0)
				{
					totalSections += pTerrainInfo[((z - 1) * nWidth) + x].position.y;
					adjacentSections++;
				}

				//하단
				if ((z + 1) < nHeight)
				{
					totalSections += pTerrainInfo[((z + 1) * nWidth) + x].position.y;
					adjacentSections++;
				}

				smooth[(z * nWidth) + x] = (
					pTerrainInfo[(z * nWidth) + x].position.y +
					(totalSections / adjacentSections)) * 0.5f;
			}
		}

		//위에서 계산된 y 스무싱 적용
		for (int i = 0; i < nTotalVertex; i++) {
			pTerrainInfo[i].position.y = smooth[i];
		}
	}

	SAFE_DELETE_ARRAY(smooth);
}

void Terrain::LoadTerrainCells()
{
	int index;
	
	nCellCount = 32;
	nCellCount = 32;

	nCellRowCount = nWidth / nCellCount;
	TotalCellSize = nCellRowCount * nCellRowCount;
	pTerrainCells = new TerrainCell[TotalCellSize];

	for (int z = 0; z < nCellRowCount; ++z)
	{
		for (int x = 0; x < nCellRowCount; ++x)
		{
			index = nCellRowCount * z + x;
			pTerrainCells[index].Init(pTerrainInfo, x, z, nCellCount, nCellCount, nWidth);
		}
	}
}

void Terrain::CreateShadowRender(Camera * pLightCamera, bool use)
{
	D3DXMATRIXA16 matWorld;

	matWorld = this->pTransform->GetFinalMatrix();

	pEffect->SetMatrix("matWorld", &matWorld);

	//뷰행렬세팅
	pEffect->SetMatrix("matViewProjection", &pLightCamera->GetViewProjectionMatrix());

	pEffect->SetTechnique("CreateShadow");

	pEffect->Begin();
	pEffect->BeginPass(0);

	for (int i = 0; i < nCellRowCount; ++i)
	{
		for (int j = 0; j < nCellRowCount; ++j)
		{
			int index = i * nCellRowCount + j;

			pTerrainCells[index].Render();
		}
	}
	pEffect->EndPass();
	pEffect->End();

	if (pVegetation) pVegetation->CreateShadowRender(pLightCamera);
}

void Terrain::Render(DirectionLight * light, Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor, D3DXPLANE* reflectedW)
{
	if (reflectedW)
		D3DXMatrixReflect(&RefMtx, reflectedW);

	D3DXMATRIXA16 matWorld;
	matWorld = this->pTransform->GetFinalMatrix() * RefMtx;

	if (pEffect != NULL)
	{
		pEffect->SetTechnique("Base");

		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());

		pEffect->SetTexture("Terrain0_Tex", tTileTex00);
		pEffect->SetTexture("Terrain1_Tex", tTileTex01);
		pEffect->SetTexture("Terrain2_Tex", tTileTex02);
		pEffect->SetTexture("Terrain3_Tex", tTileTex03);
		pEffect->SetTexture("TerrainControl_Tex", tSplatTex);

		pEffect->SetVector4("vFog", fog);
		pEffect->SetVector4("vFogColor", D3DXVECTOR4(fogColor, 1));
		if (reflectedW == NULL)
			pEffect->SetVector4("worldLightDir", D3DXVECTOR4(light->GetLightDirection(), 1));
		else
			pEffect->SetVector4("worldLightDir", D3DXVECTOR4(light->GetLightDirection().x, -light->GetLightDirection().y, light->GetLightDirection().z, 1));

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			for (int i = 0; i < nCellRowCount; ++i)
			{
				for (int j = 0; j < nCellRowCount; ++j)
				{
					int index = i * nCellRowCount + j;

					pTerrainCells[index].Render();
					TotalCellSize++;
				}
			}
		}
		pEffect->EndPass();
		pEffect->End();
	}

	if (pVegetation) pVegetation->Render(pCamera, pLightCamera, light, fog, fogColor, reflectedW);
}

void Terrain::ReciveShadowRender(DirectionLight * light, Camera * pCamera, Camera * pLightCamera, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor)
{
	if (pEffect != NULL)
	{
		pEffect->SetTechnique("ReciveShadow");

		pEffect->SetMatrix("matWorld", &this->pTransform->GetFinalMatrix());
		pEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
		pEffect->SetMatrix("matLightViewProjection", &pLightCamera->GetViewProjectionMatrix());

		pEffect->SetTexture("Shadow_Tex", pLightCamera->GetRenderTexture());
		pEffect->SetTexture("Terrain0_Tex", tTileTex00);
		pEffect->SetTexture("Terrain1_Tex", tTileTex01);
		pEffect->SetTexture("Terrain2_Tex", tTileTex02);
		pEffect->SetTexture("Terrain3_Tex", tTileTex03);
		pEffect->SetTexture("TerrainControl_Tex", tSplatTex);

		pEffect->SetVector4("vFog", fog);
		pEffect->SetVector4("vFogColor", D3DXVECTOR4(fogColor, 1));
		pEffect->SetVector4("worldLightDir", D3DXVECTOR4(light->GetLightDirection(), 1));

		TotalCellSize = 0;

		pEffect->Begin();
		pEffect->BeginPass(0);
		{
			for (int i = 0; i < nCellRowCount; ++i)
			{
				for (int j = 0; j < nCellRowCount; ++j)
				{
					int index = i * nCellRowCount + j;

					D3DXVECTOR3 minimum = D3DXVECTOR3(pTerrainCells[index].GetCellDimensions().minimumWidth, pTerrainCells[index].GetCellDimensions().minimumHeight, pTerrainCells[index].GetCellDimensions().minimumDepth);
					D3DXVECTOR3 maximum = D3DXVECTOR3(pTerrainCells[index].GetCellDimensions().maximumWidth, pTerrainCells[index].GetCellDimensions().maximumHeight, pTerrainCells[index].GetCellDimensions().maximumDepth);
					if (!pCamera->GetFrustum().IsInRectangle(&minimum, &maximum)) continue;

					pTerrainCells[index].Render();
					if (CellLineVisible) pTerrainCells[index].LineRender(&this->pTransform->GetFinalMatrix(), pCamera);
					TotalCellSize++;
				}
			}
		}
		pEffect->EndPass();
		pEffect->End();
	}

	if (pVegetation) pVegetation->Render(pCamera, pLightCamera,light, fog, fogColor);
}
