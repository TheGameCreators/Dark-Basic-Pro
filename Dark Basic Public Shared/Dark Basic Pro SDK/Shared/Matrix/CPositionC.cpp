#include "cpositionc.h"
#include ".\..\error\cerror.h"

tagObjectPos* m_pMatrixPos;

//tagObjectPos* m_pMatrixPos;

//extern tagObjectPos* m_pMatrixPos;

extern LPDIRECT3DDEVICE9		m_pD3D;

extern bool UpdateMatrixPtr ( int iID );

static void InternalMatrixUpdate ( int iID );

void GetCullDataFromModel(int);

// internal

float MatrixGetXPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pMatrixPos->vecPosition.x;
}

float MatrixGetYPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pMatrixPos->vecPosition.y;
}

float MatrixGetZPosition ( int iID )
{
	// update internal pointer
	if ( !UpdateMatrixPtr ( iID ) )
		return 0;

	return m_pMatrixPos->vecPosition.z;
}

float MatrixGetXRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pMatrixPos->vecRotate.x;
}

float MatrixGetYRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pMatrixPos->vecRotate.y;
}

float MatrixGetZRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pMatrixPos->vecRotate.z;
}


static float wrapangleoffset(float da)
{
	int breakout=100;
	while(da<0.0f || da>=360.0f)
	{
		if(da<0.0f) da=da+360.0f;
		if(da>=360.0f) da=da-360.0f;
		breakout--;
		if(breakout==0) break;
	}
	if(breakout==0) da=0.0f;
	return da;
}

static void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az)
{
	D3DXVECTOR3 Vector;
	Vector.x = x2-x1;
	Vector.y = y2-y1;
	Vector.z = z2-z1;

	// Find Y and then X axis rotation
	double yangle=atan2(Vector.x, Vector.z);
	if(yangle<0.0) yangle+=D3DXToRadian(360.0);
	if(yangle>=D3DXToRadian(360.0)) yangle-=D3DXToRadian(360.0);

	D3DXMATRIX yrotate;
	D3DXMatrixRotationY ( &yrotate, (float)-yangle );
	D3DXVec3TransformCoord ( &Vector, &Vector, &yrotate );

	double xangle=-atan2(Vector.y, Vector.z);
	if(xangle<0.0) xangle+=D3DXToRadian(360.0);
	if(xangle>=D3DXToRadian(360.0)) xangle-=D3DXToRadian(360.0);

	*ax = wrapangleoffset(D3DXToDegree((float)xangle));
	*ay = wrapangleoffset(D3DXToDegree((float)yangle));
	*az = 0.0f;
}

// commands

void MatrixScale ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// scale an object by the specified scale values

	// set the new values
	m_pMatrixPos->vecScale = D3DXVECTOR3 ( fX/100.0f, fY/100.0f, fZ/100.0f );

	// update for clipping
	GetCullDataFromModel ( iID );

	// apply the changes
	MatrixInternalUpdate ( iID );
}


void MatrixPosition ( int iID, float fX, float fY, float fZ )
{
	// set the position of an object

	// update internal data
	if ( !UpdateMatrixPtr ( iID ) )
		return;

	// set the new position
	m_pMatrixPos->vecPosition = D3DXVECTOR3 ( fX, fY, fZ );
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	// apply the changes
	MatrixInternalUpdate ( iID );
}


void MatrixRotate ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around all 3 axes

	// store the new rotation values
	m_pMatrixPos->vecRotate = D3DXVECTOR3 ( fX, fY, fZ );
	m_pMatrixPos->bFreeFlightRotation = false;

	// apply the changes
	MatrixInternalUpdate ( iID );
}

void MatrixXRotate ( int iID, float fX )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's x axis

	// store the new rotation values
//	m_pMatrixPos->vecRotate = D3DXVECTOR3 ( fX, D3DXToDegree ( m_pMatrixPos->vecRotate.y ), D3DXToDegree ( m_pMatrixPos->vecRotate.z ) );
	m_pMatrixPos->vecRotate = D3DXVECTOR3 ( fX, m_pMatrixPos->vecRotate.y, m_pMatrixPos->vecRotate.z );
	m_pMatrixPos->bFreeFlightRotation = false;

	// apply the changes
	MatrixInternalUpdate ( iID );
}

void MatrixYRotate ( int iID, float fY )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's y axis

	// store the new rotation values
	m_pMatrixPos->vecRotate = D3DXVECTOR3 ( m_pMatrixPos->vecRotate.x, fY, m_pMatrixPos->vecRotate.z );
	m_pMatrixPos->bFreeFlightRotation = false;

	// apply the changes
	MatrixInternalUpdate ( iID );
}

