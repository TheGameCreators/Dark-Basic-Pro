// include definition
#include "cenhancedmatrixmanagerc.h"

#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

extern LPDIRECT3DDEVICE9 m_pD3D;

CEnhancedMatrixManager::CEnhancedMatrixManager ( )
{
	
}

CEnhancedMatrixManager::~CEnhancedMatrixManager ( )
{
	int iTemp;

	link* pCheck = m_List.m_start;

	for ( iTemp = 0; iTemp < m_List.m_count; iTemp++ )
	{
		tagEnhancedMatrixData* ptr = NULL;
		ptr = this->GetData ( pCheck->id );

		if ( ptr == NULL )
			continue;

		ptr->m_pMode->Destroy ( );
		
		SAFE_DELETE ( ptr->m_pMode );

		delete ptr;

		pCheck = pCheck->next;
	}
}

bool CEnhancedMatrixManager::Add ( tagEnhancedMatrixData* pData, int iID )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagEnhancedMatrixData* ptr = NULL;
	ptr = ( tagEnhancedMatrixData* ) m_List.Get ( iID );
			
	if ( ptr != NULL )
		m_List.Delete ( iID );
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagEnhancedMatrixData* test;
	test = new tagEnhancedMatrixData;

	memset ( test,     0, sizeof ( test                  ) );
	memcpy ( test, pData, sizeof ( tagEnhancedMatrixData ) );

	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	return true;
}

bool CEnhancedMatrixManager::Delete ( int iID )
{
	m_List.Delete ( iID );

	return true;
}

tagEnhancedMatrixData* CEnhancedMatrixManager::GetData ( int iID )
{		
	return ( tagEnhancedMatrixData* ) m_List.Get ( iID );
}

void CEnhancedMatrixManager::Update ( float fX, float fY, float fZ )
{
	// initial renderstates
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );

	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping

	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	// set material
	D3DMATERIAL9 mtrl;
	memset ( &mtrl, 0, sizeof ( mtrl ) );
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;
	m_pD3D->SetMaterial ( &mtrl );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

	// Render all terrains
	link* check = m_List.m_start;
	for ( int temp = 0; temp < m_List.m_count; temp++ )
	{
		tagEnhancedMatrixData* ptr = NULL;
		ptr = this->GetData ( check->id );
		if ( ptr == NULL ) return;

		// Position and texture
		ptr->pD3D->SetTransform   ( D3DTS_WORLD, &ptr->matObject );
		ptr->pD3D->SetTexture   ( 0, ptr->m_pTexture );

		// Draw
		ptr->m_pMode->Draw ( fX, fY, fZ );

		// Next
		check = check->next;
	}

	// Restore
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
}