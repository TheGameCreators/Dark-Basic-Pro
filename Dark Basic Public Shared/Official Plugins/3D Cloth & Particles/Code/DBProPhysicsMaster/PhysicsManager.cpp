#include "stdafx.h"

PhysicsManager * physics;

float PhysicsManager::mainFps = 60.0f;
float PhysicsManager::mainTimeInterval = 1.0f/60.0f;

int PhysicsManager::numIterations_Particles=1;
int PhysicsManager::numIterations_Cloth=1;
int PhysicsManager::numIterations_Ragdoll=2;
int PhysicsManager::numIterations_Vehicle=2;

//These values get set for each simulation group as they are updated
float PhysicsManager::iterationFps = 60.0f;
float PhysicsManager::iterationInterval = 1.0f/60.0f;
int PhysicsManager::numIterations = 1;
int PhysicsManager::curIteration = 0;

bool PhysicsManager::hasGarbage=false;

bool PhysicsManager::ignoreEmitterCB=false;
bool PhysicsManager::ignoreEffectorCB=false;
bool PhysicsManager::ignoreColliderCB=false;
bool PhysicsManager::ignoreClothCB=false;
bool PhysicsManager::ignoreVehicleCB=false;
bool PhysicsManager::ignoreRagdollCB=false;


DLL_GETEXISTS_FP			PhysicsManager::DLL_GetExists=0;
DLL_CREATENEWOBJECT_FP		PhysicsManager::DLL_CreateNewObject=0;
DLL_DELETEOBJECT_FP			PhysicsManager::DLL_DeleteObject=0;
DLL_SETDELETECALLBACK_FP	PhysicsManager::DLL_SetDeleteCallback=0;
DLL_DISABLETRANSFORM_FP		PhysicsManager::DLL_DisableTransform=0;
DLL_GETPOSITIONDATA_FP		PhysicsManager::DLL_GetPositionData=0;
DLL_UPDATEPOSITIONDATA_FP	PhysicsManager::DLL_UpdatePositionData=0;
DLL_SETWORLDMATRIX_FP		PhysicsManager::DLL_SetWorldMatrix=0;
DLL_GETWORLDMATRIX_FP		PhysicsManager::DLL_GetWorldMatrix=0;


DLL_GETLIMBCOUNT_FP			PhysicsManager::DLL_GetLimbCount;
DLL_CREATEMESH_FP			PhysicsManager::DLL_CreateMesh=0;
DLL_LOCKVERTEXDATA_FP		PhysicsManager::DLL_LockVertexData=0;
DLL_SETVERTEXPOSITION_FP	PhysicsManager::DLL_SetVertexPosition=0;
DLL_SETVERTEXUV_FP			PhysicsManager::DLL_SetVertexUV=0;
DLL_SETINDEX_FP				PhysicsManager::DLL_SetIndex=0;
DLL_UNLOCKVERTEXDATA_FP		PhysicsManager::DLL_UnlockVertexData=0;
DLL_SETVERTEXNORMAL_FP		PhysicsManager::DLL_SetVertexNormal=0;
DLL_GETVERTEXCOUNT_FP		PhysicsManager::DLL_GetVertexCount=0;
DLL_GETVERTEXPOSITION_FP	PhysicsManager::DLL_GetVertexPosX=0;
DLL_GETVERTEXPOSITION_FP	PhysicsManager::DLL_GetVertexPosY=0;
DLL_GETVERTEXPOSITION_FP	PhysicsManager::DLL_GetVertexPosZ=0;

DLL_GETCAMERALOOK			PhysicsManager::DLL_GetCameraLook=0;
DLL_GETCAMERAUP				PhysicsManager::DLL_GetCameraUp=0;
DLL_GETCAMERARIGHT			PhysicsManager::DLL_GetCameraRight=0;
DLL_GETCAMERAMATRIX			PhysicsManager::DLL_GetCameraMatrix=0;
DLL_GETCAMERAPOSITION		PhysicsManager::DLL_GetCameraPosition=0;

DLL_SETNEWOBJECTFINALPROPERTIES	PhysicsManager::DLL_SetNewObjectFinalProperties=0;

