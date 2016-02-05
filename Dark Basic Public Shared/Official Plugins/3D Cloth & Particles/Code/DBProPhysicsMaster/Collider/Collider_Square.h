#ifndef DBPROPHYSICSMASTER_COLLIDER_SQUARE_H
#define DBPROPHYSICSMASTER_COLLIDER_SQUARE_H


class Collider_Square : public Collider
{
public:
	Collider_Square(int id);
	virtual ~Collider_Square(){};
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
	//static const int classID='SQAR';
	 //int classID;

	static const int classID;

	ePhysError setSquare(float w, float h);
private:
	float hwidth,hheight;

	//Vars needed during updating
	Plane worldPlane_Upd;
};

COLLIDER_PLUGIN_INTERFACE(Collider_Square);

#endif