#ifndef DBPROPHYSICSMASTER_EFFECTOR_CHAOS_H
#define DBPROPHYSICSMASTER_EFFECTOR_CHAOS_H



class Effector_Chaos : public Effector
{
public:
	Effector_Chaos(int id);
	virtual ~Effector_Chaos(){};
	
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
	//static const int classID='KAOS';
	//int classID;

	static const int classID;

	void setAmount(float amt){chaos=amt;}

private:
	float chaos;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Chaos);	


#endif