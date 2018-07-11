#pragma once

#include "ResourceManager.h"

#define SHADER Resource_Shader::Get()

class Resource_Shader : public ResourceManager<Effect*, Resource_Shader>
{
public:
	Resource_Shader() {};
	~Resource_Shader() {};

	virtual Effect* Load(std::string fullPath);
	virtual void ReleaseResource(Effect* data);
};

