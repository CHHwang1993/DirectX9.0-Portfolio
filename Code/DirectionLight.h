#pragma once
#include "Light.h"
class DirectionLight : public Light
{
public:
	DirectionLight();
	virtual ~DirectionLight();

	virtual D3DXMATRIXA16 GetLightMatrix();
	virtual D3DXVECTOR3 GetLightDirection();
	
};

