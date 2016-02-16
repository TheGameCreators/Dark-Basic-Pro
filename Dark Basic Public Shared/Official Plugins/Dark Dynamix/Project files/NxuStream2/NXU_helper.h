#ifndef	NXU_HELPER_H

#define	NXU_HELPER_H

#include <stdio.h>
#include <NxPhysics.h>
#include <NxScene.h>

class	NxScene;
class	NxPhysicsSDK;
class	NxJoint;
class	NxActor;
class	NxCloth;
class	NxFluid;
class	NxScene;
class	NxMat34;
class	NxSceneDesc;
class	NxJointDesc;
class	NxActorDesc;
class	NxClothDesc;
class	NxFluidDesc;
class	NxMaterial;
class NxScene;
class NxActor;
class NxJoint;
class NxCloth;
class NxFluid;
class NxMaterial;
class NxConvexMesh;
class NxTriangleMesh;
class NxHeightField;
class NxCCDSkeleton;
class NxEffector;
class NxPairFlag;
class NxGroupsMask;

class NxActorDesc;
class NxJointDesc;
class NxClothDesc;
class NxFluidDesc;
class NxMaterialDesc;
class NxConvexMeshDesc;
class NxTriangleMeshDesc;
class NxHeightFieldDesc;
class NxEffectorDesc;
class NxSimpleTriangleMesh;
class NxClothMeshDesc;
class NxClothMesh;
class NxSoftBody;
class NxSoftBodyDesc;
class NxSoftBodyMesh;
class NxSoftBodyMeshDesc;
class NxSpringAndDamperEffector;
class NxSpringAndDamperEffectorDesc;
class NxForceField;
class NxForceFieldDesc;
class NxCompartment;
class NxCompartmentDesc;

#pragma warning( push )
#pragma warning( disable : 4100 )

/**
 \brief Abstract base class to notify the application of the creation of physics objects.

 The application needs only implement the callback methods that they are interested in receiving.
 To receive notification events be absolutely certain the application prototypes the method
 in exactly the same way found here.  A small typo may cause the code to instead use the
 default method and application will never be called.

 \note During 'pre-notification' events the application is allowed to modify the descriptor provided.  If the user allocates any memory during any callback then it is responsible for de-allocating that memory.
 \note This virtual interfaces are in the global namespace.

*/
class	NXU_userNotify
{
protected:
	virtual ~NXU_userNotify(){};
public:

	/**
	\brief Notifies the application that a new scene has been created.

	\param sno The 'scene number' as a zero based indexed number.
	\param scene  A pointer to the 'NxScene' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyScene(NxU32 sno,	NxScene	*scene,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new compartment has been created.

	\param compartment  A pointer to the 'NxCompartment' that has been created.
	*/
	virtual void	NXU_notifyCompartment(NxCompartment	*compartment){};

