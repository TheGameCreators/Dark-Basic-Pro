#include "cQ2BSP.h"
#include ".\..\error\cerror.h"
#include "loader.h"
#include "textures.h"
#include "collision.h"

// Include Global Structure
#include ".\..\core\globstruct.h"

// Global Shared Data Pointer (passed in from core)
GlobStruct*						g_pGlob							= NULL;

bsp_header header;
bsp_model1* model_list1;
bsp_model2* model_list2;
rVEC3D* vertex_list;
bsp_plane* plane_list;
bsp_leaf1* leaf_list1;
bsp_leaf2* leaf_list2;
bsp_node1* node_list1;
bsp_node2* node_list2;
bsp_clipnode* clipnode_list;
bsp_texinfo1 *texinfo_list1;
bsp_texinfo* texinfo_list;
bsp_face* face_list;
unsigned short *leafface_list;
unsigned short *leafbrush_list;
int *surfedge_list;
bsp_edge* edge_list;
bsp_brush* brush_list;
bsp_brushside*  brushside_list;
bsp_area* area_list;
bsp_areaportal* areaportal_list;
byte* visdata_list;
byte* lightdata_list;
char* entdata_list;
bsp_vis* vis_list;
unsigned short* marksurfaces_list;
miptex_t* textures;
byte* texturedata;
struct _QMAP QMAP;


int nummodels, visdatasize, lightdatasize, entdatasize, numleafs, numplanes, numvertices, numnodes, numclipnodes, numtexinfo, numfaces,
	numedges, numleaffaces, numleafbrushes, numsurfedges, numbrushes, numbrushsides, numareas, numareaportals, nummarksurfaces;

vector<int_vec> leaf_visibility;

// Internal Data Globals
DWORD						g_dwCameraCullMode;
bool						g_bHardwareMultitexturingOn = true;

enum eCollisionType
{
	OBJECT_COLLISION,
	CAMERA_COLLISION,
};

struct sCollision
{
	D3DXVECTOR3		vecOldPosition;
	D3DXVECTOR3		vecNewPosition;
	D3DXVECTOR3		vecRadius;
	float			fRadius;
	eCollisionType	eType;
	int				iID;

	// mike - 240604
	float			fThreshold;
	bool			bStart;
	D3DXVECTOR3		vecResp;
	bool			bRespImpact;
};

sCollision gCollision       [ 25 ];
bool	   gCollisionSwitch [ 25 ];
bool	   bActivate = true;

//////////////////////////////////////////////////////////////////////////
// function pointer types ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef IDirect3DDevice9*   ( *RetD3DFunctionPointerPFN  ) ( ... );
typedef void                ( *CAMERA_Int1Float3PFN ) ( int, float, float, float );
typedef void                ( *CAMERA_Int1Float1PFN ) ( int, float );
typedef float               ( *CAMERA_GetFloatPFN   ) ( int );

HINSTANCE					g_Setup;
RetD3DFunctionPointerPFN	g_Setup_GetDirect3DDevice;
HINSTANCE					g_Camera;
HINSTANCE					g_Object;
CAMERA_Int1Float3PFN		g_Camera_Position;
CAMERA_GetFloatPFN			g_Camera_GetXPosition;
CAMERA_GetFloatPFN			g_Camera_GetYPosition;
CAMERA_GetFloatPFN			g_Camera_GetZPosition;
CAMERA_Int1Float3PFN		g_Object_Position;
CAMERA_GetFloatPFN			g_Object_GetXPosition;
CAMERA_GetFloatPFN			g_Object_GetYPosition;
CAMERA_GetFloatPFN			g_Object_GetZPosition;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CalcNormal ( D3DXVECTOR3 v [ 3 ], D3DXVECTOR3* out );
void CalcNormal2 ( D3DXVECTOR3* v, int v_count, D3DXVECTOR3* out );

LPDIRECT3DDEVICE9			m_pD3D;
int							gCurrentCamera = 1;

D3DXVECTOR3 ConvertVectorFromQ3 ( D3DXVECTOR3 src );
D3DXVECTOR3 ConvertVectorFromQ3 ( float src [ 3 ] );
D3DXVECTOR3 ConvertVectorFromQ3 ( int src [ 3 ] );
D3DXVECTOR3 ConvertVectorFromQ3 ( short src [ 3 ] );
D3DXVECTOR3 ConvertVectorToQ3 ( D3DXVECTOR3 src );
float       ConvertVectorToQ3 ( float src );

void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject )
{
	g_Setup = hSetup;
	g_Camera = hCamera;
	g_Object = hObject;
	g_Setup_GetDirect3DDevice		= ( RetD3DFunctionPointerPFN )  GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	g_Camera_Position               = ( CAMERA_Int1Float3PFN )		GetProcAddress ( hCamera, "?Position@@YAXHMMM@Z" );
	g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetXPosition@@YAMH@Z" );
	g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetYPosition@@YAMH@Z" );
	g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetZPosition@@YAMH@Z" );
	g_Object_Position				= ( CAMERA_Int1Float3PFN )		GetProcAddress ( hObject, "?Position@@YAXHMMM@Z" );
	g_Object_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetXPosition@@YAMH@Z" );
	g_Object_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetYPosition@@YAMH@Z" );
	g_Object_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetZPosition@@YAMH@Z" );
	
	m_pD3D = g_Setup_GetDirect3DDevice ( );

	// DBPro uses camera zero
	gCurrentCamera = 0;

	for ( int i = 0; i < 25; i++ )
	{
		gCollision [ i ].fThreshold = 0.003f;
		gCollision [ i ].bStart = true;
		gCollision [ i ].bRespImpact = false;
	}
}

extern files_found Q3A_Resources;
extern files_found Q2_Resources;

void Destructor ( void )
{
	// MIKE FIX 190303 - check data is valid
	if ( QMAP.v_count <= 0 )
		return;

	QMAP.Release ( );

	Q3A_Resources.Release ( );
	Q2_Resources.Release ( );

	files_cache.Release ( );
	Textures.Reset ( );

	FreeColOpt();	
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
		Constructor ( g_Setup, NULL, g_Camera, g_Object );
	}
}

void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
}

