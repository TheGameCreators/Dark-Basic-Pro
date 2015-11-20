
#define NAME "CSG"
#define TITLE "CSG"
#define WIN32_LEAN_AND_MEAN

#include "CSG.h"
#include "BSPTree.h"

#include "cEnhancedXC.h"
 
LPDIRECT3D8             lpD3D					= NULL;
LPDIRECT3DDEVICE8       lpDevice				= NULL;
LPDIRECT3DTEXTURE8		lpTextureSurface [ 3 ];
LPD3DXFONT				MainFont;
HWND					hwnd;
HINSTANCE				hInst;

char szMap [ 256 ];

int						rendering			= 1;
int						quit				= 0;

BRUSH*					Brushes				= NULL;
int						BrushCount			= 0;
int						NumberOfTextures	= 3;

D3DXVECTOR3				vecLook				(  0,  0,    1);
D3DXVECTOR3				vecUp				(  0,  1,    0);
D3DXVECTOR3				vecRight			(  1,  0,    0);
//D3DXVECTOR3			vecPos				(  0, 10, -300);
D3DXVECTOR3				vecPos				(  0, 150, -300);
//D3DXVECTOR3			vecPos				(  0, 0, -70);
float					Pitch				= 0;
float					time_elapsed		= 0;

int						g_iCSGBrush			= 0;
int						g_iCSGMode			= CSG_NONE;
int						g_iLastTexture		= -1;
BOOL					g_bWireFrame		= FALSE;
BOOL					g_bSafeTransform	= TRUE;
BOOL					g_bCompileFinal		= FALSE;
BOOL					g_bDisplayHelp		= FALSE;
BOOL					g_bHSR				= TRUE;

CBSPTree				*FinalBSP			= NULL;

int						LEFTKEY = 0, RIGHTKEY = 0, UPKEY = 0, DOWNKEY = 0, PGUPKEY = 0, PGDOWNKEY = 0;
BOOL					RotateBrushes [ 3 ]	= { FALSE, FALSE, FALSE };

LRESULT CALLBACK	WindowProc ( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam );
BOOL				Init_DX ( HWND hWnd );
void				InitPolygons ( void );
void				renderframe ( void );
void				UpdateViewpos ( void );
void				SetupFont ( void );
void				sleep (  clock_t wait );
void				UpdateBSPTrees ( void );
BBOX				CalculateBoundingBox ( BRUSH* brush , D3DXMATRIX* Matrix );
BOOL				BoundingBoxesIntersect ( BBOX BoxA, BBOX BoxB );
void				SetupLights ( void );
void				LoadTextures (void );
void				RotateBrush ( long Brush, float X, float Y, float Z, D3DXVECTOR3 CentrePoint );
void				CheckInput ( );
void				FreeBrushes ( BRUSH* lpBrushes, WORD MeshCount );

D3DVERTEX SetupVertex ( float posx, float posy, float posz, float tu, float tv )
{
	D3DVERTEX vertex;

	vertex.x		= posx;
	vertex.y		= posy;
	vertex.z		= posz;
	
	vertex.tu		= tu;
	vertex.tv		= tv;
	
	return vertex;
}

void CalculateNormal ( POLYGON* pPolygon, bool bFlip = false )
{
	// generate polygon normal

	D3DXVECTOR3* vec0;
	D3DXVECTOR3* vec1;
	D3DXVECTOR3* vec2;

	vec0 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ 0 ] ];
	vec1 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ 1 ] ];
	vec2 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ pPolygon->IndexCount - 1 ] ];

	if ( bFlip )
	{
		vec0 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ 0 ] ];
		vec2 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ 1 ] ];
		vec1 = ( D3DXVECTOR3* ) &pPolygon->Vertices [ pPolygon->Indices [ pPolygon->IndexCount - 1 ] ];
	}

	D3DXVECTOR3 edge1 = ( *vec1 ) - ( *vec0 );
	D3DXVECTOR3 edge2 = ( *vec2 ) - ( *vec0 );

	D3DXVec3Cross ( &pPolygon->Normal, &edge1, &edge2 );

	D3DXVec3Normalize ( &pPolygon->Normal, &pPolygon->Normal );
}


