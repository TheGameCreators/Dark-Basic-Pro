#include "cBSPTree.h"
#include "cCompiler.h"
#include "cPlane.h"
#include "cVector.h"
#include <algorithm>

cBSPFace::cBSPFace ( )
{
	// set default values for the face

    m_bUsedAsSplitter   = false;		// not used as splitter
    m_pNext             = NULL;			// set next pointer to null
    m_bDeleted          = false;		// deleted to false
	m_iOriginalIndex    = -1;			// no index
    m_iChildSplit [ 0 ] = -1;			// no child split 0
    m_iChildSplit [ 1 ] = -1;			// no child split 1
    m_iPlane            = -1;			// plane index
    m_iFrameRendered    =  0;			// frame render - not used yet
}

cBSPFace::cBSPFace ( cFace* pFace )
{
	// copy one face to another

	// set default properties
	m_bUsedAsSplitter   = false;		// not used as splitter
    m_pNext             = NULL;			// set next pointer to null
    m_bDeleted          = false;		// deleted to false
	m_iOriginalIndex    = -1;			// no index
    m_iChildSplit [ 0 ] = -1;			// no child split 0
    m_iChildSplit [ 1 ] = -1;			// no child split 1
    m_iPlane            = -1;			// plane index
    m_iFrameRendered    =  0;			// frame render - not used yet

	// copy values
    m_vecNormal        = pFace->m_vecNormal;		// normal
    m_iTextureIndex    = pFace->m_iTextureIndex;	// texture index
    m_iMaterialIndex   = pFace->m_iMaterialIndex;	// material index
    m_iFlags           = pFace->m_iFlags;			// flags
    m_iSrcBlendMode    = pFace->m_iSrcBlendMode;	// source blend mode
    m_iDestBlendMode   = pFace->m_iDestBlendMode;	// destination blend mode

	// add vertices
    AddVertices ( pFace->m_dwVertexCount );

	// copy vertices
    memcpy ( m_pVertices, pFace->m_pVertices, m_dwVertexCount * sizeof ( cVertex ) );
}

HRESULT cBSPFace::Split ( cPlane3& splitPlane, cBSPFace* pFrontSplit, cBSPFace* pBackSplit )
{
	// this function splits the current BSPFace, against the plane, the two
	// split fragments are returned via the pFrontSplit and pBackSplit
	// pointers, these must be valid pointers to already allocated faces,
	// however you can pass NULL to either parameter, in this case no
	// vertices will be calculated for that fragment

    // call base class implementation of split

    HRESULT ErrCode = cFace::Split ( splitPlane, pFrontSplit, pBackSplit );

	// check the split worked
    if ( FAILED ( ErrCode ) )
		return ErrCode;

	// copy values if the front split exists
    if ( pFrontSplit )
    {
        pFrontSplit->m_bUsedAsSplitter = m_bUsedAsSplitter;	// splitter
        pFrontSplit->m_iOriginalIndex  = m_iOriginalIndex;	// original index
        pFrontSplit->m_iPlane          = m_iPlane;			// plane
	}

	// copy values if the back split exists
    if ( pBackSplit )
    {
        pBackSplit->m_bUsedAsSplitter = m_bUsedAsSplitter;	// splitter
        pBackSplit->m_iOriginalIndex  = m_iOriginalIndex;	// original index
        pBackSplit->m_iPlane          = m_iPlane;			// plane
	}

	// all went okay
    return S_OK;
}

cBSPLeaf::cBSPLeaf ( )
{
	// leaf constructor
	m_dwFaceCount   = 0;	// face count
    m_dwPortalCount = 0;	// portal count
    m_dwObjectCount = 0;	// object count
    m_dwPVSIndex    = 0;	// pvs index
}

cBSPLeaf::~cBSPLeaf ( )
{
	// destructor
}

bool cBSPLeaf::IsSolid ( void )
{
	// is the leaf solid
	return ( m_dwFaceCount > 0 ) ? true : false;
}

bool cBSPLeaf::BuildFaceIndices ( cBSPFace* pFaceList )
{
	// takes a linked list of faces, and builds the indices for this leaf
	// this function makes sure that the list only contains one of each
	// original polygon, this should never really happen under normal
	// circumstances, but is a good thing to do none the less

	// local variables
    cBSPFace*	  pIterator      = pFaceList;	// pointer to face list
	int			  iOriginalIndex = 0;			// original index

	// reset the bounds
    m_Bounds.Reset ( );

	// go through all faces
	while ( pIterator != NULL )
	{
		// get the original polygons index
        iOriginalIndex = pIterator->m_iOriginalIndex;
	
        try 
        {
            #ifndef __GNUC__
            // make sure this index does not already exist in the array
		    if ( std::find ( m_iFaceIndices.begin ( ), m_iFaceIndices.end ( ), iOriginalIndex ) == m_iFaceIndices.end ( ) )
			{
                // resize the vector if we need to
                if ( m_iFaceIndices.size ( ) >= ( m_iFaceIndices.capacity ( ) - 1 ) )
                {
                    m_iFaceIndices.reserve ( m_iFaceIndices.size ( ) + BSP_ARRAY_THRESHOLD );
                }

                // finally add this face index to the list
                m_iFaceIndices.push_back ( iOriginalIndex );
			}
			#endif
        }
		catch (...)
		{ 
            // if something goes wrong break out
            m_dwFaceCount = 0; 
            m_iFaceIndices.clear ( );
            
			// return false
			return false;
        }

        // build into current bounding box
        m_Bounds.CalculateFromPolygon ( pIterator->m_pVertices, pIterator->m_dwVertexCount, sizeof ( cVertex ), false );

		// go to the next face
        pIterator = pIterator->m_pNext;
	}

    // copy newly built final list into this leafs index array
    m_dwFaceCount = m_iFaceIndices.size ( );

    // optimize the vector
    if ( m_dwFaceCount < m_iFaceIndices.capacity ( ) )
		m_iFaceIndices.resize ( m_dwFaceCount );
	
	// all went well
    return true;
}

