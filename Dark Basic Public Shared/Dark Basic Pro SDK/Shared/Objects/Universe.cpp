#include "universe.h"
#include "CommonC.h"
#include "BoxCollision\CCollision.h"
#include "ElipsoidCollision\Collision.h"

#include <stdio.h>
#include <mmsystem.h>
#include <algorithm>
#include <vector>
#include <list>
using namespace std;

#include ".\\compiler\\cbsptree.h"
#include ".\\compiler\\cportals.h"
#include ".\\compiler\\ccompiler.h"

extern void ComputeBoundValues ( int iPass, D3DXVECTOR3 vecXYZ, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax, D3DXVECTOR3* pvecCenter, float* pfRadius );

cUniverse::sNode::sNode ( )
{
    location = TopLeftFront;
    ppMeshList = NULL;
    pMeshPortalList = NULL;
    iMeshCount = 0;
    iMeshPortalCount = 0;
    iPolygonCount = 0;
    iMaxPolygonCount = 2;
    bChecked = false;
    bClear = false;
    vecCentre = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    vecDimension = D3DXVECTOR3( 0.0, 0.0, 0.0 );

    for (int i = 0; i < 6; ++i)
        pNeighbours[i] = NULL;

    vecColMin = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    vecColMax = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    dwNodeRefIndex = 0;
}
cUniverse::sNode::~sNode ( )
{
	SAFE_DELETE_ARRAY ( ppMeshList );
}

cUniverse::sPortal::sPortal ( )
{
	// clear out the vertex list
	// 20120327 IanM - Correctly clear the array
	memset ( vertices, 0, sizeof ( vertices ) );
//	memset ( &vertices, 0, sizeof ( vertices ) );

	// set both visible flags to true
	bVisible                 = true;
	bVisibleInViewingFrustum = true;
}

cUniverse::sArea::sArea ( )
{
    vecMin = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    vecMax = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    vecCentre = D3DXVECTOR3( 0.0, 0.0, 0.0 );
    iLinkMax = 0;
    iCount = 0;
    pDebugRegion = NULL;
    bRenderedThisCycle = false;
}

cUniverse::cUniverse ( )
{
	// initial set up
	
	// set pointers to null
	m_pNodeList			    = NULL;
	m_pNode                 = NULL;
	m_pNodeRef				= NULL;
	m_pNodeUseFlagMap		= NULL;
	
	// set bool flags to false
	m_bDebug			    = false;
	m_bGhostDebug           = false;
	m_bCreated			    = false;
	m_bPortalsActivated		= false;
	m_bPortalsCreated		= false;
	m_bRayCast              = false;
	
	// initial values for vectors
	m_vecLineStart          = D3DXVECTOR3 (        0.0f,        0.0f,        0.0f );
	m_vecLineEnd            = D3DXVECTOR3 (        0.0f,        0.0f,        0.0f );
	m_vecNodeMax            = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );
	m_vecNodeMin            = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );

	// clear vis linked object list
	m_VisLinkedObjectList.clear ( );
	m_VisLinkedObjectInNodeList.clear ( );

	// clear lists out
	m_StaticObjectList.clear ( );
	m_pMeshList.clear ( );
	m_iMeshCollisionList.clear ( );
	m_pColMeshList.clear ( );
	m_pAreaList.clear ( );
	m_pMeshShortList.clear ( );

	// clear scorch ptr
	m_pScorchMesh			= NULL;
	m_iScorchTypeMax		= 0;
	m_dwScorchVPos			= 0;
	m_dwPolyDrawLimit		= 0;

	// clear shadow data
	m_pShadowCasterMasterList.clear();
	m_pShadowLightList.clear();

	// clear areabox scanning work vars
	m_pFrustrumList.clear();

	// member data
	m_pCameraData=NULL;
	m_dwColour=0;
	m_bMoveLeft=false;

	// Scorch Data
	m_pScorchMesh=NULL;
	m_dwScorchVPos=0;
	m_dwPolyDrawLimit=0;
	m_iScorchTypeMax=0;
	m_iScorchUVWidth=0;
	m_fScorchUTile=0;
	m_fScorchVTile=0;

	// Working collision pool
	m_pCollisionPool=NULL;
	m_pCollisionDiffuse=NULL;
	m_dwCollisionPoolMax=0;
	m_dwCollisionPoolIndex=0;

	// Shadow Data
	m_pShadowCasterMasterList.clear();
	m_pShadowLightList.clear();

	// runtime cycle properties
	m_dwDrawSignature=0;
	memset ( m_dwRecurseTable, 0, sizeof ( m_dwRecurseTable ) );
}

cUniverse::~cUniverse ( )
{
	// free any internal textures this static universe used (lightmaps)
	if ( m_pMasterMeshList.size ( ) > 0 )
		for ( int i = 0; i < (int)m_pMasterMeshList.size ( ); i++ )
			FreeInternalTextures ( m_pMasterMeshList [ i ] );

	// delete the nodes in universe
	SAFE_DELETE ( m_pNodeList );
	SAFE_DELETE_ARRAY ( m_pNode );
	SAFE_DELETE_ARRAY ( m_pNodeUseFlagMap );
	SAFE_DELETE_ARRAY ( m_pNodeRef );

	// clear m_pMeshList
	if ( m_pMeshList.size ( ) > 0 )
		for ( int i = 0; i < (int)m_pMeshList.size ( ); i++ )
			SAFE_DELETE ( m_pMeshList [ i ] );

	// clear m_pColMeshList
	if ( m_pColMeshList.size ( ) > 0 )
		for ( int i = 0; i < (int)m_pColMeshList.size ( ); i++ )
			SAFE_DELETE ( m_pColMeshList [ i ] );

	// clear m_pAreaList
	if ( m_pAreaList.size ( ) > 0 )
		for ( int i = 0; i < (int)m_pAreaList.size ( ); i++ )
			SAFE_DELETE ( m_pAreaList [ i ] );

	// free scorch
	SAFE_DELETE(m_pScorchMesh);

	// free collision pool
	SAFE_DELETE(m_pCollisionPool);
	SAFE_DELETE(m_pCollisionDiffuse);
}

void cUniverse::Load ( LPSTR pFilename, int iDivideTextureSize )
{
	// clear vars
	g_iAreaBoxCount = 0;
	g_iAreaBox = 0;

	// load the universe object
	sObject* pObject = NULL;
	if ( !LoadDBO ( pFilename, &pObject ) )
		return;

	// load master mesh list
	m_pMasterMeshList.clear();
	sFrame* pFrame = pObject->pFrame;
	while ( pFrame )
	{
		sMesh* pMesh = pFrame->pMesh;
		if ( pMesh ) m_pMasterMeshList.push_back ( pMesh );
		pFrame->pMesh=NULL; //remove mesh from objects awareness
		pFrame = pFrame->pSibling;
	}

	// free object
	SAFE_DELETE ( pObject );
	
	int iMesh = 0;

	// compute master mesh bounds
	for ( iMesh = 0; iMesh < (int)m_pMasterMeshList.size ( ); iMesh++ )
		CalculateMeshBounds ( m_pMasterMeshList [ iMesh ] );

	// if divide texture flag used, need to seperate lightmaps from process
	LPSTR pLightMapString = "levelbank\\testlevel\\lightmaps";

	// prepare textures for all master meshes (load them)
	// FPSCV104RC8 - scifilevel1 - 14secs (6sec with DDS)
	for ( iMesh = 0; iMesh < (int)m_pMasterMeshList.size ( ); iMesh++ )
		LoadInternalTextures ( m_pMasterMeshList [ iMesh ], "", 2, iDivideTextureSize, pLightMapString );

	// prepare effects for all master meshes (load them)
	for ( iMesh = 0; iMesh < (int)m_pMasterMeshList.size ( ); iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = m_pMasterMeshList [ iMesh ];
		if ( pMesh->bUseVertexShader )
		{
			// Search if effect already loaded (else create a new)
			CreateNewOrSharedEffect ( pMesh, false );
		}
	}

	// load DBU storing all areaboxes and non-mesh universe data
	char pDBUFile [ _MAX_PATH ];
	strcpy ( pDBUFile, pFilename );
	pDBUFile [ strlen(pDBUFile)-4 ] = 0;
	strcat ( pDBUFile, ".dbu" );

	// universe created in here
	LoadDBU ( pDBUFile );

	// update vertex and index buffers
	CalculateNodeCollisionBoxes ();
	UploadMeshgroupsToBuffers ();

	// leenote - 310105 - could delete meshdata but USED FOR UNIVERSE RAYCAST
	// possible optimization on memory is to use 'unellipsed' COLLISION-E data

	// universe created
}

//
// TEST STRUCTURE for header!
//
struct sQuadList
{
	int iPolyVertIndexA;
	int iPolyVertIndexB;
	int iDirection;
	int iV[4][3];
	sMesh* pM;
};

void cUniverse::LeesTestFPSCQuadRemover ( void )
{
	// run through ALL meshes and find common quad-cancellations
	// array to store quads collected
	vector < sQuadList > quadList;
	quadList.clear();

	// all meshes
	for ( int iMeshIndex = 0; iMeshIndex < (int)m_pMasterMeshList.size ( ); iMeshIndex++ )
	{
		// mesh ptr
		sMesh* pMesh = m_pMasterMeshList [ iMeshIndex ];

		// vert data only
		ConvertLocalMeshToVertsOnly ( pMesh );

		// get the offset map for the FVF
		sOffsetMap offsetMap;
		GetFVFOffsetMap ( pMesh, &offsetMap );

		// array to store exclusions (polys turned to quads)
		bool* pbExclude = new bool [ (int)pMesh->dwVertexCount ];
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex++ )
			pbExclude [ iCurrentVertex ] = false;

		// go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex+=3 )
		{
			// a - for each polygon not excluded
			if ( pbExclude [ iCurrentVertex ]==false )
			{
				// gather vertex position
				D3DXVECTOR3 vecVert0 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iCurrentVertex+0) ) );
				D3DXVECTOR3 vecVert1 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iCurrentVertex+1) ) );
				D3DXVECTOR3 vecVert2 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iCurrentVertex+2) ) );
				D3DXVECTOR3 vecNormal0 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * (iCurrentVertex+0) ) );

				// reduce vertex resolution to nearest whole unit
				int iVert[3][3];
				iVert[0][0] = (int)(vecVert0.x+0.25f);
				iVert[0][1] = (int)(vecVert0.y+0.25f);
				iVert[0][2] = (int)(vecVert0.z+0.25f);
				iVert[1][0] = (int)(vecVert1.x+0.25f);
				iVert[1][1] = (int)(vecVert1.y+0.25f);
				iVert[1][2] = (int)(vecVert1.z+0.25f);
				iVert[2][0] = (int)(vecVert2.x+0.25f);
				iVert[2][1] = (int)(vecVert2.y+0.25f);
				iVert[2][2] = (int)(vecVert2.z+0.25f);

				// go through every other polygon
				for ( int iOtherVertex = 0; iOtherVertex < (int)pMesh->dwVertexCount; iOtherVertex+=3 )
				{
					// not the one we are making into a quad..
					if ( iCurrentVertex!=iOtherVertex && pbExclude[iOtherVertex]==false )
					{
						// b - possible poly to make quad..
						D3DXVECTOR3 vecOther0 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iOtherVertex+0) ) );
						D3DXVECTOR3 vecOther1 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iOtherVertex+1) ) );
						D3DXVECTOR3 vecOther2 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iOtherVertex+2) ) );
						D3DXVECTOR3 vecOtherNormal0 = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * (iOtherVertex+0) ) );

						// reduce vertex resolution to nearest whole unit
						int iOther[3][3];
						iOther[0][0] = (int)(vecOther0.x+0.25f);
						iOther[0][1] = (int)(vecOther0.y+0.25f);
						iOther[0][2] = (int)(vecOther0.z+0.25f);
						iOther[1][0] = (int)(vecOther1.x+0.25f);
						iOther[1][1] = (int)(vecOther1.y+0.25f);
						iOther[1][2] = (int)(vecOther1.z+0.25f);
						iOther[2][0] = (int)(vecOther2.x+0.25f);
						iOther[2][1] = (int)(vecOther2.y+0.25f);
						iOther[2][2] = (int)(vecOther2.z+0.25f);

						// c - match any two verts
						int iCount=0;
						bool bMatch=false;
						bool bMatchV[3];
						bool bMatchOV[3];
						bMatchV[0]=false;
						bMatchV[1]=false;
						bMatchV[2]=false;
						bMatchOV[0]=false;
						bMatchOV[1]=false;
						bMatchOV[2]=false;
						int iThirdVert = -1;
						int iUntouchedOfVertPoly = -1;
						for ( int iOV=0; iOV<3; iOV++ )
						{
							for ( int iV=0; iV<3; iV++ )
							{
								// XYZ match exactly
								if ( iVert[iV][0]==iOther[iOV][0]
								&&	 iVert[iV][1]==iOther[iOV][1]
								&&	 iVert[iV][2]==iOther[iOV][2] )
								{
									bMatchV[iV]=true;
									bMatchOV[iOV]=true;
									iCount++;
								}
							}
						}
						if ( iCount==3 )
						{
							// polygon in exact same position
							// might erase this poly if Z clash an issue (later)
						}
						else
						{
							if ( iCount==2 )
							{
								// two verts match, find the un-used third
								if ( bMatchOV[0]==false ) iThirdVert=0;
								if ( bMatchOV[1]==false ) iThirdVert=1;
								if ( bMatchOV[2]==false ) iThirdVert=2;
								if ( bMatchV[0]==false ) iUntouchedOfVertPoly=0;
								if ( bMatchV[1]==false ) iUntouchedOfVertPoly=1;
								if ( bMatchV[2]==false ) iUntouchedOfVertPoly=2;
								bMatch=true;
							}
						}
						if ( bMatch==true )
						{
							// quick ref third vector position
							bool bFormsAQuad = false;
							int iTheX = iOther[iThirdVert][0];
							int iTheY = iOther[iThirdVert][1];
							int iTheZ = iOther[iThirdVert][2];

							// imply other two verts (forming second poly)
							int iFirst, iSecnd;
							if ( iThirdVert==0 ) { iFirst=1; iSecnd=2; }
							if ( iThirdVert==1 ) { iFirst=0; iSecnd=2; }
							if ( iThirdVert==2 ) { iFirst=0; iSecnd=1; }

							// e - does third vert share a single plane of iVert poly
							if ( iVert[0][0]==iTheX	&& iVert[1][0]==iTheX && iVert[2][0]==iTheX )
							{
								// X PLANE POLY : third vert lies on plane, as does iVert poly
								// a flat pair of polys slicing into the screen distance
								if ((iOther[iThirdVert][1]==iOther[iFirst][1]
								&&   iOther[iThirdVert][2]==iOther[iSecnd][2] )
								||  (iOther[iThirdVert][1]==iOther[iSecnd][1]
								&&   iOther[iThirdVert][2]==iOther[iFirst][2] ) )
								{
									// forms a Z plane quad
									bFormsAQuad=true;
								}
							}
							if ( iVert[0][1]==iTheY	&& iVert[1][1]==iTheY && iVert[2][1]==iTheY )
							{
								// Y PLANE POLY : third vert lies on plane, as does iVert poly
								// a flat pair of polys like a floor
								if ((iOther[iThirdVert][0]==iOther[iFirst][0]
								&&   iOther[iThirdVert][2]==iOther[iSecnd][2] )
								||  (iOther[iThirdVert][0]==iOther[iSecnd][0]
								&&   iOther[iThirdVert][2]==iOther[iFirst][2] ) )
								{
									// forms a Z plane quad
									bFormsAQuad=true;
								}
							}
							if ( iVert[0][2]==iTheZ	&& iVert[1][2]==iTheZ && iVert[2][2]==iTheZ )
							{
								// Z PLANE POLY : third vert lies on plane, as does iVert poly
								// a flat pair of polys like a decal facing the camera
								if ((iOther[iThirdVert][0]==iOther[iFirst][0]
								&&   iOther[iThirdVert][1]==iOther[iSecnd][1] )
								||  (iOther[iThirdVert][0]==iOther[iSecnd][0]
								&&   iOther[iThirdVert][1]==iOther[iFirst][1] ) )
								{
									// forms a Z plane quad
									bFormsAQuad=true;
								}
							}
							if ( bFormsAQuad==true )
							{
								// f - do both polys face same direction
								int iDiffX = (int)( vecNormal0.x - vecOtherNormal0.x );
								int iDiffY = (int)( vecNormal0.y - vecOtherNormal0.y );
								int iDiffZ = (int)( vecNormal0.z - vecOtherNormal0.z );
								if ( iDiffX==0 && iDiffY==0 && iDiffZ==0 )
								{
									// this pair forms a perfect quad facing a clean direction
									sQuadList quad;
									quad.iPolyVertIndexA = iCurrentVertex;
									quad.iPolyVertIndexB = iOtherVertex;
									if ( vecNormal0.x<-0.5f ) quad.iDirection = 0;
									if ( vecNormal0.x> 0.5f ) quad.iDirection = 1;
									if ( vecNormal0.y<-0.5f ) quad.iDirection = 2;
									if ( vecNormal0.y> 0.5f ) quad.iDirection = 3;
									if ( vecNormal0.z<-0.5f ) quad.iDirection = 4;
									if ( vecNormal0.z> 0.5f ) quad.iDirection = 5;

									// to reduce quad count
									bool bIgnoreQuad=false;

									// ignore up and down
									if ( quad.iDirection>=2 && quad.iDirection<=3 )
									{
										// ignore floors and ceilings
										bIgnoreQuad=true;
									}

									// add if not ignored
									if ( bIgnoreQuad==false )
									{
										// store int vert position of quad (for speed later)
										for ( int iI=0; iI<3; iI++ )
										{
											quad.iV[0][iI]=iVert[iUntouchedOfVertPoly][iI];
											quad.iV[1][iI]=iOther[0][iI];
											quad.iV[2][iI]=iOther[1][iI];
											quad.iV[3][iI]=iOther[2][iI];
										}

										// get relative axis sizes of quad
										int iXSize= (quad.iV[0][0]-quad.iV[1][0])+
													(quad.iV[0][0]-quad.iV[2][0])+
													(quad.iV[0][0]-quad.iV[3][0]);
										int iYSize= (quad.iV[0][1]-quad.iV[1][1])+
													(quad.iV[0][1]-quad.iV[2][1])+
													(quad.iV[0][1]-quad.iV[3][1]);
										int iZSize= (quad.iV[0][2]-quad.iV[1][2])+
													(quad.iV[0][2]-quad.iV[2][2])+
													(quad.iV[0][2]-quad.iV[3][2]);

										// ignore large aspect directions too - big flat walls
										if ( quad.iDirection==0 && abs(iYSize)>=abs(iZSize)/2.0f ) bIgnoreQuad=true;
										if ( quad.iDirection==1 && abs(iYSize)>=abs(iZSize)/2.0f ) bIgnoreQuad=true;
										if ( quad.iDirection==4 && abs(iYSize)>=abs(iXSize)/2.0f ) bIgnoreQuad=true;
										if ( quad.iDirection==5 && abs(iYSize)>=abs(iXSize)/2.0f ) bIgnoreQuad=true;

										// add to quad list
										if ( bIgnoreQuad==false )
										{
											quad.pM = pMesh;
											quadList.push_back ( quad );
										}
									}

									// exclude these two polys from future scans
									pbExclude [ iCurrentVertex ] = true;
									pbExclude [ iOtherVertex ] = true;

									// continue with next iCurrentVertex poly
									iOtherVertex=pMesh->dwVertexCount;
									continue;
								}
							}
						}
					}
				}
			}
		}

		// delete temp exclusion array
		SAFE_DELETE ( pbExclude );
	}

	// go through all meshes again (now have complete quad list)
	for ( int iMeshIndex = 0; iMeshIndex < (int)m_pMasterMeshList.size ( ); iMeshIndex++ )
	{
		// mesh ptr
		sMesh* pMesh = m_pMasterMeshList [ iMeshIndex ];

		// get the offset map for the FVF
		sOffsetMap offsetMap;
		GetFVFOffsetMap ( pMesh, &offsetMap );

		// eliminate all quads that are not quad-cancelled (two quads facing each other)
		bool* pbExclude = new bool [ (int)pMesh->dwVertexCount ];
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex++ )
			pbExclude [ iCurrentVertex ] = false;

		// as the remaining list is used to destroy un-wanted polygons from mesh
		DWORD dwReducedMeshCount=0;
		for ( int iQ=0; iQ<(int)quadList.size(); iQ++ )
		{
			// take a quad, ideally to remove it
			sQuadList* pQuadPtr = &(quadList [ iQ ]);

			// work out direction and counter-direction
			int iDirection = pQuadPtr->iDirection;
			int iOppositeDirection;
			switch ( iDirection )
			{
				case 0 : iOppositeDirection=1; break;
				case 1 : iOppositeDirection=0; break;
				case 2 : iOppositeDirection=3; break;
				case 3 : iOppositeDirection=2; break;
				case 4 : iOppositeDirection=5; break;
				case 5 : iOppositeDirection=4; break;
			}

			// scan rest of quads for opposite direction match
			for ( int iOQ=0; iOQ<(int)quadList.size(); iOQ++ )
			{
				// take other quad ptr
				sQuadList* pOtherQuadPtr = &(quadList [ iOQ ]);
				int iOtherDirection = pOtherQuadPtr->iDirection;

				// do quads oppose each other
				if ( iOtherDirection==iOppositeDirection )
				{
					// do quads share exact same space
					int iCount=0;
					for ( int iV=0; iV<4; iV++ )
					{
						for ( int iOV=0; iOV<4; iOV++ )
						{
							// XYZ match exactly
							if ( pQuadPtr->iV[iV][0]==pOtherQuadPtr->iV[iOV][0]
							&&	 pQuadPtr->iV[iV][1]==pOtherQuadPtr->iV[iOV][1]
							&&	 pQuadPtr->iV[iV][2]==pOtherQuadPtr->iV[iOV][2] )
							{
								iCount++;
							}
						}
						if ( iCount<=iV ) break;
					}
					if ( iCount==4 )
					{
						// mark related polygons for removal in next section
						for ( int iErase=0; iErase<4; iErase++ )
						{
							// vertex index
							int iVI=0;
							sMesh* pM=NULL;
							if ( iErase==0 ) { pM=pQuadPtr->pM; iVI=pQuadPtr->iPolyVertIndexA; }
							if ( iErase==1 ) { pM=pQuadPtr->pM; iVI=pQuadPtr->iPolyVertIndexB; }
							if ( iErase==2 ) { pM=pOtherQuadPtr->pM; iVI=pOtherQuadPtr->iPolyVertIndexA; }
							if ( iErase==3 ) { pM=pOtherQuadPtr->pM; iVI=pOtherQuadPtr->iPolyVertIndexB; }

							// reuse exclude array to Remove Polys when re-build
							if ( pM==pMesh )
							{
								// only if currently modifying mesh
								if ( pbExclude [ iVI ]==false )
								{
									pbExclude [ iVI ] = true;
									dwReducedMeshCount += 3;
								}
							}
						}

						// continue with scan
						iOQ = quadList.size();
						continue;
					}
				}
			}
		}

		// make a new mesh deleting all polygons that are specified by remaining quadlist
		DWORD dwNewVertexCount = pMesh->dwVertexCount - dwReducedMeshCount;

		// create new arrays
		BYTE* pNewVertexData = new BYTE [ dwNewVertexCount * pMesh->dwFVFSize ];

		// copy mesh data ignoring erased quads
		int iNewVertexIndex = 0;
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex+=3 )
		{
			// only add non-excluded polys
			if ( pbExclude [ iCurrentVertex ]==false )
			{
				// copy polygon from old to new (3 verts 
				BYTE* pDst = (BYTE*) ( ( float* ) pNewVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iNewVertexIndex+0) ));
				BYTE* pSrc = (BYTE*) ( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (iCurrentVertex+0) ));
				memcpy ( pDst, pSrc, pMesh->dwFVFSize * 3 );
				iNewVertexIndex+=3;
			}
		}

		// remove old arraus
		SAFE_DELETE_ARRAY(pMesh->pVertexData);

		// replace with new arrays
		pMesh->dwVertexCount = dwNewVertexCount;
		pMesh->pVertexData = pNewVertexData;
		pMesh->iDrawVertexCount = dwNewVertexCount;
		pMesh->iDrawPrimitives = dwNewVertexCount/3;

		// flag mesh for a VB replacement
		pMesh->bMeshHasBeenReplaced=true;

		// delete temp exclusion array
		SAFE_DELETE ( pbExclude );
	}
}

void cUniverse::Save ( LPSTR pFilename )
{
	// collapse universe into final universe data (areabox meshgroups)
	BuildAreaBoxMeshGroups ( );

	// master mesh list (and where they came from)
	m_pMasterMeshList.clear();
	
	// all meshgroups in universe
	for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
	    int iIndex = 0;
	    
		sArea* pArea = m_pAreaList [ iAreaBox ];
		for ( iIndex = 0; iIndex < (int)pArea->meshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->meshgroups [ iIndex ]->pMesh;
			m_pMasterMeshList.push_back ( pMesh );
		}
		for ( iIndex = 0; iIndex < (int)pArea->sharedmeshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->sharedmeshgroups [ iIndex ]->pMesh;
			m_pMasterMeshList.push_back ( pMesh );
		}
	}

	// all meshgroup references in universe
	/*
	for ( iAreaBox = 0; iAreaBox < m_pAreaList.size ( ); iAreaBox++ )
	{
		sArea* pArea = m_pAreaList [ iAreaBox ];
		for ( int iRef = 0; iRef < pArea->meshgroupref.size ( ); iRef++ )
		{
			sMeshGroup* pMeshGroup = pArea->meshgroupref [ iRef ];
			if ( pMeshGroup )
			{
				sMesh* pMesh = pMeshGroup->pMesh;
				for ( int iMasterMeshListIndex = 0; iMasterMeshListIndex < m_pMasterMeshList.size ( ); iMasterMeshListIndex++ )
				{
					// get current area box ptr
					sMesh* pListMesh = m_pMasterMeshList [ iMasterMeshListIndex ];
					if ( pMesh==pListMesh )
					{
						// does nothing this one!
					}
				}
			}
		}
	}
	*/

	// create an object to store entire mastermeshlist
	char pDBOFile[256];
	strcpy ( pDBOFile, pFilename );
	sObject* pObject = new sObject;

	// link meshes to object for saving
	sFrame* pFrame = NULL;
	for ( int iMasterMeshListIndex = 0; iMasterMeshListIndex < (int)m_pMasterMeshList.size ( ); iMasterMeshListIndex++ )
	{
		if ( pObject->pFrame==NULL )
		{
			// new
			pFrame = new sFrame ;
			pObject->pFrame = pFrame;
		}
		else
		{
			// add mesh
			pFrame->pSibling = new sFrame;
			pFrame = pFrame->pSibling;
		}

		// apply mesh to frame
		pFrame->pMesh = m_pMasterMeshList [ iMasterMeshListIndex ];
	}

	// leeadd - 230503 - modify object to remove unseen quads
//	LeesTestFPSCQuadRemover();
	UploadMeshgroupsToBuffers ();

	// leenote - 110805 - do not attempt to smooth - messes normals (for decal splats)

	// save DBO storing all meshes ( textures and effects )
	SaveDBO ( pDBOFile, pObject );

	// compute master mesh bounds
	for ( int iMesh = 0; iMesh < (int)m_pMasterMeshList.size ( ); iMesh++ )
		CalculateMeshBounds ( m_pMasterMeshList [ iMesh ] );

	// save DBU storing all areaboxes and non-mesh universe data
	char pDBUFile [ _MAX_PATH ];
	strcpy ( pDBUFile, pDBOFile );
	pDBUFile [ strlen(pDBUFile)-4 ] = 0;
	strcat ( pDBUFile, ".dbu" );
	SaveDBU ( pDBUFile );

	// detatch meshes from object before releasing object 
	pFrame = pObject->pFrame;
	while ( pFrame )
	{
		sFrame* pThisFrame = pFrame;
		pFrame->pMesh = NULL;
		pFrame = pFrame->pSibling;
		pThisFrame->pSibling = NULL;
		SAFE_DELETE(pThisFrame);
	}
	pObject->pFrame=NULL;

	// free usages
	SAFE_DELETE ( pObject );
}

void cUniverse::SetEffectTechnique ( LPSTR pTechniqueName )
{
	// go through all meshes in universe
	for ( int iMesh = 0; iMesh < (int)m_pMasterMeshList.size ( ); iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = m_pMasterMeshList [ iMesh ];
		if ( pMesh->bUseVertexShader )
		{
			// Search for effect
			int iEffectIDFound = 0;
			for ( int iEffectID=0; iEffectID<MAX_EFFECTS; iEffectID++ )
			{
				if ( m_EffectList [ iEffectID ] )
				{
					if ( _stricmp ( m_EffectList [ iEffectID ]->pEffectObj->m_pEffectName, (LPSTR)pMesh->pEffectName )==NULL )
					{
						iEffectIDFound=iEffectID;
						break;
					}
				}
			}
			if ( iEffectIDFound>0 )
			{
				sEffectItem* pEffectItem = m_EffectList [ iEffectIDFound ];
				LPD3DXEFFECT pEffectPtr = pEffectItem->pEffectObj->m_pEffect;
				if ( pEffectPtr )
				{
					D3DXHANDLE hTechnique = pEffectPtr->GetTechniqueByName ( (LPSTR)pTechniqueName );
					if ( hTechnique )
						pEffectPtr->SetTechnique(hTechnique);
				}
			}
		}
	}
}

DWORD cUniverse::GetMasterMeshIndex ( sMesh* pFindMesh )
{
	for ( DWORD dwMasterMeshListIndex = 0; dwMasterMeshListIndex < m_pMasterMeshList.size ( ); dwMasterMeshListIndex++ )
	{
		// get current area box ptr
		sMesh* pListMesh = m_pMasterMeshList [ dwMasterMeshListIndex ];
		if ( pListMesh==pFindMesh ) return dwMasterMeshListIndex;
	}
	return 0;
}

DWORD cUniverse::GetNodeIndex ( sNode* pFindNode )
{
	for ( DWORD dwMasterNodeListIndex = 0; (int)dwMasterNodeListIndex < m_iNodeListSize; dwMasterNodeListIndex++ )
	{
		sNode* pListNode = &m_pNode [ dwMasterNodeListIndex ];
		if ( pListNode==pFindNode ) return dwMasterNodeListIndex;
	}
	return 0;
}

