#include <vector>
using namespace std;
typedef vector<int> int_vec;

//////////////////////////////////////////////////////////////////////////
// defines ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////

// quake 1
#define HEADER_LUMPS1      15
#define LUMP_ENTITIES1     0
#define LUMP_PLANES1       1
#define LUMP_TEXTURES1     2
#define LUMP_VERTEXES1     3
#define LUMP_VISIBILITY1   4
#define LUMP_NODES1        5
#define LUMP_TEXINFO1      6
#define LUMP_FACES1        7
#define LUMP_LIGHTING1     8
#define LUMP_CLIPNODES1    9
#define LUMP_LEAFS1        10
#define LUMP_MARKSURFACES1 11
#define LUMP_EDGES1        12
#define LUMP_SURFEDGES1    13
#define LUMP_MODELS1       14

// quake 2
#define HEADER_LUMPS2     19
#define LUMP_ENTITIES2    0
#define LUMP_PLANES2      1
#define LUMP_VERTEXES2    2
#define LUMP_VISIBILITY2  3
#define LUMP_NODES2       4
#define LUMP_TEXINFO2     5
#define LUMP_FACES2       6
#define LUMP_LIGHTING2    7
#define LUMP_LEAFS2       8
#define LUMP_LEAFFACES2   9
#define LUMP_LEAFBRUSHES2 10
#define LUMP_EDGES2       11
#define LUMP_SURFEDGES2   12
#define LUMP_MODELS2      13
#define LUMP_BRUSHES2     14
#define LUMP_BRUSHSIDES2  15
#define LUMP_POP2         16
#define LUMP_AREAS2       17
#define LUMP_AREAPORTALS2 18

typedef struct
{
	int	fileofs;
	int	filelen;
} bsp_lump;

struct bsp_header
{
	int		version;
	bsp_lump	lumps [ HEADER_LUMPS2 ];
};

struct bsp_header1
{
	// quake 1 header
	int version;
	bsp_lump lumps [ HEADER_LUMPS1 ];
};

struct bsp_header2
{
	// quake 2 header
	char		id [ 4 ];
	int			version;
	bsp_lump	lumps [ HEADER_LUMPS2 ];
};

typedef struct
{
	float	mins [ 3 ],
			maxs [ 3 ];
    float	origin [ 3 ];
    int		bsp_node;
    int		clip_node;
    int		clip2_node;
    int		unused;
    int		visleafs;
    int		firstface;
    int		numfaces;
} bsp_model1;

typedef struct
{
	float	mins [ 3 ],
			maxs [ 3 ];
	float	origin [ 3 ];
	int		headnode;
	int		firstface,
			numfaces;
} bsp_model2;

struct rVEC3D
{
	float x, y, z; 
};

typedef struct
{
	float   norm [ 3 ];
	float   dist;
	int     type;
} bsp_plane;

#define NUM_AMBIENTS 4

typedef struct
{
	int      contents;
    int      visofs;

    short    mins [ 3 ];
	short    maxs [ 3 ];

    unsigned short firstmarksurface;
    unsigned short nummarksurfaces;

    byte ambient_level [ NUM_AMBIENTS ];
} bsp_leaf1;

typedef struct
{
	int				contents;
	short			cluster;
	short			area;
	short			mins [ 3 ];
	short			maxs [ 3 ];
	unsigned short firstleafface, numleaffaces;
	unsigned short firstleafbrush, numleafbrushes;
} bsp_leaf2;

typedef struct
{
	int				planenum;
	short			children [ 2 ];
	short			mins [ 3 ];
	short			maxs [ 3 ];
	unsigned short	firstface;
	unsigned short	numfaces;
} bsp_node1;

typedef struct
{
	int				planenum;
	int				children [ 2 ];
	short			mins [ 3 ];
	short			maxs [ 3 ];
	unsigned short	firstface;
	unsigned short	numfaces;
} bsp_node2;

typedef struct
{
	int		planenum;
	short	children [ 2 ];
} bsp_clipnode;

typedef struct
{
    float vecs [ 2 ] [ 4 ];
    int   miptex;
    int   flags;
} bsp_texinfo1;

typedef struct
{
	float	vecs [ 2 ] [ 4 ];
	int		flags;
	int		value;
	char	texture [ 32 ];
	int		nexttexinfo;
} bsp_texinfo;

#define MAXLIGHTMAPS 4

typedef struct
{
	unsigned short	planenum;
	short			side;

	int		firstedge;
	short	numedges;
	short	texinfo;

	unsigned char	styles [ MAXLIGHTMAPS ];
	int				lightofs;
} bsp_face;

typedef struct
{
	unsigned short v [ 2 ];
} bsp_edge;

typedef struct
{
	int firstside,
		numsides,
		contents;
} bsp_brush;

typedef struct
{
	unsigned short  planenum;
	short			texinfo;
} bsp_brushside;

typedef struct
{
	int numareaportals;
	int firstareaportal;
} bsp_area;

typedef struct
{
	int portalnum;
	int otherarea;
} bsp_areaportal;

typedef struct
{
	int numclusters;
	int bitofs [ 80000 ] [ 2 ];
} bsp_vis;


#define MIPLEVELS 4

