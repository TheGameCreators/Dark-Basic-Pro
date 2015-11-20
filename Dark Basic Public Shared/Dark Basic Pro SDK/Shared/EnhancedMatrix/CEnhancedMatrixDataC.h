#ifndef _CENHANCEDMATRIXDATA_H_
#define _CENHANCEDMATRIXDATA_H_

#include ".\..\position\cpositionc.h"

#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "d3d9.lib"   )
#pragma comment ( lib, "d3dx9.lib"  )
#pragma comment ( lib, "dxerr9.lib" )

#include <d3d9.h>
#include <Dxerr9.h>
#include <D3dx9tex.h>
#include <D3dx9core.h>
#include <basetsd.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include <d3d9types.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h>

#include ".\modes\cmodec.h"

struct tagEnhancedMatrixData : public tagObjectPos
{
	CMode*					m_pMode;
	LPDIRECT3DDEVICE9		pD3D;

	int						m_iWidth;
	LPDIRECT3DTEXTURE9		m_pTexture;

};


#endif _CENHANCEDMATRIXDATA_H_