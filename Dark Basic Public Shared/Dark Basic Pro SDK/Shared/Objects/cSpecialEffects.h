//////////////////////////////////////////////////////////////////////////////////
// LOGS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*

03/05/02	1000	mj	setup this file for special effects
03/05/02	1100	mj	switched to using classes instead of an effects array
						now it's much easier and better to use, mixing multiple
						effects won't work though ( just yet )
03/05/02	1200	mj	cartoon shading in and works fine
03/05/02	1300	mj	bump mapping in, cannot set a normal texture though yet
03/05/02	1310	mj	note - need to expose more functionality with shaders,
						more functions need to go in to allow more control
03/05/02	1750	mj	finished up for now, got a lot more to add to this
*/
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// includes //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <d3d9.h>
#include <D3DX9.h>
#include ".\..\error\cerror.h"
//////////////////////////////////////////////////////////////////////////////////

#include ".\..\core\globstruct.h"
extern GlobStruct* g_pGlob;

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// information ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
	all of the special effects are contained in here like cartoon shading and bump
	mapping

	there is a base class called "cSpecialEffect", every effect must inherit from
	this and fill in the virtual functions -

			* setup - called to create the vertex shader
			* start - passes constants to the shader, must be called before rendering
			* end   - cleans up after rendering
			* mesh  - pass the mesh to this function for any alterations e.g change of FVF

	by using polymorphism we can make an instance of "cSpecialEffect" in the object
	data structure and then create a new effect dependant on what the user calls so we
	don't need lots of different variables for each effect, we only need 1

	after creating a special effect you must update other properties -

			* FVF      - the effect may have changed it
			* FVF size - the size may have changed
*/
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// globals ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
extern LPDIRECT3DDEVICE9	m_pD3D;
extern D3DXPLANE			g_plnReflectionPlane;

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// class definitions /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



class cSpecialEffect
{
	// base class for special effects

	public:
		bool	bInit;					// is shader setup ok
		DWORD	dwShader;				// fvf for shader
		DWORD	dwFVFSize;				// size of vertex data
		
		cSpecialEffect ( )
		{
			// setup default values

			bInit    = false;		// shader isn't setup
			dwShader = 0;			// no shader
		}

		// functions
		virtual void Free (void )					= 0;
		virtual int GetBaseTextureSlot(void)		= 0;	// if shader can use base texture
		virtual DWORD* GetDeclaration(void)			= 0;

		virtual void Setup ( void*, void* )         = 0;	// setup shader ( passing data is a bit naff the way it is )
		virtual void Start ( D3DXMATRIX matObject ) = 0;	// start - pass constants etc to shader
		virtual void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir ) = 0;
		virtual void End   ( void )                 = 0;	// end rendering - clean up
		virtual void Mesh  ( LPD3DXMESH pMesh )	    = 0;	// used to modify the mesh if needed

		virtual bool Mid ( void )
		{
			// used in case an object needs to be modified before the end point

			return false;
		}
};

class cCartoon : public cSpecialEffect
{
	// cartoon rendering

	private:
		LPDIRECT3DTEXTURE8 m_pToonTexture;	// actual toon texture
		LPDIRECT3DTEXTURE8 m_pEdgeTexture;	// edge texture ( usually black )
		DWORD*			   m_dwDeclaration;	// shader declaration

	public:
		~cCartoon ( )
		{
		}
		void Free( void )
		{
			// delete the vertex shader
			m_pD3D->DeleteVertexShader ( dwShader );

			// delete declaration
			SAFE_DELETE_ARRAY ( m_dwDeclaration );
		}

		int GetBaseTextureSlot(void) { return 2; }
		DWORD* GetDeclaration(void) { return m_dwDeclaration; }

