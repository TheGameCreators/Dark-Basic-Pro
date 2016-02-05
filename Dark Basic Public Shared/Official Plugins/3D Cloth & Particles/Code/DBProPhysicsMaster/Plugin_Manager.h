#ifndef DBPROPHYSICSMASTER_PLUGINMANAGER_H
#define DBPROPHYSICSMASTER_PLUGINMANAGER_H

class PhysicsObject;
class Effector;
class Emitter;
class Collider;
class Cloth;
class Vehicle;
class Ragdoll;

#define DLLEXPORT __declspec ( dllexport )

//Use typedefs so we can easily change storage type if necessary
typedef SmartLinkedList_SortID<Emitter> EmitterList;
typedef SmartLinkedList_SortID<Effector> EffectorList;
typedef SmartLinkedList_SortID<Collider> ColliderList;
typedef SmartLinkedList_SortID<Cloth> ClothList;
typedef SmartLinkedList_SortID<Vehicle> VehicleList;
typedef SmartLinkedList_SortID<Ragdoll> RagdollList;

typedef SmartPtr<PhysicsObject> PhysPtr;
typedef SmartPtr<Emitter> EmitterPtr;
typedef SmartPtr<Effector> EffectorPtr;
typedef SmartPtr<Collider> ColliderPtr;
typedef SmartPtr<Cloth> ClothPtr;
typedef SmartPtr<Vehicle> VehiclePtr;
typedef SmartPtr<Ragdoll> RagdollPtr;

typedef Effector*(*EFFECTOR_FACTORYFUNC)(int);
typedef Emitter*(*EMITTER_FACTORYFUNC)(int,int);
typedef Collider*(*COLLIDER_FACTORYFUNC)(int);
typedef Cloth*(*CLOTH_FACTORYFUNC)(int);
typedef Vehicle*(*VEHICLE_FACTORYFUNC)(int);
typedef Ragdoll*(*RAGDOLL_FACTORYFUNC)(int);
typedef void* PluginHandle;

struct EffectorPlugin
{	
	EFFECTOR_FACTORYFUNC makeInstance;
};

struct EmitterPlugin
{	
	EMITTER_FACTORYFUNC makeInstance;
};

struct ColliderPlugin
{	
	COLLIDER_FACTORYFUNC makeInstance;
};

struct ClothPlugin
{	
	CLOTH_FACTORYFUNC makeInstance;
};

struct VehiclePlugin
{	
	VEHICLE_FACTORYFUNC makeInstance;
};

struct RagdollPlugin
{	
	RAGDOLL_FACTORYFUNC makeInstance;
};

//Export this function to make it easy to add plugins from external DLLs
extern DLLEXPORT class PluginManager * getPluginManager();

class PluginManager
{
public:
	~PluginManager();

	static PluginManager * getPluginManager()
	{
#if _DEBUG
		INIT_MEMORYLEAK_DETECTION(-1);
#endif
		static PluginManager inst;
		return &inst;
	}
	void * RegisterEffector(EFFECTOR_FACTORYFUNC ff);
	EFFECTOR_FACTORYFUNC getEffectorFactoryFunc(PluginHandle handle){return reinterpret_cast<EffectorPlugin*>(handle)->makeInstance;}
	
	void * RegisterEmitter(EMITTER_FACTORYFUNC ff);
	EMITTER_FACTORYFUNC getEmitterFactoryFunc(PluginHandle handle){return reinterpret_cast<EmitterPlugin*>(handle)->makeInstance;}

	void * RegisterCollider(COLLIDER_FACTORYFUNC ff);
	COLLIDER_FACTORYFUNC getColliderFactoryFunc(PluginHandle handle){return reinterpret_cast<ColliderPlugin*>(handle)->makeInstance;}

	void * RegisterCloth(CLOTH_FACTORYFUNC ff);
	CLOTH_FACTORYFUNC getClothFactoryFunc(PluginHandle handle){return reinterpret_cast<ClothPlugin*>(handle)->makeInstance;}

	void * RegisterVehicle(VEHICLE_FACTORYFUNC ff);
	VEHICLE_FACTORYFUNC getVehicleFactoryFunc(PluginHandle handle){return reinterpret_cast<VehiclePlugin*>(handle)->makeInstance;}

	void * RegisterRagdoll(RAGDOLL_FACTORYFUNC ff);
	RAGDOLL_FACTORYFUNC getRagdollFactoryFunc(PluginHandle handle){return reinterpret_cast<RagdollPlugin*>(handle)->makeInstance;}

private:
	PluginManager(){};
	PluginManager(PluginManager& rhs);
	PluginManager& operator=(const PluginManager& rhs);

	ptrList<EffectorPlugin,16> effectorPlugins;
	ptrList<EmitterPlugin,16> emitterPlugins;
	ptrList<ColliderPlugin,16> colliderPlugins;
	ptrList<ClothPlugin,16> clothPlugins;
	ptrList<VehiclePlugin,16> vehiclePlugins;
	ptrList<RagdollPlugin,16> ragdollPlugins;
};

//This is a macro that creates a static member of a locally defined proxy class
//to keep a handle to the class's plugin structure
#define EFFECTOR_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterEffector(makeInstance);}\
	static Effector * makeInstance(int id){return new className(id);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;


#define EMITTER_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterEmitter(makeInstance);}\
	static Emitter * makeInstance(int id,int num){return new className(id,num);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;

#define COLLIDER_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterCollider(makeInstance);}\
	static Collider * makeInstance(int id){return new className(id);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;


#define CLOTH_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterCloth(makeInstance);}\
	static Cloth * makeInstance(int id){return new className(id);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;

#define VEHICLE_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterVehicle(makeInstance);}\
	static Vehicle * makeInstance(int id){return new className(id);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;

#define RAGDOLL_PLUGIN_INTERFACE(className) \
class PluginInfo_##className\
{\
public:\
	PluginInfo_##className(){handle=PluginManager::getPluginManager()->RegisterRagdoll(makeInstance);}\
	static Ragdoll * makeInstance(int id){return new className(id);}\
	static PluginHandle handle;\
}; extern PluginInfo_##className __PluginInfo_##className##_INST;



#define DECLARE_PLUGIN(className) PluginHandle PluginInfo_##className::handle=0;\
	PluginInfo_##className __PluginInfo_##className##_INST;

#define EFFECTOR_PLUGIN(className) PluginManager::getPluginManager()->getEffectorFactoryFunc(__PluginInfo_##className##_INST.handle)
#define EMITTER_PLUGIN(className) PluginManager::getPluginManager()->getEmitterFactoryFunc(__PluginInfo_##className##_INST.handle)
#define COLLIDER_PLUGIN(className) PluginManager::getPluginManager()->getColliderFactoryFunc(__PluginInfo_##className##_INST.handle)
#define CLOTH_PLUGIN(className) PluginManager::getPluginManager()->getClothFactoryFunc(__PluginInfo_##className##_INST.handle)
#define VEHICLE_PLUGIN(className) PluginManager::getPluginManager()->getVehicleFactoryFunc(__PluginInfo_##className##_INST.handle)
#define RAGDOLL_PLUGIN(className) PluginManager::getPluginManager()->getRagdollFactoryFunc(__PluginInfo_##className##_INST.handle)

#endif