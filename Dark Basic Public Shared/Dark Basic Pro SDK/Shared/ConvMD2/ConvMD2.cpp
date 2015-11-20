
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "ConvMD2.h"
#include < vector >				// using STL vector
using namespace std;			// namespace info

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DBP Format uses ObjectManager
#include ".\..\Objects\CObjectManagerC.h"
CObjectManager m_ObjectManager;
bool CObjectManager::Setup ( void ) { return false; }
bool CObjectManager::Free ( void ) { return false; }


// Globals for DBO/Manager relationship
#include <vector>
std::vector< sMesh* >		g_vRefreshMeshList;
std::vector< sObject* >     g_vAnimatableObjectList;
int							g_iSortedObjectCount;
sObject**					g_ppSortedObjectList;

typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN )	( void );

DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;		
DBPRO_GLOBAL HINSTANCE						g_hSetup						= NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D							= NULL;
DBPRO_GLOBAL sObject*						g_pObject						= NULL;
DBPRO_GLOBAL FILE*							g_FP							= NULL;
DBPRO_GLOBAL UCHAR*							g_ucpBuffer						= NULL;
DBPRO_GLOBAL UCHAR*							g_ucpBufferData					= NULL;
DBPRO_GLOBAL sMD2Header						g_MD2Header; 
DBPRO_GLOBAL sMD2Frame*						g_MD2Frames						= NULL;
DBPRO_GLOBAL sMD2Triangle*					g_MD2Triangles					= NULL;
DBPRO_GLOBAL sMD2TexCoord*					g_MD2TexCoords					= NULL;
DBPRO_GLOBAL sMD2Skin*						g_MD2Skins						= NULL;
DBPRO_GLOBAL sMD2Vertex*					g_MD2Verts						= NULL;
DBPRO_GLOBAL GlobStruct*					g_pGlob							= NULL;
DBPRO_GLOBAL sObject**						g_ObjectList;
DBPRO_GLOBAL float							g_fShrinkObjectsTo				= 0.0f;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool GetFileData           ( char* szFilename );
DARKSDK bool GetHeader             ( void );
DARKSDK bool GetFrames             ( void );
DARKSDK bool GetTriangles          ( void );
DARKSDK bool GetTextureCoordinates ( void );
DARKSDK bool CreateDBO             ( void );
DARKSDK bool SetupFrame            ( sFrame* pFrame, int iFrame );

DARKSDK bool GetD3DFromModule ( HINSTANCE hSetup )
{
	#ifndef DARKSDK_COMPILE
	// get module
	if ( !hSetup )
	{
		g_hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
		if ( !g_hSetup ) return false;
		hSetup = g_hSetup;
	}
	#endif

	// get d3d ptr
	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	// okay
	return true;
}

sMD2Frame::~sMD2Frame ( )
{
	///////////////////////////////////////////////////////////////
	// MIKE 130503 - clear up any previously allocated memory
	///////////////////////////////////////////////////////////////

	SAFE_DELETE_ARRAY ( pVerts );

	///////////////////////////////////////////////////////////////
}

DARKSDK bool Delete ( void )
{
	///////////////////////////////////////////////////////////////
	// MIKE 130503 - clear up any previously allocated memory
	///////////////////////////////////////////////////////////////

	SAFE_DELETE_ARRAY ( g_MD2Frames );
	SAFE_DELETE_ARRAY ( g_MD2Triangles );
	SAFE_DELETE_ARRAY ( g_MD2TexCoords );
	SAFE_DELETE_ARRAY ( g_MD2Skins );
	SAFE_DELETE_ARRAY ( g_MD2Verts );

	SAFE_DELETE ( g_pObject );

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	return true;
};

DARKSDK bool Load ( char* szFilename )
{
	// load an MD2 file and convert into a DBO object

	if ( ! GetFileData ( szFilename ) )
		return false;

	if ( ! GetHeader ( ) )
		return false;

	if ( ! GetFrames ( ) )
		return false;

	if ( ! GetTriangles ( ) )
		return false;

	if ( ! GetTextureCoordinates ( ) )
		return false;

	if ( ! CreateDBO ( ) )
		return false;
	
	return true;
}

DARKSDK bool GetFileData ( char* szFilename )
{
	// extract the data from the file

	// used for size of file
	int iFileSize;
	
	// open the file
	g_FP = fopen ( szFilename, "rb" );

	// check it was opened
	SAFE_MEMORY ( g_FP );

	// move to the end of the file and get the size
	fseek ( g_FP, 0, SEEK_END );
	iFileSize = ftell ( g_FP );
	fseek ( g_FP, 0, SEEK_SET );

	// allocate memory for the data
	g_ucpBuffer     = new UCHAR [ iFileSize ];
	g_ucpBufferData = g_ucpBuffer;
	
	// make sure allocation was okay
	SAFE_MEMORY ( g_ucpBuffer );

	// read data into the buffer
	if ( fread ( g_ucpBuffer, 1, iFileSize, g_FP ) != ( unsigned ) iFileSize )
		return false;

	// close the file
	fclose ( g_FP );

	return true;
}

