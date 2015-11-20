// Includes
//#define _CRT_SECURE_NO_DEPRECATE
#include "cLightMaps.h"
#include "CommonC.h"

// Externals
extern LPDIRECT3DDEVICE9 m_pD3D;
extern bool EnsureTextureStageValid ( sMesh* pMesh, int iTextureStage );
extern cUniverse* g_pUniverse;

// Functions
cLightMaps::~cLightMaps ( )
{
	// destructor for light map class

	// free allocs within lumel list
	// 20120401 IanM - Only free the array if it exists.
	if (m_ppdwLightMapLumels)
	{
		for ( int i=0; i<(int)m_dwLightmapPolys; i++)
			SAFE_DELETE ( m_ppdwLightMapLumels [ i ] );
	}

	// delete texture array
	SAFE_DELETE_ARRAY ( m_ppdwLightMapLumels );
	SAFE_DELETE_ARRAY ( m_ppdwLightMapShadow );
}

cLightMaps::cLightMaps ( )
{
	// constructor for light map class

	// clear light and frame list
	m_Lights.clear     ( );
	m_pFrameList.clear ( );

	// set default LM texture tile size
	m_iLMWidth = LMSMALLESTLM;
	m_iLMHeight = LMSMALLESTLM;
	m_pPathForLightMaps = NULL;
}

void cLightMaps::AddObject ( sObject* pObject )
{
	// add an object into the light map pool

	// ensure the object is valid
	if ( !pObject )
		return;

	// store pointer to the object
	sObject* pInstanceObject = pObject;

	// adjust pointer if needed
	if ( pObject->pInstanceOfObject )
		pInstanceObject = pObject->pInstanceOfObject;

	// store position from original object
	pInstanceObject->position = pObject->position;

	// go through all frames
	for ( int iFrame = 0; iFrame < pInstanceObject->iFrameCount; iFrame++ )
	{
		// ignore invalid meshes
		if ( !pInstanceObject->ppFrameList [ iFrame ]->pMesh )
			continue;

		// declare a static object
		sStaticObject object;

		// store values
		object.pFrame     = pInstanceObject->ppFrameList [ iFrame ];
		object.pObject    = pInstanceObject;
		object.pMesh      = pInstanceObject->ppFrameList [ iFrame ]->pMesh;
		object.iID        = -1;
		object.iCollision = -1;
		
		int i = 0;

		// make sure it is unique to list
		for ( i=0; i<(int)m_pFrameList.size(); i++ )
			if ( m_pFrameList [ i ].pMesh==object.pMesh )
				break;

		// it is not unique
		if ( i>=(int)m_pFrameList.size() )
		{
			// send object to list
			m_pFrameList.push_back ( object );
		}
	}
}

void cLightMaps::AddLimb ( sObject* pObject, int iLimb )
{
	// add a limb from an object into the light map pool

	// ensure object pointer is valid
	if ( !pObject )
		return;

	// return if invalid limb
	if ( iLimb > pObject->iFrameCount )
		return;

	// check limb is okay
	if ( pObject->ppFrameList [ iLimb ] )
	{
		// see if we have a mesh
		if ( pObject->ppFrameList [ iLimb ]->pMesh )
		{
			// static object declaration
			sStaticObject object;

			// store values
			object.pFrame     = pObject->ppFrameList [ iLimb ];
			object.pObject    = pObject;
			object.pMesh      = pObject->ppFrameList [ iLimb ]->pMesh;
			object.iID        = -1;
			object.iCollision = -1;
			
			int i = 0;

			// make sure it is unique to list
			for ( i=0; i<(int)m_pFrameList.size(); i++ )
				if ( m_pFrameList [ i ].pMesh==object.pMesh )
					break;

			// it is not unique
			if ( i>=(int)m_pFrameList.size() )
			{
				// send into list
				m_pFrameList.push_back ( object );
			}
		}
	}
}

void cLightMaps::AddUniverse ( cUniverse* pUniverse )
{
	// add a universe into the light map pool
	// see if universe is valid
	g_pGlob->dwInternalFunctionCode=10001;
	if ( !pUniverse )
		return;

	// set up a mesh list
	vector < sMesh* > meshList;

	// get meshes from the universe
	g_pGlob->dwInternalFunctionCode=10002;
	pUniverse->GetMeshList ( &meshList );

	// go through all meshes
	g_pGlob->dwInternalFunctionCode=10003;
	for ( int iMesh = 0; iMesh < (int)meshList.size ( ); iMesh++ )
	{
		// current mesh 
		g_pGlob->dwInternalFunctionCode=10004;
		sMesh* pMesh = meshList [ iMesh ];

		// declare a static object
		g_pGlob->dwInternalFunctionCode=10005;
		sStaticObject object;

		// store values
		object.pFrame     = NULL;
		object.pObject    = NULL;
		object.pMesh      = pMesh;
		object.iID        = -1;
		object.iCollision = -1;

		// mike - 020206 - addition for vs8
		int i = 0;
		
		// make sure it is unique to list
		g_pGlob->dwInternalFunctionCode=10006;

		// mike - 020206 - addition for vs8
		//for ( int i=0; i<(int)m_pFrameList.size(); i++ )
		for ( i=0; i<(int)m_pFrameList.size(); i++ )
			if ( m_pFrameList [ i ].pMesh==meshList [ iMesh ] )
				break;

		// it is not unique
		g_pGlob->dwInternalFunctionCode=10007;
		if ( i<(int)m_pFrameList.size() )
			continue;

		// send to back of list
		g_pGlob->dwInternalFunctionCode=10008;
		m_pFrameList.push_back ( object );
	}

	// completed function successfully
	g_pGlob->dwInternalFunctionCode=10009;
}

void cLightMaps::AddLight ( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, bool bCastShadow )
{
	// add a light into the pool

	// declare light
	sLight light;

	// store properties
	light.vecPosition.x = fX;			// x position
	light.vecPosition.y = fY;			// y position
	light.vecPosition.z = fZ;			// z position
	light.fRadius       = fRadius;		// radius
	light.fRed          = fRed;			// red
	light.fGreen        = fGreen;		// green
	light.fBlue         = fBlue;		// blue
	light.fBrightness   = fBrightness/100.0f;
	light.fRed          *= light.fBrightness;
	light.fGreen        *= light.fBrightness;
	light.fBlue         *= light.fBrightness;
	light.bCastShadow   = bCastShadow;	// should we cast a shadow

	// send light to back of list
	m_Lights.push_back ( light );
}

void cLightMaps::FlushAll ( void )
{
	// clear light list
	m_Lights.clear();
}

