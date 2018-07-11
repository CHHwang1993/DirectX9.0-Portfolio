#include "stdafx.h"
#include "Tree.h"


Tree::Tree()
	:pTree(NULL)
{
}


Tree::~Tree()
{
	SAFE_RELEASE(pTree);
	SAFE_DELETE(pTransform);

	vecDiffuseTex.clear();
	vecNormalTex.clear();
	vecSpecularTex.clear();
}

void Tree::LoadAsset()
{
	LPD3DXBUFFER pAdjacency = NULL;
	LPD3DXBUFFER pMaterial = NULL;

	std::string filePath = "Model/PineTree_01.x";

	HRESULT hr;
	hr = D3DXLoadMeshFromX(filePath.c_str(), D3DXMESH_MANAGED, g_pD3DDevice, &pAdjacency, &pMaterial, NULL, &this->dwMaterialsNum, &this->pTree);
	assert(hr == S_OK);


	std::string texFilePath;//최종경로
	std::string texExp;//파일 확장명
	std::string texFile;//파일명
	std::string path = "Texture/"; //경로

	LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pMaterial->GetBufferPointer();

	for (DWORD i = 0; i < this->dwMaterialsNum; ++i)
	{
		D3DMATERIAL9 mtrl = pMaterials[i].MatD3D;

		mtrl.Ambient = mtrl.Diffuse;

		this->vecMaterials.push_back(mtrl);

		if (pMaterials[i].pTextureFilename != NULL)
		{
			texFilePath = path + pMaterials[i].pTextureFilename;
			
			//디퓨즈 텍스처
			vecDiffuseTex.push_back(TEXTURE2D->GetResource(texFilePath));

			//파일 이름과 확장자 위치
			int dotIndex = texFilePath.find_last_of(".");

			//파일 명과 확장자를 나눈다.
			texFile = texFilePath.substr(0, dotIndex);
			texExp = texFilePath.substr(dotIndex + 1, filePath.length());

			//노말텍스처
			texFilePath = texFile + "_N." + texExp;

			vecNormalTex.push_back(TEXTURE2D->GetResource(texFilePath));

			//스페큘러텍스처
			texFilePath = texFile + "_S." + texExp;

			vecSpecularTex.push_back(TEXTURE2D->GetResource(texFilePath));
		}
	}

	SAFE_RELEASE(pMaterial);

	this->pTree->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)pAdjacency->GetBufferPointer(), NULL, NULL, NULL);

	SAFE_RELEASE(pAdjacency);

	pTransform = new Transform();
	pTransform->SetScale(0.01f, 0.01f, 0.01f);

	pEffect = SHADER->GetResource("fx/StaticMesh.fx");
}

void Tree::CreateShadowRender(Camera* pLightCamera)
{
	D3DXMATRIXA16 matWorld;

	matWorld = this->pTransform->GetFinalMatrix();

	if (pEffect != NULL)
	{
		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matViewProjection", &pLightCamera->GetViewProjectionMatrix());

		pEffect->Begin();
		pEffect->BeginPass(1);

		for (int m = 0; m < this->dwMaterialsNum; m++)
		{
			pEffect->CommitChange();

			this->pTree->DrawSubset(m);
		}
		pEffect->EndPass();
		pEffect->End();
	}
}

void Tree::ReciveShadowRender(DirectionLight* light, Camera* pCamera, Camera* pLightCamera, D3DXVECTOR4 fog, D3DXVECTOR3 fogColor, D3DXPLANE* reflectedW)
{
	D3DXMATRIXA16 RefMtx,matWorld;
	D3DXMatrixIdentity(&RefMtx);
	D3DXMatrixIdentity(&matWorld);
	if (reflectedW)
		D3DXMatrixReflect(&RefMtx, reflectedW);

	matWorld = this->pTransform->GetFinalMatrix() * RefMtx;

	if (pEffect != NULL)
	{
		pEffect->SetTexture("Shadow_Tex", pLightCamera->GetRenderTexture());
		pEffect->SetMatrix("matLightViewProjection", &pLightCamera->GetViewProjectionMatrix());

		pEffect->SetMatrix("matWorld", &matWorld);
		pEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
		pEffect->SetVector3("vLightColor", D3DXVECTOR3(1, 1, 1));
		if (reflectedW == NULL)
			pEffect->SetVector3("vLightDir", light->GetLightDirection());
		else
			pEffect->SetVector3("vLightDir", D3DXVECTOR3(light->GetLightDirection().x, -light->GetLightDirection().y, light->GetLightDirection().z));

		pEffect->SetVector3("vCameraPos", pCamera->GetWorldPosition());
		pEffect->SetVector4("vFog", fog);
		pEffect->SetVector4("vFogColor", D3DXVECTOR4(fogColor, 1));
		pEffect->SetFloat("camFar", pCamera->camFar);
		pEffect->SetFloat("camNear", pCamera->camNear);

		pEffect->Begin();
		if(reflectedW==NULL) pEffect->BeginPass(2);
		else pEffect->BeginPass(0);

		for (int m = 0; m < this->dwMaterialsNum; m++)
		{
			pEffect->SetTexture("Diffuse_Tex", this->vecDiffuseTex[m]);
			pEffect->SetTexture("Normal_Tex", this->vecNormalTex[m]);
			pEffect->SetTexture("Specular_Tex", this->vecSpecularTex[m]);
			//스펙파워
			pEffect->SetFloat("fSpecPower", this->vecMaterials[m].Power);

			pEffect->CommitChange();

			this->pTree->DrawSubset(m);
		}
		pEffect->EndPass();
		pEffect->End();
	}
}
