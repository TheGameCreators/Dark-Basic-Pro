
///#include "StdAfx.h"
#include "DBProMotionState.h"

///takes the transformed inital position, and the DBPro obj number
DBProMotionState::DBProMotionState(const btTransform &initialpos, int objID)
{
	m_objID = objID;
	m_Pos1 = initialpos;
	m_maxLinearVelocity = 30.0;
}

DBProMotionState::~DBProMotionState(void)
{
}

int DBProMotionState::GetObjID()
{
	return m_objID;
}

void DBProMotionState::SetObjID(int objID)
{
	m_objID = objID;
}

void DBProMotionState::getWorldTransform(btTransform &worldTrans) const 
{
        worldTrans = m_Pos1;
}

void DBProMotionState::setWorldTransform(const btTransform &worldTrans) 
{	
	btScalar scaleFactor = 40.0f;///DynamicsWorldArray[0]->m_scaleFactor;
    if(DBPro::ObjectExist(m_objID) == false)
        return; 
	btScalar yaw, pitch, roll;

	btMatrix3x3 btMatrix = worldTrans.getBasis();
	//This returns ZYX not yaw pitch roll which is in the call tip
	btMatrix.getEulerYPR(yaw, pitch, roll);

	//Convert to degress for DBpro
	btScalar zAngle = float(btDegrees(yaw));
	btScalar yAngle = float(btDegrees(pitch));
	btScalar xAngle = float(btDegrees(roll));
	
    DBPro::RotateObject(m_objID,xAngle, yAngle, zAngle);
    btVector3 pos = worldTrans.getOrigin();
    DBPro::PositionObject(m_objID,pos.x()*scaleFactor, pos.y()*scaleFactor, pos.z()*scaleFactor);
 }

//Called by The DBPro Character Controller
void DBProMotionState::setWorldTransform(int objectID, const btTransform &worldTrans, btVector3& initialRotation) 
{	
	btScalar scaleFactor = 40.0;///DynamicsWorldArray[0]->m_scaleFactor;
    if(DBPro::ObjectExist(objectID) == false)
        return; 
	btScalar yaw, pitch, roll;

	btMatrix3x3 btMatrix = worldTrans.getBasis();
	//This returns ZYX not yaw pitch roll which is in the call tip
	btMatrix.getEulerYPR(yaw, pitch, roll);

	//Convert to degress for DBpro
	btScalar zAngle = btDegrees(yaw);
	btScalar yAngle = btDegrees(pitch);
	btScalar xAngle = btDegrees(roll);
	
    DBPro::RotateObject(objectID,xAngle + initialRotation.getX(), yAngle + initialRotation.getY(), zAngle + initialRotation.getZ());

    btVector3 pos = worldTrans.getOrigin();
    DBPro::PositionObject(objectID,pos.x()*scaleFactor, pos.y()*scaleFactor, pos.z()*scaleFactor);
 }

void DBProMotionState::setWorldTransform(int objectID, const btTransform &worldTrans) 
{
	DBProMotionState::setWorldTransform(objectID, worldTrans, btVector3(0.0,0.0,0.0));
}

btScalar DBProMotionState::GetMaxLinearVelocity()
{
	return m_maxLinearVelocity;
}
void DBProMotionState::SetMaxLinearVelocity(btScalar maxLinearVelocity)
{
	m_maxLinearVelocity = maxLinearVelocity;
}
