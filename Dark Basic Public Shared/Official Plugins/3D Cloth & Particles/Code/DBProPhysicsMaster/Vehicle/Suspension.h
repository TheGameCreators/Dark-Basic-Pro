#ifndef DBPROPHYSICSMASTER_SUSPENSION_H
#define DBPROPHYSICSMASTER_SUSPENSION_H


class Suspension
{
public:

	Suspension(class Vehicle * _parent,const Vector3 &_pos, const Vector3 &_up, const Vector3 &_fwd);
	
	void Update();
	void updateGraphics();

	Vehicle * parent;	

	bool isOnGround;	

	float spring;
	float damping;
	float dist;
	float lastDist;
	float vel;
	float minDist;
	float maxDist;

	float pushDist;

	float springForce;

	bool isFlipped;
	float wheelRotVel;
	float wheelRotate;
	float wheelSteer;
	float radius;
	float width;
	int wheelObjId;

	float weightShare;

	float groundHeight;
	Vector3 wheelBase,groundNorm;
	Matrix local,world,wheel;

	Vector3 velPar,velPerp,wheelDir;
	float perpFriction;
	float brake;
	float accel;
};


#endif