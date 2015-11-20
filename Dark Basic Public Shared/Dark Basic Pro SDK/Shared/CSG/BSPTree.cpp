#include "BSPTree.h"



CBSPTree::CBSPTree ( )
{
	FatalError		= FALSE;
	BrushBase		= NULL;
	Polygons		= NULL;
	Planes			= NULL;
	Nodes			= NULL;
	Leaves			= NULL;
	PolygonDeleted	= NULL;
	RootNode		= -1;
	NodeCount		= 0;
	PolyCount		= 0;
	PlaneCount		= 0;
	LeafCount		= 0;
}

CBSPTree::~CBSPTree ( )
{
	KillTree  ( );
	KillPolys ( );
}

void CBSPTree::KillTree ( void )
{
	if ( Leaves )
	{ 
		free ( Leaves );
		Leaves = NULL;
	}

	if ( Planes )
	{ 
		free ( Planes );
		Planes = NULL;
	}

	if ( Nodes )
	{ 
		free ( Nodes );	
		Nodes = NULL; }
	
	FatalError	= FALSE;
	BrushBase	= NULL;
	RootNode	= -1;
	NodeCount	= 0;
	PlaneCount	= 0;
	LeafCount	= 0;
}

void CBSPTree::KillPolys ( void )
{
	if ( Polygons ) 
	{ 
		for ( int i = 0; i < PolyCount; i++ )
		{
			if ( Polygons [ i ].Vertices )
			{ 
				free ( Polygons [ i ].Vertices );
				Polygons [ i ].Vertices = NULL;
			}

			if ( Polygons [ i ].Indices )
			{ 
				free ( Polygons [ i ].Indices );
				Polygons [ i ].Indices  = NULL;
			}
		}
		
		free ( Polygons );
		Polygons	= NULL;
	}
	
	if ( PolygonDeleted )
	{ 
		free ( PolygonDeleted );
		PolygonDeleted = NULL;
	}

	PolyCount = 0;
}

void CBSPTree::InitPolygons ( BRUSH* brush, long BrushCount, BOOL BSPCollect )
{
	KillTree ( );
	
	BrushBase = brush;
	
	PolygonList = NULL;
	
	BSPPOLYGON *Child = NULL;

	if ( !BSPCollect )
	{
		for ( ULONG f = 0; f < brush->FaceCount; f++ )
		{
			Child = AddPolygon ( Child, &brush->Faces [ f ] );
		}
	}
	else 
	{
		for ( int b = 0; b < BrushCount; b++ )
		{
			CBSPTree* MiniBSP = ( CBSPTree* ) brush [ b ].BSPTree;

			if ( MiniBSP == NULL )
				continue;

			for ( int f = 0; f < MiniBSP->PolyCount; f++ )
			{
				if  ( !MiniBSP->PolygonDeleted [ f ] )
				{
					if ( MiniBSP->Polygons [ f ].VertexCount < 3 )
						continue;

					Child = AddBSPPolygon ( Child, &MiniBSP->Polygons [ f ] );
				}
			}
		}
	}

	KillPolys ( );
	
	RootNode = AllocAddNode ( );

	BuildBSPTree ( RootNode, PolygonList );

	IsDirty = FALSE;
}

BSPPOLYGON* CBSPTree::AddBSPPolygon ( BSPPOLYGON* Parent, BSPPOLYGON* Poly )
{
	BSPPOLYGON* Child		= new BSPPOLYGON;
	Child->IndexCount		= Poly->IndexCount;
	Child->VertexCount		= Poly->VertexCount;
	Child->Normal			= Poly->Normal;
	Child->TextureIndex		= Poly->TextureIndex;
	Child->UsedAsSplitter	= 0;
	Child->NextPoly			= NULL;

	Child->Vertices			= ( D3DVERTEX* ) malloc ( Poly->VertexCount * sizeof ( D3DVERTEX ) );
	Child->Indices			= ( WORD*      ) malloc ( Poly->IndexCount  * sizeof ( WORD      ) );

	memcpy ( &Child->Vertices [ 0 ], &Poly->Vertices [ 0 ], Poly->VertexCount * sizeof ( D3DVERTEX ) );
	memcpy ( &Child->Indices  [ 0 ], &Poly->Indices  [ 0 ], Poly->IndexCount  * sizeof ( WORD      ) );
	
	if ( Parent != NULL )
		Parent->NextPoly = Child;
	
	if ( PolygonList == NULL )
		PolygonList	= Child;
	return Child;
}

