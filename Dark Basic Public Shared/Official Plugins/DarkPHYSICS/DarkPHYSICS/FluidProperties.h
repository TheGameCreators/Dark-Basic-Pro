
#include "globals.h"

DARKSDK void dbPhySetFluidParticleBufferCap		( int iID, int iCap );
DARKSDK void dbPhySetFluidRestPerMetre			( int iID, float fRest );
DARKSDK void dbPhySetFluidRadius				( int iID, float fRadius );
DARKSDK void dbPhySetFluidMotionLimit			( int iID, float fMotion );
DARKSDK void dbPhySetFluidPacketSize			( int iID, int iSize );
DARKSDK void dbPhySetFluidPosition				( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetFluidRotation				( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetFluidStiffness				( int iID, float fStiffness );
DARKSDK void dbPhySetFluidViscosity				( int iID, float fViscosity );
DARKSDK void dbPhySetFluidRestDensity			( int iID, float fRestDensity );
DARKSDK void dbPhySetFluidDamping				( int iID, float fDamping );
DARKSDK void dbPhySetFluidRestitution			( int iID, float fRestitution );
DARKSDK void dbPhySetFluidAdhesion				( int iID, float fAdhesion );
DARKSDK void dbPhySetFluidMethod				( int iID, int iMethod );
DARKSDK void dbPhySetFluidAcceleration			( int iID, float fX, float fY, float fZ );

/* U70 - 250608 - OBSOLETE NOW IN 2.7.3 ONWARDS!
DARKSDK void dbPhySetFluidScreenSpacing			( int iID, float fSpacing );
DARKSDK void dbPhySetFluidTriangleWinding		( int iID, int iMode );
DARKSDK void dbPhySetFluidThreshold				( int iID, float fThreshold );
DARKSDK void dbPhySetFluidParticleRadius		( int iID, float fRadius );
DARKSDK void dbPhySetFluidDepthSmoothing		( int iID, float fSmoothing );
DARKSDK void dbPhySetFluidSilhouetteSmoothing	( int iID, float fSmoothing );
*/

DARKSDK void dbPhySetFluidEmitterType			( int iID, int iType );
DARKSDK void dbPhySetFluidEmitterMaxParticles	( int iID, int iMax );
DARKSDK void dbPhySetFluidEmitterShape			( int iID, int iShape );
DARKSDK void dbPhySetFluidEmitterDimensions		( int iID, float fX, float fY );
DARKSDK void dbPhySetFluidEmitterRandomPosition ( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetFluidEmitterRandomAngle	( int iID, float fAngle );
DARKSDK void dbPhySetFluidEmitterVelocity		( int iID, float fVelocity );
DARKSDK void dbPhySetFluidEmitterRate			( int iID, float fRate );
DARKSDK void dbPhySetFluidEmitterLifetime		( int iID, float fLife );
DARKSDK void dbPhySetFluidEmitterFlags			( int iID, int iFlags );

DARKSDK void dbPhySetFluidEmitterRotation		( int iID, float fX, float fY, float fZ );
DARKSDK void dbPhySetFluidEmitterPosition		( int iID, float fX, float fY, float fZ );
