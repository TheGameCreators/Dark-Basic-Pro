
/////////////////////////////////////////////////////////////////////////////////////
// PARTICLE INCLUDES ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "particleemitter.h"
#include "particles.h"
#include "error.h"
//#include <darksdk.h>

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL PARTICLE FUNCTION LISTINGS //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

PhysX::sPhysXEmitter*	dbPhyGetEmitter				( int iID, bool bDisplayError = false );
void					dbPhyUpdateEmitterVertex	( PhysX::sParticleVertex*	pVertex, float fX, float fY, float fZ, float tu, float tv, DWORD dwDiffuse );
DWORD					dbPhyInterpolate			( DWORD dwCurrentColor, DWORD dwNextColor, float fTime );
sObject* dbGetObject ( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/*
	PHY MAKE PARTICLE EMITTER%LLFFFFFFFF%?dbPhyMakeParticleEmitter@@YAXHHMMMMMMMM@Z%
	PHY MAKE FIRE EMITTER%LLFFF%?dbPhyMakeFireEmitter@@YAXHHMMM@Z%
	PHY MAKE SMOKE EMITTER%LLFFF%?dbPhyMakeSmokeEmitter@@YAXHHMMM@Z%
	PHY MAKE SNOW EMITTER%LLFFF%?dbPhyMakeSnowEmitter@@YAXHHMMM@Z%
	PHY DELETE EMITTER%L%?dbPhyDeleteEmitter@@YAXH@Z%
	
	PHY SET EMITTER LINEAR VELOCITY%LFFF%?dbPhySetEmitterLinearVelocity@@YAXHMMM@Z%
	PHY SET EMITTER LINEAR VELOCITY RANGE%LFFFFFF%?dbPhySetEmitterLinearVelocityRange@@YAXHMMMMMM@Z%
	PHY SET EMITTER GRAVITY%LL%?dbPhySetEmitterGravity@@YAXHH@Z%
	PHY SET EMITTER LIFE SPAN%LF%?dbPhySetEmitterLifeSpan@@YAXHM@Z%
	PHY SET EMITTER FADE%LL%?dbPhySetEmitterFade@@YAXHH@Z%
	PHY SET EMITTER SIZE%LF%?dbPhySetEmitterSize@@YAXHM@Z%
	PHY SET EMITTER MAX SIZE%LF%?dbPhySetEmitterMaxSize@@YAXHM@Z%
	PHY SET EMITTER GROWTH RATE%LF%?dbPhySetEmitterGrowthRate@@YAXHM@Z%
	PHY SET EMITTER RANGE%LFFFFFF%?dbPhySetEmitterRange@@YAXHMMMMMM@Z%
	PHY SET EMITTER START COLOR%LLLL%?dbPhySetEmitterStartColor@@YAXHKKK@Z%
	PHY SET EMITTER END COLOR%LLLL%?dbPhySetEmitterEndColor@@YAXHKKK@Z
	PHY SET EMITTER POSITION%LFFF%?dbPhySetEmitterPosition@@YAXHMMM@Z%

	PHY SET EMITTER COLLISION SCALE%LF%?dbPhySetEmitterCollisionScale@@YAXHM@Z%
	PHY SET EMITTER COLLISION%LL%?dbPhySetEmitterCollision@@YAXHH@Z%

	PHY SET EMITTER RATE%LF%?dbPhySetEmitterRate@@YAXHM@Z%

	PHY SET EMITTER UPDATE%LF%?dbPhySetEmitterUpdate@@YAXHM@Z%

	PHY GET EMITTER EXIST[%LL%?dbPhyGetEmitterExist@@YAHH@Z%
*/


/////////////////////////////////////////////////////////////////////////////////////
// EXPOSED FUNCTIONS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeParticleEmitter ( int iID, int iMaximum, float fEmitRate, float fX, float fY, float fZ, float fSpeed, float fXDir, float fYDir, float fZDir )
{
	dbPhyCheckScene ( );

	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID ) )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a particle emitter with an ID that is already in use", "", 0 );

	
	PhysX::sPhysXEmitter*			pEmitter		= new PhysX::sPhysXEmitter;
	sObject*						pObject			= NULL;
	DWORD							dwVertexCount	= iMaximum * 4;
	DWORD							dwIndexCount	= iMaximum * 6;
	int								iOffset			= 0;
	int								iValue			= 0;
	int								iIndex			= 0;
	PhysX::sPhysXEmitterColorKey	key				= { D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f };

	if ( !pEmitter )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory for particle emitter", "", 0 );
	
	pEmitter->iID					= iID;
	pEmitter->fSpeed				= fSpeed;
	pEmitter->fX					= fX;
	pEmitter->fY					= fY;
	pEmitter->fZ					= fZ;
	pEmitter->iFade					= 1;
	pEmitter->dwColor				= D3DCOLOR_ARGB ( 255, 255, 255, 255 );
	pEmitter->bLinearVelocityRange	= false;
	pEmitter->pEmitter				= new PhysX::ParticleEmitter ( iMaximum, fEmitRate, PhysX::pScene, NxVec3 ( fX, fY, fZ ), fSpeed, NxVec3 ( fXDir, fYDir, fZDir ) );
	pEmitter->fUpdate				= 0.01f;

	if ( !pEmitter->pEmitter )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create particle emitter", "", 0 );

	PhysX::pScene->setGroupCollisionFlag ( PhysX::cParticleCollisionGroup, PhysX::cParticleCollisionGroup, false );

	pEmitter->ColorKeys.push_back ( key );
	pEmitter->ColorKeys.push_back ( key );

	if ( !PhysX::CreateNewObject )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid function pointers, cannot create emitter", "", 0 );

	PhysX::CreateNewObject ( iID, "particle", 1 );

	pObject = PhysX::GetObject ( iID );

	if ( !pObject )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create basic 3d object for particle emitter", "", 0 );
	
	if ( !pObject->pFrame )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid frame for particle emitter", "", 0 );

	PhysX::SetupMeshFVFData ( pObject->pFrame->pMesh, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, dwVertexCount, dwIndexCount );

	if ( !pObject->pFrame->pMesh )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid mesh for particle emitter", "", 0 );

	if ( !pObject->pFrame->pMesh->pVertexData )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid vertex buffer for particle emitter", "", 0 );
	
	if ( !pObject->pFrame->pMesh->pIndices )
		return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid index buffer for particle emitter", "", 0 );

	pObject->bTransparentObject              = true;
	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = dwVertexCount;
	pObject->pFrame->pMesh->iDrawPrimitives  = dwIndexCount / 3;
	pObject->pFrame->pMesh->bCull			 = false;

	for ( iIndex = 0; iIndex < ( int ) dwIndexCount / 6; iIndex++, iValue += 4 )
	{
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 0;
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 1;
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 2;
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 2;
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 3;
		pObject->pFrame->pMesh->pIndices [ iOffset++ ] = iValue + 0;
	}

	PhysX::CalculateMeshBounds			( pObject->pFrame->pMesh );
	PhysX::SetNewObjectFinalProperties	( iID, 0.0f );
	PhysX::SetTexture					( iID, 0 );
	//PhysX::SetObjectTransparency		( iID, 1 );
	PhysX::DisableObjectZWrite			( iID );
	PhysX::SetObjectLight				( iID, 0 );

	PhysX::pEmitterList.push_back ( pEmitter );	

	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	dbPhySetEmitterCollision ( iID, 1 );
	dbPhySetEmitterCollisionScale ( iID, 1.0f );
}

