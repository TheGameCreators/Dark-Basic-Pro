#ifndef DBPROPHYSICSMASTER_EFFECTOR_COLOR_H
#define DBPROPHYSICSMASTER_EFFECTOR_COLOR_H

class EnvColor
{
public:
	EnvColor() {r=g=b=a=0;}
	EnvColor(int _r, int _g, int _b, int _a){r=_r;g=_g;b=_b;a=_a;}
	EnvColor(EnvColor& v){r=v.r;g=v.g;b=v.b;a=v.a;}
	void interpolateLinear(const EnvColor& dest, float t, EnvColor* out)
	{
		out->r=(int)floor(r*(1-t) + dest.r*t);
		out->g=(int)floor(g*(1-t) + dest.g*t);
		out->b=(int)floor(b*(1-t) + dest.b*t);
		out->a=(int)floor(a*(1-t) + dest.a*t);
	}

	int r,g,b,a;
};

class Effector_Color : public Effector
{
public:
	Effector_Color(int id);
	virtual ~Effector_Color(){};
	
	virtual bool canHandleParticles(){return true;}	

#ifdef USINGMODULE_P
	virtual void onUpdateParticles();
#endif

	virtual int childClassID(){return classID;}
	//static const int classID='SIZE';
	// int classID;

	static const int classID;

	int addKey(float time, int r, int g, int b, int a);
	ePhysError deleteKeyAtTime(float time);
	ePhysError deleteKey(int n);
	int getNumKeys();
	ePhysError getKeyRedValue(int n, int * val);
	ePhysError getKeyGreenValue(int n, int * val);
	ePhysError getKeyBlueValue(int n, int * val);
	ePhysError getKeyAlphaValue(int n, int * val);
	ePhysError getKeyTime(int n, float * time);

	bool activateOnCollide;

private:
	Envelope<EnvColor> colorEnvelope;
};


EFFECTOR_PLUGIN_INTERFACE(Effector_Color);	


#endif