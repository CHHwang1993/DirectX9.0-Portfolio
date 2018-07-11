#pragma once

class Transform;

class Light
{

public:
	Transform * pTransform; // ¶óÀÌÆ®ÀÇ Æ®·£½ºÆû
	D3DXCOLOR Color; // ºûÀÇ Ä®¶ó
	float Intensity; // ºûÀÇ °­µµ

public:
	Light();
	virtual ~Light();

	virtual D3DXMATRIXA16 GetLightMatrix() = 0;
	virtual void RotateX(float deltaX);
	virtual void RotateY(float deltaY);
	virtual void RotateZ(float deltaZ);
};


