#include "stdafx.h"
#include "Frustum.h"


Frustum::Frustum()
{
	vPrevVertex[0] = D3DXVECTOR3(-1, -1, 0);
	vPrevVertex[1] = D3DXVECTOR3(1, -1, 0);
	vPrevVertex[2] = D3DXVECTOR3(1, -1, 1);
	vPrevVertex[3] = D3DXVECTOR3(-1, -1, 1);

	vPrevVertex[4] = D3DXVECTOR3(-1, 1, 0);
	vPrevVertex[5] = D3DXVECTOR3(1, 1, 0);
	vPrevVertex[6] = D3DXVECTOR3(1, 1, 1);
	vPrevVertex[7] = D3DXVECTOR3(-1, 1, 1);
}


Frustum::~Frustum()
{
}

void Frustum::UpdateFrustum(D3DXMATRIXA16 * matViewProj)
{
	D3DXMATRIXA16 matInv; //뷰프로젝션 행렬의 역행렬

	D3DXMatrixInverse(&matInv, NULL, matViewProj);

	//버텍스의 최종행렬은 = 로컬 * 월드 * 뷰 * 프로젝션으로 구성 되며,
	//월드는 로컬과 월드가 곱해졌기 때문에 월드 * 뷰 * 프로젝션으로 구성된다.
	//여기서 뷰 * 프로젝션의 역행렬을 곱하게 되면 월드 행렬이 구해지게 되고
	//그 월드 행렬을 이용해서 월드좌표계의 절두체 좌표를 얻을 수 있다.
	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&vVertex[i], &vPrevVertex[i], &matInv);

	//얻어진 월드좌표로 절두체 평면을 만든다.
	D3DXPlaneFromPoints(&plane[0], &vVertex[4], &vVertex[7], &vVertex[6]); // 상 평면(Top)
	D3DXPlaneFromPoints(&plane[1], &vVertex[0], &vVertex[1], &vVertex[2]); // 하 평면(Bottom)
	D3DXPlaneFromPoints(&plane[2], &vVertex[0], &vVertex[4], &vVertex[5]); // 근 평면(Near)
	D3DXPlaneFromPoints(&plane[3], &vVertex[2], &vVertex[6], &vVertex[7]); // 원 평면(Far)
	D3DXPlaneFromPoints(&plane[4], &vVertex[0], &vVertex[3], &vVertex[7]); // 좌 평면(Left)
	D3DXPlaneFromPoints(&plane[5], &vVertex[1], &vVertex[5], &vVertex[6]); // 우 평면(Right)
}

bool Frustum::IsIn(D3DXVECTOR3 * pPos)
{
	float fDist;

	//6개의 평면을 다 검사해서 절두체 안에 존재할 때만 트루를 반환
	for (int i = 0; i < 6; ++i)
	{
		fDist = D3DXPlaneDotCoord(&plane[i], pPos);
		if (fDist > PLANE_EPSILON) return false;
	}

	return true;
}

bool Frustum::IsInSphere(D3DXVECTOR3 * pPos, float radius)
{
	float fDist;

	for (int i = 0; i < 6; ++i)
	{
		fDist = D3DXPlaneDotCoord(&plane[i], pPos);
		if (fDist > radius) return false;
	}

	return true;
}

bool Frustum::IsInRectangle(D3DXVECTOR3* minimum, D3DXVECTOR3* maximum)
{
	float fDist;

	for (int i = 0; i < 6; ++i)
	{
		fDist = D3DXPlaneDotCoord(&plane[i], minimum);
		if (fDist > TERRAIN_EPSILON) return false;

		fDist = D3DXPlaneDotCoord(&plane[i], maximum);
		if (fDist > TERRAIN_EPSILON) return false;
	}

	return true;
}
