
//
// DBOEffects Functions Header
//

#ifndef _DBOEFFECTS_H_
#define _DBOEFFECTS_H_

//////////////////////////////////////////////////////////////////////////////////////
// COMMON INCLUDES ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "..\global.h"

//////////////////////////////////////////////////////////////////////////////////////
// DBOFORMAT INCLUDE /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "DBOFormat.h"

//////////////////////////////////////////////////////////////////////////////////////
// SPECIAL EFFECT SUBCLASSES /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
class cExternalEffect : public cSpecialEffect {};
class cBumpMapping    : public cSpecialEffect {};
class cCartoonMapping : public cSpecialEffect {};
class cRainbowMapping : public cSpecialEffect {};
class cMetalMapping   : public cSpecialEffect {};

/* do not need this
class cShadowVolume : public cSpecialEffect
{
	private:
		class cShadow
		{
			// used to control the drawn shadow

			D3DXVECTOR3 m_pVertices [ 32000 ];
			DWORD       m_dwNumVertices;

			public:
				void Reset ( void )
				{ 
					m_dwNumVertices = 0L;
				}

				void BuildFromMesh ( LPD3DXMESH pMesh, D3DXVECTOR3 vLight )
				{
					struct MESHVERTEX
					{
						D3DXVECTOR3 p;
						D3DXVECTOR3	n;
						FLOAT		tu, tv;
					};

					MESHVERTEX* pVertices;
					WORD*       pIndices;

					pMesh->LockVertexBuffer ( 0L, ( VOID** ) &pVertices );
					pMesh->LockIndexBuffer  ( 0L, ( VOID** ) &pIndices  );

					DWORD dwNumVertices = pMesh->GetNumVertices ( );
					DWORD dwNumFaces    = pMesh->GetNumFaces    ( );

					WORD* pEdges = new WORD [ dwNumFaces * 6 ];
					DWORD dwNumEdges = 0;
					
					DWORD i = 0;

					for ( i = 0; i < dwNumFaces; i++ )
					{
						WORD wFace0 = pIndices [ 3 * i + 0 ];
						WORD wFace1 = pIndices [ 3 * i + 1 ];
						WORD wFace2 = pIndices [ 3 * i + 2 ];

						D3DXVECTOR3 v0 = pVertices [ wFace0 ].p;
						D3DXVECTOR3 v1 = pVertices [ wFace1 ].p;
						D3DXVECTOR3 v2 = pVertices [ wFace2 ].p;

						D3DXVECTOR3 vNormal;
						D3DXVec3Cross ( &vNormal, &( v2 - v1 ), &( v1 - v0 ) );

						if( D3DXVec3Dot ( &vNormal, &vLight ) >= 0.0f )
						{
							AddEdge ( pEdges, dwNumEdges, wFace0, wFace1 );
							AddEdge ( pEdges, dwNumEdges, wFace1, wFace2 );
							AddEdge ( pEdges, dwNumEdges, wFace2, wFace0 );
						}
					}

					for ( i = 0; i < dwNumEdges; i++ )
					{
						D3DXVECTOR3 v1 = pVertices [ pEdges [ 2 * i + 0 ] ].p;
						D3DXVECTOR3 v2 = pVertices [ pEdges [ 2 * i + 1 ] ].p;
						D3DXVECTOR3 v3 = v1 - vLight * 10;
						D3DXVECTOR3 v4 = v2 - vLight * 10;

						m_pVertices [ m_dwNumVertices++ ] = v1;
						m_pVertices [ m_dwNumVertices++ ] = v2;
						m_pVertices [ m_dwNumVertices++ ] = v3;

						m_pVertices [ m_dwNumVertices++ ] = v2;
						m_pVertices [ m_dwNumVertices++ ] = v4;
						m_pVertices [ m_dwNumVertices++ ] = v3;
					}
					
					SAFE_DELETE_ARRAY ( pEdges );

					pMesh->UnlockVertexBuffer ( );
					pMesh->UnlockIndexBuffer  ( );
				}

				VOID AddEdge ( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
				{
					for ( DWORD i = 0; i < dwNumEdges; i++ )
					{
						if ( ( pEdges [ 2 * i + 0 ] == v0 && pEdges [ 2 * i + 1 ] == v1 ) ||
							 ( pEdges [ 2 * i + 0 ] == v1 && pEdges [ 2 * i + 1 ] == v0 ) )
						{
							if ( dwNumEdges > 1 )
							{
								pEdges [ 2 * i + 0 ] = pEdges [ 2 * ( dwNumEdges - 1 ) + 0 ];
								pEdges [ 2 * i + 1 ] = pEdges [ 2 * ( dwNumEdges - 1 ) + 1 ];
							}

							dwNumEdges--;
							return;
						}
					}

					pEdges [ 2 * dwNumEdges + 0 ] = v0;
					pEdges [ 2 * dwNumEdges + 1 ] = v1;

					dwNumEdges++;
				}

				void Render ( LPDIRECT3DDEVICE9 pD3D )
				{
					// dx8->dx9
					pD3D->SetVertexShader ( NULL );
					pD3D->SetFVF ( D3DFVF_XYZ );

					pD3D->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, m_dwNumVertices / 3, m_pVertices, sizeof ( D3DXVECTOR3 ) );
				}
		};

		cShadow					m_Shadow;			// shadow
		D3DXMATRIX				m_matrix;			// store the object matrix

		struct sMeshList
		{
			// whenever we setup an effect we get sent pointers to the meshes so we can modify them
			// if necessary, this is usually a one off function call, in our case we need to keep a 
			// list of meshes so we can adjust them each frame, this is what this structure is used for

			LPD3DXMESH pMesh;	// pointer to a mesh
			sMeshList* pNext;	// pointer to next link for mesh container

			sMeshList ( )
			{
				pMesh = NULL;
				pNext = NULL;
			}

			~sMeshList ( )
			{
				SAFE_DELETE ( pNext );
			}
		};

		sMeshList*  m_pMeshListStart;	// start of mesh
		sMeshList*	m_pMeshListCurrent;	// current link
		int			m_iMeshCount;		// number of meshes
		
		#define D3DFVF_SHADOWVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

		struct SHADOWVERTEX
		{
			D3DXVECTOR4 p;
			D3DCOLOR    color;
		};

	public:

		// constructors
		cShadowVolume::cShadowVolume ( )
		{
		};
		cShadowVolume::~cShadowVolume ( )
		{
		};

		// functions
		bool Setup		( sMesh* pMesh, LPSTR pEffectFile, bool bUseXFile, bool bUseTextures );
		void Mesh		( sMesh* pMesh );
		DWORD Start		( sMesh* pMesh, D3DXMATRIX matObject );
		void End		( void );
};
*/

class cReflectionMapping : public cSpecialEffect
{
	public:

		// constructors
		cReflectionMapping::cReflectionMapping ( )
		{
		};
		cReflectionMapping::~cReflectionMapping ( )
		{
		};

		// functions
		bool Setup		( sMesh* pMesh, LPSTR pEffectFile, bool bUseXFile, bool bUseTextures );
		void Mesh		( sMesh* pMesh );
		DWORD Start		( sMesh* pMesh, D3DXMATRIX matObject );
		void End		( void );
};

#endif _DBOEFFECTS_H_