void SetBlockPosition ( BRUSH* pBrush, float fX, float fY, float fZ, float fScale )
{
		D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	D3DXMATRIX	matViewInverse;
	D3DXMATRIX	matView;
	
	// use this for the inverse function
	float fDet;

	// apply scaling to the object
	D3DXMatrixScaling ( &matScale, fScale, fScale, fScale );
	
	// apply translation to the object
	D3DXMatrixTranslation ( &matTranslation, fX, fY, fZ );

	// setup rotation matrices
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( 0.0f ) );	// x rotation
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( 0.0f ) );	// y rotation
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( 0.0f ) );	// z rotation

	// build final rotation matrix
	matRotation = matRotateX * matRotateY * matRotateZ;

	pBrush->Matrix = matRotation * matScale * matTranslation;

}

BRUSH* ConvertXModelToBrushFormat ( char* szFilename, BRUSH* pBrush, int* piCount, LPDIRECT3DDEVICE8 lpDevice )
{
	// takes an x model and converts it into brushes

	// check the pointers are valid
	if ( !szFilename || !piCount )
		return NULL;

	// used to access vertex data
	struct sMeshData
	{
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	// variable declarations
	tagModelData*			ptr;				// model data
	LPDIRECT3DVERTEXBUFFER8 pMeshVertexBuffer;	// vertex buffer
	LPDIRECT3DINDEXBUFFER8  pMeshIndexBuffer;	// index buffer
	sMeshData*				pMeshVertices;		// mesh vertices
	WORD*					pMeshIndices;		// mesh indices
	sMesh*					pMesh;				// mesh data
	int						iCount;
	
	// load the model
	Constructor ( lpDevice );
	ptr = Load ( 1, szFilename );

	pMesh = ptr->m_Object.m_Meshes;

	// count the number of brushes so we can allocate enough memory for them
	iCount = 0;

	while ( pMesh )
	{
		pMesh = pMesh->m_Next;
		iCount++;
	}

	// store the number of models in the brush count pointer
	*piCount = iCount;

	// now setup the brushes
	pBrush = new BRUSH [ iCount ];	// allocate memory
	
	// set the mesh pointer back to the original mesh
	pMesh = ptr->m_Object.m_Meshes;

	// run through all meshes and store the brush data
	// first off set iCount to 0 so we know which brush
	// we are dealing with
	iCount = 0;

	while ( pMesh )
	{
		int   iInd          = 0;
		DWORD dwNumVertices = pMesh->m_Mesh->GetNumVertices ( );
		DWORD dwNumFaces	= pMesh->m_Mesh->GetNumFaces ( );

		pBrush [ iCount ].Faces      = new POLYGON [ dwNumFaces ];
		pBrush [ iCount ].FaceCount  = dwNumFaces;
		pBrush [ iCount ].Bounds.Max = D3DXVECTOR3 (  150.0f,  150.0f,  150.0f );
		pBrush [ iCount ].Bounds.Min = D3DXVECTOR3 ( -150.0f, -150.0f, -150.0f );
		pBrush [ iCount ].BSPTree    = NULL;

		pMesh->m_Mesh->GetVertexBuffer ( &pMeshVertexBuffer );
		pMesh->m_Mesh->GetIndexBuffer  ( &pMeshIndexBuffer );

		DWORD dwFVF = pMesh->m_Mesh->GetFVF ( );

		pMeshVertexBuffer->Lock ( 0, pMesh->m_Mesh->GetNumVertices  ( ) * sizeof ( sMeshData ), ( BYTE** ) &pMeshVertices, 0 );
		pMeshIndexBuffer->Lock  ( 0, 3 * pMesh->m_Mesh->GetNumFaces ( ) * sizeof ( WORD ),      ( BYTE** ) &pMeshIndices,  0 );
	
		for ( int iTemp = 0; iTemp < dwNumFaces; iTemp++ )
		{
			char szX [ 256 ];
			char szY [ 256 ];
			char szZ [ 256 ];

			int iA = pMeshIndices [ iInd + 0 ];
			int iB = pMeshIndices [ iInd + 1 ];
			int iC = pMeshIndices [ iInd + 2 ];

			WORD wIndices [ ] = { 0, 1, 2 };
			
			pBrush [ iCount ].Faces [ iTemp ].IndexCount   = 3;
			pBrush [ iCount ].Faces [ iTemp ].TextureIndex = 0;
			pBrush [ iCount ].Faces [ iTemp ].VertexCount  = 3;
			pBrush [ iCount ].Faces [ iTemp ].Indices      = new WORD      [ 3 ];
			pBrush [ iCount ].Faces [ iTemp ].Vertices     = new D3DVERTEX [ 3 ];

			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 0 ] = SetupVertex ( pMeshVertices [ iA ].x, pMeshVertices [ iA ].y, pMeshVertices [ iA ].z, pMeshVertices [ iA ].tu, pMeshVertices [ iA ].tv );
			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 1 ] = SetupVertex ( pMeshVertices [ iB ].x, pMeshVertices [ iB ].y, pMeshVertices [ iB ].z, pMeshVertices [ iB ].tu, pMeshVertices [ iB ].tv );
			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 2 ] = SetupVertex ( pMeshVertices [ iC ].x, pMeshVertices [ iC ].y, pMeshVertices [ iC ].z, pMeshVertices [ iC ].tu, pMeshVertices [ iC ].tv );

			for ( int iChar = 0; iChar < 3; iChar++ )
			{
				sprintf ( szX, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].x );
				sprintf ( szY, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].y );
				sprintf ( szZ, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].z );

				pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].x = atof ( szX );
				pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].y = atof ( szY );
				pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].z = atof ( szZ );
			}
			
			memcpy ( pBrush [ iCount ].Faces [ iTemp ].Indices, wIndices, sizeof ( wIndices ) );
			CalculateNormal ( &pBrush [ iCount ].Faces [ iTemp ] );

			iInd += 3;
		}

		SetBlockPosition ( &pBrush [ iCount ], 0.0f, 0.0f, 0.0f, 1.0f );
	
		pMeshVertexBuffer->Unlock ( );
		pMeshIndexBuffer->Unlock  ( );

		iCount++;

		pMesh = pMesh->m_Next;
	}

	iCount = 0;
	pMesh  = ptr->m_Object.m_Meshes;

	for ( int iTemp = ptr->m_Object.m_NumFrames; iTemp > 0; iTemp-- )
	{
		sFrame* pFrame = ptr->m_Object.m_Frames->FindFrame ( iTemp );

		if ( pFrame )
		{
			pBrush [ iCount ].Matrix._41 = pFrame->m_matOriginal._41;
			pBrush [ iCount ].Matrix._42 = pFrame->m_matOriginal._42;
			pBrush [ iCount ].Matrix._43 = pFrame->m_matOriginal._43;

			iCount++;
		}
	}
	
	return pBrush;
}

