//////////////////////////////////////////////////////////////////////////////////
// LOGS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*

29/04/02	1036	mj	added DB_Pro define, allows me to compile without needing DB Pro files
29/04/02	1037	mj	updated the locking section to reflect for changes in "cPositionC.cpp"

*/
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// include definition
#include "cmatrixmanagerc.h"
#include "mmsystem.h"

// Used to get external Data
#include ".\..\core\globstruct.h"
extern GlobStruct* g_pGlob;

CMatrixManager::CMatrixManager ( )
{
	
}

CMatrixManager::~CMatrixManager ( )
{
	/* must be done prior to DXrelease
	int iTemp;
	link* pCheck = m_List.m_start;
	for ( iTemp = 0; iTemp < m_List.m_count; iTemp++ )
	{
		tagMatrixData* ptr = NULL;
		ptr = this->GetData ( pCheck->id );

		if ( ptr == NULL )
			continue;

		
		SAFE_DELETE_ARRAY ( ptr->pTiles );
		SAFE_DELETE_ARRAY ( ptr->pfVert );
		SAFE_DELETE_ARRAY ( ptr->ptmpTiles );

		SAFE_RELEASE ( ptr->lpIndexBuffer );
		SAFE_RELEASE ( ptr->lpVertexBuffer );


		if ( ptr->lpTexture )
			delete [ ] ptr->lpTexture;

		delete ptr;

		pCheck = pCheck->next;
	}
	*/
}

bool CMatrixManager::Add ( tagMatrixData* pData, int iID )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagMatrixData* ptr = NULL;
	ptr = ( tagMatrixData* ) m_List.Get ( iID );
			
	if ( ptr != NULL )
		m_List.Delete ( iID );
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagMatrixData* test;
	test = new tagMatrixData;

	memset ( test,     0, sizeof ( tagMatrixData          ) );
	memcpy ( test, pData, sizeof ( tagMatrixData ) );

	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	return true;
}

bool CMatrixManager::Delete ( int iID )
{
	m_List.Delete ( iID );
	return true;
}

tagMatrixData* CMatrixManager::GetData ( int iID )
{		
	return ( tagMatrixData* ) m_List.Get ( iID );
}

