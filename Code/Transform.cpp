#include "stdafx.h"
#include "Transform.h"


Transform::Transform()
{
	//�ʱ�ȭ ����

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
	//��ġ ���� �ʱ�ȭ
	this->Position = D3DXVECTOR3(0, 0, 0);

	//�����̼� ���� �ʱ�ȭ
	this->Forward = D3DXVECTOR3(0, 0, 1);
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);

	//������ �ʱ�ȭ
	this->Scale = D3DXVECTOR3(1, 1, 1);

	//������Ʈ
	this->UpdateTransform();
}

void Transform::AddChild(Transform * child)
{
	//�ڽ��� �θ� �ڽ��϶��� �� �ʿ䰡 ����
	if (child->pParent == this) return;

	//�ٸ� �θ�� ������ ����
	child->ReleaseParent();

	//�θ��� ������� ���Ѵ�.
	D3DXMATRIXA16 MatInvWorld;
	D3DXMatrixInverse(&MatInvWorld, NULL, &this->MatWorld);

	//�θ������ ���� ��ġ, ȸ��, �����ϰ��� ��´�.
	D3DXVec3TransformCoord(&child->Position, &child->Position, &MatInvWorld);

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&child->Axis[i], &child->Axis[i], &MatInvWorld);
	}

	child->Scale.x = D3DXVec3Length(&child->Right);
	child->Scale.y = D3DXVec3Length(&child->Up);
	child->Scale.z = D3DXVec3Length(&child->Forward);

	//ȸ���� ����ȭ
	D3DXVec3Normalize(&child->Right, &child->Right);
	D3DXVec3Normalize(&child->Up, &child->Up);
	D3DXVec3Normalize(&child->Forward, &child->Forward);

	//�ڽ��� �θ� �ڽ����� ���Ѵ�.
	child->pParent = this;

	//���� �ڽ�
	Transform* pChild = this->pFirstChild;

	//�� �ڽ��� ���ٸ�?
	if (pChild == NULL)
	{
		this->pFirstChild = pChild;
	}
	else
	{
		while (pChild != NULL)
		{
			//�ڽ��� �����ڽ��� ���ٸ�?
			if (pChild->pNextChild == NULL)
			{
				pChild->pNextChild = child;
				break;
			}
		}

		pChild = pChild->pNextChild;
	}

	//������Ʈ
	this->UpdateTransform();
}

void Transform::AttachToParent(Transform * parent)
{
	//�θ𿡰� �ٴ´�.
	parent->AddChild(this);
}

