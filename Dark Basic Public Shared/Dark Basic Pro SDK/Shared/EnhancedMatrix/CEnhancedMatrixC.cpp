#include "cenhancedmatrixc.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

// Global Shared Data Pointer (passed in from core)
GlobStruct*				g_pGlob							= NULL;

//////////////////////////////////////////////////////////////////////////
// setup /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef IDirect3DDevice9* ( *GFX_GetDirect3DDevicePFN ) ( void );
HINSTANCE				g_GFX;							// for dll loading
GFX_GetDirect3DDevicePFN	g_GFX_GetDirect3DDevice;	// get pointer to D3D device
//////////////////////////////////////////////////////////////////////////

//
// Some required Camera Commands
//

typedef float			        ( *CAMERA_GetFloatPFN   ) ( int );
typedef void					( *CAMERA3D_RetVoidParamFloat4PFN ) ( float, float, float, float );
CAMERA_GetFloatPFN				g_Camera_GetXPosition;
CAMERA_GetFloatPFN				g_Camera_GetYPosition;
CAMERA_GetFloatPFN				g_Camera_GetZPosition;
CAMERA3D_RetVoidParamFloat4PFN	g_Camera_SetAutoCam;		
int								g_Camera_ID;

//////////////////////////////////////////////////////////////////////////
// image /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN )  ( int );
IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;

CEnhancedMatrixManager		m_EnhancedMatrixManager;
tagEnhancedMatrixData*		m_ptr;
unsigned char*				m_pData;
LPDIRECT3DDEVICE9			m_pD3D;
CMode*						m_pMode;

extern tagObjectPos* m_pPos;

void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera )
{
	//m_pEnhancedMatrixManager = pEnhancedMatrixManager;
	m_ptr					 = NULL;
	m_pData					 = NULL;

	if ( !hSetup || !hImage )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}

	g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )    GetProcAddress ( hCamera, "?GetXPosition@@YAMH@Z" );
	g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )    GetProcAddress ( hCamera, "?GetYPosition@@YAMH@Z" );
	g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )    GetProcAddress ( hCamera, "?GetZPosition@@YAMH@Z" );
	g_Camera_SetAutoCam				= ( CAMERA3D_RetVoidParamFloat4PFN ) GetProcAddress ( hCamera, "?SetAutoCam@@YAXMMMM@Z" );

	g_Image_GetPointer				= ( IMAGE_RetLPD3DTEX9ParamIntPFN )	GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );

	// Default Camera
	#if DB_PRO
		g_Camera_ID = 0;
	#else
		g_Camera_ID = 1;
	#endif
}

void Destructor ( void )
{
	SAFE_DELETE ( m_pData );
	SAFE_DELETE ( m_pMode );
}

void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		Destructor();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		Constructor ( g_pGlob->g_GFX, g_pGlob->g_Image, g_pGlob->g_Camera3D );
		PassCoreData ( g_pGlob );
	}
}

void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
}

void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

bool UpdatePtr ( int iID )
{
	m_ptr  = NULL;
	m_ptr  = m_EnhancedMatrixManager.GetData ( iID );

	if ( m_ptr == NULL )
		return false;

	m_pPos = ( tagObjectPos* ) m_ptr;

	return true;
}

void Update ( void )
{
	m_EnhancedMatrixManager.Update ( g_Camera_GetXPosition ( 1 ), g_Camera_GetYPosition ( 1 ), g_Camera_GetZPosition ( 1 ) );
}

unsigned char GetHeight ( unsigned int iX, unsigned int iY )
{
	unsigned int x = iX % MAP_SIZE;
	unsigned int y = iY % MAP_SIZE;
	return m_pData [ x + ( y * MAP_SIZE ) ];
}

