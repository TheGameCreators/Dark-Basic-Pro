//
// Collision Functions Header
//

// Define
#ifndef _COLLISION_H_
#define _COLLISION_H_

// Includes
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
#include "col_global.h"
#include "..\\Universe.h"

// Class
class Collision 
{
	public:
		void	Init	( void );
		BOOL	World	( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius, float fScale );
};

#endif
