#include "stdafx.h"
#include "..\Common\Matrix.h"
#include "..\Common\Plane.h"



void Plane::Set(const Vector3 * norm,float dist, bool isNormalised)
{
	n=*norm;
	if(!isNormalised) n.Normalise();
	d=dist;
}




bool Plane::isSphereInside(const Vector3 * p,float radius,float * dist) const
{
	static Vector3 rp;
	rp=*p-n*radius;
	*dist = n.Dot(rp);

	*dist=d-(*dist);

	if((*dist)>0) return true;

	return false;
}

bool Plane::intersectPlane(const Plane& pln,Vector3 * o, Vector3 * dir)
{
	*dir=pln.n*n;
	dir->Normalise();

	Vector3 orig=n*d;
	Vector3 rayDir=(*dir)*n;
	rayDir.Normalise();

	//Find intersection of ray with other plane
	
	float denom=pln.n.Dot(rayDir);

	if(denom<0.00001f && denom>-0.00001f) return false;

	float t=(pln.d-pln.n.Dot(orig))/denom;
	*o=orig+t*rayDir;
	return true;
}

void Plane::rotateAndTrans(const Matrix& m)
{
	m.rotateV3(&n);
	d+=m.pos().Dot(n);
}
