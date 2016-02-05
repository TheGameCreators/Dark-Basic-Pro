
#include "FluidProperties.h"
#include "globals.h"
#include "FluidStructure.h"

/*
	PHY SET FLUID PARTICLE BUFFER CAP%LL%?dbPhySetFluidParticleBufferCap@@YAXHH@Z%
	PHY SET FLUID REST PER METERE%LF%?dbPhySetFluidRestPerMetre@@YAXHM@Z%
	PHY SET FLUID RADIUS%LF%?dbPhySetFluidRadius@@YAXHM@Z%
	PHY SET FLUID MOTION LIMIT%LF%?dbPhySetFluidMotionLimit@@YAXHM@Z
	PHY SET FLUID PACKET SIZE%LL%?dbPhySetFluidPacketSize@@YAXHH@Z%
	PHY SET FLUID POSITION%LFFF%?dbPhySetFluidPosition@@YAXHMMM@Z%
	PHY SET FLUID ROTATION%LFFF%?dbPhySetFluidRotation@@YAXHMMM@Z%
	PHY SET FLUID STIFFNESS%LF%?dbPhySetFluidStiffness@@YAXHM@Z%
	PHY SET FLUID VISCOSITY%LF%?dbPhySetFluidViscosity@@YAXHM@Z%
	PHY SET FLUID REST DENSITY%LF%?dbPhySetFluidRestDensity@@YAXHM@Z%
	PHY SET FLUID DAMPING%LF%?dbPhySetFluidDamping@@YAXHM@Z%
	PHY SET FLUID METHOD%LL%?dbPhySetFluidMethod@@YAXHH@Z%
	PHY SET FLUID ACCELERATION%LFFF%?dbPhySetFluidAcceleration@@YAXHMMM@Z%

	// U70 - 250608 - OBSOLETE NOW IN 2.7.3 ONWARDS!
	PHY SET FLUID SCREEN SPACING%LF%?dbPhySetFluidScreenSpacing@@YAXHM@Z%
	PHY SET FLUID TRIANGLE WINDING%LL%?dbPhySetFluidTriangleWinding@@YAXHH@Z%
	PHY SET FLUID THRESHOLD%LF%?dbPhySetFluidThreshold@@YAXHM@Z%
	PHY SET FLUID PARTICLE RADIUS%LF%?dbPhySetFluidParticleRadius@@YAXHM@Z%
	PHY SET FLUID DEPTH SMOOTHING%LF%?dbPhySetFluidDepthSmoothing@@YAXHM@Z%
	PHY SET FLUID SILHOUETTE SMOOTHING%LF%?dbPhySetFluidSilhouetteSmoothing@@YAXHM@Z%
	// U70 - 250608 - OBSOLETE NOW IN 2.7.3 ONWARDS!

	PHY SET FLUID EMITTER TYPE%LL%?dbPhySetFluidEmitterType@@YAXHH@Z%
	PHY SET FLUID EMITTER MAX PARTICLES%LL%?dbPhySetFluidEmitterMaxParticles@@YAXHH@Z%
	PHY SET FLUID EMITTER SHAPE%LL%?dbPhySetFluidEmitterShape@@YAXHH@Z%
	PHY SET FLUID EMITTER DIMENSIONS%LFF%?dbPhySetFluidEmitterDimensions@@YAXHMM@Z%
	PHY SET FLUID EMITTER RANDOM POSITION%LFFF%?dbPhySetFluidEmitterRandomPosition@@YAXHMMM@Z%
	PHY SET FLUID EMITTER RANDOM ANGLE%LF%?dbPhySetFluidEmitterRandomAngle@@YAXHM@Z%
	PHY SET FLUID EMITTER VELOCITY%LF%?dbPhySetFluidEmitterVelocity@@YAXHM@Z%
	PHY SET FLUID EMITTER RATE%LF%?dbPhySetFluidEmitterRate@@YAXHM@Z%
	PHY SET FLUID EMITTER LIFETIME%LF%?dbPhySetFluidEmitterLifetime@@YAXHM@Z%
	PHY SET FLUID EMITTER FLAGS%LL%?dbPhySetFluidEmitterFlags@@YAXHH@Z%
*/

void dbPhySetFluidParticleBufferCap ( int iID, int iCap )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		if ( iCap > 30000 ) iCap = 30000; // PhysX MAX 32K
		pFluid->iParticleBufferCap = iCap;
	}
}

