#ifndef DBPROPHYSICS_VECTOR_H
#define DBPROPHYSICS_VECTOR_H
#include "Math.h"

//--------------------------------------------------------------------
//------A two element vector class-----------------------------------
//--------------------------------------------------------------------

class Vector3;

class Vector2
{
	public:
		float x,y;

		Vector2():x(0),y(0){};

		Vector2(float vx,float vy)
		{
			x=vx;
			y=vy;
		}

		void Init(){x=y=0.0f;}
		
		explicit Vector2(Vector3& vec3); //made explicit for safety
		Vector2& operator = (Vector3& rhs);


		//unary minus (be careful!)
		Vector2 operator - () const
		{
			return Vector2(-x,-y);
		}


		Vector2& operator += (const Vector2& rhs)
		{
			x+=rhs.x;
			y+=rhs.y;
			return *this;
		}

		Vector2& operator -= (const Vector2& rhs)
		{
			x-=rhs.x;
			y-=rhs.y;
			return *this;
		}

		Vector2& operator *= (float scale)
		{
			x*=scale;
			y*=scale;
			return *this;
		}

		Vector2& operator /= (float scale)
		{
			if(scale==0)
			{
				x=y=0;
				return *this;
			}
			scale=1.0f/scale;
			x*=scale;
			y*=scale;
			return *this;
		}

		const Vector2 operator + (const Vector2& rhs) const
		{
			return Vector2(x+rhs.x, y+rhs.y);
		}

		const Vector2 operator - (Vector2& rhs) const
		{
			return Vector2(x-rhs.x, y-rhs.y);
		}

		const Vector2 operator * (float scale) const
		{
			return Vector2(x*scale,y*scale);
		}

		const Vector2 operator / (float scale) const
		{
			return Vector2(*this)/=scale;
		}

		const float operator * (const Vector2& rhs) const
		{
			return x * rhs.y - y * rhs.x;
		}

		friend const Vector2 operator * (float scale,Vector2& rhs)
		{
			return Vector2(rhs.x*scale,rhs.y*scale);
		}

		void Set(float i, float j){x=i;y=j;}

		float Mag(){ return float(sqrtf(x*x+y*y));} 
//		float RoughMag(){return roughSqrt(x*x+y*y);}

		float MagSqr() {return x*x+y*y;}

		float Normalise() //May as well return the size since we're calculating it
		{
			float size = Mag();
			if(size==0) return 0;
			size=1.0f/size;
			x*=size;
			y*=size;
			return size;
		}

		float Dot(Vector2& vec2)
		{
			return x*vec2.x + y*vec2.y;
		}
		
		Vector2 Lerp(Vector2& vec2,float t)
		{			
			return Vector2(
				x*t + vec2.x*(t-1.0f),
				y*t + vec2.y*(t-1.0f));
		}

		float Angle(Vector2& vec2)
		{
			float szA=Mag();
			float szB=vec2.Mag();

			if(szA==0 || szB==0) return 0.0f;						
			return float(acosf(Dot(vec2)/(szA*szB)));
		}

		float NormAngle(Vector2& vec2) //Both 'this' and 'vec2' must be of unit magnitude 
		{			
			return float(acosf(Dot(vec2)));			
		}
};


//--------------------------------------------------------------------
//------A three element vector class-----------------------------------
//--------------------------------------------------------------------


class Vector3
{
	public:
		union
		{	
			float c[3];
			struct{ float x; float y; float z;};			
		};

		Vector3():x(0),y(0),z(0){}

		Vector3(float vx,float vy,float vz):x(vx),y(vy),z(vz){}

		explicit Vector3(const Vector2& vec2):x(vec2.x),y(vec2.y),z(0){}

		Vector3(const Vector3& v):x(v.x),y(v.y),z(v.z){}

		Vector3(const Vector3& vec3,int reorder)
		{
			switch(reorder)
			{
			case 0:
				x=vec3.x;
				y=vec3.y;
				z=vec3.z;
				break;
			case 1:
				x=vec3.z;
				y=vec3.x;
				z=vec3.y;
				break;
			case 2:
				x=vec3.y;
				y=vec3.z;
				z=vec3.x;
				break;
			}
		}

		void Init(){x=y=z=0.0f;}

		//Only use operator equals for lower element classes
		Vector3& operator = (const Vector2& rhs)
		{
			x=rhs.x;
			y=rhs.y;
			z=0;			
			return *this;
		}

		Vector3& operator = (const Vector3& rhs)
		{
			//Won't test for this - might cause slow down
			//if(&rhs==this) return *this;
			x=rhs.x;
			y=rhs.y;
			z=rhs.z;			
			return *this;
		}

		//unary minus (be careful!)
		Vector3 operator - () const
		{
			return Vector3(-x,-y,-z);
		}


