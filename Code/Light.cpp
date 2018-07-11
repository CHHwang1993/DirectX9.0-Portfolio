#include "stdafx.h"
#include "Light.h"
#include "Transform.h"

Light::Light()
	: Intensity(1.0f)
{
	this->pTransform = new Transform();
}

Light::~Light()
{
	SAFE_DELETE(pTransform);
}

void Light::RotateX(float deltaX)
{
	this->pTransform->RotateWorld(deltaX, 0, 0);
}

void Light::RotateY(float deltaY)
{
	this->pTransform->RotateWorld(0, deltaY, 0);
}

void Light::RotateZ(float deltaZ)
{
	this->pTransform->RotateWorld(0, 0, deltaZ);
}
