#ifndef DBPROPHYSICSMASTER_EFFECTOR_POINT_H
#define DBPROPHYSICSMASTER_EFFECTOR_POINT_H


typedef float (*FALLOFF_FUNC)(float,float);
float falloff_None(float dist,float clipVal);
float falloff_Inv(float dist,float clipVal);
float falloff_InvSquare(float dist,float clipVal);
enum eFalloff {FALLOFF_NONE,FALLOFF_INV,FALLOFF_INVSQUARE};

class Effector_Point : public Effector
{

public:
	Effector_Point(int id);
	virtual ~Effector_Point(){};

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
	//static const int classID='POIN';
	// int classID;

	static const int classID;

	void setAmount(float amt)
	{
		force=amt;
	}

	void setForceFalloff(eFalloff f)
	{
		falloffType=f;
		if(falloffType==FALLOFF_NONE) falloffFunc=falloff_None;
		else if(falloffType==FALLOFF_INV) falloffFunc=falloff_Inv;
		else if(falloffType==FALLOFF_INVSQUARE) falloffFunc=falloff_InvSquare;
	}
	
	void useKillRad(bool state){killAtRadius=state;}

	ePhysError setKillRad(float rad)
	{
		if(rad<0.0f) return PERR_BAD_PARAMETER;
		killRadius=rad;
		return PERR_AOK;
	}

private:
	float force;
	eFalloff falloffType;
	bool killAtRadius;
	float killRadius;
	FALLOFF_FUNC falloffFunc;

	//Vars needed during updating
	float clipVal_Upd;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Point);	


#endif