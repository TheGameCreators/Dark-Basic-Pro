#ifndef DBPROPHYSICSMASTER_EFFECTOR_GRAVITY_H
#define DBPROPHYSICSMASTER_EFFECTOR_GRAVITY_H

class Effector_Gravity : public Effector
{
public:
	Effector_Gravity(int id);
	virtual ~Effector_Gravity(){};

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
	//static const int classID='GRAV';
	 //int classID;

	static const int classID;

	void setForce(float x, float y, float z);

private:
	Vector3 gravity;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Gravity);	


#endif