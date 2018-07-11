#include "StructMain.h"
#include "stdafx.h"
#include "util.h"

namespace MY_UTIL
{
	bool IsInRect(RECT * rc, POINT pt)
	{
		if ((rc->left <= pt.x) && (pt.x <= rc->right) &&
			(rc->bottom <= pt.y) && (pt.y <= rc->top))
			return TRUE;

		return FALSE;
	}

	Ray CalcPickingRay(int x, int y)
	{
		float px = 0.0f;
		float py = 0.0f;

		D3DVIEWPORT9 vp;
		g_pD3DDevice->GetViewport(&vp);

		D3DXMATRIXA16 proj;
		g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);

		px = (((2.0f*x) / vp.Width) - 1.0f) / proj._11;
		py = (((-2.0f*y) / vp.Height) + 1.0f) / proj._22;

		Ray ray;
		ray.Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		ray.Dir = D3DXVECTOR3(px, py, 1.0f);

		return ray;
	}

	Ray TransformRay(int x, int y)
	{
		Ray ray = CalcPickingRay(x, y);

		D3DXMATRIXA16 view;
		g_pD3DDevice->GetTransform(D3DTS_VIEW, &view);

		D3DXMATRIXA16 viewInverse;
		D3DXMatrixInverse(&viewInverse, NULL, &view);

		D3DXVec3TransformCoord(&ray.Pos, &ray.Pos, &viewInverse);

		D3DXVec3TransformNormal(&ray.Dir, &ray.Dir, &viewInverse);

		D3DXVec3Normalize(&ray.Dir, &ray.Dir);

		return ray;
	}

	bool rayShpereIntersection(Ray* ray, D3DXVECTOR3& Pos, float Radius)
	{
		D3DXVECTOR3 v = ray->Pos - Pos;

		float b = 2.0f*D3DXVec3Dot(&ray->Dir, &v);
		float c = D3DXVec3Dot(&v, &v) - (Radius*Radius);

		float dicriminant = (b*b) - (4.0f*c);

		if (dicriminant < 0.0f)
			return false;

		dicriminant = sqrtf(dicriminant);

		float s0 = (-b + dicriminant) / 2.0f;
		float s1 = (-b - dicriminant) / 2.0f;

		if (s0 >= 0.0f || s1 >= 0.0f)
		{
			return true;
		}
		return false;

	}

	bool IntersectTri(IN Ray * ray, IN D3DXVECTOR3 & v0, IN D3DXVECTOR3 & v1, IN D3DXVECTOR3 & v2, OUT D3DXVECTOR3 & vPickedPosition)
	{
		float u, v, t;
		bool b = D3DXIntersectTri(&v0, &v1, &v2, &ray->Pos, &ray->Dir, &u, &v, &t);
		vPickedPosition = ray->Pos + (t*ray->Dir);

		return b;
	}



	//거리 구한다
	float getDistance(float startX, float startY, float endX, float endY)
	{
		float dx = endX - startX;
		float dy = endY - startY;

		return sqrt(dx * dx + dy * dy);
	}

	//시작점부터 끝 점까지의 각을 라디안으로 구한다
	float getAngle(float startX, float startY, float endX, float endY)
	{
		float x = endX - startX;
		float y = endY - startY;

		float d = sqrt(x * x + y * y);

		float angle = acos(x / d);
		if (y > 0) angle = PI2 - angle;
		return angle;
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;

		return value;
	}

	float Clamp01(float value)
	{
		if (value < 0.0f)
			return 0.0f;
		else if (value > 1.0f)
			return 1.0f;

		return value;
	}

	float ClampMinusOnePlusOne(float value)
	{
		if (value < -1.0f)
			return -1.0f;
		else if (value > 1.0f)
			return 1.0f;

		return value;
	}

	float Square(float value)
	{
		return (value*value);
	}

	float FloatEqual(float a, float b)
	{
		return abs(a-b) < FEPSILON;
	}

	void Swap(float& a, float& b)
	{
		float temp(a);
		a = b;
		b = temp;
	}

	DWORD F2DW(float f)
	{
		return *((DWORD*)&f);
	}

	void CalculateNormal(D3DXVECTOR3 * pNormal, const D3DXVECTOR3 * pVertices, int verticeNum, const DWORD * pIndices, int indicesNum)
	{
		ZeroMemory(pNormal, sizeof(D3DXVECTOR3)*verticeNum);

		DWORD triNum = indicesNum / 3;

		for (DWORD i = 0; i < triNum; ++i)
		{
			//해당삼각형의 인덱스
			DWORD i0 = pIndices[(i * 3) + 0];
			DWORD i1 = pIndices[(i * 3) + 1];
			DWORD i2 = pIndices[(i * 3) + 2];

			//정점 3개
			D3DXVECTOR3 v0 = pVertices[i0];
			D3DXVECTOR3 v1 = pVertices[i1];
			D3DXVECTOR3 v2 = pVertices[i2];

			//모서리
			D3DXVECTOR3 edge1 = v1 - v0;
			D3DXVECTOR3 edge2 = v2 - v0;

			//cross
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &edge1, &edge2);

			D3DXVECTOR3 normal;
			D3DXVec3Normalize(&normal, &cross);

			pNormal[i0] += normal;
			pNormal[i1] += normal;
			pNormal[i2] += normal;
		}
		for (DWORD i = 0; i < verticeNum; i++)
		{
			D3DXVec3Normalize(&pNormal[i], &pNormal[i]);
		}
	}
	void CalculateTangentBinormal(D3DXVECTOR3 * pOutTangent, D3DXVECTOR3 * pOutBinormal, const D3DXVECTOR3 * pPositions, 
		const D3DXVECTOR3 * pNormal, const D3DXVECTOR2 * pUVs, const DWORD * pIndices, DWORD NumTris, DWORD NumVertices)
	{
		//초기화
		ZeroMemory(pOutTangent, sizeof(D3DXVECTOR3)*NumVertices);
		ZeroMemory(pOutBinormal, sizeof(D3DXVECTOR3)*NumVertices);

		//임시배열 생성
		D3DXVECTOR3* pTangents = new D3DXVECTOR3[NumVertices];
		D3DXVECTOR3* pBinormals = new D3DXVECTOR3[NumVertices];

		//임시배열 초기화
		ZeroMemory(pTangents, sizeof(D3DXVECTOR3)*NumVertices);
		ZeroMemory(pBinormals, sizeof(D3DXVECTOR3)*NumVertices);

		for (DWORD i = 0; i < NumTris; ++i)
		{
			//삼각형의 인덱스
			DWORD i0 = pIndices[(i * 3) + 0];
			DWORD i1 = pIndices[(i * 3) + 1];
			DWORD i2 = pIndices[(i * 3) + 2];

			//해당 삼각형의 정점 위치
			D3DXVECTOR3 p0 = pPositions[i0];
			D3DXVECTOR3 p1 = pPositions[i1];
			D3DXVECTOR3 p2 = pPositions[i2];

			//해당 삼각형의 uv 좌표
			D3DXVECTOR2 uv0 = pUVs[i0];
			D3DXVECTOR2 uv1 = pUVs[i1];
			D3DXVECTOR2 uv2 = pUVs[i2];

			//각변의 모서리
			D3DXVECTOR3 edge1 = p1 - p0;
			D3DXVECTOR3 edge2 = p2 - p0;

			//UV의 모서리
			D3DXVECTOR2 uvEdge1 = uv1 - uv0;
			D3DXVECTOR2 uvEdge2 = uv2 - uv0;

			float r = 1.0f / ((uvEdge1.x*uvEdge2.y) - (uvEdge1.y * uvEdge2.x));

			//탄젠트
			D3DXVECTOR3 t = ((edge1 * uvEdge2.y) + (edge2*-uvEdge1.y)) * r;
			D3DXVec3Normalize(&t, &t); //정규화를 해준다.

			//바이노말
			D3DXVECTOR3 b = ((edge1 * -uvEdge2.x) + (edge2*uvEdge1.x))*r;
			D3DXVec3Normalize(&b, &b);

			pTangents[i0] += t;
			pTangents[i1] += t;
			pTangents[i2] += t;

			pBinormals[i0] += b;
			pBinormals[i1] += b;
			pBinormals[i2] += b;
		}

		for (int i = 0; i < NumVertices; ++i)
		{
			D3DXVECTOR3 n = pNormal[i];
			//탄젠트 직교
			D3DXVECTOR3 t = pTangents[i] - (D3DXVec3Dot(&pTangents[i], &n)*n);
			D3DXVec3Normalize(&t, &t);

			D3DXVECTOR3 b;
			D3DXVec3Cross(&b, &n, &t);//노말과 탄젠트를 외적하여 바이노말을 구함
			D3DXVec3Normalize(&b, &b);

			pOutTangent[i] = t;
			pOutBinormal[i] = b;
		}
		SAFE_DELETE_ARRAY(pTangents);
		SAFE_DELETE_ARRAY(pBinormals);
	}

	void GenTriGrid(int numVertRows, int numVertCols,
		float dx, float dz,
		const D3DXVECTOR3& center,
		std::vector<D3DXVECTOR3>& verts,
		std::vector<DWORD>& indices)
	{
		int numVertices = numVertRows * numVertCols;
		int numCellRows = numVertRows - 1;
		int numCellCols = numVertCols - 1;

		int numTris = numCellRows * numCellCols * 2;

		float width = (float)numCellCols * dx;
		float depth = (float)numCellRows * dz;

		//===========================================
		// Build vertices.

		// We first build the grid geometry centered about the origin and on
		// the xz-plane, row-by-row and in a top-down fashion.  We then translate
		// the grid vertices so that they are centered about the specified 
		// parameter 'center'.

		verts.resize(numVertices);

		// Offsets to translate grid from quadrant 4 to center of 
		// coordinate system.
		float xOffset = -width * 0.5f;
		float zOffset = depth * 0.5f;

		int k = 0;
		for (float i = 0; i < numVertRows; ++i)
		{
			for (float j = 0; j < numVertCols; ++j)
			{
				// Negate the depth coordinate to put in quadrant four.  
				// Then offset to center about coordinate system.
				verts[k].x = j * dx + xOffset;
				verts[k].z = -i * dz + zOffset;
				verts[k].y = 0.0f;

				// Translate so that the center of the grid is at the
				// specified 'center' parameter.
				D3DXMATRIX T;
				D3DXMatrixTranslation(&T, center.x, center.y, center.z);
				D3DXVec3TransformCoord(&verts[k], &verts[k], &T);

				++k; // Next vertex
			}
		}

		//===========================================
		// Build indices.

		indices.resize(numTris * 3);

		// Generate indices for each quad.
		k = 0;
		for (DWORD i = 0; i < (DWORD)numCellRows; ++i)
		{
			for (DWORD j = 0; j < (DWORD)numCellCols; ++j)
			{
				indices[k] = i * numVertCols + j;
				indices[k + 1] = i * numVertCols + j + 1;
				indices[k + 2] = (i + 1) * numVertCols + j;

				indices[k + 3] = (i + 1) * numVertCols + j;
				indices[k + 4] = i * numVertCols + j + 1;
				indices[k + 5] = (i + 1) * numVertCols + j + 1;

				// next quad
				k += 6;
			}
		}
	}
}