void cLightMaps::Create ( int iLMSize, int iLMQuality, LPSTR pPathForLightMaps )
{
	// create light maps for objects in pool
	m_pPathForLightMaps = pPathForLightMaps;

	// ensure LM texture size is ranged
	if ( iLMSize<LMSMALLESTLM ) iLMSize = LMSMALLESTLM;
	if ( iLMSize>LMTEXTILESIZE ) iLMSize = LMTEXTILESIZE;
	if ( iLMQuality>255 ) iLMQuality = 255;
	if ( iLMQuality<0 ) iLMQuality = 0;
	m_iLMQuality = iLMQuality;
	m_iLMWidth = iLMSize;
	m_iLMHeight = iLMSize;

	// start with lightmap zero if saving them
	g_pGlob->dwInternalFunctionCode=10011;
	m_iLightMap=0;

	// first thing, find out what area box each light is in
	if ( g_pUniverse )
	{
		// all lights
		for ( int iLight = 0; iLight < (int)m_Lights.size ( ); iLight++ )
		{
			// get the light, default is no area box
			g_pGlob->dwInternalFunctionCode=10021+iLight;
			sLight* pLight = &m_Lights [ iLight ];
			pLight->iAreaBoxWitin = 0;

			// see if light sits inside an area box
			for ( int i = 0; i < (int)g_pUniverse->m_pAreaList.size ( ); i++ )
			{
				if ( g_pUniverse->CollisionBoundBoxTest (	&pLight->vecPosition,
															&pLight->vecPosition,
															&g_pUniverse->m_pAreaList [ i ]->vecMin,
															&g_pUniverse->m_pAreaList [ i ]->vecMax  ) )
				{
					pLight->iAreaBoxWitin = 1+i;
					break;
				}
			}
		}
	}

	// init consolidation of master textures
	g_pGlob->dwInternalFunctionCode=11000;
	m_pLumelConsolidation = new cLightMapConsolidation;
	if ( m_pLumelConsolidation )
	{
		m_pLumelConsolidation->SetLMSavePath ( m_pPathForLightMaps );
		m_pLumelConsolidation->Init ( m_iLMWidth, m_iLMHeight, &m_iLightMap );
	}

	//
	// GO through ALL OBJECTS in mesh lighting list
	//
	bool bWeHadToBackTrak = false;
	for ( int iObject = 0; iObject < (int)m_pFrameList.size ( ); iObject++ )
	{
		// get current work mesh
		g_pGlob->dwInternalFunctionCode=11001+iObject;
		sMesh* pMesh = m_pFrameList [ iObject ].pMesh;

		// skip completely if no primitives in mesh (tahanks to HSR::clip)
		if ( pMesh->iDrawPrimitives==0 ) continue;
		
		// for all lights that can light mesh, create frustrums-of-light or omni-light (per light)
		m_pLocalLightList.clear();
		m_pLocalLightFrustrumList.clear();
		m_pLocalLightOccluderList.clear();
		if ( g_pUniverse )
		{
			for ( int iLight = 0; iLight < (int)m_Lights.size ( ); iLight++ )
			{
				// get the light
				sLight* pLight = &m_Lights [ iLight ];

				// for each light, frustrums may be cast
				g_pUniverse->m_pFrustrumList.clear();

				// can this object SEE the light in question (directly via omni)
				bool bCanSeeOmni=false;
				D3DXVECTOR3 vecSize = (pMesh->Collision.vecMax-pMesh->Collision.vecMin)/2.0f;
				if ( g_pUniverse->CanAreaPointSeeBox ( pLight->iAreaBoxWitin, &pLight->vecPosition, &pMesh->Collision.vecCentre, &vecSize ) )
					bCanSeeOmni=true;

				// if deemed visible (via omni or via frustrum illumination ie shadows)
				if ( bCanSeeOmni || g_pUniverse->m_pFrustrumList.size()>0 )
				{
					// calculate nearest point of mesh
					D3DXVECTOR3 vecDistance = pLight->vecPosition - pMesh->Collision.vecCentre;
					float fDistance = D3DXVec3Length ( &vecDistance );
					fDistance = fabs(fDistance) - pMesh->Collision.fRadius;

					// make sure the mesh is in range of light
					if ( fDistance <= pLight->fRadius )
					{
						// record light that illuminates this mesh in some way
						m_pLocalLightList.push_back ( pLight );
						for ( int iF=0; iF<(int)g_pUniverse->m_pFrustrumList.size(); iF++ )
							m_pLocalLightFrustrumList.push_back ( g_pUniverse->m_pFrustrumList [ iF ] );
						m_pLocalLightFrustrumList.push_back ( NULL );

						// references copied, over so do not delete just clear ptrs
						g_pUniverse->m_pFrustrumList.clear();

						// leeadd - 240905 - old system ( &pLight->vecPosition, &pMesh->Collision.vecCentre )
						// g_pUniverse->ShortlistMeshesWithinBox ( &pLight->vecPosition, &pMesh->Collision.vecCentre );
						// only accounted for meshes within box created from light point to center of target surface mesh (ie floor)
						// where we need the box to be AT LEAST the box containing the light and the ENTIRE target mesh
						D3DXVECTOR3 vecLightBoxMin = pLight->vecPosition;
						D3DXVECTOR3 vecLightBoxMax = pLight->vecPosition;
						if ( vecLightBoxMin.x > pMesh->Collision.vecMin.x ) vecLightBoxMin.x = pMesh->Collision.vecMin.x;
						if ( vecLightBoxMin.y > pMesh->Collision.vecMin.y ) vecLightBoxMin.y = pMesh->Collision.vecMin.y;
						if ( vecLightBoxMin.z > pMesh->Collision.vecMin.z ) vecLightBoxMin.z = pMesh->Collision.vecMin.z;
						if ( vecLightBoxMax.x < pMesh->Collision.vecMax.x ) vecLightBoxMax.x = pMesh->Collision.vecMax.x;
						if ( vecLightBoxMax.y < pMesh->Collision.vecMax.y ) vecLightBoxMax.y = pMesh->Collision.vecMax.y;
						if ( vecLightBoxMax.z < pMesh->Collision.vecMax.z ) vecLightBoxMax.z = pMesh->Collision.vecMax.z;

						// add all meshes that interfere with this light and mesh
						g_pUniverse->ShortlistMeshesWithinBox ( &vecLightBoxMin, &vecLightBoxMax );
						for ( int iO=0; iO<(int)g_pUniverse->m_pMeshShortList.size(); iO++ )
						{
							// add only those that are occluders (cast shadows)
							sMesh* pOccluderMesh = g_pUniverse->m_pMeshShortList [ iO ];
							if ( pOccluderMesh->iCastShadowIfStatic==3 )
								m_pLocalLightOccluderList.push_back ( pOccluderMesh );
						}
						m_pLocalLightOccluderList.push_back ( NULL );
					}
				}
			}
		}

		// alter mesh to suit calculations and create array for each polygon
		if ( !SetupObject ( pMesh ) ) continue;

		// if mesh has not been transformed (which universe already does), do so now
		if ( !pMesh->bVertexTransform )
		{
			CalcObjectWorld ( m_pFrameList [ iObject ].pObject );
			CalculateAbsoluteWorldMatrix ( m_pFrameList [ iObject ].pObject, m_pFrameList [ iObject ].pFrame, m_pFrameList [ iObject ].pFrame->pMesh );
			TransformVertices ( pMesh, m_pFrameList [ iObject ].pFrame->matAbsoluteWorld );
		}

		// go through all polygons in mesh, together with local light list (with frustrums recorded for shadow calculation)
		float* pVertex = (float*)pMesh->pVertexData;
		for ( int iPolygon = 0, iOffset = 0; iPolygon < (int)pMesh->dwVertexCount / 3; iPolygon++ )
		{
			// extract triangle
			m_pVertices [ 0 ] = ( ( sLightMapVertex* ) pMesh->pVertexData + iOffset++ );
			m_pVertices [ 1 ] = ( ( sLightMapVertex* ) pMesh->pVertexData + iOffset++ );
			m_pVertices [ 2 ] = ( ( sLightMapVertex* ) pMesh->pVertexData + iOffset++ );

			// light triangle
			SetupMappingPlanes		( );				// find which major plane triangle most aligns to
			SetupCoordinates		( );				// set up 2D texture coordinates
			SetupWorldSpace			( );				// convert to world space
			bool bWasPartShadow = SetupLights ( );		// light triangle
			StoreLightmapForPoly	( iPolygon, bWasPartShadow );// store lightmap data
		}

		// must free frustrum allocations when done
		for ( int iF=0; iF<(int)m_pLocalLightFrustrumList.size(); iF++ )
			SAFE_DELETE ( m_pLocalLightFrustrumList [ iF ] );
		m_pLocalLightFrustrumList.clear();
		m_pLocalLightOccluderList.clear();

		// transform vertices back for regular use before we leave
		if ( !pMesh->bVertexTransform )
		{
			D3DXMatrixInverse ( &m_pFrameList [ iObject ].pFrame->matAbsoluteWorld, NULL, &m_pFrameList [ iObject ].pFrame->matAbsoluteWorld );
			TransformVertices ( pMesh, m_pFrameList [ iObject ].pFrame->matAbsoluteWorld );
		}

		// update object (even if no lighting UV data - obj needs LM texture assignment)
		if ( UpdateObject ( pMesh )==true )
		{
			// must redo this object as lightmap ran out of plate space
			if ( bWeHadToBackTrak==false )
			{
				// back trak
				bWeHadToBackTrak=true;
				iObject--;
			}
			else
			{
				// to back trak twice means the LM plate too small for mesj, so skip
				iObject=iObject;
			}
		}
		else
			bWeHadToBackTrak=false;

		// free allocs within lumel list
		for ( int i=0; i<(int)m_dwLightmapPolys; i++)
			SAFE_DELETE ( m_ppdwLightMapLumels [ i ] );

		// free usages
		SAFE_DELETE_ARRAY ( m_ppdwLightMapLumels );
		SAFE_DELETE_ARRAY ( m_ppdwLightMapShadow );
	}

	// complete consolidations for this process (close last master texture)
	g_pGlob->dwInternalFunctionCode=19001;
	if ( m_pLumelConsolidation )
	{
		m_pLumelConsolidation->Complete(0);
		SAFE_DELETE(m_pLumelConsolidation);
	}

	// clear frame list as we're finished
	g_pGlob->dwInternalFunctionCode=19002;
	m_pFrameList.clear ( );
}

bool cLightMaps::SetupObject ( sMesh* pMesh )
{
	// check mesh is valid
	if ( !pMesh )
		return false;

	// set up mesh properties for regular geometry (all FX should use this format)
	ConvertLocalMeshToVertsOnly ( pMesh );												// convert to vertices only
	ConvertLocalMeshToFVF       ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 );	// our new format
	GenerateNormals             ( pMesh );												// generate normals

	// clear out colour array
	// 20120326 IanM - Correctly initialise the array
	memset ( m_dwLumelColour, 0, sizeof ( m_dwLumelColour ) );
	//memset ( &m_dwLumelColour, 0, sizeof ( m_dwLumelColour ) );

	// allocate lumel array list
	m_dwLightmapPolys = pMesh->dwVertexCount / 3;
	m_ppdwLightMapLumels = new DWORD* [ m_dwLightmapPolys ];
	if ( !m_ppdwLightMapLumels )
		return false;

	// allocate shadow array list
	m_ppdwLightMapShadow = new bool [ m_dwLightmapPolys ];
	if ( !m_ppdwLightMapShadow )
		return false;
	
	// finished
	return true;
}