DARKSDK bool GetHeader ( void )
{
	// get the header
	memcpy ( &g_MD2Header, g_ucpBufferData, sizeof ( sMD2Header ) );

	// see if the header is valid
	if ( g_MD2Header.iMagicNum != 844121161 || g_MD2Header.iVersion != 8 )
		return false;
	
	return true;
}

DARKSDK bool GetFrames ( void )
{
	// get frame data from the model

	// move the data pointer to the frames
	g_ucpBufferData += g_MD2Header.iOffsetFrames;

	// allocate frame memory
	g_MD2Frames = new sMD2Frame [ g_MD2Header.iNumFrames ];

	// check the memory was allocated
	SAFE_MEMORY ( g_MD2Frames );

	for ( int iFrame = 0; iFrame < g_MD2Header.iNumFrames; iFrame++ )
	{
		float fScale [ 3 ];
		float fTrans [ 3 ];

		// allocate vertex list
		g_MD2Frames [ iFrame ].pVerts = new sMD2Vertex [ g_MD2Header.iNumVertices ];

		// check memory is valid
		SAFE_MEMORY ( g_MD2Frames [ iFrame ].pVerts );

		// expand the verices
		memcpy ( fScale, g_ucpBufferData,      12 );
		memcpy ( fTrans, g_ucpBufferData + 12, 12 );

		// copy the name
		memcpy ( g_MD2Frames [ iFrame ].szName, g_ucpBufferData + 24, 16 );

		// move the data pointer to the vertices
  		g_ucpBufferData += 40;

		// run through all of the vertices
		for ( int iVertex = 0; iVertex < g_MD2Header.iNumVertices; iVertex++ )
		{
			// get data
			g_MD2Frames [ iFrame ].pVerts [ iVertex ].fVert [ 0 ] = g_ucpBufferData [ 0 ] * fScale [ 0 ] + fTrans [ 0 ];
			g_MD2Frames [ iFrame ].pVerts [ iVertex ].fVert [ 1 ] = g_ucpBufferData [ 2 ] * fScale [ 2 ] + fTrans [ 2 ];
			g_MD2Frames [ iFrame ].pVerts [ iVertex ].fVert [ 2 ] = g_ucpBufferData [ 1 ] * fScale [ 1 ] + fTrans [ 1 ];
		
			// reserved data
			g_MD2Frames [ iFrame ].pVerts [ iVertex ].ucReserved = g_ucpBufferData [ 3 ];

			// move data pointer
			g_ucpBufferData += 4;
		}
	}

	return true;
}

DARKSDK bool GetTriangles ( void )
{
	// move the data pointer to the triangles
	g_ucpBufferData  = g_ucpBuffer;
	g_ucpBufferData += g_MD2Header.iOffsetTriangles;
	
	// allocate triangle memory
	g_MD2Triangles = new sMD2Triangle [ g_MD2Header.iNumTriangles ];

	// check memory was allocated
	SAFE_MEMORY ( g_MD2Triangles );

	// copy memory across
	memcpy ( g_MD2Triangles, g_ucpBufferData, 12 * g_MD2Header.iNumTriangles );
	
	return true;
}

