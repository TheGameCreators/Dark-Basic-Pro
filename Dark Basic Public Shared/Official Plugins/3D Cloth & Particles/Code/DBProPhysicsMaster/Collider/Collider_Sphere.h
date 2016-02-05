#ifndef DBPROPHYSICSMASTER_COLLIDER_SPHERE_H
#define DBPROPHYSICSMASTER_COLLIDER_SPHERE_H

class Collider_Sphere : public Collider
{
public:
	Collider_Sphere(int id);
	virtual ~Collider_Sphere(){};
	
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
	//static const int classID='SPHR';
	 //int classID;

	static const int classID;

	ePhysError setRadius(float rad);
private:
	float radius;
};

COLLIDER_PLUGIN_INTERFACE(Collider_Sphere);


#endif