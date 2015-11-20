
#include "Common.h"
#include "CCompiler.h"
#include "CompilerTypes.h"
#include "ProcessHSR.h"
#include "CBSPTree.h"
#include "..\\CommonC.h"


cCompiler::cCompiler ( )
{
	// set up default options
	m_OptionsHSR.bEnabled           = true;
	m_OptionsESR.bEnabled           = false;
    m_OptionsESR.iDeterminePos      = ESR_POS_AUTOMATIC;
    m_OptionsESR.fCustomX           = 0.0f;
    m_OptionsESR.fCustomY           = 0.0f;
    m_OptionsESR.fCustomZ           = 0.0f;
    m_OptionsESR.bRemoveWet         = true;
    m_OptionsESR.iSpawnPoint        = -1;
	m_OptionsBSP.bEnabled           = true;
    m_OptionsBSP.iTreeType          = BSP_TYPE_NONSPLIT;
    m_OptionsBSP.fSplitHeuristic    = 3.0f;
    m_OptionsBSP.iSplitterSample    = 60;
    m_OptionsBSP.bRemoveBackLeaves  = true;
    m_OptionsBSP.bAddBoundingPolys  = false;
    m_OptionsBSP.bLinkDetailBrushes = true;
	m_OptionsPRT.bEnabled           = true;
    m_OptionsPRT.bFullCompile       = true;
	m_OptionsPVS.bEnabled           = true;
    m_OptionsPVS.bFullCompile       = false;
    m_OptionsPVS.ucClipTestCount    = 2;
	m_OptionsTJR.bEnabled           = true;

	// clear out member variables
    m_pBSPTree     = NULL;
    m_Status       = CS_IDLE;
	m_iVertexCount = 0;
	m_pVertices    = NULL;

    memset ( &m_StatsGeneral, 0, sizeof ( sGeneralStats ) );
    memset ( &m_StatsHSR,     0, sizeof ( sHSRStats     ) );
    memset ( &m_StatsESR,     0, sizeof ( sESRStats     ) );
    memset ( &m_StatsBSP,     0, sizeof ( sBSPStats     ) );
    memset ( &m_StatsPRT,     0, sizeof ( sPRTStats     ) );
    memset ( &m_StatsPVS,     0, sizeof ( sPVSStats     ) );
    memset ( &m_StatsTJR,     0, sizeof ( sTJRStats     ) );
}

cCompiler::~cCompiler ( )
{
	// free up memory
	Release ( );
}

bool cCompiler::Clip ( cBSPTree* pTree, bool bFlag )
{
	// clip the BSP tree
	m_pBSPTree->ClipTree ( pTree, bFlag, bFlag, 0, NULL );

	return true;
}

