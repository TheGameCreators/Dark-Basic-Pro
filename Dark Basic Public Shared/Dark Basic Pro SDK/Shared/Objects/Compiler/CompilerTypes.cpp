
#include <new>
#include "CompilerTypes.h"
#include "CPlane.h"
#include "CBounds.h"
#include "CBSPTree.h"

cMesh::cMesh ( )
{
	// mesh constructor

	m_ppFaces       = NULL;
    m_dwFaceCount   = 0;
    m_dwFlags       = 0;
    m_szName        = NULL;
}

cMesh::~cMesh ( )
{
	// mesh destructor

	ReleaseFaces ( );

    if ( m_szName )
		delete [ ] m_szName;
}

int cMesh::AddFaces ( int iFaceCount )
{
	// add faces

	// set up new buffer
    cFace** ppFaceBuffer = NULL;

	// return if invalid face count
    if ( iFaceCount == 0 )
		return -1;
	
    try 
    {
		// allocate new block of memory
        ppFaceBuffer = new cFace* [ m_dwFaceCount + iFaceCount ];

		// is it valid
        if ( !ppFaceBuffer )
			throw std::bad_alloc ( );

		// clear out memory
        memset ( ppFaceBuffer, 0, ( m_dwFaceCount + iFaceCount ) * sizeof ( cFace* ) );

		// copy memory
        if ( m_ppFaces )
			memcpy ( ppFaceBuffer, m_ppFaces, m_dwFaceCount * sizeof ( cFace* ) );
            
		// allocate new faces
        for ( int iFace = 0; iFace < iFaceCount; iFace++ )
        {
			// create new face
            ppFaceBuffer [ m_dwFaceCount + iFace ] = new cFace;

			// is it valid
            if ( !ppFaceBuffer [ m_dwFaceCount + iFace ] )
				throw std::bad_alloc ( );
        }
	}
	catch ( ... )
    {
		// if something goes wrong we come here

		// has face buffer been created
        if ( ppFaceBuffer ) 
        {
			// delete each face
            for ( int iFace = 0; iFace < iFaceCount; iFace++ )
            {
                if ( ppFaceBuffer [ m_dwFaceCount + iFace ] )
					delete ppFaceBuffer [ m_dwFaceCount + iFace ];
			}

			// delete face buffer
            delete [ ] ppFaceBuffer;
        }

        return -1;
    }
		
	// delete original faces
    delete [ ] m_ppFaces;

	// now store pointer to new faces
	m_ppFaces = ppFaceBuffer;

	// increment face count
	m_dwFaceCount += iFaceCount;

	// return the new face count
	return m_dwFaceCount - iFaceCount;
}

void cMesh::ReleaseFaces ( void )
{
	// delete all faces

	// make sure buffer is valid
    if ( m_ppFaces ) 
    { 
        for ( int iFace = 0; iFace < (int)m_dwFaceCount; iFace++ )
        {
            if ( m_ppFaces [ iFace ] )
				delete m_ppFaces [ iFace ];
        }

        delete [ ] m_ppFaces;
        m_ppFaces = NULL;
    }

    m_dwFaceCount = 0;
}

bool cMesh::BuildFromBSPTree ( cBSPTree* pTree, bool Reset )
{
    int iCounter = 0;
	int iFace    = 0;

    if ( !pTree )
		return false;

    if ( Reset )
	{
		ReleaseFaces ( );
	}
    
    // first count the number of non deleted faces so that we can allocate in one batch
    for ( iFace = 0; iFace < pTree->GetFaceCount ( ); iFace++ )
    {
        if ( !pTree->GetFace ( iFace )->m_bDeleted && pTree->GetFace ( iFace )->m_dwVertexCount >= 3 )
			iCounter++;
	}

    if ( AddFaces ( iCounter ) < 0 )
		return false;

    iCounter = m_dwFaceCount - iCounter;

    for ( iFace = 0; iFace < pTree->GetFaceCount ( ); iFace++ )
    {
        cFace*		pFace     = m_ppFaces [ iCounter ];
        cBSPFace*	pTreeFace = pTree->GetFace ( iFace );
        
        if ( pTreeFace->m_bDeleted )
			continue;

        if ( pTreeFace->m_dwVertexCount < 3 )
			continue;

        pFace->m_iMaterialIndex = pTreeFace->m_iMaterialIndex;
        pFace->m_iTextureIndex  = pTreeFace->m_iTextureIndex;
        pFace->m_iFlags         = pTreeFace->m_iFlags;
        pFace->m_iSrcBlendMode  = pTreeFace->m_iSrcBlendMode;
        pFace->m_iDestBlendMode = pTreeFace->m_iDestBlendMode;
        pFace->m_vecNormal      = pTreeFace->m_vecNormal;
        
        if ( pFace->AddVertices ( pTreeFace->m_dwVertexCount ) < 0 )
			return false;

        memcpy ( pFace->m_pVertices, pTreeFace->m_pVertices, pFace->m_dwVertexCount * sizeof ( cVertex ) );

        iCounter++;
	}

    CalculateBoundingBox ( );

    return true;
}

