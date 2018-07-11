#include "stdafx.h"
#include "PondWater.h"


PondWater::PondWater(WaterInfo& initInfo)
	:pReflectSurface(NULL), pRefractSurface(NULL)
{
	stWaterInfo = initInfo;

	fWidth = (initInfo.vertX - 1)*initInfo.dX;
	fDepth = (initInfo.vertZ - 1)*initInfo.dZ;

	vWaveMapOffset0 = D3DXVECTOR2(0.0f, 0.0f);
	vWaveMapOffset1 = D3DXVECTOR2(0.0f, 0.0f);

	numTris = (initInfo.vertX - 1)*(initInfo.vertZ - 1) * 2;
	numVerts = initInfo.vertX*initInfo.vertZ;

	HRESULT hr;

	InitVIB();

	pWaveMap0 = TEXTURE2D->GetResource("Texture/wave0.dds");
	pWaveMap1 = TEXTURE2D->GetResource("Texture/wave1.dds");

	////굴절맵, 반사맵 생성
	
	hr = D3DXCreateTexture(g_pD3DDevice, WINSIZE_X, WINSIZE_Y, 0, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pReflectMap);
	assert(hr == S_OK);

	hr = D3DXCreateTexture(g_pD3DDevice, WINSIZE_X, WINSIZE_Y, 0, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pRefractMap);
	assert(hr == S_OK);

	BuildFX();
}


PondWater::~PondWater()
{
	SAFE_DELETE(pVB);
	SAFE_DELETE(pIB);
	SAFE_RELEASE(pDecl);

	SAFE_RELEASE(pReflectMap);
	SAFE_RELEASE(pRefractMap);
	SAFE_RELEASE(pReflectSurface);
	SAFE_RELEASE(pRefractSurface);
}

void PondWater::InitVIB()
{
	VertexPositionTexture* WaterInfo = new VertexPositionTexture[numVerts];

	for (int z = 0; z < stWaterInfo.vertX; ++z)
	{
		for (int x = 0; x < stWaterInfo.vertZ; ++x)
		{
			//정점 포지션 계산

			D3DXVECTOR3 pos;

			pos.x = (x * stWaterInfo.dX - (stWaterInfo.vertX/2 * stWaterInfo.dX));
			pos.z = (-z * stWaterInfo.dX + (stWaterInfo.vertZ/2 * stWaterInfo.dX));
			pos.y = 0.0f;

			D3DXVECTOR2 uv;

			uv.x = x / static_cast<float>(stWaterInfo.vertX) * stWaterInfo.texScale;
			uv.y = z / static_cast<float>(stWaterInfo.vertZ) * stWaterInfo.texScale;

			int idx = z * stWaterInfo.vertX + x;

			WaterInfo[idx].Position = pos;
			WaterInfo[idx].Tex = uv;
		}
	}

	pVB = new VertexBuffer();
	pVB->CreateVertexBuffer(numVerts * sizeof(VertexPositionTexture));
	pVB->LockAndCopyVertexBuffer(WaterInfo);


	MYINDEX* waterIndex = new MYINDEX[numTris];

	//배열 인덱스
	int idx = 0;

	for (DWORD z = 0; z < stWaterInfo.vertX - 1; ++z)
	{
		for (DWORD x = 0; x < stWaterInfo.vertZ - 1; ++x)
		{
			// lt-----rt
			//  |    /|
			//  |   / |
			//  |  /  |
			//  | /   |
			//  |/    |
			// lb-----rb

			DWORD lt = z * stWaterInfo.vertX + x;
			DWORD rt = z * stWaterInfo.vertX + x + 1;
			DWORD lb = ((z + 1) * stWaterInfo.vertX) + x;
			DWORD rb = ((z + 1) * stWaterInfo.vertX) + x + 1;

			waterIndex[idx]._0 = lt;
			waterIndex[idx]._1 = rt;
			waterIndex[idx]._2 = lb;
			idx++;

			waterIndex[idx]._0 = lb;
			waterIndex[idx]._1 = rt;
			waterIndex[idx]._2 = rb;
			idx++;
		}
	}

	pIB = new IndexBuffer();
	pIB->CreateIndexBuffer((stWaterInfo.vertX - 1)*(stWaterInfo.vertZ - 1) * 2 * sizeof(MYINDEX), D3DFMT_INDEX32);
	pIB->LockAndCopyIndexBuffer(waterIndex);


	D3DVERTEXELEMENT9 vertElement[3] =
	{
		{ 0,0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
	};

	g_pD3DDevice->CreateVertexDeclaration(vertElement, &pDecl);

	SAFE_DELETE_ARRAY(WaterInfo);
	SAFE_DELETE_ARRAY(waterIndex);
}

void PondWater::BuildFX()
{
	pEffect = SHADER->GetResource("fx/PondWater.fx");

	// We don't need to set these every frame since they do not change.
	// 매프레임 마다 바꾸지 않는 쉐이더 변수들은 세팅을 미리 해둔다.
	if (pEffect)
	{
		pEffect->SetMatrix("gWorld", &stWaterInfo.matWorld);
		D3DXMATRIXA16 matInvWorld;
		D3DXMatrixInverse(&matInvWorld, NULL, &stWaterInfo.matWorld);
		pEffect->SetMatrix("gWorldInv", &matInvWorld);
		pEffect->SetTechnique("WaterTech");
		pEffect->SetTexture("gWaveMap0", pWaveMap0);
		pEffect->SetTexture("gWaveMap1", pWaveMap1);
		pEffect->SetValue("gMtrl", &stWaterInfo.mtrl, sizeof(Mtrl));
		pEffect->SetFloat("gRefractBias", stWaterInfo.refractBias);
		pEffect->SetFloat("gRefractPower", stWaterInfo.refractPower);
		pEffect->SetVector2("gRippleScale", stWaterInfo.rippleScale);
	}
}

void PondWater::Update(DirectionLight* pLight)
{
	// Update texture coordinate offsets.  These offsets are added to the
	// texture coordinates in the vertex shader to animate them.
	//텍스쳐 좌표 오프셋 업데이트한다. 
	//이 오프셋은 버텍스쉐이더안에서 움직이기 위해 텍스쳐 좌표를 더한다.
	vWaveMapOffset0 += stWaterInfo.waveMapVelocity0 * TIMEMANAGER->getElapsedTime();
	vWaveMapOffset1 += stWaterInfo.waveMapVelocity1 * TIMEMANAGER->getElapsedTime();

	// Textures repeat every 1.0 unit, so reset back down to zero
	// so the coordinates do not grow too large.
	//텍스쳐들은 1.0마다 반복하기 때문에 0으로 리셋 시킨다~
	//그래서 좌표들은 더 커지지 않는다.
	if (vWaveMapOffset0.x >= 1.0f || vWaveMapOffset0.x <= -1.0f)
		vWaveMapOffset0.x = 0.0f;
	if (vWaveMapOffset1.x >= 1.0f || vWaveMapOffset1.x <= -1.0f)
		vWaveMapOffset1.x = 0.0f;
	if (vWaveMapOffset0.y >= 1.0f || vWaveMapOffset0.y <= -1.0f)
		vWaveMapOffset0.y = 0.0f;
	if (vWaveMapOffset1.y >= 1.0f || vWaveMapOffset1.y <= -1.0f)
		vWaveMapOffset1.y = 0.0f;

	
	stWaterInfo.dirLight.dirW = pLight->GetLightDirection();
}

void PondWater::Draw(Camera* pCamera, D3DXVECTOR4 fog,D3DXVECTOR3 fogColor)
{
	if (pEffect)
	{
		D3DXMATRIXA16 matWorldViewProjection;
		matWorldViewProjection = stWaterInfo.matWorld*pCamera->GetViewProjectionMatrix();
		pEffect->SetMatrix("gWVP", &matWorldViewProjection);
		pEffect->SetVector3("gEyePosW", pCamera->GetWorldPosition());
		pEffect->SetVector2("gWaveMapOffset0", vWaveMapOffset0);
		pEffect->SetVector2("gWaveMapOffset1", vWaveMapOffset1);
		pEffect->SetTexture("gReflectMap", pReflectMap);
		pEffect->SetTexture("gRefractMap", pRefractMap);
		pEffect->SetVector4("vFogColor", D3DXVECTOR4(fogColor, 1));
		pEffect->SetVector4("vFog", fog);
		pEffect->SetValue("gLight", &stWaterInfo.dirLight, sizeof(DirLight));

		pEffect->Begin();

		pEffect->BeginPass(0);

		g_pD3DDevice->SetStreamSource(0, pVB->Buffer(), 0, sizeof(VertexPositionTexture));
		g_pD3DDevice->SetVertexDeclaration(pDecl);
		g_pD3DDevice->SetIndices(pIB->Buffer());
		g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris);
		
		pEffect->EndPass();

		pEffect->End();
	}
}

