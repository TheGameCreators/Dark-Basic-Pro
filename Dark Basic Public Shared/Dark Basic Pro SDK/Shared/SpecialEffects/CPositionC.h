#ifndef _CPOSITION_H_
#define _CPOSITION_H_

#pragma comment ( lib, "d3dx9.lib" )

#include <D3DX9.h>

//#define DARKSDK __declspec ( dllexport )

struct tagObjectPos
{
	D3DXMATRIX		matObject;			// transformation matrix

	D3DXVECTOR3		vecLook;			// look vector
	D3DXVECTOR3		vecUp;				// up vector
	D3DXVECTOR3		vecRight;			// right vector
	D3DXVECTOR3		vecPosition;		// position vector
	D3DXVECTOR3		vecRotate;			// rotate vector
	D3DXVECTOR3		vecYawPitchRoll;	// yaw, pitch and roll
	D3DXVECTOR3		vecScale;			// scale vector
	D3DXVECTOR3		vecLast;
	D3DXVECTOR3		vecLastPosition;
	float			fAspect;			// aspect ratio
	float			fFOV;				// field of view
	float			fZNear;				// z near
	float			fZFar;				// z far
	bool			bRotate;			// rotation type
	bool			bLock;				// locked to camera
	bool			bHasBeenMovedForResponse; //used for auto-collision-response

	bool			bFreeFlightRotation;

	bool			bApplyPivot;	// used to fix pivot
	D3DXVECTOR3		vecPivot;		

	D3DXMATRIX		matObjectNoTran;	// new - get the final matrix just before translation
	bool			bColCenterUpdated;	// used to track center of actual mesh for collision
	D3DXVECTOR3		vecColCenter;

	tagObjectPos ( )
	{
		vecLook     = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
		vecUp       = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
		vecRight    = D3DXVECTOR3 ( 1, 0, 0 );		// right vector

		bFreeFlightRotation = false;
		
		memset ( &matObject, 0, sizeof ( D3DXMATRIX ) );
	}
};

void ParticleInternalUpdate ( int iID );		// update any changes to camera


// Internal Functions
float ParticleGetXPosition ( int iID );
float ParticleGetYPosition ( int iID );
float ParticleGetZPosition ( int iID );
float ParticleGetXRotation ( int iID );
float ParticleGetYRotation ( int iID );
float ParticleGetZRotation ( int iID );


// DBV1 Euler
void ParticleScale     ( int iID, float fX, float fY, float fZ );
void ParticlePosition  ( int iID, float fX, float fY, float fZ );


void ParticleRotate    ( int iID, float fX, float fY, float fZ );
void ParticleXRotate   ( int iID, float fX );
void ParticleYRotate   ( int iID, float fY );
void ParticleZRotate   ( int iID, float fZ );
void ParticlePoint     ( int iID, float fX, float fY, float fZ );

void ParticleMove      ( int iID, float fStep );
void ParticleMoveUp    ( int iID, float fStep );
void ParticleMoveDown  ( int iID, float fStep );
void ParticleMoveLeft  ( int iID, float fStep );
void ParticleMoveRight ( int iID, float fStep );

// DBV1 Freeflight
void ParticleTurnLeft  ( int iID, float fAngle );
void ParticleTurnRight ( int iID, float fAngle );
void ParticlePitchUp   ( int iID, float fAngle );
void ParticlePitchDown ( int iID, float fAngle );
void ParticleRollLeft  ( int iID, float fAngle );
void ParticleRollRight ( int iID, float fAngle );

// DBV1 Expressions
DWORD ParticleGetXPositionEx ( int iID );
DWORD ParticleGetYPositionEx ( int iID );
DWORD ParticleGetZPositionEx ( int iID );
DWORD ParticleGetXRotationEx ( int iID );
DWORD ParticleGetYRotationEx ( int iID );
DWORD ParticleGetZRotationEx ( int iID );


#endif _CPOSITION_H_
