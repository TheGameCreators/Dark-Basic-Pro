///////////////////////////////////////////////////////////////////////////////////////////////
/// image /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
typedef void	            ( *RetVoidFunctionPointerPFN )     ( ... );
typedef bool	            ( *RetBoolFunctionPointerPFN )     ( ... );
typedef int 	            ( *RetIntFunctionPointerPFN  )     ( ... );
typedef float	            ( *RetFloatFunctionPointerPFN  )     ( ... );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN ) ( int );

typedef bool				( *IMAGE_LoadExPFN ) ( LPSTR, int );

HINSTANCE						g_Image;

RetVoidFunctionPointerPFN		g_Image_Constructor;
RetVoidFunctionPointerPFN		g_Image_Destructor;


RetVoidFunctionPointerPFN		g_Image_Delete;
RetVoidFunctionPointerPFN		g_Image_Make;

IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;
RetIntFunctionPointerPFN		g_Image_GetWidth;
RetIntFunctionPointerPFN		g_Image_GetHeight;
RetBoolFunctionPointerPFN		g_Image_GetExist;
RetFloatFunctionPointerPFN		g_Image_GetUMax;
RetFloatFunctionPointerPFN		g_Image_GetVMax;

RetVoidFunctionPointerPFN		g_Image_SetSharing;
RetVoidFunctionPointerPFN		g_Image_SetMemory;

RetVoidFunctionPointerPFN		g_Image_Lock;
RetVoidFunctionPointerPFN		g_Image_Unlock;
RetVoidFunctionPointerPFN		g_Image_Write;
RetVoidFunctionPointerPFN		g_Image_Get;

RetVoidFunctionPointerPFN		g_Image_SetMipmapMode;
RetVoidFunctionPointerPFN		g_Image_SetMipmapType;
RetVoidFunctionPointerPFN		g_Image_SetMipmapBias;
RetVoidFunctionPointerPFN		g_Image_SetMipmapNum;

RetVoidFunctionPointerPFN		g_Image_SetColorKey;
RetVoidFunctionPointerPFN		g_Image_SetTranslucency;

typedef bool ( *RetLoadSpecialPFN  )     ( char*, int, int, bool );
RetLoadSpecialPFN				g_Image_LoadSpecial;

// mike - 011105 - reference 1 to 1 for animated sprites
typedef bool ( *LoadAnimatedPFN ) ( char*, int, int );
LoadAnimatedPFN					g_ImageLoadAnimated;

#ifndef DARKSDK_COMPILE
	RetBoolFunctionPointerPFN		g_Image_Load;
	RetBoolFunctionPointerPFN		g_Image_Save;	
#else
	typedef bool ( *dbLoadPFN ) ( char*, int );

	dbLoadPFN		g_Image_Load;
	dbLoadPFN		g_Image_Save;	
#endif

	IMAGE_LoadExPFN			g_Image_LoadEx;


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


