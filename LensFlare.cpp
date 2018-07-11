#include "stdafx.h"
#include "LensFlare.h"

struct FLARE_VETEX
{
	D3DXVECTOR2 p;
	D3DXVECTOR2 t;
};

static D3DVERTEXELEMENT9 VertexElement[] =
{
	// local data (stream 0)
	{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};



LensFlare::LensFlare()
	: pVB(NULL)
	, pIB(NULL)
	, pEffect(NULL)
	, pVertexDeclaration(NULL)
	, LensTexture(NULL)
	, IsVisible(false)
	, vFlareColor(0.1f, 0.1f, 0.1f, 1.0f)
{
}


LensFlare::~LensFlare()
{
	SAFE_DELETE(pVB);
	SAFE_DELETE(pIB);
	SAFE_RELEASE(LensTexture);
	SAFE_DELETE(pEffect);
	SAFE_RELEASE(pVertexDeclaration);
}

void LensFlare::LoadAssets()
{
	VertexSetting();

	pEffect = SHADER->Load("fx/LensFlare.fx");
	LensTexture = TEXTURE2D->Load("Texture/lens_flare.dds");
}

void LensFlare::VertexSetting()
{
	FLARE_VETEX quadVertex[4];

	quadVertex[0].p = D3DXVECTOR2(-1.0f, -1.0f);
	quadVertex[0].t = D3DXVECTOR2(0.0f,0.0f);
	quadVertex[1].p = D3DXVECTOR2(-1.0f, 1.0f);
	quadVertex[1].t = D3DXVECTOR2(0.0f, 0.33f);
	quadVertex[2].p = D3DXVECTOR2(1.0f, -1.0f);
	quadVertex[2].t = D3DXVECTOR2(0.33f, 0.0f);
	quadVertex[3].p = D3DXVECTOR2(1.0f, 1.0f);
	quadVertex[3].t = D3DXVECTOR2(0.33f, 0.33f);

	HRESULT hr;

	pVB = new VertexBuffer();
	pIB = new IndexBuffer();
	
	pVB->CreateVertexBuffer(4 * sizeof(FLARE_VETEX));
	pVB->LockAndCopyVertexBuffer(quadVertex);

	hr = g_pD3DDevice->CreateVertexDeclaration(&VertexElement[0], &pVertexDeclaration);
	assert(hr == S_OK);

	UINT16 index[4] = { 0,1,2,3 };

	pIB->CreateIndexBuffer(4 * sizeof(UINT16));
	pIB->LockAndCopyIndexBuffer(index);

    stFlareElements[0].linePos = 1.0f;
	stFlareElements[0].scale = 0.25f;
	stFlareElements[0].brightness = 1.0f;
	stFlareElements[0].texU = 0.0f;
	stFlareElements[0].texV = 0.0f;

	float flareStep = 2.0f / TOTALFLARE;
	float flarePos = 1.0f;

	int xPos = 0;
	int yPos = 0;

	for (int i = 1; i < TOTALFLARE; ++i)
	{
		flarePos -= flareStep;
		float posJiter = Random::Get()->GetFromFloatTo(-0.2f, 0.2f);

		stFlareElements[i].linePos = flarePos+ posJiter;

		ClampMinusOnePlusOne(stFlareElements[i].linePos);

		stFlareElements[i].scale = Random::Get()->GetFromFloatTo(0.3f,2.5f);

		stFlareElements[i].scale *= abs(stFlareElements[i].linePos);

		stFlareElements[i].brightness = Random::Get()->GetFromFloatTo(0.5f, 1.0f);

		++xPos;
		if (xPos > 2)
		{
			xPos = 0;
			++yPos;
		}
		if (yPos > 2)
		{
			yPos=0;
		}

		stFlareElements[i].texU = 0.33f*xPos;
		stFlareElements[i].texV = 0.33f*yPos;
	}

	stFlareElements[TOTALFLARE - 1].linePos = -1.0f;
}

void LensFlare::Update(D3DXVECTOR3* sunPos, Camera* pCamera)
{
	vCamPos = pCamera->GetWorldPosition();

	vSunPos = (*sunPos);

	D3DXVECTOR3 vSunDir;
	D3DXVec3Normalize(&vSunDir,&(vSunPos - vCamPos));

	D3DXMATRIXA16 ViewProj = pCamera->GetViewProjectionMatrix();

	D3DXVec3TransformCoord(&vScreenPos, &vSunPos, &ViewProj);

	//태양과 카메라의 내적을 함
	float CDotL = D3DXVec3Dot(&pCamera->GetForward(), &vSunDir);

	// is the sun in view?
	if (vScreenPos.x > -1.2f
		&& vScreenPos.x < 1.2f
		&& vScreenPos.y > -1.2f
		&& vScreenPos.y < 1.2f
		&& vScreenPos.z > 0.0f
		&& CDotL>0)
	{
		IsVisible = true;
	}
	else
	{
		IsVisible = false;
	}

	if (IsVisible)
	{
		vFlareColor = D3DXVECTOR4(1.0f, 0.8f, 0.8f, 1.0f);

		vFlareColor.w = vSunPos.y * 2.0f;

		vFlareColor.w -= 0.15f;
		vFlareColor.w = Clamp01(vFlareColor.w);

		float screen_scale = 1.0f - D3DXVec2Length(&D3DXVECTOR2(vScreenPos.x, vScreenPos.y));
		screen_scale = Clamp01(screen_scale);
		vFlareColor.w *= screen_scale;
	}
}

void LensFlare::Render()
{
	HRESULT hr;

	if (IsVisible)
	{
		hr = g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, false);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
		assert(hr == S_OK);

		pEffect->SetVector4("FlareColor", vFlareColor);
		pEffect->SetTexture("FlareMap_Tex", LensTexture);

		hr= g_pD3DDevice->SetIndices(pIB->Buffer());
		assert(hr == S_OK);
		hr= g_pD3DDevice->SetStreamSource(0, pVB->Buffer(), 0, sizeof(FLARE_VETEX));
		assert(hr == S_OK);
		hr= g_pD3DDevice->SetVertexDeclaration(pVertexDeclaration);
		assert(hr == S_OK);

		UINT numPass = 0;

		pEffect->Begin();


		pEffect->BeginPass(0);
		{
			for (int j = 0; j < TOTALFLARE; ++j)
			{
				D3DXVECTOR4 posScaleOffset(
					stFlareElements[j].scale,
					stFlareElements[j].scale,
					vScreenPos.x * stFlareElements[j].linePos,
					vScreenPos.y * stFlareElements[j].linePos);

				D3DXVECTOR4 texScaleOffset(
					1.0f,
					1.0f,
					stFlareElements[j].texU,
					stFlareElements[j].texV);

				pEffect->SetVector4("posOffset", posScaleOffset);

				pEffect->SetVector4("texOffset", texScaleOffset);

				pEffect->CommitChange();

				// render!!!
				hr = g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
				assert(hr == S_OK);
			}
		}
		hr = g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, true);
		assert(hr == S_OK);
		hr = g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
		assert(hr == S_OK);

		pEffect->EndPass();
		pEffect->End();
	}
	
}
