
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "cmatrixc.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef void					( *CAMERA3D_SetAutoCam ) ( float, float, float, float );
typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN ) ( void );
typedef D3DXVECTOR3				( *GetVectorPFN )       ( int );
typedef void					( *SetVectorPFN )       ( int, float, float, float );
typedef int						( *GetExistPFN )       ( int );
typedef void					( *RetVoidFunctionPointerPFN )     ( ... );
typedef bool					( *RetBoolFunctionPointerPFN )     ( ... );
typedef int 					( *RetIntFunctionPointerPFN  )     ( ... );
typedef LPDIRECT3DTEXTURE9		( *IMAGE_RetLPD3DTEX9ParamIntPFN ) ( int );

DBPRO_GLOBAL GlobStruct*					g_pGlob							= NULL;

DBPRO_GLOBAL CMatrixManager					m_pMatrixManager;
DBPRO_GLOBAL tagMatrixData*					m_ptr = NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D;
extern tagObjectPos*						m_pMatrixPos;

DBPRO_GLOBAL GetVectorPFN					g_Types_GetVector;
DBPRO_GLOBAL SetVectorPFN					g_Types_SetVector;
DBPRO_GLOBAL GetExistPFN					g_Types_GetExist;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Constructor;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Destructor;

DBPRO_GLOBAL RetBoolFunctionPointerPFN		g_Image_Load;
DBPRO_GLOBAL RetBoolFunctionPointerPFN		g_Image_Save;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Delete;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Make;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Stretch;

DBPRO_GLOBAL IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;
DBPRO_GLOBAL RetIntFunctionPointerPFN		g_Image_GetWidth;
DBPRO_GLOBAL RetIntFunctionPointerPFN		g_Image_GetHeight;
DBPRO_GLOBAL RetBoolFunctionPointerPFN		g_Image_GetExist;

DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetSharing;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetMemory;

DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Lock;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Unlock;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Write;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_Get;

DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetMipmapMode;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetMipmapType;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetMipmapBias;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetMipmapNum;

DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetColorKey;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_Image_SetTranslucency;

DBPRO_GLOBAL CAMERA3D_SetAutoCam			g_Camera3D_SetAutoCam = NULL;
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void ConstructorD3D ( HINSTANCE hSetup, HINSTANCE hImage )
{
	// check the handles to the DLLs are ok
	#ifndef DARKSDK_COMPILE
	if ( !hSetup || !hImage )
	{
		// if they aren't attempt to load the DLLs manually
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}
	#endif

	// setup function pointers for the image library
	#ifndef DARKSDK_COMPILE
		g_Image_GetPointer      = ( IMAGE_RetLPD3DTEX9ParamIntPFN )	GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );

		g_Image_Constructor     = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Image_Destructor      = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Destructor@@YAXXZ" );

		g_Image_Load            = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?Load@@YA_NPADH@Z" );
		g_Image_Save            = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?Save@@YA_NHPAD@Z" );
		g_Image_Delete          = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Delete@@YAXH@Z" );
		g_Image_Make            = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Make@@YAXHHH@Z" );
		g_Image_Stretch         = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Stretch@@YAXHHH@Z" );
			
		g_Image_GetWidth        = ( RetIntFunctionPointerPFN )		GetProcAddress ( hImage, "?GetWidth@@YAHH@Z" );
		g_Image_GetHeight       = ( RetIntFunctionPointerPFN )		GetProcAddress ( hImage, "?GetHeight@@YAHH@Z" );
		g_Image_GetExist        = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?GetExist@@YA_NH@Z" );

		g_Image_SetSharing      = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetSharing@@YAX_N@Z" );
		g_Image_SetMemory       = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMemory@@YAXH@Z" );

		g_Image_Lock            = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Lock@@YAXH@Z" );
		g_Image_Unlock          = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Unlock@@YAXH@Z" );
		g_Image_Write           = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Write@@YAXHHHHHHH@Z" );
		g_Image_Get             = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Get@@YAXHHHPAH00@Z" );

		g_Image_SetMipmapMode   = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapMode@@YAX_N@Z" );
		g_Image_SetMipmapType   = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapType@@YAXH@Z" );
		g_Image_SetMipmapBias   = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapBias@@YAXM@Z" );
		g_Image_SetMipmapNum    = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapNum@@YAXH@Z" );
		
		g_Image_SetColorKey     = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetColorKey@@YAXHHH@Z" );
		g_Image_SetTranslucency = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetTranslucency@@YAXHH@Z" );

		// setup function pointers for the setup library
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;

		g_Image_Stretch = ( RetVoidFunctionPointerPFN ) dbStretchImage;
		g_Image_GetWidth = ( RetIntFunctionPointerPFN ) dbGetImageWidth;
		g_Image_GetHeight = ( RetIntFunctionPointerPFN ) dbGetImageHeight;


		g_Image_GetPointer = dbGetImagePointer;
		
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	// FROM MATRIX3D DLL (maybe this should be in CAMERA ONLY)
	// NEED a common D3D Scene Default Setup Function (that all can call)
	//maybe each dll has its own setup function call...
	// FOG Default
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, 0 );
	m_pD3D->SetRenderState ( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	m_pD3D->SetRenderState ( D3DRS_FOGSTART, 0 );
	float fFullOne=50000.0f; m_pD3D->SetRenderState ( D3DRS_FOGEND, *(DWORD*)&fFullOne );

	// LIGHT Default
	D3DLIGHT9 DefaultLight;
	memset ( &DefaultLight, 0, sizeof ( D3DLIGHT9 ) );
	DefaultLight.Type         = D3DLIGHT_DIRECTIONAL;						// set to point light
	DefaultLight.Diffuse.r    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Diffuse.g    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Diffuse.b    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Direction    = D3DXVECTOR3 ( 0.1f, -0.7f, 0.2f );
	DefaultLight.Range        = 5000.0f;							// light range
	DefaultLight.Attenuation0 = 1.0f;								// light intensity over distance

	m_pD3D->SetLight    ( 0, &DefaultLight );
	m_pD3D->LightEnable ( 0, true );
}

DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage )
{
	ConstructorD3D ( hSetup, hImage );
}

DARKSDK void DestructorD3D ( void )
{
	link* pCheck = m_pMatrixManager.m_List.m_start;
	while(pCheck)
	{
		tagMatrixData* ptr = NULL;
		ptr = m_pMatrixManager.GetData ( pCheck->id );
		if ( ptr == NULL ) continue;
		
		SAFE_DELETE_ARRAY ( ptr->pTiles );
		SAFE_DELETE_ARRAY ( ptr->pfVert );
		SAFE_DELETE_ARRAY ( ptr->ptmpTiles );
		SAFE_DELETE_ARRAY ( ptr->lpTexture );
		SAFE_RELEASE ( ptr->lpIndexBuffer );
		if ( ptr->lpIndexBufferArray )
		{
			for ( DWORD n=0; n<=ptr->dwIndexBufferArraySize; n++ )
				SAFE_RELEASE ( ptr->lpIndexBufferArray[n] );

			SAFE_DELETE ( ptr->lpIndexBufferArray );
		}
		SAFE_RELEASE ( ptr->lpVertexBuffer );
		SAFE_DELETE ( ptr );

		pCheck = pCheck->next;
	}
}

