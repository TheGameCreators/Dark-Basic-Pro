//
// HSR
//

// Includes
#include "ProcessHSR.h"
#include "CCompiler.h"
#include "CBSPTree.h"

cProcessHSR::cProcessHSR ( )
{
    m_pResultMesh = NULL;
    m_pParent     = NULL;
    m_pStats      = NULL;
}

cProcessHSR::~cProcessHSR ( )
{
    Release ( );
}

void cProcessHSR::SetOptions ( sHSR& Options )
{
	m_OptionSet = Options;
}

void cProcessHSR::SetParent ( cCompiler* pParent )
{
	m_pParent = pParent;
}

void cProcessHSR::SetStats ( sHSRStats* pStats )
{
	m_pStats  = pStats;
}

void cProcessHSR::Release ( )
{
    m_vpMeshList.clear ( );

    m_pResultMesh = NULL;
}

bool cProcessHSR::AddMesh ( cMesh* pMesh )
{
    try 
    {
        if ( m_vpMeshList.size ( ) >= ( m_vpMeshList.capacity ( ) - 1 ) ) 
        {
            m_vpMeshList.reserve ( m_vpMeshList.size ( ) + HSR_ARRAY_THRESHOLD );
		}

        m_vpMeshList.push_back ( pMesh );
    }
	catch ( ... )
    { 
        m_vpMeshList.clear ( );
        return false;
	}

    return true;
}

HRESULT cProcessHSR::Process ( void )
{
    HRESULT     hRet;
    sBSP		Options;
    int         iMesh,
				a,
				b;
	cBounds3    BoundsA,
				BoundsB;
    cBSPTree**  pBSPTrees  = NULL;
    int			iMeshCount = m_vpMeshList.size ( );
    
	memset ( &Options, 0, sizeof ( sBSP ) );

    Options.iTreeType          = BSP_TYPE_NONSPLIT;
	Options.bEnabled           = true;
    Options.bRemoveBackLeaves  = true;
    Options.fSplitHeuristic    = 3.0f;
    Options.iSplitterSample    = 60;
    
    if ( m_pStats )
    {
        m_pStats->iBrushCount = iMeshCount;

        for ( iMesh = 0; iMesh < iMeshCount; iMesh++ )
			m_pStats->iPrePolygonCount += m_vpMeshList [ iMesh ]->m_dwFaceCount;  
    } 
    
	try
    {
        pBSPTrees = new cBSPTree* [ iMeshCount ];

        if ( ! pBSPTrees )
			throw S_FALSE;

        memset ( pBSPTrees, 0, iMeshCount * sizeof ( cBSPTree* ) );

        m_pResultMesh = new cMesh;

        if ( !m_pResultMesh )
			throw S_FALSE;

        for ( iMesh = 0; iMesh < iMeshCount; iMesh++ )
        {
            pBSPTrees [ iMesh ] = new cBSPTree;

            if ( ! pBSPTrees [ iMesh ] )
				throw S_FALSE;

            pBSPTrees [ iMesh ]->SetOptions ( Options );

            hRet = pBSPTrees [ iMesh ]->AddFaces ( m_vpMeshList [ iMesh ]->m_ppFaces, m_vpMeshList [ iMesh ]->m_dwFaceCount, true );

            if ( FAILED ( hRet ) )
				throw hRet;

            hRet = pBSPTrees [ iMesh ]->CompileTree ( );
            
			if ( FAILED ( hRet ) )
				throw hRet;
		}

        for ( a = 0; a < iMeshCount; a++ )
        {
			if ( !pBSPTrees [ a ] || pBSPTrees [ a ]->GetFaceCount ( ) == 0 )
				continue;	 

            for ( b = 0; b < iMeshCount; b++ )
            {
                if ( ! pBSPTrees [ b ] || pBSPTrees [ b ]->GetFaceCount ( ) == 0 )
					continue;	 
		     
                if ( a == b )
					continue;
            
                BoundsA = pBSPTrees [ a ]->GetBounds ( );
				BoundsB = pBSPTrees [ b ]->GetBounds ( );

				#ifndef __GNUC__
                if ( ! BoundsA.IntersectedByBounds ( BoundsB, cVector3 ( 0.1f, 0.1f, 0.1f ) ) )
					continue;
				#endif

                if ( ! pBSPTrees [ b ]->IntersectedByTree ( pBSPTrees [ a ] ) )
					continue;
			    
                pBSPTrees [ a ]->ClipTree ( pBSPTrees [ b ], true, false );
                pBSPTrees [ b ]->ClipTree ( pBSPTrees [ a ], true, true  );
					
                pBSPTrees [ a ]->RepairSplits ( );
				pBSPTrees [ b ]->RepairSplits ( );
			}

            m_pResultMesh->BuildFromBSPTree ( pBSPTrees [ a ], false );
	        
            delete pBSPTrees [ a ];

            pBSPTrees [ a ] = NULL;
		}

        if ( m_pStats )
        {
            m_pStats->iPostPolygonCount = m_pResultMesh->m_dwFaceCount; 
            m_pStats->iPolygonsRemoved  = m_pStats->iPrePolygonCount - m_pStats->iPostPolygonCount;
        } 
    }
	catch ( HRESULT &e )
    {
        if ( pBSPTrees )
        {
            for ( iMesh = 0; iMesh < iMeshCount; iMesh++ )
			{
				if ( pBSPTrees [ iMesh ] )
					delete pBSPTrees [ iMesh ];
			}

            delete [ ] pBSPTrees;
		}

        if ( m_pResultMesh )
		{
			delete m_pResultMesh;
			m_pResultMesh = NULL;
		}
     
        return e;
    }

    if ( pBSPTrees )
    {
        for ( iMesh = 0; iMesh < iMeshCount; iMesh++ )
		{
			if ( pBSPTrees [ iMesh ] )
				delete pBSPTrees [ iMesh ];
		}

        delete [ ] pBSPTrees;
    
    }

    return S_OK;
}

cMesh* cProcessHSR::GetResultMesh ( void )
{
    return m_pResultMesh;
}
