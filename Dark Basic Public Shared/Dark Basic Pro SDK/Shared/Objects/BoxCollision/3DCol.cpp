//
// 3DCol.cpp: implementation of C3DCollision
//

// Includes
#define D3DOVERLOAD
#include "windows.h"
#include "3DCol.h"

// Workhorse Functions
// the out is a D3DXVECTOR3[3] array
// this assumes it's the world-transform matrix,
// if you can be sure it is just a rotation, then
// remove the bit of code that handles origins
inline int matrix_to_basis(D3DXMATRIX &mat, D3DXVECTOR3 *out)
{
	D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
//	if (FAILED(D3DMath_VectorMatrixMultiply(origin, origin, mat))) return 0;
	D3DXVec3TransformCoord ( &origin, &origin, &mat );

	// X axis normal
	out[0].x = 1.0f; out[0].y = 0.0f; out[0].z = 0.0f;
//	D3DMath_VectorMatrixMultiply(out[0], out[0], mat);
//	out[0] -= origin; out[0] = Normalize(out[0]);
	D3DXVec3TransformCoord ( &out[0], &out[0], &mat );
	out[0] -= origin; D3DXVec3Normalize ( &out[0], &out[0] );

	// Y axis normal
	out[1].x = 0.0f; out[1].y = 1.0f; out[1].z = 0.0f;
//	D3DMath_VectorMatrixMultiply(out[1], out[1], mat);
//	out[1] -= origin; out[1] = Normalize(out[1]);
	D3DXVec3TransformCoord ( &out[1], &out[1], &mat );
	out[1] -= origin; D3DXVec3Normalize ( &out[1], &out[1] );

	// Z axis normal
	out[2].x = 0.0f; out[2].y = 0.0f; out[2].z = 1.0f;
//	D3DMath_VectorMatrixMultiply(out[2], out[2], mat);
//	out[2] -= origin; out[2] = Normalize(out[2]);
	D3DXVec3TransformCoord ( &out[2], &out[2], &mat );
	out[2] -= origin; D3DXVec3Normalize ( &out[2], &out[2] );
	
	return 1;
}

// Implementations
C3DCollision::C3DCollision() {}
C3DCollision::~C3DCollision() {}

// Actual Matrix Based Box-Box Check
int C3DCollision::TestBoxInBox(D3DXVECTOR3 box1p, D3DXVECTOR3 box1s, D3DXMATRIX box1r, D3DXVECTOR3 box2p, D3DXVECTOR3 box2s, D3DXMATRIX box2r)
{
	D3DXVECTOR3 basis1[3];
	D3DXVECTOR3 basis2[3];
	matrix_to_basis(box1r, basis1);
	matrix_to_basis(box2r, basis2);
	if(intersect_boxbox(box1p, box1s, basis1, box2p, box2s, basis2)==1)
		return 1;
	else
		return 0;
}

