#include "cquadtreesc.h"

float*			CQuadTreeNode::m_pError;
CQuadTreeNode*	CQuadTreeNode::m_aQuadPool;
UINT32			CQuadTreeNode::m_nNextQuad   = 0;
UINT8			CQuadTreeNode::isInitialized = FALSE;
unsigned char*	CQuadTreeNode::m_pData;

int				CQuadTreeNode::m_iCount = 0;
tagVerts CQuadTreeNode::m_Verts [ 20 ];

LPDIRECT3DDEVICE9 CQuadTreeNode::m_pD3D;

typedef float					  ( *CAMERA_GetFloatPFN   ) ( int );
extern CAMERA_GetFloatPFN			g_Camera_GetXPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetYPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetZPosition;
extern int							g_Camera_ID;

CQuadTreeNode::CQuadTreeNode ( )
{
	//#define MAP_SIZE  ( 1025 )
	//#define POOL_SIZE ( 4 * 1000 )
	//#define MIN_RESOLUTION (2.1f)
	//#define MULT_SCALE (0.25f)

	

	m_fXDivide = 64.0f;
	m_fYDivide = 64.0f;

	if ( isInitialized == TRUE )
		return;

	isInitialized = TRUE;

	if ( !m_aQuadPool )
		m_aQuadPool = new CQuadTreeNode [ POOL_SIZE ];

	if ( !m_pError )
		m_pError = new float [ MAP_SIZE * MAP_SIZE ];

	Children = NULL;
	m_aQuadPool->Children = NULL;
}

CQuadTreeNode::~CQuadTreeNode ( )
{
	if ( isInitialized == FALSE )
		return;

	isInitialized = FALSE;

	if ( m_aQuadPool )
	{
		delete [ ] m_aQuadPool;
		m_aQuadPool = NULL;
	}

	if ( m_pError )
	{
		delete m_pError;
		m_pError = NULL;
	}
}

UINT32 CQuadTreeNode::HeightField ( UINT32 y, UINT32 x )
{
	unsigned int iX = x % MAP_SIZE;
	unsigned int iY = y % MAP_SIZE;

	return m_pData [ iX + ( iY * MAP_SIZE ) ];
}

float CQuadTreeNode::CalculateRoughness ( UINT32 NW_X, UINT32 NW_Y, UINT32 nQuadWidth )
{
	#define MIN_WIDTH (2)
	#define SQR(x) ((x) * (x))
	#define VALID(x) if (x > SQR(MAP_SIZE)) x = 0;

	memset ( m_pError, 0, sizeof ( float ) * MAP_SIZE * MAP_SIZE );
	
	for ( nQuadWidth = MIN_WIDTH; nQuadWidth <= MAP_SIZE; nQuadWidth *= 2 )
	{
		UINT32 nRadius = nQuadWidth / 2;
		
		for ( UINT32 NW_X = 0; NW_X < MAP_SIZE; NW_X += nQuadWidth )
		{
			for ( UINT32 NW_Y = 0; NW_Y < MAP_SIZE; NW_Y += nQuadWidth )
			{
				UINT32 nNode = ( NW_X + nRadius ) + ( ( NW_Y + nRadius ) * MAP_SIZE );

				VALID ( nNode );

				if ( nNode == 0 )
					continue;

				// calculate the heights of this node's corners
				UINT32 nNWCorner = HeightField ( NW_Y,				NW_X              );
				UINT32 nNECorner = HeightField ( NW_Y,				NW_X + nQuadWidth );
				UINT32 nSWCorner = HeightField ( NW_Y + nQuadWidth,	NW_X              );
				UINT32 nSECorner = HeightField ( NW_Y + nQuadWidth,	NW_X + nQuadWidth );
				UINT32 nCenter   = HeightField ( NW_Y + nRadius,	NW_X + nRadius    );

				// find the error for this node's edges and center
				UINT32 nErrNorth = (UINT32) abs ( (float) HeightField ( NW_Y,				NW_X + nRadius	  ) - ( ( nNWCorner + nNECorner ) >> 1 ) );
				UINT32 nErrSouth = (UINT32) abs ( (float) HeightField ( NW_Y + nQuadWidth,	NW_X + nRadius	  ) - ( ( nSWCorner + nSECorner ) >> 1 ) );
				UINT32 nErrEast  = (UINT32) abs ( (float) HeightField ( NW_Y + nRadius,		NW_X + nQuadWidth ) - ( ( nNECorner + nSECorner ) >> 1 ) );
				UINT32 nErrWest  = (UINT32) abs ( (float) HeightField ( NW_Y + nRadius,		NW_X			  ) - ( ( nNWCorner + nSWCorner ) >> 1 ) );

				UINT32 nErrCtr1	 = (UINT32) abs ( (float) nCenter - ( ( nSECorner + nNECorner ) >> 1 ) );
				UINT32 nErrCtr2	 = (UINT32) abs ( (float) nCenter - ( ( nNWCorner + nSWCorner ) >> 1 ) );

				// find the largest error of this node...
				float fMaxError = ( float ) nErrNorth;

				if ( nErrSouth > fMaxError )
					fMaxError = ( float ) nErrSouth;

				if ( nErrEast  > fMaxError ) 
					fMaxError = ( float ) nErrEast;

				if ( nErrWest  > fMaxError ) 
					fMaxError = ( float ) nErrWest;

				if ( nErrCtr1  > fMaxError ) 
					fMaxError = ( float ) nErrCtr1;
				
				if ( nErrCtr2  > fMaxError ) 
					fMaxError = ( float ) nErrCtr2;

				if ( nRadius > 1 )
				{
					float fErr;
					INT32 nRad      = 2 * nRadius;
					float fMaxLower = 0;

					for ( INT32 x= -nRad; x <= ( int ) ( 2 * nQuadWidth ); x += nRadius )
					{
						for ( INT32 y= -nRad; y <= ( int ) ( 2 * nQuadWidth ); y += nRadius )
						{
							UINT32 nTempNode = ( ( NW_X + x ) + ( nRadius / 2 ) ) + ( ( ( NW_Y + y ) + ( nRadius / 2 ) ) * MAP_SIZE );

							VALID ( nTempNode );

							if ( nTempNode == 0 )
								continue;

							fErr = m_pError [ nTempNode ];

							if ( fErr > fMaxLower )
								fMaxLower = fErr;
						}
					}

					fMaxLower *= LEVEL_MULT;
					
					if ( fMaxLower > fMaxError ) 
						fMaxError = fMaxLower;
				}

				m_pError [ nNode ] = fMaxError;
			}
		}
	}

	return 0;
}