void cUniverse::LoadDBU ( LPSTR pDBUFilename )
{
	DWORD read;
	HANDLE hreadfile = CreateFile(pDBUFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// GENERAL SETTINGS
		DWORD dwUniverseX, dwUniverseY, dwUniverseZ;
		ReadFile(hreadfile, &dwUniverseX, 4, &read, NULL); 
		ReadFile(hreadfile, &dwUniverseY, 4, &read, NULL); 
		ReadFile(hreadfile, &dwUniverseZ, 4, &read, NULL);
		float fX = (float)dwUniverseX;
		float fY = (float)dwUniverseY;
		float fZ = (float)dwUniverseZ;
		g_pUniverse->Make ( fX, fY, fZ );

		// AREA BOXES
		DWORD dwAreaBoxMax = 0;
		ReadFile(hreadfile, &dwAreaBoxMax, 4, &read, NULL); 
		g_iAreaBoxCount = (int)dwAreaBoxMax;
		for ( int iAreaBox = 0; iAreaBox < (int)dwAreaBoxMax; iAreaBox++ )
		{
			// Get Area Box Ptr
			sArea* pArea = new sArea;
            m_pAreaList.push_back ( pArea );

			// Area Box Dimensions
			ReadFile(hreadfile, (LPSTR)&pArea->vecMin, sizeof(D3DXVECTOR3), &read, NULL); 
			ReadFile(hreadfile, (LPSTR)&pArea->vecMax, sizeof(D3DXVECTOR3), &read, NULL); 
			ReadFile(hreadfile, (LPSTR)&pArea->vecCentre, sizeof(D3DXVECTOR3), &read, NULL); 

			// Area Box Geometry (indexes into master mesh)
			DWORD dwIndex = 0;
			DWORD dwWithinMax = 0;
			ReadFile(hreadfile, &dwWithinMax, 4, &read, NULL); 
			for ( int iWithinIndex = 0; iWithinIndex < (int)dwWithinMax; iWithinIndex++ )
			{
				ReadFile(hreadfile, &dwIndex, 4, &read, NULL); 
				sMesh* pMesh = m_pMasterMeshList [ dwIndex ];
				sMeshGroup* pMeshGroup = new sMeshGroup;
				pMeshGroup->pMesh = pMesh;
				pArea->meshgroups.push_back ( pMeshGroup );
			}
			DWORD dwShared = 0;
			ReadFile(hreadfile, &dwShared, 4, &read, NULL); 
			for ( int iSharedIndex = 0; iSharedIndex < (int)dwShared; iSharedIndex++ )
			{
				ReadFile(hreadfile, &dwIndex, 4, &read, NULL); 
				sMesh* pMesh = m_pMasterMeshList [ dwIndex ];
				sMeshGroup* pMeshGroup = new sMeshGroup;
				pMeshGroup->pMesh = pMesh;
				pArea->sharedmeshgroups.push_back ( pMeshGroup );
			}
			DWORD dwRef = 0;
			ReadFile(hreadfile, &dwRef, 4, &read, NULL); 
			for ( int iRefIndex = 0; iRefIndex < (int)dwRef; iRefIndex++ )
			{
				ReadFile(hreadfile, &dwIndex, 4, &read, NULL); 
				sMesh* pMesh = m_pMasterMeshList [ dwIndex ];
				sMeshGroup* pMeshGroup = new sMeshGroup;
				pMeshGroup->pMesh = pMesh;
				pArea->meshgroupref.push_back ( pMeshGroup );
			}

			// number of links in areabox
			DWORD dwLinkMax = 0;
			ReadFile(hreadfile, &dwLinkMax, 4, &read, NULL); 
			pArea->iLinkMax = (int)dwLinkMax;
			for ( int dwLinkIndex = 0; dwLinkIndex < (int)dwLinkMax; dwLinkIndex++ )
			{
				// Get Link Ptr
				sAreaLink* pAreaLink = new sAreaLink;
				memset ( pAreaLink, 0, sizeof ( sAreaLink ) );
				pArea->pLink.push_back ( pAreaLink );

				// Link Details
				ReadFile(hreadfile, (LPSTR)&pAreaLink->iLinkedTo, sizeof(int), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->iTouchingSide, sizeof(int), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->iSolidity, sizeof(int), &read, NULL); 

				// Link Vectors
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecPortalMin, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecPortalMax, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecPortalCenter, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecPortalDim, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecA, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecB, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecC, sizeof(D3DXVECTOR3), &read, NULL); 
				ReadFile(hreadfile, (LPSTR)&pAreaLink->vecD, sizeof(D3DXVECTOR3), &read, NULL); 
			}
		}

		// NODE DATA (ultimately move bulk of node fields to 'generate area'
		DWORD dwNodeListSize = 0;
		ReadFile(hreadfile, &dwNodeListSize, 4, &read, NULL); 
		m_iNodeListSize = (int)dwNodeListSize;
		m_pNode = new sNode [ m_iNodeListSize + 1 ];
		for ( int iNode = 0; iNode < (int)dwNodeListSize; iNode++ )
		{
			// get node ptr
			sNode* pNode = &m_pNode [ iNode ];

			// node dimensions
			ReadFile(hreadfile, (LPSTR)&pNode->vecCentre, sizeof(D3DXVECTOR3), &read, NULL); 
			ReadFile(hreadfile, (LPSTR)&pNode->vecDimension, sizeof(D3DXVECTOR3), &read, NULL); 

			// node neighbors
			for ( int n=0; n<6; n++ )
			{
				DWORD dwIndex = 0;
				ReadFile(hreadfile, (LPSTR)&dwIndex, sizeof(DWORD), &read, NULL); 
				pNode->pNeighbours[n] = &m_pNode [ dwIndex ];
			}

			// node collision data
			DWORD dwColESize = 0;
			ReadFile(hreadfile, &dwColESize, 4, &read, NULL); 
			sCollisionPolygon* pColData = new sCollisionPolygon [ dwColESize ];
			ReadFile(hreadfile, pColData, sizeof(sCollisionPolygon)*dwColESize, &read, NULL); 
			pNode->collisionE.clear();
			for ( int iCol = 0; iCol < (int)dwColESize; iCol++ )
				pNode->collisionE.push_back ( pColData [ iCol ] );
			SAFE_DELETE(pColData);
		}

		// close DBU file
		CloseHandle(hreadfile);
	}
}

void cUniverse::SaveDBU ( LPSTR pDBUFilename )
{
	// open DBU file to deposit universe (V1.0 DBU format hard coded for FPSC-dev-speed)
	DWORD written;
	LPSTR pData = NULL;
	DWORD dwDataSize = 0;
	HANDLE hwritefile = CreateFile(pDBUFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hwritefile!=INVALID_HANDLE_VALUE)
	{
		// GENERAL SETTINGS
		DWORD dwUniverseX = m_pNodeList->vecDimension.x;
		DWORD dwUniverseY = m_pNodeList->vecDimension.y;
		DWORD dwUniverseZ = m_pNodeList->vecDimension.z;
		WriteFile(hwritefile, &dwUniverseX, 4, &written, NULL); 
		WriteFile(hwritefile, &dwUniverseY, 4, &written, NULL); 
		WriteFile(hwritefile, &dwUniverseZ, 4, &written, NULL); 

		// AREA BOXES
		DWORD dwAreaBoxMax = m_pAreaList.size ( );
		WriteFile(hwritefile, &dwAreaBoxMax, 4, &written, NULL); 
		for ( int iAreaBox = 0; iAreaBox < (int)dwAreaBoxMax; iAreaBox++ )
		{
			// Get Area Box Ptr
			sArea* pArea = m_pAreaList [ iAreaBox ];

			// Area Box Dimensions
			WriteFile(hwritefile, (LPSTR)&pArea->vecMin, sizeof(D3DXVECTOR3), &written, NULL); 
			WriteFile(hwritefile, (LPSTR)&pArea->vecMax, sizeof(D3DXVECTOR3), &written, NULL); 
			WriteFile(hwritefile, (LPSTR)&pArea->vecCentre, sizeof(D3DXVECTOR3), &written, NULL); 

			// Area Box Geometry (indexes into master mesh)
			DWORD dwWithinMax = pArea->meshgroups.size ( );
			WriteFile(hwritefile, &dwWithinMax, 4, &written, NULL); 
			for ( int iWithinIndex = 0; iWithinIndex < (int)dwWithinMax; iWithinIndex++ )
			{
				DWORD dwIndex = GetMasterMeshIndex ( pArea->meshgroups [ iWithinIndex ]->pMesh );
				WriteFile(hwritefile, &dwIndex, 4, &written, NULL); 
			}
			DWORD dwShared = pArea->sharedmeshgroups.size ( );
			WriteFile(hwritefile, &dwShared, 4, &written, NULL); 
			for ( int iSharedIndex = 0; iSharedIndex < (int)dwShared; iSharedIndex++ )
			{
				DWORD dwIndex = GetMasterMeshIndex ( pArea->sharedmeshgroups [ iSharedIndex ]->pMesh );
				WriteFile(hwritefile, &dwIndex, 4, &written, NULL); 
			}
			DWORD dwRef = pArea->meshgroupref.size ( );
			WriteFile(hwritefile, &dwRef, 4, &written, NULL); 
			for ( int iRefIndex = 0; iRefIndex < (int)dwRef; iRefIndex++ )
			{
				DWORD dwIndex = 0;
				sMeshGroup* pMeshGroup = pArea->meshgroupref [ iRefIndex ];
				if ( pMeshGroup )
				{
					sMesh* pMesh = pMeshGroup->pMesh;
					dwIndex = GetMasterMeshIndex ( pMesh );
				}
				WriteFile(hwritefile, &dwIndex, 4, &written, NULL); 
			}

			// number of links in areabox
			DWORD dwLinkMax = pArea->iLinkMax;
			WriteFile(hwritefile, &dwLinkMax, 4, &written, NULL); 
			for ( int dwLinkIndex = 0; dwLinkIndex < (int)dwLinkMax; dwLinkIndex++ )
			{
				// Get Link Ptr
				sAreaLink* pAreaLink = pArea->pLink [ dwLinkIndex ];

				// Link Details
				WriteFile(hwritefile, (LPSTR)&pAreaLink->iLinkedTo, sizeof(int), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->iTouchingSide, sizeof(int), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->iSolidity, sizeof(int), &written, NULL); 

				// Link Vectors
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecPortalMin, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecPortalMax, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecPortalCenter, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecPortalDim, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecA, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecB, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecC, sizeof(D3DXVECTOR3), &written, NULL); 
				WriteFile(hwritefile, (LPSTR)&pAreaLink->vecD, sizeof(D3DXVECTOR3), &written, NULL); 
			}
		}

		// NODE DATA (ultimately move bulk of node fields to 'generate area'
		DWORD dwNodeListSize = m_iNodeListSize;
		WriteFile(hwritefile, &dwNodeListSize, 4, &written, NULL); 
		for ( int iNode = 0; iNode < (int)dwNodeListSize; iNode++ )
		{
			// get node ptr
			sNode* pNode = &m_pNode [ iNode ];

			// node dimensions
			WriteFile(hwritefile, (LPSTR)&pNode->vecCentre, sizeof(D3DXVECTOR3), &written, NULL); 
			WriteFile(hwritefile, (LPSTR)&pNode->vecDimension, sizeof(D3DXVECTOR3), &written, NULL); 

			// node neighbors
			for ( int n=0; n<6; n++ )
			{
				DWORD dwIndex = GetNodeIndex ( pNode->pNeighbours [ n ] );
				WriteFile(hwritefile, (LPSTR)&dwIndex, sizeof(DWORD), &written, NULL); 
			}

			// node collision data
			DWORD dwColESize = pNode->collisionE.size ( );
			WriteFile(hwritefile, &dwColESize, 4, &written, NULL); 
			sCollisionPolygon* pColData = new sCollisionPolygon[dwColESize];
			for ( int iCol = 0; iCol < (int)dwColESize; iCol++ )
				pColData [ iCol ] = pNode->collisionE [ iCol ];
			WriteFile(hwritefile, pColData, sizeof(sCollisionPolygon)*dwColESize, &written, NULL); 
			SAFE_DELETE(pColData);
		}
			
		// close DBU file
		CloseHandle(hwritefile);
	}
}

void cUniverse::GetMeshList ( vector < sMesh* > *pMeshList )
{
	// get the mesh list

	// check the mesh list is valid
	if ( !pMeshList )
		return;

	// go through all nodes
	for ( int iObject = 0; iObject < m_iNodeListSize; iObject++ )
	{
		// find all meshes
		for ( int iMesh = 0; iMesh < m_pNode [ iObject ].iMeshCount; iMesh++ )
		{
			// get pointer to mesh
			sMesh* pMesh = m_pNode [ iObject ].ppMeshList [ iMesh ];

			// send mesh to back of list
			pMeshList->push_back ( pMesh );
		}	
	}
}

bool cUniverse::Make ( float fX, float fY, float fZ )
{
	// make the first node of the universe, all other nodes
	// must be contained within the boundaries given

	// see if the universe has already been created, if this
	// is the case then we must return as failure
	if ( m_bCreated )
		return false;

	// create the first node
	m_pNodeList = new sNode;

	// check the newly allocated memory
	SAFE_MEMORY ( m_pNodeList );

	// store node properties
	m_pNodeList->vecCentre    = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );	// centre point
	m_pNodeList->vecDimension = D3DXVECTOR3 (  fX,    fY,   fZ );	// dimensions given from parameters

	// set created flag to true
	m_bCreated = true;

	// everything went okay
	return true;
}

bool cUniverse::Attach ( sObject* pObject )
{
	// add object to vislinked list
	m_VisLinkedObjectList.push_back ( pObject );
	m_VisLinkedObjectInNodeList.push_back ( NULL );

	// complete
	return true;
}

bool cUniverse::Detach ( sObject* pObject )
{
	// find iterator and delete from both lists
	int iVisLinkedObjMax = m_VisLinkedObjectList.size ( );
	for ( int iObj = 0; iObj < iVisLinkedObjMax; iObj++ )
	{
		// object ptr
		sObject* pThisObject = m_VisLinkedObjectList [ iObj ];
		if ( pThisObject == pObject )
		{
			m_VisLinkedObjectList.erase ( m_VisLinkedObjectList.begin() + iObj );
			m_VisLinkedObjectInNodeList.erase ( m_VisLinkedObjectInNodeList.begin() + iObj );
			break;
		}
	}

	// also set universe visible back to absolutely visible
	pObject->bUniverseVisible = true;

	// complete
	return true;
}

#ifdef __GNUC__
    #define _inline 
#endif

_inline D3DXVECTOR3 cUniverse::GetTopLeftFront ( sNode* pNode )
{
	// get top left front point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x - pNode->vecDimension.x, pNode->vecCentre.y + pNode->vecDimension.y, pNode->vecCentre.z + pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetTopLeftBack ( sNode* pNode )
{
	// get top left back point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x - pNode->vecDimension.x, pNode->vecCentre.y + pNode->vecDimension.y, pNode->vecCentre.z - pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetTopRightFront ( sNode* pNode )
{
	// get top right front point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x + pNode->vecDimension.x, pNode->vecCentre.y + pNode->vecDimension.y, pNode->vecCentre.z + pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetTopRightBack ( sNode* pNode )
{
	// get top right back point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x + pNode->vecDimension.x, pNode->vecCentre.y + pNode->vecDimension.y, pNode->vecCentre.z - pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetBottomLeftFront ( sNode* pNode )
{
	// get bottom left front point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x - pNode->vecDimension.x, pNode->vecCentre.y - pNode->vecDimension.y, pNode->vecCentre.z + pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetBottomLeftBack ( sNode* pNode )
{
	// get bottom left back point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x - pNode->vecDimension.x, pNode->vecCentre.y - pNode->vecDimension.y, pNode->vecCentre.z - pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetBottomRightBack ( sNode* pNode )
{
	// get bottom right back point of node
	return D3DXVECTOR3 ( pNode->vecCentre.x + pNode->vecDimension.x, pNode->vecCentre.y - pNode->vecDimension.y, pNode->vecCentre.z - pNode->vecDimension.z );
}

_inline D3DXVECTOR3 cUniverse::GetBottomRightFront ( sNode* pNode )
{
	// get bottom right front of node
	return D3DXVECTOR3 ( pNode->vecCentre.x + pNode->vecDimension.x, pNode->vecCentre.y - pNode->vecDimension.y, pNode->vecCentre.z + pNode->vecDimension.z );
}

bool cUniverse::Add ( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	// add a limb from an object into the universe

	// make sure tree is created
	if ( !m_bCreated )
		return false;

	// check object is valid
	sObject* pObject = g_ObjectList [ iID ];
	if ( !pObject )
		return false;

	// actual object or instance of object
	sObject* pActualObject = pObject;
	if ( pObject->pInstanceOfObject )
		pActualObject = pObject->pInstanceOfObject;
	
	// clear the object list
	m_StaticObjectList.clear ( );

	// does the limb exist
	if ( pActualObject->ppFrameList [ iLimb ] )
	{
		// now see if the mesh exists
		sMesh* pMesh = pActualObject->ppFrameList [ iLimb ]->pMesh;
		if ( pMesh )
		{
			// ignore invisible meshes
			if ( pObject->pInstanceOfObject )
			{
				if ( !pObject->pInstanceMeshVisible [ iLimb ] )
					return false;
			}
			else
			{
				if ( !pMesh->bVisible )
					return false;
			}

			// ensure vertex count is valid
			if ( !pMesh->dwVertexCount )
				return false;

			// create a new object to add into list
			sStaticObject object;

			// set up object properties
			object.pFrame     = pActualObject->ppFrameList [ iLimb ];	// store frame
			object.pObject    = pObject;						// store pointer to object
			object.iID        = iID;				// id of object
			object.iCollision = iType; // collision type (0-poly/1-box/2-reduced)
			object.iCastShadow = iCastShadow;

			// set arbitary value
			pMesh->Collision.dwArbitaryValue = (DWORD)iArbitaryValue;
			pMesh->Collision.dwPortalBlocker = (DWORD)iPortalBlocker;
			
			// add this limb to the frame list
			m_StaticObjectList.push_back ( object );

			// build the tree
			if ( Build ( ) )
				return true;
		}
	}
	
	// failed
	return false;
}

bool cUniverse::Add ( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	// add an object into the universe

	// make sure tree is created
	if ( !m_bCreated )
		return false;

	// get the object and first mesh
	sObject* pObject = g_ObjectList [ iID ];
	if ( !pObject )
		return false;

	// actual object or instance of object
	sObject* pActualObject = pObject;
	if ( pObject->pInstanceOfObject )
		pActualObject = pObject->pInstanceOfObject;
	
	// clear the object list
	m_StaticObjectList.clear ( );

	// add each frame which contains a mesh into the frame list
	for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
	{
		// ignore if we don't have a mesh
		sMesh* pMesh = pActualObject->ppFrameList [ iFrame ]->pMesh;
		if ( pMesh )
		{
			// ignore invisible meshes
			if ( pObject->pInstanceOfObject )
			{
				if ( !pObject->pInstanceMeshVisible [ iFrame ] )
					continue;
			}
			else
			{
				if ( !pMesh->bVisible )
					continue;
			}

			// ensure vertex count is valid
			if ( !pMesh->dwVertexCount )
				continue;

			// our static object
			sStaticObject object;

			// set up object properties
			object.pFrame     = pActualObject->ppFrameList [ iFrame ];	// frame pointer
			object.pObject    = pObject;							// store object pointer
			object.iID        = iID;								// id of object
			object.iCollision = iType;								// collision type (0-poly/1-box/2-reduced)
			object.iCastShadow = iCastShadow;

			// set arbitary value
			pMesh->Collision.dwArbitaryValue = (DWORD)iArbitaryValue;
			pMesh->Collision.dwPortalBlocker = (DWORD)iPortalBlocker;

			// send object to back of list
			m_StaticObjectList.push_back ( object );
		}
	}

	// build the tree
	if ( Build ( ) )
		return true;

	// something went wrong
	return false;
}

bool cUniverse::Build ( void )
{
	// build the universe

	// make sure the initial node has been created
	if ( !m_bCreated )
		return false;

	// go through all objects in the list
	for ( int iFrame = 0; iFrame < (int)m_StaticObjectList.size ( ); iFrame++ )
	{
		// get a pointer to the mesh
		sMesh* pMesh = m_StaticObjectList [ iFrame ].pFrame->pMesh;
		int iCastShadow = m_StaticObjectList [ iFrame ].iCastShadow;

		// turn the object's static flag on so it will not be drawn
		// in the main loop, we are now responsible for drawing
		m_StaticObjectList [ iFrame ].pObject->bStatic = true;

		// copy the mesh as we don't want to modify the original mesh
		sMesh* pNewMesh = new sMesh;

		// check the mesh was allocated
		SAFE_MEMORY ( pNewMesh );

		// make a new mesh from the original mesh
		MakeMeshFromOtherMesh       ( true, pNewMesh, pMesh, NULL );
		CopyMeshPropertiesToNewMesh ( pMesh, pNewMesh );

		// store the ID of the object in the mesh
		pNewMesh->dwMeshID = m_StaticObjectList [ iFrame ].iID;

		// get the objects world matrix
		CalcObjectWorld ( m_StaticObjectList [ iFrame ].pObject );

		// generate reduced mesh (for collision or reduced shadow casting)
		sMesh* pReducedMesh = NULL;
		if ( m_StaticObjectList [ iFrame ].iCollision==eReducedPolygon || iCastShadow==1 )
		{
			pReducedMesh = new sMesh;
			MakeMeshFromOtherMesh ( true, pReducedMesh, pNewMesh, NULL );
			ReduceMeshPolygons ( pReducedMesh, 0, 0, 20, 1, 20 );
		}
		else
		{
			// leeadd - 240105 - make eBox mesh here (so can handle rotated at 45 degrees instead of final 90-degree box after transform)
			if ( m_StaticObjectList [ iFrame ].iCollision==eBox )
			{
				// box from original mesh - before transform
				pReducedMesh = new sMesh;
				MakeMeshBox ( 	true,
								pReducedMesh,
								pMesh->Collision.vecMin.x * 1.0f,
								pMesh->Collision.vecMin.y * 1.0f,
								pMesh->Collision.vecMin.z * 1.0f,
								pMesh->Collision.vecMax.x * 1.0f,
								pMesh->Collision.vecMax.y * 1.0f,
								pMesh->Collision.vecMax.z * 1.0f,
								D3DFVF_XYZ,	0 );
			}
		}

		// transform the vertices
		if ( m_StaticObjectList [ iFrame ].pFrame->pMesh->bVertexTransform == false )
		{
			// get the absolute world matrix
			CalculateAbsoluteWorldMatrix ( 
											m_StaticObjectList [ iFrame ].pObject,
											m_StaticObjectList [ iFrame ].pFrame,
											m_StaticObjectList [ iFrame ].pFrame->pMesh
										 );

			// transform vertices
			TransformVertices ( pNewMesh, m_StaticObjectList [ iFrame ].pFrame->matAbsoluteWorld );

			// optionally transform reduced mesh 
			if ( pReducedMesh )
			{
				TransformVertices ( pReducedMesh, m_StaticObjectList [ iFrame ].pFrame->matAbsoluteWorld );
				CalculateMeshBounds ( pReducedMesh );
			}

			// keep a reminder that vertices have been transformed
			pNewMesh->bVertexTransform = true;
		}

		// get the mesh bounds
		CalculateMeshBounds ( pNewMesh );

		// now see if the object is outside the zone
		if ( !IsObjectWithinZone ( m_pNodeList, pNewMesh ) )
		{
			SAFE_DELETE ( pReducedMesh );
			SAFE_DELETE ( pNewMesh );
			return false;
		}

		// store castshadow value in new mesh
		pNewMesh->iCastShadowIfStatic = iCastShadow;

		// set shadow if flagged
		if ( iCastShadow>0 )
		{
			// use reduced mesh for shadow, no matrix (already transformed to world)
			if ( iCastShadow==1 ) SetShadow ( pReducedMesh, NULL );
			if ( iCastShadow==2 ) SetShadow ( pNewMesh, NULL );
		}

		// add the new mesh into the buffers
		if ( m_ObjectManager.AddObjectMeshToBuffers ( pNewMesh, false ) )
		{
			// send this mesh to the back of the mesh list
			m_pMeshList.push_back ( pNewMesh );

			// store the collision info for later
			// eCollision ePolygon-0 eBox-1 eReducedPolygon-2 eNone-3
			m_iMeshCollisionList.push_back ( m_StaticObjectList [ iFrame ].iCollision );
			m_pColMeshList.push_back ( pReducedMesh );
		}
		else
		{
			// failed to add to vertex buffers on card, delete mesh
			SAFE_DELETE ( pReducedMesh );
			SAFE_DELETE ( pNewMesh );
		}
	}

	// all done
	return true;
}

void cUniverse::AddMeshToNode ( sNode* pNode, sMesh* pMesh )
{
	// adds a mesh into the mesh list of a node

	// check parameters
	if ( !pNode || !pMesh )
		return;

	// if this is the first mesh then allocate the list
	if ( pNode->iMeshCount == 0 )
	{
		// create one mesh for now
		SAFE_DELETE_ARRAY ( pNode->ppMeshList );

		// allocate a new array item
		pNode->ppMeshList = new sMesh* [ 1 ];
		pNode->iMeshCount = 1;

		// check memory was allocated
		if ( !pNode->ppMeshList )
			return;

		// store pointer to the mesh
		pNode->ppMeshList [ 0 ] = pMesh;

		// increment polygon count
		pNode->iPolygonCount += pMesh->dwVertexCount;

		// finished
		return;
	}
	
	int iMesh = 0;

	// check for a match, if the mesh is already in the list
	// then do not add it
	for ( iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
	{
		// are pointers the same
		if ( pMesh == pNode->ppMeshList [ iMesh ] )
			return;
	}
	
	// if we already have content then we must create a new list
	sMesh** ppNewMeshList = new sMesh* [ pNode->iMeshCount + 1 ];

	// memory must be valid
	if ( !ppNewMeshList )
		return;

	// copy old data across
	for ( iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
		ppNewMeshList [ iMesh ] = pNode->ppMeshList [ iMesh ];
	
	// store pointer to new mesh in new list
	ppNewMeshList [ pNode->iMeshCount ] = pMesh;

	// increment the mesh count
	pNode->iMeshCount++;

	// increment polygon count
	pNode->iPolygonCount += pMesh->dwVertexCount;

	// delete the original list
	SAFE_DELETE_ARRAY ( pNode->ppMeshList );

	// store pointer to the new list
	pNode->ppMeshList = ppNewMeshList;
}

void cUniverse::CopyCollisionDataToNode ( sNode* pNode, sMesh* pOriginalMesh, sMesh* pReducedMesh, DWORD dwArbitaryValue )
{
	// check the parameters are valid
	if ( !pNode || !pOriginalMesh )
		return;

	// select mesh
	sMesh* pMesh = pOriginalMesh;
	if ( pReducedMesh ) pMesh = pReducedMesh;

	// get the offset map
	sOffsetMap	offsetMap;				// offset map for fvf data
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// get a pointer to the vertex data
	BYTE* pVertex = pMesh->pVertexData;
	if ( !pVertex )
		return;

	// indices or vertonly
	int iIndexMax = 0;
	if ( pMesh->dwIndexCount>0 )
		iIndexMax = (int)pMesh->dwIndexCount/3;
	else
		iIndexMax = (int)pMesh->dwVertexCount/3;

	// get polygons
	int iIndexPosition = 0;		// index position
	D3DXVECTOR3 vec [ 3 ];		// used to store position
	for ( int iIndex = 0; iIndex < iIndexMax; iIndex++ )
	{
		if ( pMesh->dwIndexCount>0 )
		{
			// indices
			// position a
			vec [ 0 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) )
									);
			iIndexPosition++;

			// position b
			vec [ 1 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) )
									);
			iIndexPosition++;

			// position c
			vec [ 2 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) )
									);
			iIndexPosition++;
		}
		else
		{
			// vertonly
			// position a
			vec [ 0 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndexPosition ) )
									);
			iIndexPosition++;

			// position b
			vec [ 1 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndexPosition ) )
									);
			iIndexPosition++;

			// position c
			vec [ 2 ] = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndexPosition ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndexPosition ) )
									);
			iIndexPosition++;
		}

		// set up polygon
		sCollisionPolygon	polygon;

		// warp collision data with volume ellipse (was quicker in previous col system, now?)
		for ( int v=0; v<3; v++ )
		{
			vec [ v ].x /= 10.0f;
			vec [ v ].y /= 30.0f;
			vec [ v ].z /= 10.0f;
		}

		// add the vertices to the list
		polygon.triangle [ 0 ].vecPosition = vec [ 0 ];
		polygon.triangle [ 1 ].vecPosition = vec [ 1 ];
		polygon.triangle [ 2 ].vecPosition = vec [ 2 ];

		// apply special diffuse id used to store arbitary number (material-sound-value etc)
		polygon.diffuseid = dwArbitaryValue;

		// send polygon to back of collision layer
		pNode->collisionE.push_back ( polygon );
	}
}

bool cUniverse::IsObjectWithinZone ( sNode* pNode, sMesh* pMesh )
{
	// is the object within a node

	// start by checking our parameters
	if ( !pNode || !pMesh )
		return false;

	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// centre point
	D3DXVECTOR3 vecCentre    = pNode->vecCentre;
	D3DXVECTOR3 vecDimension = pNode->vecDimension;

	// work out the real world locations based on the centre point
	D3DXVECTOR3 vecTopLeftFront     = D3DXVECTOR3 ( vecCentre.x - vecDimension.x, vecCentre.y + vecDimension.y, vecCentre.z + vecDimension.z );
	D3DXVECTOR3 vecTopLeftBack      = D3DXVECTOR3 ( vecCentre.x - vecDimension.x, vecCentre.y + vecDimension.y, vecCentre.z - vecDimension.z );
	D3DXVECTOR3 vecTopRightBack     = D3DXVECTOR3 ( vecCentre.x + vecDimension.x, vecCentre.y + vecDimension.y, vecCentre.z - vecDimension.z );
	D3DXVECTOR3 vecTopRightFront    = D3DXVECTOR3 ( vecCentre.x + vecDimension.x, vecCentre.y + vecDimension.y, vecCentre.z + vecDimension.z );
	D3DXVECTOR3 vecBottomLeftFront  = D3DXVECTOR3 ( vecCentre.x - vecDimension.x, vecCentre.y - vecDimension.y, vecCentre.z + vecDimension.z );
	D3DXVECTOR3 vecBottomLeftBack   = D3DXVECTOR3 ( vecCentre.x - vecDimension.x, vecCentre.y - vecDimension.y, vecCentre.z - vecDimension.z );
	D3DXVECTOR3 vecBottomRightBack  = D3DXVECTOR3 ( vecCentre.x + vecDimension.x, vecCentre.y - vecDimension.y, vecCentre.z - vecDimension.z );
	D3DXVECTOR3 vecBottomRightFront = D3DXVECTOR3 ( vecCentre.x + vecDimension.x, vecCentre.y - vecDimension.y, vecCentre.z + vecDimension.z );

	// go through all vertices
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		// get a pointer to the vertex data
		BYTE* pVertex = pMesh->pVertexData;

		// check pointer
		if ( !pVertex )
			return false;

		// get position
		D3DXVECTOR3 vec = D3DXVECTOR3 ( 
										*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
										*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
										*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
									  );

		// left or right of boundary
		if ( vec.x < vecTopLeftFront.x || vec.x > vecTopRightBack.x )
			return false;

		// above or below
		if ( vec.y < vecBottomLeftFront.y || vec.y > vecTopLeftFront.y )
			return false;

		// behind or in front of boundary
		if ( vec.z < vecTopLeftBack.z || vec.z > vecTopLeftFront.z )
			return false;
	}

	// mesh must be within node
	return true;
}

