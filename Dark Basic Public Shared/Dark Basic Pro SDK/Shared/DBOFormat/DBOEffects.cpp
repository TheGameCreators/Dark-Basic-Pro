
//
// DBOEffects Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBOEFFECTS HEADER ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "DBOEffects.h"
#include "DBOMesh.h"

//////////////////////////////////////////////////////////////////////////////////
// EFFECT FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Shadow Effect
//

/*
bool cShadowVolume::Setup ( sMesh* pMesh, LPSTR pEffectFile, bool bUseXFile, bool bUseTextures )
{
	// shadow list reset
	m_iMeshCount = 0;

	// complete
	return true;
}

void cShadowVolume::Mesh ( sMesh* pMesh )
{
	// retain a list of all meshes

	// vertex data
	struct MESH_VERTEX
	{
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	// store the size
	pMesh->dwFVFSize = sizeof ( MESH_VERTEX );

	if ( m_iMeshCount == 0 )
	{
		// start the list off
		m_pMeshListStart		= new sMeshList;	// create a new link
		m_pMeshListCurrent		= m_pMeshListStart;	// keep a pointer to the current
		m_iMeshCount++;								// increment the mesh count
	}
	else
	{
		// add to the list
		m_pMeshListCurrent->pNext = new sMeshList;				// create a new link
		m_pMeshListCurrent        = m_pMeshListCurrent->pNext;	// move to the next link
		m_iMeshCount++;											// increment the mesh count
	}
}

DWORD cShadowVolume::Start ( sMesh* pMesh, D3DXMATRIX matObject )
{
	// If this stencil effect active
	if(g_pGlob->dwStencilMode==1)
	{
		// direction of the light
		float Lx, Ly, Lz;

		// Always use light zero
		D3DLIGHT9 lightzero;
		D3DXVECTOR4 LightDir;
		m_pD3D->GetLight( 0, &lightzero );

		if(lightzero.Type==D3DLIGHT_POINT)
		{
			Lx = lightzero.Position.x;
			Ly = lightzero.Position.y;
			Lz = lightzero.Position.z;
		}
		else
		{
			Lx = lightzero.Direction.x;
			Ly = lightzero.Direction.y;
			Lz = lightzero.Direction.z;
		}
		
		// transform the light vector to be in object space
		D3DXVECTOR3 vLight;
		D3DXMATRIX  m;

		D3DXMatrixInverse ( &m, NULL, &matObject );

		vLight.x = Lx * m._11 + Ly * m._21 + Lz * m._31 + m._41;
		vLight.y = Lx * m._12 + Ly * m._22 + Lz * m._32 + m._42;
		vLight.z = Lx * m._13 + Ly * m._23 + Lz * m._33 + m._43;

		// reset the shadow
		m_Shadow.Reset ( );

		// get a pointer to the start of the list
		sMeshList*  pMesh = m_pMeshListStart;

		while ( pMesh )
		{
			// send each part of the model across to the build function

			m_Shadow.BuildFromMesh ( pMesh->pMesh, vLight );
			pMesh = pMesh->pNext;
		}

		// we need to store the matrix for later on
		m_matrix = matObject;
	}

	// one pass
	return 1;
}

void cShadowVolume::End ( void )
{
	redundant??
	// If this stencil effect active
	if(g_pGlob->dwStencilMode==1)
	{
		// render the shadow
		m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     FALSE );
		m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    TRUE );
		m_pD3D->SetRenderState ( D3DRS_STENCILFUNC,      D3DCMP_ALWAYS );
		m_pD3D->SetRenderState ( D3DRS_STENCILREF,       0x1 );
		m_pD3D->SetRenderState ( D3DRS_STENCILMASK,      0xffffffff );
		m_pD3D->SetRenderState ( D3DRS_STENCILWRITEMASK, 0xffffffff );
		m_pD3D->SetRenderState ( D3DRS_STENCILPASS,      D3DSTENCILOP_INCR );
		m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ZERO );
		m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_ONE );

		// draw front of shadow
		m_pD3D->SetTransform ( D3DTS_WORLD, &m_matrix );
		m_Shadow.Render ( m_pD3D );

		// reverse cull order so we can draw the back of the shadow
		m_pD3D->SetRenderState ( D3DRS_CULLMODE,    D3DCULL_CW );
		m_pD3D->SetRenderState ( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );
		m_pD3D->SetTransform ( D3DTS_WORLD, &m_matrix );
		m_Shadow.Render ( m_pD3D );

		// restore render states
		m_pD3D->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
		m_pD3D->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_CCW );
		m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     TRUE );
		m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    FALSE );
		m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	}
}
*/

//
// Reflection Effect
//

bool cReflectionMapping::Setup ( sMesh* pMesh, LPSTR pEffectFile, bool bUseXFile, bool bUseTextures )
{
	// complete
	return true;
}

void cReflectionMapping::Mesh ( sMesh* pMesh )
{
	// vertex data
	struct MESH_VERTEX
	{
		float x, y, z;
	};

	// store the size
	pMesh->dwFVFSize = sizeof ( MESH_VERTEX );
}

DWORD cReflectionMapping::Start ( sMesh* pMesh, D3DXMATRIX matObject )
{
	// If this stencil effect active
	if(g_pGlob->dwStencilMode==2)
	{
		// Two Phases for Reflection Rendering
		if(g_pGlob->dwRedrawPhase==0)
		{
			// A : Cut into stencil buffer with plane(object)

			// both glob and g_plnReflectionPlane invade DBO!!
			D3DXPLANE g_plnReflectionPlane;
			D3DXVECTOR3 g_vecReflectionPoint;
			D3DXVECTOR3 vecPos;
			D3DXVECTOR3 vecDir;

			// set position and direction of reflection plane
			D3DXPlaneFromPointNormal ( &g_plnReflectionPlane, &vecPos, &vecDir );
			g_vecReflectionPoint = vecPos;

			// turn depth buffer off, and stencil buffer on
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    TRUE );
			m_pD3D->SetRenderState ( D3DRS_STENCILFUNC,      D3DCMP_ALWAYS );
			m_pD3D->SetRenderState ( D3DRS_STENCILREF,       0x1 );
			m_pD3D->SetRenderState ( D3DRS_STENCILMASK,      0xffffffff );
			m_pD3D->SetRenderState ( D3DRS_STENCILWRITEMASK, 0xffffffff );
			m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP );
			m_pD3D->SetRenderState ( D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP );
			m_pD3D->SetRenderState ( D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE );

			// make sure no pixels are written to the z buffer or frame buffer
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     FALSE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ZERO );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_ONE );

			// clear the stencil buffer
			m_pD3D->Clear ( 0L, NULL, D3DCLEAR_STENCIL, 0L, 1.0f, 0L );
		}
	}

	// one pass
	return 1;
}

void cReflectionMapping::End ( void )
{
	/*
	// If this stencil effect active
	if(g_pGlob->dwStencilMode==2)
	{
		if(g_pGlob->dwRedrawPhase==0)
		{
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,	FALSE );
		}
		m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,	 TRUE );
		m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	}
	*/
}
