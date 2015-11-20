//#include "DBOFormat.h"

#include "..\\DBOFormat\\DBOBlock.h"

#include ".\..\DBOFormat\DBOFormat.h"
#include ".\..\DBOFormat\DBOFrame.h"
#include ".\..\DBOFormat\DBOMesh.h"
#include ".\..\DBOFormat\DBORawMesh.h"
#include ".\..\DBOFormat\DBOEffects.h"
#include ".\..\DBOFormat\DBOFile.h"

#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#else
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
	#define DARKSDK_DLL 
#endif

//////////////////////////////////////////////////////////////////////////////////
// MD3 STRUCTURES ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct MD3Header
{
	int		iID;
	int		iVersion;
	char	szFilename [ 68 ];
	int		iNumFrames;
	int		iNumTags;
	int		iNumMeshes;
	int		iMaxSkins;
	int		iHeaderSize;
	int		iTagStart;
	int		iTagEnd;
	int		iFileSize;
};

struct MD3BoneFrame
{
	float fMin [ 3 ];
	float fMax [ 3 ];
};

struct MD3Tag
{
	char			szName [ 64 ];
	D3DXVECTOR3		vecPos;
	D3DXQUATERNION	qRot;
};

struct MD3MeshHeader
{
	char	szMeshId [ 4 ];
	char	szName [ 68 ];
	int		iNumMeshFrames;
	int		iNumSkins;
	int		iNumVerts;
	int		iNumTriangles;
	int		iTriOffset;
	int		iHeaderSize;
	int		iUVOffset;
	int		iVertexOffset;
	int		iMeshSize;
};

struct MD3Vertex 
{
	float x, y, z;
};

struct MD3Face
{
	unsigned int uiIndices [ 3 ];
};

struct MD3TexCoord
{
	float fTexCoord [ 2 ];
};

struct MD3Skin
{
	char				szName [ 68 ];
	LPDIRECT3DTEXTURE9	pTexture;
};

struct MD3KeyFrame
{
	MD3Vertex* pVertices;

	~MD3KeyFrame ( )
	{
		SAFE_DELETE_ARRAY ( pVertices );
	}
};

struct MD3Mesh
{
	MD3MeshHeader	Header;
	MD3KeyFrame*	pKeyFrames;
	MD3Face*		pFaces;
	MD3TexCoord*	pTexCoords;
	MD3Skin*		pSkins;
	MD3Vertex*		pInterp;

	~MD3Mesh ( )
	{
		SAFE_DELETE_ARRAY ( pKeyFrames );
		SAFE_DELETE_ARRAY ( pFaces );
		SAFE_DELETE_ARRAY ( pTexCoords );
		SAFE_DELETE_ARRAY ( pSkins );
		SAFE_DELETE_ARRAY ( pInterp );
	}
};

struct MD3
{
	MD3Header	Header;
	MD3Tag*		pTags;
	MD3Mesh*	pMeshes;
	MD3Tag*		pInterpTags;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// MD3 FUNCTIONS /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool Load   ( char* szFilename );
DARKSDK bool Draw   ( void );
DARKSDK bool Delete ( void );

#ifndef DARKSDK_COMPILE
extern "C"
{
	DARKSDK	bool	Convert		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	DARKSDK void	Free		( LPSTR );
}
#else
	bool	ConvertMD3		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	void	FreeMD3			( LPSTR );
#endif
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

