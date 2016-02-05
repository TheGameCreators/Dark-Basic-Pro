
#include "globals.h"

DARKSDK void dbPhyMakeFluid			( int iID );
DARKSDK void dbPhyMakeFluidEmitter  ( int iID );
DARKSDK void dbPhyBuildFluid		( int iID );
DARKSDK void dbPhyBuildFluidEx		( int iID, int iSizeOfFluidVolume );
DARKSDK void dbPhyDeleteFluid		( int iID );
DARKSDK int  dbPhyGetFluidExist		( int iID );

DARKSDK void dbPhyMakeBoxDrain		( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ );
DARKSDK void dbPhyMakeSphereDrain	( int iID, float fX, float fY, float fZ, float fRadius );
DARKSDK void dbPhyDeleteDrain		( int iID );
DARKSDK int  dbPhyGetDrainExist		( int iID );

DARKSDK void dbPhyClearFluids		( void );

void dbPhySetDrainDefaults  ( int iID, float fX, float fY, float fZ, float fAngleX, float fAngleY, float fAngleZ  );