void InitPolygons ( void )
{

	Brushes = ConvertXModelToBrushFormat ( szMap, Brushes, &BrushCount, lpDevice );

	for ( int iBrush = 0; iBrush < BrushCount; iBrush++ )
		Brushes [ iBrush ].Bounds = CalculateBoundingBox ( &Brushes [ iBrush ], &Brushes [ iBrush ].Matrix );

	LoadTextures   ( );
	UpdateBSPTrees ( );

	CBSPTree* pLink = NULL;
	CBSPTree* pArray [ 300 ];
	
	FILE* fp = NULL;
	char szText [ 256 ];
	int iLoop;
	int iA;
	int iPos = 0;

	fp = fopen ( "level.dat", "wb" );

	int iCount = 0;

	for ( iPos = 0; iPos < BrushCount; iPos++ )
	{
		pArray [ iPos ] = ( CBSPTree* ) Brushes [ iPos ].BSPTree;

		pLink = pArray [ iPos ];

		for ( iLoop = 0; iLoop < pLink->PolyCount; iLoop++ )
		{
			if ( !pLink->PolygonDeleted [ iLoop ] )
				iCount++;
		}
	}

	iPos = 0;

#define FILE_TEXT 0

#if FILE_TEXT
	sprintf ( szText, "%d\n", iCount );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
#else
	fwrite ( &iCount, sizeof ( int ), 1, fp );
#endif

	int iTest = 0;

	for ( iPos = 0; iPos < BrushCount; iPos++ )
	{
		pLink = pArray [ iPos ];

		for ( iLoop = 0; iLoop < pLink->PolyCount; iLoop++ )
		{
			if ( !pLink->PolygonDeleted [ iLoop ] )
			{
				#if FILE_TEXT
					sprintf ( szText, "%d\n", pLink->Polygons [ iLoop ].VertexCount );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					sprintf ( szText, "%d\n", pLink->Polygons [ iLoop ].IndexCount );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					sprintf ( szText, "%d\n", pLink->Polygons [ iLoop ].TextureIndex );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				#else
					fwrite ( &pLink->Polygons [ iLoop ].VertexCount,  sizeof ( WORD ), 1, fp );
					fwrite ( &pLink->Polygons [ iLoop ].IndexCount,   sizeof ( WORD ), 1, fp );
					fwrite ( &pLink->Polygons [ iLoop ].TextureIndex, sizeof ( int ), 1, fp );

					fwrite ( &pLink->Polygons [ iLoop ].Normal.x,  sizeof ( float ), 1, fp );
					fwrite ( &pLink->Polygons [ iLoop ].Normal.y,  sizeof ( float ), 1, fp );
					fwrite ( &pLink->Polygons [ iLoop ].Normal.z,  sizeof ( float ), 1, fp );
				#endif

				// go through all vertices
				for ( iA = 0; iA < pLink->Polygons [ iLoop ].VertexCount; iA++ )
				{
					#if FILE_TEXT
						sprintf ( szText, "%.1f,", pLink->Polygons [ iLoop ].Vertices [ iA ].x );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
						sprintf ( szText, "%.1f,", pLink->Polygons [ iLoop ].Vertices [ iA ].y );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
						sprintf ( szText, "%.1f,\n", pLink->Polygons [ iLoop ].Vertices [ iA ].z );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					#else
						char szX [ 256 ];
						char szY [ 256 ];
						char szZ [ 256 ];

						sprintf ( szX, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].x );
						sprintf ( szY, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].y );
						sprintf ( szZ, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].z );

						pLink->Polygons [ iLoop ].Vertices [ iA ].x = atof ( szX );
						pLink->Polygons [ iLoop ].Vertices [ iA ].y = atof ( szY );
						pLink->Polygons [ iLoop ].Vertices [ iA ].z = atof ( szZ );

						fwrite ( &pLink->Polygons [ iLoop ].Vertices [ iA ].x,  sizeof ( float ), 1, fp );
						fwrite ( &pLink->Polygons [ iLoop ].Vertices [ iA ].y,  sizeof ( float ), 1, fp );
						fwrite ( &pLink->Polygons [ iLoop ].Vertices [ iA ].z,  sizeof ( float ), 1, fp );
						fwrite ( &pLink->Polygons [ iLoop ].Vertices [ iA ].tu, sizeof ( float ), 1, fp );
						fwrite ( &pLink->Polygons [ iLoop ].Vertices [ iA ].tv, sizeof ( float ), 1, fp );
					#endif
				}

				for ( iA = 0; iA < pLink->Polygons [ iLoop ].IndexCount; iA++ )
				{
					#if FILE_TEXT
						sprintf ( szText, "%d,", pLink->Polygons [ iLoop ].Indices [ iA ] );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					#else
						fwrite ( &pLink->Polygons [ iLoop ].Indices [ iA ], sizeof ( WORD ), 1, fp );
					#endif
				}

				iTest++;

				#if FILE_TEXT
					fwrite ( "\n\n", strlen ( "\n\n" ) * sizeof ( char ), 1, fp );
				#endif
			}
		}
	}

	fclose ( fp );

	quit = 1;
}


