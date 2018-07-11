#include "stdafx.h"
#include "PointLIght.h"


PointLIght::PointLIght()
{
}


PointLIght::~PointLIght()
{
}

D3DXMATRIXA16 PointLIght::GetLightMatrix()
{
	// 라이트 행렬
	// Pos.x,         Pos.y,          Pos.z,         lightType ( 0 Direction, 1 Point, 2 Spot )
	// Dir.x,         Dir.y,          Dir.z,         DistancePow;
	// Color.r,       Color.g,        Color.b,       strength
	// MaxRange,      MinRange,       InAngle        OutAngle 

	D3DXMATRIXA16 matLight;
	D3DXMatrixIdentity(&matLight);

	matLight._14 = 1;

	D3DXVECTOR3 Pos = this->pTransform->GetWorldPosition();
	D3DXVECTOR3 Forward = this->pTransform->GetForward();

	memcpy(&matLight._11, &Pos, sizeof(D3DXVECTOR3));
	memcpy(&matLight._21, &Forward, sizeof(D3DXVECTOR3));

	matLight._24 = this->distancePow;

	memcpy(&matLight._31, &this->Color, sizeof(D3DXVECTOR3));
	matLight._34 = this->Intensity;

	matLight._41 = this->maxRange;
	matLight._42 = this->minRange;

	return matLight;
}

void PointLIght::SetPosition(float x, float y, float z)
{
	this->pTransform->SetWorldPosition(x, y, z);
}

void PointLIght::SetPosition(D3DXVECTOR3 pos)
{
	this->pTransform->SetWorldPosition(pos);
}
