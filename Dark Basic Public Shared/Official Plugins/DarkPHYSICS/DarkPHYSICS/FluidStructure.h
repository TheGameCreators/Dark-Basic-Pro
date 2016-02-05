
#ifndef _FLUIDTRUCTURE_H_
#define _FLUIDTRUCTURE_H_

#include "globals.h"
#include "rigidbodycreation.h"

namespace PhysX
{
		struct sFluidInternalVertex
	{
		NxVec3 v3Position;
		NxVec3 v3Normal; 
	};

	struct sFluid
	{
		int								iID;
		int								iType;
		NxFluid*						pFluid;
		NxFluidEmitter*					pFluidEmitter;
		//NxImplicitScreenMesh*			pFluidMesh;
		NxFluidDesc						fluidDescription;
		NxFluidEmitterDesc				emitterDescription;
		//NxImplicitScreenMeshDesc		meshDescription;
		NxParticleData					particleData;
		NxU32							iNumVertices;
		NxU32							iNumIndices;
		NxVec3*							pParticleBuffer;
		NxU32							iParticleBufferCap;
		NxU32							iParticleBufferNum;
		NxReal*                         pParticleDensityBuffer;
		//int							iMaximum; // 250608 - using new fluid renderer now
		//PhysX::sFluidInternalVertex*	pVertices;
		//int*							pIndices;
		NxVec3							vecPosition;
		NxVec3							vecRotation;

		LPDIRECT3DVERTEXBUFFER9			vertexBuffer;
	};

	struct sDrain
	{
		int					iID;
		NxActor*			pActor;
	};

	struct sFluidVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	// Custom vertex and FVF declaration for point sprite vertex points
	struct sPointVertex
	{
		D3DXVECTOR3 pos;
		D3DCOLOR    color;

		enum FVF
		{
			FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
		};

		sPointVertex(D3DXVECTOR3 pos,D3DCOLOR color) : pos(pos), color(color) {};
	};

	extern std::vector < sFluid* > pFluidList;
	extern std::vector < sDrain* > pDrainList;
}

sObject*		dbGetObject			( int iID );
PhysX::sFluid*	dbPhyGetFluid		( int iID, bool bDisplayError = true );
PhysX::sDrain*	dbPhyGetDrain		( int iID );

NxFluid* dbPhyGetFluidData ( int iID );

#endif _FLUIDTRUCTURE_H_