#include "sFrame.h"

sFrameMeshList::sFrameMeshList ( )
{
	// set all pointers to null
	m_Mesh = NULL;
	m_Next = NULL;
}

sFrameMeshList::~sFrameMeshList ( )
{
	// free up previously allocated memory
	SAFE_DELETE ( m_Next );
}

sFrame::sFrame ( )
{
	// set all variables to default values
	m_Name     = NULL;
	m_MeshList = NULL;

	D3DXMatrixIdentity ( &m_matCombined );
	D3DXMatrixIdentity ( &m_matTransformed );
	D3DXMatrixIdentity ( &m_matOriginal );

	m_Parent  = NULL;
	m_Sibling = NULL;
	m_Child   = NULL;

	m_Limb.bVisible  = true;
	m_Limb.vecOffset = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	m_Limb.vecRotate = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	m_Limb.vecScale  = D3DXVECTOR3 ( 1.0f, 1.0f, 1.0f );

	memset ( &m_Limb.material, 0, sizeof ( m_Limb.material ) );
	m_Limb.material.Diffuse.r = 1.0f;
	m_Limb.material.Diffuse.g = 1.0f;
	m_Limb.material.Diffuse.b = 1.0f;
	m_Limb.material.Diffuse.a = 1.0f;
	m_Limb.material.Ambient.r = 1.0f;
	m_Limb.material.Ambient.g = 1.0f;
	m_Limb.material.Ambient.b = 1.0f;
	m_Limb.material.Ambient.a = 1.0f;

	// naturally not linkable
	m_bFree = false;
}

sFrame::~sFrame ( )
{
	// free up previously allocated memory
	SAFE_DELETE ( m_Name );
	SAFE_DELETE ( m_MeshList );
	SAFE_DELETE ( m_Child );
	SAFE_DELETE ( m_Sibling );
}

sFrame* sFrame::FindFrame ( char* Name )
{
	// get a pointer to a frame

	sFrame* Frame = NULL;

	// if no name is specified return this frame
	if ( Name == NULL )
		return this;

	// compare names and return if exact match
	if ( m_Name != NULL && !strcmp ( Name, m_Name ) )
		return this;

	// search child lists
	if ( m_Child != NULL )
	{
		if ( ( Frame = m_Child->FindFrame ( Name ) ) != NULL )
			return Frame;
	}

	// search sibling lists
	if ( m_Sibling != NULL )
	{
		if ( ( Frame = m_Sibling->FindFrame ( Name ) ) != NULL )
			return Frame;
	}

	return NULL;
}

sFrame*	sFrame::FindFrame ( int iID )
{
	// get a pointer to a frame

	sFrame* Frame = NULL;

	// compare names and return if exact match
	if ( this->m_iID == iID )
		return this;

	// search child lists
	if ( m_Child != NULL )
	{
		if ( ( Frame = m_Child->FindFrame ( iID ) ) != NULL )
			return Frame;
	}

	// search sibling lists
	if ( m_Sibling != NULL )
	{
		if ( ( Frame = m_Sibling->FindFrame ( iID ) ) != NULL )
			return Frame;
	}

	return NULL;
}

void sFrame::ResetMatrices ( )
{
	m_matTransformed = m_matOriginal;

	if ( m_Child != NULL )
		m_Child->ResetMatrices ( );

	if ( m_Sibling != NULL )
		m_Sibling->ResetMatrices ( );      
}

void sFrame::AddMesh ( sMesh* Mesh )
{
	sFrameMeshList* List = NULL;

	List         = new sFrameMeshList ( );
	List->m_Mesh = Mesh;
	List->m_Next = m_MeshList;
	m_MeshList   = List;
}