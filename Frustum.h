#pragma once

#define PLANE_EPSILON 5.0f //약간의 여분을 준다. 이거 안주면 밑에가 짤려보임!!
#define TERRAIN_EPSILON 100.0f

class Frustum
{
private:
	D3DXVECTOR3 vVertex[8]; //절두체를 구성할 8개의 정점(변환후)
	D3DXVECTOR3 vPrevVertex[8]; //절두체를 구성할 8개의 정점(변환전)
	D3DXPLANE plane[6];  //절두체를 구성할 6개의 평면
public:
	Frustum();
	~Frustum();

	void UpdateFrustum(D3DXMATRIXA16* matViewProj); //뷰프로젝션 행렬을 인자로, 절두체 평면을 구성한다.

	bool IsIn(D3DXVECTOR3* pPos); //인자로 받은 위치가 절두체 안에 있으면 false 반환, 아니면 true반환

	bool IsInSphere(D3DXVECTOR3* pPos, float radius); //경계구가 절두체 안에 있으면 false 반환, 아니면 true반환

	bool IsInRectangle(D3DXVECTOR3* minimum,D3DXVECTOR3* maximum); //최소, 최대사이즈의 크기를 가진 사각형이 안에 있으면 false 반환 없으면 true 반환
};

