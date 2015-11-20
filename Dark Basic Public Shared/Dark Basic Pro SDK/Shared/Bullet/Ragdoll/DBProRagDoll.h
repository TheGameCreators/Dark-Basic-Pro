
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com


#pragma once

#include "BaseItem.h"
#include "DBProRagDollBone.h"
#include "btBulletDynamicsCommon.h"

class DBProRagDoll : public BaseItem
{
public:
	DBProRagDoll(int ragdollID);
	virtual ~DBProRagDoll();


	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask);
	int AddBone(int objID, int limbID1, int LimbID2, btScalar diameter, float lengthmod, int collisionGroup, int collisionMask);
	int AddBone(int objID, int limbID1, int LimbID2, btScalar diameter, int collisionGroup, int collisionMask);
	void AddJoint( int boneIndex1, int boneIndex2, int jointType, int limbID);
	void AddHingeJoint(int boneIndex1, int boneIndex2, int limbID, btVector3& jointRotation, btScalar minRotation, btScalar maxRotation);
	void AddTwistConstraint(int boneIndex1, int boneIndex2, int limbID, btVector3& jointRotation, btVector3& jointlimits);
	void Finalize();
	void Activate();
	void ApplyForce(int iLimbNumber, btVector3& Start, btVector3& End, float fForceValue);
	void Update();
	void HideBones();
	void ShowBones();
	int GetBoneObjID(int boneIndex);
	void AssignLimbIDToBone(int boneIndex, int dbproLimbID);
	void SetDamping(btScalar linear, btScalar angular);
	void SetSleepingThresholds(btScalar linear, btScalar angular);
	void SetDeactivationTime(btScalar time); 
	void ResetObjectParametersForAnimation();
	void ResetObjectParametersForCulling(); 
	bool IsSleeping();
	void SetStatic(bool bStatic);
	void SetTotalWeight(btScalar totalWeight); 
	void SetFriction(btScalar friction);
	bool IsStatic(); 
	bool IsBoneObject(int objectID);

private:
	void SetCullingRadius(float fRadius, float fScaledRadius, float fScaledLargestRadius);
	void GetCullingRadius(float &outfRadius, float &outfScaledRadius, float &outfScaledLargestRadius);

	btAlignedObjectArray<int> m_joints;
	btAlignedObjectArray<DBProRagDollBone*> m_ragDollBoneArray;

	btScalar m_modelTotalWeight;
	btScalar m_modelTotalVolume;
	btScalar m_modelTotalDensity;
	btScalar m_linearDamping;
	btScalar m_angularDamping;
	btScalar m_linearSleepingThresholds;
	btScalar m_angularSleepingThresholds;
	btScalar m_deactivationTime; 
	bool m_bIsStatic;
	float m_fRadius;
	float m_fScaledRadius;
	float m_fScaledLargestRadius;
	bool m_bNeedsUpdateForCulling;
};

extern DBProRagDoll* currentDBProRagDoll;
