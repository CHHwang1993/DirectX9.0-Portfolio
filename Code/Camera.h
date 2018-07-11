#pragma once

#include "Transform.h"
#include "Frustum.h"

class Camera : public Transform
{

public:
	float fov; // ī�޶� ȭ��
	float camNear; // ī�޶� �ٰŸ�
	float camFar; // ī�޶� ���Ÿ�
	float aspect; // ī�޶� ��Ⱦ��
	bool bOrtho; // �������� ����
	float orthoSize; // �������� ������
	float camSpeed; //ī�޶� �̵��ӵ�
	float camRotSpeed; //ī�޶� ȸ���ӵ�

private:
	
	D3DXMATRIXA16 MatView; //�����
	D3DXMATRIXA16 MatProjection;//�������� ���
	D3DXMATRIXA16 MatViewProjection; //���������� ���
	D3DXVECTOR2 rotate; //ȸ�� ����
	D3DXMATRIX rotation; //ȸ�� ����

private:

	LPDIRECT3DTEXTURE9 RenderTexture; //���� Ÿ�ٿ� �ؽ���
	LPDIRECT3DSURFACE9 RenderSurface; //���� Texture Deapth ���ۿ� Stencil ���۰� �ִ� Surface
	LPDIRECT3DSURFACE9 DeviceTargetSurface;
	LPDIRECT3DSURFACE9 DeviceDepthAndStencilSurface;

	Frustum frustum;

public:
	Camera();
	virtual ~Camera();

	void UpdateMatrix();
	void UpdateFrustum();
	void UpdateCamToDevice(LPDIRECT3DDEVICE9 pDevice);

	void Move(D3DXVECTOR3 Delta);
	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	void MoveDown();
	void MoveUp();

	void CameraControl();

	void Rotation(D3DXVECTOR3 Delta);

	D3DXMATRIXA16 GetViewMatrix() const { return this->MatView; }
	D3DXMATRIXA16 GetProjectionMatrix() const { return this->MatProjection; }
	D3DXMATRIXA16 GetViewProjectionMatrix() const { return this->MatViewProjection; }

	Frustum GetFrustum() { return frustum; }

	void ReadyRenderToTexture(int width, int height); //���� �ؽ��� �غ�
	void ReadyShadowToTexture(int size); // �׸��� �� �ؽ��� �غ�

	void RenderTextureBegin(DWORD backColor); // �����ؽ��ķ� ������ ����

	void RenderTextureEnd(); //�����ؽ��ķ� ������ ����

	LPDIRECT3DTEXTURE9 GetRenderTexture(); // �����ؽ��� �����´�.
	LPDIRECT3DTEXTURE9 GetRenderTexture2(); // �����ؽ��� �����´�.
};