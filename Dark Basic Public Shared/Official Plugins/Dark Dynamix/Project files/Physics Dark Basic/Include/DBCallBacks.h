#pragma once
#include "Physics.h"
#ifdef COMPILE_FOR_GDK        
   //extern GlobStruct* g_pGlob;
   //#include "DarkGDK.h"
#else                        
   //#include "DarkPhy2(dbpro).h"
#endif

//class UserNotifyCallback : public NxUserNotify
//{
//public:
//	bool onJointBreak();
//	std::vector<int> broken
//}

class Trigger
{
public:
    NxShape *triggerShape_;
	NxShape *otherShape_;
	NxTriggerFlag status_;
};

class MyTriggerCallback : public NxUserTriggerReport    
{   
public:
	void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
    int getTriggerData();
	Trigger* currentTrigger_;
	std::vector<Trigger*> triggerVec_;
};


class myRaycastReport : public NxUserRaycastReport    {
public: 
	virtual bool onHit(const NxRaycastHit& hits);
	int getHitData();
	NxRaycastHit* currentHit_;
	std::vector<NxRaycastHit*> hitVec_;
};


class Contact{
public:
    NxContactPair pair_;
	unsigned int type_;
};

class MyContactReport : public NxUserContactReport    {
public:
	void onContactNotify(NxContactPair& pair, NxU32 events);
	int getContactData();
	std::vector<Contact*> contactVec_;
	Contact* currentContact_;
};

class MyCContactReport : public NxUserControllerHitReport  {
public:
	MyCContactReport():collectControllerData_(false), collectShapeData_(false),
	                   currentShapeHit_(0), currentControllerHit_(0){}
	~MyCContactReport(){}

	NxControllerAction onControllerHit(const NxControllersHit& hit);
	NxControllerAction onShapeHit(const NxControllerShapeHit& hit);
	std::vector<NxControllerShapeHit*> shapeHitVec_;
	std::vector<NxControllersHit*> controllerHitVec_;

    NxControllerShapeHit *currentShapeHit_;
	NxControllersHit *currentControllerHit_;
	
	int getShapeHitData();
	int getControllerHitData();

	bool collectControllerData_;
	bool collectShapeData_;
};


class DBCallBacks
{
public:
	DBCallBacks(void);
	~DBCallBacks(void);


};