void CQuadTreeNode::Split ( UINT32 NW_X, UINT32 NW_Y, UINT32 radius )
{
	if ( m_nNextQuad < POOL_SIZE )
	{
		// create all four children when splitting
		Children = &( ( CQuadTreeNode* ) m_aQuadPool ) [ m_nNextQuad ];
		m_nNextQuad += 4;
	}
}

void CQuadTreeNode::RecursTessellate ( UINT32 NW_X, UINT32 NW_Y, UINT32 nQuadWidth )
{
	Children = NULL;

	if ( SplitMetric ( NW_X, NW_Y, nQuadWidth ) )
	{
		UINT32 radius = nQuadWidth >> 1;
		Split ( NW_X, NW_Y, radius );

		if ( Children )
		{
			Child ( ChildNW ).RecursTessellate ( NW_X,			NW_Y,			radius );
			Child ( ChildNE ).RecursTessellate ( NW_X + radius,	NW_Y,			radius );
			Child ( ChildSW ).RecursTessellate ( NW_X,			NW_Y + radius,	radius );
			Child ( ChildSE ).RecursTessellate ( NW_X + radius,	NW_Y + radius,	radius );
		}
	}
}

BOOL CQuadTreeNode::SplitMetric ( UINT32 NW_X, UINT32 NW_Y, UINT32 nQuadWidth )
{
	#define MIN_QUAD_WIDTH (3)

	int radius = nQuadWidth >> 1;
	UINT32 nNode = NW_X + radius + ( ( NW_Y + radius ) * MAP_SIZE );
	VALID ( nNode );

	float fCamX = g_Camera_GetXPosition ( g_Camera_ID );
	float fCamY = g_Camera_GetYPosition ( g_Camera_ID );
	float fCamZ = g_Camera_GetZPosition ( g_Camera_ID );

	
	float A = ( float ) fabs ( ( float ) ( NW_X + radius ) - fCamX );
	float B = ( float ) fabs ( ( float ) ( NW_Y + radius ) - (MAP_SIZE-fCamZ) );
	float C = ( float ) fabs ( ( ( float ) HeightField ( NW_Y + radius, NW_X + radius ) * MULT_SCALE ) - fCamY );

	float distance = MAX ( A, MAX ( B, C ) );

	// seems to mess up when get VERY close to the geo-
	if(distance<MAP_SIZE) distance=(float)MAP_SIZE;
	
	float	D2,
			desiredRes = 0.15f;
	
	D2 = m_pError [ nNode ];

	float fSplit = ( distance ) / ( nQuadWidth * MAX ( desiredRes * D2, MIN_RESOLUTION ) );

	m_fBlend = 1.319f * ( 1.75f - fSplit );

	if ( m_fBlend < 0 )
		m_fBlend = 0;

	if ( m_fBlend > 1 )
		m_fBlend = 1;
	
	if ( fSplit < 1 && nQuadWidth >= MIN_QUAD_WIDTH )
	{
		return TRUE;
	}

	return FALSE;
}

