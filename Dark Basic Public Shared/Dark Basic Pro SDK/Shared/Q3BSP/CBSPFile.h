//////////////////////////////////////////////////////////////////////////
// information ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 
*/
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// load q3 bsp file //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifndef _CQ3BSP_H_
#define _CQ3BSP_H_

// SDK DEFINES (DBPRO/SDK)
#define DB_PRO 1
#if DB_PRO
 #define SDK_BOOL int
 #define SDK_FLOAT DWORD
 #define SDK_LPSTR DWORD
 #define SDK_RETFLOAT(f) *(DWORD*)&f 
 #define SDK_RETSTR DWORD pDestStr,
#else
 #define SDK_BOOL bool
 #define SDK_FLOAT float
 #define SDK_LPSTR char*
 #define SDK_RETFLOAT(f) f
 #define SDK_RETSTR 
#endif

#define DEMO 0

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
    #undef DARKSDK
    #undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

//////////////////////////////////////////////////////////////////////////
// include files /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include < d3d9.h >
#include < Dxerr9.h >
#include < D3dx9tex.h >
#include < D3dx9core.h >
#include < basetsd.h >
#include < stdio.h >
#include < math.h >
#include < D3DX9.h >
#include < d3d9types.h >

#define WIN32_LEAN_AND_MEAN 
#include < windows.h >
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// link to library files /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "d3d9.lib"   )
#pragma comment ( lib, "d3dx9.lib"  )
#pragma comment ( lib, "dxerr9.lib" )
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// function pointer types ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef IDirect3DDevice9*   ( *RetD3DFunctionPointerPFN  ) ( ... );
typedef void                ( *CAMERA_Int1Float3PFN ) ( int, float, float, float );
typedef void                ( *CAMERA_Int1Float1PFN ) ( int, float );
typedef float               ( *CAMERA_GetFloatPFN   ) ( int );

