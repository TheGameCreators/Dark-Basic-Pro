#ifndef DBPROPHYSICSMASTER_RAGDOLL_H
#define DBPROPHYSICSMASTER_RAGDOLL_H


void onAttachedObjectDeletedRagdollCallback(int id, int userData);

enum eRagdollLinkType {RAGLINK_SOLID, RAGLINK_MINIMUM, RAGLINK_MAXIMUM, RAGLINK_MINMAX, RAGLINK_DISTDIFF};


class RagdollParticle
{
public:
	RagdollParticle():fixed(false),objectID(-1),DBProPosition(0){}

	//bool saveParams(FILE * file);
	//bool loadParams(FILE * file);
	
	void Set(float x, float y, float z, float colRadius, int _type)
	{
		type = _type;
		pos.Set(x,y,z);
		lastPos.Set(x,y,z);
		colRadiusSq=colRadius*colRadius;
	}

	Vector3 pos;
	Vector3 lastPos;
	Vector3 force;
	Vector3 delta;
	bool fixed;

	int type;					//User definable ID for which part of the body we are
	float colRadiusSq;			//Radius for collision

	int objectID;
	Vector3 fixedPos;	
	sPositionData * DBProPosition;
};


class RagdollLink
{
public:
	RagdollLink():p1(0),p2(0),p3(0),distSq(0),maxSq(0),type(RAGLINK_SOLID){}
	void Set(RagdollParticle * _p1, RagdollParticle * _p2)
	{
		p1=_p1;
		p2=_p2;
		type=RAGLINK_SOLID;
		distSq=((p1->pos)-(p2->pos)).MagSqr();
	}

	void Set(RagdollParticle * _p1, RagdollParticle * _p2, eRagdollLinkType _type, float _distSq, float _maxSq=0)
	{
		p1=_p1;
		p2=_p2;
		type=_type;
		distSq=_distSq;
		maxSq=_maxSq;
	}

	void Set(RagdollParticle * _p1, RagdollParticle * _p2, RagdollParticle * _p3, float dist)
	{
		p1=_p1;
		p2=_p2;
		p3=_p3; //Uses this particle as a gauge. p2 never comes closer than (p3-p1)+dist
		type=RAGLINK_DISTDIFF;
		distSq=dist;
	}

	RagdollParticle * p1;
	RagdollParticle * p2;
	RagdollParticle * p3;
	float distSq;
	float maxSq;
	eRagdollLinkType type;
};

class RagdollRotConstraint
{
public:
	RagdollRotConstraint():p1(0),p2(0),p3(0),p4(0),scale(1.0f){}
	void Set(RagdollParticle * _p1, RagdollParticle * _p2, RagdollParticle * _p3, RagdollParticle * _p4, float scaleStrength)
	{
		p1=_p1;
		p2=_p2;
		p3=_p3;
		p4=_p4;
		scale = scaleStrength;
	}
	void Evaluate();

	float scale;
	RagdollParticle *p1,*p2,*p3,*p4;
};



class Ragdoll : public PhysicsObject
{
	friend class PhysicsManager;

public:
	Ragdoll(int id);
	virtual ~Ragdoll();

	virtual void onUpdate()=0;	
	virtual void onUpdateGraphics()=0;
	void Freeze(){frozen=true;}		//Stops particles moving
	void unFreeze(){frozen=false;}	//Starts particles moving
	bool isFrozen(){return frozen;}
	void setDebugLines(bool state){drawDebugLines=state;}

	void setGravity(const Vector3& g){gravity=g;}
	void setDamping(float d){damping=d;}
	ePhysError fixPoint(int pntNo, bool fixed);
	ePhysError fixPoints(float px, float py, float pz, float radius, bool fixed);
	ePhysError fixPoints(float px, float py, float pz, float sx, float sy, float sz, bool fixed);
	ePhysError fixNearestPoint(float px, float py, float pz, bool fixed);

	ePhysError fixPointToObject(int pntNo, int objectID, Vector3 * pos);

	ePhysError freeAllPoints();
	ePhysError freePointsOnObject(int objectID);
	ePhysError freePointsOnAllObjects();

	ePhysError setRagdollMass(float mass);

	ePhysError applyForce(int pointId, float fx, float fy, float fz);
	ePhysError applyImpulse(int pointId, float ix, float iy, float iz);

	virtual ePhysError fixObjectToRagdoll(int objectId, int partNo)=0;
	virtual ePhysError setRagdollObjectOffset(int partNo, float ox, float oy, float oz)=0;
	virtual ePhysError removeRagdollObjectPart(int id)=0;


	//ePhysError saveRagdollState(const char * filename);
	//ePhysError loadRagdollState(const char * filename, bool useVelocities);

	int getNumParticles(){return numParticles;}
	float getParticleInvMass(){return particleInvMass;}

	//bool saveParams(FILE * file);
	//bool loadParams(FILE * file);

	void getParticles(RagdollParticle ** prt)
	{
		*prt = particle;
	}

	virtual int baseClassID(){return classID;}
	//static const int classID='RAGD';
	static int classID;

protected:
	bool frozen;
	bool drawDebugLines;

	RagdollParticle * particle;
	RagdollLink * link;
	RagdollRotConstraint * rotConstr;

	int numParticles;
	int numLinks;
	int numRotConstraints;

	float particleInvMass;

	float damping;
	Vector3 gravity;
	
	TypeList<int,16> refObjectID;
};






#endif