void CQuadTreeNode::RecursRender ( UINT32 NW_X, UINT32 NW_Y, UINT32 nQuadWidth, CQuadTreeNode *North, CQuadTreeNode *South, CQuadTreeNode *East, CQuadTreeNode *West )
{
	int nRadius = nQuadWidth >> 1;

	if ( Children )
	{
		Child ( ChildNW ).RecursRender 
										( 
											NW_X,
											NW_Y,
											nRadius,
											( North ) ? ( North->GetChild ( ChildSW ) ) : ( NULL ),
											&Child ( ChildSW ), 
											&Child ( ChildNE ),
											( West ) ? ( West->GetChild ( ChildNE ) ) : NULL 
										);
		
		Child ( ChildNE ).RecursRender ( 
											NW_X + nRadius,	
											NW_Y,
											nRadius,
											( North ) ? ( North->GetChild ( ChildSE ) ) : ( NULL ),
											&Child ( ChildSE ),
											( East ) ? ( East->GetChild ( ChildNW ) ) : NULL,
											&Child ( ChildNW )
										);

		Child ( ChildSW ).RecursRender ( 
											NW_X,
											NW_Y + nRadius,
											nRadius,
											&Child ( ChildNW ),
											( South ) ? ( South->GetChild ( ChildNW ) ): ( NULL ),
											&Child ( ChildSE ),
											( West ) ? ( West->GetChild ( ChildSE ) ) : NULL
										);

		Child ( ChildSE ).RecursRender ( 
											NW_X + nRadius,
											NW_Y + nRadius,
											nRadius,
											&Child ( ChildNE ),
											( South ) ? ( South->GetChild ( ChildNE ) ) : ( NULL ),
											( East  ) ? ( East->GetChild  ( ChildSW ) ) : NULL, 
											&Child(ChildSW) );
	}
	else
	{
		UINT8 nDepth = ( int ) ( logf ( ( float ) nQuadWidth ) / logf ( 2.0 ) );
		
		// these height samples are used several times, so only hit memory for them once...
		UINT32 nNorthWest = HeightField (   NW_Y,					NW_X				);
		UINT32 nNorthEast = HeightField (   NW_Y,					NW_X + nQuadWidth	);
		UINT32 nSouthWest = HeightField ( ( NW_Y + nQuadWidth ),	NW_X				);
		UINT32 nSouthEast = HeightField ( ( NW_Y + nQuadWidth ),	NW_X + nQuadWidth	);

		// center point
		bool isEast  = ( NW_X / nQuadWidth ) & 0x01;
		bool isSouth = ( NW_Y / nQuadWidth ) & 0x01;

		if ( ( isEast && isSouth ) || ( !isEast && !isSouth ) )
		{
			MakeBlendedVertex
							(	
								NW_X + nRadius, NW_Y + nRadius,
								( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
								( NW_X + ( nRadius ) ) ),
								( nNorthWest + nSouthEast ) / 2.0f,
								m_fBlend,
								nDepth
							);
		}
		else
		{
			MakeBlendedVertex
							(	
								NW_X + nRadius, NW_Y + nRadius,
								( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
								( NW_X + ( nRadius ) ) ),
								( nNorthEast + nSouthWest ) / 2.0f,
								m_fBlend,
								nDepth
							);
		}

		MakeVertex ( NW_X + 0, NW_Y + 0, ( float ) nNorthWest, nDepth );

		if ( West )
		{
			if ( West->GetChild ( ChildNE ) != 0 || West->GetChild ( ChildSE ) != 0 )
			{
				MakeVertex
						(	
							NW_X + 0, NW_Y + nRadius,
							( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
							( NW_X + ( 0 ) ) ),
							nDepth
						);
			}
			else
				MakeBlendedVertex
						(	
							NW_X + 0, NW_Y + nRadius,
							( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
							( NW_X + ( 0 ) ) ),
							( nNorthWest + nSouthWest ) / 2.0f,
							MIN ( m_fBlend, West->m_fBlend ),
							nDepth
						);
		}

		MakeVertex ( NW_X + 0, NW_Y + nQuadWidth, ( float ) nSouthWest, nDepth );

		if ( South )
		{
			if ( South->GetChild ( ChildNE ) != 0 || South->GetChild ( ChildNW ) != 0 )
			{
				MakeVertex
						(	
							NW_X + nRadius,
							NW_Y + nQuadWidth,
							( float ) HeightField ( ( ( NW_Y + ( nQuadWidth ) ) ),
							( NW_X + ( nRadius ) ) ),
							nDepth
						);
			}
			else
				MakeBlendedVertex
						(	
							NW_X + nRadius,
							NW_Y + nQuadWidth,
							( float ) HeightField ( ( ( NW_Y + ( nQuadWidth ) ) ),
							( NW_X + ( nRadius ) ) ),
							( nSouthWest + nSouthEast ) / 2.0f,
							MIN ( m_fBlend, South->m_fBlend ),
							nDepth
						);
		}

		MakeVertex ( NW_X + nQuadWidth, NW_Y + nQuadWidth, ( float ) nSouthEast, nDepth );

		if ( East )
		{
			if ( East->GetChild ( ChildNW ) != 0 || East->GetChild ( ChildSW ) != 0 )
			{
				MakeVertex
						(	
							NW_X + nQuadWidth, NW_Y + nRadius,
							( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
							( NW_X + ( nQuadWidth ) ) ),
							nDepth
						);
			}
			else
				MakeBlendedVertex
						(	
							NW_X + nQuadWidth, NW_Y + nRadius,
							( float ) HeightField ( ( ( NW_Y + ( nRadius ) ) ),
							( NW_X + ( nQuadWidth ) ) ),
							( nNorthEast + nSouthEast ) / 2.0f,
							MIN ( m_fBlend, East->m_fBlend ),
							nDepth
						);
		}

		MakeVertex ( NW_X + nQuadWidth, NW_Y + 0, ( float ) nNorthEast, nDepth );

		if ( North )
		{
			if ( North->GetChild ( ChildSE ) != 0 || North->GetChild ( ChildSW ) != 0 )
			{
				MakeVertex
						(	
							NW_X + nRadius, NW_Y + 0,
							( float ) HeightField ( ( ( NW_Y + ( 0 ) ) ),
							( NW_X + ( nRadius ) ) ),
							nDepth 
						);
			}
			else
				MakeBlendedVertex
						(	
							NW_X + nRadius, NW_Y + 0,
							( float ) HeightField ( ( ( NW_Y + ( 0 ) ) ),
							( NW_X + ( nRadius ) ) ),
							( nNorthEast + nNorthWest ) / 2.0f,
							MIN ( m_fBlend, North->m_fBlend ),
							nDepth
						);
		}

		MakeVertex ( NW_X + 0, NW_Y + 0, ( float ) nNorthWest, nDepth );


		// need to setup as vertex buffer, also could add hidden surface removal by using
		// a nodetree to store the vertices
		m_pD3D->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, m_iCount - 2, &m_Verts, 36 );

			m_iCount = 0;
	}
}

