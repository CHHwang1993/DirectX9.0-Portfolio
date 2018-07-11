#include "stdafx.h"
#include "Effect.h"


Effect::Effect() : internalEffect(NULL), isBegin(false), isBeginPass(false), numPass(0)
{
}


Effect::~Effect()
{
	SAFE_RELEASE(internalEffect);
}

HRESULT Effect::LoadEffect(std::string fullPath)
{
	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags != D3DXSHADER_DEBUG;

#endif

	D3DXCreateEffectFromFile(g_pD3DDevice, fullPath.c_str(), NULL, NULL, dwShaderFlags, NULL, &internalEffect, &pError);

	if (internalEffect == NULL && pError)
	{
		int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
			sprintf_s(str, 256, (const char*)ack, size);
			OutputDebugString(str);
			SAFE_DELETE_ARRAY(str);
		}
	}

	assert(internalEffect != NULL);

	return S_OK;
}

void Effect::Begin()
{
	assert(internalEffect != NULL);
	assert(isBegin == false);

	numPass = 0;

	HRESULT hr = internalEffect->Begin(&numPass, NULL);
	assert(hr == S_OK);

	isBegin = true;
}

void Effect::End()
{
	assert(internalEffect != NULL);
	assert(isBegin == true);

	HRESULT hr = internalEffect->End();
	assert(hr == S_OK);

	isBegin = false;
}

void Effect::BeginPass(int index)
{
	assert(internalEffect != NULL);
	assert(isBegin == true && isBeginPass==false);

	HRESULT hr = internalEffect->BeginPass(index);
	assert(hr == S_OK);

	isBeginPass = true;
}

void Effect::EndPass()
{
	assert(internalEffect != NULL);
	assert(isBegin == true && isBeginPass == true);

	HRESULT hr = internalEffect->EndPass();
	assert(hr == S_OK);

	isBeginPass = false;
}

void Effect::CommitChange()
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->CommitChanges();
	assert(hr == S_OK);
}

void Effect::SetInt(std::string name, int value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetInt(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetBool(std::string name, bool value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetBool(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetFloat(std::string name, float value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetFloat(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetVector2(std::string name, D3DXVECTOR2 value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetValue(name.c_str(), value, sizeof(D3DXVECTOR2));

	assert(hr != S_FALSE);
}

void Effect::SetVector3(std::string name, D3DXVECTOR3 value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetValue(name.c_str(), value, sizeof(D3DXVECTOR3));

	assert(hr != S_FALSE);
}

void Effect::SetVector4(std::string name, D3DXVECTOR4 value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetVector(name.c_str(), &value);

	assert(hr == S_OK);
}

void Effect::SetMatrix(std::string name, D3DXMATRIXA16* value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetMatrix(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetString(std::string name, std::string value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetString(name.c_str(), (char*)value.c_str());

	assert(hr == S_OK);
}

void Effect::SetTexture(std::string name, LPDIRECT3DTEXTURE9 value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetTexture(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetTexture(std::string name, LPDIRECT3DCUBETEXTURE9 value)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetTexture(name.c_str(), value);

	assert(hr == S_OK);
}

void Effect::SetValue(std::string name, void * value, UINT size)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetValue(name.c_str(), value, size);

	assert(hr == S_OK);
}

void Effect::SetTechnique(std::string name)
{
	assert(internalEffect != NULL);

	HRESULT hr = internalEffect->SetTechnique(name.c_str());

	assert(hr == S_OK);
}
