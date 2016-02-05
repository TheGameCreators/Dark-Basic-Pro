
#include "FluidMake.h"
#include "globals.h"
#include "fluidstructure.h"
#include "error.h"

/*
	PHY MAKE FLUID%L%?dbPhyMakeFluid@@YAXH@Z%
	PHY MAKE FLUID%LL%?dbPhyMakeFluidEx@@YAXHH@Z%
	PHY MAKE FLUID EMITTER%L%?dbPhyMakeFluidEmitter@@YAXH@Z%
	PHY BUILD FLUID%L%?dbPhyBuildFluid@@YAXH@Z%
	PHY DELETE FLUID%L%?dbPhyDeleteFluid@@YAXH@Z%
	PHY GET FLUID EXIST[%LL%?dbPhyGetFluidExist@@YAHH@Z%

	PHY MAKE BOX DRAIN%LFFFFFFFFF%?dbPhyMakeBoxDrain@@YAXHMMMMMMMMM@Z%
	PHY MAKE SPHERE DRAIN%LFFFF%?dbPhyMakeSphereDrain@@YAXHMMMM@Z%
	PHY DELETE DRAIN%L%?dbPhyDeleteDrain@@YAXH@Z%
	PHY GET DRAIN EXIST[%LL%?dbPhyGetDrainExist@@YAHH@Z%

	PHY SET FLUID RENDERER COLOR FALLOFF SCALE%F%?dbPhySetFluidRendererColorFalloffScale@@YAXM@Z%Scale
	PHY SET FLUID RENDERER DENSITY THRESHOLD%F%?dbPhySetFluidRendererDensityThreshold@@YAXM@Z%Density
	PHY SET FLUID RENDERER FLUID COLOR%FFFF%?dbPhySetFluidRendererFluidColor@@YAXMMMM@Z%R, G, B, A
	PHY SET FLUID RENDERER FLUID COLOR FALLOFF%FFFF%?dbPhySetFluidRendererFluidColorFalloff@@YAXMMMM@Z%R, G, B, A
	PHY SET FLUID RENDERER FRESNEL COLOR%FFFF%?dbPhySetFluidRendererFresnelColor@@YAXMMMM@Z%R, G, B, A
	PHY SET FLUID RENDERER PARTICLE RADIUS%F%?dbPhySetFluidRendererParticleRadius@@YAXM@Z%Radius
	PHY SET FLUID RENDERER PARTICLE SURFACE SCALE%F%?dbPhySetFluidRendererParticleSurfaceScale@@YAXM@Z%Scale
	PHY SET FLUID RENDERER SHININESS%F%?dbPhySetFluidRendererShininess@@YAXM@Z%Shininess
	PHY SET FLUID RENDERER SPECULAR COLOR%FFFF%?dbPhySetFluidRendererSpecularColor@@YAXMMMM@Z%R, G, B, A
*/

namespace PhysX
{
	DARKSDK void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
	{
		float m00 = pmatMatrix->_11;
		float m01 = pmatMatrix->_12;
		float m02 = pmatMatrix->_13;
		float m12 = pmatMatrix->_23;
		float m22 = pmatMatrix->_33;
		float heading = (float)atan2(m01,m00);
		float attitude = (float)atan2(m12,m22);
		float bank = (float)asin(-m02);

		// check for gimbal lock
		if ( fabs ( m02 ) > 1.0f )
		{
			// looking straight up or down
			float PI = D3DX_PI / 2.0f;
			pVecAngles->x = 0.0f;
			pVecAngles->y = D3DXToDegree ( PI * m02 );
			pVecAngles->z = 0.0f;
		}
		else
		{
			pVecAngles->x = D3DXToDegree ( attitude );
			pVecAngles->y = D3DXToDegree ( bank );
			pVecAngles->z = D3DXToDegree ( heading );
		}
	}
}

D3DXMATRIX dbPhyGetFluidMatrix ( void )
{
	D3DXMATRIX matPerspective;
	D3DXMatrixIdentity ( &matPerspective );
	return matPerspective;
}