void dbPhyMakeFireEmitter ( int iID, int iMaximum, float fX, float fY, float fZ )
{
	dbPhyMakeParticleEmitter			( iID, iMaximum, 0.01f, fX, fY, fZ, 0.1f, 0, 10, 0 );
	dbPhySetEmitterLifeSpan				( iID, 0.005f );
	//dbPhySetEmitterLifeSpan				( iID, 20.005f );
	dbPhySetEmitterSize					( iID, 5.0f );
	dbPhySetEmitterMaxSize				( iID, 20.0f );
	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	PhysX::GhostObjectOn1				( iID, 0 );
	dbPhySetEmitterLinearVelocityRange	( iID, -8.0f, 5.0f, -4.0f, 16.0f, 10.0f, 4.0f );
	dbPhySetEmitterGrowthRate			( iID, 5.8f );

	//dbPhySetEmitterCollision ( iID, 2 );
}

void dbPhyMakeSmokeEmitter ( int iID, int iMaximum, float fX, float fY, float fZ )
{
	dbPhyMakeParticleEmitter			( iID, iMaximum, 0.01f, fX, fY, fZ, 0.01f, 0, 5, 0 );
	dbPhySetEmitterLifeSpan				( iID, 0.008f );
	dbPhySetEmitterSize					( iID, 1.0f );
	dbPhySetEmitterMaxSize				( iID, 4.0f );
	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	PhysX::GhostObjectOn1				( iID, 0 );
	dbPhySetEmitterLinearVelocityRange	( iID, -8.0f, 4.0f, -4.0f, 16.0f, 12.0f, 4.0f );
	dbPhySetEmitterGrowthRate			( iID, 4.0f );
}

