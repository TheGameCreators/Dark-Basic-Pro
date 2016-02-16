#include "NXU_helper.h"

#include "NXU_PhysicsExport.h"
#include "NXU_schema.h"
#include "NXU_PhysicsInstantiator.h"
#include "NXU_string.h"
#include "NXU_File.h"
#include "NXU_SchemaStream.h"
#include "NXU_ColladaExport.h"
#include "NXU_ColladaImport.h"
#include "NXU_Geometry.h"
#include "NXU_customcopy.h"

#include <NxVersionNumber.h>
#include <NxPhysics.h>
#include <NxPMap.h>
#include <NxSceneStats.h>
#include <NxStream.h>

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

#if	NX_SDK_VERSION_NUMBER	>= 240
#include <NxCCDSkeleton.h>
#endif

#include "NXU_ScaledCopy.h"

namespace NXU
{

NxJointDesc * copyJoint(NxJointDesc *v)
{
  NxJointDesc *jdest = 0;

  switch ( v->mType )
  {
    case SC_NxD6JointDesc:
      if ( 1 )
      {
        NxD6JointDesc *p = static_cast<NxD6JointDesc *>(v);
        NxD6JointDesc *dest = new NxD6JointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxCylindricalJointDesc:
      if ( 1 )
      {
        NxCylindricalJointDesc *p = static_cast<NxCylindricalJointDesc *>(v);
        NxCylindricalJointDesc *dest = new NxCylindricalJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxDistanceJointDesc:
      if ( 1 )
      {
        NxDistanceJointDesc *p = static_cast<NxDistanceJointDesc *>(v);
        NxDistanceJointDesc *dest = new NxDistanceJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxFixedJointDesc:
      if ( 1 )
      {
        NxFixedJointDesc *p = static_cast<NxFixedJointDesc *>(v);
        NxFixedJointDesc *dest = new NxFixedJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxPointInPlaneJointDesc:
      if ( 1 )
      {
        NxPointInPlaneJointDesc *p = static_cast<NxPointInPlaneJointDesc *>(v);
        NxPointInPlaneJointDesc *dest = new NxPointInPlaneJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxPointOnLineJointDesc:
      if ( 1 )
      {
        NxPointOnLineJointDesc *p = static_cast<NxPointOnLineJointDesc *>(v);
        NxPointOnLineJointDesc *dest = new NxPointOnLineJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxPrismaticJointDesc:
      if ( 1 )
      {
        NxPrismaticJointDesc *p = static_cast<NxPrismaticJointDesc *>(v);
        NxPrismaticJointDesc *dest = new NxPrismaticJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxRevoluteJointDesc:
      if ( 1 )
      {
        NxRevoluteJointDesc *p = static_cast<NxRevoluteJointDesc *>(v);
        NxRevoluteJointDesc *dest = new NxRevoluteJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxSphericalJointDesc:
      if ( 1 )
      {
        NxSphericalJointDesc *p = static_cast<NxSphericalJointDesc *>(v);
        NxSphericalJointDesc *dest = new NxSphericalJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    case SC_NxPulleyJointDesc:
      if ( 1 )
      {
        NxPulleyJointDesc *p = static_cast<NxPulleyJointDesc *>(v);
        NxPulleyJointDesc *dest = new NxPulleyJointDesc;
        *dest = *p;
        jdest = dest;
      }
      break;
    default:
       break;
  }

  if ( jdest )
  {
    jdest->mInstance = 0;
  }

  return jdest;
}

void copyActor(NxActorDesc *adest,const NxActorDesc *source,const NxVec3 &scale)
{
  *adest = *source;

  adest->mInstance = 0;

  adest->globalPose.t.x*=scale.x;
  adest->globalPose.t.y*=scale.y;
  adest->globalPose.t.z*=scale.z;

  for (NxU32 i=0; i<source->mShapes.size(); i++)
  {
    NxShapeDesc *v = source->mShapes[i];
    switch ( v->mType )
    {
      case SC_NxPlaneShapeDesc:
        if ( 1 )
        {
          NxPlaneShapeDesc *p = static_cast<NxPlaneShapeDesc *>(v);
          NxPlaneShapeDesc *dest = new NxPlaneShapeDesc;
          *dest = *p;
          adest->mShapes[i] = dest; // overwrite the old pointer with the newly allocated shape pointer.
        }
        break;
      case SC_NxSphereShapeDesc:
        if ( 1 )
        {
          NxSphereShapeDesc *p = static_cast<NxSphereShapeDesc *>(v);
          NxSphereShapeDesc *dest = new NxSphereShapeDesc;
          *dest = *p;
          dest->radius*=scale.x;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxBoxShapeDesc:
        if ( 1 )
        {
          NxBoxShapeDesc *p = static_cast<NxBoxShapeDesc *>(v);
          NxBoxShapeDesc *dest = new NxBoxShapeDesc;
          *dest = *p;
          dest->dimensions.x*=scale.x;
          dest->dimensions.y*=scale.y;
          dest->dimensions.z*=scale.z;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxCapsuleShapeDesc:
        if ( 1 )
        {
          NxCapsuleShapeDesc *p = static_cast<NxCapsuleShapeDesc *>(v);
          NxCapsuleShapeDesc *dest = new NxCapsuleShapeDesc;
          *dest = *p;
          dest->radius*=scale.x;
          dest->height*=scale.y;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxWheelShapeDesc:
        if ( 1 )
        {
          NxWheelShapeDesc *p = static_cast<NxWheelShapeDesc *>(v);
          NxWheelShapeDesc *dest = new NxWheelShapeDesc;
          *dest = *p;
          dest->mInstance = 0;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxConvexShapeDesc:
        if ( 1 )
        {
          NxConvexShapeDesc *p = static_cast<NxConvexShapeDesc *>(v);
          NxConvexShapeDesc *dest = new NxConvexShapeDesc;
          *dest = *p;
          dest->mInstance = 0;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxTriangleMeshShapeDesc:
        if ( 1 )
        {
          NxTriangleMeshShapeDesc *p = static_cast<NxTriangleMeshShapeDesc *>(v);
          NxTriangleMeshShapeDesc *dest = new NxTriangleMeshShapeDesc;
          *dest = *p;
          dest->mInstance = 0;
          adest->mShapes[i] = dest;
        }
        break;
      case SC_NxHeightFieldShapeDesc:
        if ( 1 )
        {
          NxHeightFieldShapeDesc *p = static_cast<NxHeightFieldShapeDesc *>(v);
          NxHeightFieldShapeDesc *dest = new NxHeightFieldShapeDesc;
          *dest = *p;
          dest->mInstance = 0;
          adest->mShapes[i] = dest;
        }
        break;
      default:
	break;
    }
  }
}

void copyScaledScene(NxSceneDesc *dest,const NxSceneDesc *source,const NxVec3 &scale)
{
  dest->mId = source->mId;
  dest->mUserProperties = source->mUserProperties;
  dest->mHasMaxBounds = source->mHasMaxBounds;
  dest->mHasLimits    = source->mHasLimits;
  dest->mHasFilter    = source->mHasFilter;
  dest->mFilterBool   = source->mFilterBool;


  dest->mFilterOp0                      =   source->mFilterOp0;
  dest->mFilterOp1                      = source->mFilterOp1;
  dest->mFilterOp2                      = source->mFilterOp2;
  dest->mGroupMask0                     = source->mGroupMask0;
  dest->mGroupMask1                     = source->mGroupMask1;
  dest->gravity                         = source->gravity;
  dest->maxTimestep                     = source->maxTimestep;
  dest->maxIter                         = source->maxIter;
  dest->timeStepMethod                  = source->timeStepMethod;
  dest->mMaxBounds                      = source->mMaxBounds;
  dest->mLimits                         = source->mLimits;
  dest->simType                         = source->simType;
  dest->groundPlane                     = source->groundPlane;
  dest->boundsPlanes                    = source->boundsPlanes;
  dest->flags                           = source->flags;
  dest->internalThreadCount             = source->internalThreadCount;
  dest->backgroundThreadCount           = source->backgroundThreadCount;
  dest->threadMask                      = source->threadMask;
  dest->backgroundThreadMask            = source->backgroundThreadMask;
#if NX_SDK_VERSION_NUMBER >= 250
  dest->simThreadStackSize              = source->simThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 260
  dest->simThreadPriority               = source->simThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  dest->simThreadMask					= source->simThreadMask;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  dest->workerThreadStackSize           = source->workerThreadStackSize;
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  dest->workerThreadPriority            = source->workerThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  dest->backgroundThreadPriority		= source->backgroundThreadPriority;
#endif
#if NX_SDK_VERSION_NUMBER >= 261
  dest->upAxis                          = source->upAxis;
#endif
#if NX_SDK_VERSION_NUMBER >= 261
  dest->subdivisionLevel                = source->subdivisionLevel;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  dest->staticStructure                 = source->staticStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  dest->dynamicStructure                = source->dynamicStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  dest->dynamicTreeRebuildRateHint		= source->dynamicTreeRebuildRateHint;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dest->bpType							= source->bpType;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dest->nbGridCellsX					= source->nbGridCellsX;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dest->nbGridCellsY					= source->nbGridCellsY;
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  dest->solverBatchSize					= source->solverBatchSize;
#endif



//**************************************************************
//  NxArray< NxMaterialDesc * >                    mMaterials;
//**************************************************************
  for (unsigned int i=0; i<source->mMaterials.size(); ++i)
  {
    NxMaterialDesc *desc = source->mMaterials[i];
    NxMaterialDesc *d    = new NxMaterialDesc;
    *d = *desc;
    d->mInstanceIndex = 0;
    dest->mMaterials.push_back(d);
  }


//**************************************************************
//  NxArray< NxActorDesc *>                        mActors;
//**************************************************************
  for (unsigned int i=0; i<source->mActors.size(); ++i)
  {
    NxActorDesc *desc = source->mActors[i];
    NxActorDesc *d    = new NxActorDesc;
    copyActor(d,desc,scale);
    dest->mActors.push_back(d);
  }


//**************************************************************
//  NxArray< NxJointDesc *>                        mJoints;
//**************************************************************
  for (unsigned int i=0; i<source->mJoints.size(); ++i)
  {
    NxJointDesc *desc = source->mJoints[i];
    NxJointDesc *d = copyJoint(desc);
    dest->mJoints.push_back(d);
  }



//**************************************************************
//  NxArray< NxPairFlagDesc *>                     mPairFlags;
//**************************************************************
  for (unsigned int i=0; i<source->mPairFlags.size(); ++i)
  {
    NxPairFlagDesc *desc = source->mPairFlags[i];
    NxPairFlagDesc *d    = new NxPairFlagDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mPairFlags.push_back(d);
  }


//**************************************************************
//  NxArray< NxSpringAndDamperEffectorDesc *>      mEffectors;
//**************************************************************
  for (unsigned int i=0; i<source->mEffectors.size(); ++i)
  {
    NxSpringAndDamperEffectorDesc *desc = source->mEffectors[i];
    NxSpringAndDamperEffectorDesc *d    = new NxSpringAndDamperEffectorDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mEffectors.push_back(d);
  }




#if NX_SDK_VERSION_NUMBER >= 270
//**************************************************************
//  NxArray< NxForceFieldDesc *>                   mForceFields;
//**************************************************************
  for (unsigned int i=0; i<source->mForceFields.size(); ++i)
  {
    NxForceFieldDesc *desc = source->mForceFields[i];
    NxForceFieldDesc *d    = new NxForceFieldDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mForceFields.push_back(d);
  }
#endif

//**************************************************************
//  NxArray< NxCollisionGroupDesc *>               mCollisionGroups;
//**************************************************************
  for (unsigned int i=0; i<source->mCollisionGroups.size(); ++i)
  {
    NxCollisionGroupDesc *desc = source->mCollisionGroups[i];
    NxCollisionGroupDesc *d    = new NxCollisionGroupDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mCollisionGroups.push_back(d);
  }



//**************************************************************
//  NxArray< NxActorGroupPair *>                   mActorGroups;
//**************************************************************
  for (unsigned int i=0; i<source->mActorGroups.size(); ++i)
  {
    NxActorGroupPair *desc = source->mActorGroups[i];
    NxActorGroupPair *d    = new NxActorGroupPair;
    *d = *desc;
    d->mInstance = 0;
    dest->mActorGroups.push_back(d);
  }


#if NX_SDK_VERSION_NUMBER >= 260
//**************************************************************
//  NxArray< NxCompartmentDesc *>                  mCompartments;
//**************************************************************
  for (unsigned int i=0; i<source->mCompartments.size(); ++i)
  {
    NxCompartmentDesc *desc = source->mCompartments[i];
    NxCompartmentDesc *d    = new NxCompartmentDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mCompartments.push_back(d);
  }

#endif


#if NX_USE_FLUID_API
//**************************************************************
//  NxArray< NxFluidDesc *>                        mFluids;
//**************************************************************
  for (unsigned int i=0; i<source->mFluids.size(); ++i)
  {
    NxFluidDesc *desc = source->mFluids[i];
    NxFluidDesc *d    = new NxFluidDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mFluids.push_back(d);
  }
#endif


//**************************************************************
//  NxArray< NxClothDesc *>                        mCloths;
//**************************************************************
  for (unsigned int i=0; i<source->mCloths.size(); ++i)
  {
    NxClothDesc *desc = source->mCloths[i];
    NxClothDesc *d    = new NxClothDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mCloths.push_back(d);
  }


#if NX_USE_SOFTBODY_API
//**************************************************************
//  NxArray< NxSoftBodyDesc *>                     mSoftBodies;
//**************************************************************
  for (unsigned int i=0; i<source->mSoftBodies.size(); ++i)
  {
    NxSoftBodyDesc *desc = source->mSoftBodies[i];
    NxSoftBodyDesc *d    = new NxSoftBodyDesc;
    *d = *desc;
    d->mInstance = 0;
    dest->mSoftBodies.push_back(d);
  }
#endif

  dest->mInstance = 0;



}



void copyTriangleMeshScaled(NxTriangleMeshDesc *dest,NxTriangleMeshDesc *source,const NxVec3 &scale,NxPhysicsSDK *sdk,const char *sourceId,const char *destId)
{
  if ( source->mCookedData.empty() )
  {
    dest->mId             = source->mId;
    dest->mUserProperties = source->mUserProperties;
    dest->convexEdgeThreshold = source->convexEdgeThreshold;
    dest->flags = source->flags;
    dest->heightFieldVerticalAxis = source->heightFieldVerticalAxis;
    dest->heightFieldVerticalExtent = source->heightFieldVerticalExtent;
    dest->mMaterialIndices = source->mMaterialIndices;
    dest->mPmapData = 0;
    dest->mPmapDensity = 0; // scaled pmaps are not supported.
    dest->mPmapSize = 0;
    dest->mInstance = 0;

    for (unsigned int i=0; i<source->mPoints.size(); i++)
    {
      NxVec3 v = source->mPoints[i];
      v.x*=scale.x;
      v.y*=scale.y;
      v.z*=scale.z;
      dest->mPoints.push_back( v );
    }

    dest->mTriangles = source->mTriangles;
    dest->flags      = source->flags;
    dest->mCookedDataSize = 0;
  }
  else
  {
    if ( sdk )
    {
      if ( source->mInstance == 0 )
      {
        NxuPhysicsInstantiator ip;
        CustomCopy cc(0,0);
        source->mInstance = ip.instantiateTriangleMesh(*sdk,*source,sourceId,0,cc);
      }

      if ( source->mInstance ) // now we have to export just this one item..
      {
        NxuPhysicsExport pe;

        bool saveCooked = gCookOnExport;
        gCookOnExport = false;

        pe.exportTriangleMesh((NxTriangleMesh *)source->mInstance,source->mUserProperties,source->mId,*dest);

        scalePoints(*dest,scale);
        createCookedData(*dest);
        clearNonCooked(*dest);

        gCookOnExport = saveCooked;
      }
    }
    else
    {
      assert(0); // to be done...
    }
  }

}


void copyConvexMeshScaled(NxConvexMeshDesc *dest,NxConvexMeshDesc *source,const NxVec3 &scale,NxPhysicsSDK *sdk,const char *sourceId,const char *destId)
{
  if ( source->mCookedData.empty() )
  {
    dest->mId             = source->mId;
    dest->mUserProperties = source->mUserProperties;

    for (unsigned int i=0; i<source->mPoints.size(); i++)
    {
      NxVec3 v = source->mPoints[i];
      v.x*=scale.x;
      v.y*=scale.y;
      v.z*=scale.z;
      dest->mPoints.push_back( v );
    }

    dest->mTriangles = source->mTriangles;
    dest->flags      = source->flags;
    dest->mCookedDataSize = 0;
  }
  else
  {
    if ( sdk )
    {
      if ( source->mInstance == 0 )
      {
        NxuPhysicsInstantiator ip;
        CustomCopy cc(0,0);
        source->mInstance = ip.instantiateConvexMesh(*sdk,*source,sourceId,0,cc);
      }

      if ( source->mInstance ) // now we have to export just this one item..
      {
        NxuPhysicsExport pe;

        bool saveCooked = gCookOnExport;
        gCookOnExport = false;

        pe.exportConvexMesh((NxConvexMesh *)source->mInstance,source->mUserProperties,source->mId,*dest);

        scalePoints(*dest,scale);
        createCookedData(*dest);
        clearNonCooked(*dest);

        gCookOnExport = saveCooked;
      }
    }
    else
    {
      assert(0); // to be done...
    }
  }
}


NxuPhysicsCollection * scaleCopyCollection(const NxuPhysicsCollection *source,const char *newId,const NxVec3 &scale,NxPhysicsSDK *sdk)
{
  NxuPhysicsCollection *ret = 0;

  NxuPhysicsCollection *c = new NxuPhysicsCollection;


  c->mId             = getGlobalString(newId);
  c->mUserProperties = source->mUserProperties;
  c->mSdkVersion     = source->mSdkVersion;
  c->mNxuVersion     = source->mNxuVersion;

//****************************************************************
//** Copy the parameters.  No scaling involved.
//  NxArray< NxParameterDesc *>                    mParameters;
//****************************************************************
  for (unsigned int i=0; i<source->mParameters.size(); ++i)
  {
    NxParameterDesc *desc = source->mParameters[i];
    NxParameterDesc *d = new NxParameterDesc;
    *d = *desc;
    c->mParameters.push_back(d);
  }


//****************************************************************
// *** Copy and scale the convex meshes.
//  NxArray< NxConvexMeshDesc *>                   mConvexMeshes;
//****************************************************************
  for (unsigned int i=0; i<source->mConvexMeshes.size(); ++i)
  {
    NxConvexMeshDesc *desc = source->mConvexMeshes[i];
    NxConvexMeshDesc *d = new NxConvexMeshDesc;
    copyConvexMeshScaled(d,desc,scale,sdk,source->mId,c->mId);
    c->mConvexMeshes.push_back(d);
  }


//****************************************************************
//** Copy and scale the triangle meshes
//  NxArray< NxTriangleMeshDesc *>                 mTriangleMeshes;
//****************************************************************
  for (unsigned int i=0; i<source->mTriangleMeshes.size(); ++i)
  {
    NxTriangleMeshDesc *desc = source->mTriangleMeshes[i];
    NxTriangleMeshDesc *d = new NxTriangleMeshDesc;
    copyTriangleMeshScaled(d,desc,scale,sdk,source->mId,c->mId);
    c->mTriangleMeshes.push_back(d);
  }

//****************************************************************
// Copy the heightfields, heightfields cannot be scaled.
//  NxArray< NxHeightFieldDesc *>                  mHeightFields;
//****************************************************************
  for (unsigned int i=0; i<source->mHeightFields.size(); ++i)
  {
    NxHeightFieldDesc *desc = source->mHeightFields[i];
    NxHeightFieldDesc *d = new NxHeightFieldDesc;
    *d = *desc;
    d->mInstance = 0;
    c->mHeightFields.push_back(d);
  }

//****************************************************************
// Copy and scaled the CCD skeletons
//  NxArray< NxCCDSkeletonDesc *>                  mSkeletons;
//****************************************************************
  for (unsigned int i=0; i<source->mSkeletons.size(); ++i)
  {
    NxCCDSkeletonDesc *desc = source->mSkeletons[i];
    NxCCDSkeletonDesc *d = new NxCCDSkeletonDesc;
    *d = *desc;
    d->mInstance = 0;
    c->mSkeletons.push_back(d);
  }



//****************************************************************
// Copy and scale the cloth mesh
//  NxArray< NxClothMeshDesc *>                    mClothMeshes;
//****************************************************************
  for (unsigned int i=0; i<source->mClothMeshes.size(); ++i)
  {
    NxClothMeshDesc *desc = source->mClothMeshes[i];
    NxClothMeshDesc *d = new NxClothMeshDesc;
    *d = *desc;
    d->mInstance = 0;
    c->mClothMeshes.push_back(d);
  }



#if NX_USE_SOFTBODY_API

//****************************************************************
// Copy and scale the soft body meshes
//  NxArray< NxSoftBodyMeshDesc *>                 mSoftBodyMeshes;
//****************************************************************
  for (unsigned int i=0; i<source->mSoftBodyMeshes.size(); ++i)
  {
    NxSoftBodyMeshDesc *desc = source->mSoftBodyMeshes[i];
    NxSoftBodyMeshDesc *d = new NxSoftBodyMeshDesc;
    *d = *desc;
    d->mInstance = 0;
    c->mSoftBodyMeshes.push_back(d);
  }


#endif


//****************************************************************
//  NxArray< NxSceneDesc *>                        mScenes;
//****************************************************************
  for (unsigned int i=0; i<source->mScenes.size(); ++i)
  {
    NxSceneDesc *desc = source->mScenes[i];
    NxSceneDesc *d = new NxSceneDesc;

    copyScaledScene(d,desc,scale);

    c->mScenes.push_back(d);
  }




//****************************************************************
//  NxArray< NxSceneInstanceDesc *>                mSceneInstances;
//****************************************************************
  for (unsigned int i=0; i<source->mSceneInstances.size(); ++i)
  {
    NxSceneInstanceDesc *desc = source->mSceneInstances[i];
    NxSceneInstanceDesc *d = new NxSceneInstanceDesc;
    *d = *desc;
    d->mInstance = 0;
    c->mSceneInstances.push_back(d);
  }


  ret = c;

  return ret;
}

};