BSPPOLYGON* CBSPTree::AddPolygon ( BSPPOLYGON* Parent, POLYGON* Face )
{
	BSPPOLYGON* Child		= new BSPPOLYGON;
	Child->NextPoly			= NULL;
	Child->UsedAsSplitter	= 0;

	Child->VertexCount		= Face->VertexCount;
	Child->IndexCount		= Face->IndexCount;
	Child->Normal			= Face->Normal;
	Child->TextureIndex		= Face->TextureIndex;
	
	Child->Vertices			= ( D3DVERTEX* ) malloc ( Child->VertexCount * sizeof ( D3DVERTEX ));
	Child->Indices			= ( WORD*      ) malloc ( Child->IndexCount  * sizeof ( WORD      ) );

	for ( int i = 0; i < Face->VertexCount; i++ )
	{
		Child->Vertices [ i ] = Face->Vertices [ i ];
		
		if ( g_bSafeTransform )
		{
			D3DXVec3TransformCoord ( 
										( D3DXVECTOR3* ) &Child->Vertices [ i ],
										( D3DXVECTOR3* ) &Child->Vertices [ i ],
										&BrushBase->Matrix
								   );
		}
	}

	for ( i = 0; i < Face->IndexCount; i++ )
		Child->Indices [ i ] = Face->Indices [ i ];

	if ( PolygonList == NULL )
		PolygonList = Child;

	if ( Parent!=NULL)
		Parent->NextPoly = Child;

	return Child;
}

BSPPOLYGON* CBSPTree::AddPolygonSplit ( BSPPOLYGON* Parent, POLYGON* Face )
{
    D3DXVECTOR3 PreviousNorm, CurrentNormal;
	WORD		IndexOn			= 0, IndexOn2 = 0;
	BOOL		PolyFolded		= FALSE;
	BSPPOLYGON	*pParent		= Parent, *RetVal = NULL;

    PreviousNorm = CalculatePolyNormal (
											Face->Vertices [ Face->Indices [ 0 ] ],
											Face->Vertices [ Face->Indices [ 1 ] ],
											Face->Vertices [ Face->Indices [ Face->IndexCount - 1 ] ],
											NULL
									   );

	for ( int lloop = 0; lloop < ( Face->IndexCount / 3 ); lloop++ )
	{
        CurrentNormal = CalculatePolyNormal (
												Face->Vertices [ Face->Indices [   lloop * 3       ] ], 
												Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 1 ] ], 
												Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 2 ] ],
												NULL
											);
		if ( !EpsilonCompareVector ( CurrentNormal, PreviousNorm ) )
		{ 
			PolyFolded = TRUE;
			break;
		}
    }

	if ( PolyFolded == FALSE )
	{
		BSPPOLYGON* Child		= new BSPPOLYGON;
		Child->NextPoly			= NULL;
		Child->UsedAsSplitter	= 0;

		Child->VertexCount		= Face->VertexCount;
		Child->IndexCount		= Face->IndexCount;
		Child->Normal			= Face->Normal;
		Child->TextureIndex		= Face->TextureIndex;		
		
		Child->Vertices			= ( D3DVERTEX* ) malloc ( Child->VertexCount * sizeof ( D3DVERTEX ) );
		Child->Indices			= ( WORD*      ) malloc ( Child->IndexCount  * sizeof ( WORD      ) );

		memcpy ( Child->Vertices, Face->Vertices, Child->VertexCount * sizeof ( D3DVERTEX ) );
		memcpy ( Child->Indices,  Face->Indices,  Child->IndexCount  * sizeof ( WORD      ) );
		
		if ( PolygonList == NULL )
			PolygonList = Child;

		if ( Parent != NULL )
			Parent->NextPoly = Child;

		return Child;
	}

	PreviousNorm = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );

    for ( lloop = 0;  lloop < ( Face->IndexCount / 3 ); lloop++ )
	{
        CurrentNormal = CalculatePolyNormal (
												Face->Vertices [ Face->Indices [   lloop * 3       ] ], 
												Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 1 ] ], 
												Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 2 ] ],
												NULL
											);

		if ( !EpsilonCompareVector ( CurrentNormal, PreviousNorm ) )
		{ 
            BSPPOLYGON* Child = new BSPPOLYGON;
			
			if ( PolygonList == NULL )
				PolygonList = Child;

            if ( pParent != NULL )
				pParent->NextPoly = Child;

			Child->NextPoly			= NULL;
			Child->UsedAsSplitter	= 0;

			Child->Normal			= CurrentNormal;
			Child->TextureIndex		= Face->TextureIndex;
		
			Child->VertexCount		= 3;
			Child->IndexCount		= 3;
			
			Child->Vertices			= ( D3DVERTEX* ) malloc ( Child->VertexCount * sizeof ( D3DVERTEX ) );
			Child->Indices			= ( WORD*      ) malloc ( Child->IndexCount  * sizeof ( WORD      ) );

            Child->Vertices [ 0 ] = Face->Vertices [ Face->Indices [ ( lloop * 3 )     ] ];
            Child->Vertices [ 1 ] = Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 1 ] ];
            Child->Vertices [ 2 ] = Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 2 ] ];

            Child->Indices [ 0 ] = 0;
            Child->Indices [ 1 ] = 1;
            Child->Indices [ 2 ] = 2;
			
			IndexOn  = 3; 
			IndexOn2 = 2;

			pParent = Child;

        }
		else
		{
            pParent->VertexCount++;
			pParent->Vertices				               = ( D3DVERTEX* ) realloc ( pParent->Vertices, pParent->VertexCount * sizeof ( D3DVERTEX ) );
			pParent->Vertices [ pParent->VertexCount - 1 ] = Face->Vertices [ Face->Indices [ ( lloop * 3 ) + 2 ] ];
            
            pParent->IndexCount+= 3;
            pParent->Indices				= ( WORD* ) realloc ( pParent->Indices, pParent->IndexCount * sizeof ( WORD ) );

            pParent->Indices [ IndexOn     ]	= 0;
            pParent->Indices [ IndexOn + 1 ]	= IndexOn2;
            pParent->Indices [ IndexOn + 2 ]	= IndexOn2 + 1;
            
			IndexOn2++;
			IndexOn+=3;
        }

		PreviousNorm = CurrentNormal;
    }
    
	return pParent;
}