cBounds3& cMesh::CalculateBoundingBox ( void )
{
    m_Bounds.Reset ( );

    for ( int iFace = 0; iFace < (int)m_dwFaceCount; iFace++ )
    {
        m_Bounds.CalculateFromPolygon ( m_ppFaces [ iFace ]->m_pVertices, m_ppFaces [ iFace ]->m_dwVertexCount, sizeof ( cVertex ), false );
	}

    return m_Bounds;
}

cPolygon::cPolygon ( )
{
	m_pVertices		= NULL;
    m_dwVertexCount	= 0;
}

cPolygon::~cPolygon ( )
{
	ReleaseVertices ( );
}

int cPolygon::AddVertices ( int iVertexCount )
{
    cVertex* pVertexBuffer = NULL;

    if ( iVertexCount == 0 )
		return -1;
	
	try 
    {
        pVertexBuffer = new cVertex [ m_dwVertexCount + iVertexCount ];
        
		if ( !pVertexBuffer )
			throw std::bad_alloc ( );

        if ( m_pVertices )
        {    
            memcpy ( pVertexBuffer, m_pVertices, m_dwVertexCount * sizeof ( cVertex ) );
		}
    }
	catch ( ... )
    {
        if ( pVertexBuffer )
			delete [ ] pVertexBuffer;

        return -1;
	}

	delete [ ] m_pVertices;
	m_pVertices    = pVertexBuffer;

	m_dwVertexCount += iVertexCount;

	return m_dwVertexCount - iVertexCount;
}

int cPolygon::InsertVertex ( int iVertexPos )
{
    if ( AddVertices ( 1 ) < 0 )
		return -1;

    if ( iVertexPos != m_dwVertexCount - 1 )
    {
        memmove ( &m_pVertices [ iVertexPos + 1 ], &m_pVertices [ iVertexPos ], ( ( m_dwVertexCount - 1 ) - iVertexPos ) * sizeof ( cVertex ) );
	}
		   
    m_pVertices [ iVertexPos ] = cVertex ( 0.0f, 0.0f, 0.0f );

    return iVertexPos;
}

bool cPolygon::GenerateFromPlane ( cPlane3& Plane, cBounds3& Bounds )
{
    cVector3 CB, CP, U, V, A;

	CB = Bounds.GetCentre ( );

	float DistanceToPlane = CB.DistanceToPlane ( Plane );
	
	CP = CB + ( Plane.m_vecNormal * -DistanceToPlane );
	
	A = cVector3 ( 0.0f, 0.0f, 0.0f );

	if ( fabs ( Plane.m_vecNormal.y ) > fabs ( Plane.m_vecNormal.z ) )
	{
		if ( fabs ( Plane.m_vecNormal.z ) < fabs ( Plane.m_vecNormal.x ) )
			A.z = 1;
		else
			A.x = 1;
	}
	else
	{
		if ( fabs ( Plane.m_vecNormal.y ) <= fabs ( Plane.m_vecNormal.x ) )
			A.y = 1;
		else
			A.x = 1;
    }

	U = A.Cross ( Plane.m_vecNormal );
    V = U.Cross ( Plane.m_vecNormal );
	U.Normalize ( );
	V.Normalize ( );

	float fLength = ( Bounds.Max - CB ).Length ( );

	U *= fLength;
	V *= fLength;

	cVector3 P [ 4 ];

    P [ 0 ] = CP + U - V;
    P [ 1 ] = CP + U + V;
	P [ 2 ] = CP - U + V;
	P [ 3 ] = CP - U - V;

    if ( AddVertices ( 4 ) < 0 )
		return false;
	
	for ( int iVertex = 0; iVertex < 4; iVertex++ )
    {
        m_pVertices [ iVertex ]        = cVertex ( P [ iVertex ] );
        m_pVertices [ iVertex ].Normal = Plane.m_vecNormal;
    }

    return true;
}

