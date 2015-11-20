//Collision classes and structure define
#include "col_local.h"

VECTOR::VECTOR(){x=0; y=0; z=0;}
VECTOR::VECTOR(float x, float y, float z){this->x=x; this->y=y; this->z=z;}

void VECTOR::set(float x, float y, float z){this->x=x; this->y=y; this->z=z;}
void VECTOR::get(float **floatvector){(*floatvector)[0]=x; (*floatvector)[1]=y; (*floatvector)[2]=z;}

VECTOR VECTOR::operator+(VECTOR const &V1)
{
	return VECTOR(V1.x+x,V1.y+y,V1.z+z);
}

VECTOR VECTOR::operator-(VECTOR const &V1)
{
	return VECTOR(x-V1.x,y-V1.y,z-V1.z);
}

VECTOR VECTOR::operator*(VECTOR const &V1)
{
	return VECTOR(x*V1.x,y*V1.y,z*V1.z);
}

VECTOR VECTOR::operator/(VECTOR const &V1)
{
	return VECTOR(x/V1.x,y/V1.y,z/V1.z);
}

VECTOR VECTOR::operator*(float const &c)
{
    return VECTOR(x*c,y*c,z*c); 
}

VECTOR VECTOR::operator*(double const &c)
{
    return VECTOR(x*c,y*c,z*c); 
}

VECTOR VECTOR::operator/(float const &c)
{
    float Invers=1.0f/c;//only divide one time!
    return VECTOR(x*Invers,y*Invers,z*Invers); 
}

float VECTOR::length()
{
    return (float)sqrt(x*x+y*y+z*z);
}

float VECTOR::squaredLength()
{
	return (float)x*x+y*y+z*z;
}

void VECTOR::SetLength(float c)
{
	float temp=length();
	x *= c/temp;
	y *= c/temp;
	z *= c/temp;
}

void VECTOR::normalize()
{
    float dist=1/length();//find the distance, and inverse it

    x *= dist;
    y *= dist;
    z *= dist;
}

float VECTOR::dot(VECTOR& V)
{
    return V.x*x+V.y*y+V.z*z;
}

VECTOR VECTOR::cross(VECTOR& V)
{
    return VECTOR((y*V.z-z*V.y),(z*V.x-x*V.z),(x*V.y-y*V.x));
}



PLANE::PLANE(VECTOR origin, VECTOR normal) 
{
	this->normal = normal;
	this->origin = origin;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
}

// Construct from triangle:
PLANE::PLANE(VECTOR p1,VECTOR p2,VECTOR p3)
{
    #ifndef __GNUC__
	normal = (p2-p1).cross(p3-p1);
	normal.normalize();
	origin = p1;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
	#endif
}

bool PLANE::isFrontFacingTo(VECTOR direction) 
{
	double dot = normal.dot(direction);
	return (dot <= 0);
}

double PLANE::signedDistanceTo(VECTOR point) 
{
	return (point.dot(normal)) + equation[3];
}
