#ifndef DBPROPHYSICSMASTER_EMITTER_BASIC_H
#define DBPROPHYSICSMASTER_EMITTER_BASIC_H

class Emitter_Basic : public Emitter
{
public:
	Emitter_Basic(int id,int num):Emitter(id,num){}

	virtual void initParticle(Particle * p);	
	
	virtual int childClassID(){return classID;}

	//static const int classID='BEMT';
	//int classID;

	static const int classID;
};

EMITTER_PLUGIN_INTERFACE(Emitter_Basic);


#endif