void CBSPTree::BuildBSPTree ( long CurrentNode, BSPPOLYGON* PolyList )
{
	D3DXVECTOR3 a,
				b;
	float result;

	BSPPOLYGON* polyTest = NULL, *FrontList = NULL, *BackList = NULL;
	BSPPOLYGON* NextPolygon = NULL, *FrontSplit = NULL, *BackSplit = NULL;
	
	if ( FatalError == TRUE )
		return;

	Nodes [ CurrentNode ].Plane = SelectBestSplitter ( PolyList, CurrentNode );
	
	if ( FatalError == TRUE )
		return;

	polyTest = PolyList;

	while ( polyTest != NULL )
	{
		NextPolygon = polyTest->NextPoly;

		switch ( ClassifyPoly ( &Planes [ Nodes [ CurrentNode ].Plane ], polyTest ) )
		{
			case CP_ONPLANE:
			{
				a = Planes [ Nodes [ CurrentNode ].Plane ].Normal;
				b = polyTest->Normal;

				result = ( float ) fabs ( ( a.x - b.x ) + ( a.y - b.y ) + ( a.z - b.z ) );
				
				if ( result < 0.1f )
				{
					polyTest->NextPoly	= FrontList;
					FrontList			= polyTest;
				}
				else
				{
					polyTest->NextPoly	= BackList;
					BackList			= polyTest;	
				}
			}
			break;

			case CP_FRONT:
			{
				polyTest->NextPoly		= FrontList;
				FrontList				= polyTest;	
			}
			break;

			case CP_BACK:
			{
				polyTest->NextPoly		= BackList;
				BackList				= polyTest;
			}
			break;

			case CP_SPANNING:
			{
				FrontSplit = new BSPPOLYGON;
				BackSplit  = new BSPPOLYGON;
				ZeroMemory ( FrontSplit, sizeof ( BSPPOLYGON ) );
				ZeroMemory ( BackSplit,  sizeof ( BSPPOLYGON ) );
				
				SplitPolygon ( polyTest, &Planes [ Nodes [ CurrentNode ].Plane ], FrontSplit, BackSplit );
    
				FrontSplit->UsedAsSplitter = polyTest->UsedAsSplitter;
				BackSplit->UsedAsSplitter  = polyTest->UsedAsSplitter;
				
				free ( polyTest->Vertices );
				free ( polyTest->Indices );
				delete polyTest;
				polyTest = NULL;
	
				FrontSplit->NextPoly = FrontList;
				FrontList			 = FrontSplit;
				BackSplit->NextPoly	 = BackList;
				BackList			 = BackSplit;
			}
			break;

			default:
			break;
		}

		polyTest = NextPolygon;
	}
   
	int SplitterCount = 0;

	BSPPOLYGON* tempf = FrontList;
	
	while ( tempf != NULL )
	{
		if ( tempf->UsedAsSplitter == 0 ) 
			SplitterCount++;

		tempf = tempf->NextPoly;
	} 

	if ( SplitterCount == 0 )
	{
		BSPPOLYGON* Iterator = FrontList;
		BSPPOLYGON* Temp;
		
		AllocAddLeaf ( );

		Leaves [ LeafCount - 1 ].StartPoly = PolyCount;

		while ( Iterator != NULL )
		{
			Polygons [ AllocAddPoly ( ) ] = *Iterator;

			Temp     = Iterator;
			Iterator = Iterator->NextPoly;
			
			delete Temp;
		}

		Leaves [ LeafCount - 1 ].EndPoly = PolyCount;
		Nodes  [ CurrentNode   ].Front   = LeafCount - 1;
		Nodes  [ CurrentNode   ].IsLeaf  = 1;
	}
	else
	{
		Nodes [ CurrentNode ].IsLeaf = 0;
		Nodes [ CurrentNode ].Front  = AllocAddNode ( );

		BuildBSPTree ( NodeCount - 1, FrontList );  
	}

	if ( BackList == NULL )
	{
		Nodes [ CurrentNode ].Back = -1;
	}
	else 
	{
		Nodes [ CurrentNode ].Back = AllocAddNode ( );

		BuildBSPTree ( NodeCount - 1, BackList );
	}
}