void cUniverse::DetermineObjectLocation ( sNode* pNode, sMesh* pMesh, bool* pbLocationList )
{
	// find out which side the object is on

	// see if pointers are valid
	if ( !pNode || !pMesh || !pbLocationList )
		return;

	// initial location
	pbLocationList [ 0 ] = false;
	pbLocationList [ 1 ] = false;
	pbLocationList [ 2 ] = false;
	pbLocationList [ 3 ] = false;
	pbLocationList [ 4 ] = false;
	pbLocationList [ 5 ] = false;
	pbLocationList [ 6 ] = false;
	pbLocationList [ 7 ] = false;

	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// check against centre point only

	// centre point
	D3DXVECTOR3 vecCentre = pNode->vecCentre;
	D3DXVECTOR3 vec       = pMesh->Collision.vecCentre;

	if ( ( vec.x <= vecCentre.x ) && ( vec.y >= vecCentre.y ) && ( vec.z >= vecCentre.z ) )
		pbLocationList [ TopLeftFront ] = true;

	if ( ( vec.x <= vecCentre.x ) && ( vec.y >= vecCentre.y ) && ( vec.z <= vecCentre.z ) )
		pbLocationList [ TopLeftBack ] = true;

	if ( ( vec.x >= vecCentre.x ) && ( vec.y >= vecCentre.y ) && ( vec.z <= vecCentre.z ) )
		pbLocationList [ TopRightBack ] = true;

	if ( ( vec.x >= vecCentre.x ) && ( vec.y >= vecCentre.y ) && ( vec.z >= vecCentre.z ) )
		pbLocationList [ TopRightFront ] = true;

	if ( ( vec.x <= vecCentre.x ) && ( vec.y <= vecCentre.y ) && ( vec.z >= vecCentre.z ) )
		pbLocationList [ BottomLeftFront ] = true;

	if ( ( vec.x <= vecCentre.x ) && ( vec.y <= vecCentre.y ) && ( vec.z <= vecCentre.z ) )
		pbLocationList [ BottomLeftBack ] = true;

	if ( ( vec.x >= vecCentre.x ) && ( vec.y <= vecCentre.y ) && ( vec.z <= vecCentre.z ) )
		pbLocationList [ BottomRightBack ] = true;

	if ( ( vec.x >= vecCentre.x ) && ( vec.y <= vecCentre.y ) && ( vec.z >= vecCentre.z ) )
		pbLocationList [ BottomRightFront ] = true;
}

void cUniverse::TransformSelectedVertices ( sMesh* pMesh, DWORD dwFrom, DWORD dwTo, D3DXMATRIX* pmatWorld )
{
	// transform vertices by a given matrix

	// check the mesh is valid
	if ( !pMesh )
		return;

	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// world with no translation
	D3DXMATRIX matNoTrans = *pmatWorld;
	matNoTrans._41=0.0f;
	matNoTrans._42=0.0f;
	matNoTrans._43=0.0f;

	// now we need to transform the vertices for each object
	for ( int iVertex = dwFrom; iVertex < ( int ) dwTo; iVertex++ )
	{
		// get a pointer to the vertex data
		BYTE* pVertex = pMesh->pVertexData;

		// vertex pointer must be valid
		if ( !pVertex )
			return;

		// transform position
		if ( offsetMap.dwZ!=0 )
		{
			D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
												  );

			D3DXVec3TransformCoord ( &vecPosition, &vecPosition, pmatWorld );

			*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
			*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
			*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
		}

		// transform normal
		if ( offsetMap.dwNX!=0 )
		{
			D3DXVECTOR3 vecNormal = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwNY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwNZ + ( offsetMap.dwSize * iVertex ) )
												  );

			D3DXVec3TransformNormal ( &vecNormal, &vecNormal, &matNoTrans );

			*( ( float* ) pVertex + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) ) = vecNormal.x;
			*( ( float* ) pVertex + offsetMap.dwNY + ( offsetMap.dwSize * iVertex ) ) = vecNormal.y;
			*( ( float* ) pVertex + offsetMap.dwNZ + ( offsetMap.dwSize * iVertex ) ) = vecNormal.z;
		}

		// transform tangent coords
		if ( offsetMap.dwTW[1]!=0 )
		{
			D3DXVECTOR3 vecTangent = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwTU[1] + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwTV[1] + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwTZ[1] + ( offsetMap.dwSize * iVertex ) )
												  );

			D3DXVec3TransformNormal ( &vecTangent, &vecTangent, &matNoTrans );

			*( ( float* ) pVertex + offsetMap.dwTU[1] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.x;
			*( ( float* ) pVertex + offsetMap.dwTV[1] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.y;
			*( ( float* ) pVertex + offsetMap.dwTZ[1] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.z;
		}

		if ( offsetMap.dwTW[2]!=0 )
		{
			D3DXVECTOR3 vecTangent = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwTU[2] + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwTV[2] + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwTZ[2] + ( offsetMap.dwSize * iVertex ) )
												  );

			D3DXVec3TransformNormal ( &vecTangent, &vecTangent, &matNoTrans );

			*( ( float* ) pVertex + offsetMap.dwTU[2] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.x;
			*( ( float* ) pVertex + offsetMap.dwTV[2] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.y;
			*( ( float* ) pVertex + offsetMap.dwTZ[2] + ( offsetMap.dwSize * iVertex ) ) = vecTangent.z;
		}
	}
}

void cUniverse::TransformVertices ( sMesh* pMesh, D3DXMATRIX matWorld )
{
	// transform vertices by a given matrix
	TransformSelectedVertices ( pMesh, 0, pMesh->dwVertexCount, &matWorld );
}

bool cUniverse::Remove ( int iID )
{
	// remove an object from nodes

	return true;
}

void cUniverse::SetupVertex ( sPortalVertex* pVertex, D3DXVECTOR3 vecPosition, D3DXVECTOR3 vecNormal, float tu, float tv )
{
	// set vertex information

	// check vertex pointer is valid
	if ( !pVertex )
		return;

	// store values
	pVertex->vecPosition = vecPosition;							// position
	pVertex->vecNormal   = vecNormal;							// normal
	pVertex->tu          = tu;									// texture a
	pVertex->tv          = tv;									// texture b
	pVertex->dwDiffuse   = m_dwColour;							// diffuse colour
}

void cUniverse::CreatePortalVertices ( sNode* pNode, bool bSetup, DWORD dwColour )
{
	// create vertices for portals

	// check node is valid
	if ( !pNode )
		return;

	m_dwColour = dwColour;

	// back
	SetupVertex ( &pNode->portals [ 0 ].vertices [ 0 ], GetTopRightBack    ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 0 ].vertices [ 1 ], GetTopLeftBack     ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 0 ].vertices [ 2 ], GetBottomRightBack ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 0 ].vertices [ 3 ], GetBottomLeftBack  ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f ), 1.0f, 1.0f );

	// front
	SetupVertex ( &pNode->portals [ 1 ].vertices [ 0 ], GetTopRightFront    ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, -1.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 1 ].vertices [ 1 ], GetTopLeftFront     ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, -1.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 1 ].vertices [ 2 ], GetBottomRightFront ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, -1.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 1 ].vertices [ 3 ], GetBottomLeftFront  ( pNode ), D3DXVECTOR3 ( 0.0f, 0.0f, -1.0f ), 1.0f, 1.0f );

	// top
	SetupVertex ( &pNode->portals [ 2 ].vertices [ 0 ], GetTopRightBack  ( pNode ),	D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 2 ].vertices [ 1 ], GetTopLeftBack   ( pNode ), D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 2 ].vertices [ 2 ], GetTopRightFront ( pNode ),	D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 2 ].vertices [ 3 ], GetTopLeftFront  ( pNode ), D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), 1.0f, 1.0f );

	// bottom
	SetupVertex ( &pNode->portals [ 3 ].vertices [ 0 ], GetBottomRightBack  ( pNode ), D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 3 ].vertices [ 1 ], GetBottomLeftBack   ( pNode ), D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 3 ].vertices [ 2 ], GetBottomRightFront ( pNode ), D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 3 ].vertices [ 3 ], GetBottomLeftFront  ( pNode ), D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f ), 1.0f, 1.0f );

	// left
	SetupVertex ( &pNode->portals [ 4 ].vertices [ 0 ], GetTopRightBack     ( pNode ), D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 4 ].vertices [ 1 ], GetTopRightFront    ( pNode ), D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 4 ].vertices [ 2 ], GetBottomRightBack  ( pNode ), D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 4 ].vertices [ 3 ], GetBottomRightFront ( pNode ), D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f ), 1.0f, 1.0f );

	// right
	SetupVertex ( &pNode->portals [ 5 ].vertices [ 0 ], GetTopLeftBack     ( pNode ), D3DXVECTOR3 ( -1.0f, 0.0f, 0.0f ), 1.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 5 ].vertices [ 1 ], GetTopLeftFront    ( pNode ), D3DXVECTOR3 ( -1.0f, 0.0f, 0.0f ), 0.0f, 0.0f );
	SetupVertex ( &pNode->portals [ 5 ].vertices [ 2 ], GetBottomLeftBack  ( pNode ), D3DXVECTOR3 ( -1.0f, 0.0f, 0.0f ), 0.0f, 1.0f );
	SetupVertex ( &pNode->portals [ 5 ].vertices [ 3 ], GetBottomLeftFront ( pNode ), D3DXVECTOR3 ( -1.0f, 0.0f, 0.0f ), 1.0f, 1.0f );

	if ( bSetup )
	{
		// all visible by default
		pNode->portals [ 0 ].bVisible = true;
		pNode->portals [ 1 ].bVisible = true;
		pNode->portals [ 2 ].bVisible = true;
		pNode->portals [ 3 ].bVisible = true;
		pNode->portals [ 4 ].bVisible = true;
		pNode->portals [ 5 ].bVisible = true;

		// in viewing frustum by default
		pNode->portals [ 0 ].bVisibleInViewingFrustum = true;
		pNode->portals [ 1 ].bVisibleInViewingFrustum = true;
		pNode->portals [ 2 ].bVisibleInViewingFrustum = true;
		pNode->portals [ 3 ].bVisibleInViewingFrustum = true;
		pNode->portals [ 4 ].bVisibleInViewingFrustum = true;
		pNode->portals [ 5 ].bVisibleInViewingFrustum = true;
	}
}

void cUniverse::DrawNodePortals ( sNode* pNode )
{
	// draw portals in node

	// check node pointer is valid
	if ( !pNode )
		return;

	// to store fvf
	DWORD dwFVF = 0;

	// get fvf
	m_pD3D->GetFVF ( &dwFVF );

	// set texture to null
	m_pD3D->SetTexture ( 0, NULL );
	m_pD3D->SetTexture ( 1, NULL );
	m_pD3D->SetTexture ( 2, NULL );
	m_pD3D->SetTexture ( 3, NULL );

	// draw in solid mode
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

	// set fvf
	m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	// set render states
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );					// alpha blend on
	m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ONE );			// source blend
	m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCCOLOR );	// destination blend
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );			// no culling
	m_pD3D->SetRenderState ( D3DRS_LIGHTING,         FALSE );					// turn lighting off
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,          false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     false );

	// go through each side
	for ( int iPortal = 0; iPortal < 6; iPortal++ )
	{
		// skip invisible portals
		if ( !pNode->portals [ iPortal ].bVisible )
				continue;

		// skip portals that are not in viewing frustum
		if ( !pNode->portals [ iPortal ].bVisibleInViewingFrustum )
			continue;

		// draw the portal
		m_pD3D->DrawPrimitiveUP (
									D3DPT_TRIANGLESTRIP,
									2,
									&pNode->portals [ iPortal ].vertices [ 0 ],
									sizeof ( sPortalVertex )
								);
		
	}
		
	// reset certain render states
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_CCW );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     true );

	// and finally go back to original fvf
	m_pD3D->SetFVF ( dwFVF );
}

void cUniverse::DrawNodeBounds ( sNode* pNode )
{
	// draw bounds of a node

	// check node pointer
	if ( !pNode )
		return;

	// used to store fvf
	DWORD dwFVF = 0;

	// get fvf and set a new one
	m_pD3D->GetFVF ( &dwFVF );
	m_pD3D->SetFVF ( D3DFVF_XYZ );

	// set no texture and render states
	m_pD3D->SetTexture     ( 0, NULL );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pD3D->SetRenderState ( D3DRS_FILLMODE,         D3DFILL_SOLID );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,          false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     false );

	// to store lines
	D3DXVECTOR3 vecLines [ 24 ];

	// get node bounds
	vecLines [  0 ] = GetTopLeftFront     ( pNode );	vecLines [  1 ] = GetTopRightFront    ( pNode );
	vecLines [  2 ] = GetTopLeftBack      ( pNode );  	vecLines [  3 ] = GetTopRightBack     ( pNode );
	vecLines [  4 ] = GetTopLeftFront     ( pNode );	vecLines [  5 ] = GetTopLeftBack      ( pNode );
	vecLines [  6 ] = GetTopRightFront    ( pNode );	vecLines [  7 ] = GetTopRightBack     ( pNode );
	vecLines [  8 ] = GetBottomLeftFront  ( pNode );	vecLines [  9 ] = GetBottomRightFront ( pNode );
	vecLines [ 10 ] = GetBottomLeftBack   ( pNode );	vecLines [ 11 ] = GetBottomRightBack  ( pNode );
	vecLines [ 12 ] = GetBottomLeftFront  ( pNode );	vecLines [ 13 ] = GetBottomLeftBack   ( pNode );
	vecLines [ 14 ] = GetBottomRightFront ( pNode );	vecLines [ 15 ] = GetBottomRightBack  ( pNode );
	vecLines [ 16 ] = GetTopLeftFront     ( pNode );	vecLines [ 17 ] = GetBottomLeftFront  ( pNode );
	vecLines [ 18 ] = GetTopLeftBack      ( pNode );	vecLines [ 19 ] = GetBottomLeftBack   ( pNode );
	vecLines [ 20 ] = GetTopRightBack     ( pNode );	vecLines [ 21 ] = GetBottomRightBack  ( pNode );
	vecLines [ 22 ] = GetTopRightFront    ( pNode );	vecLines [ 23 ] = GetBottomRightFront ( pNode );

	// draw the node bounds
	m_pD3D->DrawPrimitiveUP (
								D3DPT_LINELIST,
								12,
								&vecLines,
								sizeof ( float ) * 3
							);


	// go back to original fvf
	m_pD3D->SetFVF ( dwFVF );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, true );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     true );
}

void cUniverse::GetAreaBounds ( sArea* pArea )
{
	// get the bounding box of a given area

	// check the area pointer is valid
	if ( !pArea )
		return;

	// go through all nodes within the area
	for ( int iNode = 0; iNode < (int)pArea->nodes.size ( ); iNode++ )
	{
		// get a pointer to the node
		sNode* pNode = pArea->nodes [ iNode ];

		// ensure node is valid
		if ( !pNode )
			continue;

		// store an array of positions for corners or nodes
		D3DXVECTOR3 vec [ ] =	{ 
									GetTopLeftFront     ( pNode ),
									GetTopLeftBack      ( pNode ),
									GetTopRightFront    ( pNode ),
									GetTopRightBack     ( pNode ),
									GetBottomLeftFront  ( pNode ),
									GetBottomLeftBack   ( pNode ),
									GetBottomRightBack  ( pNode ),
									GetBottomRightFront ( pNode )
								};

		// now loop through all positions
		for ( int i = 0; i < sizeof ( vec ) / sizeof ( D3DXVECTOR3 ); i++ )
		{
			// check minimum extents
			if ( vec [ i ].x < pArea->vecMin.x ) pArea->vecMin.x = vec [ i ].x;
			if ( vec [ i ].y < pArea->vecMin.y ) pArea->vecMin.y = vec [ i ].y;
			if ( vec [ i ].z < pArea->vecMin.z ) pArea->vecMin.z = vec [ i ].z;

			// check maximum extents
			if ( vec [ i ].x > pArea->vecMax.x ) pArea->vecMax.x = vec [ i ].x;
			if ( vec [ i ].y > pArea->vecMax.y ) pArea->vecMax.y = vec [ i ].y;
			if ( vec [ i ].z > pArea->vecMax.z ) pArea->vecMax.z = vec [ i ].z;

			// increment centre by current position
			pArea->vecCentre += vec [ i ];

			// and increment count value
			pArea->iCount++;
		}
	}
}

bool cUniverse::WillAreaBoxEnterAnotherAreaBox ( int iSide, int iAreaX1, int iAreaY1, int iAreaZ1, int iAreaX2, int iAreaY2, int iAreaZ2 )
{
	// pre-empt the advance
	if ( iSide==0 )	iAreaZ1--; 
	if ( iSide==1 )	iAreaZ2++;
	if ( iSide==2 )	iAreaY2++;
	if ( iSide==3 )	iAreaY1--;
	if ( iSide==4 )	iAreaX2++;
	if ( iSide==5 )	iAreaX1--;
	
	int iAreaBox = 0;

	// go through each area box created thus far to see if side penetrates 'any'
	for ( iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// get current area box ptr
		sArea* pCurrentArea = m_pAreaList [ iAreaBox ];
		int iAX1 = m_iHalfUniverseSizeX+((int)(pCurrentArea->vecMin.x+50.0f)/100);
		int iAY1 = m_iHalfUniverseSizeY+((int)(pCurrentArea->vecMin.y+50.0f)/100);
		int iAZ1 = m_iHalfUniverseSizeZ+((int)(pCurrentArea->vecMin.z+50.0f)/100);
		int iAX2 = m_iHalfUniverseSizeX+((int)(pCurrentArea->vecMax.x-50.0f)/100);
		int iAY2 = m_iHalfUniverseSizeY+((int)(pCurrentArea->vecMax.y-50.0f)/100);
		int iAZ2 = m_iHalfUniverseSizeZ+((int)(pCurrentArea->vecMax.z-50.0f)/100);

		// does the side penetrate (or overlap) this area box at all?
		if ( iAreaX1<=iAX2 && iAreaX2>=iAX1 )
			if ( iAreaY1<=iAY2 && iAreaY2>=iAY1 )
				if ( iAreaZ1<=iAZ2 && iAreaZ2>=iAZ1 )
					break;
	}

	// cut short, area box penetrated, so it will
	if ( iAreaBox < (int)m_pAreaList.size ( ) )
		return true;
	else
		return false;
}

bool cUniverse::BuildAreaBoxes ( void )
{
    int iNode = 0;
    
	// go through whole node universe and expand area boxes into them
	for ( iNode = 0; iNode < m_iNodeListSize; iNode++ )
	{
		// get node ptr
		sNode* pNode = &m_pNode [ iNode ];

		// for each un-used node
		if ( pNode->bChecked==false )
		{
			// start with area of single node
			int iAreaX1 = m_iHalfUniverseSizeX+((int)pNode->vecCentre.x/100);
			int iAreaY1 = m_iHalfUniverseSizeY+((int)pNode->vecCentre.y/100);
			int iAreaZ1 = m_iHalfUniverseSizeZ+((int)pNode->vecCentre.z/100);
			int iAreaX2 = iAreaX1;
			int iAreaY2 = iAreaY1;
			int iAreaZ2 = iAreaZ1;

			// expand on six sides
			for ( int iSide=0; iSide<6; iSide++ )
			{
				// prepare values for each side
				int iX1=iAreaX1, iX2=iAreaX2;
				int iY1=iAreaY1, iY2=iAreaY2;
				int iZ1=iAreaZ1, iZ2=iAreaZ2;
				if ( iSide==0 ) { iZ2=iZ1; }
				if ( iSide==1 ) { iZ1=iZ2; }
				if ( iSide==2 ) { iY1=iY2; }
				if ( iSide==3 ) { iY2=iY1; }
				if ( iSide==4 ) { iX1=iX2; }
				if ( iSide==5 ) { iX2=iX1; }

				// calculate opposite side for secondary side check
				int iOSide, iOX, iOY, iOZ;
				if ( iSide==0 ) { iOSide=1; iOX=0; iOY=0; iOZ=-1; }
				if ( iSide==1 ) { iOSide=0; iOX=0; iOY=0; iOZ= 1; }
				if ( iSide==2 ) { iOSide=3; iOX=0; iOY= 1; iOZ=0; }
				if ( iSide==3 ) { iOSide=2; iOX=0; iOY=-1; iOZ=0; }
				if ( iSide==4 ) { iOSide=5; iOX= 1; iOY=0; iOZ=0; }
				if ( iSide==5 ) { iOSide=4; iOX=-1; iOY=0; iOZ=0; }

				// scan for solid or border obstruction
				int iSolidity;
				bool bSolidFound = false;
				if ( iSide==0 && iAreaZ1==0 ) bSolidFound=true;
				if ( iSide==1 && iAreaZ2==m_iUniverseSizeZ ) bSolidFound=true;
				if ( iSide==2 && iAreaY2==m_iUniverseSizeY ) bSolidFound=true;
				if ( iSide==3 && iAreaY1==0 ) bSolidFound=true;
				if ( iSide==4 && iAreaX2==m_iUniverseSizeX ) bSolidFound=true;
				if ( iSide==5 && iAreaX1==0 ) bSolidFound=true;
				if ( bSolidFound==false )
				{
					for ( int iX=iX1; iX<=iX2; iX++ )
					{
						for ( int iY=iY1; iY<=iY2; iY++ )
						{
							for ( int iZ=iZ1; iZ<=iZ2; iZ++ )
							{
								// check the direct side we want to expand into
								if ( GetSideFree ( iSide, iX, iY, iZ, &iSolidity )==false
								||	 GetSideFree ( iOSide, iX+iOX, iY+iOY, iZ+iOZ, &iSolidity )==false )
								{
									// solid side means no more in this direction
									bSolidFound=true;
								}

								// if about to proceed, make sure no secondary solids cut into areabox
								if ( bSolidFound==false )
								{
									// for Y expansion, we need to make sure there are no Z walls if we expanded up (+1)
									if ( iSide==2 )
									{
										// only inside Z walls, not the edges where the natural areabox walls are
										if ( iZ>iZ1 && iZ<iZ2 )
										{
											// check below to see if Z wall exists
											if ( GetSideFree ( 0, iX, iY + 1, iZ, &iSolidity )==false
											||	 GetSideFree ( 1, iX, iY + 1, iZ-1, &iSolidity )==false )
												bSolidFound=true;
										}
									}

									// for Y expansion, we need to make sure there are no Z walls if we expanded down (-1)
									if ( iSide==3 )
									{
										// only inside Z walls, not the edges where the natural areabox walls are
										if ( iZ>iZ1 && iZ<iZ2 )
										{
											// check below to see if Z wall exists
											if ( GetSideFree ( 0, iX, iY - 1, iZ, &iSolidity )==false
											||	 GetSideFree ( 1, iX, iY - 1, iZ-1, &iSolidity )==false )
												bSolidFound=true;
										}
									}

									// for X expansion, we need to make sure there are no Z and Y walls if we expanded right (+1)
									if ( iSide==4 )
									{
										// only inside Z and Y walls, not the edges where the natural areabox walls are
										if ( iZ>iZ1 && iZ<iZ2 )
										{
											// check right to see if Z wall exists
											if ( GetSideFree ( 0, iX + 1, iY, iZ, &iSolidity )==false
											||	 GetSideFree ( 1, iX + 1, iY, iZ-1, &iSolidity )==false )
												bSolidFound=true;
										}
										if ( iY>iY1 && iY<iY2 ) 
										{
											// check right to see if Y wall exists
											if ( GetSideFree ( 2, iX + 1, iY, iZ, &iSolidity )==false
											||	 GetSideFree ( 3, iX + 1, iY+1, iZ, &iSolidity )==false )
												bSolidFound=true;
										}
									}

									// for X expansion, we need to make sure there are no Z and Y walls if we expanded left (-1)
									if ( iSide==5 )
									{
										// only inside Z and Y walls, not the edges where the natural areabox walls are
										if ( iZ>iZ1 && iZ<iZ2 )
										{
											// check left to see if Z wall exists
											if ( GetSideFree ( 0, iX - 1, iY, iZ, &iSolidity )==false
											||	 GetSideFree ( 1, iX - 1, iY, iZ-1, &iSolidity )==false )
												bSolidFound=true;
										}
										if ( iY>iY1 && iY<iY2 ) 
										{
											// check left to see if Y wall exists
											if ( GetSideFree ( 2, iX - 1, iY, iZ, &iSolidity )==false
											||	 GetSideFree ( 3, iX - 1, iY+1, iZ, &iSolidity )==false )
												bSolidFound=true;
										}
									}
								}

								// in case of any solid found, exit iterations now
								if ( bSolidFound )
								{
									iX=iX2; iY=iY2; iZ=iZ2;
									break;
								}
							}
						}
					}
				}

				// when solid not found, expand in alternating directions
				if ( bSolidFound==false )
				{
					if ( !WillAreaBoxEnterAnotherAreaBox ( iSide, iAreaX1, iAreaY1, iAreaZ1, iAreaX2, iAreaY2, iAreaZ2 ) )
					{
						if ( iSide==0 )	iAreaZ1--; 
						if ( iSide==1 )	iAreaZ2++;
						if ( iSide==2 )	iAreaY2++;
						if ( iSide==3 )	iAreaY1--;
						if ( iSide==4 )	iAreaX2++;
						if ( iSide==5 )	iAreaX1--;
						iSide--;
					}
				}
			}

			// create a new area
			sArea* pArea = new sArea;
			if ( !pArea ) return false;

			// set up initial area properties
			pArea->nodes.clear ( );													// make sure node list is clear
			pArea->vecCentre = D3DXVECTOR3 (       0.0f,       0.0f,       0.0f );	// set centre point
			pArea->vecMin    = D3DXVECTOR3 (  100000.0f,  100000.0f,  100000.0f );	// set minimum bounds
			pArea->vecMax    = D3DXVECTOR3 ( -100000.0f, -100000.0f, -100000.0f );	// set maximum bounds
			pArea->iCount    = 0;													// reset count to 0

			// place all nodes into newly created area box
			for ( int iFindNode = 0; iFindNode < m_iNodeListSize; iFindNode++ )
			{
				// get node ptr
				sNode* pFindNode = &m_pNode [ iFindNode ];
				if ( pFindNode->bChecked==false )
				{
					// calculate location of node
					int iFindX = m_iHalfUniverseSizeX+((int)pFindNode->vecCentre.x/100);
					int iFindY = m_iHalfUniverseSizeY+((int)pFindNode->vecCentre.y/100);
					int iFindZ = m_iHalfUniverseSizeZ+((int)pFindNode->vecCentre.z/100);

					// add node if within area
					if ( iFindX>=iAreaX1 && iFindX<=iAreaX2
					&&   iFindY>=iAreaY1 && iFindY<=iAreaY2
					&&   iFindZ>=iAreaZ1 && iFindZ<=iAreaZ2 )
					{
						// add node and set to used
						pArea->nodes.push_back ( pFindNode );
						pFindNode->bChecked = true;
					}
				}
			}

			// get the area bounds
			GetAreaBounds ( pArea );

			// set up final bound box properties
			pArea->vecCentre /= ( float ) pArea->iCount;

			// store region for debug purposes
			pArea->pDebugRegion = new sNode;
			pArea->pDebugRegion->vecCentre = pArea->vecCentre;
			pArea->pDebugRegion->vecDimension = ( pArea->vecMax - pArea->vecMin ) / 2.0f;	// store dimension in region

			// send this area to back of list
			m_pAreaList.push_back ( pArea );
		}
	}

	// create debug data for each area box
	for ( iNode = 0; iNode < (int)m_pAreaList.size ( ); iNode++ )
		CreatePortalVertices ( m_pAreaList [ iNode ]->pDebugRegion );

	// store count of all area boxes
	g_iAreaBoxCount = m_pAreaList.size ( );
	
	// complete
	return true;
}

bool cUniverse::GetSideFree ( int iPortalSide, int iRefX, int iRefY, int iRefZ, int* pSolidity )
{
	if ( iRefX>=0 && iRefY>=0 && iRefZ>=0 )
	{
		// node determines side soliditiy
		sNode* pNode = m_pNodeRef [ iRefX + (iRefY*m_iUniverseSizeX) + (iRefZ*(m_iUniverseSizeX*m_iUniverseSizeY)) ];
		if ( pNode )
		{
			// assign solidity state
			if ( pSolidity )
				if ( pNode->portals [ iPortalSide ].iSolidityMode > 0 )
					*pSolidity = pNode->portals [ iPortalSide ].iSolidityMode;

			// return visibility
			return pNode->portals [ iPortalSide ].bVisible;
		}
		else
			return false;
	}
	else
	{
		// exceeds range of universe
		return false;
	}
}