DARKSDK bool GetTextureCoordinates ( void )
{
	// get the texture coordinates

	short* sTexCoords = NULL;

	// move data pointer to the start of the file and then
	// go to the texture coordinate block
	g_ucpBufferData  = g_ucpBuffer;
	g_ucpBufferData += g_MD2Header.iOffsetTexCoords;

	// allocate memory to store coordinates
	g_MD2TexCoords = new sMD2TexCoord [ g_MD2Header.iNumTexCoords     ];
	sTexCoords     = new short        [ g_MD2Header.iNumTexCoords * 2 ];

	// check memory
	SAFE_MEMORY ( g_MD2TexCoords );
	SAFE_MEMORY ( sTexCoords );

	// copy texture array to temporary buffer
	memcpy ( sTexCoords, g_ucpBufferData, 4 * g_MD2Header.iNumTexCoords );

	// copy data across to main texture coordinate list
	for ( int iIndex = 0; iIndex < g_MD2Header.iNumTexCoords; iIndex++ )
	{
		g_MD2TexCoords [ iIndex ].fTex [ 0 ] = ( float ) sTexCoords [ 2 * iIndex     ] / g_MD2Header.iSkinWidthPx;
		g_MD2TexCoords [ iIndex ].fTex [ 1 ] = ( float ) sTexCoords [ 2 * iIndex + 1 ] / g_MD2Header.iSkinHeightPx;
	}
	
	// delete array
	SAFE_DELETE_ARRAY ( sTexCoords );

	///////////////////////////////////////////////////////////////
	// MIKE 130503 - will now load in texture data from file
	///////////////////////////////////////////////////////////////

	g_ucpBufferData  = g_ucpBuffer;								// store current buffer pointer
	g_ucpBufferData += g_MD2Header.iOffsetSkins;				// move to the skins
	g_MD2Skins       = new sMD2Skin [ g_MD2Header.iNumSkins ];	// allocate skin memory

	// check the skins array was allocated
	SAFE_MEMORY ( g_MD2Skins );

	// go through each texture
	for ( int iTexture = 0; iTexture < g_MD2Header.iNumSkins; iTexture++ )
	{
		// copy the name to the array
		memcpy ( g_MD2Skins [ iTexture ].caSkin, g_ucpBufferData, 64 );
		
		// find the last '/' in the string
		char* szEnd = strrchr ( g_MD2Skins [ iTexture ].caSkin, '/' );
		
		// now copy the end filename
		if ( szEnd )
		{
			szEnd++;
			strcpy ( g_MD2Skins [ iTexture ].caSkin, szEnd );
		}

		// all MD2's will have ".pcx" as the texture so we replace and default to ".bmp"
		g_MD2Skins [ iTexture ].caSkin [ strlen ( g_MD2Skins [ iTexture ].caSkin ) - 3 ] = 'b';
		g_MD2Skins [ iTexture ].caSkin [ strlen ( g_MD2Skins [ iTexture ].caSkin ) - 2 ] = 'm';
		g_MD2Skins [ iTexture ].caSkin [ strlen ( g_MD2Skins [ iTexture ].caSkin ) - 1 ] = 'p';

		// move the buffer pointer
		g_ucpBufferData += 64;
	}

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	return true;
}

DARKSDK bool CreateDBO ( void )
{
	// create a DBO object with the data

	// create the main object
	g_pObject = new sObject;
	SAFE_MEMORY ( g_pObject );

	// now the frame
	g_pObject->pFrame = new sFrame;
	SAFE_MEMORY ( g_pObject->pFrame );

	// and finally the mesh
	g_pObject->pFrame->pMesh = new sMesh;
	SAFE_MEMORY ( g_pObject->pFrame->pMesh );

	// get a pointer to the mesh, easier for us to access
	sMesh* pMesh = g_pObject->pFrame->pMesh;
	sFrame* pFrame = g_pObject->pFrame;

	SetupFrame ( pFrame, 0 );

	pMesh->pTextures = new sTexture [ 1 ];
	
	if ( g_MD2Header.iNumSkins > 0 )
		strcpy ( pMesh->pTextures->pName, g_MD2Skins [ 0 ].caSkin );

	pMesh->dwTextureCount = 1;

	// set up sub frames
	pMesh->pSubFrameList = new sFrame [ g_MD2Header.iNumFrames ];

	// check the new memory
	SAFE_MEMORY ( pMesh->pSubFrameList );

	// store the number of frames
	pMesh->dwSubMeshListCount = g_MD2Header.iNumFrames;

	for ( DWORD iFrame = 0; iFrame < pMesh->dwSubMeshListCount; iFrame++ )
	{
		pMesh->pSubFrameList [ iFrame ].pMesh = new sMesh;
		SetupFrame ( &pMesh->pSubFrameList [ iFrame ], iFrame );
	}

	return true;
}

