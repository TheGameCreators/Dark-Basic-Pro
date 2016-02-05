#ifndef DBPROPHYSICS_MATRIX_H
#define DBPROPHYSICS_MATRIX_H
#include "memory.h"

typedef enum _Rotation{ROTATE_XYZ,ROTATE_XZY,ROTATE_YXZ,ROTATE_YZX,ROTATE_ZXY,ROTATE_ZYX} eRotation;

class Vector3;

class Matrix
{
	private:
		//Hide operators that don't make sense
		const Matrix operator - (const Matrix& rhs);
		Matrix& operator -= (const Matrix& rhs);
		const Matrix& operator += (int);
		const Matrix& operator -= (int);

	public:
		union 
		{
			float M[16];
			float m[4][4];						
			struct {
				float        e00, e01, e02, e03;
				float        e10, e11, e12, e13;
				float        e20, e21, e22, e23;
				float        e30, e31, e32, e33;
			};
		};

		//Constructors;
		Matrix():e00(1), e01(0), e02(0), e03(0),
				 e10(0), e11(1), e12(0), e13(0),
				 e20(0), e21(0), e22(1), e23(0),
				 e30(0), e31(0), e32(0), e33(1){};
		
		explicit Matrix(const float * elements)
		{
			memcpy(m,elements,sizeof(Matrix));
		}

		Matrix(const Matrix& mat)
		{
			memcpy(m,mat.m,sizeof(Matrix));
		}