void cUniverse::CreatePortalAndReset ( sArea* pCurrentArea, int iChkAreaBox, int iTouchingSide, D3DXVECTOR3 vecPortalMin, D3DXVECTOR3 vecPortalMax, int iSolidity )
{
	// work var
	sAreaLink AreaLink;
	memset ( &AreaLink, 0, sizeof(AreaLink) );

	// set link info and add to list
	AreaLink.iLinkedTo = iChkAreaBox;
	AreaLink.iTouchingSide = iTouchingSide;
	AreaLink.iSolidity = iSolidity;
	AreaLink.vecPortalMin = vecPortalMin;
	AreaLink.vecPortalMax = vecPortalMax;

	// calculate center and dimension for culling
	AreaLink.vecPortalDim = vecPortalMax-vecPortalMin;
	AreaLink.vecPortalCenter = vecPortalMin+((AreaLink.vecPortalDim)/2.0f);
	AreaLink.vecPortalDim /= 2.0f; //rectangle refined as x-halfsize, x+halfsize
	
	// generate vectors from portal region
	switch ( iTouchingSide )
	{
		case 0 :	// BACK (NORTH)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMax.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMax.z );	break;

		case 1 :	// FRONT (SOUTH)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMin.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMin.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMin.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMin.z );	break;

		case 2 :	// TOP (UP)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMin.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMin.z );	break;

		case 3 :	// BOTTOM (DOWN)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMax.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMax.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMin.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMin.z );	break;

		case 4 :	// LEFT (WEST)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMax.y, vecPortalMin.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMin.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMin.x, vecPortalMin.y, vecPortalMax.z );	break;

		case 5 :	// RIGHT (EAST)
					AreaLink.vecA = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMin.z );
					AreaLink.vecB = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMax.y, vecPortalMax.z );
					AreaLink.vecC = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMax.z );
					AreaLink.vecD = D3DXVECTOR3 ( vecPortalMax.x, vecPortalMin.y, vecPortalMin.z );	break;
	}

	// create dynamic memory to store in list
	sAreaLink* pAreaLink = new sAreaLink;
	*pAreaLink = AreaLink;

	// add to list of links for this area box
	pCurrentArea->pLink.push_back ( pAreaLink );
	pCurrentArea->iLinkMax++;
}

void cUniverse::ExtendPortalMinMax ( int iPortalSide, int iX, int iY, int iZ, D3DXVECTOR3* pvecPortalMin, D3DXVECTOR3* pvecPortalMax )
{
	// side determines extension along approproate axis
	D3DXVECTOR3 vecExt = D3DXVECTOR3(0,0,0);

	// based on side creating portal on
	if ( iPortalSide==eBack )	vecExt = D3DXVECTOR3(100.0f,100.0f,0);
	if ( iPortalSide==eFront )	vecExt = D3DXVECTOR3(100.0f,100.0f,0);
	if ( iPortalSide==eLeft )	vecExt = D3DXVECTOR3(0,100.0f,100.0f);
	if ( iPortalSide==eRight )	vecExt = D3DXVECTOR3(0,100.0f,100.0f);
	if ( iPortalSide==eTop )	vecExt = D3DXVECTOR3(100.0f,0,100.0f);
	if ( iPortalSide==eBottom )	vecExt = D3DXVECTOR3(100.0f,0,100.0f);

	// increase size of portal rea using x,y,z
	if ( iX<pvecPortalMin->x ) pvecPortalMin->x=iX;
	if ( iY<pvecPortalMin->y ) pvecPortalMin->y=iY;
	if ( iZ<pvecPortalMin->z ) pvecPortalMin->z=iZ;
	if ( iX+vecExt.x>pvecPortalMax->x ) pvecPortalMax->x=iX+vecExt.x;
	if ( iY+vecExt.y>pvecPortalMax->y ) pvecPortalMax->y=iY+vecExt.y;
	if ( iZ+vecExt.z>pvecPortalMax->z ) pvecPortalMax->z=iZ+vecExt.z;
}

bool cUniverse::BuildAreaLinks ( void )
{
    int iAreaBox = 0;
    
	// stage 1 - work out which area boxes touch and calculate the portals that bind them
	for ( iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// get current area box ptr
		sArea* pCurrentArea = m_pAreaList [ iAreaBox ];
		int iAX1 = pCurrentArea->vecMin.x;
		int iAY1 = pCurrentArea->vecMin.y;
		int iAZ1 = pCurrentArea->vecMin.z;
		int iAX2 = pCurrentArea->vecMax.x;
		int iAY2 = pCurrentArea->vecMax.y;
		int iAZ2 = pCurrentArea->vecMax.z;

		// clear link info
		pCurrentArea->pLink.clear();

		// for each area box, check if touching other
		for ( int iChkAreaBox = 0; iChkAreaBox < (int)m_pAreaList.size ( ); iChkAreaBox++ )
		{
			// and not touching itself
			if ( iAreaBox != iChkAreaBox )
			{
				// get area box ptr to check against
				sArea* pCheckArea = m_pAreaList[iChkAreaBox];
				int iBX1 = pCheckArea->vecMin.x;
				int iBY1 = pCheckArea->vecMin.y;
				int iBZ1 = pCheckArea->vecMin.z;
				int iBX2 = pCheckArea->vecMax.x;
				int iBY2 = pCheckArea->vecMax.y;
				int iBZ2 = pCheckArea->vecMax.z;

				// touch variable
				bool bScanValid = false;
				int iTouchingSide = eUnknown;
				int iOppositeSide = eUnknown;
				int iXStart=0, iXEnd=0;
				int iYStart=0, iYEnd=0;
				int iZStart=0, iZEnd=0;
				int iS1OffX, iS1OffY, iS1OffZ;
				int iS2OffX, iS2OffY, iS2OffZ;

				// NORTH and SOUTH checks scan the XY plane
				bool bNorthSouthScan=false;
				if ( iAZ1==iBZ2 || iAZ2==iBZ1 )
				{
					if ( iAX2>=iBX1 && iAX1<=iBX2 && iAY2>=iBY1 && iAY1<=iBY2 )
					{
						// determine contact region for XY
						iXStart=iAX1; if(iXStart<iBX1) iXStart=iBX1;
						iXEnd=iAX2; if(iXEnd>iBX2) iXEnd=iBX2;
						iYStart=iAY1; if(iYStart<iBY1) iYStart=iBY1;
						iYEnd=iAY2; if(iYEnd>iBY2) iYEnd=iBY2;
						iS1OffX= 0; iS1OffY= 0; iS1OffZ=-1;
						iS2OffX= 0; iS2OffY= 0; iS2OffZ= 0;
						bNorthSouthScan=true;
					}
				}
				if ( iAZ1==iBZ2 )
				{
					if ( bNorthSouthScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eFront;
						iOppositeSide = eBack;
						iZStart = iAZ1;
						iZEnd = iZStart+100;
						bScanValid = true;
						iS1OffZ=-1;
						iS2OffZ=0;
					}
				}
				if ( iAZ2==iBZ1 )
				{
					if ( bNorthSouthScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eBack;
						iOppositeSide = eFront;
						iZStart = iAZ2;
						iZEnd = iZStart+100;
						bScanValid = true;
						iS2OffZ=-1;
						iS1OffZ=0;
					}
				}

				// EAST and WEST checks scan the YZ plane
				bool bEastWestScan=false;
				if ( iAX1==iBX2 || iAX2==iBX1 )
				{
					if ( iAY2>=iBY1 && iAY1<=iBY2 && iAZ2>=iBZ1 && iAZ1<=iBZ2 )
					{
						// determine contact region for YZ
						iYStart=iAY1; if(iYStart<iBY1) iYStart=iBY1;
						iYEnd=iAY2; if(iYEnd>iBY2) iYEnd=iBY2;
						iZStart=iAZ1; if(iZStart<iBZ1) iZStart=iBZ1;
						iZEnd=iAZ2; if(iZEnd>iBZ2) iZEnd=iBZ2;
						iS1OffX=-1; iS1OffY= 0; iS1OffZ= 0;
						iS2OffX= 0; iS2OffY= 0; iS2OffZ= 0;
						bEastWestScan=true;
					}
				}
				if ( iAX1==iBX2 )
				{
					if ( bEastWestScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eRight;
						iOppositeSide = eLeft;
						iXStart = iAX1;
						iXEnd = iXStart+100;
						bScanValid = true;
						iS1OffX=-1;
						iS2OffX=0;
					}
				}
				if ( iAX2==iBX1 )
				{
					if ( bEastWestScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eLeft;
						iOppositeSide = eRight;
						iXStart = iAX2;
						iXEnd = iXStart+100;
						bScanValid = true;
						iS2OffX=-1;
						iS1OffX=0;
					}
				}

				// UP and DOWN checks scan the XZ plane
				bool bUpDownScan=false;
				if ( iAY1==iBY2 || iAY2==iBY1 )
				{
					if ( iAX2>=iBX1 && iAX1<=iBX2 && iAZ2>=iBZ1 && iAZ1<=iBZ2 )
					{
						// determine contact region for XZ
						iXStart=iAX1; if(iXStart<iBX1) iXStart=iBX1;
						iXEnd=iAX2; if(iXEnd>iBX2) iXEnd=iBX2;
						iZStart=iAZ1; if(iZStart<iBZ1) iZStart=iBZ1;
						iZEnd=iAZ2; if(iZEnd>iBZ2) iZEnd=iBZ2;
						iS1OffX= 0; iS1OffY=-1; iS1OffZ= 0;
						iS2OffX= 0; iS2OffY= 0; iS2OffZ= 0;
						bUpDownScan=true;
					}
				}
				if ( iAY1==iBY2 )
				{
					if ( bUpDownScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eBottom;
						iOppositeSide = eTop;
						iYStart = iAY1;
						iYEnd = iYStart+100;
						bScanValid = true;
						iS2OffY=-1;
						iS1OffY=0;
					}
				}
				if ( iAY2==iBY1 )
				{
					if ( bUpDownScan==true )
					{
						// areaboxes do touch
						iTouchingSide = eTop;
						iOppositeSide = eBottom;
						iYStart = iAY2;
						iYEnd = iYStart+100;
						bScanValid = true;
						iS1OffY=-1;
						iS2OffY=0;
					}
				}

				// areabox side touch detected
				if ( bScanValid==true )
				{
					// stores nature of the portals solidity state (0-none/1-partial/2-fully solid)
					int iPartialSolidityDetected=0;

					// scan all nodes along the contact edge for holes
					D3DXVECTOR3 vecPortalMin;
					D3DXVECTOR3 vecPortalMax;
					bool bPortalBuildState = false;
					for ( int iZ=iZStart; iZ<iZEnd; iZ+=100 )
					{
						for ( int iX=iXStart; iX<iXEnd; iX+=100 )
						{
							for ( int iY=iYStart; iY<iYEnd; iY+=100 )
							{
								// get node at coordinate
								int iSolidity=0;
								int iRefX=m_iHalfUniverseSizeX+((iX+50)/100);
								int iRefY=m_iHalfUniverseSizeY+((iY+50)/100);
								int iRefZ=m_iHalfUniverseSizeZ+((iZ+50)/100);
								bool bSide1Free = GetSideFree ( iTouchingSide, iRefX+iS1OffX, iRefY+iS1OffY, iRefZ+iS1OffZ, &iSolidity );
								bool bSide2Free = GetSideFree ( iOppositeSide, iRefX+iS2OffX, iRefY+iS2OffY, iRefZ+iS2OffZ, &iSolidity );
								if ( bSide1Free && bSide2Free )
								{
									// first of new portal uses current ref
									if ( bPortalBuildState==false )
									{
										bPortalBuildState=true;
										iPartialSolidityDetected=0;
										vecPortalMin = D3DXVECTOR3(99999,99999,99999);
										vecPortalMax = D3DXVECTOR3(-99999,-99999,-99999);
									}

									// record partial solidity
									iPartialSolidityDetected = iSolidity;

									// individual node side touches area box
									ExtendPortalMinMax ( iTouchingSide, iX, iY, iZ, &vecPortalMin, &vecPortalMax );
								}
								else
								{
									// a solid detected, recreate portal so far
									if ( bPortalBuildState )
									{
										CreatePortalAndReset ( pCurrentArea, iChkAreaBox, iTouchingSide, vecPortalMin, vecPortalMax, iPartialSolidityDetected );
										iPartialSolidityDetected = 0;
										bPortalBuildState = false;
									}
								}
							}
						}
					}

					// create any portals not created in thiple fornext loop
					if ( bPortalBuildState ) CreatePortalAndReset ( pCurrentArea, iChkAreaBox, iTouchingSide, vecPortalMin, vecPortalMax, iPartialSolidityDetected );
				}
			}
		}
	}

	// stage 2 - create debug information (visuals) for merged area box link portals
	for ( iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// go through links of each area bix
		for ( int iLink=0; iLink<m_pAreaList [ iAreaBox ]->iLinkMax; iLink++ )
		{
			// get link ptr
			sAreaLink* pAreaLink = m_pAreaList [ iAreaBox ]->pLink [ iLink ];
			if ( pAreaLink )
			{
				// store region for debug purposes
				pAreaLink->pDebugRegion = new sNode;
				pAreaLink->pDebugRegion->vecCentre    = pAreaLink->vecPortalMin + ( ( pAreaLink->vecPortalMax - pAreaLink->vecPortalMin ) / 2.0f );	
				pAreaLink->pDebugRegion->vecDimension = ( pAreaLink->vecPortalMax - pAreaLink->vecPortalMin ) / 2.0f;

				// X9 - 060208 - right colours more useful as we can see the portals
				DWORD dwColor = D3DCOLOR_ARGB ( 255, 255, 0, 0 );
				if ( pAreaLink->iSolidity==1 ) dwColor = D3DCOLOR_ARGB ( 255, 0, 255, 0 );
				if ( pAreaLink->iSolidity==2 ) dwColor = D3DCOLOR_ARGB ( 255, 0, 0, 255 );
				CreatePortalVertices ( pAreaLink->pDebugRegion, true, dwColor );
			}
		}
	}

	// complete
	return true;
}

bool cUniverse::RecurseCheckArea ( D3DXVECTOR3* pAtPos, D3DXVECTOR3* pvecToCenter, D3DXVECTOR3* pvecToSize, sArea* pParentArea, DWORD dwFrustumCount, sArea* pCurrentArea )
{
	// reject early - no area box - no seeing
	if ( !pCurrentArea )
		return false;

	// To box must overlap to some degree the areabox
	float fTrimArea = -0.01f;
	if ( pvecToCenter->x + pvecToSize->x >= pCurrentArea->vecMin.x+fTrimArea
	&&   pvecToCenter->y + pvecToSize->y >= pCurrentArea->vecMin.y+fTrimArea
	&&   pvecToCenter->z + pvecToSize->z >= pCurrentArea->vecMin.z+fTrimArea
	&&	 pvecToCenter->x - pvecToSize->x <= pCurrentArea->vecMax.x-fTrimArea
	&&   pvecToCenter->y - pvecToSize->y <= pCurrentArea->vecMax.y-fTrimArea
	&&   pvecToCenter->z - pvecToSize->z <= pCurrentArea->vecMax.z-fTrimArea )
	{
		// center of box must by within (5) units of areabox for light effect
//		float fTrimArea = 5.0f;
		float fTrimArea = 25.0f; //corridors with deep structures do not get lit (why need this at all?)
		if ( pvecToCenter->x >= pCurrentArea->vecMin.x-fTrimArea
		&&   pvecToCenter->y >= pCurrentArea->vecMin.y-fTrimArea
		&&   pvecToCenter->z >= pCurrentArea->vecMin.z-fTrimArea
		&&	 pvecToCenter->x <= pCurrentArea->vecMax.x+fTrimArea
		&&   pvecToCenter->y <= pCurrentArea->vecMax.y+fTrimArea
		&&   pvecToCenter->z <= pCurrentArea->vecMax.z+fTrimArea )
		{
			// check if point can be seen inside frustrum
			if ( dwFrustumCount==0 )
			{
				// no frustrum - full omni illumination
				return true;
			}
			else
			{
				// check if box within frustrum
				if ( CheckRectangleEx ( dwFrustumCount, pvecToCenter->x, pvecToCenter->y, pvecToCenter->z, pvecToSize->x, pvecToSize->y, pvecToSize->z ) )
				{
					// frustrum intersects box, record it
					D3DXPLANE* pCaptureFrustrumPlanes = new D3DXPLANE [ NUM_CULLPLANES ];
					for ( int iP=0; iP<NUM_CULLPLANES; iP++ )
						pCaptureFrustrumPlanes [ iP ] = g_Planes [ dwFrustumCount ][ iP ];

					// add to frustrum list for this box
					m_pFrustrumList.push_back ( pCaptureFrustrumPlanes );
				}
			}
		}
	}

	// recurse through any portals from this area
	for ( int iLink=0; iLink<pCurrentArea->iLinkMax; iLink++ )
	{
		// get link ptr
		sAreaLink* pAreaLink = pCurrentArea->pLink [ iLink ];
		if ( pAreaLink )
		{
			// find linked area box ptr
			int iLinkedArea = pAreaLink->iLinkedTo;
			sArea* pLinkedArea = m_pAreaList [ iLinkedArea ];

			// quick reject if portal is a partially blocked solid (window)
			if ( pAreaLink->iSolidity==1 )
				continue;

			// ensure we dont recurse backwards or go through portal that faces away
			if ( pLinkedArea != pParentArea && IsPortalFacingLookVector ( pAreaLink->iTouchingSide, &pAreaLink->vecPortalCenter, pAtPos )==true )
			{
				// ensure portal is in view of present frustrum (frustrum zero is omni)
				if ( dwFrustumCount>0 )
					if ( !CheckRectangleEx ( dwFrustumCount, pAreaLink->vecPortalCenter.x, pAreaLink->vecPortalCenter.y, pAreaLink->vecPortalCenter.z, pAreaLink->vecPortalDim.x, pAreaLink->vecPortalDim.y, pAreaLink->vecPortalDim.z ) )
						continue;

				// store frustrum
				D3DXPLANE StorePlanes [ NUM_CULLPLANES ];
				if ( dwFrustumCount>0 )
					for ( int iP=0; iP<NUM_CULLPLANES; iP++ )
						StorePlanes [ iP ] = g_Planes [ dwFrustumCount ][ iP ];

				// vectors
				D3DXVECTOR3 vecNewA = pAreaLink->vecA;
				D3DXVECTOR3 vecNewB = pAreaLink->vecB;
				D3DXVECTOR3 vecNewC = pAreaLink->vecC;
				D3DXVECTOR3 vecNewD = pAreaLink->vecD;

				// always ensure area not already traversed in this traverse-event (different from traversing back out and in another way!)
				bool bSameAreaIndexInTraverseEevent = false;
				if ( dwFrustumCount>1 )
					for ( DWORD dwTrIndex=0; dwTrIndex<dwFrustumCount; dwTrIndex++ )
						if ( m_dwRecurseTable [ dwTrIndex ]==iLinkedArea )
							bSameAreaIndexInTraverseEevent = true;

				// render area box linked to (if not done before)
				if ( bSameAreaIndexInTraverseEevent==false )
				{
					if ( dwFrustumCount<=18 )
					{
						// calculate a new child frustrum (frustrum zero is not valid as it has no shape, ie omni so last param=false)
						SetupPortalFrustum ( dwFrustumCount+1, pAtPos, &vecNewA, &vecNewB, &vecNewC, &vecNewD, false );

						// recurse through this child frustrum
						m_dwRecurseTable [ dwFrustumCount+1 ] = iLinkedArea;
						if ( RecurseCheckArea ( pAtPos, pvecToCenter, pvecToSize, pCurrentArea, dwFrustumCount+1, pLinkedArea )==true )
							return true;
					}
					else
					{
						// use last frustum for remaining nests beyond X
						m_dwRecurseTable [ 19 ] = iLinkedArea;
						if ( RecurseCheckArea ( pAtPos, pvecToCenter, pvecToSize, pCurrentArea, 19, pLinkedArea )==true )
							return true;
					}
				}

				// restore frustrum
				if ( dwFrustumCount>0 )
					for ( int iP=0; iP<NUM_CULLPLANES; iP++ )
						g_Planes [ dwFrustumCount ][ iP ] = StorePlanes [ iP ];
			}
		}
	}

	// did not detect full omni
	return false;
}

bool cUniverse::CanAreaPointSeeBox ( int iAtAreaBox, D3DXVECTOR3* pvecAt, D3DXVECTOR3* pvecToCenter, D3DXVECTOR3* pvecToSize )
{
	// start in current area and see if can see obj from light pos
	if ( iAtAreaBox > 0 )
	{
		DWORD dwFrustrumIndex = 0;
		m_dwRecurseTable [ dwFrustrumIndex ] = iAtAreaBox - 1;
		if ( RecurseCheckArea ( pvecAt, pvecToCenter, pvecToSize, NULL, dwFrustrumIndex, m_pAreaList [ iAtAreaBox-1 ] )==true )
		{
			// can see this point - direct omni illumination
			return true;
		}
	}
	else
	{
		// light outside ALL areaboxes, so light as much as possible (everything in range)
		return true;
	}

	// could not see point from omni
	return false;
}


void cUniverse::DrawDebug ( sNode* pNode )
{
	// draw node bounds and portals

	// check node list size
	if ( m_iNodeListSize == 0 )
		return;

	// draw area box debug view
	for ( int i = 0; i < (int)m_pAreaList.size ( ); i++ )
	{
		// do not draw area box currently in
		if ( g_iAreaBox != i+1 )
		{
			// draw whole area box for those not standing within
			DrawNodeBounds  ( m_pAreaList [ i ]->pDebugRegion );
			DrawNodePortals ( m_pAreaList [ i ]->pDebugRegion );
		}
		else
		{
			// for the area box we are in, draw portals out of the area
			for ( int iLink=0; iLink<m_pAreaList [ i ]->iLinkMax; iLink++ )
			{
				// area link to work on
				sAreaLink* pAreaLink = m_pAreaList [ i ]->pLink [ iLink ];

				// if visuals do not exist, and flagged, create them for debug purposes
				if ( pAreaLink->pDebugRegion==NULL && 1 )
				{
					pAreaLink->pDebugRegion = new sNode;
					pAreaLink->pDebugRegion->vecCentre    = pAreaLink->vecPortalMin + ( ( pAreaLink->vecPortalMax - pAreaLink->vecPortalMin ) / 2.0f );	
					pAreaLink->pDebugRegion->vecDimension = ( pAreaLink->vecPortalMax - pAreaLink->vecPortalMin ) / 2.0f;
					DWORD dwColor = D3DCOLOR_ARGB ( 255, 255, 0, 0 );
					if ( pAreaLink->iSolidity==1 ) dwColor = D3DCOLOR_ARGB ( 255, 0, 255, 0 );
					if ( pAreaLink->iSolidity==2 ) dwColor = D3DCOLOR_ARGB ( 255, 0, 0, 255 );
					CreatePortalVertices ( pAreaLink->pDebugRegion, true, dwColor );
				}

				// draw debug visual
				DrawNodePortals ( pAreaLink->pDebugRegion );
			}
		}
	}
}

bool cUniverse::BuildVisibility ( sNode* pNode )
{
	if ( !pNode )
		return true;

	// if a portal exists in the mesh then we know
	// it has a hole in so the portal must be visible
	if ( pNode->iMeshPortalCount == 0 )
	{
		// go through each mesh in the node
		for ( int iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
		{
			// get pointer to mesh
			sMesh* pMesh = pNode->ppMeshList [ iMesh ];

			// quickly reject mesh if 'nothing' (2-solid,1-partial,0-not solid)
			int iSolidForVisibility=pMesh->iSolidForVisibility;
			if ( iSolidForVisibility==0 )
				continue;

			// iSolidForVisibility controls partially solid portals
			bool bVisStateFalse = false;
			if ( iSolidForVisibility==1 )
				bVisStateFalse = true;

			// * check bounding box of mesh - does it cover a side of node
			// * hide portal which object covers

			// set cover to false to start with
			int iPosition = -1;

			// use variables as lines start to get very long when referring straight
			// to the mesh and node, it's easier to read with smaller variable names
			float fMinX    = pMesh->Collision.vecMin.x;
			float fMinY    = pMesh->Collision.vecMin.y;
			float fMinZ    = pMesh->Collision.vecMin.z;
			float fMaxX    = pMesh->Collision.vecMax.x;
			float fMaxY    = pMesh->Collision.vecMax.y;
			float fMaxZ    = pMesh->Collision.vecMax.z;
			float fDimX    = pNode->vecDimension.x;
			float fDimY    = pNode->vecDimension.y;
			float fDimZ    = pNode->vecDimension.z;
			float fCentreX = pNode->vecCentre.x;
			float fCentreY = pNode->vecCentre.y;
			float fCentreZ = pNode->vecCentre.z;

			// due to floating point margins we have to use an epsilon as values can
			// be slightly off at times, if we don't test within an error range we
			// won't get any portals hidden
			//float epsilon = 0.01f;
			// X9 - 060208 - discovered that large meshes (wider than a node portal) would be rejected
			// from being given an 'iPosition' value because they are too wide as in the case of the maint. corr. deadend
			// which MinX + epsilon >= fCentreX - fDimX  meant 189.0 + 0.01 >= 250 - 50 = false even though it clearly
			// was at that end of the node and was totally blocking it, so we change the EPSILON to allow WIDER meshes by
			// up to 25.0f extra units which means even very thick walled corridors can now be placed so they can ultimately
			// be used to block visibility and seal up the corridor segments.
			float epsilon = 50.0f; // large enough to capture the Service Corridor widths (very wide)

			// now we need to find out where the mesh is in the node
			bool bLocationList [ 8 ];
			DetermineObjectLocation ( pNode, pMesh, bLocationList );
			
			// let's see if this mesh covers a whole top or bottom side
			if ( ( fMinX + epsilon >= fCentreX - fDimX ) && ( fMinX <= fCentreX - fDimX + epsilon ) )
			{
				if ( ( fMinZ + epsilon >= fCentreZ - fDimZ ) && ( fMinZ <= fCentreZ - fDimZ + epsilon ) )
				{
					if ( ( fMaxX + epsilon >= fCentreX + fDimX ) && ( fMaxX <= fCentreX + fDimX + epsilon ) )
					{
						if ( ( fMaxZ + epsilon >= fCentreZ + fDimZ ) && ( fMaxZ <= fCentreZ + fDimZ + epsilon ) )
						{
							if ( 
									bLocationList [ BottomLeftFront ] || bLocationList [ BottomLeftBack   ] ||
									bLocationList [ BottomRightBack ] || bLocationList [ BottomRightFront ]
							   )
									iPosition = eBottom;

							if ( 
									bLocationList [ TopLeftFront ] || bLocationList [ TopLeftBack   ] ||
									bLocationList [ TopRightBack ] || bLocationList [ TopRightFront ]
							   )
									iPosition = eTop;
						}
					}
				}
			}

			// see if we cover front or back of node
			if ( ( fMinX + epsilon >= fCentreX - fDimX ) && ( fMinX <= fCentreX - fDimX + epsilon ) )
			{
				if ( ( fMaxX + epsilon >= fCentreX + fDimX ) && ( fMaxX <= fCentreX + fDimX + epsilon ) )
				{
					if ( ( fMinY + epsilon >= fCentreY - fDimY ) && ( fMinY <= fCentreY - fDimY + epsilon ) )
					{
						if ( ( fMaxY + epsilon >= fCentreY + fDimY ) && ( fMaxY <= fCentreY + fDimY + epsilon ) )
						{
							if (
								 bLocationList [ TopLeftBack     ] || bLocationList [ BottomLeftBack ] ||
								 bLocationList [ BottomRightBack ] || bLocationList [ TopRightBack   ]
							   )
									iPosition = eBack;

							if (
								 bLocationList [ TopLeftFront     ] || bLocationList [ BottomLeftFront ] ||
								 bLocationList [ BottomRightFront ] || bLocationList [ TopRightFront   ]
							   )
									iPosition = eFront;
						}
					}
				}
			}

			// do we cover left or right side of node
			if ( ( fMinZ + epsilon >= fCentreZ - fDimZ ) && ( fMinZ <= fCentreZ - fDimZ + epsilon ) )
			{
				if ( ( fMaxZ + epsilon >= fCentreZ + fDimZ ) && ( fMaxZ <= fCentreZ + fDimZ + epsilon ) )
				{
					if ( ( fMinY + epsilon >= fCentreY - fDimY ) && ( fMinY <= fCentreY - fDimY + epsilon ) )
					{
						if ( ( fMaxY + epsilon >= fCentreY + fDimY ) && ( fMaxY <= fCentreY + fDimY + epsilon ) )
						{
							if (
								 bLocationList [ TopLeftFront    ] || bLocationList [ TopLeftBack    ] ||
								 bLocationList [ BottomLeftFront ] || bLocationList [ BottomLeftBack ]
							   )
									iPosition = eLeft;

							if (
								 bLocationList [ TopRightBack    ] || bLocationList [ TopRightFront    ] ||
								 bLocationList [ BottomRightBack ] || bLocationList [ BottomRightFront ]
							   )
									iPosition = eRight;
						}
					}
				}
			}

			if ( iPosition == eTop )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eTop ].iSolidityMode )
				{
					// now we can turn this portal off
					pNode->portals [ eTop ].bVisible = bVisStateFalse;
					pNode->portals [ eTop ].pBlocker = pMesh;
					pNode->portals [ eTop ].iSolidityMode = iSolidForVisibility;
					// apply to neighbor also
					sNode* pNeighbor = pNode->pNeighbours [ eTop ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eBottom ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eBottom ].pBlocker = pMesh;
						pNeighbor->portals [ eBottom ].iSolidityMode = iSolidForVisibility;
					}
				}
			}

			if ( iPosition == eBottom )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eBottom ].iSolidityMode )
				{
					// now we can turn this portal off
					pNode->portals [ eBottom ].bVisible = bVisStateFalse;
					pNode->portals [ eBottom ].pBlocker = pMesh;
					pNode->portals [ eBottom ].iSolidityMode = iSolidForVisibility;
					// apply to neighbor also
					sNode* pNeighbor = pNode->pNeighbours [ eBottom ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eTop ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eTop ].pBlocker = pMesh;
						pNeighbor->portals [ eTop ].iSolidityMode = iSolidForVisibility;
					}
				}

				// mike - 080803 - store highest vertex point -
				// lee - 271103 - why??
				pNode->portals [ eBottom ].fHighestPoint = -100000.0f;
				sOffsetMap	offsetMap;
				int			iIndexPosition = 0;
				BYTE*		pVertex        = pMesh->pVertexData;
				GetFVFOffsetMap ( pMesh, &offsetMap );
				for ( int iIndex = 0; iIndex < ( int ) pMesh->dwIndexCount / 3; iIndex++ )
				{
					float fNewY = *( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndexPosition ] ) );
					if ( fNewY > pNode->portals [ eBottom ].fHighestPoint )
						pNode->portals [ eBottom ].fHighestPoint = fNewY;
					iIndexPosition++;
				}
			}

			if ( iPosition == eBack )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eBack ].iSolidityMode )
				{
					pNode->portals [ eBack ].bVisible = bVisStateFalse;
					pNode->portals [ eBack ].pBlocker = pMesh;
					pNode->portals [ eBack ].iSolidityMode = iSolidForVisibility;
					sNode* pNeighbor = pNode->pNeighbours [ eBack ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eFront ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eFront ].pBlocker = pMesh;
						pNeighbor->portals [ eFront ].iSolidityMode = iSolidForVisibility;
					}
				}
			}

			if ( iPosition == eFront )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eFront ].iSolidityMode )
				{
					pNode->portals [ eFront ].bVisible = bVisStateFalse;
					pNode->portals [ eFront ].pBlocker = pMesh;
					pNode->portals [ eFront ].iSolidityMode = iSolidForVisibility;
					sNode* pNeighbor = pNode->pNeighbours [ eFront ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eBack ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eBack ].pBlocker = pMesh;
						pNeighbor->portals [ eBack ].iSolidityMode = iSolidForVisibility;
					}
				}
			}

			if ( iPosition == eLeft )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eLeft ].iSolidityMode )
				{
					pNode->portals [ eLeft ].bVisible = bVisStateFalse;
					pNode->portals [ eLeft ].pBlocker = pMesh;
					pNode->portals [ eLeft ].iSolidityMode = iSolidForVisibility;
					sNode* pNeighbor = pNode->pNeighbours [ eLeft ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eRight ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eRight ].pBlocker = pMesh;
						pNeighbor->portals [ eRight ].iSolidityMode = iSolidForVisibility;
					}
				}
			}

			if ( iPosition == eRight )
			{
				// only assign if can upgrade from nothing->partial or partial->solid
				if ( iSolidForVisibility>pNode->portals [ eRight ].iSolidityMode )
				{
					pNode->portals [ eRight ].bVisible = bVisStateFalse;
					pNode->portals [ eRight ].pBlocker = pMesh;
					pNode->portals [ eRight ].iSolidityMode = iSolidForVisibility;
					sNode* pNeighbor = pNode->pNeighbours [ eRight ];
					if ( pNeighbor )
					{
						pNeighbor->portals [ eLeft ].bVisible = bVisStateFalse;
						pNeighbor->portals [ eLeft ].pBlocker = pMesh;
						pNeighbor->portals [ eLeft ].iSolidityMode = iSolidForVisibility;
					}
				}
			}
		}
	}

	return true;
}