inline void CQuadTreeNode::MakeBlendedVertex ( UINT32 x, UINT32 y, float heightOne, float heightTwo, float fBlend, UINT8 nDepth )
{
	float height = ( ( 1.0f - fBlend ) * heightTwo ) + ( ( fBlend ) * heightOne );

	MakeVertex ( x, y, height, nDepth );
}

#define NUM_LAND_COLORS (16)
#define NUM_COLORS ( NUM_LAND_COLORS )

int aQuadColors [ NUM_COLORS ] [ 3 ] = 
{
	{  72,  68, 154 }, // Dk Blue
	{  84, 125, 228 }, // Med Blue
	{  91, 180, 237 }, // Lt Blue
	{ 120, 202, 172 }, // Aqua
	{ 248, 205, 136 }, // Lt Sand
	{ 223, 130,  47 }, // Dk Sand
	{ 237, 149,  91 }, // Med Sand
	{ 156, 193,   9 }, // Med Green
	{ 180, 213,  47 }, // Green
	{ 163, 176, 112 }, // Dk Grn
	{ 194, 161, 118 }, // Lt Brown
	{ 165, 120,  71 }, // Dk Brown
	{ 183, 154, 125 }, // Med Brown
	{ 195, 189, 181 }, // Med Gray 
	{ 235, 232, 227 }, // Lt Gray
	{ 255, 255, 255 }  // White

};

inline void CQuadTreeNode::MakeVertex ( UINT32 x, UINT32 y, float height, UINT8 nDepth )
{
	float fBaseShade             = 90.0f;
	float fColor                 = ( fBaseShade + height ) / ( fBaseShade + 256.0f );

	m_Verts [ m_iCount ].x       = ( float ) x;
	m_Verts [ m_iCount ].y       = ( float) y;
	m_Verts [ m_iCount ].z       = height;

	m_Verts [ m_iCount ].nx		 =  0.0f;
	m_Verts [ m_iCount ].ny		 =  0.0f;
	m_Verts [ m_iCount ].nz		 =  1.0f;

	float fNewColor = fColor * 255;

	m_Verts [ m_iCount ].diffuse = D3DCOLOR_XRGB ( 128+( int ) fNewColor/2, 128+( int ) fNewColor/2, 128+( int ) fNewColor/2 );

	m_Verts [ m_iCount ].tu = ( x / m_fXDivide );
	m_Verts [ m_iCount ].tv = ( y / m_fYDivide );

	m_iCount++;
}
