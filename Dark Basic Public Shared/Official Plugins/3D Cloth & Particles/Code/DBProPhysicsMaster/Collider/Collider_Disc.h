#ifndef DBPROPHYSICSMASTER_COLLIDER_DISC_H
#define DBPROPHYSICSMASTER_COLLIDER_DISC_H


class Collider_Disc : public Collider
{
public:
	Collider_Disc(int id);
	virtual ~Collider_Disc(){};
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
	//static const int classID='DISC';
	// int classID;

	static const int classID;

	ePhysError setDisc(float r);
private:
	float radiusSq;

	//Vars needed during updating
	Plane worldPlane_Upd;
};

COLLIDER_PLUGIN_INTERFACE(Collider_Disc);


#endif