void LoadTextures(void)  
{
	//load in textures
	NumberOfTextures = 3;
	D3DXCreateTextureFromFile ( lpDevice,"Textures\\brick.bmp",	            &lpTextureSurface [ 0 ] ); 
	D3DXCreateTextureFromFile ( lpDevice,"Textures\\metalrustyfloor1.jpg",	&lpTextureSurface [ 1 ] );
	D3DXCreateTextureFromFile ( lpDevice,"Textures\\curvyfloor.jpg",		&lpTextureSurface [ 2 ] );
}

void UpdateBSPTrees ( void )
{
	int		  f, b;
	BOOL	  ClipSolid1 = FALSE, ClipSolid2 = FALSE;
	CBSPTree* CSGTree = NULL, *TGTTree = NULL, *CPYTree = NULL;

	for ( b = 0; b < BrushCount; b++ )
	{
		if ( Brushes [ b ].BSPTree )
		{
			delete ( ( CBSPTree* ) Brushes [ b ].BSPTree );
			Brushes [ b ].BSPTree = NULL;
		}

		if ( Brushes [ b ].BSPTree == NULL )
			Brushes [ b ].BSPTree = new CBSPTree;

		( ( CBSPTree* ) Brushes [ b ].BSPTree )->InitPolygons ( &Brushes [ b ], 1, FALSE );
	}

	switch ( g_iCSGMode )
	{
		case CSG_DIFFERENCE:
			ClipSolid1 = TRUE;
			ClipSolid2 = FALSE; 
		break;

		case CSG_INTERSECTION:
			ClipSolid1 = FALSE;
			ClipSolid2 = FALSE; 
		break;
	}

	if ( g_iCSGMode != CSG_NONE )
	{
		CSGTree = ( ( CBSPTree* ) Brushes [ g_iCSGBrush ].BSPTree );

		for ( b = 0; b < BrushCount; b++ )
		{
			if ( b == g_iCSGBrush )
				continue;

			if ( !BoundingBoxesIntersect ( Brushes [ g_iCSGBrush ].Bounds, Brushes [ b ].Bounds ) )
				continue;

			TGTTree = ( ( CBSPTree* ) Brushes [ b ].BSPTree );

			CSGTree->ClipTree ( CSGTree->RootNode, NULL, 0, TGTTree, ClipSolid1, TRUE );

			CPYTree = CSGTree->CopyTree ( );
			TGTTree->ClipTree ( TGTTree->RootNode, NULL, 0, CPYTree, ClipSolid2, FALSE );
			
			if ( g_iCSGMode == CSG_DIFFERENCE )
				CPYTree->InvertPolys ( );
			
			for ( f = 0; f < CPYTree->PolyCount; f++ )
			{
				if ( !CPYTree->PolygonDeleted [ f ] ) 
					TGTTree->AddPolygonEX ( &CPYTree->Polygons [ f ] );
			}
			
			delete CPYTree;
			CPYTree = NULL;

			if ( g_iCSGMode == CSG_INTERSECTION )
				TGTTree->InitPolygons ( &Brushes [ b ], 1, TRUE );
		}
	}

	// mow perform a union on all the resulting trees
	// this is how we perform pre compile hidden surface removal
	for ( b = 0; b < BrushCount; b++ )
	{
		if ( b == g_iCSGBrush && g_iCSGMode != CSG_NONE )
			continue;

		( ( CBSPTree* ) Brushes [ b ].BSPTree )->m_bMisc = FALSE;

		for ( int o = 0; o < BrushCount; o++ )
		{
			if ( o == b || ( o == g_iCSGBrush && g_iCSGMode != CSG_NONE ) )
				continue;

			if ( ( ( CBSPTree* ) Brushes [ o ].BSPTree )->m_bMisc == TRUE )
				continue;

			if ( BoundingBoxesIntersect ( Brushes [ o ].Bounds, Brushes [ b ].Bounds ) == TRUE )
			{
				( ( CBSPTree* ) Brushes [ o ].BSPTree )->ClipTree ( ( ( CBSPTree* ) Brushes [ o ].BSPTree )->RootNode, NULL, 0, ( ( CBSPTree* ) Brushes [ b ].BSPTree ), TRUE, TRUE  );
				( ( CBSPTree* ) Brushes [ b ].BSPTree )->ClipTree ( ( ( CBSPTree* ) Brushes [ b ].BSPTree )->RootNode, NULL, 0, ( ( CBSPTree* ) Brushes [ o ].BSPTree ), TRUE, FALSE );
			}
			
			( ( CBSPTree* ) Brushes [ b ].BSPTree )->m_bMisc = TRUE;
		}
	}

	if ( g_bCompileFinal )
	{	
		if ( g_iCSGMode != CSG_NONE )
		{
			delete ( CBSPTree* ) Brushes [ g_iCSGBrush ].BSPTree;
			Brushes [ g_iCSGBrush ].BSPTree = NULL;
		}

		if ( FinalBSP == NULL )
			FinalBSP = new CBSPTree;

		FinalBSP->InitPolygons ( Brushes, BrushCount, TRUE );
	}
}