bool MakeFromData ( int iID, LPSTR pHeightMap, int iSize, int iPoolSize, float fMinResolution, float fMultScale )
{
	MAP_SIZE       = iSize;
	POOL_SIZE      = iPoolSize;
	MIN_RESOLUTION = fMinResolution;
	MULT_SCALE     = fMultScale;

	tagEnhancedMatrixData	m_Data;
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	m_Data.pD3D = m_pD3D;

	m_pData = new unsigned char [ MAP_SIZE * MAP_SIZE * sizeof ( unsigned char ) ];

	if ( m_pData == NULL )
		return false;

	if ( pHeightMap == NULL )
		return false;

	// copy height map data
	memcpy ( m_pData, pHeightMap, ( MAP_SIZE * MAP_SIZE ) );

	m_Data.m_pMode = new CQuadTrees ( );
	m_Data.m_pMode->SetDevice ( m_pD3D  );
	m_Data.m_pMode->SetData   ( m_pData );
	m_Data.m_pMode->Setup     ( );
	
	m_EnhancedMatrixManager.Add ( &m_Data, iID );

	if ( !UpdatePtr ( iID ) )
		return false;

	// LEEFIX - 061002 - As the matrix is rotated, we actually need to scale the Z
	Scale    ( iID, 1 * 100, 1 * 100, MULT_SCALE * 100 );
	Scale    ( iID, 1 * 100, 1 * 100, MULT_SCALE * 100 );
	Position ( iID, 0, 0, 0 );
	Rotate   ( iID, -90.0f, 0.0f, 0.0f );
	
	return true;
}

void GetCullDataFromModel(int)
{
}

bool Make ( int iID, char* szHeightMap, int iSize )
{
	MAP_SIZE       = iSize;
	POOL_SIZE      = 4 * 1000;
	MIN_RESOLUTION = 2.1f;
	MULT_SCALE     = 0.25f;

	tagEnhancedMatrixData	m_Data;
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	m_Data.pD3D = m_pD3D;

	FILE *fp;

	m_pData = new unsigned char [ MAP_SIZE * MAP_SIZE * sizeof ( unsigned char ) ];

	if ( m_pData == NULL )
		return false;

	fp = fopen ( szHeightMap, "rb" );

	if ( fp == NULL )
	{
		SAFE_DELETE ( m_pData );
		fclose ( fp );
	}

	fread ( m_pData, 1, ( MAP_SIZE * MAP_SIZE ), fp );

	fclose ( fp );

	m_Data.m_pMode = new CQuadTrees ( );
	m_Data.m_pMode->SetDevice ( m_pD3D  );
	m_Data.m_pMode->SetData   ( m_pData );
	m_Data.m_pMode->Setup     ( );
	
	m_EnhancedMatrixManager.Add ( &m_Data, iID );

	if ( !UpdatePtr ( iID ) )
		return false;

	Scale    ( iID, 1 * 100, MULT_SCALE * 100, 1 * 100 );
	Position ( iID, 0, 0, 0 );
	Rotate   ( iID, -90.0f, 0.0f, 0.0f );

	return true;
}

bool Make ( int iID, char* szHeightMap, int iSize, int iPoolSize, float fMinResolution, float fMultScale )
{
	MAP_SIZE       = iSize;
	POOL_SIZE      = iPoolSize;
	MIN_RESOLUTION = fMinResolution;
	MULT_SCALE     = fMultScale;

	tagEnhancedMatrixData	m_Data;
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	m_Data.pD3D = m_pD3D;

	FILE *fp;

	m_pData = new unsigned char [ MAP_SIZE * MAP_SIZE * sizeof ( unsigned char ) ];

	if ( m_pData == NULL )
		return false;

	fp = fopen ( szHeightMap, "rb" );

	if ( fp == NULL )
	{
		SAFE_DELETE ( m_pData );
		fclose ( fp );
	}

	fread ( m_pData, 1, ( MAP_SIZE * MAP_SIZE ), fp );

	fclose ( fp );

	m_Data.m_pMode = new CQuadTrees ( );
	m_Data.m_pMode->SetDevice ( m_pD3D  );
	m_Data.m_pMode->SetData   ( m_pData );
	m_Data.m_pMode->Setup     ( );
	
	m_EnhancedMatrixManager.Add ( &m_Data, iID );

	if ( !UpdatePtr ( iID ) )
		return false;

	Scale    ( iID, 1 * 100, MULT_SCALE * 100, 1 * 100 );
	Position ( iID, 0, 0, 0 );
	Rotate   ( iID, -90.0f, 0.0f, 0.0f );
	
	return true;
}