void Transform::ReleaseParent()
{
	//�θ� �������� ����
	if (this->pParent == NULL) return;

	//�θ�� �ڽİ��� ����
	Transform* pChild = this->pParent->pFirstChild;

	if (pChild == this)
	{
		//�ڽ��� ù��° �ڽ��̸� ���� �ι�° �ڽ��� ù��° �ڽ��� �ȴ�.
		this->pParent->pFirstChild = this->pParent->pNextChild;

		//�ڽ��� �����ڽĵ鵵 ���´�.
		this->pNextChild = NULL;
	}
	else
	{
		while (pChild != NULL)
		{
			//���� �ڽ��� �����ڽ��� �����?
			if (pChild->pNextChild == this)
			{
				//���� �ڽ��� �����ڽ��� ���� �ڽ����� �ѱ��.
				pChild->pNextChild = this->pNextChild;

				//�ڽ��� ���� �ڽĵ��� ���´�.
				this->pNextChild = NULL;

				break;
			}
			//���� �ڽ����� �Ѿ��
			pChild = pChild->pNextChild;
		}
	}
	//�ڽ��� �θ�� ���� ����
	this->pParent = NULL;

	//��ġ ����
	this->Position.x = this->MatWorld._41;
	this->Position.y = this->MatWorld._42;
	this->Position.z = this->MatWorld._43;

	//�� ����
	D3DXVECTOR3 ScaleRight(this->MatWorld._11, this->MatWorld._12, this->MatWorld._13);
	D3DXVECTOR3 ScaleUp(this->MatWorld._21, this->MatWorld._22, this->MatWorld._23);
	D3DXVECTOR3 ScaleForward(this->MatWorld._31, this->MatWorld._32, this->MatWorld._33);

	//�࿡�� �������� ����.
	float ScaleX = D3DXVec3Length(&ScaleRight);
	float ScaleY = D3DXVec3Length(&ScaleUp);
	float ScaleZ = D3DXVec3Length(&ScaleForward);

	//�� ����ȭ
	D3DXVECTOR3 NorRight;
	D3DXVECTOR3 NorUp;
	D3DXVECTOR3 NorForward;

	D3DXVec3Normalize(&NorRight, &ScaleRight);
	D3DXVec3Normalize(&NorUp, &ScaleUp);
	D3DXVec3Normalize(&NorForward, &ScaleForward);

	//����ȭ�� �� ����
	this->Right = NorRight;
	this->Up = NorUp;
	this->Forward = NorForward;

	//������ ����
	this->Scale.x = ScaleX;
	this->Scale.y = ScaleY;
	this->Scale.z = ScaleZ;

	//���������� Ʈ������ ������Ʈ
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

	//�ڽ��� �̵��� ��´�.
	D3DXVECTOR3 moveAxis[3];
	this->GetUnitAxies(moveAxis);
	Move += moveAxis[0] * dx;
	Move += moveAxis[1] * dy;
	Move += moveAxis[2] * dz;

	//���� ������������ ���ؿ´�.
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//��ġ ����
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionSelf(D3DXVECTOR3 delta)
{
	D3DXVECTOR3 Move(0, 0, 0);

	//�ڽ��� �̵��� ��´�.
	D3DXVECTOR3 moveAxis[3];
	this->GetUnitAxies(moveAxis);
	Move += moveAxis[0] * delta.x;
	Move += moveAxis[1] * delta.y;
	Move += moveAxis[2] * delta.z;

	//���� ������������ ���ؿ´�.
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//��ġ ����
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionWorld(float dx, float dy, float dz)
{
	//�̵���
	D3DXVECTOR3 Move(dx, dy, dz);

	//���� �����ǰ� ������
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//�̵� ����
	this->SetWorldPosition(CurWorldPos + Move);
}

void Transform::MovePositionWorld(D3DXVECTOR3 delta)
{
	//���� �����ǰ� ������
	D3DXVECTOR3 CurWorldPos = this->GetWorldPosition();

	//�̵� ����
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
	//���������� �̵�
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
	//�������� x,y,z����ŭ �ø���.
	this->Scale.x += dx;
	this->Scale.x += dy;
	this->Scale.x += dz;

	this->UpdateTransform();
}

void Transform::Scaling(D3DXVECTOR3 delta)
{
	//�������� delta����ŭ �ø���.
	this->Scale += delta;

	this->UpdateTransform();
}

void Transform::RotateWorld(float angleX, float angleY, float angleZ)
{
	if (this->pParent) //�θ� �ִ� ���
	{
		D3DXVECTOR3 worldAxis[3];
		this->GetUnitAxies(worldAxis);

		//�� �࿡ ���� ȸ�����
		D3DXMATRIXA16 matRotateX;
		D3DXMatrixRotationX(&matRotateX, angleX);

		D3DXMATRIXA16 matRotateY;
		D3DXMatrixRotationY(&matRotateY, angleY);

		D3DXMATRIXA16 matRotateZ;
		D3DXMatrixRotationZ(&matRotateZ, angleZ);

		D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

		//�θ� �����
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;

		for (int i = 0; i < 3; ++i)
		{
			D3DXVec3TransformNormal(&this->Axis[i], &worldAxis[i], &matRotate);
		}

		this->UpdateTransform();
	}
	else //�θ� ���� ���
	{
		RotateLocal(angleX, angleY, angleZ);
	}
}

void Transform::RotateWorld(D3DXVECTOR3 angle)
{
	if (this->pParent) //�θ� �ִ� ���
	{
		D3DXVECTOR3 worldAxis[3];
		this->GetUnitAxies(worldAxis);

		//�� �࿡ ���� ȸ�����
		D3DXMATRIXA16 matRotateX;
		D3DXMatrixRotationX(&matRotateX, angle.x);

		D3DXMATRIXA16 matRotateY;
		D3DXMatrixRotationY(&matRotateY, angle.y);

		D3DXMATRIXA16 matRotateZ;
		D3DXMatrixRotationZ(&matRotateZ, angle.z);

		D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

		//�θ� �����
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;

		for (int i = 0; i < 3; ++i)
		{
			D3DXVec3TransformNormal(&this->Axis[i], &worldAxis[i], &matRotate);
		}

		this->UpdateTransform();
	}
	else //�θ� ���� ���
	{
		RotateLocal(angle);
	}
}

void Transform::RotateSelf(float angleX, float angleY, float angleZ)
{
	//�� �࿡ ���� ȸ�����
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->GetRight(), angleX);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->GetUp(), angleY);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->GetForward(), angleZ);

	//���� ȸ�� ���
	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::RotateSelf(D3DXVECTOR3 angle)
{
	//�� �࿡ ���� ȸ�����
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->GetRight(), angle.x);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->GetUp(), angle.y);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->GetForward(), angle.z);

	//���� ȸ�� ���
	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::RotateLocal(float angleX, float angleY, float angleZ)
{
	//�� �࿡ ���� ȸ�����
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationX(&matRotateX, angleX);

	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationY(&matRotateY, angleY);

	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationZ(&matRotateZ, angleZ);

	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	//ȸ����� �� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
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

	//ȸ����� �� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateWorld(float eAngleX, float eAngleY, float eAngleZ)
{
	//����� �غ�
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, eAngleY, eAngleX, eAngleZ);

	//������� ȸ����ķ� ��ȯ�Ѵ�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//�θ� ���� ���
	if (this->pParent)
	{
		//�θ��� �����
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateLocal(float eAngleX, float eAngleY, float eAngleZ)
{
	//����� �غ�
	D3DXQUATERNION quatRot;
	D3DXQuaternionRotationYawPitchRoll(&quatRot, eAngleY, eAngleX, eAngleZ);

	//������� ȸ����ķ� ��ȯ�Ѵ�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateWorld(const D3DXMATRIXA16 & matWorldRotate)
{
	D3DXMATRIXA16 matRotate = matWorldRotate;

	//�θ� ���� ���
	if (this->pParent)
	{
		//�θ��� �����
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateLocal(const D3DXMATRIXA16 & matWorldRotate)
{
	D3DXMATRIXA16 matRotate = matWorldRotate;
	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateWorld(const D3DXQUATERNION & QuatWorldRotate)
{
	//������� ȸ����ķ� ��ȯ�Ѵ�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &QuatWorldRotate);

	//�θ� ���� ���
	if (this->pParent)
	{
		//�θ��� �����
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		matRotate = matRotate * MatInvWorld;
	}

	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::SetRotateLocal(const D3DXQUATERNION & QuatWorldRotate)
{
	//������� ȸ����ķ� ��ȯ�Ѵ�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &QuatWorldRotate);

	//�� ����
	this->Right = D3DXVECTOR3(1, 0, 0);
	this->Up = D3DXVECTOR3(0, 1, 0);
	this->Forward = D3DXVECTOR3(0, 0, 1);

	//ȸ����İ� ���Ϳ� ����
	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&this->Axis[i], &this->Axis[i], &matRotate);
	}
	//������Ʈ
	this->UpdateTransform();
}

void Transform::LookDirection(D3DXVECTOR3 dir, D3DXVECTOR3 up)
{
	//���麤��
	D3DXVECTOR3 newForward = dir;

	//��������
	D3DXVECTOR3 newRight;
	D3DXVec3Cross(&newRight, &up, &newForward);
	D3DXVec3Normalize(&newRight, &newRight);

	//�� ����
	D3DXVECTOR3 newUp;
	D3DXVec3Cross(&newUp, &newForward, &newRight);
	D3DXVec3Normalize(&newUp, &newUp);

	if (this->pParent)
	{
		//�θ� �����
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
	//������
	D3DXVECTOR3 WorldAxis[3];
	this->GetUnitAxies(WorldAxis);

	//���麤�Ϳ� �ٶ� ���⺤���� ������
	float distRadian = acos(ClampMinusOnePlusOne(D3DXVec3Dot(&WorldAxis[AXIS_Z], &dir)));

	//���麤�Ϳ� �ٶ� ���⺤�Ͱ� ���̰� ���ٸ� ����
	if (FLOATZERO(distRadian)) return;

	//���麤�Ϳ� ���⺤�͸� ������
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &WorldAxis[AXIS_Z], &dir);
	D3DXVec3Normalize(&cross, &cross);

	//���������� ������ŭ ȸ����Ű�� ���
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationAxis(&matRotate, &cross, min(angle, distRadian));

	//���� �θ� �ִٸ�?
	if (this->pParent)
	{
		//�θ��� �����
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

	//���⺤�� ���ϱ�
	D3DXVECTOR3 Dir = pos - WorldPos;
	D3DXVec3Normalize(&Dir, &Dir);

	//���� ���⺤�ͷ� ȸ��
	LookDirection(Dir, up);
}

void Transform::LookPosition(D3DXVECTOR3 pos, float angle)
{
	D3DXVECTOR3 WorldPos = this->GetWorldPosition();

	//���⺤�� ���ϱ�
	D3DXVECTOR3 Dir = pos - WorldPos;
	D3DXVec3Normalize(&Dir, &Dir);

	//���� ���⺤�ͷ� ȸ��
	LookDirection(Dir, angle);
}

void Transform::RotateSlerp(const Transform & from, const Transform & to, float t)
{
	//0�� 1������ ���� ���Ѵ�.
	t = Clamp01(t);

	D3DXQUATERNION fromQuat = from.GetWorldRotateQuaternion();
	D3DXQUATERNION toQuat = to.GetWorldRotateQuaternion();

	//0�� ������ from ���ʹϾ��� ȸ���� ����
	if (FLOATZERO(t))
	{
		this->SetRotateWorld(fromQuat);
	}
	else if (FLOATEQUAL(t, 1.0f)) //1�� ������ to ���ʹϾ��� ȸ���� ����
	{
		this->SetRotateWorld(toQuat);
	}
	else // 0�� 1�� �ƴҶ��� �ΰ��� ���ʹϾ��� ������
	{
		D3DXQUATERNION result;

		D3DXQuaternionSlerp(&result, &fromQuat, &toQuat, t);

		this->SetRotateWorld(result);
	}
}

void Transform::PositionLerp(const Transform & from, const Transform & to, float t)
{
	//0�� 1������ ���� ���Ѵ�.
	t = Clamp01(t);

	D3DXVECTOR3 fromPos = from.GetWorldPosition();
	D3DXVECTOR3 toPos = to.GetWorldPosition();

	//0�� ���� �� fromPos�� ���� ���������� ����
	if (FLOATZERO(t))
	{
		this->SetWorldPosition(fromPos);
	}
	else if (FLOATEQUAL(t, 1.0f)) //1�� ���� �� toPos�� ���� ���������� ����
	{
		this->SetWorldPosition(toPos);
	}
	else // 0�� 1�� �ƴҶ��� �ΰ��� ���͸� ������ ����
	{
		D3DXVECTOR3 result;
		D3DXVec3Lerp(&result, &fromPos, &toPos, t);

		this->SetWorldPosition(result);
	}
}

void Transform::Interpolate(const Transform & from, const Transform & to, float t)
{
	//0�� 1������ ���� ���Ѵ�.
	t = Clamp01(t);

	D3DXVECTOR3 resultScale;
	D3DXVECTOR3 resultPos;
	D3DXQUATERNION resultRot;

	if (FLOATZERO(t)) //0�� ���� �� from Transform ����
	{
		resultScale = from.Scale;
		resultPos = from.GetWorldPosition();
		resultRot = from.GetWorldRotateQuaternion();
	}
	else if (FLOATEQUAL(t, 1.0f)) //0�� ���� �� to Transform ����
	{
		resultScale = to.Scale;
		resultPos = to.GetWorldPosition();
		resultRot = to.GetWorldRotateQuaternion();
	}
	else // 0�� 1�� �ƴҶ��� from�� to �������� ������ ����
	{
		//������
		D3DXVECTOR3 fromScale = from.Scale;
		D3DXVECTOR3 toScale = to.Scale;

		//��ġ
		D3DXVECTOR3 fromPos = from.GetWorldPosition();
		D3DXVECTOR3 toPos = to.GetWorldPosition();

		//ȸ��
		D3DXQUATERNION fromRot = from.GetWorldRotateQuaternion();
		D3DXQUATERNION toRot = to.GetWorldRotateQuaternion();

		//������ from, to �������� �����Ѵ�.
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
	//������� ����
	this->MatWorld = matWorld;
	this->MatLocal = matWorld;

	//������ġ 
	this->Position = D3DXVECTOR3(MatWorld._41, MatWorld._42, MatWorld._43);

	//�຤�� ���ϱ�
	D3DXVECTOR3 forward = D3DXVECTOR3(MatWorld._31, MatWorld._32, MatWorld._33);
	D3DXVECTOR3 up = D3DXVECTOR3(MatWorld._21, MatWorld._22, MatWorld._23);
	D3DXVECTOR3 right = D3DXVECTOR3(MatWorld._11, MatWorld._12, MatWorld._13);

	//����ȭ
	D3DXVec3Normalize(&this->Forward, &forward);
	D3DXVec3Normalize(&this->Up, &up);
	D3DXVec3Normalize(&this->Right, &right);

	//������ ���̷� �������� �ش�.
	this->Scale.x = D3DXVec3Length(&right);
	this->Scale.y = D3DXVec3Length(&up);
	this->Scale.z = D3DXVec3Length(&forward);

	//�θ� �ִٸ�?
	if (this->pParent)
	{
		//�θ��� ����� ���ϱ�
		D3DXMATRIXA16 MatInvWorld;
		D3DXMatrixInverse(&MatInvWorld, NULL, &this->pParent->MatWorld);

		this->MatLocal = this->MatWorld * MatInvWorld;

		//���� ���� ��ġ
		memcpy(&this->Position, &this->MatLocal._41, sizeof(D3DXVECTOR3));

		//���� �� ����
		D3DXVECTOR3 forward;
		memcpy(&forward, &this->MatLocal._31, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 up;
		memcpy(&up, &this->MatLocal._21, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 right;
		memcpy(&right, &this->MatLocal._11, sizeof(D3DXVECTOR3));

		//�� ������
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

	//�ڽĵ� ������Ʈ
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

	//������ ����� ������
	D3DXVECTOR3 ScaleRight = this->Right * this->Scale.x;
	D3DXVECTOR3 ScaleUp = this->Up * this->Scale.y;
	D3DXVECTOR3 ScaleForward = this->Forward * this->Scale.z;

	//��, ��ġ������ ���ÿ� �����Ѵ�.
	memcpy(&this->MatLocal._11, &ScaleRight, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._21, &ScaleUp, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._31, &ScaleForward, sizeof(D3DXVECTOR3));
	memcpy(&this->MatLocal._41, &this->Position, sizeof(D3DXVECTOR3));

	if (this->pParent == NULL) // �θ� ���� ���
	{
		this->MatWorld = this->MatLocal;
	}
	else // �θ� ���� ���
	{
		this->MatWorld = this->MatLocal * this->pParent->MatWorld;
	}

	//�ڽ������� ������Ʈ�Ѵ�.
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
	//�ڽ��� ������ ȸ������� �����.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixIdentity(&matRotate);

	//3���� ��´�.
	D3DXVECTOR3 WorldAxis[3];
	this->GetUnitAxies(WorldAxis);

	//��Ŀ� �຤���� ���� �����Ѵ�.
	memcpy(&matRotate._11, WorldAxis[0], sizeof(D3DXVECTOR3));
	memcpy(&matRotate._21, WorldAxis[1], sizeof(D3DXVECTOR3));
	memcpy(&matRotate._31, WorldAxis[2], sizeof(D3DXVECTOR3));

	return matRotate;
}

D3DXQUATERNION Transform::GetWorldRotateQuaternion() const
{
	D3DXQUATERNION Quat;

	D3DXMATRIXA16 matRotate = GetWorldRotateMatrix();

	//ȸ����ķ� ������� �����.
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

	//�θ� ���� ���
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

	//�θ� ���� ���
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