		void Setup ( void* pStandardImage, void* pEdgeImage )
		{
			// create the shader

			// shader declaration data
			DWORD dwDeclaration [ ] =
			{
				D3DVSD_STREAM ( 0 ),									// start stream
				D3DVSD_REG ( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),		// position
				D3DVSD_REG ( D3DVSDE_NORMAL,   D3DVSDT_FLOAT3 ),		// normal
				D3DVSD_REG ( D3DVSDE_DIFFUSE,  D3DVSDT_D3DCOLOR ),		// diffuse
				D3DVSD_END ( )											// end stream
			};

			// setup space to store the stream
			m_dwDeclaration = new DWORD [ sizeof ( dwDeclaration ) / sizeof ( DWORD ) ];

			// setup the actual shader
			char szShader [ ] =
			{
				"vs.1.1\n"

				"; do the final transformation and emit it in oPos, this is completely independent of the lighting operations\n"
				"dp4 oPos.x, v0, c0\n"
				"dp4 oPos.y, v0, c1\n"
				"dp4 oPos.z, v0, c2\n"
				"dp4 oPos.w, v0, c3\n"

				"; dot product of light vector with normal vector and output it as the first texture coordinate\n"
				"dp3 oT0.x, v3, -c5\n"

				"; compute the vector from eye to vertex in object space and store the vector in r1\n"
				"sub r1, c6, v0\n"

				"; normalize the eye vector\n"
				"dp3 r1.w, r1, r1\n"
				"rsq r1.w, r1.w\n"
				"mul r1, r1, r1.w\n"

				"; dot product the eye vector with the normal and output as the second texture coordinate\n"
				"dp3 oT1.x, v3, r1\n"

				"; final color\n"
				"mov oD0, v5\n"
			};

			// now attempt to assemble and create the shader
			ID3DXBuffer* pShaderBuffer = NULL;
			
			// call the assemble function
			if ( FAILED ( D3DXAssembleShader ( szShader, strlen ( szShader ), 0, NULL, &pShaderBuffer, NULL ) ) )
			{
				// should use runtime errors for these - lee can I add to the list of errors?
				Error ( "Basic3D - cartoon shader setup - failed to assemble shader" );
				return;
			}

			// create the shader
			if ( FAILED ( m_pD3D->CreateVertexShader ( dwDeclaration, ( DWORD* ) pShaderBuffer->GetBufferPointer ( ), &dwShader, 0 ) ) )
			{
				// should use runtime errors for these - lee can I add to the list of errors?
				Error ( "Basic3D - cartoon shader setup - failed to create vertex shader" );
				return;
			}

			// release the buffer
			pShaderBuffer->Release ( );

			// get the textures
			m_pToonTexture = ( LPDIRECT3DTEXTURE8 ) pStandardImage;
			m_pEdgeTexture = ( LPDIRECT3DTEXTURE8 ) pEdgeImage;

			// setup the init value now we're ok to run
			bInit = true;
		};

		void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir ) {}
		void Start ( D3DXMATRIX matObject )
		{
			// pass data across to vertex shader

			// variable declarations
			D3DXMATRIX  matView,			// view matrix
						matProjection,		// projection matrix
						matShader;			// final shader matrix

			D3DXMATRIX	InverseWorld;		// inverse world matrix

			// eye position, need to allow changes for this
			D3DXVECTOR4 EyePos ( 0.0, 0.0f, 0.0f, 0.0f );

			// get view and projection matrix
			m_pD3D->GetTransform ( D3DTS_VIEW,       &matView );
			m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProjection );

			// build final matrix
			matShader = matObject * matView * matProjection;

			// get the transpose
			D3DXMatrixTranspose ( &matShader, &matShader );

			// send the matrix across to the shader
			m_pD3D->SetVertexShaderConstant ( 0, &matShader, 4 );

			// Always use light zero  (MIKE: light does not seem to be used in shader!!)
			D3DLIGHT8 lightzero;
			D3DXVECTOR4 LightDir;
			m_pD3D->GetLight( 0, &lightzero );
			LightDir.x = matObject._41-lightzero.Position.x;
			LightDir.y = matObject._42-lightzero.Position.y;
			LightDir.z = matObject._43-lightzero.Position.z;
			LightDir.w = 0.0f;
			
			// get the inverse matrix
			D3DXMatrixInverse ( &InverseWorld, NULL, &matObject );
			D3DXVec4Transform ( &LightDir, &LightDir, &InverseWorld );
			D3DXVec4Transform ( &EyePos, &EyePos, &InverseWorld );
			m_pD3D->SetVertexShaderConstant ( 4, &LightDir, 1 );

			// send light and eye pos across
			m_pD3D->SetVertexShaderConstant ( 5, &LightDir, 1 );
			m_pD3D->SetVertexShaderConstant ( 6, &EyePos,   1 );
			
			// set textures
			m_pD3D->SetTexture ( 0, m_pToonTexture );	// normal texture
			m_pD3D->SetTexture ( 1, m_pEdgeTexture );	// edge texture

			// setup render states
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

			// cant get the toon sahdeer to look anything ...
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
		}

		void End ( void )
		{
			// cleanup
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );

			// set both textures to null
			m_pD3D->SetTexture ( 0, NULL );
			m_pD3D->SetTexture ( 1, NULL );
		}

		void Mesh ( LPD3DXMESH pMesh )
		{
			LPDIRECT3DVERTEXBUFFER8 m_pMeshVertexBuffer;
			LPDIRECT3DINDEXBUFFER8  m_pMeshIndexBuffer;

			// get pointers to vertex and index buffer
			pMesh->GetVertexBuffer ( &m_pMeshVertexBuffer );
			pMesh->GetIndexBuffer  ( &m_pMeshIndexBuffer );

			// our mesh structure
			struct MESH_VERTEX
			{
				float x, y, z;
				float nx, ny, nz;
				DWORD color;
			};

			// store the size of the structure
			dwFVFSize = sizeof ( MESH_VERTEX );

			MESH_VERTEX *pMeshVertices;
			short       *pIndices;
			DWORD       *pAttribs;

			// lock the buffer
			m_pMeshVertexBuffer->Lock ( 0, pMesh->GetNumVertices ( ) * sizeof ( MESH_VERTEX ), ( BYTE** ) &pMeshVertices, 0 );

			// get the indices
			m_pMeshIndexBuffer->Lock ( 0, 3 * pMesh->GetNumFaces ( ) * sizeof ( short ), ( BYTE** ) &pIndices, D3DLOCK_READONLY );

			// the attribute buffer maps the materials to each face.
			pMesh->LockAttributeBuffer ( D3DLOCK_READONLY, &pAttribs );

			// setup diffuse, we need a function to let the user change this themselves
			for ( long Face = 0; Face < (long)pMesh->GetNumFaces ( ); Face++ )
			{
				// get the diffuse color
				D3DXCOLOR Diffuse = D3DCOLOR_ARGB(255,255,255,255);

				pMeshVertices [ pIndices [ Face * 3 + 0 ] ].color = Diffuse;
				pMeshVertices [ pIndices [ Face * 3 + 1 ] ].color = Diffuse;
				pMeshVertices [ pIndices [ Face * 3 + 2 ] ].color = Diffuse;
			}
			
			// unlock buffers
			m_pMeshVertexBuffer->Unlock  ( );
			m_pMeshIndexBuffer->Unlock   ( );
			pMesh->UnlockAttributeBuffer ( );

			// release interfaces
			m_pMeshVertexBuffer->Release();
			m_pMeshIndexBuffer->Release();
		}
};

