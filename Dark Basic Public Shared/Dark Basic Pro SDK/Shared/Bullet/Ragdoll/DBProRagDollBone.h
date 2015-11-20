
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

#pragma once

#include "btBulletDynamicsCommon.h"

class DBProRagDollBone
{
public:
	DBProRagDollBone(int dbproObjectID, int dbproStartLimbID, int dbproEndLimbID, btScalar diameter, float lengthmod, int collisionGroup, int collisionMask);
	~DBProRagDollBone(void);
	btRigidBody* GetRigidBody();
	int GetEndLimbID();
	int GetStartLimbID();
	int GetObjectID();
	int GetRagDollBoneID();
	btVector3 GetNormilizedVector();
	void AddJointConstraint(int jointID, int jointType);
	void AddDBProLimbID(int dbproLimbID);
	btMatrix3x3 initialRotation;
	btAlignedObjectArray <int> dbproLimbIDs;
	btAlignedObjectArray <btVector3> limbOffsets;
	btAlignedObjectArray <btMatrix3x3> limbInitalRotation;
	btAlignedObjectArray <btVector3> ceterOfObjectOffsets;
	btScalar boneVolume;

private:
	int dbproObjectID;
	int dbproStartLimbID;
	int dbproEndLimbID;
	int dbproRagDollBoneID;
	btScalar diameter;
	btScalar lengthmod;
	int collisionGroup;
	int collisionMask;
	btVector3 boneNormVec;  
	btRigidBody* rigidBody;
	btTypedConstraint* jointConstraint;
	btCollisionShape* m_collisionShape;

private:
	void CreateBone();
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask);
};

