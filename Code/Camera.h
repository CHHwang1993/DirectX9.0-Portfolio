#pragma once

#include "Transform.h"
#include "Frustum.h"

class Camera : public Transform
{

public:
	float fov; // 카메라 화각
	float camNear; // 카메라 근거리
	float camFar; // 카메라 원거리
	float aspect; // 카메라 종횡비
	bool bOrtho; // 직교투영 여부
	float orthoSize; // 직교투영 사이즈
	float camSpeed; //카메라 이동속도
	float camRotSpeed; //카메라 회전속도

private:
	
	D3DXMATRIXA16 MatView; //뷰행렬
	D3DXMATRIXA16 MatProjection;//프로젝션 행렬
	D3DXMATRIXA16 MatViewProjection; //뷰프로젝션 행렬
	D3DXVECTOR2 rotate; //회전 각도
	D3DXMATRIX rotation; //회전 공간

private:

	LPDIRECT3DTEXTURE9 RenderTexture; //렌더 타겟용 텍스쳐
	LPDIRECT3DSURFACE9 RenderSurface; //랜더 Texture Deapth 버퍼와 Stencil 버퍼가 있는 Surface
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

	void ReadyRenderToTexture(int width, int height); //렌더 텍스쳐 준비
	void ReadyShadowToTexture(int size); // 그림자 맵 텍스쳐 준비

	void RenderTextureBegin(DWORD backColor); // 렌더텍스쳐로 렌더링 시작

	void RenderTextureEnd(); //렌더텍스쳐로 렌더링 종료

	LPDIRECT3DTEXTURE9 GetRenderTexture(); // 렌더텍스쳐 가져온다.
	LPDIRECT3DTEXTURE9 GetRenderTexture2(); // 렌더텍스쳐 가져온다.
};