void MatrixZRotate ( int iID, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's z axis

	// store the new rotation values
	m_pMatrixPos->vecRotate = D3DXVECTOR3 ( m_pMatrixPos->vecRotate.x, m_pMatrixPos->vecRotate.y, fZ );
	m_pMatrixPos->bFreeFlightRotation = false;

	// apply the changes
	MatrixInternalUpdate ( iID );
}

void MatrixPoint ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	/* LEEFIX - 041002 - LOOK not tied to rotation of object
	// point an object in a direction
	m_pMatrixPos->vecLook.x = fX - m_pMatrixPos->vecPosition.x;
	m_pMatrixPos->vecLook.y = fY - m_pMatrixPos->vecPosition.y;
	m_pMatrixPos->vecLook.z = fZ - m_pMatrixPos->vecPosition.z;
	D3DXVec3Normalize(&m_pMatrixPos->vecLook,&m_pMatrixPos->vecLook);
	// set the new look direction
	m_pMatrixPos->vecLook = D3DXVECTOR3 ( fX, fY, fZ );
	*/

	// rotation from xyz diff
	GetAngleFromPoint ( m_pMatrixPos->vecPosition.x, m_pMatrixPos->vecPosition.y, m_pMatrixPos->vecPosition.z,
						fX, fY, fZ, &m_pMatrixPos->vecRotate.x, &m_pMatrixPos->vecRotate.y, &m_pMatrixPos->vecRotate.z);

	// apply the changes
	MatrixInternalUpdate ( iID );
}

void MatrixSetLook ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecLook.x = fX;
	m_pMatrixPos->vecLook.y = fY;
	m_pMatrixPos->vecLook.z = fZ;
}

void MatrixMove ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// moves an object forward in it's current direction

	// update internal data
	if ( !UpdateMatrixPtr ( iID ) )
		return;
	
	// update the position vector
//	m_pMatrixPos->vecPosition -= ( fStep * m_pMatrixPos->vecLook );
//	m_pMatrixPos->vecPosition -= ( 0.1f * m_pMatrixPos->vecLook );

	// mike : Always move INTO the Z
	m_pMatrixPos->vecPosition += ( fStep * m_pMatrixPos->vecLook );
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	// apply changes
	MatrixInternalUpdate ( iID );
}

void MatrixMoveLeft ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecPosition -= fStep * m_pMatrixPos->vecRight;
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	MatrixInternalUpdate ( iID );
}

void MatrixMoveRight ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecPosition += fStep * m_pMatrixPos->vecRight;
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	MatrixInternalUpdate ( iID );
}

void MatrixMoveUp ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecPosition += ( fStep * m_pMatrixPos->vecUp );
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	MatrixInternalUpdate ( iID );
}

void MatrixMoveDown ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecPosition -= ( fStep * m_pMatrixPos->vecUp );
	m_pMatrixPos->bHasBeenMovedForResponse=true;

	MatrixInternalUpdate ( iID );
}

void MatrixTurnLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.x -= fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixTurnRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.x += fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixPitchUp ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.y += fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixPitchDown ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.y -= fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixRollLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.z -= fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixRollRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pMatrixPos->vecYawPitchRoll.z += fAngle;
	m_pMatrixPos->bFreeFlightRotation = true;

	MatrixInternalUpdate ( iID );
}