bool cCompiler::CompileScene ( sMesh* pMeshA, D3DXMATRIX* matA, sMesh* pMeshB, D3DXMATRIX* matB )
{
	// leemove - 130306 - u60b3 - CSG needs all meshes as trilist
	if ( pMeshA )
	{
		if ( pMeshA->iPrimitiveType!=D3DPT_TRIANGLELIST )
		{
			// leenote - severe limitation in current CSG code for handling spheres, so limit this now
			return false;
		}
	}

	{
		struct sVertex
		{
			// used to store vertex information

			float x, y, z;			// position
			float nx, ny, nz;		// normals
			float tu, tv;			// texture coordinates
		};

		cMesh* pNewMesh = new cMesh ( );
		int    iOffset  = 0;
		sMesh* pMesh    = pMeshA;

		int iVertexCount = 0;

		// mike - 13083 - convert if necessary
		if ( pMesh->dwFVF != ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )
		{
			ConvertLocalMeshToFVF     ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
//			ConvertLocalMeshToTriList ( pMesh ); // moved above
		}

		pNewMesh->m_dwFaceCount = pMesh->dwIndexCount / 3;
		pNewMesh->m_ppFaces     = new cFace* [ pMesh->dwIndexCount / 3 ];

		for ( int iFace = 0; iFace < (int)pNewMesh->m_dwFaceCount; iFace++ )
		{
			pNewMesh->m_ppFaces [ iFace ] = new cFace;
			pNewMesh->m_ppFaces [ iFace ]->AddVertices ( 3 );

			for ( int iVertex = 0; iVertex < 3; iVertex++ )
			{
				sVertex pVertex = *( ( sVertex* ) pMesh->pVertexData + ( pMesh->pIndices [ iOffset++ ] ) );

				// mike - 13083 - only transform if matrix is valid
				//if ( matA )
				//	D3DXVec3TransformCoord ( ( D3DXVECTOR3* ) &pVertex, ( D3DXVECTOR3* ) &pVertex, matA );

				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].x        = pVertex.x;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].y        = pVertex.y;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].z        = pVertex.z;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tu       = pVertex.tu;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tv       = pVertex.tv;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.x = pVertex.nx;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.y = pVertex.ny;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.z = pVertex.nz;

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

		m_vpMeshList.push_back ( pNewMesh );
	}

	if ( pMeshB )
	{
		struct sVertex
		{
			// used to store vertex information

			float x, y, z;			// position
			float nx, ny, nz;		// normals
			float tu, tv;			// texture coordinates
		};

		cMesh* pNewMesh = new cMesh ( );
		int    iOffset  = 0;
		sMesh* pMesh    = pMeshB;

		// mike - 13083 - convert if necessary
		if ( pMesh->dwFVF != ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ) )
		{
			ConvertLocalMeshToFVF     ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
			ConvertLocalMeshToTriList ( pMesh );
		}

		int iVertexCount = 0;

		pNewMesh->m_dwFaceCount = pMesh->dwIndexCount / 3;
		pNewMesh->m_ppFaces     = new cFace* [ pMesh->dwIndexCount / 3 ];

		for ( int iFace = 0; iFace < (int)pNewMesh->m_dwFaceCount; iFace++ )
		{
			pNewMesh->m_ppFaces [ iFace ] = new cFace;
			pNewMesh->m_ppFaces [ iFace ]->AddVertices ( 3 );

			for ( int iVertex = 0; iVertex < 3; iVertex++ )
			{
				sVertex pVertex = *( ( sVertex* ) pMesh->pVertexData + ( pMesh->pIndices [ iOffset++ ] ) );

				// mike - 13083 - only transform if matrix is valid
				//if ( matB )
				//	D3DXVec3TransformCoord ( ( D3DXVECTOR3* ) &pVertex, ( D3DXVECTOR3* ) &pVertex, matB );

				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].x        = pVertex.x;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].y        = pVertex.y;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].z        = pVertex.z;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tu       = pVertex.tu;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].tv       = pVertex.tv;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.x = pVertex.nx;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.y = pVertex.ny;
				pNewMesh->m_ppFaces [ iFace ]->m_pVertices [ iVertex ].Normal.z = pVertex.nz;

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

		m_vpMeshList.push_back ( pNewMesh );
	}

    try
    {
        memset ( &m_StatsGeneral, 0, sizeof ( sGeneralStats ) );

        m_Status = CS_INPROGRESS;

		for ( int iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
        {
            if ( !m_vpMeshList [ iMesh ] )
				continue;

			m_StatsGeneral.iBrushCount++;
			m_StatsGeneral.iPolygonCount += m_vpMeshList [ iMesh ]->m_dwFaceCount;
		}
    }
	catch ( ... )
    {
        Release ( );
        return false;
	}

	// hidden surface removal
    if ( m_OptionsHSR.bEnabled )
	{
		m_CurrentLog = LOG_HSR;
		PerformHSR ( );
	}
    
	// exterior surface removal
    if ( m_OptionsESR.bEnabled )
	{
		m_CurrentLog = LOG_ESR;
		PerformESR ( );
	}

	// binary space partition compile
    if ( m_OptionsBSP.bEnabled )
	{
		m_CurrentLog = LOG_BSP;
		PerformBSP ( );
	}
    
	// portals
    if ( m_OptionsPRT.bEnabled )
	{
		m_CurrentLog = LOG_PRT;
		PerformPRT ( );
	}

	// potential visibility set
    if ( m_OptionsPVS.bEnabled )
	{
		m_CurrentLog = LOG_PVS;
		PerformPVS ( );
	}

	// t junction removal
    if ( m_OptionsTJR.bEnabled )
	{
		m_CurrentLog = LOG_TJR;
		PerformTJR ( );
	}
    
    m_Status = CS_IDLE;

    return true;
}

