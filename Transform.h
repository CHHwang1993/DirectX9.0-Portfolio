#pragma once

//ȸ����
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

class Transform
{
protected:
	union
	{
		struct
		{
			D3DXVECTOR3 Axis[3]; // X,Y,Z ��
		};

		struct
		{
			D3DXVECTOR3 Right; // ���� ���� ����
			D3DXVECTOR3 Up; // �� ���� ����
			D3DXVECTOR3 Forward; // ���� ���� ����
		};
	};

	D3DXVECTOR3 Position; // ��ġ
	D3DXVECTOR3 Scale; // ������

	D3DXMATRIXA16 MatLocal; // ���� ���
	D3DXMATRIXA16 MatWorld; // ���� ���

	Transform* pParent; // �θ�
	Transform* pFirstChild; // �ֻ��� �ڽ�
	Transform* pNextChild; // ���� �ڽ�

public:
	Transform();
	virtual ~Transform();

	//�ʱ�ȭ �� �θ� ����
	void Reset(); // Ʈ������ ������ ����
	void AddChild(Transform* child); //�ڽ��� ���δ�
	void AttachToParent(Transform* parent); //�θ� ���δ�.
	void ReleaseParent(); //����� �θ� ����


						  ///////////////////////////// ��ġ ���� ////////////////////////////////////////

						  //��ġ�� ������ǥ��� ����
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(D3DXVECTOR3 position);

	//��ġ�� ������ǥ��� ����
	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(D3DXVECTOR3 position);

	//�ڽ��� ��������� �̵� ��Ų��.
	void MovePositionSelf(float dx, float dy, float dz);
	void MovePositionSelf(D3DXVECTOR3 delta);

	//���� �������� �̵� ��Ų��.
	void MovePositionWorld(float dx, float dy, float dz);
	void MovePositionWorld(D3DXVECTOR3 delta);

	//�θ� �ִ� ��� ���� �������� �̵� ��Ų��.
	void MovePositionLocal(float dx, float dy, float dz);
	void MovePositionLocal(D3DXVECTOR3 delta);

	///////////////////////////// ������ ���� ////////////////////////////////////////

	void SetScale(float x, float y, float z);
	void SetScale(D3DXVECTOR3 scale);
	void Scaling(float dx, float dy, float dz);
	void Scaling(D3DXVECTOR3 delta);


	///////////////////////////// ȸ�� ���� //////////////////////////////////////////

	//���� �������� ȸ�� 
	void RotateWorld(float angleX, float angleY, float angleZ);
	void RotateWorld(D3DXVECTOR3 angle);

	//�ڽ��� ��������� ȸ��
	void RotateSelf(float angleX, float angleY, float angleZ);
	void RotateSelf(D3DXVECTOR3 angle);

	//�θ� �ִ� ��� �θ� ������ ��������� ȸ��
	void RotateLocal(float angleX, float angleY, float angleZ);
	void RotateLocal(D3DXVECTOR3 angle);

	//������� �̿��� Ư�� ȸ�������� ȸ������ ������....
	void SetRotateWorld(float eAngleX, float eAngleY, float eAngleZ);
	void SetRotateLocal(float eAngleX, float eAngleY, float eAngleZ);

	// ȸ�� ����� �־��ָ� �� ȸ�� ��Ĵ�� ȸ���Ѵ�.
	void SetRotateWorld(const D3DXMATRIXA16& matWorldRotate);
	void SetRotateLocal(const D3DXMATRIXA16& matWorldRotate);

	// ȸ�� ������� �־��ָ� �� ȸ���� ��� ȸ���Ѵ�.
	void SetRotateWorld(const D3DXQUATERNION& QuatWorldRotate);
	void SetRotateLocal(const D3DXQUATERNION& QuatWorldRotate);

	//Ư�� ������ �ٶ󺸰� ȸ���ض�.
	void LookDirection(D3DXVECTOR3 dir, D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0));

	//Ư�� ������ �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض�
	void LookDirection(D3DXVECTOR3 dir, float angle);

	//Ư����ġ�� �ٶ󺸰� ȸ���ض�.
	void LookPosition(D3DXVECTOR3 pos, D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0));

	//Ư����ġ��  �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض�
	void LookPosition(D3DXVECTOR3 pos, float angle);


	//////////////////////////////// ���� /////////////////////////////////////////////

	// �ڽ��� ȸ�� ���� from �� to ������ ȸ������ŭ ȸ������(���麸��) �Ͽ� ����
	void RotateSlerp(const Transform& from, const Transform& to, float t);

	// �ڽ��� ��ġ ���� from �� to ������ ��ġ��ŭ �������� �Ͽ� ����
	void PositionLerp(const Transform& from, const Transform& to, float t);

	// �ڽ��� ��� ������ from �� to ������ ������ŭ ���� �Ͽ� ����
	void Interpolate(const Transform& from, const Transform& to, float t);


	//////////////////////////// ���� �ϴ°͵� (Set) //////////////////////////////////
	void SetWorldMatrix(const D3DXMATRIXA16& matWorld);

	//////////////////////////// ������Ʈ ////////////////////////////////////////////
	void UpdateTransform(); //Transform ������ �����Ѵ�.

	void SetDeviceWorld(LPDIRECT3DDEVICE9 pDevice); //Device�� �ڽ��� Transform�� ����� ����
	void SetDeviceView(LPDIRECT3DDEVICE9 pDevice); //Device�� �ڽ��� Transform�� ��� ����

												   //////////////////////////// �����°͵� (Get) ///////////////////////////////////

												   //��������� ��´�.
	D3DXMATRIXA16 GetFinalMatrix() const;
	D3DXMATRIXA16 GetWorldRotateMatrix() const;
	D3DXQUATERNION GetWorldRotateQuaternion() const;

	//��ġ���� ��´�.
	D3DXVECTOR3 GetWorldPosition() const;
	D3DXVECTOR3 GetLocalPosition() const;

	//���� ��´�.
	void GetUnitAxies(D3DXVECTOR3* pVecArr) const;
	void GetScaledAxies(D3DXVECTOR3* pVecArr) const;
	D3DXVECTOR3 GetUnitAxis(int axis) const;
	D3DXVECTOR3 GetScaledAxis(int axis) const;
	D3DXVECTOR3 GetForward(bool bNormalize = true) const;
	D3DXVECTOR3 GetUp(bool bNormalize = true) const;
	D3DXVECTOR3 GetRight(bool bNormalize = true) const;

	////////////////////////// CameraDefault ///////////////////////////////////////////
};



