#ifndef _CCAMERADATA_H_
#define _CCAMERADATA_H_

#pragma comment ( lib, "d3dx9.lib"  )
#pragma comment ( lib, "gdi32.lib" )
#include <D3DX9.h>

struct tagCameraData
{
	D3DXMATRIX			matProjection;	// projection matrix
	D3DXMATRIX			matView;		// view matrix

	D3DXVECTOR3			vecLook;		// look vector
	D3DXVECTOR3			vecUp;			// up vector
	D3DXVECTOR3			vecRight;		// right vector
	D3DXVECTOR3			vecPosition;	// position vector

	D3DVIEWPORT9		viewPort2D;		// view port for 2D ( area on screen to take up )
	D3DVIEWPORT9		viewPort3D;		// view port for 3D ( area on screen to take up )

	int					iCameraToImage;
	LPDIRECT3DTEXTURE9	pCameraToImageTexture;
	LPDIRECT3DSURFACE9  pCameraToImageSurface;
	LPDIRECT3DTEXTURE9	pCameraToImageAlphaTexture;
	LPDIRECT3DSURFACE9  pCameraToImageAlphaSurface;

	// Stereoscopic mode for camera images
	int					iStereoscopicMode;
	tagCameraData*		pStereoscopicFirstCamera;
	int					iCameraToStereoImageBack;
	int					iCameraToStereoImageFront;
	LPDIRECT3DTEXTURE9	pCameraToStereoImageBackTexture;
	LPDIRECT3DTEXTURE9	pCameraToStereoImageFrontTexture;
	LPDIRECT3DSURFACE9	pCameraToStereoImageBackSurface;
	LPDIRECT3DSURFACE9	pCameraToStereoImageFrontSurface;	

	int					iBackdropState;	// used as an automatic clear
	DWORD				dwBackdropColor;// color of backdrop

	// Can use XYZ rotation or FreeFlight(YPR)
	bool				bUseFreeFlightRotation;
	D3DXMATRIX			matFreeFlightRotate;
	float				fXRotate, fYRotate, fZRotate;	// rotateangle

	float				fAspect;		// aspect ratio
	float				fAspectMod;
	float				fFOV;			// field of view
	float				fZNear;			// z near
	float				fZFar;			// z far

	bool				bRotate;		// which rotate to use

	// Override camera
	bool				bOverride;
	D3DXMATRIX			matOverride;

	// Clipping planes (so can do reflection tricks)
	int					iClipPlaneOn;
	D3DXPLANE			planeClip;

	// Image ptr storage (DarkSKY) and also some reserves
	int					iBackdropTextureMode;
	LPDIRECT3DTEXTURE9	pBackdropTexture;

	// U75 - 070410 - (used RES1) stores the third parameter to COLOR BACKDROP (special FF foreground color to override textures/effects)
	DWORD				dwForegroundColor;

	// reserved members
	DWORD				dwCameraSwitchBank; // Dave added 4th Aug 2014: 32 bit for camera flags, bit 0 will set camera FOV on/off for fixed to camera objects
	DWORD				dwRes3;
	DWORD				dwRes4;
	DWORD				dwRes5;

	tagCameraData ( );
};

#endif _CCAMERADATA_H_