class cBumpMapping : public cSpecialEffect
{
	// bump mapping

	private:
		DWORD*				m_dwDeclaration;	// shader declaration
		LPDIRECT3DTEXTURE8	m_pBumpMap;			// bump map texture
		DWORD				m_dwFVF;

	public:
		~cBumpMapping ( )
		{
		}
		void Free( void )
		{
			// delete shader
			m_pD3D->DeleteVertexShader ( dwShader );

			// delete declaration
			SAFE_DELETE_ARRAY ( m_dwDeclaration );
		}

		int GetBaseTextureSlot(void) { return 1; }
		DWORD* GetDeclaration(void) { return m_dwDeclaration; }
		
		
		void Setup ( void* pStandardImage, void* pEdgeImage )
		{
			// create the shader

			// shader declaration data
			DWORD dwDeclaration [ ] =
			{
				D3DVSD_STREAM ( 0 ),								// begin stream
				D3DVSD_REG ( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),	// position
				D3DVSD_REG ( D3DVSDE_NORMAL,    D3DVSDT_FLOAT3 ),	// normal
				D3DVSD_REG ( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ),	// tex coord 0
				D3DVSD_REG ( D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT3 ),	// tex coord 1
				D3DVSD_REG ( D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT3 ),	// tex coord 2
				D3DVSD_REG ( D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT3 ),	// tex coord 3
				D3DVSD_END ( )										// end stream
			};

			// setup space to store the stream
			m_dwDeclaration = new DWORD [ sizeof ( dwDeclaration ) / sizeof ( DWORD ) ];

			// copy the stream data across, we need it later on
			memcpy ( m_dwDeclaration, dwDeclaration, sizeof ( dwDeclaration ) );

			// setup the actual shader
			char szShader [ ] =
			{
				"vs.1.1\n"

				"; do the final transformation and emit it in oPos, this\n"
				"; is completely independent of the lighting operations\n"
				"dp4 oPos.x, v0, c0\n"
				"dp4 oPos.y, v0, c1\n"
				"dp4 oPos.z, v0, c2\n"
				"dp4 oPos.w, v0, c3\n"

				"; transform light vector by the transformed basis vectors\n"
				"dp3 r0.x, v8.xyz,  -c4\n"
				"dp3 r0.y, v9.xyz,  -c4\n"
				"dp3 r0.z, v10.xyz, -c4\n"

				"; normalize the newly transformed light vector\n"
				"dp3 r0.w, r0, r0\n"
				"rsq r0.w, r0.w\n"
				"mul r0, r0, r0.w\n"

				"; place it in the range from zero to one and place it in the \n"
				"; diffuse color.\n"
				"mad oD0, r0, c6.x, c6.x\n"

				"; output the texture coordinates\n"
				"mov oT0, v7\n"
				"mov oT1, v7\n"
			};

			// now attempt to assemble and create the shader
			ID3DXBuffer* pShaderBuffer = NULL;
			
			// call the assemble function
			if ( FAILED ( D3DXAssembleShader ( szShader, strlen ( szShader ), 0, NULL, &pShaderBuffer, NULL ) ) )
			{
				Error ( "Basic3D - bump mapping shader setup - failed to assemble shader" );
				return;
			}

			// create the shader
			if ( FAILED ( m_pD3D->CreateVertexShader ( dwDeclaration, ( DWORD* ) pShaderBuffer->GetBufferPointer ( ), &dwShader, 0 ) ) )
			{
				Error ( "Basic3D - bump mapping shader setup - failed to create vertex shader" );
				return;
			}

			// release the buffer
			pShaderBuffer->Release ( );

			// store the texture
			m_pBumpMap = ( LPDIRECT3DTEXTURE8 ) pStandardImage;

			// set init to true because everything went ok
			bInit = true;
		};

