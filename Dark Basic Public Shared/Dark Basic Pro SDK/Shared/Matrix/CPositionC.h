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


void MatrixInternalUpdate ( int iID );		// update any changes to camera


// Internal Functions
float MatrixGetXPosition ( int iID );
float MatrixGetYPosition ( int iID );
float MatrixGetZPosition ( int iID );
float MatrixGetXRotation ( int iID );
float MatrixGetYRotation ( int iID );
float MatrixGetZRotation ( int iID );


// DBV1 Euler
void MatrixScale     ( int iID, float fX, float fY, float fZ );
void MatrixPosition  ( int iID, float fX, float fY, float fZ );


void MatrixRotate    ( int iID, float fX, float fY, float fZ );
void MatrixXRotate   ( int iID, float fX );
void MatrixYRotate   ( int iID, float fY );
void MatrixZRotate   ( int iID, float fZ );
void MatrixPoint     ( int iID, float fX, float fY, float fZ );

void MatrixMove      ( int iID, float fStep );
void MatrixMoveUp    ( int iID, float fStep );
void MatrixMoveDown  ( int iID, float fStep );
void MatrixMoveLeft  ( int iID, float fStep );
void MatrixMoveRight ( int iID, float fStep );

// DBV1 Freeflight
void MatrixTurnLeft  ( int iID, float fAngle );
void MatrixTurnRight ( int iID, float fAngle );
void MatrixPitchUp   ( int iID, float fAngle );
void MatrixPitchDown ( int iID, float fAngle );
void MatrixRollLeft  ( int iID, float fAngle );
void MatrixRollRight ( int iID, float fAngle );

// DBV1 Expressions
DWORD MatrixGetXPositionEx ( int iID );
DWORD MatrixGetYPositionEx ( int iID );
DWORD MatrixGetZPositionEx ( int iID );
DWORD MatrixGetXRotationEx ( int iID );
DWORD MatrixGetYRotationEx ( int iID );
DWORD MatrixGetZRotationEx ( int iID );

#endif _CPOSITION_H_
