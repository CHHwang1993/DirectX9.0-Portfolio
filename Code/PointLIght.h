#pragma once
#include "Light.h"
class PointLIght : public Light
{
public:
	float minRange; // 빛의 최소범위
	float maxRange; // 빛의 최대범위
	float distancePow; // 거리 감쇄율
public:
	PointLIght();
	virtual ~PointLIght();

	virtual D3DXMATRIXA16 GetLightMatrix();

	virtual void SetPosition(float x,float y, float z);
	virtual void SetPosition(D3DXVECTOR3 pos);
};

