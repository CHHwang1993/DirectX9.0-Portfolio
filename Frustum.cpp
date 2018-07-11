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
	D3DXMATRIXA16 matInv; //���������� ����� �����

	D3DXMatrixInverse(&matInv, NULL, matViewProj);

	//���ؽ��� ��������� = ���� * ���� * �� * ������������ ���� �Ǹ�,
	//����� ���ð� ���尡 �������� ������ ���� * �� * ������������ �����ȴ�.
	//���⼭ �� * ���������� ������� ���ϰ� �Ǹ� ���� ����� �������� �ǰ�
	//�� ���� ����� �̿��ؼ� ������ǥ���� ����ü ��ǥ�� ���� �� �ִ�.
	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&vVertex[i], &vPrevVertex[i], &matInv);

	//����� ������ǥ�� ����ü ����� �����.
	D3DXPlaneFromPoints(&plane[0], &vVertex[4], &vVertex[7], &vVertex[6]); // �� ���(Top)
	D3DXPlaneFromPoints(&plane[1], &vVertex[0], &vVertex[1], &vVertex[2]); // �� ���(Bottom)
	D3DXPlaneFromPoints(&plane[2], &vVertex[0], &vVertex[4], &vVertex[5]); // �� ���(Near)
	D3DXPlaneFromPoints(&plane[3], &vVertex[2], &vVertex[6], &vVertex[7]); // �� ���(Far)
	D3DXPlaneFromPoints(&plane[4], &vVertex[0], &vVertex[3], &vVertex[7]); // �� ���(Left)
	D3DXPlaneFromPoints(&plane[5], &vVertex[1], &vVertex[5], &vVertex[6]); // �� ���(Right)
}

bool Frustum::IsIn(D3DXVECTOR3 * pPos)
{
	float fDist;

	//6���� ����� �� �˻��ؼ� ����ü �ȿ� ������ ���� Ʈ�縦 ��ȯ
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