		void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir ) {}
		void Start ( D3DXMATRIX matObject )
		{
			// pass data to shader

			D3DXMATRIX  matView,		// view matrix
						matProjection,	// projection matrix
						matShader;		// final shader matrix
			D3DXMATRIX	InverseWorld;	// inverse world matrix

			// eye position
			D3DXVECTOR4 EyePos ( 0.0, 0.0f, 0.0f, 0.0f );

			// get view and projection transforms
			m_pD3D->GetTransform ( D3DTS_VIEW,       &matView );
			m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProjection );

			// build up final transformation
			matShader = matObject * matView * matProjection;

			// get transpose of matrix
			D3DXMatrixTranspose ( &matShader, &matShader );

			// pass the matrix across
			m_pD3D->SetVertexShaderConstant ( 0, &matShader, 4 );

			// setup ambient light, we need to let the user change this
			D3DXVECTOR4 Ambient ( 0.1f, 0.1f, 0.1f, 0.0f );

			// pass ambient light
			m_pD3D->SetVertexShaderConstant ( 5, &Ambient, 1 );

			// Always use light zero
			D3DLIGHT8 lightzero;
			D3DXVECTOR4 LightDir;
			m_pD3D->GetLight( 0, &lightzero );
			LightDir.x = matObject._41-lightzero.Position.x;
			LightDir.y = matObject._42-lightzero.Position.y;
			LightDir.z = matObject._43-lightzero.Position.z;
			LightDir.w = 0.0f;

			//LightDir.x = 0.0f;
			//LightDir.y = -1.0f;
			//LightDir.z = 0.0f;
			//LightDir.w = 0.0f;
			
			// get the inverse matrix
			D3DXMatrixInverse ( &InverseWorld, NULL, &matObject );
			D3DXVec4Transform ( &LightDir, &LightDir, &InverseWorld );
			m_pD3D->SetVertexShaderConstant ( 4, &LightDir, 1 );

			// we need to pass some more data
			D3DXVECTOR4 Helpers ( 0.5f, 0.0f, 0.0f, 0.0f );
			m_pD3D->SetVertexShaderConstant ( 6, &Helpers, 1 );

			// base texture in place, setup stage 1 for bumpmap detail
			m_pD3D->SetTexture ( 0, m_pBumpMap );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		}

		void End ( void )
		{
//			// cleanup
//			m_pD3D->SetTexture ( 0, NULL );
//			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		}

		void Mesh ( LPD3DXMESH pMesh )
		{
			// modify lots of data
			LPDIRECT3DVERTEXBUFFER8 m_pMeshVertexBuffer;
			LPDIRECT3DINDEXBUFFER8  m_pMeshIndexBuffer;

			// get vertex and index buffer
			pMesh->GetVertexBuffer ( &m_pMeshVertexBuffer );
			pMesh->GetIndexBuffer  ( &m_pMeshIndexBuffer );

			// vertex data
			struct MESH_VERTEX
			{
				float x, y, z;
				float nx, ny, nz;
				float u, v;
				float Ux, Uy, Uz;
				float Vx, Vy, Vz;
				float UVx, UVy, UVz;
			};

			// store the size
			dwFVFSize = sizeof ( MESH_VERTEX );

			MESH_VERTEX *pMeshVertices;
			short       *pIndices;
			DWORD       *pAttribs;

			// lock the vertex buffer
			m_pMeshVertexBuffer->Lock ( 0, pMesh->GetNumVertices ( ) * sizeof ( MESH_VERTEX ), ( BYTE** ) &pMeshVertices, 0 );

			// get the indices
			m_pMeshIndexBuffer->Lock ( 0, 3 * pMesh->GetNumFaces ( ) * sizeof ( short ), ( BYTE** ) &pIndices, D3DLOCK_READONLY );

			// the attribute buffer maps the materials to each face.
			pMesh->LockAttributeBuffer ( D3DLOCK_READONLY, &pAttribs );

			for ( long i = 0; i < (long)pMesh->GetNumFaces ( ) * 3; i += 3 )
			{
				// get vertices
				MESH_VERTEX *pVertex1 = &pMeshVertices [ pIndices [ i     ] ];
				MESH_VERTEX *pVertex2 = &pMeshVertices [ pIndices [ i + 1 ] ];
				MESH_VERTEX *pVertex3 = &pMeshVertices [ pIndices [ i + 2 ] ];

				// compute the U and V in terms of X gradients
				D3DXVECTOR3 Side1 = D3DXVECTOR3 ( pVertex2->x - pVertex1->x, pVertex2->u - pVertex1->u, pVertex2->v - pVertex1->v );
				D3DXVECTOR3 Side2 = D3DXVECTOR3 ( pVertex3->x - pVertex1->x, pVertex3->u - pVertex1->u, pVertex3->v - pVertex1->v );

				// get the cross product to find the X component of the U and V vectors
				D3DXVECTOR3 CrossProduct;
				D3DXVec3Cross ( &CrossProduct, &Side1, &Side2 );

				D3DXVECTOR3 U;
				D3DXVECTOR3 V;

				// set the X components
				U.x = -CrossProduct.y / CrossProduct.x;
				V.x = -CrossProduct.z / CrossProduct.x;

				// repeat for y
				Side1 = D3DXVECTOR3 ( pVertex2->y - pVertex1->y, pVertex2->u - pVertex1->u, pVertex2->v - pVertex1->v );
				Side2 = D3DXVECTOR3 ( pVertex3->y - pVertex1->y, pVertex3->u - pVertex1->u, pVertex3->v - pVertex1->v );

				// get the cross product to find the Y component of the U and V vectors
				D3DXVec3Cross ( &CrossProduct, &Side1, &Side2 );

				// set the Y components
				U.y = -CrossProduct.y / CrossProduct.x;
				V.y = -CrossProduct.z / CrossProduct.x;

				// repeat for z
				Side1 = D3DXVECTOR3 ( pVertex2->z - pVertex1->z, pVertex2->u - pVertex1->u, pVertex2->v - pVertex1->v);
				Side2 = D3DXVECTOR3 ( pVertex3->z - pVertex1->z, pVertex3->u - pVertex1->u, pVertex3->v - pVertex1->v );

				D3DXVec3Cross ( &CrossProduct, &Side1, &Side2 );

				U.z = -CrossProduct.y / CrossProduct.x;
				V.z = -CrossProduct.z / CrossProduct.x;

  				// normalize the U and V vectors		
				D3DXVec3Normalize ( &U, &U );
  				D3DXVec3Normalize ( &V, &V );

				// now, compute the UxV vector
				D3DXVECTOR3 UxV;
				D3DXVec3Cross ( &UxV, &U, &V );

				// make sure UxT is in the same direction as the normal
				if ( D3DXVec3Dot ( &UxV, &D3DXVECTOR3 ( pVertex1->nx, pVertex1->ny, pVertex1->nz ) ) < 0.0f )
					UxV = -UxV;

				// now set each vertex component to the vectors
				pVertex1->Ux  = pVertex2->Ux  = pVertex3->Ux = U.x;
				pVertex1->Uy  = pVertex2->Uy  = pVertex3->Uy = U.y;
				pVertex1->Uz  = pVertex2->Uz  = pVertex3->Uz = U.z;
				pVertex1->Vx  = pVertex2->Vx  = pVertex3->Vx = V.x;
				pVertex1->Vy  = pVertex2->Vy  = pVertex3->Vy = V.y;
				pVertex1->Vz  = pVertex2->Vz  = pVertex3->Vz = V.z;
				pVertex1->UVx = pVertex2->UVx = pVertex3->UVx = UxV.x;
				pVertex1->UVy = pVertex2->UVy = pVertex3->UVy = UxV.y;
				pVertex1->UVz = pVertex2->UVz = pVertex3->UVz = UxV.z;
			}
			
			// unlock buffers
			m_pMeshVertexBuffer->Unlock  ( );
			m_pMeshIndexBuffer->Unlock   ( );
			pMesh->UnlockAttributeBuffer ( );

			// release interfaces
			m_pMeshVertexBuffer->Release();
			m_pMeshIndexBuffer->Release();
		}
};