void cPolygon::ReleaseVertices ( void )
{
    if ( m_pVertices )
	{
		delete [ ] m_pVertices;
		m_pVertices = NULL;
	}

    m_dwVertexCount = 0;
}

HRESULT cPolygon::Split ( cPlane3& Plane, cPolygon* pFrontSplit, cPolygon* pBackSplit )
{
    cVector3        vecIntersectPoint;
    cVertex*		pFrontList     = NULL;
	cVertex*		pBackList      = NULL;
	int				iCurrentVertex = 0;
	int				iLocation      = 0;
	int				iVertex         = 0;
    int				iInFront       = 0;
	int				iBehind        = 0;
	int				iOnPlane       = 0;
    int				iFrontCounter  = 0;
	int				iBackCounter = 0;
    int*			pPointLocation = NULL;
    float           fPercent;

    if ( !pFrontSplit && !pBackSplit )
		return S_OK;

    try 
    {
        pPointLocation = new int [ m_dwVertexCount ];

        if ( !pPointLocation )
			throw std::bad_alloc ( );
        
        if ( pFrontSplit )
        {
            pFrontList = new cVertex [ m_dwVertexCount + 1 ];

            if ( !pFrontList )
				throw std::bad_alloc ( );
        }

        if ( pBackSplit )
        {
            pBackList = new cVertex [ m_dwVertexCount + 1 ];
            
			if ( !pBackList )
				throw std::bad_alloc ( );
        }
	}
	catch ( ... )
    {
        if ( pFrontList )
			delete [ ] pFrontList;

        if ( pBackList )
			delete [ ] pBackList;

        if ( pPointLocation )
			delete [ ] pPointLocation;

        return S_FALSE;
    }

    for ( iVertex = 0; iVertex < (int)m_dwVertexCount; iVertex++ )
    {
        iLocation = Plane.ClassifyPoint ( m_pVertices [ iVertex ] );

		if ( iLocation == CP_FRONT )
		{
			iInFront++;
		}
		else if ( iLocation == CP_BACK )
		{
			iBehind++;
		}
		else
		{
			iOnPlane++;
		}
        
        pPointLocation [ iVertex ] = iLocation;
	}

    if ( !iInFront && pBackList )
    {
		memcpy ( pBackList, m_pVertices, m_dwVertexCount * sizeof ( cVertex ) );
		iBackCounter = m_dwVertexCount;
    }

    if ( !iBehind && pFrontList )
    {
		memcpy ( pFrontList, m_pVertices, m_dwVertexCount * sizeof ( cVertex ) );
		iFrontCounter = m_dwVertexCount;
    }

    if ( iInFront && iBehind )
    {
		for ( iVertex = 0; iVertex < (int)m_dwVertexCount; iVertex++ )
        {
			iCurrentVertex = ( iVertex + 1 ) % m_dwVertexCount;

			if ( pPointLocation [ iVertex ] == CP_ONPLANE ) 
            {
                if ( pFrontList )
					pFrontList [ iFrontCounter++ ] = m_pVertices [ iVertex ];

				if ( pBackList )
					pBackList [ iBackCounter++ ] = m_pVertices [ iVertex ];
				
				continue;
            }

			if ( pPointLocation [ iVertex ] == CP_FRONT ) 
            {
				if ( pFrontList )
					pFrontList [ iFrontCounter++ ] = m_pVertices [ iVertex ];
			} 
            else 
            {
				if ( pBackList )
					pBackList [ iBackCounter++ ] = m_pVertices [ iVertex ];
			}
			
			if ( pPointLocation [ iCurrentVertex ] == CP_ONPLANE || pPointLocation [ iCurrentVertex ] == pPointLocation [ iVertex ] )
				continue;
			
			Plane.GetRayIntersect ( m_pVertices [ iVertex ], m_pVertices [ iCurrentVertex ], vecIntersectPoint, &fPercent );

			cVertex NewVert = cVertex ( vecIntersectPoint );
			
            cVector3 Delta;
            Delta.x    = m_pVertices [ iCurrentVertex ].tu - m_pVertices [ iVertex ].tu;
			Delta.y    = m_pVertices [ iCurrentVertex ].tv - m_pVertices [ iVertex ].tv;
			NewVert.tu = m_pVertices [ iVertex        ].tu + ( Delta.x * fPercent );
			NewVert.tv = m_pVertices [ iVertex        ].tv + ( Delta.y * fPercent );

            Delta          = m_pVertices [ iCurrentVertex ].Normal - m_pVertices [ iVertex ].Normal;
            NewVert.Normal = m_pVertices [ iVertex        ].Normal + ( Delta * fPercent );
            NewVert.Normal.Normalize ( );

			if ( pBackList )
				pBackList [ iBackCounter++ ] = NewVert;

			if ( pFrontList )
				pFrontList [ iFrontCounter++ ] = NewVert;
		}
	}

    if ( iFrontCounter && pFrontSplit )
    {
        pFrontSplit->AddVertices ( iFrontCounter );

        memcpy ( pFrontSplit->m_pVertices, pFrontList, iFrontCounter * sizeof ( cVertex ) );
	}

    if ( iBackCounter && pBackSplit )
    {
        pBackSplit->AddVertices ( iBackCounter );

        memcpy ( pBackSplit->m_pVertices, pBackList, iBackCounter * sizeof ( cVertex ) );
	}

    if ( pFrontList )
		delete [ ] pFrontList;

    if ( pBackList )
		delete [ ] pBackList;

    if ( pPointLocation )
		delete [ ] pPointLocation;

    return S_OK;
}

