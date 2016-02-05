#ifndef DBPROPHYSICSMASTER_COLLIDERS_H
#define DBPROPHYSICSMASTER_COLLIDERS_H


class ColliderTargetInfo
{
public:
	ColliderTargetInfo():enabled(true), bounce(0.5f), smoothness(0.5f), invert(false), killer(false),
	useColor(false), color(0x0ffffffff), noResolve(false), numCollisions(0), ticksSinceLastHit(0){}

	bool enabled;			//Temporarily enables or disables the emitter effector interaction (not exposed)
	float bounce;			//Elasticity property of collisions
	float smoothness;		//Frictional property of collisions (equals 1-friction)	
	bool invert;			//Used to switch collision surface from outside to inside
	bool killer;			//Used to kill particles upon collision
	bool useColor;			//Used to change color of particle upon collision
	unsigned int color;
	bool noResolve;			//Turns off collision resolution
	int numCollisions;		//Stores number of collisions with particle system
	int ticksSinceLastHit;	//Stores number of ticks since it last collided with a particle from the emitter
};


class Collider : public PhysicsObject
{
public:
	Collider(int id):PhysicsObject(id),enabled(true)
	{useExternalMatrix(true);}

	virtual ~Collider(){}
	virtual void onUpdate();
	virtual void onUpdateGraphics(){}
	virtual int baseClassID(){return classID;}
	
	virtual void updateInit(){}

	virtual bool canHandleParticles(){return false;}
	virtual bool canHandleCloth(){return false;}
	virtual bool canHandleRagdoll(){return false;}
	virtual bool canHandleVehicle(){return false;}

	virtual void updateParticles();
	virtual void updateCloth();
	virtual void updateRagdoll();
	virtual void updateVehicle();

	virtual void onUpdateParticles(){}
	virtual void onUpdateCloth(){}
	virtual void onUpdateRagdoll(){}
	virtual void onUpdateVehicle(){}

	virtual void enable(bool state){if(enabled!=state){enabled=state;}}
	bool isEnabled(){return enabled;}

	ePhysError addEmitter(EmitterPtr& ptr);
	ePhysError removeEmitter(int id);

	ePhysError addCloth(ClothPtr& ptr);
	ePhysError removeCloth(int id);

	ePhysError addRagdoll(RagdollPtr& ptr);
	ePhysError removeRagdoll(int id);

	ePhysError addVehicle(VehiclePtr& ptr);
	ePhysError removeVehicle(int id);

	ePhysError setInteraction(int id, bool state);
	ePhysError setBounce(int id, float bounce);
	ePhysError setFriction(int id, float friction);
	ePhysError setInvert(int id, bool state);
	ePhysError setKiller(int id, bool state);
	ePhysError setUseColor(int id, bool state);
	ePhysError setColor(int id, DWORD color);
	ePhysError setResolve(int id, bool state);

	//static const int classID='COLL';
	//int classID;

	static const int classID;
protected:
	ePhysError getObjectInfo(int id,ColliderTargetInfo ** info);

	EmitterList emitterList;
	LinkedList_SortID<ColliderTargetInfo> emitterInfo;

	ClothList clothList;
	LinkedList_SortID<ColliderTargetInfo> clothInfo;

	RagdollList ragdollList;
	LinkedList_SortID<ColliderTargetInfo> ragdollInfo;

	VehicleList vehicleList;
	LinkedList_SortID<ColliderTargetInfo> vehicleInfo;

	bool enabled;	
};

#endif
