#ifndef DBPROPHYSICSMASTER_CLOTH_H
#define DBPROPHYSICSMASTER_CLOTH_H


void onAttachedObjectDeletedClothCallback(int id, int userData);


class ClothParticle
{
public:
	ClothParticle():segmentWeighting(0.0f),triWeighting(0.0f),fixed(false),
		objectID(-1),DBProPosition(0),u(0.0f),v(0.0f){}

	bool saveParams(FILE * file);
	bool loadParams(FILE * file);
	
	void Set(float x, float y, float z) {pos.Set(x,y,z);lastPos.Set(x,y,z);}

	Vector3 pos;
	Vector3 lastPos;
	Vector3 force;
	Vector3 delta;
	Vector3 normal;
	float segmentWeighting;	//Stores 1/numSegs the particle belongs to
	float triWeighting; //Stores 1/numTriangles the particle belongs to
	float u,v;
	bool fixed;
	int objectID;
	Vector3 fixedPos;
	sPositionData * DBProPosition;
};

class ClothSegment	//Represents a quad or triangular section of cloth
{
public:
	bool saveParams(FILE * file, ClothParticle * base);
	bool loadParams(FILE * file, ClothParticle * base);

	ClothSegment():numParticles(4){}

	void InitPointers()
	{
		particle[0]=particle[1]=particle[2]=particle[3]=0;
	}

	void Set(ClothParticle * p0, ClothParticle * p1, ClothParticle * p2, ClothParticle * p3);

	void Relax();

	ClothParticle * particle[4];
	float distSq[6];
	int numParticles;
};


class Cloth : public PhysicsObject
{
	friend class PhysicsManager;		

public:
	Cloth(int id);
	virtual ~Cloth();

	virtual void onUpdate();	
	virtual void onUpdateGraphics();
	void Freeze(){frozen=true;}		//Stops particles moving
	void unFreeze(){frozen=false;}	//Starts particles moving
	bool isFrozen(){return frozen;}

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


	ePhysError setClothElasticity(float elast);
	ePhysError setClothMass(float mass);

	ePhysError saveClothState(const char * filename);
	ePhysError loadClothState(const char * filename, bool useVelocities);

	ePhysError Relax();

	int getNumParticles(){return numParticles;}
	int getNumSegments(){return numSegs;}
	float getParticleInvMass(){return particleInvMass;}

	ePhysError TransformUVs(float scaleu, float scalev, float moveu, float movev, int mode);
	ePhysError ResetUVs(int mode);

	bool saveParams(FILE * file);
	bool loadParams(FILE * file);

	void getParticles(ClothParticle ** prt)
	{
		*prt = particle;
	}
	void getSegments(ClothSegment ** seg)
	{
		*seg = segment;
	}

	virtual int baseClassID(){return classID;}
	//static const int classID='CLTH';
	// int classID;

	static const int classID;

protected:
	void allocateGraphicsAndInit(bool calcUVs=true);

	bool hasMesh;

	bool frozen;

	ClothSegment * segment;
	ClothParticle * particle;
	
	bool doubleSided;
	int numSegs;		//Can be a quad or a triangle
	int numParticles;
	int numTriangles;	//Stores the number of triangles for mesh allocation purposes

	float damping;
	Vector3 gravity;
	
	//Values used to interface with programmer - they simplify the concepts
	float elastic_USER;	
	float mass_USER;
	//Values derived from above 'USER' values
	float particleInvMass;
	float clothStiffness;

	float fsu,fsv,fmu,fmv;
	float bsu,bsv,bmu,bmv;

	TypeList<int,16> refObjectID;

};


#endif