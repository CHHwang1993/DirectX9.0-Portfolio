#include "stdafx.h"
#include "Transform.h"


Transform::Transform()
{
	//초기화 과정

	this->pParent = NULL;
	this->pFirstChild = NULL;
	this->pNextChild = NULL;

	this->Reset();
}


Transform::~Transform()
{

}

void Transform::Reset()
{
	//위치 정보 초기화
	this->Position = D3DXVECTOR3(0, 0, 0);

	//로테이션 정보 초기화
	this->Forward = D3DXVECTOR3(0, 0, 1);
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);

	//스케일 초기화
	this->Scale = D3DXVECTOR3(1, 1, 1);

	//업데이트
	this->UpdateTransform();
}

void Transform::AddChild(Transform * child)
{
	//자식의 부모가 자신일때는 할 필요가 없음
	if (child->pParent == this) return;

	//다른 부모와 연결을 끊음
	child->ReleaseParent();

	//부모의 역행렬을 구한다.
	D3DXMATRIXA16 MatInvWorld;
	D3DXMatrixInverse(&MatInvWorld, NULL, &this->MatWorld);

	//부모역행렬을 통해 위치, 회전, 스케일값을 얻는다.
	D3DXVec3TransformCoord(&child->Position, &child->Position, &MatInvWorld);

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&child->Axis[i], &child->Axis[i], &MatInvWorld);
	}

	child->Scale.x = D3DXVec3Length(&child->Right);
	child->Scale.y = D3DXVec3Length(&child->Up);
	child->Scale.z = D3DXVec3Length(&child->Forward);

	//회전값 정규화
	D3DXVec3Normalize(&child->Right, &child->Right);
	D3DXVec3Normalize(&child->Up, &child->Up);
	D3DXVec3Normalize(&child->Forward, &child->Forward);

	//자식의 부모를 자신으로 정한다.
	child->pParent = this;

	//나의 자식
	Transform* pChild = this->pFirstChild;

	//내 자식이 없다면?
	if (pChild == NULL)
	{
		this->pFirstChild = pChild;
	}
	else
	{
		while (pChild != NULL)
		{
			//자식의 다음자식이 없다면?
			if (pChild->pNextChild == NULL)
			{
				pChild->pNextChild = child;
				break;
			}
		}

		pChild = pChild->pNextChild;
	}

	//업데이트
	this->UpdateTransform();
}

void Transform::AttachToParent(Transform * parent)
{
	//부모에게 붙는다.
	parent->AddChild(this);
}

void Transform::ReleaseParent()
{
	//부모가 없을때는 리턴
	if (this->pParent == NULL) return;

	//부모와 자식관계 끊기
	Transform* pChild = this->pParent->pFirstChild;

	if (pChild == this)
	{
		//자신이 첫번째 자식이면 끊고 두번째 자식이 첫번째 자식이 된다.
		this->pParent->pFirstChild = this->pParent->pNextChild;

		//자신의 다음자식들도 끊는다.
		this->pNextChild = NULL;
	}
	else
	{
		while (pChild != NULL)
		{
			//현재 자식의 다음자식이 나라면?
			if (pChild->pNextChild == this)
			{
				//현재 자식의 다음자식을 나의 자식으로 넘긴다.
				pChild->pNextChild = this->pNextChild;

				//자신의 다음 자식들을 끊는다.
				this->pNextChild = NULL;

				break;
			}
			//다음 자식으로 넘어간다
			pChild = pChild->pNextChild;
		}
	}
	//자신의 부모와 연결 해제
	this->pParent = NULL;

	//위치 갱신
	this->Position.x = this->MatWorld._41;
	this->Position.y = this->MatWorld._42;
	this->Position.z = this->MatWorld._43;

	//축 갱신
	D3DXVECTOR3 ScaleRight(this->MatWorld._11, this->MatWorld._12, this->MatWorld._13);
	D3DXVECTOR3 ScaleUp(this->MatWorld._21, this->MatWorld._22, this->MatWorld._23);
	D3DXVECTOR3 ScaleForward(this->MatWorld._31, this->MatWorld._32, this->MatWorld._33);

	//축에서 스케일을 뺀다.
	float ScaleX = D3DXVec3Length(&ScaleRight);
	float ScaleY = D3DXVec3Length(&ScaleUp);
	float ScaleZ = D3DXVec3Length(&ScaleForward);

	//축 정규화
	D3DXVECTOR3 NorRight;
	D3DXVECTOR3 NorUp;
	D3DXVECTOR3 NorForward;

	D3DXVec3Normalize(&NorRight, &ScaleRight);
	D3DXVec3Normalize(&NorUp, &ScaleUp);
	D3DXVec3Normalize(&NorForward, &ScaleForward);

	//정규화된 축 갱신
	this->Right = NorRight;
	this->Up = NorUp;
	this->Forward = NorForward;

	//스케일 갱신
	this->Scale.x = ScaleX;
	this->Scale.y = ScaleY;
	this->Scale.z = ScaleZ;

	//마지막으로 트랜스폼 업데이트
	this->UpdateTransform();
}