void cLightMaps::SetupMappingPlanes ( void )
{
	// take the first vertex and store it as our point on the plane
	m_vecPointOnPlane.x = m_pVertices [ 0 ]->x;
	m_vecPointOnPlane.y = m_pVertices [ 0 ]->y;
	m_vecPointOnPlane.z = m_pVertices [ 0 ]->z;

	// calculate the normal of the triangle
	m_vecNormal = GetNormal ( );

	// now we need to work out which major plane the triangle lies
	// on - this could be YZ, XZ or XY, this is done so we can work
	// out the texture coordinates
	m_iFlag = GetMajorPlane ( );

	// set up coordinates based on plane alignment
	switch ( m_iFlag )
	{
		case 1:
		{
			// closest to YZ
			
			// set lu and lv coordinates to YZ coordinates
			m_pVertices [ 0 ]->lu = m_pVertices [ 0 ]->y;
			m_pVertices [ 0 ]->lv = m_pVertices [ 0 ]->z;
			m_pVertices [ 1 ]->lu = m_pVertices [ 1 ]->y;
			m_pVertices [ 1 ]->lv = m_pVertices [ 1 ]->z;
			m_pVertices [ 2 ]->lu = m_pVertices [ 2 ]->y;
			m_pVertices [ 2 ]->lv = m_pVertices [ 2 ]->z;
		}
		break;

		case 2:
		{
			// closest to XZ
			
			// set lu and lv coordinates to XZ coordinates
			m_pVertices [ 0 ]->lu = m_pVertices [ 0 ]->x;
			m_pVertices [ 0 ]->lv = m_pVertices [ 0 ]->z;
			m_pVertices [ 1 ]->lu = m_pVertices [ 1 ]->x;
			m_pVertices [ 1 ]->lv = m_pVertices [ 1 ]->z;
			m_pVertices [ 2 ]->lu = m_pVertices [ 2 ]->x;
			m_pVertices [ 2 ]->lv = m_pVertices [ 2 ]->z;
		}
		break;

		case 3:
		{
			// closest to XY
			
			// set lu and lv coordinates to XY coordinates
			m_pVertices [ 0 ]->lu = m_pVertices [ 0 ]->x;
			m_pVertices [ 0 ]->lv = m_pVertices [ 0 ]->y;
			m_pVertices [ 1 ]->lu = m_pVertices [ 1 ]->x;
			m_pVertices [ 1 ]->lv = m_pVertices [ 1 ]->y;
			m_pVertices [ 2 ]->lu = m_pVertices [ 2 ]->x;
			m_pVertices [ 2 ]->lv = m_pVertices [ 2 ]->y;
		}
		break;
	}
}

int cLightMaps::GetMajorPlane ( void )
{
	// determine which of the 3 major planes the triangle lies on, this
	// is done by checking the normal of the triangle

	// YZ
	if ( fabs ( m_vecNormal.x ) > fabs ( m_vecNormal.y ) && fabs ( m_vecNormal.x ) > fabs ( m_vecNormal.z ) )
		return 1;

	// XZ
	if ( fabs ( m_vecNormal.y ) > fabs ( m_vecNormal.x ) && fabs ( m_vecNormal.y ) > fabs ( m_vecNormal.z ) )
		return 2;

	// XY
	return 3;
}

void cLightMaps::SetupCoordinates ( void )
{
	DWORD dwTimeBase = timeGetTime();

	// convert to 2D texture space
	m_uvMinU = m_pVertices [ 0 ]->lu;
	m_uvMinV = m_pVertices [ 0 ]->lv;
	m_uvMaxU = m_pVertices [ 0 ]->lu;
	m_uvMaxV = m_pVertices [ 0 ]->lv;

	//find the min and max extents of the triangle
	D3DXVECTOR2 uvMin = D3DXVECTOR2 ( m_pVertices [ 0 ]->lu, m_pVertices [ 0 ]->lv );	// first min vector
	D3DXVECTOR2 uvMax = D3DXVECTOR2 ( m_pVertices [ 0 ]->lu, m_pVertices [ 0 ]->lv );	// first max vector
	
	int iVertex = 0;

	// go through each vertex
	for ( iVertex = 0; iVertex < 3; iVertex++ )
	{
		// is lu less than min
		if ( m_pVertices [ iVertex ]->lu < uvMin.x )
		   	uvMin.x = m_pVertices [ iVertex ]->lu;

		// is lv less than min
		if ( m_pVertices [ iVertex ]->lv < uvMin.y )
		   	uvMin.y = m_pVertices [ iVertex ]->lv;

		// is lu greater than max
		if ( m_pVertices [ iVertex ]->lu > uvMax.x )
		   	uvMax.x = m_pVertices [ iVertex ]->lu;

		// is lv greater than max
		if ( m_pVertices [ iVertex ]->lv > uvMax.y )
		   	uvMax.y = m_pVertices [ iVertex ]->lv;
	}

	// store final extents
	m_uvMinU = uvMin.x;
	m_uvMinV = uvMin.y;
	m_uvMaxU = uvMax.x;
	m_uvMaxV = uvMax.y;
	
	// find difference between max and min extents
	m_uvDeltaU = m_uvMaxU - m_uvMinU;
	m_uvDeltaV = m_uvMaxV - m_uvMinV;
	
	// go through each vertex
	for ( iVertex = 0; iVertex < 3; iVertex++ )
	{
		// set new lu, lv coordinates
		m_pVertices [ iVertex ]->lu -= m_uvMinU;	// minus min u
		m_pVertices [ iVertex ]->lv -= m_uvMinV;	// minus min v
		m_pVertices [ iVertex ]->lu /= m_uvDeltaU;	// divide by u delta
		m_pVertices [ iVertex ]->lv /= m_uvDeltaV;	// divide by v delta
	}

	DWORD dwTimeEnd = timeGetTime()-dwTimeBase;
	DWORD dwTimeTaken = dwTimeEnd;
}

void cLightMaps::SetupWorldSpace ( void )
{
	// calculate the world space position for each lumel
	m_fDistance = - ( m_vecNormal.x * m_vecPointOnPlane.x + m_vecNormal.y * m_vecPointOnPlane.y + m_vecNormal.z * m_vecPointOnPlane.z );

	// find out which plane the triangle most aligns to, we need to find 2 edges
	// we can do this by taking the minimum and maximum uv coordinates and project
	// them back to the polygons plane
	switch ( m_iFlag )
	{
		case 1:
		{
			// YZ plane
			m_uvVector.x = - ( m_vecNormal.y * m_uvMinU + m_vecNormal.z * m_uvMinV + m_fDistance ) / m_vecNormal.x;
			m_uvVector.y = m_uvMinU;
			m_uvVector.z = m_uvMinV;

			m_vector1.x = - ( m_vecNormal.y * m_uvMaxU + m_vecNormal.z * m_uvMinV + m_fDistance ) / m_vecNormal.x;
			m_vector1.y = m_uvMaxU;
			m_vector1.z = m_uvMinV;

			m_vector2.x = - ( m_vecNormal.y * m_uvMinU + m_vecNormal.z * m_uvMaxV + m_fDistance ) / m_vecNormal.x;
			m_vector2.y = m_uvMinU;
			m_vector2.z = m_uvMaxV;
		}
		break;

		case 2:
		{
			// XZ plane
			m_uvVector.x = m_uvMinU;
			m_uvVector.y = - ( m_vecNormal.x * m_uvMinU + m_vecNormal.z * m_uvMinV + m_fDistance ) / m_vecNormal.y;
			m_uvVector.z = m_uvMinV;

			m_vector1.x = m_uvMaxU;
			m_vector1.y = - ( m_vecNormal.x * m_uvMaxU + m_vecNormal.z * m_uvMinV + m_fDistance ) / m_vecNormal.y;
			m_vector1.z = m_uvMinV;

			m_vector2.x = m_uvMinU;
			m_vector2.y = - ( m_vecNormal.x * m_uvMinU + m_vecNormal.z * m_uvMaxV + m_fDistance ) / m_vecNormal.y;
			m_vector2.z = m_uvMaxV;
		}
		break;

		case 3:
		{
			// XY plane
			m_uvVector.x = m_uvMinU;
			m_uvVector.y = m_uvMinV;
			m_uvVector.z = - ( m_vecNormal.x * m_uvMinU + m_vecNormal.y * m_uvMinV + m_fDistance ) / m_vecNormal.z;

			m_vector1.x = m_uvMaxU;
			m_vector1.y = m_uvMinV;
			m_vector1.z = - ( m_vecNormal.x * m_uvMaxU + m_vecNormal.y * m_uvMinV + m_fDistance ) / m_vecNormal.z;

			m_vector2.x = m_uvMinU;
			m_vector2.y = m_uvMaxV;
			m_vector2.z = - ( m_vecNormal.x * m_uvMinU + m_vecNormal.y * m_uvMaxV + m_fDistance ) / m_vecNormal.z;
		}
		break;
	}

	// create 2 edge vectors, we can find the lumel positions in world space by
	// interpolating along these edges using the width and height of the lightmap
	m_vecEdge1.x = m_vector1.x - m_uvVector.x;
	m_vecEdge1.y = m_vector1.y - m_uvVector.y;
	m_vecEdge1.z = m_vector1.z - m_uvVector.z;
	m_vecEdge2.x = m_vector2.x - m_uvVector.x;
	m_vecEdge2.y = m_vector2.y - m_uvVector.y;
	m_vecEdge2.z = m_vector2.z - m_uvVector.z;
}