cCompiler::sVertex	vertices      [ 500000 ];
cCompiler::sVertex	finalVertices [ 500000 ];

void cCompiler::BuildPolygonList ( bool bMode )
{
	// bMode specifies polygon extraction -
	//
	//		true  = get from leaves
	//		false = use face list

	if ( m_pBSPTree == NULL )
		return;

	if ( bMode )
	{
		int     iVertexPos = 0;

		memset ( vertices,      0, sizeof ( vertices      ) );
		memset ( finalVertices, 0, sizeof ( finalVertices ) );

		int iCount = 0;

		{
			for ( int i = 0; i < m_pBSPTree->GetLeafCount ( ); i++ )
			{
				cBSPLeaf* pLeaf = m_pBSPTree->GetLeaf ( i );

				for ( ULONG f = 0; f < pLeaf->m_dwFaceCount; f++ )
				{
					cBSPFace* pFace = m_pBSPTree->GetFace ( pLeaf->m_iFaceIndices [ f ] );

					if ( !pFace )
						continue;

					int iA = 0;
					int iB = 1;
					int iC = 2;
					
					int iVertex = 0;

					for ( iVertex = 0; iVertex < (int)pFace->m_dwVertexCount; iVertex++ )
					{
						vertices [ iVertex ].x  = pFace->m_pVertices [ iVertex ].x;
						vertices [ iVertex ].y  = pFace->m_pVertices [ iVertex ].y;
						vertices [ iVertex ].z  = pFace->m_pVertices [ iVertex ].z;
						vertices [ iVertex ].nx = pFace->m_pVertices [ iVertex ].Normal.x;
						vertices [ iVertex ].ny = pFace->m_pVertices [ iVertex ].Normal.y;
						vertices [ iVertex ].nz = pFace->m_pVertices [ iVertex ].Normal.z;
						vertices [ iVertex ].tu = pFace->m_pVertices [ iVertex ].tu;
						vertices [ iVertex ].tv = pFace->m_pVertices [ iVertex ].tv;
					}

					for ( iVertex = 0; iVertex < (int)pFace->m_dwVertexCount - 2; iVertex++ )
					{
						int a = 0;

						finalVertices [ iVertexPos++ ] = vertices [ iA   ];
						finalVertices [ iVertexPos++ ] = vertices [ iB++ ];
						finalVertices [ iVertexPos++ ] = vertices [ iC++ ];
					}

					iCount += pFace->m_dwVertexCount;
				}
			}

			SAFE_DELETE_ARRAY ( m_pVertices );
			m_iVertexCount = iVertexPos;
			m_pVertices    = new sVertex [ m_iVertexCount ];

			memcpy ( m_pVertices, finalVertices, sizeof ( sVertex ) * m_iVertexCount );
		}
	}
	else
	{
		int     iVertexPos = 0;

		memset ( vertices,      0, sizeof ( vertices      ) );
		memset ( finalVertices, 0, sizeof ( finalVertices ) );

		int iCount = 0;

		{
			for ( int i = 0; i < m_pBSPTree->GetFaceCount ( ); i++ )
			{
				cBSPFace* pFace = m_pBSPTree->GetFace ( i );

				{
					if ( !pFace )
						continue;

					if ( pFace->m_bDeleted )
						continue;

					int iA = 0;
					int iB = 1;
					int iC = 2;
					
					int iVertex = 0;

					for ( iVertex = 0; iVertex < (int)pFace->m_dwVertexCount; iVertex++ )
					{
						vertices [ iVertex ].x  = pFace->m_pVertices [ iVertex ].x;
						vertices [ iVertex ].y  = pFace->m_pVertices [ iVertex ].y;
						vertices [ iVertex ].z  = pFace->m_pVertices [ iVertex ].z;
						vertices [ iVertex ].nx = pFace->m_pVertices [ iVertex ].Normal.x;
						vertices [ iVertex ].ny = pFace->m_pVertices [ iVertex ].Normal.y;
						vertices [ iVertex ].nz = pFace->m_pVertices [ iVertex ].Normal.z;
						vertices [ iVertex ].tu = pFace->m_pVertices [ iVertex ].tu;
						vertices [ iVertex ].tv = pFace->m_pVertices [ iVertex ].tv;
					}

					for ( iVertex = 0; iVertex < (int)pFace->m_dwVertexCount - 2; iVertex++ )
					{
						finalVertices [ iVertexPos++ ] = vertices [ iA   ];
						finalVertices [ iVertexPos++ ] = vertices [ iB++ ];
						finalVertices [ iVertexPos++ ] = vertices [ iC++ ];
					}
					
					iCount += iVertexPos;
				}
			}

			m_iVertexCount = iVertexPos;
			m_pVertices    = new sVertex [ m_iVertexCount ];

			memcpy ( m_pVertices, finalVertices, sizeof ( sVertex ) * m_iVertexCount );
		}
	}
}

