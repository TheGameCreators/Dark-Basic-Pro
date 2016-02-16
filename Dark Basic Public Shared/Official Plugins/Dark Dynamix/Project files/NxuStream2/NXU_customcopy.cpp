#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "NXU_PhysicsExport.h"
#include "NXU_customcopy.h"
#include "NXU_schema.h"
#include "NXU_cooking.h"
#include "NXU_Streaming.h"
#include "NXU_string.h"

#ifdef WIN32
#pragma warning(disable:4996)
#endif

namespace NXU
{

bool gCookOnExport=true;

void copyBuffer(NxArray< NxU8 > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  buffer.push_back( *scan );
		  scan+=stride;
	  }
  }
}

void copyBuffer(NxArray< NxU32 > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxU32 *v = (const NxU32 *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}

void copyBuffer(NxArray< NxF32 > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxF32 *v = (const NxF32 *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}


void copyBuffer(NxArray< NxU16 > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxU16 *v = (const NxU16 *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}

void copyBuffer(NxArray< NxVec3 > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxVec3 *v = (const NxVec3 *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}

void copyBuffer(NxArray< NxTri > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxTri *v = (const NxTri *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}

void copyBuffer16(NxArray< NxTri > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxU16 *t = (const NxU16 *) scan;
		  NxTri v;

		  v.a = t[0];
		  v.b = t[1];
		  v.c = t[2];

		  buffer.push_back( v );

		  scan+=stride;
	  }
  }
}


void copyBuffer(NxArray< NxTetra > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxTetra *v = (const NxTetra *) scan;
		  buffer.push_back( *v );
		  scan+=stride;
	  }
  }
}

void copyBuffer16(NxArray< NxTetra > &buffer,const void *source,NxU32 count,NxU32 stride)
{
	buffer.clear();
  if ( source )
  {
	  const char *scan = (const char *) source;
	  for (NxU32 i=0; i<count; i++)
	  {
		  const NxU16 *t = (const NxU16 *) scan;
		  NxTetra v;

		  v.a = t[0];
		  v.b = t[1];
		  v.c = t[2];
		  v.d = t[3];

		  buffer.push_back( v );

		  scan+=stride;
	  }
  }
}



/*****************************************************************
CUSTOM: mId                            of type const char *       DONE
CUSTOM: mUserProperties                of type const char *       DONE
CUSTOM: mPoints                        of type NxArray< NxVec3 >  DONE
CUSTOM: mTriangles                     of type NxArray< NxTri >   DONE
CUSTOM: mCookedDataSize                of type NxU32              DONE
CUSTOM: mCookedData                    of type NxArray< NxU8 >    DONE
*****************************************************************/
void CustomCopy::customCopyFrom(NxConvexMeshDesc &cdesc,const ::NxConvexMeshDesc &desc)
{
  if ( gCookOnExport )
  {
	  MemoryWriteBuffer wb;
	  InitCooking();
#ifdef _DEBUG
  	  bool ok = CookConvexMesh(desc,wb);
  	  assert(ok);
#else
	  CookConvexMesh(desc,wb);
#endif
	  CloseCooking();
	  cdesc.mCookedDataSize = wb.currentSize;
	  copyBuffer(cdesc.mCookedData, wb.data, wb.currentSize, sizeof(NxU8) );
  }

	copyBuffer(cdesc.mPoints, desc.points, desc.numVertices, desc.pointStrideBytes );

	if ( desc.flags & NX_CF_16_BIT_INDICES )
 		copyBuffer16(cdesc.mTriangles, desc.triangles, desc.numTriangles, desc.triangleStrideBytes );
 	else
  	copyBuffer(cdesc.mTriangles, desc.triangles, desc.numTriangles, desc.triangleStrideBytes );

}

void CustomCopy::customCopyTo(::NxConvexMeshDesc &cdesc,const NxConvexMeshDesc &desc)
{
  cdesc.numVertices         = desc.mPoints.size();
  cdesc.numTriangles        = desc.mTriangles.size();
  cdesc.pointStrideBytes    = sizeof(NxVec3);
  cdesc.triangleStrideBytes = sizeof(NxTri);

  if ( cdesc.numVertices )
  {
    cdesc.points = &desc.mPoints[0].x;
  }

  if ( cdesc.numTriangles )
  {
    cdesc.triangles = &desc.mTriangles[0].a;
  }

}


/*****************************************************************************
CUSTOM: mId                            of type const char *           DONE
CUSTOM: mUserProperties                of type const char *           DONE
CUSTOM: materialIndices                of type NxArray< NxU32 >       DONE
CUSTOM: mPmapSize                      of type NxU32                  DONE
CUSTOM: mPmapDensity                   of type NxU32                  DONE
CUSTOM: mPmapData                      of type const char *           DONE
CUSTOM: mCookedDataSize                of type NxU32                  DONE
CUSTOM: mCookedData                    of type NxArray< NxU8 >        DONE
******************************************************************************/
void CustomCopy::customCopyFrom(NxTriangleMeshDesc &cdesc,const ::NxTriangleMeshDesc &desc)
{
	if ( gCookOnExport )
	{
		InitCooking();
		MemoryWriteBuffer wb;
#ifdef _DEBUG
  		bool ok = CookTriangleMesh(desc,wb);
  		assert(ok);
#else
		CookTriangleMesh(desc,wb);
#endif
		CloseCooking();
		cdesc.mCookedDataSize = wb.currentSize;
		copyBuffer(cdesc.mCookedData, wb.data, wb.currentSize, sizeof(NxU8) );
	}
	if (desc.materialIndices != 0)
	{
		cdesc.mMaterialIndices.resize(desc.numTriangles);
		copyBuffer(cdesc.mMaterialIndices, desc.materialIndices, desc.numTriangles, desc.materialIndexStride);
	}
}


void CustomCopy::customCopyTo(::NxTriangleMeshDesc &cdesc,const NxTriangleMeshDesc &desc)
{
	if (!desc.mCookedDataSize)
	{
		cdesc.materialIndexStride = sizeof(NxU32);
		cdesc.materialIndices = &desc.mMaterialIndices[0];
	}
}


/*********************************************************************************
CUSTOM: mId                            of type const char *      DONE
CUSTOM: mUserProperties                of type const char *      DONE
CUSTOM: mHasSpring                     of type NX_BOOL           DONE
CUSTOM: mMaterialIndex                 of type NxU16             DONE
CUSTOM: mSpring                        of type NxSpringDesc      DONE
**********************************************************************************/
void CustomCopy::customCopyFrom(NxMaterialDesc &cdesc,const ::NxMaterialDesc &desc)
{
	if ( desc.spring )
	{
		cdesc.mHasSpring = true;
		cdesc.mSpring.copyFrom(*desc.spring,*this);
	}
}

void CustomCopy::customCopyTo(::NxMaterialDesc &cdesc,const NxMaterialDesc &desc)
{
  if ( desc.mHasSpring )
  {
		cdesc.spring = (::NxSpringDesc *)&desc.mSpring; //*WARNING* ABSOLUTELY REQUIRES OUR MIRRORED COPY BEING IDENTICAL TO THE SDK VERSION!
  }
}


/**********************************************************************************
CUSTOM: mUserProperties                of type const char *     DONE
CUSTOM: mCCDSkeleton                   of type const char *     DONE
***********************************************************************************/
void CustomCopy::customCopyFrom(NxShapeDesc &cdesc,const ::NxShapeDesc &desc)
{
	cdesc.mShapeDensity = desc.density;
	cdesc.mShapeMass    = desc.mass;
  cdesc.mCCDSkeleton = getNameFromSkeleton( desc.ccdSkeleton );
}

void CustomCopy::customCopyTo(::NxShapeDesc &cdesc,const NxShapeDesc &desc)
{
	cdesc.density = desc.mShapeDensity;
	cdesc.mass    = desc.mShapeMass;
  cdesc.ccdSkeleton   = getSkeletonFromName(desc.mCCDSkeleton);
 	cdesc.materialIndex = getRemapMaterialIndex(desc.materialIndex);
}

#if NX_SDK_VERSION_NUMBER >= 272
void CustomCopy::customCopyFrom(NxForceFieldShapeDesc &cdesc,const ::NxForceFieldShapeDesc &desc)
{
}
void CustomCopy::customCopyTo(::NxForceFieldShapeDesc &cdesc,const NxForceFieldShapeDesc &desc)
{
}

/***************************************************************************
CUSTOM: mMeshData                      of type const char *        DONE
****************************************************************************/
void CustomCopy::customCopyFrom(NxConvexForceFieldShapeDesc &cdesc,const ::NxConvexForceFieldShapeDesc &desc)
{
	cdesc.mMeshData = getNameFromConvexMesh(desc.meshData);
}

void CustomCopy::customCopyTo(::NxConvexForceFieldShapeDesc &cdesc,const NxConvexForceFieldShapeDesc &desc)
{
	cdesc.meshData = getConvexMeshFromName( desc.mMeshData );
}
#endif

/********************************************************************************
CUSTOM: mId                            of type const char *              DONE
CUSTOM: mUserProperties                of type const char *              DONE
CUSTOM: mHasBody                       of type NX_BOOL                   DONE
CUSTOM: mBody                          of type NxBodyDesc                DONE
CUSTOM: mCompartment                   of type const char *              DONE
CUSTOM: mShapes                        of type NxArray< NxShapeDesc *>   DONE
*********************************************************************************/
void CustomCopy::customCopyFrom(NxActorDesc &cdesc,const ::NxActorDesc &desc)
{
	if ( desc.body )
	{
		cdesc.mHasBody = true;
		cdesc.mBody.copyFrom(*desc.body,*this);
	}
	else
	{
		cdesc.mHasBody = false;
	}
#if NX_SDK_VERSION_NUMBER >= 260
	cdesc.mCompartment = getNameFromCompartment( desc.compartment );
#endif


}

void CustomCopy::customCopyTo(::NxActorDesc &cdesc,const NxActorDesc &desc)
{
#if NX_SDK_VERSION_NUMBER >= 260
  cdesc.compartment = getCompartmentFromName(desc.mCompartment);
#endif
}

/**********************************************************************************
CUSTOM: mId                            of type const char *                 DONE
CUSTOM: mUserProperties                of type const char *                 DONE
CUSTOM: mActor0                        of type const char *                 DONE
CUSTOM: mActor1                        of type const char *                 DONE
CUSTOM: mPlaneLimitPoint               of type NxVec3                       DONE
CUSTOM: mOnActor2                      of type NX_BOOL                      DONE
CUSTOM: mPlaneInfo                     of type NxArray< NxPlaneInfoDesc *>  DONE
***********************************************************************************/
void CustomCopy::customCopyFrom(NxJointDesc &cdesc,const ::NxJointDesc &desc)
{
	cdesc.mActor0 = getNameFromActor( desc.actor[0] );
	cdesc.mActor1 = getNameFromActor( desc.actor[1] );
}

void CustomCopy::customCopyTo(::NxJointDesc &cdesc,const NxJointDesc &desc)
{
	cdesc.actor[0] = getActorFromName( desc.mActor0 );
	cdesc.actor[1] = getActorFromName( desc.mActor1 );
}


#if NX_USE_FLUID_API

/************************************************************************************
CUSTOM: mId                            of type const char *     DONE
CUSTOM: mUserProperties                of type const char *     DONE
CUSTOM: mFrameActor                    of type const char *     DONE (by import/export)
CUSTOM: mFrameShape                    of type NxU32            DONE (by import/export)
*************************************************************************************/
void CustomCopy::customCopyFrom(NxFluidEmitterDesc &cdesc,const ::NxFluidEmitterDesc &desc)
{
}

void CustomCopy::customCopyTo(::NxFluidEmitterDesc &cdesc,const NxFluidEmitterDesc &desc)
{
}

#endif

/*****************************************************************************************
CUSTOM: mId                            of type const char *                           DONE
CUSTOM: mUserProperties                of type const char *                           DONE
CUSTOM: mHasMaxBounds                  of type NX_BOOL                                DONE
CUSTOM: mHasLimits                     of type NX_BOOL                                DONE
CUSTOM: mHasFilter                     of type NX_BOOL                                DONE
CUSTOM: mFilterBool                    of type NX_BOOL                                DONE
CUSTOM: mFilterOp0                     of type NxFilterOp                             DONE
CUSTOM: mFilterOp1                     of type NxFilterOp                             DONE
CUSTOM: mFilterOp2                     of type NxFilterOp                             DONE
CUSTOM: mGroupMask0                    of type NxGroupsMask                           DONE
CUSTOM: mGroupMask1                    of type NxGroupsMask                           DONE
CUSTOM: mMaxBounds                     of type NxBounds3                              DONE
CUSTOM: mLimits                        of type NxSceneLimits                          DONE
CUSTOM: mMaterials                     of type NxArray< NxMaterialDesc * >            DONE
CUSTOM: mActors                        of type NxArray< NxActorDesc *>                DONE
CUSTOM: mJoints                        of type NxArray< NxJointDesc *>                DONE
CUSTOM: mPairFlags                     of type NxArray< NxPairFlagDesc *>             DONE
CUSTOM: mEffectors                     of type NxArray< NxSpringAndDamperEffectorDesc *> DONE
CUSTOM: mCollisionGroups               of type NxArray< NxCollisionGroupDesc *>       DONE
CUSTOM: mCompartments                  of type NxArray< NxCompartmentDesc *>          DONE
CUSTOM: mFluids                        of type NxArray< NxFluidDesc *>                DONE
CUSTOM: mCloths                        of type NxArray< NxClothDesc *>                DONE
******************************************************************************************/
void CustomCopy::customCopyFrom(NxSceneDesc &cdesc,const ::NxSceneDesc &desc)
{
	if ( desc.maxBounds )
	{
		cdesc.mHasMaxBounds = true;
		cdesc.mMaxBounds    = *desc.maxBounds;
	}
	else
	{
		cdesc.mHasMaxBounds = false;
	}

	if ( desc.limits )
	{
		cdesc.mHasLimits = true;
		cdesc.mLimits.copyFrom(*desc.limits,*this);
	}
	else
	{
		cdesc.mHasLimits = false;
	}

}

void CustomCopy::customCopyTo(::NxSceneDesc &cdesc,const NxSceneDesc &desc)
{
  if ( desc.mHasMaxBounds )
  {
    cdesc.maxBounds = (NxBounds3 *)&desc.mMaxBounds;
  }
  if ( desc.mHasLimits )
  {
		cdesc.limits = (::NxSceneLimits *)&desc.mLimits;  // WARNING: Mirrored limits structure must be identical to the SDK!
  }
}


#if NX_USE_FLUID_API

/************************************************************************
CUSTOM: mUserProperties                of type const char *                      DONE
CUSTOM: mCompartment                   of type const char *                      DONE
CUSTOM: mInitialData                                                             DONE
CUSTOM: mEmitters                      of type NxArray< NxFluidEmitterDesc *>    DONE
*************************************************************************/
void CustomCopy::customCopyFrom(NxFluidDesc &cdesc,const ::NxFluidDesc &desc)
{
	cdesc.mMaxFluidParticles = desc.maxParticles;
	customCopyFrom(cdesc.mInitialParticleData,desc.particlesWriteData);
#if NX_SDK_VERSION_NUMBER >= 260
  cdesc.mCompartment = getNameFromCompartment( desc.compartment );
#endif
}

void CustomCopy::customCopyTo(::NxFluidDesc &cdesc,const NxFluidDesc &desc)
{
#if NX_SDK_VERSION_NUMBER >= 260
  cdesc.compartment = getCompartmentFromName(desc.mCompartment);
#endif
	customCopyTo(cdesc.initialParticleData,desc.mInitialParticleData);

	cdesc.maxParticles = desc.mMaxFluidParticles;

	if ( desc.mEmitters.size() )
	{
		for (unsigned int i=0; i<desc.mEmitters.size(); i++)
		{
			NxFluidEmitterDesc *d = (NxFluidEmitterDesc *)desc.mEmitters[i];
			::NxFluidEmitterDesc e;
			d->copyTo(e,*this);
			cdesc.emitters.push_back(e);
		}
	}

}
#endif

/**********************************************************************************
CUSTOM: mId                            of type const char *         DONE
CUSTOM: mUserProperties                of type const char *         DONE
CUSTOM: mClothMesh                     of type const char *         DONE
CUSTOM: mCompartment                   of type const char *         DONE
***********************************************************************************/
void CustomCopy::customCopyFrom(NxClothDesc &cdesc,const ::NxClothDesc &desc)
{
  cdesc.mClothMesh   = getNameFromClothMesh( desc.clothMesh);
#if NX_SDK_VERSION_NUMBER >= 260
  cdesc.mCompartment = getNameFromCompartment( desc.compartment );
#endif
}

void CustomCopy::customCopyTo(::NxClothDesc &cdesc,const NxClothDesc &desc)
{
  cdesc.clothMesh   = getClothMeshFromName(desc.mClothMesh);
#if NX_SDK_VERSION_NUMBER >= 260
  cdesc.compartment = getCompartmentFromName(desc.mCompartment);
#endif
}


/***************************************************************************************
CUSTOM: mId                            of type const char *         DONE
CUSTOM: mUserProperties                of type const char *         DONE
CUSTOM: mBody1                         of type const char *         DONE
CUSTOM: mBody2                         of type const char *         DONE
****************************************************************************************/
void CustomCopy::customCopyFrom(NxSpringAndDamperEffectorDesc &cdesc,const ::NxSpringAndDamperEffectorDesc &desc)
{
  cdesc.mBody1 = getNameFromActor(desc.body1);
	cdesc.mBody2 = getNameFromActor(desc.body2);
}

void CustomCopy::customCopyTo(::NxSpringAndDamperEffectorDesc &cdesc,const NxSpringAndDamperEffectorDesc &desc)
{
	cdesc.body1 = getActorFromName( desc.mBody1 );
	cdesc.body2 = getActorFromName( desc.mBody2 );
}

NxU16        CustomCopy::getRemapMaterialIndex(NxU16 original)
{
#if 0
	NxU16 ret = 0;

	if ( mCurrentScene )
	{
		for (size_t i=0; i<mCurrentScene->mMaterials.size(); i++)
		{
			NxMaterialDesc *md = mCurrentScene->mMaterials[i];
			if ( md->mMaterialIndex == original )
			{
				ret = md->mInstanceIndex;
				break;
			}
		}
	}

  return ret;
#else
  return original;
#endif
}

NxActor    * CustomCopy::getActorFromName(const char *name)
{
	NxActor *ret = 0;
	if ( mCurrentScene && name )
	{
		for (unsigned int i=0; i<mCurrentScene->mActors.size(); i++)
		{
			NxActorDesc *a = mCurrentScene->mActors[i];
			if ( a->mId && strcmp(a->mId,name) == 0 )
			{
				ret = (NxActor *)a->mInstance;
				break;
			}
		}
	}

	return ret;
}

const char * CustomCopy::getNameFromActor(NxActor *actor)
{
	const char *ret = 0;
	if ( mCurrentScene && actor )
	{
		for (unsigned int i=0; i<mCurrentScene->mActors.size(); i++)
		{
			NxActorDesc *a = mCurrentScene->mActors[i];
			if ( a->mInstance == actor )
			{
				ret = a->mId;
				break;
			}
		}
	}
	return ret;
}

NxSceneDesc *         CustomCopy::locateSceneDesc(NxuPhysicsCollection *pc,const char *id,NxU32 &index)
{
  NxSceneDesc *ret = 0;

  if ( pc && id )
  {
    for (NxU32 i=0; i<pc->mScenes.size(); i++)
    {
      NxSceneDesc *si = pc->mScenes[i];
      if ( si->mId && strcmp(si->mId,id) == 0 )
      {
        index = i;
        ret = si;
        break;
      }
    }
  }

  return ret;
}

NxSceneInstanceDesc * CustomCopy::locateSceneInstanceDesc(NxuPhysicsCollection *pc,const char *id,NxU32 &index)
{
  NxSceneInstanceDesc *ret = 0;

  if ( pc && id )
  {
    for (NxU32 i=0; i<pc->mSceneInstances.size(); i++)
    {
      NxSceneInstanceDesc *si = pc->mSceneInstances[i];
      if ( si->mId && strcmp(si->mId,id) == 0 )
      {
        index = i;
        ret = si;
        break;
      }
    }
  }

  return ret;
}

void CustomCopy::customCopyFrom(NxSimpleTriangleMesh &cdesc,const ::NxSimpleTriangleMesh &desc)
{
	copyBuffer(cdesc.mPoints, desc.points, desc.numVertices, desc.pointStrideBytes );
	if ( desc.flags & NX_MF_16_BIT_INDICES )
 		copyBuffer16(cdesc.mTriangles, desc.triangles, desc.numTriangles, desc.triangleStrideBytes );
 	else
  	copyBuffer(cdesc.mTriangles, desc.triangles, desc.numTriangles, desc.triangleStrideBytes );
}

void CustomCopy::customCopyTo(::NxSimpleTriangleMesh &cdesc,const NxSimpleTriangleMesh &desc)
{
  cdesc.numVertices = desc.mPoints.size();
  cdesc.numTriangles = desc.mTriangles.size();
  cdesc.pointStrideBytes = sizeof(NxVec3);
  cdesc.triangleStrideBytes = sizeof(NxTri);
  if ( cdesc.numVertices )
  {
    cdesc.points = &desc.mPoints[0].x;
  }
  if ( cdesc.numTriangles )
  {
    cdesc.triangles = &desc.mTriangles[0].a;
  }
}


/***************************************************************************
CUSTOM: mMeshData                      of type const char *        DONE
****************************************************************************/
void CustomCopy::customCopyFrom(NxConvexShapeDesc &cdesc,const ::NxConvexShapeDesc &desc)
{
	cdesc.mMeshData = getNameFromConvexMesh(desc.meshData);
}

void CustomCopy::customCopyTo(::NxConvexShapeDesc &cdesc,const NxConvexShapeDesc &desc)
{
	cdesc.meshData = getConvexMeshFromName( desc.mMeshData );
}


/**************************************************************************
CUSTOM: mSamples            of type NxArray< NxU16 >     DONE
****************************************************************************/
void CustomCopy::customCopyFrom(NxHeightFieldDesc &cdesc,const ::NxHeightFieldDesc &desc)
{
	if ( desc.samples )
	{
	  copyBuffer(cdesc.mSamples, desc.samples, desc.nbRows*desc.nbColumns, desc.sampleStride );
	}
}

void CustomCopy::customCopyTo(::NxHeightFieldDesc &cdesc,const NxHeightFieldDesc &desc)
{
	cdesc.sampleStride = sizeof(NxU32);
	if ( desc.mSamples.size() )
	{
		cdesc.samples = (void *)&desc.mSamples[0];
	}
}


void CustomCopy::customCopyFrom(NxClothMeshDesc &cdesc,const ::NxClothMeshDesc &desc)
{
#if	NX_SDK_VERSION_NUMBER	>= 250
	if (desc.vertexFlags)
	{
		copyBuffer(cdesc.mVertexFlags,desc.vertexFlags, desc.numVertices, desc.vertexFlagStrideBytes);
	}
	if (desc.vertexMasses)
	{
		copyBuffer(cdesc.mVertexMasses, desc.vertexMasses, desc.numVertices, desc.vertexMassStrideBytes);
	}
	NxU32 flags = 0;
	flags |= (desc.flags & NX_CLOTH_MESH_TEARABLE);
#if	NX_SDK_VERSION_NUMBER >= 280
	flags |= (desc.flags & NX_CLOTH_MESH_WELD_VERTICES);
#endif
	cdesc.mMeshFlags = (NxClothMeshFlags) flags;
#endif
}

void CustomCopy::customCopyTo(::NxClothMeshDesc &cdesc,const NxClothMeshDesc &desc)
{
#if NX_SDK_VERSION_NUMBER >= 250
  if ( desc.mVertexFlags.size() )
  {
  	cdesc.vertexFlags = &desc.mVertexFlags[0];
  	cdesc.vertexFlagStrideBytes = sizeof(NxU32);
  }
  else
  {
  	cdesc.vertexFlags = 0;
  	cdesc.vertexFlagStrideBytes = 0;
  }
  if ( desc.mVertexMasses.size() )
  {
  	cdesc.vertexMasses = &desc.mVertexMasses[0];
  	cdesc.vertexMassStrideBytes = sizeof(float);
  }
  else
  {
  	cdesc.vertexMasses = 0;
  	cdesc.vertexMassStrideBytes = 0;
  }
  cdesc.flags |= (desc.mMeshFlags & NX_CLOTH_MESH_TEARABLE);
#if	NX_SDK_VERSION_NUMBER >= 280
  cdesc.flags |= (desc.mMeshFlags & NX_CLOTH_MESH_WELD_VERTICES);
#endif
#endif
}


/****************************************************************************
CUSTOM: mMeshData                      of type const char *        DONE
*****************************************************************************/
void CustomCopy::customCopyFrom(NxTriangleMeshShapeDesc &cdesc,const ::NxTriangleMeshShapeDesc &desc)
{
	cdesc.mMeshData = getNameFromTriangleMesh(desc.meshData);
}

void CustomCopy::customCopyTo(::NxTriangleMeshShapeDesc &cdesc,const NxTriangleMeshShapeDesc &desc)
{
	cdesc.meshData = getTriangleMeshFromName( desc.mMeshData );
}

/**********************************************************************************
CUSTOM: mHeightField                   of type const char *          DONE
CUSTOM: mMaterialIndexHighBits         of type NxU16                 **NOT DONE
***********************************************************************************/
void CustomCopy::customCopyFrom(NxHeightFieldShapeDesc &cdesc,const ::NxHeightFieldShapeDesc &desc)
{
	cdesc.mHeightField = getNameFromHeightField(desc.heightField);
}

/**** WARNING

TODO: Need to update the materialHightBits

****/
void CustomCopy::customCopyTo(::NxHeightFieldShapeDesc &cdesc,const NxHeightFieldShapeDesc &desc)
{
	cdesc.heightField  = getHeightFieldFromName( desc.mHeightField );
	cdesc.holeMaterial = getRemapMaterialIndex(desc.holeMaterial); // translate the material index
}

#if NX_USE_FLUID_API
/************************************************************************************
CUSTOM: mBufferPos                      of type NxArray< NxVec3 >    DONE
CUSTOM: mBufferVel                      of type NxArray< NxVec3 >    DONE
CUSTOM: mBufferLife                     of type NxArray< NxF32 >     DONE
CUSTOM: mBufferDensity                  of type NxArray< NxF32 >     DONE
CUSTOM: mBufferId                       of type NxArray< NxU32 >     DONE
CUSTOM: mBufferFlag                     of type NxArray< NxU32 >     DONE
*************************************************************************************/
void CustomCopy::customCopyFrom(NxParticleData &cdesc,const ::NxParticleData &desc)
{
#if NX_SDK_VERSION_NUMBER < 270
	cdesc.maxParticles = 0;
#endif
	if ( desc.numParticlesPtr )
	{
		NxU32 numParticles = *desc.numParticlesPtr;
#if NX_SDK_VERSION_NUMBER < 270
		cdesc.maxParticles = numParticles;
#endif
		if ( numParticles )
		{
			if ( desc.bufferPos && desc.bufferPosByteStride )
			{
				copyBuffer(cdesc.mBufferPos,desc.bufferPos, numParticles, desc.bufferPosByteStride );
			}
			if ( desc.bufferVel && desc.bufferVelByteStride )
			{
				copyBuffer(cdesc.mBufferVel,desc.bufferVel, numParticles, desc.bufferVelByteStride );
			}
			if ( desc.bufferLife && desc.bufferLifeByteStride )
			{
				copyBuffer(cdesc.mBufferLife, desc.bufferLife, numParticles, desc.bufferLifeByteStride );
			}
			if ( desc.bufferDensity && desc.bufferDensityByteStride )
			{
				copyBuffer(cdesc.mBufferDensity, desc.bufferDensity, numParticles, desc.bufferDensityByteStride );
			}
#if NX_SDK_VERSION_NUMBER >= 260
			if ( desc.bufferId && desc.bufferIdByteStride )
			{
				copyBuffer(cdesc.mBufferId, desc.bufferId, numParticles, desc.bufferIdByteStride );
			}
			if ( desc.bufferFlag && desc.bufferFlagByteStride )
			{
				copyBuffer(cdesc.mBufferFlag, desc.bufferFlag, numParticles, desc.bufferFlagByteStride );
			}
#endif
		}
	}
}

void CustomCopy::customCopyTo(::NxParticleData &cdesc,const NxParticleData &_desc)
{
	NxParticleData *pd = (NxParticleData *) &_desc;
	NxParticleData &desc = *pd;
	if ( desc.mBufferPos.size() )
	{

		cdesc.numParticlesPtr = &desc.mParticleCount;
		desc.mParticleCount   = desc.mBufferPos.size();
 		cdesc.bufferPos       = (NxF32 *)&desc.mBufferPos[0].x;
 		cdesc.bufferPosByteStride = sizeof(NxVec3);

 		if ( desc.mBufferVel.size() == desc.mParticleCount )
 		{
 			cdesc.bufferVel = (NxF32 *)&desc.mBufferVel[0].x;
 			cdesc.bufferVelByteStride = sizeof(NxVec3);
 		}
 		else
 		{
 			cdesc.bufferVel = 0;
 			cdesc.bufferVelByteStride = 0;
 		}

    if ( desc.mBufferLife.size() == desc.mParticleCount )
    {
    	cdesc.bufferLife = (NxF32 *)&desc.mBufferLife[0];
    	cdesc.bufferLifeByteStride = sizeof(NxF32);
    }
    else
    {
    	cdesc.bufferLife = 0;
    	cdesc.bufferLifeByteStride = 0;
    }

    if ( desc.mBufferDensity.size() == desc.mParticleCount )
    {
    	cdesc.bufferDensity = (NxF32 *)&desc.mBufferDensity[0];
    	cdesc.bufferDensityByteStride = sizeof(NxF32);
    }
    else
    {
    	cdesc.bufferDensity = 0;
    	cdesc.bufferDensityByteStride = 0;
    }
#if NX_SDK_VERSION_NUMBER >= 260
    if ( desc.mBufferId.size() == desc.mParticleCount )
    {
    	cdesc.bufferId = (NxU32 * )&desc.mBufferId[0];
    	cdesc.bufferIdByteStride = sizeof(NxU32);
    }
    else
    {
    	cdesc.bufferId = 0;
    	cdesc.bufferIdByteStride = 0;
    }

    if ( desc.mBufferFlag.size() == desc.mParticleCount )
    {
    	cdesc.bufferFlag = (NxU32 *)&desc.mBufferFlag[0];
    	cdesc.bufferFlagByteStride = sizeof(NxU32);
    }
    else
    {
    	cdesc.bufferFlag = 0;
    	cdesc.bufferFlagByteStride = 0;
    }
#endif

	}
}
#endif

NxConvexMesh * CustomCopy::getConvexMeshFromName(const char *name)
{
	NxConvexMesh *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mConvexMeshes.size(); i++)
  	{
  		NxConvexMeshDesc *sd = mCurrentCollection->mConvexMeshes[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxConvexMesh *) sd->mInstance;
				if ( ret == 0 )
				{
					reportWarning("ConvexMesh %s was not successfully created.", name );
				}
  			break;
  		}
  	}
  }

  return ret;
}