void dbPhySetFluidRestPerMetre ( int iID, float fRest )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.restParticlesPerMeter = fRest;
	}
}

void dbPhySetFluidRadius ( int iID, float fRadius )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.kernelRadiusMultiplier = fRadius;
	}
}

void dbPhySetFluidMotionLimit ( int iID, float fMotion )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.motionLimitMultiplier = fMotion;
	}
}

void dbPhySetFluidPacketSize ( int iID, int iSize )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.packetSizeMultiplier = iSize;
	}
}

void dbPhySetFluidPosition ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->vecPosition = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetFluidRotation ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->vecRotation = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetFluidStiffness	( int iID, float fStiffness )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.stiffness = fStiffness;
	}
}

void dbPhySetFluidViscosity	( int iID, float fViscosity )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.viscosity = fViscosity;
	}
}

void dbPhySetFluidRestDensity ( int iID, float fRestDensity )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.restDensity = fRestDensity;	
	}
}

void dbPhySetFluidDamping ( int iID, float fDamping )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.damping = fDamping;
	}
}

void dbPhySetFluidRestitution ( int iID, float fRestitution )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.staticCollisionRestitution = fRestitution;
		pFluid->fluidDescription.dynamicCollisionRestitution = fRestitution;//280808
	}
}

void dbPhySetFluidAdhesion ( int iID, float fAdhesion )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.staticCollisionAdhesion = fAdhesion;
		pFluid->fluidDescription.dynamicCollisionAdhesion = fAdhesion;//280808
	}
}

void dbPhySetFluidMethod ( int iID, int iMethod )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.simulationMethod = iMethod;
	}
}

void dbPhySetFluidAcceleration ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->fluidDescription.externalAcceleration = NxVec3 ( fX, fY, fZ );
	}
}



void dbPhySetFluidEmitterType ( int iID, int iType )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.type = ( NxU32 ) iType;
	}
}

void dbPhySetFluidEmitterMaxParticles ( int iID, int iMax )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		if ( iMax<=0 || iMax > (int)pFluid->iParticleBufferCap ) iMax = pFluid->iParticleBufferCap; // PhysX MAX 32K
		pFluid->emitterDescription.maxParticles = ( NxU32 ) iMax;
	}
}

void dbPhySetFluidEmitterShape ( int iID, int iShape )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.shape = ( NxU32 ) iShape;
	}
}

void dbPhySetFluidEmitterDimensions ( int iID, float fX, float fY )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.dimensionX = ( NxReal ) fX;
		pFluid->emitterDescription.dimensionY = ( NxReal ) fY;
	}
}

void dbPhySetFluidEmitterRandomPosition ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.randomPos = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetFluidEmitterRandomAngle ( int iID, float fAngle )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.randomAngle = ( NxReal ) fAngle;
	}
}

void dbPhySetFluidEmitterVelocity ( int iID, float fVelocity )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.fluidVelocityMagnitude = ( NxReal ) fVelocity;
	}
}

void dbPhySetFluidEmitterRate ( int iID, float fRate )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		if ( pFluid->pFluidEmitter )
			pFluid->pFluidEmitter->setRate ( (NxReal)fRate );
		else
			pFluid->emitterDescription.rate = (NxReal)fRate;
	}
}

void dbPhySetFluidEmitterLifetime ( int iID, float fLife )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		pFluid->emitterDescription.particleLifetime = ( NxReal ) fLife;
	}
}

void dbPhySetFluidEmitterFlags ( int iID, int iFlags )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		//NX_FEF_VISUALIZATION 
		//NX_FEF_FORCE_ON_BODY
		//NX_FEF_ADD_BODY_VELOCITY
		//NX_FEF_ENABLED 
		pFluid->emitterDescription.flags = ( NxU32 ) iFlags;
	}
}

void dbPhySetFluidEmitterRotation ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		// cheatily transport rotation angles in translation section of relPose
		// as relPose will be changed when the Fluid is BUILT
		NxVec3 vecTransportAnglesInTranslationVector = NxVec3(fX, fY, fZ);
		pFluid->emitterDescription.relPose.t = vecTransportAnglesInTranslationVector;
	}
}

void dbPhySetFluidEmitterPosition ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		NxVec3 vec = NxVec3 ( fX, fY, fZ );
		pFluid->pFluidEmitter->setGlobalPosition ( vec );
		pFluid->pFluidEmitter->resetEmission ( 0 );
		pFluid->pFluid->removeAllParticles();
	}
}
