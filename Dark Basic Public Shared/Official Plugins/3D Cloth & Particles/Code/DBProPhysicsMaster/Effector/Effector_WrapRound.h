#ifndef DBPROPHYSICSMASTER_EFFECTOR_WRAPROUND_H
#define DBPROPHYSICSMASTER_EFFECTOR_WRAPROUND_H

class Effector_WrapRound : public Effector
{
public:
	Effector_WrapRound(int id);
	virtual ~Effector_WrapRound(){};
	virtual void updateInit();
	
	virtual bool canHandleParticles(){return true;}

#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif

	virtual int childClassID(){return classID;}
	//static const int classID='WRAP';
	 //int classID;

	static const int classID;

	void setRegion(float sx, float sy, float sz);

private:
	Vector3 region;

	//Vars needed during updating
	Vector3 minb_Upd,maxb_Upd;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_WrapRound);	



#endif