void dbPhyMakeFluid ( int iID )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID, false ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a fluid object with an ID that is already in use", "", 0 );
		return;
	}

	PhysX::sFluid* pFluid = new PhysX::sFluid;

	if ( !pFluid )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory for new fluid object", "", 0 );
		return;
	}

	memset ( pFluid, 0, sizeof ( PhysX::sFluid ) );
	pFluid->fluidDescription.setToDefault ( );
	pFluid->iID												= iID;
	pFluid->iType											= 0;
	//pFluid->iParticleBufferCap  							= 30000; // PhysX MAX 32K
	pFluid->iParticleBufferCap  							= 64000; // OpenGL Demo
	pFluid->iParticleBufferNum  							= 0;
	pFluid->fluidDescription.kernelRadiusMultiplier			= 1.0f;//1.8f;
    pFluid->fluidDescription.restParticlesPerMeter			= 5.0f;//10.0f;
	pFluid->fluidDescription.motionLimitMultiplier			= 3.0f;
	pFluid->fluidDescription.packetSizeMultiplier			= 8;
    pFluid->fluidDescription.stiffness						= 1.0f;//50.0f;
    pFluid->fluidDescription.viscosity						= 200.0f;
	pFluid->fluidDescription.restDensity					= 1000.0f;
    pFluid->fluidDescription.damping						= 1.0f;//2.0f;
    pFluid->fluidDescription.staticCollisionRestitution		= 0.0001f;//0.4f;
	pFluid->fluidDescription.staticCollisionAdhesion		= 0.0003f;//0.03f;
	pFluid->fluidDescription.simulationMethod				= NX_F_SPH;
	PhysX::pFluidList.push_back ( pFluid );
}

void dbPhyBuildFluidEx ( int iID, int iSizeOfFluidVolume )
{
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build fluid due to invalid scene", "", 0 );
		return;
	}

	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		NxMat34 mat;
		NxMat33 rotX;
		NxMat33 rotY;
		NxMat33 rotZ;
		NxMat33 rotation;

		// use values added in here from EmitterRotation command to create final pose matrix
		NxVec3 vecTransportAnglesInTranslationVector = pFluid->emitterDescription.relPose.t;
		rotX.rotX ( D3DXToRadian ( vecTransportAnglesInTranslationVector.x ) );
		rotY.rotY ( D3DXToRadian ( vecTransportAnglesInTranslationVector.y ) );
		rotZ.rotZ ( D3DXToRadian ( vecTransportAnglesInTranslationVector.z ) );

		rotation = rotX * rotY * rotZ;
		mat.M = rotation;
		mat.t = pFluid->vecPosition;

		pFluid->pParticleBuffer = new NxVec3 [ pFluid->iParticleBufferCap ];
		pFluid->pParticleDensityBuffer = new NxReal [ pFluid->iParticleBufferCap ];

		if ( pFluid->iType == 0 )
		{
			float distance = 0.1f;
			pFluid->iParticleBufferNum = 0;
			unsigned sideNum = iSizeOfFluidVolume;

			float rad = sideNum*distance*0.5f;
				
			for (unsigned i=0; i<sideNum; i++)
			{
				for (unsigned j=0; j<sideNum; j++)
				{
					for (unsigned k=0; k<sideNum; k++)
					{
						NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
						if (p.distance(NxVec3(rad,rad,rad)) < rad)
						{
							// leeadd - 021008 - ensure does not overrun buffer (if user increases volume)
							if ( pFluid->iParticleBufferNum < pFluid->iParticleBufferCap )
							{
								p += pFluid->vecPosition;
								pFluid->pParticleBuffer[pFluid->iParticleBufferNum] = p;
								pFluid->pParticleDensityBuffer[pFluid->iParticleBufferNum] = pFluid->fluidDescription.restDensity;
								pFluid->iParticleBufferNum++;
							}
						}
					}
				}
			}
		}

		pFluid->particleData.numParticlesPtr			= &pFluid->iParticleBufferNum;
		pFluid->particleData.bufferPos					= &pFluid->pParticleBuffer [ 0 ].x;
		pFluid->particleData.bufferPosByteStride		= sizeof ( NxVec3 );
		pFluid->particleData.bufferDensity              = pFluid->pParticleDensityBuffer;
		pFluid->particleData.bufferDensityByteStride    = sizeof(NxReal);

		if ( pFluid->iType == 0 )
		{
			pFluid->fluidDescription.initialParticleData	= pFluid->particleData;
		}

		pFluid->fluidDescription.particlesWriteData		= pFluid->particleData;

		if ( pFluid->fluidDescription.isValid ( ) == false )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot create fluid as description is invalid", "", 0 );
			return;
		}

		//TC - June 19th to use GPU/PPU when scene type hardware is used. Without this it always runs in HW
		if (PhysX::iSimulationType == 1)
		{
			pFluid->fluidDescription.flags |= NX_FF_HARDWARE;
		}
		else
		{
			pFluid->fluidDescription.flags &= ~NX_FF_HARDWARE;
		}
		pFluid->pFluid = PhysX::pScene->createFluid ( pFluid->fluidDescription );

		if ( !pFluid->pFluid )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create fluid object", "", 0 );
			return;
		}

		if ( pFluid->iType == 1 )
		{
			pFluid->emitterDescription.relPose = mat;
			if ( pFluid->emitterDescription.isValid ( ) == false )
			{
				PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Emitter description is invalid", "", 0 );
				return;
			}
			pFluid->pFluidEmitter = pFluid->pFluid->createEmitter ( pFluid->emitterDescription );
		}

		// create new object
		PhysX::CreateNewObject ( iID, "fluid", 1 );

		sObject* pObject = PhysX::GetObject ( iID );
		if ( !pObject )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid 3D object for fluid object", "", 0 );
			return;
		}

		PhysX::SetupMeshFVFData ( pObject->pFrame->pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, 65535 * 3, 65535 );
		pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
		pObject->pFrame->pMesh->iDrawVertexCount = pFluid->iNumVertices;
		pObject->pFrame->pMesh->iDrawPrimitives  = pFluid->iNumIndices / 3;
		pObject->pFrame->pMesh->Collision.fRadius = 0.0f;
		PhysX::CalculateMeshBounds			( pObject->pFrame->pMesh );
		PhysX::SetNewObjectFinalProperties	( iID, 0.0f );
		PhysX::SetTexture					( iID, 0 );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to build fluid object", "", 0 );
		return;
	}
}

