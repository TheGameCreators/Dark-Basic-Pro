//
// Shadow Mapping Control Object
//

// Include
#include "cOcclusion.h"

// Occlusion object global
COcclusion g_Occlusion;

// Implementation
COcclusion::COcclusion(void)
{
	// default no occlusion
	m_OcclusionMode = 0;

	// HOQ occlusion technique
	for ( int i=0; i<70000; i++ )
	{
		d3dQuery[i] = NULL;
		iQueryBusyStage[i] = 0;
	}

	// HZB occlusion technique
	m_pHZB = NULL;

	// general statistics
	m_iOccluded = 0;
}
COcclusion::~COcclusion()
{
	// HOQ free usages
	for ( int i=0; i<70000; i++ )
		SAFE_RELEASE ( d3dQuery[i] );
}

void COcclusion::SetOcclusionMode ( int iOcclusionMode ) 
{ 
	// set value
	m_OcclusionMode = iOcclusionMode;

	// setup HZB resources
	if ( m_OcclusionMode==0 )
	{
		// clear any occlusion resources (returning to editor..)
		for ( DWORD iObjOccluderList = 0; iObjOccluderList < m_pHZB->m_vObjectOccluderList.size(); iObjOccluderList++ )
		{
			sObject* pObject = m_pHZB->m_vObjectOccluderList [ iObjOccluderList ].pObject;
			pObject->bUniverseVisible = true;
		}
		m_pHZB->m_vObjectOccluderList.clear();
		for ( DWORD iObjOccludeeList = 0; iObjOccludeeList < m_pHZB->m_vObjectOccludeeList.size(); iObjOccludeeList++ )
		{
			sObject* pObject = m_pHZB->m_vObjectOccludeeList [ iObjOccludeeList ];
			pObject->bUniverseVisible = true;
		}
		m_pHZB->m_vObjectOccludeeList.clear();
	}
	else
	{
		// setup HZB resources
		if ( m_OcclusionMode==2 )
		{
			// creatre HZB manager
			if ( m_pHZB==NULL )
			{
				m_pHZB = new HZBManager(m_pD3D);
				m_pHZB->Initialize();
			}
		}
	}
}

