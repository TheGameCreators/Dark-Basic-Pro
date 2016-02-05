
/////////////////////////////////////////////////////////////////////////////////////
// FLUID INCLUDES ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "fluid.h"
//#include <darksdk.h>

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// EXPOSED FUNCTIONS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
NxFluid*	gFluid = NULL;
NxVec3		gParticleBuffer[10000];
unsigned	gParticleBufferCap = 10000;
unsigned	gParticleBufferNum = 0;

#define REST_PARTICLES_PER_METER 10.f
#define KERNEL_RADIUS_MULTIPLIER 2.4f
//#define KERNEL_RADIUS_MULTIPLIER 1.4f
//#define MOTION_LIMIT_MULTIPLIER 3
//#define MOTION_LIMIT_MULTIPLIER 1
#define MOTION_LIMIT_MULTIPLIER 1
//#define PACKET_SIZE_MULTIPLIER 8
#define PACKET_SIZE_MULTIPLIER 4

struct sFluidVertex
{
	float x, y, z;
	DWORD dwDiffuse;
	float size;
	//float tu, tv;
};

void dbPhyTestFluid ( void )
{
	//NxVec3 pos(0,1.5,0);
	NxVec3 pos(11,14,-2);
	//NxVec3 pos(2, 7, -29 );
	//float distance = 0.1f;
	float distance = 0.1f;
	gParticleBufferNum = 0;
	unsigned sideNum = 19;
	float rad = sideNum*distance*0.5f;
	//float rad = sideNum*distance*2.5f;
	

	for (unsigned i=0; i<sideNum; i++)
	{
		for (unsigned j=0; j<sideNum; j++)
		{
			for (unsigned k=0; k<sideNum; k++)
			{
				NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
				if (p.distance(NxVec3(rad,rad,rad)) < rad)
				{
					p += pos;
					gParticleBuffer[gParticleBufferNum++] = p;
				}
			}
		}
	}

	// set structure to pass particles, and receive them after every simulation step
	NxParticleData particles;
	particles.maxParticles			= gParticleBufferCap;
	particles.numParticlesPtr		= &gParticleBufferNum;
	particles.bufferPos				= &gParticleBuffer[0].x;
	particles.bufferPosByteStride	= sizeof(NxVec3);

	// create a fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.kernelRadiusMultiplier		= KERNEL_RADIUS_MULTIPLIER;
    fluidDesc.restParticlesPerMeter			= REST_PARTICLES_PER_METER;
	fluidDesc.motionLimitMultiplier			= MOTION_LIMIT_MULTIPLIER;
	fluidDesc.packetSizeMultiplier			= PACKET_SIZE_MULTIPLIER;
    //fluidDesc.stiffness						= 200.0f;
	fluidDesc.stiffness						= 10.0f;
    //fluidDesc.viscosity						= 22.0f;
	fluidDesc.viscosity						= 40.0f;
	//fluidDesc.restDensity					= 1000.0f;
	fluidDesc.restDensity					= 2000.0f;
    fluidDesc.damping						= 0.0f;
    fluidDesc.staticCollisionRestitution	= 0.4f;
	fluidDesc.staticCollisionAdhesion		= 0.1f;
	fluidDesc.simulationMethod				= NX_F_SPH;
	fluidDesc.initialParticleData			= particles;
	fluidDesc.particlesWriteData			= particles;
	//fluidDesc.externalAcceleration = NxVec3(-10.0f,5.0f,0.0f);
	//fluidDesc.externalAcceleration = NxVec3(-10.0f,-5.0f,0.0f);

	gFluid = PhysX::pScene->createFluid ( fluidDesc );

	PhysX::pScene->simulate ( 1.0f / 60.0f );

	int iID = 10;
	int	iOffset			= 0;
	int	iValue			= 0;
	int	iIndex			= 0;

	int iObjectSize = 2;

	CreateNewObject ( iID, "fluid", 1 );

	sObject* pObject = dbGetObject ( iID );

	sMesh* pMesh = pObject->pFrame->pMesh;

	SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE, gParticleBufferNum, 0 );
	
	//for ( int iParticle = 0; iParticle < gParticleBufferNum; iParticle++ )
	{
	//	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData, iParticle, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	}
	
	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_POINTLIST;
	pMesh->iDrawVertexCount = gParticleBufferNum;
	pMesh->iDrawPrimitives  = gParticleBufferNum;



	CalculateMeshBounds			( pObject->pFrame->pMesh );
	SetNewObjectFinalProperties ( iID, 0.0f );
	SetTexture                  ( iID, 0 );
	dbSetObjectTransparency		( iID, 1 );
	dbDisableObjectZWrite		( iID );
	dbSetObjectLight			( iID, 0 );
	
}