	/**
	\brief Notifies the application that a new joint has been created.

	\param joint  A pointer to the joint that was created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyJoint(NxJoint	*joint,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new spring and damper effector has been created.

	\param effector  A pointer to the effector that was created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifySpringAndDamperEffector(NxSpringAndDamperEffector	*effector,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new force field effector has been created.

	\param effector  A pointer to the effector that was created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyForceField(NxForceField	*effector,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new actor has been created.

	\param actor  A pointer to the 'NxActor' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\note  Since there are not discrete callbacks for shapes, the user can iterate on the shapes in the actor desc and cast the 'userData' pointer to a 'const char *' to process the 'userProperties' field for each shape.
	*/
	virtual void	NXU_notifyActor(NxActor	*actor,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new piece of cloth has been created.

	\param cloth  A pointer to the 'NxCloth' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyCloth(NxCloth	*cloth,	const	char *userProperties){};

	/**
	\brief Notifies the application that a soft body has been created.

	\param softBody  A pointer to the 'NxSoftBody' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifySoftBody(NxSoftBody	*softBody,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new fluid has been created.

	\param fluid  A pointer to the 'NxFluid' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyFluid(NxFluid	*fluid,	const	char *userProperties){};

	/**
	\brief Notifies the application that a new TriangleMesh that has been created.

	\param t  A pointer to the 'NxTriangleMesh' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
 	virtual void 	NXU_notifyTriangleMesh(NxTriangleMesh *t,const char *userProperties){};

	/**
	\brief Notifies the application that a new convex mesh has been created.

	\param c      A pointer to the 'NxConvexMesh' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
 	virtual void 	NXU_notifyConvexMesh(NxConvexMesh *c,const char *userProperties){};

	/**
	\brief Notifies the application that a new cloth mesh has been created.

	\param t      A pointer to the 'NxClothMesh' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
 	virtual void 	NXU_notifyClothMesh(NxClothMesh *t,const char *userProperties){};

	/**
	\brief Notifies the application that a new soft body mesh has been created.

	\param t      A pointer to the 'NxSoftBodyMesh' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
 	virtual void 	NXU_notifySoftBodyMesh(NxSoftBodyMesh *t,const char *userProperties){};

	/**
	\brief Notifies the application that a new CCD Skeleton has been created.

	\param t  A pointer to the 'NxCCDSkeleton' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
 	virtual void 	NXU_notifyCCDSkeleton(NxCCDSkeleton *t,const char *userProperties){};

	/**
	\brief Notifies the application that a new heightfield has been created.

	\param t  A pointer to the 'NxHeightField' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyHeightField(NxHeightField *t,const char *userProperties){};

	/**
	\brief Notifies the application that a new material has been created.

	\param t  A pointer to the 'NxMaterial' that has been created.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyMaterial(NxMaterial *t,const char *userProperties){};

	/**
	\brief Notifies the application that an actor is being saved.

	\param pActor A pointer to the 'NxActor' that is being saved.
	\param pUserProperties A pointer to an ASCIIZ string that can be filled with arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifySaveActor(NxActor *pActor, const char** pUserProperties){};

	/**
	\brief Pre-notification event before a new scene is about to be created.

	\param sno The 'scene number' as a zero based indexed number.
	\param scene  The contents of an NxSceneDesc that is going to be used to create this scene.  The user has the right to modify the contents of this descriptor.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return a NULL pointer to have the instantiator create the scene.  A non-null pointer is the NxScene the application wants the data to be loaded into.
	*/
	virtual NxScene *NXU_preNotifyScene(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties) { return	0; };

	/**
	\brief Pre-notification event before a compartment is about to be created.

	\param compartment  The contents of an 'NxCompartmentDesc' that the application can modify.
	\return true to create the compartment or false to skip it.
	*/
	virtual bool	NXU_preNotifyCompartment(NxCompartmentDesc &compartment) { return	true; };

	/**
	\brief Pre-notification event before a joint is about to be created.

	\param joint  The contents of an 'NxJointDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the joint or false to skip it.
	*/
	virtual bool	NXU_preNotifyJoint(NxJointDesc &joint, const char	*userProperties) { return	true; };

	/**
	\brief Pre-notification event before a spring and damper effector is about to be created.

	\param effector  The contents of an 'NxSpringAndDamperEffectorDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the effector or false to skip it.
	*/
	virtual bool	NXU_preNotifySpringAndDamperEffector(NxSpringAndDamperEffectorDesc &effector, const char	*userProperties) { return	true; };

	/**
	\brief Pre-notification event before a force field effector is about to be created.

	\param effector  The contents of an 'NxForceFieldDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the force field or false to skip it.
	*/
	virtual bool	NXU_preNotifyForceField(NxForceFieldDesc &effector, const char	*userProperties) { return	true; };

	/**
	\brief Pre-notification event before an actor is created.

	\param actor The contents of an 'NxActorDesc' that the application is allowed to modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the actor or false to skip it.
	*/
	virtual bool	NXU_preNotifyActor(NxActorDesc &actor, const char	*userProperties) { return	true; };

	/**
	\brief Pre-notification event before a piece of cloth is created.

	\param cloth  The contents of an 'NxClothDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the cloth or false to skip it.
	\note  The application is responsible for initializing the contents of 'NxMeshData'.  These contain the application write buffers for the simulated mesh.  The application must allocate and manage this memory.
	\note  By default the cloth will not be created because the application is responsible for managing the write buffers.
	*/
	virtual bool	NXU_preNotifyCloth(NxClothDesc &cloth, const char	*userProperties) { return	false;};

	/**
	\brief Pre-notification event before a soft body is created.

	\param softBody  The contents of an 'NxSoftBodyDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the softBody or false to skip it.
	\note  The application is responsible for initializing the contents of 'NxMeshData'.  These contain the application write buffers for the simulated mesh.  The application must allocate and manage this memory.
	\note  By default the cloth will not be created because the application is responsible for managing the write buffers.
	*/
	virtual bool	NXU_preNotifySoftBody(NxSoftBodyDesc &softBody, const char	*userProperties) { return	false;};

	/**
	\brief Pre-notification event before a fluid is created.

	\param fluid The contents of an 'NxFluidDesc' that the user can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the fluid or false to skip it.
	\note  The application is responsible for initializing the contents of 'NxParticleData particlesWriteData'.  These contain the application write buffers for the simulated fluid.  The application must allocate and manage this memory.
	\note  By default the fluid will not be created because the application is responsible for managing these write buffers.
	*/
	virtual bool	NXU_preNotifyFluid(NxFluidDesc &fluid, const char	*userProperties) { return	false;};

	/**
	\brief Pre-notification event before a triangle mesh is created.

	\param t  The contents of an 'NxTriangleMeshDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the triangle mesh or false to skip it.
	*/
	virtual bool 	NXU_preNotifyTriangleMesh(NxTriangleMeshDesc &t,const char *userProperties) { return true;	};

	/**
	\brief Pre-notification event before a convex mesh is created.

	\param t The contents of an 'NxConexMeshDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the convex mesh or false to skip it.
	*/
	virtual bool 	NXU_preNotifyConvexMesh(NxConvexMeshDesc &t,const char *userProperties)	{	return true; };

	/**
	\brief Pre-notification event before a cloth mesh is created.

	\param t  The contents of an 'NxClothMeshDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the cloth mesh or false to skip it.
	*/
	virtual bool 	NXU_preNotifyClothMesh(NxClothMeshDesc &t,const char *userProperties) { return true; };

	/**
	\brief Pre-notification event before a soft body mesh is created.

	\param t  The contents of an 'NxSoftBodyMeshDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the soft body mesh or false to skip it.
	*/
	virtual bool 	NXU_preNotifySoftBodyMesh(NxSoftBodyMeshDesc &t,const char *userProperties) { return true; };

	/**
	\brief Pre-notification event before a CCD Skeleton is created.

	\param t The contents of an 'NxSimpleTriangleMesh' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the CCD skeleton or false to skip it.
	*/
	virtual bool 	NXU_preNotifyCCDSkeleton(NxSimpleTriangleMesh &t,const char *userProperties)	{	return true; };

	/**
	\brief Pre-notification event before a HeightField is created.

	\param t  The contents of an 'NxHeightFieldDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the heightfield or false to skip it.
	*/
	virtual bool 	NXU_preNotifyHeightField(NxHeightFieldDesc &t,const char *userProperties) {	return true; }

	/**
	\brief Pre-notification event before a Material is created.

	\param t  The contents of an 'NxMaterialDesc' that the application can modify.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\return true to create the material or false to skip it.
	*/
	virtual bool 	NXU_preNotifyMaterial(NxMaterialDesc &t,const char *userProperties) {	return true; }

	/**
	\brief Pre-notification event before a SceneInstance is executed.

	\param name The name of the new scene instance
	\param sceneName The name of the scene or sceneInstance to be instantiated.
	\param userProperties User properties associated with this scene instantiation
	\param rootNode The accumulated transform while traversing the hierarchy.  The application could modify this if they wanted.
	\param depth The recursion depth of the instantiation.
	\return true to execute the scene instance.
	*/
	virtual bool 	NXU_preNotifySceneInstance(const char *name,const char *sceneName,const char *userProperties,NxMat34 &rootNode,NxU32 depth) { return true; };

	/**
	\brief Notification event because a scene failed to be created.

	\param sno The 'scene number' as a zero based indexed number.
	\param scene  The contents of the *same* 'NxSceneDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifySceneFailed(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties) { };

	/**
	\brief Notification event because a compartment failed to be created.

	\param compartment  The contents of the *same* 'NxCompartmentDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	*/
	virtual void	NXU_notifyCompartmentFailed(NxCompartmentDesc &compartment) {  };

	/**
	\brief Notification event because a joint failed to be created.

	\param joint  The contents of the *same* 'NxJointDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyJointFailed(NxJointDesc &joint, const char	*userProperties) {  };

	/**
	\brief Notification event because a spring and damper effector failed to be created.

	\param effector  The contents of the *same* 'NxSpringAndDamperEffectorDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifySpringAndDamperEffectorFailed(NxSpringAndDamperEffectorDesc &effector, const char	*userProperties) {  };

	/**
	\brief Notification event because a force field effector failed to be created.

	\param effector  The contents of the *same* 'NxForceFieldDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyForceFieldFailed(NxForceFieldDesc &effector, const char	*userProperties) {  };

	/**
	\brief Notification event that an actor failed to be created.

	\param actor The contents of the *same* 'NxActorDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void	NXU_notifyActorFailed(NxActorDesc &actor, const char	*userProperties) { };

	/**
	\brief Notification event that a cloth creation failed.

	\param cloth  The contents of the *same* 'NxClothDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\note  The application is responsible for initializing the contents of 'NxMeshData'.  These contain the application write buffers for the simulated mesh.  The application must allocate and manage this memory.
	\note  Here is where the application can free up any write buffers that were allocated in the pre-notification event.
	*/
	virtual void	NXU_notifyClothFailed(NxClothDesc &cloth, const char	*userProperties) { };

	/**
	\brief Notification event that a soft body creation failed.

	\param softBody  The contents of the *same* 'NxSoftBodyDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\note  The application is responsible for initializing the contents of 'NxMeshData'.  These contain the application write buffers for the simulated mesh.  The application must allocate and manage this memory.
	\note  Here is where the application can free up any write buffers that were allocated in the pre-notification event.
	*/
	virtual void	NXU_notifySoftBodyFailed(NxSoftBodyDesc &softBody, const char	*userProperties) { };

	/**
	\brief Notification event that a fluid creation failed.

	\param fluid The contents of the *same* 'NxFluidDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	\note	 Here is where the application can free up any write buffers that may have been allocated in the pre-notification event.
	*/
	virtual void	NXU_notifyFluidFailed(NxFluidDesc &fluid, const char	*userProperties) { };

	/**
	\brief Notification event that a triangle mesh failed to be created.

	\param t  The contents of the *same* 'NxTriangleMeshDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyTriangleMeshFailed(NxTriangleMeshDesc &t,const char *userProperties) {	};

	/**
	\brief notification event before a convex mesh is created.

	\param t The contents of the *same* 'NxConvexMeshDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyConvexMeshFailed(NxConvexMeshDesc &t,const char *userProperties)	{	 };

	/**
	\brief Notification event that a cloth mesh failed to be created.

	\param t  The contents of the *same* 'NxClothMeshDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyClothMeshFailed(NxClothMeshDesc &t,const char *userProperties) { };

	/**
	\brief Notification event that a soft body mesh failed to be created.

	\param t  The contents of the *same* 'NxSoftBodyMeshDesc' that was supplied in the pre-notify call. (Allows the application to free up memory)
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifySoftBodyMeshFailed(NxSoftBodyMeshDesc &t,const char *userProperties) { };

	/**
	\brief Notification event that a CCD Skeleton failed to be created.

	\param t The contents of the *same* 'NxSimpleTriangleMesh' that was supplied in the pre-notify call.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyCCDSkeletonFailed(NxSimpleTriangleMesh &t,const char *userProperties)	{	 };

	/**
	\brief Notification event that a HeightField failed to be created.

	\param t  The contents of the *same* 'NxHeightFieldDesc' that was supplied in the pre-notify call.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyHeightFieldFailed(NxHeightFieldDesc &t,const char *userProperties) {	}

	/**
	\brief Notification event that a Material failed to be created.

	\param t  The contents of the *same* 'NxMaterialDesc' that was supplied in the pre-notify call.
	\param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	*/
	virtual void 	NXU_notifyMaterialFailed(NxMaterialDesc &t,const char *userProperties) {	}


};

/**
 \brief Abstract base class to notify the application of errors or warning while processing NxuStream data.

 \note    This virtual interface is in the global namespace
*/
class	NXU_errorReport
{
protected:
	virtual ~NXU_errorReport(){};

public:
	/**
	\brief An application provided virtual method to receive error or warning messages from NxuStream

	\param isError True if reporting an error, false if reporting a warning.
	\param str A string that contains a description of the error or warning encountered.
	*/
	virtual	void NXU_errorMessage(bool isError, const char	*str)	=	0;
};



/**
 \brief The NxuStream serialization library is contained within the namespace 'NXU'

  The NxuStream library handles serialization of all SDK objects.  It is delivered
  as a 'source code' library.  Simply add the source code into the application project and build
  it.  To use the NxuStream library include only a single header file 'NXU_Helper.h'
  and call the discrete methods.

  NxuStream data can be imported as binary, XML, or the COLLADA 1.4.1 physics format.  It can
  be exported in any of these three formats as well.  For example, here is how to load
  a BINARY NxuStream file and save it as a COLLADA file.  An asset, no matter how large or small,
  is referred to as an NXU::NxuPhysicsCollection.


  NXU::NxuPhysicsCollection *c = NXU::loadCollection("test.nxb", NXU::FT_BINARY );

  if ( c ) NXU::saveCollection(c,"test.dae", NXU::FT_COLLADA );

  A 'physics collection' is a set of related scenes, meshes, actors, constraints, cloth, and
  soft bodies, fluids.  If can be everything in an entire game level or something as simple as just
  one 'chair'  Once a physics collection has been instantiated on the SDK it can be released
  without memory leaks or dangling pointers.

	Collections can be loaded and saved from a buffer in memory, which allows the application the
	opportunity to perform its	own file access.

	In addition to being able to serialize the entire contents of the PhysX SDK, or a single scene, an
	application can also serialize individual objects and save them out directly.

  NxuStream allows the application to set an error reporting callback so that warnings and errors that
  might occur during processing can be captured.

  In addition to being notified after an object is created the application can also be 'pre-notified'.
  This allows the application to modify the descriptor or decline to have the object created at all.
  Additionally, each physics object has an optional 'user properties' field which can contain an arbitrary
  string of ASCII data.  This ASCII data might contain early or late binding information used by the game
  engine to associate physics content with other assets in the game.

  A small amount of persistent memory is allocated by the NxuStream library to keep track of name fields,
  user properties, and mesh data that has been instantiated.  When an application exits *or* resets the state
  of the PhysX SDK it should invoke the method NXU::releasePersistentMemory to reset this state.

*/
namespace	NXU
{

/**
\brief Load of Save a physics collection in the following file format.

*/
enum NXU_FileType
{
	/**
	\brief Load or Save in binary streaming format.	 Binary files are not compatible across different versions of the SDK.
	*/
	FT_BINARY,
	/**
	\brief Load or Save in XML NxuStream format.  Compatible across different versions of the SDK 2.4.0 and higher.
	*/
	FT_XML,
	/**
	\brief Load or Save in COLLADA Physics 1.4.1 format.  Loads and saves only physics content.  Ingores graphics and other data assets.
	*/
	FT_COLLADA
};

class	NxuPhysicsCollection;

/**
\brief Set the NxuStream error report interface.

\param er A pointer to the application supplied NXU_errorReport interface.  A NULL pointer indicates no callbacks are performed. Default is NULL.
*/
void 										setErrorReport(NXU_errorReport *er);

/**
\brief Loads an NxuPhysicsCollection either from a file on disk or a buffer in memory.

\param fname  The name of the file or asset name for this collection.
\param type   The NXU_FileType of the source asset.
\param mem    Optional pointer to a buffer in memory to load the asset from.
\param len    If providing a buffer in memory then this represents the length of the available source data.
\return Returns a pointer to an NXU::NxuPhysicsCollection if successful or NULL if it failed.
*/
NxuPhysicsCollection *	loadCollection(const char	*fname,	NXU_FileType type,void *mem=0,int len=0);

/**
\brief Save a physics collection to a file on disk.

\param c The pointer to a valid NXU::NxuPhysicsCollection
\param fname The name of the file to save it to.  This can be a fully specified or relative path name.
\param type  The file format to save in, BINARY, XML, or COLLADA
\param save_defaults True if saving in XML and want to include default values.  If this is set to false, and saving in XML, it will not save out tags that are already at default values for the descriptor.  This makes files much smaller and easier to read.
\param cook  True if saving in XML or BINARY and wish to export the fully cooked triangle meshes rather than the raw source geometry.  Supports faster file loading and is mostly used for binary formats.
\return true if the collection was successfully saved.
*/
bool 										saveCollection(NxuPhysicsCollection *c,const char	*fname,NXU_FileType type, bool	save_defaults=true, bool	cook=false);

/**
\brief Save a physics collection to a buffer in memory.

\param c  A pointer to the physics collection to save.
\param collectionId The ID of the collection.  If null, it will either generate a default Id or use the one specified by the 'createCollection' call.
\param type  The file format to save in, BINARY, XML, or COLLADA
\param save_defaults True if saving in XML and want to include default values.  If this is set to false, and saving in XML, it will not save out tags that are already at default values for the descriptor.  This makes files much smaller and easier to read.
\param cook  True if saving in XML or BINARY and want to export the fully cooked triangle meshes rather than the raw source geometry.  Supports faster file loading and is mostly used for binary formats.
\param mem   The buffer in memory to store the results.  If 'NULL' then memory will by dynamically allocated for the buffer.
\param len   The length of the write buffer passed in.
\param outputLength A reference that will receive the exact output length of the save data.
\return The buffer in memory of the save data.  This will either be the buffer passed in by the application or the allocated memory.  If this is NULL then the save process failed.
\note  If memory was dynamically allocated during the save operation then	use 'releaseCollectionMemory' to free it up.
*/
void * 									saveCollectionToMemory(NxuPhysicsCollection *c,const char	*collectionId,NXU_FileType type, bool	save_defaults, bool	cook,void *mem,int len,size_t &outputLength);

/**
\brief Release dynamically allocated memory by calling 'saveCollectionToMemory' with 'mem=NULL'

\param mem The pointer to the previously returned value from a call to 'saveCollectionToMemory' using dynamic allocation.
*/
void   									releaseCollectionMemory(void *mem);

/**
\brief Instantiates a physics collection into the PhysX SDK

\param c A pointer to the physics collection to be instantiated.
\param sdk  A reference to the instance of the PhysX SDK to use.
\param scene An optional pointer to the 'NxScene' to instantiate data into.  Otherwise a scene will be created automatically.
\param rootNode An optional root node transform that all objects will be created relative to.
\param un An optional user notification interface.
\return true if the collection was instantiated.
*/
bool 										instantiateCollection(NxuPhysicsCollection	*c,	NxPhysicsSDK &sdk, NxScene *scene,const NxMat34 *rootNode,	NXU_userNotify *un);

/**
\brief Instantiates a physics collection into the PhysX SDK

\param c A pointer to the physics collection.
\param sceneName This should refer to the name of a scene in the collection *or* the name of a root level 'sceneInstance'
\param sdk  A reference to the instance of the PhysX SDK to use.
\param scene An optional pointer to the 'NxScene' to instantiate data into.  Otherwise a scene will be created automatically.
\param rootNode An optional root node transform that all objects will be created relative to.
\param un An optional user notification interface.
\return true if the collection was instantiated.
*/
bool 										instantiateScene(NxuPhysicsCollection	*c,const char *sceneName,NxPhysicsSDK &sdk, NxScene *scene, NxMat34 *rootNode,	NXU_userNotify *un);

/**
\brief Release memory associated with a physics collection.

\param c A pointer to the physics collection to be released.
\note Does not release memory for names or user properties so that these can persist between multiple calls.  Also retains a named binding to meshes which have been created between multiple load calls.
\note For example if a ragdoll composed of convex hulls was loaded, instantiated, and released once then, the next time that same ragdoll was loaded it must avoid creating the same convex meshes over and over again.
\note A named binding between hulls in the ragdoll and the hulls that were created into the SDK has to be retained to avoid this.
*/
void 										releaseCollection(NxuPhysicsCollection	*c);

/**
\brief A convenience method to save out the entire contents of the PhysX SDK to a file on disk.

\param sdk A pointer to the PhysX SDK to serialize.
\param fname The file on disk to save it to.
\param type The type of file to save as. (BINARY, XML, COLLADA)
\param save_defaults True if saving in XML and want to include default values.  If this is set to false, and saving in XML, it will not save out tags that are already at default values for the descriptor.  This makes files much smaller and easier to read.
\param cook  True if saving in XML or BINARY and want to export the fully cooked triangle meshes rather than the raw source geometry.  Supports faster file loading and is mostly used for binary formats.
\param un An optional user notification interface.
\return true if the save was successful.
*/
bool 										coreDump(NxPhysicsSDK *sdk, const char	*fname,NXU_FileType type,	bool save_defaults=true,bool cook=false, NXU_userNotify *un = 0);

/**
\brief A convenience function to extract the contents of the PhysX SDK into a NxuPhysicsCollection

\param sdk A pointer to the PhysXSDK to be serialized into a NxuPhysicsCollection
\param collectionId An optional unique name ID for the collection.  If collectionId is null a default name will be generated.
\return A pointer to the NxuPhysicsCollection containing the serialized data from the SDK; NULL if it failed.
\param un An optional user notification interface.
*/
NxuPhysicsCollection *	extractCollectionSDK(NxPhysicsSDK	*sdk,const char *collectionId=0, NXU_userNotify *un = 0);

/**
\brief  A convenience function to extract the contents of a single NxScene into a NxuPhysicsCollection

\param scene The 'NxScene' to be extracted into a NxuPhysicsCollection
\param collectionId The optional unique ID to assign the collection to.
\param sceneId The optional unique ID to assign the scene to.
\return A pointer to the NxuPhysicsCollection containing the serialized data from the SDK.  NULL if it failed.
*/
NxuPhysicsCollection *	extractCollectionScene(NxScene *scene,const char *collectionId=0,const char *sceneId=0);

/**
\brief  Assigns a specific ID string to a collection.

\param c The NxuPhysicsCollection to set the id for.
\param collectionId An string to designate the unique 'ID' for this physics collection.
*/
void                    setCollectionID(NxuPhysicsCollection &c,const char *collectionId);

/**
\brief Creates an empty NxuPhysicsCollection
You should not use any of the methods of the NxuPhysicsCollection object, but rather use the helper functions
in this file to handle the collection.

\param collectionId the optional unique Id to associate this collection with.
\param userProperties  The optional user properties string.
\return A pointer to the new empty NxuPhysicsCollection.
*/
NxuPhysicsCollection *	createCollection(const char *collectionId=0,const char *userProperties=0);

/**
\brief adds a single parameter to an NxuPhysicsCollection

\param c The NxuPhysicsCollection to add data to.
\param param The SDK parameter to add.
\param value The value of the SDK parameter to add
\return true if successful.
*/
bool 										addParameter(NxuPhysicsCollection &c,    		NxU32 param,float value);

/**
\brief adds the PhysicsSDK descriptor settings to the collection.

\param c The NxuPhysicsCollection to add data to.
\param sdk The SDK instance.
\return true if successful.
*/
bool 										addPhysicsSDK(NxuPhysicsCollection &c,NxPhysicsSDK &sdk);

/**
\brief adds a scene desc to an NxuPhysicsCollection.  Does *not* add all of the contents of the scene.

\param c The NxuPhysicsCollection to add data to.
\param s The NxScene to be added.  This only adds the NxScene desc, it does not add the contents of the scene!
\param userProperties The optional userProperties string that will be associated with this scene.
\param sceneId Optional unique sceneId.  If null, one will be automatically assigned.
\return true if successful.
*/
bool 										addScene(NxuPhysicsCollection &c,NxScene &s,const char *userProperties=0,const char *sceneId=0);

/**
\brief adds a Scene and all of its contents to an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param s The NxScene to be added.  This not only adds the NxScene desc, it also adds all of the contents of the scene as well.
\param userProperties The optional userProperties string that will be associated with this scene.
\param sceneId Optional unique sceneId.  If null, one will be automatically assigned.
\param un An optional user notification interface.
\return true if successful.
*/
bool 										addEntireScene(NxuPhysicsCollection &c,    	NxScene &s,const char *userProperties=0,const char *sceneId=0, NXU_userNotify *un = 0);

/**
\brief adds a single Actor to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param a The NxActor to be added.
\param userProperties The optional userProperties string that will be associated with this actor.
\param actorId Optional unique actorId.  If null, one will be provided.
\return true if successful.
*/
bool 										addActor(NxuPhysicsCollection &c,          	NxActor &a,const char *userProperties=0,const char *actorId=0);


/**
\brief adds a single joint to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param j The NxJoint to be added.
\param userProperties The optional userProperties string that will be associated with this joint.
\param jointId The optional unique joint Id.  If null one will be provided.
\return true if successful.
*/
bool 										addJoint(NxuPhysicsCollection &c,          	NxJoint &j,const char *userProperties=0,const char *jointId=0);

/**
\brief adds a single piece of cloth to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param cloth The NxCloth to be added.
\param userProperties The optional userProperties string that will be associated with this cloth.
\param clothId Optional unique cloth Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addCloth(NxuPhysicsCollection &c,          	NxCloth &cloth,const char *userProperties=0,const char *clothId=0);

/**
\brief adds a single soft body to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param softBody The NxSoftBody to be added.
\param userProperties The optional userProperties string that will be associated with this soft body.
\param softBodyId Optional unique soft body Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addSoftBody(NxuPhysicsCollection &c,NxSoftBody &softBody,const char *userProperties=0,const char *softBodyId=0);

/**
\brief adds a single soft body to the current scene in an NxuPhysicsCollection from descriptors.

\param c The NxuPhysicsCollection to add data to.
\param softBody The NxSoftBodyDesc to be added.
\param softMesh The NxSoftMeshDesc to be associated with this soft body descriptor
\param userProperties The optional userProperties string that will be associated with this soft body.
\param softBodyId Optional unique soft body Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addSoftBody(NxuPhysicsCollection &c,const ::NxSoftBodyDesc &softBody,const ::NxSoftBodyMeshDesc &softMesh,const char *userProperties=0,const char *softBodyId=0);


/**
\brief adds a single fluid to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param f The NxFluid to be added.
\param userProperties The optional userProperties string that will be associated with this fluid.
\param fluidId Optional unique fluid Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addFluid(NxuPhysicsCollection &c,          	NxFluid &f,const char *userProperties=0,const char *fluidId=0);

/**
\brief adds a single material to the current scene in an NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param m The NxMaterial to be added.
\param userProperties The optional userProperties string that will be associated with this material.
\param materialId The optional unique material Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addMaterial(NxuPhysicsCollection	&c,	      NxMaterial &m,const char *userProperties=0,const char *materialId=0);

/**
\brief adds a single convex mesh to the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param mesh The NxConvexMesh to be added.
\param userProperties The optional userProperties string that will be associated with this convex mesh.
\param convexId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addConvexMesh(NxuPhysicsCollection &c,     	NxConvexMesh &mesh,const char *userProperties=0,const char *convexId=0);

/**
\brief adds a single triangle mesh to the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param mesh The NxTriangleMesh to be added.
\param userProperties The optional userProperties string that will be associated with this triangle mesh.
\param meshId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addTriangleMesh(NxuPhysicsCollection &c,   	NxTriangleMesh &mesh,const char *userProperties=0,const char *meshId=0);

/**
\brief adds a single heightfield mesh to the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param hmesh The NxHeightFieldMesh to be added.
\param userProperties The optional userProperties string that will be associated with this heightfield.
\param heightFieldId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addHeightField(NxuPhysicsCollection &c,    	NxHeightField &hmesh,const char *userProperties=0,const char *heightFieldId=0);

/**
\brief adds a single CCD skeleton mesh to the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param skel The NxCCDSkeleton to be added.
\param userProperties The optional userProperties string that will be associated with this CCD Skeleton.
\param skeletonId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addCCDSkeleton(NxuPhysicsCollection &c,    	NxCCDSkeleton &skel,const char *userProperties=0,const char *skeletonId=0);

/**
\brief adds a single effector to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param e The NxEffector to be added.
\param userProperties The optional userProperties string that will be associated with this effector.
\param effectorId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addEffector(NxuPhysicsCollection &c,				NxEffector &e,const char *userProperties=0,const char *effectorId=0);

/**
\brief adds a single force field to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param e The NxForceField to be added.
\param userProperties The optional userProperties string that will be associated with this force field.
\param forceFieldId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addForceField(NxuPhysicsCollection &c,				NxForceField &e,const char *userProperties=0,const char *forceFieldId=0);

#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief adds a single force field scale table entry to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param v The NxForceFieldVariety index.
\param mat The NxForceFieldMaterial index.
\param scale The scaling for the given coordinate in the table.
\return true if successful.
*/
bool										addForceFieldScale(NxuPhysicsCollection &c,			NxForceFieldVariety v, NxForceFieldMaterial mat, NxReal scale);
#endif

#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief adds a single force field shape group to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param g The NxForceFieldShapeGroup to be added.
\param userProperties The optional userProperties string that will be associated with this force field.
\param forceFieldId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool										addForceFieldShapeGroup(NxuPhysicsCollection &c,	NxForceFieldShapeGroup &g,	const char *userProperties = NULL,const char *forceFieldId = NULL);
#endif

#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief adds a single force field shape group to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param k The NxForceFieldLinearKernel to be added.
\param userProperties The optional userProperties string that will be associated with this force field.
\param forceFieldId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool										addForceFieldLinearKernel(NxuPhysicsCollection &c,	NxForceFieldLinearKernel &g,	const char *userProperties = NULL,const char *forceFieldId = NULL);
#endif

/**
\brief adds a single NxPairFlag to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param f The NxPairFlag to be added.
\param userProperties The optional userProperties string that will be associated with this pair flag.
\param pairFlagId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool 										addPairFlag(NxuPhysicsCollection &c,       	NxPairFlag &f,const char *userProperties=0,const char *pairFlagId=0);

#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief adds a single NxConstraintDominance item to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param group0 The group ID of the first group
\param group1 The group ID of the second group
\param f The NxConstraintDominance to be added.
\return true if successful.
*/
bool 										addDominancePair(NxuPhysicsCollection &c,       	NxU16 group0, NxU16 group1, NxConstraintDominance &f);
#endif

/**
\brief adds a single GroupCollision flag to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param group1 The group1 value (0-31)
\param group2 The group2 value (0-31)
\param enable Whether to enable or disable collisions between these group pairs
\return true if successful.
*/
bool                    addGroupCollisionFlag(NxuPhysicsCollection &c,NxU32 group1,NxU32 group2,bool enable);

/**
\brief adds a single ActorGroup flag to the current scene in the NxuPhysicsCollection.

\param c The NxuPhysicsCollection to add data to.
\param group1 The group1 value (0-65535)
\param group2 The group2 value (0-65535)
\param flags The bit flags representing any combination of bit flags from NxContactPairFlag
\return true if successful.
*/
bool                    addActorGroupFlag(NxuPhysicsCollection &c,NxU32	group1,NxU32 group2,NxU32 flags);

/**
\brief sets the filter operations and constants for the current scene.

\param c The NxuPhysicsCollection to add data to.
\param filterOp0 The value of NxFilterOp for the first operation.
\param filterOp1 The value of NxFilterOp for the second operation.
\param filterOp2 The value of NxFilterOp for the third operation
\param filterConstant0 The NxGroupsMask for filter constant 0
\param filterConstant1 The NxGroupsMask for filter constant 1
\return true if successful.
*/
bool 										setFilterOps(NxuPhysicsCollection &c,bool active,::NxFilterOp filterOp0,::NxFilterOp filterOp1,::NxFilterOp filterOp2,const ::NxGroupsMask &filterConstant0,const ::NxGroupsMask &filterConstant1);

/**
\brief Adds a single compartment to the physics collection.

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the compartment to be added to the current scene in this collection.
\param compartmentId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool                    addCompartment(NxuPhysicsCollection &c,NxCompartment &cmp,const char *compartmentId=0);

/**
\brief Adds a single cloth mesh to the collection

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the NxClothMesh
\param clothMeshId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool                    addClothMesh(NxuPhysicsCollection &c,NxClothMesh &cmp,const char *clothMeshId=0);

/**
\brief Adds a single cloth mesh to the collection

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the NxClothMesh
\param softBodyMeshId The optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool                    addSoftBodyMesh(NxuPhysicsCollection &c,NxSoftBodyMesh &cmp,const char *softBodyMeshId=0);

/**
\brief Adds a scene instantiation to the current scene or scene instance.

\param c The NxuPhysicsCollection to add data to.
\param instanceId  The id of the instance created.  Pass in NULL if you do not want this instantiated by default.
\param sceneName  The name of the scene to be instantiated.
\param rootNode   The root node transform to instantiate the scene relative to.
\param ignorePlane Whether or not to ignore ground planes upon instantiation.  True by default.
\return true if successful.
*/
bool                    addSceneInstance(NxuPhysicsCollection &c,const char *instanceId,const char *sceneName,const NxMat34 &rootNode,bool ignorePlane=true,const char *userProperties=0);

/**
\brief Adds a scene instantiation to the current scene or scene instance, and makes this new instance current.  Allows support to build a hierarchy.  Must be matched by a call to closeCurrentSceneInstance

\param c The NxuPhysicsCollection to add data to.
\param instanceId   The id of the instance created.
\param sceneName  The name of the scene to be instantiated.
\param rootNode   The root node transform to instantiate the scene relative to.
\param ignorePlane Whether or not to ignore ground planes upon instantiation.  True by default.
\return true if successful.
*/
bool                    addToCurrentSceneInstance(NxuPhysicsCollection &c,const char *instanceId,const char *sceneName,const NxMat34 &rootNode,bool ignorePlane=true,const char *userProperties=0);

/**
\brief Closes out the current scene we are adding to.

\param c The NxuPhysicsCollection to add data to.
\return true if successful.
*/
bool                    closeCurrentSceneInstance(NxuPhysicsCollection &c);

/**
\brief Returns the ID name of this physics collection.

\param c The NxuPhysicsCollection to query.
\return Returns the ID name of the collection or a null if not found.
*/
const char *            getCollectionName(NxuPhysicsCollection &c);

/**
\brief Returns the number of scenes in the collection.

\param c The NxuPhysicsCollection to query.
\return Returns the total number of scenes found in the collection.
*/
NxU32                   getSceneCount(NxuPhysicsCollection &c);

/**
\brief Returns the ID name of a particular scene in a collection

\param c The NxuPhysicsCollection to query.
\param sceneNo The index of the scene in the collection.

\return Returns the ID name of the scene in question or a null pointer if the index is invalid.
*/
const char *            getSceneName(NxuPhysicsCollection &c,NxU32 sceneNo);

/**
\brief Returns the number of scene *instances* in the collection.  Root level only.

\param c The NxuPhysicsCollection to query.
\return Returns the total number of scenes instances found at the root level of the collection.
*/
NxU32                   getSceneInstanceCount(NxuPhysicsCollection &c);

/**
\brief Returns the ID name of a particular scene instance in a collection

\param c The NxuPhysicsCollection to query.
\param sceneNo The index of the scene instance in the collection.

\return Returns the ID of the scene instance in question or a null pointer if the index is invalid.
*/
const char *            getSceneInstanceName(NxuPhysicsCollection &c,NxU32 sceneNo);

/**
\brief Returns the a particular scene instance in a collection

\param c The NxuPhysicsCollection to query.
\param sceneNo The index of the scene instance in the collection.

\return Returns the ID name of the scene instance in question or a null pointer if the index is invalid.
*/
const char *            getSceneInstanceSceneName(NxuPhysicsCollection &c,NxU32 sceneNo);

/**
\brief Releases persistent memory for name fields, user properties, and bindings to instantiated meshes.

\note This routine should be called before an application exits.
\note It is very important to call this routine if the NxPhysicsSDK is released or meshes are manually destroyed.
*/
void 										releasePersistentMemory(void);

/**
\brief Sets a global behavior for the instantiator class whether or not to attempt to serialize cloth in its actively simulated state.  Default is false.
\note  Available only on 2.6.0 SDK and higher.
*/
void										setUseClothActiveState(bool state);

/**
\brief Sets a global behavior for the instantiator class whether or not to attempt to serialize SoftBodies in their actively simulated state.  Default is false.
\note  Available only on 2.6.0 SDK and higher.
*/
void										setUseSoftBodyActiveState(bool state);

/**
\brief Sets a global behavior to auto-generate CCD skeletons on export or not.
\param shrink  The ratio to shrink the CCD skeleton relative to the original shape.
\param maxV    The maximum number of vertices to create for a CCDSkeleton
*/
void										setAutoGenerateCCDSkeletons(bool state,NxReal shrink,NxU32 maxV);

/**
\brief Automatically generates a set of CCD skeletons for all dynamic shapes in an NxuPhysicsCollection

\param c   The NxuPhysicsCollection to generate CCD skeletons for.
\param shrink  The ratio to shrink the CCD skeleton relative to the original shape.
\param maxV    The maximum number of vertices to create for a CCDSkeleton
\return   Returns the number of CCD Skeletons generated.
*/
NxU32 									createCCDSkeletons(NxuPhysicsCollection &c,NxReal shrink=0.9f,NxU32 maxV=16);


/**
\brief Automatically generates a CCD skeletons for the shapes within a single actor.

Be sure to call 'releasePersistentMemory' before you reset the SDK, especially since this routine tracks
the active skeletons that have been instantiated on the SDK.

\param actor The NxActor pointer to generate a skeleton for.
\return  Returns true if it was successful or false it it failed.
*/
bool autoGenerateCCDSkeletons(NxActor *actor,NxReal shrink=0.9f,NxU32 maxV=16);


/**
\brief Parses a user properties field in the form of "key1=value1,key2=value2" etc.

The return values are not persistent so you must process the contents before calling the routine again.

\param userProperties A ponter to a user properties field as key value pairs.
\param cont  A reference to an unsigned integer that will contain a count of how many key/value pairs were found.
\return   Returns a set of const char pointers in the form key1,value2, key2,value2, up to the total number of key value pairs.  It will be padded with two null pointers as end of data indicators.
*/
const char ** parseUserProperties(const char *userProperties,unsigned int &count);


/**
\brief Parses a user properties field in the form of "key1=value1,key2=value2" etc. and returns the value for a matching key

The return values are not persistent so you must process the contents before calling the routine again.
If the return value is NULL it indicates that the key was not found.

\param userProperties A ponter to a user properties field as key value pairs.
\param key A pointer to a string indicating the key to search for.
\return   Returns A pointer to the value for the requested key, or null if not found.
*/
const char *  getKeyValue(const char *userProperties,const char *key);  //

/**
\brief Sets the NxuStream library in Big-Endian or Little-Endian mode for loads and saves.

\param processorEndian Indicate whether the current processor this code is running on is big or little endian.
\param endianSave  Which endian type to use when saving data in binary format. Default is false for little endian on PC architecture.
*/
void  setEndianMode(bool processorEndian,bool endianSave=false);

}

#pragma warning( pop )

#endif