// Intersect Test Box Vs Box
int C3DCollision::intersect_boxbox(	D3DXVECTOR3& vBox1Position, D3DXVECTOR3& vBox1Size, 
								D3DXVECTOR3* pvBox1Basis, D3DXVECTOR3& vBox2Position,
								D3DXVECTOR3& vBox2Size, D3DXVECTOR3* pvBox2Basis)
{
	register float rot[3][3];
	register float r1, r2;
	register float test;

	// Translations
	D3DXVECTOR3 v = vBox2Position - vBox1Position;
	D3DXVECTOR3 t = D3DXVECTOR3(	D3DXVec3Dot( &v, &pvBox1Basis[0] ), 
									D3DXVec3Dot( &v, &pvBox1Basis[1] ), 
									D3DXVec3Dot( &v, &pvBox1Basis[2] )  );

	
	// Relative rotation of B to A
	/*
	rot[0][0] = DotProduct(pvBox1Basis[0], pvBox2Basis[0]);
	rot[0][1] = DotProduct(pvBox1Basis[0], pvBox2Basis[1]);
	rot[0][2] = DotProduct(pvBox1Basis[0], pvBox2Basis[2]);
	rot[1][0] = DotProduct(pvBox1Basis[1], pvBox2Basis[0]);
	rot[1][1] = DotProduct(pvBox1Basis[1], pvBox2Basis[1]);
	rot[1][2] = DotProduct(pvBox1Basis[1], pvBox2Basis[2]);
	rot[2][0] = DotProduct(pvBox1Basis[2], pvBox2Basis[0]);
	rot[2][1] = DotProduct(pvBox1Basis[2], pvBox2Basis[1]);
	rot[2][2] = DotProduct(pvBox1Basis[2], pvBox2Basis[2]);
	*/

	rot[0][0] = D3DXVec3Dot(&pvBox1Basis[0], &pvBox2Basis[0]);
	rot[0][1] = D3DXVec3Dot(&pvBox1Basis[0], &pvBox2Basis[1]);
	rot[0][2] = D3DXVec3Dot(&pvBox1Basis[0], &pvBox2Basis[2]);
	rot[1][0] = D3DXVec3Dot(&pvBox1Basis[1], &pvBox2Basis[0]);
	rot[1][1] = D3DXVec3Dot(&pvBox1Basis[1], &pvBox2Basis[1]);
	rot[1][2] = D3DXVec3Dot(&pvBox1Basis[1], &pvBox2Basis[2]);
	rot[2][0] = D3DXVec3Dot(&pvBox1Basis[2], &pvBox2Basis[0]);
	rot[2][1] = D3DXVec3Dot(&pvBox1Basis[2], &pvBox2Basis[1]);
	rot[2][2] = D3DXVec3Dot(&pvBox1Basis[2], &pvBox2Basis[2]);
	
	
	// test angles
	r1 = vBox1Size.x + vBox2Size.x*FABS(rot[0][0]) 
		+ vBox2Size.y*FABS(rot[0][1]) + vBox2Size.z*FABS(rot[0][2]);
	test = FABS(t.x);
	if (test > r1) return 0;

	r1 = vBox1Size.y + vBox2Size.x*FABS(rot[1][0]) 
		+ vBox2Size.y*FABS(rot[1][1]) + vBox2Size.z*FABS(rot[1][2]);
	test = FABS(t.y);
	if (test > r1) return 0;

	r1 = vBox1Size.z + vBox2Size.x*FABS(rot[2][0]) 
		+ vBox2Size.y*FABS(rot[2][1]) + vBox2Size.z*FABS(rot[2][2]);
	test = FABS(t.z);
	if (test > r1) return 0;

	r1 = vBox2Size.x + vBox1Size.x*FABS(rot[0][0]) 
		+ vBox1Size.y*FABS(rot[1][0]) + vBox1Size.z*FABS(rot[2][0]);
	test = FABS(t.x*rot[0][0] + t.y*rot[1][0] + t.z*rot[2][0]);
	if (test > r1) return 0;

	r1 = vBox2Size.y + vBox1Size.x*FABS(rot[0][1]) 
		+ vBox1Size.y*FABS(rot[1][1]) + vBox1Size.z*FABS(rot[2][1]);
	test = FABS(t.x*rot[0][1] + t.y*rot[1][1] + t.z*rot[2][1]);
	if (test > r1) return 0;

	r1 = vBox2Size.z + vBox1Size.x*FABS(rot[0][2]) 
		+ vBox1Size.y*FABS(rot[1][2]) + vBox1Size.z*FABS(rot[2][2]);
	test = FABS(t.x*rot[0][2] + t.y*rot[1][2] + t.z*rot[2][2]);
	if (test > r1) return 0;
	
	// test crossproducts
	r1 = vBox1Size.y*FABS(rot[2][0]) + vBox1Size.z*FABS(rot[1][0]);
	r2 = vBox2Size.y*FABS(rot[0][2]) + vBox2Size.z*FABS(rot[0][1]);
	test = FABS(t.z*rot[1][0] - t.y*rot[2][0]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.y*FABS(rot[2][1]) + vBox1Size.z*FABS(rot[1][1]);
	r2 = vBox2Size.x*FABS(rot[0][2]) + vBox2Size.z*FABS(rot[0][0]);
	test = FABS(t.z*rot[1][1] - t.y*rot[2][1]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.y*FABS(rot[2][2]) + vBox1Size.z*FABS(rot[1][2]);
	r2 = vBox2Size.x*FABS(rot[0][1]) + vBox2Size.y*FABS(rot[0][0]);
	test = FABS(t.z*rot[1][2] - t.y*rot[2][2]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[2][0]) + vBox1Size.z*FABS(rot[0][0]);
	r2 = vBox2Size.y*FABS(rot[1][2]) + vBox2Size.z*FABS(rot[1][1]);
	test = FABS(t.x*rot[2][0] - t.z*rot[0][0]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[2][1]) + vBox1Size.z*FABS(rot[0][1]);
	r2 = vBox2Size.x*FABS(rot[1][2]) + vBox2Size.z*FABS(rot[1][0]);
	test = FABS(t.x*rot[2][1] - t.z*rot[0][1]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[2][2]) + vBox1Size.z*FABS(rot[0][2]);
	r2 = vBox2Size.x*FABS(rot[1][1]) + vBox2Size.y*FABS(rot[1][0]);
	test = FABS(t.x*rot[2][2] - t.z*rot[0][2]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[1][0]) + vBox1Size.y*FABS(rot[0][0]);
	r2 = vBox2Size.y*FABS(rot[2][2]) + vBox2Size.z*FABS(rot[2][1]);
	test = FABS(t.y*rot[0][0] - t.x*rot[1][0]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[1][1]) + vBox1Size.y*FABS(rot[0][1]);
	r2 = vBox2Size.x*FABS(rot[2][2]) + vBox2Size.z*FABS(rot[2][0]);
	test = FABS(t.y*rot[0][1] - t.x*rot[1][1]);
	if (test > r1 + r2) return 0;

	r1 = vBox1Size.x*FABS(rot[1][2]) + vBox1Size.y*FABS(rot[0][2]);
	r2 = vBox2Size.x*FABS(rot[2][1]) + vBox2Size.y*FABS(rot[2][0]);
	test = FABS(t.y*rot[0][2] - t.x*rot[1][2]);
	if (test > r1 + r2) return 0;

	// yes, there's a collision
	return 1;
}
