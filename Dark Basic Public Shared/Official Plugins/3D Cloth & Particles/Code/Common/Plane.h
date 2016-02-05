#ifndef DBPROPHYSICS_PLANE_H
#define DBPROPHYSICS_PLANE_H
#include "Vector.h"

class Plane
{
public:
	Plane():d(0.0f){}
	Plane(const Vector3 * norm,float dist, bool isNormalised=false){Set(norm,dist,isNormalised);}	
	
	void Init(){n.Init();d=0.0f;}

	void Set(const Vector3 * norm,float dist, bool isNormalised=false);
	void Set(float nx, float ny, float nz, float dist)
	{
		n.x=nx;
		n.y=ny;
		n.z=nz;
		n.Normalise();
		d=dist;
	}
	
	inline bool isPointInside(const Vector3 * p,float * dist) const
	{
		*dist = n.Dot(*p);

		*dist=(*dist)-d;

		if((*dist)<0) return true;

		return false;
	}
	
	bool isSphereInside(const Vector3 * p,float radius,float * dist) const;
	bool intersectPlane(const Plane& pln,Vector3 * o, Vector3 * dir);

	void rotateAndTrans(const Matrix& m);

	Vector3 n; //Plane normal		
	float d; //d is the distance to the origin of the plane along the normal	
};


#endif