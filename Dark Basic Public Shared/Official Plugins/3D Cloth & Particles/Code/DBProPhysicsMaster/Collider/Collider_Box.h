#ifndef DBPROPHYSICSMASTER_COLLIDER_BOX_H
#define DBPROPHYSICSMASTER_COLLIDER_BOX_H

class Collider_Box : public Collider
{
public:
	Collider_Box(int id);
	virtual ~Collider_Box(){};
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
	//static const int classID='CBOX';
	 //int classID;

	static const int classID;

	ePhysError setBox(float sx, float sy, float sz);
	bool HitBoundingBoxExterior(const Vector3& origin, const Vector3& dest, Vector3 * coord, Vector3 * norm);
	bool HitBoundingBoxInterior(const Vector3& origin, const Vector3& dest, Vector3 * coord, Vector3 * norm);
private:
	float radiusSq;				//Used for initial trivial rejection
	float invertedRadiusSq;		//Used for initial trivial rejection for when box is inverted
	Vector3 size,hsize;

	//Vars needed during updating
	Matrix invM_Upd;
};

COLLIDER_PLUGIN_INTERFACE(Collider_Box);


#endif