void cUniverse::DrawRayCast ( void )
{
	DWORD dwFVF = 0;
	m_pD3D->GetFVF ( &dwFVF );
	m_pD3D->SetFVF ( D3DFVF_XYZ );

	D3DXVECTOR3 vecLines [ 2 ];
	vecLines [ 0 ] = m_vecLineStart;
	vecLines [ 1 ] = m_vecLineEnd;

	// white ray cast line
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,  D3DCULL_NONE );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING,  FALSE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	m_pD3D->DrawPrimitiveUP (
								D3DPT_LINELIST,
								1,
								&vecLines,
								sizeof ( float ) * 3
							);

	m_pD3D->SetFVF ( dwFVF );
}

void cUniverse::SetWorldMatrix ( void )
{
	// set the world matrix

	// create a scaling and position matrix
	D3DXMATRIX	matScale,
				matTranslation,
				matRotation,
				matWorld,
				matRotateX,
				matRotateY,
				matRotateZ;

	D3DXMatrixScaling     ( &matScale,       1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );

	// handle rotations
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( 0.0f ) );
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( 0.0f ) );
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( 0.0f ) );

	// build up final rotation and world matrix
	matRotation = matRotateX  * matRotateY * matRotateZ;
	matWorld    = matRotation * matScale   * matTranslation;

	// set world transform for node tree base
	if ( m_pD3D )
		m_pD3D->SetTransform ( D3DTS_WORLD, &matWorld );
}

bool cUniverse::RenderArea ( DWORD dwFrustumCount, sArea* pArea )
{
	// record draw prims before main render
	DWORD dwStoredPolygonsDrawn, dwStoredDrawPrimCount;
	if ( g_pGlob )
	{
		dwStoredPolygonsDrawn = g_pGlob->dwNumberOfPolygonsDrawn;
		dwStoredDrawPrimCount = g_pGlob->dwNumberOfPrimCalls;
	}

	// render area box
	if ( m_pMasterMeshList.size()>0 )
	{
	    int iIndex = 0;
	    
		// render all meshgroups within areabox
		for ( iIndex = 0; iIndex < (int)pArea->meshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->meshgroups [ iIndex ]->pMesh;
			if ( pMesh )
			{
				if ( pMesh->dwDrawSignature != m_dwDrawSignature )
				{
					pMesh->dwDrawSignature = m_dwDrawSignature;
					if ( m_bGhostDebug ) pMesh->bWireframe=true; else pMesh->bWireframe=false;
					m_ObjectManager.DrawMesh ( pMesh );
				}
			}
		}
		// render all 'shared' meshgroups within areabox
		for ( iIndex = 0; iIndex < (int)pArea->sharedmeshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->sharedmeshgroups [ iIndex ]->pMesh;
			if ( pMesh )
			{
				if ( pMesh->dwDrawSignature != m_dwDrawSignature )
				{
					pMesh->dwDrawSignature = m_dwDrawSignature;
					if ( m_bGhostDebug ) pMesh->bWireframe=true; else pMesh->bWireframe=false;
					m_ObjectManager.DrawMesh ( pMesh );
				}
			}
		}
		// render all meshgroup references outside but connected to areabox
		for ( int iRef = 0; iRef < (int)pArea->meshgroupref.size ( ); iRef++ )
		{
			sMeshGroup* pMeshGroup = pArea->meshgroupref [ iRef ];
			if ( pMeshGroup )
			{
				sMesh* pMesh = pMeshGroup->pMesh;
				if ( pMesh )
				{
					if ( pMesh->dwDrawSignature != m_dwDrawSignature )
					{
						pMesh->dwDrawSignature = m_dwDrawSignature;
						if ( m_bGhostDebug ) pMesh->bWireframe=true; else pMesh->bWireframe=false;
						m_ObjectManager.DrawMesh ( pMesh );
					}
				}
			}
		}
	}
	else
	{
		// when universe is not optimized (by saving it), this is used
		// old way, go through all nodes within the area
		for ( int iNode = 0; iNode < (int)pArea->nodes.size ( ); iNode++ )
		{
			// get a pointer to the node
			sNode* pNode = pArea->nodes [ iNode ];

			// only draw node once per cycle
			if ( m_pNodeUseFlagMap [ pNode->dwNodeRefIndex ]==false )
			{
				// quick reject camera frustrum check
				if ( !CheckRectangleEx ( dwFrustumCount, pNode->vecCentre.x, pNode->vecCentre.y, pNode->vecCentre.z, pNode->vecDimension.x, pNode->vecDimension.y, pNode->vecDimension.z ) )
					continue;

				// draw mesh in any old order for now
				for ( int iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
				{
					sMesh* pMesh = pNode->ppMeshList [ iMesh ];
					if ( pMesh->dwDrawSignature != m_dwDrawSignature )
					{
						// ensure only one mesh per cycle
						pMesh->dwDrawSignature = m_dwDrawSignature;

						// draw mesh from universe
						if ( m_bGhostDebug ) pMesh->bWireframe=true; else pMesh->bWireframe=false;
						m_ObjectManager.DrawMesh ( pMesh );
					}
				}

				// node used this cycle
				m_pNodeUseFlagMap [ pNode->dwNodeRefIndex ]=true;
			}
		}
	}

	// record frustrum zero drawprims for statistics
	if ( g_pGlob && dwFrustumCount==0 )
	{
		m_dwNumberOfCurrentAreaBoxPolygonsDrawn = g_pGlob->dwNumberOfPolygonsDrawn - dwStoredPolygonsDrawn;
		m_dwNumberOfCurrentAreaBoxPrimCalls = g_pGlob->dwNumberOfPrimCalls - dwStoredDrawPrimCount;
	}

	// set this as rendered (for attached obj visibility code)
	pArea->bRenderedThisCycle = true;

	// complete
	return true;
}

bool cUniverse::IsPortalFacingLookVector ( int iSide, D3DXVECTOR3* pvecPortalCenter, D3DXVECTOR3* pvecAt )
{
	// is detection directional?
	if ( pvecPortalCenter && pvecAt )
	{
		// side normal
		D3DXVECTOR3 vecSideNormal;
		switch ( iSide )
		{
			case eBack :	vecSideNormal = D3DXVECTOR3( 0, 0, 1);	break;
			case eFront :	vecSideNormal = D3DXVECTOR3( 0, 0,-1);	break;
			case eLeft :	vecSideNormal = D3DXVECTOR3( 1, 0, 0);	break;
			case eRight :	vecSideNormal = D3DXVECTOR3(-1, 0, 0);	break;
			case eTop :		vecSideNormal = D3DXVECTOR3( 0, 1, 0);	break;
			case eBottom :	vecSideNormal = D3DXVECTOR3( 0,-1, 0);	break;
		}

		// if camera normal faces side normal, we must draw beyond
		D3DXVECTOR3 vecLookNormal = *pvecPortalCenter-*pvecAt;
		float fDiff = D3DXVec3Dot ( &vecLookNormal, &vecSideNormal );
		if ( fDiff >= 0.0f )
		{
			// portal faces the camera
			return true;
		}
	}
	else
	{
		// vector amin0directional
		return true;
	}

	// portal faces away
	return false;
}

int limitrecursions = 0;
int* pReportRecursion = NULL;

bool cUniverse::RecurseRenderArea ( sArea* pParentArea, DWORD dwFrustumCount, sArea* pCurrentArea )
{
	// reject early
	if ( !pCurrentArea )
		return true;

	// render this area
	RenderArea ( dwFrustumCount, pCurrentArea );

	// recurse through any portals from this area
	for ( int iLink=0; iLink<pCurrentArea->iLinkMax; iLink++ )
	{
		// get link ptr
		sAreaLink* pAreaLink = pCurrentArea->pLink [ iLink ];
		if ( pAreaLink )
		{
			// find linked area box ptr
			int iLinkedArea = pAreaLink->iLinkedTo;
			sArea* pLinkedArea = m_pAreaList [ iLinkedArea ];

			// x9 - 060208 - if this area has ALREADY been rendered, no need to traverse it again
			// which solves the iterative chaos that slows a 38fps game down to 10fps. However is
			// there a side effect of not allowing an area to be used by a different traversal in
			// cases where two doors (portals) go through to the same room (areabox), the second
			// traversal fails to explore the portals view fully, so we only skip the areabox if
			// the areabox in question is an OUTSIDE areabox (not an inside one with rooms that
			// must always be considered). So areas with no meshes inside it and already used
			// must be airspace that we have already traversed and will not cause us to cull
			// parts of the scene that multiple portal routes may have directed us to, which
			// hopefully excludes large area spaces within a large room with portals leading in
			// and out of that space (phew).
			// X9 - V110 - 230508 - UMAN had large FPSC level which had a large amount of 
			// interior spaces that where portal divided, and the code below caused some paths
			// not to render causing geometry to disappear, so..
			// V111 - 140608 - we must distinguish between an INTERIOR blank space, and an
			// exterior blank space which I think can be done because sharedmeshgroups is
			// not zero when it is an interior space (so code commented back in to operation)
			if ( m_iFullPortalRecurse==0 )
			{
				// quick portal resurse (only primary meshgroup check made
				if ( pLinkedArea->meshgroups.size()==0 )
					if ( pLinkedArea->bRenderedThisCycle==true )
						continue;
			}
			else
			{
				// full portal recurse (checks main AND shared groups)
				if ( pLinkedArea->meshgroups.size()==0 )
					if ( pLinkedArea->sharedmeshgroups.size()==0 ) // V111 - 140608 - added
						if ( pLinkedArea->bRenderedThisCycle==true )
							continue;
			}

			// ensure we dont recurse backwards
			if ( (dwFrustumCount==0 || IsPortalFacingLookVector ( pAreaLink->iTouchingSide, &pAreaLink->vecPortalCenter, &m_pCameraData->vecPosition )==true)
			&&   pLinkedArea != pParentArea )
			{
				// ensure portal is in view of present frustrum
				// leelee test always
				if ( !CheckRectangleEx ( dwFrustumCount, pAreaLink->vecPortalCenter.x, pAreaLink->vecPortalCenter.y, pAreaLink->vecPortalCenter.z, pAreaLink->vecPortalDim.x, pAreaLink->vecPortalDim.y, pAreaLink->vecPortalDim.z ) )
					continue;
					
				int iP = 0;

				// store frustrum
				D3DXPLANE StorePlanes [ NUM_CULLPLANES ];
				for ( iP=0; iP<NUM_CULLPLANES; iP++ )
					StorePlanes [ iP ] = g_Planes [ dwFrustumCount ][ iP ];

				// vectors
				D3DXVECTOR3 vecNewA, vecNewB, vecNewC, vecNewD;
				D3DXVECTOR3 vecCam = m_pCameraData->vecPosition;
				vecNewA = pAreaLink->vecA;
				vecNewB = pAreaLink->vecB;
				vecNewC = pAreaLink->vecC;
				vecNewD = pAreaLink->vecD;

				// always ensure area not already traversed in this traverse-event (different from traversing back out and in another way!)
				bool bSameAreaIndexInTraverseEevent = false;
				if ( dwFrustumCount>1 )
					for ( DWORD dwTrIndex=0; dwTrIndex<dwFrustumCount; dwTrIndex++ )
						if ( m_dwRecurseTable [ dwTrIndex ]==iLinkedArea )
							bSameAreaIndexInTraverseEevent = true;

				// render area box linked to (if not done before)
				if ( bSameAreaIndexInTraverseEevent==false )
				{
					if ( dwFrustumCount<=18 )
					{
						// upto X frustum nest checks
						SetupPortalFrustum ( dwFrustumCount+1, &vecCam, &vecNewA, &vecNewB, &vecNewC, &vecNewD, true );

						// recurse through child frustrum
						m_dwRecurseTable [ dwFrustumCount+1 ] = iLinkedArea;
						RecurseRenderArea ( pCurrentArea, dwFrustumCount+1, pLinkedArea );
					}
					else
					{
						// use last frustum for remaining nests beyond X
						m_dwRecurseTable [ 19 ] = iLinkedArea;
						RecurseRenderArea ( pCurrentArea, 19, pLinkedArea );
					}
				}

				// restore frustrum
				for ( iP=0; iP<NUM_CULLPLANES; iP++ )
					g_Planes [ dwFrustumCount ][ iP ] = StorePlanes [ iP ];
			}
		}
	}

	// complete
	return true;
}

bool cUniverse::Render ( void )
{
	// ensure created
	if ( !m_bCreated )
		return false;

	// exist still exists
	SAFE_MEMORY ( m_pNodeList );

	// set the world matrix
	SetWorldMatrix ( );

	// gather camera zero details (V111 - 100608 - stereoscopics requires current camera to be X)
	//m_pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( 0 );
	m_pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );
	
	// determine areabox-location of camera (first check current, as 99% of the time you are still there)
	bool bFoundBox = false;
	if ( g_iAreaBox > 0 )
	{
		if ( CollisionBoundBoxTest (	&m_pCameraData->vecPosition,
										&m_pCameraData->vecPosition,
										&m_pAreaList [ g_iAreaBox-1 ]->vecMin,
										&m_pAreaList [ g_iAreaBox-1 ]->vecMax  ) ) bFoundBox = true;
	}
	if ( bFoundBox==false )
	{
		int iNewAreaBox = 0;
		for ( int i = 0; i < (int)m_pAreaList.size ( ); i++ )
		{
			if ( CollisionBoundBoxTest (	&m_pCameraData->vecPosition,
											&m_pCameraData->vecPosition,
											&m_pAreaList [ i ]->vecMin,
											&m_pAreaList [ i ]->vecMax	  ) )
			{
				iNewAreaBox = 1+i;
				break;
			}
		}

		// leemod - 210105 - do not loose last area box if leave, keep visibility of last state of universe (leap out of top of it)
		if ( iNewAreaBox>0 )
			g_iAreaBox = iNewAreaBox;
	}

	// clear visibility flags of the areaboxes
	// U75 - 290310 - can only account for camera zero (as only have one flag to hold the rendered state
	if ( g_pGlob->dwRenderCameraID==0 )
	{
		for ( int i = 0; i < (int)m_pAreaList.size ( ); i++ )
			m_pAreaList [ i ]->bRenderedThisCycle = false;
	}

	// draw area within
	if ( g_iAreaBox > 0 )
	{
		// draw signature (so meshes only draw once)
		m_dwDrawSignature++; if ( m_dwDrawSignature>65535 ) m_dwDrawSignature=0;

		// clear node usage flags (nodes only need adding to scene once)
		if ( m_pNodeUseFlagMap ) memset ( m_pNodeUseFlagMap, 0, sizeof(bool) * m_dwNodeUseFlagMapSize );

		// start with full camera frustrum
		SetupFrustum ( 0.0f );

		// pre draw state resets
		m_ObjectManager.PreDrawSettings();

		// start in current area and recursively draw out from it
		DWORD dwFrustrumIndex = 0;
		m_dwRecurseTable [ dwFrustrumIndex ] = g_iAreaBox - 1;
		RecurseRenderArea ( NULL, dwFrustrumIndex, m_pAreaList [ g_iAreaBox-1 ] );
	}

	// all objects in visible areabox to be visible
	int iVisLinkedObjMax = m_VisLinkedObjectList.size ( );
	for ( int iObj = 0; iObj < iVisLinkedObjMax; iObj++ )
	{
		// object ptr
		sObject* pObject = m_VisLinkedObjectList [ iObj ];
		if ( pObject )
		{
			// adjust object slightly so small downward penetration is allowed (fpsc-190805)
			D3DXVECTOR3 vecAdjPos = pObject->position.vecPosition;
			vecAdjPos.y+=5.0f;

			// FPGC - 050410 - use 'middle' of object to determine which areabox the obj is in
			vecAdjPos += pObject->collision.vecColCenter;

			// first check previous areabox the object was in
			if ( pObject->iInsideUniverseArea>=0 )
			{
				if ( CollisionBoundBoxTest (	&vecAdjPos,
												&vecAdjPos,
												&m_pAreaList [ pObject->iInsideUniverseArea ]->vecMin,
												&m_pAreaList [ pObject->iInsideUniverseArea ]->vecMax	  )==false )
				{
					// moved from areabox
					pObject->iInsideUniverseArea = -1;
				}
			}
			// find area box holding the object
			if ( pObject->iInsideUniverseArea==-1 )
			{
				for ( int i = 0; i < (int)m_pAreaList.size ( ); i++ )
				{
					if ( CollisionBoundBoxTest (	&vecAdjPos,
													&vecAdjPos,
													&m_pAreaList [ i ]->vecMin,
													&m_pAreaList [ i ]->vecMax ) )
					{
						pObject->iInsideUniverseArea = i;
						break;
					}
				}
			}
			// set visibility from areabox state
			if ( pObject->iInsideUniverseArea>=0 )
			{
				// U75 - 290310 - can only account for camera zero (as only have one flag to hold the rendered state
				if ( g_pGlob->dwRenderCameraID==0 )
				{
					// still in this areabox, transfer visiblity state over to object
					pObject->bUniverseVisible = m_pAreaList [ pObject->iInsideUniverseArea ]->bRenderedThisCycle;
				}
				else
					pObject->bUniverseVisible = true;
			}
		}
	}

	// if scorch active, render it
	if ( m_pScorchMesh )
	{
		// renders mesh over top of universe
		m_ObjectManager.DrawMesh ( m_pScorchMesh );
	}

	// if debug active
	if ( m_bDebug )
	{
		DrawDebug ( NULL );
		if ( m_bRayCast ) DrawRayCast ( );
	}

	return true;
}

int cUniverse::RayCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	// set the flag to true
	m_bRayCast = true;

	// perform single ray cast
	return CollisionRayCast ( fX, fY, fZ, fNewX, fNewY, fNewZ );
}

bool cUniverse::RayVolume ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize )
{
	if ( CollisionRayVolume ( fX, fY, fZ, fNewX, fNewY, fNewZ, fSize ) )
		return true;
	else
		return false;

	return false;
}

bool cUniverse::CollisionQuickRayCast ( sMesh* pMesh, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	// for best result from cast
	float fBestDistance = 99999.0f;
	float fBestU, fBestV;
	sMesh* pBestMesh;
	int iBestTriangle;
	if ( CollisionSingleRayCast ( pMesh, fX, fY, fZ, fNewX, fNewY, fNewZ, &fBestDistance, &fBestU, &fBestV, &pBestMesh, &iBestTriangle, false ) )
		return true;
	else
		return false;
}

bool cUniverse::CollisionSingleRayCast (	sMesh* pMesh, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ,
											float* fBestDistance, 
											float* fBestU, 
											float* fBestV, 
											sMesh** pBestMesh,
											int* iBestTriangle,
											bool bCollectFeedback )
{
	// true if best updated
	bool bResult=false;

	// extra data collected for checklist feedback
	int iRefToObject=0, iRefToLimbOfTheObject=0;
	DWORD dwVertex0IndexOfHitPoly, dwVertex1IndexOfHitPoly, dwVertex2IndexOfHitPoly;
	D3DXVECTOR3 vec0Hit, vec1Hit, vec2Hit;
	D3DXVECTOR3 vecHitPoint;

	// vectors
	m_vecLineStart			 = D3DXVECTOR3 ( fX, fY, fZ );				// starting point
	m_vecLineEnd			 = D3DXVECTOR3 ( fNewX, fNewY, fNewZ );		// end point
	D3DXVECTOR3 vecDifference = m_vecLineEnd - m_vecLineStart;			// direction

	// reduce the length of the direction vector
	D3DXVECTOR3 vecDirection;
	D3DXVec3Normalize ( &vecDirection, &vecDifference );

	// get a pointer to the vertex data
	BYTE* pVertex = pMesh->pVertexData;
	int   iIndex  = 0;

	// get the offset map
	sOffsetMap	 offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// indices or vertonly
	int iIndexMax = 0;
	if ( pMesh->dwIndexCount>0 )
		iIndexMax = (int)pMesh->dwIndexCount/3;
	else
		iIndexMax = (int)pMesh->dwVertexCount/3;

	// now go through each triangle
	for ( int iTriangle = 0; iTriangle < iIndexMax; iTriangle++ )
	{
		// get each vector
		D3DXVECTOR3 vecA, vecB, vecC;
		if ( pMesh->dwIndexCount>0 )
		{
			// indices
			vecA = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) )
										   );
			iIndex++;

			vecB = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) )
										   );
			iIndex++;

			vecC = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) )
										   );
			iIndex++;
		}
		else
		{
			// vertonly
			vecA = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;

			vecB = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;

			vecC = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;
		}

		// tri test against ray
		float fU, fV, fDistance = 0.0f;
		if ( D3DXIntersectTri ( &vecA, &vecB, &vecC, &m_vecLineStart, &vecDirection, &fU, &fV, &fDistance ) )
		{
			// we have a collision - is the closest?
			if ( fDistance < (*fBestDistance) )
			{
				// instant feedback data (must be put back eventually)
				*fBestDistance = fDistance;
				*fBestU = fU;
				*fBestV = fV;
				*pBestMesh = pMesh;
				*iBestTriangle = iTriangle;

				// store extra data
				if ( bCollectFeedback )
				{
					if ( pMesh->dwIndexCount>0 )
					{
						dwVertex0IndexOfHitPoly = pMesh->pIndices [ iIndex-3 ];
						dwVertex1IndexOfHitPoly = pMesh->pIndices [ iIndex-2 ];
						dwVertex2IndexOfHitPoly = pMesh->pIndices [ iIndex-1 ];
					}
					else
					{
						dwVertex0IndexOfHitPoly = iIndex-3;
						dwVertex1IndexOfHitPoly = iIndex-2;
						dwVertex2IndexOfHitPoly = iIndex-1;
					}
					vec0Hit = vecA; vec1Hit = vecB; vec2Hit = vecC;

					// new info
					vecHitPoint = m_vecLineStart + (vecDirection*fDistance);
				}

				// flag as hit
				bResult=true;
			}
		}
	}

	// if success, store extra data in feedback
	if ( bResult==true )
	{
		// collect data from final best hit
		if ( bCollectFeedback )
		{
			// get normal of polygon
			D3DXVECTOR3 vecNormal = D3DXVECTOR3 ( 	*( ( float* ) pVertex + offsetMap.dwNX + ( offsetMap.dwSize * dwVertex0IndexOfHitPoly ) ),
													*( ( float* ) pVertex + offsetMap.dwNY + ( offsetMap.dwSize * dwVertex0IndexOfHitPoly ) ),
													*( ( float* ) pVertex + offsetMap.dwNZ + ( offsetMap.dwSize * dwVertex0IndexOfHitPoly ) )   );

			// calculate the direction if the impact bounced off
			D3DXVECTOR3 vecReflectedNormal = vecNormal - ((vecDirection)*-1.0f);
			D3DXVec3Normalize ( &vecReflectedNormal, &vecReflectedNormal );
			vecReflectedNormal = vecReflectedNormal * ((*fBestDistance)/2.0f);

			// create a checklist to store all collision feedback
			MegaCollisionFeedback.iFrameCollision = iRefToObject;
			MegaCollisionFeedback.iFrameRelatedToBone = iRefToLimbOfTheObject;
			MegaCollisionFeedback.dwVertex0IndexOfHitPoly = dwVertex0IndexOfHitPoly;
			MegaCollisionFeedback.dwVertex1IndexOfHitPoly = dwVertex1IndexOfHitPoly;
			MegaCollisionFeedback.dwVertex2IndexOfHitPoly = dwVertex2IndexOfHitPoly;
			MegaCollisionFeedback.vec0Hit = vec0Hit;
			MegaCollisionFeedback.vec1Hit = vec1Hit;
			MegaCollisionFeedback.vec2Hit = vec2Hit;
			MegaCollisionFeedback.vecHitPoint = vecHitPoint;
			MegaCollisionFeedback.vecNormal = vecNormal;
			MegaCollisionFeedback.vecReflectedNormal = vecReflectedNormal;
			MegaCollisionFeedback.pHitMesh = pMesh;
			CreateCollisionChecklist();

			// also store in static data structure (for instant retrieval)
			g_DBPROCollisionResult.dwArbitaryValue = pMesh->Collision.dwArbitaryValue;
		}
	}

	// success
	return bResult;
}

bool cUniverse::CollisionVeryQuickRayCast ( sMesh* pMesh, D3DXVECTOR3* pvecLineStart, D3DXVECTOR3* pvecLineEnd )
{
	// calculate direction
	D3DXVECTOR3 vecDirection;
	D3DXVECTOR3 vecDifference = *pvecLineEnd - *pvecLineStart;
	D3DXVec3Normalize ( &vecDirection, &vecDifference );

	// get a pointer to the vertex data
	BYTE* pVertex = pMesh->pVertexData;
	int   iIndex  = 0;

	// get the offset map
	sOffsetMap	 offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// indices or vertonly
	int iIndexMax = 0;
	if ( pMesh->dwIndexCount>0 )
		iIndexMax = (int)pMesh->dwIndexCount/3;
	else
		iIndexMax = (int)pMesh->dwVertexCount/3;

	// now go through each triangle
	for ( int iTriangle = 0; iTriangle < iIndexMax; iTriangle++ )
	{
		// get each vector
		D3DXVECTOR3 vecA, vecB, vecC;
		if ( pMesh->dwIndexCount>0 )
		{
			// indices
			vecA = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex++ ] ) )
										   );

			vecB = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex++ ] ) )
										   );

			vecC = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * pMesh->pIndices [ iIndex   ] ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * pMesh->pIndices [ iIndex++ ] ) )
										   );
		}
		else
		{
			// vertonly
			vecA = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;

			vecB = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;

			vecC = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iIndex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iIndex ) )
										   );
			iIndex++;
		}

		// tri test against ray
		float fU, fV, fDistance;
		if ( D3DXIntersectTri ( &vecA, &vecB, &vecC, pvecLineStart, &vecDirection, &fU, &fV, &fDistance ) )
		{
			float fLengthOfRay = D3DXVec3Length ( &vecDifference );
			if ( fDistance < fLengthOfRay-0.01f )
				return true;
		}
	}

	// no collision with this mesh
	return false;
}

bool cUniverse::CollisionBoundBoxTest ( D3DXVECTOR3* pvecMinA, D3DXVECTOR3* pvecMaxA, D3DXVECTOR3* pvecMinB, D3DXVECTOR3* pvecMaxB )
{
	// check box intersection
	if (pvecMaxA->x >= pvecMinB->x && pvecMinA->x <= pvecMaxB->x &&
		pvecMaxA->y >= pvecMinB->y && pvecMinA->y <= pvecMaxB->y &&
		pvecMaxA->z >= pvecMinB->z && pvecMinA->z <= pvecMaxB->z )
		return true;
	else
		return false;
}

