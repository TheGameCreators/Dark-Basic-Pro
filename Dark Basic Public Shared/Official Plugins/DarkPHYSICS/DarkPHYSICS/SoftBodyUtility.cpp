
#include "SoftBodyUtility.h"
#include "globals.h"
#include "..\..\..\..\Dark Basic Pro SDK\Shared\Camera\CCameraDataC.h"

#include "objmesh.h"
#include "mysoftbody.h"
#include "cooking.h"

NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return PhysX::pScene->createActor(actorDesc);	
}

NxActor* CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return PhysX::pScene->createActor(actorDesc);	
}

NxActor* CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0,radius+(NxReal)0.5*height,0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return PhysX::pScene->createActor(actorDesc);	
}

NxActor* CreateCompoundCapsule(const NxVec3& pos0, const NxReal height0, const NxReal radius0, const NxReal height1, const NxReal radius1)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has two shapes (capsules)
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height0;
	capsuleDesc.radius = radius0;
	capsuleDesc.localPose.t = NxVec3(0,radius0+(NxReal)0.5f*height0,0);
	NxMat33 rot0, rot1;
	rot0.rotX(-0.4f);
	rot1.rotZ(-0.3f);
	rot0 = rot0*rot1;
	capsuleDesc.localPose.M = rot0;
	actorDesc.shapes.pushBack(&capsuleDesc);

	NxCapsuleShapeDesc capsuleDesc1;
	capsuleDesc1.height = height1;
	capsuleDesc1.radius = radius1;
	capsuleDesc1.localPose.t = NxVec3(0.33f,3.15f,-0.4f);
	rot0.rotY(-0.04f);
	capsuleDesc1.localPose.M = rot0;
	actorDesc.shapes.pushBack(&capsuleDesc1);

	actorDesc.body = &bodyDesc;
	actorDesc.density = 1.0f;

	actorDesc.globalPose.t = pos0;
	return PhysX::pScene->createActor(actorDesc);	
}

NxSphericalJoint* CreateRopeSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.1*NxPi;
	sphericalDesc.twistLimit.high.value = (NxReal)0.1*NxPi;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	NxSpringDesc ts;
	ts.spring = 500;
	ts.damper = 0.5;
	ts.targetValue = 0;
	sphericalDesc.twistSpring = ts;

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.3*NxPi;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	NxSpringDesc ss;
	ss.spring = 500;
	ss.damper = 0.5;
	ss.targetValue = 0;
	sphericalDesc.swingSpring = ss;

	return static_cast<NxSphericalJoint*>(PhysX::pScene->createJoint(sphericalDesc));
}

