
//
// Collision Functions Header
//


#ifndef _COLLISION_H_
#define _COLLISION_H_

#include <d3d9.h>
#include <Dxerr9.h>
#include <D3dx9tex.h>
#include <D3dx9core.h>
#include <basetsd.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include <d3d9types.h>

#include <vector>
using namespace std;

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#ifndef __GNUC__
struct sCollisionVertex
{
	D3DXVECTOR3 vecPosition;
	float		tu, tv;
};

struct sCollisionPolygon
{
	sCollisionVertex	triangle [ 3 ];
	D3DXVECTOR3			normal;
	DWORD				diffuseid;
};
#endif

#ifndef __GNUC__
extern vector < sCollisionPolygon* > g_PolygonList;
#else

#endif


class Collision 
{
	public:
		BOOL World  ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos, int cut, float fScale );
};


#endif
