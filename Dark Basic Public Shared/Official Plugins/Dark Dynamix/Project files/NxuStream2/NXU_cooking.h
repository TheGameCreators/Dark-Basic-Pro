#ifndef	NXU_COOKING_H

#define	NXU_COOKING_H

#include "NxCooking.h"

class	NxPMap;
class	NxTriangleMesh;
class	NxUserOutputStream;
struct NxCookingParams;

namespace	NXU
{

bool hasCookingLibrary(void);	// check to	see	if the cooking library is	available	or not!

bool  SetCookingParams(const ::NxCookingParams &params);
const ::NxCookingParams & GetCookingParams(void);
bool  InitCooking(void);
void  CloseCooking();
bool  CookTriangleMesh(const	::NxTriangleMeshDesc &desc,	NxStream &stream);
bool  CookConvexMesh(const	::NxConvexMeshDesc &desc,	NxStream &stream);

#if NX_USE_CLOTH_API
bool  CookClothMesh(const ::NxClothMeshDesc &desc,	NxStream &stream);
#endif

#if NX_USE_SOFTBODY_API
bool  CookSoftBodyMesh(const ::NxSoftBodyMeshDesc &desc,NxStream &stream);
#endif

bool  CreatePMap(NxPMap &pmap,	const	::NxTriangleMesh &mesh,	NxU32	density, NxUserOutputStream	*outputStream	=	NULL);
bool  ReleasePMap(NxPMap	&pmap);

}


#endif
