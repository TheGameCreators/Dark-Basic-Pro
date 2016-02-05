#ifndef DBPROPHYSICS_BOUNDINGBOX_H
#define DBPROPHYSICS_BOUNDINGBOX_H
#include "Vector.h"
#include "Matrix.h"


class AABoundingBox
{
public:
	Vector3 min, max;
};

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Matrix& parent):m(parent){}


	Vector3 min, max;

private:
	const Matrix& m;
	Matrix __m;
};


#endif