#ifndef _CBSPTREE_H_
#define _CBSPTREE_H_

// file includes
#include <vector>
#include "CompilerTypes.h"
#include "Common.h"
#include "CBounds.h"

// forward declarations
class cPlane3;
class cCompiler;

#define BSP_ARRAY_THRESHOLD     100
#define BSP_NEWSPLITNODES       0
#define BSP_SHARENODES          1

class cBSPFace : public cFace
{
	// bsp face class

	public:
		cBSPFace ( );
		cBSPFace ( cFace* pFace );

		bool        m_bUsedAsSplitter;         // polygon used as splitter
		int	        m_iOriginalIndex;          // original polygon index
		bool        m_bDeleted;                // face has been 'virtually' deleted
		int		    m_iChildSplit [ 2 ];       // during CSG, records the resulting 2 children
		cBSPFace*	m_pNext;                   // next poly in linked list
		int			m_iPlane;                  // the plane to which this face is linked
		int		    m_iFrameRendered;          // contains information about when this was last rendered

		// split a polygon against a plane
		virtual HRESULT Split ( cPlane3& splitPlane, cBSPFace* pFrontSplit, cBSPFace* pBackSplit );
};

class cBSPLeaf
{
	// bsp leaf class

	public:
				 cBSPLeaf ( );
		virtual ~cBSPLeaf ( );

		bool BuildFaceIndices ( cBSPFace* pFaceList );
		bool AddPortal        ( int iPortalIndex );
		bool AddObject        ( int iObjectIndex );
		bool IsSolid          ( void );
		
		std::vector < int >		m_iFaceIndices;     // indices to faces in this leaf
		std::vector < int >		m_iPortalIndices;   // indices to portals in this leaf
		std::vector < int >		m_iObjectIndices;	// indices to detail objects in this leaf
		DWORD					m_dwFaceCount;      // number of faces in this leaf
		DWORD					m_dwPortalCount;    // number of portals in this leaf
		DWORD					m_dwObjectCount;    // number of detail objects in this leaf
		DWORD					m_dwPVSIndex;		// index into the master PVS array
		cBounds3				m_Bounds;	        // bounding box
};

class cBSPNode
{
	// bsp node

	public:
				 cBSPNode ( );
		virtual ~cBSPNode ( ) { };

		// get node bounds
		void CalculateBounds ( cBSPFace* pFaceList, bool bResetBounds = true );

		int            m_iPlane;              // index to the plane for this node
		int            m_iFront;              // child in front of this node
		int            m_iBack;               // child behind this node
		cBounds3       m_Bounds;              // bounds of everything under this node
};

class cBSPPortal : public cPolygon
{
	// portal - not used just yet

	public:
		cBSPPortal ( );

		cBSPPortal*		m_pNextPortal;			// next portal
		cBSPPortal*		m_pPrevPortal;			// previous portal
		unsigned char   m_ucLeafCount;			// how many leaves this portal fell into
		int				m_iOwnerNode;		    // node that created this portal
		int				m_iLeafOwner [ 2 ];	    // front / back leaf owner array

		virtual HRESULT Split ( cPlane3& Plane, cBSPPortal* pFrontSplit, cBSPPortal* pBackSplit );
};

// typedefs to make declarations easier later on
typedef std::vector < cBSPNode*   >	vectorNode;
typedef std::vector < cPlane3*    >	vectorPlane;
typedef std::vector < cBSPLeaf*   >	vectorLeaf;
typedef std::vector < cBSPFace*   >	vectorBSPFace;
typedef std::vector < cBSPPortal* >	vectorBSPPortal;

class cBSPTree
{
	// the main BSP tree

	public:
				 cBSPTree ( );
		virtual ~cBSPTree ( );

		HRESULT         CompileTree      ( void );
		HRESULT         AddFaces         ( cFace** ppFaces, int iFaceCount = 1, bool bBackupOriginals = false );
		HRESULT         AddBoundingPolys ( bool bBackupOriginals = false );
		void            ReleaseTree      ( void );

