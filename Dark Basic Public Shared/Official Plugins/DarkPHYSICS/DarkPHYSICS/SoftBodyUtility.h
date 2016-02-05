
#include "NxPhysics.h"

NxActor* CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density);
NxActor* CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density);
NxActor* CreateCompoundCapsule(const NxVec3& pos0, const NxReal height0, const NxReal radius0, const NxReal height1, const NxReal radius1);
NxSphericalJoint* CreateRopeSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density);