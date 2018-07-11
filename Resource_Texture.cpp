#include "stdafx.h"
#include "Resource_Texture.h"


Resource_Texture::Resource_Texture()
{
}


Resource_Texture::~Resource_Texture()
{
}

LPDIRECT3DTEXTURE9 Resource_Texture::Load(std::string fullPath)
{
	HRESULT hr;

	LPDIRECT3DTEXTURE9 tex = NULL;

	hr = D3DXCreateTextureFromFile(g_pD3DDevice, fullPath.c_str(), &tex);
	assert(SUCCEEDED(hr));

	return tex;
}

void Resource_Texture::ReleaseResource(LPDIRECT3DTEXTURE9 data)
{
	SAFE_RELEASE(data);
}
