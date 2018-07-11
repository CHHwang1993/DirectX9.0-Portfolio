#pragma once
#include "ResourceManager.h"

#define TEXTURECUBE Resource_TextureCUBE::Get()

class Resource_TextureCUBE : public ResourceManager<LPDIRECT3DCUBETEXTURE9, Resource_TextureCUBE>
{
public:
	Resource_TextureCUBE();
	~Resource_TextureCUBE();
	virtual LPDIRECT3DCUBETEXTURE9 Load(std::string fullPath);
	virtual void ReleaseResource(LPDIRECT3DCUBETEXTURE9 data);
};