int CBSPTree::AllocAddNode ( void )
{
	NodeCount++;
	
	Nodes = ( NODE* ) realloc ( Nodes, NodeCount * sizeof ( NODE ) );

	Nodes [ NodeCount - 1 ].IsLeaf	= 0;
	Nodes [ NodeCount - 1 ].Plane	= -1;
	Nodes [ NodeCount - 1 ].Front	= -1;
	Nodes [ NodeCount - 1 ].Back	= -1;

	return NodeCount - 1;
}

int CBSPTree::AllocAddLeaf ( void )
{
	LeafCount++;

	Leaves = ( LEAF* ) realloc ( Leaves, LeafCount * sizeof ( LEAF ) );

	Leaves [ LeafCount - 1 ].StartPoly	= -1;
	Leaves [ LeafCount - 1 ].EndPoly	= -1;

	return LeafCount - 1;
}

int CBSPTree::AllocAddPoly ( void )
{
	PolyCount++;

	Polygons		= ( BSPPOLYGON* ) realloc ( Polygons,       PolyCount * sizeof ( BSPPOLYGON ) );
	PolygonDeleted  = ( BOOL*       ) realloc ( PolygonDeleted, PolyCount * sizeof ( BOOL       ) );

	PolygonDeleted [ PolyCount - 1 ]			= FALSE;
	Polygons [ PolyCount - 1 ].IndexCount		= 0;
	Polygons [ PolyCount - 1 ].Indices			= NULL;
	Polygons [ PolyCount - 1 ].VertexCount		= 0;
	Polygons [ PolyCount - 1 ].Vertices			= NULL;
	Polygons [ PolyCount - 1 ].Normal			= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	Polygons [ PolyCount - 1 ].NextPoly			= NULL;
	Polygons [ PolyCount - 1 ].UsedAsSplitter	= 0;

	return PolyCount - 1;
}

int CBSPTree::AllocAddPlane ( void )
{
	PlaneCount++;

	Planes = ( PLANE* ) realloc ( Planes, PlaneCount * sizeof ( PLANE ) );

	Planes [ PlaneCount - 1 ].PointOnPlane	= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	Planes [ PlaneCount - 1 ].Normal		= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );

	return PlaneCount - 1;
}

long CBSPTree::SelectBestSplitter ( BSPPOLYGON* PolyList, long CurrentNode )
{
	BSPPOLYGON* Splitter = PolyList, *CurrentPoly = NULL, *SelectedPoly = NULL;
	ULONG   BestScore = 1000000;

	while ( Splitter != NULL )
	{
		if ( Splitter->UsedAsSplitter != 1 )
		{
			PLANE SplittersPlane;
			SplittersPlane.Normal		= Splitter->Normal;
			SplittersPlane.PointOnPlane	= *( D3DXVECTOR3* ) &Splitter->Vertices [ 0 ];
			
			CurrentPoly					= PolyList;

			ULONG score, splits, backfaces, frontfaces;
			score = splits = backfaces = frontfaces = 0;
			
			while ( CurrentPoly != NULL )
			{
				int result = ClassifyPoly ( &SplittersPlane, CurrentPoly );
				
				switch ( result )
				{
					case CP_ONPLANE:
					break;
				
					case CP_FRONT:
						frontfaces++;
					break;
					
					case CP_BACK:
						backfaces++;
					break;

					case CP_SPANNING:
						splits++;
					break;

					default:
						break;
				}

				CurrentPoly = CurrentPoly->NextPoly;
			}
			
			score = abs ( frontfaces - backfaces ) + ( splits * 3 );
			
			if ( score < BestScore)
			{
				BestScore	 = score;
				SelectedPoly = Splitter;
			}
		}
	
		Splitter = Splitter->NextPoly;
	}

	if ( SelectedPoly == NULL )
	{
		FatalError = TRUE;
		return -1;
	}

	SelectedPoly->UsedAsSplitter = 1;

	AllocAddPlane ( );

	Planes [ PlaneCount - 1 ].PointOnPlane = *( ( D3DVECTOR* )&SelectedPoly->Vertices [ 0 ] );
	Planes [ PlaneCount - 1 ].Normal	   = SelectedPoly->Normal;
	return ( PlaneCount - 1 );
}

int CBSPTree::ClassifyPoly ( PLANE* Plane, BSPPOLYGON* Poly )
{
	int			Infront = 0, Behind = 0, OnPlane=0;
	float		result  = 0;
	D3DXVECTOR3* vec1   = &Plane->PointOnPlane;

	for ( int a = 0; a < Poly->VertexCount; a++ )
	{
		D3DXVECTOR3* vec2      = ( D3DXVECTOR3* ) &Poly->Vertices [ a ];
		D3DXVECTOR3  Direction = ( *vec1 ) - ( *vec2 );
		
		result = D3DXVec3Dot ( &Direction, &Plane->Normal );

		if ( result > g_EPSILON )
		{
			Behind++;
		}
		else if ( result<-g_EPSILON )
		{
			Infront++;
		}
		else 
		{
			OnPlane++;
			Infront++;
			Behind++;
		}
	}

	if ( OnPlane == Poly->VertexCount )
		return CP_ONPLANE;

	if ( Behind == Poly->VertexCount )
		return CP_BACK;

	if ( Infront == Poly->VertexCount )
		return CP_FRONT;

	return CP_SPANNING;
}

