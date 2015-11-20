
#define NAME "CSG"
#define TITLE "CSG"
#define WIN32_LEAN_AND_MEAN

#include "CSG.h"
#include "BSPTree.h"

#include "cEnhancedXC.h"

char szMap [ 256 ];

#include ".\..\error\cerror.h"

#if DB_PRO
	#include ".\..\core\globstruct.h"

	// Global Shared Data Pointer (passed in from core)
	GlobStruct*				g_pGlob							= NULL;
#endif

int g_iProgress = 0;

FLOAT g_EPSILON = 0.1f;

LPDIRECT3DDEVICE9       lpDevice			= NULL;
LPDIRECT3DTEXTURE9*		lpTextureSurface;
BRUSH*					Brushes				= NULL;
int						BrushCount			= 0;
int						NumberOfTextures	= 3;
int						g_iCSGBrush			= 0;
int						g_iCSGMode			= CSG_NONE;
int						g_iLastTexture		= -1;
BOOL					g_bWireFrame		= FALSE;
BOOL					g_bSafeTransform	= TRUE;
BOOL					g_bCompileFinal		= FALSE;
BOOL					g_bHSR				= TRUE;
CBSPTree				*FinalBSP			= NULL;

void				InitPolygons           ( void );
void				UpdateBSPTrees         ( void );
BBOX				CalculateBoundingBox   ( BRUSH* brush, D3DXMATRIX* Matrix );
BOOL				BoundingBoxesIntersect ( BBOX BoxA, BBOX BoxB );
void				FreeBrushes            ( BRUSH* lpBrushes, WORD MeshCount );

typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN ) ( void );
GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;

void ConstructorCSG ( HINSTANCE hSetup )
{
	if ( !hSetup )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
	}

	// setup function pointers
	g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	lpDevice                  = g_GFX_GetDirect3DDevice ( );

	g_iProgress = 0;
}

void DestructorCSG ( void )
{
	SAFE_DELETE_ARRAY ( lpTextureSurface );

	FreeBrushes ( Brushes, BrushCount );
}

void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

void PassCoreData( LPVOID pGlobPtr )
{
	// not needed
}


int* GetProgress ( void )
{
	return &g_iProgress;
}

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

tagModelData* ptr;

BRUSH* ConvertXModelToBrushFormat ( char* szFilename, BRUSH* pBrush, int* piCount, LPDIRECT3DDEVICE9 lpDevice )
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
	
	LPDIRECT3DVERTEXBUFFER9 pMeshVertexBuffer;	// vertex buffer
	LPDIRECT3DINDEXBUFFER9  pMeshIndexBuffer;	// index buffer
	sMeshData*				pMeshVertices;		// mesh vertices
	WORD*					pMeshIndices;		// mesh indices
	sMesh*					pMesh;				// mesh data
	int						iCount;
	int						iTextures;
	
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
	
	lpTextureSurface = new LPDIRECT3DTEXTURE9 [ iCount ];

	iCount = 0;

	pMesh = ptr->m_Object.m_Meshes;

	while ( pMesh )
	{
		D3DXCreateTextureFromFile ( lpDevice, pMesh->m_TextureName,	&lpTextureSurface [ iCount ] ); 

		pMesh = pMesh->m_Next;
		iCount++;
	}

	NumberOfTextures = iCount;

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

	int iTex = 0;

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
			pBrush [ iCount ].Faces [ iTemp ].TextureIndex = iCount;
			pBrush [ iCount ].Faces [ iTemp ].VertexCount  = 3;
			pBrush [ iCount ].Faces [ iTemp ].Indices      = new WORD      [ 3 ];
			pBrush [ iCount ].Faces [ iTemp ].Vertices     = new D3DVERTEX [ 3 ];

			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 0 ] = SetupVertex ( pMeshVertices [ iA ].x, pMeshVertices [ iA ].y, pMeshVertices [ iA ].z, pMeshVertices [ iA ].tu, pMeshVertices [ iA ].tv );
			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 1 ] = SetupVertex ( pMeshVertices [ iB ].x, pMeshVertices [ iB ].y, pMeshVertices [ iB ].z, pMeshVertices [ iB ].tu, pMeshVertices [ iB ].tv );
			pBrush [ iCount ].Faces [ iTemp ].Vertices [ 2 ] = SetupVertex ( pMeshVertices [ iC ].x, pMeshVertices [ iC ].y, pMeshVertices [ iC ].z, pMeshVertices [ iC ].tu, pMeshVertices [ iC ].tv );

			for ( int iChar = 0; iChar < 3; iChar++ )
			{
				//sprintf ( szX, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].x );
				//sprintf ( szY, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].y );
				//sprintf ( szZ, "%.1f", pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].z );

				//pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].x = atof ( szX );
				//pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].y = atof ( szY );
				//pBrush [ iCount ].Faces [ iTemp ].Vertices [ iChar ].z = atof ( szZ );
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

	/*
	iCount = 0;
	pMesh  = ptr->m_Object.m_Meshes;

	for ( int iTemp = ptr->m_Object.m_NumFrames; iTemp > 0; iTemp-- )
	{
		sFrame* pFrame = ptr->m_Object.m_Frames->FindFrame ( iTemp );

		if ( pFrame )
		{
			pBrush [ iCount ].Matrix = pFrame->m_matOriginal;

			iCount++;
		}
	}
	*/

		iCount = 0;
	pMesh  = ptr->m_Object.m_Meshes;

	for ( int iTemp = ptr->m_Object.m_NumFrames; iTemp > 0; iTemp-- )
	{
		sFrame* pFrame = ptr->m_Object.m_Frames->FindFrame ( iTemp );

		if ( pFrame )
		{
			//if ( iCount < BrushCount )
				pBrush [ iCount ].Matrix = pFrame->m_matOriginal;

			//pBrush [ iCount ].Matrix._41 = pFrame->m_matOriginal._41;
			//pBrush [ iCount ].Matrix._42 = pFrame->m_matOriginal._42;
			//pBrush [ iCount ].Matrix._43 = pFrame->m_matOriginal._43;

			iCount++;
		}
	}
	
	
	return pBrush;
}