extern HINSTANCE					g_Setup;
extern RetD3DFunctionPointerPFN		g_Setup_GetDirect3DDevice;
extern HINSTANCE					g_Camera;
extern CAMERA_Int1Float3PFN			g_Camera_Position;
extern CAMERA_GetFloatPFN			g_Camera_GetXPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetYPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetZPosition;
extern CAMERA_Int1Float3PFN			g_Object_Position;
extern CAMERA_GetFloatPFN			g_Object_GetXPosition;
extern CAMERA_GetFloatPFN			g_Object_GetYPosition;
extern CAMERA_GetFloatPFN			g_Object_GetZPosition;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BSP function pointer types ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef void					( *BSP_Constructor )			( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
typedef void					( *BSP_Destructor )				( void );
typedef void					( *RetVoidFCoreDataPFN )		( LPVOID );
typedef bool					( *BSP_Load )					( char* szFilename, char* szMap );
typedef bool					( *BSPOwn_Load )				( char* szFilename );
typedef void					( *BSP_Update )					( void );
typedef void					( *BSP_Delete )					( void );
typedef void					( *BSP_SetupCameraCollision )	( int iID, int iEntityID, float fRadius, int iResponse );
typedef void					( *BSP_SetupObjectCollision )	( int iID, int iEntityID, float fRadius, int iResponse );
typedef void					( *BSP_SetupCameraCollisionR )	( int iID, int iEntityID, float fX, float fY, float fZ );
typedef void					( *BSP_SetupCullingCamera )		( int );
typedef void					( *BSP_SetupCollisionOff )		( int );
typedef void					( *BSP_SetCollisionThreshhold )	( int, float );
typedef void					( *BSP_SetCollisionHeightAdjustment )	( int, float );
typedef int						( *BSP_GetCollisionResponse )	( int );
typedef float					( *BSP_GetCollisionX )			( int );
typedef float					( *BSP_GetCollisionY )			( int );
typedef float					( *BSP_GetCollisionZ )			( int );

typedef void					( *BSP_Start )					( void );
typedef void					( *BSP_ProcessCollision )		( int );
typedef void			        ( *BSP_End )					( void );

typedef void					( *BSPC_Constructor )			( HINSTANCE hSetup );
typedef void					( *BSPC_Destructor )			( void );
typedef void					( *BSPC_CompileBSP )			( char* szInput, char* szOutput, int iFastVis, float fEpsilon, int iSplits );

// Q1and2BSP
extern BSP_Constructor			g_Q2BSP_Constructor;
extern BSP_Destructor			g_Q2BSP_Destructor;
extern BSP_Load					g_Q2BSP_Load;
extern BSP_Update				g_Q2BSP_Update;
extern BSP_SetupCameraCollision	g_Q2BSP_SetupCameraCollision;
extern BSP_SetupObjectCollision	g_Q2BSP_SetupObjectCollision;
extern BSP_Start				g_Q2BSP_Start;
extern BSP_End					g_Q2BSP_End;

extern BSP_End					g_Q2BSP_End;

extern BSP_SetupCameraCollisionR g_Q2BSP_SetCameraCollisionRadius;
extern BSP_SetupCameraCollisionR g_Q2BSP_SetObjectCollisionRadius;

extern BSP_Start				g_Q2BSP_SetHardwareMultiTexturingOff;
extern BSP_Start				g_Q2BSP_SetHardwareMultiTexturingOn;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defines ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

#define ENTITIES_LUMP   0
#define SHADERS_LUMP    1
#define PLANES_LUMP     2
#define NODES_LUMP      3
#define LEAVES_LUMP     4
#define LFACES_LUMP     5
#define LBRUSHES_LUMP   6
#define MODELS_LUMP     7
#define BRUSH_LUMP      8
#define BRUSHSIDES_LUMP 9
#define VERTICES_LUMP   10
#define ELEMENTS_LUMP   11
#define FOG_LUMP        12
#define FACES_LUMP      13
#define LIGHTMAPS_LUMP  14
#define LIGHTGRID_LUMP  15
#define VISIBILITY_LUMP 16

#define LIGHTMAP_BANKSIZE ( 128 * 128 * 3 )

#define RGB_GETRED(rgb)    (((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)  (((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)   ((rgb) & 0xff)

#define RGBA_GETRED(rgb)   (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb) (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)  ((rgb) & 0xff)
#define RGBA_GETALPHA(rgb) (((rgb) >> 24) &0xff)

D3DXVECTOR3 ConvertVectorFromQ3 ( D3DXVECTOR3 src );
D3DXVECTOR3 ConvertVectorFromQ3 ( float src [ 3 ] );
D3DXVECTOR3 ConvertVectorFromQ3 ( int src [ 3 ] );
D3DXVECTOR3 ConvertVectorFromQ3 ( short src [ 3 ] );
D3DXVECTOR3 ConvertVectorToQ3 ( D3DXVECTOR3 src );
float       ConvertVectorToQ3 ( float src );

//////////////////////////////////////////////////////////////////////////
// structures ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct CUSTOMVERTEX
{
    D3DXVECTOR3 v;  // untransformed 3D position for the vertex
    D3DXVECTOR3 n;  // normal
	D3DCOLOR color; // vertex color
	FLOAT tu, tv;   // texture coordinates
	FLOAT lu, lv;   // lightmap coordinates
};

struct plane_t3
{
    float normal [ 3 ];
    float dist;
};

struct model_t3
{
	float	bbox [ 6 ];
    int		face_start;
    int		face_count;
    int		brush_start;
    int		brush_count;
};

struct node_t3
{
    int plane;          // dividing plane
    int front;
    int back;
    int bbox [ 6 ];
};

struct leaf_t3
{
    int cluster;    // visibility cluster number
    int area;
    int bbox [ 6 ];
    int face_start;
    int face_count;
    int brush_start;
    int brush_count;
};

// mike - 260604 //////////////////////////////////////
struct tBSPBrush
{
        int brushSide;           // start brush side
        int numOfBrushSides;     // number of brush sides
        int textureID;           // texture index
};

struct tBSPBrushSide
{
        int plane;				// plane index
        int textureID;			// texture index
};
///////////////////////////////////////////////////////

#define FACETYPE_NORMAL 1
#define FACETYPE_PATCH  2
#define FACETYPE_MESH   3
#define FACETYPE_FLARE  4

struct face_t3
{
    int shader;         // shader ref
    int unknown;
    int type;           // face type
    int vert_start;
    int vert_count;
    int elem_start;
    int elem_count;
    int lm_texture;     // lightmap
    int lm_offset [ 2 ];
    int lm_size [ 2 ];
    float org [ 3 ];       // facetype_normal only
    float bbox [ 6 ];      // facetype_patch only
    float normal [ 3 ];    // facetype_normal only
    int mesh_cp [ 2 ];     // patch control point dims
};

struct shader_t3
{
    char name [ 64 ];
    int surface_flags;
    int content_flags;
};

struct vertex_t3
{
    float point    [ 3 ];
    float texture  [ 2 ];
    float lightmap [ 2 ];
    float normal   [ 3 ];
    int color;
};

struct vis_t3
{
    int cluster_count;
    int row_size;
    byte data [ 1 ];
};

struct mesh_t3
{
	int				verts_count;
	CUSTOMVERTEX*	verts;
	int				elems_count;
	UINT16*			elems;
};

#define TCGEN_BASE 0
#define TCGEN_LIGHTMAP 1
#define TCGEN_ENVIRONMENT 2

// converted contents
struct shader
{
	LPSTR pk3name;  // full path to pk3
	LPSTR filename; // full path to file ( inside pk3 )
	
	LPSTR name;		// texture name

	int stage_count;

	struct tex_stage
	{
		float	ani_freq;			// 0 = no animation
		small	tex_count;
		LPSTR	tex_name [ 32 ];	// texture stage texture ( s ) ( no animation: use tex_name [ 0 ] )
		int		tex_ref  [ 32 ];	// texture ref for the texture
									// Quake3Arena requires 8, "Return to Castle Wolfenstein" wants more - 32 seems to be enough
		struct
		{
			D3DBLEND src,dest;
		} blend;

		D3DCMPFUNC	zfunc;
		BOOL		zwrite;
		BOOL		clamp;
		small		tcgen;
		small		tcmod_count;
		
		struct _tcmod
		{
			float	rotate;
			float	scale_u,
					scale_v;
			float	scroll_u,
					scroll_v;
		} *tcmod;

		struct
		{
			BOOL enable;
			byte ref;
			D3DCMPFUNC func;
		} alphafunc;
	} *stage;
	
	struct
	{
		BOOL	sky;
		BOOL	nonsolid;
		BOOL	playerclip;
		D3DCULL cull;
	} params;

	shader ( )
	{
		stage = NULL;
	}

	void Release ( )
	{
		for ( int x = 0; x < stage_count; x++ )
		{
			for ( int y = 0; y < stage [ x ].tex_count; y++ )
			{
				SAFE_DELETE_ARRAY ( stage [ x ].tex_name [ y ] );
			}

			stage [ x ].tex_count = 0;
            if (stage [ x ].tcmod)
            {
                free( stage [ x ].tcmod );
                stage [ x ].tcmod = 0;
            }
			stage [ x ].tcmod_count = 0;
		}

        if (stage)
        {
            free( stage );
            stage = 0;
        }

		SAFE_DELETE_ARRAY ( pk3name );
		SAFE_DELETE_ARRAY ( filename );
		SAFE_DELETE_ARRAY ( name );

		stage_count = 0;
	}

	BOOL loaded;

	/*
	~shader ( )
	{
		Release ( );
	}
	*/
};

extern struct _shaders
{
	int		entry_count;
	shader*	entry;

	void Init ( )
	{
		if ( entry_count > 0 )
			Release ( );

		entry       = NULL;
		entry_count = 0;
		size        = 0;
	}

	int size;

	void Resize ( int num )
	{
		if ( num > size )
		{
			num   += 100;
			entry  = ( shader* ) realloc ( entry, sizeof ( shader ) * num );
			size   = num;
		}
		else if ( size - num > 100 )
		{
			entry = ( shader* ) realloc ( entry, sizeof ( shader ) * num );
			size  = num;
		}
	}
	
	void Release ( )
	{
		for ( int x = 0; x < entry_count; x++ )
		{
			entry [ x ].Release ( );

			/*
			for ( int y = 0; y < entry [ x ].stage_count; y++ )
			{
				for ( int z = 0; z < entry [ x ].stage [ y ].tex_count; z++ )
				{
					SAFE_DELETE ( entry [ x ].stage [ y ].tex_name [ z ] );
				}

				SAFE_DELETE_ARRAY ( entry [ x ].stage [ y ].tcmod );
				
				entry [ x ].stage [ y ].tex_count = 0;
			}

			SAFE_DELETE ( entry [ x ].stage );

			entry [ x ].stage_count = 0;
			*/
		}

        if (entry)
        {
            free( entry );
            entry = 0;
        }
		entry_count = 0;
	}

	void ReInit ( )
	{
		for ( int x = 0; x < entry_count; x++ )
		{
			for ( int y = 0; y < entry [ x ].stage_count; y++ )
			{
				for ( int z = 0; z < entry [ x ].stage [ y ].tex_count; z++ )
				{
					entry [ x ].stage [ y ].tex_ref [ z ] = -1;
				}
			}

			entry [ x ].loaded = FALSE;
		}
	}

} shaders;

extern byte*      entities3;  extern int entity_size3;
extern plane_t3*  planes3;    extern int plane_count3;
extern model_t3*  models3;    extern int model_count3;
extern node_t3*   nodes3;     extern int node_count3;
extern leaf_t3*   leaves3;    extern int leaf_count3;
extern int*       lfaces3;    extern int lface_count3;
extern face_t3*   faces3;     extern int face_count3;
extern int*       elems3;     extern int elem_count3;
extern shader_t3* shaders3;   extern int shader_count3;
extern vertex_t3* vertices3;  extern int vertex_count3;
extern byte*      lightmaps3; extern int lightmap_count3;
extern vis_t3*    vis3;

extern int					m_numOfLeafBrushes;
extern int					m_numOfBrushes;
extern int					m_numOfBrushSides;
extern int*				m_pLeafBrushes;
extern tBSPBrush*			m_pBrushes;
extern tBSPBrushSide*		m_pBrushSides;

//extern struct _Q3MAP
struct _Q3MAP
{
	int				vert_count;
	CUSTOMVERTEX*	verts;
	int				elem_count;
	UINT16*			elems;
	int				mesh_count;
	mesh_t3			meshes [ 65536 ];
	
	struct face
	{
		int			type;			// face type
		int			vert_start;
		int			vert_count;
		int			elem_start;
		int			elem_count;
	    int			tex_ref;
		int			shader;			// shader ref
	    int			lmap;			// lightmap ref
		int			mesh;			// mesh ref
		float		dist;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 org;			// used for facetype NORMAL, MESH and FLARE
	} *faces;

	BOOL vis;					// some maps do not have vis information
	BOOL mark_faces [ 65536 ];	// faces that will be drawn

	struct _sort
	{
		int tex_count;

		struct _tex
		{
			int  tex_ref;
			int  face_count;
			int* face;
		} *tex;
		
		void Release ( )
		{
			for ( int x = 0; x < tex_count; x++ )
            {
                if (tex [ x ].face)
                {
                    free( tex [ x ].face );
                    tex [ x ].face = 0;
                }
            }

			tex_count = 0;

            if (tex)
            {
                free( tex );
                tex = 0;
            }
		}
	} sort;

	void Release ( )
	{
		for ( int iTemp = 0; iTemp < 65536; iTemp++ )
		{
            if (meshes [ iTemp ].verts)
            {
                free( meshes [ iTemp ].verts );
                meshes [ iTemp ].verts = 0;
            }
            if (meshes [ iTemp ].elems)
            {
                free( meshes [ iTemp ].elems );
                meshes [ iTemp ].elems = 0;
            }
		}
		
		vert_count = 0;
		SAFE_DELETE ( verts );
		elem_count = 0;
		SAFE_DELETE ( elems );
		SAFE_DELETE ( faces );
		sort.Release ( );

		SAFE_DELETE ( entities3  ); entity_size3    = 0;
		SAFE_DELETE ( planes3    ); plane_count3    = 0;
		SAFE_DELETE ( models3    ); model_count3    = 0;
		SAFE_DELETE ( nodes3     ); node_count3     = 0;
		SAFE_DELETE ( leaves3    ); leaf_count3     = 0;
		SAFE_DELETE ( lfaces3    ); lface_count3    = 0;
		SAFE_DELETE ( faces3     ); face_count3     = 0; 
		SAFE_DELETE ( elems3     ); elem_count3     = 0;
		SAFE_DELETE ( shaders3   ); shader_count3   = 0;
		SAFE_DELETE ( vertices3  ); vertex_count3   = 0;
		SAFE_DELETE ( lightmaps3 ); lightmap_count3 = 0;
		SAFE_DELETE ( vis3       );
	}

}; //Q3MAP;

extern _Q3MAP Q3MAP;

//
// Locals
//

BOOL InitMapQ3A            ( byte* data );
int  find_node3            ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere );
int  create_patch          ( int face_idx );
BOOL InitShaders           ( );
void AddShaders            ( LPSTR pk3name);
BOOL AddShaderFile         ( byte* data, int length, LPSTR pk3name, LPSTR filename );
BOOL Q3ARenderTextureFaces ( int stex );