int cUniverse::CollisionRayCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	// U75 - 150610 - reset so this value remains fresh and reflective of current material after each collision call
	// U76 - 130710 - moved from within ray cast loop (as it got wiped if complex mesh/andor many iterations)
	g_DBPROCollisionResult.dwArbitaryValue = 0;

	// for best result from cast
	float fBestDistance = 99999.0f;
	int iBestReturnDistance = 0;
	float fBestU, fBestV;
	sMesh* pBestMesh;
	int iBestTriangle;

	// calculate boundbox
	D3DXVECTOR3 vecMin, vecMax;
	vecMin.x=fX; vecMin.y=fY; vecMin.z=fZ;
	vecMax.x=fX; vecMax.y=fY; vecMax.z=fZ;
	if ( fNewX<vecMin.x ) vecMin.x=fNewX;
	if ( fNewY<vecMin.y ) vecMin.y=fNewY;
	if ( fNewZ<vecMin.z ) vecMin.z=fNewZ;
	if ( fNewX>vecMax.x ) vecMax.x=fNewX;
	if ( fNewY>vecMax.y ) vecMax.y=fNewY;
	if ( fNewZ>vecMax.z ) vecMax.z=fNewZ;

	// V119 - 081211 - bug meaning we ignore meshes in meshref (large meshes that intersect an areabox, but the
	// areabox containing them is not intersected by the ray bound box!
	// so we clear some flags and tranverse the meshref meshes too, only once
	if ( m_pMasterMeshList.size()>0 )
	{
		for ( int iMeshIndex = 0; iMeshIndex < (int)m_pMasterMeshList.size ( ); iMeshIndex++ )
		{
			sMesh* pMesh = m_pMasterMeshList [ iMeshIndex ];
			pMesh->dwTempFlagUsedDuringUniverseRayCast = 0;
		}
	}

	// go through all areaboxes that touch boundbox
	for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// get current area box ptr
		sArea* pArea = m_pAreaList [ iAreaBox ];

		// is mesh inside areabox
		if ( vecMax.x > pArea->vecMin.x
		&&   vecMin.x < pArea->vecMax.x
		&&   vecMax.y > pArea->vecMin.y
		&&   vecMin.y < pArea->vecMax.y
		&&   vecMax.z > pArea->vecMin.z
		&&   vecMin.z < pArea->vecMax.z )
		{
			if ( m_pMasterMeshList.size()>0 )
			{
				// new collision against meshgroups
				// each area box has within and ahred meshgroups
				//for ( int iBothMeshGroups=0; iBothMeshGroups<2; iBothMeshGroups++ ) // V119 also checking meshref now
				for ( int iBothMeshGroups=0; iBothMeshGroups<3; iBothMeshGroups++ )
				{
					int iIndexMax = 0;
					if ( iBothMeshGroups==0 ) iIndexMax = pArea->meshgroups.size ( );
					if ( iBothMeshGroups==1 ) iIndexMax = pArea->sharedmeshgroups.size ( );
					if ( iBothMeshGroups==2 ) iIndexMax = pArea->meshgroupref.size();
					for ( int iIndex = 0; iIndex < iIndexMax; iIndex++ )
					{
						sMesh* pMesh = NULL;
						if ( iBothMeshGroups==0 ) pMesh = pArea->meshgroups [ iIndex ]->pMesh;
						if ( iBothMeshGroups==1 ) pMesh = pArea->sharedmeshgroups [ iIndex ]->pMesh;
						if ( iBothMeshGroups==2 ) pMesh = pArea->meshgroupref [ iIndex ]->pMesh;
						if ( pMesh )
						{
							// V119 optmise - only check each mesh once for this ray check!
							if ( pMesh->dwTempFlagUsedDuringUniverseRayCast==0 )
							{
								// we check this mesh ONCE to see if the ray intersects it
								pMesh->dwTempFlagUsedDuringUniverseRayCast = 1;
								if ( CollisionBoundBoxTest ( &vecMin, &vecMax, &pMesh->Collision.vecMin, &pMesh->Collision.vecMax ) )
								{
									// raycast against mesh
									if ( CollisionSingleRayCast ( pMesh, fX, fY, fZ, fNewX, fNewY, fNewZ,
																  &fBestDistance, &fBestU, &fBestV, &pBestMesh, &iBestTriangle, true ) )
									{
										// store node of successful collision thus far
										D3DXVECTOR3 vecRayDist = vecMax - vecMin;
										float fRayDist = D3DXVec3Length ( &vecRayDist );
										if ( fBestDistance<fRayDist )
										{
											iBestReturnDistance = (int)fBestDistance;;
											if ( iBestReturnDistance<1 ) iBestReturnDistance=1;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				// old collision with meshes of node
				for ( int iNode = 0; iNode < (int)pArea->nodes.size ( ); iNode++ )
				{
					// get a pointer to the node
					sNode* pNode = pArea->nodes [ iNode ];
					for ( int iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
					{
						sMesh* pMesh = pNode->ppMeshList [ iMesh ];
						if ( pMesh )
						{
							if ( CollisionBoundBoxTest ( &vecMin, &vecMax, &pMesh->Collision.vecMin, &pMesh->Collision.vecMax ) )
							{
								// raycast against mesh
								if ( CollisionSingleRayCast ( pMesh, fX, fY, fZ, fNewX, fNewY, fNewZ,
															  &fBestDistance, &fBestU, &fBestV, &pBestMesh, &iBestTriangle, true ) )
								{
									// store node of successful collision thus far
									D3DXVECTOR3 vecRayDist = vecMax - vecMin;
									float fRayDist = D3DXVec3Length ( &vecRayDist );
									if ( fBestDistance<fRayDist )
									{
										iBestReturnDistance = (int)fBestDistance;;
										if ( iBestReturnDistance<1 ) iBestReturnDistance=1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// collision with ray
	return iBestReturnDistance;
}

void cUniverse::AddNodeColDataToPolygonList ( sNode* pCurrentNode )
{
	for ( int iA = 0; iA < (int)pCurrentNode->collisionE.size ( ); iA++ )
	{
		// expand pool if required
		if ( m_pCollisionPool==NULL )
		{
			m_dwCollisionPoolMax = 1024*9;
			m_pCollisionPool = new float [ m_dwCollisionPoolMax ];
			m_pCollisionDiffuse = new DWORD [ m_dwCollisionPoolMax/9 ];
		}
		else
		{
			if ( m_dwCollisionPoolIndex+4 >= m_dwCollisionPoolMax )
			{
				// pool
				float* pNewLargerPool = new float [ m_dwCollisionPoolMax+(512*9) ];
				memcpy ( pNewLargerPool, m_pCollisionPool, m_dwCollisionPoolMax * sizeof(float) );
				SAFE_DELETE(m_pCollisionPool);
				m_pCollisionPool = pNewLargerPool;

				// diffuse
				DWORD* pNewLargerDiffuse = new DWORD [ (m_dwCollisionPoolMax/9)+512 ];
				memcpy ( pNewLargerDiffuse, m_pCollisionDiffuse, (m_dwCollisionPoolMax/9) * sizeof(DWORD) );
				SAFE_DELETE(m_pCollisionDiffuse);
				m_pCollisionDiffuse = pNewLargerDiffuse;

				// increment permimnantly
				m_dwCollisionPoolMax+=(512*9);
			}
		}

		// fill pool with a polygon
		float* pPoolPtr = &(m_pCollisionPool [ m_dwCollisionPoolIndex ]);
		*(pPoolPtr+0) = pCurrentNode->collisionE [ iA ].triangle[0].vecPosition.x;
		*(pPoolPtr+1) = pCurrentNode->collisionE [ iA ].triangle[0].vecPosition.y;
		*(pPoolPtr+2) = pCurrentNode->collisionE [ iA ].triangle[0].vecPosition.z;
		*(pPoolPtr+3) = pCurrentNode->collisionE [ iA ].triangle[1].vecPosition.x;
		*(pPoolPtr+4) = pCurrentNode->collisionE [ iA ].triangle[1].vecPosition.y;
		*(pPoolPtr+5) = pCurrentNode->collisionE [ iA ].triangle[1].vecPosition.z;
		*(pPoolPtr+6) = pCurrentNode->collisionE [ iA ].triangle[2].vecPosition.x;
		*(pPoolPtr+7) = pCurrentNode->collisionE [ iA ].triangle[2].vecPosition.y;
		*(pPoolPtr+8) = pCurrentNode->collisionE [ iA ].triangle[2].vecPosition.z;
		m_pCollisionDiffuse [ m_dwCollisionPoolIndex/9 ] = pCurrentNode->collisionE [ iA ].diffuseid;
		m_dwCollisionPoolIndex+=9;
	}
}

void cUniverse::AddNodeColDataIfWithinBound ( sNode* pCheckNode, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax )
{
	if ( pCheckNode )
	{
//		leefix - 030205 - replaced with colE bound box check (for static entities overlapping node bounds)
		/*
		float fCentreX = pCheckNode->vecCentre.x;
		float fCentreY = pCheckNode->vecCentre.y;
		float fCentreZ = pCheckNode->vecCentre.z;
		float fDimX    = pCheckNode->vecDimension.x;
		float fDimY    = pCheckNode->vecDimension.y;
		float fDimZ    = pCheckNode->vecDimension.z;
		if ( pvecMin->x <= fCentreX+fDimX )
			if ( pvecMin->y <= fCentreY+fDimY )
				if ( pvecMin->z <= fCentreZ+fDimZ )
					if ( pvecMax->x >= fCentreX-fDimX )
						if ( pvecMax->y >= fCentreY-fDimY )
							if ( pvecMax->z >= fCentreZ-fDimZ )
								AddNodeColDataToPolygonList ( pCheckNode );
		*/
		if ( pvecMin->x <= pCheckNode->vecColMax.x )
			if ( pvecMin->y <= pCheckNode->vecColMax.y )
				if ( pvecMin->z <= pCheckNode->vecColMax.z )
					if ( pvecMax->x >= pCheckNode->vecColMin.x )
						if ( pvecMax->y >= pCheckNode->vecColMin.y )
							if ( pvecMax->z >= pCheckNode->vecColMin.z )
								AddNodeColDataToPolygonList ( pCheckNode );
	}
}

bool cUniverse::CollisionRayVolume ( float fOldX, float fOldY, float fOldZ, float fNewX, float fNewY, float fNewZ, float fScale )
{
	// result var
	bool bResult = false;

	// find pCentralNode of new pos
	sNode* pCentralNode = NULL;
	for ( int iObject = 0; iObject < m_iNodeListSize; iObject++ )
	{
		sNode* pNode = &(m_pNode [ iObject ]);
		if ( pNode )
		{
			float fCentreX = pNode->vecCentre.x;
			float fCentreY = pNode->vecCentre.y;
			float fCentreZ = pNode->vecCentre.z;
			float fDimX    = pNode->vecDimension.x;
			float fDimY    = pNode->vecDimension.y;
			float fDimZ    = pNode->vecDimension.z;
			if ( fNewX <= fCentreX+fDimX )
				if ( fNewY <= fCentreY+fDimY )
					if ( fNewZ <= fCentreZ+fDimZ )
						if ( fNewX >= fCentreX-fDimX )
							if ( fNewY >= fCentreY-fDimY )
								if ( fNewZ >= fCentreZ-fDimZ )
								{
									pCentralNode = pNode;
									break;
								}
		}
	}

	// collision data pre-warped to ellipse of 1,3,1 shape (for performance)
	D3DXVECTOR3 vecEllipse = D3DXVECTOR3 ( 10.0f, 30.0f, 10.0f );
	
	//  leeadd - 020205 - take into account not only current size, but size of maximum movement!!
	D3DXVECTOR3 vecMovingEllipse = D3DXVECTOR3 ( 20.0f, 40.0f, 20.0f );

	// calculate boundbox of volume
	D3DXVECTOR3 vecMin, vecMax;
	vecMin.x=fNewX-vecMovingEllipse.x; vecMin.y=fNewY-vecMovingEllipse.y; vecMin.z=fNewZ-vecMovingEllipse.z;
	vecMax.x=fNewX+vecMovingEllipse.x; vecMax.y=fNewY+vecMovingEllipse.y; vecMax.z=fNewZ+vecMovingEllipse.z;

	// clear polygon pool for new batch
	m_dwCollisionPoolIndex = 0;

	// include node and any neighbor nodes within range of volume
	if ( pCentralNode )
	{
		// Add any neighbors node colpolys that are within range
		for ( int iSide=0; iSide<=2; iSide++ )
		{
			// 0-X,1+X,2-center XCX
			sNode* pNode = pCentralNode->pNeighbours [ iSide ];
			if ( iSide==2 ) pNode = pCentralNode;
			AddNodeColDataIfWithinBound ( pNode, &vecMin, &vecMax );

			// for each X, do 4-Z 5+Z on this layer
			if ( pNode )
			{
				// For each node on this later, do 2-Y 3+Y
				for ( int iYSide=2; iYSide<=3; iYSide++ )
				{
					sNode* pCheckHeightNode = pNode->pNeighbours [ iYSide ];
					AddNodeColDataIfWithinBound ( pCheckHeightNode, &vecMin, &vecMax );
				}

				for ( int iZSide=4; iZSide<=5; iZSide++ )
				{
					// Both Z neighbors
					sNode* pCheckNode = pNode->pNeighbours [ iZSide ];
					AddNodeColDataIfWithinBound ( pCheckNode, &vecMin, &vecMax );

					// For each node on this later, do 2-Y 3+Y
					if ( pCheckNode )
					{
						for ( int iYSide=2; iYSide<=3; iYSide++ )
						{
							sNode* pCheckHeightNode = pCheckNode->pNeighbours [ iYSide ];
							AddNodeColDataIfWithinBound ( pCheckHeightNode, &vecMin, &vecMax );
						}
					}
				}
			}
			/* only checks a cross shape (forgets diagonals)
			// go through fore/back/down on each
			if ( pNode )
			{
				for ( int iZSide=2; iZSide<6; iZSide++ )
				{
					// choose node (9 nodes checked in all)
					sNode* pCheckNode = pNode->pNeighbours [ iZSide ];
					AddNodeColDataIfWithinBound ( pCheckNode, &vecMin, &vecMax );
				}
			}
			*/
		}
	}
	
	// Check against all sunmitted polygons
	if ( m_dwCollisionPoolIndex>0 )
	{
		// gather data for final collision check
		D3DXVECTOR3 vecOld     = D3DXVECTOR3 ( fOldX, fOldY, fOldZ );
		D3DXVECTOR3 vecNew     = D3DXVECTOR3 ( fNewX, fNewY, fNewZ );

		// do collision check
		Collision collision;
		collision.Init();
		if ( collision.World (	vecOld,	vecNew,	vecEllipse,	fScale ) )
			bResult = true;
		else
			bResult = false;

		// tally polys used
		m_dwPolygonsForCollision += (m_dwCollisionPoolIndex/9);
		m_dwVolumesTestedForCollision++;
	}
	else
	{
		// no polygons in area, no collision
		memset ( &g_DBPROCollisionResult, 0, sizeof(g_DBPROCollisionResult) );
		bResult = false;
	}

	// Using global store for final result
	g_DBPROCollisionResult.bUsed = true;

	// return result
	return bResult;
}

bool cUniverse::CollisionTest (	int iTestIndex, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2,
								float fNewX1, float fNewY1, float fNewZ1, float fNewX2, float fNewY2, float fNewZ2 )
{
	// report best collision result
	return false;
}

bool cUniverse::SetDebugOn ( void )
{
	m_bDebug = true;
	m_bGhostDebug = true;
	return true;
}

bool cUniverse::SetDebugOff ( void )
{
	m_bDebug = false;
	m_bGhostDebug = false;
	return true;
}

bool cUniverse::AddPortals ( sNode* pNode )
{
	if ( !pNode )
		return true;

	if ( pNode->iMeshCount != 0 )
	{
		cBSPTree	g_tree;
		cProcessPRT	g_portals;
		sBSP		OptionsBSP;
		sPRT		OptionsPRT;
		sBSPStats   StatsBSP;
		sPRTStats   StatsPRT;

		memset ( &StatsBSP, 0, sizeof ( sBSPStats ) );
		memset ( &StatsPRT, 0, sizeof ( sPRTStats ) );

		typedef std::vector < cMesh* > vectorMesh;

		cMesh*			pNewMesh = new cMesh ( );
		int				iOffset  = 0;
		sMesh*			pMesh    = pNode->ppMeshList [ 0 ];
		vectorMesh      vpMeshList;

		if ( pMesh->bTransparency == 0 )
		{
			int iVertexCount = 0;

			OptionsBSP.bEnabled           = true;
			OptionsBSP.iTreeType          = BSP_TYPE_NONSPLIT;
			OptionsBSP.fSplitHeuristic    = 3.0f;
			OptionsBSP.iSplitterSample    = 60;
			OptionsBSP.bRemoveBackLeaves  = true;
			OptionsBSP.bAddBoundingPolys  = false;
			OptionsBSP.bLinkDetailBrushes = false;
			OptionsPRT.bEnabled           = true;
			OptionsPRT.bFullCompile       = true;

			pNewMesh->m_dwFaceCount = pMesh->dwIndexCount / 3;
			pNewMesh->m_ppFaces     = new cFace* [ pMesh->dwIndexCount / 3 ];

			// get the offset map for the FVF
			sOffsetMap offsetMap;
			GetFVFOffsetMap ( pMesh, &offsetMap );

			// go through faces
			for ( int iFace = 0; iFace < (int)pNewMesh->m_dwFaceCount; iFace++ )
			{
				pNewMesh->m_ppFaces [ iFace ] = new cFace;
				pNewMesh->m_ppFaces [ iFace ]->AddVertices ( 3 );

				for ( int iVertex = 0; iVertex < 3; iVertex++ )
				{
					// get vertex position from original mesh
					// get the x, y and z components
					sPortalVertex pVertex;
					memset ( &pVertex, 0, sizeof(pVertex) );
					
					int iVertexOffset = pMesh->pIndices [ iOffset++ ];

					if ( offsetMap.dwZ>0 )
					{
						pVertex.vecPosition.x = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertexOffset ) );
						pVertex.vecPosition.y = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertexOffset ) );
						pVertex.vecPosition.z = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertexOffset ) );
					}

					if ( offsetMap.dwNZ>0 )
					{
						pVertex.vecNormal.x = *( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iVertexOffset ) );
						pVertex.vecNormal.y = *( ( float* ) pMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iVertexOffset ) );
						pVertex.vecNormal.z = *( ( float* ) pMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iVertexOffset ) );
					}

					if ( offsetMap.dwTV[0]>0 )
					{
						pVertex.tu = *( ( float* ) pMesh->pVertexData + offsetMap.dwTU[0] + ( offsetMap.dwSize * iVertexOffset ) );
						pVertex.tv = *( ( float* ) pMesh->pVertexData + offsetMap.dwTV[0] + ( offsetMap.dwSize * iVertexOffset ) );
					}

					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].x        = pVertex.vecPosition.x;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].y        = pVertex.vecPosition.y;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].z        = pVertex.vecPosition.z;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tu       = pVertex.tu;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tv       = pVertex.tv;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.x = pVertex.vecNormal.x;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.y = pVertex.vecNormal.y;
					pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.z = pVertex.vecNormal.z;

					iVertexCount++;
				}

				// calculate the polygon normal
				D3DXVECTOR3 vec0 = D3DXVECTOR3 ( pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 0 ].x, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 0 ].y, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 0 ].z );
				D3DXVECTOR3 vec1 = D3DXVECTOR3 ( pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 1 ].x, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 1 ].y, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 1 ].z );
				D3DXVECTOR3 vec2 = D3DXVECTOR3 ( pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 2 ].x, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 2 ].y, pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ 2 ].z );

				// get the edges
				D3DXVECTOR3 edge1 = ( vec1 ) - ( vec0 );
				D3DXVECTOR3 edge2 = ( vec2 ) - ( vec0 );
				D3DXVECTOR3 vecNormal;

				// get the cross product and normalize it
				D3DXVec3Cross     ( &vecNormal, &edge1, &edge2 );
				D3DXVec3Normalize ( &vecNormal, &vecNormal );

				pNewMesh->m_ppFaces [ iFace ]->m_vecNormal.x = vecNormal.x;
				pNewMesh->m_ppFaces [ iFace ]->m_vecNormal.y = vecNormal.y;
				pNewMesh->m_ppFaces [ iFace ]->m_vecNormal.z = vecNormal.z;
			}

			vpMeshList.push_back ( pNewMesh );

			g_tree.SetOptions ( OptionsBSP );
			g_tree.SetStats   ( &StatsBSP  );

			for ( int iMesh = 0; iMesh < (int)vpMeshList.size ( ); iMesh++ )
			{
				cMesh* pMesh = vpMeshList [ iMesh ];

				if ( !pMesh )
					continue;

				pMesh->m_Bounds.Max.x = 10000.0f;
				pMesh->m_Bounds.Max.y = 10000.0f;
				pMesh->m_Bounds.Max.z = 10000.0f;

				pMesh->m_Bounds.Min.x = -10000.0f;
				pMesh->m_Bounds.Min.y = -10000.0f;
				pMesh->m_Bounds.Min.z = -10000.0f;
				
				g_tree.AddFaces ( pMesh->m_ppFaces, pMesh->m_dwFaceCount, true );
			}

			g_tree.CompileTree ( );

			g_portals.SetOptions ( OptionsPRT );
			g_portals.SetStats   ( &StatsPRT  );

			g_portals.Process ( &g_tree );

			// get the number of portals from the mesh
			pNode->iMeshPortalCount = GetPortalCount ( &g_tree, pMesh );
		
			// if we have some portals then create lists
			if ( pNode->iMeshPortalCount )
			{
				if ( ! CopyPortalVertices ( &g_tree, pNode, pMesh ) )
					return false;
			}
		}

		SAFE_DELETE ( pNewMesh );
	}

	return true;
}

bool cUniverse::CopyPortalVertices ( cBSPTree* pTree, sNode* pNode, sMesh* pMesh )
{
	// allocate memory for portals
	pNode->pMeshPortalList = new sPortal [ pNode->iMeshPortalCount ];

	// ensure memory was allocated
	SAFE_MEMORY ( pNode->pMeshPortalList );

	// array position for mesh vertices
	int iPosition = 0;

	// go through each face
	for ( int iPortal = 0; iPortal < pTree->GetPortalCount ( ); iPortal++ )
	{
		// get the face
		cBSPPortal* pPortal = pTree->GetPortal ( iPortal );

		// see if the portal is valid
		if ( !IsPortalValid ( pPortal, pMesh ) )
			continue;

		if ( !pPortal )
			continue;

		// copy vertices from the portal into the mesh list

		// ignore portals which do not have 4 vertices
		if ( pPortal->m_dwVertexCount != 4 )
			continue;

		CopyVertices ( &pNode->pMeshPortalList [ iPosition ].vertices [ 0 ], &pPortal->m_pVertices [ 0 ] );
		CopyVertices ( &pNode->pMeshPortalList [ iPosition ].vertices [ 1 ], &pPortal->m_pVertices [ 1 ] );
		CopyVertices ( &pNode->pMeshPortalList [ iPosition ].vertices [ 2 ], &pPortal->m_pVertices [ 2 ] );
		CopyVertices ( &pNode->pMeshPortalList [ iPosition ].vertices [ 3 ], &pPortal->m_pVertices [ 3 ] );

		for ( int i = 0; i < 4; i++ )
		{
			pNode->pMeshPortalList [ iPosition ].vertices [ i ].dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 0, 0 );
		}

		// increment array position
		iPosition++;
	}

	// everything went okay
	return true;
}

void cUniverse::CopyVertices ( sPortalVertex* pTo, cVertex* pFrom )
{
	// copy vertices from one structure to another

	pTo->vecPosition.x = pFrom->x;
	pTo->vecPosition.y = pFrom->y;
	pTo->vecPosition.z = pFrom->z;
	pTo->vecNormal.x   = pFrom->Normal.x;
	pTo->vecNormal.y   = pFrom->Normal.y;
	pTo->vecNormal.z   = pFrom->Normal.z;
	pTo->tu            = pFrom->tu;
	pTo->tv            = pFrom->tv;
	pTo->dwDiffuse     = D3DCOLOR_ARGB ( 255, 0, 0, 255 );
}

int	cUniverse::GetPortalCount ( cBSPTree* pTree, sMesh* pMesh )
{
	// get the number of valid portals within a tree

	// used to store number of portals
	int iCount = 0;

	// go through each face
	for ( int iPortal = 0; iPortal < pTree->GetPortalCount ( ); iPortal++ )
	{
		// get the face
		cBSPPortal* pPortal = pTree->GetPortal ( iPortal );

		// see if the portal is valid
		//if ( !IsPortalValid ( pPortal, pMesh ) )
		//	continue;

		if ( pPortal->m_dwVertexCount != 4 )
			continue;
	
		// if it is then increment the portal counter
		iCount++;
	}

	// return the number of valid portals
	return iCount;
}

bool cUniverse::IsPortalValid ( cBSPPortal* pPortal, sMesh* pMesh )
{
	// determine if a portal is valid

	// check the face and mesh
	if ( !pPortal || !pMesh )
		return false;

	// go through each vertex
	for ( int iVertex = 0; iVertex < (int)pPortal->m_dwVertexCount; iVertex++ )
	{
		if ( pPortal->m_pVertices [ iVertex ].x > pMesh->Collision.vecMax.x + 1.0f )
			return false;

		if ( pPortal->m_pVertices [ iVertex ].y > pMesh->Collision.vecMax.y + 1.0f )
			return false;

		if ( pPortal->m_pVertices [ iVertex ].z > pMesh->Collision.vecMax.z + 1.0f )
			return false;

		if ( pPortal->m_pVertices [ iVertex ].x < pMesh->Collision.vecMin.x - 1.0f )
			return false;

		if ( pPortal->m_pVertices [ iVertex ].y < pMesh->Collision.vecMin.y - 1.0f )
			return false;

		if ( pPortal->m_pVertices [ iVertex ].z < pMesh->Collision.vecMin.z - 1.0f )
			return false;
	}

	// the portal must be valid so return true
	return true;
}

bool cUniverse::IsBoxWithinNode ( sNode* pNode, D3DXVECTOR3* pvecCenter, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax, int *piSide )
{
	// epsilon increases size of submitted mesh (so touching meshes are part of node!)
	float epsilon = 1.0f;

	// left or right
	if ( pvecMax->x+epsilon <= GetTopLeftFront ( pNode ).x || pvecMin->x-epsilon >= GetTopRightBack ( pNode ).x )
		return false;

	// above or below
	if ( pvecMax->y+epsilon <= GetBottomLeftFront ( pNode ).y || pvecMin->y-epsilon >= GetTopLeftFront ( pNode ).y )
		return false;

	// behind or front
	if ( pvecMax->z+epsilon <= GetTopLeftBack ( pNode ).z || pvecMin->z-epsilon >= GetTopLeftFront ( pNode ).z )
		return false;

	// box does touch node - work out which side-of-node box is on
	if ( piSide )
	{
		// calculate node bound
		D3DXVECTOR3 vecNodeMin = pNode->vecCentre - pNode->vecDimension;
		D3DXVECTOR3 vecNodeMax = pNode->vecCentre + pNode->vecDimension;

		// no side
		*piSide = -1;

		// left or right
		if ( pvecCenter->y>vecNodeMin.y && pvecCenter->z>vecNodeMin.z )
		{
			if ( pvecCenter->x < pNode->vecCentre.x )
				*piSide = eLeft;
			else
				*piSide = eRight;
		}

		// bottom or top
		if ( pvecCenter->x>vecNodeMin.x && pvecCenter->z>vecNodeMin.z )
		{
			if ( pvecCenter->y < pNode->vecCentre.y )
				*piSide = eBottom;
			else
				*piSide = eTop;
		}

		// front or back
		if ( pvecCenter->x>vecNodeMin.x && pvecCenter->y>vecNodeMin.y )
		{
			if ( pvecCenter->z < pNode->vecCentre.z )
				*piSide = eFront;
			else
				*piSide = eBack;
		}
	}

	// within
	return true;
}

//020205-bool cUniverse::IsBoxPurelyWithinNode ( sNode* pNode, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax )
bool cUniverse::IsBoxPurelyWithinNode ( sNode* pNode, D3DXVECTOR3* pvecCenter )
{
	// left or right
	if ( pvecCenter->x <= GetTopLeftFront ( pNode ).x || pvecCenter->x > GetTopRightBack ( pNode ).x )
		return false;

	// above or below
	if ( pvecCenter->y <= GetBottomLeftFront ( pNode ).y || pvecCenter->y > GetTopLeftFront ( pNode ).y )
		return false;

	// behind or front
	if ( pvecCenter->z <= GetTopLeftBack ( pNode ).z || pvecCenter->z > GetTopLeftFront ( pNode ).z )
		return false;

	// within
	return true;
}

bool cUniverse::IsPointWithinNode ( sNode* pNode, float x, float y, float z )
{
	float epsilon = 0.01f;

	// left or right
	if ( x + epsilon < GetTopLeftFront ( pNode ).x || x + epsilon > GetTopRightBack ( pNode ).x )
		return false;

	// above or below
	if ( y + epsilon < GetBottomLeftFront ( pNode ).y || y + epsilon > GetTopLeftFront ( pNode ).y )
		return false;

	// behind or in front of boundary
	if ( z + epsilon < GetTopLeftBack ( pNode ).z || z + epsilon > GetTopLeftFront ( pNode ).z )
		return false;

	return true;
}

bool cUniverse::SetPortalsOn ( void )
{
	m_bPortalsActivated = true;

	return true;
}

bool cUniverse::SetPortalsOff ( void )
{
	m_bPortalsActivated = false;

	return true;
}