void SetHardwareMultiTexturingOn ( void )
{
	g_bHardwareMultitexturingOn = true;
}

void SetHardwareMultiTexturingOff ( void )
{
	g_bHardwareMultitexturingOn = false;
}



void SetOldPos ( int iTemp )
{
	if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Camera_GetYPosition ( gCollision [ iTemp ].iID ),
																g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
	else
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Object_GetYPosition ( gCollision [ iTemp ].iID ),
																g_Object_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
}

void SetupCameraCollision ( int iID, int iEntity, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType      = CAMERA_COLLISION;
	gCollision [ iID ].fRadius    = fRadius;
	gCollision [ iID ].iID        = iEntity;
	//gCollision [ iID ].fThreshold = 0.003f;

	gCollisionSwitch [ iID ] = true;
	SetOldPos( iID );
}

void SetupObjectCollision ( int iID, int iEntity, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType      = OBJECT_COLLISION;
	gCollision [ iID ].fRadius    = fRadius;
	gCollision [ iID ].iID        = iEntity;
	//gCollision [ iID ].fThreshold = 0.003f;

	gCollisionSwitch [ iID ] = true;
	SetOldPos( iID );
}

void SetCameraCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = CAMERA_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;
	//gCollision [ iID ].fThreshold = 0.003f;

	gCollision [ iID ].vecRadius = D3DXVECTOR3 ( fX, fY, fZ );

	gCollisionSwitch [ iID ] = true;
	SetOldPos( iID );
}

void SetObjectCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = OBJECT_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;

	gCollision [ iID ].vecRadius = D3DXVECTOR3 ( fX, fY, fZ );
	//gCollision [ iID ].fThreshold = 0.003f;

	gCollisionSwitch [ iID ] = true;
	SetOldPos( iID );
}

void SetupCullingCamera ( int iID )
{
	gCurrentCamera=iID;
}

void SetupCollisionOff ( int iID )
{
	gCollisionSwitch [ iID ] = false;
}

extern float g_fCollisionThreshold;

void SetCollisionThreshhold ( int iID, float fSensitivity )
{
	// stop that slow sliding gravity decrement - if Ymod less than this senitivity value!



	//if ( fSensitivity < 0.003f )
	//	fSensitivity = 0.003f;

	if ( iID < 25 )
		gCollision [ iID ].fThreshold = fSensitivity;

	//g_fCollisionThreshold = fSensitivity;
}

void Start ( void )
{
	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		if ( gCollisionSwitch [ iTemp ] )
		{
			SetOldPos( iTemp );
		}
	}
}

void End ( void )
{
	// mike - 240604 - return if no data to use
	if ( QMAP.v_count <= 0 )
		return;

	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		if ( gCollisionSwitch [ iTemp ] )
		{
			if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
			{
				gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																		g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																		g_Camera_GetYPosition ( gCollision [ iTemp ].iID ),
																		g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
																  );

				if ( gCollision [ iTemp ].bStart == false )
				{
					gCollision [ iTemp ].vecOldPosition = gCollision [ iTemp ].vecNewPosition;
					gCollision [ iTemp ].bStart = true;
				}
			}
			else
			{
				gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																		g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																		g_Object_GetYPosition ( gCollision [ iTemp ].iID ),
																		g_Object_GetZPosition ( gCollision [ iTemp ].iID )
																  );

				if ( gCollision [ iTemp ].bStart )
				{
					gCollision [ iTemp ].vecOldPosition = gCollision [ iTemp ].vecNewPosition;
					gCollision [ iTemp ].bStart = false;
				}
			}

			// mike - 240604
			D3DXVECTOR3 vecLatestPos;
			vecLatestPos = gCollision [ iTemp ].vecNewPosition;
			BOOL bClipped = FALSE;

			float fSide = gCollision [ iTemp ].fRadius;
		
			if ( gCollision [ iTemp ].fRadius == -1.0f )
			{
			
						bClipped =	Collision::World ( 
								gCollision [ iTemp ].vecOldPosition, 
								gCollision [ iTemp ].vecNewPosition, 
								gCollision [ iTemp ].vecRadius,
								&gCollision [ iTemp ].vecNewPosition, 
								5,
								gCollision [ iTemp ].fThreshold
							 );
			}
			else
			{
				
					bClipped =Collision::World
									( 
										gCollision [ iTemp ].vecOldPosition, 
										gCollision [ iTemp ].vecNewPosition, 
										D3DXVECTOR3 ( gCollision [ iTemp ].fRadius, gCollision [ iTemp ].fRadius, gCollision [ iTemp ].fRadius ),
										&gCollision [ iTemp ].vecNewPosition, 
										5,
										gCollision [ iTemp ].fThreshold
							 	   );	
			}
			
			// mike - 240604 - only update if moved
			if ( bClipped )
			{
				if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
				{
					g_Camera_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
				else
				{
					g_Object_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
			}

			// mike - 240604
			gCollision [ iTemp ].vecResp = gCollision [ iTemp ].vecNewPosition - vecLatestPos;

			if ( bClipped )
				gCollision [ iTemp ].bRespImpact = true;
		}
	}
}

bool Load ( char* szFilename, char* szMap )
{
	byte* data;
	int   length;

	if ( strnicmp ( szFilename + strlen ( szFilename ) - 4, ".pak", 4 ) == 0 )
	{
		if ( !file_loader::Load::From_PAK ( szFilename, szMap, &data, &length ) )
			return FALSE;

		files_cache.AddFile ( szMap, szMap, szFilename );
	}
	else
	{
		if ( !file_loader::Load::Direct ( szMap, &data, &length ) )
		return false;
	}
	
	LPSTR path = NULL;
	SolveFullName ( szFilename, NULL, &path );

	files_found ff;
	
	////////////////
	file_loader::Find::Files ( "*.tga", path, &ff );
	
	for ( int x = 0; x < ff.num_files; x++ )
		file_loader::Q2::Add_WAD ( ff.files [ x ].fullname );

	ff.Release ( );
	////////////////

	////////////////
	file_loader::Find::Files ( "*.wad", path, &ff );

	for ( x = 0; x < ff.num_files; x++ )
		file_loader::Q2::Add_WAD ( ff.files [ x ].fullname );

	ff.Release ( );
	////////////////
	
	////////////////
	file_loader::Find::Files ( "*.pak", path, &ff );

	for ( x = 0; x < ff.num_files; x++ )
		file_loader::Q2::Add_WAD ( ff.files [ x ].fullname );

	ff.Release ( );
	////////////////

	if( SetupWorld ( data )==false)
		return false;

	return true;
}