DLL_SETVERTEXSIZE_FP		PhysicsManager::DLL_SetVertexSize=0;
DLL_SETVERTEXDIFFUSE_FP		PhysicsManager::DLL_SetVertexDiffuse=0;


#define CB_EMITTER_ID 1
#define CB_EFFECTOR_ID 2
#define CB_COLLIDER_ID 3
#define CB_CLOTH_ID 4
#define CB_VEHICLE_ID 5
#define CB_RAGDOLL_ID 6


PhysicsManager::PhysicsManager()
:DBPro_globalPtr(0)
{

}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::getDLLHandle()
{	
	HINSTANCE DBProBasic3D_DLLHandle;
	HINSTANCE DBProImage_DLLHandle;

	DBProBasic3D_DLLHandle = DBPro_globalPtr->g_Basic3D;
	DBProImage_DLLHandle = DBPro_globalPtr->g_Image;
		
	//Debug lines
	D3D_DebugLines::getInst()->Init(DBPro_globalPtr->g_GFX);

	TextureAccess::DLL_GetTexturePointer = ( DLL_GETTEXTUREPOINTER_FP ) GetProcAddress (DBProImage_DLLHandle, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );

#ifdef USINGMODULE_P
	Emitter::DLL_MakeEmitter   = (DLL_MAKEEMITTER_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?MakeEmitter@@YAXHH@Z");	
	Emitter::DLL_DeleteEmitter = (DLL_DELETEEMITTER_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?DeleteEx@@YAXH@Z");	
	Emitter::DLL_GetEmitterData  = (DLL_GETEMITTERDATA_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetEmitterData@@YAXHPAPAEPAKPAPAH@Z");
	Emitter::DLL_UpdateEmitter   = (DLL_UPDATEEMITTER_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?UpdateEmitter@@YAXH@Z");
#endif

	PhysicsManager::DLL_GetLimbCount = (DLL_GETLIMBCOUNT_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetLimbCount@@YAHH@Z");
	PhysicsManager::DLL_CreateMesh = (DLL_CREATEMESH_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?CreateMeshForObject@@YAXHKKK@Z");
	PhysicsManager::DLL_LockVertexData = (DLL_LOCKVERTEXDATA_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?LockVertexDataForLimb@@YAXHH@Z");
	PhysicsManager::DLL_SetVertexPosition = (DLL_SETVERTEXPOSITION_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?SetVertexDataPosition@@YAXHMMM@Z");
	PhysicsManager::DLL_SetVertexUV = ( DLL_SETVERTEXUV_FP ) GetProcAddress ( DBProBasic3D_DLLHandle, "?SetVertexDataUV@@YAXHMM@Z" );
	PhysicsManager::DLL_SetVertexNormal = ( DLL_SETVERTEXNORMAL_FP ) GetProcAddress ( DBProBasic3D_DLLHandle, "?SetVertexDataNormals@@YAXHMMM@Z" );
	PhysicsManager::DLL_SetIndex = (DLL_SETINDEX_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?SetIndexData@@YAXHH@Z");
	PhysicsManager::DLL_UnlockVertexData = (DLL_UNLOCKVERTEXDATA_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?UnlockVertexData@@YAXXZ");
	PhysicsManager::DLL_GetVertexCount = (DLL_GETVERTEXCOUNT_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetVertexDataVertexCount@@YAHXZ");
	PhysicsManager::DLL_GetVertexPosX = (DLL_GETVERTEXPOSITION_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetVertexDataPositionX@@YAKH@Z");
	PhysicsManager::DLL_GetVertexPosY = (DLL_GETVERTEXPOSITION_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetVertexDataPositionY@@YAKH@Z");
	PhysicsManager::DLL_GetVertexPosZ = (DLL_GETVERTEXPOSITION_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetVertexDataPositionZ@@YAKH@Z");

	PhysicsManager::DLL_SetVertexSize = (DLL_SETVERTEXSIZE_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?SetVertexDataSize@@YAXHM@Z");
	PhysicsManager::DLL_SetVertexDiffuse = (DLL_SETVERTEXDIFFUSE_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?SetVertexDataDiffuse@@YAXHK@Z");

	PhysicsManager::DLL_GetCameraLook = (DLL_GETCAMERALOOK)  GetProcAddress(DBProBasic3D_DLLHandle, "?GetCameraLook@@YA?AUD3DXVECTOR3@@XZ");
	PhysicsManager::DLL_GetCameraUp = (DLL_GETCAMERAUP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetCameraUp@@YA?AUD3DXVECTOR3@@XZ");
	PhysicsManager::DLL_GetCameraRight = (DLL_GETCAMERARIGHT) GetProcAddress(DBProBasic3D_DLLHandle, "?GetCameraRight@@YA?AUD3DXVECTOR3@@XZ");
	PhysicsManager::DLL_GetCameraMatrix = (DLL_GETCAMERAMATRIX) GetProcAddress(DBProBasic3D_DLLHandle, "?GetCameraMatrix@@YA?AUD3DXVECTOR3@@XZ");
	PhysicsManager::DLL_GetCameraPosition = (DLL_GETCAMERAPOSITION)  GetProcAddress(DBProBasic3D_DLLHandle, "?GetCameraPosition@@YA?AUD3DXVECTOR3@@XZ");

	PhysicsManager::DLL_SetNewObjectFinalProperties = (DLL_SETNEWOBJECTFINALPROPERTIES)	GetProcAddress(DBProBasic3D_DLLHandle, "?SetNewObjectFinalProperties@@YA_NHM@Z");

	PhysicsManager::DLL_GetPositionData = (DLL_GETPOSITIONDATA_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetPositionData@@YAXHPAPAUsPositionData@@@Z");
	PhysicsManager::DLL_UpdatePositionData = (DLL_UPDATEPOSITIONDATA_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?UpdatePositionStructure@@YAXPAUsPositionData@@@Z");
	PhysicsManager::DLL_SetWorldMatrix = (DLL_SETWORLDMATRIX_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?SetWorldMatrix@@YAXHPAUD3DXMATRIX@@@Z");	
	PhysicsManager::DLL_GetWorldMatrix = (DLL_GETWORLDMATRIX_FP) GetProcAddress(DBProBasic3D_DLLHandle, "?GetWorldMatrix@@YAXHHPAUD3DXMATRIX@@@Z");	

	PhysicsManager::DLL_GetExists = (DLL_GETEXISTS_FP) GetProcAddress(DBProBasic3D_DLLHandle,"?GetExist@@YAHH@Z");
	PhysicsManager::DLL_CreateNewObject = (DLL_CREATENEWOBJECT_FP) GetProcAddress(DBProBasic3D_DLLHandle,"?CreateNewObject@@YA_NHPAD@Z");
	PhysicsManager::DLL_DeleteObject = (DLL_DELETEOBJECT_FP) GetProcAddress(DBProBasic3D_DLLHandle,"?DeleteObject@@YA_NH@Z");

	PhysicsManager::DLL_SetDeleteCallback = (DLL_SETDELETECALLBACK_FP) GetProcAddress(DBProBasic3D_DLLHandle,"?SetDeleteCallBack@@YAXHP6AXHH@ZH@Z");

	PhysicsManager::DLL_DisableTransform = ( DLL_DISABLETRANSFORM_FP ) GetProcAddress (DBProBasic3D_DLLHandle, "?SetDisableTransform@@YAXH_N@Z" );	
}

void PhysicsManager::Update()
{
	//Call the general update functions for effectors and colliders
	//so that they update their matrices
	if(effectorList.gotoFirstItem())
	{
		do
		{
			effectorList.getCurItem_WARNING()->Update();	
		}while(effectorList.gotoNextItem());
	}

	if(colliderList.gotoFirstItem())
	{
		do
		{
			colliderList.getCurItem_WARNING()->Update();	
		}while(colliderList.gotoNextItem());
	}

#ifdef USINGMODULE_P
	//Update Particles
	numIterations = numIterations_Particles;
	iterationFps = mainFps*numIterations;
	iterationInterval = mainTimeInterval/numIterations;	
	curIteration = 0;
	
	for(curIteration=0;curIteration<numIterations;curIteration++)
	{		
		if(emitterList.gotoFirstItem())
		{
			do
			{
				emitterList.getCurItem_WARNING()->Update();		
			}
			while(emitterList.gotoNextItem());
		}
		if(effectorList.gotoFirstItem())
		{
			do
			{
				effectorList.getCurItem_WARNING()->updateParticles();	
			}while(effectorList.gotoNextItem());
		}

		if(colliderList.gotoFirstItem())
		{
			do
			{
				colliderList.getCurItem_WARNING()->updateParticles();	
			}while(colliderList.gotoNextItem());
		}
	}
#endif

#ifdef USINGMODULE_C
	//Update Cloth
	numIterations = numIterations_Cloth;
	iterationFps = mainFps*numIterations;
	iterationInterval = mainTimeInterval/numIterations;	
	curIteration = 0;

	for(curIteration=0;curIteration<numIterations;curIteration++)
	{
		if(clothList.gotoFirstItem())
		{
			do
			{
				clothList.getCurItem_WARNING()->Update();	
			}while(clothList.gotoNextItem());
		}
		if(effectorList.gotoFirstItem())
		{
			do
			{
				effectorList.getCurItem_WARNING()->updateCloth();	
			}while(effectorList.gotoNextItem());
		}

		if(colliderList.gotoFirstItem())
		{
			do
			{
				colliderList.getCurItem_WARNING()->updateCloth();	
			}while(colliderList.gotoNextItem());
		}
	}
#endif

#ifdef USINGMODULE_R
	//Update Ragdolls	
	numIterations = numIterations_Ragdoll;
	iterationFps = mainFps*numIterations;
	iterationInterval = mainTimeInterval/numIterations;	
	curIteration = 0;
	
	for(curIteration=0;curIteration<numIterations;curIteration++)
	{
		if(ragdollList.gotoFirstItem())
		{
			do
			{
				ragdollList.getCurItem_WARNING()->Update();	
			}while(ragdollList.gotoNextItem());
		}
		if(effectorList.gotoFirstItem())
		{
			do
			{
				effectorList.getCurItem_WARNING()->updateRagdoll();	
			}while(effectorList.gotoNextItem());
		}
		if(colliderList.gotoFirstItem())
		{
			do
			{
				colliderList.getCurItem_WARNING()->updateRagdoll();	
			}while(colliderList.gotoNextItem());
		}
	}
#endif
#ifdef USINGMODULE_V
	//Update Vehicles
	numIterations = numIterations_Vehicle;
	iterationFps = mainFps*numIterations;
	iterationInterval = mainTimeInterval/numIterations;	
	curIteration = 0;
	
	for(curIteration=0;curIteration<numIterations;curIteration++)
	{
		if(vehicleList.gotoFirstItem())
		{
			do
			{
				vehicleList.getCurItem_WARNING()->Update();	
			}while(vehicleList.gotoNextItem());
		}
		if(effectorList.gotoFirstItem())
		{
			do
			{
				effectorList.getCurItem_WARNING()->updateVehicle();	
			}while(effectorList.gotoNextItem());
		}
		if(colliderList.gotoFirstItem())
		{
			do
			{
				colliderList.getCurItem_WARNING()->updateVehicle();	
			}while(colliderList.gotoNextItem());
		}
	}
#endif
}

void PhysicsManager::UpdateGraphics()
{
#ifdef USINGMODULE_P
	if(emitterList.gotoFirstItem())
	{
		do
		{
			emitterList.getCurItem_WARNING()->UpdateGraphics();		
		}
		while(emitterList.gotoNextItem());
	}
#endif
#ifdef USINGMODULE_C
	if(clothList.gotoFirstItem())
	{
		do
		{
			clothList.getCurItem_WARNING()->UpdateGraphics();	
		}while(clothList.gotoNextItem());
	}
#endif

	if(effectorList.gotoFirstItem())
	{
		do
		{
			effectorList.getCurItem_WARNING()->UpdateGraphics();	
		}while(effectorList.gotoNextItem());
	}

	if(colliderList.gotoFirstItem())
	{
		do
		{
			colliderList.getCurItem_WARNING()->UpdateGraphics();	
		}while(colliderList.gotoNextItem());
	}
#ifdef USINGMODULE_V
	if(vehicleList.gotoFirstItem())
	{
		do
		{
			vehicleList.getCurItem_WARNING()->UpdateGraphics();	
		}while(vehicleList.gotoNextItem());
	}
#endif
#ifdef USINGMODULE_R
	if(ragdollList.gotoFirstItem())
	{
		do
		{
			ragdollList.getCurItem_WARNING()->UpdateGraphics();	
		}while(ragdollList.gotoNextItem());
	}
#endif

	D3D_DebugLines::getInst()->DrawGraphics();
}

void PhysicsManager::setFrameRate(float fps)
{
	float oldFps=mainFps;
	mainFps = fps;
	mainTimeInterval = 1.0f/fps;

#ifdef USINGMODULE_P
	if(emitterList.gotoFirstItem())
	{
		do
		{
			emitterList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);		
		}
		while(emitterList.gotoNextItem());
	}
#endif
#ifdef USINGMODULE_C
	if(clothList.gotoFirstItem())
	{
		do
		{
			clothList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);	
		}while(clothList.gotoNextItem());
	}
#endif

	if(effectorList.gotoFirstItem())
	{
		do
		{
			effectorList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);	
		}while(effectorList.gotoNextItem());
	}

	if(colliderList.gotoFirstItem())
	{
		do
		{
			colliderList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);	
		}while(colliderList.gotoNextItem());
	}
#ifdef USINGMODULE_V
	if(vehicleList.gotoFirstItem())
	{
		do
		{
			vehicleList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);	
		}while(vehicleList.gotoNextItem());
	}
#endif
#ifdef USINGMODULE_R
	if(ragdollList.gotoFirstItem())
	{
		do
		{
			ragdollList.getCurItem_WARNING()->notifyFrameRateChange(oldFps,mainFps);	
		}while(ragdollList.gotoNextItem());
	}
#endif
}

void PhysicsManager::garbageCollect()
{
	if(!hasGarbage) return;

#ifdef USINGMODULE_P
	if(emitterList.gotoFirstItem())
	{
		do
		{
			emitterList.getCurItem_WARNING()->garbageCollect();
		}
		while(emitterList.gotoNextItem());
	}
#endif

#ifdef USINGMODULE_C
	if(clothList.gotoFirstItem())
	{
		do
		{
			clothList.getCurItem_WARNING()->garbageCollect();
		}while(clothList.gotoNextItem());
	}
#endif

	if(effectorList.gotoFirstItem())
	{
		do
		{
			effectorList.getCurItem_WARNING()->garbageCollect();
		}while(effectorList.gotoNextItem());
	}

	if(colliderList.gotoFirstItem())
	{
		do
		{
			colliderList.getCurItem_WARNING()->garbageCollect();
		}while(colliderList.gotoNextItem());
	}
#ifdef USINGMODULE_V
	if(vehicleList.gotoFirstItem())
	{
		do
		{
			vehicleList.getCurItem_WARNING()->garbageCollect();
		}while(vehicleList.gotoNextItem());
	}
#endif
#ifdef USINGMODULE_R
	if(ragdollList.gotoFirstItem())
	{
		do
		{
			ragdollList.getCurItem_WARNING()->garbageCollect();
		}while(ragdollList.gotoNextItem());
	}
#endif
	hasGarbage=false;
}

ePhysError PhysicsManager::addEmitter(int id, int num, EMITTER_FACTORYFUNC emitterFactoryFunc)
{	
#ifdef USINGMODULE_P
	if(id<1) return PERR_BAD_PARAMETER;
	if(emitterList.Exists(id)) return PERR_ITEM_EXISTS;	
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;

	PhysicsManager::DLL_CreateNewObject(id,"emitter");
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_EMITTER_ID);
	PhysicsManager::DLL_CreateMesh( id, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 |D3DFVF_DIFFUSE, 4*num, 6*num );

	//Stop DBPro transforming particles - they're already in world space
	PhysicsManager::DLL_DisableTransform(id,true);

	EmitterPtr tmp = emitterFactoryFunc(id,num);	

	emitterList.Add(id,tmp);

	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::deleteEmitter(int id, bool notifiedByCallback)
{
#ifdef USINGMODULE_P
	if(id<0) return PERR_BAD_PARAMETER;
	if(!emitterList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;

    if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreEmitterCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreEmitterCB=false;
    }

	emitterList.Get(id)->removeFromUse();
	emitterList.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::getEmitter(int id,EmitterPtr& ptr)
{
#ifdef USINGMODULE_P
	if(id<0) return PERR_BAD_PARAMETER;
	if(!emitterList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=emitterList.Get(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}



ePhysError PhysicsManager::addEffector(int id, EFFECTOR_FACTORYFUNC effectorFactoryFunc)
{	
	if(id<1) return PERR_BAD_PARAMETER;
	if(effectorList.Exists(id)) return PERR_ITEM_EXISTS;
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;

	PhysicsManager::DLL_CreateNewObject(id,"Effector");	
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_EFFECTOR_ID);

	//Passing a temporary local smart pointer looks dodgy, but it's
	//no different from passing a normal pointer. The reference to
	//the object is copied, not the temporary smart pointer object itself.
	
	EffectorPtr tmp = effectorFactoryFunc(id);
	effectorList.Add(id,tmp);

	return PERR_AOK;
}

ePhysError PhysicsManager::deleteEffector(int id, bool notifiedByCallback)
{
	if(id<0) return PERR_BAD_PARAMETER;
	if(!effectorList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;	

	if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreEffectorCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreEffectorCB=false;
    }

	effectorList.Get(id)->removeFromUse();
	effectorList.Delete(id);
	return PERR_AOK;
}

ePhysError PhysicsManager::getEffector(int id,EffectorPtr& ptr)
{
	if(id<0) return PERR_BAD_PARAMETER;
	if(!effectorList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=effectorList.Get(id);
	return PERR_AOK;
}


ePhysError PhysicsManager::addCollider(int id, COLLIDER_FACTORYFUNC colliderFactoryFunc)
{	
	if(id<1) return PERR_BAD_PARAMETER;
	if(colliderList.Exists(id)) return PERR_ITEM_EXISTS;
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;


	PhysicsManager::DLL_CreateNewObject(id,"Collider");	
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_COLLIDER_ID);
	
	//Passing a temporary local smart pointer looks dodgy, but it's
	//no different from passing a normal pointer. The reference to
	//the object is copied, not the temporary smart pointer object itself.
	
	ColliderPtr tmp = colliderFactoryFunc(id);
	colliderList.Add(id,tmp);

	return PERR_AOK;
}

ePhysError PhysicsManager::deleteCollider(int id, bool notifiedByCallback)
{
	if(id<0) return PERR_BAD_PARAMETER;
	if(!colliderList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;	

	if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreColliderCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreColliderCB=false;
    }

	colliderList.Get(id)->removeFromUse();
	colliderList.Delete(id);
	return PERR_AOK;
}

ePhysError PhysicsManager::getCollider(int id,ColliderPtr& ptr)
{
	if(id<0) return PERR_BAD_PARAMETER;
	if(!colliderList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=colliderList.Get(id);
	return PERR_AOK;
}



ePhysError PhysicsManager::addCloth(int id, CLOTH_FACTORYFUNC clothFactoryFunc)
{
#ifdef USINGMODULE_C
	if(id<1) return PERR_BAD_PARAMETER;
	if(clothList.Exists(id)) return PERR_ITEM_EXISTS;	
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;

	PhysicsManager::DLL_CreateNewObject(id,"Cloth");	
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_CLOTH_ID);

	//Stop DBPro transforming cloth vertices - they're already in world space
	PhysicsManager::DLL_DisableTransform(id,true);


	ClothPtr tmp = clothFactoryFunc(id);	

	clothList.Add(id,tmp);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif

}

ePhysError PhysicsManager::deleteCloth(int id, bool notifiedByCallback)
{
#ifdef USINGMODULE_C
	if(id<0) return PERR_BAD_PARAMETER;
	if(!clothList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;

	if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreClothCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreClothCB=false;
    }

//	Cloth::DLL_DeleteCloth(id);

	clothList.Get(id)->removeFromUse();
	clothList.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::getCloth(int id,ClothPtr& ptr)
{
#ifdef USINGMODULE_C
	if(id<0) return PERR_BAD_PARAMETER;
	if(!clothList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=clothList.Get(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}



ePhysError PhysicsManager::addVehicle(int id, VEHICLE_FACTORYFUNC vehicleFactoryFunc)
{	
#ifdef USINGMODULE_V
	if(id<1) return PERR_BAD_PARAMETER;
	if(vehicleList.Exists(id)) return PERR_ITEM_EXISTS;
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;


	PhysicsManager::DLL_CreateNewObject(id,"Vehicle");		
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_VEHICLE_ID);

	//Stop DBPro transforming the vehicle - it's already in world space
	PhysicsManager::DLL_DisableTransform(id,true);

	//Passing a temporary local smart pointer looks dodgy, but it's
	//no different from passing a normal pointer. The reference to
	//the object is copied, not the temporary smart pointer object itself.
	
	VehiclePtr tmp = vehicleFactoryFunc(id);
	vehicleList.Add(id,tmp);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::deleteVehicle(int id, bool notifiedByCallback)
{
#ifdef USINGMODULE_V
	if(id<0) return PERR_BAD_PARAMETER;
	if(!vehicleList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;	

	if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreVehicleCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreVehicleCB=false;
    }

	vehicleList.Get(id)->removeFromUse();
	vehicleList.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::getVehicle(int id,VehiclePtr& ptr)
{
#ifdef USINGMODULE_V
	if(id<0) return PERR_BAD_PARAMETER;
	if(!vehicleList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=vehicleList.Get(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::addRagdoll(int id, RAGDOLL_FACTORYFUNC ragdollFactoryFunc)
{	
#ifdef USINGMODULE_R
	if(id<1) return PERR_BAD_PARAMETER;
	if(ragdollList.Exists(id)) return PERR_ITEM_EXISTS;
	if(DLL_GetExists(id)) return PERR_ITEM_EXISTS;


	PhysicsManager::DLL_CreateNewObject(id,"Ragdoll");		
	PhysicsManager::DLL_SetDeleteCallback(id,onObjectDeleteCallback,CB_RAGDOLL_ID);

	//Stop DBPro transforming the ragdoll - it's already in world space
	PhysicsManager::DLL_DisableTransform(id,true);

	//Passing a temporary local smart pointer looks dodgy, but it's
	//no different from passing a normal pointer. The reference to
	//the object is copied, not the temporary smart pointer object itself.
	
	RagdollPtr tmp = ragdollFactoryFunc(id);
	ragdollList.Add(id,tmp);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::deleteRagdoll(int id, bool notifiedByCallback)
{
#ifdef USINGMODULE_R
	if(id<0) return PERR_BAD_PARAMETER;
	if(!ragdollList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;	

	if(!notifiedByCallback)
    {
        //Object must still exist, so make sure we ignore any callbacks and delete it
		ignoreRagdollCB=true;
		PhysicsManager::DLL_DeleteObject(id);		
		ignoreRagdollCB=false;
    }

	ragdollList.Get(id)->removeFromUse();
	ragdollList.Delete(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}

ePhysError PhysicsManager::getRagdoll(int id,RagdollPtr& ptr)
{
#ifdef USINGMODULE_R
	if(id<0) return PERR_BAD_PARAMETER;
	if(!ragdollList.Exists(id)) return PERR_ITEM_DOESNT_EXIST;
	ptr=ragdollList.Get(id);
	return PERR_AOK;
#else
	return PERR_BAD_MODULE;
#endif
}





void onObjectDeleteCallback(int id, int userData)
{
	switch (userData)
	{
	case CB_EMITTER_ID:
		if(!PhysicsManager::ignoreEmitterCB) physics->deleteEmitter(id,true);
		break;
	case CB_EFFECTOR_ID:
		if(!PhysicsManager::ignoreEffectorCB) physics->deleteEffector(id,true);
		break;
	case CB_COLLIDER_ID:
		if(!PhysicsManager::ignoreColliderCB) physics->deleteCollider(id,true);
		break;
	case CB_CLOTH_ID:
		if(!PhysicsManager::ignoreClothCB) physics->deleteCloth(id,true);
		break;
	case CB_VEHICLE_ID:
		if(!PhysicsManager::ignoreVehicleCB) physics->deleteVehicle(id,true);
		break;
	case CB_RAGDOLL_ID:
		if(!PhysicsManager::ignoreRagdollCB) physics->deleteRagdoll(id,true);
		break;
	};
}