		Matrix(
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

		void Init()
		{
			e00=e11=e22=e33=1.0f;
			e01=e02=e03=e10=e12=e13=e20=e21=e23=e30=e31=e32=0.0f;			
		}

		void convertD3DMatrix(const struct D3DXMATRIX* mat)
		{
			memcpy(m,mat,sizeof(Matrix));
		}

		operator D3DXMATRIX*()
		{
			return (D3DXMATRIX*)M;
		}

		void setScaleMatrix(float s)
		{
			e00=e11=e22=s;
			e01=e02=e03=e10=e12=e13=e20=e21=e23=e30=e31=e32=0.0f;
            e33=1.0f;
		}

		void setScaleMatrix(float sx,float sy,float sz)
		{
			e00=sx;
			e11=sy;
			e22=sz;
			e01=e02=e03=e10=e12=e13=e20=e21=e23=e30=e31=e32=0.0f;
            e33=1.0f;
		}

		void Set3x3(const Matrix& mat)
		{
			e00=mat.e00;
			e01=mat.e01;
			e02=mat.e02;
			e10=mat.e10;
			e11=mat.e11;
			e12=mat.e12;
			e20=mat.e20;
			e21=mat.e21;
			e22=mat.e22;
		}

		void Set3x3(
			float v00, float v01, float v02,
			float v10, float v11, float v12,
			float v20, float v21, float v22);


		Matrix& operator = (const Matrix& rhs)
		{
			if(&rhs==this) return *this;
			memcpy(m,rhs.m,sizeof(Matrix));
			return *this;
		}

		float Det3x3()
		{
			return e00*(e11*e22 - e12*e21) - e01*(e10*e22-e12*e20) + e02*(e10*e21 - e11*e20);
		}
		
		//Note: both '*' and '*=' require temporary matrices to be created,
		//so no advantage can be gained by only implementing one of them

		const Matrix operator * (const Matrix& rhs) const
		{
			return Matrix(
				e00*rhs.e00 + e01*rhs.e10 + e02*rhs.e20 + e03*rhs.e30,
				e00*rhs.e01 + e01*rhs.e11 + e02*rhs.e21 + e03*rhs.e31,
				e00*rhs.e02 + e01*rhs.e12 + e02*rhs.e22 + e03*rhs.e32,
				e00*rhs.e03 + e01*rhs.e13 + e02*rhs.e23 + e03*rhs.e33,
				e10*rhs.e00 + e11*rhs.e10 + e12*rhs.e20 + e13*rhs.e30,
				e10*rhs.e01 + e11*rhs.e11 + e12*rhs.e21 + e13*rhs.e31,
				e10*rhs.e02 + e11*rhs.e12 + e12*rhs.e22 + e13*rhs.e32,
				e10*rhs.e03 + e11*rhs.e13 + e12*rhs.e23 + e13*rhs.e33,
				e20*rhs.e00 + e21*rhs.e10 + e22*rhs.e20 + e23*rhs.e30,
				e20*rhs.e01 + e21*rhs.e11 + e22*rhs.e21 + e23*rhs.e31,
				e20*rhs.e02 + e21*rhs.e12 + e22*rhs.e22 + e23*rhs.e32,
				e20*rhs.e03 + e21*rhs.e13 + e22*rhs.e23 + e23*rhs.e33,
				e30*rhs.e00 + e31*rhs.e10 + e32*rhs.e20 + e33*rhs.e30,
				e30*rhs.e01 + e31*rhs.e11 + e32*rhs.e21 + e33*rhs.e31,
				e30*rhs.e02 + e31*rhs.e12 + e32*rhs.e22 + e33*rhs.e32,
				e30*rhs.e03 + e31*rhs.e13 + e32*rhs.e23 + e33*rhs.e33);
		}

		Matrix& operator *= (Matrix& rhs)
		{
			//Identical to using the routine 'transformUsing'
			*this = *this * rhs;
			return *this;
		}

		//Optimisation can be made here though

		Matrix& operator *= (float scale)
		{
			e00*=scale;
			e01*=scale;
			e02*=scale;
			e03*=scale;
			e10*=scale;
			e11*=scale;
			e12*=scale;
			e13*=scale;
			e20*=scale;
			e21*=scale;
			e22*=scale;
			e23*=scale;
			e30*=scale;
			e31*=scale;
			e32*=scale;
			e33*=scale;
			return *this;
		}


		friend const Matrix operator * (const Matrix& lhs, float scale)
		{			
			return Matrix(lhs)*=scale;
		}

		Matrix& operator += (const Matrix& rhs)
		{
			e00+=rhs.e00;
			e01+=rhs.e01;
			e02+=rhs.e02;
			e03+=rhs.e03;
			e10+=rhs.e10;
			e11+=rhs.e11;
			e12+=rhs.e12;
			e13+=rhs.e13;
			e20+=rhs.e20;
			e21+=rhs.e21;
			e22+=rhs.e22;
			e23+=rhs.e23;
			e30+=rhs.e30;
			e31+=rhs.e31;
			e32+=rhs.e32;
			e33+=rhs.e33;
			return *this;
		}

		friend const Matrix operator + (const Matrix& lhs, const Matrix& rhs)
		{			
			return Matrix(lhs)+=rhs;
		}


		void transformUsing(const Matrix &rhs)
		{
			Matrix tmp;

			tmp.e00 = rhs.e00 * e00 + rhs.e10 * e01 + rhs.e20 * e02 + rhs.e30 * e03;
			tmp.e01 = rhs.e01 * e00 + rhs.e11 * e01 + rhs.e21 * e02 + rhs.e31 * e03;
			tmp.e02 = rhs.e02 * e00 + rhs.e12 * e01 + rhs.e22 * e02 + rhs.e32 * e03;
			tmp.e03 = rhs.e03 * e00 + rhs.e13 * e01 + rhs.e23 * e02 + rhs.e33 * e03;
			tmp.e10 = rhs.e00 * e10 + rhs.e10 * e11 + rhs.e20 * e12 + rhs.e30 * e13;
			tmp.e11 = rhs.e01 * e10 + rhs.e11 * e11 + rhs.e21 * e12 + rhs.e31 * e13;
			tmp.e12 = rhs.e02 * e10 + rhs.e12 * e11 + rhs.e22 * e12 + rhs.e32 * e13;
			tmp.e13 = rhs.e03 * e10 + rhs.e13 * e11 + rhs.e23 * e12 + rhs.e33 * e13;
			tmp.e20 = rhs.e00 * e20 + rhs.e10 * e21 + rhs.e20 * e22 + rhs.e30 * e23;
			tmp.e21 = rhs.e01 * e20 + rhs.e11 * e21 + rhs.e21 * e22 + rhs.e31 * e23;
			tmp.e22 = rhs.e02 * e20 + rhs.e12 * e21 + rhs.e22 * e22 + rhs.e32 * e23;
			tmp.e23 = rhs.e03 * e20 + rhs.e13 * e21 + rhs.e23 * e22 + rhs.e33 * e23;
			tmp.e30 = rhs.e00 * e30 + rhs.e10 * e31 + rhs.e20 * e32 + rhs.e30 * e33;
			tmp.e31 = rhs.e01 * e30 + rhs.e11 * e31 + rhs.e21 * e32 + rhs.e31 * e33;
			tmp.e32 = rhs.e02 * e30 + rhs.e12 * e31 + rhs.e22 * e32 + rhs.e32 * e33;
			tmp.e33 = rhs.e03 * e30 + rhs.e13 * e31 + rhs.e23 * e32 + rhs.e33 * e33;

			*this = tmp;
		}

		void transformUsing(const Matrix &rhs, Matrix * out) const
		{
			out->e00 = rhs.e00 * e00 + rhs.e10 * e01 + rhs.e20 * e02 + rhs.e30 * e03;
			out->e01 = rhs.e01 * e00 + rhs.e11 * e01 + rhs.e21 * e02 + rhs.e31 * e03;
			out->e02 = rhs.e02 * e00 + rhs.e12 * e01 + rhs.e22 * e02 + rhs.e32 * e03;
			out->e03 = rhs.e03 * e00 + rhs.e13 * e01 + rhs.e23 * e02 + rhs.e33 * e03;
			out->e10 = rhs.e00 * e10 + rhs.e10 * e11 + rhs.e20 * e12 + rhs.e30 * e13;
			out->e11 = rhs.e01 * e10 + rhs.e11 * e11 + rhs.e21 * e12 + rhs.e31 * e13;
			out->e12 = rhs.e02 * e10 + rhs.e12 * e11 + rhs.e22 * e12 + rhs.e32 * e13;
			out->e13 = rhs.e03 * e10 + rhs.e13 * e11 + rhs.e23 * e12 + rhs.e33 * e13;
			out->e20 = rhs.e00 * e20 + rhs.e10 * e21 + rhs.e20 * e22 + rhs.e30 * e23;
			out->e21 = rhs.e01 * e20 + rhs.e11 * e21 + rhs.e21 * e22 + rhs.e31 * e23;
			out->e22 = rhs.e02 * e20 + rhs.e12 * e21 + rhs.e22 * e22 + rhs.e32 * e23;
			out->e23 = rhs.e03 * e20 + rhs.e13 * e21 + rhs.e23 * e22 + rhs.e33 * e23;
			out->e30 = rhs.e00 * e30 + rhs.e10 * e31 + rhs.e20 * e32 + rhs.e30 * e33;
			out->e31 = rhs.e01 * e30 + rhs.e11 * e31 + rhs.e21 * e32 + rhs.e31 * e33;
			out->e32 = rhs.e02 * e30 + rhs.e12 * e31 + rhs.e22 * e32 + rhs.e32 * e33;
			out->e33 = rhs.e03 * e30 + rhs.e13 * e31 + rhs.e23 * e32 + rhs.e33 * e33;			
		}


		void transform3x3Using(const Matrix &rhs)
		{
			Matrix tmp;
			
			tmp.e00 = rhs.e00 * e00 + rhs.e10 * e01 + rhs.e20 * e02;
			tmp.e01 = rhs.e01 * e00 + rhs.e11 * e01 + rhs.e21 * e02;
			tmp.e02 = rhs.e02 * e00 + rhs.e12 * e01 + rhs.e22 * e02;
			tmp.e10 = rhs.e00 * e10 + rhs.e10 * e11 + rhs.e20 * e12;
			tmp.e11 = rhs.e01 * e10 + rhs.e11 * e11 + rhs.e21 * e12;
			tmp.e12 = rhs.e02 * e10 + rhs.e12 * e11 + rhs.e22 * e12;
			tmp.e20 = rhs.e00 * e20 + rhs.e10 * e21 + rhs.e20 * e22;
			tmp.e21 = rhs.e01 * e20 + rhs.e11 * e21 + rhs.e21 * e22;
			tmp.e22 = rhs.e02 * e20 + rhs.e12 * e21 + rhs.e22 * e22;

			e00=tmp.e00; e01=tmp.e01; e02=tmp.e02;
			e10=tmp.e10; e11=tmp.e11; e12=tmp.e12;
			e20=tmp.e20; e21=tmp.e21; e22=tmp.e22;
		}

		void transform3x3Using(const Matrix &rhs, Matrix * out) const
		{
			out->e00 = rhs.e00 * e00 + rhs.e10 * e01 + rhs.e20 * e02;
			out->e01 = rhs.e01 * e00 + rhs.e11 * e01 + rhs.e21 * e02;
			out->e02 = rhs.e02 * e00 + rhs.e12 * e01 + rhs.e22 * e02;
			out->e10 = rhs.e00 * e10 + rhs.e10 * e11 + rhs.e20 * e12;
			out->e11 = rhs.e01 * e10 + rhs.e11 * e11 + rhs.e21 * e12;
			out->e12 = rhs.e02 * e10 + rhs.e12 * e11 + rhs.e22 * e12;
			out->e20 = rhs.e00 * e20 + rhs.e10 * e21 + rhs.e20 * e22;
			out->e21 = rhs.e01 * e20 + rhs.e11 * e21 + rhs.e21 * e22;
			out->e22 = rhs.e02 * e20 + rhs.e12 * e21 + rhs.e22 * e22;
		}

		const Vector3 unitX() const;
		const Vector3 unitY() const;
		const Vector3 unitZ() const;
		const Vector3 pos() const;

		void setPos(float i, float j, float k) {e30=i;e31=j,e32=k;}
		void setPos(const Vector3& v);		

		void setUnitX(const Vector3& v);
		void setUnitY(const Vector3& v);
		void setUnitZ(const Vector3& v);

		void Move(float i, float j, float k) {e30+=i;e31+=j,e32+=k;}
		void Move(const Vector3& v);		

		void rotateV3(const Vector3& invector,Vector3 * outvector) const;
		void rotateV3(Vector3 * v) const;

		void rotateAndTransV3(const Vector3& invector,Vector3 * outvector) const;
		void rotateAndTransV3(Vector3 * v) const;

		void Reset()
		{
			e00=1.0; e01=0.0;  e02=0.0; e03=0.0;
			e10=0.0; e11=1.0;  e12=0.0; e13=0.0;
			e20=0.0; e21=0.0;  e22=1.0; e23=0.0;
			e30=0.0; e31=0.0;  e32=0.0; e33=1.0;
		}

        void Inverse4x4This();
        void Inverse3x3This();
		void getInverse3x3Of(const Matrix &in);

		Matrix& TransposeThis()
		{			
			float tmp;

			tmp=m[0][1];	m[0][1]=m[1][0];	m[1][0]=tmp;
			tmp=m[0][2];	m[0][2]=m[2][0];	m[2][0]=tmp;
			tmp=m[0][3];	m[0][3]=m[3][0];	m[3][0]=tmp;
			tmp=m[1][2];	m[1][2]=m[2][1];	m[2][1]=tmp;
			tmp=m[1][3];	m[1][3]=m[3][1];	m[3][1]=tmp;
			tmp=m[2][3];	m[2][3]=m[3][2];	m[3][2]=tmp;			

			return *this;
		}

		const Matrix TransposeCopy() const
		{
			return Matrix(	e00, e10, e20, e30,
            	   			e01, e11, e21, e31,
		                    e02, e12, e22, e32,
		                    e03, e13, e23, e33);
        }

		void TransposeThis3x3()
		{
			float tmp;

			tmp=m[0][1];	m[0][1]=m[1][0];	m[1][0]=tmp;
			tmp=m[0][2];	m[0][2]=m[2][0];	m[2][0]=tmp;
			tmp=m[1][2];	m[1][2]=m[2][1];	m[2][1]=tmp;
		}

		const Matrix TransposeCopy3x3() const
		{
			return Matrix(	e00, e10, e20, e03,
            	   			e01, e11, e21, e13,
		                    e02, e12, e22, e23,
		                    e30, e31, e32, e33);
        }

		void Set(
			float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33);
		
		void makeRotationX(float x);
		void makeRotationY(float y);
		void makeRotationZ(float z);
		void makeRotationMatrix(float rx, float ry, float rz,eRotation order);

		void make3x3RotationX(float x);
		void make3x3RotationY(float y);
		void make3x3RotationZ(float z);
		void make3x3RotationMatrix(float rx, float ry, float rz,eRotation order);

		// added by Simon. 16-Nov-2001
		void makeViewMatrix(const Vector3& camera, const Vector3& target, const Vector3& wup);
		void makeProjectionMatrix(float nearplane, float farplane, float fov, float aspect);
};

#endif
