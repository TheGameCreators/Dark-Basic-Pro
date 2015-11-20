#include "cpositionc.h"
#include ".\..\error\cerror.h"

tagObjectPos* m_pTextPos;

//tagObjectPos* m_pTextPos;
//extern tagObjectPos* m_pTextPos;
//extern bool UpdatePtr ( int iID );

extern LPDIRECT3DDEVICE9		m_pD3DMatrix;



static void TextInternalUpdate ( int iID );

void GetCullDataFromModel(int);

extern bool UpdateTextPtr ( int iID );

/*
#ifdef MATRIX_DLL

	extern bool UpdateMatrixPtr ( int iID );
	#define UpdatePtr( p ) UpdateMatrixPtr ( p )

	extern void GetMatrixCullDataFromModel(int);
	#define GetCullDataFromModel( p ) GetMatrixCullDataFromModel ( p )
#endif
	*/

// internal

/*
static void GetCullDataFromModel(int)
{
}
*/

float TextGetXPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pTextPos->vecPosition.x;
}

float TextGetYPosition ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pTextPos->vecPosition.y;
}

float TextGetZPosition ( int iID )
{
	// update internal pointer
	if ( !UpdateTextPtr ( iID ) )
		return 0;

	return m_pTextPos->vecPosition.z;
}

float TextGetXRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pTextPos->vecRotate.x;
}

float TextGetYRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pTextPos->vecRotate.y;
}

float TextGetZRotation ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return m_pTextPos->vecRotate.z;
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

void TextScale ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// scale an object by the specified scale values

	// set the new values
	m_pTextPos->vecScale = D3DXVECTOR3 ( fX/100.0f, fY/100.0f, fZ/100.0f );

	// update for clipping
	GetCullDataFromModel ( iID );

	// apply the changes
	TextInternalUpdate ( iID );
}


void TextPosition ( int iID, float fX, float fY, float fZ )
{
	// set the position of an object

	// update internal data
	if ( !UpdateTextPtr ( iID ) )
		return;

	// set the new position
	m_pTextPos->vecPosition = D3DXVECTOR3 ( fX, fY, fZ );
	m_pTextPos->bHasBeenMovedForResponse=true;

	// apply the changes
	TextInternalUpdate ( iID );
}


void TextRotate ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around all 3 axes

	// store the new rotation values
	m_pTextPos->vecRotate = D3DXVECTOR3 ( fX, fY, fZ );
	m_pTextPos->bFreeFlightRotation = false;

	// apply the changes
	TextInternalUpdate ( iID );
}

void TextXRotate ( int iID, float fX )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's x axis

	// store the new rotation values
//	m_pTextPos->vecRotate = D3DXVECTOR3 ( fX, D3DXToDegree ( m_pTextPos->vecRotate.y ), D3DXToDegree ( m_pTextPos->vecRotate.z ) );
	m_pTextPos->vecRotate = D3DXVECTOR3 ( fX, m_pTextPos->vecRotate.y, m_pTextPos->vecRotate.z );
	m_pTextPos->bFreeFlightRotation = false;

	// apply the changes
	TextInternalUpdate ( iID );
}

void TextYRotate ( int iID, float fY )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's y axis

	// store the new rotation values
	m_pTextPos->vecRotate = D3DXVECTOR3 ( m_pTextPos->vecRotate.x, fY, m_pTextPos->vecRotate.z );
	m_pTextPos->bFreeFlightRotation = false;

	// apply the changes
	TextInternalUpdate ( iID );
}

void TextZRotate ( int iID, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// rotate an object around it's z axis

	// store the new rotation values
	m_pTextPos->vecRotate = D3DXVECTOR3 ( m_pTextPos->vecRotate.x, m_pTextPos->vecRotate.y, fZ );
	m_pTextPos->bFreeFlightRotation = false;

	// apply the changes
	TextInternalUpdate ( iID );
}