DARKSDK void Destructor ( void )
{
	DestructorD3D();
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;

	// Acquire Camera AutoCam Function
	

	// Assign function ptrs for vector handling
	#ifndef DARKSDK_COMPILE
		g_Camera3D_SetAutoCam = ( CAMERA3D_SetAutoCam ) GetProcAddress ( g_pGlob->g_Camera3D, "?SetAutoCam@@YAXMMMM@Z" );

		g_Types_GetVector					= ( GetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetVector3@@YA?AUD3DXVECTOR3@@H@Z" );
		g_Types_SetVector					= ( SetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?SetVector3@@YAXHMMM@Z" );
		g_Types_GetExist					= ( GetExistPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetExist@@YAHH@Z" );
	#else
		g_Camera3D_SetAutoCam = dbSetAutoCam;

		g_Types_GetVector = dbGetVector3;
		g_Types_SetVector = dbSetVector3;
		g_Types_GetExist = dbGet3DMathsExist;
	#endif
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		DestructorD3D();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		ConstructorD3D ( g_pGlob->g_GFX, g_pGlob->g_Image );
		PassCoreData ( g_pGlob );
	}
}

DARKSDK void Update ( void )
{
	// This draws all the matrixes
	m_pMatrixManager.Update ( 0 );
}

DARKSDK void UpdateLast ( void )
{
	// This draws all the matrixes at the end as well (for flagged matrices)
	m_pMatrixManager.Update ( 1 );
}

bool UpdateMatrixPtr ( int iID )
{
	// update internal data pointer, every function needs to call this
	// it's easier to place it in a separate function just in case we need
	// to change anything, we could force it inline which may make it
	// slightly quicker

	// set the pointer to null and attempt to retrieve the data
	m_ptr  = NULL;
	m_ptr  = m_pMatrixManager.GetData ( iID );

	// check we got the data
	if ( m_ptr == NULL )
		return false;

	// store the position aswell
	m_pMatrixPos = ( tagObjectPos* ) m_ptr;

	// return true
	return true;
}

DARKSDK void GetCullDataFromModel(int)
{
}

DARKSDK bool Make ( int iID, int iWidth, int iDepth, int iXSegmented, int iZSegmented )
{
	// create a matrix, matrices are useful for terrains

	// make sure the values passed are valid, if they aren't then show an
	// error message and return false, still let the app continue though
	// as it isn't a fatal error, we simply don't make the matrix
	if ( iID < 1 || iWidth < 1 || iDepth < 1 || iXSegmented < 1 || iZSegmented < 1 )
	{
		Error ( "Invalid parameter specifed for matrix creation" );
		return false;
	}

	// variable declarations
	HRESULT hr;																						// used for error checking
	tagMatrixData			m_Data;					// matrix data
	int						iTemp;					// used for loops etc
	tagMatrixVertexDesc*	psVertices = NULL;		// pointer to vertices

	float fTexCoords [ ] =							// texture coordinates
							{
								0.0,	1.0,
								0.0,	0.0,
								1.0,	0.0,
								1.0,	1.0,
							};
	int		iTexNum = 0;
	float	x		= 0;															
	float	y		= 0;
	float	z		= 0;																			// z pos of matrix vertex
	float	fX		= ( float ) ( (float)iWidth / (iXSegmented*1.0f) );
	float	fY		= ( float ) ( (float)iDepth / (iZSegmented*1.0f) );
	int		iPos	= 0;
	int		iSelect = 0;

	VOID*	pVertices = NULL;																		// pointer to vertex buffer
	DWORD	dwSize    = ( iXSegmented * iZSegmented ) * 4 * sizeof ( tagMatrixVertexDesc );			// size of vertex buffer
	int*	piData       = NULL;																	// pointer to index buffer
	DWORD	dwIndexSize  = sizeof ( WORD ) * ( iXSegmented * iZSegmented ) * 6;						// size of index buffer
	DWORD	dwIndexCount = ( iXSegmented * iZSegmented ) * 6;										// number of indices
	DWORD*	pIndex       = NULL;																	// temp index buffer
	int		iNum         = 0;
	int		iCount       = 0;
	bool	bFlag        = false;

	// leefix-050803-no limit to matrix size, uses multiple indexes to achieve this
	DWORD dwNumberOfExtraIndexBuffers=0;
	if ( dwIndexCount > 65535 ) dwNumberOfExtraIndexBuffers=dwIndexCount/65535;

	// clear out the data structure
	memset ( &m_Data, 0, sizeof ( m_Data ) );

	// setup internal values
	m_Data.pD3D			= m_pD3D;			// direct 3D device
	m_Data.iID			= iID;				// reference number
	m_Data.iWidth		= iWidth;			// width of matrix
	m_Data.iDepth		= iDepth;			// height of matrix
	m_Data.iZSegmented	= iZSegmented;		// number of z segments
	m_Data.iXSegmented	= iXSegmented;		// number of x segments
	m_Data.bCull        = true;				// cull mode
	m_Data.bLight       = true;				// light mode
	m_Data.bVisible     = true;				// is  visible
	m_Data.bWireframe   = true;				// wireframe mode

	m_Data.vecLook     = D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f );		// look vector
	m_Data.vecUp       = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );		// up vector
	m_Data.vecRight    = D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f );		// right vector
	m_Data.vecPosition = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );		// position

	m_Data.vecYawPitchRoll = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	m_Data.vecRotate       = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );

	// actual data is one larger than specified
	int iXSegmentedHeight=iXSegmented+1;
	int iZSegmentedHeight=iZSegmented+1;
	m_Data.iXSegmentedFull = iXSegmentedHeight;
	m_Data.iZSegmentedFull = iZSegmentedHeight;

	// setup a height array ( it's used as a temporary array )
	if ( ! ( m_Data.pfVert = new tagMatrixVertexDesc [ iXSegmentedHeight * iZSegmentedHeight ] ) )
	{
		Error ( "Unable to allocate temporary height array for matrix" );
		return false;
	}
	if ( ! ( m_Data.ptmpTiles = new int [ iXSegmentedHeight * iZSegmentedHeight ] ) )
	{
		Error ( "Unable to allocate temporary tile array for matrix" );
		return false;
	}
	memset ( m_Data.pfVert, 0, sizeof ( tagMatrixVertexDesc ) * ( iXSegmentedHeight * iZSegmentedHeight ) );
	memset ( m_Data.ptmpTiles, 0, sizeof ( int ) * ( iXSegmentedHeight * iZSegmentedHeight ) );
	for ( iTemp = 0; iTemp < iXSegmentedHeight * iZSegmentedHeight; iTemp++ )
	{
		m_Data.pfVert [ iTemp ].x = 0.0f;
		m_Data.pfVert [ iTemp ].y = 0.0f;
		m_Data.pfVert [ iTemp ].z = 0.0f;
		m_Data.pfVert [ iTemp ].nx = 0.0;
		m_Data.pfVert [ iTemp ].ny = 0.0;
		m_Data.pfVert [ iTemp ].nz = -1.0;
	}
	for ( iTemp = 0; iTemp < iXSegmentedHeight * iZSegmentedHeight; iTemp++ )
	{
		m_Data.ptmpTiles [ iTemp ] = 0;
	}

	// setup the tile properties
	if ( ! ( m_Data.pTiles = new tagTileProperties [ iXSegmentedHeight * iZSegmentedHeight ] ) )
	{
		Error ( "Unable to allocate tile properties array for matrix" );
		return false;
	}
	for ( iTemp = 0; iTemp < iXSegmentedHeight * iZSegmentedHeight; iTemp++ )
	{
		m_Data.pTiles [ iTemp ].fHeight	= 0.0f;	
		m_Data.pTiles [ iTemp ].iTile   = 0;	// texture tile
		m_Data.pTiles [ iTemp ].vNormal = D3DXVECTOR3(0,0,-1.0f);
	}

	// create a new block of memory, this will hold all of our vertices, the size
	// of the array is dependant on the parameters passed, also check we allocated
	// the memory ok, if we didn't return back to the caller function
	if ( !( psVertices = new tagMatrixVertexDesc [ ( iXSegmented * iZSegmented ) * 4 ] ) )
	{
		Error ( "Failed to allocate vertex memory for matrix creation" );
		return false;
	}
	
	// now we can create the vertices based on the information we have, we
	// loop through the x and z segments filling in the data
	for ( int loop = 0; loop < iZSegmented; loop++ )
	{
		for ( iTemp = 0; iTemp < iXSegmented * 4; iTemp++ )
		{
			// setup vertex position
			( psVertices + iSelect )->x = x;		// x position
			( psVertices + iSelect )->y = y;		// y position
			( psVertices + iSelect )->z = z;		// z position
			( psVertices + iSelect )->nx = 0.0;		// x normal
			( psVertices + iSelect )->ny = 0.0;		// y normal
			( psVertices + iSelect )->nz = -1.0;		// z normal

			// setup diffuse
			( psVertices + iSelect )->diffuse = D3DCOLOR_ARGB ( 128, 255, 255, 255 );

			// now setup texture coordinates
			( psVertices + iSelect )->tu1 = fTexCoords [ iTexNum     ];
			( psVertices + iSelect )->tv1 = fTexCoords [ iTexNum + 1 ];
			
			// move the coordinates based on the current iteration
			switch ( iPos )
			{
				case 0:
					y += fY;
				break;

				case 1:
					x += fX;
				break;

				case 2:
					y -= fY;
				break;
			}

			// update the position
			if ( iPos < 3 )
				iPos++;
			else
				iPos = 0;

			// update texture array link
			if ( iTexNum < 6 )
				iTexNum += 2;
			else
				iTexNum = 0;

			iSelect++;
		}

		// when a line has been created we can reset values
		iPos    = 0;
		iTexNum = 0;

		x  = 0.0f;														
		y += fY;
		z  = 0;
	}

	// setup the vertex buffer for the matrix
	
	// create a vertex buffer
	if ( FAILED ( hr = m_pD3D->CreateVertexBuffer ( dwSize, D3DUSAGE_DYNAMIC, D3D9_MATRIXVERTEX, D3DPOOL_DEFAULT, &m_Data.lpVertexBuffer, NULL ) ) )
	{
		// if it failed display an error message
		Error ( "Failed to create matrix vertex buffer" );
		return false;
	}

	// lock the full vertex buffer
	if ( FAILED ( hr = m_Data.lpVertexBuffer->Lock ( 0, dwSize, ( VOID** ) &pVertices, 0 ) ) )
	{
		// if it failed display an error message
        Error ( "Failed to lock vertex buffer for matrix creation" );
		return false;
	}

	// now we can copy the data across
	memcpy ( pVertices, psVertices, dwSize );

	// unlock the vertex buffer because we're finished with it
	if ( FAILED ( hr = m_Data.lpVertexBuffer->Unlock ( ) ) )
	{
		// if it failed display an error message
        Error ( "Failed to unlock vertex buffer for matrix creation" );
		return false;
	}
	
	// delete the original vertices, we only created these to setup the matrix, now
	// everything is in a vertex buffer we can destroy them
	SAFE_DELETE ( psVertices );

	// leefix-050803-only use index buffer if specified (some cards can only have small index buffers-and matrix may be big)
	if ( dwIndexCount>0 )
	{
		// setup index buffer memory
		if ( !( pIndex = new DWORD [ dwIndexCount ] ) )
		{
			// if it failed display an error message
			Error ( "Failed to allocate temporary index buffer" );
			return false;
		}

		// setup the index buffer, by using an index buffer we get faster results
		for ( iTemp = 0; iTemp < (int)dwIndexCount / 3; iTemp++ )
		{
			pIndex [ iCount++ ] = iNum;

			if ( bFlag )
				iNum += 1;
			
			pIndex [ iCount++ ] = iNum + 1;
			pIndex [ iCount++ ] = iNum + 2;

			if ( bFlag && iCount % 3 == 0 )
			{
				iNum += 3;
				bFlag = false;
				continue;
			}

			bFlag = true;
		}

		// Wipe index buffer ptrs to begin with
		m_Data.lpIndexBuffer = NULL;
		m_Data.dwIndexBufferArraySize = 0;
		m_Data.lpIndexBufferArray = NULL;

		// CREATE SINGLE INDEX BUFFER
		if ( dwNumberOfExtraIndexBuffers==0 )
		{
			// create an index buffer
			if ( FAILED ( hr = m_pD3D->CreateIndexBuffer ( dwIndexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_Data.lpIndexBuffer, NULL ) ) )
			{
				// if it failed display an error message
				Error ( "Failed to create index buffer for matrix" );
				return false;
			}

			// lock the vertex buffer
			if ( FAILED ( hr = m_Data.lpIndexBuffer->Lock ( 0, dwIndexSize, ( VOID** )&piData, 0 ) ) )
			{
				// if it failed display an error message
				Error ( "Failed to lock index buffer for matrix" );
				return false;
			}

// temp index array is now DWORD and now WORD
//			// copy the indices across to the index buffer
//			memcpy ( piData, pIndex, dwIndexSize );

			// deduct values from index data to ensure each buffer starts at zero (WORD limit)
			// dwIndexSize is BYTES so we need interations half of that (WORD)
			WORD* pIndexBufferData = (WORD*)piData;
			DWORD* pIndexBufferMasterData = (DWORD*)pIndex;
			for ( DWORD t=0; t<dwIndexSize/2; t++ )
				pIndexBufferData[t] = (WORD)pIndexBufferMasterData[t];

			// now we're finished so unlock the vertex buffer
			if ( FAILED ( hr = m_Data.lpIndexBuffer->Unlock ( ) ) )
			{
				// if it failed display an error message
				Error ( "Failed to unlock index buffer" );
				return false;
			}
		}
		else
		{
			// CREATE INDEX ARRAY OF BUFFERS
			m_Data.lpIndexBufferArray = new LPDIRECT3DINDEXBUFFER9[1+dwNumberOfExtraIndexBuffers];

			// CREATE INDIVIDUAL BUFFERS
			DWORD dwIndexStart = 0;
			DWORD dwPrimsPerDraw=65534/3;
			DWORD dwTotalPrimCount = dwIndexCount/3;
			DWORD dwIterations = dwTotalPrimCount/dwPrimsPerDraw;
			DWORD dwPrimsLeft = dwTotalPrimCount;
			DWORD dwPrimCount = 0;
			for ( DWORD n=0; n<=dwIterations; n++ )
			{
				// work out index buffer size
				if ( dwPrimsLeft > dwPrimsPerDraw )
					dwPrimCount = dwPrimsPerDraw;
				else
					dwPrimCount = dwPrimsLeft;

				// number of indexes for primitive group
				DWORD dwIndexSize = dwPrimCount*3*sizeof(WORD);

				// create an index buffer
				if ( FAILED ( hr = m_pD3D->CreateIndexBuffer ( dwIndexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_Data.lpIndexBufferArray[n], NULL ) ) )
					return false;

				// lock the vertex buffer
				if ( FAILED ( hr = m_Data.lpIndexBufferArray[n]->Lock ( 0, dwIndexSize, ( VOID** )&piData, 0 ) ) )
					return false;

// temp index array is now DWORD and now WORD
//				// copy the indices across to the index buffer
//				memcpy ( piData, pIndex+dwIndexStart, dwIndexSize );

				// deduct values from index data to ensure each buffer starts at zero (WORD limit)
				// dwIndexSize is BYTES so we need interations half of that (WORD)
				WORD* pIndexBufferData = (WORD*)piData;
				DWORD* pIndexBufferMasterData = (DWORD*)pIndex+dwIndexStart;
				DWORD dwVertexIndexToRestartFrom = pIndexBufferMasterData[0];
				for ( DWORD t=0; t<dwIndexSize/2; t++ )
					pIndexBufferData[t] = (WORD)(pIndexBufferMasterData[t] - dwVertexIndexToRestartFrom);

				// now we're finished so unlock the vertex buffer
				if ( FAILED ( hr = m_Data.lpIndexBufferArray[n]->Unlock ( ) ) )
					return false;

				// next index buffer
				dwIndexStart += dwIndexSize/2;
				dwPrimsLeft-=dwPrimsPerDraw;
			}

			// uses this many index buffers
			m_Data.dwIndexBufferArraySize = dwIterations;
		}

		SAFE_DELETE ( pIndex );
	}
	else
	{
		// No index buffer ptr
		m_Data.lpIndexBuffer = NULL;
	}

	m_Data.bVisible      = true;
	m_Data.bLight        = true;
	m_Data.bCull         = true;
	m_Data.iFilter       = 2;
	m_Data.bTransparency = false;
	m_Data.bRotate       = false;//what is this for??true;
	m_Data.bGhost		 = false;
	m_Data.iGhostMode    = 0;
	m_Data.iTextureMode  = 1;
	m_Data.iMipFilter	 = 2;
	m_Data.bZDepth       = true;
	m_Data.bLock         = false;
	m_Data.bFog          = true;
	m_Data.bAmbient      = true;
	m_Data.bCollision    = true;
	m_Data.bRenderAfterObjects = false;

	/////////////////////////
	// setup textures ///////
	/////////////////////////
	m_Data.lpTexture = new LPDIRECT3DTEXTURE9 [ 4 ];
	m_Data.lpTexture [ 0 ] = NULL;
	m_Data.lpTexture [ 1 ] = NULL;
	m_Data.lpTexture [ 2 ] = NULL;
	m_Data.lpTexture [ 3 ] = NULL;

	m_Data.iTextureAcross = 1;
	m_Data.iTextureDown   = 1;

	m_Data.fTrimX = 0.0f;
	m_Data.fTrimY = 0.0f;

	////////////////////////////////
	// add to matrix manager ///////
	////////////////////////////////
	m_pMatrixManager.Add ( &m_Data, iID );
	////////////////////////////////

	//#ifndef DARKSDK_COMPILE
		//dbtMatrixScale    ( iID, 100, 100, 100 );
		//dbtMatrixPosition ( iID, 0.0, 0.0, 0.0 );
		//dbtMatrixRotate   ( iID, 90.0f, 0.0f, 0.0f );
	//#else
		//dbtMatrixScale    ( iID, 100, 100, 100 );
		//dbtMatrixPosition ( iID, 0.0, 0.0, 0.0 );
	//	//dbtMatrixRotate   ( iID, 90.0f, 0.0f, 0.0f );
	//#endif

	MatrixScale    ( iID, 100, 100, 100 );
	MatrixPosition ( iID, 0.0, 0.0, 0.0 );
	MatrixRotate   ( iID, 90.0f, 0.0f, 0.0f );


	return true;
}