bool cLightMaps::SetupLights ( void )
{
	// detect if poly was crossed by a shadow (half and half)
	bool bPartShadowed=false;

	// pre-cast width and height of LM tile
	float fLMWidth = ( float ) m_iLMWidth;
	float fLMHeight = ( float ) m_iLMHeight;

	// create vectors for polygon vertex positions
	D3DXVECTOR3 vecPolyVec [ 3 ];
	vecPolyVec [ 0 ] = D3DXVECTOR3 ( m_pVertices [ 0 ]->x, m_pVertices [ 0 ]->y, m_pVertices [ 0 ]->z );
	vecPolyVec [ 1 ] = D3DXVECTOR3 ( m_pVertices [ 1 ]->x, m_pVertices [ 1 ]->y, m_pVertices [ 1 ]->z );
	vecPolyVec [ 2 ] = D3DXVECTOR3 ( m_pVertices [ 2 ]->x, m_pVertices [ 2 ]->y, m_pVertices [ 2 ]->z );

	// set red, green and blue to black
	float m_fCombinedRed[LMTEXTILESIZE][LMTEXTILESIZE];
	float m_fCombinedGreen[LMTEXTILESIZE][LMTEXTILESIZE];
	float m_fCombinedBlue[LMTEXTILESIZE][LMTEXTILESIZE];

	// clear color pots
	memset(m_fCombinedRed, 0, sizeof(m_fCombinedRed) );
	memset(m_fCombinedGreen, 0, sizeof(m_fCombinedGreen) );
	memset(m_fCombinedBlue, 0, sizeof(m_fCombinedBlue) );

	// work out light values based on all lights in list (local list prepared higher up)
	int iFrustrumStart=0, iFrustrumFinish=0;
	int iOccluderStart=0, iOccluderFinish=0;
	for ( int iLocalLight=0; iLocalLight<(int)m_pLocalLightList.size(); iLocalLight++ )
	{
		// get the light
		sLight* pLight = m_pLocalLightList [ iLocalLight ];

		// for each light, build small list of frustrums
		iFrustrumFinish = iFrustrumStart;
		while ( m_pLocalLightFrustrumList [ iFrustrumFinish ]!=NULL )
			iFrustrumFinish++;

		// for each light, build small list of occulders
		iOccluderFinish = iOccluderStart;
		while ( m_pLocalLightOccluderList [ iOccluderFinish ]!=NULL )
			iOccluderFinish++;

		// a quick reject is that if entire polygon is outside all frustrums
		if ( m_pLocalLightFrustrumList.size() > 0 )
		{
			if ( m_pLocalLightFrustrumList [ iFrustrumStart ] )
			{
				// go through all frustrums for this light
				bool bSomePartOfPolyLit = false;
				for ( int iF=iFrustrumStart; iF<iFrustrumFinish; iF++ )
				{
					// if valid frustrum for this light
					if ( m_pLocalLightFrustrumList [ iF ] )
					{
						// illuminate with this light if polygon in frustrum
						if ( CheckPolygon ( m_pLocalLightFrustrumList [ iF ], &vecPolyVec [ 0 ], &vecPolyVec [ 1 ], &vecPolyVec [ 2 ] ) ) bSomePartOfPolyLit = true;
					}
				}

				// only if totally unlit by light, skip light for this poly
				if ( bSomePartOfPolyLit==false )
				{
					iFrustrumStart = iFrustrumFinish + 1;
					iOccluderStart = iOccluderFinish + 1;
					continue;
				}
			}
		}

		// calculate shortest distance vertex of poly
		D3DXVECTOR3 vecDistance = pLight->vecPosition - vecPolyVec[0];
		float fDistance = D3DXVec3Length ( &vecDistance );
		vecDistance = pLight->vecPosition - vecPolyVec[1];
		float fTry = D3DXVec3Length ( &vecDistance );
		if ( fTry < fDistance ) fDistance = fTry;
		vecDistance = pLight->vecPosition - vecPolyVec[2];
		fTry = D3DXVec3Length ( &vecDistance );
		if ( fTry < fDistance ) fDistance = fTry;

		// make sure the polygon is in range of light
		if ( fDistance <= pLight->fRadius )
		{
			// make sure the polygon is facing the light
			if ( ClassifyPoint ( pLight->vecPosition, m_vecPointOnPlane, m_vecNormal ) == 1 )
			{
				// compute extra to create texture filter data
				float fAreaWithinU = fLMWidth-2.0f;
				float fAreaWithinV = fLMHeight-2.0f;
				float fHalfOfU = 0.5f/fAreaWithinU;
				float fHalfOfV = 0.5f/fAreaWithinV;

				// for each light, go through texels of polygon
				for ( int iX = 0; iX < m_iLMWidth; iX++ )
				{
					for ( int iY = 0; iY < m_iLMHeight; iY++ )
					{
						// get the u and v factors (taking texture filter into acount)
						float ufactor = ( ( float )(iX-1) / fAreaWithinU );
						float vfactor = ( ( float )(iY-1) / fAreaWithinV );
						m_vecNewEdge1 = m_vecEdge1 * (ufactor);
						m_vecNewEdge2 = m_vecEdge2 * (vfactor);

						// now we find the position to move to (texelray)
						m_vecLumels [ iX ] [ iY ].x = m_uvVector.x + m_vecNewEdge2.x + m_vecNewEdge1.x;
						m_vecLumels [ iX ] [ iY ].y = m_uvVector.y + m_vecNewEdge2.y + m_vecNewEdge1.y;
						m_vecLumels [ iX ] [ iY ].z = m_uvVector.z + m_vecNewEdge2.z + m_vecNewEdge1.z;

						// the position not including the filter (for frustrum check)
						D3DXVECTOR3 vecFrusLumels;
						float uffactor = ( ( float )(iX) / (fAreaWithinU+2.0f) );
						float vffactor = ( ( float )(iY) / (fAreaWithinV+2.0f) );
						D3DXVECTOR3 vecNewFEdge1 = m_vecEdge1 * (uffactor);
						D3DXVECTOR3 vecNewFEdge2 = m_vecEdge2 * (vffactor);
						vecFrusLumels.x = m_uvVector.x + vecNewFEdge2.x + vecNewFEdge1.x;
						vecFrusLumels.y = m_uvVector.y + vecNewFEdge2.y + vecNewFEdge1.y;
						vecFrusLumels.z = m_uvVector.z + vecNewFEdge2.z + vecNewFEdge1.z;

						// imrpoves seamlessness but portal divisions shown UP!
						// vecFrusLumels = m_vecLumels [ iX ] [ iY ];

						// before light calc, does light ray get blocked?
						bool bBlocked = false;

						// use light frustrum to calculate shadows
						if ( pLight->bCastShadow )
						{
							// omni lit unless frustrum is available
							if ( m_pLocalLightFrustrumList [ iFrustrumStart ] )
							{
								// go through all frustrums for this light
								bBlocked = true;
								for ( int iF=iFrustrumStart; iF<iFrustrumFinish; iF++ )
									if ( m_pLocalLightFrustrumList [ iF ] )
										if ( CheckPoint ( m_pLocalLightFrustrumList [ iF ], &vecFrusLumels ) )
											bBlocked = false;

								// frustrum blocked light here
								if ( bBlocked ) bPartShadowed = true;
							}

							// if any occluders, possible secondary block
							if ( !bBlocked )//0)//
							{
								// if some occluders
								if ( m_pLocalLightOccluderList.size() > 0 )
								{
									// if current light as at least one occluder
									if ( m_pLocalLightOccluderList [ iOccluderStart ] )
									{
										// go through all occluders for this light->mesh pairing
										for ( int iO=iOccluderStart; iO<iOccluderFinish; iO++ )
										{
											// if occluder valid, check ray against it
											if ( m_pLocalLightOccluderList [ iO ] )
												if ( g_pUniverse->CollisionVeryQuickRayCast ( m_pLocalLightOccluderList [ iO ], &pLight->vecPosition, &m_vecLumels [ iX ] [ iY ] )==true )
													bBlocked = true;
										}
									}
								}

								// occluder blocked light here
								if ( bBlocked ) bPartShadowed = true;
							}
						}

						// only if light ray unblocked
						if ( !bBlocked )
						{
							// work out the light vector direction
							m_vecLightDirection.x = pLight->vecPosition.x - m_vecLumels [ iX ] [ iY ].x;
							m_vecLightDirection.y = pLight->vecPosition.y - m_vecLumels [ iX ] [ iY ].y;
							m_vecLightDirection.z = pLight->vecPosition.z - m_vecLumels [ iX ] [ iY ].z;

							// now get the distance to from the lumel to the light
							m_fLightDistance = D3DXVec3Length ( &m_vecLightDirection );

							// normalize the light direction
							D3DXVec3Normalize ( &m_vecLightDirection, &m_vecLightDirection );

							// get the angle from the direction
							m_fCosAngle = D3DXVec3Dot ( &m_vecNormal, &m_vecLightDirection );

							// is the distance less than the radius
							if ( m_fLightDistance <= pLight->fRadius && m_fLightDistance > 0.0f )
							{
								// get the intensity based on the light distance
								m_fIntensity = 1.0f - ( m_fLightDistance / pLight->fRadius );

								// slightly affect intensity (a qaurter influence)
								m_fIntensity += ( m_fIntensity * m_fCosAngle );

								// reduce components by quality (for smaller LM storage)
								float fRed = pLight->fRed		* m_fIntensity;
								float fGreen = pLight->fGreen	* m_fIntensity;
								float fBlue = pLight->fBlue		* m_fIntensity;
								if ( m_iLMQuality >0 )
								{
									fRed = ((int)fRed/m_iLMQuality)*m_iLMQuality;
									fGreen = ((int)fGreen/m_iLMQuality)*m_iLMQuality;
									fBlue = ((int)fBlue/m_iLMQuality)*m_iLMQuality;
								}
								
								// now we have our red, green and blue values
								m_fCombinedRed   [ iY ] [ iX ] += fRed;
								m_fCombinedGreen [ iY ] [ iX ] += fGreen;
								m_fCombinedBlue  [ iY ] [ iX ] += fBlue;
							}
						}
					}
				}
			}
		}

		// get ready for next frustrum set of light to follow (if any)
		iFrustrumStart = iFrustrumFinish + 1;
		iOccluderStart = iOccluderFinish + 1;
	}

	// after all lights considered for polygon, 
	// convert colormpots to lumel array
	for ( int iX = 0; iX < m_iLMWidth; iX++ )
	{
		for ( int iY = 0; iY < m_iLMHeight; iY++ )
		{
			// check red, green and blue don't go over the boundaries
			if ( m_fCombinedRed   [ iY ] [ iX ] > 255.0f ) m_fCombinedRed   [ iY ] [ iX ] = 255.0f;
			if ( m_fCombinedGreen [ iY ] [ iX ] > 255.0f ) m_fCombinedGreen [ iY ] [ iX ] = 255.0f;
			if ( m_fCombinedBlue  [ iY ] [ iX ] > 255.0f ) m_fCombinedBlue  [ iY ] [ iX ] = 255.0f;

			// now create the DWORD that gets written to the texture
			*((DWORD*)m_dwLumelColour + (iX) + (iY*m_iLMWidth)) = D3DCOLOR_ARGB ( 255, (int)m_fCombinedRed [ iY ] [ iX ], (int)m_fCombinedGreen [ iY ] [ iX ], (int)m_fCombinedBlue [ iY ] [ iX ] );
		}
	}

	// if light on poly had a shadow in it
	return bPartShadowed;
}

