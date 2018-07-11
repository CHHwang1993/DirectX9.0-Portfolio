#pragma once

#include "ResourceManager.h"

#define MODEL Resource_Model::Get()

class Resource_Model : public ResourceManager<LPD3DXMESH, Resource_Model>
{
public:
	Resource_Model();
	~Resource_Model();

	virtual LPD3DXMESH Load(std::string fullPath);
	virtual void ReleaseResource(LPD3DXMESH data);
};