bool cCompiler::PerformHSR ( void )
{
	// performs a union operation between 2 objects

    // one time compile process
    cProcessHSR ProcessHSR;
    int	        iMeshCount = 0;
	int			iMesh      = 0;

    // set our processor options
    memset ( &m_StatsHSR, 0, sizeof ( sHSRStats ) );

    ProcessHSR.SetOptions ( m_OptionsHSR );
    ProcessHSR.SetParent  ( this );
    ProcessHSR.SetStats   ( &m_StatsHSR );

    for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
    {
        cMesh* pMesh = m_vpMeshList [ iMesh ];
        
		if ( !pMesh )
			continue;

        iMeshCount++;
	}

    if ( iMeshCount <= 1 )
    {
        return true;
	}
    else
    {
		// add all meshes into the HSR class
		for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
        {
            cMesh* pMesh = m_vpMeshList [ iMesh ];
            
			if ( !pMesh )
				continue;
            
            ProcessHSR.AddMesh( pMesh );
		}

        ProcessHSR.Process ( );

        if ( ProcessHSR.GetResultMesh ( ) != NULL )
        {
            for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
            {
                cMesh* pMesh = m_vpMeshList [ iMesh ];

                if ( !pMesh )
					continue;
                
				delete pMesh;
                m_vpMeshList [ iMesh ] = NULL;
			}
    
            int iMeshIndex = -1;

            for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
            {
                if ( ! m_vpMeshList [ iMesh ] )
				{
					iMeshIndex = iMesh;
					break;
				}
            }

            if ( iMeshIndex == -1 )
            {
                m_vpMeshList.push_back ( ProcessHSR.GetResultMesh ( ) );
            }
            else
            {
                m_vpMeshList [ iMeshIndex ] = ProcessHSR.GetResultMesh ( );
            }
		}
	}

    return true;
}

bool cCompiler::PerformESR ( void )
{
    return true;
}

bool cCompiler::PerformBSP ( void )
{
    if ( m_pBSPTree )
		delete m_pBSPTree;

	int iMesh = 0;

    // allocate a new BSP tree
    m_pBSPTree = new cBSPTree;

    // set our compiler options
    memset ( &m_StatsBSP, 0, sizeof ( sBSPStats ) );

    m_pBSPTree->SetOptions ( m_OptionsBSP );
    m_pBSPTree->SetParent  ( this );
    m_pBSPTree->SetStats   ( &m_StatsBSP );

    for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
    {
        cMesh* pMesh = m_vpMeshList [ iMesh ];

        if ( !pMesh )
			continue;
		
        m_pBSPTree->AddFaces ( pMesh->m_ppFaces, pMesh->m_dwFaceCount, true );
	}
	

    if ( m_OptionsBSP.bAddBoundingPolys )
		m_pBSPTree->AddBoundingPolys ( true );

    m_pBSPTree->CompileTree ( );

    for ( iMesh = 0; iMesh < (int)m_vpMeshList.size ( ); iMesh++ )
    {
        cMesh* pMesh = m_vpMeshList [ iMesh ];

        if ( !pMesh )
			continue;

        delete pMesh;
        m_vpMeshList [ iMesh ] = NULL;
	}

    return true;
}

