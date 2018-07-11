#include "stdafx.h"
#include "Resource_TextureCUBE.h"


Resource_TextureCUBE::Resource_TextureCUBE()
{
}


Resource_TextureCUBE::~Resource_TextureCUBE()
{
}

LPDIRECT3DCUBETEXTURE9 Resource_TextureCUBE::Load(std::string fullPath)
{
	HRESULT hr;

	LPDIRECT3DCUBETEXTURE9 tex = NULL;

	hr = D3DXCreateCubeTextureFromFile(g_pD3DDevice, fullPath.c_str(), &tex);
	assert(SUCCEEDED(hr));

	return tex;
}

void Resource_TextureCUBE::ReleaseResource(LPDIRECT3DCUBETEXTURE9 data)
{
	SAFE_RELEASE(data);
}

