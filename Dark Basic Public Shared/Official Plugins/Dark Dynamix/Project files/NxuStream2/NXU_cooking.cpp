#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "NXU_cooking.h"
#include "NxPhysicsSDK.h"
#include "NxPMap.h"
#include "PhysXLoader.h"

// if	on the Windows platform	and	2.5.0	or higher, use the versioned Cooking
// interface via PhysXLoader
#if defined(WIN32) || defined(LINUX) 
#if	NX_SDK_VERSION_NUMBER	>= 250
#define	COOKING_INTERFACE	1
#endif
#endif

namespace	NXU
{

#ifdef COOKING_INTERFACE
NxCookingInterface *gCooking = 0;
#endif

bool hasCookingLibrary(void) //	check	to see if	the	cooking	library	is available or	not!
{
	bool ret = true;

	#ifdef COOKING_INTERFACE
	if (gCooking ==	0)
	{
		gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
		if (gCooking ==	0)
		{
			ret	=	false;
		}
	}
	#endif

	return ret;
}


bool CookConvexMesh(const	::NxConvexMeshDesc &desc,	NxStream &stream)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxCookConvexMesh(desc,	stream);
	#else
	return NxCookConvexMesh(desc,	stream);
	#endif
}

#if NX_USE_CLOTH_API
bool CookClothMesh(const ::NxClothMeshDesc &desc,	NxStream &stream)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxCookClothMesh(desc, stream);
	#else
	return NxCookClothMesh(desc, stream);
	#endif
}
#endif

#if NX_USE_SOFTBODY_API
bool CookSoftBodyMesh(const ::NxSoftBodyMeshDesc &desc,NxStream &stream)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxCookSoftBodyMesh(desc, stream);
	#else
	return NxCookSoftBodyMesh(desc, stream);
	#endif
}
#endif

bool CookTriangleMesh(const	::NxTriangleMeshDesc &desc,	NxStream &stream)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxCookTriangleMesh(desc,	stream);
	#else
	return NxCookTriangleMesh(desc,	stream);
	#endif
}

bool InitCooking(void)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxInitCooking();
	#else
	return NxInitCooking();
	#endif
}

void CloseCooking()
{
	#ifdef COOKING_INTERFACE
	if (!gCooking)
	{
		return ;
	}
	gCooking->NxCloseCooking();
	#else
	return NxCloseCooking();
	#endif
}


bool CreatePMap(NxPMap &pmap,	const	NxTriangleMesh &mesh,	NxU32	density, NxUserOutputStream	*outputStream)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxCreatePMap(pmap,	mesh,	density, outputStream);
	#else
	return NxCreatePMap(pmap,	mesh,	density, outputStream);
	#endif
}

bool ReleasePMap(NxPMap	&pmap)
{
	#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking)
	{
		return false;
	}
	return gCooking->NxReleasePMap(pmap);
	#else
	return NxReleasePMap(pmap);
	#endif
}


bool  SetCookingParams(const ::NxCookingParams &params)
{
	bool ret = false;
#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (gCooking)
	{
	  ret = gCooking->NxSetCookingParams(params);
	}
#else
  ret = NxSetCookingParams(params);
#endif
  return ret;
}

const ::NxCookingParams & GetCookingParams(void)
{
	static NxCookingParams r;
	NxCookingParams &ret = r;
#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (gCooking)
	{
	  ret = gCooking->NxGetCookingParams();
	}
#else
  ret = NxGetCookingParams();
#endif
  return ret;
}

};
