#ifndef _CLIGHTMAPS_H_
#define _CLIGHTMAPS_H_

//////////////////////////////////////////////////////////////////////////////////
// CLASS DESCRIPTION /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*
	
*/
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <D3D9.h>
#include <D3DX9.h>
#include ".\..\DBOFormat\DBOFormat.h"
#include "universe.h"
#include <algorithm>
#include <vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////

// CONSTANTS
#define LMSMALLESTLM 4
#define LMTEXTILESIZE 128
#define LMMASTERTEXSLOTS 512/LMSMALLESTLM

//////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class cLightMapConsolidation
{
	// the set of functions here are used to take in several light map textures
	// and merge them into one big texture

	private:
		struct sLightMapVertex
		{
			// vertex structure

			float  x,  y,  z;	// position
			float nx, ny, nz;	// normal
			float tu, tv;		// texture coordinates
			float lu, lv;		// lightmap coordinates
		};

		// stores all LMdata-per-poly for entire master texture (quick reject of repeats)
		DWORD*						m_dwLumelStore;

		int							m_iTextureCount;			// number of textures
		int							m_iSize;					// size of master texture
		int							m_iLMWidth;					// copy of LM tex tile size 
		int							m_iLMHeight;
		RECT						m_rect [ LMMASTERTEXSLOTS * LMMASTERTEXSLOTS ];
		sMesh*						m_pMesh;					// pointer to mesh
		vector < sLightMapVertex >  m_OriginalVertexList;		// original vertex list

		LPDIRECT3DTEXTURE9			m_pMasterTexture;			// main master texture
		LPDIRECT3DTEXTURE9			m_pMasterTextureDDS;		// main master texture
		DWORD*						m_pMasterPixels;			// information for global locking
		DWORD						m_dwMasterWidth;
		DWORD						m_dwMasterHeight;
		DWORD						m_dwMasterPitch;

		char						m_pLMFile [ MAX_STRING ];	// lightmap filename
		LPSTR						m_pPathForLightMaps;

		void UpdateCoordinates	( DWORD dwWidth, DWORD dwHeight, int iPolyTexture, int iXStart, int iYStart, int iImageInMaster, bool bMode );
		int  IsTextureInList	( DWORD* pLumelPoly );
		bool LockTexture		( LPDIRECT3DTEXTURE9 pTexture, DWORD** ppPixels, DWORD* dwPitch, DWORD* dwWidth, DWORD* dwHeight );
		void UnlockTexture		( LPDIRECT3DTEXTURE9 pTexture );
		void FindSpace			( int iWidth, int iHeight, int* pX, int* pY );
		bool AddTexture			( DWORD* pLumelPoly, int iPolyTexture );

	public:
		cLightMapConsolidation  ( );
		~cLightMapConsolidation ( );

		void SetLMSavePath		( LPSTR pPath ) { m_pPathForLightMaps=pPath; }
		void Init				( DWORD dwWidth, DWORD dwHeight, int* piLightMap );
		bool Run				( sMesh* pMesh, DWORD** m_ppdwLumelList, bool* m_ppdwShadowList, int iCount );
		void Complete			( int iType );

		LPDIRECT3DTEXTURE9 GetTexture ( void );
		LPSTR GetName ( void ) { return m_pLMFile; }
};

class cLightMaps
{
	public:
		struct sLightMapVertex
		{
			// vertex structure

			float  x,  y,  z;	// position
			float nx, ny, nz;	// normal
			float tu, tv;		// texture coordinates
			float lu, lv;		// lightmap coordinates
		};

		struct sAABB
		{
			// axis aligned bounding box data

			D3DXVECTOR3	vecMin;
			D3DXVECTOR3	vecMax;
			D3DXVECTOR3	vecCentre;
			float		fRadius;
		};

		struct sPlane
		{
			D3DXVECTOR3 vecNormal;
			D3DXVECTOR3 vecRefPoint;
			float		A,
						B,
						C,
						D;
			int			iProjectionPlane;
		};

		struct sLight
		{
			int			iAreaBoxWitin;
			D3DXVECTOR3 vecPosition;
			float		fRadius;
			float		fBrightness;
			float		fRed;
			float		fGreen;
			float		fBlue;
			bool		bCastShadow;
		};

		struct sStaticObject
		{
			sFrame*  pFrame;		// frame of object to add
			sMesh*   pMesh;
			sObject* pObject;		// original object
			int		 iID;			// object id
			int		 iCollision;	// collision type
		};

