#include "stdafx.h"



PhysicsObject::PhysicsObject(int id)
//leefixd-030306-u60-change
//changed isFirstGFXUpdate to false, because when set up positions, it then resets any positions set before updatephysics
//:inUse(true),hasGarbage(false),objId(id),useExternMat(false),isFirstGFXUpdate(true),DBProPosition(0)
:inUse(true),hasGarbage(false),objId(id),useExternMat(false),isFirstGFXUpdate(false),DBProPosition(0)
{
}

PhysicsObject::~PhysicsObject()
{	
}

void PhysicsObject::objectHasGarbage()
{
	hasGarbage=true;
	PhysicsManager::hasGarbage=true;
}

//////////
/*
struct sPositionMatricesEx
{
	// matrices for position

	D3DXMATRIX						matTranslation,								// translation ( position )
									matRotation,								// final rotation matrix
									matRotateX,									// x rotation
									matRotateY,									// y rotation
									matRotateZ,									// z rotation
									matScale,									// scale
									matObjectNoTran,							// final world without translation (collision)
									matWorld,									// final world matrix
									matFreeFlightRotate,						// free flight matrix rotation
									matPivot;									// pivot
};

struct sPositionPropertiesEx
{
	// position properties

	bool							bFreeFlightRotation;						// flag for euler/freeflight
	bool							bApplyPivot;								// pivot
	bool							bGlued;										// glue
	bool							bCustomWorldMatrix;							// used for when world matrix is calculated manually
	int		 						iGluedToObj;								// glued to object
	int		 						iGluedToMesh;								// glued to mesh
	DWORD							dwRotationOrder;							// euler rotation order
};

struct sPositionVectorsEx
{
	// stores position data

	D3DXVECTOR3						vecPosition,								// main position
									vecRotate,									// euler rotation
									vecScale,									// main scale
									vecLook,									// look vector
									vecUp,										// up vector
									vecRight,									// right vector
									vecLastPosition,							// last position used by auto-collision
									vecLastRotate;								// autocol uses for rotation restoration
};

struct sPositionDataEx : public sPositionVectorsEx,
                              sPositionPropertiesEx,
							  sPositionMatricesEx
{
	// final position structure

	// constructor and destructor

	
	sPositionDataEx ( )
	{
		memset ( this, 0, sizeof ( sPositionData ) );

		vecPosition	= D3DXVECTOR3 ( 0, 0, 0 );		// default settings
		vecRotate	= D3DXVECTOR3 ( 0, 0, 0 );
		vecScale    = D3DXVECTOR3 ( 1, 1, 1 );

		vecLook     = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
		vecUp       = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
		vecRight    = D3DXVECTOR3 ( 1, 0, 0 );		// right vector

		bFreeFlightRotation	= false;			// default euler rotation
		bApplyPivot			= false;
		bGlued				= false;
		D3DXMatrixIdentity ( &matFreeFlightRotate );
	}

	~sPositionDataEx ( )
	{
	}
	
};
*/

struct sPositionMatricesEx
{
	// matrices for position

	D3DXMATRIX						matTranslation,								// translation ( position )
									matRotation,								// final rotation matrix
									matRotateX,									// x rotation
									matRotateY,									// y rotation
									matRotateZ,									// z rotation
									matScale,									// scale
									matObjectNoTran,							// final world without translation (collision)
									matWorld,									// final world matrix
									matFreeFlightRotate,						// free flight matrix rotation
									matPivot;									// pivot
};

struct sPositionPropertiesEx
{
	// position properties

	bool							bFreeFlightRotation;						// flag for euler/freeflight
	bool							bApplyPivot;								// pivot
	bool							bGlued;										// glue
	bool							bCustomWorldMatrix;							// used for when world matrix is calculated manually
	int		 						iGluedToObj;								// glued to object
	int		 						iGluedToMesh;								// glued to mesh
	DWORD							dwRotationOrder;							// euler rotation order
	float							fCamDistance;								// used for depth sorting
};

struct sPositionVectorsEx
{
	// stores position data

	D3DXVECTOR3						vecPosition,								// main position
									vecRotate,									// euler rotation
									vecScale,									// main scale
									vecLook,									// look vector
									vecUp,										// up vector
									vecRight,									// right vector
									vecLastPosition,							// last position used by auto-collision
									vecLastRotate;								// autocol uses for rotation restoration
};

struct sPositionDataEx : public sPositionVectorsEx,
                              sPositionPropertiesEx,
							  sPositionMatricesEx
{
	// final position structure

	// constructor and destructor

	sPositionDataEx ( )
	{
		memset ( this, 0, sizeof ( sPositionData ) );

		vecPosition	= D3DXVECTOR3 ( 0, 0, 0 );		// default settings
		vecRotate	= D3DXVECTOR3 ( 0, 0, 0 );
		vecScale    = D3DXVECTOR3 ( 1, 1, 1 );

		vecLook     = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
		vecUp       = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
		vecRight    = D3DXVECTOR3 ( 1, 0, 0 );		// right vector

		bFreeFlightRotation	= false;			// default euler rotation
		bApplyPivot			= false;
		bGlued				= false;
		D3DXMatrixIdentity ( &matFreeFlightRotate );
	}

	~sPositionDataEx ( )
	{
	}
};

sPositionDataEx DBProPositionEx;


//////////

void PhysicsObject::getEngineMatrix()
{
	PhysicsManager::DLL_GetPositionData(objId,&DBProPosition);
	assert(DBProPosition!=0);
	// lee - 190406 - u6rc11 - as emitter obj ignores POSITION OBJECT (custom matrix) no world matrix is set, so fill XYZ here
	DBProPosition->matWorld._41 = DBProPosition->vecPosition.x;
	DBProPosition->matWorld._42 = DBProPosition->vecPosition.y;
	DBProPosition->matWorld._43 = DBProPosition->vecPosition.z;
	m.convertD3DMatrix(&DBProPosition->matWorld);
}


void PhysicsObject::setEngineMatrix()
{
	PhysicsManager::DLL_GetPositionData(objId,&DBProPosition);
	assert(DBProPosition!=0);
	PhysicsManager::DLL_SetWorldMatrix(objId,m);
}

void PhysicsObject::Update()
{
	assert(inUse);	
	if(useExternMat) getEngineMatrix();
	onUpdate();	
	if(useExternMat) setEngineMatrix();
}

void PhysicsObject::UpdateGraphics()
{
	assert(inUse);
	onUpdateGraphics();
	isFirstGFXUpdate=false;
}