		Vector3& operator += (const Vector3& rhs)
		{
			x+=rhs.x;
			y+=rhs.y;
			z+=rhs.z;
			return *this;
		}

		Vector3& operator += (float rhs)
		{
			x+=rhs;
			y+=rhs;
			z+=rhs;
			return *this;
		}

		Vector3& operator -= (const Vector3& rhs)
		{
			x-=rhs.x;
			y-=rhs.y;
			z-=rhs.z;
			return *this;
		}

		Vector3& operator -= (float rhs)
		{
			x-=rhs;
			y-=rhs;
			z-=rhs;
			return *this;
		}

		Vector3& operator *= (float scale)
		{
			x*=scale;
			y*=scale;
			z*=scale;
			return *this;
		}

		Vector3& operator /= (float scale)
		{
			if(scale==0)
			{
				x=y=z=0;
				return *this;
			}
			scale=1.0f/scale;
			x*=scale;
			y*=scale;
			z*=scale;
			return *this;
		}

		const Vector3 operator + (const Vector3& rhs) const
		{
			return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
		}

		const Vector3 operator - (const Vector3& rhs) const
		{
			return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
		}

		const Vector3 operator * (float scale) const
		{
			return Vector3(x*scale,y*scale,z*scale);
		}

		const Vector3 operator / (float scale) const
		{
			return Vector3(*this)/=scale;
		}

		const Vector3 operator * (const Vector3& rhs) const
		{
			return Vector3(
				y * rhs.z - z * rhs.y,
				z * rhs.x - x * rhs.z,
				x * rhs.y - y * rhs.x);
		}

		Vector3& operator *= (const Vector3& rhs)
		{
			*this = *this * rhs;
			return *this;
		}

		friend const Vector3 operator * (float scale,Vector3& rhs)
		{
			return Vector3(rhs.x*scale,rhs.y*scale,rhs.z*scale);
		}



		//Lower element class arithmetic
		friend const Vector3 operator + (const Vector3& lhs, const Vector2& rhs)
		{
			return Vector3(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z);
		}

		friend const Vector3 operator + (const Vector2& lhs, const Vector3& rhs)
		{
			return Vector3(lhs.x+rhs.x, lhs.y+rhs.y, rhs.z);
		}

		friend const Vector3 operator - (const Vector3& lhs, const Vector2& rhs)
		{
			return Vector3(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z);
		}

		friend const Vector3 operator - (const Vector2& lhs, const Vector3& rhs)
		{
			return Vector3(lhs.x-rhs.x, lhs.y-rhs.y, -rhs.z);
		}

		void Set(float i, float j, float k){x=i;y=j;z=k;}

		float Mag() const{ return float(sqrtf(x*x+y*y+z*z));} 
//		float roughMag(){return roughSqrt(x*x+y*y+z*z);}

		float MagSqr() const{return x*x+y*y+z*z;}

		void ReciprocalThis()
		{
			x=1.0f/x;
			y=1.0f/y;
			z=1.0f/z;
		}

		void Normalise()
		{
			float size = Mag();
			if(size==0) return;
			size=1.0f/size;
			x*=size;
			y*=size;
			z*=size;
		}

		float NormaliseRet() //Return the size
		{
			float size = Mag();			
			if(size==0) return 0;
			float invsize=1.0f/size;
			x*=invsize;
			y*=invsize;
			z*=invsize;
			return size;
		}

		float Dot(const Vector3& vec3) const
		{
			return x*vec3.x + y*vec3.y + z*vec3.z;
		}
		
		void Scale(const Vector3& vec3)
		{
			x*=vec3.x;
			y*=vec3.y;
			z*=vec3.z;
		}

		Vector3 Lerp(const Vector3& vec3,float t)
		{			
			return Vector3(
				x*t + vec3.x*(t-1.0f),
				y*t + vec3.y*(t-1.0f),
				z*t + vec3.z*(t-1.0f));
		}

		float Angle(const Vector3& vec3)
		{
			float szA=Mag();
			float szB=vec3.Mag();

			if(szA==0 || szB==0) return 0.0f;						
			return float(acosf(Dot(vec3)/(szA*szB)));
		}

		bool isEqual(const Vector3& rhs,float tol)
		{
			float diff=x-rhs.x;
			if(diff<tol && diff>-tol)
			{
				diff=y-rhs.y;
				if(diff<tol && diff>-tol)
				{
					diff=z-rhs.z;
					if(diff<tol && diff>-tol)
					{
						return true;
					}
				}
			}
			return false;
		}

		float NormAngle(const Vector3& vec3) //Both 'this' and 'vec3' must be of unit magnitude 
		{			
			return float(acosf(Dot(vec3)));			
		}

        void scaleUsingCenter(float scale,const Vector3 center)
		{
			*this-=center;
			*this*=scale;
			*this+=center;
		}
};


#endif