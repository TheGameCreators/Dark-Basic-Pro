#ifndef DBPROPHYSICSMASTER_EFFECTOR_FLOCK_H
#define DBPROPHYSICSMASTER_EFFECTOR_FLOCK_H


class Effector_Flock : public Effector
{
public:
	Effector_Flock(int id);
	virtual ~Effector_Flock(){};
	
	virtual bool canHandleParticles(){return true;}

	virtual void updateParticles();
#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif

	virtual int childClassID(){return classID;}
	//static const int classID='FLCK';
	 //int classID;

	static const int classID;

	void setAmount(float amt){flock=amt;}
	
	void useEffPos(bool state){useEffectorPosition=state;}

private:
	float flock;
	bool useEffectorPosition;	//Uses the effector position instead of average particle position
	bool lastMode;	//Just stores the previous value of useEffectorPosition
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Flock);	


#endif