void cLightMaps::StoreLightmapForPoly ( int iPolygon, bool bWasPartShadow )
{
	// create an array to store lunel color and assign to list
	DWORD* pdwLumelForPoly = new DWORD [ m_iLMWidth*m_iLMHeight ];
	DWORD dwSize = sizeof(DWORD)*m_iLMWidth*m_iLMHeight;
	memcpy ( pdwLumelForPoly, m_dwLumelColour, dwSize );
	m_ppdwLightMapLumels [ iPolygon ] = pdwLumelForPoly;
	m_ppdwLightMapShadow [ iPolygon ] = bWasPartShadow;
}

bool cLightMaps::UpdateObject ( sMesh* pMesh )
{
	// record if lightmap could not fit on plate (need to redo mesh)
	bool bRedoObjectWithNewLM = false;

	// consolidate the textures (using the final lumeldata)
	if ( m_pLumelConsolidation )
	{
		if ( !m_pLumelConsolidation->Run ( pMesh, m_ppdwLightMapLumels, m_ppdwLightMapShadow, pMesh->dwVertexCount / 3 ) )
		{
			// mid-run, ran out of LM plate, consolidate and start a new one
			m_pLumelConsolidation->Complete(0);
			m_pLumelConsolidation->Init( m_iLMWidth, m_iLMHeight, &m_iLightMap );

			// trigger repeat process of this object (or we end up with one object trying to use two lightmaps)
			bRedoObjectWithNewLM = true;
		}
	}

	// only if object ready for final process
	if ( bRedoObjectWithNewLM==false )
	{
		// make sure the stage is valid
		EnsureTextureStageValid ( pMesh, 1 );

		// record base texture (as we will be transfering it to stage 1)
		sTexture pTexStore = pMesh->pTextures [ 0 ];

		// setup lightmap as FIRST stage (diffuse+lightmap)
		pMesh->pTextures [ 0 ].dwStage			= 0;
		pMesh->pTextures [ 0 ].iImageID			= -1;
		pMesh->pTextures [ 0 ].pTexturesRef		= m_pLumelConsolidation->GetTexture ( );
		strcpy ( pMesh->pTextures [ 0 ].pName   , m_pLumelConsolidation->GetName() );
		pMesh->pTextures [ 0 ].dwBlendMode		= D3DTOP_ADD;
		pMesh->pTextures [ 0 ].dwBlendArg1		= D3DTA_TEXTURE;
		pMesh->pTextures [ 0 ].dwBlendArg2		= D3DTA_DIFFUSE;
		pMesh->pTextures [ 0 ].dwAddressU		= D3DTADDRESS_CLAMP;
		pMesh->pTextures [ 0 ].dwAddressV		= D3DTADDRESS_CLAMP;
		pMesh->pTextures [ 0 ].dwMagState		= D3DTEXF_LINEAR;
		pMesh->pTextures [ 0 ].dwMinState		= D3DTEXF_LINEAR;
		pMesh->pTextures [ 0 ].dwMipState		= D3DTEXF_POINT;//D3DTEXF_NONE;

		// copy base texture to SECOND stage (current*basetexture)
		pMesh->pTextures [ 1 ]					= pTexStore;
		pMesh->pTextures [ 1 ].dwStage			= 1;
		pMesh->pTextures [ 1 ].dwBlendMode		= D3DTOP_MODULATE;
		pMesh->pTextures [ 1 ].dwBlendArg1		= D3DTA_TEXTURE;
		pMesh->pTextures [ 1 ].dwBlendArg2		= D3DTA_CURRENT;

		// UV data stored in UV stage zero
		pMesh->pTextures [ 0 ].dwTexCoordMode	= 11;
		pMesh->pTextures [ 1 ].dwTexCoordMode	= 10;

		// signal vb refresh
		pMesh->bVBRefreshRequired = true;
		g_vRefreshMeshList.push_back ( pMesh );

		// add object mesh to buffers
		m_ObjectManager.AddObjectMeshToBuffers ( pMesh, false );
	}

	// return redo flag
	return bRedoObjectWithNewLM;
}

void cLightMaps::TransformVertices ( sMesh* pMesh, D3DXMATRIX matWorld )
{
	// transform vertices of a mesh by a given matrix

	// mesh must be valid
	if ( !pMesh )
		return;

	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// now we need to transform the vertices for each object
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		// get a pointer to the vertex data
		BYTE* pVertex = pMesh->pVertexData;

		// see if vertex pointer is valid
		if ( !pVertex )
			return;

		// convert the position into a vector so we can perform the transform
		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											  );

		// transform the position by world matrix
		D3DXVec3TransformCoord ( &vecPosition, &vecPosition, &matWorld );

		// update original data with transformed coordinates
		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
		*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
		*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
	}
}

D3DXVECTOR3 cLightMaps::GetNormal ( void )
{
	D3DXVECTOR3 vecNormal = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );

	// calculate the normal
	D3DXVECTOR3 vec0     = D3DXVECTOR3 ( m_pVertices [ 0 ]->x, m_pVertices [ 0 ]->y, m_pVertices [ 0 ]->z );
	D3DXVECTOR3 vec1     = D3DXVECTOR3 ( m_pVertices [ 1 ]->x, m_pVertices [ 1 ]->y, m_pVertices [ 1 ]->z );
	D3DXVECTOR3 vec2     = D3DXVECTOR3 ( m_pVertices [ 2 ]->x, m_pVertices [ 2 ]->y, m_pVertices [ 2 ]->z );
	D3DXVECTOR3 vecEdgeA = vec1 - vec0;
	D3DXVECTOR3 vecEdgeB = vec2 - vec0;

	// perform cross product on the edges
	D3DXVec3Cross ( &vecNormal, &vecEdgeA, &vecEdgeB );

	// normalize the final result
	D3DXVec3Normalize ( &vecNormal, &vecNormal );
	
	return vecNormal;
}