int CBSPTree::ClassifyPoint ( D3DXVECTOR3* pos, PLANE* Plane ) 
{
	float result;

	D3DXVECTOR3* vec1		= &Plane->PointOnPlane;
	D3DXVECTOR3  Direction	= ( *vec1 ) - ( *pos );
	result					= D3DXVec3Dot ( &Direction, &Plane->Normal );

	if ( result < -g_EPSILON )
		return CP_FRONT;

	if ( result > g_EPSILON )
		return CP_BACK;

	return CP_ONPLANE;
}

void CBSPTree::SplitPolygon ( BSPPOLYGON* Poly, PLANE* Plane, BSPPOLYGON* FrontSplit, BSPPOLYGON* BackSplit )
{
	D3DVERTEX		FrontList [ 50 ];
	D3DVERTEX		BackList  [ 50 ];
	int				FrontCounter = 0;
	int				BackCounter  = 0;

	int				PointLocation [ 50 ];

	int				InFront = 0, Behind = 0, OnPlane = 0;
	int				CurrentVertex = 0, Location = 0;

	for ( int i = 0; i < Poly->VertexCount; i++ )
	{
		Location = ClassifyPoint ( ( D3DXVECTOR3* ) &Poly->Vertices [ i ], Plane );

		if ( Location == CP_FRONT )
			InFront++;
		else if ( Location == CP_BACK )
			Behind++;
		else
			OnPlane++;

		PointLocation [ i ] = Location;
	}

	if ( !InFront )
	{
		memcpy ( BackList, Poly->Vertices, Poly->VertexCount * sizeof ( D3DVERTEX ) );
		BackCounter = Poly->VertexCount;
	}

	if ( !Behind ) 
	{
		memcpy ( FrontList, Poly->Vertices, Poly->VertexCount * sizeof ( D3DVERTEX ) );
		FrontCounter = Poly->VertexCount;
	}

	if ( InFront && Behind )
	{
		for ( i = 0; i < Poly->VertexCount; i++ )
		{
			CurrentVertex = ( i + 1 ) % Poly->VertexCount;

			if ( PointLocation [ i ] == CP_ONPLANE )
			{
				FrontList [ FrontCounter ] = Poly->Vertices [ i ];
				FrontCounter++;

				BackList [ BackCounter ] = Poly->Vertices [ i ];
				BackCounter++;
				
				continue;
			}

			if ( PointLocation [ i ] == CP_FRONT )
			{
				FrontList [ FrontCounter ] = Poly->Vertices [ i ];
				FrontCounter++;
			}
			else
			{
				BackList [ BackCounter ] = Poly->Vertices [ i ];
				BackCounter++;
			}
			
			if ( PointLocation [ CurrentVertex ] == CP_ONPLANE || PointLocation [ CurrentVertex ] == PointLocation [ i ] )
				continue;
			
			D3DXVECTOR3 IntersectPoint;
			float		percent;

			Get_Intersect (
							( D3DXVECTOR3* ) &Poly->Vertices [ i ],
							( D3DXVECTOR3* ) &Poly->Vertices [ CurrentVertex ],
							&Plane->PointOnPlane,
							&Plane->Normal,
							&IntersectPoint,
							&percent
						  );

			D3DVERTEX copy;
			float deltax	= Poly->Vertices [ CurrentVertex ].tu - Poly->Vertices [ i ].tu;
			float deltay	= Poly->Vertices [ CurrentVertex ].tv - Poly->Vertices [ i ].tv;
			float texx		= Poly->Vertices [ i ].tu + ( deltax * percent );
			float texy		= Poly->Vertices [ i ].tv + ( deltay * percent );
			copy.x			= IntersectPoint.x;
			copy.y			= IntersectPoint.y; 
			copy.z			= IntersectPoint.z;
			copy.tu			= texx;
			copy.tv			= texy;
	
			BackList  [ BackCounter++  ] = copy;			
			FrontList [ FrontCounter++ ] = copy;
		}
	}

	FrontSplit->Vertices		 = ( D3DVERTEX* ) malloc ( FrontCounter * sizeof ( D3DVERTEX ) );
	BackSplit->Vertices			 = ( D3DVERTEX* ) malloc ( BackCounter  * sizeof ( D3DVERTEX ) );

	FrontSplit->VertexCount		 = 0;
	BackSplit->VertexCount		 = 0;

	FrontSplit->VertexCount		 = FrontCounter;
	memcpy ( FrontSplit->Vertices, FrontList, FrontCounter * sizeof ( D3DVERTEX ) );
	
	BackSplit->VertexCount		= BackCounter;
	memcpy ( BackSplit->Vertices, BackList, BackCounter * sizeof ( D3DVERTEX ) );

	BackSplit->IndexCount		= ( BackSplit->VertexCount  - 2 ) * 3;
	FrontSplit->IndexCount		= ( FrontSplit->VertexCount - 2 ) * 3;
	
	BackSplit->Indices			= ( WORD* ) malloc ( BackSplit->IndexCount  * sizeof ( WORD ) );
	FrontSplit->Indices			= ( WORD* ) malloc ( FrontSplit->IndexCount * sizeof ( WORD ) );
	
	short IndxBase;

	for ( short loop = 0, v1 = 1, v2 = 2; loop < FrontSplit->IndexCount / 3; loop++, v1 = v2, v2++ )
	{
		IndxBase = loop * 3;
		FrontSplit->Indices [ IndxBase     ] =  0;
		FrontSplit->Indices [ IndxBase + 1 ] = v1;
		FrontSplit->Indices [ IndxBase + 2 ] = v2;
	}
	for ( loop = 0, v1 = 1, v2 = 2; loop < BackSplit->IndexCount / 3; loop++, v1 = v2, v2++ )
	{
		IndxBase = loop * 3;
		BackSplit->Indices [ IndxBase     ] =  0;
		BackSplit->Indices [ IndxBase + 1 ] = v1;
		BackSplit->Indices [ IndxBase + 2 ] = v2;
	}

	FrontSplit->Normal			= Poly->Normal;
	BackSplit->Normal			= Poly->Normal;
	FrontSplit->TextureIndex	= Poly->TextureIndex;
	BackSplit->TextureIndex		= Poly->TextureIndex;

	for ( i = 0; i <  BackSplit->VertexCount; i++ )
		BackSplit->Vertices  [ i ].Normal =  BackSplit->Normal;

	for ( i = 0; i < FrontSplit->VertexCount; i++ )
		FrontSplit->Vertices [ i ].Normal = FrontSplit->Normal;

}

