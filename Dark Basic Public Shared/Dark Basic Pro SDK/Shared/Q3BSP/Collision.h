

#ifndef _COLLISION_H_
#define _COLLISION_H_

#include < d3d9.h >
#include < Dxerr9.h >
#include < D3dx9tex.h >
#include < D3dx9core.h >
#include < basetsd.h >
#include < stdio.h >
#include < math.h >
#include < D3DX9.h >
#include < d3d9types.h >

#define WIN32_LEAN_AND_MEAN 
#include < windows.h >

struct _col_opt
{
	int		check_count;
	BOOL*	check;
};

extern _col_opt col_opt_Q3A;
extern BOOL onfloor, onceil;

void FreeColOpt ( void );

class Collision 
{
	public:
		static BOOL World  ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos, int cut, float fThreshold );
		static BOOL Player ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 mid, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos );
		static BOOL Box    ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere, D3DXVECTOR3 box1, D3DXVECTOR3 box2, float* dist, D3DXVECTOR3* out_pos );
		static BOOL Opt    ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius );
};

#endif