#ifndef DBPROPHYSICSMASTER_EFFECTOR_VORTEX_H
#define DBPROPHYSICSMASTER_EFFECTOR_VORTEX_H


class Effector_Vortex : public Effector
{
public:
	Effector_Vortex(int id);
	virtual ~Effector_Vortex(){};	
	
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
	//static const int classID='VRTX';
	 //int classID;

	static const int classID;

	void setAmount(float amt){vortex=amt;}

private:
	float vortex;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Vortex);	


#endif