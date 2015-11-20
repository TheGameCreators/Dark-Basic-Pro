
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

///#include "StdAfx.h"
#include "DBProRagDollBone.h"
#include "DBProMotionState.h"
#include "DBPro.hpp"

// externs to globals elsewhere
extern btDiscreteDynamicsWorld* g_dynamicsWorld;

DBProRagDollBone::DBProRagDollBone(int dbproObjectID, int dbproStartLimbID, int dbproEndLimbID, btScalar diameter, btScalar lengthmod, int collisionGroup, int collisionMask)
{

	this->dbproObjectID = dbproObjectID;
	this->dbproStartLimbID = dbproStartLimbID;
	this->dbproEndLimbID = dbproEndLimbID;
	this->diameter = diameter;
	this->lengthmod = lengthmod;
	this->collisionGroup = collisionGroup;
	this->collisionMask = collisionMask;
	CreateBone();
}

DBProRagDollBone::~DBProRagDollBone(void)
{
	g_dynamicsWorld->removeRigidBody(rigidBody);
	DBProMotionState* dbproMotionState = (DBProMotionState*)rigidBody->getMotionState();
	SAFE_DELETE(dbproMotionState); 
	SAFE_DELETE(rigidBody);
	SAFE_DELETE(m_collisionShape);
	if ( dbproRagDollBoneID > 0 ) DBPro::DeleteObject(dbproRagDollBoneID);
}

enum eAxis
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

int DBProPrimitiveCreateCapsule(btScalar diameter, btScalar height, eAxis axis)
{
	height = (height - diameter)/2.0f;
	//rem rows must be odd
	int rows = 11; 
	int cols = 16;
	int tempSphere = DBPro::FindFreeObject();
	DBPro::MakeObjectSphere(tempSphere, diameter, rows, cols);
	if(axis == Z_AXIS)
		DBPro::RotateObject(tempSphere,90.0,0.0,0.0);
	if(axis == X_AXIS)
		DBPro::RotateObject(tempSphere,0.0,0.0,90.0);
	DBPro::LockVertexdataForLimb(tempSphere, 0, 1);
	for(int i = 0; i < DBPro::GetVertexdataVertexCount(); i++)
	{
		btScalar vertX = DBPro::GetVertexdataPositionX(i);
		btScalar vertY = DBPro::GetVertexdataPositionY(i);
		btScalar vertZ = DBPro::GetVertexdataPositionZ(i);
		if( vertY > 0)
		{
			DBPro::SetVertexdataPosition( i, vertX, vertY + height, vertZ);
		}
		else
		{
			DBPro::SetVertexdataPosition( i, vertX, vertY - height, vertZ);
		}
	}
	DBPro::UnlockVertexdata();
	//Remake the object to get rid of all rotations which makes it the correct axis capsule
	if(axis == Z_AXIS || X_AXIS)
	{
		int tempMesh = DBPro::FindFreeMesh();
		DBPro::MakeMeshFromObject(tempMesh, tempSphere);
		DBPro::DeleteObject(tempSphere);
		int capsuleObj = DBPro::FindFreeObject();
		DBPro::MakeObject( capsuleObj,tempMesh,0);
		DBPro::DeleteMesh(tempMesh);
		return capsuleObj;
	}
	return tempSphere;
}

