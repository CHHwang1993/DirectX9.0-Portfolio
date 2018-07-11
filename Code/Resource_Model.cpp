#include "stdafx.h"
#include "Resource_Model.h"


Resource_Model::Resource_Model()
{
}


Resource_Model::~Resource_Model()
{
}

LPD3DXMESH Resource_Model::Load(std::string fullPath)
{
	LPD3DXMESH ret = NULL;

	if (FAILED(D3DXLoadMeshFromX(fullPath.c_str(), D3DXMESH_SYSTEMMEM, g_pD3DDevice, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString("모델 로딩 실패");
		OutputDebugString(fullPath.c_str());
		OutputDebugString("\n");
	}

	assert(ret != NULL);

	return ret;
}

void Resource_Model::ReleaseResource(LPD3DXMESH data)
{
	SAFE_RELEASE(data);
}