void dbPhyUpdateFluidVertex ( sFluidVertex*	pVertex, float fX, float fY, float fZ, float tu, float tv, DWORD dwDiffuse )
{
	pVertex->x			= fX;
	pVertex->y			= fY;
	pVertex->z			= fZ;
//	pVertex->tu			= tu;
//	pVertex->tv			= tv;
	pVertex->dwDiffuse	= dwDiffuse;
	//pVertex->size = 10.0f;
}


void dbPhyUpdateFluid ( void )
{
	D3DXVECTOR3				norm	= dbGetObjectCameraLook			( );
	D3DXVECTOR3				cpos	= dbGetObjectCameraPosition		( );
	D3DXVECTOR3				up		= dbGetObjectCameraUp			( );
	D3DXVECTOR3				right	= dbGetObjectCameraRight		( );
	sObject*				pObject = NULL;
	sMesh*					pMesh	= NULL;
	sFluidVertex*	pVertex = NULL;
	int						iOffset = 0;

	//for ( DWORD dwEmitter = 0; dwEmitter < PhysX::pEmitterList.size ( ); dwEmitter++ )
	{
		//PhysX::pEmitterList [ dwEmitter ]->pEmitter->update ( 0.01f );
		//PhysX::sPhysXEmitter* pEmitter = PhysX::pEmitterList [ dwEmitter ];

		//if ( pEmitter->bLinearVelocityRange )
		//	dbPhySetEmitterLinearVelocity ( pEmitter->iID, pEmitter->fMinX + dbRnd ( pEmitter->fMaxX ), pEmitter->fMinY + dbRnd ( pEmitter->fMaxY ), pEmitter->fMinZ + dbRnd ( pEmitter->fMaxZ ) );

		pObject						= dbGetObject ( 10 );
		pMesh						= pObject->pFrame->pMesh;
		pVertex						= ( sFluidVertex* ) pObject->pFrame->pMesh->pVertexData;
		iOffset						= 0;
		pMesh->bVBRefreshRequired	= true;

		for ( int i = 0; i < gParticleBufferNum; i++ )
		{
			NxVec3& particle = gParticleBuffer[i];

			D3DXVECTOR3 pos = D3DXVECTOR3 ( particle.x, particle.y, particle.z );

			float size = 1.0f;

			D3DXVECTOR3 r, u, p0, p1, p2, p3 = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
			D3DXVECTOR3 center               = D3DXVECTOR3 ( pos.x, pos.y, pos.z );
		
			r = right * size;
			u = up    * size;

			p0 = center - r + u;
			p1 = center + r + u;
			p2 = center + r - u;
			p3 = center - r - u;

			DWORD dwDiffuse = D3DCOLOR_ARGB ( 255, 255, 255, 255 );

			pVertex->x = pos.x;
			pVertex->y = pos.y;
			pVertex->z = pos.z;
			//pVertex->size = 5.0f;
			pVertex++;

			//dbPhyUpdateFluidVertex ( &pVertex [ iOffset++ ], p0.x, p0.y, p0.z, 0, 0, dwDiffuse );
			
			//dbPhyUpdateFluidVertex ( &pVertex [ iOffset++ ], p0.x, p0.y, p0.z, 0, 0, dwDiffuse );
			//dbPhyUpdateFluidVertex ( &pVertex [ iOffset++ ], p1.x, p1.y, p1.z, 1, 0, dwDiffuse );
			//dbPhyUpdateFluidVertex ( &pVertex [ iOffset++ ], p2.x, p2.y, p2.z, 1, 1, dwDiffuse );
			//dbPhyUpdateFluidVertex ( &pVertex [ iOffset++ ], p3.x, p3.y, p3.z, 0, 1, dwDiffuse );
		}
	}
}
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////