
#ifndef _COMPILERTYPES_H_
#define _COMPILERTYPES_H_

#include "Common.h"
#include "CVector.h"
#include "CBounds.h"

class cBSPTree;

#define ESR_POS_CUSTOM      0
#define ESR_POS_SPAWNPOINT  1
#define ESR_POS_AUTOMATIC   2

#define BSP_TYPE_NONSPLIT   0
#define BSP_TYPE_SPLIT      1

#define FRONT_OWNER         0
#define BACK_OWNER          1
#define NO_OWNER            2

struct sHSR
{
	// hidden surface remove
    bool			bEnabled;			// is HSR enabled
};

struct sESR
{
	// external surface removal
    bool            bEnabled;            // is ESR enabled
    int			    iDeterminePos;		 // position
    bool            bRemoveWet;          // remove polygons
    float           fCustomX;            // flood fill x
    float           fCustomY;            // flood fill y
    float           fCustomZ;            // flood fill z
    int				iSpawnPoint;         // spawn point
};

struct sBSP
{
	// binary space partition
    bool            bEnabled;            // is BSP enabled
    int				iTreeType;           // tree type
    float           fSplitHeuristic;     // split number
    int			    iSplitterSample;     // number of splitters to sample
    bool            bRemoveBackLeaves;   // remove illegal back faces
    bool            bAddBoundingPolys;   // add bounding polygons
    bool            bLinkDetailBrushes;  // determine the leaves of detail brushes
};

struct sPRT
{
	// portal
    bool            bEnabled;            // are portals enabled
    bool            bFullCompile;        // should we do a full compile
};

struct sPVS
{
	// potential visibility set
    bool            bEnabled;            // pvs enabled
    bool            bFullCompile;        // full pvs compile
    unsigned char   ucClipTestCount;     // number of portal clip tests
};

struct sTJR
{
	// t junction removal
    bool            bEnabled;            // t junction removal on
};

struct sGeneralStats
{
	// general stats
    int				iBrushCount;         // number of brushes
    int				iPolygonCount;       // polygon count
    int				iDetailBrushCount;   // number of detail brushes
    int				iSpawnPointCount;    // number of spawn points
};

struct sHSRStats
{
	// hidden surface removal stats
    int				iBrushCount;			// number of brushes
    int				iPrePolygonCount;       // number of polygons before
    int				iPostPolygonCount;      // number of polygons after
    int				iPolygonsRemoved;       // number of polygons removed
};
        
struct sESRStats
{
	// external surface removal
    int				iPrePolygonCount;       // number of polygons before
    int				iPostPolygonCount;      // number of polygons after
    int				iPolygonsRemoved;       // number of polygons removed
};

struct sBSPStats
{
	// bsp stats
    int				iPrePolygonCount;       // number of polygons before
    int				iPostPolygonCount;      // number of polygons after
    int				iNodeCount;				// number of nodes in the tree
    int				iPlaneCount;			// number of planes in the tree
    int				iLeafCount;				// number of leaves in the tree
}; 
        
struct sPRTStats
{
	// portal stats
    int				iNodeCount;          // number of nodes
    int				iPortalCount;        // number of portals
};

struct sPVSStats
{
	// pvs stats
    int				iLeafCount;          // number of leaves used during compilation
    int				iPortalCount;        // number of portals used during compilation
    int				iPVSBufferSize;      // size of the uncompressed PVS buffer in bytes
    int				iPVSCompressedSize;  // size of the compressed PVS buffer in bytes
};

struct sTJRStats
{
	// t junction removal stats
    int				iScenePolygons;      // number of polygons used during repair
    int				iPreVertexCount;     // total number of vertices prior to repair
    int				iPostVertexCount;    // total number of vertices post repair
    int				iTJuncsRepaired;     // total number of t junctions repaired
};

class cPlane3;
class cBounds3;

class cVertex : public cVector3
{
	public:
		cVertex ( ) { x = y = z = tu = tv = 0.0f; }
		cVertex ( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; tu = tv = 0.0; }
		cVertex ( cVector3& vec ) { x = vec.x; y = vec.y; z = vec.z; tu = tv = 0.0f; }
		cVertex ( float _x, float _y, float _z, float _tu, float _tv ) { x = _x; y = _y; z = _z; tu = _tu; tv = _tv; }
    
		cVector3    Normal;
		float       tu;
		float       tv;
};

class cPolygon
{
	public:
				 cPolygon( );
		virtual ~cPolygon( );

		cVertex*		m_pVertices;
		DWORD		    m_dwVertexCount;

		int             AddVertices        ( int iVertexCount = 1 );
		int             InsertVertex       ( int iVertexPos );
		void            ReleaseVertices    ( void );
		bool            IsConvex           ( void );
		cVector3        CalculateNormal    ( void );
		bool            ContainsDegenerate ( void );

		virtual HRESULT Split             ( cPlane3& Plane, cPolygon* pFrontSplit, cPolygon* pBackSplit );
		virtual bool    GenerateFromPlane ( cPlane3& Plane, cBounds3& Bounds );

};

class cFace : public cPolygon
{
	public:
		cFace ( );

		cVector3        m_vecNormal;               // Face Normal
		int             m_iTextureIndex;           // Index into texture look up
		int             m_iMaterialIndex;          // Index into material look up
		int             m_iFlags;                  // Face Flags
		int				m_iSrcBlendMode;           // Face Source Blend Mode
		int				m_iDestBlendMode;          // Face Dest Blend Mode

		float           CalculateArea ( );

		virtual HRESULT Split             ( cPlane3& Plane, cFace* pFrontSplit, cFace* pBackSplit );
		virtual bool    GenerateFromPlane ( cPlane3& Plane, cBounds3& Bounds );
};

class cMesh
{
	public:
				 cMesh ( );
		virtual ~cMesh ( );

		char*			m_szName;                 // Stored name loaded from IWF
		cFace**			m_ppFaces;				  // Mesh faces
		DWORD			m_dwFaceCount;			  // Faces in this poly
		cBounds3        m_Bounds;                 // Meshes local space bounding box
		DWORD		    m_dwFlags;                // Mesh flags

		int             AddFaces             ( int iFaceCount = 1 );
		void            ReleaseFaces         ( void );
		bool            BuildFromBSPTree     ( cBSPTree * pTree, bool Reset = false );
		cBounds3&		CalculateBoundingBox ( void );
};

class cMaterial
{
	public:
		 cMaterial ( );
		~cMaterial ( );
    
		char           *Name;
		float           Power;
};

class cTexture
{
	public:
				 cTexture ( ULONG Size );
				 cTexture ( );
		virtual ~cTexture ( );
    
		bool            AllocateData ( ULONG Size );

		UCHAR           TextureSource;  // Information about where the texture is
		char           *Name;           // The character array containing the reference name.
		UCHAR           TextureFormat;  // The format of the internal texture if used
		USHORT          TextureSize;    // Size of the TextureData array
		UCHAR          *TextureData;    // The data being referenced
};

#endif