void cUniverse::CreateNodesForScene ( bool bMode )
{
	// this function will do one of 2 tasks -
	//
	//		* count the number of nodes we need to create
	//		* or build the node list
	//
	// when "bMode" is true we count the nodes, when it's
	// false we build the node list

	int	iNodeDimension		= 50;										// node dimension
	int	iNodeSize			= 100;										// size of node
	int	iUniverseDimensionX = m_pNodeList->vecDimension.x;
	int	iUniverseDimensionY = m_pNodeList->vecDimension.y;
	int	iUniverseDimensionZ = m_pNodeList->vecDimension.z;
	int	x					= iUniverseDimensionX - iNodeDimension;		// initial x position
	int	y					= iUniverseDimensionY - iNodeDimension;		// initial y position
	int	z					= iUniverseDimensionZ - iNodeDimension;		// and initial z position
	int	iLoopX				= iUniverseDimensionX / iNodeDimension;		// how many times we need to loop around
	int	iLoopY				= iUniverseDimensionY / iNodeDimension;		// how many times we need to loop around
	int	iLoopZ				= iUniverseDimensionZ / iNodeDimension;		// how many times we need to loop around
	int	iOffset				= 0;

	// set the centre to 0, 0, 0
	m_vecNodeCentre = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	m_iUniverseSizeX = iLoopX;
	m_iUniverseSizeY = iLoopY;
	m_iUniverseSizeZ = iLoopZ;
	m_iHalfUniverseSizeX = m_iUniverseSizeX/2;
	m_iHalfUniverseSizeY = m_iUniverseSizeY/2;
	m_iHalfUniverseSizeZ = m_iUniverseSizeZ/2;

	// see what we need to do
	if ( bMode )
	{
		// we're counting the nodes so allocate one node only
		m_iNodeListSize = 0;
		m_pNode         = new sNode;
		m_pNodeRef      = NULL;
	}
	else
	{
		// we will be building the node list so allocate
		// an array which will be large enough
		m_pNode			= new sNode [ m_iNodeListSize + 1 ];
		DWORD dwNodeRefSize = m_iUniverseSizeX * m_iUniverseSizeY * m_iUniverseSizeZ;
		m_pNodeRef      = new sNode* [ dwNodeRefSize ];
		memset ( m_pNodeRef, 0, sizeof(sNode*) * dwNodeRefSize );

		// also build a nodeusefag array based on size
		m_dwNodeUseFlagMapSize = dwNodeRefSize;
		m_pNodeUseFlagMap = new bool [ m_dwNodeUseFlagMapSize ];
	}

	// go through y
	for ( int iY = 0; iY < iLoopY; iY++ )
	{
		// and x
		for ( int iX = 0; iX < iLoopX; iX++ )
		{
			// and z axis
			for ( int iZ = 0; iZ < iLoopZ; iZ++ )
			{
				// set centre point and dimension
				m_pNode [ iOffset ].vecCentre    = D3DXVECTOR3 ( x, y, z );
				m_pNode [ iOffset ].vecDimension = D3DXVECTOR3 ( iNodeDimension, iNodeDimension, iNodeDimension );

				// node is currently valid
				bool bOk = true;

				// if node is out of x bounds then it's invalid
				if ( m_pNode [ iOffset ].vecCentre.x < m_vecNodeMin.x || m_pNode [ iOffset ].vecCentre.x > m_vecNodeMax.x )
					bOk = false;
				
				// if node is out of z bounds then it's invalid
				if ( m_pNode [ iOffset ].vecCentre.z < m_vecNodeMin.z || m_pNode [ iOffset ].vecCentre.z > m_vecNodeMax.z )
					bOk = false;
				
				// if node is out of y bounds then it's invalid
				if ( m_pNode [ iOffset ].vecCentre.y < m_vecNodeMin.y || m_pNode [ iOffset ].vecCentre.y > m_vecNodeMax.y )
					bOk = false;

				// grid reference in node universe
				DWORD dwNodeRefIndex = 0;
				int iRefX = m_iHalfUniverseSizeX+(x/100);
				int iRefY = m_iHalfUniverseSizeY+(y/100);
				int iRefZ = m_iHalfUniverseSizeZ+(z/100);
				if ( iRefX>=0 && iRefY>=0 && iRefZ>=0 )
				{
					dwNodeRefIndex = iRefX + (iRefY*m_iUniverseSizeX) + (iRefZ*(m_iUniverseSizeX*m_iUniverseSizeY));
				}

				// record pure index to map reference
				m_pNode [ iOffset ].dwNodeRefIndex = dwNodeRefIndex;

				// if node is valid then send to back of node list
				if ( bOk )
				{
					if ( bMode == false )
					{
						// add node centre to universe centre
						m_vecNodeCentre += m_pNode [ iOffset ].vecCentre;

						// store reference to node
						if ( m_pNodeRef ) m_pNodeRef [ dwNodeRefIndex ] = m_pNode+iOffset;

						iOffset++;
					}
					else
					{
						m_iNodeListSize++;
					}
				}

				// move the z by the size of the node
				z -= iNodeSize;
			}

			// reset z and move x
			z  = iUniverseDimensionZ - iNodeDimension;
			x -= iNodeSize;
		}

		// reset x and z and move y down
		z  = iUniverseDimensionZ - iNodeDimension;
		x  = iUniverseDimensionX - iNodeDimension;
		y -= iNodeSize;
	}

	// divide the centre of the universe by the number of
	// created nodes, we now have our true centre point
	m_vecNodeCentre /= iOffset;

	if ( bMode )
		SAFE_DELETE ( m_pNode );
}

void cUniverse::CalculateSolidityOfMeshes ( void )
{
	// go through all meshes in scene
	for ( int iFrame = 0; iFrame < (int)m_pMeshList.size ( ); iFrame++ )
	{
		// check each mesh for solidity
		sMesh* pMesh = m_pMeshList [ iFrame ];
		if ( pMesh->Collision.dwPortalBlocker==2 )
		{
			// definately fully solid according to input param
			pMesh->iSolidForVisibility = 2;
		}
		else
		{
			if ( pMesh->Collision.dwPortalBlocker==1 )
				pMesh->iSolidForVisibility = CheckIfMeshSolid ( pMesh, 10, 10, 10 );
			else
				pMesh->iSolidForVisibility = 0;
		}
	}
}

void cUniverse::GetBoundingBoxForAllMeshes ( void )
{
	// mike - 010903

	// initial bounds for universe
	m_vecNodeMax = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );
	m_vecNodeMin = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );

	// go through all meshes in scene
	for ( int iFrame = 0; iFrame < (int)m_pMeshList.size ( ); iFrame++ )
	{
		// get mesh pointer
		sMesh* pMesh = m_pMeshList [ iFrame ];

		// update vec min if necessary
		if ( pMesh->Collision.vecMin.x < m_vecNodeMin.x ) m_vecNodeMin.x = pMesh->Collision.vecMin.x;
		if ( pMesh->Collision.vecMin.y < m_vecNodeMin.y ) m_vecNodeMin.y = pMesh->Collision.vecMin.y;
		if ( pMesh->Collision.vecMin.z < m_vecNodeMin.z ) m_vecNodeMin.z = pMesh->Collision.vecMin.z;

		// update vec max if necessary
		if ( pMesh->Collision.vecMax.x > m_vecNodeMax.x ) m_vecNodeMax.x = pMesh->Collision.vecMax.x;
		if ( pMesh->Collision.vecMax.y > m_vecNodeMax.y ) m_vecNodeMax.y = pMesh->Collision.vecMax.y;
		if ( pMesh->Collision.vecMax.z > m_vecNodeMax.z ) m_vecNodeMax.z = pMesh->Collision.vecMax.z;
	}

	// ensure minimum size for universe
	if ( m_vecNodeMin.x > - 100.0f ) m_vecNodeMin.x = -100.0f;
	if ( m_vecNodeMin.y >     0.0f ) m_vecNodeMin.y =    0.0f;
	if ( m_vecNodeMin.z > - 100.0f ) m_vecNodeMin.z = -100.0f;
	if ( m_vecNodeMax.x <   100.0f ) m_vecNodeMax.x =  100.0f;
	if ( m_vecNodeMax.y <   100.0f ) m_vecNodeMax.y =  100.0f;
	if ( m_vecNodeMax.z <   100.0f ) m_vecNodeMax.z =  100.0f;
}

void cUniverse::LinkMeshesToNodes ( void )
{
	// add meshes into created nodes
	for ( int iMesh = 0; iMesh < (int)m_pMeshList.size ( ); iMesh++ )
	{
		sMesh* pMesh = m_pMeshList [ iMesh ];
		for ( int iNode = 0; iNode < m_iNodeListSize; iNode++ )
		{
			// if mesh bound within any part of node
			if ( IsBoxWithinNode (	&m_pNode [ iNode ],
									&pMesh->Collision.vecCentre,
									&pMesh->Collision.vecMin,
									&pMesh->Collision.vecMax, NULL ) )
			{
				// add mesh to this node (mesh can be in several nodes)
				AddMeshToNode ( &m_pNode [ iNode ], pMesh );

				// only add mesh-col-data that are PURELY in the node (unlike addmeshtonode above)
//				020205 - collisionE being filled with neighboring meshes - only use own nodes meshes
//				if ( IsBoxPurelyWithinNode (	&m_pNode [ iNode ],
//												&pMesh->Collision.vecMin,
//												&pMesh->Collision.vecMax ) )
				if ( IsBoxPurelyWithinNode (	&m_pNode [ iNode ],
												&pMesh->Collision.vecCentre ) )
				{
					// check collision type of mesh
					if ( m_iMeshCollisionList [ iMesh ] == eBox )
					{
						// use reduced mesh - which stores the bound box created in BUILD
						CopyCollisionDataToNode ( &m_pNode [ iNode ], pMesh, m_pColMeshList [ iMesh ], pMesh->Collision.dwArbitaryValue );

						// leefix - 280104 - removed 1.01f as it was shifting the collision boxes slowly
						/* moved to BUILD function (reduced mesh carries it)
						sMesh* pBound = new sMesh;
						MakeMeshBox ( 	true,
										pBound,
										pMesh->Collision.vecMin.x * 1.0f,
										pMesh->Collision.vecMin.y * 1.0f,
										pMesh->Collision.vecMin.z * 1.0f,
										pMesh->Collision.vecMax.x * 1.0f,
										pMesh->Collision.vecMax.y * 1.0f,
										pMesh->Collision.vecMax.z * 1.0f,
										D3DFVF_XYZ,	0 );

						// if actual poly is less than box, use poly!
						if ( pMesh->dwIndexCount < pBound->dwIndexCount )
						{
							// use full polygon data for collision shape (HSR probably reduced it quite a lot)
							CopyCollisionDataToNode ( &m_pNode [ iNode ], pMesh, NULL, pMesh->Collision.dwArbitaryValue );
						}
						else
						{
							// use mesh box for collision shape
							CopyCollisionDataToNode ( &m_pNode [ iNode ], pBound, NULL, pMesh->Collision.dwArbitaryValue );
						}

						// free usages
						SAFE_DELETE ( pBound );
						*/
					}
					else if ( m_iMeshCollisionList [ iMesh ] == ePolygon )
					{
						// use full polygon data for collision shape
						CopyCollisionDataToNode ( &m_pNode [ iNode ], pMesh, NULL, pMesh->Collision.dwArbitaryValue );
					}
					else if ( m_iMeshCollisionList [ iMesh ] == eReducedPolygon )
					{
						// use reduced mesh from polygon data for collision shape
						CopyCollisionDataToNode ( &m_pNode [ iNode ], pMesh, m_pColMeshList [ iMesh ], pMesh->Collision.dwArbitaryValue );
					}
				}
			}
		}
	}
}

bool cUniverse::BuildPortals ( void )
{
	// set portal flag to true
	m_bPortalsCreated = true;

	// stage 1 - initial set up
	CalculateSolidityOfMeshes  ( );			// uses IsMeshSolid on all meshes in universe
	GetBoundingBoxForAllMeshes ( );			// find the bounding box of all meshes
	CreateNodesForScene        ( true  );	// find out how many nodes we need to create
	CreateNodesForScene        ( false );	// now create the nodes
	LinkMeshesToNodes          ( );			// link meshes to the newly created nodes
	FindNeighbouringNodes      ( );			// find neighbours for all nodes

	// stage 2 - building portals
	for ( int iNode = 0; iNode < m_iNodeListSize; iNode++ )
	{
		// create initial portals
		CreatePortalVertices ( &m_pNode [ iNode ] );
		
		// only need to work on portals if we have a mesh in node
		if ( m_pNode [ iNode ].iMeshCount )
			BuildVisibility	( &m_pNode [ iNode ] );
	}

	// stage 3 - build area boxes
	BuildAreaBoxes ( );

	// stage 4 - build area links
	BuildAreaLinks ( );

	// stage 5 - node meshes may exceed node itself
	CalculateNodeCollisionBoxes ();

	// return back to caller
	return true;
}

void cUniverse::FindNeighbouringNodes ( void )
{
	for ( int iA = 0; iA < m_iNodeListSize; iA++ )
	{
//		// exclude node that has no meshes
//		if ( !m_pNode [ iA ].ppMeshList )
//			continue;

		for ( int iB = 0; iB < m_iNodeListSize; iB++ )
		{
			if ( iA == iB )
				continue;

//			// exclude node that has no meshes
//			if ( !m_pNode [ iB ].ppMeshList )
//				continue;

			// right
			if ( m_pNode [ iA ].vecCentre.x + 100.0f == m_pNode [ iB ].vecCentre.x )
			{
				if ( m_pNode [ iA ].vecCentre.y == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.z == m_pNode [ iB ].vecCentre.z )
					{
						m_pNode [ iA ].pNeighbours [ 4 ] = &m_pNode [ iB ];
					}
				}
			}

			// left
			if ( m_pNode [ iA ].vecCentre.x - 100.0f == m_pNode [ iB ].vecCentre.x )
			{
				if ( m_pNode [ iA ].vecCentre.y == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.z == m_pNode [ iB ].vecCentre.z )
					{
						m_pNode [ iA ].pNeighbours [ 5 ] = &m_pNode [ iB ];
					}
				}
			}

			// back
			if ( m_pNode [ iA ].vecCentre.z - 100.0f == m_pNode [ iB ].vecCentre.z )
			{
				if ( m_pNode [ iA ].vecCentre.y == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.x == m_pNode [ iB ].vecCentre.x )
					{
						m_pNode [ iA ].pNeighbours [ 0 ] = &m_pNode [ iB ];
					}
				}
			}

			// front
			if ( m_pNode [ iA ].vecCentre.z + 100.0f == m_pNode [ iB ].vecCentre.z )
			{
				if ( m_pNode [ iA ].vecCentre.y == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.x == m_pNode [ iB ].vecCentre.x )
					{
						m_pNode [ iA ].pNeighbours [ 1 ] = &m_pNode [ iB ];
					}
				}
			}

			// bottom
			if ( m_pNode [ iA ].vecCentre.z == m_pNode [ iB ].vecCentre.z )
			{
				if ( m_pNode [ iA ].vecCentre.y - 100.0f == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.x == m_pNode [ iB ].vecCentre.x )
					{
						m_pNode [ iA ].pNeighbours [ 3 ] = &m_pNode [ iB ];
					}
				}
			}

			// top
			if ( m_pNode [ iA ].vecCentre.z == m_pNode [ iB ].vecCentre.z )
			{
				if ( m_pNode [ iA ].vecCentre.y + 100.0f == m_pNode [ iB ].vecCentre.y )
				{
					if ( m_pNode [ iA ].vecCentre.x == m_pNode [ iB ].vecCentre.x )
					{
						m_pNode [ iA ].pNeighbours [ 2 ] = &m_pNode [ iB ];
					}
				}
			}
		}
	}
}

bool cUniverse::CopyMeshPropertiesToNewMesh ( sMesh* pOriginalMesh, sMesh* pNewMesh )
{
	// copy mesh properties from one to another

	// check the parameters
	SAFE_MEMORY ( pOriginalMesh );
	SAFE_MEMORY ( pNewMesh );

	// copy mesh properties
	pNewMesh->bWireframe           = pOriginalMesh->bWireframe;
	pNewMesh->bLight               = pOriginalMesh->bLight;
	pNewMesh->bCull                = pOriginalMesh->bCull;
	pNewMesh->bFog                 = pOriginalMesh->bFog;
	pNewMesh->bAmbient             = pOriginalMesh->bAmbient;
	pNewMesh->bGhost               = pOriginalMesh->bGhost;
	pNewMesh->bTransparency        = pOriginalMesh->bTransparency;
	pNewMesh->iGhostMode           = pOriginalMesh->iGhostMode;
	pNewMesh->dwAlphaTestValue     = pOriginalMesh->dwAlphaTestValue;
	pNewMesh->bVisible             = pOriginalMesh->bVisible;
	pNewMesh->bZRead			   = pOriginalMesh->bZRead;
	pNewMesh->bZWrite			   = pOriginalMesh->bZWrite;
	pNewMesh->bZBiasActive		   = pOriginalMesh->bZBiasActive;
	pNewMesh->fZBiasSlopeScale	   = pOriginalMesh->fZBiasSlopeScale;
	pNewMesh->fZBiasDepth		   = pOriginalMesh->fZBiasDepth;
	pNewMesh->bUsesMaterial        = pOriginalMesh->bUsesMaterial;
	pNewMesh->dwTextureCount       = pOriginalMesh->dwTextureCount;
	pNewMesh->dwMeshID			   = pOriginalMesh->dwMeshID;

	// more mesh properties
	pNewMesh->Collision.dwArbitaryValue = pOriginalMesh->Collision.dwArbitaryValue;
	pNewMesh->Collision.dwPortalBlocker = pOriginalMesh->Collision.dwPortalBlocker;

	// copy a reference to the shader\effect
	CopyReferencesToShaderEffects ( pNewMesh, pOriginalMesh );

	// copy collsion and material information
	memcpy ( &pNewMesh->Collision, &pOriginalMesh->Collision, sizeof ( sCollisionData ) );

	// allocat texture array
	pNewMesh->pTextures = new sTexture [ pNewMesh->dwTextureCount ];

	// copy textures
	memcpy ( pNewMesh->pTextures, pOriginalMesh->pTextures, sizeof ( sTexture ) * pNewMesh->dwTextureCount );

	// maybe not the best way to do this?
	if ( strlen ( pNewMesh->pTextures [ 0 ].pName ) < 1 )
	{
		if ( pOriginalMesh->pMultiMaterial )
		{
			strcpy ( pNewMesh->pTextures->pName, pOriginalMesh->pMultiMaterial->pName );
			
			pNewMesh->pTextures->pTexturesRef = pOriginalMesh->pMultiMaterial->pTexturesRef;
		}
	}

	return true;
}

// 
// FINAL UNIVERSE BUILDING CODE
//

void cUniverse::FindAreaBoxHoldingMesh ( sMesh* pMesh, sArea** ppArea )
{
	// for each areabox
	for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// get current area box ptr
		sArea* pArea = m_pAreaList [ iAreaBox ];

		// is mesh inside areabox
//		if ( pMesh->Collision.vecCentre.x > pArea->vecMin.x
//		&&   pMesh->Collision.vecCentre.x < pArea->vecMax.x
//		&&   pMesh->Collision.vecCentre.y > pArea->vecMin.y
//		&&   pMesh->Collision.vecCentre.y < pArea->vecMax.y
//		&&   pMesh->Collision.vecCentre.z > pArea->vecMin.z
//		&&   pMesh->Collision.vecCentre.z < pArea->vecMax.z )
// FPSCV108 - 290208 - meshes on edge can slip through gaps and not ger assigned to an areabox!!
		if ( pMesh->Collision.vecCentre.x >= pArea->vecMin.x
		&&   pMesh->Collision.vecCentre.x < pArea->vecMax.x
		&&   pMesh->Collision.vecCentre.y >= pArea->vecMin.y
		&&   pMesh->Collision.vecCentre.y < pArea->vecMax.y
		&&   pMesh->Collision.vecCentre.z >= pArea->vecMin.z
		&&   pMesh->Collision.vecCentre.z < pArea->vecMax.z )
		{
			// mesh inside this areabox
			*ppArea = pArea;
			return;
		}
	}

	// mesh outside all areaboxes
	*ppArea = NULL;
	return;
}

