
#include "sMesh.h"

sMesh::sMesh ( )
{
	// clear out variables and set to default values

	m_Name           = NULL;
	m_Mesh           = NULL;
	m_SkinMesh       = NULL;
	m_Materials      = NULL;
	m_Textures       = NULL;
	m_BoneNames      = NULL;
	m_BoneTransforms = NULL;
	m_Matrices       = NULL;
	m_FrameMatrices  = NULL;
	m_pAttributeTable= NULL;
	m_Next           = NULL;
	m_NumBones       = 0;
	m_NumMaterials   = 0;
	m_Min.x			 = 0.0f;
	m_Min.y			 = 0.0f;
	m_Min.z			 = 0.0f;
	m_Max.x			 = 0.0f;
	m_Max.y			 = 0.0f;
	m_Max.z			 = 0.0f;
	m_Radius         = 0.0f;

	memset ( m_TextureName, 0, sizeof ( m_TextureName ) );

	#if DEBUG_MODE_MESH
		m_pMeshBox		= NULL;
	#endif
}

sMesh::~sMesh ( )
{
	// delete the name
	SAFE_DELETE_ARRAY ( m_Name );

	// both mesh objects
	SAFE_RELEASE ( m_Mesh );
	SAFE_RELEASE ( m_SkinMesh );

	// materials array
	SAFE_DELETE_ARRAY ( m_Materials );

	// any textures (but not texture interfaces - job of imageDLL)
	if ( m_Textures != NULL )
	{
		SAFE_DELETE_ARRAY ( m_Textures );
	}

	// bone information
	SAFE_RELEASE ( m_BoneNames );
	SAFE_RELEASE ( m_BoneTransforms );

	// matrices
	SAFE_DELETE_ARRAY ( m_Matrices );
	SAFE_DELETE_ARRAY ( m_FrameMatrices );
	SAFE_DELETE_ARRAY ( m_pAttributeTable );
	
	// pointer to next mesh
	SAFE_DELETE ( m_Next );
}

sMesh* sMesh::FindMesh ( char* Name )
{
	// gets a pointer to the specified mesh

	sMesh* Mesh = NULL;

	// if we don't have a name return the first mesh
	if ( Name == NULL )
		return this;

	// compare names and return if exact match
	if ( m_Name != NULL && !strcmp ( Name, m_Name ) )
		return this;

	// search next in list
	if ( m_Next != NULL )
	{
		// see if it's the next mesh
		if ( ( Mesh = m_Next->FindMesh ( Name ) ) != NULL )
			return Mesh;
	}

	// if we don't find a match return null
	return NULL;
}

DWORD sMesh::GetNumMaterials ( void )
{
	// returns the number of materials in a mesh
	return m_NumMaterials;
}

D3DMATERIAL8* sMesh::GetMaterial ( unsigned long Num )
{
	// get a pointer to material data

	// if the specified index is more than the number of materials
	// or we don't have any materials then return null
	if ( Num >= m_NumMaterials || m_Materials == NULL )
		return NULL;

	// return the material data
	return &m_Materials [ Num ];
}

IDirect3DTexture8* sMesh::GetTexture ( unsigned long Num )
{
	// get a pointer to a texture

	// if the specified index is more than the number of materials
	// or we don't have any materials then return null
	if ( Num >= m_NumMaterials || m_Textures == NULL )
		return NULL;

	// return the texture data
	return m_Textures [ Num ];
}

void sMesh::CopyFrameToBoneMatrices ( void )
{
	// copy all matrices from frames to local bone matrix array

	// used for loops
	DWORD dwTemp;

	// check values are valid first
	if ( m_NumBones && m_Matrices != NULL && m_FrameMatrices != NULL )
	{
		// run through all bones
		for ( dwTemp = 0; dwTemp < m_NumBones; dwTemp++ )
		{
			if ( m_FrameMatrices [ dwTemp ] != NULL )
			{
				D3DXMatrixMultiply ( &m_Matrices [ dwTemp ], &m_BoneMatrices [ dwTemp ], m_FrameMatrices [ dwTemp ] );
			}
			else
			{
				D3DXMatrixIdentity ( &m_Matrices [ dwTemp ] );
			}
		}
	}

	// process next in list
	if ( m_Next != NULL )
		m_Next->CopyFrameToBoneMatrices ( );
}