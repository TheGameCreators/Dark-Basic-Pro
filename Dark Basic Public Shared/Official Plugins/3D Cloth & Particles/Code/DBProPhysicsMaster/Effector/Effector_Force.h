#ifndef DBPROPHYSICSMASTER_EFFECTOR_FORCE_H
#define DBPROPHYSICSMASTER_EFFECTOR_FORCE_H


class Effector_Force : public Effector
{
public:
	Effector_Force(int id);
	virtual ~Effector_Force(){};
	
	virtual bool canHandleParticles(){return true;}
	virtual bool canHandleCloth(){return true;}
	virtual bool canHandleRagdoll(){return true;}

#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif
#ifdef USINGMODULE_C
	virtual void onUpdateCloth();
#endif
#ifdef USINGMODULE_R
	virtual void onUpdateRagdoll();
#endif

	virtual int childClassID(){return classID;}
	//static const int classID='FRCE';
	 //int classID;

	 static const int classID;

	void setAmount(const Vector3& amt)
	{
		force=amt;
	}
	
private:
	Vector3 force;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Force);	


#endif