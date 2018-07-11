#pragma once
#include "Light.h"
class PointLIght : public Light
{
public:
	float minRange; // ���� �ּҹ���
	float maxRange; // ���� �ִ����
	float distancePow; // �Ÿ� ������
public:
	PointLIght();
	virtual ~PointLIght();

	virtual D3DXMATRIXA16 GetLightMatrix();

	virtual void SetPosition(float x,float y, float z);
	virtual void SetPosition(D3DXVECTOR3 pos);
};