DARKSDK bool SetupFrame ( sFrame* pFrame, int iFrame )
{
	SAFE_MEMORY ( pFrame );

	// get a pointer to the mesh, easier for us to access
	sMesh* pMesh = pFrame->pMesh;
	
	// set up mesh properties
	pMesh->dwVertexCount    = g_MD2Header.iNumTriangles * 3;				// number of vertices
	pMesh->dwIndexCount     = 0;											// number of indices
	pMesh->iDrawVertexCount = g_MD2Header.iNumTriangles;					// number of vertices to draw
	pMesh->iDrawPrimitives  = g_MD2Header.iNumTriangles;					// number of primitives to draw
	pMesh->dwFVFOriginal    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;		// original fvf
	pMesh->dwFVF		    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;		// current fvf
	pMesh->dwFVFSize        = sizeof ( float ) * 8;							// fvf size

	// create FVF space
	SetupMeshFVFData ( pMesh, pMesh->dwFVF, pMesh->dwVertexCount, pMesh->dwIndexCount );

	// get the offset map for the vertex data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	int iVertexPosition = 0;
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount / 3; iVertex++ )
	{
		for ( int iTriangle = 0; iTriangle < 3; iTriangle++ )
		{
			// get position and texture coordinates
			float fX  = g_MD2Frames [ iFrame ].pVerts [ g_MD2Triangles [ iVertex ].usVertexIndices  [ iTriangle ] ].fVert [ 0 ];
			float fY  = g_MD2Frames [ iFrame ].pVerts [ g_MD2Triangles [ iVertex ].usVertexIndices  [ iTriangle ] ].fVert [ 1 ];
			float fZ  = g_MD2Frames [ iFrame ].pVerts [ g_MD2Triangles [ iVertex ].usVertexIndices  [ iTriangle ] ].fVert [ 2 ];
			float fTU = g_MD2TexCoords                [ g_MD2Triangles [ iVertex ].usTextureIndices [ iTriangle ] ].fTex  [ 0 ];
			float fTV = g_MD2TexCoords                [ g_MD2Triangles [ iVertex ].usTextureIndices [ iTriangle ] ].fTex  [ 1 ];

			// copy data across
			*( ( float* ) pMesh->pVertexData + offsetMap.dwX        + ( offsetMap.dwSize * iVertexPosition ) ) = fX;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwY        + ( offsetMap.dwSize * iVertexPosition ) ) = fY;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwZ        + ( offsetMap.dwSize * iVertexPosition ) ) = fZ;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iVertexPosition ) ) = fTU;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iVertexPosition ) ) = fTV;

			// next vertex
			iVertexPosition++;
		}
	}

	// leefix - 150503 - generate normals for mesh
	GenerateNormals ( pMesh );

	// remove redundant index data created (no benefit to MD2)
	SAFE_DELETE ( pMesh->pIndices );
	pMesh->dwIndexCount = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// CONVERT FUNCTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

bool	Convert		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
void	Free		( LPSTR );

bool ConvertMD2 ( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	return Convert ( pFilename, pBlock, pdwSize );
}

void FreeMD2 ( LPSTR pBlock )
{
	Free ( pBlock );
}

#endif

DARKSDK bool Convert ( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	// obtain D3D
	GetD3DFromModule ( NULL );

	// load object
	if ( !Load ( pFilename ) )
	{
		// Failed to load
		Free ( NULL );
		return false;
	}

	// create block
	if ( !DBOConvertObjectToBlock ( g_pObject, pBlock, pdwSize ) )
	{
		// Failed to create
		Free ( NULL );
		return false;
	}

	// okay
	return true;
}

DARKSDK void Free ( LPSTR pBlock )
{
	// delete block
	SAFE_DELETE(pBlock);

	// free object
	Delete();

	// free local DLL hook
	if ( g_hSetup )
	{
		FreeLibrary ( g_hSetup );
		g_hSetup = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// TO STOP LINKER ERRORS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK float wrapangleoffset(float da)
{
	int breakout=100;
	while(da<0.0f || da>=360.0f)
	{
		if(da<0.0f) da=da+360.0f;
		if(da>=360.0f) da=da-360.0f;
		breakout--;
		if(breakout==0) break;
	}
	if(breakout==0) da=0.0f;
	return da;
}

DARKSDK void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az)
{
	D3DXVECTOR3 Vector;
	Vector.x = x2-x1;
	Vector.y = y2-y1;
	Vector.z = z2-z1;

	// Find Y and then X axis rotation
	double yangle=atan2(Vector.x, Vector.z);
	if(yangle<0.0) yangle+=D3DXToRadian(360.0);
	if(yangle>=D3DXToRadian(360.0)) yangle-=D3DXToRadian(360.0);

	D3DXMATRIX yrotate;
	D3DXMatrixRotationY ( &yrotate, (float)-yangle );
	D3DXVec3TransformCoord ( &Vector, &Vector, &yrotate );

	double xangle=-atan2(Vector.y, Vector.z);
	if(xangle<0.0) xangle+=D3DXToRadian(360.0);
	if(xangle>=D3DXToRadian(360.0)) xangle-=D3DXToRadian(360.0);

	*ax = wrapangleoffset(D3DXToDegree((float)xangle));
	*ay = wrapangleoffset(D3DXToDegree((float)yangle));
	*az = 0.0f;
}

DARKSDK void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
	// Calculate angle vector based on matrix
	D3DXVECTOR3 pDirVec = D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f );
	D3DXVec3TransformCoord ( &pDirVec, &pDirVec, pmatMatrix );
	GetAngleFromPoint ( 0.0f, 0.0f, 0.0f, pDirVec.x, pDirVec.y, pDirVec.z, &(pVecAngles->x), &(pVecAngles->y), &(pVecAngles->z));
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////