void TextPoint ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	/* LEEFIX - 041002 - LOOK not tied to rotation of object
	// point an object in a direction
	m_pTextPos->vecLook.x = fX - m_pTextPos->vecPosition.x;
	m_pTextPos->vecLook.y = fY - m_pTextPos->vecPosition.y;
	m_pTextPos->vecLook.z = fZ - m_pTextPos->vecPosition.z;
	D3DXVec3Normalize(&m_pTextPos->vecLook,&m_pTextPos->vecLook);
	// set the new look direction
	m_pTextPos->vecLook = D3DXVECTOR3 ( fX, fY, fZ );
	*/

	// rotation from xyz diff
	GetAngleFromPoint ( m_pTextPos->vecPosition.x, m_pTextPos->vecPosition.y, m_pTextPos->vecPosition.z,
						fX, fY, fZ, &m_pTextPos->vecRotate.x, &m_pTextPos->vecRotate.y, &m_pTextPos->vecRotate.z);

	// apply the changes
	TextInternalUpdate ( iID );
}

void TextSetLook ( int iID, float fX, float fY, float fZ )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecLook.x = fX;
	m_pTextPos->vecLook.y = fY;
	m_pTextPos->vecLook.z = fZ;
}

void TextMove ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	// moves an object forward in it's current direction

	// update internal data
	if ( !UpdateTextPtr ( iID ) )
		return;
	
	// update the position vector
//	m_pTextPos->vecPosition -= ( fStep * m_pTextPos->vecLook );
//	m_pTextPos->vecPosition -= ( 0.1f * m_pTextPos->vecLook );

	// mike : Always move INTO the Z
	m_pTextPos->vecPosition += ( fStep * m_pTextPos->vecLook );
	m_pTextPos->bHasBeenMovedForResponse=true;

	// apply changes
	TextInternalUpdate ( iID );
}

void TextMoveLeft ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecPosition -= fStep * m_pTextPos->vecRight;
	m_pTextPos->bHasBeenMovedForResponse=true;

	TextInternalUpdate ( iID );
}

void TextMoveRight ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecPosition += fStep * m_pTextPos->vecRight;
	m_pTextPos->bHasBeenMovedForResponse=true;

	TextInternalUpdate ( iID );
}

void TextMoveUp ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecPosition += ( fStep * m_pTextPos->vecUp );
	m_pTextPos->bHasBeenMovedForResponse=true;

	TextInternalUpdate ( iID );
}

void TextMoveDown ( int iID, float fStep )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecPosition -= ( fStep * m_pTextPos->vecUp );
	m_pTextPos->bHasBeenMovedForResponse=true;

	TextInternalUpdate ( iID );
}

void TextTurnLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.x -= fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextTurnRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.x += fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextPitchUp ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.y += fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextPitchDown ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.y -= fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextRollLeft ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.z -= fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextRollRight ( int iID, float fAngle )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return;
	}

	m_pTextPos->vecYawPitchRoll.z += fAngle;
	m_pTextPos->bFreeFlightRotation = true;

	TextInternalUpdate ( iID );
}