bool cBSPLeaf::AddPortal ( int iPortalIndex )
{
	// add a portal

    try 
    {
        // resize the vector if we need to
        if ( m_iPortalIndices.size ( ) >= ( m_iPortalIndices.capacity ( ) - 1 ) ) 
        {
            m_iPortalIndices.reserve ( m_iPortalIndices.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        // finally add this face index to the list
        m_iPortalIndices.push_back ( iPortalIndex );
    }
	catch (...)
    { 
        // clean up and bail
        m_dwPortalCount = 0; 
        m_iPortalIndices.clear ( );

        return false; 
    }

	// increase portal count
    m_dwPortalCount++;

    return true;
}

bool cBSPLeaf::AddObject ( int iObjectIndex )
{
	// add object

    try
    {
        // resize the vector if we need to
        if ( m_iObjectIndices.size ( ) >= ( m_iObjectIndices.capacity ( ) - 1 ) )
        {
            m_iObjectIndices.reserve ( m_iObjectIndices.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        m_iObjectIndices.push_back ( iObjectIndex );
    }
	catch (...) 
    { 
        m_dwObjectCount = 0; 
        m_iObjectIndices.clear ( );

        return false; 
    }

	// increase object count
    m_dwObjectCount++;

    return true;
}

cBSPNode::cBSPNode ( )
{
	// bsp node constructor

	m_iPlane = -1;
    m_iFront = -1;
    m_iBack  = -1;
}

void cBSPNode::CalculateBounds ( cBSPFace* pFaceList, bool bResetBounds )
{
	// work out bounds
    bool      bReset    = bResetBounds;
    cBSPFace* pIterator = pFaceList;
    
	// go through all faces
    while ( pIterator ) 
    {
		// get the bounds
        m_Bounds.CalculateFromPolygon ( pIterator->m_pVertices, pIterator->m_dwVertexCount, sizeof ( cVertex ), bReset );
        
		bReset    = false;
        pIterator = pIterator->m_pNext;
    }
}

cBSPPortal::cBSPPortal ( )
{
	// portal constructor

	m_iLeafOwner [ 0 ] = NO_OWNER;
    m_iLeafOwner [ 1 ] = NO_OWNER;
    m_pNextPortal      = NULL;
    m_pPrevPortal      = NULL;
    m_ucLeafCount      =  0;
    m_iOwnerNode       = -1;
}


HRESULT cBSPPortal::Split ( cPlane3& Plane, cBSPPortal* pFrontSplit, cBSPPortal* pBackSplit )
{
	// this function splits the current portal, against the plane, the two
	// split fragments are returned via the pFrontSplit and pBackSplit
	// pointers, these must be valid pointers to already allocated portals,
	// however you can pass NULL to either parameter, in this case no
	// vertices will be calculated for that fragment

    // call base class implementation of split
    HRESULT ErrCode = cPolygon::Split ( Plane, pFrontSplit, pBackSplit );

	// did it work
    if ( FAILED ( ErrCode ) )
		return ErrCode;

	// update front split
    if ( pFrontSplit ) 
    {
        pFrontSplit->m_ucLeafCount      = m_ucLeafCount;
        pFrontSplit->m_iOwnerNode       = m_iOwnerNode;
        pFrontSplit->m_iLeafOwner [ 0 ] = m_iLeafOwner [ 0 ];
        pFrontSplit->m_iLeafOwner [ 1 ] = m_iLeafOwner [ 1 ];
	}

	// update back split
    if ( pBackSplit )
    {
        pBackSplit->m_ucLeafCount      = m_ucLeafCount;
        pBackSplit->m_iOwnerNode       = m_iOwnerNode;
        pBackSplit->m_iLeafOwner [ 0 ] = m_iLeafOwner [ 0 ];
        pBackSplit->m_iLeafOwner [ 1 ] = m_iLeafOwner [ 1 ];
    }

	// return ok
    return S_OK;
}

cBSPTree::cBSPTree ( )
{
	// bsp constructor
	m_pFaceList         = NULL;
    m_iActiveFaces      = 0;
    m_pPVSData          = NULL;
    m_iPVSDataSize      = 0;
    m_bPVSCompressed    = false;
    m_pParent           = NULL;
    m_pStats            = NULL;
}

cBSPTree::~cBSPTree ( )
{
	// clean up
//    ReleaseTree ( );
}

cBounds3& cBSPTree::GetBounds ( void )
{
	// get bounds
	return m_Bounds;
}

int cBSPTree::GetNodeCount ( void )
{
	// get node count
	return m_vpNodes.size ( );
}

int cBSPTree::GetLeafCount ( void )
{
	// get leaf count
	return m_vpLeaves.size ( );
}

int cBSPTree::GetPlaneCount ( void )
{
	// get plane count
	return m_vpPlanes.size ( );
}

int cBSPTree::GetFaceCount ( void )
{
	// get face count
	return m_vpFaces.size ( );
}

int cBSPTree::GetPortalCount ( void )
{
	// get portal count
	return m_vpPortals.size ( );
}

cBSPNode* cBSPTree::GetNode ( int iIndex )
{
	// get a node
	return ( iIndex < (int)m_vpNodes.size ( ) ) ? m_vpNodes [ iIndex ] : NULL;
}

cPlane3* cBSPTree::GetPlane ( int iIndex )
{
	// get a plane
	return ( iIndex < (int)m_vpPlanes.size ( ) ) ? m_vpPlanes [ iIndex ] : NULL;
}

cBSPLeaf* cBSPTree::GetLeaf ( int iIndex )
{
	// get a leaf
	return ( iIndex < (int)m_vpLeaves.size ( ) ) ? m_vpLeaves [ iIndex ] : NULL;
}

cBSPFace* cBSPTree::GetFace ( int iIndex )
{
	// return a face
	return ( iIndex < (int)m_vpFaces.size ( ) ) ? m_vpFaces [ iIndex ] : NULL;
}

cBSPPortal*	cBSPTree::GetPortal ( int iIndex )
{
	// get portal
	return ( iIndex < (int)m_vpPortals.size ( ) ) ? m_vpPortals [ iIndex ] : NULL;
}

void cBSPTree::SetNode ( int iIndex, cBSPNode* pNode )
{
	if ( iIndex < (int)m_vpNodes.size ( ) )
		m_vpNodes [ iIndex ] = pNode;
}

void cBSPTree::SetPlane ( int iIndex, cPlane3* pPlane )
{
	if ( iIndex < (int)m_vpPlanes.size ( ) )
		m_vpPlanes [ iIndex ] = pPlane;
}

void cBSPTree::SetLeaf ( int iIndex, cBSPLeaf* pLeaf )
{
	if ( iIndex < (int)m_vpLeaves.size ( ) )
		m_vpLeaves [ iIndex ] = pLeaf;
}

void cBSPTree::SetFace ( int iIndex, cBSPFace* pFace )
{
	if ( iIndex < (int)m_vpFaces.size ( ) )
		m_vpFaces [ iIndex ] = pFace;
}

void cBSPTree::SetPortal ( int iIndex, cBSPPortal* pPortal )
{
	if ( iIndex < (int)m_vpPortals.size ( ) )
		m_vpPortals [ iIndex ] = pPortal;
}

void cBSPTree::SetOptions ( sBSP& Options )
{
	m_OptionSet = Options;
}

void cBSPTree::SetParent ( cCompiler* pParent )
{ 
	m_pParent = pParent;
}

void cBSPTree::SetStats ( sBSPStats* pStats )
{ 
	m_pStats = pStats;
}

void cBSPTree::ReleaseTree ( void )
{
	// free up any previously allocated memory

	int iTemp;

    for ( iTemp = 0; iTemp < GetFaceCount ( ); iTemp++ )
	{
		if ( GetFace ( iTemp ) )
			delete GetFace ( iTemp );
	}

    for ( iTemp = 0; iTemp < GetNodeCount ( ); iTemp++ )
	{
		if ( GetNode ( iTemp ) )
			delete GetNode ( iTemp );
	}

    for ( iTemp = 0; iTemp < GetLeafCount ( ); iTemp++ )
	{
		if ( GetLeaf ( iTemp ) )
			delete GetLeaf ( iTemp );
	}

    for ( iTemp = 0; iTemp < GetPlaneCount ( ); iTemp++ )
	{
		if ( GetPlane ( iTemp ) )
			delete GetPlane ( iTemp );
	}

    for ( iTemp = 0; iTemp < GetPortalCount ( ); iTemp++ )
	{
		if ( GetPortal ( iTemp ) )
			delete GetPortal ( iTemp );
	}
    
    for ( iTemp = 0; iTemp < (int)m_vpGarbage.size ( ); iTemp++ )
	{
		if ( m_vpGarbage [ iTemp ] )
			delete m_vpGarbage [ iTemp ];
	}

    for ( iTemp = 0; iTemp < (int)m_vpRemovedFaces.size ( ); iTemp++ )
	{
		if ( m_vpRemovedFaces [ iTemp ] )
			delete m_vpRemovedFaces [ iTemp ];
	}

    if ( m_pPVSData )
		delete [ ] m_pPVSData;

    m_vpNodes.clear   ( );
    m_vpPlanes.clear  ( );
    m_vpLeaves.clear  ( );
    m_vpFaces.clear   ( );
    m_vpPortals.clear ( );
    m_vpGarbage.clear ( );
    
    m_pPVSData      = NULL;
    m_pFaceList     = NULL;
    m_iActiveFaces  = 0;
    m_iPVSDataSize  = 0;
}

HRESULT cBSPTree::CompileTree ( void )
{
    HRESULT ErrCode;

	if ( !m_pFaceList )
		return S_FALSE;

    if ( FAILED ( ErrCode = BuildPlaneArray ( ) ) )
		return ErrCode;

    if ( !IncreaseNodeCount ( ) )
		return S_FALSE;

    if ( m_pStats )
    {
        m_pStats->iPrePolygonCount = m_iActiveFaces;
        m_pStats->iPlaneCount      = GetPlaneCount ( );    
    } 
    
    if ( FAILED ( ErrCode = BuildBSPTree ( 0, m_pFaceList ) ) )
		return ErrCode;

    if ( m_pStats )
    {
        m_pStats->iPostPolygonCount = GetFaceCount ( );
        m_pStats->iNodeCount        = GetNodeCount ( );
        m_pStats->iLeafCount        = GetLeafCount ( );
    }

	m_pFaceList = NULL;

	m_Bounds.Reset ( );

    for ( int iFace = 0; iFace < GetFaceCount ( ); iFace++ )
    {
        m_Bounds.CalculateFromPolygon ( GetFace ( iFace )->m_pVertices, GetFace ( iFace )->m_dwVertexCount, sizeof ( cVertex ), false );
	}

    return S_OK;
}

HRESULT cBSPTree::AddFaces ( cFace** ppFaces, int iFaceCount, bool bBackupOriginals )
{
    cBSPFace* pNewFace = NULL;

    if ( !ppFaces || iFaceCount <= 0 )
		return S_FALSE;

    if ( !m_pFaceList )
		ReleaseTree ( );

    if ( m_OptionSet.iTreeType == BSP_TYPE_SPLIT )
		bBackupOriginals = false;

    for ( int iFace = 0; iFace < iFaceCount; iFace++ ) 
    {
        if ( ppFaces [ iFace ] )
		{
            if ( ! ( pNewFace = AllocBSPFace ( ppFaces [ iFace ] ) ) ) 
            {
                ReleaseTree ( ); 
                return S_FALSE; 
            }

            pNewFace->m_iOriginalIndex = m_iActiveFaces;
            m_iActiveFaces++;

            pNewFace->m_pNext = m_pFaceList;
            m_pFaceList       = pNewFace;
            
            // now we add the original face into the BSP Trees
            // RESULT face list ( if requested ) when compiling
            // a non-split resulting BSP Tree, In this case the
            // Indices stored in the leaves will point to the
            // relevant location in the m_vpFaces array
            if ( bBackupOriginals )
            {
                // allocate some storage space
                if ( !IncreaseFaceCount ( ) )
				{
					ReleaseTree ( );
					return S_FALSE;
				}
                
                // allocate a duplicate of the original original
                if ( ! ( pNewFace = AllocBSPFace ( ppFaces [ iFace ] ) ) )
				{
					ReleaseTree ( );
					return S_FALSE;
				}

                SetFace ( GetFaceCount ( ) - 1, pNewFace );
			}
		}
	}

    return S_OK;
}

HRESULT cBSPTree::AddBoundingPolys ( bool bBackupOriginals )
{
    #ifndef __GNUC__
	// this function adds 6, inward facing, polygons which bound the level
	// can only be called prior to compilation, it simply adds these 6 polys
	// onto the linked list to be considered for compilation

    cBSPFace* pIterator;
    cBounds3  Bounds;
    int	      i, j;

    // cannot backup originals in a split type tree
    if ( m_OptionSet.iTreeType == BSP_TYPE_SPLIT )
		bBackupOriginals = false;

    // build a bounding box from the data ready for compilation
    Bounds.Reset ( );

    for ( pIterator = m_pFaceList; pIterator; pIterator = pIterator->m_pNext )
    {
        Bounds.CalculateFromPolygon ( pIterator->m_pVertices, pIterator->m_dwVertexCount, sizeof ( cVertex ), false );
	}

    // increase the size of the bounds a little to ensure no intersection occurs
    Bounds.Max += cVector3 ( 10.0f, 10.0f, 10.0f );
    Bounds.Min -= cVector3 ( 10.0f, 10.0f, 10.0f );

    cVector3 Size   = Bounds.GetDimensions ( ) / 2.0f;
    cVector3 Centre = Bounds.GetCentre     ( );

    // generate 6 bounding polys
    for ( i = 0; i < 6; i++ )
    {
        cBSPFace* pFace = AllocBSPFace ( );

        if ( !pFace )
			return S_FALSE;

        // generate the face
        if ( pFace->AddVertices ( 4 ) < 0 )
		{
			delete pFace;
			return S_FALSE;
		}

        // store original index for later use
        pFace->m_iOriginalIndex = m_iActiveFaces;
        m_iActiveFaces++;

        // which face are we building?
        switch ( i )
        {
            case 0:
			{
                // top face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 ( -Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 (  Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 (  Size.x,  Size.y,  Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 ( -Size.x,  Size.y,  Size.z );

                pFace->m_vecNormal = cVector3 ( 0.0f, -1.0f, 0.0f );
			}
			break;

            case 1:
			{
                // bottom face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 ( -Size.x, -Size.y,  Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 (  Size.x, -Size.y,  Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 (  Size.x, -Size.y, -Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 ( -Size.x, -Size.y, -Size.z );
                
				pFace->m_vecNormal = cVector3 ( 0.0f, 1.0f, 0.0f );
			}
			break;

            case 2:
			{
                // left face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 ( -Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 ( -Size.x,  Size.y,  Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 ( -Size.x, -Size.y,  Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 ( -Size.x, -Size.y, -Size.z );
                
				pFace->m_vecNormal = cVector3 ( 1.0f, 0.0f, 0.0f );
			}
			break;

            case 3:
			{
                // right face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 (  Size.x,  Size.y,  Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 (  Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 (  Size.x, -Size.y, -Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 (  Size.x, -Size.y,  Size.z );
                
				pFace->m_vecNormal = cVector3 ( -1.0f, 0.0f, 0.0f );
			}
			break;

            case 4:
			{
                // front face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 (  Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 ( -Size.x,  Size.y, -Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 ( -Size.x, -Size.y, -Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 (  Size.x, -Size.y, -Size.z );

                pFace->m_vecNormal = cVector3 ( 0.0f, 0.0f, 1.0f );
			}
            break;

            case 5:
			{
                // back face
                pFace->m_pVertices [ 0 ] = Centre + cVector3 ( -Size.x,  Size.y,  Size.z );
                pFace->m_pVertices [ 1 ] = Centre + cVector3 (  Size.x,  Size.y,  Size.z );
                pFace->m_pVertices [ 2 ] = Centre + cVector3 (  Size.x, -Size.y,  Size.z );
                pFace->m_pVertices [ 3 ] = Centre + cVector3 ( -Size.x, -Size.y,  Size.z );

                pFace->m_vecNormal = cVector3 ( 0.0f, 0.0f, -1.0f );
			}
            break;
		}

        // copy over vertex normal
        for ( j = 0; j < 4; j++ )
			pFace->m_pVertices [ j ].Normal = pFace->m_vecNormal;
        
        // add to head of linked list
        pFace->m_pNext = m_pFaceList;
        m_pFaceList    = pFace;

        if ( bBackupOriginals )
        {
			cBSPFace* pNewFace = NULL;

            // allocate some storage space
            if ( !IncreaseFaceCount ( ) )
			{
				ReleaseTree ( );
				return S_FALSE;
			}
            
            // allocate a duplicate of the original original
            if ( ! ( pNewFace = AllocBSPFace ( pFace ) ) )
			{
				ReleaseTree ( );
				return S_FALSE;
			}

            // store this new pointer
            SetFace ( GetFaceCount ( ) - 1, pNewFace );
		}
	}
	#endif

    return S_OK;
}

HRESULT cBSPTree::BuildPlaneArray ( void )
{
	// takes a list of all the faces stored, and builds the plane array from
	// them, combining wherever possible

    cBSPFace*  pIterator = NULL;
    cPlane3    Plane;
	cPlane3*   pTestPlane = NULL;
    cVector3   vecNormal;
	cVector3   vecCentrePoint;
    int        i           = 0;
	int		   iPlaneCount = 0;
    float      fDistance   = 0.0f;

    // loop through each face
    for ( pIterator = m_pFaceList; pIterator; pIterator = pIterator->m_pNext )
    {
        // calculate polygons centre point
        vecCentrePoint = pIterator->m_pVertices [ 0 ];

        for ( i = 1; i < (int)pIterator->m_dwVertexCount; i++ )
			vecCentrePoint += pIterator->m_pVertices [ i ];

        vecCentrePoint /= (float)pIterator->m_dwVertexCount;

        // calculate polygons plane
        Plane = cPlane3 ( pIterator->m_vecNormal, vecCentrePoint );

        // search through plane list to see if one already exists ( must use small tolerances )
        iPlaneCount = GetPlaneCount ( );

		for ( i = 0; i < iPlaneCount; i++ )
        {
            // retrieve the plane details
            pTestPlane = GetPlane ( i );
            
            // rest the plane details
            if ( 
					fabsf ( pTestPlane->m_fDistance - Plane.m_fDistance ) < 1e-5f
					&&
					Plane.m_vecNormal.FuzzyCompare ( pTestPlane->m_vecNormal, 1e-5f )
			   )
			   break;
		}

        if ( i == iPlaneCount )
        {
            if ( ! IncreasePlaneCount ( ) )
				return S_FALSE;

            *GetPlane ( iPlaneCount ) = Plane;
		}

        pIterator->m_iPlane = i;

        vecNormal = GetPlane ( i )->m_vecNormal;
        fDistance = GetPlane ( i )->m_fDistance;

        // ensure that all vertices are on the selected plane
        for ( int v = 0; v < (int)pIterator->m_dwVertexCount; v++ )
        {
            float fResult = pIterator->m_pVertices [ v ].Dot ( vecNormal ) - fDistance;
            pIterator->m_pVertices [ v ] += ( vecNormal * -fResult );
        }
	}

    return S_OK;
}

HRESULT cBSPTree::BuildBSPTree ( int iNode, cBSPFace* pFaceList )
{  
	// build's the entire BSP Tree using the already initialised poly data
	// we use 'goto' for the bail procedure for speed and stack space 
	// purposes only

    cBSPFace*	pTestFace	= NULL;
	cBSPFace*	pNextFace	= NULL;
	cBSPFace*	pFrontList	= NULL;
	cBSPFace*	pBackList	= NULL;
	cBSPFace*	pFrontSplit = NULL;
	cBSPFace*	pBackSplit	= NULL;
    cBSPFace*	pSplitter   = NULL;
    HRESULT     ErrCode     = S_OK;
    CLASSIFY    Result;
    int         v;

    // select the best splitter from the list of faces passed
    pSplitter = SelectBestSplitter ( pFaceList, m_OptionSet.iSplitterSample, m_OptionSet.fSplitHeuristic );

    if ( !pSplitter )
	{ 
		ErrCode = S_FALSE;
		goto BuildError;
	}

    // flag as used, and store plane
    pSplitter->m_bUsedAsSplitter = true;
    GetNode ( iNode )->m_iPlane  = pSplitter->m_iPlane;

    // begin face iteration....
    // at each step we set the top level face list to the next face as well
    // as the iterator so that we will have a chance to release the remaining faces.
    for ( pTestFace = pFaceList; pTestFace != NULL; pTestFace = pNextFace, pFaceList = pNextFace ) 
    {
        // store plane for easy access
        cPlane3* pPlane = GetPlane ( pTestFace->m_iPlane );

        // store next face, as 'TestFace' may be modified / deleted 
		pNextFace = pTestFace->m_pNext;

        // classify the polygon
        if ( pTestFace->m_iPlane == pSplitter->m_iPlane )
        {
            Result = CP_ONPLANE;
        }
        else
        {
            Result = GetPlane ( pSplitter->m_iPlane )->ClassifyPoly ( pTestFace->m_pVertices, pTestFace->m_dwVertexCount, sizeof ( cVertex ) );
        }

        // classify the polygon against the selected plane
		switch ( Result ) 
        {
            case CP_ONPLANE:
			{
                #if ( BSP_SHARENODES )
					// mark matching planes as used ( accuracy may be decreased )
                    if ( !pTestFace->m_bUsedAsSplitter )
                    {
                        pTestFace->m_bUsedAsSplitter = true;
                    }
				#endif

                // test the direction of the face against the plane.
                if ( GetPlane ( pSplitter->m_iPlane )->SameFacing ( pPlane->m_vecNormal ) ) 
                {
                    pTestFace->m_pNext = pFrontList;
				    pFrontList	   	   = pTestFace;
			    }
                else
                {
				    pTestFace->m_pNext = pBackList;
				    pBackList	   	   = pTestFace;	
			    }
			}
			break;

            case CP_FRONT:
			{
			    // pass the face straight down the front list
			    pTestFace->m_pNext = pFrontList;
			    pFrontList		   = pTestFace;
			}
			break;

            case CP_BACK:
			{
			    // pass the face straight down the back list
			    pTestFace->m_pNext = pBackList;
			    pBackList	 	  = pTestFace;
			}
			break;

            case CP_SPANNING:
			{
                // ensure this is not an invalid operation
                if ( pTestFace == pSplitter )
				{ 
					ErrCode = S_FALSE;
					goto BuildError;
				}

                // allocate new front fragment
                if ( ! ( pFrontSplit = AllocBSPFace ( ) ) )
				{
					ErrCode = S_FALSE;
					goto BuildError;
				}

                pFrontSplit->m_pNext = pFrontList;
			    pFrontList			 = pFrontSplit;
                
                // allocate new back fragment
                if ( ! ( pBackSplit = AllocBSPFace ( ) ) )
				{
					ErrCode = S_FALSE;
					goto BuildError;
				}

                pBackSplit->m_pNext = pBackList;
			    pBackList  		    = pBackSplit;

                // split the polygon
                if ( FAILED ( ErrCode = pTestFace->Split ( *GetPlane ( pSplitter->m_iPlane ), pFrontSplit, pBackSplit ) ) )
					goto BuildError; 

                // ensure that all vertices of the new fragment are on the original plane
                for ( v = 0; v < (int)pFrontSplit->m_dwVertexCount; v++ )
                {
                    float fResult = pFrontSplit->m_pVertices [ v ].Dot ( pPlane->m_vecNormal ) - pPlane->m_fDistance;
                    pFrontSplit->m_pVertices [ v ] += ( pPlane->m_vecNormal * -fResult );
				}

                // ensure that all vertices of the new fragment are on the original plane
                for ( v = 0; v < (int)pBackSplit->m_dwVertexCount; v++ )
                {
                    float fResult = pBackSplit->m_pVertices [ v ].Dot ( pPlane->m_vecNormal ) - pPlane->m_fDistance;
                    pBackSplit->m_pVertices [ v ] += ( pPlane->m_vecNormal * -fResult );
				}
                
                #if ( BSP_NEWSPLITNODES )
                    // allow split fragments to generate their own node ( accuracy may be increased )
                    pFrontSplit->m_bUsedAsSplitter = false;
                    pBackSplit->m_bUsedAsSplitter  = false;
				#endif

                // + 2 Fragments - 1 Original
                m_iActiveFaces++; 

                delete pTestFace;
			}
			break;
		}
	}
   
    // calculate the nodes bounding box
	GetNode ( iNode )->CalculateBounds ( pFrontList, true  );
    GetNode ( iNode )->CalculateBounds ( pBackList,  false );

	// if the front list is empty flag this as an empty leaf
	// otherwise push the front list down the front of this node
    if ( CountSplitters ( pFrontList ) == 0 ) 
    {
        // add a new leaf and store the resulting faces
        if ( !IncreaseLeafCount ( ) )
		{
			ErrCode = S_FALSE;
			goto BuildError;
		}

		GetNode ( iNode )->m_iFront = -GetLeafCount ( );

        if ( FAILED ( ErrCode = ProcessLeafFaces ( GetLeaf ( GetLeafCount ( ) - 1 ), pFrontList ) ) )
			goto BuildError;
    } 
    else
    {
        // allocate a new node and step into it
        if ( !IncreaseNodeCount ( ) )
		{ 
			ErrCode = S_FALSE;
			goto BuildError;
		}

	    GetNode ( iNode )->m_iFront = GetNodeCount ( ) - 1;

	    ErrCode = BuildBSPTree ( GetNode ( iNode )->m_iFront, pFrontList );
	}

    // front list has been passed off, we no longer own these
    pFrontList = NULL;

    if ( FAILED ( ErrCode ) )
		goto BuildError;

    // should we back leaf cull?
    if ( m_OptionSet.bRemoveBackLeaves ) 
    {
        // if no potential splitters remain, free the back list
        if ( pBackList && CountSplitters ( pBackList ) == 0 )
        { 
			// move illegal faces into temporary removed list
            for ( pTestFace = pBackList; pTestFace; pTestFace = pTestFace->m_pNext ) 
            {
                m_vpRemovedFaces.push_back ( pTestFace );
                m_iActiveFaces--;
            }
			
            pBackList = NULL;
		}
    }

    // if the back list is empty flag this as solid
	// otherwise push the back list down the back of this node
	if ( !pBackList )
    {
        GetNode ( iNode )->m_iBack = -1;
	}
    else
    {
        // allocate a new node and step into it
        if ( !IncreaseNodeCount ( ) )
		{
			ErrCode = S_FALSE;
			goto BuildError;
		}

	    GetNode ( iNode )->m_iBack = GetNodeCount ( ) - 1;

	    ErrCode = BuildBSPTree ( GetNode ( iNode )->m_iBack, pBackList );
	}

    // back list has been passed off, we no longer own these
    pBackList = NULL;

    if ( FAILED ( ErrCode ) )
		goto BuildError;

    return S_OK;

	BuildError:
		// add all currently allocated faces to garbage heap
		TrashFaceList ( pFaceList );
		TrashFaceList ( pFrontList );
		TrashFaceList ( pBackList  );

    return ErrCode;
}

HRESULT cBSPTree::ProcessLeafFaces ( cBSPLeaf* pLeaf, cBSPFace* pFaceList )
{
	// this function decides what to do with the faces, intended for use
	// within a leaf, based upon the type of tree being compiled
	
    HRESULT ErrCode = S_OK;

    // depending on the tree type we may need to store these resulting faces
    if ( m_OptionSet.iTreeType == BSP_TYPE_NONSPLIT )
    {
        // add these faces to the leaf
        if ( FAILED ( ErrCode = pLeaf->BuildFaceIndices ( pFaceList ) ) )
			return ErrCode;

        // release the faces
        FreeFaceList ( pFaceList );
    }
    else
    {
        // split tree, we need to store the face pointers
        cBSPFace* pIterator = pFaceList;

        while ( pIterator != NULL )
        {
            if ( ! IncreaseFaceCount ( ) )
				return S_FALSE;

            SetFace ( GetFaceCount ( ) - 1, pIterator );

            // set our original index to the final position it 
            // is stored in the arrray
            pIterator->m_iOriginalIndex = GetFaceCount ( ) - 1;
            pIterator = pIterator->m_pNext;
        }

        // add these faces to the leaf
        if ( FAILED ( ErrCode = pLeaf->BuildFaceIndices ( pFaceList ) ) )
			return ErrCode;
	}

    return S_OK;
}
        
int cBSPTree::CountSplitters ( cBSPFace* pFaceList )
{
    int       iSplitterCount = 0;
    cBSPFace* pIterator      = pFaceList;
    
    // count the number of splitters
	while ( pIterator != NULL ) 
    {
		if ( !pIterator->m_bUsedAsSplitter )
			iSplitterCount++;

		pIterator = pIterator->m_pNext;
	}

    // return number of splitters remaining
    return iSplitterCount;
}

cBSPFace* cBSPTree::SelectBestSplitter ( cBSPFace* pFaceList, int iSplitterSample, float fSplitHeuristic )
{
	// picks the next face in the list to be used as the Splitting plane.
	// you can pass a value to SplitHeuristic, the higher the value
	// the higher the importance is put on reducing splits

    int	iScore,
		iSplits,
		iBackFaces,
		iFrontFaces;

	int iBestScore     = 10000000,
		iSplitterCount = 0;

    cBSPFace* pSplitter     = pFaceList;
	cBSPFace* pCurrentFace  = NULL;
	cBSPFace* pSelectedFace = NULL;
	
	// traverse the face list
	while ( pSplitter != NULL ) 
    {
		// if this has not been used as a splitter then
		if ( ! pSplitter->m_bUsedAsSplitter ) 
        {
			// create testing splitter plane
			cPlane3 SplittersPlane ( pSplitter->m_vecNormal, pSplitter->m_pVertices [ 0 ] );
			
            // test against the other poly's and count the score
            pCurrentFace = pFaceList;
			iScore = iSplits = iBackFaces = iFrontFaces = 0;

			while ( pCurrentFace != NULL )
            {
                CLASSIFY Result = SplittersPlane.ClassifyPoly ( pCurrentFace->m_pVertices, pCurrentFace->m_dwVertexCount, sizeof ( cVertex ) );

				switch ( Result ) 
                {
                    case CP_FRONT:
				        iFrontFaces++;
				    break;

                    case CP_BACK:
					    iBackFaces++;
				    break;

				    case CP_SPANNING:
					    iSplits++;
				    break;

				    default:
				    break;
				}

				pCurrentFace = pCurrentFace->m_pNext;
            }
			
            // tally the score ( modify the splits * n )
			iScore = ( int ) ( abs ( iFrontFaces - iBackFaces ) + ( iSplits * fSplitHeuristic ) );
			
			if ( iScore < iBestScore )
            {
				iBestScore	  = iScore;
				pSelectedFace = pSplitter;
            }

            iSplitterCount++;
		}

		pSplitter = pSplitter->m_pNext;
        
        if ( iSplitterSample != 0 && iSplitterCount >= iSplitterSample && pSelectedFace )
			break;
	}

    return pSelectedFace;
}

HRESULT cBSPTree::ClipTree ( cBSPTree* pTree, bool bClipSolid, bool bRemoveCoPlanar, int iCurrentNode, cBSPFace* pFaceList )
{
	// used to clip the BSP Tree passed in, against this tree
	// this function is primarily used by CSG operations

    cBSPFace*	pTestFace	 = NULL;
	cBSPFace*	pNextFace	 = NULL;
	cBSPFace*	pFrontList	 = NULL;
	cBSPFace*	pBackList	 = NULL;
    cBSPFace*	pFrontSplit  = NULL;
	cBSPFace*	pBackSplit	 = NULL;
    int			iPlane       = 0;
    HRESULT     ErrCode      = S_OK;

    if ( !pTree )
		return S_FALSE;

    if ( pTree->GetFaceCount ( ) < 1 || GetFaceCount ( ) < 1 )
		return S_FALSE;

	// if this is the first call to ClipTree, then we must build our 
	// lists to work with ( this just takes some work away from the caller )
	if ( pFaceList == NULL )
	{
	    UINT i = 0;
	    
		// build our sequential linked list
		for ( i = 0; i < (UINT)pTree->GetFaceCount ( ); i++ )
		{
			pTree->GetFace ( i )->m_iChildSplit [ 0 ] = -1;
			pTree->GetFace ( i )->m_iChildSplit [ 1 ] = -1;

            if ( i > 0 )
				pTree->GetFace ( i - 1 )->m_pNext = pTree->GetFace ( i );
		}
		
        // reset last face just to be certain
		pTree->GetFace ( i - 1 )->m_pNext = NULL;

        pFaceList = pTree->GetFace ( 0 );
	}

    // all polygons are now hooked together in a linked list with the last one in the array pointing to NULL,
	// now we loop through the linked list starting at the first face in the array and send them down this tree
    iPlane = GetNode ( iCurrentNode )->m_iPlane;
    
    for ( pTestFace = pFaceList; pTestFace; pTestFace = pNextFace )
    {   
        // store next face, as 'TestFace' may be modified / deleted 
		pNextFace = pTestFace->m_pNext;

        // skip this polygon it has been deleted in some previous csg op
		if ( pTestFace->m_bDeleted )
			continue;

        // classify the polygon against the selected plane
		switch ( GetPlane ( iPlane )->ClassifyPoly ( pTestFace->m_pVertices, pTestFace->m_dwVertexCount, sizeof ( cVertex ) ) ) 
        {
            case CP_ONPLANE:
			{
			    // test the direction of the face against the plane
                if ( GetPlane ( iPlane )->SameFacing ( pTestFace->m_vecNormal ) )
                {
                    if ( bRemoveCoPlanar )
					{
						pTestFace->m_pNext = pBackList;
						pBackList          = pTestFace;
					}
                    else
					{
						pTestFace->m_pNext = pFrontList;
						pFrontList         = pTestFace;
					}	
			    } 
                else 
                {
				    pTestFace->m_pNext = pBackList;
				    pBackList		   = pTestFace;	
			    }
			}
            break;

            case CP_FRONT:
			{
			    pTestFace->m_pNext = pFrontList;
			    pFrontList		   = pTestFace;
			}
			break;

            case CP_BACK:
			{
			    pTestFace->m_pNext = pBackList;
			    pBackList		   = pTestFace;	
			}
			break;

            case CP_SPANNING:
			{
			    // allocate new front within the passed tree
                if ( ! ( pFrontSplit = AllocBSPFace ( ) ) )
				{
					ErrCode = S_FALSE;
					goto ClipError;
				}

                if ( !pTree->IncreaseFaceCount ( ) )
				{
					ErrCode = S_FALSE;
					goto ClipError;
				}

                pTree->SetFace ( pTree->GetFaceCount ( ) - 1, pFrontSplit );

                // allocate new back fragment within the passed tree
                if ( ! ( pBackSplit = AllocBSPFace ( ) ) )
				{
					ErrCode = S_FALSE;
					goto ClipError;
				}

                if ( ! pTree->IncreaseFaceCount ( ) )
				{
					ErrCode = S_FALSE;
					goto ClipError;
				}

                pTree->SetFace ( pTree->GetFaceCount ( ) - 1, pBackSplit );
			    
                // split the polygon
                if ( FAILED ( ErrCode = pTestFace->Split ( *GetPlane ( iPlane ), pFrontSplit, pBackSplit ) ) )
					goto ClipError;
                
                // since this is a coincidental pre-process on mini bsp trees
				// we don't actually need to update the leaf polys
				pTestFace->m_bDeleted          = true;
				pTestFace->m_iChildSplit [ 0 ] = pTree->GetFaceCount ( ) - 2;
				pTestFace->m_iChildSplit [ 1 ] = pTree->GetFaceCount ( ) - 1;

                // add it to the head of our recursion lists
                pFrontSplit->m_pNext = pFrontList;
                pFrontList           = pFrontSplit;
                pBackSplit->m_pNext  = pBackList;
                pBackList            = pBackSplit;
			}
			break;
		}
	}
	
	// now onto the clipping
	if ( bClipSolid )
	{
		if ( GetNode ( iCurrentNode )->m_iBack < 0 ) 
		{
			// iterate through and flag all back polys as deleted
			for ( pTestFace = pBackList; pTestFace; pTestFace = pTestFace->m_pNext )
				pTestFace->m_bDeleted = true;
			
			// empty back list
			pBackList = NULL;     
        }
	}
	else
	{
		if ( GetNode ( iCurrentNode )->m_iFront < 0 ) 
		{
			// iterate through and flag all front polys as deleted
			for ( pTestFace = pFrontList; pTestFace; pTestFace = pTestFace->m_pNext )
				pTestFace->m_bDeleted = true;
			
			// empty front list
			pFrontList = NULL;     
        }
	}

	// pass down the front of the node
    if ( pFrontList && GetNode ( iCurrentNode )->m_iFront >= 0 )
		ErrCode = ClipTree ( pTree, bClipSolid, bRemoveCoPlanar, GetNode ( iCurrentNode )->m_iFront, pFrontList );
    
    if ( FAILED ( ErrCode ) )
		goto ClipError;

    // pass down the back of the node
    if ( pBackList && GetNode ( iCurrentNode )->m_iBack >= 0 )
		ErrCode = ClipTree ( pTree, bClipSolid, bRemoveCoPlanar, GetNode ( iCurrentNode )->m_iBack, pBackList );

    if ( FAILED ( ErrCode ) )
		goto ClipError;

	return S_OK;

	ClipError:
		return ErrCode;
}

void cBSPTree::RepairSplits ( void )
{
	// repairs any unnecesary splits caused during the ClipTree operation,
	// because polygons are only flagged as being deleted during clipping
	// this function merely alters the deleted flag values.

    // test for child polygon survival and replace with parent polygons if possible
    for ( int i = GetFaceCount ( ) - 1; i >= 0; i-- )
    {
        // if one isn't -1, neither is the other
        if ( GetFace ( i )->m_iChildSplit [ 0 ] != -1 )
        {
            // if the two children are valid this split should be repaired
            if ( !GetFace ( GetFace ( i )->m_iChildSplit [ 0 ] )->m_bDeleted && ! GetFace ( GetFace ( i )->m_iChildSplit [ 1 ] )->m_bDeleted )
            {
                // restore the parent, and delete the children
                GetFace ( i                                  )->m_bDeleted = false;
                GetFace ( GetFace ( i )->m_iChildSplit [ 0 ] )->m_bDeleted = true;
                GetFace ( GetFace ( i )->m_iChildSplit [ 1 ] )->m_bDeleted = true;
			}
		}
	}
}

bool cBSPTree::IntersectedByTree ( cBSPTree* pTree )
{
	// tests each face in the tree passed to see if it intersects this

    // loop through each face testing for intersection
    for ( int i = 0; i < pTree->GetFaceCount ( ); i++ ) 
    {
        if ( IntersectedByFace ( pTree->GetFace ( i ) ) )
			return true;
	}

    return false;
}

bool cBSPTree::IntersectedByFace ( cFace* pFace, int iNode )
{
	// this function tests the passed face against the BSP tree, if any part
	// of the face ends up in solid space, then this face is considered to
	// be intersecting the BSP Tree

    // validate Params
    if ( !pFace || iNode < 0 || iNode >= GetNodeCount ( ) )
		return false;

    int iNodeFront = GetNode ( iNode )->m_iFront;
    int iNodeBack  = GetNode ( iNode )->m_iBack;

    // classify this poly against the nodes plane
    switch ( GetPlane ( GetNode ( iNode )->m_iPlane )->ClassifyPoly ( pFace->m_pVertices, pFace->m_dwVertexCount, sizeof ( cVertex ) ) ) 
    {
        case CP_SPANNING:
        case CP_ONPLANE:
		{
            // solid Leaf
            if ( iNodeBack < 0 )
				return true;

            // pass down the front
            if ( iNodeFront >= 0 )
			{
				if ( IntersectedByFace ( pFace, iNodeFront ) )
					return true;
			}

            // pass down the back
            if ( iNodeBack >= 0 )
			{
				if ( IntersectedByFace ( pFace, iNodeBack ) )
					return true;
			}
		}
        break;

        case CP_FRONT:
		{
            // pass down the front
            if ( iNodeFront >= 0 )
			{ 
				if ( IntersectedByFace ( pFace, iNodeFront ) )
					return true;
			}
		}
        break;

        case CP_BACK:
		{
            // solid leaf
            if ( iNodeBack < 0 )
				return true;

            // pass down the back
            if ( iNodeBack >= 0 )
			{ 
				if ( IntersectedByFace ( pFace, iNodeBack ) )
					return true;
			}
		}
        break;
	}

    return false;
}

int cBSPTree::FindLeaf ( cVector3& vecPosition )
{
	// given a position, this function attempts to determine which leaf
	// that point falls into

    int	iNode = 0,
		iLeaf = 0;
	
	for ( ;; )
    {
        cBSPNode* pNode = GetNode ( iNode );

		switch ( GetPlane ( pNode->m_iPlane )->ClassifyPoint ( vecPosition ) ) 
        {
            case CP_ONPLANE:
            case CP_FRONT:
			{
                if ( pNode->m_iFront < 0 )
                {
                    return abs ( pNode->m_iFront + 1 );
                }
                else
                {
                    iNode = pNode->m_iFront;
                }
			}
			break;

			case CP_BACK: 
			{	
                if ( pNode->m_iBack < 0 ) 
                {
					return -1;
                }
                else 
                {
					iNode = pNode->m_iBack;
				}
			}
			break;
		}
	}

    // will never get here!
    return -1;
}

bool cBSPTree::IntersectedBySphere ( cVector3& vecSphereCentre, float fSphereRadius, int iNode )
{
    cBSPNode* pNode  = GetNode  ( iNode );
    cPlane3*  pPlane = GetPlane ( pNode->m_iPlane );

    // get the distance from the sphere, to the node ( a long the node plane's normal )
    float      fDistance = fabsf ( vecSphereCentre.DistanceToPlane ( *pPlane ) );

    // classify the sphere centre against the node ( retrieve distance to plane )
    switch ( pPlane->ClassifyPoint ( vecSphereCentre ) )
    {
        case CP_ONPLANE:
        case CP_FRONT:
		{       
            // if the sphere is in front of the node, pass it down the front
            if ( pNode->m_iFront >= 0 )
            {
                // traverse down the front
                if ( IntersectedBySphere ( vecSphereCentre, fSphereRadius, pNode->m_iFront ) )
					return true;
            }

            // if we are spanning the plane, we must also traverse down the back
            if ( fDistance < fSphereRadius )
            {
                if ( pNode->m_iBack < 0 )
                {
                    return true;
                }
                else
                {
                    if ( IntersectedBySphere ( vecSphereCentre, fSphereRadius, pNode->m_iBack ) )
						return true;
				}
			}
		}
        break;

        case CP_BACK:
        {
			// if the sphere is behind the node, pass it down the back
            if ( pNode->m_iBack < 0 )
            {
				return true;
			}
            else
            {
                if ( IntersectedBySphere ( vecSphereCentre, fSphereRadius, pNode->m_iBack ) )
					return true;
			}

            // if we are spanning the plane, we must also traverse down the back
            if ( fDistance < fSphereRadius )
            {
                if ( pNode->m_iFront >= 0 )
                {
                    if ( IntersectedBySphere ( vecSphereCentre, fSphereRadius, pNode->m_iFront ) )
						return true;
				}
			}
		}
        break;
    }

    return false;
}

bool cBSPTree::LinkDetailObject ( int iObjectIndex, cVector3& vecSphereCentre, float fSphereRadius, int iNode )
{
	// we can add in complex objects which we don't want used in the BSP compile into the tree

    return false;
}

HRESULT cBSPTree::SetPVSData ( UCHAR PVSData [ ], int iPVSSize, bool bPVSCompressed )
{
    if ( m_pPVSData )
		delete [ ] m_pPVSData;

    try
    {
        m_pPVSData = new UCHAR [ iPVSSize ];

        if ( !m_pPVSData )
			throw std::bad_alloc ( );

        memcpy ( m_pPVSData, PVSData, iPVSSize );
	}

    catch ( std::bad_alloc )
    {
        return S_FALSE;
	}

    m_iPVSDataSize   = iPVSSize;
    m_bPVSCompressed = bPVSCompressed;

    return S_OK;
}

void cBSPTree::FreeFaceList ( cBSPFace* pFaceList )
{
    cBSPFace* pTestFace = pFaceList;
	cBSPFace* pNextFace = NULL;

    while ( pTestFace != NULL )
    {
        pNextFace = pTestFace->m_pNext;
        delete pTestFace;
        m_iActiveFaces--;
        pTestFace = pNextFace;
    }
}

void cBSPTree::TrashFaceList ( cBSPFace* pFaceList )
{
    #ifndef __GNUC__
    cBSPFace* pIterator = pFaceList;

    while ( pIterator != NULL )
    {
        if ( std::find ( m_vpGarbage.begin ( ), m_vpGarbage.end ( ), pIterator ) == m_vpGarbage.end ( ) )
        {
			try
            {
                if ( m_vpGarbage.size ( ) >= ( m_vpGarbage.capacity ( ) - 1 ) )
                {
                    m_vpGarbage.reserve ( m_vpGarbage.size ( ) + BSP_ARRAY_THRESHOLD );
                }
                
                m_vpGarbage.push_back ( pIterator );
            }
			catch ( std::exception& )
			{
				return;
			}
        }

        pIterator = pIterator->m_pNext;
	}
	#endif
}

bool cBSPTree::IncreaseNodeCount ( void )
{
	cBSPNode *pNewNode = NULL;

    try 
    {
        // resize the vector if we need to
        if ( m_vpNodes.size ( ) >= ( m_vpNodes.capacity ( ) - 1 ) )
        {
            m_vpNodes.reserve ( m_vpNodes.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        if ( ! ( pNewNode = new cBSPNode ) )
			throw std::bad_alloc ( );

        m_vpNodes.push_back ( pNewNode );
    }
    catch ( std::bad_alloc )
    { 
        return false; 
    } 
    catch (...) 
    { 
        if ( pNewNode )
			delete pNewNode;

        return false;
    }

    return true;
}

bool cBSPTree::IncreaseLeafCount ( void )
{
	cBSPLeaf* pNewLeaf = NULL;

    try 
    {
        if ( m_vpLeaves.size ( ) >= ( m_vpLeaves.capacity ( ) - 1 ) )
        {
            m_vpLeaves.reserve ( m_vpLeaves.size ( ) + BSP_ARRAY_THRESHOLD );
        }
        
        if ( ! ( pNewLeaf = new cBSPLeaf ) )
			throw std::bad_alloc ( );

        m_vpLeaves.push_back ( pNewLeaf );
    }
	catch ( std::bad_alloc ) 
    { 
        return false; 
    } 
    catch (...) 
    { 
        if ( pNewLeaf )
			delete pNewLeaf;
        
		return false;
    }

    return true;
}

bool cBSPTree::IncreasePlaneCount ( void )
{
	cPlane3* pNewPlane = NULL;

    try 
    {
        if ( m_vpPlanes.size ( ) >= ( m_vpPlanes.capacity ( ) - 1 ) )
        {
            m_vpPlanes.reserve ( m_vpPlanes.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        if ( ! ( pNewPlane = new cPlane3 ) )
			throw std::bad_alloc ( );

        m_vpPlanes.push_back ( pNewPlane );
    }
	catch ( std::bad_alloc )
    { 
        return false; 
    } 
    catch ( ... )
    { 
        if ( pNewPlane )
			delete pNewPlane;

        return false;
    }

    return true;
}

bool cBSPTree::IncreaseFaceCount ( void )
{
	try
    {
        if ( m_vpFaces.size ( ) >= ( m_vpFaces.capacity ( ) - 1 ) )
        {
            m_vpFaces.reserve ( m_vpFaces.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        m_vpFaces.push_back ( NULL );
    }
	catch ( ... )
    { 
        return false; 
    }

    return true;
}

bool cBSPTree::IncreasePortalCount ( void )
{
    try 
    {
        if ( m_vpPortals.size ( ) >= ( m_vpPortals.capacity ( ) - 1 ) )
        {
            m_vpPortals.reserve ( m_vpPortals.size ( ) + BSP_ARRAY_THRESHOLD );
        }

        m_vpPortals.push_back ( NULL );
    }
	catch ( ... ) 
    { 
        return false; 
    }

    return true;
}

cBSPFace* cBSPTree::AllocBSPFace ( cFace* pDuplicate )
{
    cBSPFace* pNewFace = NULL;

    try
    {
        if ( pDuplicate != NULL )
        {
            pNewFace = new cBSPFace ( pDuplicate );
        } 
        else 
        {
            pNewFace = new cBSPFace;
        }
        
        if ( !pNewFace )
			throw std::bad_alloc ( );
	}
    catch ( ... )
	{
		return NULL;
	}

    return pNewFace;
}

cBSPPortal* cBSPTree::AllocBSPPortal ( void )
{
    cBSPPortal* pNewPortal = NULL;

    try
    {
        pNewPortal = new cBSPPortal;

        if ( !pNewPortal )
			throw std::bad_alloc ( );
	}
    catch ( ... )
	{
		return NULL;
	}

    return pNewPortal;
}