DARKSDK void SetSphereMap ( int iID, int iSphereImage )
{
	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->lpTexture [ 1 ] = g_Image_GetPointer ( iSphereImage );
}

DARKSDK void SetCubeMap ( int iID, int iCubeImage )
{
	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->lpTexture [ 1 ] = g_Image_GetPointer ( iCubeImage );
}

DARKSDK void SetBumpMap ( int iID, int iEnvironmentImage, int iBumpMap )
{
	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->lpTexture [ 1 ] = g_Image_GetPointer ( iEnvironmentImage );
	m_ptr->lpTexture [ 2 ] = g_Image_GetPointer ( iBumpMap );
}

DARKSDK void SetHeightReal ( int iID, tagMatrixVertexDesc* pData, int iX, int iZ, float fHeight )
{
	// update the height of the specified block
	fHeight *= -1.0f; // leefix - 210703 - Because Y is Z and minus Z will make a hill!

	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	DWORD					dwSize      = ( m_ptr->iXSegmented * m_ptr->iZSegmented ) *
										  4 * sizeof ( tagMatrixVertexDesc );				// size of matrix
	int						iTemp		= 0;												// used for loops etc.
	int						iIndex		= ( iX * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) );

	// set the selected blocks height
	if ( iX < m_ptr->iXSegmented && iZ < m_ptr->iZSegmented )
		( pData + ( iIndex ) )->z = fHeight;

	// update the block to the left
	if ( iZ < m_ptr->iZSegmented )
	{
		if ( iX > 0 )
		{
			if ( iX==m_ptr->iXSegmented )
			{
				( pData + ( iIndex - 1 ) )->z = fHeight;
			}
			else
			{
				if ( ( ( iIndex / 4 ) % m_ptr->iXSegmented != 0 ) )
					( pData + ( iIndex - 1 ) )->z = fHeight;
			}
		}
	}

	// update the block behind it, make sure it exists first
	if ( ( iIndex / 4 ) >= m_ptr->iXSegmented )
	{
		int iLeft;
		if ( iX==m_ptr->iXSegmented && iZ>0 )
		{
			if(iZ<=m_ptr->iZSegmented)
			{
				// Left Only
				iLeft = ( ( iX - m_ptr->iXSegmented ) * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) ) + 1;
				( pData + ( iLeft - 3 ) )->z = fHeight;
			}
		}
		else
		{
			// Right
			iLeft = ( ( iX - m_ptr->iXSegmented ) * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) ) + 1;
			( pData + ( iLeft ) )->z = fHeight;

			// Left
			if ( ( iIndex / 4 ) > 0 && ( ( iIndex / 4 ) % m_ptr->iXSegmented != 0 ) )
				( pData + ( iLeft - 3 ) )->z = fHeight;
		}
	}
}

DARKSDK void SetNormalReal ( int iID, tagMatrixVertexDesc* pData, int iX, int iZ, D3DXVECTOR3 vNormal )
{
	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	DWORD					dwSize      = ( m_ptr->iXSegmented * m_ptr->iZSegmented ) *
										  4 * sizeof ( tagMatrixVertexDesc );				// size of matrix
	int						iTemp		= 0;												// used for loops etc.
	int						iIndex		= ( iX * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) );

	// set the selected blocks height
	if ( iX < m_ptr->iXSegmented && iZ < m_ptr->iZSegmented )
	{
		( pData + ( iIndex ) )->nx = vNormal.x;
		( pData + ( iIndex ) )->ny = vNormal.y;
		( pData + ( iIndex ) )->nz = vNormal.z;
	}

	// update the block to the left
	if ( iZ < m_ptr->iZSegmented )
	{
		if ( iX > 0 )
		{
			if ( iX==m_ptr->iXSegmented )
			{
				( pData + ( iIndex - 1 ) )->nx = vNormal.x;
				( pData + ( iIndex - 1 ) )->ny = vNormal.y;
				( pData + ( iIndex - 1 ) )->nz = vNormal.z;
			}
			else
			{
				if ( ( ( iIndex / 4 ) % m_ptr->iXSegmented != 0 ) )
				{
					( pData + ( iIndex - 1 ) )->nx = vNormal.x;
					( pData + ( iIndex - 1 ) )->ny = vNormal.y;
					( pData + ( iIndex - 1 ) )->nz = vNormal.z;
				}
			}
		}
	}

	// update the block behind it, make sure it exists first
	if ( ( iIndex / 4 ) >= m_ptr->iXSegmented )
	{
		int iLeft;
		int iRight;

		if ( iX==m_ptr->iXSegmented && iZ>0 )
		{
			if(iZ<m_ptr->iZSegmented)
			{
				iLeft = ( ( iX - m_ptr->iXSegmented ) * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) ) + 1;
				
				( pData + ( iLeft ) )->nx = vNormal.x;
				( pData + ( iLeft ) )->ny = vNormal.y;
				( pData + ( iLeft ) )->nz = vNormal.z;

				iRight = iLeft + 1;

				if ( ( iIndex / 4 ) > 0 )
				{
					( pData + ( iLeft - 3 ) )->nx = vNormal.x;
					( pData + ( iLeft - 3 ) )->ny = vNormal.y;
					( pData + ( iLeft - 3 ) )->nz = vNormal.z;
				}
			}

			// mike - 230406 - addition so last tile is set properly
			if ( iZ == m_ptr->iZSegmented && iX == m_ptr->iXSegmented )
			{
				iLeft = ( ( iX - m_ptr->iXSegmented ) * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) ) + 1;
				
				iRight = iLeft + 1;
				if ( ( iIndex / 4 ) > 0 )
				{
					( pData + ( iLeft - 3 ) )->nx = vNormal.x;
					( pData + ( iLeft - 3 ) )->ny = vNormal.y;
					( pData + ( iLeft - 3 ) )->nz = vNormal.z;
				}
			}
			
		}
		else
		{
			iLeft = ( ( iX - m_ptr->iXSegmented ) * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) ) + 1;
			
			( pData + ( iLeft ) )->nx = vNormal.x;
			( pData + ( iLeft ) )->ny = vNormal.y;
			( pData + ( iLeft ) )->nz = vNormal.z;

			iRight = iLeft + 1;
			if ( ( iIndex / 4 ) > 0 && ( ( iIndex / 4 ) % m_ptr->iXSegmented != 0 ) )
			{
				( pData + ( iLeft - 3 ) )->nx = vNormal.x;
				( pData + ( iLeft - 3 ) )->ny = vNormal.y;
				( pData + ( iLeft - 3 ) )->nz = vNormal.z;
			}
		}
	}
}