void SetTextureDivide ( int iID, int iX, int iY )
{
	if ( !UpdatePtr ( iID ) )
		return;

	// does not do anything..
	m_ptr->m_pMode->SetTextureDivide ( float ( iX ), float ( iY ) );
}

void MakeTerrainCore ( int iID, SDK_LPSTR szHeightMap, int iPoolSize, float fMinResolution, float fMultScale )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return;
	}
	if ( UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINALREADYEXIST );
		return;
	}

	// get picture info
	D3DXIMAGE_INFO Info;
	int iWidth;
	HRESULT hRes = D3DXGetImageInfoFromFile( (char*)szHeightMap, &Info );
	if ( SUCCEEDED ( hRes ) )
	{
		// create surface
		iWidth=Info.Width;
		int iHeight=Info.Height;
		LPDIRECT3DSURFACE9 pDestSurface;

		// DX8->DX9
//		m_pD3D->CreateImageSurface( iWidth, iHeight, D3DFMT_R8G8B8, &pDestSurface ); 
		m_pD3D->CreateOffscreenPlainSurface ( iWidth, iHeight, D3DFMT_R8G8B8, D3DPOOL_SCRATCH, &pDestSurface, NULL );

		hRes = D3DXLoadSurfaceFromFile( pDestSurface, NULL, NULL, (char*)szHeightMap, NULL, D3DX_FILTER_POINT, 0, &Info);
		if ( SUCCEEDED ( hRes ) )
		{
			// extract data
			D3DLOCKED_RECT d3dlr;
			if ( SUCCEEDED ( pDestSurface->LockRect ( &d3dlr, NULL, D3DLOCK_NOSYSLOCK ) ) )
			{
				// if dimensions not equal
				if(iWidth!=iHeight)
				{
					RunTimeError ( RUNTIMEERROR_TERRAINFILEMUSTBESQUARE );
					return;
				}

				// create byte map of height data
				LPSTR pHeightMap = new char[iWidth*iHeight];
				for(int y=0; y<iHeight; y++)
				{
					LPSTR pPtr = (LPSTR)d3dlr.pBits + (y*d3dlr.Pitch);
					for(int x=0; x<iWidth; x++)
					{
						DWORD dwCol = *(DWORD*)pPtr;
						DWORD dwRed   = ((( dwCol ) >>  24 ) & 0xFF );
						DWORD dwGreen = ((( dwCol ) >>  16 ) & 0xFF );
						DWORD dwBlue  = ((( dwCol ) >>  8  ) & 0xFF );
						float fIntensity = ((float)(dwRed) + (float)(dwGreen) + (float)(dwBlue)) / (255*3);
						unsigned char bByte = (unsigned char)(255*fIntensity);
						pHeightMap[(y*iWidth)+x]=bByte;
						pPtr+=3;
					}
				}

				// make terrain from raw data and specified Size
				if ( MakeFromData ( iID, pHeightMap, iWidth, iPoolSize, fMinResolution, fMultScale )==false )
				{
					RunTimeError ( RUNTIMEERROR_TERRAINFAILEDCREATE );
					return;
				}

				// free byte map of heightdata
				SAFE_DELETE(pHeightMap);

				// unlock surface
				pDestSurface->UnlockRect();
			}

			// release surface when done
			SAFE_RELEASE(pDestSurface);
		}
		else
		{
			// Failed to load heightmap
			RunTimeError ( RUNTIMEERROR_TERRAINFAILEDCREATE );
			SAFE_RELEASE(pDestSurface);
			return;
		}
	}
	else
	{
		// Failed to load heightmap
		RunTimeError ( RUNTIMEERROR_TERRAINFAILEDCREATE );
		return;
	}

	// set position 
	Position ( iID, 0.0f, 0.0f, (float)iWidth );

	// clear texture
	UpdatePtr ( iID );

	m_ptr->m_iWidth=iWidth;
	m_ptr->m_pTexture=NULL;

	// height of terrain
	float fHeight = 100.0f;

	// set autocam 
	if ( g_Camera_SetAutoCam )
		g_Camera_SetAutoCam ( iWidth/2.0f, 0.0f, iWidth/2.0f, fHeight/2.0f );
}

