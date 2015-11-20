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
#ifndef __GNUC__
void InternalUpdate ( int iID );		// update any changes to camera
#endif


// Internal Functions
#ifndef __GNUC__
float GetXPosition ( int iID );
float GetYPosition ( int iID );
float GetZPosition ( int iID );
float GetXRotation ( int iID );
float GetYRotation ( int iID );
float GetZRotation ( int iID );


// DBV1 Euler
void Scale     ( int iID, float fX, float fY, float fZ );
void Position  ( int iID, float fX, float fY, float fZ );


void Rotate    ( int iID, float fX, float fY, float fZ );
void XRotate   ( int iID, float fX );
void YRotate   ( int iID, float fY );
void ZRotate   ( int iID, float fZ );
void Point     ( int iID, float fX, float fY, float fZ );

void Move      ( int iID, float fStep );
void MoveUp    ( int iID, float fStep );
void MoveDown  ( int iID, float fStep );
void MoveLeft  ( int iID, float fStep );
void MoveRight ( int iID, float fStep );

// DBV1 Freeflight
void TurnLeft  ( int iID, float fAngle );
void TurnRight ( int iID, float fAngle );
void PitchUp   ( int iID, float fAngle );
void PitchDown ( int iID, float fAngle );
void RollLeft  ( int iID, float fAngle );
void RollRight ( int iID, float fAngle );

// DBV1 Expressions
DWORD GetXPositionEx ( int iID );
DWORD GetYPositionEx ( int iID );
DWORD GetZPositionEx ( int iID );
DWORD GetXRotationEx ( int iID );
DWORD GetYRotationEx ( int iID );
DWORD GetZRotationEx ( int iID );
#endif

#endif _CPOSITION_H_