DARKSDK void SetTileReal ( int iID, tagMatrixVertexDesc* pData, int iX, int iZ, int iTile )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	int	iStart	= 0;

	iTile--;

	float	fTuSeg	= (float)( 1.0 / m_ptr->iTextureAcross );
	float	fTvSeg	= (float)( 1.0 / m_ptr->iTextureDown   );

	int		iXTile	= iTile % m_ptr->iTextureAcross;
	int		iYTile	= ( iTile - iXTile ) / m_ptr->iTextureAcross;

	float	fTu		= ( fTuSeg * iXTile );
	float	fTv		= ( fTvSeg * iYTile );

	int   iIndex = ( iX * 4 ) + ( iZ * ( m_ptr->iXSegmented * 4 ) );

	/*
	// LEEFIX - 051002 -No need to cropping - texture clamp,mirror,etc can handle things like this
	( pData + ( iIndex + 2 ) )->tu1	=	( fTu + fTuSeg );
	( pData + ( iIndex + 2 ) )->tv1	=   ( fTv );
	( pData + ( iIndex + 3 ) )->tu1 =	( fTu + fTuSeg );
	( pData + ( iIndex + 3 ) )->tv1 =   ( fTv + fTvSeg );
	( pData + ( iIndex + 0 ) )->tu1 =	( fTu );
	( pData + ( iIndex + 0 ) )->tv1 =   ( fTv + fTvSeg );
	( pData + ( iIndex + 1 ) )->tu1 =	( fTu );
	( pData + ( iIndex + 1 ) )->tv1 =   ( fTv );
	*/

	// LEEADD - 200603 - SET MATRIX TRIM so user can decide trim level
	( pData + ( iIndex + 2 ) )->tu1	=	( fTu + fTuSeg )	- m_ptr->fTrimX;
	( pData + ( iIndex + 2 ) )->tv1	=   ( fTv )				+ m_ptr->fTrimY;
	( pData + ( iIndex + 3 ) )->tu1 =	( fTu + fTuSeg )	- m_ptr->fTrimX;
	( pData + ( iIndex + 3 ) )->tv1 =   ( fTv + fTvSeg )	- m_ptr->fTrimY;
	( pData + ( iIndex + 0 ) )->tu1 =	( fTu )				+ m_ptr->fTrimX;
	( pData + ( iIndex + 0 ) )->tv1 =   ( fTv + fTvSeg )	- m_ptr->fTrimY;
	( pData + ( iIndex + 1 ) )->tu1 =	( fTu )				+ m_ptr->fTrimX;
	( pData + ( iIndex + 1 ) )->tv1 =   ( fTv )				+ m_ptr->fTrimY;

}

DARKSDK void SetGlobalColor ( int iID, int iA, int iR, int iG, int iB )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	HRESULT hr;
	VOID*	pVertices = NULL;
	DWORD	dwSize    = ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 4 * sizeof ( tagMatrixVertexDesc );
	tagMatrixVertexDesc*	pData	= NULL;

	if ( FAILED ( hr = m_ptr->lpVertexBuffer->Lock ( 0, dwSize, ( VOID** )&pVertices, 0 ) ) )
		Error ( "Failed to lock vertex buffer for matrix manipulation" );

	// cast pointer to our own type
	pData = ( tagMatrixVertexDesc* ) pVertices;

	for ( int iTemp = 0; iTemp < ( ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 4 ); iTemp++ )
	{
		( pData + ( iTemp ) )->diffuse = D3DCOLOR_ARGB ( iA, iR, iG, iB );
	}

	// unlock
	if ( FAILED ( hr = m_ptr->lpVertexBuffer->Unlock ( ) ) )
        return;
}

DARKSDK void SetTileColor ( int iID, int iA, int iR, int iG, int iB )
{

}

DARKSDK void SetWireframe ( int iID, bool bFlag )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bWireframe = bFlag;
}

DARKSDK void SetTransparency ( int iID, bool bFlag )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bTransparency = bFlag;
}

DARKSDK void SetCull ( int iID, bool bCull )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bCull = bCull;
}

DARKSDK void SetFilter ( int iID, int iFilter )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->iFilter = iFilter;
}

DARKSDK void SetLight ( int iID, bool bLight )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bLight = bLight;
}

DARKSDK void SetFog ( int iID, bool bFog )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
}

DARKSDK void SetAmbient ( int iID, bool bAmbient )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
}

DARKSDK bool GetExist ( int iID )
{
	// returns true if the matrix does exist

	if ( !UpdateMatrixPtr ( iID ) )
		return false;
	
	return true;
}

DARKSDK float GetHeight ( int iID, int iX, int iZ )
{
	// get height at tile
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0.0f;
	}

	return m_ptr->pTiles [ iX + ( iZ * m_ptr->iXSegmentedFull ) ].fHeight;
}

DARKSDK float GetGroundHeight ( int iID, int iX, int iZ )
{
	// get y height at tile
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0.0f;
	}

	return m_ptr->pTiles [ iX + ( iZ * m_ptr->iXSegmentedFull ) ].fHeight;
}

DARKSDK int GetTileCount ( int iID )
{
	// get number of texture tiles
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iTextureAcross * m_ptr->iTextureDown;
}

DARKSDK bool GetTilesExist ( int iID )
{
	// do tiles exist
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	if ( m_ptr->lpTexture [ 0 ] )
		return true;
	else
		return false;
}

DARKSDK int GetWidth ( int iID )
{
	// get the width of the matrix
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iWidth;
}

DARKSDK int GetDepth ( int iID )
{
	// get the depth of the matrix
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iDepth;
}

DARKSDK int GetXSegments ( int iID )
{
	// get the number of x segments
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iXSegmented;
}

DARKSDK int GetZSegments ( int iID )
{
	// get the number of z segments
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iZSegmented;
}

DARKSDK int GetTilesAcross ( int iID )
{
	// get the number of texture tiles across
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iTextureAcross;
}

DARKSDK int GetTilesDown ( int iID )
{
	// get the number of texture tiles down
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iTextureDown;
}

DARKSDK int GetFilter ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iFilter;
}

DARKSDK int GetGhostMode ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iGhostMode;
}

DARKSDK bool GetVisible ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bVisible;
}

DARKSDK bool GetWireframe ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bWireframe;
}

DARKSDK bool GetLight ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bLight;
}

DARKSDK bool GetCull ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bCull;
}

DARKSDK bool GetTransparency ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bTransparency;
}

DARKSDK bool GetZDepth ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bZDepth;
}

DARKSDK bool GetFog ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bFog;
}

DARKSDK bool GetAmbient ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bAmbient;
}

DARKSDK bool GetGhost ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bGhost;
}

DARKSDK bool GetLock ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	return m_ptr->bLock;
}

DARKSDK bool LockVertexData ( int iID )
{
	// lock vertex buffer
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	// get size of vertex buffer
	DWORD dwSize = ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 4 * sizeof ( tagMatrixVertexDesc );
	
	// lock the vertex buffer so we can get access to it's data
	if ( FAILED ( m_ptr->lpVertexBuffer->Lock ( 0, dwSize, ( VOID** ) &m_ptr->pBuffer, 0 ) ) )
	{
		Error ( "Failed to lock vertex buffer for matrix manipulation" );
		return false;
	}
	
	return true;
	
}

DARKSDK bool UnlockVertexData ( int iID )
{
	// unlock vertex buffer
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	// unlock
	if ( FAILED ( m_ptr->lpVertexBuffer->Unlock ( ) ) )
        return false;

	// success
	return true;
}

DARKSDK bool LockIndexData ( int iID )
{
	// locks the index buffer
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	// get the size of the index buffer
	DWORD dwIndexSize = sizeof ( WORD ) * ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 6;

	// attempt to lock
	if ( FAILED ( m_ptr->lpIndexBuffer->Lock ( 0, dwIndexSize, ( VOID** )&m_ptr->pBuffer, 0 ) ) )
	{
		Error ( "Failed to lock index buffer for matrix" );
		return false;
	}

	return true;
}

DARKSDK bool UnlockIndexData ( int iID )
{
	// unlock the index buffer
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return false;
	}

	if ( FAILED ( m_ptr->lpIndexBuffer->Unlock ( ) ) )
	{
		Error ( "Failed to unlock index buffer" );
		return false;
	}

	return true;
}

DARKSDK void* GetVertexData ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
		return NULL;

	if ( m_ptr->pBuffer )
		return m_ptr->pBuffer;

	return NULL;
}

DARKSDK void* GetIndexData ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
		return NULL;

	if ( m_ptr->pBuffer )
		return m_ptr->pBuffer;

	return NULL;
}

