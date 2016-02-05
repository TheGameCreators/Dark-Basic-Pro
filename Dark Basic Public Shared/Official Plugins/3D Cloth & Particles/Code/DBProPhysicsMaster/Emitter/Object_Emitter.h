#ifndef DBPROPHYSICSMASTER_EMITTERS_H
#define DBPROPHYSICSMASTER_EMITTERS_H


typedef void (*DLL_MAKEEMITTER_FP)(int, int);
typedef void (*DLL_DELETEEMITTER_FP)(int);
typedef void (*DLL_GETEMITTERDATA_FP)(int, BYTE**, DWORD*, int**);
typedef void (*DLL_UPDATEEMITTER_FP)(int);

//Define this to allow easy change of rand() resolution
#define RAND(val) ((val)*(rand()%10000)*0.0001f)

struct DBPro_Particle
{
	float x, y, z;
	DWORD dwDiffuse;	
};

class Particle
{
public:
	Particle():invMass(1),energy(0),frameCount(0),baseFrameCount(0),
	size(1.0f),color(0xffffffff),flags(0),colliderID(-1){}
	
	Vector3 pos;
	Vector3 lastPos;
	Vector3 force;
	float invMass;	// 1.0f/Mass
	int energy;		//gets decremented by varying amounts during lifetime, dies at <= zero
	int frameCount; //gets incremented by 1 each tick
	int baseFrameCount;	//Used to animate size & color after an event (i.e. collision)
	float size;
	unsigned int color;	
	char flags;		//Bit field for storing flags
	int colliderID;	//Stores last collider hit
};

//Constants for data bit field
#define PARTICLEDATA_COLLIDED (1<<0)
#define PARTICLEDATA_RESERVED1 (1<<1)
#define PARTICLEDATA_RESERVED2 (1<<2)
#define PARTICLEDATA_RESERVED3 (1<<3)
#define PARTICLEDATA_RESERVED4 (1<<4)
#define PARTICLEDATA_RESERVED5 (1<<5)
#define PARTICLEDATA_RESERVED6 (1<<6)
#define PARTICLEDATA_RESERVED7 (1<<7)

typedef LinkedList<Particle> ParticleList;
typedef Link<Particle>* ParticleLink;

struct ParticleDepth
{
	Particle * particle;
	float depth;
};

class Emitter : public PhysicsObject
{
	friend class PhysicsManager;

public:
	Emitter(int id, int num);
	~Emitter();

	void Start(){emitting=true;}		//Starts emission
	void Stop(){emitting=false;}		//Stops emission
	bool isEmitting(){return emitting;}
	void Freeze(){frozen=true;}		//Stops particles moving
	void unFreeze(){frozen=false;}	//Starts particles moving
	bool isFrozen(){return frozen;}

	virtual void onUpdate();	
	virtual void onUpdateGraphics();
	void onUpdateGraphicsZSort();

	virtual void initParticle(Particle * p)=0;

	void useZSorting(bool state);

	ePhysError setRate(float particlesPerSec);
	ePhysError setExplode(float param);
	void setGravity(const Vector3& g){gravity=g;}
	void setDamping(float d){damping=d;}

	ePhysError setParticleVelocity(float velocity, float percentVar);	
	ePhysError setParticleMass(float mass, float percentVar);
	ePhysError setParticleSize(float size, float percentVar);
	ePhysError setParticleLife(float secs, float percentVar);
	void setColor(DWORD col){color_Init=col;}

	void getParticleLists(ParticleList ** alive, ParticleList ** dead)
	{
		*alive = &particlesAlive;
		*dead = &particlesDead;
	}

	//static const int classID='EMIT';
	//int classID;
	static const int classID;
	virtual int baseClassID(){return classID;}
	//virtual int baseClassID(){return 0;}

	
protected:

	bool emitting;
	bool frozen;
	bool useZSort;

	float rate;			//Emission rate (particles per frame)	
	float explode;		//A value of 0 means a thin stream, 1 means sparkler effect

	float distPerFrame_Init;	//Initial distance moved of particle in 1st frame	
	float mass_Init;			//Initial particle mass
	float size_Init;			//Initial particle mass
	float energy_Init;			//Initial particle size
	DWORD color_Init;			//Initial particle color

	float distPerFrame_varAmt;	//Variance in particles initial velocity (1.0f = 100%)
	float mass_varAmt;			//Variance in particles initial 1.0f/mass
    float size_varAmt;			//Variance in particles initial size
	float energy_varAmt;		//Variance in particles initial energy

	int numAlive;				//Number of particles currently alive
	float fracWaiting;			//Fractional number of particle waiting to be released (always less than 1)	

	Vector3 gravity;
	float damping;

	int numParticles;
	ParticleList particlesDead;
	ParticleList particlesAlive;

	ParticleDepth * depthList;

private:
	static DLL_MAKEEMITTER_FP DLL_MakeEmitter;
	static DLL_DELETEEMITTER_FP DLL_DeleteEmitter;
	static DLL_GETEMITTERDATA_FP DLL_GetEmitterData;
	static DLL_UPDATEEMITTER_FP DLL_UpdateEmitter;
};


#endif