#ifndef DBPROPHYSICSMASTER_EFFECTOR_DAMPING_H
#define DBPROPHYSICSMASTER_EFFECTOR_DAMPING_H

class Effector_Damping : public Effector
{
public:
	Effector_Damping(int id);
	virtual ~Effector_Damping(){};

	virtual bool canHandleParticles(){return true;}
	virtual bool canHandleCloth(){return true;}
	virtual bool canHandleRagdoll(){return true;}
	virtual bool canHandleVehicle(){return true;}

	virtual void updateParticles();
	virtual void updateCloth();
	virtual void updateRagdoll();
	virtual void updateVehicle();

#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif
#ifdef USINGMODULE_C
	virtual void onUpdateCloth();
#endif
#ifdef USINGMODULE_R
	virtual void onUpdateRagdoll();
#endif
#ifdef USINGMODULE_V
	virtual void onUpdateVehicle();
#endif

	virtual int childClassID(){return classID;}
	//static const int classID='DAMP';
	 //int classID;

	static const int classID;

	void setAmount(float dmp);

private:
	float damping;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Damping);	


#endif