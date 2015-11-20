
//
// CCollision Functions Header
//

#ifndef _CCOLLISION_H_
#define _CCOLLISION_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE COMMON ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "..\\CommonC.h"
#include "cBoxCol.h"

//////////////////////////////////////////////////////////////////////////////////
// Internal Data ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct sMegaCollisionFeedback
{
	int iFrameCollision;
	int iFrameRelatedToBone;
	DWORD dwVertex0IndexOfHitPoly;
	DWORD dwVertex1IndexOfHitPoly;
	DWORD dwVertex2IndexOfHitPoly;
	D3DXVECTOR3 vec0Hit;
	D3DXVECTOR3 vec1Hit;
	D3DXVECTOR3 vec2Hit;
	D3DXVECTOR3 vecHitPoint;
	D3DXVECTOR3 vecNormal;
	D3DXVECTOR3 vecReflectedNormal;
	DWORD dwArbitaryValue;
	sMesh* pHitMesh;
};
extern sMegaCollisionFeedback MegaCollisionFeedback;

//////////////////////////////////////////////////////////////////////////////////
// Internal Commands ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void		CreateCollisionChecklist	( void );
bool		UpdateColCenter				( sObject* pObject );

//////////////////////////////////////////////////////////////////////////////////
// Collision Commands ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Basic Collision

void		SetColOn					( sObject* pObject );
void		SetColOff					( sObject* pObject );
void		SetColBox					( sObject* pObject, float iX1, float iY1, float iZ1, float iX2, float iY2, float iZ2, int iRotatedBoxFlag );
void		FreeColBox					( sObject* pObject );
void		SetColToSpheres				( sObject* pObject );
void		SetColToBoxes				( sObject* pObject );
void		SetColToPolygons			( sObject* pObject );
void		SetSphereRadius				( sObject* pObject, float fRadius );
void		GlobalColOn					( void );
void		GlobalColOff				( void );

float		CheckIntersectObject		( sObject* pObject, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ );
int			CheckCol					( int iObjectA, int iObjectB );
int			CheckHit					( int iObjectA, int iObjectB );
int			CheckLimbCol				( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
int			CheckLimbHit				( int iObjectA, int iLimbA, int iObjectB, int iLimbB );
float		GetColRadius				( sObject* pObject );
float		GetColCenterX				( sObject* pObject );
float		GetColCenterY				( sObject* pObject );
float		GetColCenterZ				( sObject* pObject );
float		GetColX						( void );
float		GetColY						( void );
float		GetColZ						( void );
int 		GetCollidedAgainstFloor		( void );

// Static Collision

void		SetStaticColBox				( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 );
void		FreeStaticColBoxes			( void );
int			GetStaticLineOfSightEx		( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy );
float		GetStaticLineOfSightExX		( void );
float		GetStaticLineOfSightExY		( void );
float		GetStaticLineOfSightExZ		( void );
int			GetStaticHitEx				( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2    );

// Static Collision (Plus Extra Expressions)

float		GetStaticColX				( void );
float		GetStaticColY				( void );
float		GetStaticColZ				( void );
float		GetStaticColPosX			( void );
float		GetStaticColPosY			( void );
float		GetStaticColPosZ			( void );
float		GetStaticColNormalX			( void );
float		GetStaticColNormalY			( void );
float		GetStaticColNormalZ			( void );
float		GetStaticColTextureU		( void );
float		GetStaticColTextureV		( void );
int			GetStaticColPolysChecked	( void );
DWORD		GetStaticColArbitaryValue	( void );

// Automatic Collision 

void		DoAutomaticStart			( void );
void		DoAutomaticEnd				( void );
void		AutoObjectCol				( sObject* pObject, float fRadius, int iResponse );
void		AutoCameraCol				( int iCameraID, float fRadius, int iResponse, int iStandGroundMode );
void		AutoCameraCol				( int iCameraID, float fRadius, int iResponse );

#endif _CCOLLISION_H_