void dbPhyMakeSnowEmitter ( int iID, int iMaximum, float fX, float fY, float fZ )
{
	dbPhyMakeParticleEmitter			( iID, iMaximum, 0.1f, fX, fY, fZ, 0.1f, 0, 5, 0 );
	dbPhySetEmitterLifeSpan				( iID, 0.004f );
	dbPhySetEmitterSize					( iID, 0.25f );
	dbPhySetEmitterMaxSize				( iID, 0.25f );
	dbPhySetEmitterStartColor			( iID, 255, 255, 255 );
	dbPhySetEmitterEndColor				( iID, 0, 0, 0 );
	dbPhySetEmitterGravity				( iID, 1 );
	dbPhySetEmitterLinearVelocityRange	( iID, -2.0f, -1.0f, -2.0f, 2.0f, -1.0f, 2.0f );
	dbPhySetEmitterRange				( iID, -20, 20, -20, 40, 0.01f, 40 );
	PhysX::GhostObjectOn				( iID );
}

void dbPhyDeleteEmitter ( int iID )
{
	dbPhyCheckScene ( );

	for ( DWORD dwEmitter = 0; dwEmitter < PhysX::pEmitterList.size ( ); dwEmitter++ )
	{
		if ( PhysX::pEmitterList [ dwEmitter ]->iID == iID )
		{
			// get a pointer to our object
			PhysX::sPhysXEmitter* pEmitter = PhysX::pEmitterList [ dwEmitter ];

			// see if an actor exists
			if ( pEmitter->pEmitter )
			{
				PhysX::DeleteObject ( pEmitter->iID );

				// release the emitter
				delete pEmitter->pEmitter;

				// erase the item in the list
				PhysX::pEmitterList.erase ( PhysX::pEmitterList.begin ( ) + dwEmitter );

				// clear up the memory
				delete pEmitter;
				pEmitter = NULL;

				return;
			}
		}
	}

	PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to delete emitter that does not exist with phy delete emitter", "", 0 );
}

void dbPhySetEmitterRange ( int iID, float fRangeX1, float fRangeY1, float fRangeZ1, float fRangeX2, float fRangeY2, float fRangeZ2 )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->m_fRangeX1 = fRangeX1;
		pEmitter->pEmitter->m_fRangeY1 = fRangeY1;
		pEmitter->pEmitter->m_fRangeZ1 = fRangeZ1;
		pEmitter->pEmitter->m_fRangeX2 = fRangeX2;
		pEmitter->pEmitter->m_fRangeY2 = fRangeY2;
		pEmitter->pEmitter->m_fRangeZ2 = fRangeZ2;
		pEmitter->pEmitter->m_bRange   = true;
	}
}

void dbPhySetEmitterLinearVelocityRange ( int iID, float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		if ( fMinX == 0.0f ) fMinX = 1.0f;
		if ( fMinY == 0.0f ) fMinY = 1.0f;
		if ( fMinZ == 0.0f ) fMinZ = 1.0f;
		if ( fMaxX == 0.0f ) fMaxX = 1.0f;
		if ( fMaxY == 0.0f ) fMaxY = 1.0f;
		if ( fMaxZ == 0.0f ) fMaxZ = 1.0f;

		pEmitter->fMinX = fMinX;
		pEmitter->fMinY = fMinY;
		pEmitter->fMinZ = fMinZ;

		pEmitter->fMaxX = fMaxX;
		pEmitter->fMaxY = fMaxY;
		pEmitter->fMaxZ = fMaxZ;

		pEmitter->bLinearVelocityRange = true;
	}
}


