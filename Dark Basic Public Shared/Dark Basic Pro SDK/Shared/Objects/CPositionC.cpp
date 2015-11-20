
//
// Position Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE COMMON ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include ".\..\Objects\CommonC.h"

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL MANAGEMENT FUNCTIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK float GetXPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];

	return pObject->position.vecPosition.x;
}

DARKSDK float GetYPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecPosition.y;
}

DARKSDK float GetZPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecPosition.z;
}

DARKSDK float GetXRotation ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecRotate.x;
}

DARKSDK float GetYRotation ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecRotate.y;
}

DARKSDK float GetZRotation ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0.0f;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecRotate.z;
}

DARKSDK D3DXVECTOR3 GetPosVec ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return D3DXVECTOR3(0,0,0);

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecPosition;
}

DARKSDK D3DXVECTOR3 GetRotVec ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return D3DXVECTOR3(0,0,0);

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return pObject->position.vecRotate;
}

//////////////////////////////////////////////////////////////////////////////////
// POSITION COMMANDS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Scale ( int iID, float fX, float fY, float fZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// scale down
	fX /= 100.0f;
	fY /= 100.0f;
	fZ /= 100.0f;

	// set the new scale value
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecScale = D3DXVECTOR3 ( fX, fY, fZ );

	// repcalculate scaled radius for visibility culling
	float fBiggestScale = fX;
	if ( fY > fBiggestScale ) fBiggestScale = fY;
	if ( fZ > fBiggestScale ) fBiggestScale = fZ;
	pObject->collision.fScaledRadius = pObject->collision.fRadius * fBiggestScale;

	// leefix - 010306 - u60 - this is cumilative
	if ( pObject->collision.fScaledLargestRadius > 0.0f )
	{
		pObject->collision.fScaledLargestRadius = pObject->collision.fLargestRadius * fBiggestScale;
	}

	// lee - 060406 - u6rc6 - shadow casters have a larger visual cull radius
	if ( pObject->bCastsAShadow==true )
	{
		// increase largest range to encompass possible shadow
		if ( pObject->collision.fScaledLargestRadius > 0.0f ) 
		{
			pObject->collision.fScaledLargestRadius = pObject->collision.fLargestRadius;
			pObject->collision.fScaledLargestRadius += 3000.0f;
		}
	}

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void Position  ( int iID, float fX, float fY, float fZ, int iUpdateAbsWorldImmediately )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void Position ( int iID, float fX, float fY, float fZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object position setting
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecLastPosition = pObject->position.vecPosition;
	pObject->position.vecPosition = D3DXVECTOR3 ( fX, fY, fZ );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void Rotate ( int iID, float fX, float fY, float fZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, false );

	// object rotation setting
	pObject->position.vecRotate = D3DXVECTOR3 ( fX, fY, fZ );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void XRotate ( int iID, float fX )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, false );

	// object rotation setting
	pObject->position.vecRotate.x = fX;

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void YRotate ( int iID, float fY )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, false );

	// object rotation setting
	pObject->position.vecRotate.y = fY;

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void ZRotate ( int iID, float fZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, false );

	// object rotation setting
	pObject->position.vecRotate.z = fZ;

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void Point ( int iID, float fX, float fY, float fZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, false );

	// object rotation setting
	GetAngleFromPoint ( pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z,
						fX, fY, fZ, &pObject->position.vecRotate.x, &pObject->position.vecRotate.y, &pObject->position.vecRotate.z);

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void Move ( int iID, float fStep )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// move object position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecLastPosition = pObject->position.vecPosition;
	pObject->position.vecPosition += ( fStep * pObject->position.vecLook );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void MoveLeft ( int iID, float fStep )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// move object position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecPosition -= ( fStep * pObject->position.vecRight );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void MoveRight ( int iID, float fStep )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// move object position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecPosition += ( fStep * pObject->position.vecRight );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void MoveUp ( int iID, float fStep )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// move object position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecPosition += ( fStep * pObject->position.vecUp );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void MoveDown ( int iID, float fStep )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// move object position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.vecPosition -= ( fStep * pObject->position.vecUp );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void TurnLeft ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationY ( &matRotation, D3DXToRadian ( -fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void TurnRight ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationY ( &matRotation, D3DXToRadian ( fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void PitchUp ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationX ( &matRotation, D3DXToRadian ( -fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void PitchDown ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationX ( &matRotation, D3DXToRadian ( fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void RollLeft ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationZ ( &matRotation, D3DXToRadian ( fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

DARKSDK void RollRight ( int iID, float fAngle )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// rotation object
	sObject* pObject = g_ObjectList [ iID ];

	// check for rotation conversion
	CheckRotationConversion( pObject, true );

	// object rotation setting
	D3DXMATRIX matRotation;
	D3DXMatrixRotationZ ( &matRotation, D3DXToRadian ( -fAngle ) );
	pObject->position.matFreeFlightRotate = matRotation * pObject->position.matFreeFlightRotate;

    AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );

	// regenerate look vectors
	RegenerateLookVectors( pObject );

	// also triggers move-response-flag
	pObject->collision.bHasBeenMovedForResponse=true;
	pObject->collision.bColCenterUpdated=false;

	// leefix - 011103 - proper world recalc (with glue)
	CalcObjectWorld ( pObject );
}

