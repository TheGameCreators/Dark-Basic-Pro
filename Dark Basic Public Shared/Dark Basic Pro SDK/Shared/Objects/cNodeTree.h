#ifndef _NODETREE_H_
#define _NODETREE_H_

//////////////////////////////////////////////////////////////////////////////////
// INFORMATION ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
	the nodetree functionality is in the object manager DLL rather than in the
	actual model format DLL, the reasons for this are
	
		* it will eventually work independant of model format
		* by doing this it's more generic
		* if we didn't do this we would have to code individual implementations
		  for each model format

*/
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <D3DX8.h>
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

//#include "cObjectDataC.h"
#include "Collision.h"

// enumerate the two types of tree structures
enum TreeTypes	{ 
					QUADTREE = 0, 
					OCTREE
				};

class cNodeTreeMesh
{
	private:

		ID3DXMesh*				m_Mesh;
		LPDIRECT3DTEXTURE8*		m_pTextures;
		D3DMATERIAL8*			m_pMaterials;
		DWORD					m_dwNumMaterials;

		DWORD					m_dwNumVertices;

		// the sVertex structure is a custom vertex structure that
		// contains only the 3D coordinates, this is used to
		// retrieve coordinate info from a mesh's vertex buffer
		typedef struct sVertex
		{
			float x, y, z;
		} sVertex;

		// polygon information
		typedef struct sPolygon
		{
			unsigned short Vertex0;        // vertex index #1
			unsigned short Vertex1;        // vertex index #2
			unsigned short Vertex2;        // vertex index #3
			unsigned long  Group;          // texture group
			unsigned long  Timer;          // frame last drawn
		} sPolygon;

		// the node structure keeps count of the # of polygons in
		// its 3D space, an array of sPolygon structures, the
		// 3D coordinates of the node ( as well as the size, which
		// is the distance from the center to one edge making the node
		// a perfect cube ), and pointers to the child nodes
		typedef struct sNode
		{
			float           XPos, YPos, ZPos; // center coordinate of node
			float           Size;             // size of node

			unsigned long   NumPolygons;      // number of polygons in node
			unsigned long*  PolygonList;      // polygon list

			sNode*			Nodes [ 8 ];      // child nodes 4 = quad, 8 = oct

			sNode ( )
			{
				XPos = YPos = ZPos = Size = 0.0f;	// position and size
				NumPolygons = 0;					// set to no polygons in node
				PolygonList = NULL;					// clear polygon list
				
				for ( short i = 0; i < 8; i++ )
					Nodes [ i ] = NULL;
			}

			~sNode ( )
			{
				delete [ ] PolygonList;
				
				for ( short i = 0; i < 8; i++ )
					delete Nodes [ i ];
			}
		} sNode;

		typedef struct sGroup 
		{
			unsigned long			NumPolygons;
			unsigned long			NumPolygonsToDraw;

			IDirect3DIndexBuffer8*	IndexBuffer;
			unsigned short*			IndexPtr;

			sGroup ( )
			{
				NumPolygons = 0;
				IndexBuffer = NULL;
			}

			~sGroup ( )
			{ 
				if ( IndexBuffer != NULL )
					IndexBuffer->Release ( );
			}

		} sGroup;

		int				m_TreeType;     // type of nodetree
		
		float			m_Size;         // size of world cube
		float			m_MaxSize;      // maximum node size

		sNode*			m_ParentNode;   // node linked list parent

		unsigned long	m_NumGroups;    // number of texture groups
		sGroup*			m_Groups;       // texture groups

		unsigned long	m_MaxPolygons;  // maximum number of polygons per node

		unsigned long	m_NumPolygons;  // number polygons in scene
		sPolygon*		m_Polygons;     // list of polygons

		unsigned long	m_Timer;        // current draw timer

		char*			m_VertexPtr;    // pointer to mesh vertices
		unsigned long   m_VertexFVF;    // mesh vertex FVF
		unsigned long   m_VertexSize;   // size of mesh vertex

		
		void          SortNode           ( sNode* Node, float XPos, float YPos, float ZPos, float Size );
		void          AddNode            ( sNode* Node );
		BOOL		  IsPolygonContained ( sPolygon* Polygon, float XPos, float YPos, float ZPos, float Size );
		unsigned long CountPolygons      ( float XPos, float YPos, float ZPos, float Size );

		sCollisionData* m_pCollisionData;

	public:
		cNodeTreeMesh  ( );
		~cNodeTreeMesh ( );

		BOOL Create ( ID3DXMesh* pMeshData, LPDIRECT3DTEXTURE8* pTextures, D3DMATERIAL8* pMaterials, DWORD dwNumMaterials, int TreeType, float MaxSize, long MaxPolygons, D3DXMATRIX matObject );
		BOOL Free   ( );

		BOOL  Render           ( float ZDistance = 0.0f );
		float GetClosestHeight ( float XPos, float YPos, float ZPos );
		float GetHeightBelow   ( float XPos, float YPos, float ZPos );
		float GetHeightAbove   ( float XPos, float YPos, float ZPos );

		BOOL  CheckIfNodePointWithinSphere ( float X, float Y, float Z, float Radius, float* Length );
		BOOL  CheckIntersect   ( float XStart, float YStart, float ZStart, float XEnd, float YEnd, float ZEnd, float* Length );
};

void QuickSortArray    ( int* array, int, int );
void SwapInts          ( int* array, int, int );
void CreateSortedArray ( int* piArray, int** ppiSorted, int iCurrentSize, int* piFinalSize );

#endif
