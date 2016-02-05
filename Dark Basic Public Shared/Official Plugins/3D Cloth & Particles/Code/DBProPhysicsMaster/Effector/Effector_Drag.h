#ifndef DBPROPHYSICSMASTER_EFFECTOR_DRAG_H
#define DBPROPHYSICSMASTER_EFFECTOR_DRAG_H

class Effector_Drag : public Effector
{
public:
	Effector_Drag(int id);
	virtual ~Effector_Drag(){};
	
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
	//static const int classID='DRAG';
	 //int classID;

	static const int classID;

	void setAmount(float amt){drag=amt;}

private:
	float drag;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Drag);	



#endif