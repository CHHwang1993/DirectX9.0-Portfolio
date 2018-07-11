#pragma once

#include "ResourceManager.h"

#define TEXTURE2D Resource_Texture::Get()

class Resource_Texture : public ResourceManager<LPDIRECT3DTEXTURE9, Resource_Texture>
{
public:
	Resource_Texture();
	~Resource_Texture();

	virtual LPDIRECT3DTEXTURE9 Load(std::string fullPath);
	virtual void ReleaseResource(LPDIRECT3DTEXTURE9 data);
};

