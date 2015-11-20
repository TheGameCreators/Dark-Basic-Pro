
#ifndef _UNIVERSE_H_
#define _UNIVERSE_H_

//////////////////////////////////////////////////////////////////////////////////////
// NOTES /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/*
	enhanced nodetree commands, wrapped up in a class to make things easier
*/

//////////////////////////////////////////////////////////////////////////////////////
// COMMON INCLUDES ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include ".\..\DBOFormat\DBOFormat.h"
#include ".\..\data\cdatac.h"
#include ".\\ElipsoidCollision\collision.h"

//////////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#ifndef SAFE_DELETE
# define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
# define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
# define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
# define SAFE_MEMORY( p )       { if ( p == NULL ) return false; }
# define MAX_STRING				256
#endif

//////////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITION //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// forward declarations for classes and structures
class  cBSPPortal;
class  cBSPTree;
class  cVertex;
struct tagCameraData;

struct sCollisionVertex
{
	D3DXVECTOR3 vecPosition;
//	float		tu, tv;
};

struct sCollisionPolygon
{
	sCollisionVertex	triangle [ 3 ];
//	D3DXVECTOR3			normal;
	DWORD				diffuseid;
};

struct sPortalVertex
{
	D3DXVECTOR3		vecPosition;			// position
	D3DXVECTOR3		vecNormal;				// normal
	DWORD			dwDiffuse;				// diffuse
	float			tu, tv;					// texture coordinates
};

struct sShadowCaster
{
	D3DXMATRIX		matPos;
	sMesh*			pMeshRef;
};

struct sShadowLight
{
	float fX, fY, fZ, fRange;			// light controls its own shadowmeshes
	vector < sShadowCaster* > pCasterRefList;
	vector < sMesh* > pShadowMeshList;

    sShadowLight()
    {
        fX = 0.0;
        fY = 0.0;
        fZ = 0.0;
        fRange = 0.0;
    }
};

class cUniverse
{
	private:

		enum eLocation
		{
			// location variables
			TopLeftFront,
			TopLeftBack,
			TopRightBack,
			TopRightFront,
			BottomLeftFront,
			BottomLeftBack,
			BottomRightBack,
			BottomRightFront,
			UnknownLocation
		};

		enum ePoint
		{
			// point variables, used for determining a
			// polygons location against a plane

			CP_FRONT,
			CP_BACK,
			CP_ONPLANE,
			CP_SPANNING,
			CP_OUTSIDE
		};

		enum ePosition
		{
			eBack,
			eFront,
			eTop,
			eBottom,
			eRight,
			eLeft,
			eUnknown
		};

		enum eCollision
		{
			ePolygon, //0
			eBox, //1
			eReducedPolygon, //2
			eNone //3
		};

		struct sPortal
		{
			// portals for nodes

			sPortalVertex	vertices [ 4 ];				// portal vertices
			sMesh*			pBlocker;					// mesh which blocks portal
			
			D3DXVECTOR3 vecCentre;					// centre
			D3DXVECTOR3 vecNormal;					// portal normal
			D3DXVECTOR3 vecMin;						// min bounds
			D3DXVECTOR3 vecMax;						// max bounds

			float		fRadius;					// radius
			float		fHighestPoint;				// highest vertex point in portal

			bool		bVisible;					// is portal on / off
			bool		bVisibleInViewingFrustum;	// is portal in viewing frustm
			int			iSolidityMode;				// 1-partial,2-full

			sPortal ( );
		};

		struct sNode
		{
			// node data used to generate universe
			eLocation   location;
			sMesh**		ppMeshList;						// list of meshes within the node
			sPortal*	pMeshPortalList;				// portals created from meshes
			sPortal		portals [ 6 ];					// portals for a node
			int			iMeshCount;						// number of meshes within the node
			int			iMeshPortalCount;				// number of mesh portals
			int			iPolygonCount;					// number of polygons in node
			int			iMaxPolygonCount;				// maximum polygon count allowed for node
			bool		bChecked;
			bool		bClear;
//			vector < sCollisionPolygon > collisionF;	// full polygon data
//			vector < sCollisionPolygon > collisionB;	// blocklod data (fast collision)