void DeleteEx ( void )
{
	// MIKE FIX 190303 - delete BSP data
	Destructor ( );
}

int ReadLump12 ( byte* data, int lump, void** dest, int size )
{
	int length,
		ofs;

	length = header.lumps [ lump ].filelen;
	ofs    = header.lumps [ lump ].fileofs;

	*dest = new char [ length ];

	memcpy ( *dest, data + ofs, length );

	return length / size;
}

bool SetupWorld ( byte* data )
{
	if ( data [ 0 ] == 'I' && data [ 1 ] == 'B' && data [ 2 ] == 'S' && data [ 3 ] == 'P' )
	{
		bsp_header2 header2;
		memcpy ( &header2, data, sizeof ( header2 ) );

		if ( header2.version == 38 )
		{
			// quake 2 map
			memcpy ( &header, data + 4, sizeof ( header ) );

			nummodels      = ReadLump12 ( data, LUMP_MODELS2,      ( void** ) &model_list2,     sizeof ( bsp_model2 ) );
			numvertices    = ReadLump12 ( data, LUMP_VERTEXES2,    ( void** ) &vertex_list,     sizeof ( rVEC3D ) );
			numplanes      = ReadLump12 ( data, LUMP_PLANES2,      ( void** ) &plane_list,      sizeof ( bsp_plane ) );
			numleafs       = ReadLump12 ( data, LUMP_LEAFS2,       ( void** ) &leaf_list2,      sizeof ( bsp_leaf2 ) );
			numnodes       = ReadLump12 ( data, LUMP_NODES2,       ( void** ) &node_list2,      sizeof ( bsp_node2 ) );
			numtexinfo     = ReadLump12 ( data, LUMP_TEXINFO2,     ( void** ) &texinfo_list,    sizeof ( bsp_texinfo ) );
			numfaces       = ReadLump12 ( data, LUMP_FACES2,       ( void** ) &face_list,       sizeof ( bsp_face ) );
			numleaffaces   = ReadLump12 ( data, LUMP_LEAFFACES2,   ( void** ) &leafface_list,   sizeof ( leafface_list  [ 0 ] ) );
			numleafbrushes = ReadLump12 ( data, LUMP_LEAFBRUSHES2, ( void** ) &leafbrush_list,  sizeof ( leafbrush_list [ 0 ] ) );
			numsurfedges   = ReadLump12 ( data, LUMP_SURFEDGES2,   ( void** ) &surfedge_list,   sizeof ( surfedge_list [ 0 ] ) );
			numedges       = ReadLump12 ( data, LUMP_EDGES2,       ( void** ) &edge_list,       sizeof ( bsp_edge ) );
			numbrushes     = ReadLump12 ( data, LUMP_BRUSHES2,     ( void** ) &brush_list,      sizeof ( bsp_brush ) );
			numbrushsides  = ReadLump12 ( data, LUMP_BRUSHSIDES2,  ( void** ) &brushside_list,  sizeof ( bsp_brushside ) );
			numareas       = ReadLump12 ( data, LUMP_AREAS2,       ( void** ) &area_list,       sizeof ( bsp_area ) );
			numareaportals = ReadLump12 ( data, LUMP_AREAPORTALS2, ( void** ) &areaportal_list, sizeof ( bsp_areaportal ) );
       		visdatasize    = ReadLump12 ( data, LUMP_VISIBILITY2,  ( void** ) &visdata_list,    1 );
			lightdatasize  = ReadLump12 ( data, LUMP_LIGHTING2,    ( void** ) &lightdata_list,  1 );
			entdatasize    = ReadLump12 ( data, LUMP_ENTITIES2,    ( void** ) &entdata_list,    1 );
        
			vis_list = ( bsp_vis* ) visdata_list;
		}
		else
		{
			// failed
			return false;
		}
	}
	else
	{
		// quake 1 / half life map

		bsp_header1 header1;

		memcpy ( &header1, data, sizeof ( header1 ) );

		// quake 1 is 29 and half life is 30, turns out that half life is based on a modified version
		// of the quake 1 engine, I always thought it was the same as quake 2 until I tried to load in a
		// quake 2 level and it failed, read up on the internet and found out the differences
		if ( header1.version == 29 || header1.version == 30 )
		{
			memcpy ( &header, data, sizeof ( header ) );

			nummodels      = ReadLump12 ( data, LUMP_MODELS1,       ( void** ) &model_list1,	   sizeof ( bsp_model1 ) );
			numfaces       = ReadLump12 ( data, LUMP_FACES1,        ( void** ) &face_list,		   sizeof ( bsp_face ) );
			numplanes      = ReadLump12 ( data, LUMP_PLANES1,       ( void** ) &plane_list,		   sizeof ( bsp_plane ) );
			numedges       = ReadLump12 ( data, LUMP_EDGES1,        ( void** ) &edge_list,		   sizeof ( bsp_edge ) );
			numsurfedges   = ReadLump12 ( data, LUMP_SURFEDGES1,    ( void** ) &surfedge_list,     sizeof ( surfedge_list [ 0 ] ) );
			numvertices    = ReadLump12 ( data, LUMP_VERTEXES1,     ( void** ) &vertex_list,       sizeof ( rVEC3D ) );
			lightdatasize  = ReadLump12 ( data, LUMP_LIGHTING1,     ( void** ) &lightdata_list,    1 );
			numnodes       = ReadLump12 ( data, LUMP_NODES1,        ( void** ) &node_list1,		   sizeof ( bsp_node1 ) );
			numclipnodes   = ReadLump12 ( data, LUMP_CLIPNODES1,    ( void** ) &clipnode_list,	   sizeof ( bsp_clipnode ) );
			numleafs       = ReadLump12 ( data, LUMP_LEAFS1,        ( void** ) &leaf_list1,		   sizeof ( bsp_leaf1 ) );
			nummarksurfaces= ReadLump12 ( data, LUMP_MARKSURFACES1, ( void** ) &marksurfaces_list, sizeof ( unsigned short ) );
			visdatasize    = ReadLump12 ( data, LUMP_VISIBILITY1,   ( void** ) &visdata_list,      1 );
			entdatasize    = ReadLump12 ( data, LUMP_ENTITIES1,     ( void** ) &entdata_list,      1 );

			vis_list = ( bsp_vis* ) visdata_list;

			numtexinfo    = ReadLump12 ( data, LUMP_TEXINFO1,  ( void** ) &texinfo_list1, sizeof ( bsp_texinfo1 ) );
			int tex_count = ReadLump12 ( data, LUMP_TEXTURES1, ( void** ) &texturedata,   1 );

			mip_header_t* m = ( mip_header_t* ) texturedata;
			tex_count       = m->count;
			textures        = new miptex_t [ tex_count ];

			for ( int i = 0; i < m->count; i++ )
			{
				miptex_t* t = ( miptex_t* ) ( texturedata + m->offsets [ i ] );

				memcpy ( &textures [ i ], t, sizeof ( miptex_t ) );

				textures [ i ].offsets [ 0 ] += m->offsets [ i ];
				textures [ i ].offsets [ 1 ] += m->offsets [ i ];
				textures [ i ].offsets [ 2 ] += m->offsets [ i ];
				textures [ i ].offsets [ 3 ] += m->offsets [ i ];
			}
		}
		else
		{
			// failed
		}
	}

	if ( header.version == 29 || header.version == 30 )
	{
		QMAP.type = QUAKE1;
	}
	else if ( header.version == 38 )
	{
		QMAP.type = QUAKE2;
	}
	else
	{
		// unknown map type
		return false;
	}

	leaf_visibility.clear ( );

	// decompress quake 1 visdata
	if ( QMAP.type == QUAKE1 )
	{
		for ( int i = 0; i < numleafs; i++ )
		{
			int_vec		visible;
			bsp_leaf1&	leaf	= leaf_list1 [ i ];
			int			v		= leaf.visofs;
			const byte* vis     = visdata_list;

			for ( int c = 1; c < model_list1 [ 0 ].visleafs; v++ )
			{
		        if ( vis [ v ] == 0 )
				{
					v++;
					c += ( vis [ v ] << 3 );
				}
				else
				{
					for ( byte bit = 1; bit; bit <<= 1, c++ )
					{
						if ( vis [ v ] & bit )
							visible.push_back ( c );
					}
				}
			}

			leaf_visibility.push_back ( visible );
		}
	}
	else if ( QMAP.type == QUAKE2 )
	{
		// decompress quake 2 visdata

		for ( int i = 0; i < numleafs; i++ )
		{
			int_vec visible;
			short	cluster = leaf_list2 [ i ].cluster;

			if ( cluster != -1 )
			{
				int			v   = vis_list->bitofs [ cluster ] [ 0 ];
				const byte* vis = visdata_list;

				for ( int c = 0; c < vis_list->numclusters; v++ )
				{
					if ( vis [ v ] == 0 )
					{
						v++;
						c += ( vis [ v ] << 3 );
					}
					else
					{
						for ( byte bit = 1; bit; bit <<= 1, c++ )
						{
							if ( vis [ v ] & bit )
							{
								for ( int t = 0; t < numleafs; t++ )
								{
									if ( leaf_list2 [ t ].cluster == c )
										visible.push_back ( t );
								}
							}
						}
					}
				}
			}

			leaf_visibility.push_back ( visible );
		}
	}

	// setup all of the textures
	for ( int i = 0; i < numfaces; i++ )
	{
		bsp_face f = face_list [ i ];

		face_t12 face;

		// get plane info
		memcpy ( &face.plane, plane_list + f.planenum, sizeof ( bsp_plane ) );
		face.side = f.side;

		int x = -1;
		bsp_texinfo* ti;
		
		if ( QMAP.type == QUAKE1 )
		{
			bsp_texinfo1* ti1 = texinfo_list1 + f.texinfo;
			face.flags        = ti1->flags;
			
			// sky
			if ( strncmp ( textures [ ti1->miptex ].name, "sky", 3 ) == 0 )
				face.flags |= ( QSURF_DRAWSKY | QSURF_DRAWTILED );

			// turbulent
			if ( strncmp ( textures [ ti1->miptex ].name, "*", 1 ) == 0 )
				face.flags |= ( QSURF_DRAWTURB | QSURF_DRAWTILED );

			int offset  = textures [ ti1->miptex ].offsets [ 0 ];
			int offset2 = textures [ ti1->miptex ].offsets [ 1 ];
			
			if ( header.version == 29 )
			{
				// quake 1
				
				UINT w = textures [ ti1->miptex ].width;
				UINT h = textures [ ti1->miptex ].height;

				byte* data = texturedata + offset;

				x = Textures.Load_By_Data_Q2 ( textures [ ti1->miptex ].name, data, w, h, 8, TRUE );
				
				if ( x == -1 )
				{
					x = Textures.Load_By_FileName_Q2 ( textures [ ti1->miptex ].name );
				}
			}

			if ( header.version == 30 )
			{
				// half life

				x = Textures.Load_By_FileName_Q2 ( textures [ ti1->miptex ].name );
				
				if ( x == -1 )
				{
					if ( offset2 != offset )
					{
						UINT w = textures [ ti1->miptex ].width;
						UINT h = textures [ ti1->miptex ].height;

						byte* data = texturedata + offset;

						x = Textures.Load_By_Data_Q2 ( textures [ ti1->miptex ].name, data, w, h, 8, TRUE );
					}
				}
			}
		
			ti = ( bsp_texinfo* ) ti1;
		}
		else if ( QMAP.type == QUAKE2 )
		{
			ti         = texinfo_list + f.texinfo;
			face.flags = ti->flags;
			x          = Textures.Load_By_FileName_Q2 ( ti->texture );
		}
			
		face.texture = x;

		// compute s and t extents
		float	min [ 2 ],
				max [ 2 ];

		min [ 0 ] = min [ 1 ] =  100000;
		max [ 0 ] = max [ 1 ] = -100000;

		for ( int c = 0; c < f.numedges; c++ )
		{
	        rVEC3D *v;

			int eidx = surfedge_list [ f.firstedge + c ];

			if ( eidx >= 0 )
			{
	            v = vertex_list + edge_list [ eidx ].v [ 0 ];
			}
			else
			{
	            v = vertex_list + edge_list [ -eidx ].v [ 1 ];
			}

			// compute extents
			for ( int x = 0; x < 2; x++ )
			{
	            float d = v->x * ti->vecs [ x ] [ 0 ] +
						  v->y * ti->vecs [ x ] [ 1 ] +
						  v->z * ti->vecs [ x ] [ 2 ] +
						  ti->vecs [ x ] [ 3 ];

				if ( d < min [ x ] )
					min [ x ] = d;

				if ( d > max [ x ] )
					max [ x ] = d;
			}
		}

		int lw, lh;

		face.lightmap = -1;

		if ( face.flags == 0 )
		{
	        // compute lightmap size
			int size [ 2 ];

			for ( c = 0; c < 2; c++ )
			{
	            float tmin = ( float ) floor ( min [ c ] / 16.0f );
				float tmax = ( float ) ceil  ( max [ c ] / 16.0f );

				size [ c ] = ( int ) ( tmax - tmin );
			}

			lw = size [ 0 ] + 1;
			lh = size [ 1 ] + 1;

			int lsz = lw * lh * 3;

			if ( f.styles [ 0 ] != -1 )
			{
				/*
				if ( f->styles [ c ] == -1 )
					break;

                face->styles [ c ] = f->styles [ c ];

                rgb_image_t r;
                r.width  = lw;
                r.height = lh;
                r.data   = new byte [ lsz ];

                memcpy ( r.data, bsp->lightmaps + f->lightofs + ( lsz * c ), lsz );

				// mj add - disables light maps when commented -
                face->lightmaps [ c ] = new Texture ( r.data, r.width, r.height, GL_RGB, Texture::nearest_mipmap_nearest );
				*/

				byte* bits = lightdata_list + f.lightofs + lsz * 0;

				face.lightmap = Textures.Load_LightMap_By_Data_Q2 ( bits, lw, lh, 24 );
				
				face.styles [ 0 ] = f.styles [ 0 ];
			}

		}

		float is = 0,
			  it = 0;

		if ( face.texture != -1 )
		{
			is = 1.0f / ( float ) Textures.Tex [ face.texture ].original_width;
			it = 1.0f / ( float ) Textures.Tex [ face.texture ].original_height;
		}

		face.first = QMAP.v_count;
		face.count = f.numedges;

		for ( c = 0; c < f.numedges; c++ )
		{
	        float v [ 7 ];

			int eidx = *( surfedge_list + f.firstedge + c );

			if ( eidx < 0 )
			{
	            eidx        = -eidx;
				bsp_edge* e = edge_list + eidx;

				v [ 0 ] = vertex_list [ e->v [ 1 ] ].x;
				v [ 1 ] = vertex_list [ e->v [ 1 ] ].y;
				v [ 2 ] = vertex_list [ e->v [ 1 ] ].z;
			}
			else
			{
				bsp_edge* e = edge_list + eidx;

	            v [ 0 ] = vertex_list [ e->v [ 0 ] ].x;
	            v [ 1 ] = vertex_list [ e->v [ 0 ] ].y;
				v [ 2 ] = vertex_list [ e->v [ 0 ] ].z;
	        }

			D3DXVECTOR3 vertex = v;

			float s = D3DXVec3Dot ( &vertex, &( D3DXVECTOR3 ) ti->vecs [ 0 ] ) + ti->vecs [ 0 ] [ 3 ];
			float t = D3DXVec3Dot ( &vertex, &( D3DXVECTOR3 ) ti->vecs [ 1 ] ) + ti->vecs [ 1 ] [ 3 ];

			v [ 3 ] = s * is;
			v [ 4 ] = t * it;

			if ( face.flags == 0 )
			{
	            // compute lightmap coords
				float mid_poly_s = ( min [ 0 ] + max [ 0 ] ) / 2.0f;
				float mid_poly_t = ( min [ 1 ] + max [ 1 ] ) / 2.0f;
				float mid_tex_s  = ( float ) lw / 2.0f;
				float mid_tex_t  = ( float ) lh / 2.0f;
				float ls         = mid_tex_s + ( s - mid_poly_s ) / 16.0f;
				float lt         = mid_tex_t + ( t - mid_poly_t ) / 16.0f;

				ls /= ( float ) lw;
				lt /= ( float ) lh;

				v [ 5 ] = ls;
				v [ 6 ] = lt;
			}

			QMAP.v [ QMAP.v_count ].v     = ConvertVectorFromQ3 ( v );
			QMAP.v [ QMAP.v_count ].color = D3DCOLOR_RGBA ( 255, 255, 255, 255 );
		
			if ( face.texture != -1 )
			{
				QMAP.v [ QMAP.v_count ].tu = v [ 3 ];
				QMAP.v [ QMAP.v_count ].tv = v [ 4 ];
			}

			if ( face.lightmap != -1 )
			{
				QMAP.v [ QMAP.v_count ].lu = v [ 5 ];
				QMAP.v [ QMAP.v_count ].lv = v [ 6 ];
			}

			QMAP.v_count++;
		}

		D3DXVECTOR3 ve [ 32 ];

		for ( int y = 0; y < face.count; y++ )
			ve [ y ] = QMAP.v [ face.first + y ].v;

		D3DXVECTOR3 n;

		CalcNormal2 ( ve, face.count, &n );
		D3DXVec3Normalize ( &n, &n );

		for ( y = 0; y < face.count; y++ )
			QMAP.v [ face.first + y ].n = n;			

		float d = face.plane.dist;
		
		if ( face.side )
		{
	       //face.plane = plane ( -n, d );
		}
		else
		{
	       //face.plane = plane_t ( n, -d );
		}

		// add face
		QMAP.faces [ i ] = face;
	}


	SAFE_DELETE_ARRAY ( data );

	// world correct
	return true;
}



