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

	//인스턴스 얻기 함수
	static T* Get();
	//인스턴스 해제 함수
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