			// node data needed in real-time
			D3DXVECTOR3	vecCentre;						// centre point of node
			D3DXVECTOR3	vecDimension;					// node dimensions
			sNode*		pNeighbours [ 6 ];				// instant reference to neighbors
			vector < sCollisionPolygon > collisionE;	// working collision (ellipse...)

			D3DXVECTOR3	vecColMin;						// collisionE bound box
			D3DXVECTOR3	vecColMax;

			// node reference for nodeuseflag (runtime reference to memory array)
			DWORD		dwNodeRefIndex;

			sNode  ( );							// constructor
			~sNode ( );							// destructor
		};

		struct sStaticObject
		{
			sFrame*		pFrame;			// frame of object to add
			sObject*	pObject;		// original object
			int			iID;			// object id
			int			iCollision;		// collision type
			int			iCastShadow;	// casts a shadow
		};

		// member variables
		sNode*						m_pNodeList;				// main node pointer for tree
		sNode*						m_pNode;					// list of nodes
		sNode**						m_pNodeRef;					// list of nodes by coordinate

		DWORD						m_dwNodeUseFlagMapSize;		// size of final node universe
		bool*						m_pNodeUseFlagMap;			// flag so draw once only node per cycle

		bool						m_bDebug;					// flag for debug mode
		bool						m_bGhostDebug;				// special internal flag for ghosting meshes
		bool						m_bCreated;					// flag to determine if node has been created
		bool						m_bPortalsActivated;		// should we use portals
		bool						m_bPortalsCreated;			// are portals created
		bool						m_bRayCast;					// is ray cast on
		
		D3DXVECTOR3					m_vecLineStart;				// start of ray cast
		D3DXVECTOR3					m_vecLineEnd;				// end of ray cast
		D3DXVECTOR3					m_vecNodeMax;				// max bounds of universe
		D3DXVECTOR3					m_vecNodeMin;				// min bounds of universe
		D3DXVECTOR3					m_vecNodeCentre;			// centre of universe

		int							m_iNodeListSize;			// number of nodes
		int							m_iUniverseSizeX;			// size of universe node dimensions
		int							m_iUniverseSizeY;			// size of universe node dimensions
		int							m_iUniverseSizeZ;			// size of universe node dimensions
		int							m_iHalfUniverseSizeX;
		int							m_iHalfUniverseSizeY;
		int							m_iHalfUniverseSizeZ;

		tagCameraData*				m_pCameraData;				// camera data

		vector < sStaticObject >	m_StaticObjectList;			// all static objects
		vector < sMesh*        >	m_pMeshList;				// list of meshes
		vector < int           >	m_iMeshCollisionList;		// collision data
		vector < sMesh*        >	m_pColMeshList;				// list of collision meshes
		vector < sObject*      >	m_VisLinkedObjectList;		// all nonstatic objects with visibility tied to nodetree
		vector < sNode*        >	m_VisLinkedObjectInNodeList;// stores current node of said object (see above)

		DWORD						m_dwColour;
		bool						m_bMoveLeft;

		// Final Scene Data

		struct sAreaLink
		{
			// One Link(portal) From an area box
			int							iLinkedTo;
			int							iTouchingSide;
			int							iSolidity;
			D3DXVECTOR3					vecPortalMin;
			D3DXVECTOR3					vecPortalMax;
			D3DXVECTOR3					vecPortalCenter;
			D3DXVECTOR3					vecPortalDim;
			D3DXVECTOR3					vecA;
			D3DXVECTOR3					vecB;
			D3DXVECTOR3					vecC;
			D3DXVECTOR3					vecD;

			// Work data (not saved with universe)
			sNode*						pDebugRegion;
		};

		struct sArea
		{
			// Area Box Dimensions
			D3DXVECTOR3					vecMin;
			D3DXVECTOR3					vecMax;
			D3DXVECTOR3					vecCentre;

			// Area Box Geometry
			vector < sMeshGroup* >		meshgroups;
			vector < sMeshGroup* >		sharedmeshgroups;
			vector < sMeshGroup* >		meshgroupref;