void PerformCSGOnXModel ( char* szInput, char* szOutput, float fEpsilon )
{
	g_EPSILON = fEpsilon;

	Brushes = ConvertXModelToBrushFormat ( szInput, Brushes, &BrushCount, lpDevice );
	
	//BrushCount = 12;

	for ( int iTemp = 0; iTemp < BrushCount; iTemp++ )
	{
		Brushes [ iTemp ].Bounds = CalculateBoundingBox ( &Brushes [ iTemp ], &Brushes [ iTemp ].Matrix );

		for ( ULONG f = 0; f < Brushes [ iTemp ].FaceCount; f++ )
		{
			Brushes [ iTemp ].Faces [ f ].Normal = CalculatePolyNormal (
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ 0 ] ], 
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ 1 ] ], 
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ Brushes [ iTemp ].Faces [ f ].IndexCount - 1 ] ], 
																	&Brushes [ iTemp ].Matrix
																	);
		
			for ( int v = 0; v < Brushes [ iTemp ].Faces [ f ].VertexCount; v++ )
			{
				Brushes [ iTemp ].Faces [ f ].Vertices [ v ].Normal = Brushes [ iTemp ].Faces [ f ].Normal;
			}
		}
	}
	
	

	UpdateBSPTrees ( );

	

	/*
	///////////////////////////////////////////////////////////////////////////////////////
	// crap - used the wrong list of polygons..................
	char szText [ 256 ];
	FILE* fp = fopen ( szOutput, "wt" );

	sprintf ( szText, "xof 0302txt 0032\n" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	sprintf ( szText, "Header {\n1;\n0;\n1;\n}\n" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	for ( int iTemp = 0; iTemp < BrushCount; iTemp++ )
	{
		BRUSH* pBrush = &Brushes [ iTemp ];

		//////////////////////
		// material
		sprintf ( szText, "Material material_%d {\n", iTemp );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "1.0;1.0;1.0;1.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;0.0;0.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;0.0;0.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "TextureFilename {\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "\"" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "brick.bmp" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "\";\n}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		//////////////////////

		//////////////////////
		// world matix
		sprintf ( szText, "Frame world {\nFrameTransformMatrix {\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "1.0,0.0,0.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,1.0,0.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,0.0,1.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,0.0,0.0,1.0;;\n}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		//////////////////////

		//////////////////////
		// object matrix
		sprintf ( szText, "Frame object%d {\n", iTemp );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "FrameTransformMatrix {\n", iTemp );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._11, pBrush->Matrix._12, pBrush->Matrix._13, pBrush->Matrix._14 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._21, pBrush->Matrix._22, pBrush->Matrix._23, pBrush->Matrix._24 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._31, pBrush->Matrix._32, pBrush->Matrix._33, pBrush->Matrix._34 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f;;\n}\n", pBrush->Matrix._41, pBrush->Matrix._42, pBrush->Matrix._43, pBrush->Matrix._44 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		//////////////////////

		//////////////////////
		// mesh
		int iFaces;
		int iVertex;
		int iIndex;

		for ( iFaces = 0; iFaces < pBrush->FaceCount; iFaces++ )
		//for ( iFaces = 0; iFaces < 1; iFaces++ )
		{
			sprintf ( szText, "Mesh object%d {\n", iTemp );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			//sprintf ( szText, "%d;\n", pBrush->FaceCount * 3 );
			//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			sprintf ( szText, "%d;\n", pBrush->Faces [ iFaces ].VertexCount );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			// vertices
			for ( iVertex = 0; iVertex < pBrush->Faces [ iFaces ].VertexCount; iVertex++ )
			{
				sprintf ( szText, "%f;", pBrush->Faces [ iFaces ].Vertices [ iVertex ].x );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				sprintf ( szText, "%f;", pBrush->Faces [ iFaces ].Vertices [ iVertex ].y );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				if ( iVertex == pBrush->Faces [ iFaces ].VertexCount - 1 )
				{
					sprintf ( szText, "%f;;\n", pBrush->Faces [ iFaces ].Vertices [ iVertex ].z );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
				else
				{
					sprintf ( szText, "%f;,\n", pBrush->Faces [ iFaces ].Vertices [ iVertex ].z );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
			}

			// indices
			//sprintf ( szText, "%d;\n", pBrush->Faces [ iFaces ].IndexCount );
			sprintf ( szText, "1;\n", pBrush->Faces [ iFaces ].IndexCount );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			for ( iIndex = 0; iIndex < pBrush->Faces [ iFaces ].IndexCount; iIndex++ )
			{
				if ( iIndex == 0 )
				{
					sprintf ( szText, "%d;", pBrush->Faces [ iFaces ].IndexCount );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}

				//sprintf ( szText, "%d;", pBrush->Faces [ iFaces ].IndexCount );
				//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				if ( iIndex == pBrush->Faces [ iFaces ].IndexCount - 1 )
				{
					sprintf ( szText, "%d;;\n", pBrush->Faces [ iFaces ].Indices [ iIndex ] );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
				else
				{
					sprintf ( szText, "%d,", pBrush->Faces [ iFaces ].Indices [ iIndex ] );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
			}

			// texture coordinates
			sprintf ( szText, "MeshTextureCoords {\n" );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			sprintf ( szText, "%d;\n", pBrush->Faces [ iFaces ].VertexCount );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			for ( iVertex = 0; iVertex < pBrush->Faces [ iFaces ].VertexCount; iVertex++ )
			{
				sprintf ( szText, "%f;", pBrush->Faces [ iFaces ].Vertices [ iVertex ].tu );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				if ( iVertex == pBrush->Faces [ iFaces ].VertexCount - 1 )
				{
					sprintf ( szText, "%f;;\n", pBrush->Faces [ iFaces ].Vertices [ iVertex ].tv );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
				else
				{
					sprintf ( szText, "%f;,\n", pBrush->Faces [ iFaces ].Vertices [ iVertex ].tv );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				}
			}

			sprintf ( szText, "}\n", pBrush->Faces [ iFaces ].VertexCount );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			sprintf ( szText, "MeshMaterialList {\n1;\n1;\n0;\n{material_0}\n" );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			sprintf ( szText, "}\n}\n", pBrush->Faces [ iFaces ].VertexCount );
			fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

			
		}

		sprintf ( szText, "}\n", pBrush->Faces [ iFaces ].VertexCount );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
	}

	fclose ( fp );
	///////////////////////////////////////////////////////////////////////////////////////
	*/

	char szText [ 256 ];
	FILE* fp = fopen ( szOutput, "wt" );

	CBSPTree* pLink = NULL;
	CBSPTree* pArray [ 500 ];

	BRUSH* pBrush = &Brushes [ 0 ];
	
	int iLoop;
	int iA;
	int iPos = 0;

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

	sprintf ( szText, "xof 0302txt 0032\n" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	sprintf ( szText, "Header {\n1;\n0;\n1;\n}\n" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	
	sMesh* pMesh = ptr->m_Object.m_Meshes;

	int iTex = 0;
	while ( pMesh )
	{
		sprintf ( szText, "Material material_%d {\n", iTex++ );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "1.0;1.0;1.0;1.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;0.0;0.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0;0.0;0.0;;\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "TextureFilename {\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "\"" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, pMesh->m_TextureName );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "\";\n}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		//////////////////////

		pMesh = pMesh->m_Next;
	}



		//////////////////////
		// world matix
		sprintf ( szText, "Frame world {\nFrameTransformMatrix {\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "1.0,0.0,0.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,1.0,0.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,0.0,1.0,0.0,\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "0.0,0.0,0.0,1.0;;\n}\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		//////////////////////

		//////////////////////
		// object matrix
		/*
		sprintf ( szText, "Frame object%d {\n", iPos );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "FrameTransformMatrix {\n" );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._11, pBrush->Matrix._12, pBrush->Matrix._13, pBrush->Matrix._14 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._21, pBrush->Matrix._22, pBrush->Matrix._23, pBrush->Matrix._24 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f,\n", pBrush->Matrix._31, pBrush->Matrix._32, pBrush->Matrix._33, pBrush->Matrix._34 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

		sprintf ( szText, "%f,%f,%f,%f;;\n}\n", pBrush->Matrix._41, pBrush->Matrix._42, pBrush->Matrix._43, pBrush->Matrix._44 );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		*/
		//////////////////////

	for ( iPos = 0; iPos < BrushCount; iPos++ )
	{
		pLink = pArray [ iPos ];

		
		int iTemp = 0;
		BRUSH* pBrush = &Brushes [ iTemp ];

	
		//////////////////////
		// mesh
		int iFaces;
		int iVertex;
		int iIndex;

		for ( iFaces = 0; iFaces < pLink->PolyCount; iFaces++ )
		{
			if ( !pLink->PolygonDeleted [ iFaces ] )
			{
				sprintf ( szText, "Mesh object%d {\n", iTemp );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				sprintf ( szText, "%d;\n", pLink->Polygons [ iFaces ].VertexCount );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				// vertices
				for ( iVertex = 0; iVertex < pLink->Polygons [ iFaces ].VertexCount; iVertex++ )
				{
					sprintf ( szText, "%f;", pLink->Polygons [ iFaces ].Vertices [ iVertex ].x );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

					sprintf ( szText, "%f;", pLink->Polygons [ iFaces ].Vertices [ iVertex ].y );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

					if ( iVertex == pLink->Polygons [ iFaces ].VertexCount - 1 )
					{
						sprintf ( szText, "%f;;\n", pLink->Polygons [ iFaces ].Vertices [ iVertex ].z );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
					else
					{
						sprintf ( szText, "%f;,\n", pLink->Polygons [ iFaces ].Vertices [ iVertex ].z );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
				}

				// indices
				//sprintf ( szText, "%d;\n", pBrush->Faces [ iFaces ].IndexCount );
				sprintf ( szText, "1;\n", pLink->Polygons [ iFaces ].IndexCount );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				for ( iIndex = 0; iIndex < pLink->Polygons [ iFaces ].IndexCount; iIndex++ )
				{
					if ( iIndex == 0 )
					{
						sprintf ( szText, "%d;", pLink->Polygons [ iFaces ].IndexCount );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}

					//sprintf ( szText, "%d;", pBrush->Faces [ iFaces ].IndexCount );
					//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

					if ( iIndex == pLink->Polygons [ iFaces ].IndexCount - 1 )
					{
						sprintf ( szText, "%d;;\n", pLink->Polygons [ iFaces ].Indices [ iIndex ] );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
					else
					{
						sprintf ( szText, "%d,", pLink->Polygons [ iFaces ].Indices [ iIndex ] );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
				}

				// texture coordinates
				sprintf ( szText, "MeshTextureCoords {\n" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				sprintf ( szText, "%d;\n", pLink->Polygons [ iFaces ].VertexCount );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				for ( iVertex = 0; iVertex < pLink->Polygons [ iFaces ].VertexCount; iVertex++ )
				{
					sprintf ( szText, "%f;", pLink->Polygons [ iFaces ].Vertices [ iVertex ].tu );
					fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

					if ( iVertex == pLink->Polygons [ iFaces ].VertexCount - 1 )
					{
						sprintf ( szText, "%f;;\n", pLink->Polygons [ iFaces ].Vertices [ iVertex ].tv );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
					else
					{
						sprintf ( szText, "%f;,\n", pLink->Polygons [ iFaces ].Vertices [ iVertex ].tv );
						fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
					}
				}

				sprintf ( szText, "}\n", pLink->Polygons [ iFaces ].VertexCount );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );


				//sprintf ( szText, "MeshMaterialList {\n1;\n1;\n0;\n{material_0}\n" );
				sprintf ( szText, "MeshMaterialList {\n1;\n1;\n0;\n{material_%d}\n", pLink->Polygons [ iFaces ].TextureIndex );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				


				sprintf ( szText, "}\n}\n", pLink->Polygons [ iFaces ].VertexCount );
				
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			
		}

		sprintf ( szText, "}\n", pLink->Polygons [ iFaces ].VertexCount );
		fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
	}

	fclose ( fp );

	Delete ( 1 );
}

void PerformCSGOnXModel ( char* szInput, char* szOutput )
{
	PerformCSGOnXModel ( szInput, szOutput, 0.1f );
}

void CompileCSG ( char* szInput, char* szOutput, float fEpsilon )
{
	g_EPSILON = fEpsilon;

	Brushes = ConvertXModelToBrushFormat ( szInput, Brushes, &BrushCount, lpDevice );
	
	//BrushCount = 12;

	for ( int iTemp = 0; iTemp < BrushCount; iTemp++ )
	{
		Brushes [ iTemp ].Bounds = CalculateBoundingBox ( &Brushes [ iTemp ], &Brushes [ iTemp ].Matrix );

		for ( ULONG f = 0; f < Brushes [ iTemp ].FaceCount; f++ )
		{
			Brushes [ iTemp ].Faces [ f ].Normal = CalculatePolyNormal (
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ 0 ] ], 
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ 1 ] ], 
																	Brushes [ iTemp ].Faces [ f ].Vertices [ Brushes [ iTemp ].Faces [ f ].Indices [ Brushes [ iTemp ].Faces [ f ].IndexCount - 1 ] ], 
																	&Brushes [ iTemp ].Matrix
																	);
		
			for ( int v = 0; v < Brushes [ iTemp ].Faces [ f ].VertexCount; v++ )
			{
				Brushes [ iTemp ].Faces [ f ].Vertices [ v ].Normal = Brushes [ iTemp ].Faces [ f ].Normal;
			}
		}
	}
	
	

	UpdateBSPTrees ( );

	CBSPTree* pLink = NULL;
	CBSPTree* pArray [ 500 ];
	
	FILE* fp = NULL;
	char szText [ 256 ];
	int iLoop;
	int iA;
	int iPos = 0;

#define FILE_TEXT 0

#if FILE_TEXT
	fp = fopen ( szOutput, "wt" );
#else
	fp = fopen ( szOutput, "wb" );
#endif

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

#if FILE_TEXT
	sprintf ( szText, "%d\n", NumberOfTextures );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
#else
	fwrite ( &NumberOfTextures, sizeof ( int ), 1, fp );
#endif

	sMesh* pMesh = ptr->m_Object.m_Meshes;

	while ( pMesh )
	{
		fwrite ( pMesh->m_TextureName, sizeof ( char ) * 256, 1, fp );

		pMesh = pMesh->m_Next;
	}

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

						/*
						sprintf ( szX, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].x );
						sprintf ( szY, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].y );
						sprintf ( szZ, "%.1f", pLink->Polygons [ iLoop ].Vertices [ iA ].z );

						pLink->Polygons [ iLoop ].Vertices [ iA ].x = atof ( szX );
						pLink->Polygons [ iLoop ].Vertices [ iA ].y = atof ( szY );
						pLink->Polygons [ iLoop ].Vertices [ iA ].z = atof ( szZ );
						*/

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

//	SaveToX ( "test.x" );

	Delete ( 1 );

	g_iProgress = 100;
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

		g_iProgress++;
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

			g_iProgress++;
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

			g_iProgress++;
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