void COcclusion::StartOcclusionStep ( void )
{
	// reset occlusion counter
	m_iOccluded = 0;

	// HZB occlusion test before main scene rendered
	if ( m_OcclusionMode==2 )
	{
		// get view and proj from main camera
		tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( 0 );

		// store current render target
		LPDIRECT3DSURFACE9 pCurrentRT = NULL;
		LPDIRECT3DSURFACE9 pCurrentDS = NULL;
		m_pD3D->GetRenderTarget( 0, &pCurrentRT );
		m_pD3D->GetDepthStencilSurface( &pCurrentDS );

		// NOTE: Can speed this up by retaining the boundboxes array each cycle, and modify it when an object moves (hook into DBP) = performance!
		D3DXVECTOR4 vecRealWorldCenterPosAndRadius;
		UINT iBoundBoxesCount = m_pHZB->m_vObjectOccludeeList.size();
        float* results = NULL;
        D3DXVECTOR4* bounds = NULL;
		if ( m_pHZB->m_vObjectOccludeeList.size()>0 )
		{
			results = new float[iBoundBoxesCount];
			bounds = new D3DXVECTOR4[iBoundBoxesCount];
			for ( DWORD iObjOccludeeList = 0; iObjOccludeeList < m_pHZB->m_vObjectOccludeeList.size(); iObjOccludeeList++ )
			{
				sObject* pObject = m_pHZB->m_vObjectOccludeeList [ iObjOccludeeList ];
				vecRealWorldCenterPosAndRadius.x = pObject->position.vecPosition.x+pObject->collision.vecColCenter.x;
				vecRealWorldCenterPosAndRadius.y = pObject->position.vecPosition.y+pObject->collision.vecColCenter.y;
				vecRealWorldCenterPosAndRadius.z = pObject->position.vecPosition.z+pObject->collision.vecColCenter.z;
				vecRealWorldCenterPosAndRadius.w = pObject->collision.fScaledLargestRadius;
				bounds[iObjOccludeeList] = vecRealWorldCenterPosAndRadius;
			}

			// Construct occluder shortlist (at some point from main occluder list) and submit these to
			// build the depth render vertex buffer for the single depth render draw call
			if ( m_pHZB->m_bTriggerDepthOccluderVertUpdate==true )
			{
				bool bTryAgain = true;
				while ( bTryAgain==true )
				{
					bTryAgain = false;
					m_pHZB->FillDepthRenderStart();
					for ( DWORD iObjOccluderList = 0; iObjOccluderList < m_pHZB->m_vObjectOccluderList.size(); iObjOccluderList++ )
					{
						sOccluderObject* pOccObject = &m_pHZB->m_vObjectOccluderList [ iObjOccluderList ];
						if ( pOccObject )
						{
							DWORD dwVertexStart, dwVertexCount;
							if ( m_pHZB->FillDepthRenderWithVerts ( pOccObject->pObject, &dwVertexStart, &dwVertexCount )==false )
							{
								pOccObject->dwStartVertex = dwVertexStart;
								pOccObject->dwVertexToRenderCount = dwVertexCount;
							}
							else
							{ 
								bTryAgain = true; 
								break; 
							}
						}
					}
					m_pHZB->FillDepthRenderDone();
					m_pHZB->m_bTriggerDepthOccluderVertUpdate = false;
				}
			}

			// Renders the occluders and generates the HZB
			m_pHZB->Render(m_Camera_Ptr->matView, m_Camera_Ptr->matProjection);

			// Sends out a request to determine what would be culled given the list of bounds.  This
			// operation can take several milliseconds GPU time, because we're still waiting for the 
			// HZB mipmap generation to occur.
			m_pHZB->BeginCullingQuery(iBoundBoxesCount, bounds, m_Camera_Ptr->matView, m_Camera_Ptr->matProjection);

			// Some CPU calculations while I 'wait' for the GPU to finish this stall (stall not noticable at 60 fps overall)
			// Calculate the real-time vegetation distribution here (which is an intense 10-20% drain when running through
			// a field full of grass and can take my 60 fps down to 42 fps!
			// TODO

			// Attempts to read the results of the query, given the latency of the rendering overhead it's
			// best to schedule some CPU work inbetween the Begin and End Culling Query calls.
			m_pHZB->EndCullingQuery(iBoundBoxesCount, results);

			// use results to switch occludee objects visibility ON and OFF 
			// NOTE: Again, can speed this for performance by toggling object directly (no need for writing to result array) = performance!
			for ( DWORD iObjOccludeeList = 0; iObjOccludeeList < m_pHZB->m_vObjectOccludeeList.size(); iObjOccludeeList++ )
			{
				sObject* pObject = m_pHZB->m_vObjectOccludeeList [ iObjOccludeeList ];
				if ( results[iObjOccludeeList]==1 )
					pObject->bUniverseVisible = true;
				else
					pObject->bUniverseVisible = false;
			}
		}

		// restore current render target
		if ( pCurrentRT ) m_pD3D->SetRenderTarget( 0, pCurrentRT );
		if ( pCurrentDS ) m_pD3D->SetDepthStencilSurface( pCurrentDS );

		// free usages
        if ( results) delete[] results;
		if ( bounds ) delete[] bounds;
    }
}

// HOQ
void COcclusion::RenderOcclusionObject ( sObject* pObject )
{
	if( 1 )
	{
		// render basic bound shape (not actual mesh)
		sObject* pActualObject = pObject;
		if ( pObject->pInstanceOfObject ) pActualObject = pObject->pInstanceOfObject;
		if ( pActualObject->ppMeshList )
		{
			if ( pActualObject->ppMeshList[0] )
			{
				if ( pActualObject->ppMeshList[0] )
				{
					// bound mesh to render
					sMesh* pMesh = pActualObject->ppMeshList[0];

					// set world position to place object in world
					m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pObject->position.matWorld );

					// only render to depth for test (performance friendly render)
					pActualObject->ppMeshList[0]->bZWrite = false;
					m_pD3D->SetRenderState ( D3DRS_COLORWRITEENABLE, 0 );

					// render mesh as bound mesh
					m_ObjectManager.DrawMesh ( pMesh );
					m_iOccluded++;

					// counter the count as it's not a real rendered obe (just a depth one)
					if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn -= pMesh->pDrawBuffer->dwPrimitiveCount;
					if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls--;

					// only render to depth for test (performance friendly render) - restore 
					m_pD3D->SetRenderState ( D3DRS_COLORWRITEENABLE, 0x0000000F );
					pActualObject->ppMeshList[0]->bZWrite = true;
				}
			}
		}
	}
}

// HZB

void COcclusion::AddOccluder ( sObject* pObject )
{
	if ( m_pHZB ) 
	{
		m_pHZB->AddOccluder(pObject);
		m_pHZB->m_bTriggerDepthOccluderVertUpdate = true;
	}
}

void COcclusion::AddOccludee ( sObject* pObject )
{
	if ( m_pHZB ) 
	{
		m_pHZB->AddOccludee(pObject);
	}
}

void COcclusion::DeleteOccludite ( sObject* pObject )
{
	if ( m_pHZB ) m_pHZB->DeleteOccludite(pObject);
}
