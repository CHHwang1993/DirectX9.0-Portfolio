#include "stdafx.h"
#include "VertexBuffer.h"


VertexBuffer::VertexBuffer()
	: vertexBuffer(NULL), dataArray(NULL), dataSize(0)
{
}


VertexBuffer::~VertexBuffer()
{
	SAFE_RELEASE(vertexBuffer);
}

void VertexBuffer::CreateVertexBuffer(UINT datasize, DWORD fvf)
{
	assert(vertexBuffer == NULL);
	assert(datasize > 0);
	this->dataSize = datasize;

	HRESULT hr = g_pD3DDevice->CreateVertexBuffer(datasize, 0, fvf, D3DPOOL_DEFAULT, &vertexBuffer, NULL);
	assert(hr == S_OK);
	assert(vertexBuffer != NULL);
}

void VertexBuffer::LockAndCopyVertexBuffer(void * data)
{
	assert(vertexBuffer != NULL);
	this->dataArray = data;

	void* vertex = NULL;
	HRESULT hr = vertexBuffer->Lock(0, 0, (void**)&vertex, 0);
	assert(hr == S_OK);

	memcpy(vertex, this->dataArray, this->dataSize);

	hr = vertexBuffer->Unlock();
	assert(hr == S_OK);
}