bool cPolygon::IsConvex ( void )
{
    int		iFlag         = 0,
			iCurrentVertex = 0;
    float   fDot		  = 0.0f;

    if ( !m_pVertices || m_dwVertexCount < 3 )
		return false;

    cVector3	vecPreviousEdge,
				vecCurrentEdge,
				vecCrossProduct,
				vecNormal = CalculateNormal ( );

    vecPreviousEdge = ( m_pVertices [ 0 ] - m_pVertices [ m_dwVertexCount - 1 ] );
    vecPreviousEdge.Normalize ( );

    for ( int iVertex = 0; iVertex < (int)m_dwVertexCount; iVertex++ )
    {
        iCurrentVertex = ( iVertex + 1 ) % m_dwVertexCount;
        vecCurrentEdge = ( m_pVertices [ iCurrentVertex ] - m_pVertices [ iVertex ] );
        vecCurrentEdge.Normalize ( );
        
        vecCrossProduct = vecCurrentEdge.Cross ( vecPreviousEdge );
        
        fDot = vecCrossProduct.Dot ( vecNormal );
        
		if ( fDot < -1e-5f )
		{
			iFlag |= 1;
		}
        else if ( fDot > +1e-5f )
		{
			iFlag |= 2;
		}
        
        if ( iFlag == 3 )
			return false;
        
        vecPreviousEdge = vecCurrentEdge;
	}

    return true;
}

cVector3 cPolygon::CalculateNormal ( void )
{
	cVector3    vecEdge1,
				vecEdge2,
				vecBestEdge1,
				vecBestEdge2;

    float       fBestAngle = 99999.0f,
				fTempAngle = 0.0f;
    
    for ( int iVertex = 0; iVertex < (int)m_dwVertexCount - 2; iVertex++ )
	{
	    vecEdge1 = ( m_pVertices [ iVertex + 1 ] - m_pVertices [ 0 ] ).Normalize ( );
        vecEdge2 = ( m_pVertices [ iVertex + 2 ] - m_pVertices [ 0 ] ).Normalize ( );
		
        fTempAngle = fabsf ( vecEdge1.Dot ( vecEdge2 ) );
			
        if ( fTempAngle < fBestAngle )
		{
			fBestAngle   = fTempAngle;
			vecBestEdge1 = vecEdge1;
			vecBestEdge2 = vecEdge2;
		}
	}

    cVector3 vecNormal = vecBestEdge1.Cross ( vecBestEdge2 );
    vecNormal.Normalize ( );

    return vecNormal;
}

