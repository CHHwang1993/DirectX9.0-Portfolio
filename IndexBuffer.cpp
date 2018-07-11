#include "stdafx.h"
#include "IndexBuffer.h"


IndexBuffer::IndexBuffer()
{
}
IndexBuffer::~IndexBuffer()
{
	SAFE_RELEASE(indexBuffer);
}

void IndexBuffer::CreateIndexBuffer(UINT datasize, D3DFORMAT format)
{
	assert(indexBuffer == NULL);
	assert(datasize > 0);
	this->dataSize = datasize;
	this->format = format;

	HRESULT hr;

	hr = g_pD3DDevice->CreateIndexBuffer(datasize, 0, format, D3DPOOL_DEFAULT, &indexBuffer, NULL);
	assert(hr == S_OK);
	assert(indexBuffer != NULL);
}

void IndexBuffer::LockAndCopyIndexBuffer(void * data)
{
	assert(data != NULL);
	this->dataArray = data;

	UINT* index = NULL;
	HRESULT hr = indexBuffer->Lock(0, 0, (void**)&index, 0);
	assert(hr == S_OK);

	memcpy(index, this->dataArray, this->dataSize);

	hr = indexBuffer->Unlock();
	assert(hr == S_OK);
}
