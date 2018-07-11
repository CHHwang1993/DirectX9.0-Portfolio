#pragma once

class Transform;

class Light
{

public:
	Transform * pTransform; // ����Ʈ�� Ʈ������
	D3DXCOLOR Color; // ���� Į��
	float Intensity; // ���� ����

public:
	Light();
	virtual ~Light();

	virtual D3DXMATRIXA16 GetLightMatrix() = 0;
	virtual void RotateX(float deltaX);
	virtual void RotateY(float deltaY);
	virtual void RotateZ(float deltaZ);
};


