#ifndef DBPROPHYSICSMASTER_EMITTER_BOX_H
#define DBPROPHYSICSMASTER_EMITTER_BOX_H


class Emitter_Box : public Emitter
{
public:
	Emitter_Box(int id, int num):Emitter(id,num),radialEmit(false){}

	virtual void initParticle(Particle * p);	
	
	virtual int childClassID(){return classID;}
	//static const int classID='BBOX';
	//int classID;

	static const int classID;

	ePhysError setBox(float sx,float sy,float sz);
	void setRadialEmission(bool state){radialEmit=state;}

private:
	bool radialEmit;
	Vector3 size,min;
};

EMITTER_PLUGIN_INTERFACE(Emitter_Box);


#endif