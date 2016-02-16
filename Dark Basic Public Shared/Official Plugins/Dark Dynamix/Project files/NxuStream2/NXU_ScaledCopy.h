#ifndef NXU_SCALED_COPY_H

#define NXU_SCALED_COPY_H

class NxVec3;

namespace NXU
{

class NxuPhysicsCollection;

// This is a helper function that will scale the assets in a physics collection to a new copy.  New id is *required* and must be unique.
NxuPhysicsCollection * scaleCopyCollection(const NxuPhysicsCollection *source,const char *newId,const NxVec3 &scale,NxPhysicsSDK *sdk);

}

#endif