class cRainbow : public cSpecialEffect
{
	// rainbow rendering

	private:
		DWORD*				m_dwDeclaration;	// shader declaration
		LPDIRECT3DTEXTURE8	m_pShadeTexture;

	public:
		~cRainbow ( )
		{
		}
		void Free( void )
		{
			// delete shader
			m_pD3D->DeleteVertexShader ( dwShader );

			// delete declaration
			SAFE_DELETE_ARRAY ( m_dwDeclaration );
		}

		int GetBaseTextureSlot(void) { return 2; }
		DWORD* GetDeclaration(void) { return m_dwDeclaration; }

		void Setup ( void* pStandardImage, void* pEdgeImage )
		{
			// create the shader

			// shader declaration data
			DWORD dwDeclaration [ ] =
			{
				D3DVSD_STREAM ( 0 ),								// begin stream
				D3DVSD_REG ( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),	// position
				D3DVSD_REG ( D3DVSDE_NORMAL,    D3DVSDT_FLOAT3 ),	// normal
				D3DVSD_REG ( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT1 ),	// tex coord 0
				D3DVSD_REG ( D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT1 ),	// tex coord 1
				D3DVSD_END ( )										// end stream
			};

			// setup space to store the strem
			m_dwDeclaration = new DWORD [ sizeof ( dwDeclaration ) / sizeof ( DWORD ) ];

			// copy the stream data across, we need it later on
			memcpy ( m_dwDeclaration, dwDeclaration, sizeof ( dwDeclaration ) );

			// setup the actual shader
			char szShader [ ] =
			{
				"; v0  -- position\n"
				"; v3  -- normal\n"
				"; v7  -- tex coord\n"
				"; v8 -- tex coord1\n"
				";\n"
				"; c0-3   -- world/view/proj matrix\n"
				"; c4     -- light vector\n"
				"; c5-8   -- inverse/transpose world matrix\n"
				"; c9     -- {0.0, 0.5, 1.0, -1.0}\n"
				"; c10    -- eye point\n"
				"; c11-14 -- world matrix\n"

				"vs.1.0\n"

				";transform position\n"
				"dp4 oPos.x, v0, c0\n"
				"dp4 oPos.y, v0, c1\n"
				"dp4 oPos.z, v0, c2\n"
				"dp4 oPos.w, v0, c3\n"

				";transform normal\n"
				"dp3 r0.x, v3, c5\n"
				"dp3 r0.y, v3, c6\n"
				"dp3 r0.z, v3, c7\n"

				";normalize normal\n"
				"dp3 r0.w, r0, r0\n"
				"rsq r0.w, r0.w\n"
				"mul r0, r0, r0.w\n"

				";compute world space position\n"
				"dp4 r1.x, v0, c11\n"
				"dp4 r1.y, v0, c12\n"
				"dp4 r1.z, v0, c13\n"
				"dp4 r1.w, v0, c14\n"

				";vector from point to eye\n"
				"add r2, c10, -r1\n"

				";normalize e\n"
				"dp3 r2.w, r2, r2\n"
				"rsq r2.w, r2.w\n"
				"mul r2, r2, r2.w\n"

				";h = Normalize( n + e )\n"
				"add r1, r0, r2\n"

				";normalize h\n"
				"dp3 r1.w, r1, r1\n"
				"rsq r1.w, r1.w\n"
				"mul r1, r1, r1.w\n"

				";h dot n\n"
				"dp3 oT0.x, r0, r2\n"
				"dp3 oT0.y, r1, r0\n"

				"mov oT1.x, r2\n"
				"mov oT1.y, r1\n"
			};

			// now attempt to assemble and create the shader
			ID3DXBuffer* pShaderBuffer = NULL;
			
			// call the assemble function
			if ( FAILED ( D3DXAssembleShader ( szShader, strlen ( szShader ), 0, NULL, &pShaderBuffer, NULL ) ) )
			{
				Error ( "Basic3D - rainbow shader setup - failed to assemble shader" );
				return;
			}

			// create the shader
			if ( FAILED ( m_pD3D->CreateVertexShader ( dwDeclaration, ( DWORD* ) pShaderBuffer->GetBufferPointer ( ), &dwShader, 0 ) ) )
			{
				Error ( "Basic3D - rainbow shader setup - failed to create vertex shader" );
				return;
			}

			// release the buffer
			pShaderBuffer->Release ( );

			// store the texture
			m_pShadeTexture = ( LPDIRECT3DTEXTURE8 ) pStandardImage;

			// set init to true because everything went ok
			bInit = true;
		};

