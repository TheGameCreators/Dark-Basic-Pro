#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#include "CSG.h"

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK void ConstructorCSG ( HINSTANCE hSetup );
DARKSDK void DestructorCSG  ( void );

DARKSDK void CompileCSG ( char* szInput, char* szOutput, float fEpsilon );

DARKSDK int* GetProgress ( void );

DARKSDK void PerformCSGOnXModel ( char* szInput, char* szOutput, float fEpsilon );
DARKSDK void PerformCSGOnXModel ( char* szInput, char* szOutput );

//////////////////////////////////////////////////////////////////////////////////

typedef struct tagPLANE
{
	D3DXVECTOR3 PointOnPlane;
	D3DXVECTOR3 Normal;
} PLANE;

typedef struct BSPPOLYGON
{
	D3DVERTEX*	Vertices;
	WORD*		Indices;
	WORD		VertexCount;
	WORD		IndexCount;
	D3DXVECTOR3	Normal;
	int			TextureIndex;
	BSPPOLYGON*	NextPoly;
	BYTE		UsedAsSplitter;
} BSPPOLYGON;

typedef struct
{
	long		StartPoly;
	long		EndPoly;
} LEAF;

typedef struct
{
	BYTE		IsLeaf;
	long		Plane;
	long		Front;
	long		Back;
} NODE;

#define			CP_FRONT			1
#define			CP_BACK				2
#define			CP_ONPLANE			3
#define			CP_SPANNING			4


class CBSPTree 
{
	public:
		CBSPTree  ( );
		~CBSPTree ( );

		BOOL		ClipTree ( long CurrentNode, long* Polygons, long pPolyCount, CBSPTree* BSPTree, BOOL ClipSolid, BOOL RemoveCoPlanar );
		void		InitPolygons ( BRUSH* brush, long BrushCount, BOOL BSPCollect );
		void		RenderBSPPolys ( int BrushIndex );
		void		InvertPolys ( void );

		BSPPOLYGON* AddPolygonEX ( BSPPOLYGON* Poly );
		CBSPTree*	CopyTree ( void );

		BSPPOLYGON*		Polygons;
		BOOL			FatalError;
		BOOL*			PolygonDeleted;
		BRUSH*			BrushBase;
		long			PolyCount;
		long			RootNode;
		BSPPOLYGON		*PolygonList;
		PLANE*			Planes;
		NODE*			Nodes;
		LEAF*			Leaves;
		long			NodeCount;
		long			PlaneCount;
		long			LeafCount;

		BOOL			IsDirty;
		BOOL			m_bMisc;

	private:
		BOOL		Get_Intersect ( D3DXVECTOR3* linestart, D3DXVECTOR3* lineend, D3DXVECTOR3* vertex, D3DXVECTOR3* normal, D3DXVECTOR3* intersection, float* percentage );
		void		SplitPolygon ( BSPPOLYGON* Poly,PLANE* Plane, BSPPOLYGON* FrontSplit, BSPPOLYGON* BackSplit );
		long		SelectBestSplitter ( BSPPOLYGON* PolyList, long CurrentNode );
		void		BuildBSPTree ( long CurrentNode, BSPPOLYGON* PolyList );

		int			ClassifyPoly  ( PLANE* Plane, BSPPOLYGON* Poly );
		int			ClassifyPoint ( D3DXVECTOR3* pos, PLANE* Plane );
		int			AllocAddPlane ( void );
		int			AllocAddPoly  ( void );
		int			AllocAddNode  ( void );
		int			AllocAddLeaf  ( void );
		void		KillTree      ( void );
		void		KillPolys     ( void );

		BSPPOLYGON* AddPolygonSplit ( BSPPOLYGON* Parent, POLYGON*	  Face );
		BSPPOLYGON* AddPolygon      ( BSPPOLYGON* Parent, POLYGON*	  Face );
		BSPPOLYGON* AddBSPPolygon   ( BSPPOLYGON* Parent, BSPPOLYGON* Poly );

};

#endif