BBOX CalculateBoundingBox ( BRUSH* brush, D3DXMATRIX* Matrix )
{
	BBOX tBBox;
	D3DXVECTOR3 vec;

	tBBox.Min = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );
	tBBox.Max = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );

	for ( ULONG f = 0; f < brush->FaceCount; f++ )
	{
		for ( int v = 0; v < brush->Faces [ f ].VertexCount; v++ )
		{
			if ( Matrix == NULL )
			{
				vec = *( D3DXVECTOR3* ) &brush->Faces [ f ].Vertices [ v ];
			}
			else 
			{
				D3DXVec3TransformCoord ( &vec, ( D3DXVECTOR3* ) &brush->Faces [ f ].Vertices [ v ], Matrix );
			}

			if ( vec.x < tBBox.Min.x ) tBBox.Min.x = vec.x;
			if ( vec.y < tBBox.Min.y ) tBBox.Min.y = vec.y;
			if ( vec.z < tBBox.Min.z ) tBBox.Min.z = vec.z;

			if ( vec.x > tBBox.Max.x ) tBBox.Max.x = vec.x;
			if ( vec.y > tBBox.Max.y ) tBBox.Max.y = vec.y;
			if ( vec.z > tBBox.Max.z ) tBBox.Max.z = vec.z;
		}
	}

	return tBBox;
}

