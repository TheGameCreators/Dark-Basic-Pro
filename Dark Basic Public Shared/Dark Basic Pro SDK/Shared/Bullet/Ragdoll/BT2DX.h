#pragma once

#include "Windows.h"
#include "btBulletDynamicsCommon.h"
#include "D3dx9math.h"

class BT2DX
{
public:
	BT2DX(void);
	~BT2DX(void);
	static D3DXVECTOR3 BT2DX_VECTOR3(const btVector3 &v);
	static D3DXQUATERNION BT2DX_QUATERNION(const btQuaternion &q);
	static D3DXMATRIX BT2DX_MATRIX(const btTransform &ms);
	static D3DXMATRIX ConvertBulletMotionState(const btMotionState &ms);
	static btVector3 DX_VECTOR3_2BT( D3DXVECTOR3 &v);
	static D3DXMATRIX ConvertBulletTransform( btTransform *bulletTransformMatrix );
	static btTransform ConvertD3DXMatrix( D3DXMATRIX *d3dMatrix );
	static void XPrepareMatrixFromRULP( D3DXMATRIX &matOutput, D3DXVECTOR3 *R, D3DXVECTOR3 *U, D3DXVECTOR3 *L, D3DXVECTOR3 *P );
};

