#include "stdafx.h"
#include "Vegetation.h"
#include "Tree.h"

Vegetation::Vegetation()
	:pEffect(NULL), pVB(NULL), pDecl(NULL), tGrassTexture(NULL)
{
	pEffect = SHADER->GetResource("fx/Billboard.fx");
	tGrassTexture = TEXTURE2D->GetResource("Texture/grassShrub.tga");
}


Vegetation::~Vegetation()
{
	SAFE_DELETE(pVB);
	SAFE_RELEASE(pDecl);

	for (int i = 0; i < vecTree.size(); ++i)
	{
		SAFE_DELETE(vecTree[i]);
	}
	vecTree.clear();
}

void Vegetation::VegetationSetting(TERRAININFO * terrainVertices, int totalVertex)
{
	float min = 15; //최소값
	float middle = 72; //중간값
	float max = 216; //최대값

	vector<D3DXVECTOR3> grassArray;


	int totalVertexLength = totalVertex;

	for (int i = 0; i < totalVertexLength; ++i)
	{
		TERRAININFO terrainVertex = terrainVertices[i];
		float height = terrainVertex.position.y;
		float baseProbability = 0;

		if (terrainVertex.position.x > 240 || terrainVertex.position.z < -240) continue;

		if (height > min && height < max)
		{
			float flatness = D3DXVec3Dot(&terrainVertex.normal, &D3DXVECTOR3(0, 1, 0));
			float minFlatness = (float)cos(D3DXToRadian(30));

			if (flatness > minFlatness)
			{
				if (height < middle)
				{
					baseProbability = (height - min) / (middle - min);
				}
			}
		}

		baseProbability *= 0.1f;
		float treeProbability = baseProbability * 0.04f;
		float roll = Random::Get()->GetFloat(1.0f);

		if (baseProbability > roll)
		{
			if (treeProbability > roll)
			{
				Tree* pTree = new Tree();
				pTree->LoadAsset();
				pTree->GetTransform()->SetWorldPosition(terrainVertex.position);
				vecTree.push_back(pTree);
			}
			else
			{
				grassArray.push_back(terrainVertex.position);
			}
		}
	}


	VertexGrass* billboardVertices = new VertexGrass[grassArray.size() * 6];

	int index = 0;

	for (int i = 0; i < grassArray.size(); ++i)
	{

		D3DXVECTOR2 scale = D3DXVECTOR2(3, 2);
		D3DXVECTOR3 normal = D3DXVECTOR3(0, 1, 0);

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(1, 1);
		billboardVertices[index].Scale = scale;
		index++;

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(1, 0);
		billboardVertices[index].Scale = scale;
		index++;

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(0, 0);
		billboardVertices[index].Scale = scale;
		index++;

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(1, 1);
		billboardVertices[index].Scale = scale;
		index++;

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(0, 0);
		billboardVertices[index].Scale = scale;
		index++;

		billboardVertices[index].Position = grassArray[i];
		billboardVertices[index].Normal = normal;
		billboardVertices[index].Tex = D3DXVECTOR2(0, 1);
		billboardVertices[index].Scale = scale;
		index++;
	}

	pVB = new VertexBuffer();
	pVB->CreateVertexBuffer(grassArray.size() * 6 * sizeof(VertexGrass));
	pVB->LockAndCopyVertexBuffer(billboardVertices);

	D3DVERTEXELEMENT9 vertElement[5] =
	{
		{ 0,0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0,24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0,32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	D3DDECL_END()
	};

	g_pD3DDevice->CreateVertexDeclaration(vertElement, &pDecl);

	grassArray.clear();
}

void Vegetation::Render(Camera * pCamera, Camera * pLightCamera, DirectionLight * pLight, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor, D3DXPLANE * reflectPlane)
{
	float Time = TIMEMANAGER->getWorldTime();

	if (pEffect != NULL)
	{
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);

		if (reflectPlane)
			D3DXMatrixReflect(&matWorld, reflectPlane);

		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matView", &pCamera->GetViewMatrix());
		pEffect->SetMatrix("matProjection", &pCamera->GetProjectionMatrix());
		pEffect->SetVector3("vCameraPos", pCamera->GetWorldPosition());
		pEffect->SetVector3("vAllowedRotDir", pCamera->GetUp());
		pEffect->SetVector3("vWorldLightDir", pLight->GetLightDirection());
		pEffect->SetVector4("vFog", fog);
		pEffect->SetVector3("vFogColor", fogColor);
		pEffect->SetFloat("fTime", Time);
		pEffect->SetTexture("BillboardTexture", tGrassTexture);

		pEffect->Begin();
		for (int i = 0; i<pEffect->NumPass(); ++i)
		{
			pEffect->BeginPass(i);
			g_pD3DDevice->SetStreamSource(0, pVB->Buffer(), 0, sizeof(VertexGrass));
			g_pD3DDevice->SetVertexDeclaration(pDecl);
			int noVertices = pVB->DataSize() / sizeof(VertexGrass);
			int noTriangles = noVertices / 3;
			g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, noTriangles);

			pEffect->EndPass();
		}

		pEffect->End();

		g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}

	for (int i = 0; i < vecTree.size(); ++i)
	{
		vecTree[i]->ReciveShadowRender(pLight, pCamera, pLightCamera, fog, fogColor, reflectPlane);
	}
}

void Vegetation::CreateShadowRender(Camera * pLightCamera)
{
	for (int i = 0; i < vecTree.size(); ++i)
	{
		vecTree[i]->CreateShadowRender(pLightCamera);
	}
}
