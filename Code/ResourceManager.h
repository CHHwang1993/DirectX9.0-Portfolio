#pragma once

#include <map>
#include <string>

#include "SingletonBase.h"

template<typename T, typename A>
class ResourceManager : public SingletonBase<A>
{
protected:
	typedef std::map<std::string, T>	 MAP_RESOURCE;

	MAP_RESOURCE m_ResourceMap;

	virtual T Load(std::string fullPath) = 0;
	virtual void ReleaseResource(T data) = 0;

public:
	ResourceManager();
	~ResourceManager();

	//파일경로로 리소스를 불러온다.
	T GetResource(std::string fullPath);

	//특정리소스 해제
	void RemoveResource(std::string fullPath);

	//모든 리소스 해제
	void ClearResource();

};

template<typename T, typename A>
inline ResourceManager<T, A>::ResourceManager()
{
}

template<typename T, typename A>
inline ResourceManager<T, A>::~ResourceManager()
{
	this->ClearResource();
}


template<typename T, typename A>
T ResourceManager<T,A>::GetResource(std::string fullPath)
{
	std::map<std::string, T>::iterator pFind;

	pFind = m_ResourceMap.find(fullPath);

	if (pFind == m_ResourceMap.end())
	{
		T newResource = Load(fullPath);

		if (newResource == NULL) 
			return NULL;

		m_ResourceMap.insert(std::make_pair(fullPath, newResource));

		return newResource;
	}

	return pFind->second;
}

template<typename T, typename A>
void ResourceManager<T,A>::RemoveResource(std::string fullPath)
{
	std::map<std::string, T>::iterator pFind;

	pFind = m_ResourceMap.find(fullPath);

	if (pFind != m_ResourceMap.end())
	{
		ReleaseResource(pFind->second);

		m_ResourceMap.erase(pFind);
	}
}

template<typename T, typename A>
void ResourceManager<T,A>::ClearResource()
{
	std::map<std::string, T>::iterator pIter;

	for (pIter = m_ResourceMap.begin(); pIter != m_ResourceMap.end(); ++pIter)
	{
		ReleaseResource(pIter->second);
	}

	m_ResourceMap.clear();
}