BOOL BoundingBoxesIntersect ( BBOX BoxA, BBOX BoxB )
{
	RECT Rect1, Rect2, DestRect;
    
	Rect1.left = ( long ) BoxA.Min.x - 1; Rect1.right  = ( long ) BoxA.Max.x + 1;
    Rect1.top  = ( long ) BoxA.Min.z - 1; Rect1.bottom = ( long ) BoxA.Max.z + 1;
    Rect2.left = ( long ) BoxB.Min.x - 1; Rect2.right  = ( long ) BoxB.Max.x + 1;
    Rect2.top  = ( long ) BoxB.Min.z - 1; Rect2.bottom = ( long ) BoxB.Max.z + 1;
    
    if ( IntersectRect ( &DestRect, &Rect1, &Rect2 ) == 0 )
		return FALSE;
    
    Rect1.left = ( long ) BoxA.Min.x - 1; Rect1.right  = ( long ) BoxA.Max.x + 1;
    Rect1.top  = ( long ) BoxA.Min.y - 1; Rect1.bottom = ( long ) BoxA.Max.y + 1;
    Rect2.left = ( long ) BoxB.Min.x - 1; Rect2.right  = ( long ) BoxB.Max.x + 1;
    Rect2.top  = ( long ) BoxB.Min.y - 1; Rect2.bottom = ( long ) BoxB.Max.y + 1;
    
    if ( IntersectRect ( &DestRect, &Rect1, &Rect2 ) == 0 )
		return FALSE;
    
	return TRUE;
}

