#pragma once
class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();
	void CreateVertexBuffer(UINT datasize,DWORD fvf=NULL);
	void LockAndCopyVertexBuffer(void* data);

	LPDIRECT3DVERTEXBUFFER9 Buffer() { return vertexBuffer; }
	UINT DataSize() { return dataSize; }
private:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

	void* dataArray;
	UINT dataSize;

};

