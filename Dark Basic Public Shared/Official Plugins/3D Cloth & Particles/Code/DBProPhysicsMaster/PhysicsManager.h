#ifndef DBPROPHYSICSMASTER_PHYSICSMANAGER_H
#define DBPROPHYSICSMASTER_PHYSICSMANAGER_H

extern class PhysicsManager * physics;


typedef int  ( *DLL_GETEXISTS_FP)			( int );
typedef bool ( *DLL_CREATENEWOBJECT_FP )	( int iID, LPSTR pName ); 
typedef bool ( *DLL_DELETEOBJECT_FP )		( int iID );

typedef void ( *DLL_ON_OBJECT_DELETE_CALLBACK_FP )	( int Id, int userData);
typedef void ( *DLL_SETDELETECALLBACK_FP )			( int, DLL_ON_OBJECT_DELETE_CALLBACK_FP, int );
typedef void ( *DLL_DISABLETRANSFORM_FP )			( int Id, bool state);
typedef void ( *DLL_GETPOSITIONDATA_FP)				( int, struct sPositionData**);
typedef void ( *DLL_UPDATEPOSITIONDATA_FP)			(struct sPositionData*);
typedef void ( *DLL_SETWORLDMATRIX_FP)				( int Id, D3DXMATRIX * mat );
typedef void ( *DLL_GETWORLDMATRIX_FP)				( int Id, int Limb, D3DXMATRIX * mat );

typedef int   ( *DLL_GETLIMBCOUNT_FP )		( int );
typedef void  ( *DLL_CREATEMESH_FP )		( int, DWORD, DWORD, DWORD );
typedef void  ( *DLL_LOCKVERTEXDATA_FP )	( int, int );
typedef void  ( *DLL_SETVERTEXPOSITION_FP ) ( int, float, float, float );
typedef void  ( *DLL_SETVERTEXUV_FP )		( int, float, float );
typedef void  ( *DLL_SETVERTEXNORMAL_FP )	( int, float, float, float );
typedef void  ( *DLL_SETINDEX_FP )			( int, int );
typedef void  ( *DLL_UNLOCKVERTEXDATA_FP )	( void );
typedef int   ( *DLL_GETVERTEXCOUNT_FP )	( void );
typedef DWORD ( *DLL_GETVERTEXPOSITION_FP ) ( int );

typedef void ( *DLL_SETVERTEXSIZE_FP )		( int, float );
typedef void ( *DLL_SETVERTEXDIFFUSE_FP )	( int, DWORD );

typedef D3DXVECTOR3 ( *DLL_GETCAMERALOOK )		( void );
typedef D3DXVECTOR3 ( *DLL_GETCAMERAUP )		( void );
typedef D3DXVECTOR3 ( *DLL_GETCAMERARIGHT )		( void );
typedef D3DXVECTOR3 ( *DLL_GETCAMERAMATRIX )	( void );
typedef D3DXVECTOR3 ( *DLL_GETCAMERAPOSITION )	( void );

typedef bool (*DLL_SETNEWOBJECTFINALPROPERTIES ) ( int, float );

typedef void (*EMITTER_CALLBACK)( EmitterPtr&  ptr, void * userData);
typedef void (*EFFECTOR_CALLBACK)( EffectorPtr& ptr, void * userData );
typedef void (*COLLIDER_CALLBACK)( ColliderPtr& ptr, void * userData );
typedef void (*CLOTH_CALLBACK)( ClothPtr& ptr, void * userData );
typedef void (*VEHICLE_CALLBACK)( VehiclePtr& ptr, void * userData );
typedef void (*RAGDOLL_CALLBACK)( RagdollPtr& ptr, void * userData );

void onObjectDeleteCallback(int Id, int userData);

class PhysicsManager
{
	friend void onObjectDeleteCallback(int Id, int userData);
public:
	PhysicsManager();
	~PhysicsManager();

	void getDLLHandle();

	void Update();
	void UpdateGraphics();
	void setFrameRate(float fps);
	void garbageCollect();

	ePhysError addEmitter(int id, int num, EMITTER_FACTORYFUNC emitterFactoryFunc);
	ePhysError deleteEmitter(int id, bool notifiedByCallback=false);
	ePhysError getEmitter(int id,EmitterPtr& ptr);

	ePhysError addEffector(int id, EFFECTOR_FACTORYFUNC effectorFactoryFunc);
	ePhysError deleteEffector(int id, bool notifiedByCallback=false);
	ePhysError getEffector(int id,EffectorPtr& ptr);

