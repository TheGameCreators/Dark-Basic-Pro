#ifndef DBPROPHYSICSMASTER_EMITTER_SPHERE_H
#define DBPROPHYSICSMASTER_EMITTER_SPHERE_H


class Emitter_Sphere : public Emitter
{
public:
 	Emitter_Sphere(int id, int num):Emitter(id,num),radialEmit(false),spawnRadius(0.0f),spawnRadiusVar(1.0f),useHemisph(false){}

	virtual void initParticle(Particle * p);	
	
	virtual int childClassID(){return classID;}
	//static const int classID='SPHR';
	 //int classID;

	static const int classID;

	ePhysError setRadius(float minRad, float maxRad);
	void useHemisphere(bool state){useHemisph=state;}
	void setRadialEmission(bool state){radialEmit=state;}

private:
	bool radialEmit;
	float spawnRadius;
	float spawnRadiusVar;
	bool useHemisph;
};

EMITTER_PLUGIN_INTERFACE(Emitter_Sphere);


#endif