D3DXVECTOR3 CalculatePolyNormal ( D3DVERTEX v1, D3DVERTEX v2, D3DVERTEX v3, D3DXMATRIX* Matrix )
{
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 vec0, vec1, vec2;

	if ( Matrix == NULL )
	{
		vec0 = *( D3DXVECTOR3* ) &v1;
		vec1 = *( D3DXVECTOR3* ) &v2;
		vec2 = *( D3DXVECTOR3* ) &v3;
	}
	else 
	{
		D3DXVec3TransformCoord ( &vec0, ( D3DXVECTOR3* ) &v1, Matrix );
		D3DXVec3TransformCoord ( &vec1, ( D3DXVECTOR3* ) &v2, Matrix );
		D3DXVec3TransformCoord ( &vec2, ( D3DXVECTOR3* ) &v3, Matrix );
	}
	
	D3DXVec3Cross ( &Normal, &( ( vec1 ) - ( vec0 ) ), &( ( vec2 ) - ( vec0 ) ) );
	D3DXVec3Normalize ( &Normal, &Normal );
	return Normal;
}

static BOOL doInit ( HINSTANCE hInstance, int nCmdShow )
{	
	WNDCLASS            wc;
   
    // Set up and register window class
    wc.style		 = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	 = ( WNDPROC ) WindowProc;
    wc.cbClsExtra	 = 0;
    wc.cbWndExtra	 = sizeof ( DWORD );
    wc.hInstance	 = hInstance;
    wc.hIcon		 = NULL;
    wc.hCursor		 = LoadCursor ( NULL, IDC_ARROW );
    wc.hbrBackground = ( HBRUSH ) GetStockObject ( BLACK_BRUSH );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CSG";
    
	if ( !RegisterClass ( &wc ) )
		return FALSE;

    int ScreenWidth	 = GetSystemMetrics ( SM_CXSCREEN );
    int ScreenHeight = GetSystemMetrics ( SM_CYSCREEN );

    // Create a window and display
	HWND hWnd;
    hWnd = CreateWindow( "CSG",
                        "DB Pro CSG",
						//WS_VISIBLE|WS_OVERLAPPED,
						WS_OVERLAPPED,
						0,
						0,
						10,
						10,
                        NULL,
                        NULL,
                        hInstance,
                        NULL );

    if ( !hWnd )
		return FALSE;

	//ShowWindow ( hWnd, nCmdShow );

    UpdateWindow ( hWnd );

	Init_DX ( hWnd );
	hwnd = hWnd;
	SetupFont    ( );
	InitPolygons ( );

	return TRUE;
	
}

int PASCAL WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	strcpy ( szMap, lpCmdLine );
	//strcpy ( szMap, "csg1.x" );

		char szFinal  [ 256 ];
	
	memset ( szFinal, 0, sizeof ( szFinal ) );

	for ( int iTemp = strlen ( szMap ); iTemp > 0; iTemp-- )
	{
		if ( szMap [ iTemp ] == '/' || szMap [ iTemp ] == '\\' )
		{
			memcpy ( szFinal, &szMap [ iTemp + 1 ], sizeof ( char ) * iTemp );

			break;
		}
	}

	strcpy ( szMap, szFinal );

