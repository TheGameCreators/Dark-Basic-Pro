#include "DBCallBacks.h"


//bool UserNotifyCallback::onJointBreak(){
//    
//	return false;
//}

void MyTriggerCallback::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{     
    Trigger *trigger = new Trigger();
	trigger->otherShape_ = &otherShape;
	trigger->triggerShape_ = &triggerShape;
	trigger->status_ = status;
	this->triggerVec_.push_back(trigger);
}  

int MyTriggerCallback::getTriggerData(){
	int size = this->triggerVec_.size();
	if(size){
		if(this->currentTrigger_){
			delete this->currentTrigger_;
		}
	    this->currentTrigger_ = this->triggerVec_.back();
	    this->triggerVec_.pop_back();
	}
	else{
		this->currentTrigger_ = 0;
	}
	return size;
}



bool myRaycastReport::onHit(const NxRaycastHit& hits){
	NxRaycastHit *hit = new NxRaycastHit();
	*hit = hits;
	this->hitVec_.push_back(hit);
	return true;
}

int myRaycastReport::getHitData(){
	int size = this->hitVec_.size();
	if(size){
		if(this->currentHit_){
			delete this->currentHit_;
		}
	    this->currentHit_ = this->hitVec_.back();
	    this->hitVec_.pop_back();
	}
	else{
		this->currentHit_ = 0;
	}
	return size;
}

void MyContactReport::onContactNotify(NxContactPair& pair, NxU32 events){ 
	Contact* c = new Contact();
	c->pair_ = pair;
	c->type_ = events;
	this->contactVec_.push_back(c);        
}  

int MyContactReport::getContactData(){
	int size = this->contactVec_.size();
	if(size){
		if(this->currentContact_){
			delete this->currentContact_;
		}
	    this->currentContact_ = this->contactVec_.back();
	    this->contactVec_.pop_back();
	}
	else{
		this->currentContact_ = 0;
	}
	return size;
}



NxControllerAction MyCContactReport::onControllerHit(const NxControllersHit& hit){
	if(this->collectControllerData_){
	   NxControllersHit *aHit = new NxControllersHit();
	   *aHit = hit;
	   this->controllerHitVec_.push_back(aHit);
	}
    return NX_ACTION_NONE;
}

NxControllerAction MyCContactReport::onShapeHit(const NxControllerShapeHit& hit){
	if(this->collectShapeData_){
	   NxControllerShapeHit *aHit = new NxControllerShapeHit();
	   *aHit = hit;
	   this->shapeHitVec_.push_back(aHit);
	}

	//NxActor *actor = &hit.shape->getActor();
	//this->shapeHitVec_.push_back(hit);
	//if(actor->isDynamic()){
	    //NxVec3 force = hit.dir * hit.length * 10000;
	//    NxVec3 force = hit.dir * 10000;
	//    actor->addForce(force);
	//}
    return NX_ACTION_PUSH;
}

int MyCContactReport::getShapeHitData(){
	int size = this->shapeHitVec_.size();
	if(size){
		if(this->currentShapeHit_){
			delete this->currentShapeHit_;
		}
	    this->currentShapeHit_ = this->shapeHitVec_.back();
	    this->shapeHitVec_.pop_back();
	}
	else{
		this->currentShapeHit_ = 0;
	}
	return size;
}

int MyCContactReport::getControllerHitData(){
	int size = this->controllerHitVec_.size();
	if(size){
		if(this->currentControllerHit_){
			delete this->currentControllerHit_;
		}
	    this->currentControllerHit_ = this->controllerHitVec_.back();
	    this->controllerHitVec_.pop_back();
	}
	else{
		this->currentControllerHit_ = 0;
	}
	return size;
}





DBCallBacks::DBCallBacks(void)
{
}

DBCallBacks::~DBCallBacks(void)
{
}