void PondWater::ReflectMapBegin()
{
	HRESULT hr;

	hr = g_pD3DDevice->GetRenderTarget(0, &pReflectSurface);
	assert(hr == S_OK);

	LPDIRECT3DSURFACE9 textureSurface = NULL;

	hr = pReflectMap->GetSurfaceLevel(0, &textureSurface);
	assert(hr == S_OK);

	//텍스쳐 표면을 Device의 타겟버퍼로 세팅한다.
	hr = g_pD3DDevice->SetRenderTarget(0, textureSurface);
	assert(hr == S_OK);
	//쓰고 해제
	SAFE_RELEASE(textureSurface);

	g_pD3DDevice->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
}

void PondWater::ReflectMapEnd()
{
	HRESULT hr;

	hr = g_pD3DDevice->SetRenderTarget(0, pReflectSurface);
	assert(hr == S_OK);
	SAFE_RELEASE(pReflectSurface);
}

void PondWater::RefractMapBegin()
{
	HRESULT hr;

	hr = g_pD3DDevice->GetRenderTarget(0, &pRefractSurface);
	assert(hr == S_OK);

	LPDIRECT3DSURFACE9 textureSurface = NULL;

	hr = pRefractMap->GetSurfaceLevel(0, &textureSurface);
	assert(hr == S_OK);

	//텍스쳐 표면을 Device의 타겟버퍼로 세팅한다.
	hr = g_pD3DDevice->SetRenderTarget(0, textureSurface);
	assert(hr == S_OK);
	//쓰고 해제
	SAFE_RELEASE(textureSurface);

	g_pD3DDevice->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
}

void PondWater::RefractMapEnd()
{
	HRESULT hr;

	hr = g_pD3DDevice->SetRenderTarget(0, pRefractSurface);
	assert(hr == S_OK);
	SAFE_RELEASE(pRefractSurface);
}