bool cLightMaps::GetIntersect ( 
								D3DXVECTOR3* vecLineStart, D3DXVECTOR3* vecLineEnd, D3DXVECTOR3* pVertex,
								D3DXVECTOR3* pVecNormal, D3DXVECTOR3* pVecIntersection, float* pPercentage
							  )
{
	// return the intersection point between a ray and a plane

	// check the parameters passed in are valid
	SAFE_MEMORY ( vecLineStart     );
	SAFE_MEMORY ( vecLineEnd       );
	SAFE_MEMORY ( pVertex          );
	SAFE_MEMORY ( pVecNormal       );
	SAFE_MEMORY ( pVecIntersection );
	SAFE_MEMORY ( pPercentage      );
	
	// local variables
	D3DXVECTOR3 vecDirection;
	D3DXVECTOR3 vecL1;
	float	    fLineLength;
	float		fDistanceFromPlane;

	// get the lines direction vector
	vecDirection.x = vecLineEnd->x - vecLineStart->x;
	vecDirection.y = vecLineEnd->y - vecLineStart->y;
	vecDirection.z = vecLineEnd->z - vecLineStart->z;

	// get the line length
	fLineLength = D3DXVec3Dot ( &vecDirection, pVecNormal );

	// see if the line is parallel to the plane
	if ( fabsf ( fLineLength ) < 0.01f )
		return false; 

	vecL1.x = pVertex->x - vecLineStart->x;
	vecL1.y = pVertex->y - vecLineStart->y;
	vecL1.z = pVertex->z - vecLineStart->z;

	// get the distance from the plane
	fDistanceFromPlane = D3DXVec3Dot ( &vecL1, pVecNormal );

	// how far from linestart, intersection is as a percentage of 0 to 1 
	*pPercentage = fDistanceFromPlane / fLineLength; 

	// the plane is behind the start of the line or the line does not reach the plane
	if ( *pPercentage < 0.0f || *pPercentage > 1.0f )
		return false;

	// add the percentage of the line to line start
	pVecIntersection->x = vecLineStart->x + vecDirection.x * ( *pPercentage );
	pVecIntersection->y = vecLineStart->y + vecDirection.y * ( *pPercentage );
	pVecIntersection->z = vecLineStart->z + vecDirection.z * ( *pPercentage );

	// there was an intersection
	return true;
}

bool cLightMaps::AreCoPlanar ( sPlane &a, sPlane &b, float fCosEps, float fDistEps )
{
	// are the points co planer

	float fCos = D3DXVec3Dot ( &a.vecNormal, &b.vecNormal ); 

	if ( fCos < 0.0f || fDistEps < fCosEps )
		return false;

	if ( ( a.D + fDistEps < b.D ) || ( b.D + fDistEps < a.D ) )
		return false;
	
	return true;
}

int cLightMaps::ClassifyPoint ( D3DXVECTOR3 vecPoint, D3DXVECTOR3 vecA, D3DXVECTOR3 vecB )
{
	// is the point to front or back of plane

	D3DXVECTOR3 vecDirection;

	vecDirection.x = vecA.x - vecPoint.x;
	vecDirection.y = vecA.y - vecPoint.y;
	vecDirection.z = vecA.z - vecPoint.z;

	float fResult = D3DXVec3Dot ( &vecDirection, &vecB );

	if ( fResult < -0.01f )
		return 1;
	
	if ( fResult > 0.01f )
		return -1;

	return 0;
}

cLightMaps::sPlane cLightMaps::ConstructPlane ( D3DXVECTOR3* vecRefPoint, D3DXVECTOR3* vecNormal )
{
	// create a plane

	sPlane result;

	result.vecNormal    = *vecNormal;
	result.vecRefPoint  = *vecRefPoint;

	result.A = vecNormal->x;
	result.B = vecNormal->y;
	result.C = vecNormal->z;
	result.D = -( D3DXVec3Dot ( vecNormal, vecRefPoint ) );

	float fNorm = ( float ) fabs ( vecNormal->x );
	result.iProjectionPlane = 1;

	if ( fNorm < fabs ( vecNormal->y ) )
	{
		fNorm = ( float ) fabs ( vecNormal->y );
		result.iProjectionPlane = 2;
	}

	if ( fNorm < fabs ( vecNormal->z ) )
	{
		fNorm = ( float ) fabs ( vecNormal->z );
		result.iProjectionPlane = 3;
	}

	return result;
}

cLightMaps::sAABB cLightMaps::GetTriangleBoundingBox ( D3DXVECTOR3* pVector )
{
	sAABB result;

	// reset values
	result.fRadius   = 0.0f;
	result.vecCentre = D3DXVECTOR3 (        0.0f,        0.0f,        0.0f );
	result.vecMin    = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );
	result.vecMax    = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );

	// run through all vertices
	for ( int iTemp = 0; iTemp < 3; iTemp++ )
	{
		// get the vertex info
		D3DXVECTOR3 vec = pVector [ iTemp ];

		// work out bounding box extents
		if ( vec.x < result.vecMin.x ) result.vecMin.x = vec.x;
		if ( vec.y < result.vecMin.y ) result.vecMin.y = vec.y;
		if ( vec.z < result.vecMin.z ) result.vecMin.z = vec.z;

		if ( vec.x > result.vecMax.x ) result.vecMax.x = vec.x;
		if ( vec.y > result.vecMax.y ) result.vecMax.y = vec.y;
		if ( vec.z > result.vecMax.z ) result.vecMax.z = vec.z;
		
		// get the centre of the object
		result.vecCentre = result.vecMin + result.vecMax;
		result.vecCentre /= 2.0f;

		// now work out the radius
		D3DXVECTOR3 temp = result.vecMax - result.vecMin;
		result.fRadius   = sqrt ( temp.x * temp.x + temp.y * temp.y + temp.z * temp.z ) * 0.5f;
	}
	
	return result;
}

bool point_in_poly ( D3DXVECTOR3& p, D3DXVECTOR3 verts [ 3 ] )
{
    int i = 0;
    
	D3DXVECTOR3 v [ 3 ];
	for ( i = 0; i < 3; i++ )
	{
	    v [ i ] = p - verts [ i ];
		D3DXVec3Normalize ( &v [ i ], &v [ i ] );
	}

	float total = 0.0f;

	for ( i = 0; i < 3 - 1; i++ )
	    total += ( float ) acos ( D3DXVec3Dot ( &v [ i ], &v [ i + 1 ] ) );
	
	total += ( float ) acos ( D3DXVec3Dot ( &v [ 3 - 1 ], &v [ 0 ] ) );
	
	if ( fabsf ( total - 6.28f ) < 0.01f )
		return true;

	return false;
}

bool cLightMaps::LineOfSight ( sMesh* pMesh, D3DXVECTOR3 vecStart, D3DXVECTOR3 vecEnd, sPlane plane )
{
	// determine if line is visible for shadows

	sOffsetMap	 offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	int iIndex = 0;

	for ( int j = 0; j < ( int ) ( pMesh->dwVertexCount / 3 ); j++ )
	{
		D3DXVECTOR3 vecPos = D3DXVECTOR3 ( 
											*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iIndex++ ) )
									   );

		D3DXVECTOR3 vecPos1 = D3DXVECTOR3 ( 
											*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iIndex++ ) )
									   );

		D3DXVECTOR3 vecPos2 = D3DXVECTOR3 ( 
											*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iIndex   ) ),
											*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iIndex++ ) )
									   );

		D3DXVECTOR3 test [ 3 ];
		D3DXVECTOR3 vecIntersectPoint;

		test [ 0 ] = vecPos;
		test [ 1 ] = vecPos1;
		test [ 2 ] = vecPos2;

		D3DXVECTOR3 vecNormal,
					vecEdge1,
					vecEdge2,
					vec0,
					vec1,
					vec2;

		vec0 = vecPos;
		vec1 = vecPos1;
		vec2 = vecPos2;

		vecEdge1 = vec1 - vec0;
		vecEdge2 = vec2 - vec0;

		D3DXVec3Cross ( &vecNormal, &vecEdge1, &vecEdge2 );
		D3DXVec3Normalize ( &vecNormal, &vecNormal );

		sPlane planeA = ConstructPlane ( &vec0, &vecNormal );

		float pPercent = 0;

		if ( ! GetIntersect ( &vecStart, &vecEnd, &vecPos, &planeA.vecNormal, &vecIntersectPoint, &pPercent ) )
			continue;

		D3DXVECTOR3	vecVerts [ 3 ];

		vecVerts [ 0 ] = vecPos;
		vecVerts [ 1 ] = vecPos1;
		vecVerts [ 2 ] = vecPos2;

		if ( point_in_poly ( vecIntersectPoint, vecVerts ) )
		{
			if ( AreCoPlanar ( plane, planeA, 0.02f, 0.02f ) )
				continue;

			return false;
		}
		
	}

	return true;
}

