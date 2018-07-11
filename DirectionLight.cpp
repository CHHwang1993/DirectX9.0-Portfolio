#include "stdafx.h"
#include "DirectionLight.h"


DirectionLight::DirectionLight()
{
}


DirectionLight::~DirectionLight()
{
}

D3DXMATRIXA16 DirectionLight::GetLightMatrix()
{
	// 라이트 행렬
	// Pos.x,         Pos.y,          Pos.z,         lightType ( 0 Direction, 1 Point, 2 Spot )
	// Dir.x,         Dir.y,          Dir.z,         DistancePow;
	// Color.r,       Color.g,        Color.b,       strength
	// MaxRange,      MinRange,       InAngle        OutAngle 

	D3DXMATRIXA16 matLight;
	D3DXMatrixIdentity(&matLight);

	matLight._14 = 0;

	D3DXVECTOR3 Forward = this->pTransform->GetForward();
	memcpy(&matLight._21, &Forward, sizeof(D3DXVECTOR3));
	memcpy(&matLight._31, &this->Color, sizeof(D3DXVECTOR3));
	matLight._34 = this->Intensity;

	return matLight;
}

D3DXVECTOR3 DirectionLight::GetLightDirection()
{
	return D3DXVECTOR3(this->GetLightMatrix()._21, this->GetLightMatrix()._22, this->GetLightMatrix()._23);
}