//MessageBox ( NULL, szFinal, "info", MB_OK );

	//MessageBox ( NULL, szMap, "info", MB_OK );
	//MessageBox ( NULL, lpCmdLine, "lpCmdLine", MB_OK );

	hInst		  = hInstance;
    hPrevInstance = hPrevInstance;

	MSG			msg;

	BOOL		perf_flag	= FALSE;
	LONGLONG	last_time	= 0;
	LONGLONG	cur_time;
	LONGLONG	perf_cnt;
	float		time_scale;
	
	if ( !doInit ( hInst, nCmdShow ) ) 
		return FALSE;
	
	if ( QueryPerformanceFrequency ( ( LARGE_INTEGER* ) &perf_cnt ) )
	{ 
		QueryPerformanceCounter ( ( LARGE_INTEGER* ) &last_time );
		time_scale	= 1.0f / perf_cnt;
		perf_flag	= TRUE;
	}
	else
	{
		last_time	= timeGetTime ( );
		time_scale	= 0.001f;
	} 	

	// SetCursorPos ( 320,200 );
    // ShowCursor ( FALSE );

    BOOL bGotMsg;

	while ( quit != 1 )
	{
		while ( bGotMsg = PeekMessage ( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage ( &msg );
            DispatchMessage  ( &msg );
        }

		if ( rendering == 1 )
		{
			if ( perf_flag ) 
				QueryPerformanceCounter ( ( LARGE_INTEGER* ) &cur_time );
			else 
				cur_time = timeGetTime ( ); 

			time_elapsed = ( cur_time - last_time ) * time_scale;
			last_time	 = cur_time;
	
			// CheckInput  ( );
			// renderframe ( );
		}
    }

	ShowCursor ( TRUE );

	return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch ( uMsg )
	{
		case WM_DESTROY:
			rendering	= 0;
			FreeBrushes ( Brushes, BrushCount );

			if ( FinalBSP ) delete FinalBSP;
			
			for ( i = 0; i < 3; i++ )
			{
				lpTextureSurface [ i ]->Release ( );
			}

			lpDevice->Release ( );
			lpD3D->Release ( );
			PostQuitMessage ( 0 );
           break;

		case WM_MOUSEMOVE:
			SetCursor ( NULL );
		break;

	
		case WM_KEYUP: 
	        switch ( wParam )
			{
				case VK_ESCAPE:
					quit = 1;
					DestroyWindow ( hWnd );
				break;
			}
			break;
		default:
            return DefWindowProc ( hWnd, uMsg, wParam, lParam );
    }
	return 0L;
}

BOOL Init_DX( HWND hWnd )
{
	D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory ( &d3dpp, sizeof ( d3dpp ) );
    d3dpp.Windowed   = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
    d3dpp.BackBufferWidth	= SCREENWIDTH;
	d3dpp.BackBufferHeight	= SCREENHEIGHT;
	d3dpp.BackBufferFormat	= D3DFMT_R5G6B5;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	
	lpD3D = Direct3DCreate8 ( D3D_SDK_VERSION ) ;
   

	if( FAILED( lpD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                     D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                     &d3dpp, &lpDevice ) ) )
	{
		
		d3dpp.BackBufferFormat = D3DFMT_X1R5G5B5;

		if ( FAILED ( lpD3D->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                         &d3dpp, &lpDevice ) ) )
										 return false;

	}

	D3DCAPS8 DevCaps;
	ZeroMemory( &DevCaps, sizeof ( D3DCAPS8 ) );
	lpDevice->GetDeviceCaps ( &DevCaps );


	return TRUE;
}



void renderframe ( void )
{


}


void UpdateViewpos ( void )
{  

	
}


void CheckInput ( void )
{

}
 
void SetupFont ( void )
{
	


}


void sleep ( clock_t wait )
{
   clock_t  goal;
   goal = wait + clock ( );

   while ( goal > clock ( ) );

}

BOOL EpsilonCompareVector ( D3DXVECTOR3 Vec1, D3DXVECTOR3 Vec2 ) 
{
	if ( ( fabsf ( Vec1.x - Vec2.x ) <= g_EPSILON ) &&
		 ( fabsf ( Vec1.y - Vec2.y ) <= g_EPSILON ) &&
		 ( fabsf ( Vec1.z - Vec2.z ) <= g_EPSILON )  )
		 return TRUE;

	return FALSE;
}

void FreeBrushes ( BRUSH* lpBrushes, WORD MeshCount )
{
	if ( lpBrushes )
	{
		for ( int b = 0; b < MeshCount; b++ )
		{
			if ( lpBrushes [ b ].Faces )
			{
				for ( ULONG f = 0; f < lpBrushes [ b ].FaceCount; f++ )
				{
					delete [ ] lpBrushes [ b ].Faces [ f ].Vertices;
					delete [ ] lpBrushes [ b ].Faces [ f ].Indices;
				}

				free ( lpBrushes [ b ].Faces );
			}

			if ( lpBrushes [ b ].BSPTree )
				delete ( ( CBSPTree* ) lpBrushes [ b ].BSPTree );
		}

		delete [ ] lpBrushes;
	}
}