DARKSDK int GetNumVertices ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return -1;
	}

	return ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 4;
}

DARKSDK int GetNumIndices ( int iID )
{
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return -1;
	}

	return ( m_ptr->iXSegmented * m_ptr->iZSegmented ) * 6;
}

//
// DBV1 Command Functions
//

DARKSDK void MakeEx ( int iID, float fWidth, float fDepth, int iXSegmented, int iZSegmented )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXALREADYEXISTS);
		return;
	}

	// Validate
	bool bOkay=false;
	if(fWidth>0.0f && fDepth>0.0f)
	{
		if(iXSegmented>0 && iZSegmented>0)
		{
			bOkay=true;
		}
		else
			RunTimeError(RUNTIMEERROR_B3DMATRIXSEGMENTWRONG);
	}
	else
		RunTimeError(RUNTIMEERROR_B3DMATRIXDIMENSIONWRONG);

	if(bOkay)
	{
		if ( Make ( iID, (int)fWidth, (int)fDepth, iXSegmented, iZSegmented ) )
		{
			float fRadius = (fWidth + fDepth)/10.0f;
			if(g_Camera3D_SetAutoCam) g_Camera3D_SetAutoCam ( fWidth/2.0f, 0.0f, fDepth/2.0f, fRadius );
		}
		else
			RunTimeError(RUNTIMEERROR_B3DMATRIXTOOLARGE);
	}
}

DARKSDK void Delete ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	SAFE_DELETE_ARRAY ( m_ptr->pTiles );
	SAFE_DELETE_ARRAY ( m_ptr->pfVert );
	SAFE_DELETE_ARRAY ( m_ptr->ptmpTiles );
	SAFE_DELETE_ARRAY ( m_ptr->lpTexture );
	SAFE_RELEASE ( m_ptr->lpIndexBuffer );
	if ( m_ptr->lpIndexBufferArray )
	{
		for ( DWORD n=0; n<=m_ptr->dwIndexBufferArraySize; n++ )
			SAFE_RELEASE ( m_ptr->lpIndexBufferArray[n] );

		SAFE_DELETE ( m_ptr->lpIndexBufferArray );
	}
	SAFE_RELEASE ( m_ptr->lpVertexBuffer );
	SAFE_DELETE ( m_ptr );

	m_pMatrixManager.Delete ( iID );
}

DARKSDK void Fill ( int iID, float fHeight, int iTile )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	int tilecount=m_ptr->iTextureAcross * m_ptr->iTextureDown;
	if(iTile<1 || iTile>tilecount)
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXTILEILLEGAL);
		return;
	}

	// set the heights
	for(int iX=0; iX<=m_ptr->iXSegmented; iX++)
		for(int iZ=0; iZ<=m_ptr->iZSegmented; iZ++)
			SetHeight ( iID, iX, iZ, fHeight );

	// set the tiles
	// mike - 020206 - addition for vs8
	//for(iX=0; iX<m_ptr->iXSegmented; iX++)
	for(int iX=0; iX<m_ptr->iXSegmented; iX++)
		for(int iZ=0; iZ<m_ptr->iZSegmented; iZ++)
			SetTile ( iID, iX, iZ, iTile );
}

DARKSDK void SetTransparencyOn ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	m_ptr->bGhost = true;
	m_ptr->iGhostMode = 0;
	m_ptr->bRenderAfterObjects = true;
}

DARKSDK void SetTransparencyOn ( int iID, int iMode )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	m_ptr->bGhost = true;
	m_ptr->iGhostMode = iMode;
	m_ptr->bRenderAfterObjects = true;
}

DARKSDK void SetTransparencyOff ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	m_ptr->bGhost = false;
	m_ptr->bRenderAfterObjects = false;
}

DARKSDK void PositionEx ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	MatrixPosition ( iID, fX, fY, fZ );
	Apply ( iID );
}

DARKSDK int GetPowerSquareOfSize( int Size )
{
	if ( Size <= 1 ) Size = 1;
	else if ( Size <= 2 ) Size = 2;
	else if ( Size <= 4 ) Size = 4;
	else if ( Size <= 8 ) Size = 8;
	else if ( Size <= 16 ) Size = 16;
	else if ( Size <= 32 ) Size = 32;
	else if ( Size <= 64 ) Size = 64;
	else if ( Size <= 128 ) Size = 128;
	else if ( Size <= 256 ) Size = 256;
	else if ( Size <= 512 ) Size = 512;
	else if ( Size <= 1024 ) Size = 1024;
	else if ( Size <= 2048 ) Size = 2048;
	else if ( Size <= 4096 ) Size = 4096;
	return Size;
}

DARKSDK void PrepareTexture ( int iID, int iImage, int iAcross, int iDown )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	if( iImage<1 || iImage>MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}
	if( g_Image_GetPointer ( iImage )==NULL )
	{
		RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);
		return;
	}
	if( iAcross==0 || iDown==0 )
	{
		return;
	}

	// Texture must be a power of two so replace with correct texture size
	int Width = g_Image_GetWidth ( iImage );
	int Height = g_Image_GetHeight ( iImage );
	int SqWidth = GetPowerSquareOfSize ( Width );
	int SqHeight = GetPowerSquareOfSize ( Height );
	if ( Width==SqWidth && Height==SqHeight )
	{
		// Image is texture Perfect - carry on
	}
	else
	{
		// Adjust image to texture square size
		g_Image_Stretch ( iImage, SqWidth, SqHeight );
	}

	// setup the texture for the matrix
	
	// iID     = matrix id
	// iImage  = reference to image
	// iAcross = how many tiles to break up the image across
	// iDown   = how many tiles to break up the image down

	// the specified texture is taken and broken up into segments
	// e.g. you call PrepareTexture ( 1, 1, 2, 2 ), this would
	// break the texture up into 4 segments, 2 across and 2 down
	// you can then later on reference each segment by index

	// variable declarations
	int iTemp = 0;		// used for loops etc.

	// setup properties
	m_ptr->lpTexture [ 0 ] = g_Image_GetPointer ( iImage );	// get image pointer
	m_ptr->iTextureAcross  = iAcross;						// texture divide across
	m_ptr->iTextureDown    = iDown;							// texture divide down

	// check the texture was valid
	if ( !m_ptr->lpTexture [ 0 ] )
		Error ( "Invalid image specified for matrix" );

	// check the divide properties are valid
	if ( iAcross < 1 || iDown < 1 )
		Error ( "Invalid texture divide" );

	// Resets to solid mode
	m_ptr->bWireframe=false;

	// by default run through all of the tiles and
	// set their texture tile to 1
	for ( int iX = 0; iX < m_ptr->iXSegmented; iX++ )
		for ( int iZ = 0; iZ < m_ptr->iZSegmented; iZ++ )
			SetTile ( iID, iX, iZ, 1 );

	// Apply it for DBV1 backwards compatability
	Apply ( iID );

	return;
}

DARKSDK void Randomize ( int iID, int iHeight )
{
	// this function will take a matrix and set
	// all of it's blocks to a random height
	// specified by iHeight

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	int iX = 0;
	int iZ = 0;

	// update the internal data
	if ( !UpdateMatrixPtr ( iID ) )
		return;

	// leefix - 080908 - true random
	srand((unsigned int)timeGetTime());

	for ( iZ = 0; iZ <= m_ptr->iZSegmented; iZ++ )
	{
		for ( iX = 0; iX <= m_ptr->iXSegmented; iX++ )
		{
			if ( iHeight<=0 )
				SetHeight ( iID, iX, iZ, 0.0f );
			else
				SetHeight ( iID, iX, iZ, (float)(rand()%iHeight) );
		}
	}
}

DARKSDK void SetHeight ( int iID, int iX, int iZ, float fHeight )
{
	// update the height of the specified block
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	
	if ( iX < 0 || iX > m_ptr->iXSegmented || iZ < 0 || iZ > m_ptr->iZSegmented )
	{

  // mike

		RunTimeError(RUNTIMEERROR_B3DMATRIXTILECOORDSWRONG);
		return;
	}
	
	m_ptr->pTiles [ ( iX ) + ( iZ * ( m_ptr->iXSegmentedFull ) ) ].fHeight = fHeight;// * -1.0f;  leefix-200603-shiftinvertbug
}

DARKSDK void SetNormalCore ( int iID, int iX, int iZ, float fX, float fY, float fZ )
{
	// update the internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	D3DXVECTOR3 vNorm = D3DXVECTOR3 ( fX, fY, fZ );
	m_ptr->pTiles [ ( iX ) + ( iZ * ( m_ptr->iXSegmentedFull ) ) ].vNormal = vNorm;
}

DARKSDK void SetNormal ( int iID, int iX, int iZ, float fX, float fY, float fZ )
{
	// update the internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	if ( iX < 0 || iX > m_ptr->iXSegmented || iZ < 0 || iZ > m_ptr->iZSegmented )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXTILECOORDSWRONG);
		return;
	}

	// Special normal assignment to rotated matrix
	D3DXVECTOR3 vNorm = D3DXVECTOR3 ( fX, fZ, fY*-1.0f );
	m_ptr->pTiles [ ( iX ) + ( iZ * ( m_ptr->iXSegmentedFull ) ) ].vNormal = vNorm;
}

DARKSDK void SetTexture ( int iID, int iTextureMode, int iMipGen )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	if ( iTextureMode < 0 || iTextureMode > 2 )
		return;

	// 0 = wrap the texture
	// 1 = mirror the texture 
	// 2 = clamp the texture
	m_ptr->iTextureMode = 1+iTextureMode;

	// 0 = no mipmap
	// 1 = mipmap filter mode
	m_ptr->iMipFilter = iMipGen;
}

DARKSDK void SetTileCore ( int iID, int iX, int iZ, int iTile )
{
	// set the tile pattern (tile based, not corner based)
	if(iX<m_ptr->iXSegmented)
		if(iZ<m_ptr->iZSegmented) 
			m_ptr->pTiles [ ( iX ) + ( iZ * ( m_ptr->iXSegmentedFull ) ) ].iTile = iTile;
}