			// Area Box Link Info
			int							iLinkMax;
			vector < sAreaLink* >		pLink;

			// Work data (not saved with universe)
			int							iCount;
			vector < sNode* >			nodes;
			sNode*						pDebugRegion;
			bool						bRenderedThisCycle;

            sArea();
		};

		// Scorch Data
		sMesh*						m_pScorchMesh;
		DWORD						m_dwScorchVPos;
		DWORD						m_dwPolyDrawLimit;
		int							m_iScorchTypeMax;
		int							m_iScorchUVWidth;
		float						m_fScorchUTile;
		float						m_fScorchVTile;

		// Shadow Data
		vector < sShadowCaster >	m_pShadowCasterMasterList;
		vector < sShadowLight >		m_pShadowLightList;

		// runtime cycle properties
		DWORD						m_dwDrawSignature;
		DWORD						m_dwRecurseTable [ 20 ];

		// get node points

		#ifndef __GNUC__
		_inline D3DXVECTOR3 GetTopLeftFront     ( sNode* pNode );
		_inline D3DXVECTOR3 GetTopLeftBack      ( sNode* pNode );
		_inline D3DXVECTOR3 GetTopRightFront    ( sNode* pNode );
		_inline D3DXVECTOR3 GetTopRightBack     ( sNode* pNode );
		_inline D3DXVECTOR3 GetBottomLeftFront  ( sNode* pNode );
		_inline D3DXVECTOR3 GetBottomLeftBack   ( sNode* pNode );
		_inline D3DXVECTOR3 GetBottomRightBack  ( sNode* pNode );
		_inline D3DXVECTOR3 GetBottomRightFront ( sNode* pNode );
		#else
		D3DXVECTOR3 GetTopLeftFront     ( sNode* pNode );
		D3DXVECTOR3 GetTopLeftBack      ( sNode* pNode );
		D3DXVECTOR3 GetTopRightFront    ( sNode* pNode );
		D3DXVECTOR3 GetTopRightBack     ( sNode* pNode );
		D3DXVECTOR3 GetBottomLeftFront  ( sNode* pNode );
		D3DXVECTOR3 GetBottomLeftBack   ( sNode* pNode );
		D3DXVECTOR3 GetBottomRightBack  ( sNode* pNode );
		D3DXVECTOR3 GetBottomRightFront ( sNode* pNode );		
		#endif
		
		// vertex functions
		void TransformSelectedVertices ( sMesh* pMesh, DWORD dwFrom, DWORD dwTo, D3DXMATRIX* pmatWorld );
		void TransformVertices		( sMesh* pMesh, D3DXMATRIX matWorld );
		void SetupVertex			( sPortalVertex* pVertex, D3DXVECTOR3 vecPosition, D3DXVECTOR3 vecNormal, float tu, float tv );
		void CopyVertices			( sPortalVertex* pTo, cVertex* pFrom );
		bool CopyPortalVertices		( cBSPTree* pTree, sNode* pNode, sMesh* pMesh );
		void CreatePortalVertices	( sNode* pNode, bool bSetup = true, DWORD dwColour = D3DCOLOR_ARGB ( 255, 0, 0, 255 ) );
		
		// draw functions
		void DrawDebug				( sNode* pNode );
		void DrawNodePortals		( sNode* pNode );
		void DrawNodeBounds			( sNode* pNode );
		void DrawRayCast			( void );
		
		// build functions
		void BuildPortalCentre     ( sNode* pNode );
		bool BuildDistanceList     ( sNode* pNode, D3DXVECTOR3* pVecNormals, sPortal** ppPortals, int iCount, int* piA, int* piB, int* piC );
		bool BuildVisibility       ( sNode* pNode );
		bool Build                 ( void );
		
