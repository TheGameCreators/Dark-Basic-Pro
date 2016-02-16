#pragma once
#ifndef DARKPHY2CPP
#define DARKPHY2CPP

#include "DarkPhy2.h"
#include "DBPhyStruct.h"

#ifndef COMPILE_FOR_GDK
  #define bool DWORD
#endif

class LoadUserNotify : public NXU_userNotify
{
    public:
	std::vector<NxActor*> actorVec;
    NxSoftBodyMesh* sbm;
	
	//
	//param actor  A pointer to the 'NxActor' that has been created.
	//param userProperties An optional ASCIIZ string that contains arbitrary 'user properties' defined in the source data.  This pointer is persistent even after the NxuPhysicsCollection has been released.
	//note  Since there are not discrete callbacks for shapes, the user can iterate on the shapes in the actor desc and cast the 'userData' pointer to a 'const char *' to process the 'userProperties' field for each shape.
	//
	virtual void NXU_notifyActor(NxActor *actor, const char *userProperties){
		actor->userData = (void*)userProperties;
	    actorVec.push_back(actor);
	};

	virtual void NXU_preNotifySoftBodyMesh(NxSoftBodyMesh *t, const char *userProperties){
       sbm = t;

	};
	//JOINT

	//MESH
};

//For char* used in save callbacks
//May need to do same for load callbacks
std::vector<char*> charVec_;


class SaveUserNotify : public NXU_userNotify
{
    public:
	virtual void NXU_notifySaveActor(NxActor *actor, const char **pUserProperties){
	    ActorSync* aSync = 0;
		aSync = (ActorSync*)actor->userData;
		if(!aSync)		
			return;
		
	    char* type = new char[255];
		//put pointer away for deletion
		charVec_.push_back(type);

		string sType;
		if(aSync->type == 1){
			//dynamic object
			aSync = (ObjectFromActor*)aSync;
			sType = string("FP_DYNAMIC_OBJECT ");
		}
		else if(aSync->type == 2){
            //dynamic limb
			aSync = (LimbFromActor*)aSync;
			sType = string("FP_DYNAMIC_LIMB ");
	        stringstream out;
	        out << aSync->limb_;
			sType.append(out.str());
		}
		for(unsigned int i = 0; i < sType.size(); i++){
			type[i] = sType.at(i);
		}
		type[sType.size()] = '\0';
	    *pUserProperties = type;
	};
};


//Globals
//Static member for message class
int Message::errorFlagged = 0;
int Message::maxErrors = 3;
DBPhyStruct *phyGlobStruct_ = new DBPhyStruct();

/*
* @description On some occasions you may find that when creating an actor its scale does not match up with the model, you can set this value to adjust the scale of the created actor, remember you may need to set it back to the default(1.0) when you are done.
* @param scaleMultiplier - Value you wish to adjust scale by.
* @dbpro DYN SET SCALE MULTIPLIER
* @category CORE
*/
void dynSetScaleMultiplier(float scaleMultiplier){
	phyGlobStruct_->phyScaleMultiplier_ = scaleMultiplier;
}

/*
* @description On some occasions you may find that when creating an actor its scale does not match up with the model, you can set this value to adjust the scale of the created actor, remember you may need to set it back to the default(1.0) when you are done.
* @param scaleMultiplier - Value you wish to adjust scale by.
* @dbpro DYN GET SCALE MULTIPLIER
* @category CORE
*/
DWORDFLOAT dynGetScaleMultiplier(){
	float value = phyGlobStruct_->phyScaleMultiplier_;
    RETURNCAST
}

/*
* @description Initialises physics and creates default scene.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN START
* @category CORE
*/
bool dynStart(void) {
	return dynStart(false);
	//if(phyGlobStruct_ == 0){	
	//   phyGlobStruct_ = new DBPhyStruct();
	//}

 //   #ifdef DEBUG
	//	if(phyGlobStruct_->phy_){
	//		Message::startCalledError();
	//		return false;
	//	}
	//#endif
	//    phyGlobStruct_->phy_ = new Physics();
	//	bool result = phyGlobStruct_->phy_->start();
	//	if(result){			
 //           //Note: DBPro d3d device needs to be initialized
	//	    phyGlobStruct_->d3dUtil_ = new D3DUtil(dbGetDirect3DDevice(), phyGlobStruct_->phy_->getDebugRenderable());
	//		//Mouse
	//		phyGlobStruct_->mDrag_ = SOFTBODY::createMouseDrag(phyGlobStruct_->phy_->physicsSDK_, phyGlobStruct_->phy_->currentScene_, dbScreenWidth(), dbScreenHeight()); 		
	//	
	//		if(phyGlobStruct_->contact_)
	//		    phyGlobStruct_->phy_->currentScene_->setUserContactReport(phyGlobStruct_->contact_);
	//		if(phyGlobStruct_->triggerCallback_){
	//			phyGlobStruct_->phy_->currentScene_->setUserTriggerReport(phyGlobStruct_->triggerCallback_);
	//		}
	//	}
	//	else{
	//		if(phyGlobStruct_->phy_){
	//			delete phyGlobStruct_->phy_;
	//			phyGlobStruct_->phy_ = 0;
	//		}
	//		if(phyGlobStruct_){
 //      		    delete phyGlobStruct_;
	//			phyGlobStruct_ = 0;
	//		}
	//	}
	//	return result;
}


/*
* @description Allows you to check whether hardware acceleration is available.
* @return TRUE if available, FALSE otherwise.
* @dbpro DYN HARDWARE PRESENT
* @category HARDWARE
*/
bool dynHardwarePresent() 
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	return phyGlobStruct_->phy_->physicsSDK_->getHWVersion() != NX_HW_VERSION_NONE;
}


/*
* @description Initialises physics and creates default scene. This overload can also enables hardware acceleration.
* @param enableHardware - Set to TRUE to enable hardware acceleration, FALSE otherwise.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN START
* @category CORE
*/
bool dynStart(bool enableHardware)
{
#ifdef COMPILE_WITH_TIME_LIMIT
    MessageBox(NULL,(LPCSTR)"Dark Dynamix will stop updating after 5 minutes.", (LPCSTR)"Time limited version", MB_OK);
#endif
	if(phyGlobStruct_ == 0){	
	   phyGlobStruct_ = new DBPhyStruct();
	}

    #ifdef DEBUG
		if(phyGlobStruct_->phy_){
			Message::startCalledError();
			return false;
		}
	#endif
	    phyGlobStruct_->phy_ = new Physics();
		bool result = phyGlobStruct_->phy_->start(enableHardware);
		if(result){			
            //Note: DBPro d3d device needs to be initialized
		    phyGlobStruct_->d3dUtil_ = new D3DUtil(dbGetDirect3DDevice(), phyGlobStruct_->phy_->getDebugRenderable());
			//Mouse
			phyGlobStruct_->mDrag_ = SOFTBODY::createMouseDrag(phyGlobStruct_->phy_->physicsSDK_, phyGlobStruct_->phy_->currentScene_, dbScreenWidth(), dbScreenHeight()); 		
		
			if(phyGlobStruct_->contact_)
			    phyGlobStruct_->phy_->currentScene_->setUserContactReport(phyGlobStruct_->contact_);
			if(phyGlobStruct_->triggerCallback_){
				phyGlobStruct_->phy_->currentScene_->setUserTriggerReport(phyGlobStruct_->triggerCallback_);
			}
		}
		else{
			if(phyGlobStruct_->phy_){
				delete phyGlobStruct_->phy_;
				phyGlobStruct_->phy_ = 0;
			}
			if(phyGlobStruct_){
       		    delete phyGlobStruct_;
				phyGlobStruct_ = 0;
			}
		}
		return result;
}


/*
* @description Checks if the given material exists.
* @param ID - ID number of the material.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN MATERIAL EXISTS
* @category CONSTRUCTION
*/
bool dynMaterialExists(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->materialMap_.count(ID);
}

/*
* @description Sets the shape space direction (unit vector) of anisotropy. This is used when anisotropic friction is being applied. I.e. the NX_MF_ANISOTROPIC flag is set.
* @param ID - ID number of the material.
* @param x - Direction along the x axis.
* @param y - Direction along the y axis.
* @param z - Direction along the z axis.
* @dbpro DYN MATERIAL SET DIR OF ANISOTROPY
* @category MATERIAL
*/
void dynMaterialSetDirOfAnisotropy(int ID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}

	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		return phyGlobStruct_->materialMap_[ID]->setDirOfAnisotropy(NxVec3(x, y, z));
}

/*
* @description Retrieves the direction of anisotropy value. 
* @param ID - ID number of the material.
* @param vec3Out - ID number of the vector3 into which the values will be placed.
* @dbpro DYN MATERIAL GET DIR OF ANISOTROPY
* @category MATERIAL
*/
void dynMaterialGetDirOfAnisotropy(int ID, int vec3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}

	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		NxVec3 vec =  phyGlobStruct_->materialMap_[ID]->getDirOfAnisotropy();
		dbSetVector3(vec3Out, vec.x, vec.y, vec.z);
}

/*
* @description Sets the coefficient of dynamic friction. The coefficient of dynamic friction should be in [0, +inf]. If set to greater than staticFriction, the effective value of staticFriction will be increased to match. If the flag NX_MF_ANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)
* @param ID - ID number of the material.
* @param value - Coefficient of dynamic friction. Range: [0, +inf] 
* @dbpro DYN MATERIAL SET DYNAMIC FRICTION
* @category MATERIAL
*/
void dynMaterialSetDynamicFriction(int ID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		phyGlobStruct_->materialMap_[ID]->setDynamicFriction(value);
}

/*
* @description Retrieves the Dynamic Friction value. 
* @param ID - ID number of the material.
* @return Dynamic friction value.
* @dbpro DYN MATERIAL GET DYNAMIC FRICTION
* @category MATERIAL
*/
DWORDFLOAT dynMaterialGetDynamicFriction(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return 0;
		}
		float value = phyGlobStruct_->materialMap_[ID]->getDynamicFriction();
		RETURNCAST
}

/*
* @description Sets the dynamic friction coefficient along the secondary (V) axis. This is used when anisotropic friction is being applied. I.e. the NX_MF_ANISOTROPIC flag is set.
* @param ID - ID number of the material.
* @param value - Coefficient of dynamic friction in the V axis. Range: [0, +inf] 
* @dbpro DYN MATERIAL SET DYNAMIC FRICTION V
* @category MATERIAL
*/
void dynMaterialSetDynamicFrictionV(int ID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		phyGlobStruct_->materialMap_[ID]->setDynamicFrictionV(value);
}

/*
* @description Retrieves the dynamic friction coefficient for the V direction.
* @param ID - ID number of the material.
* @return The coefficient if dynamic friction in the V direction.
* @dbpro DYN MATERIAL GET DYNAMIC FRICTION V
* @category MATERIAL
*/
DWORDFLOAT dynMaterialGetDynamicFrictionV(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return 0;
		}
		float value = phyGlobStruct_->materialMap_[ID]->getDynamicFrictionV();
		RETURNCAST
}













/*
* @description Sets the coefficient of static friction. The coefficient of static friction should be in the range [0, +inf] if flags & NX_MF_ANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)
* @param ID - ID number of the material.
* @param value - Coefficient of static friction. Range: [0, +inf] 
* @dbpro DYN MATERIAL SET STATIC FRICTION
* @category MATERIAL
*/
void dynMaterialSetStaticFriction(int ID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		phyGlobStruct_->materialMap_[ID]->setStaticFriction(value);
}

/*
* @description Retrieves the Static Friction value. 
* @param ID - ID number of the material.
* @return Static friction value.
* @dbpro DYN MATERIAL GET STATIC FRICTION
* @category MATERIAL
*/
DWORDFLOAT dynMaterialGetStaticFriction(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return 0;
		}
		float value = phyGlobStruct_->materialMap_[ID]->getStaticFriction();
		RETURNCAST
}

/*
* @description Sets the static friction coefficient along the secondary (V) axis. This is used when anisotropic friction is being applied. I.e. the NX_MF_ANISOTROPIC flag is set.
* @param ID - ID number of the material.
* @param value - Coefficient of static friction in the V axis. Range: [0, +inf] 
* @dbpro DYN MATERIAL SET STATIC FRICTION V
* @category MATERIAL
*/
void dynMaterialSetStaticFrictionV(int ID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		phyGlobStruct_->materialMap_[ID]->setStaticFrictionV(value);
}

/*
* @description Retrieves the static friction coefficient for the V direction.
* @param ID - ID number of the material.
* @return The coefficient if static friction in the V direction.
* @dbpro DYN MATERIAL GET STATIC FRICTION V
* @category MATERIAL
*/
DWORDFLOAT dynMaterialGetStaticFrictionV(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return 0;
		}
		float value = phyGlobStruct_->materialMap_[ID]->getStaticFrictionV();
		RETURNCAST
}

/*
* @description Sets the flags, a combination bits. i.e. the NX_MF_ANISOTROPIC flag.
* @param ID - ID number of the material.
* @param flags - Flag combination: NX_MF_ANISOTROPIC = 1, NX_MF_DISABLE_FRICTION = 16, NX_MF_DISABLE_STRONG_FRICTION = 32
* @dbpro DYN MATERIAL SET FLAGS
* @category MATERIAL
*/
void dynMaterialSetFlags(int ID, unsigned int flags){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return;
		}
		phyGlobStruct_->materialMap_[ID]->setFlags(flags);
}

/*
* @description Gets the flags, a combination bits.
* @param ID - ID number of the material.
* @return Flag combination. A combination of: NX_MF_ANISOTROPIC = 1, NX_MF_DISABLE_FRICTION = 16, NX_MF_DISABLE_STRONG_FRICTION = 32
* @dbpro DYN MATERIAL GET FLAGS
* @category MATERIAL
*/
unsigned int dynMaterialGetFlags(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return 0;
		}
		return phyGlobStruct_->materialMap_[ID]->getFlags();
}

/*
* @description The index of the material can be retrieved using this function. Materials are associated with mesh faces and shapes using 16 bit identifiers. If you release a material while its material ID is still in use by shapes or meshes, the material usage of these objects becomes undefined as the material index gets recycled.
* @param ID - ID number of the material.
* @return Material index value.
* @dbpro DYN MATERIAL GET MATERIAL INDEX
* @category MATERIAL
*/
int dynMaterialGetMaterialIndex(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		//check here as no error message
		if(!phyGlobStruct_->materialMap_.count(ID)){
            return -1;
		}
		return phyGlobStruct_->materialMap_[ID]->getMaterialIndex();
}



/*
* @description Makes the given actor kinematic, a kinematic actor is moved directly by the user and is good for things like moving platforms or characters where direct control is needed.
* @param objectID - ID number of the object/actor.
* @dbpro DYN KINEMATIC ON
* @category KINEMATIC
*/
void dynKinematicOn(int objectID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynKinematicOn");
            return;
		}
	#endif
	NxActor *actor = 0;
	actor = phyGlobStruct_->actorMap_[objectID];
	actor->raiseBodyFlag(NX_BF_KINEMATIC);
}

/*
* @description Makes the given actor non-kinematic, a kinematic actor is moved directly by the user and is good for things like moving platforms or characters where direct control is needed.
* @param objectID - ID number of the object/actor.
* @dbpro DYN KINEMATIC OFF
* @category KINEMATIC
*/
void dynKinematicOff(int objectID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynKinematicOff");
            return;
		}
	#endif
	NxActor *actor = 0;
	actor = phyGlobStruct_->actorMap_[objectID];
	actor->clearBodyFlag(NX_BF_KINEMATIC);
}

/*
* @description If an actor has been made kinematic then you may use this command to position the actor.
* @param objectID - ID number of the object/actor.
* @param posX - Global position along the x axis.
* @param posY - Global position along the y axis.
* @param posZ - Global position along the z axis.
* @dbpro DYN KINEMATIC MOVE POS
* @category KINEMATIC
*/
void dynKinematicMovePos(int objectID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynKinematicMovePos");
            return;
		}
	#endif
	NxActor *actor = 0;
	actor = phyGlobStruct_->actorMap_[objectID];
	actor->moveGlobalPosition(NxVec3(posX, posY, posZ));

}

/*
* @description If an actor has been made kinematic then you may use this command to rotate the actor.
* @param objectID - ID number of the object/actor.
* @param rotX - Global orientation about the x axis.
* @param royY - Global orientation about the y axis.
* @param rotZ - Global orientation about the z axis.
* @dbpro DYN KINEMATIC MOVE ROT
* @category KINEMATIC
*/
void dynKinematicMoveRot(int objectID, float rotX, float rotY, float rotZ){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynKinematicMoveRot");
            return;
		}
	#endif
	NxActor *actor = 0;
	actor = phyGlobStruct_->actorMap_[objectID];
    NxVec3 eulerAngle = NxVec3(rotX,rotY,rotZ);

    // Quaternion
    NxQuat q1, q2, q3;
    q1.fromAngleAxis(eulerAngle.x, NxVec3(1,0,0));
    q2.fromAngleAxis(eulerAngle.y, NxVec3(0,1,0));
    q3.fromAngleAxis(eulerAngle.z, NxVec3(0,0,1));

    // Orientation matrix
    NxMat33 orient1, orient2, orient3;
    orient1.fromQuat(q1);
    orient2.fromQuat(q2);
    orient3.fromQuat(q3);

    NxMat33 orient;
    orient = orient3*orient2*orient1;  // Use global axes
	actor->moveGlobalOrientation(orient);
}



/*
* @description Creates a box shape using the object given. The shape is added to an internal shape list awaiting the creation of a compound actor. Unlike other 'make' commands this does not sync the object with the shape, the object is free to be deleted after the shape is created, an object is only assigned to these shapes when the compound actor is created. 
* @param objectID - ID number of the object the shape will be created from.
* @param density - Density value for the shape.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE SHAPE BOX
* @category CONSTRUCTION
*/
bool dynMakeShapeBox(int objectID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeShapeBox")){
            return false; 
		}
	#endif
	return phyGlobStruct_->phy_->makeShapeBox(DBUtil::getMatrixFromObject(objectID),
			                                  DBUtil::getObjectDimensions(objectID) * phyGlobStruct_->phyScaleMultiplier_,
			                                  density);
}

/*
* @description Creates a sphere shape using the object given. The shape is added to an internal shape list awaiting the creation of a compound actor. Unlike other 'make' commands this does not sync the object with the shape, the object is free to be deleted after the shape is created, an object is only assigned to these shapes when the compound actor is created. 
* @param objectID - ID number of the object the shape will be created from.
* @param density - Density value for the shape.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE SHAPE SPHERE
* @category CONSTRUCTION
*/
bool dynMakeShapeSphere(int objectID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeShapeSphere")){
            return false; 
		}
	#endif
	NxVec3 dim = DBUtil::getObjectDimensions(objectID);
	//Radius = average of all dimensions
	float radius = (dim.x + dim.y + dim.z) / 3; 
	return phyGlobStruct_->phy_->makeShapeSphere(DBUtil::getMatrixFromObject(objectID),
		                                         radius,
			                                     density);
}

/*
* @description Creates a capsule shape using the object given. The shape is added to an internal shape list awaiting the creation of a compound actor. Unlike other 'make' commands this does not sync the object with the shape, the object is free to be deleted after the shape is created, an object is only assigned to these shapes when the compound actor is created. 
* @param objectID - ID number of the object the shape will be created from.
* @param density - Density value for the shape.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE SHAPE CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeShapeCapsule(int objectID, float height, float radius, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeShapeCapsule")){
            return false; 
		}
	#endif
	height *= phyGlobStruct_->phyScaleMultiplier_;
	radius *= phyGlobStruct_->phyScaleMultiplier_;
	return phyGlobStruct_->phy_->makeShapeCapsule(DBUtil::getMatrixFromObject(objectID),
		                                         height,
		                                         radius,
			                                     density);
}

/*
* @description Creates an actor using all the shapes held in an internal shape list which you add to using the 'dynMakeShape' commands. For example, if you have a model of a table which you desire to be dynamic you would create four leg objects and a surface object and match them to your model the best you can, you then create shapes using these temp objects, finally, create the compound actor assigning it to your table model.
* @param objectID - ID number of the object the compound actor will represent in the simulation.
* @param density - Density value for the object.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE COMPOUND FROM SHAPES
* @category CONSTRUCTION
*/
bool dynMakeCompoundFromShapes(int objectID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeCompoundFromShapes")){
            return false; 
		}
		if(phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorAlreadyExist(objectID, "dynMakeCompoundFromShapes");
            return false;
		}
	#endif
		ObjectFromActor *aSync = new ObjectFromActor(objectID);
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeCompoundActor(aSync,
			                 DBUtil::getMatrixFromObject(objectID),
			                 density);
		if(actor){
			phyGlobStruct_->actorMap_[objectID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}

/*
* @description Clears all shapes held in the internal shape list, this shape list is used for the creation of compound actors(actors with multiple shapes). 
* @dbpro DYN CLEAR SHAPES
* @category CONSTRUCTION
*/
void dynClearShapes(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->clearShapes();
}


/*
* @description Releases the ground plane from the simulation. 
* @dbpro DYN DELETE GROUND PLANE
* @category CONSTRUCTION
*/
void dynDeleteGroundPlane(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	phyGlobStruct_->phy_->releaseGroundPlane();
}







/*
* @description Helper function to give you quick control of your actors using the mouse pointer.
* @param cameraID - ID number of the camera you are using.
* @param screenX - You will usually put the x coordinate of your mouse pointer here.
* @param screenY - You will usually put the y coordinate of your mouse pointer here.
* @param flag - Defines what type of actors you wish to drag: none-0 static-1 dynamic-2 cloth-4 softbody-8 all-15
* @dbpro DYN PICK
* @category PICK
*/
void dynPick(int cameraID, int screenX, int screenY, int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	D3DXMATRIX projMat = dbGetProjectionMatrix(cameraID);
	D3DXMATRIX viewMat = dbGetViewMatrix(cameraID);
	phyGlobStruct_->dInfo_ = SOFTBODY::mouseDrag(phyGlobStruct_->mDrag_, (const float*)viewMat.m, (const float*)projMat.m, screenX, screenY, (SOFTBODY::NxDragType)flag, 1 / 60.0f);

}

/*
* @description When you use the drag commands a ray is cast, you can use this command to retrieve the ray direction as a unit vector.
* @param vector3ID - ID number of the vector you wish to store the direction values in.
* @dbpro DYN PICK GET RAY DIRECTION
* @category PICK
*/
void dynPickGetRayDirection(int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
    if(phyGlobStruct_->dInfo_){
	   NxVec3 dir = phyGlobStruct_->dInfo_->mRay.dir;
	   dbSetVector3(vector3ID, dir.x, dir.y, dir.z);
	}
}

/*
* @description Helper function to give you quick control of your actors using the mouse pointer. This command will release any actor you are currently dragging.
* @dbpro DYN PICK DESELECT
* @category PICK
*/
void dynPickDeselect(){
  	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif  
	SOFTBODY::mouseDragDeselect(phyGlobStruct_->mDrag_);
}

/*
* @description Helper function to give you quick control of your actors using the mouse pointer. This command will push or pull the actor in and out of the screen.
* @param - You can put the current state of the mouse wheel here if desired.
* @dbpro DYN PICK DEPTH
* @category PICK
*/
void dynPickDepth(int delta){
  	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif 
	SOFTBODY::mouseDragWheel(phyGlobStruct_->mDrag_, delta);
}

/*
* @description Helper function to give you quick control of your actors using the mouse pointer. If you have changed the screen size you will need to call this function to update the drag functionality. This command may also be needed if you are using a second camera with different view dimensions.
* @param - You can put the current state of the mouse wheel here if desired.
* @dbpro DYN PICK SET SCREEN DIM
* @category PICK
*/
void dynPickSetScreenDim(int screenWidth, int screenHeight){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	if(phyGlobStruct_->mDrag_){
		SOFTBODY::releaseMouseDrag(phyGlobStruct_->mDrag_);
	}
    phyGlobStruct_->mDrag_ = SOFTBODY::createMouseDrag(phyGlobStruct_->phy_->physicsSDK_, phyGlobStruct_->phy_->currentScene_, screenWidth, screenHeight); 
}


/*
* @description Releases physics along with any scenes that have been created.
* @dbpro DYN STOP
* @category CORE
*/
void dynStop(void) {
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->release();  //releases memory and clears all maps act
		delete phyGlobStruct_;
		phyGlobStruct_ = 0;
}

/*
* @description Gets the current scene.
* @return ID number of the current scene.
* @dbpro DYN GET CURRENT SCENE
* @category CORE
*/
int dynGetCurrentScene(void){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
	    return phyGlobStruct_->phy_->getCurrentScene();
}

/*
* @description Sets the current scene.
* @param sceneID - ID number of the scene you want to set to the current scene.
* @dbpro DYN SET CURRENT SCENE 
* @category CORE
*/
void dynSetCurrentScene(int sceneID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	    phyGlobStruct_->phy_->setCurrentScene(sceneID);
}

/*
* @description Creates a new scene.
* @param sceneID - ID number you wish to assign to the new scene.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SCENE
* @category CONSTRUCTION
*/
bool dynMakeScene(int sceneID){
	return dynMakeScene(sceneID, false);
	//#ifdef DEBUG
	//	if(!phyGlobStruct_->phy_){
	//		Message::ignitionError();
	//		return false;
	//	}
	//#endif
	//    bool result = phyGlobStruct_->phy_->makeScene(sceneID);
	//	if(phyGlobStruct_->contact_){
	//		phyGlobStruct_->phy_->currentScene_->setUserContactReport(phyGlobStruct_->contact_);
	//	}
	//	if(phyGlobStruct_->triggerCallback_){
	//		phyGlobStruct_->phy_->currentScene_->setUserTriggerReport(phyGlobStruct_->triggerCallback_);
	//	}
	//	return result;
}

/*
* @description Creates a new scene with the option of creating a hardware scene.
* @param sceneID - ID number you wish to assign to the new scene.
* @param hardwareScene - TRUE will attempt to create a hardware scene, FALSE will create a software scene.
* @return TRUE if succeeded, FALSE otherwise.
* @category CONSTRUCTION
* @dbpro DYN MAKE SCENE
*/
bool dynMakeScene(int sceneID, bool hardwareScene){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	    bool result = phyGlobStruct_->phy_->makeScene(sceneID, hardwareScene);
		if(result){
			if(phyGlobStruct_->contact_){
				phyGlobStruct_->phy_->currentScene_->setUserContactReport(phyGlobStruct_->contact_);
			}
			if(phyGlobStruct_->triggerCallback_){
				phyGlobStruct_->phy_->currentScene_->setUserTriggerReport(phyGlobStruct_->triggerCallback_);
			}
		}
		return result;
}



/*
* @description Releases a scene, this also releases any actors belonging to this scene.
* @param sceneID - ID number of the scene.
* @dbpro DYN DELETE SCENE
* @category CONSTRUCTION
*/
void dynDeleteScene(int sceneID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	    phyGlobStruct_->phy_->releaseScene(sceneID);
}
	
/*
* @description Gets the total number of scenes in the simulation.
* @return Amount of scenes currently in the simulation.
* @dbpro DYN GET SCENE COUNT
* @category CORE
*/
int dynGetSceneCount(void){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
	    return phyGlobStruct_->phy_->getSceneCount();
}

/*
* @description Sets gravity for the current scene.
* @param x - Gravity along the x axis.
* @param Y - Gravity along the Y axis.
* @param Z - Gravity along the Z axis.
* @dbpro DYN SET GRAVITY
* @category CORE
*/
void dynSetGravity(float x, float y, float z){
 	#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
	#endif
	phyGlobStruct_->phy_->setGravity(x, y, z);
}

/*
* @description Creates a box actor.
* @param objectID - ID number of the object the box will represent in the simulation.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE BOX
* @category CONSTRUCTION
*/
bool dynMakeBox(int objectID, float density){
	#ifdef DEBUG
	if(!DBError::objectExists(objectID, "dynMakeBox"))
			return false;
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		ObjectFromActor *aSync = new ObjectFromActor(objectID);
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeBox(aSync,
			                 DBUtil::getMatrixFromObject(objectID),
							 DBUtil::getObjectDimensions(objectID) * phyGlobStruct_->phyScaleMultiplier_,
			                 density,
							 DBUtil::getCollisionCentre(objectID));
		if(actor){
			phyGlobStruct_->actorMap_[objectID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}


/*
* @description Creates a capsule actor.
* @param objectID - ID number of the object the capsule will represent in the simulation.
* @param height - The distance between the two hemispherical ends of the capsule.
* @param radius - The radius of the capsules hemispherical ends and its trunk.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeCapsule(int objectID, float height, float radius, float density){
	#ifdef DEBUG
	    if(!DBError::objectExists(objectID, "dynMakeCapsule"))
			return false;
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		ObjectFromActor *aSync = new ObjectFromActor(objectID);;
		NxActor *actor = 0;
		height *= phyGlobStruct_->phyScaleMultiplier_;
		radius *= phyGlobStruct_->phyScaleMultiplier_;
		actor = phyGlobStruct_->phy_->makeCapsule(aSync,
			                 DBUtil::getMatrixFromObject(objectID),
			                 height,
                             radius,
			                 density,
							 DBUtil::getCollisionCentre(objectID));
		if(actor){
			phyGlobStruct_->actorMap_[objectID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}


/*
* @description Creates a sphere actor.
* @param objectID - ID number of the object the sphere will represent in the simulation.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SPHERE
* @category CONSTRUCTION
*/
bool dynMakeSphere(int objectID, float density){
	#ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynMakeSphere"))
			return false;
	    //Check object exists
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		ObjectFromActor *aSync = new ObjectFromActor(objectID);;
		NxActor *actor = 0;
		NxVec3 dim = DBUtil::getObjectDimensions(objectID);
		//Radius = average of all dimensions
		float radius = (dim.x + dim.y + dim.z) / 3;  
		radius *= phyGlobStruct_->phyScaleMultiplier_;
		actor = phyGlobStruct_->phy_->makeSphere(aSync,
			                 DBUtil::getMatrixFromObject(objectID),
			                 radius,
			                 density,
							 DBUtil::getCollisionCentre(objectID));
		if(actor){
			phyGlobStruct_->actorMap_[objectID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}





void dynSaveScene(char* file){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
    NXU::NxuPhysicsCollection *collection;
	collection = NXU::createCollection();
	SaveUserNotify un;
	NXU::addEntireScene(*collection, *phyGlobStruct_->phy_->currentScene_, 0, 0, &un);
	NXU::saveCollection(collection, file, NXU::FT_XML);
	NXU::releaseCollection(collection);
	collection = 0;

	//delete char arrays 
	for(unsigned int i = 0; i < charVec_.size(); i++){
		delete []charVec_.at(i);
		charVec_.at(i) = 0;
	}
	charVec_.clear();
}

/*
* @description Setting the second parameter to TRUE will give the physics control of the limbs in the given object. It also returns the limbs back to animation control when the second parameter is set to FALSE.  
* @param objectID - ID number of the owning object.
* @param physics - TRUE will give limb control to physics simulation, FALSE will release the limbs from physics control.
* @dbpro DYN SET LIMBS
* @category RIGID BODY
*/
void dynSetLimbs(int objectID, bool physics){
    #ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynSetLimbs"))
			return;
	#endif
    sObject *obj = dbGetObject(objectID);
	for(int i = 0; i < dbLimbCount(objectID) - 1; i++){
	   obj->ppFrameList[i + 1]->bOverride = physics;
	}
}

/*
* @description Setting the second parameter to TRUE will give the physics control of the limbs in the given object. It also returns the limb back to animation control when the second parameter is set to FALSE.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb you wish to set.
* @param physics - TRUE will give limb control to physics simulation, FALSE will release the limb from physics control.
* @dbpro DYN SET LIMB
* @category RIGID BODY
*/
void dynSetLimb(int objectID, int limbID, bool physics){
    #ifdef DEBUG
		if(!DBError::limbExists(objectID, limbID, "dynSetLimb"))
			return;
	#endif
    sObject *obj = dbGetObject(objectID);
	obj->ppFrameList[limbID]->bOverride = physics;
}


/*
* @description Takes a limb and positions and orientates the associated actor to that of the given limb, this command can be used when swapping your model from animation to ragdoll.  
* @param objectID - ID number of the object which has a ragdoll set up.
* @param limbID - ID number of the limb.
* @dbpro DYN SET ACTOR TO LIMB
* @category RIGID BODY
*/
void dynSetActorToLimb(int objectID, int limbID){
    #ifdef DEBUG
		if(!DBError::limbExists(objectID, limbID, "dynSetActorToLimb")){
				return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetActorToLimb");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetActorToLimb");
			return;
		}
	#endif
	NxMat34 mat = DBUtil::getMatrixFromLimb(objectID, limbID);
	NxActor *actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	actor->setGlobalPose(mat);
}

/*
* @description Iterates through all the limbs in an object, positioning and orientating all their actor counterparts to that of the limb, this command can be used when swapping your model from animation to ragdoll.  
* @param objectID - ID number of the object which has a ragdoll set up.
* @dbpro DYN SET ACTORS TO LIMBS
* @category RIGID BODY
*/
void dynSetActorsToLimbs(int objectID){
    #ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynSetActorsToLimbs"))
			return;
	#endif
	//map is checked
	//limbs dont need check(limbCount)
	NxActor* actor;
	NxMat34 mat;
	for(int i = 1; i < dbLimbCount(objectID); i++){
		if(phyGlobStruct_->actorLimbMap_[objectID].count(i) != 0){
	       mat = DBUtil::getMatrixFromLimb(objectID, i);
	       actor = phyGlobStruct_->actorLimbMap_[objectID][i];
	       actor->setGlobalPose(mat);
		}
	}
}


void dynLoadRagdoll(int objectID, char* file){
    #ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynLoadRagdoll"))
			return;
		if(!DBError::fileExists(string(file), "dynLoadRagdoll"))
			return;
	#endif
	NXU::NxuPhysicsCollection *collection = 0;
	collection = NXU::loadCollection(file, NXU::FT_XML);
	if(!collection)
		return;

	LoadUserNotify userNotify;
	NXU::instantiateCollection(collection, *phyGlobStruct_->phy_->physicsSDK_, phyGlobStruct_->phy_->currentScene_, NULL, &userNotify);
	NXU::releaseCollection(collection);
	collection = 0;

	std::vector<NxActor*> actorVec = userNotify.actorVec;
	for(unsigned int a = 0; a < actorVec.size(); a++)
	{
		NxActor* actor = actorVec[a];
		if(actor->userData){
			//split string
			string space(" ");

			const char* up = (const char*)actor->userData;
			string upSTR(up);
			ActorSync *aSync;

			string flag;
			string id;
			int size = upSTR.size();
			int i = 0;
			bool swap = false;
			while(i < size){
				if(upSTR.at(i) == ' '){
				   swap = true;
				}
				if(!swap){
					flag.append(upSTR, i, 1);
				}
				else{
					id.append(upSTR, i, 1);
				}
				i++;
			}
			if(flag.compare("FP_DYNAMIC_OBJECT") == 0){
			   //delete up; // cant do
			   aSync = new ObjectFromActor(objectID);
			}
			else if(flag.compare("FP_DYNAMIC_LIMB" )== 0){
				int limb = atoi(id.c_str());
				aSync = new LimbFromActor(objectID, limb, DBUtil::getLimbScale(objectID, limb, phyGlobStruct_->phyScaleMultiplier_)); 
			    //add limb  to map
				//check map
				phyGlobStruct_->actorLimbMap_[objectID][limb] = actor;
			}
			actor->userData = (void*)aSync;
		}
	}
	userNotify.actorVec.clear();
}

// ****************************************************************************************** //
// *********************************** DB-JOINT ************************************* //
// ****************************************************************************************** //
//
//bool dynMakeJoint(int actorA, int limbA, int actorB, int limbB, float globAnchorX, float globAnchorY, float globAnchorZ){
//    #ifdef DEBUG
//	    //Check object exists
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return false;
//		}
//		
//	#endif
//		phyGlobStruct_->phy_->makeJointSpherical(phyGlobStruct_->actorLimbMap_[actorA][limbA], phyGlobStruct_->actorLimbMap_[actorB][limbB], globAnchorX, globAnchorY, globAnchorZ);
//        return true;
//}

// ****************************************************************************************** //
// *********************************** DB-CONSTRUNCTION ************************************* //
// ****************************************************************************************** //

// ** CONVEX MESH ** //

/*
* @description Cooks a convex mesh and saves it to file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the object to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, char* file){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookConvexMesh"))
			return false;
	#endif
	    std::string fileStr(file);
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0);
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();
		bool result = phyGlobStruct_->phy_->cookConvexMesh(fileStr, mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;
		return result;
}

/*
* @description Cooks and then creates a convex mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the object to create the mesh from.
* @param convexMeshID - ID number ou wish to assign to the convex mesh, you use this ID number when creating an actor from this convex mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, int convexMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookConvexMesh"))
			return false;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshAlreadyExists(convexMeshID, "dynCookConvexMesh");
			return false;
		}
	#endif
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;

		dbLockVertexDataForLimb(objectID, 0); 
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();

		NxConvexMesh *pMesh = 0;
		pMesh =  phyGlobStruct_->phy_->cookConvexMesh(mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;

		if(pMesh){
           phyGlobStruct_->convexMeshMap_[convexMeshID] = pMesh;
		   return true;
		}
		else{
           return false;
		}
}

/*
* @description Cooks and then creates a convex mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb to create the mesh from.
* @param convexMeshID - ID number you wish to assign to the convex mesh, use this number when creating any actor(s) that want to use this mesh. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, int limbID, int convexMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, limbID, "dynCookConvexMesh"))
			return false;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshAlreadyExists(convexMeshID, "dynCookConvexMesh");
			return false;
		}
	#endif
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0); 
		DBUtil::getMeshData(obj, limbID, phyGlobStruct_->phyScaleMultiplier_, mesh);
		dbUnlockVertexData();
		NxConvexMesh *pMesh = 0;
		pMesh =  phyGlobStruct_->phy_->cookConvexMesh(mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;

		if(pMesh){
           phyGlobStruct_->convexMeshMap_[convexMeshID] = pMesh;
		   return true;
		}
		else{
           return false;
		}

}

/*
* @description Creates a convex mesh and saves it to file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, int limbID, char* file){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, limbID, "dynCookConvexMesh"))
			return false;
	#endif
		std::string fileStr(file);
		sObject *obj = dbGetObject(objectID);
		
		//lock?
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, limbID); 
		DBUtil::getMeshData(obj, limbID, phyGlobStruct_->phyScaleMultiplier_, mesh);
        dbUnlockVertexData();
		bool result = phyGlobStruct_->phy_->cookConvexMesh(fileStr, mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;
		return result;
}    


/*
* @description Creates an actor from a convex mesh.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param convexMeshID - ID number of the mesh you wish to create the actor from.
* @param density - Density value of dynamic actor, 0.0 for static actor.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CONVEX MESH
* @category CONSTRUCTION
*/
bool dynMakeConvexMesh(int objectID, int convexMeshID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeConvexMesh"))
			return false;
		if(!phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshActorNotExist(convexMeshID, "dynMakeConvexMesh");
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorAlreadyExist(objectID, "dynMakeConvexMesh");
            return false;
		}
	#endif
		NxConvexMesh *pMesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		//Create updater object
		ObjectFromActor *aSync = new ObjectFromActor(objectID);
        //Make actor with updater
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeConvexMesh(aSync,
			                        pMesh,                        
									DBUtil::getMatrixFromObject(objectID),
									density);
		//add to map if successful
		if(actor){
			phyGlobStruct_->actorMap_[objectID] = actor;
			phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}

/*
* @description Creates an actor from a convex mesh.
* @param objectID     - ID number of the owning object.
* @param limbID       - ID number of the limb the actor will represent in the simulation.
* @param convexMeshID - ID number of the mesh you wish to create the actor from.
* @param density      - Density value of dynamic actor, 0.0 for static actor.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CONVEX MESH
* @category CONSTRUCTION
*/
bool dynMakeConvexMesh(int objectID, int limbID, int convexMeshID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::limbExists(objectID, limbID, "dynMakeConvexMesh"))
			return false;
		if(!phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshActorNotExist(convexMeshID, "dynMakeConvexMesh");
			return false;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(limbID)){
			Message::actorAlreadyExist(objectID, limbID, "dynMakeConvexMesh");
			return false;		
		}
		
	#endif
		NxConvexMesh *pMesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		//Create updater object
		LimbFromActor *aSync = new LimbFromActor(objectID,
			                                     limbID,
												 DBUtil::getLimbScale(objectID, limbID, phyGlobStruct_->phyScaleMultiplier_));
        //Make actor with updater
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeConvexMesh(aSync,
			                        pMesh,                        
									DBUtil::getMatrixFromLimb(objectID, limbID),
									density);
		//add to map if successful
		if(actor){
			phyGlobStruct_->actorLimbMap_[objectID][limbID] = actor;
			phyGlobStruct_->actorSyncList_.push_back(aSync);

			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}


/*
* @description Loads a convex mesh which has been pre-cooked to file.
* @param file - Name of the mesh file you wish to load the mesh from.
* @param convexMeshID - ID number you wish to assign to the convex mesh, use this number when creating any actor(s) that want to use this mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN LOAD CONVEX MESH
* @category CONSTRUCTION
*/
bool dynLoadConvexMesh(char* file, int convexMeshID){
		std::string fileStr(file);
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::fileExists(fileStr, "dynLoadConvexMesh"))
			return false;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID) == 1){
			Message::meshAlreadyExists(convexMeshID, "dynLoadConvexmesh");
			return false;
		}
	#endif
		NxConvexMesh* pMesh = 0;
		pMesh = phyGlobStruct_->phy_->loadConvexMesh(fileStr);
		if(pMesh){
		    phyGlobStruct_->convexMeshMap_[convexMeshID] = pMesh;
			return true;
		}
		else{
			return false;
		}
}

/*
* @description Releases the mesh from memory, it's very important to call this when you are done using the mesh, you can only release the mesh if it is not being used by any actors.
* @param convexMeshID - ID number of the mesh.
* @return TRUE if succeeded, FALSE otherwise. NOTE: Will return false if mesh is currently in use.
* @dbpro DYN DELETE CONVEX MESH
* @category CONSTRUCTION
*/
bool dynDeleteConvexMesh(int convexMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID) == 0){
			Message::meshActorNotExist(convexMeshID, "dynDeleteConvexMesh");
			return false;
		}
    #endif
		bool result = phyGlobStruct_->phy_->releaseConvexMesh(phyGlobStruct_->convexMeshMap_[convexMeshID]);
		if(result){
			phyGlobStruct_->convexMeshMap_.erase(convexMeshID);
			return true;
		}
		else{
			Message::errorBox(string("Mesh not deleted, delete all actors that reference the mesh first"), string("dynDeleteConvexMesh"));
			return false;
		}
}


// ** TRIANGLE MESH ** //

/*
* @description Cooks and then creates a triangle mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. All triangle meshes are static, their main use is for terrain and complex static objects.  
* @param objectID - ID number of the object to create the mesh from.
* @param triangleMeshID - ID number you wish to assign to the triangle mesh, you use this ID number when creating an actor from this triangle mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynCookTriangleMesh(int objectID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookTriangleMesh"))
			return false;
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID)){
			Message::meshAlreadyExists(triangleMeshID, "dynCookTriangleMesh");
            return false;
		}
	#endif
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0); 
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();
		NxTriangleMesh *pMesh = 0;
		pMesh =  phyGlobStruct_->phy_->cookTriangleMesh(mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;

		if(pMesh){
           phyGlobStruct_->triangleMeshMap_[triangleMeshID] = pMesh;
		   return true;
		}
		else{
           return false;
		}
}

/*
* @description Cooks and then creates a triangle mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. All triangle meshes are static, their main use is for terrain and complex static objects.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb you wish to create the mesh from.
* @param triangleMeshID - ID number you wish to assign to the triangle mesh, you use this ID number when creating an actor from this triangle mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynCookTriangleMesh(int objectID, int limbID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, limbID, "dynCookTriangleMesh"))
			return false;
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID)){
			Message::meshAlreadyExists(triangleMeshID, "dynCookTriangleMesh");
            return false;
		}
	#endif
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, limbID); 
		DBUtil::getMeshData(obj, limbID, phyGlobStruct_->phyScaleMultiplier_, mesh);
		dbUnlockVertexData();
		NxTriangleMesh *pMesh = 0;
		pMesh =  phyGlobStruct_->phy_->cookTriangleMesh(mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;

		if(pMesh){
           phyGlobStruct_->triangleMeshMap_[triangleMeshID] = pMesh;
		   return true;
		}
		else{
           return false;
		}
}


/* 
* @description Cooks a triangle mesh and saves it to file. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly.  All triangle meshes are static, their main use is for terrain and complex static objects.  
* @param objectID - ID number of the object to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK TRIANGLE MESH
* @category CONSTRUCTION
*/ 
bool dynCookTriangleMesh(int objectID, char* file){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookTriangleMesh"))
			return false;
	#endif
	    std::string fileStr(file);
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0); 
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();
		bool result =  phyGlobStruct_->phy_->cookTriangleMesh(fileStr, mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;
		return result;

}

/*
* @description Cooks a triangle mesh and saves it to file. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly.  All triangle meshes are static, their main use is for terrain and complex static objects.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynCookTriangleMesh(int objectID, int limbID, char* file){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, limbID, "dynCookTriangleMesh"))
			return false;
	#endif
		std::string fileStr(file);
		sObject *obj = dbGetObject(objectID);
		
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, limbID); 
		DBUtil::getMeshData(obj, limbID, phyGlobStruct_->phyScaleMultiplier_, mesh);
		dbUnlockVertexData();
		bool result =  phyGlobStruct_->phy_->cookTriangleMesh(fileStr, mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_);
		delete []mesh.indices_;
		delete []mesh.vertices_;
		return result;
}

/*
* @description Loads a cooked triangle mesh from file.
* @param file - Name of the file you wish to load the mesh from.
* @param triangleMeshID - ID number you wish to assign to the triangle mesh, use this number when creating any actor(s) that want to use this mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN LOAD TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynLoadTriangleMesh(char* file, int triangleMeshID){
		std::string fileStr(file);
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::fileExists(fileStr, "dynLoadTriangleMesh"))
			return false;
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 1){
			Message::meshAlreadyExists(triangleMeshID, "dynLoadTriangleMesh");
			return false;
		}
	#endif
		NxTriangleMesh* pMesh = 0;
		pMesh = phyGlobStruct_->phy_->loadTriangleMesh(fileStr);
		if(pMesh){
		    phyGlobStruct_->triangleMeshMap_[triangleMeshID] = pMesh;
			return true;
		}
		else{
			return false;
		}
}

/*
* @description Creates an actor from a triangle mesh, all triangle meshes are static.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param triangleMeshID - ID number of the mesh you wish to create the actor from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynMakeTriangleMesh(int objectID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeTriangleMesh"))
			return false;
		if(phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorAlreadyExist(objectID, "dynMakeTriangleMesh");
            return false;
		}
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 0){
			Message::meshActorNotExist(triangleMeshID, "dynMakTriangleMesh");
			return false;
		}
	#endif
        ObjectFromActor *aSync = new ObjectFromActor(objectID);
		NxActor *actor = 0;
		NxTriangleMesh* pMesh = phyGlobStruct_->triangleMeshMap_[triangleMeshID];
		actor = phyGlobStruct_->phy_->makeTriangleMesh(pMesh, DBUtil::getMatrixFromObject(objectID));
		//add to map if successful
		if(actor){
			//NOTE:Core physics should take sync on creation
			actor->userData = (void*)aSync;
			phyGlobStruct_->actorMap_[objectID] = actor;
            phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}
 
/*
* @description Creates an actor from a triangle mesh, all triangle meshes are static.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor will represent in the simulation.
* @param triangleMeshID - ID number of the mesh you wish to create the actor from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynMakeTriangleMesh(int objectID, int limbID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeTriangleMesh")){
			return false;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(limbID)){
			Message::actorAlreadyExist(objectID, limbID, "dynMakeTriangleMesh");
			return false;	
		}
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 0){
			Message::meshActorNotExist(triangleMeshID, "dynMakeTriangleMesh");
			return false;
		}
	#endif
        LimbFromActor *aSync = new LimbFromActor(objectID, limbID, NxVec3(1, 1, 1));
		NxActor *actor = 0;
		NxTriangleMesh* pMesh = phyGlobStruct_->triangleMeshMap_[triangleMeshID];
		actor = phyGlobStruct_->phy_->makeTriangleMesh(pMesh, DBUtil::getMatrixFromLimb(objectID, limbID));
		//add to map if successful
		if(actor){
			actor->userData = (void*)aSync;
			phyGlobStruct_->actorLimbMap_[objectID][limbID] = actor;
            phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}

/*
* @description Releases the mesh from memory, it's very important to call this when you are done using the mesh, you can only release the mesh if it is not being used by any actors.
* @param triangleMeshID - ID number of the mesh.
* @return TRUE if succeeded, FALSE otherwise. NOTE: Will return false if mesh is currently in use.
* @dbpro DYN DELETE TRIANGLE MESH
* @category CONSTRUCTION
*/
bool dynDeleteTriangleMesh(int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 0){
			Message::meshActorNotExist(triangleMeshID, "dynDeleteTriangleMesh");
			return false;
		}
    #endif
		bool result = phyGlobStruct_->phy_->releaseTriangleMesh(phyGlobStruct_->triangleMeshMap_[triangleMeshID]);
		if(result){
			phyGlobStruct_->triangleMeshMap_.erase(triangleMeshID);
			return true;
		}
		else{
			Message::errorBox(string("Mesh not deleted, delete all actors that reference the mesh first"), string("dynDeleteTriangleMesh"));
			return false;
		}
}

/*
* @description Returns the number of actors currently using this mesh, this is important to know as a mesh can not be deleted while it is in use by an actor.
* @param meshID - ID number of the triangle mesh.
* @return Number of actors referencing this mesh.
* @dbpro DYN TRIANGLE MESH GET REFERENCE COUNT
* @category CONSTRUCTION
*/
int dynTriangleMeshGetReferenceCount(int meshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->triangleMeshMap_.count(meshID) == 0){
			Message::meshActorNotExist(meshID, "dynTriangleMeshGetReferenceCount");
			return -1;
		}
    #endif
		return phyGlobStruct_->triangleMeshMap_[meshID]->getReferenceCount();
}

/*
* @description Returns the number of actors currently using this mesh, this is important to know as a mesh can not be deleted while it is in use by an actor.
* @param meshID - ID number of the convex mesh.
* @return Number of actors referencing this mesh.
* @dbpro DYN CONVEX MESH GET REFERENCE COUNT
* @category CONSTRUCTION
*/
int dynConvexMeshGetReferenceCount(int meshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->convexMeshMap_.count(meshID) == 0){
			Message::meshActorNotExist(meshID, "dynConvexMeshGetReferenceCount");
			return -1;
		}
    #endif
		return phyGlobStruct_->convexMeshMap_[meshID]->getReferenceCount();
}


/*
* @description Releases the actor from the simulation.
* @param objectID - ID number of the owning objec.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @return TRUE if succeeded, FALSE otherwise. 
* @dbpro DYN DELETE ACTOR
* @category CONSTRUCTION
*/
void dynDeleteActor(int objectID, int limbID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, limbID, "dynDeleteActor");
            return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynDeleteActor");
			return;
		}
	#endif
    NxActor *actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	//Release actor sync and remove from list
	if(actor->userData != 0){
		ActorSync* as = (ActorSync*)actor->userData;
    	delete as;
		actor->userData = 0;
		phyGlobStruct_->actorSyncList_.remove(as);
	}
	actor->getScene().releaseActor(*actor);
	//Release from map
	phyGlobStruct_->actorLimbMap_[objectID].erase(limbID);
}


/*
* @description Releases the actor from the simulation.
* @param objectID - ID number of the actor.
* @dbpro DYN DELETE ACTOR
* @category CONSTRUCTION
*/
void dynDeleteActor(int objectID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynDeleteActor");
            return;
		}
		phyGlobStruct_->actorDeleted_ = true; // gets reset after fetch results, prevents crash
	#endif
    NxActor *actor = phyGlobStruct_->actorMap_[objectID];
	//Release actor sync and remove from list
	if(actor->userData != 0){
		ActorSync* as = (ActorSync*)actor->userData;
		//Moved v1.1
    	phyGlobStruct_->actorSyncList_.remove(as);
		delete as;
		actor->userData = 0;
		//phyGlobStruct_->actorSyncList_.remove(as);
	}
	actor->getScene().releaseActor(*actor);
	//Release from map
	phyGlobStruct_->actorMap_.erase(objectID);
}


/*
* @description Positions a rigid body in world space, you can use this command when you are setting or re-setting your scene.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @dbpro DYN SET POSITION
* @category RIGID BODY
*/
void dynSetPosition(int objectID, float x, float y, float z){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetPosition");
            return;
		}
		phyGlobStruct_->actorDeleted_ = true; // gets reset after fetch results, prevents crash
	#endif
    phyGlobStruct_->actorMap_[objectID]->setGlobalPosition(NxVec3(x, y, z));
}

/*
* @description Rotates a rigid body in world space, you can use this command when you are setting or re-setting your scene.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @dbpro DYN SET ROTATION
* @category RIGID BODY
*/
void dynSetRotation(int objectID, float x, float y, float z){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetPosition");
            return;
		}
	#endif
		D3DXMATRIX d3dmat;
		NxMat34 nxmat;
		MathUtil::setMatrix(d3dmat, x, y, z);
		nxmat.setColumnMajor44(d3dmat.m);
		phyGlobStruct_->actorMap_[objectID]->setGlobalOrientation(nxmat.M);
}


/*
* @description Adds any shapes that have been added to the shape list to an actor, the shape list is then cleared.
* @param objectID - ID number of the object/actor the shapes will be added to.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN ADD SHAPES
* @category RIGID BODY
*/
bool dynAddShapes(int actorID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynAddShapes"); 
			return false;
		}
	#endif	
	NxActor *actor = phyGlobStruct_->actorMap_[actorID];
	for(unsigned int i = 0; i < phyGlobStruct_->shapeDescVec_.size(); i++){
		phyGlobStruct_->phy_->addShape(phyGlobStruct_->shapeDescVec_.at(i), actor);
		//Release memory for shape
		delete phyGlobStruct_->shapeDescVec_.at(i);
	}
	//Clear vector
	phyGlobStruct_->shapeDescVec_.clear();
	return true;
}

bool dynAddShapes(int actorID, int limbID){
	return true;
}

/*
* @description Creates a shape from a convex mesh. The shape is added to a list, you can add as many shapes as you like to the list and then either create an actor from the shapes or add them to an existing actor.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the shape will position/orientate to.
* @param convexMeshID - ID number of the convex mesh you wish to instance.
* @return The amount of shapes currently held in the list, if an actor is created from the current list of shapes then they will keep the shape number returned from this function.
* @dbpro DYN MAKE SHAPE CONVEX
* @category CONSTRUCTION
*/
int dynMakeShapeConvex(int objectID, int limbID, int convexMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!DBError::limbExists(objectID, limbID, "dynMakeShapeConvex"))
			return -1;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID) == 0){
			Message::meshActorNotExist(convexMeshID, "dynMakeShapeConvex");
			return -1;
		}
	#endif	
		NxConvexMesh *pMesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		NxShapeDesc *shapeDesc = phyGlobStruct_->phy_->makeShapeConvex(DBUtil::getMatrixFromLimb(objectID, limbID),
			                                          pMesh);
		phyGlobStruct_->shapeDescVec_.push_back(shapeDesc);
		return phyGlobStruct_->shapeDescVec_.size();
}

/*
* @description Creates a shape from a convex mesh. The shape is added to a list, you can add as many shapes as you like to the list and then either create an actor from the shapes or add them to an existing actor. Do not add a triangle mesh shape to an actor that is dynamic as triangle meshes are static only.
* @param objectID ID number of the object the shape will position/orientate to.
* @param triangleMeshID ID number of the convex mesh you wish to instance.
* @return The amount of shapes currently held in the list, if an actor is created from the current list of shapes then they will keep the shape number returned from this function.
* @dbpro DYN MAKE SHAPE TRIANGLE MESH
* @category CONSTRUCTION
*/
int dynMakeShapeTriangleMesh(int objectID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!DBError::objectExists(objectID, "dynMakeShapeTriangleMesh"))
			return -1;
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 0){
			Message::meshActorNotExist(triangleMeshID, "dynMakeShapeTriangleMesh");
			return -1;
		}
	#endif	
		NxTriangleMesh *pMesh = phyGlobStruct_->triangleMeshMap_[triangleMeshID];
		NxShapeDesc *shapeDesc = phyGlobStruct_->phy_->makeShapeTriangleMesh(DBUtil::getMatrixFromObject(objectID),
			                                          pMesh);
		phyGlobStruct_->shapeDescVec_.push_back(shapeDesc);
		return phyGlobStruct_->shapeDescVec_.size();
}


/*
* @description Creates a shape from a triangle mesh. The shape is added to a list, you can add as many shapes as you like to the list and then either create an actor from the shapes or add them to an existing actor. Do not add a triangle mesh shape to an actor that is dynamic as triangle meshes are static only.
* @param objectID ID number of the owning object.
* @param limbID ID number of the limb the shape will position/orientate to.
* @param triangleMeshID ID number of the triangle mesh you wish to instance.
* @return The amount of shapes currently held in the list, if an actor is created from the current list of shapes then they will keep the shape number returned from this function.
* @dbpro DYN MAKE SHAPE TRIANGLE MESH
* @category CONSTRUCTION
*/
int dynMakeShapeTriangleMesh(int objectID, int limbID, int triangleMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!DBError::limbExists(objectID, limbID, "dynMakeShapeTriangleMesh"))
			return -1;
		if(phyGlobStruct_->triangleMeshMap_.count(triangleMeshID) == 0){
			Message::meshActorNotExist(triangleMeshID, "dynMakeShapeTriangleMesh");
			return -1;
		}
	#endif	
		NxTriangleMesh *pMesh = phyGlobStruct_->triangleMeshMap_[triangleMeshID];
		NxShapeDesc *shapeDesc = phyGlobStruct_->phy_->makeShapeTriangleMesh(DBUtil::getMatrixFromLimb(objectID, limbID),
			                                          pMesh);
		phyGlobStruct_->shapeDescVec_.push_back(shapeDesc);
		return phyGlobStruct_->shapeDescVec_.size();
}

/*
* @description Creates a shape from a triangle mesh. The shape is added to a list, you can add as many shapes as you like to the list and then either create an actor from the shapes or add them to an existing actor.
* @param objectID ID number of the object the shape will position/orientate to.
* @param convexMeshID ID number of the convex mesh you wish to instance.
* @return The amount of shapes currently held in the list, if an actor is created from the current list of shapes then they will keep the shape number returned from this function.
* @dbpro DYN MAKE SHAPE CONVEX
* @category CONSTRUCTION
*/
int dynMakeShapeConvex(int objectID, int convexMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!DBError::objectExists(objectID, "dynMakeShapeConvex"))
			return -1;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID) == 0){
			Message::meshActorNotExist(convexMeshID, "dynMakeShapeConvex");
			return -1;
		}
	#endif	
		NxConvexMesh *pMesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		NxShapeDesc *shapeDesc = phyGlobStruct_->phy_->makeShapeConvex(DBUtil::getMatrixFromObject(objectID),
			                                          pMesh);
		phyGlobStruct_->shapeDescVec_.push_back(shapeDesc);
		return phyGlobStruct_->shapeDescVec_.size();
}



/*
* @description Allows you to raise or lower shape flags
* @param objectID - ID number of the owning object.
* @param shapeIndex - Index ID of the shape, default shape = 0.
* @param flag - Flag you would like to raise or lower.
* @param value - TRUE to raise flag, FALSE to lower flag.
* @dbpro DYN SHAPE SET FLAG
* @category RIGID BODY
*/
void dynShapeSetFlag(int objectID, int shapeIndex, int flag, bool value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
		    Message::actorNotExist(objectID, "dynShapeSetFlag");
		    return;
		}
		//Shapes get checked in core physics
	#endif
		phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->setFlag((NxShapeFlag)flag, value);
}




/*
* @description Sets the position of the shape relative to the local frame of the actor.
* @param objectID ID number of the object/actor to which the shape belongs.
* @param shapeID Index ID of the shape.
* @param posX Position along the X axis.
* @param posY Position along the Y axis.
* @param posZ Position along the Z axis.
* @dbpro DYN SHAPE SET LOC POS
* @category RIGID BODY
*/
void dynShapeSetLocPos(int actorID, int shapeID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynShapeSetLocPos");
		}
		//Shapes get checked in core physics
	#endif
		phyGlobStruct_->phy_->setShapeLocPos(phyGlobStruct_->actorMap_[actorID], shapeID, NxVec3(posX, posY, posZ)); 
}

/*
* @description Sets the rotation of the shape relative to the local frame of the actor.
* @param objectID ID number of the object/actor to which the shape belongs.
* @param shapeID Index ID of the shape.
* @param angleX Rotation about the X axis.
* @param angleY Rotation about the Y axis.
* @param angleZ Rotation about the Z axis.
* @dbpro DYN SHAPE SET LOC ROT
* @category RIGID BODY
*/
void dynShapeSetLocRot(int actorID, int shapeID, float angleX, float angleY, float angleZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynShapeSetLocRot");
		}
		//Shapes get checked in core physics
	#endif
		D3DXMATRIX mat;
		NxMat34 nxMat;
		MathUtil::setMatrix(mat, 0, 0, 0 ,angleX, angleY, angleZ);
	    nxMat.setColumnMajor44(mat.m);
		phyGlobStruct_->phy_->setShapeLocRot(phyGlobStruct_->actorMap_[actorID], shapeID, nxMat.M); 
}

/*
* @description Sets the position of the shape in global space.
* @param objectID ID number of the object/actor to which the shape belongs.
* @param shapeID Index ID of the shape.
* @param posX Position along the X axis.
* @param posY Position along the Y axis.
* @param posZ Position along the Z axis.
* @dbpro DYN SHAPE SET GLOB POS
* @category RIGID BODY
*/
void dynShapeSetGlobPos(int actorID, int shapeID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynShapeSetGlobPos");
		}
		//Shapes get checked in core physics
	#endif
		phyGlobStruct_->phy_->setShapeGlobPos(phyGlobStruct_->actorMap_[actorID], shapeID, NxVec3(posX, posY, posZ)); 
}


/*
* @description Sets the rotation of the shape in global space.
* @param objectID ID number of the object/actor to which the shape belongs.
* @param shapeID Index ID of the shape.
* @param angleX Rotation about the X axis.
* @param angleY Rotation about the Y axis.
* @param angleZ Rotation about the Z axis.
* @dbpro DYN SHAPE SET GLOB ROT
* @category RIGID BODY
*/
void dynShapeSetGlobRot(int actorID, int shapeID, float angleX, float angleY, float angleZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynShapeSetGlobRot");
		}
		//Shapes get checked in core physics
	#endif
		D3DXMATRIX mat;
		NxMat34 nxMat;
		MathUtil::setMatrix(mat, angleX, angleY, angleZ);
	    nxMat.setColumnMajor44(mat.m);
		phyGlobStruct_->phy_->setShapeGlobRot(phyGlobStruct_->actorMap_[actorID], shapeID, nxMat.M); 
}









/*
* @description Sets the position of the shape relative to the local frame of the actor.
* @param objectID ID number of the owning object.
* @param limbID ID number of the limb the actor represents in the simulation.
* @param shapeID Index ID of the shape.
* @param posX Position along the X axis.
* @param posY Position along the Y axis.
* @param posZ Position along the Z axis.
* @dbpro DYN SHAPE SET LOC POS
* @category RIGID BODY
*/
void dynShapeSetLocPos(int actorID, int limbID, int shapeID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(actorID)){
		    Message::actorNotExist(actorID, limbID, "dynShapeSetLocPos");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[actorID].count(limbID) == 0){
			Message::actorNotExist(actorID, limbID, "dynShapeSetLocPos");
			return;		
		}
		//Shapes get checked in core physics
	#endif
		phyGlobStruct_->phy_->setShapeLocPos(phyGlobStruct_->actorLimbMap_[actorID][limbID], shapeID, NxVec3(posX, posY, posZ)); 
}

/*
* @description Sets the rotation of the shape relative to the local frame of the actor.
* @param objectID ID number of the owning object.
* @param limbID ID number of the limb the actor represents in the simulation.
* @param shapeID Index ID of the shape.
* @param angleX Rotation about the X axis.
* @param angleY Rotation about the Y axis.
* @param angleZ Rotation about the Z axis.
* @dbpro DYN SHAPE SET LOC ROT
* @category RIGID BODY
*/
void dynShapeSetLocRot(int actorID, int limbID, int shapeID, float angleX, float angleY, float angleZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(actorID)){
		    Message::actorNotExist(actorID, limbID, "dynShapeSetLocRot");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[actorID].count(limbID) == 0){
			Message::actorNotExist(actorID, limbID, "dynShapeSetLocRot");
			return;		
		}
		//Shapes get checked in core physics
	#endif
		D3DXMATRIX mat;
		NxMat34 nxMat;
		MathUtil::setMatrix(mat, 0, 0, 0 ,angleX, angleY, angleZ);
	    nxMat.setColumnMajor44(mat.m);
		phyGlobStruct_->phy_->setShapeLocRot(phyGlobStruct_->actorLimbMap_[actorID][limbID], shapeID, nxMat.M); 
}

/*
* @description Sets the position of the shape in global space.
* @param objectID ID number of the owning object.
* @param limbID ID number of the limb the actor represents in the simulation.
* @param shapeID Index ID of the shape.
* @param posX Position along the X axis.
* @param posY Position along the Y axis.
* @param posZ Position along the Z axis.
* @dbpro DYN SHAPE SET GLOB POS
* @category RIGID BODY
*/
void dynShapeSetGlobPos(int actorID, int limbID, int shapeID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(actorID)){
		    Message::actorNotExist(actorID, limbID, "dynShapeSetGlobPos");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[actorID].count(limbID) == 0){
			Message::actorNotExist(actorID, limbID, "dynShapeSetGlobPos");
			return;		
		}
		//Shapes get checked in core physics
	#endif
		phyGlobStruct_->phy_->setShapeGlobPos(phyGlobStruct_->actorLimbMap_[actorID][limbID], shapeID, NxVec3(posX, posY, posZ)); 
}


/*
* @description Sets the rotation of the shape in global space.
* @param objectID ID number of the owning object.
* @param limbID ID number of the limb the actor represents in the simulation.
* @param shapeID Index ID of the shape.
* @param angleX Rotation about the X axis.
* @param angleY Rotation about the Y axis.
* @param angleZ Rotation about the Z axis.
* @dbpro DYN SHAPE SET GLOB ROT
* @category RIGID BODY
*/
void dynShapeSetGlobRot(int actorID, int limbID, int shapeID, float angleX, float angleY, float angleZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(actorID)){
		    Message::actorNotExist(actorID, limbID, "dynShapeSetGlobRot");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[actorID].count(limbID) == 0){
			Message::actorNotExist(actorID, limbID, "dynShapeSetGlobRot");
			return;		
		}
		//Shapes get checked in core physics
	#endif
		D3DXMATRIX mat;
		NxMat34 nxMat;
		MathUtil::setMatrix(mat, angleX, angleY, angleZ);
	    nxMat.setColumnMajor44(mat.m);
		phyGlobStruct_->phy_->setShapeGlobRot(phyGlobStruct_->actorLimbMap_[actorID][limbID], shapeID, nxMat.M); 
}

















//#ifdef COMPILE_FOR_GDK

/*
* @description Returns the maximum angular velocity of the given actor. Because for various internal computations, very quickly rotating actors introduce error into the simulation, which leads to undesired results. However, because some actors, such as car wheels, should be able to rotate quickly, you can override the default setting on a per-actor basis with the below call. Note that objects such as wheels which are approximated with spherical or other smooth collision primitives can be simulated with stability at a much higher angular velocity than, say, a box that has corners.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @return The maximum angular velocity of the given actor 
* @dbpro DYN GET MAX ANGULAR VELOCITY
* @category RIGID BODY
*/
DWORDFLOAT dynGetMaxAngularVelocity(int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetMaxAngularVelocity");
			return 0;
		}
	#endif
		float value = phyGlobStruct_->actorMap_[objectID]->getMaxAngularVelocity();
        RETURNCAST
}
//#else
//DWORD dynGetMaxAngularVelocity(int objectID){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return 0;
//		}
//		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
//			Message::actorNotExist(objectID, "dynGetMaxAngularVelocity");
//			return 0;
//		}
//	#endif
//		float result = phyGlobStruct_->actorMap_[objectID]->getMaxAngularVelocity();
//        return *(DWORD*)&result;
//}
//#endif

/*
* @description Sets the maximum angular velocity of the given actor. Because for various internal computations, very quickly rotating actors introduce error into the simulation, which leads to undesired results. However, because some actors, such as car wheels, should be able to rotate quickly, you can override the default setting on a per-actor basis with the below call. Note that objects such as wheels which are approximated with spherical or other smooth collision primitives can be simulated with stability at a much higher angular velocity than, say, a box that has corners.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param value - The maximum angular velocity of this actor. 
* @dbpro DYN SET MAX ANGULAR VELOCITY
* @category RIGID BODY
*/
void dynSetMaxAngularVelocity(int objectID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetMaxAngularVelocity");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setMaxAngularVelocity(value);
}





/*
* @description Fetches the angular momentum of an actor and places the values in the given vector.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET ANGULAR MOMENTUM
* @category RIGID BODY
*/
void dynGetAngularMomentum(int objectID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetAngularMomentum");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorMap_[objectID]->getAngularMomentum();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Sets the angular momentum of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param x - Momentum about the x axis.
* @param y - Momentum about the y axis.
* @param z - Momentum about the z axis.
* @dbpro DYN SET ANGULAR MOMENTUM
* @category RIGID BODY
*/
void dynSetAngularMomentum(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetAngularMomentum");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setAngularMomentum(NxVec3(x, y, z));
}







/*
* @description Fetches the linear momentum of an actor and places the values in the given vector.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET LINEAR MOMENTUM
* @category RIGID BODY
*/
void dynGetLinearMomentum(int objectID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetLinearMomentum");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorMap_[objectID]->getLinearMomentum();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Sets the linear momentum of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param x - Momentum along the x axis.
* @param y - Momentum along the y axis.
* @param z - Momentum along the z axis.
* @dbpro DYN SET LINEAR MOMENTUM
* @category RIGID BODY
*/
void dynSetLinearMomentum(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetLinearMomentum");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setLinearMomentum(NxVec3(x, y, z));
}



/*
* @description Fetches the angular velocity of an actor and places the values in the given vector.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET ANGULAR VELOCITY
* @category RIGID BODY
*/
void dynGetAngularVelocity(int objectID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetAngularVelocity");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorMap_[objectID]->getAngularVelocity();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Fetches the linear velocity of an actor and places the values in the given vector.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET LINEAR VELOCITY
* @category RIGID BODY
*/
void dynGetLinearVelocity(int objectID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetLinearVelocity");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorMap_[objectID]->getLinearVelocity();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Sets the angular velocity of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param x - Velocity about the x axis.
* @param y - Velocity about the y axis.
* @param z - Velocity about the z axis.
* @dbpro DYN SET ANGULAR VELOCITY
* @category RIGID BODY
*/
void dynSetAngularVelocity(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetAngularVelocity");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setAngularVelocity(NxVec3(x, y, z));
}

/*
* @description Sets the linear velocity of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param x - Velocity along the x axis.
* @param y - Velocity along the y axis.
* @param z - Velocity along the z axis.
* @dbpro DYN SET LINEAR VELOCITY
* @category RIGID BODY
*/
void dynSetLinearVelocity(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetLinearVelocity");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setLinearVelocity(NxVec3(x, y, z));
}














/*
* @description Fetches the angular velocity of an actor and places the values in the given vector.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation. 
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET ANGULAR VELOCITY
* @category RIGID BODY
*/
void dynGetAngularVelocity(int objectID, int limbID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetAngularVelocity");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetAngularVelocity");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getAngularVelocity();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Fetches the linear velocity of an actor and places the values in the given vector.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation. 
* @param vector3ID - ID number of the vector you wish to use to collect the data.
* @dbpro DYN GET LINEAR VELOCITY
* @category RIGID BODY
*/
void dynGetLinearVelocity(int objectID, int limbID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetLinearVelocity");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetLinearVelocity");
			return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getLinearVelocity();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Sets the angular velocity of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation. 
* @param x - Velocity about the x axis.
* @param y - Velocity about the y axis.
* @param z - Velocity about the z axis.
* @dbpro DYN SET ANGULAR VELOCITY
* @category RIGID BODY
*/
void dynSetAngularVelocity(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetAngularVelocity");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynSetAngularVelocity");
			return;
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setAngularVelocity(NxVec3(x, y, z));
}

/*
* @description Sets the linear velocity of an actor, you should usually apply forces to change the velocity of an actor although this command can be used to reset your actor etc.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation. 
* @param x - Velocity along the x axis.
* @param y - Velocity along the y axis.
* @param z - Velocity along the z axis.
* @dbpro DYN SET LINEAR VELOCITY
* @category RIGID BODY
*/
void dynSetLinearVelocity(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetLinearVelocity");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynSetLinearVelocity");
			return;
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setLinearVelocity(NxVec3(x, y, z));
}



















/*
* @description Returns the angular damping of an actor.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @return Angular damping value.
* @dbpro DYN GET ANGULAR DAMPING
* @category RIGID BODY
*/
DWORDFLOAT dynGetAngularDamping(int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetAngularDamping");
			return 0;
		}
	#endif
		float value = phyGlobStruct_->actorMap_[objectID]->getAngularDamping();
        RETURNCAST
}

/*
* @description Returns the angular damping of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @return Angular damping value.
* @dbpro DYN GET ANGULAR DAMPING
* @category RIGID BODY
*/
DWORDFLOAT dynGetAngularDamping(int objectID, int limbID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynGetAngularDamping");
		    return 0;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynGetAngularDamping");
			return 0;		
		}
	#endif
		float value = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getAngularDamping();
        RETURNCAST
}

/*
* @description Fetches the angular momentum of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param vec3Out - ID number of the vector3 in which the data will be placed.
* @dbpro DYN GET ANGULAR MOMENTUM
* @category RIGID BODY
*/
void dynGetAngularMomentum(int objectID, int limbID, int vec3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynGetAngularMomentum");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynGetAngularMomentum");
			return;		
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getAngularMomentum();
        dbSetVector3(vec3Out, vec.x, vec.y, vec.z);
}


/*
* @description Returns the linear damping of an actor.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @return Linear damping value. 
* @dbpro DYN GET LINEAR DAMPING
* @category RIGID BODY
*/
DWORDFLOAT dynGetLinearDamping(int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetLinearDamping");
			return 0;
		}
	#endif
		float value = phyGlobStruct_->actorMap_[objectID]->getLinearDamping();
        RETURNCAST
}

/*
* @description Returns the linear damping of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @return Linear damping value. 
* @dbpro DYN GET LINEAR DAMPING
* @category RIGID BODY
*/
DWORDFLOAT dynGetLinearDamping(int objectID, int limbID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynGetLinearDamping");
		    return 0;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynGetLinearDamping");
			return 0;		
		}
	#endif
		float value = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getLinearDamping();
        RETURNCAST
}

/*
* @description Fetches the linear momentum of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param vec3Out - ID number of the vector3 in which the data will be placed.
* @dbpro DYN GET LINEAR MOMENTUM
* @category RIGID BODY
*/
void dynGetLinearMomentum(int objectID, int limbID, int vec3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynGetLinearMomentum");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynGetLinearMomentum");
			return;		
		}
	#endif
		NxVec3 vec = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getLinearMomentum();
        dbSetVector3(vec3Out, vec.x, vec.y, vec.z);
}


/*
* @description Sets the angular damping of an actor.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param value - Angular damping value.
* @dbpro DYN SET ANGULAR DAMPING
* @category RIGID BODY
*/
void dynSetAngularDamping(int objectID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetAngularDamping");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setAngularDamping(value);
}


/*
* @description Sets the angular damping of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param value - Angular damping value.
* @dbpro DYN SET ANGULAR DAMPING
* @category RIGID BODY
*/
void dynSetAngularDamping(int objectID, int limbID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynSetAngularDamping");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetAngularDamping");
			return;		
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setAngularDamping(value);
}

/*
* @description Sets the angular momentum of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param x - Angular damping value about the x axis.
* @param y - Angular damping value about the y axis.
* @param z - Angular damping value about the z axis.
* @dbpro DYN SET ANGULAR MOMENTUM
* @category RIGID BODY
*/
void dynSetAngularMomentum(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynSetAngularMomentum");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetAngularMomentum");
			return;		
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setAngularMomentum(NxVec3(x, y, z));
}

/*
* @description Sets the linear damping of an actor.
* @param objectID - ID number of the object whose associated actor you wish to access.
* @param value - Linear damping value.
* @dbpro DYN SET LINEAR DAMPING
* @category RIGID BODY
*/
void dynSetLinearDamping(int objectID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetLinearDamping");
			return;
		}
	#endif
		phyGlobStruct_->actorMap_[objectID]->setLinearDamping(value);
}

/*
* @description Sets the linear damping of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param value - Linear damping value.
* @dbpro DYN SET LINEAR DAMPING
* @category RIGID BODY
*/
void dynSetLinearDamping(int objectID, int limbID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynSetLinearDamping");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetLinearDamping");
			return;		
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setLinearDamping(value);
}

/*
* @description Sets the linear momentum of an actor. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @param x - Angular damping value along the x axis.
* @param y - Angular damping value along the y axis.
* @param z - Angular damping value along the z axis.
* @dbpro DYN SET LINEAR MOMENTUM
* @category RIGID BODY
*/
void dynSetLinearMomentum(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynSetLinearMomentum");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetLinearMomentum");
			return;		
		}
	#endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setLinearMomentum(NxVec3(x, y, z));
}

















bool dynMakeBox(int objectID, int limbID, float sizeX, float sizeY, float sizeZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::limbExists(objectID, limbID, "dynMakeBox")){
            return false;
		}
	#endif
		LimbFromActor *aSync = new LimbFromActor(objectID, limbID, DBUtil::getLimbScale(objectID, limbID, phyGlobStruct_->phyScaleMultiplier_));
		NxActor *actor = 0;
		float density = 10.0f;
		actor = phyGlobStruct_->phy_->makeBox(aSync, 
			                 DBUtil::getMatrixFromLimb(objectID, limbID),
			                 NxVec3(sizeX, sizeY, sizeZ),
							 density,
							 NxVec3(0, 0, 0));
		if(actor){
			phyGlobStruct_->actorLimbMap_[objectID][limbID] = actor;
			phyGlobStruct_->actorSyncList_.push_back(aSync);
			//Only override if actor, not shape
			//When overriding we may need to set override matrix
			D3DXMATRIX mat;
			sObject *obj = dbGetObject(objectID);
			dbGetObjectWorldMatrix(objectID, limbID, &mat);//with scale
			obj->ppFrameList[limbID]->matOverride = mat;
			obj->ppFrameList[limbID]->bOverride = true;
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}

/*
* @description Creates a standard ground plane.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE GROUND PLANE
* @category CONSTRUCTION
*/
bool dynMakeGroundPlane(){
   	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->phy_->makeGroundPlane();
}

bool checkTimer()
{
	if(GetTickCount() > DBPhyStruct::timeLimit_)
	{
		if(!phyGlobStruct_->limitMessageSent_)
		{
		    Message::errorBox("Time expired - Trial version only.", "Warning");
			phyGlobStruct_->limitMessageSent_ = true;
		}
		return false;
	}
	return true;
}

/*
* @description Synchronises your objects to their actor counterparts for the current scene.
* @dbpro DYN UPDATE
* @category CORE
*/
void dynUpdate(){
	//D3DCAPS9 caps;
	//dbGetDirect3DDevice()->GetDeviceCaps(&caps);
	
    #ifdef COMPILE_WITH_TIME_LIMIT
	    if(!checkTimer())
			return;
		//if(GetTickCount() > DBPhyStruct::timeLimit_)
		//{
		//	if(!phyGlobStruct_->limitMessageSent_)
		//	{
		//	    Message::errorBox("Time expired - Trial version only.", "Warning");
		//		phyGlobStruct_->limitMessageSent_ = true;
		//	}
		//	return;
		//}
    #endif
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorDeleted_ == true){
			Message::errorBox("update() called after actor removed from simulation!", "Error");
			return;
		}
	#endif
	phyGlobStruct_->phy_->update();
}

/*
* @description Fetches the results from the simulation on the current scene, you can now update your objects, make/delete actors and apply any forces etc.
* @dbpro DYN FETCH RESULTS
* @category CORE
*/
void dynFetchResults(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		phyGlobStruct_->actorDeleted_ = false;
	#endif
	    phyGlobStruct_->phy_->fetchResults();
}

/*
* @description Starts the simulation on the current scene, do not make/delete any actors or apply any forces while simulating.
* @dbpro DYN SIMULATE
* @category CORE
*/
void dynSimulate(){
	#ifdef COMPILE_WITH_TIME_LIMIT
	    if(!checkTimer())
		{
             return;
		}
		//if(dbTimer() > DBPhyStruct::timeLimit_)
		//{
		//	if(!phyGlobStruct_->limitMessageSent_)
		//	{
		//	    Message::errorBox("Time expired - Trial version only.", "Warning");
		//		phyGlobStruct_->limitMessageSent_ = true;
		//	}
		//	return;
		//}
    #endif
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	    phyGlobStruct_->phy_->simulate();
}

/*
* @description Starts the simulation on the current scene. This version of the command gives you direct control over the amount of time you wish to advance the simulation by, this is useful when using timer based movement for consistancy across machines running at different speeds, you may need to alter the timing method of the scene as the default is a fixed timestep. Do not make/delete any actors or apply any forces while simulating.
* @param timeStep - Amount of time you wish to advance the simulation by.
* @dbpro DYN SIMULATE
* @category CORE
*/
void dynSimulate(float timeStep){
	#ifdef COMPILE_WITH_TIME_LIMIT
	    if(!checkTimer())
		{
             return;
		}
		//if(dbTimer() > DBPhyStruct::timeLimit_)
		//{
		//	if(!phyGlobStruct_->limitMessageSent_)
		//	{
		//	    Message::errorBox("Time expired - Trial version only.", "Warning");
		//		phyGlobStruct_->limitMessageSent_ = true;
		//	}
		//	return;
		//}
    #endif
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	    phyGlobStruct_->phy_->simulate(timeStep);
}

/*
* @description Sets the timing for the current scene. Simulation is done one time step at a time, typically using a fixed step size ranging between 1/100th of a second and 1/50th of a second. For a real-time simulation, the application must perform several of these time steps to synchronize the physics behavior with the rendered frame rate. Note that longer timesteps lead to poor stability in the simulation, and that values much above the aforementioned range are not recommended.
* @param maxTimeStep - When using fixed timing this is the maximum timestep the simulation will advance the simulation by. The default is (1/60)/4, as the default timestep is (1/60) the simulation will do 4 substeps for each call to simulate.
* @param maxIter - When using fixed timing this is the maximum substeps the simulation will do each time simulate is called. The default value is 4 which directly relates to the default maxTimeStep.
* @param timeStepMethod - Set to 0 for fixed timing and 1 for variable timing. When set to 1 the first two parameters have no effect.
* @dbpro DYN SET TIMING
* @category CORE
*/
void dynSetTiming(float maxTimeStep, int maxIter, int timeStepMethod){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->currentScene_->setTiming(maxTimeStep, maxIter, (NxTimeStepMethod)timeStepMethod);
}

/*
* @description Connects to the visual debugger.
* @dbpro DYN VISUAL DEBUGGER ON
* @category DEBUG
*/
void dynVisualDebuggerOn(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->visualDebuggerOn();
}

/*
* @description Disconnects from the visual debugger.
* @dbpro DYN VISUAL DEBUGGER OFF
* @category DEBUG
*/
void dynVisualDebuggerOff(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->visualDebuggerOff();
}

/*
* @description Checks if the visual debugger is connected.
* @dbpro DYN VISUAL DEBUGGER CONNECTED
* @return TRUE if connected, FALSE otherwise.
* @category DEBUG
*/
bool dynVisualDebuggerConnected(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->phy_->visualDebuggerConnected();
}


/*
* @description  Call this command inside your game/app loop to render parts of the simulation alongside your game/app, you can setup exactly what gets rendered with the 'set parameter' command, be sure to set the visualization scale first to activate debugging.
* @dbpro DYN DEBUG RENDER
* @category DEBUG
*/
void dynDebugRender(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->d3dUtil_){
			//Message::
            return;
		}
	#endif	
		phyGlobStruct_->d3dUtil_->d3d_ = dbGetDirect3DDevice();
	    phyGlobStruct_->d3dUtil_->debugRender();		
}

/*
* @description This command allows you to set the values for the global parameters of the Physics SDK. You can change the default skin width, sleep velocities, bounce threshold, continuous collision detection, friction scaling and all the parameters used for setting up the debug renderer. 
* @param parameter ID number of the parameter that you wish to set.
* @param value New value for the given parameter.
* @dbpro DYN SET PARAMETER
* @category CORE
*/
void dynSetParameter(int parameter, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->setParameter(parameter, value);
}


/*
* @description This command allows you to get the current values for the global parameters of the Physics SDK. You can get the values for the default skin width, sleep velocities, bounce threshold, continuous collision detection, friction scaling and all the parameters used for setting up the debug renderer. 
* @param parameter ID number of the parameter that you wish to retrieve.
* @return Current value for the given parameter.
* @dbpro DYN GET PARAMETER
* @category CORE
*/
DWORDFLOAT dynGetParameter(int parameter){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		float value =  phyGlobStruct_->phy_->getParameter(parameter);
        RETURNCAST
}


/*
* @description Creates a material which you can then apply to actor shapes, producing the effect of the actor being composed of a certain material. 
* @param materialID - ID number you wish to assign to the material.
* @param restitution - The 'bounciness' of the material, a value between 0.0 and 1.0.
* @param staticFriction - The friction of the material while the shape is static, a value between 0.0 and 1.0.
* @param dynamicFriction -  The friction of the material while the shape is moving, a value between 0.0 and 1.0.
* @return True if successful, FALSE otherwise.
* @dbpro DYN MAKE MATERIAL
* @category CONSTRUCTION
*/
bool dynMakeMaterial(int materialID, float restitution, float staticFriction, float dynamicFriction){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->materialMap_.count(materialID) == 1){
			//exists
            return false;
		}
    #endif
	    NxMaterial *material = 0;
		material = phyGlobStruct_->phy_->makeMaterial(restitution, staticFriction, dynamicFriction);
		if(material){
			phyGlobStruct_->materialMap_[materialID] = material;
			return true;
		}
		else{
			return false;
		}
}

/*
* @description Sets the material of a particular shape in an actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param shapeIndex - Index number of the shape you wish to apply the material to, if the actor only has one shape the index will be 0.
* @param materialID - ID number of the material.
* @dbpro DYN SET MATERIAL
* @category MATERIAL
*/
void dynSetMaterial(int objectID, int shapeIndex, int materialID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetMaterial");
            return;
		}
		if(phyGlobStruct_->materialMap_.count(materialID) == 0){
			//non existent
            return;
		}
		if((unsigned int)shapeIndex > phyGlobStruct_->actorMap_[objectID]->getNbShapes() - 1){
			//non existent
            return;
		}
    #endif
		NxMaterial *material = phyGlobStruct_->materialMap_[materialID];
		int *id = new int();
		*id = materialID;
		material->userData = (void*)id;
		phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->setMaterial(phyGlobStruct_->materialMap_[materialID]->getMaterialIndex());
		//NOTE: release id
}

void dynSetMaterial(int objectID, int limbID, int shapeIndex, int materialID){
}

/*
* @description Gets the material ID of a particular shape in an actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param shapeIndex - Index number of the shape you wish to apply the material to, if the actor only has one shape the index will be 0.
* @return ID number of the material.
* @dbpro DYN GET MATERIAL
* @category MATERIAL
*/
int dynGetMaterial(int objectID, int shapeIndex){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetMaterial");
            return -1;
		}
		if((unsigned int)shapeIndex > phyGlobStruct_->actorMap_[objectID]->getNbShapes() - 1){
			//non existent
            return -1;
		}
    #endif
		NxActor *actor = phyGlobStruct_->actorMap_[objectID];
		NxMaterialIndex index = actor->getShapes()[shapeIndex]->getMaterial();
		NxMaterial *material = actor->getScene().getMaterialFromIndex(index);
		return *(int*)material->userData;
}


//#ifdef COMPILE_FOR_GDK

/*
* @description Returns the mass of the given actor, returns 0 for static actors.
* @param objectID - ID number of the object the actor represents in the simulation.
* @return The mass of the given actor.
* @dbpro DYN GET MASS
* @category RIGID BODY
*/
DWORDFLOAT dynGetMass(int objectID){
	#ifdef DEBUG
	    float ret = -1.0;
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return (DWORDFLOAT)ret;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynGetMass");
            return (DWORDFLOAT)ret;
		}
    #endif
		float value = phyGlobStruct_->actorMap_[objectID]->getMass();
        RETURNCAST
}

/*
* @description Returns the mass of the given actor, returns 0 for static actors, use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb/bone the actor represents in the simulation.
* @return The mass of the given actor.
* @dbpro DYN GET MASS
* @category RIGID BODY
*/
DWORDFLOAT dynGetMass(int objectID, int limbID){
	#ifdef DEBUG
	    float ret = -1.0;
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return (DWORDFLOAT)ret;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynGetMass");
		    return (DWORDFLOAT)ret;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynGetMass");
			return (DWORDFLOAT)ret;		
		}
    #endif
		float value = phyGlobStruct_->actorLimbMap_[objectID][limbID]->getMass();
        RETURNCAST
}


//#else
//DWORD dynGetMass(int objectID){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return -1;
//		}
//		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
//			Message::actorNotExist(objectID, "dynGetMass");
//            return -1;
//		}
//    #endif
//        float result = phyGlobStruct_->actorMap_[objectID]->getMass();
//		return *(DWORD*)&result;
//}
//#endif

/*
* @description Sets the mass of the given actor, mass must be positive and actor must be dynamic. This command does not update the inertial properties of the actor, to change the inertia tensor use the command that updates mass from shapes.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param mass - New mass of the given actor.
* @dbpro DYN SET MASS
* @category RIGID BODY
*/
void dynSetMass(int objectID, float mass){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetMass");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->setMass(mass);
}

/*
* @description Sets the mass of the given actor, mass must be positive and actor must be dynamic. This command does not update the inertial properties of the actor, to change the inertia tensor use the command that updates mass from shapes. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param mass - New mass of the given actor.
* @dbpro DYN SET MASS
* @category RIGID BODY
*/
void dynSetMass(int objectID, int limbID, float mass){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynSetMass");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynSetMass");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->setMass(mass);
}


/*
* @description  Recomputes a dynamic actor's mass properties from its shapes. Given a constant density or total mass, the actors mass properties can be recomputed using the shapes attached to the actor. If the actor has no shapes, then only the totalMass parameter can be used. If all shapes in the actor are trigger shapes (non-physical), the call will fail. The mass of each shape is either the shape's local density multiplied by the shape's volume or a directly specified shape mass. The inertia tensor, mass frame and center of mass will always be recomputed. If there are no shapes in the actor, the mass will be totalMass, and the mass frame will be set to the center of the actor. If you supply a non-zero total mass, the actor's mass and inertia will first be computed as above and then scaled to fit this total mass. If you supply a non-zero density, the actor's mass and inertia will first be computed as above and then scaled by this factor. Either totalMass or density must be non-zero and the actor must be dynamic.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param density - Density scale factor of the shapes belonging to the actor.
* @param totalMass - Total mass of the actor(or zero).
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN UPDATE MASS FROM SHAPES
* @category RIGID BODY
*/
bool dynUpdateMassFromShapes(int objectID, float density, float totalMass){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynUpdateMassFromShapes");
            return false;
		}
    #endif
		return phyGlobStruct_->actorMap_[objectID]->updateMassFromShapes(density, totalMass);
}

/*
* @description Allows you to switch between force modes, default is NX_FORCE.
* @param mode - NX_FORCE = 0, NX_IMPULSE = 1, NX_VELOCITY_CHANGE = 2, NX_SMOOTH_IMPULSE = 3, NX_SMOOTH_VELOCITY_CHANGE = 4, NX_ACCELERATION = 5.
* @dbpro DYN SET FORCE MODE
* @category CORE
*/
void dynSetForceMode(int mode){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    #endif
		phyGlobStruct_->forceMode = (NxForceMode)mode;
}

/*
* @description Applies a force(or impulse) defined in the global coordinate frame of the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @dbpro DYN ADD FORCE
* @category RIGID BODY
*/
void dynAddForce(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddForce");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addForce(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}

/*
* @description Applies a force(or impulse) defined in the global coordinate frame of the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @dbpro DYN ADD FORCE
* @category RIGID BODY
*/
void dynAddForce(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddForce");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddForce");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addForce(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}











/*
* @description Applies a force (or impulse) defined in the global coordinate frame, acting at a particular point in global coordinates, to the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD FORCE AT POS
* @category RIGID BODY
*/
void dynAddForceAtPos(int objectID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddForceAtPos");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addForceAtPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode );
}

/*
* @description Applies a force (or impulse) defined in the global coordinate frame, acting at a particular point in global coordinates, to the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD FORCE AT POS
* @category RIGID BODY
*/
void dynAddForceAtPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddForceAtPos");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddForceAtPos");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addForceAtPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode);
}







/*
* @description Applies a force (or impulse) defined in the global coordinate frame, acting at a particular point in local coordinates, to the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD FORCE AT LOCAL POS
* @category RIGID BODY
*/
void dynAddForceAtLocalPos(int objectID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddForceAtLocalPos");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addForceAtLocalPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode );
}

/*
* @description Applies a force (or impulse) defined in the global coordinate frame, acting at a particular point in local coordinates, to the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD FORCE AT LOCAL POS
* @category RIGID BODY
*/
void dynAddForceAtLocalPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddForceAtLocalPos");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddForceAtLocalPos");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addForceAtLocalPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode);
}










/*
* @description Applies a force (or impulse) defined in the actor local coordinate frame, acting at a particular point in global coordinates, to the actor. You can change the force mode with the 'set force mode' command. 
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD LOCAL FORCE AT POS
* @category RIGID BODY
*/
void dynAddLocalForceAtPos(int objectID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddLocalForceAtPos");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addLocalForceAtPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode );
}

/*
* @description Applies a force (or impulse) defined in the actor local coordinate frame, acting at a particular point in global coordinates, to the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD LOCAL FORCE AT POS
* @category RIGID BODY
*/
void dynAddLocalForceAtPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddLocalForceAtPos");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddLocalForceAtPos");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addLocalForceAtPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode);
}













/*
* @description Applies a force(or impulse) defined in the local coordinate frame of the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @dbpro DYN ADD LOCAL FORCE
* @category RIGID BODY
*/
void dynAddLocalForce(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddLocalForce");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addLocalForce(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}

















/*
* @description Applies a force(or impulse) defined in the local coordinate frame of the actor at a local point. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Force to be applied along the x axis.
* @param posY - Force to be applied along the y axis.
* @param posZ - Force to be applied along the z axis.
* @dbpro DYN ADD LOCAL FORCE AT LOCAL POS
* @category RIGID BODY
*/
void dynAddLocalForceAtLocalPos(int objectID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddLocalForceAtLocalPos");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addLocalForceAtLocalPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode);
}

/*
* @description Applies a force (or impulse) defined in the actor local coordinate frame, acting at a particular point in local coordinates, to the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @param posX - Position the force is to be applied to.
* @param posY - Position the force is to be applied to.
* @param posZ - Position the force is to be applied to.
* @dbpro DYN ADD LOCAL FORCE AT LOCAL POS
* @category RIGID BODY
*/
void dynAddLocalForceAtLocalPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddLocalForceAtLocalPos");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddLocalForceAtLocalPos");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addLocalForceAtLocalPos(NxVec3(x, y, z), NxVec3(posX, posY, posZ), phyGlobStruct_->forceMode);
}

















/*
* @description Applies an impulsive torque defined in the global coordinate frame of the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Torque to be applied about the x axis.
* @param y - Torque to be applied about the y axis.
* @param z - Torque to be applied about the z axis.
* @dbpro DYN ADD TORQUE
* @category RIGID BODY
*/
void dynAddTorque(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddTorque");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addTorque(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}

/*
* @description Applies an impulsive torque defined in the local coordinate frame of the actor. You can change the force mode with the 'set force mode' command.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Torque to be applied about the x axis.
* @param y - Torque to be applied about the y axis.
* @param z - Torque to be applied about the z axis.
* @dbpro DYN ADD LOCAL TORQUE
* @category RIGID BODY
*/
void dynAddLocalTorque(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynAddLocalTorque");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->addLocalTorque(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}


/*
* @description Applies a force(or impulse) defined in the local coordinate frame of the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Force to be applied along the x axis.
* @param y - Force to be applied along the y axis.
* @param z - Force to be applied along the z axis.
* @dbpro DYN ADD LOCAL FORCE
* @category RIGID BODY
*/
void dynAddLocalForce(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddLocalForce");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddLocalForce");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addLocalForce(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}

/*
* @description Applies an impulsive torque defined in the global coordinate frame of the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Torque to be applied about the x axis.
* @param y - Torque to be applied about the y axis.
* @param z - Torque to be applied about the z axis.
* @dbpro DYN ADD TORQUE
* @category RIGID BODY
*/
void dynAddTorque(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddTorque");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddTorque");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addTorque(NxVec3(x, y, z), phyGlobStruct_->forceMode );
}

/*
* @description Applies an impulsive torque defined in the local coordinate frame of the actor. You can change the force mode with the 'set force mode' command. Use this overload if the actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param x - Torque to be applied about the x axis.
* @param y - Torque to be applied about the y axis.
* @param z - Torque to be applied about the z axis.
* @dbpro DYN ADD LOCAL TORQUE
* @category RIGID BODY
*/
void dynAddLocalTorque(int objectID, int limbID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynAddLocalTorque");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynAddLocalTorque");
			return;		
		}

    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->addLocalTorque(NxVec3(x, y, z), phyGlobStruct_->forceMode);
}

/*
* @description This command allows you to check if any given actor is dynamic or static.
* @param objectID - ID number of the object the actor represents in the simulation.
* @return TRUE if dynamic, FALSE if static.
* @dbpro DYN IS DYNAMIC
* @category RIGID BODY
*/
bool dynIsDynamic(int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynIsDynamic");
            return false;
		}
    #endif
		return phyGlobStruct_->actorMap_[objectID]->isDynamic();
}

/*
* @description Sets the centre of mass relative to the actors local space.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param x - Position along the local x axis.
* @param y - Position along the local y axis.
* @param z - Position along the local z axis.
* @dbpro DYN SET CMASS LOC POS
* @category RIGID BODY
*/
void dynSetCMassLocPos(int objectID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynSetCMassLocPos");
            return;
		}
    #endif
		return phyGlobStruct_->actorMap_[objectID]->setCMassOffsetLocalPosition(NxVec3(x, y, z));;
}


/*
* @description This command allows you to change the object the given actor will update. The old ID number(s) no longer represent any actor in the simulation.
* @param newObjectID - ID number of the new object to be updated. If the actor is to now represent a limb, place the ID of the owning object here.
* @param newLimbID - ID number of the new limb to be updated, set to 0 if it is the whole object and not a limb that is to be represented by the actor in the simulation.
* @param oldObjectID - ID number of the object that the actor currently represents in the simulation. If the actor currently represents a limb then place the ID of the owning object here.
* @param oldLimbID - ID number of the limb that the actor currently represents in the simulation, set to 0 if it is the object that is currently represented by the actor and not a limb.
* @dbpro DYN SET TO NEW OBJECT
* @category RIGID BODY
*/
void dynSetToNewObject(int nObject, int nLimb, int oObject, int oLimb){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		//--CURRENT 
		//--If current actor represents object
		if(oLimb == 0){
		   if(phyGlobStruct_->actorMap_.count(oObject) == 0){
			   Message::actorNotExist(oObject, "dynSetToNewObject");
               return;
		   }
		}
		else{
     	   if(phyGlobStruct_->actorLimbMap_.count(oObject) == 0){
			   Message::actorNotExist(oObject, oLimb, "dynSetToNewObject");
               return;
		   }
		   if(phyGlobStruct_->actorLimbMap_[oObject].count(oLimb) == 0){
			   Message::actorNotExist(oObject, oLimb, "dynSetToNewObject");
               return;
		   }
		}
		//--NEW
		//--If new actor represents object
		if(nLimb == 0){
		   if(phyGlobStruct_->actorMap_.count(nObject)){
			   Message::actorAlreadyExist(nObject, "dynSetToNewObject");
               return;
		   }
		}
		else{
     	   if(phyGlobStruct_->actorLimbMap_.count(nObject)){
			   Message::actorAlreadyExist(nObject, nLimb, "dynSetToNewObject");
               return;
		   }
		   if(phyGlobStruct_->actorLimbMap_[nObject].count(nLimb) == 0){
			   Message::actorAlreadyExist(nObject, nLimb, "dynSetToNewObject");
               return;
		   }
		}
    #endif
		NxActor *actor;
		int wasLimb;
    //Get actor
		if(oLimb == 0){
			actor = phyGlobStruct_->actorMap_[oObject];
			phyGlobStruct_->actorMap_.erase(oObject);
			wasLimb = false;
		}
		else{
			actor = phyGlobStruct_->actorLimbMap_[oObject][oLimb];
			phyGlobStruct_->actorLimbMap_[oObject].erase(oLimb);
			phyGlobStruct_->actorLimbMap_.erase(oObject);
			wasLimb = true;
		}
    //Set user data to new actor
		ActorSync *as = (ActorSync*)actor->userData;
		as->object_ = nObject;
		as->limb_   = nLimb;

	//If limb to object, change update.  If object to limb, change object
		if(nLimb == 0){
			if(wasLimb){
                //limb to object
                delete as;
				as = new ObjectFromActor(nObject);
			}
			else{//object to object
                phyGlobStruct_->actorMap_[nObject] = actor;
			}
		}
		else{
			if(!wasLimb){
               //object to limb
				NxVec3 scale = as->scale_;
				delete as;
				//REMOVE as FROM LIST
				as = new LimbFromActor(nObject, nLimb, scale); 
			}
			else{//limb to limb

			}

		}
	//If object to limb, change object

	//Place new actor in map

}

//bool dynMakeForceField(){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return false;
//		}
//    #endif
//	phyGlobStruct_->phy_->makeForceField();
//	return true;
//}


/*
* @description This command creates a descriptor for a height field. You can then place the height data in this descriptor before creating a height field. You should delete this descriptor after creating your height field as the height data is copied and no longer needed. 
* @param ID - ID number you wish to assign to the height field descriptor.
* @param columns - Number of columns.
* @param rows - Number of rows.
* @param verticalExtent - This is the extent to which the collision shape will be effective along the y axis, unlike triangle meshes, height fields have a volume which can detect collisions even if the simulation timestep takes an actor completely past the height field surface. A good value for this parameter might be -1000. You should set thi sto zero if you are using a thickness value instead. 
* @param thickness - This value sets the thickness of the height field, unlike the verticla extent, the depth volume matches the shape of the height field. You should set this to 0 if you are using a vertical extent value instead.   
* @param convexEdgeThreshold - Larger values for this parameter will result in fewer edge contacts, a value of 0 is acceptable.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE HEIGHT FIELD DESC
* @category CONSTRUCTION
*/
bool dynMakeHeightFieldDesc(int ID, int columns, int rows, float verticalExtent, float thickness, float convexEdgeThreshold){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->heightFieldDescMap_.count(ID) == 1){
			Message::meshAlreadyExists(ID, "dynMakeHeightFieldDesc");
			return false;
		}
	#endif
    NxHeightFieldDesc *heightFieldDesc = new NxHeightFieldDesc();

	//-- new
	heightFieldDesc->setToDefault();
   

	heightFieldDesc->nbColumns           = columns;
	heightFieldDesc->nbRows              = rows;
	heightFieldDesc->verticalExtent      = verticalExtent;
	heightFieldDesc->thickness           = thickness;
	heightFieldDesc->convexEdgeThreshold = convexEdgeThreshold;
   


    //Allocate storage for samples.    
	heightFieldDesc->samples             = new NxU32[columns*rows];    
	heightFieldDesc->sampleStride        = sizeof(NxU32);

 //   //-- New(initialise) // not needed, it was hole problem
 //   NxU8* currentByte = (NxU8*)heightFieldDesc->samples;    
 //   for (NxU32 row = 0; row < rows; row++)        
	//{        
	//	for (NxU32 column = 0; column < columns; column++)
	//	{            
	//	    NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
	//        currentSample->height         = 15; //Desired height value. Signed 16 bit integer.            
	//        currentSample->materialIndex0 = 0;            
	//        currentSample->materialIndex1 = 0; 
	//        currentSample->tessFlag = 0;            
	//        currentByte += heightFieldDesc->sampleStride;            
	//	}        
	//}


	phyGlobStruct_->heightFieldDescMap_[ID] = heightFieldDesc;
	return true;
}


/*
* @description This command allows you to set the height data for the given height field descriptor.
* @param ID - ID number of the height field descriptor.
* @param column - Column index value.
* @param row - Row index value.
* @param height - Height value for the given location. 
* @dbpro DYN HEIGHT FIELD DESC SET DATA
* @category HEIGHTFIELD
*/
void dynHeightFieldDescSetData(int heightFieldDescID, int row, int column, int height){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->heightFieldDescMap_.count(heightFieldDescID) == 0){
			Message::meshActorNotExist(heightFieldDescID, "dynHeightFieldDescSetData");
			return;
		}
	#endif
		NxHeightFieldDesc *desc = phyGlobStruct_->heightFieldDescMap_[heightFieldDescID];
 
		//-- Problem with DBPro release?
		//-- Try unsigned int's
		if((unsigned int)row >= desc->nbRows || (unsigned int)column >= desc->nbColumns){
           return;
		}
		if((unsigned int)row < 0 || (unsigned int)column < 0){
           return;
		}

		int arrPos = (row) * desc->nbColumns + (column);                 //((row-1) * (column-1)) + (column-1); 
        NxU8* currentByte = (NxU8*)desc->samples;
		currentByte += arrPos * desc->sampleStride;
		NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
		currentSample->height = height;
}


/*
* @description This command allows you to set the height data for the given height field descriptor. This overload allows you to set material indices and a tesselation flag.
* @param ID - ID number of the height field descriptor.
* @param column - Column index value.
* @param row - Row index value.
* @param height - Height value for the given location. 
* @param materialIndex0 - The low 7 bits of the triangle material index, these are concatenated with the high bits you can set when creating the actor to produce an index into the scenes material array. This value determines the value for the lower of the quads two triangles.
* @param materialIndex1 - The low 7 bits of the triangle material index, these are concatenated with the high bits you can set when creating the actor to produce an index into the scenes material array. This value determines the value for the higher of the quads two triangles..
* @param tessFlag - This flag specifies which way the quad is split. 
* @dbpro DYN HEIGHT FIELD DESC SET DATA
* @category HEIGHTFIELD
*/
void dynHeightFieldDescSetData(int heightFieldDescID, int row, int column, int height, int materialIndex0, int materialIndex1, int tessFlag){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->heightFieldDescMap_.count(heightFieldDescID) == 0){
			Message::meshActorNotExist(heightFieldDescID, "dynHeightFieldDescSetData");
			return;
		}
	#endif
		NxHeightFieldDesc *desc = phyGlobStruct_->heightFieldDescMap_[heightFieldDescID];	
		if((unsigned int)row >= desc->nbRows || (unsigned int)column >= desc->nbColumns){
           return;
		}
		if((unsigned int)row < 0 || (unsigned int)column < 0){
           return;
		}
		int arrPos = (row) * desc->nbColumns + (column);                 //((row-1) * (column-1)) + (column-1); 
        NxU8* currentByte = (NxU8*)desc->samples;
		currentByte += arrPos * desc->sampleStride;
		NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
		currentSample->height = height;
		currentSample->materialIndex0 = materialIndex0;
		currentSample->materialIndex1 = materialIndex1;
		currentSample->tessFlag = tessFlag;
}


/*
* @description This command creates a height field from the given descriptor. No collision shape/actor is created at this point, the heightfield should be instanced to create an actor.
* @param ID - ID number you wish to assign to the height field.
* @param heightFieldDescID - ID number of the height field descriptor.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE HEIGHT FIELD
* @category CONSTRUCTION
*/
bool dynMakeHeightField(int ID, int heightFieldDescID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->heightFieldDescMap_.count(heightFieldDescID) == 0){
			Message::meshActorNotExist(heightFieldDescID, "dynMakeHeightField");
			return false;
		}
		if(phyGlobStruct_->heightFieldMap_.count(ID) == 1){
			Message::meshAlreadyExists(ID, "dynMakeHeightField");
			return false;
		}
	#endif
		NxHeightField *hf = 0;
		hf = phyGlobStruct_->phy_->makeHeightField(phyGlobStruct_->heightFieldDescMap_[heightFieldDescID]);
		if(!hf)
		   return false;	
		phyGlobStruct_->heightFieldMap_[ID] = hf;
		return true;
}


/*
* @description After creating a height field descriptor, filling in the data and then creating a height field you can finally create your height field actor using this command.
* @param ID - ID number of the object that the height field will represent. As with most static actors there is a very loose connection between actor and object, they share an ID number and not much else, you do need an object to exist though as it uses its position upon creation.
* @param heightFieldID - ID number of the height field you wish to instance.
* @param heightScale - Scale value for the height.
* @param rowScale - Scale value for the rows.
* @param columnScale - Scale value for the columns.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN INSTANCE HEIGHT FIELD
* @category CONSTRUCTION
*/
bool dynInstanceHeightField(int ID, int heightFieldID, float heightScale, float rowScale, float columnScale){
    return dynInstanceHeightField(ID, heightFieldID, heightScale, rowScale, columnScale, 0, -1); 
//#ifdef DEBUG
//	if(!phyGlobStruct_->phy_){
//		Message::ignitionError();
//		return false;
//	}
//	if(phyGlobStruct_->heightFieldMap_.count(heightFieldID) == 0){
//		Message::meshActorNotExist(heightFieldID, "dynInstanceHeightField");
//		return false;
//	}
//	if(phyGlobStruct_->actorMap_.count(ID) == 1){
//		Message::actorAlreadyExist(ID, "dynInstanceHeightField");
//		return false;
//	}
//	if(!DBError::objectExists(ID, "dynInstanceHeightField")){
//        return false;
//	}
//#endif
//
//
//    sObject* obj = dbGetObject(ID);
//    NxHeightFieldShapeDesc heightFieldShapeDesc;
//	heightFieldShapeDesc.heightField     = phyGlobStruct_->heightFieldMap_[heightFieldID];
//    heightFieldShapeDesc.heightScale     = heightScale;
//    heightFieldShapeDesc.rowScale        = rowScale;
//    heightFieldShapeDesc.columnScale     = columnScale;
//    heightFieldShapeDesc.materialIndexHighBits = 0;
//    //heightFieldShapeDesc.holeMaterial = 2;
//    NxActorDesc actorDesc;
//    actorDesc.shapes.pushBack(&heightFieldShapeDesc);
//	actorDesc.globalPose.t = NxVec3(obj->position.vecPosition.x, obj->position.vecPosition.y, obj->position.vecPosition.z);//pos;
//	NxActor* actor = 0;
//	actor = phyGlobStruct_->phy_->currentScene_->createActor(actorDesc);
//    if(!actor)
//		return false;
//
//	//actor->userData
//	phyGlobStruct_->actorMap_[ID] = actor;
//	return true;
}


/*
* @description After creating a height field descriptor, filling in the data and then creating a height field you can finally create your height field actor using this command. This overload allows to to set a materialIndexhighBits value and a hole material.
* @param ID - ID number of the object that the height field will represent. As with most static actors there is a very loose connection between actor and object, they share an ID number and not much else, you do need an object to exist though as it uses its position upon creation.
* @param heightFieldID - ID number of the height field you wish to instance.
* @param heightScale - Scale value for the height.
* @param rowScale - Scale value for the rows.
* @param columnScale - Scale value for the columns.
* @param materialIndexHighBits - The 9 high bits of this number are used to complete the material indices, the low 7 bits must be 0.
* @param holeMaterial - The material index which designated holes in the height field.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN INSTANCE HEIGHT FIELD
* @category CONSTRUCTION
*/
bool dynInstanceHeightField(int ID, int heightFieldID, float heightScale, float rowScale, float columnScale, int materialIndexHighBits, int holeMaterial){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->heightFieldMap_.count(heightFieldID) == 0){
			Message::meshActorNotExist(heightFieldID, "dynInstanceHeightField");
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(ID) == 1){
			Message::actorAlreadyExist(ID, "dynInstanceHeightField");
			return false;
		}
		if(!DBError::objectExists(ID, "dynInstanceHeightField")){
            return false;
		}
	#endif
    ObjectFromActor *aSync = new ObjectFromActor(ID);
    sObject* obj = dbGetObject(ID);
    NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField     = phyGlobStruct_->heightFieldMap_[heightFieldID];
    heightFieldShapeDesc.heightScale     = heightScale;
    heightFieldShapeDesc.rowScale        = rowScale;
    heightFieldShapeDesc.columnScale     = columnScale;
    heightFieldShapeDesc.materialIndexHighBits = materialIndexHighBits;
    heightFieldShapeDesc.holeMaterial = holeMaterial;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.globalPose.t = NxVec3(obj->position.vecPosition.x, obj->position.vecPosition.y, obj->position.vecPosition.z);//pos;
	NxActor* actor = 0;
	actor = phyGlobStruct_->phy_->currentScene_->createActor(actorDesc);
	if(!actor){
		delete aSync;
		aSync = 0;
		return false;
	}
    actor->userData = (void*)aSync;
	phyGlobStruct_->actorMap_[ID] = actor;
    phyGlobStruct_->actorSyncList_.push_back(aSync);
	return true;
}

/*
* @description Releases the height field descriptor from memory, it is important that you call this function after you have created the height field as all the height field data is copied to the height field and is no longer needed.
* @param ID - ID number of the height field descriptor.
* @dbpro DYN DELETE HEIGHT FIELD DESC
* @category CONSTRUCTION
*/
void dynDeleteHeightFieldDesc(int ID){ //release samples
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->heightFieldDescMap_.count(ID) == 0){
			Message::meshActorNotExist(ID, "dynDeleteHeightFieldDesc");
			return;
		}
	#endif
	NxHeightFieldDesc* desc = phyGlobStruct_->heightFieldDescMap_[ID];
	if(desc->samples){ 
		delete[] desc->samples;
	}
	delete desc;
	phyGlobStruct_->heightFieldDescMap_.erase(ID);
}


/*
* @description Releases the height field from memory, will fail if any actors which are instances of this height field still exist.
* @param ID - ID number of the height field.
* @dbpro DYN DELETE HEIGHT FIELD
* @category CONSTRUCTION
*/
void dynDeleteHeightField(int ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->heightFieldMap_.count(ID) == 0){
			Message::meshActorNotExist(ID, "dynDeleteHeightField");
			return;
		}
	#endif
	NxHeightField* hf = phyGlobStruct_->heightFieldMap_[ID];
	if(hf->getReferenceCount() > 0){
       	Message::errorBox(string("Height field not deleted, delete all actors that reference the height field first."), string("dynDeleteHeightField"));
		return;
	}
	phyGlobStruct_->phy_->physicsSDK_->releaseHeightField(*hf);
	phyGlobStruct_->heightFieldMap_.erase(ID);
}

/*
* @description Creates a character controller for the given object, character controllers are not controlled by the simulation but by the user. 
* @param objectID - ID number of the object you wish the controller to represent.
* @param yOffset - Most character models have their local origin at their feet, use this command to raise the controller to the same position as your character. This value will usually be half the characters height.
* @return - TRUE if successful, FALSE otherwise. 
* @dbpro DYN MAKE CONTROLLER CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeControllerCapsule(int objectID, float yOffset){
	return dynMakeControllerCapsule(objectID, yOffset, 45.0f, 1.0f);
}

/*
* @description Creates a character controller for the given object, character controllers are not controlled by the simulation but by the user. 
* @param objectID - ID number of the object you wish the controller to represent.
* @param yOffset - Most character models have their local origin at their feet, use this command to raise the controller to the same position as your character. This value will usually be half the characters height.
* @return - TRUE if successful, FALSE otherwise. 
* @dbpro DYN MAKE CONTROLLER BOX
* @category CONSTRUCTION
*/
bool dynMakeControllerBox(int objectID, float yOffset){
	return dynMakeControllerBox(objectID, yOffset, 45.0f, 1.0f);
}

/*
* @description Creates a character controller for the given object, character controllers are not controlled by the simulation but by the user. 
* @param objectID - ID number of the object you wish the controller to represent.
* @param yOffset - Most character models have their local origin at their feet, use this command to raise the controller to the same position as your character. This value will usually be half the characters height.
* @param slopeLimit - Maximum slope, in degrees, that the controller will be able to climb.
* @param stepOffset - Maximum step height that the controller will be able to climb.
* @return - TRUE if successful, FALSE otherwise. 
* @dbpro DYN MAKE CONTROLLER CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeControllerCapsule(int objectID, float yOffset, float slopeLimit, float stepOffset){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeControllerCapsule")){
            return false;
		}
		if(phyGlobStruct_->controllerMap_.count(objectID) == 1){
			Message::actorAlreadyExist(objectID, "dynMakeControllerCapsule");
			return false;
		}
	#endif
   //-- Collect object data
   sObject *obj = dbGetObject(objectID);
   D3DXVECTOR3 objPos = obj->position.vecPosition;
   NxExtendedVec3 pos = NxExtendedVec3(objPos.x, objPos.y + yOffset, objPos.z);
   NxVec3 scale       = (NxVec3)obj->position.vecScale;
   NxVec3 extents = (NxVec3)(obj->collision.vecMax - obj->collision.vecMin);
  
   //-- Create controller desc
   NxCapsuleControllerDesc desc;
   desc.position = pos;
   desc.height = scale.y * phyGlobStruct_->phyScaleMultiplier_ * extents.y / 2;
   float averageScale = (scale.x + scale.z) / 2;
   desc.radius = averageScale * phyGlobStruct_->phyScaleMultiplier_ * (extents.x + extents.z) / 4;
   desc.upDirection = NX_Y;
   desc.callback = phyGlobStruct_->cContact_; 
   desc.slopeLimit = (NxF32)(slopeLimit / 180 * 3.142);
   desc.stepOffset = stepOffset;
 
   if(!desc.isValid()){
       return false;
   }

   NxController *controller = 0;
   controller = phyGlobStruct_->phy_->cManager_->createController(phyGlobStruct_->phy_->currentScene_, desc);
   if(controller){
	   //Place in map
	   phyGlobStruct_->controllerMap_[objectID] = controller;
       //aSync
	   ControllerSync *aSync = new ControllerSync();
       aSync->object_ = objectID;
	   aSync->yOffset_ = yOffset;
       desc.userData = (void*)aSync;
	   phyGlobStruct_->actorSyncList_.push_back(aSync);
	   controller->getActor()->userData = aSync;
       return true;
   }
   else{
       return false;
   }
}

	
/*
* @description Creates a character controller for the given object, character controllers are not controlled by the simulation but by the user. 
* @param objectID - ID number of the object you wish the controller to represent.
* @param yOffset - Most character models have their local origin at their feet, use this command to raise the controller to the same position as your character. This value will usually be half the characters height.
* @param slopeLimit - Maximum slope, in degrees, that the controller will be able to climb.
* @param stepOffset - Maximum step height that the controller will be able to climb.
* @return - TRUE if successful, FALSE otherwise. 
* @dbpro DYN MAKE CONTROLLER BOX
* @category CONSTRUCTION
*/
bool dynMakeControllerBox(int objectID, float yOffset, float slopeLimit, float stepOffset){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeControllerBox")){
            return false;
		}
		if(phyGlobStruct_->controllerMap_.count(objectID) == 1){
			Message::actorAlreadyExist(objectID, "dynMakeControllerBox");
			return false;
		}
	#endif
   //-- Collect object data
   sObject *obj = dbGetObject(objectID);
   D3DXVECTOR3 objPos = obj->position.vecPosition;
   NxExtendedVec3 pos = NxExtendedVec3(objPos.x, objPos.y + yOffset, objPos.z);
   NxVec3 extents;
   NxVec3 dim = (NxVec3)(obj->collision.vecMax - obj->collision.vecMin);
   extents.y = dim.y;
   float average = (dim.x + dim.z) / 2;
   extents.x = average;
   extents.z = average;
   NxVec3 scale       = (NxVec3)obj->position.vecScale;
   extents.arrayMultiply(extents, scale);

   //-- Create controller desc
   NxBoxControllerDesc desc;
   desc.position = pos;
   desc.extents = phyGlobStruct_->phyScaleMultiplier_ * extents / 2;
   desc.upDirection = NX_Y;
   desc.slopeLimit = (NxF32)(slopeLimit / 180 * 3.142);
   desc.stepOffset = stepOffset;
   desc.callback = phyGlobStruct_->cContact_;

   if(!desc.isValid()){
       return false;
   }

   NxController *controller = 0;
   controller = phyGlobStruct_->phy_->cManager_->createController(phyGlobStruct_->phy_->currentScene_, desc);
  
   if(controller){
	   //Place in map
	   phyGlobStruct_->controllerMap_[objectID] = controller;
       //aSync
	   ControllerSync *aSync = new ControllerSync();
       aSync->object_ = objectID;
	   aSync->yOffset_ = yOffset;
       desc.userData = (void*)aSync;
	   phyGlobStruct_->actorSyncList_.push_back(aSync);
	   controller->getActor()->userData = aSync;
       return true;
   }
   else{
       return false;
   }
}



/*
* @description This command allows you to set the maximum step height that the controller can climb. 
* @param objectID - ID number of the object the controller represents in the simulation. 
* @param stepOffset - Maximum step height that the controller will be able to climb.
* @dbpro DYN CONTROLLER SET STEP OFFSET
* @category CONTROLLER
*/
void dynControllerSetStepOffset(int ID, float stepOffset){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerSetStepOffset");
			return;
		}
	#endif
		phyGlobStruct_->controllerMap_[ID]->setStepOffset(stepOffset);
}

//Weird - Can't do this
//void dynControllerSetSlopeLimit(int ID, float slopeLimit){
//    #ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return false;
//		}
//		if(phyGlobStruct_->controllerMap_.count(objectID) == 0){
//			Message::actorNotExist(objectID, "dynControllerSetSlopeLimitt");
//			return false;
//		}
//	#endif
//		NxBoxController* controller = (NxBoxController*)phyGlobStruct_->controllerMap_[ID];
//		controller->
//}


/*
* @description This command allows you to change the size of the given box controller. 
* @param objectID - ID number of the object the controller represents in the simulation. 
* @param x - New size along the x axis.
* @param y - New size along the y axis.
* @param z - New size along the z axis.
* @dbpro DYN CONTROLLER BOX SET EXTENTS
* @category CONTROLLER
*/
void dynControllerBoxSetExtents(int ID, float x, float y, float z){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerBoxSetExtents");
			return;
		}
	#endif
        NxController* c = phyGlobStruct_->controllerMap_[ID];
		if(c->getType() == NX_CONTROLLER_BOX){
		   NxBoxController *controller = (NxBoxController*)c;
		   controller->setExtents(NxVec3(x, y, z));
		}
}

/*
* @description This command allows you to change the radius of the given capsule controller. 
* @param objectID - ID number of the object the controller represents in the simulation. 
* @param radius - New radius value.
* @dbpro DYN CONTROLLER CAPSULE SET RADIUS
* @category CONTROLLER
*/
void dynControllerCapsuleSetRadius(int ID, float radius){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerCapsuleSetRadius");
			return;
		}
	#endif
        NxController* c = phyGlobStruct_->controllerMap_[ID];
		if(c->getType() == NX_CONTROLLER_CAPSULE){
		   NxCapsuleController *controller = (NxCapsuleController*)c;
		   controller->setRadius(radius);
		}
}

/*
* @description This command allows you to change the height of the given capsule controller. 
* @param objectID - ID number of the object the controller represents in the simulation. 
* @param height - New height value.
* @dbpro DYN CONTROLLER CAPSULE SET HEIGHT
* @category CONTROLLER
*/
void dynControllerCapsuleSetHeight(int ID, float height){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerCapsuleSetHeight");
			return;
		}
	#endif
        NxController* c = phyGlobStruct_->controllerMap_[ID];
		if(c->getType() == NX_CONTROLLER_CAPSULE){
		   NxCapsuleController *controller = (NxCapsuleController*)c;
		   controller->setHeight(height);
		}
}

/*
* @description This command allows you to change the y offset of the given controller. 
* @param objectID - ID number of the object the controller represents in the simulation. 
* @param height - New y offset value.
* @dbpro DYN CONTROLLER SET Y OFFSET
* @category CONTROLLER
*/
void dynControllerSetYOffset(int ID, float yOffset){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerSetYOffset");
			return;
		}
	#endif
		ControllerSync *cSync = 0;
		cSync = (ControllerSync*)phyGlobStruct_->controllerMap_[ID]->getActor()->userData;
		if(cSync){
			cSync->yOffset_ = yOffset;
		}
}


//
// *
// * @description This will move your character the given amount on each axis in global space. 
// * @param objectID - ID number of the controller.
// * @param x - Distance to move along the x axis.
// * @param y - Distance to move along the y axis.
// * @param z - Distance to move along the z axis.
// * @dbpro DYN CONTROLLER MOVE
// *

//void dynControllerMove(int ID, float x, float y, float z){
//    #ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return;
//		}
//		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
//			Message::actorNotExist(ID, "dynControllerMove");
//			return;
//		}
//	#endif
//	NxU32 cf = 0; 
//	phyGlobStruct_->controllerMap_[ID]->move(NxVec3(x, y, z), 1, 0, cf);
//}

/*
* @description This will move your character the given amount on each axis in global space. Return the collision flag which gives you information about current collisions around the controller.  
* @param objectID - ID number of the controller.
* @param x - Distance to move along the x axis.
* @param y - Distance to move along the y axis.
* @param z - Distance to move along the z axis.
* @return Flag which gives you information on current contacts above, below, to the sides or a combination of the three.
* @dbpro DYN CONTROLLER MOVE
* @category CONTROLLER
*/
int dynControllerMove(int ID, float x, float y, float z){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerMove");
			return -1;
		}
	#endif
	NxU32 cf = 0; 
	//v1.1 active groups from 1 to ?
	phyGlobStruct_->controllerMap_[ID]->move(NxVec3(x, y, z), 1, 0, cf);
	return cf;
}


/*
* @description This will move your character the given amount on each axis in global space. Return the collision flag which gives you information about current collisions around the controller.  
* @param objectID - ID number of the controller.
* @param x - Distance to move along the x axis.
* @param y - Distance to move along the y axis.
* @param z - Distance to move along the z axis.
* @param collisionGroups - Combination of bits determining which collision groups should be used.
* @return Flag which gives you information on current contacts above, below, to the sides or a combination of the three.
* @dbpro DYN CONTROLLER MOVE
* @category CONTROLLER
*/
int dynControllerMove(int ID, float x, float y, float z, unsigned int activeGroups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerMove");
			return -1;
		}
	#endif
	NxU32 cf = 0; 
	//v1.1 active groups from 1 to ?
	phyGlobStruct_->controllerMap_[ID]->move(NxVec3(x, y, z), activeGroups, 0, cf);
	return cf;
}

/*
* @description Controllers use caching to speed up collision detection, therefore you must call this function if any static actors have been moved.
* @param objectID - ID number of the controller.
* @dbpro DYN CONTROLLER REPORT SCENE CHANGED
* @category CONTROLLER
*/
void dynControllerReportSceneChanged(int ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynControllerReportSceneChanged");
			return;
		}
	#endif
	phyGlobStruct_->controllerMap_[ID]->reportSceneChanged();
}


/*
* @description This will release the controller from the simulation. 
* @param ID - ID number of the controller.
* @dbpro DYN DELETE CONTROLLER
* @category CONSTRUCTION
*/
void dynDeleteController(int ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->controllerMap_.count(ID) == 0){
			Message::actorNotExist(ID, "dynDeleteController");
			return;
		}
	#endif
	NxController *controller = phyGlobStruct_->controllerMap_[ID];
	//-- Release user data
	if(controller->getActor()->userData){
		//delete (ControllerSync*)controller->getActor()->userData;
        //controller->getActor()->userData = 0;
		ActorSync* as = (ActorSync*)controller->getActor()->userData;
        phyGlobStruct_->actorSyncList_.remove(as);
		delete as;
		as = 0;
	}
	//-- Release controller
	phyGlobStruct_->phy_->cManager_->releaseController(*controller);
	//-- Remove from map
	phyGlobStruct_->controllerMap_.erase(ID);
}

/*
* @description Controllers can collect two types of contact hit data, this command allows you to start collecting controller-controller contacts. Be sure to process this data otherwise you it will take up more and more memory as the hits build up.
* @param collect - TRUE to collect, FALSE(default) otherwise.
* @dbpro DYN CONTROLLER COLLECT C HITS
* @category CONTROLLER
*/
void dynControllerCollectCHits(bool collect){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
#endif
	phyGlobStruct_->cContact_->collectControllerData_ = collect;
}

/*
* @description Controllers can collect two types of contact hit data, this command allows you to start collecting controller-shape contacts. Be sure to process this data otherwise you it will take up more and more memory as the hits build up.
* @param collect - TRUE to collect, FALSE(default) otherwise.
* @dbpro DYN CONTROLLER COLLECT S HITS
* @category CONTROLLER
*/
void dynControllerCollectSHits(bool collect){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
#endif
	phyGlobStruct_->cContact_->collectShapeData_= collect;
}



/*
* @description This command collects controller-controller contact data, you should work through all the data until it returns 0, meaning there is no more data. Prior to using this command you must inform the simulation to wish to collect this data with the 'dyn controller collect c hits' command. 
* @return Amount of hits waiting to be processed.
* @dbpro DYN CONTROLLER C HIT GET DATA
* @category CONTROLLER
*/
int dynControllerCHitGetData(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return 0;
	}
#endif
	return phyGlobStruct_->cContact_->getControllerHitData();
}


/*
* @description This command returns the object ID of the controller associated with the current hit data. 
* @return ID number of the object that the controller represents in the simulation.
* @dbpro DYN CONTROLLER C HIT GET CONTROLLER A
* @category CONTROLLER
*/
int dynControllerCHitGetControllerA(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return -1;
	}
#endif
	if(phyGlobStruct_->cContact_->currentControllerHit_){
	   ActorSync* as = (ActorSync*)phyGlobStruct_->cContact_->currentControllerHit_->controller->getActor()->userData;
	   if(as)
	     return as->object_;
	}
	return 0;
}

/*
* @description This command returns the object ID of the second controller associated with the current hit data. 
* @return ID number of the object that the controller represents in the simulation.
* @dbpro DYN CONTROLLER C HIT GET CONTROLLER B
* @category CONTROLLER
*/
int dynControllerCHitGetControllerB(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return -1;
	}
#endif
	if(phyGlobStruct_->cContact_->currentControllerHit_){
	   ActorSync* as = (ActorSync*)phyGlobStruct_->cContact_->currentControllerHit_->other->getActor()->userData;
	   if(as)
	      return as->object_;
	}
	return 0;
}


/*
* @description This command collects controller-shape contact data, you should work through all the data until it returns 0, meaning there is no more data. Prior to using this command you must inform the simulation to wish to collect this data with the 'dyn controller collect s hits' command. 
* @return Amount of hits waiting to be processed.
* @dbpro DYN CONTROLLER S HIT GET DATA
* @category CONTROLLER
*/
int dynControllerSHitGetData(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return 0;
	}
#endif
	return phyGlobStruct_->cContact_->getShapeHitData();
}

/*
* @description This command returns the object ID of the owning actor of the shape associated with the current hit data. 
* @return ID number of the object that the actor represents in the simulation.
* @dbpro DYN CONTROLLER S HIT GET SHAPE
* @category CONTROLLER
*/
int dynControllerSHitGetShape(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return 0;
	}
#endif
	if(phyGlobStruct_->cContact_->currentShapeHit_){
	   ActorSync* as = (ActorSync*)phyGlobStruct_->cContact_->currentShapeHit_->shape->getActor().userData;
	   if(as)
	     return as->object_;
	}
	return 0;
}

/*
* @description This command provides you with the direction of the current hit, you can then use this information to add a force to the actor that has been hit. 
* @param vector3ID The hit direction will be placed in the vector with this ID.
* @dbpro DYN CONTROLLER S HIT GET DIRECTION
* @category CONTROLLER
*/
void dynControllerSHitGetDirection(int vector3ID){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
#endif
	if(phyGlobStruct_->cContact_->currentShapeHit_){
	   NxVec3 dir = phyGlobStruct_->cContact_->currentShapeHit_->dir;
	   dbSetVector3(vector3ID, dir.x, dir.y, dir.z);
	}
}

/*
* @description This command provides you with the length of the current hit. 
* @return Length of the current hit.
* @dbpro DYN CONTROLLER S HIT GET LENGTH
* @category CONTROLLER
*/
DWORDFLOAT dynControllerSHitGetLength(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return 0;
	}
#endif 
	if(phyGlobStruct_->cContact_->currentShapeHit_){
	    float value = phyGlobStruct_->cContact_->currentShapeHit_->length;
	    RETURNCAST
	}
	else
		return 0;
}

/*
* @description This command returns the object ID of the controller associated with the current hit data. 
* @return ID number of the object that the controller represents in the simulation.
* @dbpro DYN CONTROLLER S HIT GET CONTROLLER
* @category CONTROLLER
*/
int dynControllerSHitGetController(){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return -1;
	}
#endif
	if(phyGlobStruct_->cContact_->currentShapeHit_){
	   ActorSync* as = (ActorSync*)phyGlobStruct_->cContact_->currentShapeHit_->controller->getActor()->userData;
	   return as->object_;
	}
	else return 0;
}

/*
* @description This command provides you with the world normal of the current hit. 
* @param vector3ID The world normal will be placed in the vector with this ID.
* @dbpro DYN CONTROLLER S HIT GET WORLD NORMAL
* @category CONTROLLER
*/
void dynControllerSHitGetWorldNormal(int vector3ID){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
#endif 
	if(phyGlobStruct_->cContact_->currentShapeHit_){
		NxVec3 wn = phyGlobStruct_->cContact_->currentShapeHit_->worldNormal;
	    dbSetVector3(vector3ID, wn.x, wn.y, wn.z);
	}
}

/*
* @description This command provides you with the world position of the current hit. 
* @param vector3ID The world position will be placed in the vector with this ID.
* @dbpro DYN CONTROLLER S HIT GET WORLD POS
* @category CONTROLLER
*/
void dynControllerSHitGetWorldPos(int vector3ID){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
#endif 
	if(phyGlobStruct_->cContact_->currentShapeHit_){
		NxExtendedVec3 wp = phyGlobStruct_->cContact_->currentShapeHit_->worldPos;
	    dbSetVector3(vector3ID, wp.x, wp.y, wp.z);
	}
}

/*
* @description Sets the position of the character controller, does not check for collisions. Use this to reset/teleport your character. 
* @param controllerID - ID number of the object the controller represents in the simulation.
* @param x - New position along the x axis.
* @param y - New position along the y axis.
* @param z - New position along the z axis.
* @dbpro DYN CONTROLLER SET POSITION
* @category CONTROLLER
*/
void dynControllerSetPosition(int controllerID, float x, float y, float z){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
	if(phyGlobStruct_->controllerMap_.count(controllerID) == 0){
		Message::actorNotExist(controllerID, "dynControllerSetPosition");
		return;
	}
#endif
	phyGlobStruct_->controllerMap_[controllerID]->setPosition(NxExtendedVec3(x, y, z));
}

/*
* @description Fetches the position of the given character controller. 
* @param controllerID - ID number of the object the controller represents in the simulation.
* @param vector3ID - ID number of the vector to be used to hold the position data.
* @dbpro DYN CONTROLLER GET POSITION
* @category CONTROLLER
*/
void dynControllerGetPosition(int controllerID, int vector3ID){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
	if(phyGlobStruct_->controllerMap_.count(controllerID) == 0){
		Message::actorNotExist(controllerID, "dynControllerGetPosition");
		return;
	}
#endif
	NxExtendedVec3 pos = phyGlobStruct_->controllerMap_[controllerID]->getPosition();
	dbSetVector3(vector3ID, pos.x, pos.y, pos.z);
}

/*
* @description This command allows you to set whether the actors in the given two shape groups collide with each other, all shapes start out in group 0 and all groups are enabled by default.
* @param groupA - The first group of the pair you wish to set collision for.
* @param groupB - the second group of the pair you wish to set collisions for.
* @param enable - TRUE enables collisions, FALSE disables collisions.
* @dbpro DYN GROUP SET COLLISION
* @category GROUP
*/
void dynGroupSetCollision(int groupA, int groupB, bool enable){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
	phyGlobStruct_->phy_->currentScene_->setGroupCollisionFlag(groupA, groupB, enable);
}

/*
* @description This command allows you to set which collision group the given actor belongs to, all actors start off in group 0.
* @param actorID - ID number of the object that the actor represents in the simulation.
* @param group - Group number you wish to assign the given actor to.
* @dbpro DYN SET GROUP
* @category GROUP
*/
void dynSetGroup(int actorID, int group){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynSetGroup");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorMap_[actorID];
	
	//-- v1.11 21/08/2012
	//-- commented out to add as seperate commands
	//unsigned int shapeCount = actor->getNbShapes();
	//for(unsigned int i = 0; i < shapeCount; i++){ 
	//	actor->getShapes()[i]->setGroup(group);
	//}
	//-- Another function
	
	actor->setGroup(group);
}

/*
* @description This command allows you to retrieve which collision group the given actor belongs to, all actors start off in group 0.
* @param actorID - ID number of the object that the actor represents in the simulation.
* @return Group number in which the given actor belongs.
* @dbpro DYN GET GROUP
* @category GROUP
*/
int dynGetGroup(int actorID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynGetGroup");
			return -1;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorMap_[actorID];
	//unsigned int shapeCount = actor->getNbShapes();
	//for(unsigned int i = 0; i < shapeCount; i++){ 
	//	actor->getShapes()[i]->setGroup(group);
	//}
	//-- Another function
	return (int)actor->getGroup();
}


/*
* @description Sets the pair flag for the given pair of actors, you can use this flag to enable contact reports between the two given actors. 
* @param actorA - First actor of actor pair. 
* @param actorB - Second actor of actor pair.
* @param flag - Contact pair flag, this determines whether you wish to recieve contacts on touch, start touch or end touch, or a combination. 
* @dbpro DYN ACTOR PAIR SET FLAGS
* @category RIGID BODY
*/
void dynActorPairSetFlags(int actorA, int actorB, int flag){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorA) == 0){
			Message::actorNotExist(actorA, "dynActorPairSetFlags");
            return;
		}
		if(phyGlobStruct_->actorMap_.count(actorB) == 0){
			Message::actorNotExist(actorB, "dynActorPairSetFlags");
            return;
		}
	#endif
        NxActor *actorOne = phyGlobStruct_->actorMap_[actorA];
        NxActor *actorTwo = phyGlobStruct_->actorMap_[actorB];
		phyGlobStruct_->phy_->currentScene_->setActorPairFlags(*actorOne, *actorTwo, flag);
}

/*
* @description Sets the pair flag for the given pair of actor shapes, you can't use this flag to enable contact reports between the two given shapes, this is only possible at actor level. You can use this command to disable collisions 'NX_IGNORE_PAIR' between two specific shapes. 
* @param actorA - First actor of actor pair.
* @param shapeIndexA - Shape index for the first actor, default shape is 0.
* @param actorB - Second actor of actor pair.
* @param shapeIndexB - Shape index for the second actor, default shape is 0.
* @param flag - NX_IGNORE_PAIR is the only non-zero value allowed as a shape pair flag. Passing zero stops ignoring the shape pair flag again.
* @dbpro DYN SHAPE PAIR SET FLAGS
* @category RIGID BODY
*/
void dynShapePairSetFlags(int actorA, int shapeIndexA, int actorB, int shapeIndexB, int flag){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorA) == 0){
			Message::actorNotExist(actorA, "dynShapePairSetFlags");
            return;
		}
		if(phyGlobStruct_->actorMap_.count(actorB) == 0){
			Message::actorNotExist(actorB, "dynShapePairSetFlags");
            return;
		}
	#endif
        NxActor *actorOne = phyGlobStruct_->actorMap_[actorA];
        NxActor *actorTwo = phyGlobStruct_->actorMap_[actorB];
		if( actorOne->getNbShapes() <= shapeIndexA )
			return;
		if( actorTwo->getNbShapes() <= shapeIndexB )
			return;
		phyGlobStruct_->phy_->currentScene_->setShapePairFlags(*actorOne->getShapes()[shapeIndexA], *actorTwo->getShapes()[shapeIndexB], flag);
}


/*
* @description Fetches any contact data generated from the simulation, you must have previously set up some actors or actor groups to generate contact reports. You can keep calling this function until it returns 0, meaning there are no contacts left to retrieve. Each time you call this function you can retrieve the specific data from the contact using the other contact commands.
* @return The amount of contact data remaining to be retrieved.
* @dbpro DYN CONTACT GET DATA
* @category CONTACT
*/
int dynContactGetData(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		return phyGlobStruct_->contact_->getContactData();
}

/*
* @description Call this function to find the first object which is associated with the current contact data. 
* @return ID number of the object.
* @dbpro DYN CONTACT GET ACTOR A
* @category CONTACT
*/
int dynContactGetActorA(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		if(phyGlobStruct_->contact_->currentContact_){
		   NxActor* actor = phyGlobStruct_->contact_->currentContact_->pair_.actors[0];
		   ActorSync* as = (ActorSync*)actor->userData; 
		   return as->object_;	   
		}
		else return -1;
}

/*
* @description Call this function to find the second object which is associated with the current contact data. 
* @return ID number of the object.
* @dbpro DYN CONTACT GET ACTOR B
* @category CONTACT
*/
int dynContactGetActorB(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		if(phyGlobStruct_->contact_->currentContact_){
		   NxActor* actor = phyGlobStruct_->contact_->currentContact_->pair_.actors[1];
		   ActorSync* as = (ActorSync*)actor->userData; 
		   return as->object_;
		}
		else return -1;
}

/*
* @description Call this function to retrieve the flag associated with the currect contact data. 
* @return Flag, contact on touch, start touch or end touch, or combination of each.
* @dbpro DYN CONTACT GET FLAG
* @category CONTACT
*/
int dynContactGetFlag(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		if(phyGlobStruct_->contact_->currentContact_)
		   return phyGlobStruct_->contact_->currentContact_->type_;
		else return -1;
}

/*
* @description Sets the flag for the given pair of groups of actors, you can use this flag to enable contact reports between the two given actor groups. 
* @param groupA - First group. 
* @param groupB - Second group.
* @param flag - Contact pair flag, this determines whether you wish to receive contacts on touch, start touch or end touch, or a combination. 
* @dbpro DYN GROUP PAIR SET FLAGS
* @category GROUP
*/
void dynGroupPairSetFlags(int groupA, int groupB, int flag){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->currentScene_->setActorGroupPairFlags(groupA, groupB, flag);
}



/*
* @description Casts a ray from a given position in the scene in a given direction. You can then use the other raycast commands to retrieve any hit data. This command can generate more than one hit per raycast.
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data.
* @return Number of hits.
* @dbpro DYN RAYCAST ALL SHAPES
* @category RAYCAST
*/
int dynRaycastAllShapes(int origVec3ID, int dirVec3ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif
		return phyGlobStruct_->phy_->currentScene_->raycastAllShapes(worldRay, *phyGlobStruct_->raycast_, NX_ALL_SHAPES, -1, phyGlobStruct_->maxRayDistance_);
}

/*
* @description Casts a ray from a given position in the scene in a given direction. You can then use the other raycast commands to retrieve any hit data. This command can generate more than one hit per raycast.
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIX_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return Number of hits.
* @dbpro DYN RAYCAST ALL SHAPES
* @category RAYCAST
*/
int dynRaycastAllShapes(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		NxRay worldRay;
		
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif

		return phyGlobStruct_->phy_->currentScene_->raycastAllShapes(worldRay, *phyGlobStruct_->raycast_, (NxShapesType)flag, groups, phyGlobStruct_->maxRayDistance_);
}

/*
* @description Casts a ray from a given position in the scene in a given direction, the cast is tested against shape bounding boxes. You can then use the other raycast commands to retrieve any hit data. This command can generate more than one hit per raycast.
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data, this vector must be normalized.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIX_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return Number of hits.
* @dbpro DYN RAYCAST ALL BOUNDS
* @category RAYCAST
*/
int dynRaycastAllBounds(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif
		return phyGlobStruct_->phy_->currentScene_->raycastAllBounds(worldRay, *phyGlobStruct_->raycast_, (NxShapesType)flag, groups, phyGlobStruct_->maxRayDistance_);
}

/*
* @description Casts a ray from a given position in the scene in a given direction. This is the fastest raycast method, it does not record any hit data.
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data, this vector must be normalized.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIX_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return TRUE if hit, FALSE otherwise.
* @dbpro DYN RAYCAST ANY SHAPE
* @category RAYCAST
*/
bool dynRaycastAnyShape(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif
		return phyGlobStruct_->phy_->currentScene_->raycastAnyShape(worldRay, (NxShapesType)flag, groups, phyGlobStruct_->maxRayDistance_);
}

/*
* @description Casts a ray from a given position in the scene in a given direction, the cast is tested against shape bounding boxes. This is the fastest raycast method, it does not record any hit data.
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data, this vector must be normalized.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIC_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return TRUE if hit, FALSE otherwise.
* @dbpro DYN RAYCAST ANY BOUNDS
* @category RAYCAST
*/
bool dynRaycastAnyBounds(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif
		return phyGlobStruct_->phy_->currentScene_->raycastAnyBounds(worldRay, (NxShapesType)flag, groups, phyGlobStruct_->maxRayDistance_);
}


/*
* @description Casts a ray from a given position in the scene in a given direction. This can record a maximum of one hit, you can get the hit data using the 'raycast hit' commands. 
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data, this vector must be normalized.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIX_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return TRUE if hit, FALSE otherwise.
* @dbpro DYN RAYCAST CLOSEST SHAPE
* @category RAYCAST
*/
bool dynRaycastClosestShape(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif

		//-- Place data in contact reports hit vector so the user can retrieve in the same way.
	    NxRaycastHit *hit = new NxRaycastHit();
		//phyGlobStruct_->raycast_->hitVec_.push_back(hit);
		NxShape* shape = 0;
		shape = phyGlobStruct_->phy_->currentScene_->raycastClosestShape(worldRay, (NxShapesType)flag, *hit, groups, phyGlobStruct_->maxRayDistance_);
		if(shape){
		    phyGlobStruct_->raycast_->hitVec_.push_back(hit);
			return true;
		}
		else{
			delete hit;
			return false;
		}
}

/*
* @description Casts a ray from a given position in the scene in a given direction, the cast is tested against shape bounding boxes. This can record a maximum of one hit, you can get the hit data using the 'raycast hit' commands. 
* @param origVec3ID - ID number of the vector containing the global position data.
* @param dirVec3ID - ID number of the vector containing the global direction data, this vector must be normalized.
* @param flag - NX_STATIC_SHAPES = 1, NX_DYNAMIX_SHAPES = 2, NX_ALL_SHAPES = 3
* @param groups - Groups mask to filter shapes.
* @return TRUE if hit, FALSE otherwise.
* @dbpro DYN RAYCAST CLOSEST BOUNDS
* @category RAYCAST
*/
bool dynRaycastClosestBounds(int origVec3ID, int dirVec3ID, int flag, int groups){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		NxRay worldRay;
    #ifdef COMPILE_FOR_GDK
		worldRay.orig.x = dbXVector3(origVec3ID);
		worldRay.orig.y = dbYVector3(origVec3ID);
		worldRay.orig.z = dbZVector3(origVec3ID);
		worldRay.dir.x = dbXVector3(dirVec3ID);
		worldRay.dir.y = dbYVector3(dirVec3ID);
		worldRay.dir.z = dbZVector3(dirVec3ID);
    #else
        DWORD tempOrigX = dbXVector3(origVec3ID);
		DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
        
		DWORD tempDirX = dbXVector3(dirVec3ID);
		DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);

		worldRay.orig.x = *(float*)&tempOrigX;
		worldRay.orig.y = *(float*)&tempOrigY;
		worldRay.orig.z = *(float*)&tempOrigZ;
		worldRay.dir.x = *(float*)&tempDirX;
		worldRay.dir.y = *(float*)&tempDirY;
		worldRay.dir.z = *(float*)&tempDirZ;
    #endif

		//-- Place data in contact reports hit vector so the user can retrieve in the same way.
	    NxRaycastHit *hit = new NxRaycastHit();
		NxShape* shape = 0;
		shape = phyGlobStruct_->phy_->currentScene_->raycastClosestBounds(worldRay, (NxShapesType)flag, *hit, groups, phyGlobStruct_->maxRayDistance_);
		if(shape){
		    phyGlobStruct_->raycast_->hitVec_.push_back(hit);
			return true;
		}
		else{
			delete hit;
			return false;
		}
}





/*
* @description Sets the maximum distance all raycasts can travel.
* @param distance - Maximum distance the ray will travel.
* @dbpro DYN RAYCAST SET MAX DISTANCE
* @category RAYCAST
*/
void dynRaycastSetMaxDistance(float distance){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->maxRayDistance_ = distance;
}

/*
* @description Gets the maximum distance all raycasts can travel.
* @return Maximum distance the ray will travel.
* @dbpro DYN RAYCAST GET MAX DISTANCE
* @category RAYCAST
*/
DWORDFLOAT dynRaycastGetMaxDistance(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		float value = phyGlobStruct_->maxRayDistance_;
		RETURNCAST
}










/*
* @description Fetches raycast hit data generated from certain raycasts, you can use the other raycast hit commands to retrieve this data.
* @return The amount of raycast hit data remaining to be retrieved.
* @dbpro DYN RAYCAST HIT GET DATA
* @category RAYCAST
*/
int dynRaycastHitGetData(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		return phyGlobStruct_->raycast_->getHitData();
}

/*
* @description Use this to retrieve which object is associated with the current raycast hit data.
* @return ID number of the object.
* @dbpro DYN RAYCAST HIT GET OBJECT
* @category RAYCAST
*/
int dynRaycastHitGetObject(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		if(!phyGlobStruct_->raycast_->currentHit_){
           return 0;
		}
		NxShape* shape = phyGlobStruct_->raycast_->currentHit_->shape;
		ActorSync* as = (ActorSync*)shape->getActor().userData;
		return as->object_;
}

/*
* @description Use this to retrieve which limb is associated with the current raycast hit data. This will only work if you have assigned actors/shapes to the limbs of an object.
* @return ID number of the limb.
* @dbpro DYN RAYCAST HIT GET LIMB
* @category RAYCAST
*/
int dynRaycastHitGetLimb(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		if(!phyGlobStruct_->raycast_){
           return 0;
		}
		NxShape* shape = phyGlobStruct_->raycast_->currentHit_->shape;
		ActorSync* as = 0;
		as = (ActorSync*)shape->getActor().userData;
		if(as){
		    return as->limb_;
		}
		else return 0;
}

/*
* @description Use this to retrieve the distance value associated with the current raycast hit data.
* @return Distance the ray travelled before the current hit.
* @dbpro DYN RAYCAST HIT GET DISTANCE
* @category RAYCAST
*/
DWORDFLOAT dynRaycastHitGetDistance(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return 0;
		}
		float value = phyGlobStruct_->raycast_->currentHit_->distance;
		RETURNCAST
}

/*
* @description Use this to retrieve the world position which is associated with the current raycast hit data.
* @param ID number of the vector which the data will be placed into.
* @dbpro DYN RAYCAST HIT GET WORLD IMPACT
* @category RAYCAST
*/
void dynRaycastHitGetWorldImpact(int vector3ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return;
		}
		NxVec3 vec(phyGlobStruct_->raycast_->currentHit_->worldImpact);
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Use this to retrieve the world normal which is associated with the current raycast hit data.
* @param ID number of the vector which the data will be placed into.
* @dbpro DYN RAYCAST HIT GET WORLD NORMAL
* @category RAYCAST
*/
void dynRaycastHitGetWorldNormal(int vector3ID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return;
		}
		NxVec3 vec(phyGlobStruct_->raycast_->currentHit_->worldNormal);
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Use this to retrieve one of two barycentric coordinates, since the simulation does not have access to the actual UV data of the model you may need to somehow convert these coordinates to the actual UV ccordinates of the raycast hit.
* @return Barycentric coordinate U.
* @dbpro DYN RAYCAST HIT GET U
* @category RAYCAST
*/
DWORDFLOAT dynRaycastHitGetU(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return 0;
		}
		float value = phyGlobStruct_->raycast_->currentHit_->u;
		RETURNCAST
}

/*
* @description Use this to retrieve one of two barycentric coordinates, since the simulation does not have access to the actual UV data of the model you may need to somehow convert these coordinates to the actual UV ccordinates of the raycast hit.
* @return Barycentric coordinate V.
* @dbpro DYN RAYCAST HIT GET V
* @category RAYCAST
*/
DWORDFLOAT dynRaycastHitGetV(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return 0;
		}
		float value = phyGlobStruct_->raycast_->currentHit_->v;
		RETURNCAST
}

/*
* @description Returns the ID of the triangle which was hit by the raycast. If the object has index data then each three indices relates to one face, using this information you should be able to get the three indices which the faceID represents, you can then retrieve the vertices with this index data.
* @return Face ID
* @dbpro DYN RAYCAST HIT GET FACE ID
* @category RAYCAST
*/
unsigned int dynRaycastHitGetFaceID(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		if(!phyGlobStruct_->raycast_){
           return 0;
		}
		return phyGlobStruct_->raycast_->currentHit_->faceID;
		//RETURNCAST
}
/*
* @description Sets the CCD motion threshold for the given actor.
* @param objectID ID number of the object which the actor represents in the simulation.
* @param limbID ID number of the limb which the actor represents in the simulation, if the actor was not created using a limb then set to 0.
* @dbpro DYN CCD SET MOTION THRESHOLD
* @category CCD
*/
void dynCCDSetMotionThreshold(int objectID, int limbID, float threshold){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		//-- If limb
		if(limbID < 1){
			if(phyGlobStruct_->actorMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, string("dynDeleteCCDSkeleton"));
			    return;
			}
		}
		//-- If object
		else{
     		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynDeleteCCDSkeleton"));
				return;
			}
     		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynDeleteCCDSkeleton"));
			    return;
			}
		}
	#endif
	NxActor *actor;
	if(limbID < 1){
	    actor = phyGlobStruct_->actorMap_[objectID];
	}
    else{
        actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	}
	actor->setCCDMotionThreshold(threshold);
}

/*
* @description Releases the given CCD skeleton from the simulation.
* @param objectID ID number of the object which the actor represents in the simulation.
* @param limbID ID number of the limb which the actor represents in the simulation, if the actor was not created using a limb then set to 0.
* @dbpro DYN DELETE CCD SKELETON
* @category CONSTRUCTION
*/
void dynDeleteCCDSkeleton(int objectID, int limbID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		//-- If limb
		if(limbID < 1){
			if(phyGlobStruct_->actorMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, string("dynDeleteCCDSkeleton"));
			    return;
			}
		}
		//-- If object
		else{
     		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynDeleteCCDSkeleton"));
				return;
			}
     		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynDeleteCCDSkeleton"));
			    return;
			}
		}
	#endif

	NxActor *actor;
	if(limbID < 1){
	    actor = phyGlobStruct_->actorMap_[objectID];
	}
    else{
        actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	}
	//-- Loop through shapes
	int nbShapes=actor->getNbShapes();
	NxShape *const* shapeArray = actor->getShapes();
	for(int i=0;i <nbShapes;i++)
	{
		//NxVec3 scale(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_,phyGlobStruct_->phyScaleMultiplier_);
		phyGlobStruct_->phy_->releaseCCDSkeleton(shapeArray[i]);
	}   	
}


/*
* @description This will add a single vertex mesh to the given actor which will then be used for continuous collision detection if enabled.
* @param objectID ID number of the object which the actor represents in the simulation.
* @param limbID ID number of the limb which the actor represents in the simulation, if the actor was not created using a limb then set to 0.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CCD SKELETON VERTEX
* @category CONSTRUCTION
*/
bool dynMakeCCDSkeletonVertex(int objectID, int limbID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		//-- If limb
		if(limbID < 1){
			if(phyGlobStruct_->actorMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, string("dynMakeCCDSkeletonVertex"));
			    return 0;
			}
		}
		//-- If object
		else{
     		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynMakeCCDSkeletonVertex"));
				return 0;
			}
     		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynMakeCCDSkeletonVertex"));
			    return 0;
			}
		}
	#endif
		NxActor *actor;
		if(limbID < 1){
		    actor = phyGlobStruct_->actorMap_[objectID];
		}
        else{
            actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
		}
		//-- Loop through shapes
		int nbShapes=actor->getNbShapes();
		NxShape *const* shapeArray = actor->getShapes();
		for(int i=0;i <nbShapes;i++)
		{
			//NxVec3 scale(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_,phyGlobStruct_->phyScaleMultiplier_);
			phyGlobStruct_->phy_->makeCCDSkeletonVertex(shapeArray[i]);
		}   	
	return 1;
}


/*
* @description This will add a box mesh to the given actor which will then be used for continuous collision detection if enabled. You can use the 'scale multiplier' function to alter the size of the skeleton relative to the shape it represents.
* @param objectID ID number of the object which the actor represents in the simulation.
* @param limbID ID number of the limb which the actor represents in the simulation, if the actor was not created using a limb then set to 0.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CCD SKELETON BOX
* @category CONSTRUCTION
*/
bool dynMakeCCDSkeletonBox(int objectID, int limbID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		//-- If limb
		if(limbID < 1){
			if(phyGlobStruct_->actorMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, string("dynMakeCCDSkeletonBox"));
			    return 0;
			}
		}
		//-- If object
		else{
     		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynMakeCCDSkeletonBox"));
				return 0;
			}
     		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
				Message::actorNotExist(objectID, limbID, string("dynMakeCCDSkeletonBox"));
			    return 0;
			}
		}
	#endif
		NxActor *actor;
		if(limbID < 1){
		    actor = phyGlobStruct_->actorMap_[objectID];
		}
        else{
            actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
		}
		//-- Loop through shapes
		int nbShapes=actor->getNbShapes();
		NxShape *const* shapeArray = actor->getShapes();
		for(int i=0;i <nbShapes;i++)
		{
			NxVec3 scale(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_,phyGlobStruct_->phyScaleMultiplier_);
			phyGlobStruct_->phy_->makeCCDSkeletonBox(shapeArray[i], scale);
		}   	
	return 1;
}


/*
* @description This command allows you to turn an actors shape into a trigger, you can use the other trigger commands to retrieve trigger infromation when another shape/actor enters the trigger shape.
* @param objectID ID number of the object which the actor represents in the simulation.
* @param shapeIndex Index number of the shape you wish to become a trigger, default shape is 0.
* @param isTrigger TRUE to set as trigger, FALSE otherwise.
* @dbpro DYN TRIGGER SET SHAPE
* @category TRIGGER
*/
void dynTriggerSetShape(int objectID, int shapeIndex, int flag, bool value){
#ifdef DEBUG
	if(!phyGlobStruct_->phy_){
		Message::ignitionError();
		return;
	}
	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		Message::actorNotExist(objectID, string("dynTriggerSetShape"));
		return;
	} 
#endif
	NxActor* actor = phyGlobStruct_->actorMap_[objectID];
	if(actor->getNbShapes() < shapeIndex + 1){
        return;
	}
	NxShape* shape = actor->getShapes()[shapeIndex];
	shape->setFlag((NxShapeFlag)flag, value);
//NX_TRIGGER_ON_ENTER, NX_TRIGGER_ON_LEAVE, and NX_TRIGGER_ON_STAY
}

/*
* @description Fetches trigger data, you can use the other trigger commands to retrieve this data.
* @return The amount of trigger data remaining to be retrieved.
* @dbpro DYN TRIGGER GET DATA
* @category TRIGGER
*/
int dynTriggerGetData(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		return phyGlobStruct_->triggerCallback_->getTriggerData();
}

/*
* @description Use this to retrieve which object is the trigger associated with the current trigger data.
* @return ID number of the object.
* @dbpro DYN TRIGGER GET TRIGGER OBJECT
* @category TRIGGER
*/
int dynTriggerGetTriggerObject(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		if(!phyGlobStruct_->triggerCallback_->currentTrigger_){
           return 0;
		}
		NxShape* shape = phyGlobStruct_->triggerCallback_->currentTrigger_->triggerShape_;
		ActorSync* as = (ActorSync*)shape->getActor().userData;
		if(as) 
			return as->object_;
		else 
			return 0;
}

/*
* @description Use this to retrieve which object has entered the trigger shape associated with the current trigger data.
* @return ID number of the object.
* @dbpro DYN TRIGGER GET OTHER OBJECT
* @category TRIGGER
*/
int dynTriggerGetOtherObject(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		if(!phyGlobStruct_->triggerCallback_->currentTrigger_){
           return 0;
		}
		NxShape* shape = phyGlobStruct_->triggerCallback_->currentTrigger_->otherShape_;
		ActorSync* as = (ActorSync*)shape->getActor().userData;
		if(as)
			return as->object_;
		else
			return 0;
}

/*
* @description Use this to retrieve which object has entered the trigger shape associated with the current trigger data.
* @return ID number of the object.
* @dbpro DYN TRIGGER GET STATUS
* @category TRIGGER
*/
int dynTriggerGetStatus(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif   
		if(!phyGlobStruct_->triggerCallback_->currentTrigger_){
           return 0;
		}
		return (int)phyGlobStruct_->triggerCallback_->currentTrigger_->status_;
}


/*
* @description Restricts the actor from moving along the x axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE POSITION X
* @category RIGID BODY
*/
void dynFreezePositionX(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezePositionX"));
		    return;
		}
	#endif 
		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_POS_X );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_POS_X );
		}
}

/*
* @description Restricts the actor from moving along the y axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE POSITION Y
* @category RIGID BODY
*/
void dynFreezePositionY(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezePositionY"));
		    return;
		}
	#endif 
		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_POS_Y );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_POS_Y );
		}
}

/*
* @description Restricts the actor from moving along the z axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE POSITION Z
* @category RIGID BODY
*/
void dynFreezePositionZ(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezePositionZ"));
		    return;
		}
	#endif 
		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_POS_Z );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_POS_Z );
		}
}

/*
* @description Restricts the actor from rotating around the x axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE ROTATION X
* @category RIGID BODY
*/
void dynFreezeRotationX(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezeRotationX"));
		    return;
		}
	#endif 
		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_ROT_X );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_ROT_X );
		}
}


/*
* @description Restricts the actor from rotating around the y axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE ROTATION Y
* @category RIGID BODY
*/
void dynFreezeRotationY(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezeRotationY"));
		    return;
		}
	#endif 
		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_ROT_Y );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_ROT_Y );
		}
}

/*
* @description Restricts the actor from rotating around the z axis.
* @param objectID Object that the actor represents in the simulation.
* @param freeze TRUE to freeze, FALSE otherwise.
* @dbpro DYN FREEZE ROTATION Z
* @category RIGID BODY
*/
void dynFreezeRotationZ(int objectID, bool freeze)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynFreezeRotationZ"));
		    return;
		}
	#endif 

		if( freeze )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_FROZEN_ROT_Z );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_FROZEN_ROT_Z );
		}
}

/*
* @description Allows you to set whether the given actor should be affected by gravity.
* @param objectID Object that the actor represents in the simulation.
* @param gravity TRUE to enable gravity, FALSE otherwise.
* @dbpro DYN ENABLE GRAVITY
* @category RIGID BODY
*/
void dynEnableGravity(int objectID, bool gravity)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    	if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, string("dynEnableGravity"));
		    return;
		}
	#endif 

		if( !gravity )
		{
		    phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
		}
		else
		{
			phyGlobStruct_->actorMap_[objectID]->clearBodyFlag( NX_BF_DISABLE_GRAVITY );
		}
}


/*
* @description Checks for any shapes within a sphere shape.
* @param positionVec3ID ID number of the vector holding the position of the sphere.
* @param radius Radius of the sphere.
* @param flag Allows you to test for static, dynamic or all shapes(static - 1, dynamic - 2, all - 3).
* @return TRUE if a shape is detected in overlap, FALSE otherwise.
* @dbpro DYN OVERLAP CHECK SPHERE
* @category OVERLAP
*/
bool dynOverlapCheckSphere(int positionVec3ID, float radius, int flag)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
    NxSphere worldSphere(NxVec3(dbXVector3(positionVec3ID),dbYVector3(positionVec3ID),dbZVector3(positionVec3ID)), radius / 2); 
	return phyGlobStruct_->phy_->currentScene_->checkOverlapSphere(worldSphere, (NxShapesType)flag);
}

/*
* @description Checks for any shapes within the given world bounds.
* @param worldBoundsMinVec3ID ID number of the vector holding the minimum bounds.
* @param worldBoundsMaxVec3ID ID number of the vector holding the maximum bounds.
* @param flag Allows you to test for static, dynamic or all shapes(static - 1, dynamic - 2, all - 3).
* @return TRUE if a shape is detected within given bounds, FALSE otherwise.
* @dbpro DYN OVERLAP CHECK AABB
* @category OVERLAP
*/
bool dynOverlapCheckAABB(int worldBoundsMinVec3ID, int worldBoundsMaxVec3ID, int flag)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	NxBounds3 worldBounds;
	worldBounds.set(NxVec3(dbXVector3(worldBoundsMinVec3ID), dbYVector3(worldBoundsMinVec3ID), dbZVector3(worldBoundsMinVec3ID)),
		            NxVec3(dbXVector3(worldBoundsMaxVec3ID), dbYVector3(worldBoundsMaxVec3ID), dbZVector3(worldBoundsMaxVec3ID)));
	return phyGlobStruct_->phy_->currentScene_->checkOverlapAABB(worldBounds, (NxShapesType)flag);
}

/*
* @description Checks for any shapes within the given capsule shaped bounds.
* @param pos1Vec3ID ID number of the first position vector for one end of the capsule.
* @param pos2Vec3ID ID number of the second position vector for the other end of the capsule.
* @param flag Allows you to test for static, dynamic or all shapes(static - 1, dynamic - 2, all - 3).
* @return TRUE if a shape is detected within given bounds, FALSE otherwise.
* @dbpro DYN OVERLAP CHECK CAPSULE
* @category OVERLAP
*/
bool dynOverlapCheckCapsule(int pos1Vec3ID, int pos2Vec3ID, float radius, int flag)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	NxSegment seg(NxVec3(dbXVector3(pos1Vec3ID), dbYVector3(pos1Vec3ID), dbZVector3(pos1Vec3ID)),
		            NxVec3(dbXVector3(pos2Vec3ID), dbYVector3(pos2Vec3ID), dbZVector3(pos2Vec3ID)));
	NxCapsule worldCapsule(seg, radius);
	return phyGlobStruct_->phy_->currentScene_->checkOverlapCapsule(worldCapsule, (NxShapesType)flag); 
}

/*
* @description Checks for any shapes within the given orientated bounding box bounds.
* @param positionVec3ID ID number of the position vector.
* @param positionVec3ID ID number of the dimension vector.
* @param dimensionVec3ID ID number of the rotation vector.
* @param flag Allows you to test for static, dynamic or all shapes(static - 1, dynamic - 2, all - 3).
* @return TRUE if a shape is detected within given bounds, FALSE otherwise.
* @dbpro DYN OVERLAP CHECK OBB
* @category OVERLAP
*/
bool dynOverlapCheckOBB(int positionVec3ID, int dimensionVec3ID, int rotationVec3ID, int flag)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	NxMat33 rotMat;
	rotMat.rotX(dbXVector3(rotationVec3ID));
	rotMat.rotY(dbYVector3(rotationVec3ID));
	rotMat.rotZ(dbZVector3(rotationVec3ID));
	NxBox worldBox(NxVec3(dbXVector3(positionVec3ID), dbYVector3(positionVec3ID), dbZVector3(positionVec3ID)),
		            NxVec3(dbXVector3(dimensionVec3ID), dbYVector3(dimensionVec3ID), dbZVector3(dimensionVec3ID)),
					rotMat);
	return phyGlobStruct_->phy_->currentScene_->checkOverlapOBB(worldBox, (NxShapesType)flag);
}

/*
* @description Checks an object to see whether it has an actor assigned to it.
* @param objectID ID number of the object you wish to check.
* @return TRUE if actor exists, FALSE otherwise.
* @dbpro DYN ACTOR EXISTS
* @category CONSTRUCTION
*/
bool dynActorExists(int objectID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
    return phyGlobStruct_->actorMap_.count(objectID);
}


/*
* @description Checks an object to see whether it has an actor assigned to it.
* @param objectID ID number of the object which the limb belongs.
* @param limbID ID number of the limb you wish to check.
* @return TRUE if actor exists, FALSE otherwise.
* @dbpro DYN ACTOR EXISTS
* @category CONSTRUCTION
*/
bool dynActorExists(int objectID, int limbID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		if(phyGlobStruct_->actorLimbMap_.count(objectID))
		{
			return phyGlobStruct_->actorLimbMap_[objectID].count(limbID);
		}
		else return 0;
}

/*
* @description Checks to see whether a triangle mesh exists with the given ID number.
* @param objectID ID number of the mesh.
* @return TRUE if mesh exists, FALSE otherwise.
* @dbpro DYN TRIANGLE MESH EXISTS
* @category CONSTRUCTION
*/
bool dynTriangleMeshExists(int meshID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->triangleMeshMap_.count(meshID);
}

/*
* @description Checks to see whether a convex mesh exists with the given ID number.
* @param objectID ID number of the mesh.
* @return TRUE if mesh exists, FALSE otherwise.
* @dbpro DYN CONVEX MESH EXISTS
* @category CONSTRUCTION
*/
bool dynConvexMeshExists(int meshID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->convexMeshMap_.count(meshID);
}

/*
* @description Checks to see whether a controller exists with the given ID number.
* @param objectID ID number of the mesh.
* @return TRUE if controller exists, FALSE otherwise.
* @dbpro DYN CONTROLLER EXISTS
* @category CONSTRUCTION
*/
bool dynControllerExists(int objectID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->controllerMap_.count(objectID);
}




























#ifdef COMPILE_DYNAMIX_FULL

//// JOINTS ////

/*
* @description Creates a joint descriptor, this is simply a structure that holds the data that determines what properties a joint will have when it's created. Descriptors do not share ID numbers with joints, or anything else.
* @param ID ID number you wish to assign to the descriptor, you may then reference this descriptor to create multiple joints.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE JOINT DESC
* @category CONSTRUCTION
*/
bool dynMakeJointDesc(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->jointDescMap_.count(ID) == 1){
			Message::jointDescAlreadyExists(ID, "dynMakeJointDesc");
            return false;
		}
	#endif
	NxD6JointDesc *jointDesc = new NxD6JointDesc();
	phyGlobStruct_->jointDescMap_[ID] = jointDesc;
	return true;
}

/*
* @description Creates a joint from a descriptor, joints do not share ID numbers with descriptors, or anything else.
* @param ID ID number you wish to assign to the joint.
* @param jointDescID ID number of the decriptor you wish to use to create joint.
* @return TRUE if successful, FALSE otherwise.
* @dbpro DYN MAKE JOINT
* @category CONSTRUCTION
*/
bool dynMakeJoint(int ID, int jointDescID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynMakeJoint");
            return false;
		}
		if(phyGlobStruct_->jointMap_.count(ID) == 1){
			Message::jointAlreadyExists(ID, "dynMakeJoint");
            return false;
		}
	#endif
	//NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
	//desc->flags = NX_JF_VISUALIZATION;// | NX_JF_COLLISION_ENABLED;
	NxD6Joint* joint = phyGlobStruct_->phy_->makeJoint(*phyGlobStruct_->jointDescMap_[jointDescID]);
	if(!joint){
         return false;
	}
	phyGlobStruct_->jointMap_[ID] = joint;//phyGlobStruct_->phy_->makeJoint(*phyGlobStruct_->jointDescMap_[jointDescID]);
	return true;
}


/*
* @description Releases the joint descriptor.
* @param jointDescID - ID number of the decriptor.
* @dbpro DYN DELETE JOINT DESC
* @category CONSTRUCTION
*/
void dynDeleteJointDesc(int jointDescID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynDeleteJointDesc");
            return;
		}
	#endif
        delete phyGlobStruct_->jointDescMap_[jointDescID];
		phyGlobStruct_->jointDescMap_.erase(jointDescID);
}

/*
* @description Releases the given joint.
* @param jointDescID - ID number of the joint.
* @dbpro DYN DELETE JOINT
* @category CONSTRUCTION
*/
void dynDeleteJoint(int jointID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynDeleteJoint");
            return;
		}
	#endif
		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->getScene().releaseJoint(*joint);
		phyGlobStruct_->jointMap_.erase(jointID);
}




/*
* @description Sets the degree of freedom for the swing1 motion of the given joint descriptor. If locked, any joint created using this descriptor will not allow rotation about the local y-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET SWING1 MOTION
* @category JOINT
*/
void dynJointDescSetSwing1Motion(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSwing1Motion");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->swing1Motion = (NxD6JointMotion)motion;
}
/*
* @description Sets the degree of freedom for the swing2 motion of the given joint descriptor. If locked, any joint created using this descriptor will not allow rotation about the local z-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET SWING2 MOTION
* @category JOINT
*/
void dynJointDescSetSwing2Motion(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSwing2Motion");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->swing2Motion = (NxD6JointMotion)motion;
}

/*
* @description Sets the degree of freedom for the twist motion of the given joint descriptor. If locked, any joint created using this descriptor will not allow rotation about the local x-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET TWIST MOTION
* @category JOINT
*/
void dynJointDescSetTwistMotion(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetTwistMotion");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->twistMotion = (NxD6JointMotion)motion;
}

/*
* @description Sets the degree of freedom for the motion along the x-axis of the given joint descriptor. If locked, any joint created using this descriptor will not allow movement along the local x-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET MOTION X
* @category JOINT
*/
void dynJointDescSetMotionX(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetMotionX");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->xMotion = (NxD6JointMotion)motion;
}

/*
* @description Sets the degree of freedom for the motion along the y-axis of the given joint descriptor. If locked, any joint created using this descriptor will not allow movement along the local y-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET MOTION Y
* @category JOINT
*/
void dynJointDescSetMotionY(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetMotionY");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->yMotion = (NxD6JointMotion)motion;
}

/*
* @description Sets the degree of freedom for the motion along the z-axis of the given joint descriptor. If locked, any joint created using this descriptor will not allow movement along the local z-axis. 
* @param jointDescID ID number of the descriptor.
* @param motion Flag used to set degree of freedom, set to 0 for complete freedom(default), 1 for limited freedom, 2 for no freedom(locked).
* @dbpro DYN JOINT DESC SET MOTION Z
* @category JOINT
*/
void dynJointDescSetMotionZ(int jointDescID, int motion){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetMotionZ");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->zMotion = (NxD6JointMotion)motion;
}


/*
* @description This command is used to set a rotational limit about the local y-axis. 
* @param jointDescID ID number of the descriptor.
* @param value Angle beyond which the limit is active, angle expressed in degrees.
* @param spring If greater than zero then the limit is soft, this means the spring will pull the joint back to the given limit.
* @param damping If spring is greater than zero then this is the damping of the spring.
* @param restitution Controls the amount of bounce when the joint hits a limit, a value of 0 causes the joint to stop dead, a value of 1.0 will cause the joint to bounce back with same velocity at which it hit the limit.
* @dbpro DYN JOINT DESC SET SWING1 LIMIT
* @category JOINT
*/
void dynJointDescSetSwing1Limit(int jointDescID, float value, float spring, float damping, float restitution){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSwing1Limit");
            return;
		}
	#endif
	NxJointLimitSoftDesc *limit = &phyGlobStruct_->jointDescMap_[jointDescID]->swing1Limit;
	limit->value       = NxMath::degToRad(value);
	limit->spring      = spring;
	limit->damping     = damping;
	limit->restitution = restitution;
}



/*
* @description This command is used to set the linear limit of the given joint decriptor. 
* @param jointDescID ID number of the descriptor.
* @param value Position beyond which the limit is active.
* @param spring If greater than zero then the limit is soft, this means the spring will pull the joint back to the given limit.
* @param damping If spring is greater than zero then this is the damping of the spring.
* @param restitution Controls the amount of bounce when the joint hits a limit, a value of 0 causes the joint to stop dead, a value of 1.0 will cause the joint to bounce back with same velocity at which it hit the limit.
* @dbpro DYN JOINT DESC SET LINEAR LIMIT
* @category JOINT
*/
void dynJointDescSetLinearLimit(int jointDescID, float value, float spring, float damping, float restitution){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetLinearLimit");
            return;
		}
	#endif
		NxJointLimitSoftDesc *limit = &phyGlobStruct_->jointDescMap_[jointDescID]->linearLimit;
	    limit->value       = value;
	    limit->spring      = spring;
	    limit->damping     = damping;
	    limit->restitution = restitution;
}





/*
* @description This command is used to set a rotational limit about the local z-axis. 
* @param jointDescID ID number of the descriptor.
* @param value Angle beyond which the limit is active, angle expressed in degrees.
* @param spring If greater than zero then the limit is soft, this means the spring will pull the joint back to the given limit.
* @param damping If spring is greater than zero then this is the damping of the spring.
* @param restitution Controls the amount of bounce when the joint hits a limit, a value of 0 causes the joint to stop dead, a value of 1.0 will cause the joint to bounce back with same velocity at which it hit the limit.
* @dbpro DYN JOINT DESC SET SWING2 LIMIT
* @category JOINT
*/
void dynJointDescSetSwing2Limit(int jointDescID, float value, float spring, float damping, float restitution){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSwing2Limit");
            return;
		}
	#endif	
	
	NxJointLimitSoftDesc *limit = &phyGlobStruct_->jointDescMap_[jointDescID]->swing2Limit;
	limit->value       = NxMath::degToRad(value);
	limit->spring      = spring;
	limit->damping     = damping;
	limit->restitution = restitution;
}

/*
* @description This command is used to set a rotational high limit about the local x-axis. The twist limit is a little different than the swing limits, specified using a pair of angles(high,low) rather than just a single radius. This allows the limit orientations to be nonsymmetrical around the twist axis, unlike the swing axis. 
* @param jointDescID ID number of the descriptor.
* @param value Angle beyond which the limit is active, angle expressed in degrees.
* @param spring If greater than zero then the limit is soft, this means the spring will pull the joint back to the given limit.
* @param damping If spring is greater than zero then this is the damping of the spring.
* @param restitution Controls the amount of bounce when the joint hits a limit, a value of 0 causes the joint to stop dead, a value of 1.0 will cause the joint to bounce back with same velocity at which it hit the limit.
* @dbpro DYN JOINT DESC SET TWIST LIMIT HIGH
* @category JOINT
*/
void dynJointDescSetTwistLimitHigh(int jointDescID, float value, float spring, float damping, float restitution){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetTwistLimitHigh");
            return;
		}
	#endif	
	NxJointLimitSoftDesc *limit = &phyGlobStruct_->jointDescMap_[jointDescID]->twistLimit.high;
	limit->value       = NxMath::degToRad(value);
	limit->spring      = spring;
	limit->damping     = damping;
	limit->restitution = restitution;
}

/*
* @description This command is used to set a rotational low limit about the local x-axis. The twist limit is a little different than the swing limits, specified using a pair of angles(high,low) rather than just a single radius. This allows the limit orientations to be nonsymmetrical around the twist axis, unlike the swing axis. 
* @param jointDescID ID number of the descriptor.
* @param value Angle beyond which the limit is active, angle expressed in degrees.
* @param spring If greater than zero then the limit is soft, this means the spring will pull the joint back to the given limit.
* @param damping If spring is greater than zero then this is the damping of the spring.
* @param restitution Controls the amount of bounce when the joint hits a limit, a value of 0 causes the joint to stop dead, a value of 1.0 will cause the joint to bounce back with same velocity at which it hit the limit.
* @dbpro DYN JOINT DESC SET TWIST LIMIT LOW
* @category JOINT
*/
void dynJointDescSetTwistLimitLow(int jointDescID, float value, float spring, float damping, float restitution){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetTwistLimitLow");
            return;
		}
	#endif
	NxJointLimitSoftDesc *limit = &phyGlobStruct_->jointDescMap_[jointDescID]->twistLimit.low;
	limit->value       = NxMath::degToRad(value);
	limit->spring      = spring;
	limit->damping     = damping;
	limit->restitution = restitution;
}

/*
* @description The flag value determines whether debug visualisation of the joint will be enabled(default) and also whether the joined actors will collide with each other(default = no collision).
* @param jointDescID - ID number of the descriptor.
* @param flags - Combination of values to determine visualisation(NX_JF_VISUALIZATION = 2) and collision(NX_JF_COLLISION_ENABLED = 1).
* @dbpro DYN JOINT DESC SET FLAGS
* @category JOINT
*/
void dynJointDescSetFlags(int jointDescID, int flags){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetFlags");
            return;
		}
	#endif
	NxJointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
	desc->jointFlags = flags;
}


/*
* @description Sets the first actor for the given joint descriptor.
* @param jointDescID ID number of the descriptor.
* @param objectID ID number of the object counterpart of the actor we want to associate with the given joint descriptor.
* @dbpro DYN JOINT DESC SET ACTOR A
* @category JOINT
*/
void dynJointDescSetActorA(int jointDescID, int objectID){
	//WE SHOULD BE ALLOWED TO PASS 0 AS STATIC WORLD ACTOR
	//Fixed v1.1
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(objectID != 0 && phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynJointDescSetActorA");
            return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetActorA");
            return;
		}
	#endif	
	if(objectID != 0)
	    phyGlobStruct_->jointDescMap_[jointDescID]->actor[0] = phyGlobStruct_->actorMap_[objectID];
	else
	    phyGlobStruct_->jointDescMap_[jointDescID]->actor[0] = 0;
}

/*
* @description Sets the second actor for the given joint descriptor.
* @param jointDescID ID number of the descriptor.
* @param objectID ID number of the object counterpart of the actor we want to associate with the given joint descriptor.
* @dbpro DYN JOINT DESC SET ACTOR B
* @category JOINT
*/
void dynJointDescSetActorB(int jointDescID, int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(objectID != 0 && phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynJointDescSetActorB");
            return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetActorB");
            return;
		}
	#endif
	if(objectID != 0)
		phyGlobStruct_->jointDescMap_[jointDescID]->actor[1] = phyGlobStruct_->actorMap_[objectID];
	else
	    phyGlobStruct_->jointDescMap_[jointDescID]->actor[1] = 0;
}

/*
* @description Sets the first actor for the given joint descriptor.
* @param jointDescID ID number of the descriptor.
* @param objectID ID of the parent object.
* @param limbID ID number of the limb counterpart of the actor we want to associate with the given joint descriptor.
* @dbpro DYN JOINT DESC SET ACTOR A
* @category JOINT
*/
void dynJointDescSetActorA(int jointDescID, int objectID, int limbID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if((phyGlobStruct_->actorLimbMap_.count(objectID) == 0) || phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynJointDescSetActorA");
            return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetActorA");
            return;
		}
	#endif
    phyGlobStruct_->jointDescMap_[jointDescID]->actor[0] = phyGlobStruct_->actorLimbMap_[objectID][limbID];
}

/*
* @description Sets the second actor for the given joint descriptor.
* @param jointDescID ID number of the descriptor.
* @param objectID ID of the parent object.
* @param limbID ID number of the limb counterpart of the actor we want to associate with the given joint descriptor.
* @dbpro DYN JOINT DESC SET ACTOR B
* @category JOINT
*/
void dynJointDescSetActorB(int jointDescID, int objectID, int limbID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if((phyGlobStruct_->actorLimbMap_.count(objectID) == 0) || phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynJointDescSetActorB");
            return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetActorB");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->actor[1] = phyGlobStruct_->actorLimbMap_[objectID][limbID];
}

/*
* @description Sets the the position of the joint in global space, this is then internally converted to the local space of each associated actor, for this reason it is important to set the actors first before calling this command. 
* @param jointDescID ID number of the descriptor.
* @param posX Position along the global x-axis.
* @param posY Position along the global y-axis.
* @param posZ Position along the global z-axis.
* @dbpro DYN JOINT DESC SET GLOBAL ANCHOR
* @category JOINT
*/
void dynJointDescSetGlobalAnchor(int jointDescID, float posX, float posY, float posZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetGlobalAnchor");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->setGlobalAnchor(NxVec3(posX, posY, posZ));
}

/*
* @description Sets the the orientation of the joint in global space, this is then internally converted to the local axis of each associated actor, for this reason it is important to set the actors first before calling this command. 
* @param jointDescID ID number of the descriptor.
* @param angleX Orientation about the global x-axis.
* @param angleY Orientation about the global y-axis.
* @param angleZ Orientation about the global z-axis.
* @dbpro DYN JOINT DESC SET GLOBAL AXIS
* @category JOINT
*/
void dynJointDescSetGlobalAxis(int jointDescID, float angleX, float angleY, float angleZ){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetGlobalAxis");
            return;
		}
	#endif
	phyGlobStruct_->jointDescMap_[jointDescID]->setGlobalAxis(NxVec3(angleX, angleY, angleZ));
}





/*
* @description Sets the maximum amount of torque the joint can withstand before it breaks. Broken joints are not deleted, they are dectivated the same as if you deleted one of the actors affected by the joint.
* @param jointDescID ID number of the descriptor.
* @param torque Maximum torque before the joint breaks.
* @dbpro DYN JOINT DESC SET MAX TORQUE
* @category JOINT
*/
void dynJointDescSetMaxTorque(int jointDescID, float torque){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetMaxTorque");
            return;
		}
	#endif
		phyGlobStruct_->jointDescMap_[jointDescID]->maxTorque = torque;
}


/*
* @description Sets the maximum amount of force the joint can withstand before it breaks. Broken joints are not deleted, they are dectivated the same as if you deleted one of the actors affected by the joint.  
* @param jointDescID ID number of the descriptor.
* @param force Maximum force before the joint breaks.
* @dbpro DYN JOINT DESC SET MAX FORCE
* @category JOINT
*/
void dynJointDescSetMaxForce(int jointDescID, float force){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetMaxForce");
            return;
		}
	#endif
		phyGlobStruct_->jointDescMap_[jointDescID]->maxForce = force;
}


/*
* @description Sets the local anchor point of one of the two actors associated with the joint descriptor. Most of the time you should use the 'global anchor' command which sets the both actors local anchor to the same point in the world.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param x - Position along the x axis in the space of the given actor.
* @param y - Position along the y axis in the space of the given actor.
* @param z - Position along the z axis in the space of the given actor.
* @dbpro DYN JOINT DESC SET LOCAL ANCHOR
* @category JOINT
*/
void dynJointDescSetLocalAnchor(int jointDescID, int AorB, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetLocalAnchor");
            return;
		}
	#endif
		phyGlobStruct_->jointDescMap_[jointDescID]->localAnchor[AorB] = NxVec3(x, y, z);
}


/*
* @description Sets the local axis of one of the two actors associated with the joint descriptor. Most of the time you should use the 'global axis' command which sets the both actors local axis to the same frame in the world. The axis values should be taken from a normalised vector.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param x - Axis about the x axis in the space of the given actor. 
* @param y - Axis about the y axis in the space of the given actor.
* @param z - Axis about the z axis in the space of the given actor.
* @dbpro DYN JOINT DESC SET LOCAL AXIS
* @category JOINT
*/
void dynJointDescSetLocalAxis(int jointDescID, int AorB, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetLocalAxis");
            return;
		}
	#endif
		phyGlobStruct_->jointDescMap_[jointDescID]->localAxis[AorB] = NxVec3(x, y, z);
}

/*
* @description Sets the local normal of one of the two actors associated with the joint descriptor. The normal values should be taken from a normalised vector.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param x - Normal about the x axis in the space of the given actor. 
* @param y - Normal about the y axis in the space of the given actor.
* @param z - Normal about the z axis in the space of the given actor.
* @dbpro DYN JOINT DESC SET LOCAL NORMAL
* @category JOINT
*/
void dynJointDescSetLocalNormal(int jointDescID, int AorB, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetLocalNormal");
            return;
		}
	#endif
		phyGlobStruct_->jointDescMap_[jointDescID]->localNormal[AorB] = NxVec3(x, y, z);
}








/*
* @description Gets the local anchor point of one of the two actors associated with the joint descriptor.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param vector3Out - ID number of the vector3 the values will be placed.
* @dbpro DYN JOINT DESC GET LOCAL ANCHOR
* @category JOINT
*/
void dynJointDescGetLocalAnchor(int jointDescID, int AorB, int vector3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescGetLocalAnchor");
            return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->jointDescMap_[jointDescID]->localAnchor[AorB];
		dbSetVector3(vector3Out, vec.x, vec.y, vec.z);
}


/*
* @description Gets the local axis of one of the two actors associated with the joint descriptor.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param vector3Out - ID number of the vector3 the values will be placed in.
* @dbpro DYN JOINT DESC GET LOCAL AXIS
* @category JOINT
*/
void dynJointDescGetLocalAxis(int jointDescID, int AorB, int vector3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescGetLocalAxis");
            return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->jointDescMap_[jointDescID]->localAxis[AorB];
		dbSetVector3(vector3Out, vec.x, vec.y, vec.z);
}

/*
* @description Gets the local normal of one of the two actors associated with the joint descriptor.  
* @param jointDescID ID number of the descriptor.
* @param AorB - 0 for actorA, 1 for actorB.
* @param vector3Out - ID number of the vector3 the values will be placed.
* @dbpro DYN JOINT DESC GET LOCAL NORMAL
* @category JOINT
*/
void dynJointDescGetLocalNormal(int jointDescID, int AorB, int vector3Out){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescGetLocalNormal");
            return;
		}
	#endif
		NxVec3 vec = phyGlobStruct_->jointDescMap_[jointDescID]->localNormal[AorB];
		dbSetVector3(vector3Out, vec.x, vec.y, vec.z);
}



/*
* @description Allows you to check if the descriptor is valid before you attempt to create a joint from it. 
* @param jointDescID ID number of the descriptor.
* @return TRUE if valid, FALSE otherwise.
* @dbpro DYN JOINT DESC IS VALID
* @category JOINT
*/
bool dynJointDescIsValid(int jointDescID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescIsValid");
            return 0;
		}
	#endif
		return phyGlobStruct_->jointDescMap_[jointDescID]->isValid();
}




/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET SWING DRIVE
* @category JOINT
*/
void dynJointDescSetSwingDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSwingDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->swingDrive.damping = damping;
		desc->swingDrive.driveType = driveType;
		desc->swingDrive.forceLimit = forceLimit;
		desc->swingDrive.spring = spring;	
}


/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET TWIST DRIVE
* @category JOINT
*/
void dynJointDescSetTwistDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetTwistDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->twistDrive.damping = damping;
		desc->twistDrive.driveType = driveType;
		desc->twistDrive.forceLimit = forceLimit;
		desc->twistDrive.spring = spring;	
}


/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET X DRIVE
* @category JOINT
*/
void dynJointDescSetXDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetXDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->xDrive.damping = damping;
		desc->xDrive.driveType = driveType;
		desc->xDrive.forceLimit = forceLimit;
		desc->xDrive.spring = spring;	
}



/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET Y DRIVE
* @category JOINT
*/
void dynJointDescSetYDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetYDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->yDrive.damping = damping;
		desc->yDrive.driveType = driveType;
		desc->yDrive.forceLimit = forceLimit;
		desc->yDrive.spring = spring;	
}


/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET Z DRIVE
* @category JOINT
*/
void dynJointDescSetZDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetZDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->zDrive.damping = damping;
		desc->zDrive.driveType = driveType;
		desc->zDrive.forceLimit = forceLimit;
		desc->zDrive.spring = spring;	
}


/*
* @description These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is set.  
* @param jointDescID ID number of the descriptor.
* @param driveType - NX_D6JOINT_DRIVE_POSITION = 1, NX_D6JOINT_DRIVE_VELOCITY = 2. Default value = 0
* @param forceLimit - The maximum force (or torque) the drive can exert. Default value = max float
* @param damping - Damper coefficient. Default value = 0
* @param spring - Spring coefficient. Default value = 0
* @dbpro DYN JOINT DESC SET SLERP DRIVE
* @category JOINT
*/
void dynJointDescSetSlerpDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetSlerpDrive");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->slerpDrive.damping = damping;
		desc->slerpDrive.driveType = driveType;
		desc->slerpDrive.forceLimit = forceLimit;
		desc->slerpDrive.spring = spring;	
}


/*
* @description when the flag NX_D6JOINT_GEAR_ENABLED is set, the angular velocity of the second actor is driven towards the angular velocity of the first actor times gearRatio (both w.r.t. their primary axis)   
* @param jointDescID ID number of the descriptor.
* @param ratio - Default = 1.0, Range = -inf to +inf
* @dbpro DYN JOINT DESC SET GEAR RATIO
* @category JOINT
*/
void dynJointDescSetGearRatio(int jointDescID, float ratio){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
			Message::jointDescNotExist(jointDescID, "dynJointDescSetGearRatio");
            return;
		}
	#endif
		NxD6JointDesc *desc = phyGlobStruct_->jointDescMap_[jointDescID];
		desc->gearRatio = ratio;	
}










/*
* @description Allows you to check if a joint has been broken, this may be due to one of the actors being deleted or the joint reached it's force or torque limit.  
* @param jointID ID number of the joint.
* @return TRUE if broken, FALSE otherwise.
* @dbpro DYN JOINT IS BROKEN
* @category JOINT
*/
bool dynJointIsBroken(int jointID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointIsBroken");
            return false;
		}
	#endif
		NxJointState state = phyGlobStruct_->jointMap_[jointID]->getState();
		if((int)state == 2){
            return true;
		}
		else{
            return false;
		}
}


/*
* @description Set the drive angular velocity goal when it is being driven. The drive angular velocity is specified relative to the drive orientation target in the case of a slerp drive. The drive angular velocity is specified in the actor[0] or (ActorA) joint frame in all other cases.
* @param jointID - ID number of the joint.
* @param x - Velocity about the x axis.
* @param y - Velocity about the y axis.
* @param z - Velocity about the z axis.
* @dbpro DYN JOINT SET DRIVE ANGULAR VELOCITY
* @category JOINT
*/
void dynJointSetDriveAngularVelocity(int jointID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveAngularVelocity");
            return;
		}
	#endif
		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDriveAngularVelocity(NxVec3(x, y, z));
}

/*
* @description Set the drive goal linear velocity when it is being driven. The drive linear velocity is specified relative to the actor[0] joint frame.
* @param jointID - ID number of the joint.
* @param x - Velocity along the x axis.
* @param y - Velocity along the y axis.
* @param z - Velocity along the z axis.
* @dbpro DYN JOINT SET DRIVE LINEAR VELOCITY
* @category JOINT
*/
void dynJointSetDriveLinearVelocity(int jointID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveLinearVelocity");
            return;
		}
	#endif
		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDriveLinearVelocity(NxVec3(x, y, z));
}

/*
* @description Set the drive position goal position when it is being driven. The goal position is specified relative to the joint frame corresponding to actor[0].
* @param jointID - ID number of the joint.
* @param x - The goal position if NX_D6JOINT_DRIVE_POSITION is set for xDrive.
* @param y - The goal position if NX_D6JOINT_DRIVE_POSITION is set for yDrive.
* @param z - The goal position if NX_D6JOINT_DRIVE_POSITION is set for Drive.
* @dbpro DYN JOINT SET DRIVE POSITION
* @category JOINT
*/
void dynJointSetDrivePosition(int jointID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDrivePosition");
            return;
		}
	#endif
		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDrivePosition(NxVec3(x, y, z));
}


/*
* @description Set the drive goal orientation when it is being driven. The goal orientation is specified relative to the joint frame corresponding to actor[0].
* @param jointID - ID number of the joint.
* @param w - Quaternion element.
* @param x - Quaternion element.
* @param y - Quaternion element.
* @param z - Quaternion element.
* @dbpro DYN JOINT SET DRIVE ORIENTATION
* @category JOINT
*/
void dynJointSetDriveOrientation(int jointID, float w, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveOrientation");
            return;
		}
	#endif
	    NxQuat quat;
		quat.x = x;
		quat.y = y;
		quat.z = z;
		quat.w = w;
		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDriveOrientation(quat);
}

/*
* @description Set the drive goal orientation when it is being driven. The goal orientation is specified relative to the joint frame corresponding to actor[0].
* @param jointID - ID number of the joint.
* @param x - Angle value about the x axis.
* @param y - Angle value about the y axis.
* @param z - Angle value about the z axis.
* @dbpro DYN JOINT SET DRIVE ORIENTATION FROM GLOBAL AXIS
* @category JOINT
*/
void dynJointSetDriveOrientationFromGlobalAxis(int jointID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveOrientationFromGlobalAxis");
            return;
		}
	#endif
		// Quaternion
		NxQuat q1, q2, q3;
		q1.fromAngleAxis(x, NxVec3(1,0,0));
		q2.fromAngleAxis(y, NxVec3(0,1,0));
		q3.fromAngleAxis(z, NxVec3(0,0,1));

        NxQuat q;	
        q = q3*q2*q1;  // Use global axes
        // q = q1*q2*q3;  // Use local axes

		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDriveOrientation(q);
}

/*
* @description Set the drive goal orientation when it is being driven. The goal orientation is specified relative to the joint frame corresponding to actor[0].
* @param jointID - ID number of the joint.
* @param x - Angle value about the x axis.
* @param y - Angle value about the y axis.
* @param z - Angle value about the z axis.
* @dbpro DYN JOINT SET DRIVE ORIENTATION FROM LOCAL AXIS
* @category JOINT
*/
void dynJointSetDriveOrientationFromLocalAxis(int jointID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveOrientationFromLocalAxis");
            return;
		}
	#endif
		// Quaternion
		NxQuat q1, q2, q3;
		q1.fromAngleAxis(x, NxVec3(1,0,0));
		q2.fromAngleAxis(y, NxVec3(0,1,0));
		q3.fromAngleAxis(z, NxVec3(0,0,1));

        NxQuat q;	
        //q = q3*q2*q1;  // Use global axes
        q = q1*q2*q3;  // Use local axes

		NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
		joint->setDriveOrientation(q);
}


/*
* @description Set the drive goal orientation when it is being driven. The goal orientation is specified relative to the joint frame corresponding to actor[0].
* @param jointID - ID number of the joint.
* @param axisX- Normalised axis value.
* @param axisY - Normalised axis value.
* @param axisZ - Normalised axis value.
* @param angle - Angle value.
* @dbpro DYN JOINT SET DRIVE ORIENTATION FROM ANGLE AXIS
* @category JOINT
*/
void dynJointSetDriveOrientationFromAngleAxis(int jointID, float axisX, float axisY, float axisZ, float angle){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->jointMap_.count(jointID) == 0){
			Message::jointNotExist(jointID, "dynJointSetDriveOrientationFromAngleAxis");
            return;
		}
	#endif
    NxVec3 v(axisX, axisY, axisZ);
    NxQuat q;
    q.fromAngleAxis(angle, v);
	NxD6Joint *joint = phyGlobStruct_->jointMap_[jointID];
	joint->setDriveOrientation(q);
}









/*
* @description Creates a Wheel actor.
* @param objectID - ID number of the object the wheel will represent in the simulation.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE WHEEL
* @category CONSTRUCTION
*/
bool dynMakeWheel(int objectID, float density){
	#ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynMakeWheel"))
			return false;
	    //Check object exists
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		ObjectFromWheel *aSync = new ObjectFromWheel(objectID);;
		NxActor *actor = 0;
		NxVec3 dim = DBUtil::getObjectDimensions(objectID);
		//Radius = average of all dimensions
		float radius = dim.y;//(dim.x + dim.y + dim.z) / 3;  
		actor = phyGlobStruct_->phy_->makeWheel(aSync,
			                 DBUtil::getMatrixFromObject(objectID),
			                 radius,
			                 density,
							 DBUtil::getCollisionCentre(objectID));		
		if(actor){
		    //Wheel
		    actor->getShapes()[0]->userData = (void*)aSync;
			phyGlobStruct_->actorMap_[objectID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}

/*
* @description Creates a Wheel actor from a limb.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the wheel will represent in the simulation.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE WHEEL
* @category CONSTRUCTION
*/
bool dynMakeWheel(int objectID, int limbID, float radius, float density){
	#ifdef DEBUG
	if(!DBError::limbExists(objectID, limbID, "dynMakeWheel"))
			return false;
	    //Check actor does not exist
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		LimbFromWheel *aSync = new LimbFromWheel(objectID, limbID,DBUtil::getLimbScale(objectID, limbID, phyGlobStruct_->phyScaleMultiplier_));
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeWheel(aSync,
			                 DBUtil::getMatrixFromLimb(objectID, limbID),
			                 radius,
			                 density,
							 NxVec3(0, 0, 0));		//Shape
		//Wheel
		actor->getShapes()[0]->userData = (void*)aSync;

		if(actor){
		    //Wheel
		    actor->getShapes()[0]->userData = (void*)aSync;
			phyGlobStruct_->actorLimbMap_[objectID][limbID] = actor;
		    phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}	
}




/*
* @description This command will update the given wheel shapes position and orientation in repect to the current steer angle, spin and suspension. These aspects of a wheel shape are not updated in the CORE 'update' command as it's also possible for you to add the steer angle, spin and suspension yourself, getting the information you need from some of the other wheel commands provided. 
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @dbpro DYN WHEEL UPDATE
* @category WHEEL
*/
void dynWheelUpdate(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
	NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
	ObjectFromWheel *ud = (ObjectFromWheel*)shape->userData;
	ud->updateWheel(shape);
}

   /*
* @description This command will update the given wheel shapes position and orientation in repect to he current steer angle, spin and suspension. This overload allows you to set the timestep if you are using variable timing.
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param timestep - Amount of time passed since last update.
* @dbpro DYN WHEEL UPDATE
* @category WHEEL
*/
void dynWheelUpdate(int objectID, int shapeIndex, float timestep){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
	NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
	ObjectFromWheel *ud = (ObjectFromWheel*)shape->userData;
	ud->updateWheel(shape, timestep);
}

/*
* @description This command will update the given wheel shapes position and orientation in repect to he current steer angle, spin and suspension. These aspects of a wheel shape are not updated in the CORE 'update' command as it's also possible for you to add the steer angle, spin and suspension yourself, getting the information you need from some of the other wheel commands provided. 
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @dbpro DYN WHEEL UPDATE
* @category WHEEL
*/
void dynWheelUpdate(int objectID, int limbID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, limbID,  "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID,  "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
	NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex];
	LimbFromWheel *ud = (LimbFromWheel*)shape->userData;
	ud->updateWheel(shape);
}

/*
* @description This command will update the given wheel shapes position and orientation in repect to he current steer angle, spin and suspension. This overload allows you to set the timestep if you are using variable timing.
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param timestep - Amount of time passed since last update.
* @dbpro DYN WHEEL UPDATE
* @category WHEEL
*/
void dynWheelUpdate(int objectID, int limbID, int shapeIndex, float timestep){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, limbID,  "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID,  "dynWheelUpdate");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
	NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex];
	LimbFromWheel *ud = (LimbFromWheel*)shape->userData;
	ud->updateWheel(shape, timestep);
}

/*
* @description This command will tell you if the given wheel shape is currently in contact with anything. After using this command, if the wheel shape is currently in contact with anything you can use the other contact commands to get more information about the contact.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @return TRUE if in contact, FALSE otherwise.
* @dbpro DYN WHEEL GET CONTACT
* @category WHEEL
*/
bool dynWheelGetContact(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelGetContact");
		    return false;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return false;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return false;
		}
    #endif
		NxWheelShape *shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		if(shape->getContact(phyGlobStruct_->wheelCData) == 0){
           return false;
		}
		else{
           return true;
		}
}

/*
* @description This command will return the radius of the given wheel shape.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @return TRUE if in contact, FALSE otherwise.
* @dbpro DYN WHEEL GET RADIUS
* @category WHEEL
*/
DWORDFLOAT dynWheelGetRadius(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelGetRadius");
		    return false;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return false;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return false;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		float value = shape->getRadius();
		RETURNCAST
}

/*
* @description This command will return the maximum travel distance along the wheel shapes -Y axis, the minimum suspension travel is always 0.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @return Maximum suspension travel distance for the given wheel shape.
* @dbpro DYN WHEEL GET SUSPENSION TRAVEL
* @category WHEEL
*/
DWORDFLOAT dynWheelGetSuspensionTravel(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelGetSuspensionTravel");
		    return false;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return false;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return false;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		float value = shape->getSuspensionTravel();
        RETURNCAST
}

/*
* @description This command will return the current axle rotation speed of the given wheel shape.   
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @return Current axle speed.
* @dbpro DYN WHEEL GET AXLE SPEED
* @category WHEEL
*/
DWORDFLOAT dynWheelGetAxleSpeed(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelGetAxelSpeed");
		    return false;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return false;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return false;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		float value = shape->getAxleSpeed();
		RETURNCAST
}

/*
* @description Use this command to set the steer angle of the given wheel shape.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param angle - Value you wish to set the steer angle to, in degrees.
* @dbpro DYN WHEEL SET STEER ANGLE
* @category WHEEL
*/
void dynWheelSetSteerAngle(int objectID, int shapeIndex, float angle){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelGetAxelSpeed");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setSteerAngle(6.284f /360.0f * angle);
}

/*
* @description Use this command to set the motor torque of the given wheel shape, this can be positive or negative depending on direction.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param torque - Value you wish to set the motor torque to.
* @dbpro DYN WHEEL SET MOTOR TORQUE
* @category WHEEL
*/
void dynWheelSetMotorTorque(int objectID, int shapeIndex, float torque){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetMotorTorque");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setMotorTorque(torque);
}

/*
* @description Sets the maximum extension distance along shapes local y-axis, minimum extension is 0;
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param suspensionTravel - Value you wish to set the suspension travel to.
* @dbpro DYN WHEEL SET SUSPENSION TRAVEL
* @category WHEEL
*/
void dynWheelSetSuspensionTravel(int objectID, int shapeIndex, float suspensionTravel){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetSuspensionTravel");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setSuspensionTravel(suspensionTravel);
}

/*
* @description Sets the spring variables on the given wheels suspension.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param spring - Spring coefficient.
* @param damper - Damper coefficient, default value is 0.
* @param targetValue - Target position of spring where the spring force is 0, default value is 0.
* @dbpro DYN WHEEL SET SUSPENSION SPRING
* @category WHEEL
*/
void dynWheelSetSuspensionSpring(int objectID, int shapeIndex, float spring, float damper, float targetValue){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetSuspensionSpring");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setSuspension(NxSpringDesc(spring, damper, targetValue));
}

/*
* @description This command will tell you if the given shape in a given actor is a wheel shape, wheel shapes have alot of additional commands as well as the regular shape commands.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @return TRUE if given shape is a wheel, FALSE otherwise.
* @dbpro DYN IS WHEEL
* @category WHEEL
*/
bool dynIsWheel(int objectID, int shapeIndex){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynIsWheel");
		    return false;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return false;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		if(shape->isWheel()){
			return true;
		}
		else{
            return false;
		}
}


/*
* @description Use this command to set the brake torque of the given wheel shape, this must be positive, high values will lock the wheel but should be stable.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param torque - Value you wish to set the brake torque to.
* @dbpro DYN WHEEL SET BRAKE TORQUE
* @category WHEEL
*/
void dynWheelSetBrakeTorque(int objectID, int shapeIndex, float torque){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetBrakeTorque");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setBrakeTorque(torque);
}



/*
* @description Use this command to set the motor torque of the given wheel shape, this can be positive or negative depending on direction.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param torque - Value you wish to set the motor torque to.
* @dbpro DYN WHEEL SET MOTOR TORQUE
* @category WHEEL
*/
void dynWheelSetMotorTorque(int objectID, int limbID, int shapeIndex, float torque){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, limbID, "dynWheelSetMotorTorque");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
		    Message::actorNotExist(objectID, limbID, "dynWheelSetMotorTorque");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex];
		shape->setMotorTorque(torque);
}


/*
* @description If the wheel is currently in contact you can use this command to get the distance on the spring travel distance where the wheel would end up if it was resting on the contact point.  
* @dbpro DYN WHEEL GET CONTACT POSITION
* @category WHEEL
*/
DWORDFLOAT dynWheelGetContactPosition(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
    #endif
		float value =  phyGlobStruct_->wheelCData.contactPosition;
        RETURNCAST
}

/*
* @description This function creates a descriptor of a tire function. You can then change the default asymptote, extremum and stiffness values if desired, you can then set the lateral or longitudal tire force function of a given wheel using this descriptor.  
* @param ID - ID number to assign to the tire function descriptor.
* @dbpro DYN MAKE TIRE FUNC DESC
* @category CONSTRUCTION
*/
bool dynMakeTireFuncDesc(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 1){
			Message::tireFuncDescAlreadyExists(ID, "dynMakeTireFuncDesc");
            return false;
		}
	#endif
    NxTireFunctionDesc* desc = new NxTireFunctionDesc();
	phyGlobStruct_->tireFuncDescMap_[ID] = desc;
	return true;
}

/*
* @description This function will delete the given tire function descriptor.  
* @param ID - ID number of the tire function descriptor.
* @dbpro DYN DELETE TIRE FUNC DESC
* @category CONSTRUCTION
*/
void dynDeleteTireFuncDesc(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynDeleteTireFuncDesc");
            return;
		}
	#endif
    delete phyGlobStruct_->tireFuncDescMap_[ID];
	phyGlobStruct_->tireFuncDescMap_.erase(ID);
}


/*
* @description This function will set the given tire function descriptor back to its default values.  
* @param ID - ID number of the tire function descriptor.
* @dbpro DYN TIRE FUNC DESC SET TO DEFAULT
* @category WHEEL
*/
void dynTireFuncDescSetToDefault(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescSetToDefault");
            return;
		}
	#endif
	phyGlobStruct_->tireFuncDescMap_[ID]->setToDefault();
}


/*
* @description This function will inform you if the given tire function descriptor is valid.  
* @param ID - ID number of the tire function descriptor.
* @return TRUE if valid, FALSE otherwise.
* @dbpro DYN TIRE FUNC DESC IS VALID
* @category WHEEL
*/
bool dynTireFuncDescIsValid(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescIsValid");
            return false;
		}
	#endif
		return phyGlobStruct_->tireFuncDescMap_[ID]->isValid();
}

/*
* @description This function evaluates the force(slip) function.  
* @param ID - ID number of the tire function descriptor.
* @return Hermite eval at the given point.
* @dbpro DYN TIRE FUNC DESC GET HERMITE EVAL
* @category WHEEL
*/
DWORDFLOAT dynTireFuncDescGetHermiteEval(int ID, float t){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescHermiteEval");
            return false;
		}
	#endif
		float value = phyGlobStruct_->tireFuncDescMap_[ID]->hermiteEval(t);
		RETURNCAST
}


/*
* @description This function will set the asymptote variables of the given tire force function descriptor.  
* @param ID - ID number of the tire function descriptor.
* @param asymptoteSlip - Point on curve at which for all x > minumumX, function equals minimumY. Both values must be positive. Range:(0,inf) Default: 2.0
* @param asymptoteValue - Point on curve at which for all x > minumumX, function equals minimumY. Both values must be positive. Range:(0,inf) Default: 0.01
* @dbpro DYN TIRE FUNC DESC SET ASYMPTOTE
* @category WHEEL
*/
void dynTireFuncDescSetAsymptote(int ID, float asymptoteSlip, float asymptoteValue){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescSetAsymptote");
            return;
		}
	#endif
    NxTireFunctionDesc desc = *phyGlobStruct_->tireFuncDescMap_[ID];
	desc.asymptoteSlip = asymptoteSlip;
	desc.asymptoteValue = asymptoteValue;
}

/*
* @description This function will set the extremum variables of the given tire force function descriptor.  
* @param ID - ID number of the tire function descriptor.
* @param extremumSlip - Extremal point of curve. Both values must be positive. Range:(0,inf) Default: 1.0
* @param extremumValue - Extremal point of curve. Both values must be positive. Range:(0,inf) Default: 0.02
* @dbpro DYN TIRE FUNC DESC SET EXTREMUM
* @category WHEEL
*/
void dynTireFuncDescSetExtremum(int ID, float extremumSlip, float extremumValue){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescSetExtremum");
            return;
		}
	#endif
    NxTireFunctionDesc desc = *phyGlobStruct_->tireFuncDescMap_[ID];
	desc.extremumSlip = extremumSlip;
	desc.extremumValue = extremumValue;
}


/*
* @description This function will set the stiffness factor variable of the given tire force function descriptor.  
* @param ID - ID number of the tire function descriptor.
* @param stiffnessFactor - Scaling factor for tire force. This is an additional overall positive scaling that gets applied to the tire forces before passing them to the solver. Higher values make for better grip. If you raise the other tire force function variables, you may need to lower this. A setting of zero will disable all friction in this direction. Range:(0,inf) Default: 1000000.0(quite stiff)
* @dbpro DYN TIRE FUNC DESC SET STIFFNESS FACTOR
* @category WHEEL
*/
void dynTireFuncDescSetStiffnessFactor(int ID, float stiffnessFactor){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(ID) == 0){
			Message::tireFuncDescNotExist(ID, "dynTireFuncDescSetStiffnessFactor");
            return;
		}
	#endif
		phyGlobStruct_->tireFuncDescMap_[ID]->stiffnessFactor = stiffnessFactor;
}

/*
* @description Use this command to set the lateral tire force function of the given wheel shape.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param tireFuncDesc - ID number of the tire function descriptor.
* @dbpro DYN WHEEL SET LAT TIRE FORCE FUNC
* @category WHEEL
*/
void dynWheelSetLatTireForceFunc(int objectID, int shapeIndex, int tireFuncDescID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetLatTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(tireFuncDescID) == 0){
			Message::tireFuncDescNotExist(tireFuncDescID, "dynWheelSetLatTireForceFunc");
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setLateralTireForceFunction(*phyGlobStruct_->tireFuncDescMap_[tireFuncDescID]);
}

/*
* @description Use this command to set the longitudal tire force function of the given wheel shape.  
* @param objectID - ID number of the object the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param tireFuncDesc - ID number of the tire function descriptor.
* @dbpro DYN WHEEL SET LONG TIRE FORCE FUNC
* @category WHEEL
*/
void dynWheelSetLongTireForceFunc(int objectID, int shapeIndex, int tireFuncDescID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
		    Message::actorNotExist(objectID, "dynWheelSetLongTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(tireFuncDescID) == 0){
			Message::tireFuncDescNotExist(tireFuncDescID, "dynWheelSetLongTireForceFunc");
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex];
		shape->setLongitudalTireForceFunction(*phyGlobStruct_->tireFuncDescMap_[tireFuncDescID]);
}

/*
* @description Use this command to set the lateral tire force function of the given wheel shape.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param tireFuncDesc - ID number of the tire function descriptor.
* @dbpro DYN WHEEL SET LAT TIRE FORCE FUNC
* @category WHEEL
*/
void dynWheelSetLatTireForceFunc(int objectID, int limbID, int shapeIndex, int tireFuncDescID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
		    Message::limbNotExist(objectID, limbID, "dynWheelSetLatTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
		    Message::limbNotExist(objectID, limbID, "dynWheelSetLatTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(tireFuncDescID) == 0){
			Message::tireFuncDescNotExist(tireFuncDescID, "dynWheelSetLatTireForceFunc");
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex];
		shape->setLateralTireForceFunction(*phyGlobStruct_->tireFuncDescMap_[tireFuncDescID]);
}

/*
* @description Use this command to set the longitudal tire force function of the given wheel shape.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the wheel represents in the simulation.
* @param shapeIndex - Index value of the wheel shape in the actor, the default shape is 0. 
* @param tireFuncDesc - ID number of the tire function descriptor.
* @dbpro DYN WHEEL SET LONG TIRE FORCE FUNC
* @category WHEEL
*/
void dynWheelSetLongTireForceFunc(int objectID, int limbID, int shapeIndex, int tireFuncDescID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
		    Message::limbNotExist(objectID, limbID, "dynWheelSetLongTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
		    Message::limbNotExist(objectID, limbID, "dynWheelSetLongTireForceFunc");
		    return;
	    }
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (NxU32)shapeIndex){
			return;
		}
		if(!phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]->isWheel()){
            return;
		}
		if(phyGlobStruct_->tireFuncDescMap_.count(tireFuncDescID) == 0){
			Message::tireFuncDescNotExist(tireFuncDescID, "dynWheelSetLongTireForceFunc");
            return;
		}
    #endif
		NxWheelShape* shape = (NxWheelShape*)phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex];
		shape->setLongitudalTireForceFunction(*phyGlobStruct_->tireFuncDescMap_[tireFuncDescID]);
}





/*
 * @description Cooks and then creates a cloth mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
 * @param objectID - ID number of the object to create the mesh from.
 * @param clothMeshID - ID number you wish to assign to the cloth mesh, you use this ID number when creating an actor from this cloth mesh.
 * @param flags - Use this to enable features such as cloth tearing or hardware acceleration. NX_CLOTH_MESH_TEARABLE = 256, NX_MF_HARDWARE_MESH = 4 
 * @return TRUE if succeeded, FALSE otherwise.
 * @dbpro DYN COOK CLOTH MESH
 * @category CONSTRUCTION
*/
bool dynCookClothMesh(int objectID, int clothMeshID, unsigned int flags){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookClothMesh")){
			return false;
		}
		if(phyGlobStruct_->clothMeshMap_.count(clothMeshID)){
			Message::meshAlreadyExists(clothMeshID, "dynCookClothMesh");
			return false;
		}
	#endif
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0); 
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();
		NxClothMesh *pMesh = 0;
		pMesh =  phyGlobStruct_->phy_->cookClothMesh(mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_, flags);
		delete []mesh.indices_;
		delete []mesh.vertices_;

		if(pMesh){
           phyGlobStruct_->clothMeshMap_[clothMeshID] = pMesh;
		   return true;
		}
		else{
           return false;
		}
}


/*
* @description Cooks a cloth mesh and saves it to file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the object to create the mesh from.
* @param file - Name of the file you wish to create.
* @param flags - Use this to enable features such as cloth tearing or hardware acceleration. NX_CLOTH_MESH_TEARABLE = 256, NX_MF_HARDWARE_MESH = 4 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CLOTH MESH
* @category CONSTRUCTION
*/
bool dynCookClothMesh(int objectID, char* file, unsigned int flags){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::meshExists(objectID, "dynCookClothMesh"))
			return false;
	#endif
	    std::string fileStr(file);
		sObject *obj = dbGetObject(objectID);
		MeshData mesh;
		dbLockVertexDataForLimb(objectID, 0);
		DBUtil::getMeshData(obj, mesh);
		dbUnlockVertexData();
		bool result = phyGlobStruct_->phy_->cookClothMesh(fileStr, mesh.vertexCount_,
		mesh.indexCount_, mesh.vertices_, mesh.indices_, flags);
		delete []mesh.indices_;
		delete []mesh.vertices_;
		return result;
}

/*
* @description Loads a cloth mesh which has been pre-cooked to file.
* @param file - Name of the mesh file you wish to load the mesh from.
* @param clothMeshID - ID number you wish to assign to the cloth mesh, use this number when creating any actor(s) that want to use this mesh.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN LOAD CLOTH MESH
* @category CONSTRUCTION
*/
bool dynLoadClothMesh(char* file, int clothMeshID){
	std::string fileStr(file);
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::fileExists(fileStr, "dynLoadClothMesh"))
			return false;
		if(phyGlobStruct_->clothMeshMap_.count(clothMeshID) == 1){
			Message::meshAlreadyExists(clothMeshID, "dynLoadClothMesh");
			return false;
		}
	#endif
		NxClothMesh* pMesh = 0;
		pMesh = phyGlobStruct_->phy_->loadClothMesh(fileStr);
		if(pMesh){
		    phyGlobStruct_->clothMeshMap_[clothMeshID] = pMesh;
			return true;
		}
		else{
			return false;
		}
}

/*
* @description Releases the mesh from memory, it's very important to call this when you are done using the mesh, you can only release the mesh if it is not being used by any actors.
* @param convexMeshID - ID number of the mesh.
* @return TRUE if succeeded, FALSE otherwise. NOTE: Will return false if mesh is currently in use.
* @dbpro DYN DELETE CLOTH MESH
* @category CONSTRUCTION
*/
void dynDeleteClothMesh(int clothMeshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->clothMeshMap_.count(clothMeshID) == 0){
			Message::meshActorNotExist(clothMeshID, "dynDeleteClothMesh");
			return;
		}
    #endif
		bool result = phyGlobStruct_->phy_->releaseClothMesh(phyGlobStruct_->clothMeshMap_[clothMeshID]);
		if(result){
			phyGlobStruct_->clothMeshMap_.erase(clothMeshID);
			return;
		}
		else{
			Message::errorBox(string("Mesh not deleted, delete all actors that reference the mesh first"), string("dynDeleteClothMesh"));
			return;
		}
}

/*
* @description Creates a cloth from a cloth mesh.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param clothMeshID - ID number of the mesh you wish to create the actor from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CLOTH
* @category CONSTRUCTION
*/
bool dynMakeCloth(int objectID, int clothMeshID){
	return dynMakeCloth(objectID, clothMeshID, 0);
 //   #ifdef DEBUG
	//	if(!phyGlobStruct_->phy_){
	//		Message::ignitionError();
	//		return false;
	//	}
	//	//Check mesh?
	//	if(!DBError::objectExists(objectID, "dynMakeCloth"))
	//		return false;
	//	if(!phyGlobStruct_->clothMeshMap_.count(clothMeshID)){
	//		Message::meshActorNotExist(clothMeshID, "dynMakeCloth");
	//		return false;
	//	}
	//	if(phyGlobStruct_->clothMap_.count(objectID)){
	//		Message::actorAlreadyExist(objectID, "dynMakeCloth");
 //           return false;
	//	}
	//#endif
	//	NxClothMesh *pMesh = phyGlobStruct_->clothMeshMap_[clothMeshID];
	//	//Create updater object
	//	ObjectFromCloth *aSync = new ObjectFromCloth(objectID, NxVec3(1, 1, 1));
	//	NxCloth* cloth = 0;
	//	cloth = phyGlobStruct_->phy_->makeCloth(aSync, pMesh, DBUtil::getMatrixFromObject(objectID), 0);
	//	if(cloth){
	//		phyGlobStruct_->clothMap_[objectID] = cloth;
	//		phyGlobStruct_->clothSyncList_.push_back(aSync);
	//	    return true;
	//	}
	//	else{
 //           delete aSync;
	//		aSync = 0;
 //           return false;
	//	}
}


/*
* @description Creates a cloth from a cloth mesh.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param clothMeshID - ID number of the mesh you wish to create the actor from.
* @param flags - Flags for the cloth, allows you to set certain properties for the cloth. If simulating on hardware or simulating with tearing you must set the flag here, all other flags can be set after creation if desired.   
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CLOTH
* @category CONSTRUCTION
*/
bool dynMakeCloth(int objectID, int clothMeshID, unsigned int flags){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		//Check mesh?
		if(!DBError::objectExists(objectID, "dynMakeCloth"))
			return false;
		if(!phyGlobStruct_->clothMeshMap_.count(clothMeshID)){
			Message::meshActorNotExist(clothMeshID, "dynMakeCloth");
			return false;
		}
		if(phyGlobStruct_->clothMap_.count(objectID)){
			Message::actorAlreadyExist(objectID, "dynMakeCloth");
            return false;
		}
	#endif
		NxClothMesh *pMesh = phyGlobStruct_->clothMeshMap_[clothMeshID];
		//Create updater object
		ObjectFromCloth *aSync = new ObjectFromCloth(objectID, NxVec3(1, 1, 1));
		NxCloth* cloth = 0;
		cloth = phyGlobStruct_->phy_->makeCloth(aSync, pMesh, DBUtil::getMatrixFromObject(objectID), flags);
		if(cloth){
			phyGlobStruct_->clothMap_[objectID] = cloth;
			phyGlobStruct_->clothSyncList_.push_back(aSync);
		    return true;
		}
		else{
            delete aSync;
			aSync = 0;
            return false;
		}
}



/*
* @description Releases the given cloth, this does not release the mesh which was used to create the cloth. 
* @param objectID - ID number of the object the cloth represents in the simulation.
* @dbpro DYN DELETE CLOTH
* @category CONSTRUCTION
*/
void dynDeleteCloth(int objectID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynDeleteCloth");
			return;
		}
    #endif		
	NxCloth* cloth = 0;
	cloth = phyGlobStruct_->clothMap_[objectID];
	if(cloth)
	{
		ObjectFromCloth *aSync = (ObjectFromCloth*)cloth->userData;
		NxScene* scene = &cloth->getScene();
		scene->releaseCloth(*cloth);
		//remove sync from map
		delete aSync;
		phyGlobStruct_->clothSyncList_.remove(aSync);
		//remove from map
		phyGlobStruct_->clothMap_.erase(objectID);
	}
}

/*
* @description Checks whether a cloth exists for the given object. 
* @param objectID - ID number of the object the cloth represents in the simulation.
* @dbpro DYN CLOTH EXISTS
* @category CONSTRUCTION
*/
bool dynClothExists(int objectID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
    #endif	
	return phyGlobStruct_->clothMap_.count(objectID);
}

/*
* @description Sets the given flags for the given cloth, these enable/disable certain features such as debug rendering, self collision etc. See cloth example for list of flags available. 
* @param objectID - ID number of the object the cloth represents in the simulation.
* @param flags - Flag or combination of flags which will determine what cloth features are enabled.
* @dbpro DYN CLOTH SET FLAGS
* @category CLOTH
*/
void dynClothSetFlags(int clothID, unsigned int flags)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetFlags");
			return;
		}
    #endif	
		phyGlobStruct_->clothMap_[clothID]->setFlags(flags);
}

/*
* @description Gets the given flags for the given cloth, these enable/disable certain features such as debug rendering, self collision etc. See cloth example for list of flags available. 
* @param objectID - ID number of the object the cloth represents in the simulation.
* @dbpro DYN CLOTH GET FLAGS
* @category CLOTH
*/
unsigned int dynClothGetFlags(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetFlags");
			return 0;
		}
    #endif	
		return phyGlobStruct_->clothMap_[clothID]->getFlags();
}


/*
* @description Attaches the cloth to the given shape, it will attach the cloth vertices which are currently inside the given shape.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param shapeIndex - Shape index, default shape index of an actor is 0.
* @param attachmentFlags - Flag or combination of flags which will determine whether the attachment is one way or two way or whether the attachment is tearable.
* @dbpro DYN CLOTH ATTACH TO SHAPE
* @category CLOTH
*/
void dynClothAttachToShape(int clothID, int objectID, int shapeIndex, unsigned int attachmentFlags)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynClothAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynClothAttachToShape");
            return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAttachToShape");
			return;
		}
    #endif	
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	NxActor * actor = phyGlobStruct_->actorMap_[objectID];
	NxShape * shape = actor->getShapes()[shapeIndex];
	cloth->attachToShape(shape, attachmentFlags);
}

/*
* @description Detaches the cloth from the given shape.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param shapeIndex - Shape index, default shape index of an actor is 0.
* @dbpro DYN CLOTH DETACH FROM SHAPE
* @category CLOTH
*/
void dynClothDetachFromShape(int clothID, int objectID, int shapeIndex)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynClothDetachFromShape");
			return;
		}
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynClothDetachFromShape");
            return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothDetachFromShape");
			return;
		}
    #endif	
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	NxActor * actor = phyGlobStruct_->actorMap_[objectID];
	NxShape * shape = actor->getShapes()[shapeIndex];
	cloth->detachFromShape(shape);
}


/*
* @description Attaches the cloth to any shape it is currently colliding with.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param attachmentFlags - Flag or combination of flags which will determine whether the attachment is one way or two way or whether the attachment is tearable.
* @dbpro DYN CLOTH ATTACH TO COLLIDING SHAPES
* @category CLOTH
*/
void dynClothAttachToCollidingShapes(int clothID, unsigned int attachmentFlags)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAttachToCollidingShapes");
			return;
		}
    #endif

	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setStretchingStiffness(1.0);
	cloth->setHardStretchLimitationFactor(1.0f);
	cloth->attachToCollidingShapes(attachmentFlags);
}




/*
* @description Sets the bending stiffness of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - Bending stiffness in the range from 0 to 1.
* @dbpro DYN CLOTH SET BENDING STIFFNESS
* @category CLOTH
*/
void dynClothSetBendingStiffness(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetBendingStiffness");
			return;
		}
    #endif
    
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setBendingStiffness(value);
}

/*
* @description Gets the bending stiffness of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return Bending stiffness.
* @dbpro DYN CLOTH GET BENDING STIFFNESS
* @category CLOTH
*/
DWORDFLOAT dynClothGetBendingStiffness(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetBendingStiffness");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getBendingStiffness();
	RETURNCAST
}

/*
* @description Sets the stretching stiffness of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - Stretching stiffness.
* @dbpro DYN CLOTH SET STRETCHING STIFFNESS
* @category CLOTH
*/
void dynClothSetStretchingStiffness(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetStretchingStiffness");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setStretchingStiffness(value);
}

/*
* @description Gets the stretching stiffness of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return Stretching stiffness.
* @dbpro DYN CLOTH GET STRETCHING STIFFNESS
* @category CLOTH
*/
DWORDFLOAT dynClothGetStretchingStiffness(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetStretchingStiffness");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getStretchingStiffness();
	RETURNCAST
}





/*
* @description Sets the hard stretch elongation limit of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - Hard stretch limitation factor.
* @dbpro DYN CLOTH SET HARD STRETCH LIMITATION FACTOR
* @category CLOTH
*/
void dynClothSetHardStretchLimitationFactor(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetHardStretchLimitationFactor");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setHardStretchLimitationFactor(value);
}

/*
* @description Gets the hard stretch elongation limit of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return Hard stretch limitation factor.
* @dbpro DYN CLOTH GET HARD STRETCH LIMITATION FACTOR
* @category CLOTH
*/
DWORDFLOAT dynClothGetHardStretchLimitationFactor(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetHardStretchLimitationFactor");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getHardStretchLimitationFactor();
	RETURNCAST
}

/*
* @description Sets the damping coefficient of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - Damping coefficient, in the range from 0 to 1.
* @dbpro DYN CLOTH SET DAMPING COEFFICIENT
* @category CLOTH
*/
void dynClothSetDampingCoefficient(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetDampingCoefficient");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setDampingCoefficient(value);
}

/*
* @description Gets the damping coefficient of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return Damping coefficient.
* @dbpro DYN CLOTH GET DAMPING COEFFICIENT
* @category CLOTH
*/
DWORDFLOAT dynClothGetDampingCoefficient(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetDampingCoefficient");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getDampingCoefficient();
	RETURNCAST
}

/*
* @description Sets the friction of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - friction, in the range from 0 to 1.
* @dbpro DYN CLOTH SET FRICTION
* @category CLOTH
*/
void dynClothSetFriction(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetFriction");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setFriction(value);
}

/*
* @description Gets the friction of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return friction.
* @dbpro DYN CLOTH GET FRICTION
* @category CLOTH
*/
DWORDFLOAT dynClothGetFriction(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetFriction");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getFriction();
	RETURNCAST
}


/*
* @description Sets the pressure of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - pressure, must not be negative.
* @dbpro DYN CLOTH SET PRESSURE
* @category CLOTH
*/
void dynClothSetPressure(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetPressure");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setPressure(value);
}

/*
* @description Gets the pressure of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return pressure.
* @dbpro DYN CLOTH GET PRESSURE
* @category CLOTH
*/
DWORDFLOAT dynClothGetPressure(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetPressure");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getPressure();
	RETURNCAST
}






/*
* @description Sets the attachment tear factor of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - attachment tear factor, must be larger than 1.
* @dbpro DYN CLOTH SET ATTACHMENT TEAR FACTOR
* @category CLOTH
*/
void dynClothSetAttachmentTearFactor(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetAttachmentTearFactor");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setAttachmentTearFactor(value);
}

/*
* @description Gets the attachment tear factor of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return attachment tear factor.
* @dbpro DYN CLOTH GET ATTACHMENT TEAR FACTOR
* @category CLOTH
*/
DWORDFLOAT dynClothGetAttachmentTearFactor(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetAttachmentTearFactor");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getAttachmentTearFactor();
	RETURNCAST
}



/*
* @description Sets the thickness of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - thickness, must be positive.
* @dbpro DYN CLOTH SET THICKNESS
* @category CLOTH
*/
void dynClothSetThickness(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetThickness");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setThickness(value);
}

/*
* @description Gets the thickness of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return thickness.
* @dbpro DYN CLOTH GET THICKNESS
* @category CLOTH
*/
DWORDFLOAT dynClothGetThickness(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetThickness");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getThickness();
	RETURNCAST
}


/*
* @description Sets the self collision thickness (particle diameter) of the cloth, there may be a flag which also needs to be raised, see cloth example for list of flags.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param value - self collision thickness, must be positive.
* @dbpro DYN CLOTH SET SELF COLLISION THICKNESS
* @category CLOTH
*/
void dynClothSetSelfCollisionThickness(int clothID, float value)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetSelfCollisionThickness");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setSelfCollisionThickness(value);
}

/*
* @description Gets the self collision thickness of the cloth.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return self collision thickness.
* @dbpro DYN CLOTH GET SELF COLLISION THICKNESS
* @category CLOTH
*/
DWORDFLOAT dynClothGetSelfCollisionThickness(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetSelfCollisionThickness");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	float value = cloth->getSelfCollisionThickness();
	RETURNCAST
}


/*
* @description Sets the collision group for the given cloth, default group is 0.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param group - Collision group for the given cloth.
* @dbpro DYN CLOTH SET GROUP
* @category CLOTH
*/
void dynClothSetGroup(int clothID, int group)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetGroup");
			return;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->setGroup((NxCollisionGroup)group);
}

/*
* @description Gets the collision group for the given cloth, default group is 0.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return Group ID.
* @dbpro DYN CLOTH GET GROUP
* @category CLOTH
*/
int dynClothGetGroup(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetGroup");
			return 0;
		}
    #endif
	NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	return(NxCollisionGroup)cloth->getGroup();
}

/*
* @description Checks whether a cloth mesh exists. 
* @param clothMeshID - ID number of the cloth mesh.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN CLOTH MESH EXISTS
* @category CLOTH
*/
bool dynClothMeshExists(int meshID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
    #endif	
	return phyGlobStruct_->clothMeshMap_.count(meshID);
}


/*
* @description Allows a cloth to simulate a metal surface, normal simulation will use the attached actor/object until a force over a given magnitude is detected, the simulation will then run on the cloth to simulate metal deformation. Call this function only once right after the cloth is created. Turning cloth into metal and vice versa during the simulation is not recommended. This feature is well suited for volumetric objects like barrels. It cannot handle two dimensional flat pieces well. The core actor's geometry is adjusted automatically. Its size also depends on the cloth thickness. Thus, it is recommended to choose small values for the thickness. At impacts, colliding objects are moved closer to the cloth by the value provided in penetrationDepth which causes a more dramatic collision result. The core actor must have at least one shape, and currently supported shapes are spheres, capsules, boxes and compounds of spheres. It is recommended to specify the density rather than the mass of the core body. This way the mass and inertia tensor are updated when the core deforms.
* @param clothMeshID - ID number of the object the cloth represents in the simulation.
* @param objectID - ID number of the object the attached actor represents in the simulation.
* @param impulseThreshold - Threshold for when deformation is allowed. 
* @param penetrationDepth - Amount by which colliding objects are brought closer to the cloth. 
* @param maxDeformationDistance - Maximum deviation of cloth particles from initial position.
* @dbpro DYN CLOTH ATTACH TO CORE
* @category CLOTH
*/
void dynClothAttachToCore(int clothID, int objectID, float impulseThreshold, float penetrationDepth, float maxDeformationDistance)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAttachToCore");
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynClothAttachToCore");
		}
    #endif
		NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
		NxActor * actor = phyGlobStruct_->actorMap_[objectID];
		cloth->attachToCore(actor, impulseThreshold, penetrationDepth, maxDeformationDistance);
}


/*
* @description Sets an acceleration acting normal to the cloth surface at each vertex. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param normalVector3ID - The acceleration vector (unit length / s^2). 
* @dbpro DYN CLOTH SET WIND ACCELERATION
* @category CLOTH
*/
void dynClothSetWindAcceleration(int clothID, int normalVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothSetWindAcceleration");
			return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
    #ifdef COMPILE_FOR_GDK
	    cloth->setWindAcceleration(NxVec3(dbXVector3(normalVector3ID), dbYVector3(normalVector3ID), dbZVector3(normalVector3ID))); 
    #else
        DWORD tempNormalX = dbXVector3(normalVector3ID);
        DWORD tempNormalY = dbYVector3(normalVector3ID);
        DWORD tempNormalZ = dbZVector3(normalVector3ID);
	    cloth->setWindAcceleration(NxVec3( *(float*)&tempNormalX, *(float*)&tempNormalY, *(float*)&tempNormalZ)); 
    #endif
}


/*
* @description Gets an acceleration acting normal to the cloth surface at each vertex. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param normalVector3ID - The acceleration vector(unit length / s^2). ID number of the vector3 you wish to fill. 
* @dbpro DYN CLOTH GET WIND ACCELERATION
* @category CLOTH
*/
void dynClothGetWindAcceleration(int clothID, int normalVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetWindAcceleration");
			return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	NxVec3 vec = cloth->getWindAcceleration(); 
    dbSetVector3(normalVector3ID, vec.x, vec.y, vec.z);
}

/*
* @description Casts a ray against the given cloth. You can retrieve the hit position and the closest vertexID using the other cloth raycast commands, be sure to only do so if this command returns true otherwise the data will be from the last ray that hit. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param origVector3ID - Origin of the ray in world space. 
* @param dirVector3ID - Direction the ray should travel.
* @return TRUE if ray hits cloth, FALSE otherwise.
* @dbpro DYN CLOTH RAYCAST
* @category CLOTH
*/
bool dynClothRaycast(int clothID, int origVector3ID, int dirVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothRaycast");
			return false;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	bool result;
    #ifdef COMPILE_FOR_GDK
        NxRay ray;
		ray.dir = NxVec3(dbXVector3(dirVector3ID), dbYVector3(dirVector3ID), dbZVector3(dirVector3ID));
		ray.orig = NxVec3(dbXVector3(origVector3ID), dbYVector3(origVector3ID), dbZVector3(origVector3ID));
		result = cloth->raycast(ray, phyGlobStruct_->clothRayHit_, phyGlobStruct_->clothRayVertexID_); 
    #else
        DWORD tempDirX = dbXVector3(dirVector3ID);
        DWORD tempDirY = dbYVector3(dirVector3ID);
        DWORD tempDirZ = dbZVector3(dirVector3ID);
        DWORD tempOrigX = dbXVector3(origVector3ID);
        DWORD tempOrigY = dbYVector3(origVector3ID);
        DWORD tempOrigZ = dbZVector3(origVector3ID);
	    NxRay ray;
		ray.dir = NxVec3(*(float*)&tempDirX, *(float*)&tempDirY, *(float*)&tempDirZ);
		ray.orig = NxVec3(*(float*)&tempOrigX, *(float*)&tempOrigY, *(float*)&tempOrigZ);
    #endif
	return cloth->raycast(ray, phyGlobStruct_->clothRayHit_, phyGlobStruct_->clothRayVertexID_);  	
}


/*
* @description When a ray has been cast against a cloth and it has hit the cloth then the world position of the hit can be retrieved using this command. 
* @param posVector3ID - ID number of the vector you wish the data to be stored in.
* @dbpro DYN CLOTH RAYCAST GET HIT POS
* @category CLOTH
*/
void dynClothRaycastGetHitPos(int posVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    #endif
	dbSetVector3(posVector3ID, phyGlobStruct_->clothRayHit_.x, phyGlobStruct_->clothRayHit_.y, phyGlobStruct_->clothRayHit_.z);
}

/*
* @description When a ray has been cast against a cloth and it has hit the cloth then the closest vertexID can be retrieved using this command. 
* @return ID number of the closest vertex.
* @dbpro DYN CLOTH RAYCAST GET VERTEX ID
* @category CLOTH
*/
int dynClothRaycastGetVertexID()
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
    #endif
	return (int)phyGlobStruct_->clothRayVertexID_;
}


/*
* @description Tears a given vertex in the given direction. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param vertexID - ID number of the vertex to be torn. 
* @param normalVector3ID - Direction the tear should take, if possible.
* @return TRUE if cloth is torn, FALSE otherwise.
* @dbpro DYN CLOTH TEAR VERTEX
* @category CLOTH
*/
bool dynClothTearVertex(int clothID, int vertexID, int normalVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothTearVertex");
			return false;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	NxVec3 normal;
    #ifdef COMPILE_FOR_GDK
        normal.x = dbXVector3(normalVector3ID);
        normal.y = dbYVector3(normalVector3ID);
        normal.z = dbZVector3(normalVector3ID);
    #else
	    DWORD tempX = dbXVector3(normalVector3ID);
	    DWORD tempY = dbYVector3(normalVector3ID);
	    DWORD tempZ = dbZVector3(normalVector3ID);
        normal.x = *(float*)&tempX;
        normal.y = *(float*)&tempY;
        normal.z = *(float*)&tempZ;
    #endif
    return cloth->tearVertex(vertexID, normal);
}

/*
* @description Gets the current world position of the given vertex of the given cloth. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param vertexID - ID number of the vertex. 
* @param posVec3ID - ID number of the vector you wish to store the position in.
* @dbpro DYN CLOTH GET VERTEX POS
* @category CLOTH
*/
void dynClothGetVertexPos(int clothID, int vertexID, int posVec3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothGetVertexPos");
			return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	NxVec3 pos = cloth->getPosition(vertexID);
	dbSetVector3(posVec3ID, pos.x, pos.y, pos.z);
}

/*
* @description Allows you to check if the cloth is currently sleeping. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @return TRUE if cloth is asleep, FALSE otherwise.
* @dbpro DYN CLOTH IS SLEEPING
* @category CLOTH
*/
bool dynClothIsSleeping(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothIsSleeping");
			return false;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	return cloth->isSleeping();
}


/*
* @description Allows you to put a given cloth to sleep. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @dbpro DYN CLOTH PUT TO SLEEP
* @category CLOTH
*/
void dynClothPutToSleep(int clothID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothPutToSleep");
			return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->putToSleep();
}


/*
* @description Allows you to wake a cloth for a given amount of time. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param wakeCounterValue - Amount of time to wake the cloth for, a value of 0 is the same as putting the cloth to sleep. 
* @dbpro DYN CLOTH WAKE UP
* @category CLOTH
*/
void dynClothWakeUp(int clothID, float wakeCounterValue)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothWakeUp");
			return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
	cloth->wakeUp(wakeCounterValue);
}




/*
* @description This command will apply the given force at the given cloth vertex. 
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param vertexID - ID number of the vertex you wish to apply the force to.
* @param forceVec3ID - ID number of the vector containing the force to be added.
* @dbpro DYN CLOTH ADD FORCE AT VERTEX
* @category CLOTH
*/
void dynClothAddForceAtVertex(int clothID, int vertexID, int forceVec3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAddForceAtVertex");
            return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(forceVec3ID), dbYVector3(forceVec3ID), dbZVector3(forceVec3ID));
		cloth->addForceAtVertex(vec, vertexID);
    #else
        DWORD X = dbXVector3(forceVec3ID);
        DWORD Y = dbYVector3(forceVec3ID);
        DWORD Z = dbZVector3(forceVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		cloth->addForceAtVertex(vec, vertexID);
    #endif
}


/*
* @description Applies a radial force (or impulse) at a particular position. All vertices within radius will be affected with a quadratic drop-off. Because forces are reset at the end of every timestep, you can maintain a total external force on an object by calling this once every frame.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param posVec3ID - ID number of the vector containing the position.
* @param magnitude - Magnitude of the force/impulse to apply.
* @param radius - The sphere radius in which particles will be affected.
* @dbpro DYN CLOTH ADD FORCE AT POS
* @category CLOTH
*/
void dynClothAddForceAtPos(int clothID, int posVec3ID, float magnitude, float radius)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAddForceAtPos");
            return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(posVec3ID), dbYVector3(posVec3ID), dbZVector3(posVec3ID));
		cloth->addForceAtPos(vec, magnitude, radius);
    #else
        DWORD X = dbXVector3(posVec3ID);
        DWORD Y = dbYVector3(posVec3ID);
        DWORD Z = dbZVector3(posVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		cloth->addForceAtPos(vec, magnitude, radius);
    #endif
}

/*
* @description Applies a directed force (or impulse) at a particular position. All vertices within radius will be affected with a quadratic drop-off. Because forces are reset at the end of every timestep, you can maintain a total external force on an object by calling this once every frame.
* @param clothID - ID number of the object the cloth represents in the simulation.
* @param posVec3ID - ID number of the vector containing the position.
* @param forceVec3ID - ID number of the vector containing the force.
* @param radius - The sphere radius in which particles will be affected.
* @dbpro DYN CLOTH ADD DIRECTED FORCE AT POS
* @category CLOTH
*/
void dynClothAddDirectedForceAtPos(int clothID, int posVec3ID, int forceVec3ID, float radius)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->clothMap_.count(clothID)){
			Message::actorNotExist(clothID, "dynClothAddDirectedForceAtPos");
            return;
		}
    #endif
    NxCloth * cloth = phyGlobStruct_->clothMap_[clothID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(posVec3ID), dbYVector3(posVec3ID), dbZVector3(posVec3ID));
        NxVec3 vec2(dbXVector3(forceVec3ID), dbYVector3(forceVec3ID), dbZVector3(forceVec3ID));
		cloth->addDirectedForceAtPos(vec, vec2, radius);
    #else
        DWORD X = dbXVector3(posVec3ID);
        DWORD Y = dbYVector3(posVec3ID);
        DWORD Z = dbZVector3(posVec3ID);
        DWORD X2 = dbXVector3(forceVec3ID);
        DWORD Y2 = dbYVector3(forceVec3ID);
        DWORD Z2 = dbZVector3(forceVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		NxVec3 vec2(*(float*)&X2, *(float*)&Y2, *(float*)&Z2);
		cloth->addDirectedForceAtPos(vec, vec2, radius);
    #endif
}










/*
* @description Utility function which attempts to increase the vertex buffer for the given object. This can be used for cloth tearing. When the cloth tears, more vertices are added by the simulation so it is important that your object can accomodate these extra vertices. You could achieve the same result in a modelling program. Note that cloth tearing does not create extra indices or primitives, the amount of triangles remains the same but some of the existing indices are altered to use the new vertices as the simulation adds them.
* @param clothID - ID number of the object.
* @param extraVertexCount - Amount of new vertices to be added to the object.
* @param extraIndexCount - Amount of new indices to be added to the object, should be 0 if intention is to enable cloth tearing.
* @return The amount of new vertices successfully added, should be the same as 'extraVertexCount'.
* @dbpro DYN UTIL EXPAND VERTEX BUFFER
* @category UTIL
*/
unsigned int dynUtilExpandVertexBuffer(int id, int extraVertexCount, int extraIndexCount)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!dbObjectExist(id)){
			Message::objectNotExist(id, "dynUtilExpandVertexBuffer");
			return 0;
		}
    #endif
	//-- Get object and some data
	sObject * obj = dbGetObject(id);
	sMesh * pMesh = obj->ppMeshList[0];
	DWORD fvf = pMesh->dwFVF;
	int originalPrims =  pMesh->iDrawPrimitives;
	int originalVertexCount = pMesh->dwVertexCount;
	//int originalIndexCount = pMesh->dwIndexCount;
    //int originalLimbCount = dbLimbCount(id);
	
	//-- Create a new mesh from a new object
	int plainObj = 100;
	while(dbObjectExist(plainObj))
	{
        plainObj++;
	}
	dbMakeObjectPlain(plainObj, 0, 0);
    dbCreateMeshForObject(plainObj, pMesh->dwFVF, extraVertexCount, extraIndexCount);
    //dbConvertObjectFVF( plainObj, pMesh->dwFVF );
	dbLockVertexDataForLimb(plainObj, 0);
	    int vertCount = dbGetVertexDataVertexCount();
		int vertPos = 0;
		while(vertPos < vertCount) 
		{
			 dbSetVertexDataDiffuse(vertPos, dbRGB(255, 255, 255));
		     vertPos++;
		}
	dbUnlockVertexData();
	
	int mesh = 100;
	while(dbMeshExist(mesh))
	{
         mesh++;
	}
	dbMakeMeshFromObject(mesh, plainObj);
	dbDeleteObject(plainObj);
	
	//-- Add new mesh to object
	dbLockVertexDataForLimb(id, 0);
        dbAddMeshToVertexData(mesh);
	dbUnlockVertexData();

	////-- Keep prim count?
 //   pMesh = obj->ppMeshList[0];
	//if(fixPrimitiveCount)
	//{
	//    //int newPrims = pMesh->iDrawPrimitives;
	//    // Never draw more triangles
	//    pMesh->iDrawPrimitives = originalPrims;
	//}

	//-- Return extra vertices count
	return pMesh->dwVertexCount - originalVertexCount;

	//int newIndexCount = pMesh->dwIndexCount;
	//int newVertexCount = pMesh->dwVertexCount;
    //int newLimbCount = dbLimbCount(id);

}

























// SOFT BODY

/*
* @description Cooks and then creates a soft body mesh, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should try to avoid this function if possible and cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param softBodyMeshID - ID number you wish to assign to the mesh.
* @param tetFile - Name of the .tet file you wish to cook the mesh from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK SOFT BODY MESH
* @category CONSTRUCTION
*/
bool dynCookSoftBodyMesh(int softBodyMeshID, char* tetFile){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		string sTetFile(tetFile);
		if(!DBError::fileExists(string(tetFile), "dynCookSoftBodyMesh")){
			return false;
		}
		if(phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
			Message::meshAlreadyExists(softBodyMeshID, "dynCookSoftBodyMesh");
			return false;
		}
	#endif
		
    NxSoftBodyMesh *mesh = 0;   
	mesh = phyGlobStruct_->phy_->cookSoftBodyMesh(tetFile);
	if(mesh){
        phyGlobStruct_->softBodyMeshMap_[softBodyMeshID] = mesh;
		return true;
	}
	else{
        return false;
	}
}



/*
* @description Cooks a soft body mesh to a file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process. 
* @param sourceTetFile - Name you wish to assign to the created file.
* @param tetFile - Name of the .tet file you wish to cook the mesh from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK SOFT BODY MESH
* @category CONSTRUCTION
*/
bool dynCookSoftBodyMesh(char* destinationFile, char* sourceTetFile){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::fileExists(string(sourceTetFile), "dynCookSoftBodyMesh")){
           return false;
		}
	#endif   
	bool result = phyGlobStruct_->phy_->cookSoftBodyMesh(sourceTetFile, destinationFile);
	return result;
}

/*
* @description Loads a pre-cooked soft body mesh from a file.
* @param softBodyMeshID - ID number you wish to assign to the soft body mesh.
* @param file - Name of the .tet file you wish to load the mesh from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN LOAD SOFT BODY MESH
* @category CONSTRUCTION
*/
bool dynLoadSoftBodyMesh(int softBodyMeshID, char* file){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::fileExists(string(file), "dynLoadSoftBodyMesh")){
            return false;
		}
		if(phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
			Message::meshAlreadyExists(softBodyMeshID, "dynLoadSoftBodyMesh");
			return false;
		}
	#endif 
		NxSoftBodyMesh* mesh = 0;
	    mesh = phyGlobStruct_->phy_->loadSoftBodyMesh(file);
		if(mesh){
			phyGlobStruct_->softBodyMeshMap_[softBodyMeshID] = mesh;
		    return true;
		}
		else{
            return false;
		}
}


/*
* @description Creates a soft body actor from a soft body mesh.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param softBodyMeshID - ID number of the mesh you wish to create the actor from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SOFT BODY
* @category CONSTRUCTION
*/
bool dynMakeSoftBody(int objectID, int softBodyMeshID, float density){
    return dynMakeSoftBody(objectID, softBodyMeshID, density, 0);
 //   #ifdef DEBUG
	//	if(!phyGlobStruct_->phy_){
	//		Message::ignitionError();
	//		return false;
	//	}
	//	if(!DBError::objectExists(objectID, "dynMakeSoftBody"))
	//		return false;
	//	if(!phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
	//		Message::meshActorNotExist(softBodyMeshID, "dynMakeSoftBody");
	//		return false;
	//	}
	//	if(phyGlobStruct_->softBodyMap_.count(objectID)){
	//		Message::actorAlreadyExist(objectID, "dynMakeSoftBody");
 //           return false;
	//	}
	//#endif
	//	NxSoftBodyMesh *pMesh = phyGlobStruct_->softBodyMeshMap_[softBodyMeshID];
	//	//Create updater object
	//	ObjectFromSoftBody *aSync = new ObjectFromSoftBody(objectID, NxVec3(1, 1, 1));
	//	NxSoftBody* softBody = 0;
	//	softBody = phyGlobStruct_->phy_->makeSoftBody(aSync, pMesh, DBUtil::getMatrixFromObject(objectID), density);
	//	if(softBody){
	//	    phyGlobStruct_->softBodyMap_[objectID] = softBody;
	//		phyGlobStruct_->softBodySyncList_.push_back(aSync);
	//	    return true;
	//	}
	//	else{
 //           delete aSync;
	//		aSync = 0;
 //           return false;
	//	}
}

/*
* @description Creates a soft body actor from a soft body mesh.
* @param objectID - ID number of the object the actor will represent in the simulation.
* @param softBodyMeshID - ID number of the mesh you wish to create the actor from.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SOFT BODY
* @category CONSTRUCTION
*/
bool dynMakeSoftBody(int objectID, int softBodyMeshID, float density, unsigned int flags){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::objectExists(objectID, "dynMakeSoftBody"))
			return false;
		if(!phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
			Message::meshActorNotExist(softBodyMeshID, "dynMakeSoftBody");
			return false;
		}
		if(phyGlobStruct_->softBodyMap_.count(objectID)){
			Message::actorAlreadyExist(objectID, "dynMakeSoftBody");
            return false;
		}
	#endif
		NxSoftBodyMesh *pMesh = phyGlobStruct_->softBodyMeshMap_[softBodyMeshID];
		//Create updater object
		ObjectFromSoftBody *aSync = new ObjectFromSoftBody(objectID, NxVec3(1, 1, 1));
		NxSoftBody* softBody = 0;
		softBody = phyGlobStruct_->phy_->makeSoftBody(aSync, pMesh, DBUtil::getMatrixFromObject(objectID), density, flags);
		if(softBody){
		    phyGlobStruct_->softBodyMap_[objectID] = softBody;
			phyGlobStruct_->softBodySyncList_.push_back(aSync);
		    return true;
		}
		else{
            delete aSync;
			aSync = 0;
            return false;
		}
}



/*
* @description Sets the stretching stiffness of the given soft body. Volume and stretching stiffnesses are used the specify how strongly the soft body responds to deviations from the rest state given by the input mesh. The soft body simulation manipulates the vertex positions to counteract deformations that change the volume and edge lengths of each tetrahedral element.
* @param softBodyID - ID number of the soft body.
* @param value - Value you wish to set the stretching stiffness, between 0.0 - 1.0, the default value is 0.5.
* @dbpro DYN SOFT BODY SET STRETCHING STIFFNESS
* @category SOFT BODY
*/
void dynSoftBodySetStretchingStiffness(int softBodyID, float value){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetStretchingStiffness");
            return;
		}
	#endif
		phyGlobStruct_->softBodyMap_[softBodyID]->setStretchingStiffness(value);
}

/*
* @description Gets the stretching stiffness of the given soft body. Volume and stretching stiffnesses are used the specify how strongly the soft body responds to deviations from the rest state given by the input mesh. The soft body simulation manipulates the vertex positions to counteract deformations that change the volume and edge lengths of each tetrahedral element.
* @param softBodyID - ID number of the soft body.
* @return value - The current value of the stretching stiffness, the default value is 0.5.
* @dbpro DYN SOFT BODY GET STRETCHING STIFFNESS
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetStretchingStiffness(int softBodyID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetStretchingStiffness");
            return -1;
		}
	#endif
		float value = phyGlobStruct_->softBodyMap_[softBodyID]->getStretchingStiffness();
        RETURNCAST
}

/*
* @description Releases the mesh from memory, its very important to call this when you are done using the mesh, you can only release the mesh if it is not being used by any actors.
* @param softBodyMeshID - ID number of the mesh.
* @return TRUE if succeeded, FALSE otherwise. NOTE: Will return false if mesh is currently in use.
* @dbpro DYN DELETE SOFT BODY MESH
* @category SOFT BODY
*/
bool dynDeleteSoftBodyMesh(int meshID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(phyGlobStruct_->softBodyMeshMap_.count(meshID) == 0){
			Message::meshActorNotExist(meshID, "dynDeleteSoftBodyMesh");
			return false;
		}
    #endif
		bool result = phyGlobStruct_->phy_->releaseSoftBodyMesh(phyGlobStruct_->softBodyMeshMap_[meshID]);
		if(result){
			phyGlobStruct_->softBodyMeshMap_.erase(meshID);
			return true;
		}
		else{
			return false;
		}
}

/*
* @description Releases the soft body, although the mesh that the soft body was created from is not released. 
* @param softBodyID - ID number of the soft body.
* @dbpro DYN DELETE SOFT BODY
* @category SOFT BODY
*/
void dynDeleteSoftBody(int softBodyID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->softBodyMap_.count(softBodyID) == 0){
			Message::actorNotExist(softBodyID, "dynDeleteSoftBody");
			return;
		}
    #endif
	NxSoftBody *sb = phyGlobStruct_->softBodyMap_[softBodyID];
	//Release soft body sync and remove from list
	if(sb->userData != 0){
		SoftBodySync *sbs = (SoftBodySync*)sb->userData;
    	delete sbs;
		phyGlobStruct_->softBodySyncList_.remove(sbs);
	}
	sb->getScene().releaseSoftBody(*sb);
	phyGlobStruct_->softBodyMap_.erase(softBodyID);
}

/*
* @description This will create a new object from a soft body, only it will represent the tetrehedral mesh directly. This newly created object has no texture/normal data etc, it is for debugging purposes only.  
* @param objectID - ID number you wish to assign to the object.
* @param softBodyID - ID number of the soft body which is represented by the terehedral mesh you wish to debug.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SOFT BODY DEBUG
* @category DEBUG
*/
bool dynMakeSoftBodyDebug(int objectID, int softBodyID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		//if(!DBError::objectExists(objectID, "dynMakeSoftBodyDebug"))
		//	return false;
		//if(!phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
		//	Message::meshActorNotExist(softBodyMeshID, "dynMakeSoftBodyDebug");
		//	return false;
		//}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynMakeSoftBodyDebug");
            return false;
		}
	#endif
        NxSoftBody *sb = 0;
		sb = phyGlobStruct_->softBodyMap_[softBodyID];
		NxSoftBodyDesc sbDesc;
		sb->saveToDesc(sbDesc);

		NxSoftBodyMeshDesc sbMeshDesc;
		sbDesc.softBodyMesh->saveToDesc(sbMeshDesc);
		NxU32 tetCount = sbMeshDesc.numTetrahedra;
		NxU32 vertCount = sbMeshDesc.numVertices;
        
		//Each tet will have 4 debug triangles to draw
		//Create here but then update from soft body data

        //Create limbs
		int meshID = 1;
		int faceCount = tetCount * 4; //Should be same at vertCount
		dbMakeObjectTriangle(objectID, 0, 0, 0, 1, 1, 0, 2, 0, 0);
		dbMakeMeshFromObject(meshID, objectID);
		for(int i = 1; i < faceCount; i++){  
    		dbAddLimb(objectID,i, meshID);
		}
		dbDeleteMesh(meshID);

		//Make object without lots of limbs
		dbMakeMeshFromObject(meshID, objectID);

		dbDeleteObject(objectID);
		dbMakeObject(objectID, meshID, 0);
		dbDeleteMesh(meshID);
         
		//DBUtil::setNormals(objectID, 0);
		dbMakeMeshFromObject(meshID, objectID);
		dbDeleteObject(objectID);
		dbMakeObject(objectID, meshID, 0);
		dbDeleteMesh(meshID);
        //dbSetObjectNormals(objectID);
		return false;
}



/*
* @description  This command will set the soft body debug object to represent the current state of the soft body in the simulation, call this in your loop to keep it in sync with the simulation.  
* @param objectID - ID number of the soft body debug object.
* @param softBodyID - ID number of the soft body which is represented by the terehedral mesh you wish to debug.
* @dbpro DYN UPDATE SOFT BODY DEBUG
* @category DEBUG
*/
void dynUpdateSoftBodyDebug(int objectID, int softBodyID){
	    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		//if(!DBError::objectExists(objectID, "dynMakeSoftBodyDebug"))
		//	return false;
		//if(!phyGlobStruct_->softBodyMeshMap_.count(softBodyMeshID)){
		//	Message::meshActorNotExist(softBodyMeshID, "dynMakeSoftBodyDebug");
		//	return false;
		//}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynUpdateSoftBodyDebug");
            return;
		}
	#endif
	NxSoftBody *sb = phyGlobStruct_->softBodyMap_[softBodyID];
	NxMeshData md = sb->getMeshData();
	NxVec3 *vertBuf = (NxVec3*)md.verticesPosBegin;
	NxU32 *indexBuf = (NxU32*)md.indicesBegin;
	NxU32 *numVerts = md.numVerticesPtr;
	NxU32 *numIndices = md.numIndicesPtr;
	//Each 4 verts draws 4 tris
	//tri0 = (p2, p1, p0)
    //tri1 = (p0, p1, p3)
	//tri2 = (p1, p2, p3)
	//tri3 = (p2, p0, p3)

	NxVec3 p0;
	NxVec3 p1;
	NxVec3 p2;
	NxVec3 p3;

	NxU32 i0, i1, i2, i3;


	//Set vertex data
	dbLockVertexDataForLimb(objectID, 0);
	int vCount = dbGetVertexDataVertexCount();
	int iCount = dbGetVertexDataIndexCount();
	for(int i = 0; i < vCount; i = i + 12){
          i0 = *indexBuf++;
		  i1 = *indexBuf++;
		  i2 = *indexBuf++;
		  i3 = *indexBuf++;
          p0 = vertBuf[i0];
		  p1 = vertBuf[i1];
          p2 = vertBuf[i2];
		  p3 = vertBuf[i3];
          //Tri0
		  dbSetVertexDataPosition(i, p2.x, p2.y, p2.z);
		  dbSetVertexDataPosition(i + 1, p1.x, p1.y, p1.z);
		  dbSetVertexDataPosition(i + 2, p0.x, p0.y, p0.z);
          //Tri1
		  dbSetVertexDataPosition(i + 3, p0.x, p0.y, p0.z);
		  dbSetVertexDataPosition(i + 4, p1.x, p1.y, p1.z);
		  dbSetVertexDataPosition(i + 5, p3.x, p3.y, p3.z);
          //Tri2
		  dbSetVertexDataPosition(i + 6, p1.x, p1.y, p1.z);
		  dbSetVertexDataPosition(i + 7, p2.x, p2.y, p2.z);
		  dbSetVertexDataPosition(i + 8, p3.x, p3.y, p3.z);
          //Tri3
		  dbSetVertexDataPosition(i + 9, p2.x, p2.y, p2.z);
		  dbSetVertexDataPosition(i + 10, p0.x, p0.y, p0.z);
		  dbSetVertexDataPosition(i + 11, p3.x, p3.y, p3.z);
	}
	dbUnlockVertexData();

}

/*
* @description  This command will attatch the soft body to a rigid body shape, the rigid body shape must not be a triangle mesh. Each soft body vertex which is currently inside the shape will be attached.  
* @param softBodyID - ID number of the soft body.
* @param objectID - ID number of the object whose associated actor is to be attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to attach, the default shape of an actor usually has an index number of 0.
* @param attachmentFlag - Allows you to determine whether the relationship between soft body and the shape is one way or two way. Set as 0 for one way interaction, the object can pull the soft body, but the soft body cannot move the object. If the object moves then the soft body will follow it. However, if the soft body is pulled, the object will behave as if it is fixed and not move. Set to 1 for two way interaction.
* @dbpro DYN SOFT BODY ATTACH TO SHAPE
* @category SOFT BODY
*/
void dynSoftBodyAttachToShape(int softBodyID, int objectID, int shapeIndex, int attachmentFlag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::objectNotExist(objectID, "dynSoftBodyAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyAttachToShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToShape");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->attachToShape(phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex], attachmentFlag);	
}

/*
* @description  This command will attatch the soft body to a rigid body shape, the rigid body shape must not be a triangle mesh. Each soft body vertex which is currently inside the shape will be attached.  
* @param softBodyID - ID number of the soft body.
* @param objectID - ID number of the limbs owning object.
* @param limbID - ID number of the limb whose associated actor is to be attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to attach, the default shape of an actor usually has an index number of 0.
* @param attachmentFlag - Allows you to determine whether the relationship between soft body and the shape is one way or two way. Set as 0 for one way interaction, the object can pull the soft body, but the soft body cannot move the object. If the object moves then the soft body will follow it. However, if the soft body is pulled, the object will behave as if it is fixed and not move. Set to 1 for two way interaction.
* @dbpro DYN SOFT BODY ATTACH TO SHAPE
* @category SOFT BODY
*/
void dynSoftBodyAttachToShape(int softBodyID, int objectID, int limbID, int shapeIndex, int attachmentFlag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID].count(limbID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyAttachToShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToShape");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->attachToShape(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex], attachmentFlag);	
}

/*
* @description  This command will detach the soft body from a rigid body shape.  
* @param softBodyID - ID number of the soft body.
* @param objectID - ID number of the object whose associated actor is currently attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to detach, the default shape of an actor usually has an index number of 0.
* @dbpro DYN SOFT BODY DETACH FROM SHAPE
* @category SOFT BODY
*/
void dynSoftBodyDetachFromShape(int softBodyID, int objectID, int shapeIndex){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynSoftBodyAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyDetachFromShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyDetachFromShape");
            return;
		}
	#endif 
		phyGlobStruct_->softBodyMap_[softBodyID]->detachFromShape(phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex]);	
}

/*
* @description  This command will detach the soft body from a rigid body shape.  
* @param softBodyID - ID number of the soft body.
* @param objectID - ID number of the limbs owning object. 
* @param limbID - ID number of the limb whose associated actor is currently attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to detach, the default shape of an actor usually has an index number of 0.
* @dbpro DYN SOFT BODY DETACH FROM SHAPE
* @category SOFT BODY
*/
void dynSoftBodyDetachFromShape(int softBodyID, int objectID, int limbID, int shapeIndex){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID].count(limbID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyAttachToShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToShape");
            return;
		}
	#endif 
		phyGlobStruct_->softBodyMap_[softBodyID]->detachFromShape(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex]);	
}

/*
* @description  This command will attatch the soft body to a rigid body shape at the given vertex, the rigid body shape must not be a triangle mesh.  
* @param softBodyID - ID number of the soft body.
* @param vertexIndex - Index number of the soft body vertex you wish to attach to the shape.
* @param objectID - ID number of the object whose associated actor is to be attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to attach, the default shape of an actor usually has an index number of 0.
* @param localX - The position along the x axis local to the given shape.
* @param localY - The position along the y axis local to the given shape.
* @param localZ - The position along the z axis local to the given shape.
* @param attachmentFlag - Allows you to determine whether the relationship between soft body and the shape is one way or two way. Set as 0 for one way interaction, the object can pull the soft body, but the soft body cannot move the object. If the object moves then the soft body will follow it. However, if the soft body is pulled, the object will behave as if it is fixed and not move. Set to 1 for two way interaction.
* @dbpro DYN SOFT BODY ATTACH VERTEX TO SHAPE
* @category SOFT BODY
*/
void dynSoftBodyAttachVertexToShape(int softBodyID, int vertexIndex, int objectID, int shapeIndex, float localX, float localY, float localZ, int attachmentFlag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::objectNotExist(objectID, "dynSoftBodyAttachToShape");
			return;
		}
		//Check vertex index?
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyAttachToShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToShape");
            return;
		}
	#endif  
	phyGlobStruct_->softBodyMap_[softBodyID]->attachVertexToShape(vertexIndex, phyGlobStruct_->actorMap_[objectID]->getShapes()[shapeIndex],
			                                                          NxVec3(localX, localY, localZ),
			                                                          attachmentFlag);	
}

/*
* @description  This command will attatch the soft body to a rigid body shape at the given vertex, the rigid body shape must not be a triangle mesh.  
* @param softBodyID - ID number of the soft body.
* @param vertexIndex - Index number of the soft body vertex you wish to attach to the shape.
* @param objectID - ID number of the limbs owning object. 
* @param limbID - ID number of the limb whose associated actor is to be attached to the soft body.
* @param shapeIndex - Index number of the shape you wish to attach, the default shape of an actor usually has an index number of 0.
* @param localX - The position along the x axis local to the given shape.
* @param localY - The position along the y axis local to the given shape.
* @param localZ - The position along the z axis local to the given shape.
* @param attachmentFlag - Allows you to determine whether the relationship between soft body and the shape is one way or two way. Set as 0 for one way interaction, the object can pull the soft body, but the soft body cannot move the object. If the object moves then the soft body will follow it. However, if the soft body is pulled, the object will behave as if it is fixed and not move. Set to 1 for two way interaction.
* @dbpro DYN SOFT BODY ATTACH VERTEX TO SHAPE
* @category SOFT BODY
*/
void dynSoftBodyAttachVertexToShape(int softBodyID, int vertexIndex, int objectID, int limbID, int shapeIndex, float localX, float localY, float localZ, int attachmentFlag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_[objectID].count(limbID)){
			Message::limbNotExist(objectID, limbID, "dynSoftBodyAttachToShape");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID][limbID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynSoftBodyAttachToShape");
            return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToShape");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->attachVertexToShape(vertexIndex, phyGlobStruct_->actorLimbMap_[objectID][limbID]->getShapes()[shapeIndex],
			                                                          NxVec3(localX, localY, localZ),
			                                                          attachmentFlag);	
}

/*
* @description This command will attatch the soft body to any shape which is currently touching.  
* @param softBodyID - ID number of the soft body.
* @param attachmentFlag - Allows you to determine whether the relationship between soft body and the shape is one way or two way. Set as 0 for one way interaction, the object can pull the soft body, but the soft body cannot move the object. If the object moves then the soft body will follow it. However, if the soft body is pulled, the object will behave as if it is fixed and not move. Set to 1 for two way interaction.
* @dbpro DYN SOFT BODY ATTACH TO COLLIDING SHAPES
* @category SOFT BODY
*/
void dynSoftBodyAttachToCollidingShapes(int softBodyID, int attachmentFlag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAttachToCollidingShapes");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->attachToCollidingShapes(attachmentFlag);
}

/*
* @description This command will set the stiffness of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Soft body volume stiffness in the range from 0 to 1.
* @dbpro DYN SOFT BODY SET VOLUME STIFFNESS
* @category SOFT BODY
*/
void dynSoftBodySetVolumeStiffness(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetVolumeStiffness");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setVolumeStiffness(value);
}

/*
* @description This command will set the particle radius of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Particle radius, must be positive.
* @dbpro DYN SOFT BODY SET PARTICLE RADIUS
* @category SOFT BODY
*/
void dynSoftBodySetParticleRadius(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetParticleRadius");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setParticleRadius(value);
}

/*
* @description This command will set the hard stretch elongation limit of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Factor.
* @dbpro DYN SOFT BODY SET HARD STRETCH LIMITATION FACTOR
* @category SOFT BODY
*/
void dynSoftBodySetHardStretchLimitationFactor(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetHardStretchLimitationFactor");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setHardStretchLimitationFactor(value);
}

/*
* @description This command will set the damping coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Damping coefficient.
* @dbpro DYN SOFT BODY SET DAMPING COEFFICIENT
* @category SOFT BODY
*/
void dynSoftBodySetDampingCoefficient(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetDampingCoefficient");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setDampingCoefficient(value);
}

/*
* @description This command will set the friction of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Friction.
* @dbpro DYN SOFT BODY SET FRICTION
* @category SOFT BODY
*/
void dynSoftBodySetFriction(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetFriction");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setFriction(value);
}

/*
* @description This command will set the attachment tear factor of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - factor.
* @dbpro DYN SOFT BODY SET ATTACHMENT TEAR FACTOR
* @category SOFT BODY
*/
void dynSoftBodySetAttachmentTearFactor(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetAttachmentTearFactor");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setAttachmentTearFactor(value);
}

/*
* @description This command will set the self collision thickness of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Self collsision thickness, must be positive.
* @dbpro DYN SOFT BODY SET SELF COLLISION THICKNESS
* @category SOFT BODY
*/
void dynSoftBodySetSelfCollisionThickness(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetSelfCollisionThickness");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setSelfCollisionThickness(value);
}

/*
* @description This command will set the collision response coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Coefficient.
* @dbpro DYN SOFT BODY SET COLLISION RESPONSE COEFFICIENT
* @category SOFT BODY
*/
void dynSoftBodySetCollisionResponseCoefficient(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetCollisionResponseCoefficient");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setCollisionResponseCoefficient(value);
}

/*
* @description This command will set the Attachment response coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @param value - Coefficient.
* @dbpro DYN SOFT BODY SET ATTACHMENT RESPONSE COEFFICIENT
* @category SOFT BODY
*/
void dynSoftBodySetAttachmentResponseCoefficient(int softBodyID, float value)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetAttachmentResponseCoefficient");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setAttachmentResponseCoefficient(value);
}







//-- GETS

/*
* @description This command will get the stiffness of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Soft body volume stiffness in the range from 0 to 1.
* @dbpro DYN SOFT BODY GET VOLUME STIFFNESS
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetVolumeStiffness(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetVolumeStiffness");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getVolumeStiffness();
        RETURNCAST
}

/*
* @description This command will get the particle radius of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Particle radius, must be positive.
* @dbpro DYN SOFT BODY GET PARTICLE RADIUS
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetParticleRadius(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetParticleRadius");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getParticleRadius();
        RETURNCAST
}

/*
* @description This command will get the hard stretch elongation limit of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Factor.
* @dbpro DYN SOFT BODY GET HARD STRETCH LIMITATION FACTOR
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetHardStretchLimitationFactor(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetHardStretchLimitationFactor");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getHardStretchLimitationFactor();
        RETURNCAST
}

/*
* @description This command will get the damping coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Damping coefficient.
* @dbpro DYN SOFT BODY GET DAMPING COEFFICIENT
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetDampingCoefficient(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetDampingCoefficient");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getDampingCoefficient();
        RETURNCAST
}

/*
* @description This command will get the friction of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Friction.
* @dbpro DYN SOFT BODY GET FRICTION
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetFriction(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetFriction");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getFriction();
        RETURNCAST
}

/*
* @description This command will get the attachment tear factor of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - factor.
* @dbpro DYN SOFT BODY GET ATTACHMENT TEAR FACTOR
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetAttachmentTearFactor(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetAttachmentTearFactor");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getAttachmentTearFactor();
        RETURNCAST
}

/*
* @description This command will get the self collision thickness of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Self collsision thickness, must be positive.
* @dbpro DYN SOFT BODY GET SELF COLLISION THICKNESS
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetSelfCollisionThickness(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetSelfCollisionThickness");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getSelfCollisionThickness();
        RETURNCAST
}

/*
* @description This command will get the collision response coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Coefficient.
* @dbpro DYN SOFT BODY GET COLLISION RESPONSE COEFFICIENT
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetCollisionResponseCoefficient(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetCollisionResponseCoefficient");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getCollisionResponseCoefficient();
        RETURNCAST
}

/*
* @description This command will get the Attachment response coefficient of the given soft body.  
* @param softBodyID - ID number of the soft body.
* @return - Coefficient.
* @dbpro DYN SOFT BODY GET ATTACHMENT RESPONSE COEFFICIENT
* @category SOFT BODY
*/
DWORDFLOAT dynSoftBodyGetAttachmentResponseCoefficient(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetAttachmentResponseCoefficient");
            return 0;
		}
	#endif  
		DWORD value = phyGlobStruct_->softBodyMap_[softBodyID]->getAttachmentResponseCoefficient();
        RETURNCAST
}



/*
* @description Frees a previously attached soft body vertex.  
* @param softBodyID - ID number of the soft body.
* @param vertexID - ID of the vertex you wish to free.
* @dbpro DYN SOFT BODY FREE VERTEX
* @category SOFT BODY
*/
void dynSoftBodyFreeVertex(int softBodyID, int vertexID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyFreeVertex");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->freeVertex(vertexID);
}

/*
* @description Sets which collision group this soft body is part of. 
* @param softBodyID - ID number of the soft body.
* @param group - Collision group, default is 0.
* @dbpro DYN SOFT BODY SET GROUP
* @category SOFT BODY
*/
void dynSoftBodySetGroup(int softBodyID, int group)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetGroup");
            return;
		}
	#endif  
		phyGlobStruct_->softBodyMap_[softBodyID]->setGroup((NxCollisionGroup)group);
}

/*
* @description Gets which collision group this soft body is part of. 
* @param softBodyID - ID number of the soft body.
* @return Collision group, default is 0.
* @dbpro DYN SOFT BODY GET GROUP
* @category SOFT BODY
*/
int dynSoftBodySetGroup(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetGroup");
            return 0;
		}
	#endif  
		return (int)phyGlobStruct_->softBodyMap_[softBodyID]->getGroup();
}

/*
* @description Sets an external acceleration which affects all non attached particles of the soft body. 
* @param softBodyID - ID number of the soft body.
* @param accelerationVec3ID - ID number of the vector containing the acceleration values along each axis.
* @dbpro DYN SOFT BODY SET EXTERNAL ACCELERATION
* @category SOFT BODY
*/
void dynSoftBodySetExternalAcceleration(int softBodyID, int accelerationVec3ID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetExternalAcceleration");
            return;
		}
	#endif      

    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(accelerationVec3ID), dbYVector3(accelerationVec3ID), dbZVector3(accelerationVec3ID));
    #else
        DWORD X = dbXVector3(accelerationVec3ID);
        DWORD Y = dbYVector3(accelerationVec3ID);
        DWORD Z = dbZVector3(accelerationVec3ID);
	    NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z); 
    #endif
	    phyGlobStruct_->softBodyMap_[softBodyID]->setExternalAcceleration(vec);	
}

/*
* @description Gets the external acceleration which affects all non attached particles of the soft body. 
* @param softBodyID - ID number of the soft body.
* @param accelerationVec3ID - ID number of the vector you wish to store the acceleration values along each axis.
* @dbpro DYN SOFT BODY GET EXTERNAL ACCELERATION
* @category SOFT BODY
*/
void dynSoftBodyGetExternalAcceleration(int softBodyID, int accelerationVec3ID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetExternalAcceleration");
            return;
		}
	#endif      
	    NxVec3 vec = phyGlobStruct_->softBodyMap_[softBodyID]->getExternalAcceleration();	
        dbSetVector3(accelerationVec3ID, vec.x, vec.y, vec.z);
}

/*
* @description Allows you to check if the soft body is currently asleep. 
* @param softBodyID - ID number of the soft body.
* @return TRUE if asleep, FALSE otherwise.
* @dbpro DYN SOFT BODY IS SLEEPING
* @category SOFT BODY
*/
bool dynSoftBodyIsSleeping(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyIsSleeping");
            return 0;
		}
	#endif      
	return phyGlobStruct_->softBodyMap_[softBodyID]->isSleeping();
}

/*
* @description Wakes the soft body up for a given amount of time. 
* @param softBodyID - ID number of the soft body.
* @param wakeCounterValue - Amount of time before the soft body falls asleep.
* @dbpro DYN SOFT BODY WAKE UP
* @category SOFT BODY
*/
void dynSoftBodyWakeUp(int softBodyID, float wakeCounterValue)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyWakeUp");
            return;
		}
	#endif      
		phyGlobStruct_->softBodyMap_[softBodyID]->wakeUp(wakeCounterValue);
}

/*
* @description Puts the soft body to sleep. 
* @param softBodyID - ID number of the soft body.
* @dbpro DYN SOFT BODY PUT TO SLEEP
* @category SOFT BODY
*/
void dynSoftBodyPutToSleep(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyPutToSleep");
            return;
		}
	#endif      
		phyGlobStruct_->softBodyMap_[softBodyID]->putToSleep();
}

/*
* @description Sets the flags for the given soft body. 
* @param softBodyID - ID number of the soft body.
* @param flags - Combination of flags which determine certain properties of the soft body eg NX_SBF_COLLISION_TWOWAY = 256.
* @dbpro DYN SOFT BODY SET FLAGS
* @category SOFT BODY
*/
void dynSoftBodySetFlags(int softBodyID, unsigned int flags)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodySetFlags");
            return;
		}
	#endif   
		phyGlobStruct_->softBodyMap_[softBodyID]->setFlags(flags);
}

/*
* @description Gets the flags for the given soft body. 
* @param softBodyID - ID number of the soft body.
* @return Combination of flags which determine certain properties of the soft body eg NX_SBF_COLLISION_TWOWAY = 256.
* @dbpro DYN SOFT BODY GET FLAGS
* @category SOFT BODY
*/
unsigned int dynSoftBodyGetFlags(int softBodyID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyGetFlags");
            return 0;
		}
	#endif   
		return phyGlobStruct_->softBodyMap_[softBodyID]->getFlags();
}

/*
* @description Casts a ray against the given soft body. You can retrieve the hit position and the closest vertexID using the other cloth raycast commands, be sure to only do so if this command returns true otherwise the data will be from the last ray that hit. 
* @param softBodyID - ID number of the object the soft body represents in the simulation.
* @param origVector3ID - Origin of the ray in world space. 
* @param dirVector3ID - Direction the ray should travel.
* @return TRUE if ray hits, FALSE otherwise.
* @dbpro DYN SOFT BODY RAYCAST
* @category SOFT BODY
*/
bool dynSoftBodyRaycast(int softBodyID, int origVec3ID, int dirVec3ID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyRaycast");
            return 0;
		}
	#endif   
    NxSoftBody * sb = phyGlobStruct_->softBodyMap_[softBodyID];
    NxRay ray;
    #ifdef COMPILE_FOR_GDK
		ray.dir = NxVec3(dbXVector3(dirVec3ID), dbYVector3(dirVec3ID), dbZVector3(dirVec3ID));
		ray.orig = NxVec3(dbXVector3(origVec3ID), dbYVector3(origVec3ID), dbZVector3(origVec3ID));
    #else
        DWORD tempDirX = dbXVector3(dirVec3ID);
        DWORD tempDirY = dbYVector3(dirVec3ID);
        DWORD tempDirZ = dbZVector3(dirVec3ID);
        DWORD tempOrigX = dbXVector3(origVec3ID);
        DWORD tempOrigY = dbYVector3(origVec3ID);
        DWORD tempOrigZ = dbZVector3(origVec3ID);
		ray.dir = NxVec3(*(float*)&tempDirX, *(float*)&tempDirY, *(float*)&tempDirZ);
		ray.orig = NxVec3(*(float*)&tempOrigX, *(float*)&tempOrigY, *(float*)&tempOrigZ);
    #endif
	return sb->raycast(ray, phyGlobStruct_->softBodyRayHit_, phyGlobStruct_->softBodyRayVertexID_);  	
}


/*
* @description When a ray has been cast against a soft body and it has hit the soft body then the world position of the hit can be retrieved using this command. 
* @param posVector3ID - ID number of the vector you wish the data to be stored in.
* @dbpro DYN SOFT BODY RAYCAST GET HIT POS
* @category SOFT BODY
*/
void dynSoftBodyRaycastGetHitPos(int posVector3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
    #endif
	dbSetVector3(posVector3ID, phyGlobStruct_->softBodyRayHit_.x, phyGlobStruct_->softBodyRayHit_.y, phyGlobStruct_->softBodyRayHit_.z);
}

/*
* @description When a ray has been cast against a soft body and it has hit the soft body then the closest vertexID can be retrieved using this command. 
* @return ID number of the closest vertex.
* @dbpro DYN SOFT BODY RAYCAST GET VERTEX ID
* @category SOFT BODY
*/
int dynSoftBodyRaycastGetVertexID()
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
    #endif
	return (int)phyGlobStruct_->softBodyRayVertexID_;
}

/*
* @description This command will apply the given force at the given soft body vertex. 
* @param softBodyID - ID number of the object the soft body represents in the simulation.
* @param vertexID - ID number of the vertex you wish to apply the force to.
* @param forceVec3ID - ID number of the vector containing the force to be added.
* @dbpro DYN SOFT BODY ADD FORCE AT VERTEX
* @category SOFT BODY
*/
void dynSoftBodyAddForceAtVertex(int softBodyID, int vertexID, int forceVec3ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAddForceAtVertex");
            return;
		}
    #endif
    NxSoftBody * sb = phyGlobStruct_->softBodyMap_[softBodyID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(forceVec3ID), dbYVector3(forceVec3ID), dbZVector3(forceVec3ID));
		sb->addForceAtVertex(vec, vertexID);
    #else
        DWORD X = dbXVector3(forceVec3ID);
        DWORD Y = dbYVector3(forceVec3ID);
        DWORD Z = dbZVector3(forceVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		sb->addForceAtVertex(vec, vertexID);
    #endif
}


/*
* @description Applies a radial force (or impulse) at a particular position. All vertices within radius will be affected with a quadratic drop-off. Because forces are reset at the end of every timestep, you can maintain a total external force on an object by calling this once every frame.
* @param softBodyID - ID number of the object the soft body represents in the simulation.
* @param posVec3ID - ID number of the vector containing the position.
* @param magnitude - Magnitude of the force/impulse to apply.
* @param radius - The sphere radius in which particles will be affected.
* @dbpro DYN SOFT BODY ADD FORCE AT POS
* @category SOFT BODY
*/
void dynSoftBodyAddForceAtPos(int softBodyID, int posVec3ID, float magnitude, float radius)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAddForceAtPos");
            return;
		}
    #endif
    NxSoftBody * sb = phyGlobStruct_->softBodyMap_[softBodyID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(posVec3ID), dbYVector3(posVec3ID), dbZVector3(posVec3ID));
		sb->addForceAtPos(vec, magnitude, radius);
    #else
        DWORD X = dbXVector3(posVec3ID);
        DWORD Y = dbYVector3(posVec3ID);
        DWORD Z = dbZVector3(posVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		sb->addForceAtPos(vec, magnitude, radius);
    #endif
}

/*
* @description Applies a directed force (or impulse) at a particular position. All vertices within radius will be affected with a quadratic drop-off. Because forces are reset at the end of every timestep, you can maintain a total external force on an object by calling this once every frame.
* @param softBodyID - ID number of the object the soft body represents in the simulation.
* @param posVec3ID - ID number of the vector containing the position.
* @param forceVec3ID - ID number of the vector containing the force.
* @param radius - The sphere radius in which particles will be affected.
* @dbpro DYN SOFT BODY ADD DIRECTED FORCE AT POS
* @category SOFT BODY
*/
void dynSoftBodyAddDirectedForceAtPos(int softBodyID, int posVec3ID, int forceVec3ID, float radius)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->softBodyMap_.count(softBodyID)){
			Message::actorNotExist(softBodyID, "dynSoftBodyAddDirectedForceAtPos");
            return;
		}
    #endif
    NxSoftBody * sb = phyGlobStruct_->softBodyMap_[softBodyID];
    #ifdef COMPILE_FOR_GDK
        NxVec3 vec(dbXVector3(posVec3ID), dbYVector3(posVec3ID), dbZVector3(posVec3ID));
        NxVec3 vec2(dbXVector3(forceVec3ID), dbYVector3(forceVec3ID), dbZVector3(forceVec3ID));
		sb->addDirectedForceAtPos(vec, vec2, radius);
    #else
        DWORD X = dbXVector3(posVec3ID);
        DWORD Y = dbYVector3(posVec3ID);
        DWORD Z = dbZVector3(posVec3ID);
        DWORD X2 = dbXVector3(forceVec3ID);
        DWORD Y2 = dbYVector3(forceVec3ID);
        DWORD Z2 = dbZVector3(forceVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		NxVec3 vec2(*(float*)&X2, *(float*)&Y2, *(float*)&Z2);
		sb->addDirectedForceAtPos(vec, vec2, radius);
    #endif
}



/*
* @description Checks whether a soft body mesh exists. 
* @param softBodyMeshID - ID number of the soft body mesh.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN SOFT BODY MESH EXISTS
* @category CONSTRUCTION
*/
bool dynSoftBodyMeshExists(int meshID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
    #endif	
	return phyGlobStruct_->softBodyMeshMap_.count(meshID);
}

/*
* @description Checks whether a soft body exists. 
* @param softBodyID - ID number of the soft body.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN SOFT BODY EXISTS
* @category CONSTRUCTION
*/
bool dynSoftBodyExists(int ID)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
    #endif	
	return phyGlobStruct_->softBodyMap_.count(ID);
}






// ***NOTE: this is how you remove shape from renderable*** //

//NxShape* terrain;
//
//// Get the flags of the shape and remove the visualization flag
//NxShapeFlag flags = terrain.getFlags();
//flags &= ~NxShapeFlag::NX_SF_VISUALIZATION;
//
//terrain->setFlag(flags);
//
//To enable vis again; use the same code, except OR the visualization flag in:
//flags |= NxShapeFlag::NX_SF_VISUALIZATION; 


//TESTING
//NOTE:  Perhaps user should pass in all particles(objects or limbs),
//       physics will just update positions and nothing else, perhaps put
//       in option to fade/scale particle.
//NOTE:  There should be two ways to render(type), the first when each particle is identicle
//       (fastest), the second keeps one to one relation with rendered object.
//IDEAS: dynMakeParticlesFromObjects(int particleGroupID, int firstObjectID, int lastObjectID);
//       dynMakeParticlesFromLimbs(int particleGroupID, int boundingObjectID, int firstLimbID, int lastLimb);
//       dynMakeFluid(int fluidID, int particleGroup, int type);
//       dynAddEmitter(int emitterID, int fluidID, float rate, float life);

/*
* @description Creates particles which can then be used to simulate things like smoke, water or weather effects.  
* @param fluidID - ID number you wish to assign to this fluid. 
* @param maxParticles - Maximum amount of particles the fluid.
* @param particleLife - Life of the particles, in seconds.
* @param collisionRadiusMultiplier - The collision radius of the particles.
* @param flags - Allows you to set properties for the fluid such as gravity, two way collision, hardware/software etc.
* @param usePositionData - If you set this to TRUE the particles will be initially positioned using position data held in the dynUtilArrayVec3 structure which you must fill first, set to FALSE if you wish to use emitters. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FLUID
* @category CONSTRUCTION
*/
bool dynMakeFluid(int fluidID, unsigned int maxParticles, float particleLife, float collisionRadiusMultiplier, unsigned int flags, bool usePositionData)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorAlreadyExist(fluidID, "dynMakeFluid");
			return 0;
		}
	#endif
	NxFluid * fluid = 0;
	if(usePositionData)
	{
		fluid = phyGlobStruct_->phy_->makeFluid(maxParticles, particleLife, collisionRadiusMultiplier, flags, phyGlobStruct_->vec3Array_, phyGlobStruct_->vec3ArraySize_);
	}
	else
	{
	    fluid = phyGlobStruct_->phy_->makeFluid(maxParticles, particleLife, collisionRadiusMultiplier, flags);
	}
	if(fluid){
		phyGlobStruct_->fluidMap_[fluidID] = fluid;
	    return true;
	}
	return false;
}

/*
* @description Released the given fluid from the simulation.  
* @param fluidID - ID number of the fluid you wish to release. 
* @dbpro DYN DELETE FLUID
* @category CONSTRUCTION
*/
void dynDeleteFluid(int fluidID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynDeleteFluid");
			return;
		}
	#endif
    NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	phyGlobStruct_->phy_->releaseFluid(fluid);
	phyGlobStruct_->fluidMap_.erase(fluidID);

}

/*
* @description This command will point all your fluid objects toward the given camera, if you are using multiple cameras you would need to sync the first camera and then call this function again to point toward the second camera before syncing the second camera.  
* @param fluidID - ID number of the fluid. 
* @param initialObjectID - First object which was assigned to this fluid.
* @param camID - ID number of the camera you wish to point the objects toward.
* @dbpro DYN FLUID SET OBJECTS TO CAMERA
* @category FLUID/PARTICLES
*/
void dynFluidSetObjectsToCamera(int fluidID, int initialObjectID, int camID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetObjectsToCamera");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	if(!fluid->getFlag(NX_FF_ENABLED)){
         return;
	}
	//NxF32 *position = fluid->getParticlesWriteData().bufferPos;
	NxU32 *id = fluid->getParticlesWriteData().bufferId;
	NxU32 *count = fluid->getParticlesWriteData().numParticlesPtr;
	//NxF32 *life = fluid->getParticlesWriteData().bufferLife;

#ifdef COMPILE_FOR_GDK
		float camX = dbCameraPositionX(camID);
		float camY = dbCameraPositionY(camID);
		float camZ = dbCameraPositionZ(camID);
#else
        DWORD X = dbCameraPositionX(camID);
        DWORD Y = dbCameraPositionY(camID);
        DWORD Z = dbCameraPositionZ(camID);
		float camX = *(float*)&X;
		float camY = *(float*)&Y;
		float camZ = *(float*)&Z;
#endif

	for(int i = initialObjectID; i < initialObjectID + *count; i++)
	{
		NxU32 currentID = *(id++) + initialObjectID;
		dbPointObject(currentID, camX, camY, camZ);		
	}
}

/*
* @description This command will fade all your fluid objects, you must call this function in your game loop.  
* @param fluidID - ID number of the fluid. 
* @param initialObjectID - First object which was assigned to this fluid.
* @param particleLife - Particle life you assigned to this fluid.
* @param fadeIn - Amount of time in seconds it should take for the particles to fade in.
* @param fadeOut - Amount of time in seconds it should take for the particles to fade out.
* @dbpro DYN FLUID FADE OBJECTS
* @category FLUID/PARTICLES
*/
void dynFluidFadeObjects(int fluidID, int initialObjectID, float particleLife, float fadeIn, float fadeOut)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidFadeObjects");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	if(!fluid->getFlag(NX_FF_ENABLED)){
         return;
	}
	NxU32 *id = fluid->getParticlesWriteData().bufferId;
	NxU32 *count = fluid->getParticlesWriteData().numParticlesPtr;
	NxF32 *life = fluid->getParticlesWriteData().bufferLife;
    if(life)
	{
		for(int i = initialObjectID; i < initialObjectID + *count; i++)
		{
            NxU32 currentID = *(id++) + initialObjectID;
			float currentLife = *(life++);
			if(fadeOut){
				if(currentLife < fadeOut)
				{
					float alpha = currentLife / fadeOut * 100;
					dbFadeObject(currentID, alpha);
				}
			}
			if(fadeIn){
				float endFade = particleLife - fadeIn;
				if(currentLife > endFade)
				{
					float alpha = 100 - (((currentLife - endFade) / fadeIn) * 100);
					//float alpha = currentLife / fadeInFinish * 100;
					dbFadeObject(currentID, alpha);
				}
			}
		}
	}
}


/*
* @description This command will fade all your fluid objects, you must call this function in your game loop.  
* @param fluidID - ID number of the fluid. 
* @param initialObjectID - First object which was assigned to this fluid.
* @param particleLife - Particle life you assigned to this fluid.
* @param fadeIn - Amount of time in seconds it should take for the particles to fade in.
* @param fadeOut - Amount of time in seconds it should take for the particles to fade out.
* @dbpro DYN FLUID FADE OBJECTS ALPHA
* @category FLUID/PARTICLES
*/
void dynFluidFadeObjectsAlpha(int fluidID, int initialObjectID, float particleLife, float fadeIn, float fadeOut)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidFadeObjectsAlpha");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	if(!fluid->getFlag(NX_FF_ENABLED)){
         return;
	}
	NxU32 *id = fluid->getParticlesWriteData().bufferId;
	NxU32 *count = fluid->getParticlesWriteData().numParticlesPtr;
	NxF32 *life = fluid->getParticlesWriteData().bufferLife;
    if(life)
	{
		for(int i = initialObjectID; i < initialObjectID + *count; i++)
		{
            NxU32 currentID = *(id++) + initialObjectID;
			float currentLife = *(life++);
			if(fadeOut){
				if(currentLife < fadeOut)
				{
					float alpha = currentLife / fadeOut * 100;
					//if(alpha < 0.1) alpha = 0.1;
					dbSetAlphaMappingOn(currentID, alpha);
					//dbFadeObject(currentID, alpha);
				}
			}
			if(fadeIn){
				float endFade = particleLife - fadeIn;
				if(currentLife > endFade)
				{
					float alpha = 100 - (((currentLife - endFade) / fadeIn) * 100);
					//float alpha = currentLife / fadeInFinish * 100;
					//dbFadeObject(currentID, alpha);
				    //if(alpha > 99) alpha = 99;
					dbSetAlphaMappingOn(currentID, alpha);
				}
			}
		}
	}
}







/*
* @description This command will scale all your fluid objects, you must call this function in your game loop.  
* @param fluidID - ID number of the fluid. 
* @param initialObjectID - First object which was assigned to this fluid.
* @param particleLife - Particle life you assigned to this fluid.
* @param scaleStart - Scale the paricles will be when created. A value of 100 will mean the objects will not be scaled upon creation.
* @param scaleFinish - Scale the particles will be when their life expires, A value of 100 will mean the objects will not be scaled as their life expires.
* @dbpro DYN FLUID SCALE OBJECTS
* @category FLUID/PARTICLES
*/
void dynFluidScaleObjects(int fluidID, int initialObjectID, float particleLife, float scaleStart, float scaleFinish)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidScaleObjects");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	if(!fluid->getFlag(NX_FF_ENABLED)){
         return;
	}
	NxU32 *id = fluid->getParticlesWriteData().bufferId;
	NxU32 *count = fluid->getParticlesWriteData().numParticlesPtr;
	NxF32 *life = fluid->getParticlesWriteData().bufferLife;
    if(life)
	{
		for(int i = initialObjectID; i < initialObjectID + *count; i++)
		{
            NxU32 currentID = *(id++) + initialObjectID;
			float currentLife = *(life++);
			float diff = scaleFinish - scaleStart;
			float scale = scaleStart + (((particleLife - currentLife) / particleLife) * diff);
            dbScaleObject(currentID, scale, scale, scale);
		}
	}
}



/*
* @description This command will position all your fluid object to the given fluid, it will also show/hide your objects as the particles are created/destroyed.  
* @param fluidID - ID number of the fluid. 
* @param initialObjectID - First object which was assigned to this fluid.
* @dbpro DYN FLUID SYNC OBJECTS
* @category FLUID/PARTICLES
*/
void dynFluidSyncObjects(int fluidID, int initialObjectID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSyncObjects");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	if(!fluid->getFlag(NX_FF_ENABLED)){
         return;
	}
	NxU32 *deletedCount = fluid->getParticleDeletionIdWriteData().numIdsPtr;
	//changed
	if(*deletedCount > 0){
		NxU32 *deletedPtr = fluid->getParticleDeletionIdWriteData().bufferId;
		for(int i = 0; i < *deletedCount; i++){
            unsigned int objID = *(deletedPtr++) + initialObjectID;
            dbHideObject(objID);
		}
	}

	NxU32 *createdCount = fluid->getParticleCreationIdWriteData().numIdsPtr;
	if(*createdCount > 0){
		NxU32 *createdPtr = fluid->getParticleCreationIdWriteData().bufferId;
		for(int i = 0; i < *createdCount; i++){
            unsigned int objID = *(createdPtr++) + initialObjectID;
			dbFadeObject(objID, 100.0f);
			dbShowObject(objID);
		}
	}

	NxF32 *position = fluid->getParticlesWriteData().bufferPos;
	NxU32 *id = fluid->getParticlesWriteData().bufferId;
	NxU32 *count = fluid->getParticlesWriteData().numParticlesPtr;
	for(int i = initialObjectID; i < initialObjectID + *count; i++)
	{
		float x = *(position++);
		float y = *(position++);
		float z = *(position++);
		NxU32 currentID = *(id++) + initialObjectID;
		dbPositionObject(currentID, x, y, z);
	}
}


/*
* @description This command will return the collision flag for the fluid.  
* @param fluidID - ID number of the fluid. 
* @return Collision flag, 0 = none, 1 = static, 2 = dynamic, 3 = static and dynamic. 
* @dbpro DYN FLUID GET COLLISION METHOD
* @category FLUID/PARTICLES
*/
int dynFluidGetCollisionMethod(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetCollisionMethod");
			return - 1;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];  
	return fluid->getCollisionMethod();
}

/*
* @description This command will set the collision flag for the fluid.  
* @param fluidID - ID number of the fluid. 
* @param flag - Collision flag, 0 = none, 1 = static, 2 = dynamic, 3 = static and dynamic. 
* @dbpro DYN FLUID SET COLLISION METHOD
* @category FLUID/PARTICLES
*/
void dynFluidSetCollisionMethod(int fluidID, int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetCollisionMethod");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];  
	fluid->setCollisionMethod(flag);
}

/*
* @description This command allows you to randomize the angle the particles leave the emitter, the default is for the particles to leave the emitter at the exact angle the emitter is facing, which is not always desirable. 
* @param fluidID - ID number of the fluid. 
* @param emitterIndex - Index of the emitter for the given fluid.
* @param angle - Value in radians. 
* @dbpro DYN FLUID SET RANDOM ANGLE
* @category FLUID/PARTICLES
*/
void dynEmitterSetRandomAngle(int fluidID, int emitterIndex, float angle){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterSetRandomAngle");
			return;
		}
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterSetRandomAngle");
			return;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	fluid->getEmitters()[emitterIndex]->setRandomAngle(angle);
}

/*
* @description This command returns the maximum angle the particles leave the emitter, the default is for the particles to leave the emitter at the exact angle the emitter is facing, which is not always desirable. 
* @param fluidID - ID number of the fluid. 
* @param emitterIndex - Index of the emitter for the given fluid.
* @return - Angle value in radians. 
* @dbpro DYN FLUID GET RANDOM ANGLE
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynEmitterGetRandomAngle(int fluidID, int emitterIndex){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterGetRandomAngle");
			return 0;
		}
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterGetRandomAngle");
			return 0;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	float value = fluid->getEmitters()[emitterIndex]->getRandomAngle();
	RETURNCAST
}


/*
* @description Creates an emitter for the given fluid, emitters can share a single fluid although a fluid only has a maximum amount of particles available. An emitter becomes responsible for creating the particles, they are only destroyed if their life runs out or they come into contact with a drain. The emitter can only create particles as long as the fluid has particles available.  
* @param sizeX - Size of the emitter along its local x axis.
* @param sizeY - Size of the emitter along its local y axis.
* @param particleLife - Life of the particles.
* @param type - The emitter can either create particles at a given rate or maintain a given pressure. Constant flow rate = 2, Pressure = 1
* @param posVec3ID - ID number of the vector3 containing positional data.
* @param rotVec3ID - ID number of the vector3 containing rotational data.
* @return Number of emitters belonging to the given fluid.
* @dbpro DYN FLUID MAKE EMITTER
* @category FLUID/PARTICLES
*/
int dynFluidMakeEmitter(int fluidID, float sizeX, float sizeY, float particleLife, int type, int posVec3ID, int rotVec3ID)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidMakeEmitter");
			return 0;
		}
	#endif
    NxFluid *fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitterDesc desc;
	desc.dimensionX = sizeX;
	desc.dimensionY = sizeY;
	desc.particleLifetime = particleLife;
	desc.type = (NxEmitterType)type;
	desc.rate = 10;
	desc.fluidVelocityMagnitude = 50.0f;
	D3DXMATRIX mat;

#ifdef COMPILE_FOR_GDK
	MathUtil::setMatrix(mat, dbXVector3(posVec3ID), dbYVector3(posVec3ID), dbZVector3(posVec3ID),
                             dbXVector3(rotVec3ID), dbYVector3(rotVec3ID), dbZVector3(rotVec3ID));
#else
        DWORD X = dbXVector3(posVec3ID);
        DWORD Y = dbYVector3(posVec3ID);
        DWORD Z = dbZVector3(posVec3ID);
        DWORD rotX = dbXVector3(rotVec3ID);
        DWORD rotY = dbYVector3(rotVec3ID);
        DWORD rotZ = dbZVector3(rotVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
		NxVec3 rotvec(*(float*)&rotX, *(float*)&rotY, *(float*)&rotZ);
	   	MathUtil::setMatrix(mat, vec.x, vec.y, vec.z,
                             rotvec.x, rotvec.y, rotvec.z);
#endif
	NxMat34 phyMat;
	phyMat.setColumnMajor44(mat.m);
	desc.relPose = phyMat;
	fluid->createEmitter(desc);
	return fluid->getNbEmitters() - 1;
}

/*
* @description This command will glue the emitter to a given shape of a given actor. The world position of the emitter will become the local position relative to the shape it has been attached to. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @param objectID - ID number of the object whose actor you wish to glue the emitter to.
* @param shapeIndex - Index of the shape inside the given actor, default shape is 0.
* @dbpro DYN EMITTER SET FRAME SHAPE
* @category FLUID/PARTICLES
*/
void dynEmitterSetFrameShape(int fluidID, int emitterIndex, int objectID, int shapeIndex)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterSetFrameShape");
			return;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterSetFrameShape");
			return;
		}
		if(!phyGlobStruct_->actorMap_.count(objectID)){
			Message::actorNotExist(objectID, "dynEmitterSetFrameShape");
			return;
		}
		if(phyGlobStruct_->actorMap_[objectID]->getNbShapes() <= (unsigned int)shapeIndex){
			Message::errorBox("Shape index out of bounds", "dynEmitterSetFrameShape");
            return;
		}
	#endif
    NxActor* actor = phyGlobStruct_->actorMap_[objectID];
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	fluid->getEmitters()[emitterIndex]->setFrameShape(actor->getShapes()[shapeIndex]);
}



/*
* @description This command will create an internal array of vectors, each holding 3 floats. This can be used for feeding in positional data for particles to use upon fluid creation.
* @param size - Maximum number of elements you wish the array to hold.
* @dbpro DYN UTIL ARRAY VEC3 INIT
* @category UTIL
*/
void dynUtilArrayVec3Init(unsigned int size)
{
	if(phyGlobStruct_->vec3Array_){
        delete []phyGlobStruct_->vec3Array_;
		phyGlobStruct_->vec3ArraySize_ = 0;
	}
	phyGlobStruct_->vec3Array_ = new NxVec3[size];
	phyGlobStruct_->vec3ArraySize_ = size;
}

/*
* @description This command will set the 3 float vector at the given array index.
* @param position - Position in the array, starts at 0.
* @param inVec3ID - ID number of the dark basic vector holding the required data.
* @category UTIL
* @dbpro DYN UTIL ARRAY VEC3 SET
*/
void dynUtilArrayVec3Set(unsigned int position, int inVec3ID)
{
	if(position > phyGlobStruct_->vec3ArraySize_ - 1){
        return;
	}
#ifdef COMPILE_FOR_GDK
	NxVec3 vec(dbXVector3(inVec3ID), dbYVector3(inVec3ID), dbZVector3(inVec3ID));
	phyGlobStruct_->vec3Array_[position] = vec;
#else
        DWORD X = dbXVector3(inVec3ID);
        DWORD Y = dbYVector3(inVec3ID);
        DWORD Z = dbZVector3(inVec3ID);
		NxVec3 vec(*(float*)&X, *(float*)&Y, *(float*)&Z);
	    phyGlobStruct_->vec3Array_[position] = vec;
#endif
}

/*
* @description This command will get the 3 float vector at the given array index.
* @param position - Position in the array, starts at 0.
* @param ourVec3ID - ID number of the dark basic vector to place the required data.
* @dbpro DYN UTIL ARRAY VEC3 GET
* @category UTIL
*/
void dynUtilArrayVec3Get(unsigned int position, int outVec3ID)
{
	if(position > phyGlobStruct_->vec3ArraySize_ - 1){
        return;
	}
	NxVec3 vec = phyGlobStruct_->vec3Array_[position];
    dbSetVector3(outVec3ID, vec.x, vec.y, vec.z);
}

/*
* @description This command will release the 3 float vector array from memory, you are free to initialize another after calling this command, it does not have to be the same size as the previous one.
* @dbpro DYN UTIL ARRAY VEC3 CLEAR
* @category UTIL
*/
void dynUtilArrayVec3Clear(){
	if(phyGlobStruct_->vec3Array_){
		delete[] phyGlobStruct_->vec3Array_;
		phyGlobStruct_->vec3Array_ = 0;
		phyGlobStruct_->vec3ArraySize_ = 0;
	}
}


/*
* @description Checks to see if the given fluid exists.
* @param fluidID - ID number of the fluid.
* @param return TRUE if exists, FALSE otherwise.
* @dbpro DYN FLUID EXISTS
* @category CONSTRUCTION
*/
bool dynFluidExists(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
	return phyGlobStruct_->fluidMap_.count(fluidID);
}

/*
* @description Sets the simulation method for the fluid, a fluid simulation would need the particles to interact with each other while a particle simulation may not. Can either be set to NX_F_SPH, NX_F_NO_PARTICLE_INTERACTION or NX_F_MIXED_MODE. Note that depending on the spatial arrangement of the particles, switching from NX_F_NO_PARTICLE_INTERACTION or NX_F_MIXED_MODE to NX_F_SPH might lead to an unstable simulation state.
* @param fluidID - ID number of the fluid.
* @param method - NX_F_SPH = 1, NX_F_NO_PARTICLE_INTERACTION = 2, NX_F_MIXED_MODE = 4
* @dbpro DYN FLUID SET SIMULATION METHOD
* @category FLUID/PARTICLES
*/
void dynFluidSetSimulationMethod(int fluidID, int method){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetSimulationMethod");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		fluid->setSimulationMethod(method);
}

/*
* @description Gets the simulation method for the fluid.
* @param fluidID - ID number of the fluid.
* @return Simulation method, NX_F_SPH = 1, NX_F_NO_PARTICLE_INTERACTION = 2, NX_F_MIXED_MODE = 4
* @dbpro DYN FLUID GET SIMULATION METHOD
* @category FLUID/PARTICLES
*/
int dynFluidGetSimulationMethod(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetSimulationMethod");
			return 0;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		return fluid->getSimulationMethod();
}


/*
* @description Sets the stiffness value of the given fluid. The stiffness (or gas constant) influences the calculation of the pressure force field. Pressure is calculated from (density-initialDenisty)*stiffness. Low values of stiffness make the fluid more compressible (i.e., springy), while high values make it less compressible. The stiffness value has a weighty impact on the numerical stability of the simulation; setting very high values will result in instability. Reasonable values: 1-200
* @param fluidID - ID number of the fluid.
* @param stiffness - Stiffness value(must be positive)
* @dbpro DYN FLUID SET STIFFNESS
* @category FLUID/PARTICLES
*/
void dynFluidSetStiffness(int fluidID, float stiffness){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetStiffness");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		fluid->setStiffness(stiffness);
}

/*
* @description Gets the stiffness value for the given fluid.
* @param fluidID - ID number of the fluid.
* @return Stiffness value.
* @dbpro DYN FLUID GET STIFFNESS
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynFluidGetStiffness(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetStiffness");
			return 0;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		float value = fluid->getStiffness();
		RETURNCAST
}



/*
* @description Sets the viscosity value of the given fluid. Viscosity controls a fluid's thickness. For example, a fluid with a high viscosity will behave like treacle while a fluid with a low viscosity will be more runny like water. The viscosity member scales the viscosity force field which applies force to reduce the relative velocity of particles within the fluid. If a pair of particles are close together and have a high viscosity, then a strong force will be applied to reduce there relative velocity. If they are far apart and have a low viscosity value, then only a small force will be applied to reduce there relative motion. Reasonable values: 5-300
* @param fluidID - ID number of the fluid.
* @param value - Viscosity value.
* @dbpro DYN FLUID SET VISCOSITY
* @category FLUID/PARTICLES
*/
void dynFluidSetViscosity(int fluidID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetViscosity");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		fluid->setViscosity(value);
}

/*
* @description Gets the viscosity value for the given fluid.
* @param fluidID - ID number of the fluid.
* @return Viscosity value.
* @dbpro DYN FLUID GET VISCOSITY
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynFluidGetViscosity(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetViscosity");
			return 0;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		float value = fluid->getViscosity();
		RETURNCAST
}



/*
* @description Sets the damping value of the given fluid. The damping parameter is applied similarly to the damping parameter for rigid bodies. It is used to reduce the velocity of the particles over time. Reasonable values: 0-1 
* @param fluidID - ID number of the fluid.
* @param value - Damping value.
* @dbpro DYN FLUID SET DAMPING
* @category FLUID/PARTICLES
*/
void dynFluidSetDamping(int fluidID, float value){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetDamping");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		fluid->setDamping(value);
}

/*
* @description Gets the damping value for the given fluid.
* @param fluidID - ID number of the fluid.
* @return Damping value.
* @dbpro DYN FLUID GET DAMPING
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynFluidGetDamping(int fluidID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetDamping");
			return 0;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		float value = fluid->getDamping();
		RETURNCAST
}





/*
* @description Sets the external acceleration of the given fluid. External acceleration is applied directly to particles. It can be used to counteract the effect of gravity or simulate wind, etc.
* @param fluidID - ID number of the fluid.
* @param x - Acceleration value along the global x axis.
* @param y - Acceleration value along the global y axis.
* @param z - Acceleration value along the global z axis.
* @dbpro DYN FLUID SET EXTERNAL ACCELERATION
* @category FLUID/PARTICLES
*/
void dynFluidSetExternalAcceleration(int fluidID, float x, float y, float z){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidSetExternalAcceleration");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		fluid->setExternalAcceleration(NxVec3(x, y, z));
}

/*
* @description Gets the external acceleration of the given fluid.
* @param fluidID - ID number of the fluid.
* @param vector3ID - ID number of the vector you wish to store the values in. 
* @dbpro DYN FLUID GET EXTERNAL ACCELERATION
* @category FLUID/PARTICLES
*/
void dynFluidGetExternalAcceleration(int fluidID, int vector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidGetExternalAcceleration");
			return;
		}
	#endif
		NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
		NxVec3 vec = fluid->getExternalAcceleration();
		dbSetVector3(vector3ID, vec.x, vec.y, vec.z);
}


/*
* @description This command will remove the given emitter from the given fluid. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @dbpro DYN FLUID DELETE EMITTER
* @category FLUID/PARTICLES
*/
void dynFluidDeleteEmitter(int fluidID, int emitterIndex)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynFluidDeleteEmitter");
			return;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynFluidDeleteEmitter");
			return;
		}
	#endif
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitter * emitter = fluid->getEmitters()[emitterIndex];
	fluid->releaseEmitter(*emitter);
}

/*
* @description This command will set the rate the particles are emitted. The 'rate' and the 'life' of the particles will determine if you have enough particles in your fluid to keep it emitting consistantly, if the emitter runs out of particles it will wait until more are available before emitting again. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @param rate - Rate the particles will be emitted.
* @dbpro DYN EMITTER SET RATE
* @category FLUID/PARTICLES
*/
void dynEmitterSetRate(int fluidID, int emitterIndex, float rate)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterSetRate");
			return;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterSetRate");
			return;
		}
	#endif
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitter * emitter = fluid->getEmitters()[emitterIndex];
	emitter->setRate(rate);
}

/*
* @description This command will get the rate the particles are emitted. The 'rate' and the 'life' of the particles will determine if you have enough particles in your fluid to keep it emitting consistantly, if the emitter runs out of particles it will wait until more are available before emitting again. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @return - Rate the particles are emitted.
* @dbpro DYN EMITTER GET RATE
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynEmitterGetRate(int fluidID, int emitterIndex)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterGetRate");
			return 0;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterGetRate");
			return 0;
		}
	#endif
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitter * emitter = fluid->getEmitters()[emitterIndex];
	float value = emitter->getRate();
	RETURNCAST
}

/*
* @description This command will set the velocity magnitude the particles are emitted at. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @param velocity - Rate the particles will be emitted.
* @dbpro DYN EMITTER SET VELOCITY MAGNITUDE
* @category FLUID/PARTICLES
*/
void dynEmitterSetVelocityMagnitude(int fluidID, int emitterIndex, float velocity)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterSetVelocityMagnitude");
			return;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterSetVelocityMagnitude");
			return;
		}
	#endif
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitter * emitter = fluid->getEmitters()[emitterIndex];
	emitter->setFluidVelocityMagnitude(velocity);
}

/*
* @description This command will get the velocity magnitude the particles are emitted at. 
* @param fluidID - ID number of the fluid the given emitter belongs to.
* @param emitterIndex - Index of the emitter, the index was returned when the emitter was created, if there is only one emitter for the given fluid then the index will be 0.
* @return Rate the particles will be emitted.
* @dbpro DYN EMITTER GET VELOCITY MAGNITUDE
* @category FLUID/PARTICLES
*/
DWORDFLOAT dynEmitterGetVelocityMagnitude(int fluidID, int emitterIndex)
{
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->fluidMap_.count(fluidID)){
			Message::actorNotExist(fluidID, "dynEmitterGetVelocityMagnitude");
			return 0;
		}	
		if(phyGlobStruct_->fluidMap_[fluidID]->getNbEmitters() <= emitterIndex){
			Message::errorBox("Emitter index out of bounds", "dynEmitterGetVelocityMagnitude");
			return 0;
		}
	#endif
	NxFluid* fluid = phyGlobStruct_->fluidMap_[fluidID];
	NxFluidEmitter * emitter = fluid->getEmitters()[emitterIndex];
	float value = emitter->getFluidVelocityMagnitude();
	RETURNCAST
}

/*
* @description This command will add a box shape to an internal force field shape list, when you create a force field shape group it uses the shapes contained in this list, the list can then be cleared. 
* @param posVec3ID - ID number of the vector containing position data.
* @param rotVec3ID - ID number of the vector containing rotation data.
* @param dimVec3ID - ID number of the vector containing dimension data.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FF SHAPE BOX
* @category CONSTRUCTION
*/
bool dynMakeFFShapeBox(int posVector3ID, int rotVector3ID, int dimVector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		NxMat34 mat;
        D3DXMATRIX d3dMat;
        float posX, posY, posZ, rotX, rotY, rotZ, dimX, dimY, dimZ;   
#ifdef COMPILE_FOR_GDK
       posX = dbXVector3(posVector3ID);
       posY = dbYVector3(posVector3ID);
       posZ = dbZVector3(posVector3ID);
       rotX = dbXVector3(rotVector3ID);
       rotY = dbYVector3(rotVector3ID);
       rotZ = dbZVector3(rotVector3ID);
       dimX = dbXVector3(dimVector3ID);
       dimY = dbYVector3(dimVector3ID);
       dimZ = dbZVector3(dimVector3ID);
#else
       DWORD tposX = dbXVector3(posVector3ID);
       DWORD tposY = dbYVector3(posVector3ID);
       DWORD tposZ = dbZVector3(posVector3ID);
       DWORD trotX = dbXVector3(rotVector3ID);
       DWORD trotY = dbYVector3(rotVector3ID);
       DWORD trotZ = dbZVector3(rotVector3ID);
       DWORD tdimX = dbXVector3(dimVector3ID);
       DWORD tdimY = dbYVector3(dimVector3ID);
       DWORD tdimZ = dbZVector3(dimVector3ID);

       posX = *(float*)&tposX;
       posY = *(float*)&tposY;
       posZ = *(float*)&tposZ;
       rotX = *(float*)&trotX;
       rotY = *(float*)&trotY;
       rotZ = *(float*)&trotZ;
       dimX = *(float*)&tdimX;
       dimY = *(float*)&tdimY;
       dimZ = *(float*)&tdimZ;
#endif
	   MathUtil::setMatrix(d3dMat, posX, posY, posZ, rotX, rotY, rotZ);
	   mat.setColumnMajor44(d3dMat.m);
       NxVec3 dim(dimX, dimY, dimZ);
	   return phyGlobStruct_->phy_->makeFFShapeBox(mat, dim);
}

/*
* @description This command will add a sphere shape to an internal force field shape list, when you create a force field shape group it uses the shapes contained in this list, the list can then be cleared. 
* @param posVec3ID - ID number of the vector containing position data.
* @param rotVec3ID - ID number of the vector containing rotation data.
* @param radius - Radius for the sphere shape.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FF SHAPE SPHERE
* @category CONSTRUCTION
*/
bool dynMakeFFShapeSphere(int posVector3ID, int rotVector3ID, float radius){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		NxMat34 mat;
        D3DXMATRIX d3dMat;
        float posX, posY, posZ, rotX, rotY, rotZ;   
#ifdef COMPILE_FOR_GDK
       posX = dbXVector3(posVector3ID);
       posY = dbYVector3(posVector3ID);
       posZ = dbZVector3(posVector3ID);
       rotX = dbXVector3(rotVector3ID);
       rotY = dbYVector3(rotVector3ID);
       rotZ = dbZVector3(rotVector3ID);
#else
       DWORD tposX = dbXVector3(posVector3ID);
       DWORD tposY = dbYVector3(posVector3ID);
       DWORD tposZ = dbZVector3(posVector3ID);
       DWORD trotX = dbXVector3(rotVector3ID);
       DWORD trotY = dbYVector3(rotVector3ID);
       DWORD trotZ = dbZVector3(rotVector3ID);

       posX = *(float*)&tposX;
       posY = *(float*)&tposY;
       posZ = *(float*)&tposZ;
       rotX = *(float*)&trotX;
       rotY = *(float*)&trotY;
       rotZ = *(float*)&trotZ;
#endif
	   MathUtil::setMatrix(d3dMat, posX, posY, posZ, rotX, rotY, rotZ);
	   mat.setColumnMajor44(d3dMat.m);
	   return phyGlobStruct_->phy_->makeFFShapeSphere(mat, radius);
}

/*
* @description This command will add a calsule shape to an internal force field shape list, when you create a force field shape group it uses the shapes contained in this list, the list can then be cleared. 
* @param posVec3ID - ID number of the vector containing position data.
* @param rotVec3ID - ID number of the vector containing rotation data.
* @param radius - Radius for the capsule shape.
* @param height - Height value for the capsule shape.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FF SHAPE CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeFFShapeCapsule(int posVector3ID, int rotVector3ID, float radius, float height){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
	#endif
		NxMat34 mat;
        D3DXMATRIX d3dMat;
        float posX, posY, posZ, rotX, rotY, rotZ;   
#ifdef COMPILE_FOR_GDK
       posX = dbXVector3(posVector3ID);
       posY = dbYVector3(posVector3ID);
       posZ = dbZVector3(posVector3ID);
       rotX = dbXVector3(rotVector3ID);
       rotY = dbYVector3(rotVector3ID);
       rotZ = dbZVector3(rotVector3ID);
#else
       DWORD tposX = dbXVector3(posVector3ID);
       DWORD tposY = dbYVector3(posVector3ID);
       DWORD tposZ = dbZVector3(posVector3ID);
       DWORD trotX = dbXVector3(rotVector3ID);
       DWORD trotY = dbYVector3(rotVector3ID);
       DWORD trotZ = dbZVector3(rotVector3ID);

       posX = *(float*)&tposX;
       posY = *(float*)&tposY;
       posZ = *(float*)&tposZ;
       rotX = *(float*)&trotX;
       rotY = *(float*)&trotY;
       rotZ = *(float*)&trotZ;
#endif
	   MathUtil::setMatrix(d3dMat, posX, posY, posZ, rotX, rotY, rotZ);
	   mat.setColumnMajor44(d3dMat.m);
	   return phyGlobStruct_->phy_->makeFFShapeCapsule(mat, height, radius);
}

/*
* @description This command will add a convex shape to an internal force field shape list, when you create a force field shape group it uses the shapes contained in this list, the list can then be cleared. 
* @param posVec3ID - ID number of the vector containing position data.
* @param rotVec3ID - ID number of the vector containing rotation data.
* @param convexMeshID - ID number of the convex mesh which must have already been created or loaded from file.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FF SHAPE CONVEX
* @category CONSTRUCTION
*/
bool dynMakeFFShapeConvex(int posVector3ID, int rotVector3ID, int convexMeshID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshActorNotExist(convexMeshID, "dynMakeFFShapeConvex");
		}
	#endif
		NxConvexMesh *mesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		NxMat34 mat;
        D3DXMATRIX d3dMat;
        float posX, posY, posZ, rotX, rotY, rotZ;   
#ifdef COMPILE_FOR_GDK
       posX = dbXVector3(posVector3ID);
       posY = dbYVector3(posVector3ID);
       posZ = dbZVector3(posVector3ID);
       rotX = dbXVector3(rotVector3ID);
       rotY = dbYVector3(rotVector3ID);
       rotZ = dbZVector3(rotVector3ID);
#else
       DWORD tposX = dbXVector3(posVector3ID);
       DWORD tposY = dbYVector3(posVector3ID);
       DWORD tposZ = dbZVector3(posVector3ID);
       DWORD trotX = dbXVector3(rotVector3ID);
       DWORD trotY = dbYVector3(rotVector3ID);
       DWORD trotZ = dbZVector3(rotVector3ID);

       posX = *(float*)&tposX;
       posY = *(float*)&tposY;
       posZ = *(float*)&tposZ;
       rotX = *(float*)&trotX;
       rotY = *(float*)&trotY;
       rotZ = *(float*)&trotZ;
#endif
	   MathUtil::setMatrix(d3dMat, posX, posY, posZ, rotX, rotY, rotZ);
	   mat.setColumnMajor44(d3dMat.m);
	   return phyGlobStruct_->phy_->makeFFShapeConvex(mat, mesh);
}

/*
* @description This command will create a shape group from an intenal list of shapes. You must first add to this force field shape list using the relevent commands (dynMakeFFShapeBox etc), once the shape group is created tyou may clear the internal list(dynFFClearShapes). 
* @param ID - ID number you wish to assign to the shape group.
* @param excludeGroup - Set to TRUE if you wish to exclude the force field from the shapes in this group, presuming you have an inclusive shape group already set up.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE FF SHAPE GROUP
* @category CONSTRUCTION
*/
bool dynMakeFFShapeGroup(int ID, bool excludeGroup){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->ffShapeGroup_.count(ID)){
			Message::actorAlreadyExist(ID, std::string("dynMakeFFShapeGroup"));
			return 0;
		}
	#endif
		NxForceFieldShapeGroup * sg = 0;
		sg = phyGlobStruct_->phy_->makeFFShapeGroup(excludeGroup);
		if(sg){
			phyGlobStruct_->ffShapeGroup_[ID] = sg;
			return true;
		}
		return false;
}

/*
* @description This command will add a shape group to the given force field, the force field will only act within the shapes assigned to it using shape groups, you may also exclude areas of space using exclusion shape groups. 
* @param ID - ID number of the force field.
* @param shapeGroupID - ID number of the shape group.
* @dbpro DYN FF ADD SHAPE GROUP
* @category FORCE FIELD
*/
void dynFFAddShapeGroup(int ID, int shapeGroupID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->ffMap_.count(ID)){
			Message::actorNotExist(ID, std::string("dynFFAddShapeGroup::ForceField"));
			return;
		}
		if(!phyGlobStruct_->ffShapeGroup_.count(shapeGroupID)){
			Message::actorNotExist(ID, std::string("dynFFAddShapeGroup::ShapeGroup"));
			return;
		}
	#endif
		NxForceField *ff = phyGlobStruct_->ffMap_[ID];
		NxForceFieldShapeGroup *sg = phyGlobStruct_->ffShapeGroup_[shapeGroupID];
		ff->addShapeGroup(*sg);
}

/*
* @description This command will remove a shape group to the given force field, the force field will only act within the shapes assigned to it using shape groups, you may also exclude areas of space using exclusion shape groups. 
* @param ID - ID number of the force field.
* @param shapeGroupID - ID number of the shape group.
* @dbpro DYN FF REMOVE SHAPE GROUP
* @category FORCE FIELD
*/
void dynFFRemoveShapeGroup(int ID, int shapeGroupID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->ffMap_.count(ID)){
			Message::actorNotExist(ID, std::string("dynFFAddShapeGroup::ForceField"));
			return;
		}
		if(!phyGlobStruct_->ffShapeGroup_.count(shapeGroupID)){
			Message::actorNotExist(ID, std::string("dynFFAddShapeGroup::ShapeGroup"));
			return;
		}
	#endif
		NxForceField *ff = phyGlobStruct_->ffMap_[ID];
		NxForceFieldShapeGroup *sg = phyGlobStruct_->ffShapeGroup_[shapeGroupID];
		ff->removeShapeGroup(*sg);
}

/*
* @description This command will create a linear kernel for use with a force field, this structure holds data which will determine how, and by how much, the force field will affect objects at different positions in the force field. 
* @param ID - ID number you wish to assign to this kernel.
* @param constantVector3ID - The constant part of force field function. For a force field with cartesian coordinate space, this would affect the CORE direction objects would move in the force field.
* @param falloffLinearVector3ID - Affects how strong the force field is relative to its centre with linear falloff. 
* @param falloffQuadraticVector3ID - Affects how strong the force field is relative to its centre with quadratic falloff. 
* @param noiseVector3ID - Adds the given amount of randomness to how objects are affected along each axis. 
* @dbpro DYN MAKE FF LINEAR KERNEL
* @category CONSTRUCTION
*/
bool dynMakeFFLinearKernel(int ID, int constantVector3ID, int falloffLinearVector3ID, int falloffQuadraticVector3ID, int noiseVector3ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->kernelMap_.count(ID)){
			Message::actorAlreadyExist(ID, std::string("dynMakeFFLinearKernal"));
		}
	#endif
    NxVec3 cVec, flVec, fqVec, nVec;
#ifdef COMPILE_FOR_GDK
    cVec  = NxVec3(dbXVector3(constantVector3ID), dbYVector3(constantVector3ID),dbZVector3(constantVector3ID));
    flVec = NxVec3(dbXVector3(falloffLinearVector3ID), dbYVector3(falloffLinearVector3ID),dbZVector3(falloffLinearVector3ID));
    fqVec = NxVec3(dbXVector3(falloffQuadraticVector3ID), dbYVector3(falloffQuadraticVector3ID),dbZVector3(falloffQuadraticVector3ID));
    nVec  = NxVec3(dbXVector3(noiseVector3ID), dbYVector3(noiseVector3ID),dbZVector3(noiseVector3ID));
#else
    DWORD cVecX = dbXVector3(constantVector3ID);
    DWORD cVecY = dbYVector3(constantVector3ID);
    DWORD cVecZ = dbZVector3(constantVector3ID);
    DWORD flVecX = dbXVector3(falloffLinearVector3ID);
    DWORD flVecY = dbYVector3(falloffLinearVector3ID);
    DWORD flVecZ = dbZVector3(falloffLinearVector3ID);
    DWORD fqVecX = dbXVector3(falloffQuadraticVector3ID);
    DWORD fqVecY = dbYVector3(falloffQuadraticVector3ID);
    DWORD fqVecZ = dbZVector3(falloffQuadraticVector3ID);
    DWORD nVecX = dbXVector3(noiseVector3ID);
    DWORD nVecY = dbYVector3(noiseVector3ID);
    DWORD nVecZ = dbZVector3(noiseVector3ID);

    cVec = NxVec3(*(float*)&cVecX, *(float*)&cVecY, *(float*)&cVecZ);
    flVec = NxVec3(*(float*)&flVecX, *(float*)&flVecY, *(float*)&flVecZ);
    fqVec = NxVec3(*(float*)&fqVecX, *(float*)&fqVecY, *(float*)&fqVecZ);
    nVec = NxVec3(*(float*)&nVecX, *(float*)&nVecY, *(float*)&nVecZ);
#endif
	NxForceFieldLinearKernel *kernel = 0;
	kernel = phyGlobStruct_->phy_->makeForceFieldLinearKernel(cVec, flVec, fqVec, nVec);
	if(kernel){
		phyGlobStruct_->kernelMap_[ID] = kernel;
		return true;
	}
	return false;
}

/*
* @description This command will create a force field. 
* @param ID - ID number you wish to assign to this force field.
* @param coordinates - Type of space. NX_FFC_CARTESIAN: The X, Y, and Z axis are equivalent to the force field's own local coordinate system. NX_FFC_SPHERICAL: The X axis is directed outwards from the center of the sphere; the Y and Z axes are the tangents of the spherical surface. NX_FFC_CYLINDRICAL: The X axis is directed outwards from the cylinder axis. The Y axis is the cylinder axis, and the Z axis is the cylindrical surface tangent (directed to make XYZ a right-handed system). The z component of the p vector in the function is always 0, and the target position in this direction is ignored. NX_FFC_TOROIDAL: The X axis is directed outwards from the closest point on the torus' equator; the Y axis is the tangent of the equator at its closest point (directed in the clockwise direction as seen from the direction of the torus' axis), and the Z axis is normal to both forming a right-handed system XYZ.
* @param actorID - ID number of the object/actor to attach the force field to, set to 0 if not needed. 
* @dbpro DYN MAKE FORCE FIELD
* @category CONSTRUCTION
*/
bool dynMakeForceField(int ID, int kernelID, int coordinates, int actorID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->ffMap_.count(ID)){
			Message::actorAlreadyExist(ID, std::string("dynMakeForceField"));
			return 0;
		}
		if(!phyGlobStruct_->kernelMap_.count(ID)){
			Message::actorNotExist(ID, std::string("dynMakeForceField::kernelID"));
			return 0;
		}
	#endif
		NxForceFieldLinearKernel *kernel;
		kernel = phyGlobStruct_->kernelMap_[kernelID];
		NxActor * actor = 0;
		if(phyGlobStruct_->actorMap_.count(actorID)){
            actor = phyGlobStruct_->actorMap_[actorID];
		}
		NxForceField *ff = 0;
		ff = phyGlobStruct_->phy_->makeForceField(kernel, (NxForceFieldCoordinates)coordinates, actor);
		if(ff){
			phyGlobStruct_->ffMap_[ID] = ff;
			return true;
		}
        return false;
}


/*
* @description This command clears the force field shapes held in the internal shape list, you are free to call this once your shape group(s) has been created. 
* @dbpro DYN FF CLEAR SHAPES
* @category FORCE FIELD
*/
void dynFFClearShapes(){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
	#endif
		phyGlobStruct_->phy_->clearFFShapes();
}

/*
* @description Releases the given linear kernel.
* @dbpro DYN DELETE FF LINEAR KERNEL
* @category CONSTRUCTION
*/
void dynDeleteFFLinearKernel(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->kernelMap_.count(ID)){
			Message::actorNotExist(ID, std::string("dynDeleteFFLinearKernel"));
			return;
		}
	#endif
		phyGlobStruct_->phy_->releaseForceFieldLinearKernel(phyGlobStruct_->kernelMap_[ID]);
	    phyGlobStruct_->kernelMap_.erase(ID);
}

/*
* @description Releases the given force feild shape group.
* @dbpro DYN DELETE FF SHAPE GROUP
* @category CONSTRUCTION
*/
void dynDeleteFFShapeGroup(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->ffShapeGroup_.count(ID)){
			Message::actorNotExist(ID, std::string("dynDeleteFFShapeGroup"));
			return;
		}
	#endif
		phyGlobStruct_->phy_->releaseForceFieldShapeGroup(phyGlobStruct_->ffShapeGroup_[ID]);
	    phyGlobStruct_->ffShapeGroup_.erase(ID);
}

/*
* @description Releases the given force field.
* @dbpro DYN DELETE FORCE FIELD
* @category CONSTRUCTION
*/
void dynDeleteForceField(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->ffMap_.count(ID)){
			Message::actorNotExist(ID, std::string("dynDeleteForceField"));
			return;
		}
	#endif
		phyGlobStruct_->phy_->releaseForceField(phyGlobStruct_->ffMap_[ID]);
	    phyGlobStruct_->ffMap_.erase(ID);
}

/*
* @description Checks if the given shape group exists.
* @param ID - ID number of the shape group.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN FF SHAPE GROUP EXISTS
* @category CONSTRUCTION
*/
bool dynFFShapeGroupExists(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
	return phyGlobStruct_->ffShapeGroup_.count(ID);
}
/*
* @description Checks if the given linear kernel exists.
* @param ID - ID number of the kernel.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN FF LINEAR KERNEL EXISTS
* @category CONSTRUCTION
*/
bool dynFFLinearKernelExists(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->kernelMap_.count(ID);
}
/*
* @description Checks if the given force field exists.
* @param ID - ID number of the force field.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN FORCE FIELD EXISTS
* @category CONSTRUCTION
*/
bool dynForceFieldExists(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->ffMap_.count(ID);
}

/*
* @description Checks if the given joint desc exists.
* @param ID - ID number of the joint desc.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN JOINT DESC EXISTS
* @category CONSTRUCTION
*/
bool dynJointDescExists(int ID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
	#endif
		return phyGlobStruct_->jointDescMap_.count(ID);
}


























//std::map<int, std::map<int, NxActor*>> boneMapMap_;
//std::map<int, std::map<int, NxConvexMesh*>> convexMeshMapMap_;

//bool dynCookConvexMeshFromBone(int objectID, int meshID, int boneID){
//	//-- Seperate bone verices from whole mesh, then 
//	//-- cook convex mesh for bones only.
//	//-- Bone matrix should be set to identity prior
//	//-- to calling this function.
//	sObject *obj;
//	obj = dbGetObject(objectID);
//	sMesh *mesh = obj->ppMeshList[meshID];
//	int vCount = mesh->pBones[boneID].dwNumInfluences;
//	char* name = mesh->pBones[boneID].szName;
//	int totalVertCount = mesh->dwVertexCount;
//	NxVec3 *verts = new NxVec3[totalVertCount];
//	NxVec3 scale;
//	scale.x = obj->position.vecScale.x;
//	scale.y = obj->position.vecScale.y;
//	scale.z = obj->position.vecScale.z;
//	verts = DBUtil::getVertexData(mesh, scale);
//	DWORD* pVerts = mesh->pBones[boneID].pVertices; 
//	NxVec3* boneVerts = new NxVec3[vCount];
//	for(int i = 0; i < vCount; i++){
//        int index = *pVerts;
//        boneVerts[i] = verts[index];
//		pVerts++;
//	}
//
//	NxConvexMesh* cMesh = 0;
//	cMesh = phyGlobStruct_->phy_->cookConvexMesh(vCount, 0, boneVerts, 0);
//	convexMeshMapMap_[objectID][boneID] = cMesh;
//	delete []boneVerts;
//	delete []verts;
//
//	if(cMesh){
//	    return true;
//	}
//	else{
//        return false;
//	}
//}

//bool dynMakeConvexMeshFromBone(int sourceObjectID, int sourceBoneID, int objectID, int meshID, int boneID, float density)
//{
//	//-- Get convex mesh, created from a object/bone combination.
//	NxConvexMesh* convMesh = convexMeshMapMap_[sourceObjectID][sourceBoneID];
//	sObject* obj = dbGetObject(objectID);
//	NxVec3 scale;
//	scale.x = obj->position.vecScale.x;
//	scale.y = obj->position.vecScale.y;
//	scale.z = obj->position.vecScale.z;
//	//-- Create actor to represent new object/bone combination
//	//-- Most of the time this will be the same object/bone it was created from. 
//	BoneFromActor *aSync = new BoneFromActor(objectID, boneID, scale);
//	NxActor *actor = 0;
//	actor = phyGlobStruct_->phy_->makeConvexMesh(aSync, convMesh, DBUtil::getMatrixFromObject(objectID), density);
//	if(actor == 0){
//        delete aSync;
//		aSync = 0;
//        return false;
//	}
//	else{
//	    boneMapMap_[objectID][boneID] = actor;
//	    return true;
//	}
//}



/*
* @description Sets all bones in an object to either be updated by physics simulation or updated in the normal way. Note that it is sometimes required to call 'sync' before any changes are noticable. 
* @param objectID - ID number of the object.
* @param physics - TRUE for physics update method, FALSE otherwise.
* @dbpro DYN SET BONES
* @category RIGID BODY
*/
void dynSetBones(int objectID, bool physics){
    #ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynSetBones")){
            return;
		}
	#endif 
    dbGetObject(objectID)->position.bCustomBoneMatrix = physics;
}



/*
* @description Returns the amount of bones in the given mesh for the given object.
* @param objectID - ID number of the object.
* @param meshID - ID number of the mesh which the bones belong.
* @return Number of bones belonging to the given mesh..
* @dbpro DYN UTIL GET BONE COUNT
* @category UTIL
*/
int dynUtilGetBoneCount(int objectID, int meshID){
    #ifdef DEBUG
		if(!DBError::boneMeshExists(objectID, meshID, "dynUtilGetBoneCount")){
            return -1;
		}
	#endif 
    return dbGetObject(objectID)->ppMeshList[meshID]->dwBoneCount;
}

/*
* @description Returns the amount of meshes for the given object.
* @param objectID - ID number of the object.
* @return Number of meshes belonging to the given mesh.
* @dbpro DYN UTIL GET MESH COUNT
* @category UTIL
*/
int dynUtilGetMeshCount(int objectID){
    #ifdef DEBUG
		if(!DBError::objectExists(objectID, "dynUtilGetMeshCount")){
            return -1;
		}
	#endif 
	return dbGetObject(objectID)->iMeshCount;
}

/*
* @description This command will position and orientate the actor to the bone it represents in the simulation. The bones of your character model will change whenever the animation frame is changed or if the model is orientated/positioned in the world, you would call this command just before the physics takes over control of the bone.
* @param objectID - ID number of the object.
* @param meshID - ID number of the mesh which the bones belong.
* @param boneID - ID number of the bone.
* @dbpro DYN BONE SYNC ACTOR
* @category RIGID BODY
*/
void dynBoneSyncActor(int objectID, int meshID, int boneID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynBoneSyncActor")){
            return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(boneID) == 0){
			Message::limbNotExist(objectID, boneID, "dynBoneSyncActor");
            return;
		}
	#endif   
	NxActor* actor = 0;
	actor = phyGlobStruct_->actorLimbMap_[objectID][boneID]; 
   	sObject *obj;
	obj = dbGetObject(objectID);
	D3DXMATRIX d3dmat = *obj->ppMeshList[meshID]->pFrameMatrices[boneID];
	D3DXMATRIX objMat;
	dbGetObjectWorldMatrix(objectID, 0, &objMat);

	BoneFromActor *sync = (BoneFromActor*)actor->userData;
	D3DXMATRIX offsetMat = sync->offsetMat_;
	offsetMat._41 *= -1;
	offsetMat._42 *= -1;
	offsetMat._43 *= -1;

	d3dmat = (offsetMat * d3dmat) * objMat;
    MathUtil::RemoveScaleFromMatrix(d3dmat);
	NxMat34 physicsMat;
	physicsMat.setColumnMajor44(d3dmat.m);
	actor->setGlobalPose(physicsMat);

	//char *name = obj->ppMeshList[meshID]->pBones[boneID].szName;
	//bool stop = true;
}


/*
* @description This command will clear the matrix for the given bone, this means that after calling this command the bone will be positioned and orientated at 0,0,0. This command is used just before creating an actor to represent the given bone, since the simulation will update the bone with global coordinates it is important that the bone matrix is cleared before the actor is created.
* @param objectID - ID number of the object.
* @param meshID - ID number of the mesh which the bones belong.
* @param boneID - ID number of the bone.
* @dbpro DYN BONE CLEAR MATRIX
* @category RIGID BODY
*/
void dynBoneClearMatrix(int objectID, int meshID, int boneID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynBoneClearMatrix")){
            return;
		}
	#endif 
	sObject* obj = dbGetObject(objectID);
	D3DXMatrixIdentity(obj->ppMeshList[meshID]->pFrameMatrices[boneID]);
}


/*
* @description Fetches the global position of the biven bone, this is useful when creating joints between bones for ragdolls. Most of the time you will find the bone position is the exact position you need to create the joint.
* @param objectID - ID number of the object.
* @param meshID - ID number of the mesh which the bones belong.
* @param boneID - ID number of the bone.
* @param vec3Out - ID number of the vector3 in which the positional data will be placed.
* @dbpro DYN BONE GET POS
* @category UTIL
*/
void dynBoneGetPos(int objectID, int meshID, int boneID, int vec3Out){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynBoneGetPos")){
            return;
		}
	#endif 
	sObject* obj = dbGetObject(objectID);
	D3DXMATRIX mat;
	dbGetObjectWorldMatrix(objectID, 0, &mat);
	D3DXMATRIX frameMat = *obj->ppMeshList[meshID]->pFrameMatrices[boneID] * mat;
	dbSetVector3(vec3Out, frameMat._41, frameMat._42, frameMat._43);
}


//float dynBoneGetPosX(int objectID, int meshID, int boneID){
//	sObject* obj = dbGetObject(objectID);
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(objectID, 0, &mat);
//	D3DXMATRIX frameMat = *obj->ppMeshList[meshID]->pFrameMatrices[boneID];
//    D3DXMATRIX resultMat = frameMat * mat;
//    return resultMat._41;
//}
//
//float dynBoneGetPosY(int objectID, int meshID, int boneID){
//	sObject* obj = dbGetObject(objectID);
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(objectID, 0, &mat);
//	D3DXMATRIX frameMat = *obj->ppMeshList[meshID]->pFrameMatrices[boneID];
//    D3DXMATRIX resultMat = frameMat * mat;
//    return resultMat._42;
//}
//
//float dynBoneGetPosZ(int objectID, int meshID, int boneID){
//	sObject* obj = dbGetObject(objectID);
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(objectID, 0, &mat);
//	D3DXMATRIX frameMat = *obj->ppMeshList[meshID]->pFrameMatrices[boneID];
//    D3DXMATRIX resultMat = frameMat * mat;
//    return resultMat._43;
//}


/*
* @description Creates a convex mesh and saves it to file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, int boneID, int meshID, char* file){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynCookConvexMesh"))
			return false;
	#endif
		std::string fileStr(file);
		sObject *obj;
		obj = dbGetObject(objectID);
		dbLockVertexDataForLimb(objectID, meshID);
		int bvCount = DBUtil::getBoneVertexCount(obj, meshID, boneID);
		NxVec3* boneVertices = 0;
		boneVertices = DBUtil::getBoneVerts(obj, meshID, boneID);
		dbUnlockVertexData();
		if(!boneVertices){
            return false;
		}
		bool result = phyGlobStruct_->phy_->cookConvexMesh(fileStr, bvCount, 0, boneVertices, 0);
        delete []boneVertices;
        return result;	
}    


/*
* @description Creates a convex mesh and saves it to file, 'cooking' is just a term which means it is converting the mesh to a format PhysX can use for fast collision detection. If the mesh you provide is not convex, a convex shape will be computed for you when possible, the number of polygons is limited to 256. You should cook all your meshes to file and then when your game/app needs them there is no need for the slow cooking process, if you do need to cook meshes on the fly then be sure to check the return value and deal with both possible outcomes accordingly. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb to create the mesh from.
* @param file - Name of the file you wish to create.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN COOK CONVEX MESH
* @category CONSTRUCTION
*/
bool dynCookConvexMesh(int objectID, int boneID, int meshID, int convexMeshID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynCookConvexMesh"))
			return false;
		if(phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshAlreadyExists(convexMeshID, "dynCookConvexMesh");
			return false;
		}
	#endif
		sObject *obj;
		obj = dbGetObject(objectID);
		dbLockVertexDataForLimb(objectID, meshID);
		int bvCount = DBUtil::getBoneVertexCount(obj, meshID, boneID);
		NxVec3* boneVertices = 0;
		boneVertices = DBUtil::getBoneVerts(obj, meshID, boneID);
		dbUnlockVertexData();
		if(!boneVertices){
            return false;
		}
		NxConvexMesh* cMesh = 0;
		cMesh = phyGlobStruct_->phy_->cookConvexMesh(bvCount, 0, boneVertices, 0);
        delete []boneVertices;
		if(cMesh){
           phyGlobStruct_->convexMeshMap_[convexMeshID] = cMesh;
		   return true;
		}
		else{
           return false;
		}
}    

/*
* @description Creates an actor from a convex mesh.
* @param objectID     - ID number of the owning object.
* @param limbID       - ID number of the limb the actor will represent in the simulation.
* @param convexMeshID - ID number of the mesh you wish to create the actor from.
* @param density      - Density value of dynamic actor, 0.0 for static actor.
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CONVEX MESH
* @category CONSTRUCTION
*/
bool dynMakeConvexMesh(int objectID, int boneID, int meshID, int convexMeshID, float density){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynCookConvexMesh"))
			return false;
		if(!phyGlobStruct_->convexMeshMap_.count(convexMeshID)){
			Message::meshActorNotExist(convexMeshID, "dynMakeConvexMesh");
			return false;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(boneID)){
			Message::actorAlreadyExist(objectID, boneID, "dynMakeConvexMesh");
			return false;		
		}		
	#endif
	    sObject* obj = dbGetObject(objectID);
	    if(boneID >= obj->ppMeshList[meshID]->dwBoneCount)
		   return false;
		
		NxConvexMesh *pMesh = phyGlobStruct_->convexMeshMap_[convexMeshID];
		//sObject* obj = dbGetObject(objectID);
	    NxVec3 scale;
	    scale.x = obj->position.vecScale.x;
	    scale.y = obj->position.vecScale.y;
	    scale.z = obj->position.vecScale.z;
		scale = scale * phyGlobStruct_->phyScaleMultiplier_;
		//Create updater object
		D3DXMATRIX offset;
		D3DXMatrixIdentity(&offset);
		BoneFromActor *aSync = new BoneFromActor(objectID,
			                                     boneID,
												 meshID,
												 scale,
												 offset);     //DBUtil::getLimbScale(objectID, limbID, phyGlobStruct_->phyScaleMultiplier_));
        //Make actor with updater
		NxActor *actor = 0;
		actor = phyGlobStruct_->phy_->makeConvexMesh(aSync,
			                        pMesh,                        
									DBUtil::getMatrixFromObject(objectID), //DBUtil::getMatrixFromLimb(objectID, limbID),
									density);

		//add to map if successful
		if(actor){
			phyGlobStruct_->actorLimbMap_[objectID][boneID] = actor;
			phyGlobStruct_->actorSyncList_.push_back(aSync);
			return true;
		}
		else{
			delete aSync;
			aSync = 0;
			return false;
		}
}

#endif //compile dynamix full

















/*
* @description Raises the given flag of the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to raise.
* @dbpro DYN ACTOR RAISE FLAG
* @category RIGID BODY
*/
void dynActorRaiseFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynActorRaiseFlag");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->raiseActorFlag((NxActorFlag)flag);
}

/*
* @description Raises the given flag of the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to raise.
* @dbpro DYN ACTOR RAISE FLAG
* @category RIGID BODY
*/
void dynActorRaiseFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynActorRaiseFlag");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynActorRaiseFlag");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->raiseActorFlag((NxActorFlag)flag);
}



/*
* @description Clears the given flag of the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to clear.
* @dbpro DYN ACTOR CLEAR FLAG
* @category RIGID BODY
*/
void dynActorClearFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynActorClearFlag");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->clearActorFlag((NxActorFlag)flag);
}

/*
* @description Clears the given flag of the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to clear.
* @dbpro DYN ACTOR CLEAR FLAG
* @category RIGID BODY
*/
void dynActorClearFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynActorClearFlag");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynActorClearFlag");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->clearActorFlag((NxActorFlag)flag);
}

/*
* @description Wakes up the given actor for a given amount of time.
* @param objectID - ID number of object the actor represents in the simulation.
* @param wakeCounterValue - Amount of time the actor should remain awake for, a value of 0 is equivalent to putting the actor to sleep.
* @dbpro DYN WAKE UP
* @category RIGID BODY
*/
void dynWakeUp(int objectID, float wakeCounterValue){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynWakeUp");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->wakeUp(wakeCounterValue);
}


/*
* @description Puts the given actor to sleep, actors fall asleep themselves when they are left idle long enough, saving cpu. Use this command if you need them to fall asleep in situations when they would not otherwise do so themselves.
* @param objectID - ID number of object the actor represents in the simulation.
* @dbpro DYN PUT TO SLEEP
* @category RIGID BODY
*/
void dynPutToSleep(int objectID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynPutToSleep");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->putToSleep();
}

/*
* @description Wakes up the given actor for a given amount of time. Use this overload if the actor was created from a limb/bone. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of limb the actor represents in the simulation.
* @param wakeCounterValue - Amount of time the actor should remain awake for, a value of 0 is equivalent to putting the actor to sleep.
* @dbpro DYN WAKE UP
* @category RIGID BODY
*/
void dynWakeUp(int objectID, int limbID, float wakeCounterValue){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynWakeUp");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynWakeUp");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->wakeUp(wakeCounterValue);;
}

/*
* @description Puts the given actor to sleep, actors fall asleep themselves when they are left idle long enough, saving cpu. Use this command if you need them to fall asleep in situations when they would not otherwise do so themselves. Use this overload if the actor was created from a limb/bone. 
* @param objectID - ID number of the owning object.
* @param limbID - ID number of limb the actor represents in the simulation.
* @dbpro DYN PUT TO SLEEP
* @category RIGID BODY
*/
void dynPutToSleep(int objectID, int limbID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynPutToSleep");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynPutToSleep");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->putToSleep();
}




/*
* @description Creates a box actor. This overload creates the box from a bone in the given mesh of the object.
* @param objectID - ID number of the owning object.
* @param boneID - ID number of the bone the actor will represent in the simulation.
* @param meshID - ID number of the owning mesh.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE BOX
* @category CONSTRUCTION
*/
bool dynMakeBox(int objectID, int boneID, int meshID, float density)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynMakeBox"))
			return false;
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(boneID)){
			Message::actorAlreadyExist(objectID, boneID, "dynMakeBox");
			return false;		
		}		
	#endif
	sObject* obj = dbGetObject(objectID);
	if(boneID >= obj->ppMeshList[meshID]->dwBoneCount)
		return false;   
	BoneData bData;
	DBUtil::getBoneData(bData, objectID, meshID, boneID, NxVec3(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_));
	//sObject* obj = dbGetObject(objectID);
	NxVec3 scaleVec(obj->position.vecScale.x, obj->position.vecScale.y, obj->position.vecScale.z);

	BoneFromActor *aSync = new BoneFromActor(objectID, boneID, meshID,  scaleVec, bData.offsetMat_);
	NxActor *actor = 0;	
	actor = phyGlobStruct_->phy_->makeBox(aSync, bData.mat_, bData.extent_, density,
		              NxVec3(0, 0, 0)); 
	if(actor){
		phyGlobStruct_->actorLimbMap_[objectID][boneID] = actor;
		phyGlobStruct_->actorSyncList_.push_back(aSync);
	}
	else{
        delete aSync;
		aSync = 0;
	}
	return true;
}


/*
* @description Creates a sphere actor. This overload creates the sphere from a bone in the given mesh of the object.
* @param objectID - ID number of the owning object.
* @param boneID - ID number of the bone the actor will represent in the simulation.
* @param meshID - ID number of the owning mesh.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE SPHERE
* @category CONSTRUCTION
*/
bool dynMakeSphere(int objectID, int boneID, int meshID, float density)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynMakeSphere"))
			return false;
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(boneID)){
			Message::actorAlreadyExist(objectID, boneID, "dynMakeSphere");
			return false;		
		}		
	#endif
	sObject* obj = dbGetObject(objectID);
	if(boneID >= obj->ppMeshList[meshID]->dwBoneCount)
		return false;
    BoneData bData;
	DBUtil::getBoneData(bData, objectID, meshID, boneID, NxVec3(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_));
	//sObject* obj = dbGetObject(objectID);
	NxVec3 scaleVec(obj->position.vecScale.x, obj->position.vecScale.y, obj->position.vecScale.z);

	BoneFromActor *aSync = new BoneFromActor(objectID, boneID, meshID,  scaleVec, bData.offsetMat_);
	NxActor *actor = 0;
	float radius = (bData.extent_.x + bData.extent_.y + bData.extent_.z) / 3.0f;
	actor = phyGlobStruct_->phy_->makeSphere(aSync, bData.mat_, radius, density,
		              NxVec3(0, 0, 0)); 

	if(actor){
		phyGlobStruct_->actorLimbMap_[objectID][boneID] = actor;
		phyGlobStruct_->actorSyncList_.push_back(aSync);
	}
	else{
        delete aSync;
		aSync = 0;
	}
	return true;
}

/*
* @description Creates a capsule actor. This overload creates the capsule from a bone in the given mesh of the object.
* @param objectID - ID number of the owning object.
* @param boneID - ID number of the bone the actor will represent in the simulation.
* @param meshID - ID number of the owning mesh.
* @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
* @return TRUE if succeeded, FALSE otherwise.
* @dbpro DYN MAKE CAPSULE
* @category CONSTRUCTION
*/
bool dynMakeCapsule(int objectID, int boneID, int meshID, float density)
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return false;
		}
		if(!DBError::boneMeshExists(objectID, meshID, "dynMakeCapsule"))
			return false;
		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(boneID)){
			Message::actorAlreadyExist(objectID, boneID, "dynMakeCapsule");
			return false;		
		}		
		//Check for bone
	#endif
	sObject* obj = dbGetObject(objectID);
	if(boneID >= obj->ppMeshList[meshID]->dwBoneCount)
		return false;
    
	BoneData bData;
	DBUtil::getBoneData(bData, objectID, meshID, boneID, NxVec3(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_));

	NxVec3 scaleVec(obj->position.vecScale.x, obj->position.vecScale.y, obj->position.vecScale.z);

	BoneFromActor *aSync = new BoneFromActor(objectID, boneID, meshID,  scaleVec, bData.offsetMat_);
	NxActor *actor = 0;
	float height = bData.extent_.y / 2;
	float radius = (bData.extent_.x + bData.extent_.z) / 4.0f;
	actor = phyGlobStruct_->phy_->makeCapsule(aSync, bData.mat_, height, radius, density,
		              NxVec3(0, 0, 0)); 

	if(actor){
		phyGlobStruct_->actorLimbMap_[objectID][boneID] = actor;
		phyGlobStruct_->actorSyncList_.push_back(aSync);
	}
	else{
        delete aSync;
		aSync = 0;
	}
	return true;
}






/*
* @description Raises the given flag of the body in the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to raise.
* @dbpro DYN BODY RAISE FLAG
* @category RIGID BODY
*/
void dynBodyRaiseFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynBodyRaiseFlag");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->raiseBodyFlag((NxBodyFlag)flag);
}

/*
* @description Raises the given flag of the body in the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to raise.
* @dbpro DYN BODY RAISE FLAG
* @category RIGID BODY
*/
void dynBodyRaiseFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynBodyRaiseFlag");
		    return;

		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynBodyRaiseFlag");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->raiseBodyFlag((NxBodyFlag)flag);
}



/*
* @description Clears the given flag of the body in the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to clear.
* @dbpro DYN BODY CLEAR FLAG
* @category RIGID BODY
*/
void dynBodyClearFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynBodyClearFlag");
            return;
		}
    #endif
		phyGlobStruct_->actorMap_[objectID]->clearBodyFlag((NxBodyFlag)flag);
}

/*
* @description Clears the given flag of the body in the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to clear.
* @dbpro DYN BODY CLEAR FLAG
* @category RIGID BODY
*/
void dynBodyClearFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynBodyClearFlag");
		    return;
		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynBodyClearFlag");
			return;		
		}
    #endif
		phyGlobStruct_->actorLimbMap_[objectID][limbID]->clearBodyFlag((NxBodyFlag)flag);
}



/*
* @description Reads the given flag of the body in the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to read.
* @return TRUE if flag is raised, FALSE otherwise.
* @dbpro DYN BODY READ FLAG
* @category RIGID BODY
*/
bool dynBodyReadFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynBodyReadFlag");
            return 0;
		}
    #endif
		return phyGlobStruct_->actorMap_[objectID]->readBodyFlag((NxBodyFlag)flag);
}
/*
* @description Reads the given flag of the body in the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to read.
* @return TRUE if flag is raised, false otherwise.
* @dbpro DYN BODY READ FLAG
* @category RIGID BODY
*/
bool dynBodyReadFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynBodyReadFlag");
		    return 0;
		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynBodyReadFlag");
			return 0;		
		}
    #endif
		return phyGlobStruct_->actorLimbMap_[objectID][limbID]->readBodyFlag((NxBodyFlag)flag);
}

/*
* @description Reads the given flag of the given actor.
* @param objectID - ID number of the object the actor represents in the simulation.
* @param flag - Flag you wish to read.
* @return TRUE if flag is raised, FALSE otherwise.
* @dbpro DYN ACTOR READ FLAG
* @category RIGID BODY
*/
bool dynActorReadFlag(int objectID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::actorNotExist(objectID, "dynActorReadFlag");
            return 0;
		}
    #endif
		return phyGlobStruct_->actorMap_[objectID]->readActorFlag((NxActorFlag)flag);
}

/*
* @description Reads the given flag in the given actor. Use this overload if your actor was created from a limb/bone.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param flag - Flag you wish to read.
* @return TRUE if flag is raised, false otherwise.
* @dbpro DYN ACTOR READ FLAG
* @category RIGID BODY
*/
bool dynActorReadFlag(int objectID, int limbID, unsigned int flag){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(!phyGlobStruct_->actorLimbMap_.count(objectID)){
		    Message::actorNotExist(objectID, limbID, "dynActorReadFlag");
		    return 0;
		}
		else if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::actorNotExist(objectID, limbID, "dynActorReadFlag");
			return 0;		
		}
    #endif
		return phyGlobStruct_->actorLimbMap_[objectID][limbID]->readActorFlag((NxActorFlag)flag);
}


/*
* @description Allows you to check if a joint exists or not.
* @param jointID - ID number of the joint.
* @return TRUE if exists, FALSE otherwise.
* @dbpro DYN JOINT EXISTS
* @category CONSTRUCTION
*/
bool dynJointExists(int jointID){
	#ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
    #endif
	return phyGlobStruct_->jointMap_.count(jointID);
}



/*
* @description Call this in your game loop to make the given camera follow the given object. 
* @param cameraID - ID number of the camera.
* @param objectID - ID number of the object you wish to follow.
* @param offsetPosX - Offset value along the objects local x axis to position the camera.
* @param offsetPosY - Offset value along the objects local y axis to position the camera.
* @param offsetPosZ - Offset value along the objects local z axis to position the camera.
* @param offsetPointX - Offset value along the objects local x axis to point the camera.
* @param offsetPointY - Offset value along the objects local y axis to point the camera.
* @param offsetPointZ - Offset value along the objects local z axis to point the camera.
* @param smooth - Smooths the camera movement. Range: 0.0 to 1.0 (the lower the value the smoother the camera movement will be) 
* @dbpro DYN UTIL SET CAMERA TO OBJECT
* @category UTIL
*/
void dynUtilSetCameraToObject(int cameraID, int objectID, float offsetPosX, float offsetPosY, float offsetPosZ, float offsetPointX, float offsetPointY, float offsetPointZ, float smooth){
#ifdef DEFINE_FOR_GDK    
	float camX = dbCameraPositionX(cameraID);
    float camY = dbCameraPositionY(cameraID);
    float camZ = dbCameraPositionZ(cameraID);
#else
    DWORD tempX = dbCameraPositionX(cameraID);
	DWORD tempY = dbCameraPositionY(cameraID);
    DWORD tempZ = dbCameraPositionZ(cameraID);
	float camX = *(float*)&tempX;
	float camY = *(float*)&tempY;
	float camZ = *(float*)&tempZ;
#endif
		D3DXMATRIX camPosOffset;
		D3DXMatrixTranslation(&camPosOffset, offsetPosX, offsetPosY, offsetPosZ);
	    D3DXMATRIX camPointOffset;
		D3DXMatrixTranslation(&camPointOffset, offsetPointX, offsetPointY, offsetPointZ);
		
		D3DXMATRIX mat;	   
		dbGetObjectWorldMatrix(objectID, 0, &mat);

		D3DXMATRIX camPos;
		camPos = camPosOffset * mat;

		D3DXMATRIX camPoint;
		camPoint = camPointOffset * mat;
     
        float cX = camX + ((camPos._41 - camX) * smooth);
		float cY = camY + ((camPos._42 - camY) * smooth);
		float cZ = camZ + ((camPos._43 - camZ) * smooth);

		dbPositionCamera(cameraID, cX, cY, cZ);
		dbPointCamera(cameraID, camPoint._41, camPoint._42, camPoint._43);
}


/*
* @description Call this function to find the first limb which is associated with the current contact data. You will only find a use for this command if you have created any actors from limbs or bones.
* @return ID number of the limb. Will return 0 if actor does not represent a limb.
* @dbpro DYN CONTACT GET LIMB A
* @category CONTACT
*/
int dynContactGetLimbA(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		if(phyGlobStruct_->contact_->currentContact_){
		   NxActor* actor = phyGlobStruct_->contact_->currentContact_->pair_.actors[0];
		   ActorSync* as = (ActorSync*)actor->userData; 
		   return as->limb_;	   
		}
		else return -1;
}

/*
* @description Call this function to find the second limb which is associated with the current contact data. You will only find a use for this command if you have created any actors from limbs or bones.
* @return ID number of the limb. Will return 0 if actor does not represent a limb.
* @dbpro DYN CONTACT GET LIMB B
* @category CONTACT
*/
int dynContactGetLimbB(){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
	#endif
		if(phyGlobStruct_->contact_->currentContact_){
		   NxActor* actor = phyGlobStruct_->contact_->currentContact_->pair_.actors[1];
		   ActorSync* as = (ActorSync*)actor->userData; 
		   return as->limb_;	   
		}
		else return -1;
}



/*
* @description This command allows you to set which collision group the given actor belongs to, all actors start off in group 0.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @param group - Group number you wish to assign the given actor to.
* @dbpro DYN SET GROUP
* @category GROUP
*/
void dynSetGroup(int objectID, int limbID, int group){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::limbNotExist(objectID, limbID, "dynSetGroup");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynSetGroup");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	actor->setGroup(group);
}

/*
* @description This command allows you to retrieve which collision group the given actor belongs to, all actors start off in group 0.
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb the actor represents in the simulation.
* @return Group number in which the given actor belongs.
* @dbpro DYN GET GROUP
* @category GROUP
*/
int dynGetGroup(int objectID, int limbID){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return -1;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetGroup");
			return -1;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetGroup");
			return -1;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	return (int)actor->getGroup();
}






/*
* @description This command allows you to set which collision group the given shape belongs to, all shapes start off in group 0.
* @param actorID - ID number of the object that the actor represents in the simulation.
* @param shapeIndex - Index of the shape you wish to assign to a group. Set to -1 to assign all shapes in this actor to the group.
* @param group - Group number you wish to assign the given shape to. Range: 0 - 31
* @dbpro DYN SET SHAPE GROUP
* @category GROUP
*/
void dynSetShapeGroup(int actorID, int shapeIndex, int group){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(actorID) == 0){
			Message::actorNotExist(actorID, "dynSetShapeGroup");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorMap_[actorID];
	if( shapeIndex == -1 )
	{   // ALL SHAPES
		unsigned int shapeCount = actor->getNbShapes();
		for(unsigned int i = 0; i < shapeCount; i++){ 
			actor->getShapes()[i]->setGroup(group);
		}
	}
	else
	{   // ONE SHAPE
		if(actor->getNbShapes( ) <= shapeIndex )
			return;
		actor->getShapes()[shapeIndex]->setGroup(group);
	}
}

/*
* @description This command allows you to set which collision group the given shape belongs to, all shapes start off in group 0.
* @param actorID - ID number of the owning object.
* @param limbID - ID number of the limb which the actor represents in the simulation.
* @param shapeIndex - Index of the shape you wish to assign to a group. Set to -1 to assign all shapes in this actor to the group.
* @param group - Group number you wish to assign the given shape to. Range: 0 - 31
* @dbpro DYN SET SHAPE GROUP
* @category GROUP
*/
void dynSetShapeGroup(int actorID, int limbID, int shapeIndex, int group){
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(actorID) == 0){
			Message::limbNotExist(actorID, limbID, "dynSetShapeGroup");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[actorID].count(limbID) == 0){
			Message::limbNotExist(actorID, limbID, "dynSetShapeGroup");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorLimbMap_[actorID][limbID];
	if( shapeIndex == -1 )
	{   // ALL SHAPES
		unsigned int shapeCount = actor->getNbShapes();
		for(unsigned int i = 0; i < shapeCount; i++){ 
			actor->getShapes()[i]->setGroup(group);
		}
	}
	else
	{   // ONE SHAPE
		if(actor->getNbShapes( ) <= shapeIndex )
			return;
		actor->getShapes()[shapeIndex]->setGroup(group);
	}
}


/*
* @description This command calculates and provides you with the given actors orientation. You should only use this command if you have a special reason to do so, otherwise use the dark basic angle commands.  
* @param objectID - ID number of the object the actor represents in the simulation.
* @param outVec3ID - ID number of the vector3 the result will be placed into. 
* @dbpro DYN GET ORIENTATION
* @category RIGID BODY
*/
void dynGetOrientation( int objectID, int outVec3ID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::objectNotExist(objectID, "dynGetOrientation");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorMap_[objectID];
 	D3DXVECTOR3 vec;
	NxQuat quat = actor->getGlobalOrientationQuat();
	MathUtil::quaternionToEuler(quat, vec);
	dbSetVector3( outVec3ID, vec.x, vec.y, vec.z );
}

/*
* @description This command calculates and provides you with the given actors position. You can also use the dark basic position commands.  
* @param objectID - ID number of the object the actor represents in the simulation.
* @param outVec3ID - ID number of the vector3 the result will be placed into. 
* @dbpro DYN GET POSITION
* @category RIGID BODY
*/
void dynGetPosition( int objectID, int outVec3ID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::objectNotExist(objectID, "dynGetPosition");
			return;
		}
	#endif
    NxActor* actor = phyGlobStruct_->actorMap_[objectID];
	NxVec3 pos = actor->getGlobalPosition();
	dbSetVector3( outVec3ID, pos.x, pos.y, pos.z );
}

/*
* @description This command calculates and provides you with the given actors orientation. You should only use this command if you have a special reason to do so(such as retrieving bone angles), otherwise use the dark basic angle commands.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb which the actor represents in the simulation.
* @param outVec3ID - ID number of the vector3 the result will be placed into. 
* @dbpro DYN GET ORIENTATION
* @category RIGID BODY
*/
void dynGetOrientation( int objectID, int limbID, int outVec3ID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetOrientation");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetOrientation");
			return;
		}
	#endif
	NxActor* actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
 	D3DXVECTOR3 vec;
	NxQuat quat = actor->getGlobalOrientationQuat();
	MathUtil::quaternionToEuler(quat, vec);
	dbSetVector3( outVec3ID, vec.x, vec.y, vec.z );
}


/*
* @description This command calculates and provides you with the given actors position. You can also use the dark basic position commands.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb which the actor represents in the simulation.
* @param outVec3ID - ID number of the vector3 the result will be placed into. 
* @dbpro DYN GET POSITION
* @category RIGID BODY
*/
void dynGetPosition( int objectID, int limbID, int outVec3ID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetPosition");
			return;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynGetPosition");
			return;
		}
	#endif
    NxActor* actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	NxVec3 pos = actor->getGlobalPosition();
	dbSetVector3( outVec3ID, pos.x, pos.y, pos.z );
}



/*
* @description This command allows you to check if an actor is currently asleep. Actors fall asleep when they are no longer moving, this means they are taken out of simulation until a force is applied either directly or from contact with another actor.  
* @param objectID - ID number of the object the actor represents in the simulation.
* @return TRUE if sleeping, FALSE otherwise.
* @dbpro DYN IS SLEEPING
* @category RIGID BODY
*/
bool dynIsSleeping( int objectID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorMap_.count(objectID) == 0){
			Message::objectNotExist(objectID, "dynIsSleeping");
			return 0;
		}
	#endif
    NxActor* actor = phyGlobStruct_->actorMap_[objectID];
	return actor->isSleeping( );
}

/*
* @description This command allows you to check if an actor is currently asleep. Actors fall asleep when they are no longer moving, this means they are taken out of simulation until a force is applied either directly or from contact with another actor.  
* @param objectID - ID number of the owning object.
* @param limbID - ID number of the limb which the actor represents in the simulation.
* @return TRUE if sleeping, FALSE otherwise.
* @dbpro DYN IS SLEEPING
* @category RIGID BODY
*/
bool dynIsSleeping( int objectID, int limbID )
{
    #ifdef DEBUG
		if(!phyGlobStruct_->phy_){
			Message::ignitionError();
			return 0;
		}
		if(phyGlobStruct_->actorLimbMap_.count(objectID) == 0){
			Message::limbNotExist(objectID, limbID, "dynIsSleeping");
			return 0;
		}
		if(phyGlobStruct_->actorLimbMap_[objectID].count(limbID) == 0){
			Message::limbNotExist(objectID, limbID, "dynIsSleeping");
			return 0;
		}
	#endif
    NxActor* actor = phyGlobStruct_->actorLimbMap_[objectID][limbID];
	return actor->isSleeping( );
}

/*
* @description This command returns the Dark Dynamix version number as an integer value eg 11 = v1.1, 215 = v2.15 etc.
* @return Dark Dynamix version number.
* @dbpro DYN GET VERSION
* @category CORE
*/
int dynGetVersion( )
{
    return DYNAMIX_VERSION;
}



//
///*
//* @description Sets the first actor for the given joint descriptor.
//* @param jointDescID ID number of the descriptor.
//* @param objectID ID number of the object counterpart of the actor we want to associate with the given joint descriptor.
//* @dbpro DYN JOINT DESC SET ACTOR A
//*/
//void dynJointDescSetActorA(int jointDescID, int objectID){
//	//WE SHOULD BE ALLOWED TO PASS 0 AS STATIC WORLD ACTOR
//	//Fixed v1.1
//    #ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return;
//		}
//		if(objectID != 0 && phyGlobStruct_->actorMap_.count(objectID) == 0){
//			Message::actorNotExist(objectID, "dynJointDescSetActorA");
//            return;
//		}
//		if(phyGlobStruct_->jointDescMap_.count(jointDescID) == 0){
//			Message::jointDescNotExist(jointDescID, "dynJointDescSetActorA");
//            return;
//		}
//	#endif	
//	NxJointDesc* jointDesc = phyGlobStruct_->jointDescMap_[jointDescID];
//}


///*
//* @description Checks if the given material exists.
//* @param ID - ID number of the material.
//* @return TRUE if exists, FALSE otherwise.
//* @dbpro DYN MATERIAL EXISTS
//*/
//bool dynMaterialExists(int ID){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return false;
//		}
//		if(!phyGlobStruct_->materialMap_.count(ID)){
//			return 0;
//		}
//	#endif
//		NxMaterial *material =  phyGlobStruct_->materialMap_[ID];
//		NxMaterialFlag::
//		material->setF
//}

//
//bool  dynIsKinematic(int objectID){
//    NxActor* actor;
//	actor->get
//}
//
//bool  dynIsKinematic(int objectID, int limbID){
//
//}





///
// @description Creates a capsule actor. This overload creates the capsule from a bone in the given mesh of the object.
// @param objectID - ID number of the owning object.
// @param boneID - ID number of the bone the actor will represent in the simulation.
// @param meshID - ID number of the owning mesh.
// @param density - The density of the actor, along with its size this will determine how heavy the actor is. 
// @param locRotX - Local rotation about the x axis.
// @param locRotY - Local rotation about the y axis.
// @param locRotZ - Local rotation about the z axis.
// @return TRUE if succeeded, FALSE otherwise.
// @dbpro DYN MAKE CAPSULE
///
//bool dynMakeCapsule(int objectID, int boneID, int meshID, float density, float locRotX, float locRotY, float locRotZ)
//{
//    #ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return false;
//		}
//		if(!DBError::boneMeshExists(objectID, meshID, "dynMakeCapsule"))
//			return false;
//		if(phyGlobStruct_->actorLimbMap_.count(objectID) && phyGlobStruct_->actorLimbMap_[objectID].count(boneID)){
//			Message::actorAlreadyExist(objectID, boneID, "dynMakeCapsule");
//			return false;		
//		}		
//	#endif
//    BoneData bData;
//	DBUtil::getBoneData(bData, objectID, meshID, boneID, NxVec3(phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_, phyGlobStruct_->phyScaleMultiplier_));
//	sObject* obj = dbGetObject(objectID);
//	NxVec3 scaleVec(obj->position.vecScale.x, obj->position.vecScale.y, obj->position.vecScale.z);
//
//	BoneFromActor *aSync = new BoneFromActor(objectID, boneID, meshID,  scaleVec, bData.offsetMat_);
//	NxActor *actor = 0;
//	float height = bData.extent_.y / 2;
//	float radius = (bData.extent_.x + bData.extent_.z) / 4.0f;
//	//actor = phyGlobStruct_->phy_->makeCapsule(aSync, bData.mat_, height, radius, density,
//	//	              NxVec3(0, 0, 0)); 
//	NxMat33 rotMat;
//	float PI = 1.0f / 180.0f * 3.142f;
//	rotMat.rotX(locRotX * PI);
//	rotMat.rotY(locRotY * PI);
//	rotMat.rotZ(locRotZ * PI);
//	actor = phyGlobStruct_->phy_->makeCapsule(aSync, bData.mat_, height, radius, density,
//		              rotMat); 
//
//	if(actor){
//		phyGlobStruct_->actorLimbMap_[objectID][boneID] = actor;
//		phyGlobStruct_->actorSyncList_.push_back(aSync);
//	}
//	else{
//        delete aSync;
//		aSync = 0;
//	}
//	return true;
//}



//void dynSetUpdateObject(int sourceObjectID, int newObjectID, int limbID){
//	//Updater must be altered
//	NxActor *actor = 0;
//	actor = phyGlobStruct_->actorLimbMap_[sourceObjectID][limbID];
//	ActorSync* as = (ActorSync*)actor->userData;
//	as->object_ = newObjectID;
//
//	as->obj_ = dbGetObject(newObjectID);
//
//	//should I move from one map to the other? Probably.
//	phyGlobStruct_->actorLimbMap_[sourceObjectID].erase(limbID);
//	//remove if last?
//	//phyGlobStruct_->actorLimbMap_.erase(sourceObjectID);
//	phyGlobStruct_->actorLimbMap_[newObjectID][limbID] = actor;
//
//	// Anything else?
//
//}






#ifdef COMPILE_FOR_GDK



//void dynMakeFluid(int boundingObjectID, int particleObjectID, float rate, float life, int max){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return;
//		}
//	#endif
//	
//	phyGlobStruct_->phy_->makeFluid(rate, life, max);
//	//MULTIPLE LIMBS
//	dbMakeMeshFromObject(1, particleObjectID);
//	int exist = dbMeshExist(1);
//	for (int i = 1; i < rate * life + 1; i++){
//       	dbAddLimb(boundingObjectID, i, 1);
//	    dbTextureLimb(boundingObjectID, i, 100);
//		dbHideLimb(boundingObjectID, i);
//		phyGlobStruct_->phy_->waitingVec_.push_back(i);
//	}
//	dbSetObjectTransparency(boundingObjectID, 6);
//	dbGhostObjectOn(boundingObjectID);
//	dbDisableObjectZWrite(1);
//	dbHideLimb(boundingObjectID, 0);
//	dbHideObject(particleObjectID);
//	
//
//	bool stop = true;
//
//	//MULTIPLE OBJECTS
//	////first paticle
// //   dbHideObject(particleID);
//	//phyGlobStruct_->phy_->waitingVec_.push_back(particleID);
//	//for(int i = particleID + 1; i < particleID + rate * life; i++){
//	//    
// //       //dbMakeObjectPlain(i, 0.3, 0.3);
// //       //dbRotateObject(i, 0, -90, 0);
//	//	dbCloneObject(i, particleID);
// //       dbHideObject(i);
//	//	phyGlobStruct_->phy_->waitingVec_.push_back(i);
//	//}
//
//
//}
//
//void dynUpdateFluid(){
//	#ifdef DEBUG
//		if(!phyGlobStruct_->phy_){
//			Message::ignitionError();
//			return;
//		}
//	#endif
//
//	while(phyGlobStruct_->phy_->gParticleBufferNum > phyGlobStruct_->phy_->renderedParticleCount){
//		phyGlobStruct_->phy_->renderedVec_.push_back(phyGlobStruct_->phy_->waitingVec_.back());
//		//dbShowObject(phyGlobStruct_->phy_->renderedVec_.back());
//        dbShowLimb(1, phyGlobStruct_->phy_->renderedVec_.back());
//		phyGlobStruct_->phy_->waitingVec_.pop_back();
//		phyGlobStruct_->phy_->renderedParticleCount++;
//	}
//	while(phyGlobStruct_->phy_->gParticleBufferNum < phyGlobStruct_->phy_->renderedParticleCount){
//		phyGlobStruct_->phy_->waitingVec_.push_back(phyGlobStruct_->phy_->renderedVec_.back());
//	    //dbHideObject(phyGlobStruct_->phy_->waitingVec_.back());
//		dbHideLimb(1, phyGlobStruct_->phy_->waitingVec_.back());
//		phyGlobStruct_->phy_->renderedVec_.pop_back();
//        phyGlobStruct_->phy_->renderedParticleCount--;
//	}
//	//MULTIPLE LIMBS
//	for(int i = 0; i < phyGlobStruct_->phy_->renderedVec_.size(); i++){
//		dbOffsetLimb(1, phyGlobStruct_->phy_->renderedVec_.at(i), 
//			              phyGlobStruct_->phy_->gParticleBuffer[i].x,
//                           phyGlobStruct_->phy_->gParticleBuffer[i].y,
//                            phyGlobStruct_->phy_->gParticleBuffer[i].z);
//	}
//
//	//dbPointObject(1, dbCameraPositionX(), dbCameraPositionY(), dbCameraPositionZ());
//    
//	//dbSetObjectToCameraOrientation(1);
//	//dbTurnObjectLeft(1, 180);
//
//
//
//	//MULTIPLE OBJECTS
//	//for(int i = 0; i < phyGlobStruct_->phy_->renderedVec_.size(); i++){
//	//	dbPositionObject(phyGlobStruct_->phy_->renderedVec_.at(i), 
//	//		              phyGlobStruct_->phy_->gParticleBuffer[i].x,
// //                          phyGlobStruct_->phy_->gParticleBuffer[i].y,
// //                           phyGlobStruct_->phy_->gParticleBuffer[i].z);
//	//}
//}

#endif
//#endif //compile dynamix full  //moved further up v1.11
#endif


#ifndef COMPILE_FOR_GDK
  #undef bool
#endif
 

