#pragma once

//회전축
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
			D3DXVECTOR3 Axis[3]; // X,Y,Z 축
		};

		struct
		{
			D3DXVECTOR3 Right; // 우측 방향 벡터
			D3DXVECTOR3 Up; // 위 방향 벡터
			D3DXVECTOR3 Forward; // 전방 방향 벡터
		};
	};

	D3DXVECTOR3 Position; // 위치
	D3DXVECTOR3 Scale; // 스케일

	D3DXMATRIXA16 MatLocal; // 로컬 행렬
	D3DXMATRIXA16 MatWorld; // 월드 행렬

	Transform* pParent; // 부모
	Transform* pFirstChild; // 최상위 자식
	Transform* pNextChild; // 다음 자식

public:
	Transform();
	virtual ~Transform();

	//초기화 및 부모 설정
	void Reset(); // 트랜스폼 정보를 리셋
	void AddChild(Transform* child); //자식을 붙인다
	void AttachToParent(Transform* parent); //부모를 붙인다.
	void ReleaseParent(); //연결된 부모 해제


						  ///////////////////////////// 위치 세팅 ////////////////////////////////////////

						  //위치를 월드좌표계로 세팅
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(D3DXVECTOR3 position);

	//위치를 로컬좌표계로 세팅
	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(D3DXVECTOR3 position);

	//자신의 축기준으로 이동 시킨다.
	void MovePositionSelf(float dx, float dy, float dz);
	void MovePositionSelf(D3DXVECTOR3 delta);

	//월드 기준으로 이동 시킨다.
	void MovePositionWorld(float dx, float dy, float dz);
	void MovePositionWorld(D3DXVECTOR3 delta);

	//부모가 있는 경우 로컬 기준으로 이동 시킨다.
	void MovePositionLocal(float dx, float dy, float dz);
	void MovePositionLocal(D3DXVECTOR3 delta);

	///////////////////////////// 스케일 세팅 ////////////////////////////////////////

	void SetScale(float x, float y, float z);
	void SetScale(D3DXVECTOR3 scale);
	void Scaling(float dx, float dy, float dz);
	void Scaling(D3DXVECTOR3 delta);


	///////////////////////////// 회전 세팅 //////////////////////////////////////////

	//월드 기준으로 회전 
	void RotateWorld(float angleX, float angleY, float angleZ);
	void RotateWorld(D3DXVECTOR3 angle);

	//자신의 축기준으로 회전
	void RotateSelf(float angleX, float angleY, float angleZ);
	void RotateSelf(D3DXVECTOR3 angle);

	//부모가 있는 경우 부모 로컬의 축기준으로 회전
	void RotateLocal(float angleX, float angleY, float angleZ);
	void RotateLocal(D3DXVECTOR3 angle);

	//사원수를 이용한 특정 회전값으로 회전량을 가져라....
	void SetRotateWorld(float eAngleX, float eAngleY, float eAngleZ);
	void SetRotateLocal(float eAngleX, float eAngleY, float eAngleZ);

	// 회전 행렬을 넣어주면 그 회전 행렬대로 회전한다.
	void SetRotateWorld(const D3DXMATRIXA16& matWorldRotate);
	void SetRotateLocal(const D3DXMATRIXA16& matWorldRotate);

	// 회전 사원수를 넣어주면 그 회전값 대로 회전한다.
	void SetRotateWorld(const D3DXQUATERNION& QuatWorldRotate);
	void SetRotateLocal(const D3DXQUATERNION& QuatWorldRotate);

	//특정 방향을 바라보게 회전해라.
	void LookDirection(D3DXVECTOR3 dir, D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0));

	//특정 방향을 바라보는데 angle 각만큼만 회전 해라
	void LookDirection(D3DXVECTOR3 dir, float angle);

	//특정위치를 바라보게 회전해라.
	void LookPosition(D3DXVECTOR3 pos, D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0));

	//특정위치를  바라보는데 angle 각만큼만 회전 해라
	void LookPosition(D3DXVECTOR3 pos, float angle);


	//////////////////////////////// 보간 /////////////////////////////////////////////

	// 자신의 회전 값을 from 과 to 사이의 회전량만큼 회전보간(구면보간) 하여 적용
	void RotateSlerp(const Transform& from, const Transform& to, float t);

	// 자신의 위치 값을 from 과 to 사이의 위치만큼 선형보간 하여 적용
	void PositionLerp(const Transform& from, const Transform& to, float t);

	// 자신의 모든 정보를 from 과 to 사이의 정보만큼 보간 하여 적용
	void Interpolate(const Transform& from, const Transform& to, float t);


	//////////////////////////// 세팅 하는것들 (Set) //////////////////////////////////
	void SetWorldMatrix(const D3DXMATRIXA16& matWorld);

	//////////////////////////// 업데이트 ////////////////////////////////////////////
	void UpdateTransform(); //Transform 정보를 갱신한다.

	void SetDeviceWorld(LPDIRECT3DDEVICE9 pDevice); //Device에 자신의 Transform을 월드로 세팅
	void SetDeviceView(LPDIRECT3DDEVICE9 pDevice); //Device에 자신의 Transform을 뷰로 세팅

												   //////////////////////////// 얻어오는것들 (Get) ///////////////////////////////////

												   //최종행렬을 얻는다.
	D3DXMATRIXA16 GetFinalMatrix() const;
	D3DXMATRIXA16 GetWorldRotateMatrix() const;
	D3DXQUATERNION GetWorldRotateQuaternion() const;

	//위치값을 얻는다.
	D3DXVECTOR3 GetWorldPosition() const;
	D3DXVECTOR3 GetLocalPosition() const;

	//축을 얻는다.
	void GetUnitAxies(D3DXVECTOR3* pVecArr) const;
	void GetScaledAxies(D3DXVECTOR3* pVecArr) const;
	D3DXVECTOR3 GetUnitAxis(int axis) const;
	D3DXVECTOR3 GetScaledAxis(int axis) const;
	D3DXVECTOR3 GetForward(bool bNormalize = true) const;
	D3DXVECTOR3 GetUp(bool bNormalize = true) const;
	D3DXVECTOR3 GetRight(bool bNormalize = true) const;

	////////////////////////// CameraDefault ///////////////////////////////////////////
};