DARKSDK void SetTile ( int iID, int iX, int iZ, int iTile )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	if ( iX < 0 || iX >= m_ptr->iXSegmented || iZ < 0 || iZ >= m_ptr->iZSegmented )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXTILECOORDSWRONG);
		return;
	}
	int tilecount=m_ptr->iTextureAcross * m_ptr->iTextureDown;
	if(iTile<1 || iTile>tilecount)
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXTILEILLEGAL);
		return;
	}

	// set the tile pattern (tile based, not corner based)
	if(iX<m_ptr->iXSegmented && iZ<m_ptr->iZSegmented) 
		m_ptr->pTiles [ ( iX ) + ( iZ * ( m_ptr->iXSegmentedFull ) ) ].iTile = iTile;
}

DARKSDK void SetWireframeOn ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bWireframe = true;
}

DARKSDK void SetWireframeOff ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	m_ptr->bWireframe = false;
}

DARKSDK void SetEx ( int iID, int iWireframe, int iTransparency, int iCull, int iFilter, int iLight, int iFog, int iAmbient )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// clear settings
	m_ptr->bWireframe		= false;
	m_ptr->bTransparency	= false;
	m_ptr->bCull			= false;
	m_ptr->iFilter			= 0;
	m_ptr->bLight			= false;
	m_ptr->bFog				= false;
	m_ptr->bAmbient			= false;

	// update settings
	if(iWireframe==1)		m_ptr->bWireframe		= true;
	if(iTransparency==1)	m_ptr->bTransparency	= true;
	if(iCull==1)			m_ptr->bCull			= true;
							m_ptr->iFilter			= iFilter; // texture filter
	if(iLight==1)			m_ptr->bLight			= true;
	if(iFog==1)				m_ptr->bFog				= true;
	if(iAmbient==1)			m_ptr->bAmbient			= true;
}

// mike - 230406 - reported as not moving in right direction
//				 - shift down now becomes shift up
//void ShiftDown ( int iID )
DARKSDK void ShiftUp ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// mike : could you wrap the texture UV coords too :)

	// move all of the matrix tiles down by one,
	// and wrap them around e.g.
	
	// using a 5 * 5 grid, before shift down
	
	// 5 = 20 21 22 23 24
	// 4 = 15 16 17 18 19
	// 3 = 10 11 12 13 14
	// 2 =  5  6  7  8  9
	// 1 =  0  1  2  3  4


	// 5 = 15 16 17 18 19
	// 4 = 10 11 12 13 14
	// 3 =  5  6  7  8  9
	// 2 =  0  1  2  3  4
	// 1 = 20 21 22 23 24

	int iX    = 0;
	int iZ	  = 0;
	int iTemp = 0;

	// copy all of the height properties across to the temp buffer
	for ( iTemp = 0; iTemp < m_ptr->iXSegmentedFull * m_ptr->iZSegmentedFull; iTemp++ )
	{
		m_ptr->pfVert [ iTemp ].y = m_ptr->pTiles [ iTemp ].fHeight;
		m_ptr->pfVert [ iTemp ].nx = m_ptr->pTiles [ iTemp ].vNormal.x;
		m_ptr->pfVert [ iTemp ].ny = m_ptr->pTiles [ iTemp ].vNormal.y;
		m_ptr->pfVert [ iTemp ].nz = m_ptr->pTiles [ iTemp ].vNormal.z;
		m_ptr->ptmpTiles [ iTemp ] = m_ptr->pTiles [ iTemp ].iTile;
	}
	
	int iOffset = 0;
	for ( iZ = 1; iZ < m_ptr->iZSegmentedFull; iZ++ )
	{
		for ( iX = 0; iX < m_ptr->iXSegmentedFull; iX++ )	
		{
			SetHeight ( iID, iX, iZ,	m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].y );
			SetNormalCore ( iID, iX, iZ,	m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nx,
											m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].ny,
											m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nz );

			SetTileCore ( iID, iX, iZ,	m_ptr->ptmpTiles [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ] );
		}
		iOffset++;
	}

	iZ=0;
	iOffset = m_ptr->iZSegmentedFull - 1;
	// leefix - 211211 - u78 - not sure where these extra -1 came from, not needed RE DarkGDK
	// iOffset = iOffset - 1; // leefix - 270206 - u60 - we're copying from unshifted temp data (extra -1!)
	for ( iX = 0; iX < m_ptr->iXSegmentedFull; iX++ )
	{
		SetHeight ( iID, iX, iZ,	m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].y );
		SetNormalCore ( iID, iX, iZ,	m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nx,
										m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].ny,
										m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nz );

		SetTileCore ( iID, iX, iZ,	m_ptr->ptmpTiles [ iX + ( (iOffset) * m_ptr->iXSegmentedFull ) ] );
	}

}

DARKSDK void ShiftLeft ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	int iX    = 0;
	int iZ	  = 0;
	int iTemp = 0;
	int iOffset = 0;

	// copy all of the height properties across to the temp buffer
	for ( iTemp = 0; iTemp < m_ptr->iXSegmentedFull * m_ptr->iZSegmentedFull; iTemp++ )
	{
		m_ptr->pfVert [ iTemp ].y = m_ptr->pTiles [ iTemp ].fHeight;
		m_ptr->pfVert [ iTemp ].nx = m_ptr->pTiles [ iTemp ].vNormal.x;
		m_ptr->pfVert [ iTemp ].ny = m_ptr->pTiles [ iTemp ].vNormal.y;
		m_ptr->pfVert [ iTemp ].nz = m_ptr->pTiles [ iTemp ].vNormal.z;
		m_ptr->ptmpTiles [ iTemp ] = m_ptr->pTiles [ iTemp ].iTile;
	}
	
	/*

	// mike - 220406 - comment out because causes a crash, see below

	for ( iZ = 0; iZ < m_ptr->iZSegmentedFull; iZ++ )
	{
		if ( iZ < m_ptr->iZSegmentedFull - 1 )
			iOffset++;
		else
			iOffset = 0;

		for ( iX = 0; iX < m_ptr->iXSegmentedFull; iX++ )
		{
			SetHeight ( iID, iZ, iX, m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].y );
			SetNormalCore ( iID, iZ, iX,	m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nx,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].ny,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nz );

			int iTileOffset = iOffset % m_ptr->iZSegmented;
			SetTileCore ( iID, iZ, iX,	m_ptr->ptmpTiles [ iTileOffset + ( iX * m_ptr->iXSegmentedFull ) ] );
		}
	}
	*/
	
	// mike - 220406 - stop crash with matrices of different sizes
	//				 - e.g. 32 x 64

	int iZMax = m_ptr->iZSegmentedFull;
	int iXMax = m_ptr->iXSegmentedFull;

	//if ( m_ptr->iZSegmentedFull > m_ptr->iXSegmentedFull )
	{
		iZMax = m_ptr->iXSegmentedFull;
		iXMax = m_ptr->iZSegmentedFull;
	}
	
	for ( iZ = 0; iZ < iZMax; iZ++ )
	{
		if ( iZ < iZMax - 1 )
			iOffset++;
		else
			// lee - 270206 - u60 - from unshifted buffer data, extra 1
//			iOffset = 0;
//			iOffset = 1;
			// lee - u78 - 211211 - seems the 2006 correction copied the WRONG line to the rightmost column (DarkGDK report)
			iOffset = 0;

		for ( iX = 0; iX < iXMax; iX++ )
		{
			SetHeight ( iID, iZ, iX, m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].y );
			SetNormalCore ( iID, iZ, iX,	m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nx,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].ny,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nz );

			int iTileOffset = iOffset % m_ptr->iZSegmented;
			SetTileCore ( iID, iZ, iX,	m_ptr->ptmpTiles [ iTileOffset + ( iX * m_ptr->iXSegmentedFull ) ] );
		}
	}
}

DARKSDK void ShiftRight ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	int iX    = 0;
	int iZ	  = 0;
	int iTemp = 0;

	// copy all of the height properties across to the temp buffer
	for ( iTemp = 0; iTemp < m_ptr->iXSegmentedFull * m_ptr->iZSegmentedFull; iTemp++ )
	{
		m_ptr->pfVert [ iTemp ].y = m_ptr->pTiles [ iTemp ].fHeight;
		m_ptr->pfVert [ iTemp ].nx = m_ptr->pTiles [ iTemp ].vNormal.x;
		m_ptr->pfVert [ iTemp ].ny = m_ptr->pTiles [ iTemp ].vNormal.y;
		m_ptr->pfVert [ iTemp ].nz = m_ptr->pTiles [ iTemp ].vNormal.z;
		m_ptr->ptmpTiles [ iTemp ] = m_ptr->pTiles [ iTemp ].iTile;
	}
	
	// mike - 220406 - stop crash with matrices of different sizes
	//				 - e.g. 32 x 64

	int iZMax = m_ptr->iZSegmentedFull;
	int iXMax = m_ptr->iXSegmentedFull;

	//if ( m_ptr->iZSegmentedFull > m_ptr->iXSegmentedFull )
	{
		// lee - 270206 - ?!? X becoming Z?
		iZMax = m_ptr->iXSegmentedFull;
		iXMax = m_ptr->iZSegmentedFull;
	}
	
	int iOffset = 0;
	for ( iZ = 1; iZ < iZMax; iZ++ )
	{
		for ( iX = 0; iX < iXMax; iX++ )	
		{
			SetHeight ( iID, iZ, iX, m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].y );
			SetNormalCore ( iID, iZ, iX,	m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nx,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].ny,
											m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nz );

			SetTileCore ( iID, iZ, iX,	m_ptr->ptmpTiles [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ] );
		}

		iOffset++;
	}

	// copy rightmost data to left side
	iZ = 0;
	//iOffset = iZMax - 1 - 1; // leefix - 270206 - u60 - we're copying from unshifted temp data (extra -1!)
	iOffset = iZMax - 1; // leefix - 211211 - u78 - no need for extra -1 according to DarkGDK
	for ( iX = 0; iX < iXMax; iX++ )
	{
		SetHeight ( iID, iZ, iX, m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].y );
		SetNormalCore ( iID, iZ, iX,	m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nx,
										m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].ny,
										m_ptr->pfVert [ iOffset + ( iX * m_ptr->iXSegmentedFull ) ].nz );

		SetTileCore ( iID, iZ, iX,	m_ptr->ptmpTiles [ (iOffset) + ( iX * m_ptr->iXSegmentedFull ) ] );
	}
}