D3DXVECTOR3 ConvertVectorFromQ3 ( D3DXVECTOR3 src )
{
	src *= 0.029f;

	swap ( src.y, src.z );

	return src;
}

D3DXVECTOR3 ConvertVectorFromQ3 ( float src [ 3 ] )
{
	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorFromQ3 ( int src [ 3 ] )
{
	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( (float) src [ 0 ], (float) src [ 1 ], (float) src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorFromQ3 ( short src [ 3 ] )
{
	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorToQ3 ( D3DXVECTOR3 src )
{
	src /= 0.029f;

	swap ( src.y, src.z );

	return src;
}

float ConvertVectorToQ3 ( float src )
{
	return src / 0.029f;
}

void swap ( float* x, float* y )
{
	float temp = *x;
	
	*x = *y;
	*y = temp;
}

void swap ( double* x, double* y )
{
	double temp = *x;
	
	*x = *y;
	*y = temp;
}

void swap ( int* x, int* y )
{
	int temp = *x;
	
	*x = *y;
	*y = temp;
}

void CalcNormal ( D3DXVECTOR3 v [ 3 ], D3DXVECTOR3* out )
{
	D3DXVECTOR3 v1, v2;

	D3DXVec3Subtract ( &v1, &v [ 1 ], &v [ 0 ] );
	D3DXVec3Subtract ( &v2, &v [ 2 ], &v [ 0 ] );
	
	D3DXVec3Cross ( out, &v1, &v2 );
}

void CalcNormal2 ( D3DXVECTOR3* v, int v_count, D3DXVECTOR3* out )
{
	float len = 0;

	D3DXVECTOR3 v2 [ 3 ];
	
	v2 [ 0 ] = v [ 0 ];

	for ( int x = 0; x < v_count - 2; x++ )
	{
		v2 [ 1 ] = v [ x + 1 ];
		v2 [ 2 ] = v [ x + 2 ];

		D3DXVECTOR3 n;

		CalcNormal ( v2, &n );

		float l = D3DXVec3Length ( &n );
		
		if ( l > len )
		{
			len  = l;
			*out = n;
		}
	}
}


D3DXPLANE frustum_plane;

bool frustum_cull ( D3DXVECTOR3 min, D3DXVECTOR3 max )
{
    D3DXPLANE p = frustum_plane;

    if ( p.a * min.x + p.b * min.y + p.c * min.z + p.d > 0 ) return false;
    if ( p.a * max.x + p.b * min.y + p.c * min.z + p.d > 0 ) return false;
    if ( p.a * min.x + p.b * max.y + p.c * min.z + p.d > 0 ) return false;
    if ( p.a * max.x + p.b * max.y + p.c * min.z + p.d > 0 ) return false;
    if ( p.a * min.x + p.b * min.y + p.c * max.z + p.d > 0 ) return false;
    if ( p.a * max.x + p.b * min.y + p.c * max.z + p.d > 0 ) return false;
    if ( p.a * min.x + p.b * max.y + p.c * max.z + p.d > 0 ) return false;
    if ( p.a * max.x + p.b * max.y + p.c * max.z + p.d > 0 ) return false;

    return true;
}

void frustum_setup ( )
{
	D3DMATRIX m;
	m_pD3D->GetTransform ( D3DTS_VIEW, &m );

    // near plane
	frustum_plane.a = m._14 + m._13;
    frustum_plane.b = m._24 + m._23;
    frustum_plane.c = m._34 + m._33;
    frustum_plane.d = m._44 + m._43;
}


// 0 - 2 are axial planes
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2

// 3 - 5 are non - axial planes snapped to the nearest
#define PLANE_ANYX 3
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

int find_leaf ( D3DXVECTOR3 pos )
{
	int i;

	if ( QMAP.type == QUAKE1 )
		i = model_list1 [ 0 ].bsp_node;

	if ( QMAP.type == QUAKE2 )
		i = model_list2 [ 0 ].headnode;

    while ( i >= 0 )
	{
		bsp_node1* n1;
		bsp_node2* n2;

		if ( QMAP.type == QUAKE1 )
			n1 = &node_list1 [ i ];

		if ( QMAP.type == QUAKE2 )
			n2 = &node_list2 [ i ];
        
		bsp_plane* p;

		if ( QMAP.type == QUAKE1 )
			p = &plane_list [ n1->planenum ];

		if ( QMAP.type == QUAKE2)
			p = &plane_list [ n2->planenum ];

        float d;

        if ( p->type <= PLANE_Z )
		{
			// easier for axial planes
            d = pos [ p->type ] - p->dist;
        }
        else
		{
            d =   p->norm [ 0 ] * pos.x
                + p->norm [ 1 ] * pos.y
                + p->norm [ 2 ] * pos.z
                - p->dist;
        }

        if ( d >= 0 )
		{
			// in front or on the plane
			if ( QMAP.type == QUAKE1 )
				i = n1->children [ 0 ];

			if ( QMAP.type == QUAKE2 )
				i = n2->children [ 0 ];
        }
        else
		{
			// behind the plane
            if ( QMAP.type == QUAKE1 )
				i = n1->children [ 1 ];

			if ( QMAP.type == QUAKE2 )
				i = n2->children [ 1 ];
        }
    }

    return -( i + 1 );
}

int find_node ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere )
{
	int i;

	if ( QMAP.type == QUAKE1 )
		i = model_list1 [ 0 ].bsp_node;

	if ( QMAP.type == QUAKE2 )
		i = model_list2 [ 0 ].headnode;

    while ( i >= 0 )
	{
		bsp_node1* n1;
		bsp_node2* n2;

		if ( QMAP.type == QUAKE1 )
			n1 = &node_list1 [ i ];

		if ( QMAP.type == QUAKE2 )
			n2 = &node_list2 [ i ];
        
		bsp_plane* p;

		if ( QMAP.type == QUAKE1 )
			p = &plane_list [ n1->planenum ];

		if ( QMAP.type == QUAKE2 )
			p = &plane_list [ n2->planenum ];

        float d1, d2;

        if ( p->type <= PLANE_Z )
		{
			// easier for axial planes
            d1 = o_pos [ p->type ] - p->dist;
			d2 = n_pos [ p->type ] - p->dist;
        }
        else
		{
            d1 = p->norm [ 0 ] * o_pos.x + p->norm [ 1 ] * o_pos.y + p->norm [ 2 ] * o_pos.z -p->dist;
			d2 = p->norm [ 0 ] * n_pos.x + p->norm [ 1 ] * n_pos.y + p->norm [ 2 ] * n_pos.z -p->dist;
        }

        if ( d1 > BoundingSphere && d2 > BoundingSphere )
		{
			// in front or on the plane
			if ( QMAP.type == QUAKE1 )
				i = n1->children [ 0 ];

			if ( QMAP.type == QUAKE2 )
				i = n2->children [ 0 ];
        }
        else if ( d1 < -BoundingSphere && d2 < -BoundingSphere )
		{
			// behind the plane
            if ( QMAP.type == QUAKE1 )
				i = n1->children [ 1 ];

			if ( QMAP.type == QUAKE2 )
				i = n2->children [ 1 ];
        }
		else
		{
			break;
		}
    }

    return i;
}

void BeginRenderStates ( void )
{
	// set material
	D3DMATERIAL9 mtrl;
	memset ( &mtrl, 0, sizeof ( mtrl ) );
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;
	m_pD3D->SetMaterial ( &mtrl );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

	// wireframe
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

	// lighting
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );
	//m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );

	// fogenable
	//m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	// mike - 240604
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );

	// transparency
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		FALSE );
	m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,			D3DCMP_ALWAYS );

	// ghost
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

	// default material render states
	m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					TRUE );
	m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1 );
	m_pD3D->SetRenderState ( D3DRS_SPECULARMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_EMISSIVEMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE,				FALSE );

	// texture coordinate data
	m_pD3D->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pD3D->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
}

