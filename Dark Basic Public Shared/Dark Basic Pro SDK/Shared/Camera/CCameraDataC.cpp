#include "CCameraDataC.h"

tagCameraData::tagCameraData ( )
{
	// setup default values for the structure, by creating a constructor
	// here it saves us having to call memset every time we create an instance
	// of it and we can also setup default values

	// clear the structure
	memset ( this, 0, sizeof ( tagCameraData ) );

	// mike - 250903 - override camera
	bOverride = false;

	//Dave extra camera flags
	dwCameraSwitchBank = 0;

	// setup default values
	fAspect     = 0.0f;							// the aspect will come in later on
	fAspectMod  = 0.0f;							// modifier
	fFOV        = D3DX_PI / 4;						// setup the field of view (degrees)
	fZNear      = 1.0;							// near clipping range
	fZFar       = 3000.0;						// far clipping range
	vecLook     = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
	vecUp       = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
	vecRight    = D3DXVECTOR3 ( 1, 0, 0 );		// right vector
	vecPosition = D3DXVECTOR3 ( 0, 0, 0 );		// position

	D3DXMatrixIdentity ( &matFreeFlightRotate );// freeflight

	bRotate     = true;							// rotation for x, y and then z

	iBackdropState=0;							// used as an automatic clear
	// aaron - 20120813 - give backdrop new color for DB3
	//dwBackdropColor=D3DCOLOR_XRGB(0,0,128);		// color of backdrop
	dwBackdropColor = 0xFF194C7F;				// color of backdrop

	dwForegroundColor=0;						// U75 - 070410 - color of foreground (used to WIPE OUT textures/effects)

	iBackdropTextureMode=0;						// no backdrop texture by default
	pBackdropTexture=NULL;
}