void DBProRagDollBone::CreateBone()
{
	btScalar scaleFactor = 40.0f;///DynamicsWorldArray[0]->m_scaleFactor;
	btVector3 ObjectPosition(DBPro::ObjectPositionX(dbproObjectID), DBPro::ObjectPositionY(dbproObjectID),
										   DBPro::ObjectPositionZ(dbproObjectID));
	btVector3 jointVec1(DBPro::LimbPositionX(dbproObjectID,dbproStartLimbID), DBPro::LimbPositionY(dbproObjectID,dbproStartLimbID), 
								   DBPro::LimbPositionZ(dbproObjectID,dbproStartLimbID));
	btVector3 jointVec2(DBPro::LimbPositionX(dbproObjectID,dbproEndLimbID), DBPro::LimbPositionY(dbproObjectID,dbproEndLimbID), 
								   DBPro::LimbPositionZ(dbproObjectID,dbproEndLimbID));

	//get distance between models joints
	btVector3 resultVec;
	resultVec = jointVec1 - jointVec2;

	//Get a vector which represents a line from jointVec1 to jointVec2
	//Then store this vector to calculate a min/max angle for the joint in add joint.
	btVector3 boneVec;
	boneVec = resultVec;
	boneNormVec = boneVec.normalize();

	btScalar height = resultVec.length();
	height = height * lengthmod;
	//make capsule height from distance between joints

	//Capsule is made along the Z axis with zero rotations
	///dbproRagDollBoneID = DBProPrimatives::CreateCapsule(diameter, height, Axis::Z_AXIS);
	dbproRagDollBoneID = DBProPrimitiveCreateCapsule(diameter, height, Z_AXIS);

	//Store the Total volume for mass calculations
	boneVolume = diameter * diameter * height;
	//Divide distance to get center to position capsule
	resultVec = resultVec / 2;
	btVector3 positionVec;
	positionVec = jointVec1 - resultVec;
	DBPro::PositionObject(dbproRagDollBoneID, positionVec.getX(), positionVec.getY(), positionVec.getZ());

	//Rotate capsule to point at second joint, now capsule has rotations.
	DBPro::PointObject(dbproRagDollBoneID, jointVec2.getX(), jointVec2.getY(), jointVec2.getZ());

	m_collisionShape = new btCapsuleShapeZ(btScalar(diameter/scaleFactor/2),btScalar((height - diameter)/scaleFactor));

	//Create a World Transform for the bone
	btTransform boneTrans;
	boneTrans.setIdentity();
	boneTrans.setOrigin(positionVec/scaleFactor);

	//Using a matrix it takes it ZYX
	btMatrix3x3 boneRotation;
	boneRotation.setEulerYPR(btScalar(btRadians(DBPro::ObjectAngleZ(dbproRagDollBoneID))),
											 btScalar(btRadians(DBPro::ObjectAngleY(dbproRagDollBoneID))),
											 btScalar(btRadians(DBPro::ObjectAngleX(dbproRagDollBoneID))));
	boneTrans.setBasis(boneRotation);

	rigidBody = localCreateRigidBody(btScalar(1.0), boneTrans, m_collisionShape, dbproRagDollBoneID,collisionGroup, collisionMask);
	initialRotation = rigidBody->getWorldTransform().getBasis();

	//This line was for debug only
	//DBPro::HideObject(dbproRagDollBoneID);
	DBPro::DeleteObject(dbproRagDollBoneID);
	dbproRagDollBoneID = 0;
}

btRigidBody* DBProRagDollBone::GetRigidBody()
{
	return rigidBody;
}

int DBProRagDollBone::GetStartLimbID()
{
	return dbproStartLimbID;
}
int DBProRagDollBone::GetEndLimbID()
{
	return dbproEndLimbID;
}
int DBProRagDollBone::GetObjectID()
{
	return dbproObjectID;
}
int DBProRagDollBone::GetRagDollBoneID()
{
	return dbproRagDollBoneID;
}

btVector3  DBProRagDollBone::GetNormilizedVector()
{
	return boneNormVec;
}

btRigidBody* DBProRagDollBone::localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask)
{
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic)
	{
		shape->calculateLocalInertia(mass,localInertia);
	}
	DBProMotionState* myMotionState = new DBProMotionState(startTransform, objID);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	//add the body to the dynamics world
	g_dynamicsWorld->addRigidBody(body, collisionGroup, collisionMask);
	return body;
}

void DBProRagDollBone::AddDBProLimbID(int dbproLimbID)
{
	btScalar scaleFactor = 40.0f;///DynamicsWorldArray[0]->m_scaleFactor;
	dbproLimbIDs.push_back(dbproLimbID);

	//Get limb world Position
	btVector3 limbWorldPosition(btScalar(DBPro::LimbPositionX(GetObjectID(), dbproLimbID)), 
												 btScalar(DBPro::LimbPositionY(GetObjectID(), dbproLimbID)), 
												 btScalar(DBPro::LimbPositionZ(GetObjectID(), dbproLimbID)));
	limbWorldPosition = limbWorldPosition/scaleFactor;

	btVector3 limbOffset = limbWorldPosition - GetRigidBody()->getWorldTransform().getOrigin();

	//Store the Vector rotated
	limbOffsets.push_back(limbOffset * GetRigidBody()->getWorldTransform().getBasis());


	btVector3 objectTranslation = btVector3(DBPro::ObjectPositionX(GetObjectID()),
													DBPro::ObjectPositionY(GetObjectID()),
													DBPro::ObjectPositionZ(GetObjectID()));
	objectTranslation = objectTranslation/scaleFactor;
	ceterOfObjectOffsets.push_back(limbWorldPosition - objectTranslation);
}



