#ifndef _CQUADTREES_H_
#define _CQUADTREES_H_

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

struct tagVerts
{
	float	x, y, z;	// position
	float	nx, ny, nz;	// normals
	DWORD	diffuse;	// diffuse colour
	float	tu, tv;		// texture coordinates
};


#include ".\..\cmodec.h"

class CQuadTreeNode
{
	//#define MAP_SIZE  ( 1025 )
	//#define POOL_SIZE ( 4 * 1000 )
	//#define MIN_RESOLUTION (2.1f)
	//#define MULT_SCALE (0.25f)

	#define LEVEL_MULT (2/(2*(MIN_RESOLUTION-1)))
	#define Child(a) (((CQuadTreeNode *)Children)[a])

	#define ChildNW 0
	#define ChildNE 1
	#define ChildSW 2
	#define ChildSE 3

	//#define MULT_SCALE (0.25f)
	
	
	#define MAX(a,b) ((a < b) ? (b) : (a))
	#define MIN( a, b ) ((a < b)?(a):(b))

	public:
		static unsigned char*	m_pData;
		CQuadTreeNode*			Children;
		float					m_fBlend;
		static float*			m_pError;
		static CQuadTreeNode*	m_aQuadPool;
		static UINT32			m_nNextQuad;
		static UINT8			isInitialized;

		static UINT32 HeightField ( UINT32 y, UINT32 x );
		static float  CalculateRoughness ( UINT32 NW_X, UINT32 NW_Y, UINT32 nQuadWidth );

		static tagVerts m_Verts [ 20 ];
		static int m_iCount;

		float				m_fXDivide;
		float				m_fYDivide;

	public:
		float fXCam;
		float fYCam;
		float fZCam;

		CQuadTreeNode  ( );
		~CQuadTreeNode ( );

		void Split( UINT32 NW_X, UINT32 NW_Y, UINT32 radius );
		void RecursTessellate (	UINT32 NW_X, UINT32 NW_Y, UINT32 Quad_Width );
		void RecursRender
						(
							UINT32 NW_X,
							UINT32 NW_Y,
							UINT32 Quad_Width,
							CQuadTreeNode* North = NULL,
							CQuadTreeNode* South = NULL,
							CQuadTreeNode* East  = NULL,
							CQuadTreeNode* West  = NULL
						);

		BOOL SplitMetric ( UINT32 NW_X, UINT32 NW_Y, UINT32 Quad_Width );

		CQuadTreeNode* GetChild ( UINT32 idx )
		{ 
			return ( Children ) ? ( &( Children [ idx ] ) ) :( NULL );
		}

		void MakeBlendedVertex ( UINT32 x, UINT32 y, float heightOne, float heightTwo, float fBlend, UINT8 nDepth );
		void MakeVertex        ( UINT32 x, UINT32 y, float height, UINT8 nDepth );

		void SetData ( unsigned char* pData )
		{
			m_pData = pData;
		};

		static LPDIRECT3DDEVICE9	m_pD3D;

		void SetDevice ( LPDIRECT3DDEVICE9 lpD3D )
		{
			// get a pointer to d3d device
			m_pD3D = lpD3D;
		}
};

class CQuadTrees : public CMode
{
	private:
		LPDIRECT3DTEXTURE9	m_lpTexture;
		LPDIRECT3DTEXTURE9	m_lpTexture1;

		CQuadTreeNode*		m_QuadTreeRoot;

	public:
		CQuadTrees  ( )
		{
			m_QuadTreeRoot = NULL;

		}

		~CQuadTrees ( )
		{
			SAFE_DELETE ( m_QuadTreeRoot );
		}

		void Destroy ( void )
		{
			SAFE_DELETE ( m_QuadTreeRoot );
		}

		void Setup ( void )
		{
			m_QuadTreeRoot = new CQuadTreeNode ( );
			
			CQuadTreeNode::m_pD3D = m_pD3D;
			m_QuadTreeRoot->SetData ( m_pData );
			CQuadTreeNode::CalculateRoughness ( 0, 0, MAP_SIZE );
		
			// D3DXCreateTextureFromFileExA ( m_pD3D, "ground_01.jpg", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_lpTexture  );
			// D3DXCreateTextureFromFileExA ( m_pD3D, "texture3.tga", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_lpTexture1 );
		};

		void SetTextureDivide ( float fX, float fY )
		{
			CQuadTreeNode*  pPtr     = m_QuadTreeRoot;
			bool			bLoop    = true;
		
			while ( bLoop )
			{
				if ( pPtr )
				{
					pPtr->m_fXDivide = fX;
					pPtr->m_fYDivide = fY;
				}

				if ( pPtr->Children )
					pPtr = pPtr->Children;
				else
					bLoop = false;
			}

			bLoop = true;
			pPtr  = m_QuadTreeRoot->m_aQuadPool;
			pPtr->m_fXDivide = fX;
			pPtr->m_fYDivide = fY;
		}

		void Reset ( void )
		{
			
		};

		void Draw ( float fX, float fY, float fZ )
		{
			CQuadTreeNode::m_nNextQuad = 0;
			m_QuadTreeRoot->RecursTessellate ( 0, 0, MAP_SIZE );

		
			m_QuadTreeRoot->fXCam = fX;
			m_QuadTreeRoot->fYCam = fY;
			m_QuadTreeRoot->fZCam = fZ;

			// DX8->DX9
			m_pD3D->SetVertexShader ( NULL );
			m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		
			//////
			
			// lee - is this going to be moved across to the main rendering section so that
			// all of the render states etc will be in one place?

			// for now part of terrain manager - but yes we'll sort this and unify it
			// m_pD3D->SetTexture ( 0, m_lpTexture );
//			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//			// m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
//			m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
//			m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
			m_QuadTreeRoot->RecursRender ( 0, 0, MAP_SIZE, NULL, NULL, NULL, NULL );
	};
};

#endif _CQUADTREES_H_
