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

	//����� ī�޶� ������ġ�� ���� ������̴�.
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
	//��� ������Ʈ
	this->UpdateMatrix();

	//����
	
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
		//ī�޶� �̵�
		if (Keyboard::Get()->Press('W')) this->MoveForward();
		if (Keyboard::Get()->Press('S')) this->MoveBackward();
		if (Keyboard::Get()->Press('A')) this->MoveLeft();
		if (Keyboard::Get()->Press('D')) this->MoveRight();
		if (Keyboard::Get()->Press('E')) this->MoveUp();
		if (Keyboard::Get()->Press('Q')) this->MoveDown();

		//ī�޶� ȸ��
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
	//���� ����Ǿ� ���� ���� ������ �̸� ����
	SAFE_RELEASE(RenderTexture);
	SAFE_RELEASE(RenderSurface);

	HRESULT hr;

	//���� Ÿ���� �� �ؽ��ĸ� �����.
	hr = g_pD3DDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &RenderTexture, NULL);
	assert(hr == S_OK);

	//������ �����̽�
	hr =  g_pD3DDevice->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &RenderSurface, NULL);
	assert(hr == S_OK);
}

void Camera::ReadyShadowToTexture(int size)
{
	//���� ����Ǿ� ���� ���� ������ �̸� ����
	SAFE_RELEASE(RenderTexture);
	SAFE_RELEASE(RenderSurface);

	HRESULT hr;

	//���� Ÿ���� �� �ؽ��ĸ� �����. (��� �÷����۸� Red�� �� ����ϱ� ���� D3DFMT_R32F�� ����Ѵ�)
	hr = g_pD3DDevice->CreateTexture(size, size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &RenderTexture, NULL);
	assert(hr == S_OK);
	//������ �����̽�
	hr = g_pD3DDevice->CreateDepthStencilSurface(size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &RenderSurface, NULL);
	assert(hr == S_OK);
}

void Camera::RenderTextureBegin(DWORD backColor)
{
	HRESULT hr;

	//�� ����̽��� Ÿ�� ������ ǥ��� ���� ���ٽ� ������ ǥ�������� ����Ѵ�.(Get!!!!)
	hr = g_pD3DDevice->GetRenderTarget(0, &DeviceTargetSurface);
	assert(hr == S_OK);
	hr = g_pD3DDevice->GetDepthStencilSurface(&DeviceDepthAndStencilSurface);
	assert(hr == S_OK);

	//�����ؽ����� �����̽��� ��´�.
	LPDIRECT3DSURFACE9 textureSurface = NULL;
	if (SUCCEEDED(this->RenderTexture->GetSurfaceLevel(0, &textureSurface)))
	{
		//�ؽ��� ǥ���� Device�� Ÿ�ٹ��۷� �����Ѵ�.
		hr = g_pD3DDevice->SetRenderTarget(0, textureSurface);
		assert(hr == S_OK);
		//���� ����
		SAFE_RELEASE(textureSurface);
	}

	//���� ���ۿ� ���ٽ� ������ �����̽��� ���������̽��� ����
	hr = g_pD3DDevice->SetDepthStencilSurface(RenderSurface);
	assert(hr == S_OK);

	//����̽����� Ŭ����
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
}

void Camera::RenderTextureEnd()
{
	HRESULT hr;

	//������ �����ص� ���������̽� ����
	hr = g_pD3DDevice->SetRenderTarget(0, DeviceTargetSurface);
	assert(hr == S_OK);
	//������ �����ص� ���̽��ٽǼ����̽� ����
	hr = g_pD3DDevice->SetDepthStencilSurface(DeviceDepthAndStencilSurface);
	assert(hr == S_OK);
	//�����ص״��͵��� ����
	SAFE_RELEASE(DeviceTargetSurface);
	SAFE_RELEASE(DeviceDepthAndStencilSurface);
}


LPDIRECT3DTEXTURE9 Camera::GetRenderTexture()
{
	//ī�޶��� ����Ÿ�� �ؽ��ĸ� �����´�.
	return RenderTexture;
}



