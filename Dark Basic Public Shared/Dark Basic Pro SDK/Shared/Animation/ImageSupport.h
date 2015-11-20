///////////////////////////////////////////////////////////////////////////////////////////////
/// image /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
typedef void	            ( *RetVoidFunctionPointerPFN )     ( ... );
typedef bool	            ( *RetBoolFunctionPointerPFN )     ( ... );
typedef int 	            ( *RetIntFunctionPointerPFN  )     ( ... );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN ) ( int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamInt3PFN ) ( int, int, int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamInt3DWPFN ) ( int, int, int, DWORD );

static HINSTANCE						g_Image;

static RetVoidFunctionPointerPFN		g_Image_Constructor;
static RetVoidFunctionPointerPFN		g_Image_Destructor;

static RetBoolFunctionPointerPFN		g_Image_Load;
static RetBoolFunctionPointerPFN		g_Image_Save;
static RetVoidFunctionPointerPFN		g_Image_Delete;

static IMAGE_RetLPD3DTEX9ParamInt3PFN		g_Image_Make;
static IMAGE_RetLPD3DTEX9ParamInt3DWPFN		g_Image_MakeUsage;
static IMAGE_RetLPD3DTEX9ParamIntPFN		g_Image_GetPointer;

static RetIntFunctionPointerPFN			g_Image_GetWidth;
static RetIntFunctionPointerPFN			g_Image_GetHeight;
static RetBoolFunctionPointerPFN		g_Image_GetExist;

static RetVoidFunctionPointerPFN		g_Image_SetSharing;
static RetVoidFunctionPointerPFN		g_Image_SetMemory;

static RetVoidFunctionPointerPFN		g_Image_Lock;
static RetVoidFunctionPointerPFN		g_Image_Unlock;
static RetVoidFunctionPointerPFN		g_Image_Write;
static RetVoidFunctionPointerPFN		g_Image_Get;

static RetVoidFunctionPointerPFN		g_Image_SetMipmapMode;
static RetVoidFunctionPointerPFN		g_Image_SetMipmapType;
static RetVoidFunctionPointerPFN		g_Image_SetMipmapBias;
static RetVoidFunctionPointerPFN		g_Image_SetMipmapNum;

static RetVoidFunctionPointerPFN		g_Image_SetColorKey;
static RetVoidFunctionPointerPFN		g_Image_SetTranslucency;
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


