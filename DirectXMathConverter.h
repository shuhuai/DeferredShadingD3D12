#ifndef DirectXMathConverter
#define DirectXMathConverter

#include <DirectXMath.h>
using namespace DirectX;

inline XMVECTOR GMathFV(XMFLOAT3& val)
{
	return XMLoadFloat3(&val);
}

inline XMVECTOR GMathFV(XMFLOAT2& val)
{
	return XMLoadFloat2(&val);
}

inline XMFLOAT3 GMathVF(XMVECTOR& vec)
{
	XMFLOAT3 val;
	XMStoreFloat3(&val, vec);
	return val;
}

inline XMFLOAT2 GMathVF2(XMVECTOR& vec)
{
	XMFLOAT2 val;
	XMStoreFloat2(&val, vec);
	return val;
}


inline XMMATRIX GMathFM(XMFLOAT4X4& mat)
{
	return XMLoadFloat4x4(&mat);
}

inline XMFLOAT4X4 GMathMF(XMMATRIX& mat)
{

	XMFLOAT4X4 val;
	XMStoreFloat4x4(&val, mat);
	return val;

}
#endif