//
// LM CONSOLIDATION CODE
//

cLightMapConsolidation::cLightMapConsolidation ( )
{
//	memset ( this, 0, sizeof ( cLightMapConsolidation ) );
	// 20120326 IanM - Properly construct the object
	m_dwLumelStore = 0;
	m_iTextureCount = 0;
	m_iSize = 0;
	m_iLMWidth = 0;
	m_iLMHeight = 0;
	m_pMesh = 0;
	m_pMasterTexture = 0;
	m_pMasterTextureDDS = 0;
	m_pMasterPixels = 0;
	m_dwMasterWidth = 0;
	m_dwMasterHeight = 0;
	m_dwMasterPitch = 0;
	m_pPathForLightMaps = 0;

	memset(m_rect, 0, sizeof(m_rect));
	memset(m_pLMFile, 0, sizeof(m_pLMFile));
}

cLightMapConsolidation::~cLightMapConsolidation ( )
{
}

LPDIRECT3DTEXTURE9 cLightMapConsolidation::GetTexture ( void )
{
	return m_pMasterTextureDDS;
}

int cLightMapConsolidation::IsTextureInList ( DWORD* pLumelPoly )
{
	// determine if a texture is in the list
	if ( m_iTextureCount == 0 )
		return -1;

	DWORD* pPixelsA = NULL;
	DWORD* pPixelsB = NULL;
	bool   bMatch = true;
	int    iFound = -1;

	// go through all textures
	for ( int iImageInMaster = 0; iImageInMaster < m_iTextureCount; iImageInMaster++ )
	{
		// set match to true
		bMatch = true;

		// get start x and y
		int iXStart = m_rect [ iImageInMaster ].left;
		int iYStart = m_rect [ iImageInMaster ].top;

		// go through all pixels and see if there are any different
		DWORD dwLMSingleSize = sizeof(DWORD)*m_iLMWidth*m_iLMHeight;
		DWORD* pLMDataBase = m_dwLumelStore+((dwLMSingleSize/sizeof(DWORD))*iImageInMaster);
		for ( int iX = 0; iX < m_iLMWidth; iX++ )
		{
			for ( int iY = 0; iY < m_iLMHeight; iY++ )
			{
				int iOffset = (iX+(iY*m_iLMHeight));
				DWORD* pLumerColA = pLumelPoly + iOffset;
				DWORD* pLumerColB = pLMDataBase + iOffset;
				if ( *pLumerColA != *pLumerColB )
				{
					bMatch = false;
					iX = m_iLMWidth; iY = m_iLMHeight;
					break;
				}
			}
		}

		// if all pixels are same then we have a match
		// so no need to add texture as it's already in list
		if ( bMatch )
		{
			iFound = iImageInMaster;
			break;
		}
	}

	// return index if found
	if ( bMatch )
		return iFound;

	// not in list
	return -1;
}

bool cLightMapConsolidation::LockTexture ( LPDIRECT3DTEXTURE9 pTexture, DWORD** ppPixels, DWORD* dwPitch, DWORD* dwWidth, DWORD* dwHeight )
{
	// lock a texture so it can be written to

	D3DLOCKED_RECT	d3dlr;
	D3DSURFACE_DESC desc;

	if ( !ppPixels || !dwPitch || !dwWidth || !dwHeight )
		return false;
			
	if ( FAILED ( pTexture->GetLevelDesc ( 0, &desc ) ) )
		return false;

	if ( FAILED ( pTexture->LockRect ( 0, &d3dlr, 0, 0 ) ) )
		return false;

	*ppPixels = ( DWORD* ) d3dlr.pBits;
	*dwPitch  = d3dlr.Pitch;
	*dwWidth  = desc.Width;
	*dwHeight = desc.Height;

	return true;
}

void cLightMapConsolidation::UnlockTexture ( LPDIRECT3DTEXTURE9 pTexture )
{
	// unlock texture
	pTexture->UnlockRect ( NULL );
}

void cLightMapConsolidation::FindSpace ( int iWidth, int iHeight, int* pX, int* pY )
{
	// find a space within the master texture for the
	// new texture

	// return 0, 0 when no textures are in list
	if ( m_iTextureCount == 0 )
	{
		*pX = 0;
		*pY = 0;
		return;
	}

	for ( int iImageInMaster = 0; iImageInMaster < m_iTextureCount; iImageInMaster++ )
	{
		// we need to find a space for this texture

		// new x and y position
		*pX = m_rect [ iImageInMaster ].left;
		*pY = m_rect [ iImageInMaster ].bottom;

		// move to next column if larger than texture size
		if ( m_rect [ iImageInMaster ].bottom > m_iSize - iWidth )
		{
			*pX += iWidth;
			*pY  = 0;
		}
	}
}

bool cLightMapConsolidation::AddTexture ( DWORD* pLumelPoly, int iPolyTexture )
{
	// when hit maximum, consolidate results and start new LM plate (by failing)
	if ( m_iTextureCount >= (512/m_iLMWidth)*(512/m_iLMHeight) )
		return false;

	// add texture to master texture
	int iXStart = 0, iYStart  = 0;

	// this texture identified as
	int iImageInMaster = m_iTextureCount;

	// find a space for it
	FindSpace ( m_iLMWidth, m_iLMHeight, &iXStart, &iYStart );

	// copy store into an the lumer array for a structured use
	DWORD dwLMSingleSize = sizeof(DWORD)*m_iLMWidth*m_iLMHeight;
	DWORD* pLMDataBase = m_dwLumelStore+((dwLMSingleSize/sizeof(DWORD))*iImageInMaster);
	memcpy ( pLMDataBase, pLumelPoly, dwLMSingleSize );

	// leeadd - 300704 - blur the texture on any edges (black meets non-black)
	if ( m_iLMWidth<=32 && m_iLMHeight<=32 )
	{
		// hard coded for quick-test of technique
		// better shadows, no jaggies but polygons shown up due to anti-alias disparity

		// extract from lumel buffer
		DWORD px[32][32];
		for ( int iY = 0; iY < m_iLMHeight; iY++ )
			for ( int iX = 0; iX < m_iLMWidth; iX++ )
				px[iX][iY] = *(pLMDataBase + iX + (iY*m_iLMWidth));

		// create new buffer
		DWORD newpx[32][32];

		// loop through each pixel
		for ( int x = 0; x < m_iLMWidth; x++ )
		{
			for ( int y = 0; y < m_iLMHeight; y++ )
			{
				// copy over pixel data
				newpx[x][y] = px[x][y];

				// reset flag to determine if blur pixel should be used
				bool okay=false;

				// reset blur vars
				DWORD colr=0;
				DWORD colg=0;
				DWORD colb=0;
				DWORD colcount=0;

				// go through 3x3 area over pixel
				for ( int blx=-1; blx<2; blx++ )
				{
					for ( int bly=-1; bly<2; bly++ )
					{
						// only for pixels in tile data
						if ( x+blx>=0 && x+blx<m_iLMWidth )
						{
							if ( y+bly>=0 && y+bly<m_iLMHeight )
							{
								// get the source pixel colour
								DWORD col = px[x+blx][y+bly];

								// if the colour is zero, it is an edge pixel to use as a blur pixel
								if ( ((col & 0x00FF0000)>>16) < 32
								&&   ((col & 0x0000FF00)>>8) < 32
								&&   ((col & 0x000000FF)) < 32 )
									okay=true;

								// add to averaging vars
								colr+=((col & 0x00FF0000)>>16);
								colg+=((col & 0x0000FF00)>>8);
								colb+=((col & 0x000000FF));
								colcount++;
							}
						}
					}
				}

				// average the final colours collected
				colr=colr/colcount; if ( colr>255 ) colr=255;
				colg=colg/colcount; if ( colg>255 ) colg=255;
				colb=colb/colcount; if ( colb>255 ) colb=255;

				// if blur pixel should be used, write it
				if ( okay ) newpx[x][y] = 0xFF000000+(colr<<16)+(colg<<8)+(colb);
			}
		}

		// put back in lumel buffer
		for ( int iY = 0; iY < m_iLMHeight; iY++ )
			for ( int iX = 0; iX < m_iLMWidth; iX++ )
				*(pLMDataBase + iX + (iY*m_iLMWidth)) = newpx[iX][iY];
	}

	// and then copy data from LMdata
	int iOffset = 0;
	int iFloatPitch = m_dwMasterPitch / 4;
	int iPixelOffset = iXStart + (iYStart*iFloatPitch);
	for ( int iY = iYStart; iY < iYStart+m_iLMHeight; iY++ )
	{
		memcpy ( m_pMasterPixels+iPixelOffset, pLMDataBase+iOffset, m_iLMWidth*4 );
		iPixelOffset+=iFloatPitch;
		iOffset+=m_iLMWidth;
	}

	// store the position
	m_rect       [ iImageInMaster ].left   = iXStart;
	m_rect       [ iImageInMaster ].top    = iYStart;
	m_rect       [ iImageInMaster ].right  = m_iLMWidth;
	m_rect       [ iImageInMaster ].bottom = iYStart + m_iLMHeight;

	// increment texture count
	m_iTextureCount++;

	// update UV data to point to place on master texture
	UpdateCoordinates ( m_iLMWidth, m_iLMHeight, iPolyTexture, iXStart, iYStart, -1, true );

	// success
	return true;
}

