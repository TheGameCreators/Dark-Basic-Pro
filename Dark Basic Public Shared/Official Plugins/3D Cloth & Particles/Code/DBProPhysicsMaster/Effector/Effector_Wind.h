#ifndef DBPROPHYSICSMASTER_EFFECTOR_WIND_H
#define DBPROPHYSICSMASTER_EFFECTOR_WIND_H


class Effector_Wind : public Effector
{
public:
	Effector_Wind(int id);
	virtual ~Effector_Wind(){};	

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
	//static const int classID='WIND';
	 //int classID;

	static const int classID;

	void setAmount(const Vector3& amt)
	{
		wind=amt;
		windSizeSqr=wind.MagSqr();		
	}
	
private:
	Vector3 wind;
	float windSizeSqr;
};

EFFECTOR_PLUGIN_INTERFACE(Effector_Wind);	


#endif