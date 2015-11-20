
//
// DBOExternals Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBOEXTERNAL HEADER ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "DBOExternals.h"

//////////////////////////////////////////////////////////////////////////
// IMAGE
//////////////////////////////////////////////////////////////////////////




DBPRO_GLOBAL IMAGE_LoadInternalTexturePFN		g_Image_InternalLoad;
DBPRO_GLOBAL IMAGE_LoadInternalTextureDividePFN	g_Image_InternalLoadEx;

DBPRO_GLOBAL IMAGE_RetBoolParamIntPCharPFN		g_Image_Load;						// load an image from disk
DBPRO_GLOBAL IMAGE_RetBoolParamIntPCharPFN		g_Image_Save;						// save an image to disk
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN			g_Image_Delete;						// delete an image from memory
DBPRO_GLOBAL IMAGE_RetLPD3DTEX9ParamInt3PFN		g_Image_Make;
DBPRO_GLOBAL IMAGE_RetLPD3DTEX9ParamInt4PFN		g_Image_MakeFormat;

DBPRO_GLOBAL IMAGE_RetIntParamIntPFN			g_Image_GetWidth;					// get width of an image
DBPRO_GLOBAL IMAGE_RetIntParamIntPFN			g_Image_GetHeight;					// get height of an image
DBPRO_GLOBAL IMAGE_RetBoolParamIntPFN			g_Image_GetExist;					// does image exist
DBPRO_GLOBAL IMAGE_RetLPSTRParamIntPFN			g_Image_GetName;
DBPRO_GLOBAL IMAGE_RetVoidParamBoolPFN			g_Image_SetSharing;					// set sharing mode on / off
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN			g_Image_SetMemory;					// set memory pool
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN			g_Image_Lock;						// lock an image so you can write data to it
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN			g_Image_Unlock;						// unlock an image
DBPRO_GLOBAL IMAGE_RetVoidParamInt7PFN			g_Image_Write;						// write data to an image
DBPRO_GLOBAL IMAGE_RetVoidParamBoolPFN			g_Image_SetMipmapMode;				// set mipmapping on / off
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapType;				// set type of mipmapping - none, point and linear
DBPRO_GLOBAL IMAGE_RetVoidParamFloatPFN			g_Image_SetMipmapBias;				// set mipmap bias

DBPRO_GLOBAL  IMAGE_RetVoidParamInt4PFN			g_Image_SetColorKey;				// set image color key
DBPRO_GLOBAL  IMAGE_RetVoidParamInt2PFN			g_Image_SetTranslucency;			// set image translucency, specify a percentage for it

DBPRO_GLOBAL IMAGE_RetVoidParamICUBEIPFN		g_Image_SetCubeFace;				// set image up as face of cube (render target potential)

#ifdef DARKSDK_COMPILE
	static IMAGE_RetVoidParamHINSTANCEPFN		g_Image_Constructor;
	static IMAGE_RetVoidParamVoidPFN			g_Image_Destructor;
	static IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapNum;
	static IMAGE_RetLPD3DTEX9ParamIntPFN		g_Image_GetPointer;
#else
	IMAGE_RetVoidParamHINSTANCEPFN		g_Image_Constructor;
	IMAGE_RetVoidParamVoidPFN			g_Image_Destructor;					// destructor
	IMAGE_RetVoidParamIntPFN			g_Image_SetMipmapNum;
	IMAGE_RetLPD3DTEX9ParamIntPFN		g_Image_GetPointer;
#endif


//////////////////////////////////////////////////////////////////////////
// CAMERA
//////////////////////////////////////////////////////////////////////////
DBPRO_GLOBAL CAMERA3D_RetVoidParamFloat4PFN		g_Camera3D_SetAutoCam;		
DBPRO_GLOBAL CAMERA3D_GetInternalDataPFN		g_Camera3D_GetInternalData;
DBPRO_GLOBAL CAMERA3D_GetInternalUpdatePFN		g_Camera3D_GetInternalUpdate;
DBPRO_GLOBAL HINSTANCE							g_Camera;
DBPRO_GLOBAL CAMERA_Int1Float3PFN				g_Camera_Position;
DBPRO_GLOBAL CAMERA_GetFloatPFN					g_Camera_GetXPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN					g_Camera_GetYPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN					g_Camera_GetZPosition;

// LIGHT
DBPRO_GLOBAL LIGHT3D_GetInternalDataPFN			g_Light3D_GetInternalData;