BOOL CBSPTree::Get_Intersect ( D3DXVECTOR3* linestart, D3DXVECTOR3* lineend, D3DXVECTOR3* vertex, D3DXVECTOR3* normal, D3DXVECTOR3* intersection, float* percentage )
{
	D3DXVECTOR3 direction, L1;
	float	    linelength, dist_from_plane;

	direction.x = lineend->x - linestart->x;
	direction.y = lineend->y - linestart->y;
	direction.z = lineend->z - linestart->z;

	linelength = D3DXVec3Dot ( &direction, normal );

	if ( fabsf ( linelength ) < g_EPSILON )
		return false; 

	L1.x = vertex->x - linestart->x;
	L1.y = vertex->y - linestart->y;
	L1.z = vertex->z - linestart->z;

	dist_from_plane = D3DXVec3Dot ( &L1, normal );

	*percentage = dist_from_plane / linelength; 

	if ( *percentage < 0.0f || *percentage > 1.0f )
		return FALSE;

	intersection->x = linestart->x + direction.x * ( *percentage );
	intersection->y = linestart->y + direction.y * ( *percentage );
	intersection->z = linestart->z + direction.z * ( *percentage );

	return TRUE;
}

void CBSPTree::RenderBSPPolys ( int BrushIndex ) 
{
	if ( g_iCSGBrush == BrushIndex && !g_bWireFrame && !g_bCompileFinal && g_iCSGMode != CSG_NONE )
	{
		lpDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	}

	for ( int i = 0; i < PolyCount; i++ )
	{
		if ( PolygonDeleted [ i ] == FALSE )
		{
			int Texture = Polygons [ i ].TextureIndex;

			if ( Texture != g_iLastTexture && Texture < NumberOfTextures )
			{
				lpDevice->SetTexture ( 0, lpTextureSurface [ Polygons [ i ].TextureIndex ] );
				g_iLastTexture = Polygons [ i ].TextureIndex;
			}

			lpDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, Polygons [ i ].VertexCount, ( Polygons [ i ].IndexCount / 3 ) , &Polygons [ i ].Indices [ 0 ], D3DFMT_INDEX16, &Polygons [ i ].Vertices [ 0 ], 32 );
		}
	}
	
	if ( g_iCSGBrush == BrushIndex && !g_bWireFrame && !g_bCompileFinal && g_iCSGMode != CSG_NONE )
	{
		lpDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	}
}


BOOL CBSPTree::ClipTree ( long CurrentNode, long* Polygons, long pPolyCount, CBSPTree* BSPTree, BOOL ClipSolid, BOOL RemoveCoPlanar )
{   
	D3DXVECTOR3 a, b;
    long* FrontList = NULL;
    long* BackList = NULL;
    long  FSplit = -1, BSplit = -1;
    long  FListCount = 0;
    long  BListCount = 0;
	float result;

	BSPTree->IsDirty = TRUE;

	BOOL FreePolyList = FALSE;
	
	if ( Polygons == NULL )
	{
		FreePolyList = TRUE;
		Polygons  = new long [ BSPTree->PolyCount ];

		for ( int i = 0; i < BSPTree->PolyCount; i++ )
		{
			Polygons [ i ] = i;
		}

		pPolyCount = BSPTree->PolyCount;
		
		if ( pPolyCount <= 0 )
			return FALSE;

		if ( PolyCount <= 0 )
			return FALSE;
	}
        
	for ( int p = 0; p < pPolyCount; p++ )
	{
        if ( BSPTree->PolygonDeleted [ Polygons [ p ] ] )
			continue;

		switch ( ClassifyPoly ( &Planes [ Nodes [ CurrentNode ].Plane ], &BSPTree->Polygons [ Polygons [ p ] ] ) ) 
		{
			case CP_ONPLANE:
			{
				a = Planes [ Nodes [ CurrentNode ].Plane ].Normal;
				b = BSPTree->Polygons [ Polygons [ p ] ].Normal;

				result = ( float ) fabs ( ( a.x - b.x ) + ( a.y - b.y ) + ( a.z - b.z ) );

				if ( result < 0.1f )
				{
					if ( RemoveCoPlanar )
					{
						BListCount++;
						BackList = ( long* ) realloc ( BackList, BListCount * sizeof ( long ) );
						BackList [ BListCount - 1 ] = Polygons [ p ];
					}
					else 
					{
						FListCount++;
						FrontList = ( long* ) realloc ( FrontList, FListCount * sizeof ( long ) );
						FrontList [ FListCount - 1 ] = Polygons [ p ];
					}
				}
				else
				{
					BListCount++;
					BackList = ( long* ) realloc ( BackList, BListCount * sizeof ( long ) );
					BackList [ BListCount - 1 ] = Polygons [ p ];
				}
			}
			break;

            case CP_FRONT:
			{
                FListCount++;
				FrontList = ( long* ) realloc ( FrontList, FListCount * sizeof ( long ) );
                FrontList [ FListCount - 1 ] = Polygons [ p ];
			}
			break;

            case CP_BACK:
			{
                BListCount++;
				BackList = ( long* ) realloc ( BackList, BListCount * sizeof ( long ) );
                BackList [ BListCount - 1 ] = Polygons [ p ];
			}
			break;

            case CP_SPANNING:
			{
				FListCount++;
				FrontList = ( long* ) realloc ( FrontList, FListCount * sizeof ( long ) );
				FrontList [ FListCount - 1 ] = BSPTree->AllocAddPoly ( );
				FSplit = FListCount - 1;
				BListCount++;
				BackList = ( long* ) realloc ( BackList, BListCount * sizeof ( long ) );
				BackList [ BListCount - 1 ] = BSPTree->AllocAddPoly ( );
				BSplit = BListCount - 1;
				SplitPolygon ( &BSPTree->Polygons [ Polygons [ p ] ], &Planes [ Nodes [ CurrentNode ].Plane ], &BSPTree->Polygons [ FrontList [ FSplit ] ], &BSPTree->Polygons [ BackList [ BSplit ] ] );
                
				BSPTree->PolygonDeleted [ Polygons [ p ] ] = TRUE;
			}
			break;
        }
    }

	if ( ClipSolid )
	{
		if ( Nodes [ CurrentNode ].Back == -1 )
		{
			for ( int i = 0; i < BListCount; i++ )
			{
				BSPTree->PolygonDeleted [ BackList [ i ] ] = TRUE;
			}

			BListCount = 0;
		}
	}
	else
	{
		if ( Nodes [ CurrentNode ].IsLeaf == 1 )
		{
			for ( int i = 0; i < FListCount; i++ )
			{
				BSPTree->PolygonDeleted [ FrontList [ i ] ] = TRUE;
			}

			FListCount = 0;
		}
	}

	if ( FListCount > 0 && Nodes [ CurrentNode ].IsLeaf == 0 && Nodes [ CurrentNode ].Front > -1 )
		ClipTree ( Nodes [ CurrentNode ].Front, FrontList, FListCount, BSPTree, ClipSolid, RemoveCoPlanar );
    
    if ( BListCount > 0 && Nodes [ CurrentNode ].Back > -1 )
		ClipTree ( Nodes [ CurrentNode ].Back, BackList, BListCount, BSPTree, ClipSolid, RemoveCoPlanar );

	if ( FrontList )
	{ 
		free ( FrontList );
		FrontList = NULL;
	}

	if ( BackList )
	{ 
		free ( BackList );
		BackList = NULL;
	}
	
	if ( FreePolyList )
		delete [ ] Polygons;

	return TRUE;
}