		vector < sLight			> m_Lights;
		vector < sStaticObject	> m_pFrameList;

		vector < sLight*		> m_pLocalLightList;
		vector < D3DXPLANE*		> m_pLocalLightFrustrumList;
		vector < sMesh*			> m_pLocalLightOccluderList;

		int						m_iLMWidth;		// width and height of single LM tile
		int						m_iLMHeight;
		int						m_iLMQuality;
		LPSTR					m_pPathForLightMaps;
		int						m_iLightMap;

		D3DXVECTOR3 m_vecLumels [ LMTEXTILESIZE ] [ LMTEXTILESIZE ],// texture positions
					m_vecNormal,									// triangle normal
					m_vecPointOnPlane,								// point on plane
					m_vector1,										// temp vector 1
					m_vector2,										// temp vector 2
					m_uvVector,										// uv vector
					m_vecEdge1,										// triangle edge 1
					m_vecEdge2,										// triangle edge 2
					m_vecNewEdge1,									// triangle edge 1
					m_vecNewEdge2,									// triangle edge 2
					m_vecLightDirection;							// light direction
		DWORD		m_dwLumelColour [ LMTEXTILESIZE ] [ LMTEXTILESIZE ];// final light colour
		float		m_uvMinU,										// uv min extents
					m_uvMinV,										//      ""
					m_uvMaxU,										// uv max extents
					m_uvMaxV,										//      ""
					m_uvDeltaU,										// delta u
					m_uvDeltaV,										// delta v
					m_fX,											// x position
					m_fY,											// y position
					m_fZ,											// z position
					m_fDistance,									// distance from plane
					m_fCombinedRed,									// current red value
					m_fCombinedGreen,								// current green value
					m_fCombinedBlue,								// current blue value
					m_fIntensity,									// light intensity
					m_fLightDistance,								// distance from light
					m_fCosAngle;									// angle to lumel
		int			m_iFlag;
		int			m_iTriangle;

		cLightMapConsolidation*			m_pLumelConsolidation;
		cLightMapConsolidation*			m_pShadowConsolidation;

		DWORD							m_dwLightmapPolys;
		DWORD**							m_ppdwLightMapLumels;				// light map lumel arrays
		bool*							m_ppdwLightMapShadow;				// store if poly was shadowed
		sLightMapVertex*				m_pVertices [ 3 ];					// working vertices

		D3DXVECTOR3 GetNormal              ( void );
		sPlane      ConstructPlane         ( D3DXVECTOR3* vecRefPoint, D3DXVECTOR3* vecNormal );
		sAABB       GetTriangleBoundingBox ( D3DXVECTOR3* pVector );
		bool        LineOfSight            ( sMesh* pMesh, D3DXVECTOR3 vecStart, D3DXVECTOR3 vecEnd, sPlane plane );
		bool        AreCoPlanar            ( sPlane &a, sPlane &b, float fCosEps, float fDistEps );
		int         ClassifyPoint          ( D3DXVECTOR3 vecPoint, D3DXVECTOR3 vecA, D3DXVECTOR3 vecB );
		bool        GetIntersect           ( D3DXVECTOR3* vecLineStart, D3DXVECTOR3* vecLineEnd, D3DXVECTOR3* pVertex, D3DXVECTOR3* pVecNormal, D3DXVECTOR3* pVecIntersection, float* pPercentage );
		void        TransformVertices      ( sMesh* pMesh, D3DXMATRIX matWorld );
		void        SetupMappingPlanes     ( void );
		int         GetMajorPlane          ( void );
		
		void SetupCoordinates		( void );
		void SetupWorldSpace		( void );
		bool SetupLights			( void );
		void StoreLightmapForPoly   ( int iPolygon, bool bWasPartShadow );
		bool UpdateObject			( sMesh* pMesh );
		bool SetupObject			( sMesh* pMesh );
		
	public:
		cLightMaps  ( );
		~cLightMaps ( );

		void AddObject   ( sObject* pObject );
		void AddLimb     ( sObject* pObject, int iLimb );
		void AddUniverse ( cUniverse* pUniverse );
		void AddLight    ( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, bool bCastShadow );
		void FlushAll	 ( void );
		void Create      ( int iLMSize, int iLMQuality, LPSTR pPathForLightMaps );
};
//////////////////////////////////////////////////////////////////////////////////

#endif _CLIGHTMAPS_H_
