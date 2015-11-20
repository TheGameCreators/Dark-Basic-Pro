#ifndef _CMATRIX_H_
#define _CMATRIX_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3dx9.lib"  )

#include <D3DX9.h>

#include "cmatrixmanagerc.h"
#include "cmatrixdatac.h"
#include ".\..\position\cpositionc.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void UpdateMatrix			( void );
		void UpdateLastMatrix		( void );
		void ConstructorMatrix		( HINSTANCE hSetup, HINSTANCE hImage );
		void DestructorMatrix		( void );
		void SetErrorHandlerMatrix	( LPVOID pErrorHandlerPtr );
		void PassCoreDataMatrix		( LPVOID pGlobPtr );
		void RefreshD3DMatrix		( int iMode );
#endif

DARKSDK void Update					( void );
DARKSDK void UpdateLast				( void );
DARKSDK void Constructor			( HINSTANCE hSetup, HINSTANCE hImage );
DARKSDK void Destructor				( void );
DARKSDK void SetErrorHandler		( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData			( LPVOID pGlobPtr );
DARKSDK void RefreshD3D				( int iMode );
DARKSDK void SetHeightReal			( int iID, tagMatrixVertexDesc* pData, int iX, int iZ, float fHeight );
DARKSDK void SetTileReal			( int iID, tagMatrixVertexDesc* pData, int iX, int iZ, int iTile );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool  Make					( int iID, int iWidth, int iDepth,  int iXSegmented, int iZSegmented );

DARKSDK void  SetTransparency		( int iID, bool bFlag    );
DARKSDK void  SetCull				( int iID, bool bCull    );
DARKSDK void  SetFilter				( int iID, int  iFilter  );
DARKSDK void  SetLight				( int iID, bool bLight   );
DARKSDK void  SetFog				( int iID, bool bFog     );
DARKSDK void  SetAmbient			( int iID, bool bAmbient );

DARKSDK void  Set					( int iID, bool bWireframe, bool bTransparency, bool bCull );
DARKSDK void  Set					( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter );
DARKSDK void  Set					( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight );
DARKSDK void  Set					( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog );
DARKSDK void  Set					( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog, bool bAmbient );

DARKSDK void  SetTransparency		( int iID, bool bFlag );
DARKSDK void  SetWireframe			( int iID, bool bFlag );

DARKSDK void  SetSphereMap			( int iID, int iSphereImage );
DARKSDK void  SetCubeMap			( int iID, int iCubeImage );
DARKSDK void  SetBumpMap			( int iID, int iEnvironmentImage, int iBumpMap );

DARKSDK int   GetWidth				( int iID );
DARKSDK int   GetDepth				( int iID );
DARKSDK int   GetXSegments			( int iID );
DARKSDK int   GetZSegments			( int iID );
DARKSDK int   GetTilesAcross		( int iID );
DARKSDK int   GetTilesDown			( int iID );
DARKSDK int   GetFilter				( int iID );
DARKSDK int   GetGhostMode			( int iID );
DARKSDK bool  GetVisible			( int iID );
DARKSDK bool  GetWireframe			( int iID );
DARKSDK bool  GetLight				( int iID );
DARKSDK bool  GetCull				( int iID );
DARKSDK bool  GetTransparency		( int iID );
DARKSDK bool  GetZDepth				( int iID );
DARKSDK bool  GetFog				( int iID );
DARKSDK bool  GetAmbient			( int iID );
DARKSDK bool  GetGhost				( int iID );
DARKSDK bool  GetLock				( int iID );
	
DARKSDK void  SetGlobalColor		( int iID, int iA, int iR, int iG, int iB );
DARKSDK void  SetTileColor			( int iID, int iA, int iR, int iG, int iB );

DARKSDK bool  LockVertexData		( int iID );		// lock the buffer so you can access matrix data
DARKSDK bool  UnlockVertexData		( int iID );		// unlock the buffer
DARKSDK bool  LockIndexData			( int iID );		// lock index buffer
DARKSDK bool  UnlockIndexData		( int iID );
DARKSDK void* GetVertexData			( int iID );		// retrieve vertex data ( must lock & unlock data inbetween calls )
DARKSDK void* GetIndexData			( int iID );		// get index data
DARKSDK int   GetNumVertices		( int iID );		// get the number of vertices
DARKSDK int   GetNumIndices			( int iID );		// get the number of indices

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DBV1 commands
DARKSDK void  MakeEx				( int iID, float fWidth, float fDepth,  int iXSegmented, int iZSegmented );
DARKSDK void  Delete				( int iID );
DARKSDK void  Fill					( int iID, float fHeight, int iTile );
DARKSDK void  SetTransparencyOn		( int iID );
DARKSDK void  SetTransparencyOff	( int iID );
DARKSDK void  PositionEx			( int iID, float fX, float fY, float fZ );
DARKSDK void  PrepareTexture		( int iID, int iImage, int iAcross, int iDown );
DARKSDK void  Randomize				( int iID, int iHeight );						// randomize height of a matrix
DARKSDK void  SetHeight				( int iID, int iX, int iZ, float fHeight );	// set the height of a block
DARKSDK void  SetNormal				( int iID, int iX, int iZ, float fX, float fY, float fZ );
DARKSDK void  SetTexture			( int iID, int iTextureMode, int iMipGen );
DARKSDK void  SetTile				( int iID, int iX, int iZ, int iTile );		// set the texture on a block
DARKSDK void  SetWireframeOn		( int iID );
DARKSDK void  SetTransparencyOn		( int iID, int iMode );
DARKSDK void  SetWireframeOff		( int iID );
DARKSDK void  SetEx					( int iID, int bWireframe, int bTransparency, int bCull, int iFilter, int bLight, int bFog, int bAmbient );
DARKSDK void  ShiftUp				( int iID );									// shift matrix up and wrap around
DARKSDK void  ShiftDown				( int iID );									// shift matrix down and wrap around
DARKSDK void  ShiftLeft				( int iID );									// shift matrix left and wrap around
DARKSDK void  ShiftRight			( int iID );									// shift matrix right and wrap around
DARKSDK void  Apply					( int iID );

DARKSDK void  SetTrim				( int iID, float fTrimX, float fTrimY );
DARKSDK void  SetPriority			( int iID, int iPriority );

DARKSDK void  SetPositionVector3	( int iID, int iVector );
DARKSDK void  GetPositionVector3	( int iVector, int iID );
//DARKSDK void  SetRotationVector3	( int iID, int iVector );
//DARKSDK void  GetRotationVector3	( int iVector, int iID );

// DBV1 expressions
DARKSDK DWORD GetPositionXEx		( int iID );
DARKSDK DWORD GetPositionYEx		( int iID );
DARKSDK DWORD GetPositionZEx		( int iID );
DARKSDK DWORD GetGroundHeightEx		( int iID, float fX, float fZ );					// get y height at tile
DARKSDK DWORD GetHeightEx			( int iID, int iX, int iZ );					// get height at tile
DARKSDK int   GetExistEx			( int iID );									// does matrix exist
DARKSDK int   GetTileCountEx		( int iID );									// get number of texture tiles
DARKSDK int   GetTilesExistEx		( int iID );									// do tiles exist
DARKSDK int   GetWireframeEx		( int iID );

#ifdef DARKSDK_COMPILE
		void  dbMakeMatrix					( int iID, float fWidth, float fDepth,  int iXSegmented, int iZSegmented );
		void  dbDeleteMatrix				( int iID );
		void  dbFillMatrix					( int iID, float fHeight, int iTile );
		void  dbGhostMatrixOn				( int iID );
		void  dbGhostMatrixOff				( int iID );
		void  dbPositionMatrix				( int iID, float fX, float fY, float fZ );
		void  dbPrepareMatrixTexture		( int iID, int iImage, int iAcross, int iDown );
		void  dbRandomizeMatrix				( int iID, int iHeight );
		void  dbSetMatrixHeight				( int iID, int iX, int iZ, float fHeight );
		void  dbSetMatrixNormal				( int iID, int iX, int iZ, float fX, float fY, float fZ );
		void  dbSetMatrixTexture			( int iID, int iTextureMode, int iMipGen );
		void  dbSetMatrixTile				( int iID, int iX, int iZ, int iTile );
		void  dbSetMatrixWireframeOn		( int iID );
		void  dbGhostMatrixOn				( int iID, int iMode );
		void  dbSetMatrixWireframeOff		( int iID );
		void  dbSetMatrix					( int iID, int bWireframe, int bTransparency, int bCull, int iFilter, int bLight, int bFog, int bAmbient );
		void  dbShiftMatrixUp				( int iID );
		void  dbShiftMatrixDown				( int iID );
		void  dbShiftMatrixLeft				( int iID );
		void  dbShiftMatrixRight			( int iID );
		void  dbUpdateMatrix				( int iID );

		void  dbSetMatrixTrim				( int iID, float fTrimX, float fTrimY );
		void  dbSetMatrixPriority			( int iID, int iPriority );

		void  dbPositionMatrix				( int iID, int iVector );
		void  dbSetVector3ToMatrixPosition	( int iVector, int iID );
		void  dbRotateMatrix				( int iID, int iVector );
		void  dbSetVector3ToMatrixRotation	( int iVector, int iID );

		float dbMatrixPositionX				( int iID );
		float dbMatrixPositionY				( int iID );
		float dbMatrixPositionZ				( int iID );
		float dbGetGroundHeight				( int iID, float fX, float fZ );
		float dbGetMatrixHeight				( int iID, int iX, int iZ );
		int   dbMatrixExist					( int iID );
		int   dbMatrixTileCount				( int iID );
		int   dbMatrixTilesExist			( int iID );
		int   dbMatrixWireframeState		( int iID );

#endif

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CMATRIX_H_