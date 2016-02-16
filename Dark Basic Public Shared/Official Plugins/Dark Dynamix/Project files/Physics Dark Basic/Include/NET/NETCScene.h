#pragma once

#include "Physics.h"
#include "MathUtil.h"
#include "DGDKPlugin.h"

/**
* This class controls any scenes in your simulation.
*/
public ref class CScene : public CDGDKPlugin
{
    public:

	CScene( CDGDKGlobals^ oGlobals )
		: CDGDKPlugin( oGlobals ){
	}

   /**
	* Sets the current scene. 
	* @param sceneID ID number of the scene.
	*/
	void SetCurrentScene(int sceneID){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->setCurrentScene(sceneID);
	}

   /**
	* Gets the current scene. 
	* @return ID number of the current scene. 
	*/
	int GetCurrentScene(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return -1;
			}
		#endif
	    return myPhy->getCurrentScene();
	}

   /**
	* Creates a new scene. 
	* @param sceneID Allocate an ID number to reference the newly created scene. 
	*/
	void MakeScene(int sceneID){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->makeScene(sceneID);
	}

   /**
	* Releases a scene. 
	* @param sceneID ID number of the scene you wish to delete. 
	*/
	void DeleteScene(int sceneID){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->releaseScene(sceneID);
	}

   /**
	* Gets the amount of scenes in the simulation. 
	* @return Amount of scenes currently held in the simulation. 
	*/
	int GetSceneCount(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return -1;
			}
		#endif
		return myPhy->getSceneCount();
	}

	/**
	* Set the gravity for the current scene.
	* @param x Gravity along the x-axis.
	* @param y Gravity along the y-axis.
	* @param z Gravity along the z-axis.
	*/
	void SetGravity(float x, float y, float z){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->setGravity(x, y, z);
	}

//  /**
//	* This command keeps your rendered objects in sync with the simulation,
//	* use this as opposed to update(), which updates all scenes.
//	* @param sceneID ID number of the scene you wish to update. 
//	*/
//	void UpdateScene(int sceneID){
//		NETUserData *ud = 0;
//		NxU32 count = 0;
//		NxActiveTransform *at = myPhy->getActiveTransforms(sceneID, count);
//		if(count && at){
//			for(NxU32 i = 0; i < count; i++){		
//				ud = (NETUserData*)at[i].userData;
//				D3DXVECTOR3 orient;
//				NxVec3 pos;
//                MathUtil::quaternionToEuler(at[i].actor->getGlobalOrientationQuat(), orient);
//				pos = at[i].actor->getGlobalPosition();
//				m_oGbl->m_pDGDKBasic3D->RotateObject(ud->object_, orient.x, orient.y, orient.z);
//				m_oGbl->m_pDGDKBasic3D->PositionObject(ud->object_, pos.x, pos.y, pos.z);	
//			}
//		}
//	}
};