		// portal functions
		bool IsPortalValid			( cBSPPortal* pPortal, sMesh* pMesh );
		bool IsBoxWithinNode		( sNode* pNode, D3DXVECTOR3* pvecCenter, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax, int *iSide );
//		bool IsBoxPurelyWithinNode	( sNode* pNode, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax );
		bool IsBoxPurelyWithinNode	( sNode* pNode, D3DXVECTOR3* pvecCenter );
		bool IsPointWithinNode		( sNode* pNode, float x, float y, float z );
		int	 GetPortalCount			( cBSPTree* pTree, sMesh* pMesh );
		bool AddPortals				( sNode* pNode );
		void GetBoundingBox			( sPortal* pPortal, D3DXVECTOR3* pVecMin, D3DXVECTOR3* pVecMax, float* pfRadius, bool bSkip0 );
		void CalculateNormal		( sPortal* pPortal, D3DXVECTOR3* pVecNormal );
		
		// misc functions
		bool IsObjectWithinZone          ( sNode* pNode, sMesh* pMesh );
		void DetermineObjectLocation     ( sNode* pNode, sMesh* pMesh, bool* pbLocationList );
		void AddMeshToNode               ( sNode* pNode, sMesh* pMesh );
		void CopyCollisionDataToNode     ( sNode* pNode, sMesh* pMesh, sMesh* pReducedMesh, DWORD dwArbitaryValue );
		void CalculateSolidityOfMeshes		( void );
		void GetBoundingBoxForAllMeshes  ( void );
		void LinkMeshesToNodes           ( void );
		void FindNeighbouringNodes       ( void );
		void CreateNodesForScene         ( bool bMode );
		void SetWorldMatrix              ( void );
		bool CopyMeshPropertiesToNewMesh ( sMesh* pOriginalMesh, sMesh* pNewMesh );
		void GetAreaBounds  ( sArea* pArea );
		bool WillAreaBoxEnterAnotherAreaBox ( int iSide, int iAreaX1, int iAreaY1, int iAreaZ1, int iAreaX2, int iAreaY2, int iAreaZ2 );
		bool BuildAreaBoxes ( void );
		bool GetSideFree ( int iPortalSide, int iRefX, int iRefY, int iRefZ, int* pSolidity );
		void ExtendPortalMinMax ( int iPortalSide, int iX, int iY, int iZ, D3DXVECTOR3* pvecPortalMin, D3DXVECTOR3* pvecPortalMax );
		void CreatePortalAndReset ( sArea* pCurrentArea, int iChkAreaBox, int iTouchingSide, D3DXVECTOR3 vecPortalMin, D3DXVECTOR3 vecPortalMax, int iSolidity );
		bool BuildAreaLinks ( void );

		// final universe building functions
		DWORD GetMasterMeshIndex ( sMesh* pFindMesh );
		DWORD GetNodeIndex ( sNode* pFindNode );
		void FindAreaBoxHoldingMesh ( sMesh* pMesh, sArea** ppArea );
		void BuildAreaBoxMeshGroups ( void );
		bool DoMeshTexturesMatch ( sMesh* pMeshA, sMesh* pMeshB );
		sMeshGroup* FindMeshInAreaBox ( sArea* pArea, sMesh* pMesh, sArea* pNeighborArea );
		sMeshGroup* AddMeshToAreaBox ( sArea* pArea, sMesh* pMesh, sArea* pNeighborArea );
		void UploadMeshgroupsToBuffers ( void );
		void CalculateNodeCollisionBoxes ( void );

	public:

		// V111BETA4 - used to control whethe full portal resursing used (faster without=default)
		int							m_iFullPortalRecurse;

		// areabox accessable outside of univers class
		vector < sArea* >			m_pAreaList;

		// areabox scanning work vars
		vector < D3DXPLANE* >		m_pFrustrumList;

		// Shortlist of meshes (for quick processing)
		vector < sMesh* >			m_pMeshShortList;

		// Master Mesh used during saving/loading of universe
		vector < sMesh* >			m_pMasterMeshList;

		// Working Polygon List for refreshing collision pool
		DWORD						m_dwCollisionPoolMax;
		DWORD						m_dwCollisionPoolIndex;
		float*						m_pCollisionPool;
		DWORD*						m_pCollisionDiffuse;