void LoadQ3Map  ( LPSTR filename );
BOOL LoadQ3AMap ( LPSTR filename );
BOOL LoadQ3AMap ( LPSTR filename, char* szMap, BOOL reset );

//
// Internals
//

#ifndef DARKSDK_COMPILE
	DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
	DARKSDK void Destructor ( void );
	DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr );
	DARKSDK void PassCoreData( LPVOID pGlobPtr );
	DARKSDK void RefreshD3D ( int iMode );

	DARKSDK void End ( void );
#else
	static void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
	static void Destructor ( void );
	static void SetErrorHandler ( LPVOID pErrorHandlerPtr );
	static void PassCoreData( LPVOID pGlobPtr );
	static void RefreshD3D ( int iMode );

	void ConstructorWorld ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
	void DestructorWorld ( void );
	void SetErrorHandlerWorld ( LPVOID pErrorHandlerPtr );
	void PassCoreDataWorld( LPVOID pGlobPtr );
	void RefreshD3DWorld ( int iMode );

	void End ( void );
#endif

#ifndef DARKSDK_COMPILE
	DARKSDK void Update ( void );
	DARKSDK void Delete ( void );
	DARKSDK void Start ( void );
#else
	void dbUpdateBSP ( void );
	void DeleteBSP ( void );
	void StartBSP ( void );
	void dbEndBSP ( void );

	static void Update ( void );
	static void Delete ( void );
	static void Start ( void );
