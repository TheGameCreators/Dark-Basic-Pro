#ifndef DBPROPHYSICSMASTER_COLLIDER_PLANE_H
#define DBPROPHYSICSMASTER_COLLIDER_PLANE_H

class Collider_Plane : public Collider
{
public:
	Collider_Plane(int id);
	virtual ~Collider_Plane(){};
	virtual void updateInit();
	
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
	//static const int classID='PLAN';
	 //int classID;

	static const int classID;

	//ePhysError setPlane(float nx, float ny, float nz, float dist);
private:
	Plane plane;

	//Vars needed during updating
	Plane worldPlane_Upd;
};

COLLIDER_PLUGIN_INTERFACE(Collider_Plane);


#endif