		void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir ) {}
		void Start ( D3DXMATRIX matObject )
		{
			D3DXVECTOR3 eye, lookAt, up;
	
			eye.x    = 0.0f; eye.y	  = 0.0f; eye.z	   = -28.0f;
			lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
			up.x	 = 0.0f; up.y 	  = 1.0f; up.z	   = 0.0f;

			m_pD3D->SetVertexShaderConstant ( 10, &eye, 1 );
	
			D3DXMATRIX	matView,
						matProjection;

			m_pD3D->GetTransform ( D3DTS_VIEW,       &matView );
			m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProjection );
			
			m_pD3D->SetVertexShaderConstant ( 9, D3DXVECTOR4 ( 0.0f, 0.5f, 1.0f, -1.0f ), 1 );
			D3DXVECTOR4 lightDir ( 1.0f, 0.0f, -1.0f, 0.0f );
			D3DXVec4Normalize ( &lightDir, &lightDir );
			
			m_pD3D->SetVertexShaderConstant ( 4, &lightDir, 1 );
			
			D3DXVECTOR4 constNums ( 0.0f, 0.5f, 1.0f, -1.0f );
			m_pD3D->SetVertexShaderConstant ( 9, &constNums, 1 );

			D3DXMATRIX	mvpMat,
						worldITMat,
						worldMat;
			
			D3DXMATRIX tempMat;
			
			tempMat = matObject;

			D3DXMatrixTranspose ( &worldMat,   &tempMat );
			D3DXMatrixIdentity  ( &mvpMat );
			D3DXMatrixMultiply  ( &mvpMat,     &tempMat, &matView );
			D3DXMatrixInverse   ( &worldITMat, NULL,     &tempMat );
			D3DXMatrixMultiply  ( &mvpMat,     &mvpMat,  &matProjection );
			D3DXMatrixTranspose ( &mvpMat,     &mvpMat );
			
			m_pD3D->SetVertexShaderConstant (  0, &mvpMat,     4 );
			m_pD3D->SetVertexShaderConstant (  5, &worldITMat, 4 );
			m_pD3D->SetVertexShaderConstant ( 11, &worldMat,   4 );
			//////////////////////////////////////////////////////////////
		
			// set render states
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,          D3DZB_TRUE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
			m_pD3D->SetRenderState ( D3DRS_LIGHTING,         FALSE );
			m_pD3D->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_CCW );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_ZERO );

			m_pD3D->SetTexture ( 0, m_pShadeTexture );
			m_pD3D->SetTexture ( 1, m_pShadeTexture );

			m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
			
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_MIRROR );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

			m_pD3D->SetTextureStageState ( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

			// we're going to pass the material color in through tfactor
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );

			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );

			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			
			// and modulate this resulting color with the edge texture ( which should be white everywhere but on the edge )
			m_pD3D->SetTextureStageState ( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
		}

		void End ( void )
		{
			m_pD3D->SetTexture ( 0, NULL );
			m_pD3D->SetTexture ( 1, NULL );
		}

		void Mesh ( LPD3DXMESH pMesh )
		{
			// all we need to do here is to store the size of the vertex setup

			// vertex data
			struct MESH_VERTEX
			{
				float x, y, z;
				float nx, ny, nz;
				float u0;
				float u1;
			};

			// store the size
			dwFVFSize = sizeof ( MESH_VERTEX );
		}
};

