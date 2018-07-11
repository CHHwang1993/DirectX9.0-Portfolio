#include "stdafx.h"
#include "SceneBase.h"


SceneBase::SceneBase()
	: pCamera(NULL), pPostEffect(NULL), pDirLight(NULL), pDirLightCamera(NULL), isWireFrame(false),
	nPassNumber(0), fBloomIntensity(0.2f)
{
	pCamera = new Camera(); //¸ŞÀÎÄ«¸Ş¶ó »ı¼º
	pCamera->ReadyRenderToTexture(4096, 4096); //·»´õÅ¸°Ù ¸Ê ¼¼ÆÃ

	pPostEffect = NULL; //³ªÁß¿¡ ¼¼ÆÃ!

	pDirLight = new DirectionLight(); //¹æÇâ¼º ±¤¿ø ¼¼ÆÃ
	pDirLight->Color = D3DXCOLOR(1, 1, 1, 1);
	pDirLight->Intensity = 1.0f;

	pDirLightCamera = new Camera; //¹æÇâ¼º ±¤¿øÀÇ ¹æÇâÀ» °¡Áø ºû Ä«¸Ş¶ó

	this->pDirLightCamera->bOrtho = true; //Á÷±³Åõ¿µÀ¸·Î ¹Ù²Û´Ù.
	this->pDirLightCamera->camNear = 0.1f; //Ä«¸Ş¶ó °¡±î¿î°Å¸®
	this->pDirLightCamera->camFar = 300.0f; //Ä«¸Ş¶ó ¸Õ°Å¸®
	this->pDirLightCamera->aspect = 1.0f; //Á¾È¾ºñ
	this->pDirLightCamera->orthoSize = 600.0f; //Åõ¿µÅ©±â´Â ±×¸²ÀÚ Å©±â·Î

	this->pDirLightCamera->ReadyShadowToTexture(8192); //¹æÇâ¼º Ä«¸Ş¶óÀÇ ±×¸²ÀÚ ·»´õÅ¸°Ù ÅØ½ºÃÄ ÁØºñ
}


SceneBase::~SceneBase()
{
}

HRESULT SceneBase::Init()
{
	if (FAILED(this->Scene_Init()))
		return E_FAIL;

	this->pCamera->ReadyRenderToTexture(WINSIZE_X, WINSIZE_Y); //¸ŞÀÎÄ«¸Ş¶óÀÇ ·»´õÅ¸°Ù ÅØ½ºÃÄ ÁØºñ

	//0----1
	//|   /|
	//|  / |
	//| /  |
	//|/   |
	//3----2

	//Åõ¿µÁÂÇ¥ Æ÷Áö¼Ç
	this->stSceneVertex[0].Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	this->stSceneVertex[1].Position = D3DXVECTOR3( 1.0f, 1.0f, 0.0f);
	this->stSceneVertex[2].Position = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	this->stSceneVertex[3].Position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);

	//UV ÁÂÇ¥
	this->stSceneVertex[0].UV = D3DXVECTOR2(0.0f,0.0f);
	this->stSceneVertex[1].UV = D3DXVECTOR2(1.0f,0.0f);
	this->stSceneVertex[2].UV = D3DXVECTOR2(1.0f,1.0f);
	this->stSceneVertex[3].UV = D3DXVECTOR2(0.0f,1.0f);

	//ÀÎµ¦½º
	this->dwSceneIndex[0] = 0;
	this->dwSceneIndex[1] = 1;
	this->dwSceneIndex[2] = 3;
	this->dwSceneIndex[3] = 3;
	this->dwSceneIndex[4] = 1;
	this->dwSceneIndex[5] = 2;

	this->pPostEffect = SHADER->GetResource("fx/PostEffect.fx");

	this->pDirLight->RotateX(D3DX_PI / 2);
	this->pDirLight->RotateX(-2.0f*TIMEMANAGER->getElapsedTime());

	//TweakBar ÁØºñ
	TwBar* bar = TweakBar::Get()->GetBar();
	TwAddVarRW(bar, "PostEffect Pass", TW_TYPE_INT32, &nPassNumber, "min=0 max=5 step=1 group='PostEffect'");
	TwAddVarRW(bar, "Bloom Intensity", TW_TYPE_FLOAT, &fBloomIntensity, "min=0 max=1 step=0.1 group='PostEffect'");
}

