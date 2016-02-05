#include "stdafx.h"
#include "Vector.h"


Vector2::Vector2(Vector3& vec3) //made explicit for safety
{
	x=vec3.x;
	y=vec3.y;
}

Vector2& Vector2::operator = (Vector3& rhs)
{
	//Won't test for this - might cause slow down
	//if(&rhs==this) return *this;
	x=rhs.x;
	y=rhs.y;
	return *this;
}