		cBounds3& GetBounds  ( void );
		int   GetNodeCount   ( void );
		int   GetLeafCount   ( void );
		int   GetPlaneCount  ( void );
		int   GetFaceCount   ( void );
		int   GetPortalCount ( void );

		cBSPNode*		GetNode   ( int iIndex );
		cPlane3*		GetPlane  ( int iIndex );
		cBSPLeaf*		GetLeaf   ( int iIndex );
		cBSPFace*		GetFace   ( int iIndex );
		cBSPPortal*		GetPortal ( int iIndex );

		void            SetNode    ( int iIndex, cBSPNode*   pNode   );
		void            SetPlane   ( int iIndex, cPlane3*    pPlane  );
		void            SetLeaf    ( int iIndex, cBSPLeaf*   pLeaf   );
		void            SetFace    ( int iIndex, cBSPFace*   pFace   );
		void            SetPortal  ( int iIndex, cBSPPortal* pPortal );
		HRESULT         SetPVSData ( UCHAR PVSData [ ], int iPVSSize, bool bPVSCompressed );
		void            SetOptions ( sBSP& Options );
		void            SetParent  ( cCompiler* pParent );
		void            SetStats   ( sBSPStats* pStats );

		bool            IncreaseFaceCount   ( void );
		bool            IncreaseNodeCount   ( void );
		bool            IncreaseLeafCount   ( void );
		bool            IncreasePlaneCount  ( void );
		bool            IncreasePortalCount ( void );

		bool            LinkDetailObject ( int iObjectIndex, cVector3& vecSphereCentre, float fSphereRadius, int iNode = 0 );
		HRESULT         ClipTree         ( cBSPTree* pTree, bool bClipSolid, bool bRemoveCoPlanar, int iCurrentNode = 0, cBSPFace* pFaceList = NULL );
		void            RepairSplits     ( void );
		int	            FindLeaf         ( cVector3& vecPosition );
		

		bool            IntersectedByTree   ( cBSPTree* pTree );
		bool            IntersectedByFace   ( cFace* pFace, int iNode = 0 );
		bool            IntersectedBySphere ( cVector3& vecSphereCentre, float fSphereRadius, int iNode = 0 );

		static cBSPFace*	AllocBSPFace   ( cFace* pDuplicate = NULL );
		static cBSPPortal*  AllocBSPPortal ( );

		vectorBSPFace   m_vpRemovedFaces;       // back leaf removed faces
		UCHAR*			m_pPVSData;             // PVS data set
		int			    m_iPVSDataSize;         // size of the PVS data set
		bool            m_bPVSCompressed;       // is the PVS data compressed

	public:
		HRESULT         BuildPlaneArray    ( void );
		HRESULT         BuildBSPTree       ( int iNode, cBSPFace* pFaceList );
		HRESULT         ProcessLeafFaces   ( cBSPLeaf* pLeaf, cBSPFace* pFaceList );
		cBSPFace*		SelectBestSplitter ( cBSPFace* pFaceList, int iSplitterSample, float fSplitHeuristic );
    
		int			    CountSplitters ( cBSPFace* pFaceList );
		void            FreeFaceList   ( cBSPFace* pFaceList );
		void            TrashFaceList  ( cBSPFace* pFaceList );

		cBSPFace*		m_pFaceList;        // linked list head for pre compiled faces
		int			    m_iActiveFaces;     // number of active faces in the pre compiled list    
		vectorNode      m_vpNodes;          // nodes created by the BSP compiler
		vectorPlane     m_vpPlanes;         // node planes created by the BSP compiler
		vectorLeaf      m_vpLeaves;         // leaves created by the BSP compiler
		vectorBSPFace   m_vpFaces;          // resulting faces, either the originals or split versions
		vectorBSPPortal m_vpPortals;        // a set of portals
		vectorBSPFace   m_vpGarbage;        // faces with errors
		cBounds3        m_Bounds;           // bounding box
    
		sBSP		    m_OptionSet;        // BSP options
		sBSPStats*		m_pStats;           // stats
		cCompiler*		m_pParent;          // parent compiler
};

#endif
