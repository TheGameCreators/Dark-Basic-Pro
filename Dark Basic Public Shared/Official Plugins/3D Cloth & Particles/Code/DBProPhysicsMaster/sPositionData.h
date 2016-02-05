#ifndef _POSITIONDATA_H_
#define _POSITIONDATA_H_
/*
struct sPositionData
{
	// position information

	D3DXVECTOR3		vecPosition;		// main position

	// rotation information

	bool			bFreeFlightRotation;// flag for euler/freeflight
	D3DXMATRIX		matFreeFlightRotate;// free flight matrix rotation
	DWORD			dwRotationOrder;	// euler rotation order
	D3DXVECTOR3		vecRotate;			// euler rotation

	// scale information

	D3DXVECTOR3		vecScale;			// main scale

	// transformation variables

	D3DXMATRIX		matTranslation,		// translation ( position )
					matRotation,		// final rotation matrix
					matRotateX,			// x rotation
					matRotateY,			// y rotation
					matRotateZ,			// z rotation
					matScale,			// scale
					matObjectNoTran,	// final world without translation (collision)
					matWorld;			// final world matrix

	bool			bApplyPivot;		// pivot
	D3DXMATRIX		matPivot;

	bool			bGlued;				// glue
	int		 		iGluedToObj;
	int		 		iGluedToMesh;

	// movement variables

	D3DXVECTOR3		vecLook,			// look vector
					vecUp,				// up vector
					vecRight;			// right vector

	// history variables

	D3DXVECTOR3		vecLastPosition;	// last position

	// constructors

	sPositionData ( )
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

	~sPositionData ( )
	{
	}
};
*/


/*
struct sPositionMatrices
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

struct sPositionProperties
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

struct sPositionVectors
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

struct sPositionData : public sPositionVectors,
                              sPositionProperties,
							  sPositionMatrices
{
	// final position structure

	// constructor and destructor

	
};
*/

/* commented out this nono on 070406
struct sPositionMatrices
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

struct sPositionProperties
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

struct sPositionVectors
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

struct sPositionData : public sPositionVectors,
                              sPositionProperties,
							  sPositionMatrices
{
	// final position structure

	// constructor and destructor

	//sPositionData  ( );
	//~sPositionData ( );
};

naughty naughty, just cut and pasting is fine, but using this structure to cast a ptr taken from 
a constantly updating DBO format fom the latest Basic3D is a BIG nonono!
*/

// This hold the latest structures you need for the local data structure below
#include "..\..\..\..\Dark Basic Pro SDK\Shared\DBOFormat\DBOData.h"


#endif