// mike - 230406 - reported as not moving in right direction
//				 - shift up now becomes shift down
//void ShiftUp ( int iID )
DARKSDK void ShiftDown ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// move all of the matrix tiles up by one,
	// and wrap them around e.g.
	
	// using a 5 * 5 grid, before shift up
	// 5 = 20 21 22 23 24
	// 4 = 15 16 17 18 19
	// 3 = 10 11 12 13 14
	// 2 =  5  6  7  8  9
	// 1 =  0  1  2  3  4

	// after shift up
	// 5 =  0  1  2  3  4
	// 4 = 20 21 22 23 24
	// 3 = 15 16 17 18 19
	// 2 = 10 11 12 13 14
	// 1 =  5  6  7  8  9

	// shift up again
	// 5 =  5  6  7  8  9
	// 4 =  0  1  2  3  4
	// 3 = 20 21 22 23 24
	// 2 = 15 16 17 18 19
	// 1 = 10 11 12 13 14

	// basically take the bottom line of 
	// the grid and move it to the top

	int iX    = 0;
	int iZ	  = 0;
	int iTemp = 0;
	int iOffset = 0;

	// copy all of the height properties across to the temp buffer
	for ( iTemp = 0; iTemp < m_ptr->iXSegmentedFull * m_ptr->iZSegmentedFull; iTemp++ )
	{
		m_ptr->pfVert [ iTemp ].y = m_ptr->pTiles [ iTemp ].fHeight;
		m_ptr->pfVert [ iTemp ].nx = m_ptr->pTiles [ iTemp ].vNormal.x;
		m_ptr->pfVert [ iTemp ].ny = m_ptr->pTiles [ iTemp ].vNormal.y;
		m_ptr->pfVert [ iTemp ].nz = m_ptr->pTiles [ iTemp ].vNormal.z;
		m_ptr->ptmpTiles [ iTemp ] = m_ptr->pTiles [ iTemp ].iTile;
	}

	for ( iZ = 0; iZ < m_ptr->iZSegmentedFull; iZ++ )
	{
		if ( iZ < m_ptr->iZSegmentedFull - 1 )
			iOffset++;
		else
			// lee - 270206 - u60 - from unshifted buffer data, extra 1
//			iOffset = 0;
//			iOffset = 1;
			// lee - u78 - 211211 - seems the 2006 correction copied the WRONG line to the top row (DarkGDK report)
			iOffset = 0;

		for ( iX = 0; iX < m_ptr->iXSegmentedFull; iX++ )
		{
			SetHeight ( iID, iX, iZ, m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].y );
			SetNormalCore ( iID, iX, iZ,	m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nx,
											m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].ny,
											m_ptr->pfVert [ iX + ( iOffset * m_ptr->iXSegmentedFull ) ].nz );

			int iTileOffset = iOffset % m_ptr->iZSegmented;
			SetTileCore ( iID, iX, iZ,	m_ptr->ptmpTiles [ iX + ( iTileOffset * m_ptr->iXSegmentedFull ) ] );
		}
	}
}

DARKSDK void Apply ( int iID )
{
	// apply any changes to the matrix

	// update the internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// variable declarations
	int						iX     = 0;														// x segment
	int						iZ     = 0;														// z segment
	int						iCount = 0;														// counter
	VOID*					pVertices	= NULL;												// pointer for vertices data
	tagMatrixVertexDesc*	pData		= NULL;												// pointer for vertices data
	DWORD					dwSize      = ( m_ptr->iXSegmented * m_ptr->iZSegmented ) *
										  4 * sizeof ( tagMatrixVertexDesc );				// size of matrix in bytes
	
	// lock the vertex buffer so we can get access to it's data
	if ( FAILED ( m_ptr->lpVertexBuffer->Lock ( 0, dwSize, ( VOID** ) &pVertices, 0 ) ) )
		Error ( "Failed to lock vertex buffer for matrix manipulation" );
	
	// cast pointer to our own type
	pData = ( tagMatrixVertexDesc* ) pVertices;

	// run through all segments and apply properties
	for ( iZ = 0; iZ < m_ptr->iZSegmentedFull; iZ++ )
	{
		for ( iX = 0; iX < m_ptr->iXSegmentedFull; iX++ )
		{
			// set the tile height
			SetHeightReal ( iID, pData, iX, iZ, m_ptr->pTiles [ iCount   ].fHeight );

			// set the tile normal
			SetNormalReal ( iID, pData, iX, iZ, m_ptr->pTiles [ iCount   ].vNormal );

			// set the tile pattern (tile based, not corner based)
			if(iX<m_ptr->iXSegmented && iZ<m_ptr->iZSegmented) 
				SetTileReal   ( iID, pData, iX, iZ, m_ptr->pTiles [ iCount ].iTile );

			// next in tiledata
			iCount++;
		}
	}

	// unlock
	if ( FAILED ( m_ptr->lpVertexBuffer->Unlock ( ) ) )
        return;
}

DARKSDK void SetTrim ( int iID, float fTrimX, float fTrimY )
{
	// update the internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// set trim values
	m_ptr->fTrimX = fTrimX;
	m_ptr->fTrimY = fTrimY;
}

DARKSDK void SetPriority ( int iID, int iPriority )
{
	// update the internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// set whethe render first or last
	if ( iPriority==1 )
		m_ptr->bRenderAfterObjects = true;
	else
		m_ptr->bRenderAfterObjects = false;
}

//
// DBV1 Expression Functions
//

DARKSDK DWORD GetPositionXEx ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}
	return MatrixGetXPositionEx( iID );
}

DARKSDK DWORD GetPositionYEx ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}
	return MatrixGetYPositionEx( iID );
}

DARKSDK DWORD GetPositionZEx ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}
	return MatrixGetZPositionEx( iID );
}

DARKSDK float DB_GetGridGroundLevel(float AtX, float AtZ, float fX, float fZ, float fSizeX, float fSizeZ, float fHeightA, float fHeightB, float fHeightC, float fHeightD)
{
	// Find triangle from tile pointed to by lpVector
//	D3DVALUE width = model[ModelIndex].width;
//	D3DVALUE depth = model[ModelIndex].depth;
//	int xseg = model[ModelIndex].xsegments;
//	int zseg = model[ModelIndex].zsegments;
//	int tilex = (int)(AtX / (width / (D3DVALUE)xseg));
//	int tilez = (int)(AtZ / (depth / (D3DVALUE)zseg));
//	int MeshIndex = model[ModelIndex].solid[0].MeshIndex;
//	LPD3DVECTOR lpVector = (LPD3DVECTOR)(MeshObject[MeshIndex].ObjVertices + (tilex * (zseg*12)) + (tilez * 12));

	// Work out actual Y from XZ vector
	D3DXVECTOR3		Vector0, Vector1, Vector2;
	float			TopX, TopZ, NowAtX, NowAtZ;
	float			Percentage;
	D3DXVECTOR3		Cut0to2, Cut1to2;
	float			Distance, FinalY;

	TopX = fX;
	TopZ = fZ;
	NowAtX = AtX - TopX;
	NowAtZ = AtZ - TopZ;

	if(NowAtZ<NowAtX)
	{
		// Triangle A
		Vector0.x = fX;			Vector0.z = fZ;			Vector0.y = fHeightA;//0
		Vector1.x = fX+fSizeX;	Vector1.z = fZ;			Vector1.y = fHeightB;//1
		Vector2.x = fX+fSizeX;	Vector2.z = fZ+fSizeZ;	Vector2.y = fHeightD;//2
	}
	else
	{
		// Triangle B
		Vector0.x = fX+fSizeX;	Vector0.z = fZ+fSizeZ;	Vector0.y = fHeightD;//2
		Vector1.x = fX;			Vector1.z = fZ+fSizeZ;	Vector1.y = fHeightC;//3
		Vector2.x = fX;			Vector2.z = fZ;			Vector2.y = fHeightA;//0
	}

	// Cut vector 0 to 2 to make new point
	if(Vector2.z > Vector0.z)
	{
		if((Vector2.z - Vector0.z)==0.0f)
			Percentage = 0.0f;
		else
			Percentage	=	NowAtZ / (Vector2.z - Vector0.z);
		
		Cut0to2.y	=	Vector0.y + ((Vector2.y - Vector0.y) * Percentage);
		Cut0to2.x	=	Vector0.x + ((Vector2.x - Vector0.x) * Percentage);
	}
	else
	{
		if((Vector0.z - Vector2.z)==0.0f)
			Percentage = 0.0f;
		else
			Percentage	=	NowAtZ / (Vector0.z - Vector2.z);

		Cut0to2.y	=	Vector2.y + ((Vector0.y - Vector2.y) * Percentage);
		Cut0to2.x	=	Vector2.x + ((Vector0.x - Vector2.x) * Percentage);
	}

	// Cut vector 1 to 2 to make new point
	if(Vector2.z > Vector1.z)
	{
		if((Vector2.z - Vector1.z)==0.0f)
			Percentage = 0.0f;		
		else
			Percentage	=	NowAtZ / (Vector2.z - Vector1.z);

		Cut1to2.y	=	Vector1.y + ((Vector2.y - Vector1.y) * Percentage);
		Cut1to2.x	=	Vector1.x + ((Vector2.x - Vector1.x) * Percentage);
	}
	else
	{
		if((Vector1.z - Vector2.z)==0.0f)
			Percentage = 0.0f;		
		else
			Percentage	=	NowAtZ / (Vector1.z - Vector2.z);

		Cut1to2.y	=	Vector2.y + ((Vector1.y - Vector2.y) * Percentage);
		Cut1to2.x	=	Vector2.x + ((Vector1.x - Vector2.x) * Percentage);
	}

	// Intersect new line with X to get real Y
	Distance = Cut1to2.x - Cut0to2.x;
	if(Distance==0.0f)
		Percentage=0.0f;
	else
		Percentage = (AtX - Cut0to2.x) / Distance;
	
	FinalY = Cut0to2.y + ((Cut1to2.y - Cut0to2.y) * Percentage);

	return FinalY;
}

