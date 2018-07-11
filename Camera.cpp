#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	:bOrtho(false)
	, rotate(0, 0)
	, RenderTexture(NULL), RenderSurface(NULL), DeviceTargetSurface(NULL), DeviceDepthAndStencilSurface(NULL)
{
	D3DXMatrixIdentity(&MatView);
	D3DXMatrixIdentity(&MatProjection);
	D3DXMatrixIdentity(&MatViewProjection);

	this->fov = 60.0f*ONE_RAD;
	this->aspect = static_cast<float>(WINSIZE_X) / static_cast<float>(WINSIZE_Y);
	this->camNear = 2.0f;
	this->camSpeed = 100;
	this->camRotSpeed = 0.5f;
	this->camFar = 1000.0f;
	this->orthoSize = 10.0f;

	this->SetWorldPosition(D3DXVECTOR3(0, 50, -5));
}


Camera::~Camera()
{
	SAFE_RELEASE(RenderTexture);
	SAFE_RELEASE(RenderSurface);
}

void Camera::UpdateMatrix()
{
	if (this->bOrtho == false)
	{
		D3DXMatrixPerspectiveFovLH(&MatProjection, this->fov, this->aspect, this->camNear, this->camFar);
	}
	else
	{
		D3DXMatrixOrthoLH(&MatProjection, this->aspect*this->orthoSize, orthoSize, this->camNear, this->camFar);
	}

	//뷰행렬 카메라 월드위치에 대한 역행렬이다.
	//D3DXMatrixInverse(&MatView, NULL, &this->MatWorld);

	D3DXMatrixLookAtLH(&MatView, &this->Position, &(this->Position + this->Forward), &this->Up);

	this->MatViewProjection = this->MatView* this->MatProjection;
}

void Camera::UpdateFrustum()
{
	frustum.UpdateFrustum(&this->MatViewProjection);
}

void Camera::UpdateCamToDevice(LPDIRECT3DDEVICE9 pDevice)
{
	//행렬 업데이트
	this->UpdateMatrix();

	//세팅
	
	pDevice->SetTransform(D3DTS_VIEW, &this->MatView);
	pDevice->SetTransform(D3DTS_PROJECTION, &this->MatProjection);
}

void Camera::Move(D3DXVECTOR3 Delta)
{
	this->MovePositionWorld(Delta*TIMEMANAGER->getElapsedTime());
}

void Camera::MoveForward()
{
	Move(this->Forward*camSpeed);
}

void Camera::MoveBackward()
{
	Move(-this->Forward*camSpeed);
}

void Camera::MoveRight()
{
	Move(this->Right*camSpeed);
}

void Camera::MoveLeft()
{
	Move(-this->Right*camSpeed);
}

void Camera::MoveDown()
{
	Move(-this->Up*camSpeed);
}

void Camera::MoveUp()
{
	Move(this->Up*camSpeed);
}

void Camera::CameraControl()
{
	if (this)
	{
		//카메라 이동
		if (Keyboard::Get()->Press('W')) this->MoveForward();
		if (Keyboard::Get()->Press('S')) this->MoveBackward();
		if (Keyboard::Get()->Press('A')) this->MoveLeft();
		if (Keyboard::Get()->Press('D')) this->MoveRight();
		if (Keyboard::Get()->Press('E')) this->MoveUp();
		if (Keyboard::Get()->Press('Q')) this->MoveDown();

		//카메라 회전
		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 Rot = Mouse::Get()->GetMoveValue();

			this->Rotation(D3DXVECTOR3(Rot.y, Rot.x, 0));
		}

		this->UpdateCamToDevice(g_pD3DDevice);
		this->UpdateFrustum();
	}
}

void Camera::Rotation(D3DXVECTOR3 Delta)
{
	this->rotate.x += Delta.x * TIMEMANAGER->getElapsedTime() * camRotSpeed;
	this->rotate.y += Delta.y * TIMEMANAGER->getElapsedTime() * camRotSpeed;

	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotate.x);
	D3DXMatrixRotationY(&y, rotate.y);

	rotation = x * y;

	this->SetRotateWorld(rotation);
}

void Camera::ReadyRenderToTexture(int width, int height)
{
	//값이 저장되어 있을 수도 있으니 미리 해제
	SAFE_RELEASE(RenderTexture);
	SAFE_RELEASE(RenderSurface);

	HRESULT hr;

	//렌더 타겟할 빈 텍스쳐를 만든다.
	hr = g_pD3DDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &RenderTexture, NULL);
	assert(hr == S_OK);

	//렌더할 서페이스
	hr =  g_pD3DDevice->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &RenderSurface, NULL);
	assert(hr == S_OK);
}

void Camera::ReadyShadowToTexture(int size)
{
	//값이 저장되어 있을 수도 있으니 미리 해제
	SAFE_RELEASE(RenderTexture);
	SAFE_RELEASE(RenderSurface);

	HRESULT hr;

	//렌더 타겟할 빈 텍스쳐를 만든다. (모든 컬러버퍼를 Red로 다 사용하기 위해 D3DFMT_R32F를 사용한다)
	hr = g_pD3DDevice->CreateTexture(size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &RenderTexture, NULL);
	assert(hr == S_OK);
	//렌더할 서페이스
	hr = g_pD3DDevice->CreateDepthStencilSurface(size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &RenderSurface, NULL);
	assert(hr == S_OK);
}

void Camera::RenderTextureBegin(DWORD backColor)
{
	HRESULT hr;

	//현 디바이스의 타겟 버퍼의 표면과 깊이 스텐실 버퍼의 표면정보를 기억한다.(Get!!!!)
	hr = g_pD3DDevice->GetRenderTarget(0, &DeviceTargetSurface);
	assert(hr == S_OK);
	hr = g_pD3DDevice->GetDepthStencilSurface(&DeviceDepthAndStencilSurface);
	assert(hr == S_OK);

	//렌더텍스쳐의 서페이스를 얻는다.
	LPDIRECT3DSURFACE9 textureSurface = NULL;
	if (SUCCEEDED(this->RenderTexture->GetSurfaceLevel(0, &textureSurface)))
	{
		//텍스쳐 표면을 Device의 타겟버퍼로 세팅한다.
		hr = g_pD3DDevice->SetRenderTarget(0, textureSurface);
		assert(hr == S_OK);
		//쓰고 해제
		SAFE_RELEASE(textureSurface);
	}

	//깊이 버퍼와 스텐실 버퍼의 서페이스를 렌더서페이스로 세팅
	hr = g_pD3DDevice->SetDepthStencilSurface(RenderSurface);
	assert(hr == S_OK);

	//디바이스버퍼 클리어
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
}

void Camera::RenderTextureEnd()
{
	HRESULT hr;

	//기존에 저장해둔 렌더서페이스 세팅
	hr = g_pD3DDevice->SetRenderTarget(0, DeviceTargetSurface);
	assert(hr == S_OK);
	//기존에 저장해둔 깊이스텐실서페이스 세팅
	hr = g_pD3DDevice->SetDepthStencilSurface(DeviceDepthAndStencilSurface);
	assert(hr == S_OK);
	//저장해뒀던것들은 해제
	SAFE_RELEASE(DeviceTargetSurface);
	SAFE_RELEASE(DeviceDepthAndStencilSurface);
}


LPDIRECT3DTEXTURE9 Camera::GetRenderTexture()
{
	//카메라의 렌더타겟 텍스쳐를 가져온다.
	return RenderTexture;
}



