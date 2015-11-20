
// BulletPhysicsWrapper for DarkBasic Proffessional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

#pragma once

///#include "stdafx.h"
#include "btBulletDynamicsCommon.h"
#include "DBPro.hpp"

///Used to interpolate DBpro objects 
class DBProMotionState : public btMotionState
{
public:
	DBProMotionState(const btTransform &initialpos, int objID);
	~DBProMotionState(void);
	int GetObjID();
	void SetObjID(int objID);
	btScalar GetMaxLinearVelocity();
	void SetMaxLinearVelocity(btScalar maxLinearVelocity);
	virtual void getWorldTransform(btTransform &worldTrans) const ;
	virtual void setWorldTransform(const btTransform &worldTrans);
	static void setWorldTransform(int objectID, const btTransform &worldTrans);
	static void setWorldTransform(int objectID, const btTransform &worldTrans, btVector3& initialRotation);
protected:
	int m_objID;
	btScalar m_maxLinearVelocity;
    btTransform m_Pos1;
};

