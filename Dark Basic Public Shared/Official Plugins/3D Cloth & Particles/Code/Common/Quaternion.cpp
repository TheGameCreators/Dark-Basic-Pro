#include "stdafx.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"


Quat::Quat(const Vector3& v)
{
	r=0;
	i=v.x;
	j=v.y;
	k=v.z;
}

Quat::Quat(const float angle, const Vector3& axis)
{
	//axis must be of unit length
	float angSin = (float)sinf(0.5f*angle);
	r = (float)cosf(0.5f*angle);
	i = angSin*axis.x;
	j = angSin*axis.y;
	k = angSin*axis.z;
}

Quat::Quat(const Matrix& m)
{
	Set(m);
}

void Quat::Set(const Matrix& m)
{
	//Note this routine was sourced from David Baraff's paper
	//'An Introduction to Physically Based Modelling'

    //The elements have been transposed to work with the matrix class properly

	float tr,s;
	
	tr = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if(tr>=0)
	{
		s=(float)sqrtf(tr+1);
		r=0.5f*s;
		s=0.5f/s;
		i=(m.m[1][2] - m.m[2][1])*s;
		j=(m.m[2][0] - m.m[0][2])*s;
		k=(m.m[0][1] - m.m[1][0])*s;
	}
	else
	{
		int a=0;

		if(m.m[1][1]>m.m[0][0]) a=1;
		if(m.m[2][2]>m.m[a][a]) a=2;

		switch(a)
		{
		case 0:
			s=(float)sqrtf((m.m[0][0] - (m.m[1][1] + m.m[2][2]))+1);
			i=0.5f*s;
			s=0.5f/s;
			j=(m.m[1][0] + m.m[0][1])*s;
			k=(m.m[0][2] + m.m[2][0])*s;
			r=(m.m[1][2] - m.m[2][1])*s;
			break;
		case 1:
			s=(float)sqrtf((m.m[1][1] - (m.m[2][2] + m.m[0][0]))+1);
			j=0.5f*s;
			s=0.5f/s;
			k=(m.m[2][1] + m.m[1][2])*s;
			i=(m.m[1][0] + m.m[0][1])*s;
			r=(m.m[2][0] - m.m[0][2])*s;
			break;
		case 2:
			s=(float)sqrtf((m.m[2][2] - (m.m[0][0] + m.m[1][1]))+1);
			k=0.5f*s;
			s=0.5f/s;
			i=(m.m[0][2] + m.m[2][0])*s;
			j=(m.m[2][1] + m.m[1][2])*s;
			r=(m.m[0][1] - m.m[1][0])*s;
		}
	}
}


const Quat& Quat::slerpTo(const Quat& q,float t)
{
    float ang = (float)acosf((*this).Dot(q));

	if(fabsf(ang)<0.001f) return *this;
    
    float invSin = 1.0f/(float)sinf(ang);
    float c0 = (float)sinf((1.0f-t)*ang)*invSin;
    float c1 = (float)sinf(t*ang)*invSin;
    	
	(*this)*=c0;
	(*this)+=c1*q;
	return *this;
}




void Quat::rotateV3(Vector3 * v) const
{
	Matrix mat(*this);
	mat.rotateV3(v);	
}

void Quat::rotateV3(const Vector3& invector,Vector3 * outvector) const
{
	Matrix mat(*this);
	mat.rotateV3(invector,outvector);
}

Quat operator* (float a, const Quat& q)
{
	return Quat(a*q.r,a*q.i,a*q.j,a*q.k);
}


Quat Slerp(const Quat& p,const Quat& q,float t)
{    
    float ang = (float)acosf(p.Dot(q));

	if(fabsf(ang)<0.001f) return p;
    
    float invSin = 1.0f/(float)sinf(ang);
    float c0 = (float)sinf((1.0f-t)*ang)*invSin;
    float c1 = (float)sinf(t*ang)*invSin;
    return c0*p + c1*q;
}