void CBSPTree::InvertPolys ( void )
{
	int			Counter	= 1;
	D3DVERTEX*	TVerts = NULL;

    for ( int i = 0; i < PolyCount; i++ )
	{
		if ( PolygonDeleted [ i ] )
			continue;

        TVerts = new D3DVERTEX [ Polygons [ i ].VertexCount ];

		TVerts [ 0 ] = Polygons [ i ].Vertices [ 0 ];
		Counter = 1;

		for ( int k = Polygons [ i ].VertexCount; k > 1; k--, Counter++ )
		{
            TVerts [ Counter ] = Polygons [ i ].Vertices [ k - 1 ];
        }
		
		memcpy ( &Polygons [ i ].Vertices [ 0 ], &TVerts [ 0 ], Polygons [ i ].VertexCount * sizeof ( D3DVERTEX ) );
		delete [ ] TVerts;

		Polygons [ i ].Normal = -Polygons [ i ].Normal;

		for ( k = 0; k < Polygons [ i ].VertexCount; k++ )
		{
			Polygons [ i ].Vertices [ k ].Normal = Polygons [ i ].Normal;
		}
        
		int pCount = Polygons [ i ].IndexCount / 3;

		for ( int lloop = 0, v1 = 1, v2 = 2; lloop < pCount; lloop ++ )
		{
			Polygons [ i ].Indices [   lloop * 3       ] = 0;
			Polygons [ i ].Indices [ ( lloop * 3 ) + 1 ] = v1;
			Polygons [ i ].Indices [ ( lloop * 3 ) + 2 ] = v2;

			v1 = v2;
			v2 = v2 + 1;
		}
	}
}

BSPPOLYGON* CBSPTree::AddPolygonEX ( BSPPOLYGON* Poly )
{
	int Child = AllocAddPoly ( );

	Polygons [ Child ].IndexCount		= Poly->IndexCount;
	Polygons [ Child ].VertexCount		= Poly->VertexCount;
	Polygons [ Child ].Normal			= Poly->Normal;
	Polygons [ Child ].TextureIndex		= Poly->TextureIndex;
	Polygons [ Child ].UsedAsSplitter	= 0;
	Polygons [ Child ].NextPoly			= NULL;

	Polygons [ Child ].Vertices			= ( D3DVERTEX* ) malloc ( Poly->VertexCount * sizeof ( D3DVERTEX ) );
	Polygons [ Child ].Indices			= ( WORD*      ) malloc ( Poly->IndexCount  * sizeof ( WORD ) );

	memcpy ( &Polygons [ Child ].Vertices [ 0 ], &Poly->Vertices [ 0 ], Poly->VertexCount * sizeof ( D3DVERTEX ) );
	memcpy ( &Polygons [ Child ].Indices  [ 0 ], &Poly->Indices  [ 0 ], Poly->IndexCount  * sizeof ( short ) );

	return &Polygons [ Child ];

}

CBSPTree* CBSPTree::CopyTree ( void )
{
	CBSPTree* DESTree = NULL;

	if ( DESTree == NULL )
		DESTree = new CBSPTree;

	DESTree->FatalError		= FatalError;
	DESTree->BrushBase		= BrushBase;
	DESTree->RootNode		= RootNode;
	DESTree->NodeCount		= NodeCount;
	DESTree->PolyCount		= PolyCount;
	DESTree->PlaneCount		= PlaneCount;
	DESTree->LeafCount		= LeafCount;
	DESTree->PolygonList	= PolygonList;

	DESTree->PolygonDeleted	= ( BOOL* )			malloc ( PolyCount	* sizeof ( BOOL )		);
	DESTree->Polygons		= ( BSPPOLYGON* )	malloc ( PolyCount	* sizeof ( BSPPOLYGON ) );
	DESTree->Planes			= ( PLANE* )		malloc ( PlaneCount	* sizeof ( PLANE )		);
	DESTree->Nodes			= ( NODE* )			malloc ( NodeCount	* sizeof ( NODE )		);
	DESTree->Leaves			= ( LEAF* )			malloc ( LeafCount	* sizeof ( LEAF )		);

	memcpy ( DESTree->PolygonDeleted,	PolygonDeleted,		PolyCount	* sizeof ( BOOL )		);
	memcpy ( DESTree->Polygons,			Polygons,			PolyCount	* sizeof ( BSPPOLYGON ) );
	memcpy ( DESTree->Planes,			Planes,				PlaneCount	* sizeof ( PLANE )		);
	memcpy ( DESTree->Nodes,			Nodes,				NodeCount	* sizeof ( NODE )		);
	memcpy ( DESTree->Leaves,			Leaves,				LeafCount	* sizeof ( LEAF )		);

	for ( int i = 0; i < PolyCount; i++ )
	{
		DESTree->Polygons [ i ].Vertices = NULL;
		DESTree->Polygons [ i ].Vertices = ( D3DVERTEX* ) malloc ( Polygons [ i ].VertexCount * sizeof ( D3DVERTEX ) );

		memcpy ( DESTree->Polygons [ i ].Vertices, Polygons [ i ].Vertices, Polygons [ i ].VertexCount * sizeof ( D3DVERTEX ) );
		
		DESTree->Polygons [ i ].Indices = NULL;
		DESTree->Polygons [ i ].Indices = ( WORD* ) malloc ( Polygons [ i ].IndexCount * sizeof ( WORD ) );
		
		memcpy ( DESTree->Polygons [ i ].Indices, Polygons [ i ].Indices, Polygons [ i ].IndexCount * sizeof ( WORD ) );
	}

	return DESTree;
}