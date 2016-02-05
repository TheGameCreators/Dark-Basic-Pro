#ifndef DBPROPHYSICSMASTER_EFFECTOR_SIZE_H
#define DBPROPHYSICSMASTER_EFFECTOR_SIZE_H


class EnvFloat
{
public:
	EnvFloat() {val=0.0f;}
	EnvFloat(float v){val=v;}
	EnvFloat(EnvFloat& v){val = v.val;}
	void interpolateLinear(const EnvFloat& dest, float t, EnvFloat* out)
	{
		out->val=val*(1-t) + dest.val*t;
	}
	float val;
};

class Effector_Size : public Effector
{
public:
	Effector_Size(int id);
	virtual ~Effector_Size(){};
	
	virtual bool canHandleParticles(){return true;}	

#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif

	// int classID;
	virtual int childClassID(){return classID;}
	//static const int classID='SIZE';
	
	static const int classID;

	int addKey(float time, float size);
	ePhysError deleteKeyAtTime(float time);
	ePhysError deleteKey(int n);
	int getNumKeys();
	ePhysError getKeyValue(int n, float * val);
	ePhysError getKeyTime(int n, float * time);

	bool activateOnCollide;

private:
	Envelope<EnvFloat> sizeEnvelope;
};


EFFECTOR_PLUGIN_INTERFACE(Effector_Size);	


#endif