void Transform::SetWorldPosition(float x, float y, float z)
{
	D3DXVECTOR3 pos(x, y, z);

	if (this->pParent != NULL)
	{
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		D3DXVec3TransformCoord(&pos, &pos, &MatInvWorld);
	}

	this->Position.x = pos.x;
	this->Position.y = pos.y;
	this->Position.z = pos.z;

	this->UpdateTransform();
}

void Transform::SetWorldPosition(D3DXVECTOR3 position)
{
	if (this->pParent != NULL)
	{
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		D3DXVec3TransformCoord(&position, &position, &MatInvWorld);
	}

	this->Position.x = position.x;
	this->Position.y = position.y;
	this->Position.z = position.z;

	this->UpdateTransform();
}

void Transform::SetLocalPosition(float x, float y, float z)
{
	this->Position.x = x;
	this->Position.y = y;
	this->Position.z = z;
}

void Transform::SetLocalPosition(D3DXVECTOR3 position)
{
	this->Position.x = position.x;
	this->Position.y = position.y;
	this->Position.z = position.z;
}

void Transform::MovePositionSelf(float dx, float dy, float dz)
{
	D3DXVECTOR3 Move(0, 0, 0);

	//자신의 이동축 얻는다.
	D3DXVECTOR3 moveAxis[3];
	this->GetUnitAxies(moveAxis);
	Move += moveAxis[0] * dx;
	Move += moveAxis[1] * dy;
	Move += moveAxis[2] * dz;

	//현재 월드포지션을 구해온다.
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//위치 갱신
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionSelf(D3DXVECTOR3 delta)
{
	D3DXVECTOR3 Move(0, 0, 0);

	//자신의 이동축 얻는다.
	D3DXVECTOR3 moveAxis[3];
	this->GetUnitAxies(moveAxis);
	Move += moveAxis[0] * delta.x;
	Move += moveAxis[1] * delta.y;
	Move += moveAxis[2] * delta.z;

	//현재 월드포지션을 구해온다.
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//위치 갱신
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionWorld(float dx, float dy, float dz)
{
	//이동값
	D3DXVECTOR3 Move(dx, dy, dz);

	//월드 포지션값 얻어오기
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//이동 갱신
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionWorld(D3DXVECTOR3 delta)
{
	//월드 포지션값 얻어오기
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//이동 갱신
	this->SetWorldPosition(CurWorldPos + delta);
}

void Transform::MovePositionLocal(float dx, float dy, float dz)
{
	this->Position.x += dx;
	this->Position.y += dy;
	this->Position.z += dz;

	this->UpdateTransform();
}

void Transform::MovePositionLocal(D3DXVECTOR3 delta)
{
	//로컬포지션 이동
	this->Position += delta;

	this->UpdateTransform();
}

void Transform::SetScale(float x, float y, float z)
{
	this->Scale.x = x;
	this->Scale.y = y;
	this->Scale.z = z;

	this->UpdateTransform();
}

void Transform::SetScale(D3DXVECTOR3 scale)
{
	this->Scale = scale;

	this->UpdateTransform();
}

void Transform::Scaling(float dx, float dy, float dz)
{
	//스케일을 x,y,z값만큼 늘린다.
	this->Scale.x += dx;
	this->Scale.x += dy;
	this->Scale.x += dz;

	this->UpdateTransform();
}

void Transform::Scaling(D3DXVECTOR3 delta)
{
	//스케일을 delta값만큼 늘린다.
	this->Scale += delta;

	this->UpdateTransform();
}

void Transform::RotateWorld(float angleX, float angleY, float angleZ)
{
	if (this->pParent) //부모가 있는 경우
	{
		D3DXVECTOR3 worldAxis[3];
		this->GetUnitAxies(worldAxis);

		//각 축에 대한 회전행렬
		D3DXMATRIXA16 matRotateX;
		D3DXMatrixRotationX(&matRotateX, angleX);

		D3DXMATRIXA16 matRotateY;
		D3DXMatrixRotationY(&matRotateY, angleY);

		D3DXMATRIXA16 matRotateZ;
		D3DXMatrixRotationZ(&matRotateZ, angleZ);

		D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

		//부모 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;

		for (int i = 0; i < 3; ++i)
		{
			D3DXVec3TransformNormal(&this->Axis[i], &worldAxis[i], &matRotate);
		}

		this->UpdateTransform();
	}
	else //부모가 없는 경우
	{
		RotateLocal(angleX, angleY, angleZ);
	}
}

void Transform::RotateWorld(D3DXVECTOR3 angle)
{
	if (this->pParent) //부모가 있는 경우
	{
		D3DXVECTOR3 worldAxis[3];
		this->GetUnitAxies(worldAxis);

		//각 축에 대한 회전행렬
		D3DXMATRIXA16 matRotateX;
		D3DXMatrixRotationX(&matRotateX, angle.x);

		D3DXMATRIXA16 matRotateY;
		D3DXMatrixRotationY(&matRotateY, angle.y);

		D3DXMATRIXA16 matRotateZ;
		D3DXMatrixRotationZ(&matRotateZ, angle.z);

		D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

		//부모 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;

		for (int i = 0; i < 3; ++i)
		{
			D3DXVec3TransformNormal(&this->Axis[i], &worldAxis[i], &matRotate);
		}

		this->UpdateTransform();
	}
	else //부모가 없는 경우
	{
		RotateLocal(angle);
	}
}

void Transform::RotateSelf(float angleX, float angleY, float angleZ)
{
	//각 축에 대한 회전행렬
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->GetRight(), angleX);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->GetUp(), angleY);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->GetForward(), angleZ);

	//최종 회전 행렬
	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::RotateSelf(D3DXVECTOR3 angle)
{
	//각 축에 대한 회전행렬
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->GetRight(), angle.x);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->GetUp(), angle.y);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->GetForward(), angle.z);

	//최종 회전 행렬
	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::RotateLocal(float angleX, float angleY, float angleZ)
{
	//각 축에 대한 회전행렬
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationX(&matRotateX, angleX);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationY(&matRotateY, angleY);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationZ(&matRotateZ, angleZ);

	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	//회전행렬 축 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::RotateLocal(D3DXVECTOR3 angle)
{
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationX(&matRotateX, angle.x);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationY(&matRotateY, angle.y);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationZ(&matRotateZ, angle.z);

	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	//회전행렬 축 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateWorld(float eAngleX, float eAngleY, float eAngleZ)
{
	//사원수 준비
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, eAngleY, eAngleX, eAngleZ);

	//사원수를 회전행렬로 변환한다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//부모가 있을 경우
	if (this->pParent)
	{
		//부모의 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateLocal(float eAngleX, float eAngleY, float eAngleZ)
{
	//사원수 준비
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, eAngleY, eAngleX, eAngleZ);

	//사원수를 회전행렬로 변환한다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateWorld(const D3DXMATRIXA16 & matWorldRotate)
{
	D3DXMATRIXA16 matRotate = matWorldRotate;

	//부모가 있을 경우
	if (this->pParent)
	{
		//부모의 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateLocal(const D3DXMATRIXA16 & matWorldRotate)
{
	D3DXMATRIXA16 matRotate = matWorldRotate;
	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateWorld(const D3DXQUATERNION & QuatWorldRotate)
{
	//사원수를 회전행렬로 변환한다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &QuatWorldRotate);

	//부모가 있을 경우
	if (this->pParent)
	{
		//부모의 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::SetRotateLocal(const D3DXQUATERNION & QuatWorldRotate)
{
	//사원수를 회전행렬로 변환한다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &QuatWorldRotate);

	//축 리셋
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//회전행렬값 벡터에 적용
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//업데이트
	this->UpdateTransform();
}

void Transform::LookDirection(D3DXVECTOR3 dir, D3DXVECTOR3 up)
{
	//정면벡터
	D3DXVECTOR3 newForward = dir;

	//우측벡터
	D3DXVECTOR3 newRight;
	D3DXVec3Cross(&newRight, &up, &newForward);
	D3DXVec3Normalize(&newRight, &newRight);

	//업 벡터
	D3DXVECTOR3 newUp;
	D3DXVec3Cross(&newUp, &newForward, &newRight);
	D3DXVec3Normalize(&newUp, &newUp);

	if (this->pParent)
	{
		//부모 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		D3DXVec3TransformNormal(&this->Right, &newRight, &MatInvWorld);
		D3DXVec3TransformNormal(&this->Up, &newUp, &MatInvWorld);
		D3DXVec3TransformNormal(&this->Forward, &newForward, &MatInvWorld);
	}
	else
	{
		this->Right = newRight;
		this->Forward = newForward;
		this->Up = newUp;
	}

	this->UpdateTransform();
}

void Transform::LookDirection(D3DXVECTOR3 dir, float angle)
{
	//월드축
	D3DXVECTOR3 WorldAxis[3];
	this->GetUnitAxies(WorldAxis);

	//정면벡터와 바라볼 방향벡터의 각도차
	float distRadian = acos(ClampMinusOnePlusOne(D3DXVec3Dot(&WorldAxis[AXIS_Z], &dir)));

	//정면벡터와 바라볼 방향벡터가 차이가 없다면 리턴
	if (FLOATZERO(distRadian)) return;

	//정면벡터와 방향벡터를 외적함
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &WorldAxis[AXIS_Z], &dir);
	D3DXVec3Normalize(&cross, &cross);

	//외적축으로 각도만큼 회전시키는 행렬
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationAxis(&matRotate, &cross, min(angle, distRadian));

	//만약 부모가 있다면?
	if (this->pParent)
	{
		//부모의 역행렬
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &WorldAxis[i], &matRotate);
	}

	this->UpdateTransform();
}

void Transform::LookPosition(D3DXVECTOR3 pos, D3DXVECTOR3 up)
{
	D3DXVECTOR3 WorldPos = this->GetWorldPosition();

	//방향벡터 구하기
	D3DXVECTOR3 Dir = pos - WorldPos;
	D3DXVec3Normalize(&Dir, &Dir);

	//구한 방향벡터로 회전
	LookDirection(Dir, up);
}

void Transform::LookPosition(D3DXVECTOR3 pos, float angle)
{
	D3DXVECTOR3 WorldPos = this->GetWorldPosition();

	//방향벡터 구하기
	D3DXVECTOR3 Dir = pos - WorldPos;
	D3DXVec3Normalize(&Dir, &Dir);

	//구한 방향벡터로 회전
	LookDirection(Dir, angle);
}

void Transform::RotateSlerp(const Transform & from, const Transform & to, float t)
{
	//0과 1사이의 값을 구한다.
	t = Clamp01(t);

	D3DXQUATERNION fromQuat = from.GetWorldRotateQuaternion();
	D3DXQUATERNION toQuat = to.GetWorldRotateQuaternion();

	//0과 같을때 from 쿼터니언값을 회전에 적용
	if (FLOATZERO(t))
	{
		this->SetRotateWorld(fromQuat);
	}
	else if (FLOATEQUAL(t, 1.0f)) //1과 같을때 to 쿼터니언값을 회전에 적용
	{
		this->SetRotateWorld(toQuat);
	}
	else // 0과 1이 아닐때는 두개의 쿼터니언값을 보간함
	{
		D3DXQUATERNION result;

		D3DXQuaternionSlerp(&result, &fromQuat, &toQuat, t);

		this->SetRotateWorld(result);
	}
}

void Transform::PositionLerp(const Transform & from, const Transform & to, float t)
{
	//0과 1사이의 값을 구한다.
	t = Clamp01(t);

	D3DXVECTOR3 fromPos = from.GetWorldPosition();
	D3DXVECTOR3 toPos = to.GetWorldPosition();

	//0과 같을 때 fromPos를 월드 포지션으로 세팅
	if (FLOATZERO(t))
	{
		this->SetWorldPosition(fromPos);
	}
	else if (FLOATEQUAL(t, 1.0f)) //1과 같을 때 toPos를 월드 포지션으로 세팅
	{
		this->SetWorldPosition(toPos);
	}
	else // 0과 1이 아닐때는 두개의 벡터를 보간해 구함
	{
		D3DXVECTOR3 result;
		D3DXVec3Lerp(&result, &fromPos, &toPos, t);

		this->SetWorldPosition(result);
	}
}

void Transform::Interpolate(const Transform & from, const Transform & to, float t)
{
	//0과 1사이의 값을 구한다.
	t = Clamp01(t);

	D3DXVECTOR3 resultScale;
	D3DXVECTOR3 resultPos;
	D3DXQUATERNION resultRot;

	if (FLOATZERO(t)) //0과 같을 때 from Transform 세팅
	{
		resultScale = from.Scale;
		resultPos = from.GetWorldPosition();
		resultRot = from.GetWorldRotateQuaternion();
	}
	else if (FLOATEQUAL(t, 1.0f)) //0과 같을 때 to Transform 세팅
	{
		resultScale = to.Scale;
		resultPos = to.GetWorldPosition();
		resultRot = to.GetWorldRotateQuaternion();
	}
	else // 0과 1이 아닐때는 from과 to 정보들을 보간해 구함
	{
		//스케일
		D3DXVECTOR3 fromScale = from.Scale;
		D3DXVECTOR3 toScale = to.Scale;

		//위치
		D3DXVECTOR3 fromPos = from.GetWorldPosition();
		D3DXVECTOR3 toPos = to.GetWorldPosition();

		//회전
		D3DXQUATERNION fromRot = from.GetWorldRotateQuaternion();
		D3DXQUATERNION toRot = to.GetWorldRotateQuaternion();

		//각각의 from, to 정보들을 보간한다.
		D3DXVec3Lerp(&resultScale, &fromScale, &toScale, t);
		D3DXVec3Lerp(&resultPos, &fromPos, &toPos, t);
		D3DXQuaternionSlerp(&resultRot, &fromRot, &toRot, t);
	}

	this->SetScale(resultScale);
	this->SetWorldPosition(resultPos);
	this->SetRotateWorld(resultRot);

	this->UpdateTransform();
}

void Transform::SetWorldMatrix(const D3DXMATRIXA16 & matWorld)
{
	//월드행렬 세팅
	this->MatWorld = matWorld;
	this->MatLocal = matWorld;

	//월드위치 
	this->Position = D3DXVECTOR3(MatWorld._41, MatWorld._42, MatWorld._43);

	//축벡터 구하기
	D3DXVECTOR3 forward = D3DXVECTOR3(MatWorld._31, MatWorld._32, MatWorld._33);
	D3DXVECTOR3 up = D3DXVECTOR3(MatWorld._21, MatWorld._22, MatWorld._23);
	D3DXVECTOR3 right = D3DXVECTOR3(MatWorld._11, MatWorld._12, MatWorld._13);

	//정규화
	D3DXVec3Normalize(&this->Forward, &forward);
	D3DXVec3Normalize(&this->Up, &up);
	D3DXVec3Normalize(&this->Right, &right);

	//벡터의 길이로 스케일을 준다.
	this->Scale.x = D3DXVec3Length(&right);
	this->Scale.y = D3DXVec3Length(&up);
	this->Scale.z = D3DXVec3Length(&forward);

	//부모가 있다면?
	if (this->pParent)
	{
		//부모의 역행렬 구하기
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		this->MatLocal = this->MatWorld * MatInvWorld;

		//나의 로컬 위치
		memcpy(&this->Position, &this->MatLocal._41, sizeof(D3DXVECTOR3));

		//나의 축 벡터
		D3DXVECTOR3 forward;
		memcpy(&forward, &this->MatLocal._31, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 up;
		memcpy(&up, &this->MatLocal._21, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 right;
		memcpy(&right, &this->MatLocal._11, sizeof(D3DXVECTOR3));

		//각 스케일
		float scaleX = D3DXVec3Length(&right);
		float scaleY = D3DXVec3Length(&up);
		float scaleZ = D3DXVec3Length(&forward);

		this->Scale = D3DXVECTOR3(scaleX, scaleY, scaleZ);

		forward *= (1.0f) / scaleX;
		up *= (1.0f) / scaleY;
		right *= (1.0f) / scaleZ;

		this->Forward = forward;
		this->Up = up;
		this->Right = right;
	}

	//자식도 업데이트
	Transform* pChild = this->pFirstChild;

	while (pChild != NULL)
	{
		pChild->UpdateTransform();
		pChild = pChild->pNextChild;
	}

}

void Transform::UpdateTransform()
{
	D3DXMatrixIdentity(&this->MatLocal);

	//스케일 적용된 축정보
	D3DXVECTOR3 ScaleRight = this->Right * this->Scale.x;
	D3DXVECTOR3 ScaleUp = this->Up * this->Scale.y;
	D3DXVECTOR3 ScaleForward = this->Forward * this->Scale.z;

	//축, 위치정보를 로컬에 복사한다.
	memcpy(&this->MatLocal._11, &ScaleRight, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._21, &ScaleUp, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._31, &ScaleForward, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._41, &this->Position, sizeof(D3DXVECTOR3));

	if (this->pParent == NULL) // 부모가 없는 경우
	{
		this->MatWorld = this->MatLocal;
	}
	else // 부모가 있을 경우
	{
		this->MatWorld = this->MatLocal * this->pParent->MatWorld;
	}

	//자식정보도 업데이트한다.
	Transform* pChild = this->pFirstChild;

	while (pChild != NULL)
	{
		pChild->UpdateTransform();
		pChild = pChild->pNextChild;
	}
}

void Transform::SetDeviceWorld(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetTransform(D3DTS_WORLD, &this->MatWorld);
}

void Transform::SetDeviceView(LPDIRECT3DDEVICE9 pDevice)
{
	D3DXMATRIXA16 matView;
	D3DXMatrixInverse(&matView, NULL, &this->MatWorld);

	pDevice->SetTransform(D3DTS_VIEW, &matView);
}

D3DXMATRIXA16 Transform::GetFinalMatrix() const
{
	return this->MatWorld;
}

D3DXMATRIXA16 Transform::GetWorldRotateMatrix() const
{
	//자신의 축으로 회전행렬을 만든다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixIdentity(&matRotate);

	//3축을 얻는다.
	D3DXVECTOR3 WorldAxis[3];
	this->GetUnitAxies(WorldAxis);

	//행렬에 축벡터의 값을 복사한다.
	memcpy(&matRotate._11, WorldAxis[0], sizeof(D3DXVECTOR3));
	memcpy(&matRotate._21, WorldAxis[1], sizeof(D3DXVECTOR3));
	memcpy(&matRotate._31, WorldAxis[2], sizeof(D3DXVECTOR3));

	return matRotate;
}

D3DXQUATERNION Transform::GetWorldRotateQuaternion() const
{
	D3DXQUATERNION Quat;

	D3DXMATRIXA16 matRotate = GetWorldRotateMatrix();

	//회전행렬로 사원수를 만든다.
	D3DXQuaternionRotationMatrix(&Quat, &matRotate);

	return Quat;
}

D3DXVECTOR3 Transform::GetWorldPosition() const
{
	D3DXVECTOR3 pos = this->Position;

	if (this->pParent)
	{
		D3DXVec3TransformCoord(&pos, &pos, &this->pParent->MatWorld);
	}

	return pos;
}

D3DXVECTOR3 Transform::GetLocalPosition() const
{
	D3DXVECTOR3 pos = this->Position;

	return pos;
}

void Transform::GetUnitAxies(D3DXVECTOR3 * pVecArr) const
{
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3Normalize(&pVecArr[i], &this->Axis[i]);
	}

	if (this->pParent)
	{
		for (int i = 0; i < 3; ++i)
		{
			D3DXMATRIXA16 ParentWorld = this->pParent->MatWorld;
			D3DXVec3TransformNormal(&pVecArr[i], &pVecArr[i], &ParentWorld);
		}
	}
}

void Transform::GetScaledAxies(D3DXVECTOR3 * pVecArr) const
{
	for (int i = 0; i < 3; ++i)
	{
		pVecArr[i] = this->Axis[i];
	}

	if (this->pParent)
	{
		for (int i = 0; i < 3; ++i)
		{
			D3DXMATRIXA16 ParentWorld = this->pParent->MatWorld;
			D3DXVec3TransformNormal(&pVecArr[i], &pVecArr[i], &ParentWorld);
		}
	}
}

D3DXVECTOR3 Transform::GetUnitAxis(int axis) const
{
	D3DXVECTOR3 result;
	D3DXVec3Normalize(&result, &this->Axis[axis]);

	//부모가 있을 경우
	if (this->pParent)
	{
		D3DXMATRIXA16 ParentWorld = this->pParent->MatWorld;
		D3DXVec3TransformNormal(&result, &result, &ParentWorld);
	}

	return result;
}

D3DXVECTOR3 Transform::GetScaledAxis(int axis) const
{
	D3DXVECTOR3 result = this->Axis[axis];

	//부모가 있을 경우
	if (this->pParent)
	{
		D3DXMATRIXA16 ParentWorld = this->pParent->MatWorld;
		D3DXVec3TransformNormal(&result, &result, &ParentWorld);
	}

	return result;
}

D3DXVECTOR3 Transform::GetForward(bool bNormalize) const
{
	if (bNormalize) return this->GetUnitAxis(AXIS_Z);

	return this->GetScaledAxis(AXIS_Z);
}

D3DXVECTOR3 Transform::GetUp(bool bNormalize) const
{
	if (bNormalize) return this->GetUnitAxis(AXIS_Y);

	return this->GetScaledAxis(AXIS_Y);
}

D3DXVECTOR3 Transform::GetRight(bool bNormalize) const
{
	if (bNormalize) return this->GetUnitAxis(AXIS_X);

	return this->GetScaledAxis(AXIS_X);
}

