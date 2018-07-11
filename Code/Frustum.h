#pragma once

#define PLANE_EPSILON 5.0f //�ణ�� ������ �ش�. �̰� ���ָ� �ؿ��� ©������!!
#define TERRAIN_EPSILON 100.0f

class Frustum
{
private:
	D3DXVECTOR3 vVertex[8]; //����ü�� ������ 8���� ����(��ȯ��)
	D3DXVECTOR3 vPrevVertex[8]; //����ü�� ������ 8���� ����(��ȯ��)
	D3DXPLANE plane[6];  //����ü�� ������ 6���� ���
public:
	Frustum();
	~Frustum();

	void UpdateFrustum(D3DXMATRIXA16* matViewProj); //���������� ����� ���ڷ�, ����ü ����� �����Ѵ�.

	bool IsIn(D3DXVECTOR3* pPos); //���ڷ� ���� ��ġ�� ����ü �ȿ� ������ false ��ȯ, �ƴϸ� true��ȯ

	bool IsInSphere(D3DXVECTOR3* pPos, float radius); //��豸�� ����ü �ȿ� ������ false ��ȯ, �ƴϸ� true��ȯ

	bool IsInRectangle(D3DXVECTOR3* minimum,D3DXVECTOR3* maximum); //�ּ�, �ִ�������� ũ�⸦ ���� �簢���� �ȿ� ������ false ��ȯ ������ true ��ȯ
};

