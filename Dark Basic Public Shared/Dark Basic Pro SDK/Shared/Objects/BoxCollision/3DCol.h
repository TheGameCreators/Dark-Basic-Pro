//
// 3DCol.h: interface for the C3DCollision class.
//

#include <windows.h>
#include <D3DX9.h>

#define EPSILON 1.0e-5f
#define FABS _fabs

// floating absolute value
// the fabs C runtime uses doubles
inline float _fabs(float n) {
	return (n < 0.0f ? -n : n);
}

class C3DCollision
{
	public:
		C3DCollision();
		~C3DCollision();

		// Main Checks
		int TestBoxInBox(D3DXVECTOR3 box1p, D3DXVECTOR3 box1s, D3DXMATRIX box1r, D3DXVECTOR3 box2p, D3DXVECTOR3 box2s, D3DXMATRIX box2r);

		// Core Tests
		int intersect_boxbox(D3DXVECTOR3& vBox1Position, D3DXVECTOR3& vBox1Size, D3DXVECTOR3* pvBox1Basis, D3DXVECTOR3& vBox2Position, D3DXVECTOR3& vBox2Size, D3DXVECTOR3* pvBox2Basis);
};