void dbPhyBuildFluid ( int iID )
{
	// default is a dimension of 19
	dbPhyBuildFluidEx ( iID, 19 );
}

void dbPhyMakeFluidEmitter ( int iID )
{
	dbPhyMakeFluid ( iID );
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		// emitter specific data
		pFluid->iType = 1;
		pFluid->emitterDescription.setToDefault();
		pFluid->emitterDescription.dimensionX = 0.25;
		pFluid->emitterDescription.dimensionY = 0.25;
		pFluid->emitterDescription.rate = 500.0;
		pFluid->emitterDescription.randomAngle = 0.1f;
		pFluid->emitterDescription.fluidVelocityMagnitude = 6.5f;
		pFluid->emitterDescription.maxParticles = pFluid->iParticleBufferCap;
		pFluid->emitterDescription.particleLifetime = 100.0f;
		pFluid->emitterDescription.type = NX_FE_CONSTANT_FLOW_RATE;
		pFluid->emitterDescription.shape = NX_FE_ELLIPSE;
	}
}

void dbPhyDeleteFluid ( int iID )
{
	// delete a single rigid body in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwFluid = 0; dwFluid < PhysX::pFluidList.size ( ); dwFluid++ )
	{
		// see if we have a match
		if ( PhysX::pFluidList [ dwFluid ]->iID == iID )
		{
			// get a pointer to our object
			PhysX::sFluid* pFluid = PhysX::pFluidList [ dwFluid ];

			if ( pFluid->pFluid )
			{
				PhysX::pScene->releaseFluid ( *pFluid->pFluid );

				PhysX::DeleteObject ( pFluid->iID );
				
				// erase the item in the list
				PhysX::pFluidList.erase ( PhysX::pFluidList.begin ( ) + dwFluid );

				// clear up the memory
				delete pFluid;
				pFluid = NULL;
			}
		}
	}
}

void dbPhyClearFluids ( void )
{
	// delete a single rigid body in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwFluid = 0; dwFluid < PhysX::pFluidList.size ( ); dwFluid++ )
	{
		// get a pointer to our object
		PhysX::sFluid* pFluid = PhysX::pFluidList [ dwFluid ];

		if ( pFluid->pFluid )
		{
			delete [ ] pFluid->pParticleBuffer;
			delete [ ] pFluid->pParticleDensityBuffer;
			PhysX::DeleteObject ( pFluid->iID );
			PhysX::pScene->releaseFluid ( *pFluid->pFluid );
			
			// clear up the memory
			delete pFluid;
			pFluid = NULL;
		}
	}

	PhysX::pFluidList.clear ( );
}

