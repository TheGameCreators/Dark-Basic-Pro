#ifndef _CIMAGE_H_
#define _CIMAGE_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "gdi32.lib" )


#include <D3D9.h>
#include <D3DX9.h>

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

#define DB_PRO 1
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct tagData
{
	LPDIRECT3DTEXTURE9		lpTexture;					// texture
	LPSTR					lpName;

	LPDIRECT3DCUBETEXTURE9	pCubeMapRef;				// for cubemap rendertarget override
	int						iCubeMapFace;				// which face of the cube to render too
	
	int						iWidth;						// width of texture
	int						iHeight;					// height of texture
	int						iDepth;						// depth of texture
	bool					bLocked;					// is locked

	float					fTexUMax;					// U-coord for texture clip
	float					fTexVMax;					// V-coord for texture clip

	D3DLOCKED_RECT			d3dlr;						// information structure

	char					szLongFilename  [ 256 ];	// long filename  e.g. "c:\\images\\a.bmp"
	char					szShortFilename [ 256 ];	// short filename e.g. ".\\a.bmp"
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void 				ConstructorImage			( HINSTANCE hSetup );
		void 				DestructorImage				( void );
		void 				SetErrorHandlerImage		( LPVOID pErrorHandlerPtr );
		void 				PassSpriteInstanceImage		( HINSTANCE hSprite );
		void				RefreshD3DImage				( int iMode );
		void				PassCoreDataImage			( LPVOID pGlobPtr );
#endif

DARKSDK bool                UpdatePtr					( int iID );
DARKSDK LPDIRECT3DTEXTURE9	GetTexture					( char* szFilename, int iOneToOnePixels );

DARKSDK void				Constructor					( HINSTANCE hSetup );
DARKSDK void				Destructor					( void );
DARKSDK void				SetErrorHandler				( LPVOID pErrorHandlerPtr );
DARKSDK void				PassSpriteInstance			( HINSTANCE hSprite );
DARKSDK void				RefreshD3D					( int iMode );
DARKSDK void				PassCoreData				( LPVOID pGlobPtr );

DARKSDK void				Stretch						( int iID, int iSizeX, int iSizeY );

DARKSDK int					LoadInternal				( char* szFilename );
DARKSDK int					LoadInternal				( char* szFilename, int iDivideTextureSize );
DARKSDK void				ClearAnyLightMapInternalTextures ( void );

DARKSDK LPDIRECT3DTEXTURE9	Make						( int iID, int iWidth, int iHeight );	// makes a new image
DARKSDK LPDIRECT3DTEXTURE9	MakeUsage					( int iID, int iWidth, int iHeight, DWORD dwUsage );
DARKSDK LPDIRECT3DTEXTURE9	MakeJustFormat				( int iID, int iWidth, int iHeight, D3DFORMAT Format );
DARKSDK LPDIRECT3DTEXTURE9	MakeRenderTarget			( int iID, int iWidth, int iHeight, D3DFORMAT Format );

DARKSDK void				SetCubeFace					( int iID, LPDIRECT3DCUBETEXTURE9 pCubeMap, int iFace );
DARKSDK void				GetCubeFace					( int iID, LPDIRECT3DCUBETEXTURE9* ppCubeMap, int* piFace );
	
DARKSDK LPDIRECT3DTEXTURE9	GetPointer					( int iID );		// get the image
DARKSDK int					GetWidth					( int iID );		// get image width
DARKSDK int					GetHeight					( int iID );		// get image height
DARKSDK float				GetUMax						( int iID );
DARKSDK float				GetVMax						( int iID );

DARKSDK void				SetSharing					( bool bMode );					// set image share mode
DARKSDK void				SetMemory					( int iMode );					// set memory location

DARKSDK void				Lock						( int iID );													// lock image
DARKSDK void				Unlock						( int iID );													// unlock image
DARKSDK void				Write						( int iID, int iX, int iY, int iA, int iR, int iG, int iB );	// write data to image
DARKSDK void				Get							( int iID, int iX, int iY, int* piR, int* piG, int* piB );		// get pixel data from an image

DARKSDK void				SetMipmapMode				( bool bMode );		// set mip map mode on / off
DARKSDK void 				SetMipmapType 				( int iType );		// set type e.g linear
DARKSDK void				SetMipmapBias 				( float fBias );		// set mip map bias
DARKSDK void 				SetMipmapNum  				( int iNum );		// set num of mip maps when image is loaded
DARKSDK void 				SetTranslucency 			( int iID, int iPercent );			// set translucency

DARKSDK bool 				Load						( char* szFilename, int iID );							// load an image specifying the filename
DARKSDK bool 				LoadRetainName				( char* szRealName, char* szFilename, int iID, int TextureFlag, bool bIgnoreNegLimit, int iDivideTextureSize );	
DARKSDK bool 				Load						( char* szFilename, int iID, int TextureFlag, bool bIgnoreNegLimit, int iDivideTextureSize );			// load an image specifying the filename
DARKSDK bool 				Load						( char* szFilename, LPDIRECT3DTEXTURE9* pImage );		// loads an image direct to texture
DARKSDK bool 				LoadFullTex					( char* szFilename, LPDIRECT3DTEXTURE9* pImage, int iFullTexMode, int iDivideTextureSize );		// loads an image direct to texture
DARKSDK bool 				Save						( char* szFilename, int iID );							// saves the image to disk
DARKSDK bool 				Save						( char* szFilename, int iID, int iCompressionMode );							// saves the image to disk
DARKSDK bool 				GrabImage 					( int iID, int iX1, int iY1, int iX2, int iY2 );		// grab image
DARKSDK bool 				GrabImage 					( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlag );		// grab image
DARKSDK void				TransferImage				( int iDestImgID, int iSrcImageID, int iTransferMode, int iMemblockAssistor );
DARKSDK void 				Paste						( int iID, int iX, int iY );								// paste image to backbuffer
DARKSDK void 				Paste						( int iID, int iX, int iY, int iFlag );								// paste image to backbuffer
DARKSDK void 				Delete						( int iID );											// delete an image in the list
DARKSDK bool 				GetExist					( int iID );											// does image exist
DARKSDK LPSTR 				GetName						( int iID );

DARKSDK void 				OpenImageBlock				( char* szFilename, int iMode );
DARKSDK void 				ExcludeFromImageBlock		( char* szExcludePath );
DARKSDK void 				CloseImageBlock				( void );
DARKSDK void 				PerformChecklistForImageBlockFiles ( void );
DARKSDK bool				AddToImageBlock				( LPSTR pAddFilename );
DARKSDK LPSTR				RetrieveFromImageBlock		( LPSTR pRetrieveFilename, DWORD* pdwFileSize );
DARKSDK int 				GetImageFileExist			( LPSTR pFilename );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void 				LoadEx						( LPSTR szFilename, int iID );							// load an image specifying the filename
DARKSDK void 				LoadEx						( LPSTR szFilename, int iID, int TextureFlag );							// load an image specifying the filename
DARKSDK void 				LoadEx						( LPSTR szFilename, int iID, int TextureFlag, int iDivideTextureSize );							// load an image specifying the filename
DARKSDK void 				LoadEx						( LPSTR szFilename, int iID, int TextureFlag, int iDivideTextureSize, int iSilentError );
DARKSDK void 				SaveEx						( LPSTR szFilename, int iID );							// saves the image to disk
DARKSDK void 				SaveEx						( LPSTR szFilename, int iID, int iCompressionMode );
DARKSDK void 				GrabImageEx					( int iID, int iX1, int iY1, int iX2, int iY2 );		// grab image
DARKSDK void 				GrabImageEx					( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlag );		// grab image
DARKSDK void 				PasteEx						( int iID, int iX, int iY );								// paste image to backbuffer
DARKSDK void 				PasteEx						( int iID, int iX, int iY, int iFlag );					// paste image to backbuffer
DARKSDK void 				DeleteEx					( int iID );											// delete an image in the list
DARKSDK void 				RotateImageEx				( int iID, int iAngle );											// delete an image in the list
DARKSDK int  				GetExistEx					( int iID );											// does image exist

DARKSDK void  				SetColorKey					( int iR, int iG, int iB );		// set image color key
//DARKSDK void  				SaveIconFromImage			( LPSTR pFilename, int iID );
DARKSDK DWORD 				LoadIcon					( LPSTR pFilename );
DARKSDK void  				FreeIcon					( DWORD dwIconHandle );

DARKSDK void 				GetImageData				( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
DARKSDK void 				SetImageData				( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize );

DARKSDK void 				SetImageAutoMipMap			( int iGenerateMipMaps );

#ifdef DARKSDK_COMPILE
		void 				dbLoadImage					( char* szFilename, int iID );
		void 				dbLoadImage					( char* szFilename, int iID, int TextureFlag );
		void 				dbLoadImage					( char* szFilename, int iID, int TextureFlag, int iDivideTextureSize );
		void 				dbSaveImage					( char* szFilename, int iID );
		void 				dbSaveImage					( char* szFilename, int iID, int iCompressionMode );
		void 				dbGetImage					( int iID, int iX1, int iY1, int iX2, int iY2 );
		void 				dbGetImage					( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlag );
		void 				dbPasteImage				( int iID, int iX, int iY );
		void 				dbPasteImage				( int iID, int iX, int iY, int iFlag );
		void 				dbDeleteImage				( int iID );						
		void 				dbRotateImage				( int iID, int iAngle );			
		int  				dbImageExist				( int iID );

		void  				dbSetImageColorKey			( int iR, int iG, int iB );
		void  				dbSaveIconFromImage			( char* pFilename, int iID );
		DWORD 				dbLoadIcon					( char* pFilename );
		void  				dbFreeIcon					( DWORD dwIconHandle );

		void 				dbGetImageData				( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
		void 				dbSetImageData				( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize );

		LPDIRECT3DTEXTURE9	dbGetImagePointer			( int iID );
		LPDIRECT3DTEXTURE9	dbMakeImage					( int iID, int iWidth, int iHeight );
		LPDIRECT3DTEXTURE9	dbMakeImageUsage			( int iID, int iWidth, int iHeight, DWORD dwUsage );
		LPDIRECT3DTEXTURE9	dbMakeImageJustFormat		( int iID, int iWidth, int iHeight, D3DFORMAT Format );
		LPDIRECT3DTEXTURE9	dbMakeImageRenderTarget		( int iID, int iWidth, int iHeight, D3DFORMAT Format );
		LPDIRECT3DTEXTURE9	dbGetImagePointer			( int iID );
		int					dbGetImageWidth				( int iID );
		int					dbGetImageHeight			( int iID );
		void 				dbPasteImage				( int iID, int iX, int iY );
		void 				dbPasteImage				( int iID, int iX, int iY, int iFlag );

		int					dbLoadImageInternal			( char* szFilename );
		int					dbLoadImageInternal			( char* szFilename, int iDivideTextureSize );

		bool 				dbGetImageExist				( int iID );
		LPSTR 				dbGetImageName				( int iID );

		void				dbSetImageSharing			( bool bMode );
		void				dbSetImageMemory			( int iMode );

		void				dbLockImage					( int iID );
		void				dbUnlockImage				( int iID );
		void				dbWriteImage				( int iID, int iX, int iY, int iA, int iR, int iG, int iB );
		void				dbGetImage					( int iID, int iX, int iY, int* piR, int* piG, int* piB );

		void				dbSetImageMipmapMode		( bool bMode );
		void 				dbSetImageMipmapType 		( int iType );
		void				dbSetImageMipmapBias 		( float fBias );
		void 				dbSetImageMipmapNum  		( int iNum );
		void 				dbSetImageTranslucency 		( int iID, int iPercent );

		void				dbSetImageCubeFace			( int iID, LPDIRECT3DCUBETEXTURE9 pCubeMap, int iFace );
		void				dbGetImageCubeFace			( int iID, LPDIRECT3DCUBETEXTURE9* ppCubeMap, int* piFace );

		bool 				dbLoadImageEx				( char* szFilename, int iID );

		float				dbGetImageUMax				( int iID );
		float				dbGetImageVMax				( int iID );

		void				dbStretchImage				( int iID, int iSizeX, int iSizeY );
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CIMAGE_H_