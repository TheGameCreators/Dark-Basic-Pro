#ifndef __ENGINEQUATERNION_HEADER
#define __ENGINEQUATERNION_HEADER

#include "math.h"

class Vector3;
class Matrix;

class Quat
{
	public:
		float r,i,j,k;

		Quat(){}
		Quat(float realn,float ima,float imb, float imc) : r(realn),i(ima),j(imb),k(imc){}		
		explicit Quat(const Vector3& v);
		
		Quat(const float angle, const Vector3& axis);

		explicit Quat(const Matrix& m);

        void Set(const Matrix& m);

		void Init(){i=j=k=0.0;r=1.0f;}

		const Quat& operator= (const Quat& q)
		{
			r = q.r;
			i = q.i;
			j = q.j;
			k = q.k;
			return *this;
		}

		const Quat operator+ (const Quat& q) const
		{
			return Quat(r+q.r,i+q.i,j+q.j,k+q.k);
		}

		const Quat operator- (const Quat& q) const
		{
			return Quat(r-q.r,i-q.i,j-q.j,k-q.k);
		}

		const Quat operator* (const Quat& q) const
		{
			//Note that a*b is not usually equal to b*a

			return Quat(r*q.r - i*q.i - j*q.j - k*q.k,
						r*q.i + i*q.r + j*q.k - k*q.j,
						r*q.j + j*q.r + k*q.i - i*q.k,
						r*q.k + k*q.r + i*q.j - j*q.i);    
		}

		const Quat operator* (float a) const
		{
			return Quat(a*r,a*i,a*j,a*k);
		}

		Quat& operator*= (float a)
		{
			r*=a;
			i*=a;
			j*=a;
			k*=a;
			return *this;
		}

		Quat& operator+= (const Quat& q)
		{
			r+=q.r;
			i+=q.i;
			j+=q.j;
			k+=q.k;
			return *this;
		}

		const Quat operator- () const
		{
			return Quat(-r,-i,-j,-k);
		}

		float Dot(const Quat& q) const
		{
			return r*q.r + i*q.i + j*q.j + k*q.k;
		}

		float Mag() const
		{
			return float(sqrtf(r*r+i*i+j*j+k*k));
		}

		void Normalise()
		{
			(*this)*=1.0f/Mag();
		}

		Quat Inverse () const
		{  
			float invSz = 1.0f/(r*r+i*i+j*j+k*k);
			return Quat(r*invSz,-i*invSz,-j*invSz,-k*invSz);
		}


		Quat UnitInverse () const
		{
			// assert:  'this' is unit length
			return Quat(r,-i,-j,-k);
		}

		const Quat& slerpTo(const Quat& q,float t);
		void rotateV3(Vector3 * v) const;
		void rotateV3(const Vector3& invector,Vector3 * outvector) const;


	private:
};

Quat Slerp (const Quat& p,const Quat& q, float t);

Quat operator* (float a, const Quat& q);

#endif