DARKSDK DWORD GetGroundHeightEx	( int iID, float fX, float fZ )
{
	// get y height at tile
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	// this coordin and next
	float fBitX = (float)m_ptr->iWidth/m_ptr->iXSegmented;
	float fBitZ = (float)m_ptr->iDepth/m_ptr->iZSegmented;
	int iX = (int)(fX/fBitX);
	int iZ = (int)(fZ/fBitZ);
	int iX2 = iX+1;
	int iZ2 = iZ+1;

	// ensure coordinare within datamap
	if(iX<0) iX=0;
	if(iZ<0) iZ=0;
	if(iX>m_ptr->iXSegmented) iX=m_ptr->iXSegmented;
	if(iZ>m_ptr->iZSegmented) iZ=m_ptr->iZSegmented;
	if(iX2<0) iX2=0;
	if(iZ2<0) iZ2=0;
	if(iX2>m_ptr->iXSegmented) iX2=m_ptr->iXSegmented;
	if(iZ2>m_ptr->iZSegmented) iZ2=m_ptr->iZSegmented;

	// establish corners of tile we are on
	int aX = iX;  int aZ = iZ;
	int bX = iX2; int bZ = iZ;
	int cX = iX;  int cZ = iZ2;
	int dX = iX2; int dZ = iZ2;

	// get heights of each corner
	float fHeightA = m_ptr->pTiles [ aX + ( aZ * m_ptr->iXSegmentedFull ) ].fHeight;
	float fHeightB = m_ptr->pTiles [ bX + ( bZ * m_ptr->iXSegmentedFull ) ].fHeight;
	float fHeightC = m_ptr->pTiles [ cX + ( cZ * m_ptr->iXSegmentedFull ) ].fHeight;
	float fHeightD = m_ptr->pTiles [ dX + ( dZ * m_ptr->iXSegmentedFull ) ].fHeight;

	// old DBV! matrix height finder
	float fHeight = DB_GetGridGroundLevel(fX, fZ, aX*fBitX, aZ*fBitZ, fBitX, fBitZ, fHeightA, fHeightB, fHeightC, fHeightD);

	// final height
	return *(DWORD*)&fHeight;
}

DARKSDK DWORD GetHeightEx ( int iID, int iX, int iZ )
{
	// get height at tile
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}
	if ( iX < 0 || iX > m_ptr->iXSegmented || iZ < 0 || iZ > m_ptr->iZSegmented )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXTILECOORDSWRONG);
		return 0;
	}

	float fValue = m_ptr->pTiles [ iX + ( iZ * m_ptr->iXSegmentedFull ) ].fHeight;
	return *(DWORD*)&fValue;
}

DARKSDK int GetExistEx ( int iID )
{
	// returns true if the matrix does exist
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
		return 0;
	
	return 1;
}

DARKSDK int GetTileCountEx ( int iID )
{
	// get number of texture tiles
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	return m_ptr->iTextureAcross * m_ptr->iTextureDown;
}

DARKSDK int GetTilesExistEx ( int iID )
{
	// do tiles exist
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	if ( m_ptr->lpTexture [ 0 ] )
		return 1;
	else
		return 0;
}

DARKSDK int GetWireframeEx ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return 0;
	}

	if ( m_ptr->bWireframe )
		return 1;
	else
		return 0;
}

//
// DBPro commands
//

DARKSDK void Set ( int iID, bool bWireframe, bool bTransparency, bool bCull )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// update settings
	m_ptr->bWireframe    = bWireframe;		// wireframe
	m_ptr->bTransparency = bTransparency;	// transparency
	m_ptr->bCull         = bCull;			// cull
}

DARKSDK void Set ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// update settings
	m_ptr->bWireframe    = bWireframe;		// wireframe
	m_ptr->bTransparency = bTransparency;	// transparency
	m_ptr->bCull         = bCull;			// cull
	m_ptr->iFilter       = iFilter;			// texture filter
}

DARKSDK void Set ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// update settings
	m_ptr->bWireframe    = bWireframe;		// wireframe
	m_ptr->bTransparency = bTransparency;	// transparency
	m_ptr->bCull         = bCull;			// cull
	m_ptr->iFilter       = iFilter;			// texture filter
	m_ptr->bLight        = bLight;			// light
}

DARKSDK void Set ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// update settings
	m_ptr->bWireframe    = bWireframe;		// wireframe
	m_ptr->bTransparency = bTransparency;	// transparency
	m_ptr->bCull         = bCull;			// cull
	m_ptr->iFilter       = iFilter;			// texture filter
	m_ptr->bLight        = bLight;			// light
	m_ptr->bFog          = bFog;			// fog
}

DARKSDK void Set ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog, bool bAmbient )
{
	// set object properties

	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}

	// update settings
	m_ptr->bWireframe    = bWireframe;		// wireframe
	m_ptr->bTransparency = bTransparency;	// transparency
	m_ptr->bCull         = bCull;			// cull
	m_ptr->iFilter       = iFilter;			// texture filter
	m_ptr->bLight        = bLight;			// light
	m_ptr->bFog          = bFog;			// fog
	m_ptr->bAmbient      = bAmbient;		// ambient
}

DARKSDK void  SetPositionVector3 ( int iID, int iVector )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	D3DXVECTOR3 vec = g_Types_GetVector ( iVector );
	MatrixPosition ( iID, vec.x, vec.y, vec.z );
	Apply ( iID );
}

DARKSDK void GetPositionVector3 ( int iVector, int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMATRIXNOTEXISTS);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->vecPosition.x, m_ptr->vecPosition.y, m_ptr->vecPosition.z );
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void UpdateMatrix ( void )
{
	Update ( );
}

void UpdateLastMatrix ( void )
{
	UpdateLast ( );
}

void ConstructorMatrix ( HINSTANCE hSetup, HINSTANCE hImage )
{
	Constructor ( hSetup, hImage );
}

void DestructorMatrix ( void )
{
	Destructor ( );
}

void SetErrorHandlerMatrix ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataMatrix ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DMatrix ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbMakeMatrix ( int iID, float fWidth, float fDepth, int iXSegmented, int iZSegmented )
{
	MakeEx ( iID, fWidth, fDepth, iXSegmented, iZSegmented );
}

void dbDeleteMatrix ( int iID )
{
	Delete ( iID );
}

void dbFillMatrix ( int iID, float fHeight, int iTile )
{
	Fill ( iID, fHeight, iTile );
}

void dbGhostMatrixOn ( int iID )
{
	SetTransparencyOn ( iID );
}

void dbGhostMatrixOff ( int iID )
{
	SetTransparencyOff ( iID );
}

void dbPositionMatrix ( int iID, float fX, float fY, float fZ )
{
	PositionEx ( iID, fX, fY, fZ );
}

void dbPrepareMatrixTexture ( int iID, int iImage, int iAcross, int iDown )
{
	PrepareTexture ( iID, iImage, iAcross, iDown );
}

void dbRandomizeMatrix ( int iID, int iHeight )
{
	Randomize ( iID, iHeight );
}

void dbSetMatrixHeight ( int iID, int iX, int iZ, float fHeight )
{
	SetHeight ( iID, iX, iZ, fHeight );
}

void dbSetMatrixNormal ( int iID, int iX, int iZ, float fX, float fY, float fZ )
{
	SetNormal ( iID, iX, iZ, fX, fY, fZ );
}

void dbSetMatrixTexture ( int iID, int iTextureMode, int iMipGen )
{
	SetTexture ( iID, iTextureMode, iMipGen );
}

void dbSetMatrixTile ( int iID, int iX, int iZ, int iTile )
{
	SetTile ( iID, iX, iZ, iTile );
}

void dbSetMatrixWireframeOn ( int iID )
{
	SetWireframeOn ( iID );
}

void dbGhostMatrixOn ( int iID, int iMode )
{
	SetTransparencyOn ( iID, iMode );
}

void dbSetMatrixWireframeOff ( int iID )
{
	SetWireframeOff ( iID );
}

void dbSetMatrix ( int iID, int bWireframe, int bTransparency, int bCull, int iFilter, int bLight, int bFog, int bAmbient )
{
	SetEx ( iID, bWireframe, bTransparency, bCull, iFilter, bLight, bFog, bAmbient );
}

void dbShiftMatrixUp ( int iID )
{
	ShiftUp ( iID );
}

void dbShiftMatrixDown ( int iID )
{
	ShiftDown ( iID );
}

void dbShiftMatrixLeft ( int iID )
{
	ShiftLeft ( iID );
}

void dbShiftMatrixRight ( int iID )
{
	ShiftRight ( iID );
}

void dbUpdateMatrix ( int iID )
{
	Apply ( iID );
}

void dbSetMatrixTrim ( int iID, float fTrimX, float fTrimY )
{
	SetTrim ( iID, fTrimX, fTrimY );
}

void dbSetMatrixPriority ( int iID, int iPriority )
{
	SetPriority ( iID, iPriority );
}

void dbPositionMatrix ( int iID, int iVector )
{
	SetPositionVector3 ( iID, iVector );
}

void dbSetVector3ToMatrixPosition ( int iVector, int iID )
{
	GetPositionVector3 ( iVector, iID );
}

void dbRotateMatrix ( int iID, int iVector )
{
	//SetRotationVector3 ( iID, iVector );
}

void dbSetVector3ToMatrixRotation ( int iVector, int iID )
{
	//GetRotationVector3 ( iVector, iID );
}

float dbMatrixPositionX ( int iID )
{
	DWORD dwReturn = GetPositionXEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbMatrixPositionY ( int iID )
{
	DWORD dwReturn = GetPositionYEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbMatrixPositionZ ( int iID )
{
	DWORD dwReturn = GetPositionZEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbGetGroundHeight ( int iID, float fX, float fZ )
{
	DWORD dwReturn = GetGroundHeightEx ( iID, fX, fZ );
	
	return *( float* ) &dwReturn;
}

float dbGetMatrixHeight ( int iID, int iX, int iZ )
{
	DWORD dwReturn = GetHeightEx ( iID, iX, iZ );
	
	return *( float* ) &dwReturn;
}

int dbMatrixExist ( int iID )
{
	return GetExistEx ( iID );
}

int dbMatrixTileCount ( int iID )
{
	return GetTileCountEx ( iID );
}

int dbMatrixTilesExist ( int iID )
{
	return GetTilesExistEx ( iID );
}

int dbMatrixWireframeState ( int iID )
{
	return GetWireframeEx ( iID );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////