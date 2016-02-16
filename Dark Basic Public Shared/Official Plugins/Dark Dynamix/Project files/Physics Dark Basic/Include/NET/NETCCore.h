#pragma once

#include "Physics.h"
#include "DGDKPlugin.h"
#include "NETCScene.h"

/**
* Core physics commands. 
*/
public ref class CCore : public CDGDKPlugin
{
    public:
   // CScene ^oScene_;
	CCore( CDGDKGlobals^ oGlobals)//, CScene^ oScene )  //CScene^ &oScene has no affect(it seems)
		: CDGDKPlugin( oGlobals ){
			//We need access to scene object
			//oScene_ = oScene;
	}

   /**
	* Initialises the physics and creates the default scene. This command will fail if the
	* required PhysX dll's can't be found. If you are relying on the user to 
	* have the PhysX system software then this command will fail if they don't
	* have it.
	* @return TRUE if succeeded, FALSE otherwise. 
	*/
	bool Start(void)
	{	
		#ifdef DEBUG
			if(myPhy){
				startCalledError();
				return false;
			}
		#endif
		myPhy = new Physics();
		bool result = myPhy->start();
		if(result){
			IDirect3DDevice9 *d3ddev = 0;
			d3ddev = (IDirect3DDevice9*)m_oGbl->m_pDGDKDisplay->GetDirect3DDevice();
			d3dUtil = new D3DUtil(d3ddev, myPhy->getDebugRenderable());
		}
		return result;
	}

   /**
	* Releases the physics and any scenes that have been created along with
	* everything they may contain, actors, controllers etc...
	*/
	void Stop(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		if(d3dUtil){
			delete d3dUtil;
			d3dUtil = 0;
		}
		myPhy->stop();
		delete myPhy;
		myPhy = 0;
	}

   /**
    * This command is responsible for keeping your rendered objects in sync with the 
	* simulation, this command will update your objects for the current scene.
	*/
	void Update(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		NETUserData *ud = 0;
		NxU32 count = 0;
		NxActiveTransform *at = myPhy->getActiveTransforms(count);
		if(count && at){
			for(NxU32 i = 0; i < count; i++){		
				ud = (NETUserData*)at[i].userData;
				D3DXVECTOR3 orient;
				NxVec3 pos;
                MathUtil::quaternionToEuler(at[i].actor->getGlobalOrientationQuat(), orient);
				pos = at[i].actor->getGlobalPosition();
				m_oGbl->m_pDGDKBasic3D->RotateObject(ud->object_, orient.x, orient.y, orient.z);
				m_oGbl->m_pDGDKBasic3D->PositionObject(ud->object_, pos.x, pos.y, pos.z);	
			}
		}
		//for(myPhy->sceneIt_ = myPhy->sceneMap_.begin(); myPhy->sceneIt_ != myPhy->sceneMap_.end(); ++myPhy->sceneIt_)
		//{
		//	//access to scene object
		//	oScene_->UpdateScene((*myPhy->sceneIt_).first);
		//}
		
	}

   /**
    * Starts the simulation, you would normally call this near the end of your game loop and
	* match it with a call to FetchResults() near the start of your loop.
	* Try to avoid applying forces, making/deleting actors etc while simulating.
	*/
    void Simulate(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->simulate();
	}

  /**
    * Fetches the results from the last call to Simulate(), you can then call Update()
	* to sync your objects with the results of the simulation.
	* Now is the best time to apply forces, make/delete actors etc before
	* the next call to Simulate().
	*/
	void FetchResults(){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->fetchResults();
	}

	/*
	* This command allows you to set the values for the global parameters of the Physics SDK. You can change the default skin width, sleep velocities, bounce threshold, continuous collision detection, friction scaling and all the parameters used for setting up the debug renderer. 
	* @param parameter ID number of the parameter that you wish to set.
	* @param value New value for the given parameter.
	*/
	void SetParameter(int parameter, float value){
		#ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return;
			}
		#endif
		myPhy->setParameter(parameter, value);
	}

};