	ePhysError addCollider(int id, COLLIDER_FACTORYFUNC colliderFactoryFunc);
	ePhysError deleteCollider(int id, bool notifiedByCallback=false);
	ePhysError getCollider(int id,ColliderPtr& ptr);

	ePhysError addCloth(int id, CLOTH_FACTORYFUNC clothFactoryFunc);
	ePhysError deleteCloth(int id, bool notifiedByCallback=false);
	ePhysError getCloth(int id,ClothPtr& ptr);

	ePhysError addVehicle(int id, VEHICLE_FACTORYFUNC vehicleFactoryFunc);
	ePhysError deleteVehicle(int id, bool notifiedByCallback=false);
	ePhysError getVehicle(int id,VehiclePtr& ptr);

	ePhysError addRagdoll(int id, RAGDOLL_FACTORYFUNC ragdollFactoryFunc);
	ePhysError deleteRagdoll(int id, bool notifiedByCallback=false);
	ePhysError getRagdoll(int id,RagdollPtr& ptr);


	int getNumEmitter(){return emitterList.Count();}
	int getNumEffector(){return effectorList.Count();}
	int getNumCollider(){return colliderList.Count();}
	int getNumCloth(){return clothList.Count();}
	int getNumVehicle(){return vehicleList.Count();}
	int getNumRagdoll(){return ragdollList.Count();}

	GlobStruct * DBPro_globalPtr;

	TimerUtil timer;

	static bool hasGarbage;

	static float mainFps;
	static float mainTimeInterval;

	static float iterationFps;
	static float iterationInterval;	
	static int numIterations;
	static int curIteration;

	static int numIterations_Particles;
	static int numIterations_Cloth;
	static int numIterations_Ragdoll;
	static int numIterations_Vehicle;

	static DLL_GETEXISTS_FP			DLL_GetExists;
	static DLL_CREATENEWOBJECT_FP	DLL_CreateNewObject;
	static DLL_DELETEOBJECT_FP		DLL_DeleteObject;
	
	static DLL_SETDELETECALLBACK_FP		DLL_SetDeleteCallback;
	static DLL_DISABLETRANSFORM_FP		DLL_DisableTransform;	
	static DLL_GETPOSITIONDATA_FP		DLL_GetPositionData;
	static DLL_UPDATEPOSITIONDATA_FP	DLL_UpdatePositionData;
	static DLL_SETWORLDMATRIX_FP		DLL_SetWorldMatrix;
	static DLL_GETWORLDMATRIX_FP		DLL_GetWorldMatrix;

	static DLL_GETLIMBCOUNT_FP		DLL_GetLimbCount;
	static DLL_CREATEMESH_FP		DLL_CreateMesh;
	static DLL_LOCKVERTEXDATA_FP	DLL_LockVertexData;
	static DLL_SETVERTEXPOSITION_FP DLL_SetVertexPosition;
	static DLL_SETVERTEXNORMAL_FP	DLL_SetVertexNormal;
	static DLL_SETVERTEXUV_FP		DLL_SetVertexUV;
	static DLL_SETINDEX_FP			DLL_SetIndex;
	static DLL_UNLOCKVERTEXDATA_FP	DLL_UnlockVertexData;		
	static DLL_GETVERTEXCOUNT_FP	DLL_GetVertexCount;
	static DLL_GETVERTEXPOSITION_FP	DLL_GetVertexPosX;
	static DLL_GETVERTEXPOSITION_FP	DLL_GetVertexPosY;
	static DLL_GETVERTEXPOSITION_FP	DLL_GetVertexPosZ;

	static DLL_SETVERTEXSIZE_FP		DLL_SetVertexSize;
	static DLL_SETVERTEXDIFFUSE_FP	DLL_SetVertexDiffuse;

	static DLL_GETCAMERALOOK		DLL_GetCameraLook;
	static DLL_GETCAMERAUP			DLL_GetCameraUp;
	static DLL_GETCAMERARIGHT		DLL_GetCameraRight;
	static DLL_GETCAMERAMATRIX		DLL_GetCameraMatrix;
	static DLL_GETCAMERAPOSITION	DLL_GetCameraPosition;

	static DLL_SETNEWOBJECTFINALPROPERTIES	DLL_SetNewObjectFinalProperties;

private:
	EmitterList emitterList;
	EffectorList effectorList;
	ColliderList colliderList;
	ClothList clothList;
	VehicleList vehicleList;
	RagdollList ragdollList;

	static bool ignoreEmitterCB;
	static bool ignoreEffectorCB;
	static bool ignoreColliderCB;
	static bool ignoreClothCB;
	static bool ignoreVehicleCB;
	static bool ignoreRagdollCB;
};

#endif