//////////////////////////////////////////////////////////////////////////////////
// POSITION EXPRESSIONS //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK DWORD GetXPositionEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];

	#ifdef DARKSDK_COMPILE
		if ( pObject->position.bGlued == true )
		{
			sObject* pObjectGlueTo = g_ObjectList [ pObject->position.iGluedToObj ];

			CalcObjectWorld ( pObjectGlueTo );
			CalcObjectWorld ( pObject );

			return *(DWORD*)&pObject->position.matWorld._41;
		}
	#endif

	return *(DWORD*)&pObject->position.vecPosition.x;
}

DARKSDK DWORD GetYPositionEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];

	#ifdef DARKSDK_COMPILE
		if ( pObject->position.bGlued == true )
		{
			sObject* pObjectGlueTo = g_ObjectList [ pObject->position.iGluedToObj ];

			CalcObjectWorld ( pObjectGlueTo );
			CalcObjectWorld ( pObject );

			return *(DWORD*)&pObject->position.matWorld._42;
		}
	#endif


	return *(DWORD*)&pObject->position.vecPosition.y;
}

DARKSDK DWORD GetZPositionEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];

	#ifdef DARKSDK_COMPILE
		if ( pObject->position.bGlued == true )
		{
			sObject* pObjectGlueTo = g_ObjectList [ pObject->position.iGluedToObj ];

			CalcObjectWorld ( pObjectGlueTo );
			CalcObjectWorld ( pObject );

			return *(DWORD*)&pObject->position.matWorld._43;
		}
	#endif

	return *(DWORD*)&pObject->position.vecPosition.z;
}

DARKSDK DWORD GetXRotationEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return *(DWORD*)&pObject->position.vecRotate.x;
}

DARKSDK DWORD GetYRotationEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return *(DWORD*)&pObject->position.vecRotate.y;
}

DARKSDK DWORD GetZRotationEx ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// object information
	sObject* pObject = g_ObjectList [ iID ];
	return *(DWORD*)&pObject->position.vecRotate.z;
}

///////////////////////

#ifdef DARKSDK_COMPILE

float dbtObjectGetXPosition ( int iID )
{
	return GetXPosition ( iID );
}

float dbtObjectGetYPosition ( int iID )
{
	return GetYPosition ( iID );
}

float dbtObjectGetZPosition ( int iID )
{
	return GetZPosition ( iID );
}

float dbtObjectGetXRotation ( int iID )
{
	return GetXRotation ( iID );
}

float dbtObjectGetYRotation ( int iID )
{
	return GetYRotation ( iID );
}

float dbtObjectGetZRotation ( int iID )
{
	return GetZRotation ( iID );
}

void dbtObjectScale ( int iID, float fX, float fY, float fZ )
{
	Scale ( iID, fX, fY, fZ );
}

void dbtObjectPosition ( int iID, float fX, float fY, float fZ )
{
	Position ( iID, fX, fY, fZ );
}

void dbtObjectRotate ( int iID, float fX, float fY, float fZ )
{
	Rotate ( iID, fX, fY, fZ );
}

void dbtObjectXRotate ( int iID, float fX )
{
	XRotate ( iID, fX );
}

void dbtObjectYRotate ( int iID, float fY )
{
	YRotate ( iID, fY );
}

void dbtObjectZRotate ( int iID, float fZ )
{
	ZRotate ( iID, fZ );
}

void dbtObjectPoint ( int iID, float fX, float fY, float fZ )
{
	Point ( iID, fX, fY, fZ );
}

void dbtObjectMove ( int iID, float fStep )
{
	Move ( iID, fStep );
}

void dbtObjectMoveUp ( int iID, float fStep )
{
	MoveUp ( iID, fStep );
}

void dbtObjectMoveDown ( int iID, float fStep )
{
	MoveDown ( iID, fStep );
}

void dbtObjectMoveLeft ( int iID, float fStep )
{
	MoveLeft ( iID, fStep );
}

void dbtObjectMoveRight ( int iID, float fStep )
{
	MoveRight ( iID, fStep );
}

void dbtObjectTurnLeft ( int iID, float fAngle )
{
	TurnLeft ( iID, fAngle );
}

void dbtObjectTurnRight ( int iID, float fAngle )
{
	TurnRight ( iID, fAngle );
}

void dbtObjectPitchUp ( int iID, float fAngle )
{
	PitchUp ( iID, fAngle );
}

void dbtObjectPitchDown ( int iID, float fAngle )
{
	PitchDown ( iID, fAngle );
}

void dbtObjectRollLeft ( int iID, float fAngle )
{
	RollLeft ( iID, fAngle );
}

void dbtObjectRollRight ( int iID, float fAngle )
{
	RollRight ( iID, fAngle );
}

DWORD dbtObjectGetXPositionEx ( int iID )
{
	return GetXPositionEx ( iID );
}

DWORD dbtObjectGetYPositionEx ( int iID )
{
	return GetYPositionEx ( iID );
}

DWORD dbtObjectGetZPositionEx ( int iID )
{
	return GetZPositionEx ( iID );
}

DWORD dbtObjectGetXRotationEx ( int iID )
{
	return GetXRotationEx ( iID );
}

DWORD dbtObjectGetYRotationEx ( int iID )
{
	return GetYRotationEx ( iID );
}

DWORD dbtObjectGetZRotationEx ( int iID )
{
	return GetZRotationEx ( iID );
}

#endif
