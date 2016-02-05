#ifndef DBPROPHYSICSMASTER_VEHICLE_H
#define DBPROPHYSICSMASTER_VEHICLE_H


float getGroundHeight(const Vector3 &pos, Vector3 * groundNormal, float * planeD);

class Vehicle;


class VehicleParticle
{
public:
	VehicleParticle(float x, float y, float z):pos(x,y,z){}
	VehicleParticle(const Vector3& p):pos(p){}
	Vector3 pos; //Local position to car body
	Vector3 wpos; //World transformed position
};


class Vehicle : public PhysicsObject
{
	friend class PhysicsManager;
	friend class Suspension;

public:
	Vehicle(int id);
	virtual ~Vehicle();

	virtual void onUpdate();	
	virtual void onUpdateGraphics();
	void Freeze(){frozen=true;}		//Stops particles moving
	void unFreeze(){frozen=false;}	//Starts particles moving
	bool isFrozen(){return frozen;}	
	void setDebugLines(bool state){drawDebugLines=state;}

	void setGravity(const Vector3& g){gravity=g.y;}
	void setDamping(float d){damping=d;}
	
	void addNewCollisionPoint(const Vector3& p);	
	ePhysError addObjectToCollisionPoints(int objectId);

	ePhysError addSuspension(const Vector3 &pos, const Vector3 &dir, const Vector3 &axle, int * suspID);
	ePhysError setSuspensionLimits(int id, float minDist, float maxDist);
	ePhysError setSuspensionPhysics(int id, float spring, float damping);
	ePhysError setWheelSize(int id, float radius, float width);
	ePhysError setWheelSteer(int id, float angle);
	ePhysError setWheelControls(int id, float accel, float brake);

	void setVehicleCenterOfGravity(float cx, float cy, float cz);
	ePhysError setVehicleMass(float mass);
	ePhysError setVehicleMomentOfIntertia(float mx, float my, float mz);

	ePhysError addWheelImpulse(int id, const Vector3& impulse);
	ePhysError addWheelForce(int id, const Vector3& force);

	ePhysError setBodyObject(int id);
	ePhysError setWheelObject(int suspId, int wheelObjectId, bool flip);
	ePhysError removeBodyObject(int id);
	ePhysError removeWheelObject(int id);

	virtual int baseClassID(){return classID;}
	//static const int classID='VHCL';
	static int classID;

protected:
	
	bool frozen;	
	bool drawDebugLines;

	float damping;
	float gravity; //Gravity can only exist vertically	

	ptrList<VehicleParticle,16> particleList;
	LinkedList_SortID<Suspension> suspensionList;

private:
	int chassisObjId;
	RigidBody body;
};


#endif