NxConvexMeshDesc * CustomCopy::getConvexMeshDescFromName(const char *name)
{
	NxConvexMeshDesc *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mConvexMeshes.size(); i++)
  	{
  		NxConvexMeshDesc *sd = mCurrentCollection->mConvexMeshes[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = sd;
  			break;
  		}
  	}
  }

  return ret;
}

const char *   CustomCopy::getNameFromConvexMesh(NxConvexMesh *mesh)
{
	const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mConvexMeshes.size(); i++)
  	{
  		NxConvexMeshDesc *sd = mCurrentCollection->mConvexMeshes[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}


NxTriangleMesh * CustomCopy::getTriangleMeshFromName(const char *name)
{
	NxTriangleMesh *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mTriangleMeshes.size(); i++)
  	{
  		NxTriangleMeshDesc *sd = mCurrentCollection->mTriangleMeshes[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxTriangleMesh *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

const char *   CustomCopy::getNameFromTriangleMesh(NxTriangleMesh *mesh)
{
	const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mTriangleMeshes.size(); i++)
  	{
  		NxTriangleMeshDesc *sd = mCurrentCollection->mTriangleMeshes[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

NxHeightField * CustomCopy::getHeightFieldFromName(const char *name)
{
	NxHeightField *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mHeightFields.size(); i++)
  	{
  		NxHeightFieldDesc *sd = mCurrentCollection->mHeightFields[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxHeightField *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

const char *   CustomCopy::getNameFromHeightField(NxHeightField *mesh)
{
	const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mHeightFields.size(); i++)
  	{
  		NxHeightFieldDesc *sd = mCurrentCollection->mHeightFields[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

NxForceField * CustomCopy::getForceFieldFromName(const char *name)
{
	NxForceField *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && name )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFields.size(); i++)
		{
			NxForceFieldDesc *fd = mCurrentScene->mForceFields[i];
			if ( strcmp(fd->mId,name) == 0 )
			{
				ret = (NxForceField *) fd->mInstance;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}

const char *  CustomCopy::getNameFromForceField(NxForceField *field)
{
	const char *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && field )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFields.size(); i++)
		{
			NxForceFieldDesc *fd = mCurrentScene->mForceFields[i];
			if ( fd->mInstance == field )
			{
				ret = fd->mId;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}

NxForceFieldShapeGroup * CustomCopy::getForceFieldShapeGroupFromName(const char *name)
{
	NxForceFieldShapeGroup *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && name )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFieldShapeGroups.size(); i++)
		{
			NxForceFieldShapeGroupDesc *sg = mCurrentScene->mForceFieldShapeGroups[i];
			if ( strcmp(sg->mId,name) == 0 )
			{
				ret = (NxForceFieldShapeGroup *) sg->mInstance;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}

const char *  CustomCopy::getNameFromForceFieldShapeGroup(NxForceFieldShapeGroup *group)
{
	const char *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && group )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFieldShapeGroups.size(); i++)
		{
			NxForceFieldShapeGroupDesc *sg = mCurrentScene->mForceFieldShapeGroups[i];
			if ( sg->mInstance == group )
			{
				ret = sg->mId;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}

NxForceFieldLinearKernel * CustomCopy::getForceFieldLinearKernelFromName(const char *name)
{
	NxForceFieldLinearKernel *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && name )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFieldLinearKernels.size(); i++)
		{
			NxForceFieldLinearKernelDesc *fd = mCurrentScene->mForceFieldLinearKernels[i];
			if ( strcmp(fd->mId,name) == 0 )
			{
				ret = (NxForceFieldLinearKernel *) fd->mInstance;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}

const char *  CustomCopy::getNameFromForceFieldLinearKernel(NxForceFieldLinearKernel *kernel)
{
	const char *ret = 0;

	assert(mCurrentScene);
	if ( mCurrentScene && kernel )
	{
		for (unsigned int i=0; i<mCurrentScene->mForceFieldLinearKernels.size(); i++)
		{
			NxForceFieldLinearKernelDesc *fd = mCurrentScene->mForceFieldLinearKernels[i];
			if ( fd->mInstance == kernel )
			{
				ret = fd->mId;
				assert(ret);
				break;
			}
		}
	}

	return ret;
}


NxClothMesh * CustomCopy::getClothMeshFromName(const char *name)
{
	NxClothMesh *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mClothMeshes.size(); i++)
  	{
  		NxClothMeshDesc *sd = mCurrentCollection->mClothMeshes[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxClothMesh *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

const char *   CustomCopy::getNameFromClothMesh(NxClothMesh *mesh)
{
	const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mClothMeshes.size(); i++)
  	{
  		NxClothMeshDesc *sd = mCurrentCollection->mClothMeshes[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

#if NX_SDK_VERSION_NUMBER >= 260
NxCompartment * CustomCopy::getCompartmentFromName(const char *name)
{
	NxCompartment *ret = 0;

  assert(mCurrentScene);

  if ( mCurrentScene && name )
  {
  	for (unsigned int i=0; i<mCurrentScene->mCompartments.size(); i++)
  	{
  		NxCompartmentDesc *sd = mCurrentScene->mCompartments[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxCompartment *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}

		assert(ret);
  }
  return ret;
}

const char *    CustomCopy::getNameFromCompartment(NxCompartment *nc)
{
	const char *ret = 0;

  assert(mCurrentScene);
  if ( mCurrentScene && nc )
  {
  	for (unsigned int i=0; i<mCurrentScene->mCompartments.size(); i++)
  	{
  		NxCompartmentDesc *sd = mCurrentScene->mCompartments[i];
  		if ( sd->mInstance == nc )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
    if ( ret == 0 ) // new compartment, we have to add it!
    {
      ::NxCompartmentDesc cd;
#if 0 // Not yet available
			c->saveToDesc(desc);
#else
			cd.type = nc->getType();
			cd.deviceCode = nc->getDeviceCode();
			cd.gridHashCellSize = nc->getGridHashCellSize();
			cd.gridHashTablePower = nc->gridHashTablePower();
#if NX_SDK_VERSION_NUMBER >= 270
			cd.flags = nc->getFlags();
			cd.timeScale = nc->getTimeScale();
#endif
#endif

      NxCompartmentDesc *ncd = new NxCompartmentDesc;
			ncd->mInstance = nc;
      char scratch[512];
      sprintf(scratch,"Compartment_%d", mCurrentScene->mCompartments.size() );
      ret = ncd->mId = getGlobalString(scratch);
      ncd->copyFrom(cd,*this);
      mCurrentScene->mCompartments.push_back(ncd);
    }
  }

  return ret;
}
#endif

void CustomCopy::initCollection(void)
{
  if ( mCurrentCollection )
  {
    mCurrentCollection->mInstance = 0;
    mCurrentCollection->mCurrentSceneInstance = 0;
    mCurrentCollection->mCurrentScene = 0;

    for (NxU32 i=0; i<mCurrentCollection->mConvexMeshes.size(); i++)    mCurrentCollection->mConvexMeshes[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentCollection->mTriangleMeshes.size(); i++)  mCurrentCollection->mTriangleMeshes[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentCollection->mHeightFields.size(); i++)    mCurrentCollection->mHeightFields[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentCollection->mSkeletons.size(); i++)       mCurrentCollection->mSkeletons[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentCollection->mClothMeshes.size(); i++)     mCurrentCollection->mClothMeshes[i]->mInstance = 0;

    for (NxU32 i=0; i<mCurrentCollection->mScenes.size(); i++)
    {
      mCurrentScene = mCurrentCollection->mScenes[i];
      initScene();
    }
    mCurrentScene = 0;
  }
}

void CustomCopy::initScene(void)
{
  if ( mCurrentScene )
  {
    for (NxU32 i=0; i<mCurrentScene->mMaterials.size(); i++) mCurrentScene->mMaterials[i]->mInstanceIndex = 0;
    for (NxU32 i=0; i<mCurrentScene->mActors.size(); i++) mCurrentScene->mActors[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentScene->mJoints.size(); i++) mCurrentScene->mJoints[i]->mInstance = 0;
    for (NxU32 i=0; i<mCurrentScene->mEffectors.size(); i++) mCurrentScene->mEffectors[i]->mInstance = 0;
#if NX_SDK_VERSION_NUMBER >= 260
    for (NxU32 i=0; i<mCurrentScene->mCompartments.size(); i++) mCurrentScene->mCompartments[i]->mInstance = 0;
#endif
  }
}


NxCCDSkeleton * CustomCopy::getSkeletonFromName(const char *name)
{
  NxCCDSkeleton *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mSkeletons.size(); i++)
  	{
  		NxCCDSkeletonDesc *sd = mCurrentCollection->mSkeletons[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxCCDSkeleton *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

const char *    CustomCopy::getNameFromSkeleton(NxCCDSkeleton *mesh)
{
  const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mSkeletons.size(); i++)
  	{
  		NxCCDSkeletonDesc *sd = mCurrentCollection->mSkeletons[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

#if NX_USE_SOFTBODY_API
void CustomCopy::customCopyFrom(NxSoftBodyMeshDesc &cdesc,const ::NxSoftBodyMeshDesc &desc)
{
	copyBuffer(cdesc.mVertices, desc.vertices, desc.numVertices, desc.vertexStrideBytes );

	if ( desc.flags & NX_SOFTBODY_MESH_16_BIT_INDICES )
	{
		copyBuffer16(cdesc.mTetrahedra, desc.tetrahedra,	desc.numTetrahedra, desc.tetrahedronStrideBytes);
	}
	else
	{
		copyBuffer(cdesc.mTetrahedra, desc.tetrahedra,	desc.numTetrahedra, desc.tetrahedronStrideBytes);
	}
	if (desc.vertexFlags)
	{
		copyBuffer(cdesc.mVertexFlags,desc.vertexFlags, desc.numVertices, desc.vertexFlagStrideBytes);
	}
	if (desc.vertexMasses)
	{
		copyBuffer(cdesc.mVertexMasses, desc.vertexMasses, desc.numVertices, desc.vertexMassStrideBytes);
	}
}

void CustomCopy::customCopyTo(::NxSoftBodyMeshDesc &cdesc,const NxSoftBodyMeshDesc &desc)
{
  cdesc.numVertices = desc.mVertices.size();
  cdesc.numTetrahedra = desc.mTetrahedra.size();
  cdesc.vertexStrideBytes = sizeof(NxVec3);
  cdesc.tetrahedronStrideBytes = sizeof(NxTetra);
  if ( cdesc.numVertices )
  {
    cdesc.vertices = &desc.mVertices[0].x;
  }
  if ( cdesc.numTetrahedra )
  {
    cdesc.tetrahedra = &desc.mTetrahedra[0].a;
  }

  if ( desc.mVertexFlags.size() )
  {
  	cdesc.vertexFlags = &desc.mVertexFlags[0];
  	cdesc.vertexFlagStrideBytes = sizeof(NxU32);
  }
  else
  {
  	cdesc.vertexFlags = 0;
  	cdesc.vertexFlagStrideBytes = 0;
  }
  if ( desc.mVertexMasses.size() )
  {
  	cdesc.vertexMasses = &desc.mVertexMasses[0];
  	cdesc.vertexMassStrideBytes = sizeof(float);
  }
  else
  {
  	cdesc.vertexMasses = 0;
  	cdesc.vertexMassStrideBytes = 0;
  }


}


void CustomCopy::customCopyFrom(NxSoftBodyDesc &cdesc,const ::NxSoftBodyDesc &desc)
{
  cdesc.mSoftBodyMesh   = getNameFromSoftBodyMesh( desc.softBodyMesh);
  cdesc.mCompartment = getNameFromCompartment( desc.compartment );
}

void CustomCopy::customCopyTo(::NxSoftBodyDesc &cdesc,const NxSoftBodyDesc &desc)
{
  cdesc.softBodyMesh   = getSoftBodyMeshFromName(desc.mSoftBodyMesh);
  cdesc.compartment = getCompartmentFromName(desc.mCompartment);
}

NxSoftBodyMesh * CustomCopy::getSoftBodyMeshFromName(const char *name)
{
	NxSoftBodyMesh *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && name )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mSoftBodyMeshes.size(); i++)
  	{
  		NxSoftBodyMeshDesc *sd = mCurrentCollection->mSoftBodyMeshes[i];
  		if ( strcmp(sd->mId,name) == 0 )
  		{
  			ret = (NxSoftBodyMesh *) sd->mInstance;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}

const char *   CustomCopy::getNameFromSoftBodyMesh(NxSoftBodyMesh *mesh)
{
	const char *ret = 0;

  assert(mCurrentCollection);
  if ( mCurrentCollection && mesh )
  {
  	for (unsigned int i=0; i<mCurrentCollection->mSoftBodyMeshes.size(); i++)
  	{
  		NxSoftBodyMeshDesc *sd = mCurrentCollection->mSoftBodyMeshes[i];
  		if ( sd->mInstance == mesh )
  		{
  			ret = sd->mId;
  			assert(ret);
  			break;
  		}
  	}
  }

  return ret;
}




#endif


#if NX_SDK_VERSION_NUMBER >= 270
void CustomCopy::customCopyFrom(NxForceFieldDesc &cdesc,const ::NxForceFieldDesc &desc)
{
	cdesc.mActor = getNameFromActor(desc.actor);
#if NX_SDK_VERSION_NUMBER >= 280
	// get kernel ptr
	NxForceField* f = static_cast<NxForceField*>(cdesc.mInstance);
	NxForceFieldKernel* k = f->getForceFieldKernel();
	if(k && k->getType() == NX_FFK_LINEAR_KERNEL)
	{
		NxForceFieldLinearKernel* lKernel = static_cast<NxForceFieldLinearKernel*>(k);
		cdesc.mKernel = getNameFromForceFieldLinearKernel(lKernel);
	}
	// includeGroup
	copyForceFieldShapesFrom(&f->getIncludeShapeGroup(), cdesc.mIncludeShapes);
	// shape groups
	f->resetShapeGroupsIterator();
	NxU32 nbGroups = f->getNbShapeGroups();
	while(nbGroups--)
	{
		::NxForceFieldShapeGroup* group = f->getNextShapeGroup();
		NxForceFieldGroupReference* reference = new NxForceFieldGroupReference();
		reference->mRef = getNameFromForceFieldShapeGroup(group);
		cdesc.mShapeGroups.pushBack(reference);
	}

#endif
}

void CustomCopy::customCopyTo(::NxForceFieldDesc &cdesc,const NxForceFieldDesc &desc)
{
	cdesc.actor = getActorFromName(desc.mActor );
#if NX_SDK_VERSION_NUMBER >= 280
	cdesc.kernel = getForceFieldLinearKernelFromName(desc.mKernel);
	copyForceFieldShapesTo(cdesc.includeGroupShapes, desc.mIncludeShapes, desc.mId);
	for(NxU32 i = 0; i < desc.mShapeGroups.size(); i++ )
	{
		cdesc.shapeGroups.pushBack(getForceFieldShapeGroupFromName(desc.mShapeGroups[i]->mRef));
	}
#endif
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
void CustomCopy::customCopyFrom(NxForceFieldShapeGroupDesc &cdesc,const ::NxForceFieldShapeGroupDesc &desc)
{
	NxForceFieldShapeGroup* group = static_cast<NxForceFieldShapeGroup*>(cdesc.mInstance);
	copyForceFieldShapesFrom(group, cdesc.mShapes);
}

void CustomCopy::customCopyTo(::NxForceFieldShapeGroupDesc &cdesc,const NxForceFieldShapeGroupDesc &desc)
{
	copyForceFieldShapesTo(cdesc.shapes, desc.mShapes, desc.mId);
}

void CustomCopy::copyForceFieldShapesFrom(NxForceFieldShapeGroup* group, NxArray<NxForceFieldShapeDesc*>& nxuShapes)
{
	//ok..now time to copy the shapes
	NxU32 nb = group->getNbShapes();

	group->resetShapesIterator();

	for	(NxU32 k = 0;	k	<	nb; k++)
	{
		const NxForceFieldShape *shape = group->getNextShape();

		NxForceFieldShapeDesc *save = 0;

		switch ( shape->getType() )
		{
		case NX_SHAPE_SPHERE:
			{
			const NxSphereForceFieldShape *p = shape->isSphere();
			assert(p);
			::NxSphereForceFieldShapeDesc d;
			p->saveToDesc(d);
			NxSphereForceFieldShapeDesc *sd = new NxSphereForceFieldShapeDesc;
			sd->copyFrom(d,*this);
			save = static_cast<NxForceFieldShapeDesc *>(sd);
			save->mType = SC_NxSphereForceFieldShapeDesc;
			break;
			}
		case NX_SHAPE_BOX:
			{
			const NxBoxForceFieldShape *p = shape->isBox();
			assert(p);
			::NxBoxForceFieldShapeDesc d;
			p->saveToDesc(d);
			NxBoxForceFieldShapeDesc *sd = new NxBoxForceFieldShapeDesc;
			sd->copyFrom(d,*this);
			save = static_cast<NxForceFieldShapeDesc *>(sd);
			save->mType = SC_NxBoxForceFieldShapeDesc;
			break;
			}
		case NX_SHAPE_CAPSULE:
			{
			const NxCapsuleForceFieldShape *p = shape->isCapsule();
			assert(p);
			::NxCapsuleForceFieldShapeDesc d;
			p->saveToDesc(d);
			NxCapsuleForceFieldShapeDesc *sd = new NxCapsuleForceFieldShapeDesc;
			sd->copyFrom(d,*this);
			save = static_cast<NxForceFieldShapeDesc *>(sd);
			save->mType = SC_NxCapsuleForceFieldShapeDesc;
			break;
			}
		case NX_SHAPE_CONVEX:
			{
			const NxConvexForceFieldShape *p = shape->isConvex();
			assert(p);
			::NxConvexForceFieldShapeDesc d;
			p->saveToDesc(d);

			if	(d.meshData)
			{
				NxuPhysicsExport exporter(mCurrentCollection);
				exporter.Write(d.meshData,0);
			}

			NxConvexForceFieldShapeDesc *sd = new NxConvexForceFieldShapeDesc;
			sd->copyFrom(d,*this);
			save = static_cast<NxForceFieldShapeDesc *>(sd);
			save->mType = SC_NxConvexForceFieldShapeDesc;
			break;
			}
		default:
			break;
		}
		save->mInstance = (void *)shape;
		nxuShapes.push_back(save);
	}
}

void CustomCopy::copyForceFieldShapesTo(NxArray< ::NxForceFieldShapeDesc*>& outArray, const NxArray<NxForceFieldShapeDesc*>& inArray, const char* forceFieldID)
{
	// Now for the force field shapes
	int nbShapes = inArray.size();
	for (int j = 0; j < nbShapes; j++)
	{
		bool isOk = false;
		NxForceFieldShapeDesc *fsd = inArray[j];
		::NxForceFieldShapeDesc *shapeDesc=0;

		switch (fsd->mType)
		{
		case SC_NxSphereForceFieldShapeDesc:
			{
				::NxSphereForceFieldShapeDesc *d2 = new ::NxSphereForceFieldShapeDesc;
				NxSphereForceFieldShapeDesc *s	=	static_cast<NxSphereForceFieldShapeDesc*>(fsd);
				s->copyTo(*d2,*this);
				shapeDesc = d2;
				isOk = d2->isValid();
			}
			break;
		case SC_NxBoxForceFieldShapeDesc:
			{
				::NxBoxForceFieldShapeDesc *d3 = new ::NxBoxForceFieldShapeDesc;
				NxBoxForceFieldShapeDesc *s	=	static_cast<NxBoxForceFieldShapeDesc*>(fsd);
				s->copyTo(*d3,*this);
				shapeDesc = d3;
				isOk = d3->isValid();
			}
			break;
		case SC_NxCapsuleForceFieldShapeDesc:
			{
				::NxCapsuleForceFieldShapeDesc *d4 = new ::NxCapsuleForceFieldShapeDesc;
				NxCapsuleForceFieldShapeDesc *s	=	static_cast<NxCapsuleForceFieldShapeDesc*>(fsd);
				s->copyTo(*d4,*this);
				shapeDesc = d4;
				isOk = d4->isValid();
			}
			break;
		case SC_NxConvexForceFieldShapeDesc:
			{
				::NxConvexForceFieldShapeDesc *d6 = new ::NxConvexForceFieldShapeDesc;
				NxConvexForceFieldShapeDesc *s = static_cast<NxConvexForceFieldShapeDesc*>(fsd);
				s->copyTo(*d6,*this);
				shapeDesc = d6;
				isOk = d6->isValid();
			}
			break;
		default:
			NX_ASSERT(false);	//Unknown	shape	type
		}

		shapeDesc->userData = (void *)fsd->mUserProperties;
		
		if ( shapeDesc )
		{
			if ( isOk )
			{
				outArray.pushBack(shapeDesc);
			}
			else
			{
				delete shapeDesc;
				reportWarning("Failed to construct valid force field shape descriptor type(%s) for Field (%s)", EnumToString( fsd->mType) , forceFieldID );			
			}
		}
		else
		{
			reportWarning("Unable to construct valid shape descriptor type(%s) for Actor (%s)", EnumToString( fsd->mType) ,  forceFieldID);
		}
	}

}

#endif

bool createCookedData(NxConvexMeshDesc &desc)
{
  bool ret = false;

  if ( desc.mCookedDataSize == 0 ) // if we do not already have cooked data, then create it!
  {
    ::NxConvexMeshDesc cdesc;

    cdesc.numVertices         = desc.mPoints.size();
    cdesc.numTriangles        = desc.mTriangles.size();
    cdesc.pointStrideBytes    = sizeof(NxVec3);
    cdesc.triangleStrideBytes = sizeof(NxTri);

    if ( cdesc.numVertices )
    {
      cdesc.points = &desc.mPoints[0].x;
    }

    if ( cdesc.numTriangles )
    {
      cdesc.triangles = &desc.mTriangles[0].a;
    }


  	MemoryWriteBuffer wb;
  	InitCooking();
#ifdef _DEBUG
  	bool ok = CookConvexMesh(cdesc,wb);
  	assert(ok);
#else
	CookConvexMesh(cdesc,wb);
#endif
  	CloseCooking();

  	desc.mCookedDataSize = wb.currentSize;

  	copyBuffer(desc.mCookedData, wb.data, wb.currentSize, sizeof(NxU8) );

  	copyBuffer(desc.mPoints, cdesc.points, cdesc.numVertices, cdesc.pointStrideBytes );

  	if ( cdesc.flags & NX_CF_16_BIT_INDICES )
   		copyBuffer16(desc.mTriangles, cdesc.triangles, cdesc.numTriangles, cdesc.triangleStrideBytes );
   	else
    	copyBuffer(desc.mTriangles, cdesc.triangles, cdesc.numTriangles, cdesc.triangleStrideBytes );

    ret = true;

  }

  return ret;
}

bool createCookedData(NxTriangleMeshDesc &desc)
{
  bool ret = false;

  if ( desc.mCookedDataSize == 0 ) // if we do not already have cooked data, then create it!
  {
    ::NxTriangleMeshDesc cdesc;

    cdesc.numVertices         = desc.mPoints.size();
    cdesc.numTriangles        = desc.mTriangles.size();
    cdesc.pointStrideBytes    = sizeof(NxVec3);
    cdesc.triangleStrideBytes = sizeof(NxTri);

    if ( cdesc.numVertices )
    {
      cdesc.points = &desc.mPoints[0].x;
    }

    if ( cdesc.numTriangles )
    {
      cdesc.triangles = &desc.mTriangles[0].a;
    }


  	MemoryWriteBuffer wb;
  	InitCooking();

#ifdef _DEBUG
  	bool ok = CookTriangleMesh(cdesc,wb);
  	assert(ok);
#else
	CookTriangleMesh(cdesc,wb);
#endif

  	CloseCooking();

  	desc.mCookedDataSize = wb.currentSize;

  	copyBuffer(desc.mCookedData, wb.data, wb.currentSize, sizeof(NxU8) );

  	copyBuffer(desc.mPoints, cdesc.points, cdesc.numVertices, cdesc.pointStrideBytes );

  	if ( cdesc.flags & NX_CF_16_BIT_INDICES )
   		copyBuffer16(desc.mTriangles, cdesc.triangles, cdesc.numTriangles, cdesc.triangleStrideBytes );
   	else
    	copyBuffer(desc.mTriangles, cdesc.triangles, cdesc.numTriangles, cdesc.triangleStrideBytes );


    ret = true;

  }


  return ret;
}


bool scalePoints(NxConvexMeshDesc &desc,const NxVec3 &scale)     // scale the input points.
{
  bool ret = false;

  if ( !desc.mPoints.empty() )
  {
    ret = true;
    NxU32 count = desc.mPoints.size();
    NxVec3 *points = &desc.mPoints[0];
    for (NxU32 i=0; i<count; i++)
    {
      points->x*=scale.x;
      points->y*=scale.y;
      points->z*=scale.z;
      points++;
    }
  }



  return ret;
}

bool scalePoints(NxTriangleMeshDesc &desc,const NxVec3 &scale)   // scale the input points.
{
  bool ret = false;

  if ( !desc.mPoints.empty() )
  {
    ret = true;
    NxU32 count = desc.mPoints.size();
    NxVec3 *points = &desc.mPoints[0];
    for (NxU32 i=0; i<count; i++)
    {
      points->x*=scale.x;
      points->y*=scale.y;
      points->z*=scale.z;
      points++;
    }
  }
  return ret;
}

bool clearNonCooked(NxConvexMeshDesc &desc)    // remove the non-cooked data since we only want the cooked data remaining.
{
  bool ret = false;

  if (desc.mCookedDataSize )
  {
    ret = true;
    desc.mPoints.clear();
    desc.mTriangles.clear();
  }

  return ret;
}

bool clearNonCooked(NxTriangleMeshDesc &desc)  // remove the non-cooked data since we only want the cooked data remaining.
{
  bool ret = false;

  if ( desc.mCookedDataSize )
  {
    ret = true;
    desc.mPoints.clear();
    desc.mTriangles.clear();
    desc.mMaterialIndices.clear();
  }

  return ret;
}

};

