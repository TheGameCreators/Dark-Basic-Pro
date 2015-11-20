#ifndef _CMODE_H_
#define _CMODE_H_

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

#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

/*
#define MAP_SIZE  ( 1025 )
#define POOL_SIZE ( 4 * 1000 )
#define MIN_RESOLUTION (2.1f)
#define MULT_SCALE (0.25f)
*/

// these were defines before, you can now set the properties manually
extern UINT32				MAP_SIZE;
extern UINT32				POOL_SIZE;
extern float				MIN_RESOLUTION;
extern float				MULT_SCALE;

class CMode
{
	protected:
		LPDIRECT3DDEVICE9	m_pD3D;
		unsigned char*		m_pData;

	public:
		void SetDevice ( LPDIRECT3DDEVICE9 lpD3D )
		{
			// get a pointer to d3d device
			m_pD3D = lpD3D;
		}

		virtual void Setup ( void )
		{
		
		};

		virtual void Destroy ( void )
		{
		
		};

		virtual void Reset ( void )
		{
			
		};

		virtual void Draw ( float fX, float fY, float fZ )
		{
		
		};

		virtual void SetTextureDivide ( float fX, float fY )
		{

		}

		virtual void SetData ( unsigned char* pData )
		{
			m_pData = pData;
		};

		virtual unsigned char* GetData ( void )
		{
			return m_pData;
		};
};

#endif _CMODE_H_