void MatrixInternalUpdate ( int iID )
{
	// apply any changes to the object e.g. position

	// update internal pointer
	if ( !UpdateMatrixPtr ( iID ) )
		return;

	// variable declarations
	D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	bool		bUpdate = false;									// used to update look, up and right vector

	// apply scaling to the object
	D3DXMatrixScaling ( &matScale, m_pMatrixPos->vecScale.x, m_pMatrixPos->vecScale.y, m_pMatrixPos->vecScale.z );
	
	// apply translation to the object
	D3DXMatrixTranslation ( &matTranslation, m_pMatrixPos->vecPosition.x, m_pMatrixPos->vecPosition.y, m_pMatrixPos->vecPosition.z );

	// setup rotation matrices
	if ( m_pMatrixPos->bFreeFlightRotation )
	{
// LEEFIX - 0941002 - Should be converted to radians 
//		D3DXMatrixRotationYawPitchRoll ( &matRotation, m_pMatrixPos->vecYawPitchRoll.x, m_pMatrixPos->vecYawPitchRoll.y, m_pMatrixPos->vecYawPitchRoll.z ); 
		D3DXMatrixRotationYawPitchRoll ( &matRotation,	D3DXToRadian ( m_pMatrixPos->vecYawPitchRoll.x ),
														D3DXToRadian ( m_pMatrixPos->vecYawPitchRoll.y ),
														D3DXToRadian ( m_pMatrixPos->vecYawPitchRoll.z ) ); 
	}
	else
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pMatrixPos->vecRotate.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pMatrixPos->vecRotate.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pMatrixPos->vecRotate.z ) );	// z rotation

		// build final rotation matrix
		matRotation = matRotateX * matRotateY * matRotateZ;
	}

	// Apply pivot if any
	if ( m_pMatrixPos->bApplyPivot )
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pMatrixPos->vecPivot.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pMatrixPos->vecPivot.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pMatrixPos->vecPivot.z ) );	// z rotation

		// build final rotation matrix
		D3DXMATRIX matPivotRotation = matRotateX * matRotateY * matRotateZ;

		// modify current rotation
		matRotation = matPivotRotation * matRotation;
	}

	// scale first, then rotation, then translation (notran is a quick way of stripping translation for colcenter calc)
	m_pMatrixPos->matObjectNoTran = matScale * matRotation;
	m_pMatrixPos->matObject = m_pMatrixPos->matObjectNoTran * matTranslation;

	
	// now we have to update the look, up and right vector, we do this because
	// when we want to move an object we move it using these vectors so we need
	// to update them to reflect any changes setup by the rotations

	// if any of the rotation values have changed then we must reset the look, up
	// and right vectors to their original states
	// LEEFIX - 171002 - Ensure free flight is seperate from euler
	if( m_pMatrixPos->bFreeFlightRotation==false )
	{
		if ( m_pMatrixPos->vecLast != m_pMatrixPos->vecRotate )
		{
			// save the current rotation vector
			m_pMatrixPos->vecLast = m_pMatrixPos->vecRotate;

			// regenerate the look, up and right vectors
			m_pMatrixPos->vecLook  = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
			m_pMatrixPos->vecUp    = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
			m_pMatrixPos->vecRight = D3DXVECTOR3 ( 1, 0, 0 );		// right vector
		
			// signal that we need to update the vectors
			bUpdate = true;
		}
	}
	else
	{
		// free flight always applies rotation
		bUpdate = true;
	}

	// only update if there has been a change in rotation
	if ( bUpdate )
	{
		if ( m_pMatrixPos->bFreeFlightRotation )
		{
			// free flight modifies lookupright directly (uses current rotation matrix)
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecLook, &m_pMatrixPos->vecLook,		&matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecRight,   &m_pMatrixPos->vecRight,    &matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecUp,   &m_pMatrixPos->vecUp,			&matRotation );
		}
		else
		{
			// x rotation
			D3DXMatrixRotationAxis ( &matRotation, &m_pMatrixPos->vecRight, D3DXToRadian ( m_pMatrixPos->vecRotate.x ) );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecLook, &m_pMatrixPos->vecLook,  &matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecUp,   &m_pMatrixPos->vecUp,    &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), D3DXToRadian ( m_pMatrixPos->vecRotate.y ) );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecLook, &m_pMatrixPos->vecLook, &matRotation );
	// LEEFIX - 051002 - Y rotation changes right, not up which stays the same throughout a Y rotation
	//		D3DXVec3TransformCoord ( &m_pMatrixPos->vecUp,   &m_pMatrixPos->vecUp,   &matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecRight,   &m_pMatrixPos->vecRight,   &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &m_pMatrixPos->vecLook, D3DXToRadian ( m_pMatrixPos->vecRotate.z ) );
	// LEEFIX - 051002 - Z rotation changes right, not look which stays the same throughout a Z rotation
	//		D3DXVec3TransformCoord ( &m_pMatrixPos->vecLook, &m_pMatrixPos->vecLook, &matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecRight, &m_pMatrixPos->vecRight, &matRotation );
			D3DXVec3TransformCoord ( &m_pMatrixPos->vecUp,   &m_pMatrixPos->vecUp,   &matRotation );
		}
	}

	// May need to recalc colcenter if used later
	m_pMatrixPos->bColCenterUpdated=false;
}

D3DXVECTOR3 MatrixGetPosVec ( int iID )
{
	return m_pMatrixPos->vecPosition;
}

D3DXVECTOR3 MatrixGetRotVec ( int iID )
{
	return m_pMatrixPos->vecRotate;
}

DWORD MatrixGetXPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pMatrixPos->vecPosition.x;
}

DWORD MatrixGetYPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}	
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pMatrixPos->vecPosition.y;
}

DWORD MatrixGetZPositionEx ( int iID )
{
	// update internal pointer
	if ( !UpdateMatrixPtr ( iID ) )
		return 0;

	return *(DWORD*)&m_pMatrixPos->vecPosition.z;
}

DWORD MatrixGetXRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pMatrixPos->vecRotate.x;
}

DWORD MatrixGetYRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pMatrixPos->vecRotate.y;
}

DWORD MatrixGetZRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateMatrixPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pMatrixPos->vecRotate.z;
}
