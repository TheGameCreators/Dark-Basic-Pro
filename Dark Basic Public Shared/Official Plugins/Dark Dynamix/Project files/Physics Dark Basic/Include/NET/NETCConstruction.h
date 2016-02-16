#pragma once

#include "Physics.h"
#include "DGDKPlugin.h"
#include "dbodata.h"


/**
* This class is what you use to create/release most actors in
* the simulation.
*/
public ref class CConstruction : public CDGDKPlugin
{
    public:

	CConstruction( CDGDKGlobals^ oGlobals )
		: CDGDKPlugin( oGlobals ){
	}

   /**
	* Creates a box actor in the current scene
	* @param objectID The ID number of the object you want the actor to represent in the simulation
	* @return TRUE if succeeded, FALSE otherwise. 
	*/
	bool MakeBox(int objectID){
	    #ifdef DEBUG
			if(!myPhy){
				ignitionError();
				return false;
			}
		#endif

		sObject *obj = 0;
	    obj = (sObject*)m_oGbl->m_pDGDKBasic3D->GetObject(objectID);
	   
        #ifdef DEBUG
			if(!obj)
			   return false;
        #endif

		D3DXMATRIX mat;
		m_oGbl->m_pDGDKBasic3D->GetObjectWorldMatrix(objectID, 0, 
			mat._11, mat._12, mat._13, mat._14, 
            mat._21, mat._22, mat._23, mat._24, 
            mat._31, mat._32, mat._33, mat._34, 
            mat._41, mat._42, mat._43, mat._44);

		NETUserData *ud = new NETUserData(objectID, obj, mat);
		NxActor* actor = 0;
		actor = myPhy->makeBox(ud);
		if(!actor)
			return false;
       
		actorMap[objectID] = actor;
		return true;
	}
};