void TextInternalUpdate ( int iID )
{
	// apply any changes to the object e.g. position

	// update internal pointer
	if ( !UpdateTextPtr ( iID ) )
		return;

	// variable declarations
	D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	bool		bUpdate = false;									// used to update look, up and right vector

	// apply scaling to the object
	D3DXMatrixScaling ( &matScale, m_pTextPos->vecScale.x, m_pTextPos->vecScale.y, m_pTextPos->vecScale.z );
	
	// apply translation to the object
	D3DXMatrixTranslation ( &matTranslation, m_pTextPos->vecPosition.x, m_pTextPos->vecPosition.y, m_pTextPos->vecPosition.z );

	// setup rotation matrices
	if ( m_pTextPos->bFreeFlightRotation )
	{
// LEEFIX - 0941002 - Should be converted to radians 
//		D3DXMatrixRotationYawPitchRoll ( &matRotation, m_pTextPos->vecYawPitchRoll.x, m_pTextPos->vecYawPitchRoll.y, m_pTextPos->vecYawPitchRoll.z ); 
		D3DXMatrixRotationYawPitchRoll ( &matRotation,	D3DXToRadian ( m_pTextPos->vecYawPitchRoll.x ),
														D3DXToRadian ( m_pTextPos->vecYawPitchRoll.y ),
														D3DXToRadian ( m_pTextPos->vecYawPitchRoll.z ) ); 
	}
	else
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pTextPos->vecRotate.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pTextPos->vecRotate.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pTextPos->vecRotate.z ) );	// z rotation

		// build final rotation matrix
		matRotation = matRotateX * matRotateY * matRotateZ;
	}

	// Apply pivot if any
	if ( m_pTextPos->bApplyPivot )
	{
		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( m_pTextPos->vecPivot.x ) );	// x rotation
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( m_pTextPos->vecPivot.y ) );	// y rotation
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( m_pTextPos->vecPivot.z ) );	// z rotation

		// build final rotation matrix
		D3DXMATRIX matPivotRotation = matRotateX * matRotateY * matRotateZ;

		// modify current rotation
		matRotation = matPivotRotation * matRotation;
	}

	// scale first, then rotation, then translation (notran is a quick way of stripping translation for colcenter calc)
	m_pTextPos->matObjectNoTran = matScale * matRotation;
	m_pTextPos->matObject = m_pTextPos->matObjectNoTran * matTranslation;

	
	// now we have to update the look, up and right vector, we do this because
	// when we want to move an object we move it using these vectors so we need
	// to update them to reflect any changes setup by the rotations

	// if any of the rotation values have changed then we must reset the look, up
	// and right vectors to their original states
	// LEEFIX - 171002 - Ensure free flight is seperate from euler
	if( m_pTextPos->bFreeFlightRotation==false )
	{
		if ( m_pTextPos->vecLast != m_pTextPos->vecRotate )
		{
			// save the current rotation vector
			m_pTextPos->vecLast = m_pTextPos->vecRotate;

			// regenerate the look, up and right vectors
			m_pTextPos->vecLook  = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
			m_pTextPos->vecUp    = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
			m_pTextPos->vecRight = D3DXVECTOR3 ( 1, 0, 0 );		// right vector
		
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
		if ( m_pTextPos->bFreeFlightRotation )
		{
			// free flight modifies lookupright directly (uses current rotation matrix)
			D3DXVec3TransformCoord ( &m_pTextPos->vecLook, &m_pTextPos->vecLook,		&matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecRight,   &m_pTextPos->vecRight,    &matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecUp,   &m_pTextPos->vecUp,			&matRotation );
		}
		else
		{
			// x rotation
			D3DXMatrixRotationAxis ( &matRotation, &m_pTextPos->vecRight, D3DXToRadian ( m_pTextPos->vecRotate.x ) );
			D3DXVec3TransformCoord ( &m_pTextPos->vecLook, &m_pTextPos->vecLook,  &matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecUp,   &m_pTextPos->vecUp,    &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), D3DXToRadian ( m_pTextPos->vecRotate.y ) );
			D3DXVec3TransformCoord ( &m_pTextPos->vecLook, &m_pTextPos->vecLook, &matRotation );
	// LEEFIX - 051002 - Y rotation changes right, not up which stays the same throughout a Y rotation
	//		D3DXVec3TransformCoord ( &m_pTextPos->vecUp,   &m_pTextPos->vecUp,   &matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecRight,   &m_pTextPos->vecRight,   &matRotation );

			D3DXMatrixRotationAxis ( &matRotation, &m_pTextPos->vecLook, D3DXToRadian ( m_pTextPos->vecRotate.z ) );
	// LEEFIX - 051002 - Z rotation changes right, not look which stays the same throughout a Z rotation
	//		D3DXVec3TransformCoord ( &m_pTextPos->vecLook, &m_pTextPos->vecLook, &matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecRight, &m_pTextPos->vecRight, &matRotation );
			D3DXVec3TransformCoord ( &m_pTextPos->vecUp,   &m_pTextPos->vecUp,   &matRotation );
		}
	}

	// May need to recalc colcenter if used later
	m_pTextPos->bColCenterUpdated=false;
}

D3DXVECTOR3 TextGetPosVec ( int iID )
{
	return m_pTextPos->vecPosition;
}

D3DXVECTOR3 TextGetRotVec ( int iID )
{
	return m_pTextPos->vecRotate;
}

DWORD TextGetXPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pTextPos->vecPosition.x;
}

DWORD TextGetYPositionEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}	
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pTextPos->vecPosition.y;
}

DWORD TextGetZPositionEx ( int iID )
{
	// update internal pointer
	if ( !UpdateTextPtr ( iID ) )
		return 0;

	return *(DWORD*)&m_pTextPos->vecPosition.z;
}

DWORD TextGetXRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pTextPos->vecRotate.x;
}

DWORD TextGetYRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pTextPos->vecRotate.y;
}

DWORD TextGetZRotationEx ( int iID )
{
	// update internal data
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdateTextPtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_B3DMODELNOTEXISTS);
		return 0;
	}

	return *(DWORD*)&m_pTextPos->vecRotate.z;
}