void cLightMapConsolidation::UpdateCoordinates ( DWORD dwWidth, DWORD dwHeight, int iPolyTexture, int iXStart, int iYStart, int iImageInMaster, bool bMode )
{
	// get poly vert ptrs
	sLightMapVertex* pVertices [ 3 ];
	pVertices [ 0 ] = ( ( sLightMapVertex* ) m_pMesh->pVertexData + ( ( iPolyTexture * 3 ) + 0 ) );
	pVertices [ 1 ] = ( ( sLightMapVertex* ) m_pMesh->pVertexData + ( ( iPolyTexture * 3 ) + 1 ) );
	pVertices [ 2 ] = ( ( sLightMapVertex* ) m_pMesh->pVertexData + ( ( iPolyTexture * 3 ) + 2 ) );

	// get coordinate in master texture
	float fX, fY;
	if ( bMode )
	{
		fX = ( ( float ) ( iXStart ) ) / ( float ) m_iSize;
		fY = ( ( float ) ( iYStart ) ) / ( float ) m_iSize;
	}
	else
	{
		fX = ( ( float ) m_rect [ iImageInMaster ].left ) / ( float ) m_iSize;
		fY = ( ( float ) m_rect [ iImageInMaster ].top ) / ( float ) m_iSize;
	}

	// work out scaling factor to go from 0-1 to 0-0.002 (scaled down to master UVs)
	float fWidth  = dwWidth;
	float fHeight = dwHeight;
	float uFactor = ( float ) ( fWidth ) / ( float ) m_iSize;
	float vFactor = ( float ) ( fHeight ) / ( float ) m_iSize;
	float fTexFilterBitU = 1.0f/fWidth;
	float fTexFilterBitV = 1.0f/fHeight;
	float fTexFilterShiftU = fTexFilterBitU*(fWidth-2);
	float fTexFilterShiftV = fTexFilterBitV*(fHeight-2);

	// modify UVs to use master texture
	for ( int iVertex = 0; iVertex < 3; iVertex++ )
	{
		// for each vertex
		float currentU = m_OriginalVertexList [ ( iPolyTexture * 3 ) + iVertex ].lu;
		float currentV = m_OriginalVertexList [ ( iPolyTexture * 3 ) + iVertex ].lv;
		// modify UV factor by stepping in ONE TEXEL for use by texture filter
		currentU = (currentU*fTexFilterShiftU) + fTexFilterBitU;
		currentV = (currentV*fTexFilterShiftV) + fTexFilterBitV;
		// scale down to actual place on master texture
		currentU *= uFactor;
		currentV *= vFactor;
		// copy to vertex for final lightmap data UV
		pVertices [ iVertex ]->lu = fX + currentU;
		pVertices [ iVertex ]->lv = fY + currentV;
	}
}

void cLightMapConsolidation::Init ( DWORD dwWidth, DWORD dwHeight, int* piLightMap )
{
	// clear out values
	m_iTextureCount = 0;
	m_iSize			= 512;
	m_iLMWidth		= dwWidth;
	m_iLMHeight		= dwHeight;

	// max. number of LMs in a 512x512 space
	DWORD dwMaxLMElementsInTexture = (512/m_iLMWidth)*(512/m_iLMHeight);
		
	// create lumel store per poly
	DWORD dwLMSingleSize = sizeof(DWORD)*m_iLMWidth*m_iLMHeight;
	DWORD dwSize = dwLMSingleSize * dwMaxLMElementsInTexture;
	m_dwLumelStore = (DWORD*)new char [ dwSize ];
	memset ( m_dwLumelStore, 0, dwSize );

	// create our new MASTER texture
	if ( FAILED ( m_pD3D->CreateTexture ( m_iSize, m_iSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pMasterTexture, NULL ) ) )
		return;

	// create our DDS MASTER texture (the final one to be used)
	if ( FAILED ( m_pD3D->CreateTexture ( m_iSize, m_iSize, 1, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, &m_pMasterTextureDDS, NULL ) ) )
		return;
	
	// lock master texture once
	if ( !LockTexture ( m_pMasterTexture, &m_pMasterPixels, &m_dwMasterPitch, &m_dwMasterWidth, &m_dwMasterHeight ) )
		return;

	// work out lightmap filename
	strcpy ( m_pLMFile, "" );
	if ( m_pPathForLightMaps )
	{
		if ( strcmp ( m_pPathForLightMaps, "")>NULL )
		{
			char szImageFilename [ MAX_STRING ];
			sprintf ( szImageFilename, "%s\\%d.dds", m_pPathForLightMaps, *piLightMap );
			*piLightMap = (*piLightMap) + 1;
			if ( strlen ( szImageFilename ) < MAX_STRING )
				strcpy ( m_pLMFile, szImageFilename );
		}
	}
}

bool cLightMapConsolidation::Run ( sMesh* pMesh, DWORD** m_ppdwLumelList, bool* m_ppdwShadowList, int iNumberOfPolygons )
{
	// run through light mapping consolidation process
	m_pMesh = pMesh;
	if ( !m_pMesh )
		return true;

	// copy 'vertex data' to 'lightmap vertex data'
	m_OriginalVertexList.clear();
	for ( int iVertex = 0; iVertex < (int)pMesh->dwVertexCount; iVertex++ )
	{
		sLightMapVertex* pVertices = ( ( sLightMapVertex* ) m_pMesh->pVertexData + ( iVertex ) );
		m_OriginalVertexList.push_back ( *pVertices );
	}

	// go through each texture/poly
	for ( int iPolyTexture = 0; iPolyTexture < iNumberOfPolygons; iPolyTexture++ )
	{
		// see if LM image exists in master texture (via m_dwLumelStore)
		int iIndex = -1;
		if ( m_ppdwShadowList [ iPolyTexture ]==false )
			iIndex = IsTextureInList ( m_ppdwLumelList [ iPolyTexture ] );

		// only if LM unique, need to add
		if ( iIndex < 0 )
		{
			// if not, add as new LM image to master texture
			if ( !AddTexture ( m_ppdwLumelList [ iPolyTexture ], iPolyTexture ) )
				return false;
		}
		else
		{
			// can re-use existing LM image already existing in master texture
			UpdateCoordinates ( m_iLMWidth, m_iLMHeight, iPolyTexture, -1, -1, iIndex, false );
		}
	}

	// success
	return true;
}

void cLightMapConsolidation::Complete ( int iType )
{
	// unlock master texture
	UnlockTexture ( m_pMasterTexture );

	// Convert final lightmap texture to DDS
	if ( m_pMasterTextureDDS )
	{
		// copy RAW to DDS
		LPDIRECT3DSURFACE9 pDDSSurface;
		m_pMasterTextureDDS->GetSurfaceLevel ( 0, &pDDSSurface );
		if ( pDDSSurface )
		{
			LPDIRECT3DSURFACE9 pMasterSurface;
			m_pMasterTexture->GetSurfaceLevel ( 0, &pMasterSurface );
			if ( pMasterSurface )
			{
				D3DXLoadSurfaceFromSurface ( pDDSSurface, NULL, NULL, pMasterSurface, NULL, NULL, D3DX_DEFAULT, 0 );
			}
			SAFE_RELEASE ( pMasterSurface );
		}
		SAFE_RELEASE ( pDDSSurface );
	}

	// free usages
	SAFE_DELETE ( m_dwLumelStore );

	// if path specified, save lightmap and record filename in image
	if ( m_iTextureCount > 0 )
	{
		if ( strcmp ( m_pLMFile, "")>NULL )
		{
			D3DXSaveTextureToFile ( m_pLMFile, D3DXIFF_DDS, m_pMasterTextureDDS, NULL );

			// debug output (disabled for now)
//			char pLMFileBMP [ _MAX_PATH ];
//			strcpy ( pLMFileBMP, m_pLMFile );
//			strcat ( pLMFileBMP, ".bmp" );
//			D3DXSaveTextureToFile ( pLMFileBMP, D3DXIFF_BMP, m_pMasterTexture, NULL );
		}
	}

	// release RAW (DDS remains as it is used by meshes than are now lighmapped)
	SAFE_RELEASE ( m_pMasterTexture );
}
