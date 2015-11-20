//////////////////////////////////////////////////////////////////////////////////
// information ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
	this file ( "sMesh.h" ) and "sMesh.cpp" contain a structure and it's
	functions which are responsible for storing mesh information for a model
*/
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// MESH STRUCTURE ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#ifndef _SMESH_H_
#define _SMESH_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#pragma comment ( lib, "d3dx8.lib"  )
//#pragma comment ( lib, "d3dxof.lib" )
//#pragma comment ( lib, "winmm.lib"  )

//#pragma comment ( lib, "dxguid.lib" )
//#pragma comment ( lib, "d3d8.lib"   )
//#pragma comment ( lib, "dxerr8.lib" )

//#include <d3d8.h>
#include <math.h>
//#include <D3DX8.h>

#define WIN32_LEAN_AND_MEAN
#include < windows.h >
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define GXRELEASE(_p) do       { if ((_p) != NULL) {(_p)->Release(); (_p) = NULL;} } while (0)

#define DEBUG_MODE_MESH	0
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct sMesh
{
	// stores mesh information

	char*				m_Name;				// mesh name

	ID3DXMesh*			m_Mesh;				// mesh object
	ID3DXSkinMesh*		m_SkinMesh;			// skinned mesh object

	DWORD               m_NumMaterials;		// number of materials in mesh
	D3DMATERIAL8*		m_Materials;		// array of materials
	IDirect3DTexture8** m_Textures;			// array of textures

	char				m_TextureName [ 256 ];

	DWORD               m_NumBones;			// number of bones
	ID3DXBuffer*		m_BoneNames;		// names of bones
	ID3DXBuffer*		m_BoneTransforms;	// internal transformations

	D3DXMATRIX*			m_Matrices;			// bone matrices
	D3DXMATRIX**		m_FrameMatrices;	// pointers to frame matrices
	D3DXMATRIX*			m_BoneMatrices;		// x file bone matrices

	D3DXVECTOR3         m_Min;				// min bounds
	D3DXVECTOR3			m_Max;				// max bounds
	float               m_Radius;			// Bounding sphere

	D3DPRIMITIVETYPE	m_iPrimType;		// when mesh modifying, this is required

	sMesh*				m_Next;				// Next mesh in list

	D3DXATTRIBUTERANGE* m_pAttributeTable;
	DWORD				m_dwAttributeTableSize;

	#if DEBUG_MODE_MESH
		LPD3DXMESH      m_pMeshBox;
		D3DXMATRIX      m_matBox;
	#endif

		D3DXVECTOR3		m_vecMin;
		D3DXVECTOR3		m_vecMax;

	// functions in structure

	sMesh  ( );															// constructor
	~sMesh ( );															// destructor

	sMesh*		       FindMesh                ( char* Name );			// gets a pointer to a mesh
	DWORD              GetNumMaterials         ( void );				// get the number of materials used
	D3DMATERIAL8*      GetMaterial             ( unsigned long Num );	// get material data
	IDirect3DTexture8* GetTexture              ( unsigned long Num );	// get a pointer to a texture
	void               CopyFrameToBoneMatrices ( void );				// copy frame matrices
};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _SMESH_H_