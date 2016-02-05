#ifndef DBPROPHYSICSMASTER_EFFECTORS_H
#define DBPROPHYSICSMASTER_EFFECTORS_H

#define CLAMP(var,a,b) var<a?var=a:(var>b?var=b:0);
#define SPHFALLOFF(info,vectorA,vectorB) info->normRadSqr*(info->minRadSqr-(vectorA-vectorB).MagSqr());

class EffectorTargetInfo
{
public:
	EffectorTargetInfo():enabled(true),fade(1.0f),
	useFalloff(false),useCylFalloff(false),falloffPerParticle(false),
	minRadSqr(1.0f),normRadSqr(1.0f),minHgt(1.0f),normHgt(1.0f){}

	inline float CylindricalFalloff(const Vector3& vec, const Matrix& mat)
	{
		Vector3 loc(vec-mat.pos());
		Vector3 comp(loc.Dot(mat.unitX()),loc.Dot(mat.unitY()),loc.Dot(mat.unitZ()));
		float f1=normRadSqr*(minRadSqr-(comp.x*comp.x+comp.z*comp.z));
		CLAMP(f1,0.0f,1.0f);
		if(comp.y<0.0f) comp.y=-comp.y;
		float f2=normHgt*(minHgt-comp.y);
		CLAMP(f2,0.0f,1.0f);
		return f1*f2;
	}

	bool enabled;	//Temporarily enables or disables the emitter effector interaction (not exposed)
	float fade;		//Dictates amount of effect the effector has on the emitter(0->1)
	Vector3 v;		//A place to store data pertinent to the emitter effector interaction

	//Parameters for falloff - not necessarily used by every type of effector
	bool useFalloff;
	bool useCylFalloff;	//As opposed to spherical
	bool falloffPerParticle;
	float minRadSqr,normRadSqr;
	float minHgt,normHgt;
};

class Effector : public PhysicsObject
{
public:
	Effector(int id):PhysicsObject(id),enabled(true)
	{
		useExternalMatrix(true);
		needUpdate[0]=true;
		needUpdate[1]=true;
		needUpdate[2]=true;
		needUpdate[3]=true;		
	}

	virtual ~Effector(){}
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

	virtual void enable(bool state)
	{
		if(enabled!=state)
		{
			enabled=state;
			needUpdate[0]=true;
			needUpdate[1]=true;
			needUpdate[2]=true;
			needUpdate[3]=true;
		}
	}

	bool isEnabled(){return enabled;}
    
	ePhysError addEmitter(EmitterPtr& ptr);
	ePhysError removeEmitter(int id);
	ePhysError addCloth(ClothPtr& ptr);
	ePhysError removeCloth(int id);
	ePhysError addVehicle(VehiclePtr& ptr);
	ePhysError removeVehicle(int id);
	ePhysError addRagdoll(RagdollPtr& ptr);
	ePhysError removeRagdoll(int id);

	ePhysError setInteraction(int id, float pc);

	ePhysError setFallOff(int id, bool state);
	ePhysError setFallOffPerParticle(int id, bool state);
	ePhysError useSphereFalloff(int id);
	ePhysError useCylinderFalloff(int id);
	ePhysError setFalloffRadius(int id, float mnRad, float mxRad);
	ePhysError setFalloffHeight(int id, float mnHgt, float mxHgt);

	//static const int classID='EFCT';
	//int classID;

	static const int classID;
	
protected:
	ePhysError getObjectInfo(int id,EffectorTargetInfo ** info);

	EmitterList emitterList;
	ClothList clothList;
	VehicleList vehicleList;
	RagdollList ragdollList;
	LinkedList_SortID<EffectorTargetInfo> emitterInfo;
	LinkedList_SortID<EffectorTargetInfo> clothInfo;
	LinkedList_SortID<EffectorTargetInfo> vehicleInfo;
	LinkedList_SortID<EffectorTargetInfo> ragdollInfo;

	bool enabled;
	bool needUpdate[4];	//Used for effectors that only need to access objects when their state changes
};


#endif