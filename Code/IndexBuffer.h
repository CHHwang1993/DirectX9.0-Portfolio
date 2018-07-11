#pragma once
class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();
	
	void CreateIndexBuffer(UINT datasize,D3DFORMAT format=D3DFMT_INDEX16);
	void LockAndCopyIndexBuffer(void* data);

	LPDIRECT3DINDEXBUFFER9 Buffer() { return indexBuffer; }
	UINT DataSize() { return dataSize; }

private:
	LPDIRECT3DINDEXBUFFER9 indexBuffer;

	D3DFORMAT format;
	void* dataArray;
	UINT dataSize;
};