#endif

 
DARKSDK bool Load   ( char* szFilename, char* szMap );
DARKSDK void SetupCameraCollision		( int iID, int iEntityID, float fRadius, int iResponse );
DARKSDK void SetupObjectCollision		( int iID, int iEntityID, float fRadius, int iResponse );
DARKSDK void SetupCullingCamera			( int iID );
DARKSDK void SetupCullingCamera			( int iID );




//
// Commands
//

DARKSDK void LoadEx						( SDK_LPSTR szFilename, SDK_LPSTR szMap );
DARKSDK void DeleteEx					( void );
DARKSDK void UpdateEx					( void );
DARKSDK void StartEx					( void );
DARKSDK void ProcessCollisionEx			( int iID );
DARKSDK void EndEx						( void );
DARKSDK void SetupCameraCollisionEx		( int iID, int iEntityID, float fRadius, int iResponse );
DARKSDK void SetupObjectCollisionEx		( int iID, int iEntityID, float fRadius, int iResponse );
DARKSDK void SetupCullingCameraEx		( int iID );
DARKSDK void SetupCollisionOffEx		( int iID );
DARKSDK void SetCollisionThreshholdEx	( int iID, float fSensitivity );
DARKSDK void SetCollisionHeightAdjustmentEx	( int iID, float fHeight );
DARKSDK void SetCameraCollisionRadiusEx ( int iID, int iEntityID, float fX, float fY, float fZ );
DARKSDK void SetObjectCollisionRadiusEx ( int iID, int iEntityID, float fX, float fY, float fZ );

DARKSDK void SetHardwareMultiTexturingOnEx  ( void );
DARKSDK void SetHardwareMultiTexturingOffEx ( void );

//
// BSP Compiler Generator
//

//DARKSDK void CompileBSPEx ( SDK_LPSTR szInput, SDK_LPSTR szOutput, int iFastVis, SDK_FLOAT fEpsilon, int iSplits );

//
// Expressions
//

DARKSDK int GetCollisionResponseEx		( int iID );
DARKSDK DWORD GetCollisionXEx			( int iID );
DARKSDK DWORD GetCollisionYEx			( int iID );
DARKSDK DWORD GetCollisionZEx			( int iID );

#endif _CQ3BSP_H_