void CMatrixManager::Update ( int iMode )
{
	// obtain external render states not tracked by manager
	DWORD g_dwFogColor;
	DWORD g_dwAmbientColor;
	bool gbFogOverride = false;
	m_pD3D->GetRenderState ( D3DRS_FOGCOLOR, &g_dwFogColor );
	m_pD3D->GetRenderState ( D3DRS_AMBIENT, &g_dwAmbientColor );

	// lee - 040306 - u6rc6 - if last-draw and stencil mode one, do NOT draw any matrices (or change states)
	if ( g_pGlob && iMode==1 )
		if(g_pGlob->dwStencilMode==2)
			if(g_pGlob->dwRedrawPhase==1)
				return;

	link* check = m_List.m_start;
	for ( int temp = 0; temp < m_List.m_count; temp++, check = check->next )
	{
		// get matrix ptr
		tagMatrixData* ptr = NULL;
		ptr = this->GetData ( check->id );
		if ( ptr == NULL )
			return;

		// no matrix visibility commands, so hide another below -50000!
		if ( ptr->vecPosition.y<=-50000 )
			continue;

		bool bGo = true;

		// skip if not correct for mode
		if ( iMode==0 && ptr->bRenderAfterObjects==true )
			continue;
			//bGo = false;

		if ( iMode==1 && ptr->bRenderAfterObjects==false )
			continue;
			//bGo = false;

		//if ( iMode == 0 )
		//	continue;

		//if ( bGo )
		{
			// handle matrix states
			if ( ptr->bWireframe )
				ptr->pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			else
				ptr->pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

			if ( ptr->bCull )
			{
				// Render direct view or reflected stencil view
				DWORD dwCameraCullMode = D3DCULL_CCW;
				if ( g_pGlob )
				{
					if(g_pGlob->dwStencilMode==2)
					{
						if(g_pGlob->dwRedrawPhase==1)
						{
							// View is a reflection
							dwCameraCullMode = D3DCULL_CW;
						}
					}
				}
				ptr->pD3D->SetRenderState ( D3DRS_CULLMODE, dwCameraCullMode );
			}
			else	
				ptr->pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );

			if ( ptr->bZDepth )
				ptr->pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
			else	
				ptr->pD3D->SetRenderState ( D3DRS_ZENABLE, FALSE );


			// fog system (from light DLL)
			if(g_pGlob)
			{
				if(g_pGlob->iFogState==1)
				{
					// fogenable
					if ( ptr->bFog )
						ptr->pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );
					else	
						ptr->pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );

					// ghosts in fog must override fog color part (fog override)
					if ( ptr->bFog && ptr->bGhost )
					{
						if ( gbFogOverride==false )
						{
							m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, D3DCOLOR_RGBA ( 0, 0, 0, 0 ) );
							gbFogOverride=true;
						}
					}
					else
					{
						if ( gbFogOverride==true )
						{
							m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, g_dwFogColor );
							gbFogOverride=false;
						}
					}
				}
			}
			
			// 240203 - ambient mode added for long term bug fix
			if ( ptr->bAmbient )
				ptr->pD3D->SetRenderState ( D3DRS_AMBIENT, g_dwAmbientColor );
			else
				ptr->pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB(255,255,255,255) );

			if ( ptr->bTransparency )
			{
				ptr->pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
				ptr->pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
				ptr->pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

				// Ensure pixels that are not required are not drawn (uses source alpha mask)
				ptr->pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, true );
				ptr->pD3D->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
				ptr->pD3D->SetRenderState ( D3DRS_ALPHAREF, 0x00000000 );
			}
			else
			{
				ptr->pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
				ptr->pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, false );
				ptr->pD3D->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
			}

			if ( ptr->bGhost )
			{
				ptr->pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );

				switch ( ptr->iGhostMode )
				{
					case 0:
					{
						ptr->pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
						ptr->pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
					}
					break;

					case 1:
					{
						ptr->pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
						ptr->pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
					}
					break;

					case 2:
					{
						ptr->pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
						ptr->pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
					}
					break;

					case 3:
					{
						ptr->pD3D->SetRenderState ( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
						ptr->pD3D->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
					}
					break;

					case 4:
					{
						ptr->pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
						ptr->pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
					}
					break;
				}

	// leefix : 120203 - fixed snippet that used transparency matrix
	//					 plus it also solves the issue of a sphere under a water matrix
	//					 until we improve ghosted matrices to be rendered 'after' solids
	//			// z write enable off, by switching off z write
	//			ptr->pD3D->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
			}
			else
			{
	//			// z write enable on
				//ptr->pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
			}


			// z write enable on
			ptr->pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU, ptr->iTextureMode==0 ? D3DTADDRESS_WRAP : ptr->iTextureMode );
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV, ptr->iTextureMode==0 ? D3DTADDRESS_WRAP : ptr->iTextureMode );
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, ptr->iFilter==0 ? D3DTEXF_LINEAR : ptr->iFilter );
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, ptr->iFilter==0 ? D3DTEXF_LINEAR : ptr->iFilter );
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, ptr->iMipFilter==0 ? D3DTEXF_LINEAR : ptr->iMipFilter );
		
			// see if an object should be locked to the camera
			if ( ptr->bLock )
			{
				// setup some matrices
				D3DXMATRIX	matTrans,
							matScale,
							matRot,
							matView,
							matViewInverse;
				
				// use this for the inverse function
				float fDet;

				// get the current view matrix
				ptr->pD3D->GetTransform ( D3DTS_VIEW, &matView );

				// get the inverse of the view matrix
				D3DXMatrixInverse ( &matViewInverse, &fDet, &matView );

				// apply position to matrix
				D3DXMatrixTranslation ( 
										&matTrans,
										ptr->vecPosition.x,
										ptr->vecPosition.y,
										ptr->vecPosition.z
									  );
				
				// scale the matrix
				D3DXMatrixScaling ( &matScale, ptr->vecScale.x, ptr->vecScale.y, ptr->vecScale.z );

				// add x rotation
				D3DXMatrixRotationX ( &matRot, ptr->vecRotate.x );
				D3DXMatrixMultiply  ( &matTrans, &matRot, &matTrans );

				// add y rotation
				D3DXMatrixRotationY ( &matRot, ptr->vecRotate.y );
				D3DXMatrixMultiply  ( &matTrans, &matRot, &matTrans );

				// add z rotation
				D3DXMatrixRotationZ ( &matRot, ptr->vecRotate.z );
				D3DXMatrixMultiply  ( &matTrans, &matRot, &matTrans );

				// finally build up the new matrix
				ptr->matObject = matRot * matScale * matTrans * matViewInverse; 
			}

			ptr->pD3D->SetTransform ( D3DTS_WORLD, &ptr->matObject );

			ptr->pD3D->SetVertexShader ( NULL );
			ptr->pD3D->SetFVF ( D3D9_MATRIXVERTEX );

			ptr->pD3D->SetStreamSource ( 0, ptr->lpVertexBuffer, 0, sizeof ( tagMatrixVertexDesc ) );
			ptr->pD3D->SetIndices      ( ptr->lpIndexBuffer );

			ptr->pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			ptr->pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			ptr->pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	// 240203 - this caused the matrix to ghost when in transparent mode!
	//		ptr->pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	//		ptr->pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	//		ptr->pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			ptr->pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			ptr->pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			ptr->pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			
			if ( ptr->lpTexture [ 0 ] && !ptr->bWireframe )
			{
				ptr->pD3D->SetTexture ( 0, ptr->lpTexture [ 0 ] );

				if ( ptr->lpTexture [ 1 ] )
				{
					ptr->pD3D->SetTexture ( 1, ptr->lpTexture [ 1 ] );
				}
			}
			else
				ptr->pD3D->SetTexture ( 0, NULL );

			if ( ptr->bLight )
			{
				ptr->pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

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
				ptr->pD3D->SetMaterial ( &mtrl );
			}
			else
			{
				ptr->pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
			}

			
			

			if ( ptr->lpIndexBuffer )
			{
				// draw with index data
				ptr->pD3D->DrawIndexedPrimitive 
												( 
													D3DPT_TRIANGLELIST, 0,
													0,
													( ptr->iXSegmented * ptr->iZSegmented ) * 4,
													0,
													( ptr->iXSegmented * ptr->iZSegmented ) * 2
												);

				// mike - 230604
				g_pGlob->dwNumberOfPolygonsDrawn += ( ptr->iXSegmented * ptr->iZSegmented ) * 2;
			}
			else
			{
				// draw with index sequence data (several index buffers for single matrix)
				DWORD dwVertCount = 0;
				DWORD dwPrimsPerDraw=65534/3;
				DWORD dwTotalPrimCount = ( ptr->iXSegmented * ptr->iZSegmented ) * 2;
				DWORD dwIterations = dwTotalPrimCount/dwPrimsPerDraw;
				DWORD dwPrimsLeft = dwTotalPrimCount;
				DWORD dwPrimCount = 0;
				for ( DWORD n=0; n<=dwIterations; n++ )
				{
					if ( ptr->lpIndexBufferArray[n] )
					{
						if ( dwPrimsLeft > dwPrimsPerDraw )
							dwPrimCount = dwPrimsPerDraw;
						else
							dwPrimCount = dwPrimsLeft;

						ptr->pD3D->SetIndices ( ptr->lpIndexBufferArray[n] );
						ptr->pD3D->DrawIndexedPrimitive 
														( 
															D3DPT_TRIANGLELIST,
															dwVertCount,
															0,
															dwPrimCount*2,
															0,
															dwPrimCount
														);

						dwVertCount += dwPrimCount*2;
						dwPrimsLeft-=dwPrimsPerDraw;

						// mike - 230604
						g_pGlob->dwNumberOfPolygonsDrawn += dwPrimCount;
					} 
				}
			}

			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	// mip mapping
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	// mip mapping
			ptr->pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping

			ptr->pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
			ptr->pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
			ptr->pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
		}

		// next matrix
		//check = check->next;
	}

	// must always restore ambient level (for layers)
	m_pD3D->SetRenderState ( D3DRS_AMBIENT, g_dwAmbientColor );
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, g_dwFogColor );
}