int dbPhyGetFluidExist ( int iID )
{
	if ( PhysX::pScene == NULL )
		return 0;

	for ( DWORD dwFluid = 0; dwFluid < PhysX::pFluidList.size ( ); dwFluid++ )
	{
		if ( PhysX::pFluidList [ dwFluid ]->iID == iID )
		{
			PhysX::sFluid* pFluid = PhysX::pFluidList [ dwFluid ];
			if ( pFluid->pFluid )
				return 1;
		}
	}

	return 0;
}

void dbPhyMakeBoxDrain ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ )
{
	if ( PhysX::pScene == NULL )
		return;

	if ( PhysX::sDrain* pDrain = dbPhyGetDrain ( iID ) )
		return;

	PhysX::sDrain* pDrain = new PhysX::sDrain;

	if ( !pDrain )
		return;

	pDrain->iID = iID;

	NxBoxShapeDesc	DrainDesc;
	NxActorDesc		DrainActorDesc;

    DrainDesc.setToDefault ( );
	DrainDesc.shapeFlags |= NX_SF_FLUID_DRAIN;
	DrainDesc.dimensions.set ( fSizeX, fSizeY, fSizeZ );
	DrainActorDesc.shapes.pushBack ( &DrainDesc );
	pDrain->pActor = PhysX::pScene->createActor ( DrainActorDesc );
	PhysX::pDrainList.push_back ( pDrain );

	dbPhySetDrainDefaults ( iID, fX, fY, fZ, fAngleX, fAngleY, fAngleZ );
}

void dbPhyMakeSphereDrain ( int iID, float fX, float fY, float fZ, float fRadius )
{
	if ( PhysX::pScene == NULL )
		return;

	if ( PhysX::sDrain* pDrain = dbPhyGetDrain ( iID ) )
		return;

	PhysX::sDrain* pDrain = new PhysX::sDrain;

	if ( !pDrain )
		return;

	pDrain->iID = iID;

	NxSphereShapeDesc	DrainDesc;
	NxActorDesc		DrainActorDesc;

    DrainDesc.setToDefault ( );
	DrainDesc.shapeFlags |= NX_SF_FLUID_DRAIN;
	DrainDesc.radius = fRadius;
	DrainActorDesc.shapes.pushBack ( &DrainDesc );
	pDrain->pActor = PhysX::pScene->createActor ( DrainActorDesc );
	PhysX::pDrainList.push_back ( pDrain );

	dbPhySetDrainDefaults ( iID, fX, fY, fZ, 0.0f, 0.0f, 0.0f );
}

void dbPhyDeleteDrain ( int iID )
{
	if ( PhysX::pScene == NULL )
		return;


	if ( PhysX::sDrain* pDrain = dbPhyGetDrain ( iID ) )
	{
		PhysX::pScene->releaseActor ( *pDrain->pActor );

		DWORD dwDrain = 0;
		for ( dwDrain = 0; dwDrain < PhysX::pDrainList.size ( ); dwDrain++ )
			if ( PhysX::pDrainList [ dwDrain ]->iID == iID )
				break;

		PhysX::pDrainList.erase ( PhysX::pDrainList.begin ( ) + dwDrain );
		SAFE_DELETE ( pDrain );
	}
}

int dbPhyGetDrainExist ( int iID )
{
	if ( PhysX::pScene == NULL )
		return 0;

	if ( PhysX::sDrain* pDrain = dbPhyGetDrain ( iID ) )
		return 1;
	else
		return 0;
}

void dbPhySetDrainDefaults ( int iID, float fX, float fY, float fZ, float fAngleX, float fAngleY, float fAngleZ )
{
	if ( PhysX::sDrain* pDrain = dbPhyGetDrain ( iID ) )
	{
		NxMat33 matRotX;
		NxMat33 matRotY;
		NxMat33 matRotZ;
		NxMat33 matRotation;
		matRotX.rotX ( D3DXToRadian ( fAngleX ) );
		matRotY.rotY ( D3DXToRadian ( fAngleY ) );
		matRotZ.rotZ ( D3DXToRadian ( fAngleZ ) );
		matRotation = matRotX * matRotY * matRotZ;
		pDrain->pActor->setGlobalPosition    ( NxVec3 ( fX, fY, fZ ) );
		pDrain->pActor->setGlobalOrientation ( matRotation );
	}
}