void dbPhySetEmitterGrowthRate ( int iID, float fRate )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setGrowthRate ( fRate );
	}
}

void dbPhySetEmitterLinearVelocity ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setLinearVelocity ( fX, fY, fZ );
	}
}

void dbPhySetEmitterGravity ( int iID, int iState )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setGravity ( iState );
	}
}

void dbPhySetEmitterLifeSpan ( int iID, float fLife )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setLifeSpan ( fLife );
	}
}

void dbPhySetEmitterFade ( int iID, int iState )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->iFade = iState;
	}
}

void dbPhySetEmitterStartColor ( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		PhysX::sPhysXEmitterColorKey key = { D3DCOLOR_ARGB ( 255, dwRed, dwGreen, dwBlue ), 0.0f };
		pEmitter->ColorKeys [ 0 ] = key;
	}
}

void dbPhySetEmitterEndColor ( int iID, DWORD dwRed, DWORD dwGreen, DWORD dwBlue )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		PhysX::sPhysXEmitterColorKey key = { D3DCOLOR_ARGB ( 0, dwRed, dwGreen, dwBlue ), 0.0f };
		pEmitter->ColorKeys [ 1 ] = key;
	}
}

void dbPhyAddEmitterColorKey ( int iID, float fTime, DWORD dwRed, DWORD dwGreen, DWORD dwBlue )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		PhysX::sPhysXEmitterColorKey key = { D3DCOLOR_ARGB ( 255, dwRed, dwGreen, dwBlue ), fTime };

		pEmitter->ColorKeys.push_back ( key );
	}
}

void dbPhySetEmitterSize ( int iID, float fSize )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setSize ( fSize );
	}
}

void dbPhySetEmitterMaxSize ( int iID, float fSize )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setMaxSize ( fSize );
	}
}

void dbPhySetEmitterPosition ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setPosition ( fX, fY, fZ );
	}
}

void dbPhySetEmitterCollisionScale ( int iID, float fScale )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setCollisionScale ( fScale );
	}
}

void dbPhySetEmitterRate ( int iID, float fRate )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setEmitRate ( fRate );
	}
}

void dbPhySetEmitterUpdate ( int iID, float fUpdate )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->fUpdate = fUpdate;
	}
}

void dbPhySetEmitterCollision ( int iID, int iMode )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID, true ) )
	{
		pEmitter->pEmitter->setCollision ( iMode );
	}
}

