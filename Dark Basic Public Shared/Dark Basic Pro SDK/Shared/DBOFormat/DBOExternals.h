
//
// DBOExternals Functions Header
//

#ifndef _DBOEXTERNALS_H_
#define _DBOEXTERNALS_H_

//////////////////////////////////////////////////////////////////////////////////////
// COMMON INCLUDES ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "..\global.h"

//////////////////////////////////////////////////////////////////////////////////////
// GLOBSTRUCT INCLUDE ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "..\core\globstruct.h" // DBO needs to know about glob for stencil management - maybe a better way to keep DBO cleaner..
#include "..\\error\\cerror.h"

//////////////////////////////////////////////////////////////////////////////////////
// COMMON EXTERNALS //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
extern LPDIRECT3DDEVICE9	m_pD3D;
extern GlobStruct*			g_pGlob;

//////////////////////////////////////////////////////////////////////////////////////
// TYPEDEFS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// IMAGE TYPEDEFS
typedef void				( *IMAGE_RetVoidParamHINSTANCEPFN ) ( HINSTANCE );
typedef void				( *IMAGE_RetVoidParamVoidPFN )      ( void );
typedef int					( *IMAGE_LoadInternalTexturePFN )	( char* );
typedef int					( *IMAGE_LoadInternalTextureDividePFN )	( char*, int );

typedef bool				( *IMAGE_RetBoolParamIntPCharPFN )  ( int, char* );
typedef void				( *IMAGE_RetVoidParamIntPFN )       ( int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN )  ( int );
typedef int  				( *IMAGE_RetIntParamIntPFN )        ( int );
typedef bool  				( *IMAGE_RetBoolParamIntPFN )       ( int );
typedef LPSTR				( *IMAGE_RetLPSTRParamIntPFN )		( int );
typedef void  				( *IMAGE_RetVoidParamBoolPFN )      ( bool );
typedef void  				( *IMAGE_RetVoidParamInt7PFN )      ( int, int, int, int, int, int, int );
typedef void  				( *IMAGE_RetVoidParamFloatPFN )     ( float );
typedef void  				( *IMAGE_RetVoidParamInt4PFN )      ( int, int, int, int );
typedef void  				( *IMAGE_RetVoidParamInt2PFN )      ( int, int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamInt3PFN )  ( int, int, int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamInt4PFN )  ( int, int, int, int );
typedef void				( *IMAGE_RetVoidParamICUBEIPFN )     ( int, LPDIRECT3DCUBETEXTURE9, int );

// IMAGE
extern IMAGE_RetVoidParamHINSTANCEPFN	g_Image_Constructor;				// constructor
extern IMAGE_RetVoidParamVoidPFN		g_Image_Destructor;					// destructor
extern IMAGE_LoadInternalTexturePFN		g_Image_InternalLoad;				// new int.tex image loader
extern IMAGE_LoadInternalTextureDividePFN g_Image_InternalLoadEx;
extern IMAGE_RetBoolParamIntPCharPFN	g_Image_Load;						// load an image from disk
extern IMAGE_RetBoolParamIntPCharPFN	g_Image_Save;						// save an image to disk
extern IMAGE_RetVoidParamIntPFN			g_Image_Delete;						// delete an image from memory
extern IMAGE_RetLPD3DTEX9ParamInt3PFN	g_Image_Make;
extern IMAGE_RetLPD3DTEX9ParamInt4PFN	g_Image_MakeFormat;
extern IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;					// get pointer to image data, useful to external apps
extern IMAGE_RetIntParamIntPFN			g_Image_GetWidth;					// get width of an image
extern IMAGE_RetIntParamIntPFN			g_Image_GetHeight;					// get height of an image
extern IMAGE_RetBoolParamIntPFN			g_Image_GetExist;					// does image exist
extern IMAGE_RetLPSTRParamIntPFN		g_Image_GetName;
extern IMAGE_RetVoidParamBoolPFN		g_Image_SetSharing;					// set sharing mode on / off
extern IMAGE_RetVoidParamIntPFN			g_Image_SetMemory;					// set memory pool
extern IMAGE_RetVoidParamIntPFN			g_Image_Lock;						// lock an image so you can write data to it
extern IMAGE_RetVoidParamIntPFN			g_Image_Unlock;						// unlock an image
extern IMAGE_RetVoidParamInt7PFN		g_Image_Write;						// write data to an image
extern IMAGE_RetVoidParamBoolPFN		g_Image_SetMipmapMode;				// set mipmapping on / off
extern IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapType;				// set type of mipmapping - none, point and linear
extern IMAGE_RetVoidParamFloatPFN		g_Image_SetMipmapBias;				// set mipmap bias
extern IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapNum;				// set number of mipmaps for when an image is created
extern IMAGE_RetVoidParamInt4PFN		g_Image_SetColorKey;				// set image color key
extern IMAGE_RetVoidParamInt2PFN		g_Image_SetTranslucency;			// set image translucency, specify a percentage for it

extern IMAGE_RetVoidParamICUBEIPFN		g_Image_SetCubeFace;				// set image up as face of cube (render target potential)

// CAMERA TYPEDEFS
typedef void						( *CAMERA3D_RetVoidParamFloat4PFN ) ( float, float, float, float );
typedef void*						( *CAMERA3D_GetInternalDataPFN ) ( int );
typedef void						( *CAMERA3D_GetInternalUpdatePFN ) ( int );
typedef void						( *CAMERA_Int1Float3PFN ) ( int, float, float, float );
typedef void						( *CAMERA_Int1Float1PFN ) ( int, float );
typedef float						( *CAMERA_GetFloatPFN   ) ( int );

// CAMERA
extern CAMERA3D_RetVoidParamFloat4PFN		g_Camera3D_SetAutoCam;		
extern CAMERA3D_GetInternalDataPFN			g_Camera3D_GetInternalData;
extern CAMERA3D_GetInternalUpdatePFN		g_Camera3D_GetInternalUpdate;
extern HINSTANCE							g_Camera;
extern CAMERA_Int1Float3PFN					g_Camera_Position;
extern CAMERA_GetFloatPFN					g_Camera_GetXPosition;
extern CAMERA_GetFloatPFN					g_Camera_GetYPosition;
extern CAMERA_GetFloatPFN					g_Camera_GetZPosition;

// LIGHT
typedef void*								( *LIGHT3D_GetInternalDataPFN ) ( int );
extern LIGHT3D_GetInternalDataPFN			g_Light3D_GetInternalData;

// mike - 050803 - delete call back function
//typedef void(*ON_OBJECT_DELETE_CALLBACK)(int Id, int userData);

#endif _DBOEXTERNALS_H_

// FUNCTIONS FROM COMMON CODE

//extern void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles );
DARKSDK void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles );
extern void ScalesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecScales );
extern void DirectionsFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecDir );
