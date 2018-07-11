#pragma once

template<typename T>
class SingletonBase
{
protected:
	static T* instance;
protected:
	SingletonBase() {};
	~SingletonBase() {};

public:

	//�ν��Ͻ� ��� �Լ�
	static T* Get();
	//�ν��Ͻ� ���� �Լ�
	static void ReleaseInstance();
};

template<typename T>
T* SingletonBase<T>::instance = NULL;

template<typename T>
T * SingletonBase<T>::Get()
{
	if (instance == NULL)
		instance = new T();

	return instance;
}

template<typename T>
void SingletonBase<T>::ReleaseInstance()
{
	SAFE_DELETE(instance);
}