void dbPhyUpdateParticles ( void )
{
	D3DXVECTOR3				norm	= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3				cpos	= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3				up		= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3				right	= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	sObject*				pObject = NULL;
	sMesh*					pMesh	= NULL;
	PhysX::sParticleVertex*	pVertex = NULL;
	int						iOffset = 0;

	if ( PhysX::GetCameraLook     )	norm  = PhysX::GetCameraLook	 ( );
	if ( PhysX::GetCameraPosition )	cpos  = PhysX::GetCameraPosition ( );
	if ( PhysX::GetCameraUp       )	up    = PhysX::GetCameraUp	     ( );
	if ( PhysX::GetCameraRight    ) right = PhysX::GetCameraRight    ( );

	// mike - 041207
	norm = -norm;

	for ( DWORD dwEmitter = 0; dwEmitter < PhysX::pEmitterList.size ( ); dwEmitter++ )
	{
		if ( !PhysX::pEmitterList [ dwEmitter ]->pEmitter )
			return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Emitter data invalid when attempting to update particles", "", 0 );

		//PhysX::pEmitterList [ dwEmitter ]->pEmitter->update ( 0.01f );
		//PhysX::pEmitterList [ dwEmitter ]->pEmitter->update ( 0.2f );

		PhysX::pEmitterList [ dwEmitter ]->pEmitter->update ( PhysX::pEmitterList [ dwEmitter ]->fUpdate );

		PhysX::sPhysXEmitter* pEmitter = PhysX::pEmitterList [ dwEmitter ];

		if ( pEmitter->bLinearVelocityRange )
			dbPhySetEmitterLinearVelocity ( pEmitter->iID, pEmitter->fMinX + rand()%((int)pEmitter->fMaxX), pEmitter->fMinY + rand()%((int)pEmitter->fMaxY), pEmitter->fMinZ + rand()%((int)pEmitter->fMaxZ) );
		
		pObject = PhysX::GetObject ( PhysX::pEmitterList [ dwEmitter ]->iID );

		if ( !pObject )
			return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Object data invalid when attempting to update particles", "", 0 );

		if ( !pObject->pFrame )
			return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Object data invalid when attempting to update particles", "", 0 );

		if ( !pObject->pFrame->pMesh )
			return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Object data invalid when attempting to update particles", "", 0 );

		pMesh = pObject->pFrame->pMesh;

		if ( !pObject->pFrame->pMesh->pVertexData )
			return PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Object data invalid when attempting to update particles", "", 0 );

		pVertex						= ( PhysX::sParticleVertex* ) pObject->pFrame->pMesh->pVertexData;
		iOffset						= 0;
		pMesh->bVBRefreshRequired	= true;

		int						iOffset = 0;

		for ( int i = 0; i < PhysX::pEmitterList [ dwEmitter ]->pEmitter->_iParticleCount; i++ )
		{
			NxActor* pActor = PhysX::pEmitterList [ dwEmitter ]->pEmitter->_aParticleArray [ i ]->_pActor;

			if ( !pActor )
				PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Rigid body data invalid when attempting to update particles", "", 0 );

			NxMat34  matPose   = pActor->getGlobalPose ( );
			NxMat33  matOrient = matPose.M;
			NxVec3   vecPos    = matPose.t;	
			float	 fLife	   = PhysX::pEmitterList [ dwEmitter ]->pEmitter->_aParticleArray [ i ]->m_fLife;
			//float	 fLife	   = 1.0f;

			float glmat [ 16 ];
			matOrient.getColumnMajorStride4 ( &( glmat [ 0 ] ) );
			vecPos.get ( &( glmat [ 12 ] ) );

			D3DXVECTOR3 pos = D3DXVECTOR3 ( glmat [ 12 ], glmat [ 13 ], glmat [ 14 ] );

			//float size = PhysX::pEmitterList [ dwEmitter ]->pEmitter->_aParticleArray [ i ]->m_fCurrentRadius;
			float size = PhysX::pEmitterList [ dwEmitter ]->pEmitter->_aParticleArray [ i ]->m_fDisplayRadius;
			//float size = 1.0f;

			D3DXVECTOR3 r, u, p0, p1, p2, p3 = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
			D3DXVECTOR3 center               = D3DXVECTOR3 ( pos.x, pos.y, pos.z );
		
			r = right * size;
			u = up    * size;

			p0 = center - r + u;
			p1 = center + r + u;
			p2 = center + r - u;
			p3 = center - r - u;


			int iLife = ( int ) fLife * 255;

			if ( iLife > 255 )
				iLife = 255;

			if ( iLife < 1 )
				iLife = 0;

			//if ( PhysX::pEmitterList [ dwEmitter ]->iFade == 0 )
			//	iLife = 255;


			//iLife = 255;

			//iLife = 50;

			iLife = 0;
			//fLife = 0.5f;

			if ( fLife < 0.0f )
				fLife = 0.0f;

			DWORD dwDiffuse = D3DCOLOR_ARGB ( iLife, 255, 255, 255 );
			
			dwDiffuse = dbPhyInterpolate ( pEmitter->ColorKeys [ 0 ].dwColor, pEmitter->ColorKeys [ 1 ].dwColor, fLife / 2 );
			
			DWORD dwAlpha = dwDiffuse >> 24;
			DWORD dwRed   = ((( dwDiffuse ) >> 16 ) & 0xff );
			DWORD dwGreen = ((( dwDiffuse ) >>  8 ) & 0xff );
			DWORD dwBlue  = ((( dwDiffuse ) )       & 0xff );

			dwAlpha   = iLife;
			dwDiffuse = D3DCOLOR_ARGB ( iLife, dwRed, dwGreen, dwBlue );

			dbPhyUpdateEmitterVertex ( &pVertex [ iOffset++ ], p0.x, p0.y, p0.z, 0, 0, dwDiffuse );
			dbPhyUpdateEmitterVertex ( &pVertex [ iOffset++ ], p1.x, p1.y, p1.z, 1, 0, dwDiffuse );
			dbPhyUpdateEmitterVertex ( &pVertex [ iOffset++ ], p2.x, p2.y, p2.z, 1, 1, dwDiffuse );
			dbPhyUpdateEmitterVertex ( &pVertex [ iOffset++ ], p3.x, p3.y, p3.z, 0, 1, dwDiffuse );
		}

		// mike - 110708 - fix problem with particles vanishing
		pObject->bUpdateOverallBounds = true; 
	}
}