typedef struct
{
    char     name [ 16 ];
    unsigned width, height;
    unsigned offsets [ MIPLEVELS ];
} miptex_t;

struct mip_header_t
{
	int count;
	int offsets [ 1 ];
};

typedef struct
{
	byte* data;
	int width;
	int height;
	int format;
} Texture;

#define	QSURF_PLANEBACK			2
#define	QSURF_DRAWSKY			4
#define QSURF_DRAWSPRITE		8
#define QSURF_DRAWTURB			0x10
#define QSURF_DRAWTILED			0x20
#define QSURF_DRAWBACKGROUND	0x40
#define QSURF_UNDERWATER		0x80

#define QSURF_LIGHT   0x1
#define QSURF_SLICK   0x2
#define QSURF_SKY     0x4
#define QSURF_WARP    0x8
#define QSURF_TRANS33 0x10
#define QSURF_TRANS66 0x20
#define QSURF_FLOWING 0x40
#define QSURF_NODRAW  0x80
#define QSURF_HINT    0x100
#define QSURF_SKIP    0x200

#define MAX_LIGHTMAPS 4

typedef struct
{
	int flags;
	int texture;
	int lightmap;
	int styles [ MAX_LIGHTMAPS ];

	bsp_plane plane;
	int side;

	int first;
	int count;
} face_t12;

extern bsp_header header;
extern bsp_model1* model_list1;
extern bsp_model2* model_list2;
extern rVEC3D* vertex_list;
extern bsp_plane* plane_list;
extern bsp_leaf1* leaf_list1;
extern bsp_leaf2* leaf_list2;
extern bsp_node1* node_list1;
extern bsp_node2* node_list2;
extern bsp_clipnode* clipnode_list;
extern bsp_texinfo1 *texinfo_list1;
extern bsp_texinfo* texinfo_list;
extern bsp_face* face_list;
extern unsigned short *leafface_list;
extern unsigned short *leafbrush_list;
extern int *surfedge_list;
extern bsp_edge* edge_list;
extern bsp_brush* brush_list;
extern bsp_brushside*  brushside_list;
extern bsp_area* area_list;
extern bsp_areaportal* areaportal_list;
extern byte* visdata_list;
extern byte* lightdata_list;
extern char* entdata_list;
extern bsp_vis* vis_list;
extern unsigned short* marksurfaces_list;
extern miptex_t* textures;
extern byte* texturedata;

extern int nummodels, visdatasize, lightdatasize, entdatasize, numleafs, numplanes, numvertices, numnodes, numclipnodes, numtexinfo, numfaces,
	       numedges, numleaffaces, numleafbrushes, numsurfedges, numbrushes, numbrushsides, numareas, numareaportals, nummarksurfaces;

struct CUSTOMVERTEX
{
    D3DXVECTOR3 v;  // untransformed 3D position for the vertex
    D3DXVECTOR3 n;  // normal
	D3DCOLOR color; // vertex color
	FLOAT tu, tv;   // texture coordinates
	FLOAT lu, lv;   // lightmap coordinates
};

#define QUAKE1 1
#define QUAKE2 2

extern struct _QMAP
{
	short type;
	int v_count;
	CUSTOMVERTEX v [ 65536 * 4 ];
	face_t12 faces [ 65536 ];
	BOOL mark_faces [ 65536 ];
	
	void Release ( )
	{
		v_count = 0;
	
		SAFE_DELETE ( model_list2 ); nummodels = 0;
		SAFE_DELETE ( vertex_list ); numvertices = 0;
		SAFE_DELETE ( plane_list ); numplanes = 0;
		SAFE_DELETE ( leaf_list2 ); numleafs = 0;
		SAFE_DELETE ( node_list2 ); numnodes = 0;
		SAFE_DELETE ( texinfo_list ); numtexinfo = 0;
		SAFE_DELETE ( face_list ); numfaces = 0;
		SAFE_DELETE ( leafface_list ); numleaffaces = 0;
		SAFE_DELETE ( leafbrush_list ); numleafbrushes = 0;
		SAFE_DELETE ( surfedge_list ); numsurfedges = 0;
		SAFE_DELETE ( edge_list ); numedges = 0;
		SAFE_DELETE ( brush_list ); numbrushes = 0;
		SAFE_DELETE ( brushside_list ); numbrushsides = 0;
		SAFE_DELETE ( area_list ); numareas = 0;
		SAFE_DELETE ( areaportal_list ); numareaportals = 0;
		SAFE_DELETE ( visdata_list ); visdatasize = 0;
		SAFE_DELETE ( lightdata_list ); lightdatasize = 0;
		SAFE_DELETE ( entdata_list ); entdatasize = 0;
		
		SAFE_DELETE ( model_list1 ); nummodels = 0;
		SAFE_DELETE ( surfedge_list ); numsurfedges = 0;
		SAFE_DELETE ( node_list1 ); numnodes = 0;
		SAFE_DELETE ( clipnode_list ); numclipnodes = 0;
		SAFE_DELETE ( leaf_list1 ); numleafs = 0;
		SAFE_DELETE ( marksurfaces_list ); nummarksurfaces = 0;
		SAFE_DELETE ( texinfo_list1 );
		SAFE_DELETE ( texturedata );
		SAFE_DELETE ( textures );
	}

} QMAP;
