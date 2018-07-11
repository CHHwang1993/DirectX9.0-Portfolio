#include "stdafx.h"
#include "Resource_Shader.h"


Effect* Resource_Shader::Load(std::string fullPath)
{
	Effect* effect = new Effect();
	HRESULT hr = effect->LoadEffect(fullPath);
	assert(hr == S_OK);

	return effect;
}

void Resource_Shader::ReleaseResource(Effect * data)
{
	SAFE_DELETE(data);
}