		// Statistical Data
		DWORD						m_dwNumberOfCurrentAreaBoxPolygonsDrawn;
		DWORD						m_dwNumberOfCurrentAreaBoxPrimCalls;
		DWORD						m_dwPolygonsForCollision;
		DWORD						m_dwVolumesTestedForCollision;

		cUniverse  ( );
		~cUniverse ( );

		void LeesTestFPSCQuadRemover();
		void Load ( LPSTR pFilename, int iDivideTextureSize );
		void Save ( LPSTR pFilename );
		void LoadDBU ( LPSTR pDBUFilename );
		void SaveDBU ( LPSTR pDBUFilename );
		void SetEffectTechnique ( LPSTR pTechniqueName );

		bool Make				( float fX, float fY, float fZ );					// make a universe
		bool Attach				( sObject* pObject );
		bool Detach				( sObject* pObject );
		bool Add				( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );	
		bool Add				( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker );
		bool Remove				( int iID );
		bool RenderArea			( DWORD dwFrustumCount, sArea* pArea );
		bool IsPortalFacingLookVector ( int iSide, D3DXVECTOR3* pvecPortalCenter, D3DXVECTOR3* pvecLookNormal );
		bool RecurseRenderArea	( sArea* pParentArea, DWORD dwFrustumCount, sArea* pCurrentArea );
		bool Render				( void );
		bool SetDebugOn			( void );											// set debug mode on
		bool SetDebugOff		( void );											// set debug mode off
		int  RayCast	 		( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
		bool RayVolume			( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize );
		bool SetPortalsOn		( void );
		bool SetPortalsOff		( void );
		bool BuildPortals		( void );

		bool CollisionQuickRayCast ( sMesh* pMesh, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
		bool CollisionSingleRayCast (	sMesh* pMesh,
										float fX, float fY, float fZ,
										float fNewX, float fNewY, float fNewZ,
										float* fBestDistance, 
										float* fBestU, 
										float* fBestV, 
										sMesh** pBestMesh,
										int* iBestTriangle,
										bool bCollectFeedback );
		bool CollisionVeryQuickRayCast ( sMesh* pMesh, D3DXVECTOR3* pvecLineStart, D3DXVECTOR3* pvecLineEnd );

		bool CollisionBoundBoxTest ( D3DXVECTOR3* pvecMinA, D3DXVECTOR3* pvecMaxA, D3DXVECTOR3* pvecMinB, D3DXVECTOR3* pvecMaxB );
		void AddNodeColDataToPolygonList ( sNode* pCurrentNode );
		int  CollisionRayCast	( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
		void AddNodeColDataIfWithinBound ( sNode* pCheckNode, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax );
		bool CollisionRayVolume	( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize );
		bool CollisionTest		( int iTestIndex, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float fNewX1, float fNewY1, float fNewZ1, float fNewX2, float fNewY2, float fNewZ2 );
		void GetMeshList		( vector < sMesh* > *pMeshList );

		bool RecurseCheckArea ( D3DXVECTOR3* pAtPos, D3DXVECTOR3* pvecToCenter, D3DXVECTOR3* pvecToSize, sArea* pParentArea, DWORD dwFrustumCount, sArea* pCurrentArea );
		bool CanAreaPointSeeBox ( int iAtAreaBox, D3DXVECTOR3* pvecToCenter, D3DXVECTOR3* pvecToSize, D3DXVECTOR3* pvecToMax );

		void SetScorchTexture	( int iImageID, int iWidth, int iHeight );
		void AddScorch			( float fSize, int iScorchType );

		void SetShadow			( sMesh* pMesh, D3DXMATRIX* pMeshMatrix );
		void SetLight			( int iLightIndex, float fX, float fY, float fZ, float fRange, bool bRecreateAllShadows );
		void RenderShadows		( void );

		// fast collision (for lightmapping)
		void ShortlistMeshesWithinBox ( D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax );
		bool RayHitShortlistMesh ( D3DXVECTOR3* pvecFrom, D3DXVECTOR3* pvecTo );

		// mike - 080305 - so we can get the master mesh list
		void GetMasterMeshList ( vector < sMesh* > *pMeshList );
};

#endif _NODETREE_H_
