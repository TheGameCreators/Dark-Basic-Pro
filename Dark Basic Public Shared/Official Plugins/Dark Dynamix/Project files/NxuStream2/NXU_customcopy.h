#ifndef CUSTOM_COPY_H

#define CUSTOM_COPY_H

#include "NxSimpleTypes.h"
#include "NxArray.h"
#include "NxVec3.h"
#include "NxQuat.h"
#include "NxMat33.h"
#include "NxMat34.h"
#include "NxBounds3.h"
#include <NxVersionNumber.h>
#include <NxPhysics.h>
#include <NxPMap.h>
#include <NxSceneStats.h>
#include <NxStream.h>

namespace NXU
{

class NxConvexMeshDesc;
class NxTriangleMeshDesc;
class NxMaterialDesc;
class NxShapeDesc;
class NxActorDesc;
class NxJointDesc;
class NxFluidEmitterDesc;
class NxSceneDesc;
class NxClothDesc;
class NxFluidDesc;
class NxSpringAndDamperEffectorDesc;
class NxForceFieldDesc;
class NxForceFieldShapeDesc;
class NxForceFieldShapeGroupDesc;
class NxConvexForceFieldShapeDesc;
class NxuPhysicsCollection;
class NxSceneInstanceDesc;
class NxSimpleTriangleMesh;
class NxConvexShapeDesc;
class NxHeightFieldDesc;
class NxClothMeshDesc;
class NxTriangleMeshShapeDesc;
class NxHeightFieldShapeDesc;
class NxParticleData;
class NxSoftBodyMeshDesc;
class NxSoftBodyDesc;

class CustomCopy
{
public:
  CustomCopy(NxuPhysicsCollection *c=0,NxSceneDesc *s=0)
  {
  	mCurrentCollection = c;
  	mCurrentScene      = s;
  }

#if NX_USE_SOFTBODY_API
  void customCopyFrom(NxSoftBodyMeshDesc &cdesc,const ::NxSoftBodyMeshDesc &desc);
  void customCopyTo(::NxSoftBodyMeshDesc &cdesc,const NxSoftBodyMeshDesc &desc);
  void customCopyFrom(NxSoftBodyDesc &cdesc,const ::NxSoftBodyDesc &desc);
  void customCopyTo(::NxSoftBodyDesc &cdesc,const NxSoftBodyDesc &desc);
#endif

  void customCopyFrom(NxConvexMeshDesc &cdesc,const ::NxConvexMeshDesc &desc);
  void customCopyTo(::NxConvexMeshDesc &cdesc,const NxConvexMeshDesc &desc);

  void customCopyFrom(NxSimpleTriangleMesh &cdesc,const ::NxSimpleTriangleMesh &desc);
  void customCopyTo(::NxSimpleTriangleMesh &cdesc,const NxSimpleTriangleMesh &desc);

  void customCopyFrom(NxTriangleMeshDesc &cdesc,const ::NxTriangleMeshDesc &desc);
  void customCopyTo(::NxTriangleMeshDesc &cdesc,const NxTriangleMeshDesc &desc);

  void customCopyFrom(NxHeightFieldDesc &cdesc,const ::NxHeightFieldDesc &desc);
  void customCopyTo(::NxHeightFieldDesc &cdesc,const NxHeightFieldDesc &desc);

  void customCopyFrom(NxClothMeshDesc &cdesc,const ::NxClothMeshDesc &desc);
  void customCopyTo(::NxClothMeshDesc &cdesc,const NxClothMeshDesc &desc);

  void customCopyFrom(NxMaterialDesc &cdesc,const ::NxMaterialDesc &desc);
  void customCopyTo(::NxMaterialDesc &cdesc,const NxMaterialDesc &desc);

  void customCopyFrom(NxShapeDesc &cdesc,const ::NxShapeDesc &desc);
  void customCopyTo(::NxShapeDesc &cdesc,const NxShapeDesc &desc);

  void customCopyFrom(NxConvexShapeDesc &cdesc,const ::NxConvexShapeDesc &desc);
  void customCopyTo(::NxConvexShapeDesc &cdesc,const NxConvexShapeDesc &desc);

  void customCopyFrom(NxTriangleMeshShapeDesc &cdesc,const ::NxTriangleMeshShapeDesc &desc);
  void customCopyTo(::NxTriangleMeshShapeDesc &cdesc,const NxTriangleMeshShapeDesc &desc);

  void customCopyFrom(NxHeightFieldShapeDesc &cdesc,const ::NxHeightFieldShapeDesc &desc);
  void customCopyTo(::NxHeightFieldShapeDesc &cdesc,const NxHeightFieldShapeDesc &desc);

  void customCopyFrom(NxActorDesc &cdesc,const ::NxActorDesc &desc);
  void customCopyTo(::NxActorDesc &cdesc,const NxActorDesc &desc);

  void customCopyFrom(NxJointDesc &cdesc,const ::NxJointDesc &desc);
  void customCopyTo(::NxJointDesc &cdesc,const NxJointDesc &desc);

#if NX_USE_FLUID_API
  void customCopyFrom(NxParticleData &cdesc,const ::NxParticleData &desc);
  void customCopyTo(::NxParticleData &cdesc,const NxParticleData &desc);

  void customCopyFrom(NxFluidEmitterDesc &cdesc,const ::NxFluidEmitterDesc &desc);
  void customCopyTo(::NxFluidEmitterDesc &cdesc,const NxFluidEmitterDesc &desc);