void EndRenderStates ( void )
{
	// filter and mipmapping
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// alpha blending
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,			FALSE );
	m_pD3D->SetRenderState ( D3DRS_SRCBLEND,					D3DBLEND_ONE );
	m_pD3D->SetRenderState ( D3DRS_DESTBLEND,					D3DBLEND_ZERO );
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,				FALSE );

	// lighting
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE,				TRUE );

	// render misc
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,						TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZFUNC,						D3DCMP_LESSEQUAL );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,					D3DCULL_CCW );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,		0 );

	// mike - 240604
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
}

void Update ( void )
{
	D3DXMATRIX	matTranslation,
				matScale,
				matObject;

	// MIKE FIX 190303 - make sure we have some valid data
	if ( QMAP.v_count <= 0 )
		return;

	// World Transform
	D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );
	D3DXMatrixScaling     ( &matScale,       1.0f, 1.0f, 1.0f );
	matObject = matScale * matTranslation;
	m_pD3D->SetTransform ( D3DTS_WORLD, &matObject );

	// Setup renderstates
	BeginRenderStates();

// LEEFIX - 171002 - For Reflection rendering the BSP culling must be reversed
//	m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE, g_dwCameraCullMode );

	/* added renderstate setup above
	// initial renderstates
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	
	// LEEFIX - 210703 - NO SPECULAR AT THIS TIME
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, FALSE );
	*/
	
	// stage 1
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE     );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	
	// filter
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	
	
	// stage 2
	m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
	m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT     );
	m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT     );
	m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	// filter
	m_pD3D->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	
	
	// stage 3
	m_pD3D->SetTextureStageState ( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pD3D->SetTextureStageState ( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	////////
	//m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	// mike - 080704 - disable alpha blending
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	////////

	frustum_setup ( );
	int rendered = 0;

	// Render direct view or reflected stencil view
	g_dwCameraCullMode = D3DCULL_CCW;
	D3DXVECTOR3 viewpoint = D3DXVECTOR3 ( g_Camera_GetXPosition ( gCurrentCamera ), g_Camera_GetYPosition ( gCurrentCamera ), g_Camera_GetZPosition ( gCurrentCamera ) );
	if(g_pGlob)
	{
		if(g_pGlob->dwStencilMode==2)
		{
			if(g_pGlob->dwRedrawPhase==1)
			{
				// View is a reflection
				viewpoint.x = g_pGlob->fReflectionPlaneX;
				viewpoint.y = g_pGlob->fReflectionPlaneY;
				viewpoint.z = g_pGlob->fReflectionPlaneZ;				
				g_dwCameraCullMode = D3DCULL_CW;
			}
		}
	}

	D3DXVECTOR3 pos  = ConvertVectorToQ3 ( viewpoint );
	int         idx = find_leaf ( pos );

	if ( ( QMAP.type == QUAKE1 ) || ( QMAP.type == QUAKE2 && leaf_list2 [ idx ].cluster >= 0 ) )
	{
		ZeroMemory ( &QMAP.mark_faces, sizeof ( BOOL ) * numfaces );

		int_vec& v = leaf_visibility [ idx ];

		// go through leaf visibility list
        for ( std::vector<int>::size_type i = 0; i < v.size ( ); i++ )
		{
			bsp_leaf1 leaf1;
			bsp_leaf2 leaf2;

			if ( QMAP.type == QUAKE1 )
				leaf1 = leaf_list1 [ v [ i ] ];

			if ( QMAP.type == QUAKE2 )
				leaf2 = leaf_list2 [ v [ i ] ];

			// discard leafs outside frustum
			if ( (
					QMAP.type == QUAKE1 && 
					!frustum_cull ( ConvertVectorFromQ3 ( leaf1.mins ), ConvertVectorFromQ3 ( leaf1.maxs ) ) ) ||
					( QMAP.type == QUAKE2 && !frustum_cull ( ConvertVectorFromQ3 ( leaf2.mins ), ConvertVectorFromQ3 ( leaf2.maxs ) ) ) 
				  )
			{
				int num_faces;

				if ( QMAP.type == QUAKE1 )
					num_faces = leaf1.nummarksurfaces;

				if ( QMAP.type == QUAKE2 )
					num_faces = leaf2.numleaffaces;

				for ( int x = 0; x < num_faces; x++ )
				{
					// don't render those already rendered
					int face_idx;

					if ( QMAP.type == QUAKE1 )
						face_idx = marksurfaces_list [ leaf1.firstmarksurface + x ];

					if ( QMAP.type == QUAKE2 )
						face_idx = leafface_list [ leaf2.firstleafface + x ];

					if ( !QMAP.mark_faces [ face_idx ] )
					{
						if ( QMAP.type == QUAKE1 )
						{
							//if ( QMAP.faces [ face_idx ].flags & ( QSURF_DRAWSKY | QSURF_UNDERWATER ) )
							//	continue;

							if ( QMAP.faces [ face_idx ].flags & ( QSURF_NODRAW | QSURF_HINT | QSURF_SKIP ) )
								continue;
						}
						
						if ( QMAP.type == QUAKE2 )
						{
							//if ( QMAP.faces [ face_idx ].flags & ( QSURF_NODRAW | QSURF_HINT | QSURF_SKIP | QSURF_SKY ) )
							//	continue;

							
						}

						// back face culling
						face_t12    f = QMAP.faces [ face_idx ];
						D3DXVECTOR3 n = D3DXVECTOR3 ( f.plane.norm [ 0 ], f.plane.norm [ 1 ], f.plane.norm [ 2 ] );

						float d = D3DXVec3Dot ( &pos, &n ) - f.plane.dist;

						if ( f.side )
						{
							if ( d > 0 )
								continue;
						}
						else
						{
							if ( d < 0 )
								continue;
						}

						// mark face as visible
						QMAP.mark_faces [ face_idx ] = TRUE;
						rendered++;
					}
				}
			}
		}
	}
	else
		memset ( &QMAP.mark_faces, 1, sizeof ( BOOL ) * numfaces );

	if ( g_bHardwareMultitexturingOn )
	{
		m_pD3D->SetVertexShader ( NULL );
		m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 );

		for ( int x = 0; x < numfaces; x++ )
		{
			if ( !QMAP.mark_faces [ x ] )
				continue;

			
			if ( QMAP.faces [ x ].texture == -1 )
			{
				m_pD3D->SetTexture ( 0, NULL );

				continue;
			}
			else
			{
				m_pD3D->SetTexture ( 0, Textures.Tex [ QMAP.faces [ x ].texture ].lpD3DTex );
			}
			
			if ( QMAP.faces [ x ].lightmap == -1 )
			{
				m_pD3D->SetTexture ( 1, NULL );

				continue;
			}
			else
			{
				m_pD3D->SetTexture ( 1, Textures.LightMap [ QMAP.faces [ x ].lightmap ].lpD3DTex );
			}

			m_pD3D->DrawPrimitiveUP ( 
										D3DPT_TRIANGLEFAN, 
										QMAP.faces [ x ].count - 2,
										( void** ) &QMAP.v [ QMAP.faces [ x ].first ],
										sizeof ( CUSTOMVERTEX )
									);
		}
	}
	else
	{
		m_pD3D->SetVertexShader ( NULL );
		m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

		struct SOFTWAREVERTEX
		{
			D3DXVECTOR3 v;
			D3DXVECTOR3 n;
			D3DCOLOR color;
			FLOAT tu, tv;
		};

		SOFTWAREVERTEX	vertex [ 1000 ];
		int				iVertexCount = 0;

		for ( int x = 0; x < numfaces; x++ )
		{
			if ( !QMAP.mark_faces [ x ] )
				continue;

			for ( int iPass = 0; iPass < 2; iPass++ )
			{
				iVertexCount = 0;

				// stage 2
				m_pD3D->SetTexture ( 1, NULL );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE   );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT     );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE     );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

				// filter
				m_pD3D->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pD3D->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pD3D->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

				if ( iPass == 0 )
				{
					int iStart = QMAP.faces [ x ].first;

					for ( int iSwap = 0; iSwap < QMAP.faces [ x ].count; iSwap++ )
					{
						vertex [ iSwap ].v     = QMAP.v [ iStart ].v;
						vertex [ iSwap ].n     = QMAP.v [ iStart ].n;
						vertex [ iSwap ].color = QMAP.v [ iStart ].color;
						vertex [ iSwap ].tu    = QMAP.v [ iStart ].tu;
						vertex [ iSwap ].tv    = QMAP.v [ iStart ].tv;

						iVertexCount++;
						iStart++;
					}
					
					m_pD3D->SetRenderState ( D3DRS_SRCBLEND,  D3DBLEND_ONE );
					m_pD3D->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ZERO );

					if ( QMAP.faces [ x ].texture == -1 )
						m_pD3D->SetTexture ( 0, NULL );
					else
						m_pD3D->SetTexture ( 0, Textures.Tex [ QMAP.faces [ x ].texture ].lpD3DTex );
				}
				
				if ( iPass == 1 )
				{
					int iStart = QMAP.faces [ x ].first;

					for ( int iSwap = 0; iSwap < QMAP.faces [ x ].count; iSwap++ )
					{
						vertex [ iSwap ].v     = QMAP.v [ iStart ].v;
						vertex [ iSwap ].n     = QMAP.v [ iStart ].n;
						vertex [ iSwap ].color = QMAP.v [ iStart ].color;
						vertex [ iSwap ].tu    = QMAP.v [ iStart ].lu;
						vertex [ iSwap ].tv    = QMAP.v [ iStart ].lv;

						iVertexCount++;
						iStart++;
					}

					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );

					
					if ( QMAP.faces [ x ].lightmap == -1 )
						m_pD3D->SetTexture ( 0, NULL );
					else
						m_pD3D->SetTexture ( 0, Textures.LightMap [ QMAP.faces [ x ].lightmap ].lpD3DTex );
				}

				m_pD3D->DrawPrimitiveUP ( 
											D3DPT_TRIANGLEFAN, 
											iVertexCount - 2,
											( void** ) &vertex,
											sizeof ( SOFTWAREVERTEX )
										);
			}
		}
	}

	// Restore renderstates
	EndRenderStates();
}

int GetCollisionResponse ( int iID )
{
	if ( gCollision [ iID ].bRespImpact )
	{
		gCollision [ iID ].bRespImpact=false;
		return 1;
	}
	else
		return 0;
}

float GetCollisionX ( int iID )
{
	return gCollision [ iID ].vecResp.x;
}

float GetCollisionY ( int iID )
{
	return gCollision [ iID ].vecResp.y;
}

float GetCollisionZ ( int iID )
{
	return gCollision [ iID ].vecResp.z;
}