class cShadowVolume : public cSpecialEffect
{
	// real time shadows using the stencil buffer

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

					pMesh->LockVertexBuffer ( 0L, ( BYTE** ) &pVertices );
					pMesh->LockIndexBuffer  ( 0L, ( BYTE** ) &pIndices  );

					DWORD dwNumVertices = pMesh->GetNumVertices ( );
					DWORD dwNumFaces    = pMesh->GetNumFaces    ( );

					WORD* pEdges = new WORD [ dwNumFaces * 6 ];
					DWORD dwNumEdges = 0;

					for ( DWORD i = 0; i < dwNumFaces; i++ )
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

				void Render ( LPDIRECT3DDEVICE8 pD3D )
				{
					pD3D->SetVertexShader ( D3DFVF_XYZ );

					pD3D->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, m_dwNumVertices / 3, m_pVertices, sizeof ( D3DXVECTOR3 ) );
				}
		};

		DWORD*					m_dwDeclaration;	// shader declaration
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

		~cShadowVolume ( )
		{
		}
		void Free( void )
		{
			SAFE_DELETE       ( m_pMeshListStart );
			SAFE_DELETE_ARRAY ( m_dwDeclaration );
		}

		int GetBaseTextureSlot ( void )
		{ 
			// we need to return 0 here so that the object uses it's own textures
			return 0;
		}
		
		DWORD* GetDeclaration ( void )
		{ 
			return m_dwDeclaration;
		}

		void Setup ( void* pStandardImage, void* pEdgeImage )
		{
			m_iMeshCount = 0;

			// shader declaration data
			DWORD dwDeclaration [ ] =
			{
				D3DVSD_STREAM ( 0 ),								// begin stream
				D3DVSD_REG ( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),	// position
				D3DVSD_REG ( D3DVSDE_NORMAL,    D3DVSDT_FLOAT3 ),	// normal
				D3DVSD_REG ( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT1 ),	// tex coord 0
				D3DVSD_REG ( D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT1 ),	// tex coord 1
				D3DVSD_END ( )										// end stream
			};

			// setup space to store the strem
			m_dwDeclaration = new DWORD [ sizeof ( dwDeclaration ) / sizeof ( DWORD ) ];

			// copy the stream data across, we need it later on
			memcpy ( m_dwDeclaration, dwDeclaration, sizeof ( dwDeclaration ) );

			dwShader = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

			// set init to true because everything went ok
			bInit = true;
		};

		void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir ) {}
		void Start ( D3DXMATRIX matObject )
		{
			// If this stencil effect active
			if(g_pGlob->dwStencilMode==1)
			{
				// direction of the light
				float Lx, Ly, Lz;

				// Always use light zero
				D3DLIGHT8 lightzero;
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
				

				//float Lx = matObject._41 + 5.0f;
				//float Ly = matObject._42 + 5.0f;
				//float Lz = matObject._43 - 5.0f;

				//float Lx = 5.0f;
				//float Ly = 5.0f;
				//float Lz = -5.0f;
				
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
		}

		void End ( void )
		{
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

		void Mesh ( LPD3DXMESH pMesh )
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
			dwFVFSize = sizeof ( MESH_VERTEX );

			if ( m_iMeshCount == 0 )
			{
				// start the list off
				m_pMeshListStart		= new sMeshList;	// create a new link
				m_pMeshListStart->pMesh = pMesh;			// store pointer to the mesh
				m_pMeshListCurrent		= m_pMeshListStart;	// keep a pointer to the current
				m_iMeshCount++;								// increment the mesh count
			}
			else
			{
				// add to the list
				m_pMeshListCurrent->pNext = new sMeshList;				// create a new link
				m_pMeshListCurrent        = m_pMeshListCurrent->pNext;	// move to the next link
				m_pMeshListCurrent->pMesh = pMesh;						// store pointer to the mesh
				m_iMeshCount++;											// increment the mesh count
			}
		}
};