void cUniverse::BuildAreaBoxMeshGroups ( void )
{
    int iAreaBox = 0;
    
	// clear geometry lists for all areaboxes
	for ( iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		// get current area box ptr
		sArea* pArea = m_pAreaList [ iAreaBox ];
		pArea->meshgroups.clear ( );
		pArea->sharedmeshgroups.clear ( );
		pArea->meshgroupref.clear ( );
	}

	// clear flags in meshes (should only use a mesh ONCE)
	// Signature Modes (0-not dealt with / 1-added)
	for ( iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
		sArea* pArea = m_pAreaList [ iAreaBox ];
		for ( int iNode = 0; iNode < (int)pArea->nodes.size ( ); iNode++ )
		{
			sNode* pNode = pArea->nodes [ iNode ];
			for ( int iMesh = 0; iMesh < (int)pNode->iMeshCount; iMesh++ )
			{
				sMesh* pMesh = pNode->ppMeshList [ iMesh ];
				pMesh->dwDrawSignature = 0;
			}
		}
	}

	// 0 : first for 'shared'
	// 1 : then for meshes 'within'
	// 2 : then for meshes not used (outside universe)
	for ( int iFirstShared=0; iFirstShared<3; iFirstShared++ )
	{
		// for each areabox
		for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
		{
			// get current area box ptr
			sArea* pArea = m_pAreaList [ iAreaBox ];
			if ( !pArea ) continue;

			// go through all nodes and build list of meshgroups
			for ( int iNode = 0; iNode < (int)pArea->nodes.size ( ); iNode++ )
			{
				// get a pointer to the node
				sNode* pNode = pArea->nodes [ iNode ];
				if ( !pNode ) continue;

				// create min max for node
				D3DXVECTOR3 vecNodeMin = pNode->vecCentre - pNode->vecDimension;
				D3DXVECTOR3 vecNodeMax = pNode->vecCentre + pNode->vecDimension;

				// get meshes from node (can be outside node for walls that border on the split)
				for ( int iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
				{
					// get mesh from node
					sMesh* pMesh = pNode->ppMeshList [ iMesh ];
					if ( !pMesh ) continue;

					// mesh either inside or portal blocker
//					if ( pMesh->Collision.vecCentre.x >= pArea->vecMin.x
//					&&   pMesh->Collision.vecCentre.x < pArea->vecMax.x
//					&&   pMesh->Collision.vecCentre.y >= pArea->vecMin.y
//					&&   pMesh->Collision.vecCentre.y < pArea->vecMax.y
//					&&   pMesh->Collision.vecCentre.z >= pArea->vecMin.z
//					&&   pMesh->Collision.vecCentre.z < pArea->vecMax.z )
// FPSCV108 - 290208 - thin meshes exactly on north/west edges did not get shared - use BOUNDBOX against areabox, not center!
					if ( pMesh->Collision.vecMin.x > pArea->vecMin.x
					&&   pMesh->Collision.vecMax.x < pArea->vecMax.x
					&&   pMesh->Collision.vecMin.y > pArea->vecMin.y
					&&   pMesh->Collision.vecMax.y < pArea->vecMax.y
					&&   pMesh->Collision.vecMin.z > pArea->vecMin.z
					&&   pMesh->Collision.vecMax.z < pArea->vecMax.z )
					{
						// do non-shared meshes second
						if ( iFirstShared==1 )
						{
							// mesh not added in yet
							if ( pMesh->dwDrawSignature==0 )
							{
								// mesh is inside correct node
// FPSCV108 - 290208 - no reason to check against node, we are adding to the areabox once, and that's it
//								if ( pMesh->Collision.vecCentre.x >= vecNodeMin.x
//								&&   pMesh->Collision.vecCentre.x < vecNodeMax.x
//								&&   pMesh->Collision.vecCentre.y >= vecNodeMin.y
//								&&   pMesh->Collision.vecCentre.y < vecNodeMax.y
//								&&   pMesh->Collision.vecCentre.z >= vecNodeMin.z
//								&&   pMesh->Collision.vecCentre.z < vecNodeMax.z )
								{
									// WITHIN - entirely property of this node (within the areabox)
									AddMeshToAreaBox ( pArea, pMesh, 0 );
									pMesh->dwDrawSignature = 1;
								}
							}
						}
					}
					else
					{
						/* see below, i think i am right
						// do all shared meshes first
						if ( iFirstShared==0 )
						{
							// on edge, so must be blocker
							int iSharedMeshSideIndex = -1;
							if ( pNode->portals[0].pBlocker==pMesh ) iSharedMeshSideIndex = 0;
							if ( pNode->portals[1].pBlocker==pMesh ) iSharedMeshSideIndex = 1;
							if ( pNode->portals[2].pBlocker==pMesh ) iSharedMeshSideIndex = 2;
							if ( pNode->portals[3].pBlocker==pMesh ) iSharedMeshSideIndex = 3;
							if ( pNode->portals[4].pBlocker==pMesh ) iSharedMeshSideIndex = 4;
							if ( pNode->portals[5].pBlocker==pMesh ) iSharedMeshSideIndex = 5;

							// disregard if blocker side does not match contact against areabox
							if ( iSharedMeshSideIndex>=0 )
							{
								// check side this mesh blocking, and ignore it if not on correct side of areabox
								if ( iSharedMeshSideIndex==3 && pMesh->Collision.vecCentre.y>=pArea->vecMax.y ) iSharedMeshSideIndex = -1;
								if ( iSharedMeshSideIndex==2 && pMesh->Collision.vecCentre.y<=pArea->vecMax.y ) iSharedMeshSideIndex = -1;
							}

							// my service corridor corners are not blockers, but need drawing!
// 280904 - floor added to shared, even though only wall required for rendering!
//							iSharedMeshSideIndex = 6;//all on edge, share with this AB

							// 061204 - absolute should add a mesh that is bounbox overlapping the node!
							if ( iSharedMeshSideIndex==-1 )
							{
								// mesh bound box overlapping node - not on border with it!
								float fEpsilon = 0.1f;
								if ( pMesh->Collision.vecMax.x > vecNodeMin.x + fEpsilon
								&&   pMesh->Collision.vecMin.x < vecNodeMax.x - fEpsilon
								&&   pMesh->Collision.vecMax.y > vecNodeMin.y + fEpsilon
								&&   pMesh->Collision.vecMin.y < vecNodeMax.y - fEpsilon
								&&   pMesh->Collision.vecMax.z > vecNodeMin.z + fEpsilon
								&&   pMesh->Collision.vecMin.z < vecNodeMax.z - fEpsilon )
									iSharedMeshSideIndex = 6;
							}
							
							if ( iSharedMeshSideIndex>=0 )
							{
								// Find neighbor areaboxes so we can add mesh to its shard list
								sArea* pNeighborArea = NULL;
								FindAreaBoxHoldingMesh ( pMesh, &pNeighborArea );

								// if neighbor, associate mesh with that areabox
								if ( pNeighborArea )
								{
									// mesh not added in yet
									if ( pMesh->dwDrawSignature==0 )
									{
										// SHARED - add this mesh to neighboring areabox as a shared meshgroup
										sMeshGroup* pMeshGroupRef = AddMeshToAreaBox ( pArea, pMesh, pNeighborArea );
										pMesh->dwDrawSignature = 1;

										// record meshgroup which now resides in neighboring areabox
										pArea->meshgroupref.push_back ( pMeshGroupRef );
									}
									else
									{
										// find meshgroup in neighbor that holds this mesh
										sMeshGroup* pMeshGroupRef = FindMeshInAreaBox ( pArea, pMesh, pNeighborArea );
										if ( pMeshGroupRef==NULL )
										{
											// odd, as mesh was 'used' and placed in neighbor?
											int lee=42;
										}
										else
										{
											// this areabox ALSO needs to draw mesh, so draw the meshgroup that holds it 
											int iRef = 0;
											
											int iRefMax = pArea->meshgroupref.size();
											for (  iRef = 0; iRef < iRefMax; iRef++ )
												if ( pArea->meshgroupref [ iRef ]==pMeshGroupRef )
													break;

											// only if it is unique to ref list
											if ( iRef>=iRefMax )
												pArea->meshgroupref.push_back ( pMeshGroupRef );
										}
									}
								}
								else
								{
									// no neighbor, must be on edge of universe
									AddMeshToAreaBox ( pArea, pMesh, 0 );
									pMesh->dwDrawSignature = 1;
								}
							}
						}

						// 110804 - finally handle meshes outside ALL areaboxes
						if ( iFirstShared==2 )
						{
							// mesh not added in yet
							if ( pMesh->dwDrawSignature==0 )
							{
								// we know mesh is PART of node, so simply add it
								AddMeshToAreaBox ( pArea, pMesh, 0 );
								pMesh->dwDrawSignature = 1;
							}
						}
						// 110804 - finally handle meshes outside ALL areaboxes
						if ( iFirstShared==2 )
						{
							// mesh not added in yet
							if ( pMesh->dwDrawSignature==0 )
							{
								// we know mesh is PART of node, so simply add it
								AddMeshToAreaBox ( pArea, pMesh, 0 );
								pMesh->dwDrawSignature = 1;
							}
						}
						*/

						// on edge, so must be blocker
						int iSharedMeshSideIndex = -1;
						if ( pNode->portals[0].pBlocker==pMesh ) iSharedMeshSideIndex = 0;
						if ( pNode->portals[1].pBlocker==pMesh ) iSharedMeshSideIndex = 1;
						if ( pNode->portals[2].pBlocker==pMesh ) iSharedMeshSideIndex = 2;
						if ( pNode->portals[3].pBlocker==pMesh ) iSharedMeshSideIndex = 3;
						if ( pNode->portals[4].pBlocker==pMesh ) iSharedMeshSideIndex = 4;
						if ( pNode->portals[5].pBlocker==pMesh ) iSharedMeshSideIndex = 5;

						// disregard if blocker side does not match contact against areabox
						if ( iSharedMeshSideIndex>=0 )
						{
							// check side this mesh blocking, and ignore it if not on correct side of areabox
							if ( iSharedMeshSideIndex==3 && pMesh->Collision.vecCentre.y>=pArea->vecMax.y ) iSharedMeshSideIndex = -1;
							if ( iSharedMeshSideIndex==2 && pMesh->Collision.vecCentre.y<=pArea->vecMax.y ) iSharedMeshSideIndex = -1;
						}

						// 061204 - absolute should add a mesh that is bounbox overlapping the node!
						if ( iSharedMeshSideIndex==-1 )
						{
							// mesh bound box overlapping node - not on border with it!
							float fEpsilon = 0.1f;
							if ( pMesh->Collision.vecMax.x > vecNodeMin.x + fEpsilon
							&&   pMesh->Collision.vecMin.x < vecNodeMax.x - fEpsilon
							&&   pMesh->Collision.vecMax.y > vecNodeMin.y + fEpsilon
							&&   pMesh->Collision.vecMin.y < vecNodeMax.y - fEpsilon
							&&   pMesh->Collision.vecMax.z > vecNodeMin.z + fEpsilon
							&&   pMesh->Collision.vecMin.z < vecNodeMax.z - fEpsilon )
								iSharedMeshSideIndex = 6;
						}
						
						if ( iSharedMeshSideIndex>=0 )
						{
							// do all shared meshes first
							if ( iFirstShared==0 )
							{
								// Find neighbor areaboxes so we can add mesh to its shard list
								sArea* pNeighborArea = NULL;
								FindAreaBoxHoldingMesh ( pMesh, &pNeighborArea );

								// if neighbor, associate mesh with that areabox
								// FPSCV108 - 290208 - changes allow more shared meshes, but need to make sure areabox does not find itself as the neighbor!
								if ( pNeighborArea && pNeighborArea!=pArea )
								{
									// mesh not added in yet
									if ( pMesh->dwDrawSignature==0 )
									{
										// SHARED - add this mesh to neighboring areabox as a shared meshgroup
										sMeshGroup* pMeshGroupRef = AddMeshToAreaBox ( pArea, pMesh, pNeighborArea );
										pMesh->dwDrawSignature = 1;

										// record meshgroup which now resides in neighboring areabox
										pArea->meshgroupref.push_back ( pMeshGroupRef );
									}
									else
									{
										// find meshgroup in neighbor that holds this mesh
										sMeshGroup* pMeshGroupRef = FindMeshInAreaBox ( pArea, pMesh, pNeighborArea );
										if ( pMeshGroupRef==NULL )
										{
											// odd, as mesh was 'used' and placed in neighbor?
											int lee=42;
										}
										else
										{
											// this areabox ALSO needs to draw mesh, so draw the meshgroup that holds it 
											int iRef = 0;
											
											int iRefMax = pArea->meshgroupref.size();
											for (  iRef = 0; iRef < iRefMax; iRef++ )
												if ( pArea->meshgroupref [ iRef ]==pMeshGroupRef )
													break;

											// only if it is unique to ref list
											if ( iRef>=iRefMax )
												pArea->meshgroupref.push_back ( pMeshGroupRef );
										}
									}
								}
							}
							else
							{
								// final pass means all interior and shared meshes are spoken for
								if ( iFirstShared==2 )
								{
									// leeadd - 170105 - only if not used
									if ( pMesh->dwDrawSignature==0 )
									{
										// no neighbor AT ALL, must be on edge of universe
										AddMeshToAreaBox ( pArea, pMesh, 0 );
										pMesh->dwDrawSignature = 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// update vertex and index buffers
	CalculateNodeCollisionBoxes ();
	UploadMeshgroupsToBuffers ();
}

void cUniverse::UploadMeshgroupsToBuffers ( void )
{
	// add final meshgroups to vertex and index buffers
	for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
	    int iIndex = 0;
	    
		// for each meshgroup
		for ( iIndex = 0; iIndex < (int)m_pAreaList [ iAreaBox ]->meshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = m_pAreaList [ iAreaBox ]->meshgroups [ iIndex ]->pMesh;
			if ( pMesh ) m_ObjectManager.AddObjectMeshToBuffers ( pMesh, true );
		}

		// for shared meshgroup
		for ( iIndex = 0; iIndex < (int)m_pAreaList [ iAreaBox ]->sharedmeshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = m_pAreaList [ iAreaBox ]->sharedmeshgroups [ iIndex ]->pMesh;
			if ( pMesh ) m_ObjectManager.AddObjectMeshToBuffers ( pMesh, true );
		}
	}
}

void cUniverse::CalculateNodeCollisionBoxes ( void )
{
	for ( int iObject = 0; iObject < m_iNodeListSize; iObject++ )
	{
		sNode* pCurrentNode = &(m_pNode [ iObject ]);
		if ( pCurrentNode )
		{
			// find bound box of collision data
			D3DXVECTOR3 vecMin = D3DXVECTOR3(999999,999999,999999);
			D3DXVECTOR3 vecMax = D3DXVECTOR3(-999999,-999999,-999999);
			for ( int iA = 0; iA < (int)pCurrentNode->collisionE.size ( ); iA++ )
			{
				for ( int iT=0; iT<3; iT++ )
				{
					// get raw ellipse-crushed geometry of collisionE
					D3DXVECTOR3 vecPos = pCurrentNode->collisionE [ iA ].triangle[iT].vecPosition;

					// uncrush it to get original vectors
					vecPos.x*=10.0f;
					vecPos.y*=30.0f;
					vecPos.z*=10.0f;

					// expand bound box
					if ( vecPos.x < vecMin.x ) vecMin.x=vecPos.x;
					if ( vecPos.y < vecMin.y ) vecMin.y=vecPos.y;
					if ( vecPos.z < vecMin.z ) vecMin.z=vecPos.z;
					if ( vecPos.x > vecMax.x ) vecMax.x=vecPos.x;
					if ( vecPos.y > vecMax.y ) vecMax.y=vecPos.y;
					if ( vecPos.z > vecMax.z ) vecMax.z=vecPos.z;
				}
			}
			pCurrentNode->vecColMin = vecMin;
			pCurrentNode->vecColMax = vecMax;
		}
	}
}

bool cUniverse::DoMeshTexturesMatch ( sMesh* pMeshA, sMesh* pMeshB )
{
	bool bTexturesMatchPerfectly = false;
	if ( pMeshA->dwTextureCount==pMeshB->dwTextureCount )
	{
		if ( pMeshA->pVertexShaderEffect==pMeshB->pVertexShaderEffect )
		{
			// get both texture array ptrs
			sTexture* pCurrentTex = pMeshA->pTextures;
			sTexture* pInputTex = pMeshB->pTextures;
			if ( pCurrentTex && pInputTex )
			{
				// same texture count and effect ptr
				bTexturesMatchPerfectly = true;

				// check all texture stage data for mismatch
				for ( int iStage=0; iStage<(int)pMeshB->dwTextureCount; iStage++ )
				{
					sTexture* pCurrentTexStage = &(pCurrentTex [ iStage ]);
					sTexture* pInputTexStage = &(pInputTex [ iStage ]);
//					if ( pCurrentTexStage->pTexturesRef != pInputTexStage->pTexturesRef) - was NULL for re-builds of UNIVERSE textures
//					if ( pCurrentTexStage->iImageID != pInputTexStage->iImageID )
					if ( stricmp ( pCurrentTexStage->pName, pInputTexStage->pName )!=NULL )
					{
						// data mismatch - textures not the same
						bTexturesMatchPerfectly = false;
					}
				}
			}
		}
	}

	// do these mesh textures match
	return bTexturesMatchPerfectly;
}

sMeshGroup* cUniverse::FindMeshInAreaBox ( sArea* pArea, sMesh* pInputMesh, sArea* pNeighborArea )
{
	// find meshgroup that matches mesh (good bet that it is where similar meshes are)
	if ( pNeighborArea==NULL )
	{
		for ( int iList = 0; iList < (int)pArea->meshgroups.size ( ); iList++ )
		{
			sMeshGroup* pMeshGroup = pArea->meshgroups [ iList ];
			if ( DoMeshTexturesMatch ( pMeshGroup->pMesh, pInputMesh ) )
				return pMeshGroup;
		}
	}
	else
	{
		for ( int iList = 0; iList < (int)pNeighborArea->sharedmeshgroups.size ( ); iList++ )
		{
			sMeshGroup* pMeshGroup = pNeighborArea->sharedmeshgroups [ iList ];
			if ( DoMeshTexturesMatch ( pMeshGroup->pMesh, pInputMesh ) )
				return pMeshGroup;
		}
	}
	return NULL;
}

sMeshGroup* cUniverse::AddMeshToAreaBox ( sArea* pArea, sMesh* pInputMesh, sArea* pNeighborArea )
{
	// find if mesh texture matches any in meshgroup list
	sMeshGroup* pMeshGroup = NULL;
	bool bMeshIsUniqueToMeshGroupList = true;
	if ( pNeighborArea==NULL )
	{
		for ( int iList = 0; iList < (int)pArea->meshgroups.size ( ); iList++ )
		{
			// get meshgroup from list
			pMeshGroup = pArea->meshgroups [ iList ];

			// only consider meshgroups I can fill
			if ( ((pMeshGroup->pMesh->dwVertexCount + pInputMesh->dwVertexCount)/4) >= (0xFFFF/6) )
				continue;

			// leefix - 050408 - V109 BETA2 - ensure 16bit index buffer not exceeded either
			if ( (pMeshGroup->pMesh->dwIndexCount + pInputMesh->dwIndexCount) >= 0x0000FFFF )
				continue;

			// leefix - 050408 - V109 BETA2 - ensure polygons would fit in a 512 lightmap
			if ( (pMeshGroup->pMesh->dwIndexCount + pInputMesh->dwIndexCount)/3 >= 1000 )
				continue;

			// does this texture match new mesh passed in
			if ( DoMeshTexturesMatch ( pMeshGroup->pMesh, pInputMesh ) )
			{
				// yes, can add this mesh to existing meshgroup
				bMeshIsUniqueToMeshGroupList = false;
				break;
			}
		}
	}
	else
	{
		// See if mesh unique or matches an existing meshgroup in neighbor areabox
		if ( pNeighborArea )
		{
			for ( int iList = 0; iList < (int)pNeighborArea->sharedmeshgroups.size ( ); iList++ )
			{
				// get texture from meshgroup list
				pMeshGroup = pNeighborArea->sharedmeshgroups [ iList ];

				// only consider meshgroups I can fill
				if ( ((pMeshGroup->pMesh->dwVertexCount + pInputMesh->dwVertexCount)/4) >= (0xFFFF/6) )
					continue;

				// leefix - 050408 - V109 BETA2 - ensure 16bit index buffer not exceeded either
				if ( (pMeshGroup->pMesh->dwIndexCount + pInputMesh->dwIndexCount) >= 0x0000FFFF )
					continue;

				// leefix - 050408 - V109 BETA2 - ensure polygons would fit in a 512 lightmap
				if ( (pMeshGroup->pMesh->dwIndexCount + pInputMesh->dwIndexCount)/3 >= 1000 )
					continue;

				// does this texture match new mesh passed in
				if ( DoMeshTexturesMatch ( pMeshGroup->pMesh, pInputMesh ) )
				{
					bMeshIsUniqueToMeshGroupList = false;
					break;
				}
			}
		}
	}

	// is mesh texture unique
	if ( bMeshIsUniqueToMeshGroupList==true )
	{
		// create new texture for list (new meshgroup)
		pMeshGroup = new sMeshGroup;
		pMeshGroup->pMesh = new sMesh;

		// fill new meshgroup with data from pMesh
		MakeMeshFromOtherMesh ( true, pMeshGroup->pMesh, pInputMesh, NULL );
		CopyMeshPropertiesToNewMesh ( pInputMesh, pMeshGroup->pMesh );

		// create texture array for new mesh
		pMeshGroup->pMesh->dwTextureCount = pInputMesh->dwTextureCount;
		if ( pMeshGroup->pMesh->dwTextureCount > 0 )
			pMeshGroup->pMesh->pTextures = new sTexture[pInputMesh->dwTextureCount];
		CloneInternalTextures ( pMeshGroup->pMesh, pInputMesh );
		CloneShaderEffects ( pMeshGroup->pMesh, pInputMesh );

		// add new meshgroup to list
		if ( pNeighborArea )
			pNeighborArea->sharedmeshgroups.push_back ( pMeshGroup );
		else
			pArea->meshgroups.push_back ( pMeshGroup );
	}
	else
	{
		// add mesh data only to existing meshgroup
		DWORD dwNewIndexCount = pMeshGroup->pMesh->dwIndexCount;
		DWORD dwNewVertexCount = pMeshGroup->pMesh->dwVertexCount;
		dwNewIndexCount += pInputMesh->dwIndexCount;
		dwNewVertexCount += pInputMesh->dwVertexCount;

		// OMLY if fit
		if ( (dwNewVertexCount/4) < (0xFFFF/6) )
		{
			// create new larger mesh (copy existing mesh data to larger mesh)
			sMesh* pLargerMesh = new sMesh;
			MakeMeshFromOtherMesh ( true, pLargerMesh, pMeshGroup->pMesh, NULL, dwNewIndexCount, dwNewVertexCount );
			CopyMeshPropertiesToNewMesh ( pInputMesh, pLargerMesh );

			// create texture array for new mesh
			pLargerMesh->dwTextureCount = pInputMesh->dwTextureCount;
			if ( pLargerMesh->dwTextureCount > 0 )
				pLargerMesh->pTextures = new sTexture[pInputMesh->dwTextureCount];
			CloneInternalTextures ( pLargerMesh, pInputMesh );
			CloneShaderEffects ( pLargerMesh, pInputMesh );

			// copy additional mesh data passed in to larger mesh
			CopyIndexMeshData ( pLargerMesh, pInputMesh, pMeshGroup->pMesh->dwIndexCount, pInputMesh->dwIndexCount );
			IncrementIndexMeshData ( pLargerMesh, pMeshGroup->pMesh->dwIndexCount, pInputMesh->dwIndexCount, pMeshGroup->pMesh->dwVertexCount );
			CopyVertexMeshDataSameFVF ( pLargerMesh, pInputMesh, pMeshGroup->pMesh->dwVertexCount, pInputMesh->dwVertexCount );

			// set new primitive counts (assume trilist)
			pLargerMesh->iDrawVertexCount = dwNewVertexCount;
			pLargerMesh->iDrawPrimitives  = dwNewIndexCount/3;

			// delete old meshgroup mesh
			SAFE_DELETE ( pMeshGroup->pMesh );

			// larger mesh is now the new meshgroup
			pMeshGroup->pMesh = pLargerMesh;
		}
		else
		{
			// should not get here, if does, means a single area filling up mesh with too many polygons!
			int lee=42;
		}
	}

	// update mesh of meshgroup (if any)
	if ( pMeshGroup ) pMeshGroup->pMesh->bMeshHasBeenReplaced = true;

	// return meshgroup used
	return pMeshGroup;
}

//
// SCORCH CODE
//

void cUniverse::SetScorchTexture ( int iImageID, int iWidth, int iHeight )
{
	// create scorch mesh if not exist
	if ( m_pScorchMesh==NULL )
	{
		// create large mesh to hold all scorch geometry for scene
		sMesh* pMesh = new sMesh;

		// scorch mesh description
		DWORD dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
		DWORD dwIndexCount  = 0xFFFF;
		DWORD dwScorchCount = dwIndexCount / 6;
		DWORD dwVertexCount = dwScorchCount*4;

		// create the single scorch mesh
		if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
		{
			SAFE_DELETE(pMesh);
			return;
		}

		// create layer of scorch polys for visual reference
		DWORD v=0, i=0;
		for ( DWORD p=0; p<dwScorchCount; p++ )
		{
			// poly detail
			float fX = 10.0f;
			float fY = p*0.01f;
			float fZ = 10.0f;

			// create vertex data
			SetupStandardVertex ( dwFVF, pMesh->pVertexData, v+0, 0.0f, fY, 0.0f,  0.0f, 1.0f, 0.0f,  0xFFFFFFFF,  0.0f, 0.0f );
			SetupStandardVertex ( dwFVF, pMesh->pVertexData, v+1, 0.0f, fY, fZ,    0.0f, 1.0f, 0.0f,  0xFFFFFFFF,  0.0f, 1.0f );
			SetupStandardVertex ( dwFVF, pMesh->pVertexData, v+2, fX,   fY, 0.0f,  0.0f, 1.0f, 0.0f,  0xFFFFFFFF,  1.0f, 0.0f );
			SetupStandardVertex ( dwFVF, pMesh->pVertexData, v+3, fX,   fY, fZ,    0.0f, 1.0f, 0.0f,  0xFFFFFFFF,  1.0f, 1.0f );

			// create index data
			pMesh->pIndices [ i++ ] = v+0;
			pMesh->pIndices [ i++ ] = v+1;
			pMesh->pIndices [ i++ ] = v+2;
			pMesh->pIndices [ i++ ] = v+2;
			pMesh->pIndices [ i++ ] = v+1;
			pMesh->pIndices [ i++ ] = v+3;

			// next
			v+=4;
		}

		// reset draw limit
		m_dwPolyDrawLimit = 0;
		
		// setup mesh drawing properties
		pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
		pMesh->iDrawVertexCount = dwVertexCount;
		pMesh->iDrawPrimitives  = m_dwPolyDrawLimit;

		// set some properties common to scorch
		pMesh->bTransparency=true;
		pMesh->bZWrite=false;

		// add this mesh to the draw buffers
		m_ObjectManager.AddObjectMeshToBuffers ( pMesh, true );

		// assign new mesh to ptr
		m_pScorchMesh = pMesh;
		m_iScorchTypeMax = (iWidth*iHeight)-1;
		m_iScorchUVWidth = iWidth;
		m_fScorchUTile = 1.0f/iWidth;
		m_fScorchVTile = 1.0f/iHeight;
	}

	// apply texture image to scorch mesh
	SetBaseTexture ( m_pScorchMesh, 0, iImageID );

	// set texture address mode
	SetTextureMode ( m_pScorchMesh, 3, 0 );

	// scorch texture must account for FOG (FPSCV104RC5)
	SetGhost ( m_pScorchMesh, true, 11 ); //1 );

	// set scorch texture to use texture only ( no diffuse )
	m_pScorchMesh->pTextures [ 0 ].dwBlendMode = D3DTOP_MODULATE;
	m_pScorchMesh->pTextures [ 0 ].dwBlendArg1 = D3DTA_TEXTURE;
	m_pScorchMesh->pTextures [ 0 ].dwBlendArg2 = D3DTA_DIFFUSE;
	m_pScorchMesh->bFog = true;
}

void cUniverse::AddScorch ( float fSize, int iScorchType )
{
	// only if scorch setup
	if ( m_pScorchMesh )
	{
		// ensure type does not exceed max
		if ( iScorchType < 0 ) iScorchType=0;
		if ( iScorchType > m_iScorchTypeMax ) iScorchType=m_iScorchTypeMax;

		// UV data for correct bullet hole
		int iDown = (iScorchType/m_iScorchUVWidth);
		int iAcross = iScorchType - (iDown * m_iScorchUVWidth);
		float fBaseU = iAcross*m_fScorchUTile;
		float fBaseV = iDown*m_fScorchVTile;
		float fU0 = fBaseU+m_fScorchUTile;
		float fV0 = fBaseV;
		float fU1 = fBaseU+m_fScorchUTile;
		float fV1 = fBaseV+m_fScorchVTile;
		float fU2 = fBaseU;
		float fV2 = fBaseV;
		float fU3 = fBaseU;
		float fV3 = fBaseV+m_fScorchVTile;
		D3DXVECTOR3 vec0 = MegaCollisionFeedback.vecHitPoint;
		D3DXVECTOR3 vec1 = MegaCollisionFeedback.vecHitPoint;
		D3DXVECTOR3 vec2 = MegaCollisionFeedback.vecHitPoint;
		D3DXVECTOR3 vec3 = MegaCollisionFeedback.vecHitPoint;
		D3DXVECTOR3 vecN = MegaCollisionFeedback.vecNormal;
		float ax, ay, az;
		GetAngleFromPoint ( 0.0f, 0.0f, 0.0f, vecN.x, vecN.y, vecN.z, &ax, &ay, &az );
		D3DXMATRIX matOut;
		D3DXMatrixRotationYawPitchRoll ( &matOut, D3DXToRadian(ay), D3DXToRadian(ax), D3DXToRadian(az) );
		D3DXVECTOR3 vecA = D3DXVECTOR3( -0.5f,  0.5f, 0.0f);
		D3DXVECTOR3 vecB = D3DXVECTOR3( -0.5f, -0.5f, 0.0f);
		D3DXVECTOR3 vecC = D3DXVECTOR3(  0.5f,  0.5f, 0.0f);
		D3DXVECTOR3 vecD = D3DXVECTOR3(  0.5f, -0.5f, 0.0f);
		vecA*=fSize;
		vecB*=fSize;
		vecC*=fSize;
		vecD*=fSize;
		D3DXVec3TransformCoord ( &vecA, &vecA, &matOut );
		D3DXVec3TransformCoord ( &vecB, &vecB, &matOut );
		D3DXVec3TransformCoord ( &vecC, &vecC, &matOut );
		D3DXVec3TransformCoord ( &vecD, &vecD, &matOut );
		D3DXVECTOR3 vecAt = vecN;
		vecAt=vecAt/5.0f;
		vec0+=vecA+vecAt;
		vec1+=vecB+vecAt;
		vec2+=vecC+vecAt;
		vec3+=vecD+vecAt;

		// if corners of scorch touch mesh, add it (not leaking off edge)
		if ( MegaCollisionFeedback.pHitMesh )
		{
			if(	CollisionQuickRayCast ( MegaCollisionFeedback.pHitMesh, vec0.x+vecN.x, vec0.y+vecN.y, vec0.z+vecN.z, vec0.x-vecN.x, vec0.y-vecN.y, vec0.z-vecN.z )
			&&	CollisionQuickRayCast ( MegaCollisionFeedback.pHitMesh, vec1.x+vecN.x, vec1.y+vecN.y, vec1.z+vecN.z, vec1.x-vecN.x, vec1.y-vecN.y, vec1.z-vecN.z )
			&&	CollisionQuickRayCast ( MegaCollisionFeedback.pHitMesh, vec2.x+vecN.x, vec2.y+vecN.y, vec2.z+vecN.z, vec2.x-vecN.x, vec2.y-vecN.y, vec2.z-vecN.z )
			&&	CollisionQuickRayCast ( MegaCollisionFeedback.pHitMesh, vec3.x+vecN.x, vec3.y+vecN.y, vec3.z+vecN.z, vec3.x-vecN.x, vec3.y-vecN.y, vec3.z-vecN.z ) )
			{
				// add detected scorch polygons to scorch mesh
				DWORD dwVertPos=0;
				sMesh* pMesh = m_pScorchMesh;
				SetupShortVertex ( pMesh->dwFVF, pMesh->pVertexData, m_dwScorchVPos+0, vec0.x, vec0.y, vec0.z, fU0, fV0 );
				SetupShortVertex ( pMesh->dwFVF, pMesh->pVertexData, m_dwScorchVPos+1, vec1.x, vec1.y, vec1.z, fU1, fV1 );
				SetupShortVertex ( pMesh->dwFVF, pMesh->pVertexData, m_dwScorchVPos+2, vec2.x, vec2.y, vec2.z, fU2, fV2 );
				SetupShortVertex ( pMesh->dwFVF, pMesh->pVertexData, m_dwScorchVPos+3, vec3.x, vec3.y, vec3.z, fU3, fV3 );

				// update mesh at this point (record for later)
				DWORD m_dwScorchVPosAdded = m_dwScorchVPos;

				// wrap around when get to end of scorch-polys
				m_dwScorchVPos+=4;
				if ( m_dwScorchVPos>pMesh->dwVertexCount-4 )
					m_dwScorchVPos=0;

				// wrap around when INDEX BUFFER max reached
				if ( (m_dwScorchVPos/4) >= (0xFFFF/6) )
					m_dwScorchVPos=0;

				// increment draw limit (only need to draw what has been created)
				if ( m_dwPolyDrawLimit<m_dwScorchVPos/2 )
				{
					m_dwPolyDrawLimit=m_dwScorchVPos/2;
					pMesh->iDrawPrimitives  = m_dwPolyDrawLimit;
				}

				// update mesh (quickly)
				m_ObjectManager.QuicklyUpdateObjectMeshInBuffer ( pMesh, m_dwScorchVPosAdded, m_dwScorchVPosAdded+4 );
			}
		}
	}
}

//
// Shadow functions
//

void cUniverse::SetShadow ( sMesh* pMeshCastingShadow, D3DXMATRIX* pMeshMatrix )
{
	// Activate stencil effect
	g_pGlob->dwStencilShadowCount++;
	g_pGlob->dwStencilMode=1;
	g_pGlob->dwRedrawCount=1;

	// add caster mesh and matrix to shadow caster master list
	sShadowCaster ShadowCaster;
	if ( pMeshMatrix )
		ShadowCaster.matPos = *pMeshMatrix;
	else
		D3DXMatrixIdentity ( &ShadowCaster.matPos );

	ShadowCaster.pMeshRef = pMeshCastingShadow;
	m_pShadowCasterMasterList.push_back ( ShadowCaster );
}

void cUniverse::SetLight ( int iLightIndex, float fX, float fY, float fZ, float fRange, bool bRecreateAllShadows )
{
	// check range
	if ( iLightIndex < 1 || iLightIndex > 256 )
		return;

	// expand list to include index
	if ( iLightIndex >= (int)m_pShadowLightList.size ( ) )
	{
		sShadowLight shadowlight;

		while ( iLightIndex >= (int)m_pShadowLightList.size ( ) )
			m_pShadowLightList.push_back ( shadowlight );
	}

	// set data for specified light
	sShadowLight* pCurrentLight = &m_pShadowLightList[iLightIndex];
	if ( pCurrentLight )
	{
		// set light position
		pCurrentLight->fX = fX;
		pCurrentLight->fY = fY;
		pCurrentLight->fZ = fZ;
		pCurrentLight->fRange = fRange;

		// find all casters within light range (range code later)
		if ( bRecreateAllShadows==true && pCurrentLight->pShadowMeshList.size()==0)
		{
			// free any old shadows
			for ( int iShadow=0; iShadow<(int)pCurrentLight->pShadowMeshList.size(); iShadow++ )
				SAFE_DELETE ( pCurrentLight->pShadowMeshList [ iShadow ] );

			// clears lists
			pCurrentLight->pCasterRefList.clear();
			pCurrentLight->pShadowMeshList.clear();

			// create new shadows
			for ( int iCaster=0; iCaster<(int)m_pShadowCasterMasterList.size(); iCaster++ )
			{
				// for each caster
				sShadowCaster* pShadowCaster = &m_pShadowCasterMasterList [ iCaster ];
				if ( pShadowCaster )
				{
					// create new shadow mesh just for this caster and light (ie caster can have ten shadows if there are ten lights)
					sMesh* pShadowMesh = NULL;
					if ( CreateShadowMesh ( pShadowCaster->pMeshRef, &pShadowMesh, 0 ) )
					{
						// shadow valid
						if ( pShadowMesh )
						{
							// shadow mesh is outside object update cycle, so do manually
							m_ObjectManager.AddObjectMeshToBuffers ( pShadowMesh, false );

							// add to list
							pCurrentLight->pCasterRefList.push_back ( pShadowCaster );
							pCurrentLight->pShadowMeshList.push_back ( pShadowMesh );
						}
					}
				}
			}
		}

		// for each shadow owned by light
		for ( int iShadow=0; iShadow<(int)pCurrentLight->pShadowMeshList.size(); iShadow++ )
		{
			// update shadow to new altered light position
			sMesh* pCasterMesh = pCurrentLight->pCasterRefList [ iShadow ]->pMeshRef;
			D3DXMATRIX* pCasterPos = &pCurrentLight->pCasterRefList [ iShadow ]->matPos;
			sMesh* pShadowMesh = pCurrentLight->pShadowMeshList [ iShadow ];
			// migrated shadows to shader - finish this when time is right
			// UpdateShadowMesh ( pCurrentLight->fX, pCurrentLight->fY, pCurrentLight->fZ, pCurrentLight->fRange, pCasterMesh, pShadowMesh, pCasterPos, NULL );
			// Put this back on 050908 as it stopped static shadows working..
			// but we need the shadows to cast 'away from light', so we need to fill the caster pos which seems
			// not to have any world data for the static objects in this calculation right now, so we add it:
			pCurrentLight->pCasterRefList [ iShadow ]->matPos._41 = pCasterMesh->Collision.vecCentre.x;
			pCurrentLight->pCasterRefList [ iShadow ]->matPos._42 = pCasterMesh->Collision.vecCentre.y;
			pCurrentLight->pCasterRefList [ iShadow ]->matPos._43 = pCasterMesh->Collision.vecCentre.z;
			UpdateShadowMesh ( pCurrentLight->fX, pCurrentLight->fY, pCurrentLight->fZ, pCurrentLight->fRange, pCasterMesh, pShadowMesh, pCasterPos, NULL );
			m_ObjectManager.UpdateObjectMeshInBuffer ( pShadowMesh );
		}

		// unlock all buffers after updates
		m_ObjectManager.CompleteUpdateInBuffers();
	}
}

void cUniverse::RenderShadows ( void )
{
	// see debug
//	bool bNoDebugShadows = false;
	bool bNoDebugShadows = true;

	// set world matrix
	D3DXMATRIX matDef;
	D3DXMatrixIdentity ( &matDef );
	m_pD3D->SetTransform ( D3DTS_WORLD, &matDef );

	// setup for stencil writing
	if ( bNoDebugShadows ) m_ObjectManager.StartStencilMeshWrite();

	// render all shadows owned by lights within range of current location
	for ( int iLightIndex=0; iLightIndex<(int)m_pShadowLightList.size(); iLightIndex++ )
	{
		// for all shadow lights in universe
		sShadowLight* pShadowLight = &m_pShadowLightList [ iLightIndex ];
		if ( pShadowLight )
		{
			// all shadows belonging to light
			for ( int iShadow=0; iShadow<(int)pShadowLight->pShadowMeshList.size(); iShadow++ )
			{
				// draw all universe shadow meshes
				if ( bNoDebugShadows ) 
					m_ObjectManager.DrawStencilMesh ( 0, NULL, pShadowLight->pShadowMeshList [ iShadow ] );
				else
				{
					pShadowLight->pShadowMeshList [ iShadow ]->bZRead = false;
					pShadowLight->pShadowMeshList [ iShadow ]->bLight = false;
					pShadowLight->pShadowMeshList [ iShadow ]->bWireframe = true;
					m_ObjectManager.DrawMesh ( pShadowLight->pShadowMeshList [ iShadow ] );
				}
			}
		}
	}

	// finish from stencil writing
	if ( bNoDebugShadows ) m_ObjectManager.FinishStencilMeshWrite();
}

//
// fast collision (for lightmapping)
//

void cUniverse::ShortlistMeshesWithinBox ( D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax )
{
	// calculate correct boundbox
	D3DXVECTOR3 vecMin = *pvecMin;
	D3DXVECTOR3 vecMax = *pvecMax;
	if ( pvecMax->x<vecMin.x) { vecMin.x=pvecMax->x; vecMax.x=pvecMin->x; }
	if ( pvecMax->y<vecMin.y) { vecMin.y=pvecMax->y; vecMax.y=pvecMin->y; }
	if ( pvecMax->z<vecMin.z) { vecMin.z=pvecMax->z; vecMax.z=pvecMin->z; }

	// scan nodes, and collect all meshes touching input box
	m_pMeshShortList.clear();

	// go through whole node universe
	for ( int iNode = 0; iNode < m_iNodeListSize; iNode++ )
	{
		// get node ptr
		sNode* pNode = &m_pNode [ iNode ];

		// reject quickly if nodebox completely outside input box
		D3DXVECTOR3 vecNodeMin = pNode->vecCentre - pNode->vecDimension;
		D3DXVECTOR3 vecNodeMax = pNode->vecCentre + pNode->vecDimension;
		if ( CollisionBoundBoxTest ( &vecMin, &vecMax, &vecNodeMin, &vecNodeMax ) )
		{
			// go through each mesh in the node
			for ( int iMesh = 0; iMesh < pNode->iMeshCount; iMesh++ )
			{
				// get pointer to mesh
				sMesh* pMesh = pNode->ppMeshList [ iMesh ];

				// if meshbox touches input box
				if ( CollisionBoundBoxTest ( &vecMin, &vecMax, &pMesh->Collision.vecMin, &pMesh->Collision.vecMax ) )
				{
					// add mesh to shortlist of meshes
					m_pMeshShortList.push_back ( pMesh );
				}
			}
		}
	}
}

// mike - 080305 - so we can get the master mesh list
void cUniverse::GetMasterMeshList ( vector < sMesh* > *pMeshList )
{
	// all meshgroups in universe
	for ( int iAreaBox = 0; iAreaBox < (int)m_pAreaList.size ( ); iAreaBox++ )
	{
	    int iIndex = 0;
	    
		sArea* pArea = m_pAreaList [ iAreaBox ];
		for ( iIndex = 0; iIndex < (int)pArea->meshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->meshgroups [ iIndex ]->pMesh;
			pMeshList->push_back ( pMesh );
		}
		for ( iIndex = 0; iIndex < (int)pArea->sharedmeshgroups.size ( ); iIndex++ )
		{
			sMesh* pMesh = pArea->sharedmeshgroups [ iIndex ]->pMesh;
			pMeshList->push_back ( pMesh );
		}
	}
}