  void customCopyFrom(NxFluidDesc &cdesc,const ::NxFluidDesc &desc);
  void customCopyTo(::NxFluidDesc &cdesc,const NxFluidDesc &desc);
#endif

  void customCopyFrom(NxClothDesc &cdesc,const ::NxClothDesc &desc);
  void customCopyTo(::NxClothDesc &cdesc,const NxClothDesc &desc);

  void customCopyFrom(NxSpringAndDamperEffectorDesc &cdesc,const ::NxSpringAndDamperEffectorDesc &desc);
  void customCopyTo(::NxSpringAndDamperEffectorDesc &cdesc,const NxSpringAndDamperEffectorDesc &desc);

#if NX_SDK_VERSION_NUMBER >= 270
  void customCopyFrom(NxForceFieldDesc &cdesc,const ::NxForceFieldDesc &desc);
  void customCopyTo(::NxForceFieldDesc &cdesc,const NxForceFieldDesc &desc);
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  void customCopyFrom(NxForceFieldShapeDesc &cdesc,const ::NxForceFieldShapeDesc &desc);
  void customCopyTo(::NxForceFieldShapeDesc &cdesc,const NxForceFieldShapeDesc &desc);
  void customCopyFrom(NxConvexForceFieldShapeDesc &cdesc,const ::NxConvexForceFieldShapeDesc &desc);
  void customCopyTo(::NxConvexForceFieldShapeDesc &cdesc,const NxConvexForceFieldShapeDesc &desc);
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  void customCopyFrom(NxForceFieldShapeGroupDesc &cdesc,const ::NxForceFieldShapeGroupDesc &desc);
  void customCopyTo(::NxForceFieldShapeGroupDesc &cdesc,const NxForceFieldShapeGroupDesc &desc);
  void copyForceFieldShapesFrom(NxForceFieldShapeGroup* group, NxArray<NxForceFieldShapeDesc*>& nxuShapes);
  void copyForceFieldShapesTo(NxArray< ::NxForceFieldShapeDesc*>& outArray, const NxArray<NxForceFieldShapeDesc*>& inArray, const char* forceFieldID);
#endif

  void customCopyFrom(NxSceneDesc &cdesc,const ::NxSceneDesc &desc);
  void customCopyTo(::NxSceneDesc &cdesc,const NxSceneDesc &desc);


  NxU16        getRemapMaterialIndex(NxU16 original);
  NxActor    * getActorFromName(const char *name);
  const char * getNameFromActor(NxActor *actor);

  NxConvexMesh * getConvexMeshFromName(const char *name);
  const char *   getNameFromConvexMesh(NxConvexMesh *mesh);
  NxConvexMeshDesc * getConvexMeshDescFromName(const char *name);

  NxTriangleMesh * getTriangleMeshFromName(const char *name);
  const char *   getNameFromTriangleMesh(NxTriangleMesh *mesh);

  NxHeightField * getHeightFieldFromName(const char *name);
  const char *   getNameFromHeightField(NxHeightField *mesh);

  NxClothMesh * getClothMeshFromName(const char *name);
  const char *   getNameFromClothMesh(NxClothMesh *mesh);

#if NX_USE_SOFTBODY_API
  NxSoftBodyMesh * getSoftBodyMeshFromName(const char *name);
  const char *   getNameFromSoftBodyMesh(NxSoftBodyMesh *mesh);
#endif

#if NX_SDK_VERSION_NUMBER >= 260
  NxCompartment * getCompartmentFromName(const char *name);
  const char *    getNameFromCompartment(NxCompartment *nc);
#endif

  NxCCDSkeleton * getSkeletonFromName(const char *name);
  const char *    getNameFromSkeleton(NxCCDSkeleton *s);

#if NX_SDK_VERSION_NUMBER >= 280
  NxForceField * getForceFieldFromName(const char *name);
  const char *   getNameFromForceField(NxForceField *field);

  NxForceFieldShapeGroup * getForceFieldShapeGroupFromName(const char *name);
  const char *   getNameFromForceFieldShapeGroup(NxForceFieldShapeGroup *group);

  NxForceFieldLinearKernel * getForceFieldLinearKernelFromName(const char *name);
  const char *   getNameFromForceFieldLinearKernel(NxForceFieldLinearKernel *kernel);

#endif

  static NxSceneDesc *         locateSceneDesc(NxuPhysicsCollection *pc,const char *id,NxU32 &index);
  static NxSceneInstanceDesc * locateSceneInstanceDesc(NxuPhysicsCollection *pc,const char *id,NxU32 &index);

  void initCollection(void);
  void initScene(void);


private:
  NxuPhysicsCollection	*mCurrentCollection;
  NxSceneDesc           *mCurrentScene;


};


bool scalePoints(NxConvexMeshDesc &desc,const NxVec3 &scale);     // scale the input points.
bool scalePoints(NxTriangleMeshDesc &desc,const NxVec3 &scale);   // scale the input points.

bool createCookedData(NxConvexMeshDesc &desc);     // create cooked data from the points and triangles.
bool createCookedData(NxTriangleMeshDesc &desc);   // create cooked data from the points and triangles

bool clearNonCooked(NxConvexMeshDesc &desc);    // remove the non-cooked data since we only want the cooked data remaining.
bool clearNonCooked(NxTriangleMeshDesc &desc);  // remove the non-cooked data since we only want the cooked data remaining.

extern bool gSaveDefaults;
extern bool gSaveCooked;    // save cooked data on export.
extern bool gCookOnExport;  // true if cooking data should be generated at export time.

}

#endif