bool cCompiler::PerformPRT ( void )
{
	return true;
}

bool cCompiler::PerformPVS ( void )
{
	return true;
}

bool cCompiler::PerformTJR ( void )
{
	return true;
}

void cCompiler::Release ( void )
{
    int iTemp;

    for ( iTemp = 0; iTemp < (int)m_vpMeshList.size ( ); iTemp++ )
    {
        if ( m_vpMeshList [ iTemp ] )
			delete m_vpMeshList [ iTemp ];
	}

    for ( iTemp = 0; iTemp < (int)m_vpTextureList.size ( ); iTemp++ )
    {
        if ( m_vpTextureList [ iTemp ] )
			delete m_vpTextureList [ iTemp ];
	}

    for ( iTemp = 0; iTemp < (int)m_vpMaterialList.size ( ); iTemp++ )
    {
        if ( m_vpMaterialList [ iTemp ] )
			delete m_vpMaterialList [ iTemp ];
	}

    m_vpMeshList.clear     ( );
    m_vpTextureList.clear  ( );
    m_vpMaterialList.clear ( );

    if ( m_pBSPTree )
		delete m_pBSPTree;

    m_pBSPTree = NULL;

	if ( m_pVertices )
		delete [ ] m_pVertices;
}

void cCompiler::SetOptions ( int iProcess, void* pvOptions )
{
    switch ( iProcess )
    {
        case PROCESS_HSR:
            m_OptionsHSR = *( ( sHSR* ) pvOptions );
		break;

        case PROCESS_ESR:
            m_OptionsESR = *( ( sESR* ) pvOptions );
		break;

        case PROCESS_BSP:
            m_OptionsBSP = * ( ( sBSP* ) pvOptions );
		break;

        case PROCESS_PRT:
            m_OptionsPRT = *( ( sPRT* ) pvOptions );
		break;

        case PROCESS_PVS:
            m_OptionsPVS = *( ( sPVS* ) pvOptions );
		break;

        case PROCESS_TJR:
            m_OptionsTJR = *( ( sTJR* ) pvOptions );
		break;
	}
}

void cCompiler::GetOptions ( int iProcess, void* pvOptions )
{
    switch ( iProcess )
    {
        case PROCESS_HSR:
            *( ( sHSR* ) pvOptions ) = m_OptionsHSR;
		break;

        case PROCESS_ESR:
            *( ( sESR* ) pvOptions ) = m_OptionsESR;
		break;

        case PROCESS_BSP:
            *( ( sBSP* ) pvOptions ) = m_OptionsBSP;
		break;

        case PROCESS_PRT:
            *( ( sPRT* ) pvOptions ) = m_OptionsPRT;
		break;

        case PROCESS_PVS:
            *( ( sPVS* ) pvOptions ) = m_OptionsPVS;
		break;

        case PROCESS_TJR:
            *( ( sTJR* ) pvOptions ) = m_OptionsTJR;
		break;
	}
}

void cCompiler::GetStatistics ( int iProcess, void* pvStats )
{
    switch ( iProcess )
    {
        case -1:
            *( ( sGeneralStats* ) pvStats ) = m_StatsGeneral;
		break;

        case PROCESS_HSR:
            *( ( sHSRStats* ) pvStats ) = m_StatsHSR;
        break;

        case PROCESS_ESR:
            *( ( sESRStats* ) pvStats ) = m_StatsESR;
		break;

        case PROCESS_BSP:
            *( ( sBSPStats* ) pvStats ) = m_StatsBSP;
		break;

        case PROCESS_PRT:
            *( ( sPRTStats* ) pvStats ) = m_StatsPRT;
		break;

        case PROCESS_PVS:
            *( ( sPVSStats* ) pvStats ) = m_StatsPVS;
		break;

        case PROCESS_TJR:
            *( ( sTJRStats* ) pvStats ) = m_StatsTJR;
		break;
	}
}
