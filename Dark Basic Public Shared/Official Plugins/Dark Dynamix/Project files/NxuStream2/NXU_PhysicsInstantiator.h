/*----------------------------------------------------------------------
Copyright	(c)	2010 NVIDIA Corporation

NxuPhysicsInstantiator.h

This is	the	common import	implementation that	contains format	independent	instantiation	logic.

Changelist
----------

 */
#ifndef	NXU_PHYSICS_INSTANTIATOR
#define	NXU_PHYSICS_INSTANTIATOR

#include <stdio.h>

#include "NXU_schema.h"
#include "NXU_helper.h"
#include "NXU_customcopy.h"

namespace	NXU
{


/**
\brief Format	independant	importer.

The	class	can	read in	data from	a	provided NxuStream and create	the	appropriate	descriptors.
The	imported data	can	then optionally	be instantiated

 */
class	NxuPhysicsInstantiator
{
public:

		NxuPhysicsInstantiator(void);
		NxuPhysicsInstantiator(NxuPhysicsCollection	*coll);
		virtual	~NxuPhysicsInstantiator();

		/**
		\brief Adds	the	collection to	the	provided SDK,	including	adding scenes

		\param[in] sdk Physics SDK to	load the collection	into.
		\param[in] notify	Optional notification	events to	the	application	as the scene is	loaded.
		\param[in] create	Optional callback	if the user	wants	control	over scene creation.
		\param[in] defaultScene	Optional NxScene to	load the collections default model into.
		\param[in] defaultSceneOffset	Optional The placement of	the	default	model	within the default scene
		\param[in] stype Override	to force creation	of a HW	or SW	scenes

		If defaultScene	is NULL,	defaultSceneOffset is	ignored.

		 */
		void instantiate(NxPhysicsSDK	&sdk,	NXU_userNotify *notify = 0,	NxScene	*defaultScene	=	0,const NxMat34 *defaultSceneOffset = 0);

		NxScene *instantiateScene(const	char *sceneName,	// null	means	the	'root' scene.
		NX_BOOL	ignore_plane,	 //	don't	instantiate	'planes'
		NxPhysicsSDK &sdk, NXU_userNotify	*notify	=	0, NxScene *defaultScene = 0,const	NxMat34	*defaultSceneOffset	=	0);

  void instantiateSceneInstance(NxSceneInstanceDesc *nsi,
  															NxPhysicsSDK	&sdk,	 //	SDK	to load	the	collection into.
  															NXU_userNotify *callback,	 //	notification events	to the application as	the	scene	is loaded.
  															const NxMat34	*defaultSceneOffset,
  															int depth);

  void setInstanceScene(NxScene *scene)
  {
	  instanceDefaultScene = scene;
  }


  NxTriangleMesh * instantiateTriangleMesh(NxPhysicsSDK &sdk,NxTriangleMeshDesc &desc,const char *collectionId,NXU_userNotify	*callback,CustomCopy &cc);
  NxConvexMesh   * instantiateConvexMesh(NxPhysicsSDK &sdk,NxConvexMeshDesc &desc,const char *collectionId,NXU_userNotify	*callback,CustomCopy &cc);

private:
		NxuPhysicsCollection *mCollection;

		void instanceConvexes(NxPhysicsSDK &sdk,NXU_userNotify *callback);
		void instanceSkeletons(NxPhysicsSDK	&sdk,NXU_userNotify *callback);
		void instanceTrimeshes(NxPhysicsSDK &sdk, NXU_userNotify *callback);
		void instanceHeightFields(NxPhysicsSDK &sdk,NXU_userNotify *callback);
#if NX_USE_CLOTH_API
		void instanceClothMeshes(NxPhysicsSDK	&sdk,NXU_userNotify *callback);
#endif
#if NX_USE_SOFTBODY_API
		void instanceSoftBodyMeshes(NxPhysicsSDK	&sdk,NXU_userNotify *callback);
#endif
#if NX_SDK_VERSION_NUMBER >= 260
    void instanceCompartments(NxPhysicsSDK &sdk,NxSceneDesc &sd, NXU_userNotify *callback, bool existingScene);
#endif

		void instanceModel(NxScene &scene,NxSceneDesc	&model,	NxMat34	&pose, bool	isHSM, NXU_userNotify	*callback, NX_BOOL ignorePlane);

	NxScene	*instanceDefaultScene;

	NxSceneDesc	*mCurrentScene; // the current scene we are instancing

};

}


#endif //NXU_PHYSICS_INSTANTIATOR_H


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
