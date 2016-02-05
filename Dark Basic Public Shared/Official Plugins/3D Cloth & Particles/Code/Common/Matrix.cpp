#include "stdafx.h"
#include "assert.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"



//Slower and bulkier code goes here rather than be placed inline in the header

void Matrix::Set(
	float v00, float v01, float v02, float v03,
	float v10, float v11, float v12, float v13,
	float v20, float v21, float v22, float v23,
	float v30, float v31, float v32, float v33)
{
	e00=v00; e01=v01; e02=v02; e03=v03; 
	e10=v10; e11=v11; e12=v12; e13=v13; 
	e20=v20; e21=v21; e22=v22; e23=v23; 
	e30=v30; e31=v31; e32=v32; e33=v33; 
}

void Matrix::Set3x3(
	float v00, float v01, float v02,
	float v10, float v11, float v12,
	float v20, float v21, float v22)
{
	e00=v00; e01=v01; e02=v02;
	e10=v10; e11=v11; e12=v12;
	e20=v20; e21=v21; e22=v22;
}

void Matrix::Set3x3(const Vector3& v0,const Vector3& v1,const Vector3& v2)
{
	e00=v0.x; e01=v0.y; e02=v0.z;
	e10=v1.x; e11=v1.y; e12=v1.z;
	e20=v2.x; e21=v2.y; e22=v2.z;
}
 