void SceneBase::Release()
{
	this->Scene_Release(); //¾À ÇØÁ¦

	//°øÅëµÈ ¾À¿¡¼­ ¾²¿´´ø °´Ã¼µé ÇØÁ¦
	SAFE_DELETE(pCamera);
	SAFE_DELETE(pDirLight);
	SAFE_DELETE(pDirLightCamera);
}

void SceneBase::Update()
{
	if (Keyboard::Get()->Down('1')) isWireFrame = !isWireFrame; //¿ÍÀÌ¾îÇÁ·¹ÀÓ ¼¼ÆÃ

	pCamera->CameraControl(); //Ä«¸Ş¶ó ¿òÁ÷ÀÓ

	this->Scene_Update(); //¾ÀÀÇ ¾÷µ¥ÀÌÆ®
}

void SceneBase::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, isWireFrame ? D3DFILL_WIREFRAME : D3DFILL_SOLID); //¿ÍÀÌ¾îÇÁ·¹ÀÓ ±×¸±²¨³Ä ¾È±×¸±²¨³Ä?
	
	this->pCamera->RenderTextureBegin(0x00101010);

	pCamera->UpdateFrustum();

	this->Scene_Render(); //¾ÀÀÇ ·»´õ

	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID); //¿ÍÀÌ¾îÇÁ·¹ÀÓ ±×¸±²¨³Ä ¾È±×¸±²¨³Ä?
	
	this->pCamera->RenderTextureEnd();
	
	//Æ÷½ºÆ®ÀÌÆåÆ® ·»´õ
	
	if (this->pPostEffect)
	{
		this->pPostEffect->SetTechnique("PostEffect");
	
		this->pPostEffect->SetTexture("screenTex", this->pCamera->GetRenderTexture()); //¸ŞÀÎÄ«¸Ş¶óÀÇ ·»´õÅ¸°Ù
		float pixelU = 1.0f / WINSIZE_X;
		float pixelV = 1.0f / WINSIZE_Y;
	
		//ÇÈ¼¿ UV ÁÂÇ¥
		this->pPostEffect->SetFloat("pixelSizeU", pixelU);
		this->pPostEffect->SetFloat("pixelSizeV", pixelV);
		this->pPostEffect->SetFloat("pixelHalfSizeU", pixelU*0.5f);
		this->pPostEffect->SetFloat("pixelHalfSizeV", pixelV*0.5f);
		
		switch (nPassNumber)
		{
		case 1 : //¾Æ¿ô¶óÀÎ ½¦ÀÌ´õ
			this->pPostEffect->SetTexture("normalTex", this->pCamera->GetRenderTexture());
			break;
			 
		case 2: case 3: //ºí·¯ X, ºí·¯ Y
			this->pPostEffect->SetFloat("blurScale", 3.0f);
			break;
	
		case 4: //BBO (¾êµµ ºí·ë)
			this->pPostEffect->SetTexture("blurTex", this->pCamera->GetRenderTexture());
			this->pPostEffect->SetFloat("blurAmount", fBloomIntensity);
	
		case 5: //ºí·ë
			this->pPostEffect->SetTexture("normalTex", this->pCamera->GetRenderTexture());
			this->pPostEffect->SetTexture("blurTex", this->pCamera->GetRenderTexture());
		}
	
		this->pPostEffect->Begin();
	
		this->pPostEffect->BeginPass(nPassNumber);
	
		g_pD3DDevice->SetFVF(SCENEVERTEX::FVF);
		g_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, this->dwSceneIndex, D3DFMT_INDEX32, this->stSceneVertex, sizeof(SCENEVERTEX));

		this->pPostEffect->EndPass();
		
	
		this->pPostEffect->End();
	}
}
