#ifndef SOFT_MESH_OBJ_H

#define SOFT_MESH_OBJ_H
#include "NxPhysics.h"

namespace SOFTBODY
{

class SoftMeshInterface;

bool loadSoftMeshObj(const char *oname,const char *tname,SoftMeshInterface *smi);
//NxArray<NxU32> loadSoftMeshObjRet(const char *oname,const char *tname,SoftMeshInterface *smi);

}; // END OF SOFTBODY NAMESPACE

#endif