void Matrix::makeRotationX(float x)
{
	float sn=sinf(x);
	float cs=cosf(x);

	Set(1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	cs,		sn,	0.0f,
		0.0f,	-sn,		cs,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

void Matrix::makeRotationY(float y)
{
	float sn=sinf(y);
	float cs=cosf(y);

	Set(cs,		0.0f,	-sn,		0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		sn,	0.0f,	cs,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

void Matrix::makeRotationZ(float z)
{
	float sn=sinf(z);
	float cs=cosf(z);

	Set(cs,		sn,	0.0f,	0.0f,
		-sn,		cs,		0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

 
void Matrix::make3x3RotationX(float x)
{
	float sn=sinf(x);
	float cs=cosf(x);

	Set3x3(1.0f,	0.0f,	0.0f,
			0.0f,	cs,		sn,
			0.0f,	-sn,	cs);
}

void Matrix::make3x3RotationY(float y)
{
	float sn=sinf(y);
	float cs=cosf(y);

	Set3x3(cs,		0.0f,	-sn,
			0.0f,	1.0f,	0.0f,
			sn,		0.0f,	cs	);
}

void Matrix::make3x3RotationZ(float z)
{
	float sn=sinf(z);
	float cs=cosf(z);

	Set3x3(cs,		sn,		0.0f,
			-sn,	cs,		0.0f,
			0.0f,	0.0f,	1.0f);
}



void Matrix::makeRotationMatrix(float rx, float ry, float rz,eRotation order)
{
	float a = cosf(rx);
    float b = sinf(rx);
	float c = cosf(ry);
    float d = sinf(ry);
	float e = cosf(rz);
    float f = sinf(rz);


	//Note: Converted routines to left handed rotations

	switch(order)
	{
	case ROTATE_YXZ:
		{
			float bf = b*f, be = b*e;

			Set(
				c*e - d*bf,		c*f + d*be,		-d*a,	0,
				-a*f,			a*e,			b,		0,
				d*e + c*bf,		d*f - c*be,		c*a,	0,
				0,				0,				0,		1);

			break;
		}
	case ROTATE_XYZ:
		{
			float de = d*e, df = d*f;
			Set(
				c*e,			c*f,			-d,		0,
				b*de-a*f,		b*df+a*e,		b*c,	0,
				a*de+b*f,		a*df-b*e,		a*c,	0,
				0,				0,				0,		1);
			break;
		}
	case ROTATE_XZY:
		{
			float cf = c*f, df = d*f;
			Set(
				c*e,		f,				-d*e,		0,
				-a*cf+b*d,	a*e,			a*df+b*c,	0,
				b*cf+a*d,	-b*e,			-b*df+a*c,	0,
				0,			0,				0,			1);

			break;
		}
	case ROTATE_YZX:
		{
			float cf = c*f, df = d*f;
			Set(
				c*e,		a*cf+b*d,		b*cf-a*d,	0,
				-f,			a*e,			b*e,		0,
				d*e,		a*df-b*c,		b*df+a*c,	0,
				0,			0,				0,			1);
			break;
		}
	case ROTATE_ZXY:
		{
			float bd = b*d, bc = b*c;
			Set(
				c*e+bd*f,	a*f,			-d*e+bc*f,	0,
				-c*f+bd*e,	a*e,			d*f+bc*e,	0,
				a*d,		-b,				a*c,		0,
				0,			0,				0,			1);
			break;
		}
	case ROTATE_ZYX:
		{
			float de = d*e, df = d*f;
			Set(
				c*e,		a*f+b*de,		b*f-a*de,	0,
				-c*f,		a*e-b*df,		b*e+a*df,	0,
				d,			-b*c,			a*c,		0,
				0,			0,				0,			1);
			break;
		}
	}
}


void Matrix::make3x3RotationMatrix(float rx, float ry, float rz,eRotation order)
{
	float a = cosf(rx);
    float b = sinf(rx);
	float c = cosf(ry);
    float d = sinf(ry);
	float e = cosf(rz);
    float f = sinf(rz);


	//Note: Converted routines to left handed rotations

	switch(order)
	{
	case ROTATE_YXZ:
		{
			float bf = b*f, be = b*e;

			Set3x3(
				c*e - d*bf,		c*f + d*be,		-d*a,
				-a*f,			a*e,			b,
				d*e + c*bf,		d*f - c*be,		c*a);

			break;
		}
	case ROTATE_XYZ:
		{
			float de = d*e, df = d*f;
			Set3x3(
				c*e,			c*f,			-d,
				b*de-a*f,		b*df+a*e,		b*c,
				a*de+b*f,		a*df-b*e,		a*c);
			break;
		}
	case ROTATE_XZY:
		{
			float cf = c*f, df = d*f;
			Set3x3(
				c*e,		f,				-d*e,
				-a*cf+b*d,	a*e,			a*df+b*c,
				b*cf+a*d,	-b*e,			-b*df+a*c);

			break;
		}
	case ROTATE_YZX:
		{
			float cf = c*f, df = d*f;
			Set3x3(
				c*e,		a*cf+b*d,		b*cf-a*d,
				-f,			a*e,			b*e,
				d*e,		a*df-b*c,		b*df+a*c);
			break;
		}
	case ROTATE_ZXY:
		{
			float bd = b*d, bc = b*c;
			Set3x3(
				c*e+bd*f,	a*f,			-d*e+bc*f,
				-c*f+bd*e,	a*e,			d*f+bc*e,
				a*d,		-b,				a*c);
			break;
		}
	case ROTATE_ZYX:
		{
			float de = d*e, df = d*f;
			Set3x3(
				c*e,		a*f+b*de,		b*f-a*de,
				-c*f,		a*e-b*df,		b*e+a*df,
				d,			-b*c,			a*c);
			break;
		}
	}
}


void Matrix::rotateV3(const Vector3& invector,Vector3 * outvector) const
{
	outvector->x = e00 * invector.x + e10 * invector.y + e20 * invector.z;
	outvector->y = e01 * invector.x + e11 * invector.y + e21 * invector.z;
	outvector->z = e02 * invector.x + e12 * invector.y + e22 * invector.z;
}

void Matrix::rotateV3(Vector3 * v) const
{
	float x,y,z;

	x = e00 * v->x + e10 * v->y + e20 * v->z;
	y = e01 * v->x + e11 * v->y + e21 * v->z;
	z = e02 * v->x + e12 * v->y + e22 * v->z;

	v->x=x;
	v->y=y;
	v->z=z;
}

void Matrix::rotateAndTransV3(const Vector3& invector,Vector3 * outvector) const
{
	outvector->x = e00 * invector.x + e10 * invector.y + e20 * invector.z + e30;
	outvector->y = e01 * invector.x + e11 * invector.y + e21 * invector.z + e31;
	outvector->z = e02 * invector.x + e12 * invector.y + e22 * invector.z + e32;
}

void Matrix::rotateAndTransV3(Vector3 * v) const
{
	float x,y,z;

	x = e00 * v->x + e10 * v->y + e20 * v->z + e30;
	y = e01 * v->x + e11 * v->y + e21 * v->z + e31;
	z = e02 * v->x + e12 * v->y + e22 * v->z + e32;

	v->x=x;
	v->y=y;
	v->z=z;	
}


void Matrix::Inverse4x4This()
{
    float tmp[12]; /* temp array for pairs */
    float *src;
    float dst[16];
    float det; /* determinant */

    TransposeThis();

    src = (float*) this;

    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];

    /* calculate first 8 elements (cofactors) */
    dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
    dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
    dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
    dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
    dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
    dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
    dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
    dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
    dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
    dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
    dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
    dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
    dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
    dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
    dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
    dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src[2]*src[7];
    tmp[1] = src[3]*src[6];
    tmp[2] = src[1]*src[7];
    tmp[3] = src[3]*src[5];
    tmp[4] = src[1]*src[6];
    tmp[5] = src[2]*src[5];
    tmp[6] = src[0]*src[7];
    tmp[7] = src[3]*src[4];
    tmp[8] = src[0]*src[6];
    tmp[9] = src[2]*src[4];
    tmp[10] = src[0]*src[5];
    tmp[11] = src[1]*src[4];

    /* calculate second 8 elements (cofactors) */
    dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
    dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
    dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
    dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
    dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
    dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
    dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
    dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
    dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
    dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
    dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
    dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
    dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
    dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
    dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
    dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

    /* calculate determinant */
    det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

    assert(det>0.00001 || det<-0.00001); //check not zero

    /* calculate matrix inverse */
    det = 1/det;

    for ( int j = 0; j < 16; j++)
    {
    	dst[j] *= det;
    }

    Set(dst[0],dst[1],dst[2],dst[3],
    	dst[4],dst[5],dst[6],dst[7],
        dst[8],dst[9],dst[10],dst[11],
        dst[12],dst[13],dst[14],dst[15]);
}


void Matrix::Inverse3x3This()
{
    Matrix tmp;

    tmp.m[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
    tmp.m[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
    tmp.m[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
    tmp.m[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
    tmp.m[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
    tmp.m[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
    tmp.m[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
    tmp.m[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
    tmp.m[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

    float det = m[0][0]*tmp.m[0][0] + m[0][1]*tmp.m[1][0]+ m[0][2]*tmp.m[2][0];

    assert(det>0.00001 || det<-0.00001); //check not zero

    float invDet = 1.0f/det;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
            tmp.m[row][col] *= invDet;
    }
	this->Set3x3(tmp);
}

void Matrix::setUnitX(const Vector3& v)
{
	e00=v.x;e01=v.y;e02=v.z;e03=0.0f;
}
void Matrix::setUnitY(const Vector3& v)
{
	e10=v.x;e11=v.y;e12=v.z;e13=0.0f;
}
void Matrix::setUnitZ(const Vector3& v)
{
	e20=v.x;e21=v.y;e22=v.z;e23=0.0f;
}

void Matrix::makeRotationAxisAngle(const Vector3& axis, float angle)
{	
    float s = sinf(angle);
	float c = cosf(angle);
	float t = 1.0f - c;
	const float& x = axis.x;
	const float& y = axis.y;
	const float& z = axis.z;

    Set( t*x*x + c,		t*x*y + s*z,	t*x*z - s*y,	0,
		t*x*y - s*z,	t*y*y + c,		t*y*z + s*x,	0,
		t*x*z + s*y,	t*y*z - s*x,	t*z*z + c,		0,
		0,				0,				0,				1);
}

void Matrix::make3x3RotationAxisAngle(const Vector3& axis, float angle)
{	
    float s = sinf(angle);
	float c = cosf(angle);
	float t = 1.0f - c;
	const float& x = axis.x;
	const float& y = axis.y;
	const float& z = axis.z;

    Set3x3( t*x*x + c,		t*x*y + s*z,	t*x*z - s*y,
			t*x*y - s*z,	t*y*y + c,		t*y*z + s*x,
			t*x*z + s*y,	t*y*z - s*x,	t*z*z + c);
}

void Matrix::makeViewMatrix(const Vector3& camera, const Vector3& target, const Vector3& wup)
{
	Init();

	Vector3 dir = target - camera;
	dir.Normalise();

	Vector3 right = wup * dir;
	Vector3 up = dir * right;
	right.Normalise();
	up.Normalise();

	e00	= right.x;
	e10	= right.y;
	e20	= right.z;
	e01	= up.x;
	e11	= up.y;
	e21	= up.z;
	e02	= dir.x;
	e12	= dir.y;
	e22	= dir.z;
	e30	= -right.Dot(camera);
	e31	= -up.Dot(camera);
	e32	= -dir.Dot(camera);
	e33 = 0;

}

Matrix::Matrix(const Quat& q)
{
	//Note this routine was sourced from David Baraff's paper
	//'An Introduction to Physically Based Modelling'

    //The elements have been transposed to work with the matrix class properly

	float s=q.r;
	float x=q.i;
	float y=q.j;
	float z=q.k;

	e00 = 1.0f - 2.0f*y*y - 2.0f*z*z;
	e01 = 2.0f*x*y + 2.0f*s*z;
	e02 = 2.0f*x*z - 2.0f*s*y;

	e10 = 2.0f*x*y - 2.0f*s*z;
	e11 = 1.0f - 2.0f*x*x - 2.0f*z*z;
	e12 = 2.0f*y*z + 2.0f*s*x;

	e20 = 2.0f*x*z + 2.0f*s*y;
	e21 = 2.0f*y*z - 2.0f*s*x;
	e22 = 1.0f - 2.0f*x*x - 2.0f*y*y;

	e30=e31=e32=e03=e13=e23=e33=0.0f;
}

void Matrix::Set3x3(const Quat& q)
{
	//Note this routine was sourced from David Baraff's paper
	//'An Introduction to Physically Based Modelling'

    //The elements have been transposed to work with the matrix class properly

	float s=q.r;
	float x=q.i;
	float y=q.j;
	float z=q.k;
	
	e00 = 1.0f - 2.0f*y*y - 2.0f*z*z;
	e01 = 2.0f*x*y + 2.0f*s*z;
	e02 = 2.0f*x*z - 2.0f*s*y;

	e10 = 2.0f*x*y - 2.0f*s*z;
	e11 = 1.0f - 2.0f*x*x - 2.0f*z*z;
	e12 = 2.0f*y*z + 2.0f*s*x;

	e20 = 2.0f*x*z + 2.0f*s*y;
	e21 = 2.0f*y*z - 2.0f*s*x;
	e22 = 1.0f - 2.0f*x*x - 2.0f*y*y;
}

void Matrix::set3x3ToQuat(const Quat& q)
{
	//Note this routine was sourced from David Baraff's paper
	//'An Introduction to Physically Based Modelling'

    //The elements have been transposed to work with the matrix class properly

	float s=q.r;
	float x=q.i;
	float y=q.j;
	float z=q.k;

	e00 = 1.0f - 2.0f*y*y - 2.0f*z*z;
	e01 = 2.0f*x*y + 2.0f*s*z;
	e02 = 2.0f*x*z - 2.0f*s*y;

	e10 = 2.0f*x*y - 2.0f*s*z;
	e11 = 1.0f - 2.0f*x*x - 2.0f*z*z;
	e12 = 2.0f*y*z + 2.0f*s*x;

	e20 = 2.0f*x*z + 2.0f*s*y;
	e21 = 2.0f*y*z - 2.0f*s*x;
	e22 = 1.0f - 2.0f*x*x - 2.0f*y*y;
}

void Matrix::getInverse3x3Of(const Matrix &in)
{
    m[0][0] = in.m[1][1]*in.m[2][2] - in.m[1][2]*in.m[2][1];
    m[0][1] = in.m[0][2]*in.m[2][1] - in.m[0][1]*in.m[2][2];
    m[0][2] = in.m[0][1]*in.m[1][2] - in.m[0][2]*in.m[1][1];
    m[1][0] = in.m[1][2]*in.m[2][0] - in.m[1][0]*in.m[2][2];
    m[1][1] = in.m[0][0]*in.m[2][2] - in.m[0][2]*in.m[2][0];
    m[1][2] = in.m[0][2]*in.m[1][0] - in.m[0][0]*in.m[1][2];
    m[2][0] = in.m[1][0]*in.m[2][1] - in.m[1][1]*in.m[2][0];
    m[2][1] = in.m[0][1]*in.m[2][0] - in.m[0][0]*in.m[2][1];
    m[2][2] = in.m[0][0]*in.m[1][1] - in.m[0][1]*in.m[1][0];

    float det = in.m[0][0]*m[0][0] + in.m[0][1]*m[1][0]+ in.m[0][2]*m[2][0];

    assert(det>0.00001 || det<-0.00001); //check not zero

    float  invDet = 1.0f/det;
    for ( int row = 0; row < 3; row++)
    {
        for ( int col = 0; col < 3; col++)
            m[row][col] *=  invDet;
    }
}

void Matrix::makeProjectionMatrix(float nearplane, float farplane, float fov, float aspect)
{
	float c, Q;
	
	Init();

	c = cosf(fov/2.0f) / sinf(fov/2.0f);
    Q = farplane / ( farplane - nearplane );

    e00	= aspect * c;
    e11	= 1.0f * c;
    e22	= Q;
    e23	= 1.0f;
    e32	= -Q * nearplane;
	e33 = 0;
}


void Matrix::Move(const Vector3& v)
{
	e30+=v.x;e31+=v.y,e32+=v.z;
}