//
// Commands
//

void Make ( int iID, SDK_LPSTR szHeightMap )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szHeightMap);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	MakeTerrainCore ( iID, (SDK_LPSTR)VirtualFilename, 4 * 1000, 2.1f, 0.25f );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

void Delete ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return;
	}
	m_EnhancedMatrixManager.Delete ( iID );
}

void SetPosition ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return;
	}
	Position ( iID, fX, fY, fZ );
}

void SetTexture ( int iID, int iImageIndex )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return;
	}

	// set texture
	m_ptr->m_pTexture = g_Image_GetPointer ( iImageIndex );
}

void SetTextureDivideEx ( int iID, float fX, float fY )
{
	// NOT IN DBPROV1
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return;
	}

	// just crashes because the child node of the quadnode is not initialised..
	m_ptr->m_pMode->SetTextureDivide ( fX, fY );
}

//
// Expressions
//

SDK_BOOL GetExist ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return false;
	}

	if ( UpdatePtr ( iID ) )
		return true;
	else
		return false;
}

SDK_FLOAT GetPositionX ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return 0;
	}

	float fPos=m_ptr->vecPosition.x;
	return SDK_RETFLOAT(fPos);
}

SDK_FLOAT GetPositionY ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return 0;
	}

	float fPos=m_ptr->vecPosition.y;
	return SDK_RETFLOAT(fPos);
}

SDK_FLOAT GetPositionZ ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return 0;
	}

	float fPos=m_ptr->vecPosition.z;
	return SDK_RETFLOAT(fPos);
}

SDK_FLOAT GetHeight ( int iID, float fX, float fZ )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return 0;
	}

	// this coordin and next
	float realX = fX;
	float realZ = m_ptr->m_iWidth-fZ;
	int iX = (int)realX;
	int iZ = (int)realZ;
	int iX2 = iX+1;
	int iZ2 = iZ+1;

	// ensure coordinare within datamap
	if(iX<0) iX=0;
	if(iZ<0) iZ=0;
	if(iX>m_ptr->m_iWidth) iX=m_ptr->m_iWidth;
	if(iZ>m_ptr->m_iWidth) iZ=m_ptr->m_iWidth;
	if(iX2<0) iX2=0;
	if(iZ2<0) iZ2=0;
	if(iX2>m_ptr->m_iWidth) iX2=m_ptr->m_iWidth;
	if(iZ2>m_ptr->m_iWidth) iZ2=m_ptr->m_iWidth;

	// establish corners of tile we are on
	int aX = iX;  int aZ = iZ;
	int bX = iX2; int bZ = iZ;
	int cX = iX;  int cZ = iZ2;
	int dX = iX2; int dZ = iZ2;

	// get heights of each corner
	unsigned char* pData = m_ptr->m_pMode->GetData();
	float fHeightA = ( pData[ (aZ*m_ptr->m_iWidth)+aX ]/255.0f ) * 100.0f;
	float fHeightB = ( pData[ (bZ*m_ptr->m_iWidth)+bX ]/255.0f ) * 100.0f;
	float fHeightC = ( pData[ (cZ*m_ptr->m_iWidth)+cX ]/255.0f ) * 100.0f;
	float fHeightD = ( pData[ (dZ*m_ptr->m_iWidth)+dX ]/255.0f ) * 100.0f;

	// find X and Y percentages away from first corner	
	float pPercX = realX-aX;
	float pPercZ = realZ-aZ;

	// simple average for now (until get something proper)
	float fHeightX = fHeightA + (((fHeightC+fHeightD)-(fHeightA+fHeightB))*pPercX);
	float fHeightZ = fHeightA + (((fHeightB+fHeightD)-(fHeightA+fHeightC))*pPercZ);
	float fHeight = (fHeightX+fHeightZ)/2.0f;

	// return final height
	return SDK_RETFLOAT(fHeight);
}

SDK_FLOAT GetTotalHeight ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_TERRAINNOTEXIST );
		return 0;
	}

	float fHeight=100.0f;
	return SDK_RETFLOAT(fHeight);
}