bool cPolygon::ContainsDegenerate ( void )
{
    cVector3	vecPreviousEdge,
				vecCurrentEdge;

    int			iCurrentVertex = 0;
    float       fDot;

    if ( !m_pVertices || m_dwVertexCount < 3 )
		return false;

    vecPreviousEdge = m_pVertices [ 0 ] - m_pVertices [ m_dwVertexCount - 1 ];
    vecPreviousEdge.Normalize ( );

    for ( int iVertex = 0; iVertex < (int)m_dwVertexCount; iVertex++ ) 
    {
        iCurrentVertex = ( iVertex + 1 ) % m_dwVertexCount;
        vecCurrentEdge = m_pVertices [ iCurrentVertex ] - m_pVertices [ iVertex ];
        vecCurrentEdge.Normalize ( );
        
        fDot = vecCurrentEdge.Dot ( vecPreviousEdge );

        if ( fabsf ( fDot ) > 0.999999f )
			return true;
        
        vecPreviousEdge = vecCurrentEdge;
	}

    return false;
}

cFace::cFace ( )
{
	m_iMaterialIndex = -1;
    m_iTextureIndex  = -1;
    m_iFlags         =  0;
    m_iSrcBlendMode  =  0;
    m_iDestBlendMode =  0;
}

HRESULT cFace::Split ( cPlane3& Plane, cFace* pFrontSplit, cFace* pBackSplit )
{
    HRESULT ErrCode = cPolygon::Split ( Plane, pFrontSplit, pBackSplit );

    if ( FAILED ( ErrCode ) )
		return ErrCode;

    if ( pFrontSplit )
    {
	    pFrontSplit->m_vecNormal      = m_vecNormal;
        pFrontSplit->m_iMaterialIndex = m_iMaterialIndex;
        pFrontSplit->m_iTextureIndex  = m_iTextureIndex;
        pFrontSplit->m_iFlags         = m_iFlags;
        pFrontSplit->m_iSrcBlendMode  = m_iSrcBlendMode;
        pFrontSplit->m_iDestBlendMode = m_iDestBlendMode;
    }

    if ( pBackSplit )
    {
        pBackSplit->m_vecNormal       = m_vecNormal;
        pBackSplit->m_iMaterialIndex  = m_iMaterialIndex;
        pBackSplit->m_iTextureIndex   = m_iTextureIndex;
        pBackSplit->m_iFlags          = m_iFlags;
        pBackSplit->m_iSrcBlendMode   = m_iSrcBlendMode;
        pBackSplit->m_iDestBlendMode  = m_iDestBlendMode;
    }

    return S_OK;
}

bool cFace::GenerateFromPlane ( cPlane3& Plane, cBounds3& Bounds )
{
    if ( ! cPolygon::GenerateFromPlane ( Plane, Bounds ) )
		return false;

    m_vecNormal = Plane.m_vecNormal;

    return true;
}

float cFace::CalculateArea ( void )
{
    float fArea = 0.0f;

    for ( int iVertex = 1; iVertex < (int)m_dwVertexCount - 1; iVertex++ )
	{
        fArea += 0.5f * ( ( m_pVertices [ iVertex ] - m_pVertices [ 0 ] ).Cross ( m_pVertices [ iVertex + 1 ] - m_pVertices [ 0 ] ) ).Dot ( m_vecNormal );
    }

    return fabsf ( fArea );
}

cMaterial::cMaterial ( )
{
    Name            = NULL;
    Power           = 0.0f;
}

cMaterial::~cMaterial ( )
{
	if ( Name )
		delete [ ] Name;

    Name = NULL;
}

cTexture::cTexture ( )
{
    TextureSource   = 0;
    Name            = NULL;
    TextureFormat   = 0;
    TextureSize     = 0;
    TextureData     = NULL;
}

cTexture::cTexture ( ULONG Size )
{
    TextureSource   = 0;
    Name            = NULL;
    TextureFormat   = 0;
    TextureSize     = 0;
    TextureData     = NULL;

    AllocateData ( Size );
}

cTexture::~cTexture ( )
{
	if ( Name )
		delete [ ] Name;

    if ( TextureData )
		delete [ ] TextureData;

    TextureData = NULL;
    Name        = NULL;
}

bool cTexture::AllocateData ( ULONG Size )
{
    if ( TextureData )
		delete [ ] TextureData;

    TextureSize = (USHORT) Size;
    TextureData = new UCHAR [ Size ];
    
    return ( TextureData != NULL );
}
