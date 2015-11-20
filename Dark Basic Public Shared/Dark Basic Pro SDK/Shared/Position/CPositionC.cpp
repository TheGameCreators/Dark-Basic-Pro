#include "cpositionc.h"
#include ".\..\error\cerror.h"

tagObjectPos* m_pPos;

//tagObjectPos* m_pPos;

//extern tagObjectPos* m_pPos;

extern LPDIRECT3DDEVICE9		m_pD3D;

extern bool UpdatePtr ( int iID );

static void InternalUpdate ( int iID );

void GetCullDataFromModel(int);

// internal

float GetXPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pPos->vecPosition.x;
}

float GetYPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pPos->vecPosition.y;
}

float GetZPosition ( int iID )
{
	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return 0;

	return m_pPos->vecPosition.z;
}

float GetXRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pPos->vecRotate.x;
}

float GetYRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pPos->vecRotate.y;
}

float GetZRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pPos->vecRotate.z;
}

float wrapangleoffset(float da)
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

void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az)
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

void Scale ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// scale an object by the specified scale values

	// set the new values
	m_pPos->vecScale = D3DXVECTOR3 ( fX/100.0f, fY/100.0f, fZ/100.0f );

	// update for clipping
	GetCullDataFromModel ( iID );

	// apply the changes
	InternalUpdate ( iID );
}


void Position ( int iID, float fX, float fY, float fZ )
{
	// set the position of an object

	// update internal data
	if ( !UpdatePtr ( iID ) )
		return;

	// set the new position
	m_pPos->vecPosition = D3DXVECTOR3 ( fX, fY, fZ );
	m_pPos->bHasBeenMovedForResponse=true;

	// apply the changes
	InternalUpdate ( iID );
}


void Rotate ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around all 3 axes

	// store the new rotation values
	m_pPos->vecRotate = D3DXVECTOR3 ( fX, fY, fZ );
	m_pPos->bFreeFlightRotation = false;

	// apply the changes
	InternalUpdate ( iID );
}

void XRotate ( int iID, float fX )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's x axis

	// store the new rotation values
//	m_pPos->vecRotate = D3DXVECTOR3 ( fX, D3DXToDegree ( m_pPos->vecRotate.y ), D3DXToDegree ( m_pPos->vecRotate.z ) );
	m_pPos->vecRotate = D3DXVECTOR3 ( fX, m_pPos->vecRotate.y, m_pPos->vecRotate.z );
	m_pPos->bFreeFlightRotation = false;

	// apply the changes
	InternalUpdate ( iID );
}

void YRotate ( int iID, float fY )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's y axis

	// store the new rotation values
	m_pPos->vecRotate = D3DXVECTOR3 ( m_pPos->vecRotate.x, fY, m_pPos->vecRotate.z );
	m_pPos->bFreeFlightRotation = false;

	// apply the changes
	InternalUpdate ( iID );
}

void ZRotate ( int iID, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's z axis

	// store the new rotation values
	m_pPos->vecRotate = D3DXVECTOR3 ( m_pPos->vecRotate.x, m_pPos->vecRotate.y, fZ );
	m_pPos->bFreeFlightRotation = false;

	// apply the changes
	InternalUpdate ( iID );
}

void Point ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	/* LEEFIX - 041002 - LOOK not tied to rotation of object
	// point an object in a direction
	m_pPos->vecLook.x = fX - m_pPos->vecPosition.x;
	m_pPos->vecLook.y = fY - m_pPos->vecPosition.y;
	m_pPos->vecLook.z = fZ - m_pPos->vecPosition.z;
	D3DXVec3Normalize(&m_pPos->vecLook,&m_pPos->vecLook);
	// set the new look direction
	m_pPos->vecLook = D3DXVECTOR3 ( fX, fY, fZ );
	*/

	// rotation from xyz diff
	GetAngleFromPoint ( m_pPos->vecPosition.x, m_pPos->vecPosition.y, m_pPos->vecPosition.z,
						fX, fY, fZ, &m_pPos->vecRotate.x, &m_pPos->vecRotate.y, &m_pPos->vecRotate.z);

	// apply the changes
	InternalUpdate ( iID );
}

void SetLook ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecLook.x = fX;
	m_pPos->vecLook.y = fY;
	m_pPos->vecLook.z = fZ;
}

void Move ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// moves an object forward in it's current direction

	// update internal data
	if ( !UpdatePtr ( iID ) )
		return;
	
	// update the position vector
//	m_pPos->vecPosition -= ( fStep * m_pPos->vecLook );
//	m_pPos->vecPosition -= ( 0.1f * m_pPos->vecLook );

	// mike : Always move INTO the Z
	m_pPos->vecPosition += ( fStep * m_pPos->vecLook );
	m_pPos->bHasBeenMovedForResponse=true;

	// apply changes
	InternalUpdate ( iID );
}

void MoveLeft ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecPosition -= fStep * m_pPos->vecRight;
	m_pPos->bHasBeenMovedForResponse=true;

	InternalUpdate ( iID );
}

void MoveRight ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecPosition += fStep * m_pPos->vecRight;
	m_pPos->bHasBeenMovedForResponse=true;

	InternalUpdate ( iID );
}

void MoveUp ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecPosition += ( fStep * m_pPos->vecUp );
	m_pPos->bHasBeenMovedForResponse=true;

	InternalUpdate ( iID );
}

void MoveDown ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecPosition -= ( fStep * m_pPos->vecUp );
	m_pPos->bHasBeenMovedForResponse=true;

	InternalUpdate ( iID );
}

void TurnLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.x -= fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void TurnRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.x += fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void PitchUp ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.y += fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void PitchDown ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.y -= fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void RollLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.z -= fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void RollRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pPos->vecYawPitchRoll.z += fAngle;
	m_pPos->bFreeFlightRotation = true;

	InternalUpdate ( iID );
}

void InternalUpdate ( int iID )
{
	// apply any changes to the object e.g. position

	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return;

	// variable declarations
	D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	bool		bUpdate = false;									// used to update look, up and right vector

	// apply scaling to the object
	D3DXMatrixScaling ( &matScale, m_pPos->vecScale.x, m_pPos->vecScale.y, m_pPos->vecScale.z );
	
	// apply translation to the object
	D3DXMatrixTranslation ( &matTranslation, m_pPos->vecPosition.x, m_pPos->vecPosition.y, m_pPos->vecPosition.z );

	// setup rotation matrices
	if ( m_pPos->bFreeFlightRotation )
	{
// LEEFIX - 0941002 - Should be converted to radians 
//		D3DXMatrixRotationYawPitchRoll ( &matRotation, m_pPos->vecYawPitchRoll.x, m_pPos->vecYawPitchRoll.y, m_pPos->vecYawPitchRoll.z ); 
		D3DXMatrixRotationYawPitchRoll ( &matRotation,	D3DXToRadian ( m_pPos->vecYawPitchRoll.x ),
														D3DXToRadian ( m_pPos->vecYawPitchRoll.y ),
														D3DXToRadian ( m_pPos->vecYawPitchRoll.z ) ); 
	}
	else
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pPos->vecRotate.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pPos->vecRotate.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pPos->vecRotate.z ) );	// z rotation

		// build final rotation matrix
		matRotation = matRotateX * matRotateY * matRotateZ;
	}

	// Apply pivot if any
	if ( m_pPos->bApplyPivot )
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pPos->vecPivot.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pPos->vecPivot.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pPos->vecPivot.z ) );	// z rotation

		// build final rotation matrix
		D3DXMATRIX matPivotRotation = matRotateX * matRotateY * matRotateZ;

		// modify current rotation
		matRotation = matPivotRotation * matRotation;
	}

	// scale first, then rotation, then translation (notran is a quick way of stripping translation for colcenter calc)
	m_pPos->matObjectNoTran = matScale * matRotation;
	m_pPos->matObject = m_pPos->matObjectNoTran * matTranslation;

	
	// now we have to update the look, up and right vector, we do this because
	// when we want to move an object we move it using these vectors so we need
	// to update them to reflect any changes setup by the rotations

	// if any of the rotation values have changed then we must reset the look, up
	// and right vectors to their original states
	// LEEFIX - 171002 - Ensure free flight is seperate from euler
	if( m_pPos->bFreeFlightRotation==false )
	{
		if ( m_pPos->vecLast != m_pPos->vecRotate )
		{
			// save the current rotation vector
			m_pPos->vecLast = m_pPos->vecRotate;

			// regenerate the look, up and right vectors
			m_pPos->vecLook  = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
			m_pPos->vecUp    = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
			m_pPos->vecRight = D3DXVECTOR3 ( 1, 0, 0 );		// right vector
		
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
		if ( m_pPos->bFreeFlightRotation )
		{
			// free flight modifies lookupright directly (uses current rotation matrix)
			D3DXVec3TransformCoord ( &m_pPos->vecLook, &m_pPos->vecLook,		&matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecRight,   &m_pPos->vecRight,    &matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecUp,   &m_pPos->vecUp,			&matRotation );
		}
		else
		{
			// x rotation
			D3DXMatrixRotationAxis ( &matRotation, &m_pPos->vecRight, D3DXToRadian ( m_pPos->vecRotate.x ) );
			D3DXVec3TransformCoord ( &m_pPos->vecLook, &m_pPos->vecLook,  &matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecUp,   &m_pPos->vecUp,    &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), D3DXToRadian ( m_pPos->vecRotate.y ) );
			D3DXVec3TransformCoord ( &m_pPos->vecLook, &m_pPos->vecLook, &matRotation );
	// LEEFIX - 051002 - Y rotation changes right, not up which stays the same throughout a Y rotation
	//		D3DXVec3TransformCoord ( &m_pPos->vecUp,   &m_pPos->vecUp,   &matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecRight,   &m_pPos->vecRight,   &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &m_pPos->vecLook, D3DXToRadian ( m_pPos->vecRotate.z ) );
	// LEEFIX - 051002 - Z rotation changes right, not look which stays the same throughout a Z rotation
	//		D3DXVec3TransformCoord ( &m_pPos->vecLook, &m_pPos->vecLook, &matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecRight, &m_pPos->vecRight, &matRotation );
			D3DXVec3TransformCoord ( &m_pPos->vecUp,   &m_pPos->vecUp,   &matRotation );
		}
	}

	// May need to recalc colcenter if used later
	m_pPos->bColCenterUpdated=false;
}

D3DXVECTOR3 GetPosVec ( int iID )
{
	return m_pPos->vecPosition;
}

D3DXVECTOR3 GetRotVec ( int iID )
{
	return m_pPos->vecRotate;
}

DWORD GetXPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pPos->vecPosition.x;
}

DWORD GetYPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}	
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pPos->vecPosition.y;
}

DWORD GetZPositionEx ( int iID )
{
	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return 0;

	return *(DWORD*)&m_pPos->vecPosition.z;
}

DWORD GetXRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pPos->vecRotate.x;
}

DWORD GetYRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pPos->vecRotate.y;
}

DWORD GetZRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pPos->vecRotate.z;
}
