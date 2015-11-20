#ifndef _COLLISIONMGR_H__
#define _COLLISIONMGR_H__

#include <d3d9.h>
#include <d3dx9.h>

#include "CCustomBSPData.h"

struct VECTOR3 : public D3DXVECTOR3 
{
	VECTOR3 ( ) {};
	
	VECTOR3 ( const float *f )
	{
		x = *f; y = *f; z = *f;
	}
	
	VECTOR3 ( const D3DVECTOR& v )
	{
		x = v.x; y = v.y; z = v.z;
	}

	VECTOR3 ( float x, float y, float z ) 
	{
		this->x = x; this->y = y; this->z = z;
	}

	VECTOR3& operator *= ( const VECTOR3 v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		
		return* this;
	}

	VECTOR3& operator /= ( const VECTOR3 v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return* this;
	}

	VECTOR3& operator *= ( const float f )
	{
		x *= f;
		y *= f;
		z *= f;
	
		return* this;
	}

	friend VECTOR3 operator * ( const VECTOR3& v1, const VECTOR3& v2 )
	{
		return VECTOR3 ( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z  );
	}

	friend VECTOR3 operator / ( const VECTOR3& v1, const VECTOR3& v2 )
	{
		return VECTOR3 ( v1.x / v2.x, v1.y / v2.y, v1.z / v2.z  );
	}

	friend VECTOR3 operator / ( const float d, const VECTOR3& v2 )
	{
		return VECTOR3 ( d / v2.x, d / v2.y, d / v2.z  );
	}
};

class CCollisionManager
{
	public:
		CCollisionManager  ( void );
		~CCollisionManager ( void );
	
	public:
		enum COLLISION_TYPE 
							{
								COLLIDE_AND_SLIDE,
								COLLIDE_AND_STOP,
								COLLIDE_AND_BOUNCE, 
								COLLIDE_NONE
							};

		VECTOR3 collisionDetection ( VECTOR3& origin, VECTOR3& velocityVector, float fdeltaTime );
		
		__inline void setGravity       ( VECTOR3& g )          { m_gravity                  = g;      }
		__inline void setFriction      ( VECTOR3& f )          { m_friction                 = f;      }
		__inline void setEllipsoid     ( VECTOR3& radius )     { m_ellipRadius              = radius; }
		__inline void setEyePoint      ( VECTOR3& eye )        { m_eyePoint                 = eye;    }
		__inline void setCollisionType ( COLLISION_TYPE type ) { m_collisionType            = type;   }
		__inline void enableCollisions ( bool enable = true )  { m_enableCollisionDetection = enable; }

		__inline VECTOR3			getGravity           ( void )   { return m_gravity;                  }
		__inline VECTOR3			getFriction          ( void )   { return m_friction;                 }
		__inline VECTOR3			getEllipsoid         ( void )   { return m_ellipRadius;              }
		__inline VECTOR3			getEyePoint          ( void )   { return m_eyePoint;                 }
		__inline COLLISION_TYPE		getCollisionType     ( void )   { return m_collisionType;            }
		__inline bool				getCollisionsEnabled ( void )   { return m_enableCollisionDetection; }

		__inline int getNumPolysChecked ( void ) { return m_numPolysChecked; }

	private:
		VECTOR3  collideAndStop   ( VECTOR3& origin, VECTOR3& velocityVector );
		VECTOR3  collideAndSlide  ( VECTOR3  origin, VECTOR3  velocityVector );
		VECTOR3  collideAndBounce ( VECTOR3& origin, VECTOR3& velocityVector );
		POLYGON* getAABBColliders ( VECTOR3  origin, VECTOR3  velocityVector );
	
		void copyAndScalePolygon ( POLYGON* polyToCopy, POLYGON* scaledPoly, VECTOR3 &scalar );

		VECTOR3					m_gravity;
		VECTOR3					m_friction;
		VECTOR3					m_ellipRadius;
		VECTOR3					m_eyePoint;
		COLLISION_TYPE			m_collisionType; 

		bool					m_enableCollisionDetection;

		int						m_numPolysChecked;
};

#endif