class cReflection : public cSpecialEffect
{
	// real time reflections using the stencil buffer

	private:
		DWORD*					m_dwDeclaration;

		/*
		LPDIRECT3DVERTEXBUFFER8 m_pMirrorVB;
		D3DXMATRIX				m_matMirrorMatrix;
		D3DXMATRIX				m_matViewSaved;

		struct MIRRORVERTEX
		{
			D3DXVECTOR3 p;
			D3DXVECTOR3 n;
		};

		#define D3DFVF_MIRRORVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL )
		*/
		
	public:

		cReflection ( )
		{
		}
		~cReflection ( )
		{
		}
		void Free( void )
		{
			SAFE_DELETE_ARRAY ( m_dwDeclaration );
		}

		int GetBaseTextureSlot ( void )
		{ 
//			// we need to return 0 here so that the object uses it's own texture
			// no normal texture use
			return 2;
		}
		
		DWORD* GetDeclaration ( void )
		{ 
			return m_dwDeclaration;
		}

		void Setup ( void* pStandardImage, void* pEdgeImage )
		{
			// shader declaration data
			DWORD dwDeclaration [ ] =
			{
				D3DVSD_STREAM ( 0 ),								// begin stream
				D3DVSD_REG ( D3DVSDE_POSITION,  D3DVSDT_FLOAT3 ),	// position
				D3DVSD_END ( )										// end stream
			};

			// setup space to store the strem
			m_dwDeclaration = new DWORD [ sizeof ( dwDeclaration ) / sizeof ( DWORD ) ];

			// copy the stream data across, we need it later on
			memcpy ( m_dwDeclaration, dwDeclaration, sizeof ( dwDeclaration ) );

			dwShader = D3DFVF_XYZ;

			/*
		    m_pD3D->CreateVertexBuffer (
											4 * sizeof ( MIRRORVERTEX ),
											D3DUSAGE_WRITEONLY,
											D3DFVF_MIRRORVERTEX,
											D3DPOOL_MANAGED,
											&m_pMirrorVB
									   );
			MIRRORVERTEX* v;
			m_pMirrorVB->Lock ( 0, 0, ( BYTE** ) &v, 0 );

//				v [ 0 ].p = D3DXVECTOR3 ( -180.0f, 0.0f, 80.0f );
//				v [ 0 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
//				v [ 1 ].p = D3DXVECTOR3 ( -180.0f,  50.0f, 80.0f );
//				v [ 1 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
//				v [ 2 ].p = D3DXVECTOR3 (  180.0f, 0.0f, 80.0f );
//				v [ 2 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
//				v [ 3 ].p = D3DXVECTOR3 (  180.0f,  50.0f, 80.0f );
//				v [ 3 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
				v [ 0 ].p = D3DXVECTOR3 ( -100.0f, -100.0f, 280.0f );
				v [ 0 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
				v [ 1 ].p = D3DXVECTOR3 ( -100.0f, 100.0f,  280.0f );
				v [ 1 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
				v [ 2 ].p = D3DXVECTOR3 (  100.0f, -100.0f, 280.0f );
				v [ 2 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );
				v [ 3 ].p = D3DXVECTOR3 (  100.0f, 100.0f,  280.0f );
				v [ 3 ].n = D3DXVECTOR3 (   0.0f, 0.0f,   -1.0f );

			m_pMirrorVB->Unlock ( );
			*/
			
			// set init to true because everything went ok
			bInit = true;
		};

		void Start ( D3DXMATRIX matObject ) {}
		void Start ( D3DXVECTOR3 vecPos, D3DXVECTOR3 vecDir )
		{
			// If this stencil effect active
			if(g_pGlob->dwStencilMode==2)
			{
				// Two Phases for Reflection Rendering
				if(g_pGlob->dwRedrawPhase==0)
				{
					// A : Cut into stencil buffer with plane(object)

					// set position and direction of reflection plane
					D3DXPlaneFromPointNormal ( &g_plnReflectionPlane, &vecPos, &vecDir );

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
		}

		bool Mid ( void )
		{
			// If this stencil effect active
			if(g_pGlob->dwStencilMode==2)
			{
				// Two Phases for Reflection Rendering
				if(g_pGlob->dwRedrawPhase==0)
				{
					/*
					// A : Cut into stencil buffer with plane(object)

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
					*/
				}
			}
			return true;
		}

		void End ( void )
		{
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
		}

		void Mesh ( LPD3DXMESH pMesh )
		{
			// retain a list of all meshes

			// vertex data
			struct MESH_VERTEX
			{
				float x, y, z;
			};

			// store the size
			dwFVFSize = sizeof ( MESH_VERTEX );
		}
};