int dbPhyGetEmitterExist ( int iID )
{
	if ( PhysX::sPhysXEmitter* pEmitter = dbPhyGetEmitter ( iID ) )
	{
		return 1;
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyClearParticles ( void )
{
	// delete a single rigid body in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwEmitter = 0; dwEmitter < PhysX::pEmitterList.size ( ); dwEmitter++ )
	{
		// get a pointer to our object
		PhysX::sPhysXEmitter* pEmitter = PhysX::pEmitterList [ dwEmitter ];

		if ( pEmitter->pEmitter )
		{
			PhysX::DeleteObject ( pEmitter->iID );

			delete pEmitter->pEmitter;

			delete pEmitter;
			pEmitter = NULL;
		}
	}

	PhysX::pEmitterList.clear ( );
}


PhysX::sPhysXEmitter* dbPhyGetEmitter ( int iID, bool bDisplayError )
{
	for ( DWORD dwEmitter = 0; dwEmitter < PhysX::pEmitterList.size ( ); dwEmitter++ )
	{
		if ( PhysX::pEmitterList [ dwEmitter ]->iID == iID )
			return PhysX::pEmitterList [ dwEmitter ];
	}

	
	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Emitter with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}

void dbPhyUpdateEmitterVertex ( PhysX::sParticleVertex*	pVertex, float fX, float fY, float fZ, float tu, float tv, DWORD dwDiffuse )
{
	pVertex->x			= fX;
	pVertex->y			= fY;
	pVertex->z			= fZ;
	pVertex->tu			= tu;
	pVertex->tv			= tv;
	pVertex->dwDiffuse	= dwDiffuse;
}

DWORD dbPhyInterpolate ( DWORD dwCurrentColor, DWORD dwNextColor, float fTime )
{
	DWORD dwAlpha1 = dwCurrentColor >> 24;
	DWORD dwRed1   = ((( dwCurrentColor ) >> 16 ) & 0xff );
	DWORD dwGreen1 = ((( dwCurrentColor ) >>  8 ) & 0xff );
	DWORD dwBlue1  = ((( dwCurrentColor ) )       & 0xff );

	DWORD dwAlpha2 = dwNextColor >> 24;
	DWORD dwRed2   = ((( dwNextColor ) >> 16 ) & 0xff );
	DWORD dwGreen2 = ((( dwNextColor ) >>  8 ) & 0xff );
	DWORD dwBlue2  = ((( dwNextColor ) )       & 0xff );

	DWORD dwAlpha = ( int ) floor ( dwAlpha2 * ( 1 - fTime ) + dwAlpha1 * fTime );
	DWORD dwRed   = ( int ) floor ( dwRed2   * ( 1 - fTime ) + dwRed1   * fTime );
	DWORD dwGreen = ( int ) floor ( dwGreen2 * ( 1 - fTime ) + dwGreen1 * fTime );
	DWORD dwBlue  = ( int ) floor ( dwBlue2  * ( 1 - fTime ) + dwBlue1  * fTime );

	return D3DCOLOR_ARGB ( dwAlpha, dwRed, dwGreen, dwBlue );
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////