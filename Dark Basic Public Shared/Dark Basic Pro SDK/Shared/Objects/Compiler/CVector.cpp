
#include <math.h>
#include "cVector.h"
#include "cPlane.h"

cVector4::cVector4 ( )
{
    x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

cVector4::cVector4 ( const cVector4 & vec )
{
    x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}

cVector4::cVector4 ( float _x, float _y, float _z, float _w )
{
    x = _x;
	y = _y;
	z = _z;
	w = _w;
}

cVector4::operator float * ( )
{
    return ( float* ) &x;
}

cVector4::operator const float * ( ) const
{
    return ( const float* ) &x;
}

cVector4 cVector4::operator + ( ) const
{
    return *this;
}

cVector4 cVector4::operator - ( ) const
{
    return cVector4 ( -x, -y, -z, -w );
}

cVector4& cVector4::operator += ( const cVector4& vec )
{
    x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;

    return *this;
}

cVector4& cVector4::operator -= ( const cVector4& vec )
{
    x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;

    return *this;
}

cVector4& cVector4::operator *= ( const float& Value  )
{
    x *= Value;
	y *= Value;
	z *= Value;
	w *= Value;

    return *this;
}

cVector4& cVector4::operator /= ( const float& Value  )
{
    float fValue = 1.0f / Value;
    
	x *= fValue;
	y *= fValue;
	z *= fValue;
	w *= fValue;

    return *this;
}

cVector4 cVector4::operator + ( const cVector4& vec ) const
{
    return cVector4 ( x + vec.x, y + vec.y, z + vec.z, w + vec.w );
}

cVector4 cVector4::operator - ( const cVector4& vec ) const
{
    return cVector4 ( x - vec.x, y - vec.y, z - vec.z, w - vec.w );
}

cVector4 cVector4::operator * ( const float& Value ) const
{
    return cVector4 ( x * Value, y * Value, z * Value, w * Value );
}

cVector4 cVector4::operator / ( const float& Value  ) const
{
    float fValue = 1.0f / Value;
    return cVector4 ( x * fValue, y * fValue, z * fValue, w * fValue );
}

cVector4& cVector4::operator = ( const cVector4& vec )
{
    x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;

    return *this;
}

bool cVector4::operator == ( const cVector4& vec ) const
{
    return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z ) && ( w == vec.w );
}

bool cVector4::operator != ( const cVector4& vec ) const
{
    return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z ) || ( w != vec.w );
}
    
cVector4 operator * ( float Value, const cVector4& vec )
{
    return cVector4 ( vec.x * Value, vec.y * Value, vec.z * Value, vec.w * Value );
}

bool cVector4::IsEmpty ( void ) const
{
    return ( x == 0.0f ) && ( y == 0.0f ) && ( z == 0.0f ) && ( w == 0.0f );
}

bool cVector4::SetBaryCentric ( const cVector4& V1, const cVector4& V2, const cVector4& V3, const float& f, const float& g )
{
    x = V1.x + f * ( V2.x - V1.x ) + g * ( V3.x - V1.x );
    y = V1.y + f * ( V2.y - V1.y ) + g * ( V3.y - V1.y );
    z = V1.z + f * ( V2.z - V1.z ) + g * ( V3.z - V1.z );
    w = V1.w + f * ( V2.w - V1.w ) + g * ( V3.w - V1.w );

    return true;
}

bool cVector4::SetCatmullRom ( const cVector4& V1, const cVector4& V2, const cVector4& V3, const cVector4& V4, const float& s )
{
    float   ss, sss, a, b, c, d;

    ss  = s * s;
    sss = s * ss;

    a = -0.5f * sss + ss - 0.5f * s;
    b =  1.5f * sss - 2.5f * ss + 1.0f;
    c = -1.5f * sss + 2.0f * ss + 0.5f * s;
    d =  0.5f * sss - 0.5f * ss;

    x = a * V1.x + b * V2.x + c * V3.x + d * V4.x;
    y = a * V1.y + b * V2.y + c * V3.y + d * V4.y;
    z = a * V1.z + b * V2.z + c * V3.z + d * V4.z;
    w = a * V1.w + b * V2.w + c * V3.w + d * V4.w;

    return true;
}

bool cVector4::SetHermite ( const cVector4& V1, const cVector4& T1, const cVector4& V2, const cVector4& T2, const float& s )
{
    float   ss, sss, a, b, c, d;

    ss  = s * s;
    sss = s * ss;

    a =  2.0f * sss - 3.0f * ss + 1.0f;
    b = -2.0f * sss + 3.0f * ss;
    c =  sss - 2.0f * ss + s;
    d =  sss - ss;

    x = a * V1.x + b * V2.x + c * T1.x + d * T2.x;
    y = a * V1.y + b * V2.y + c * T1.y + d * T2.y;
    z = a * V1.z + b * V2.z + c * T1.z + d * T2.z;
    w = a * V1.w + b * V2.w + c * T1.w + d * T2.w;

    return true;
}

cVector4 cVector4::Lerp ( const cVector4& V1, const float& s ) const
{
    return cVector4 ( x + s * ( V1.x - x ), y + s * ( V1.y - y ), z + s * ( V1.z - z ), w + s * ( V1.w - w ) );
}

cVector4 cVector4::Maximize ( const cVector4& V1 ) const
{
    return cVector4 ( ( x > V1.x ) ? x : V1.x, ( y > V1.y ) ? y : V1.y, ( z > V1.z ) ? z : V1.z, ( w > V1.w ) ? w : V1.w );
}

cVector4 cVector4::Minimize ( const cVector4& V1 ) const
{
    return cVector4 ( ( x < V1.x ) ? x : V1.x, ( y < V1.y ) ? y : V1.y, ( z < V1.z ) ? z : V1.z, ( w < V1.w ) ? w : V1.w );
}

cVector4& cVector4::Scale ( const float &Scale )
{
    x *= Scale;
	y *= Scale;
	z *= Scale;
	w *= Scale;

    return *this;
}

cVector4 cVector4::Cross ( const cVector4& V1, const cVector4& V2 ) const
{
    float   a, b, c, d, e, f;

    a = V1.x * V2.y - V1.y * V2.x;
    b = V1.x * V2.z - V1.z * V2.x;
    c = V1.x * V2.w - V1.w * V2.x;
    d = V1.y * V2.z - V1.z * V2.y;
    e = V1.y * V2.w - V1.w * V2.y;
    f = V1.z * V2.w - V1.w * V2.z;

    return cVector4 ( (  f * y - e * z + d * w ), 
                      ( -f * x - c * z + b * w ),
                      (  e * x - c * y + a * w ),
                      ( -d * x - b * y + a * z ) );
}

float cVector4::Dot ( const cVector4& vec ) const
{
    return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

float cVector4::Length ( void ) const
{
    return sqrtf ( x * x + y * y + z * z + w * w );
}

float cVector4::SquareLength ( void ) const
{
    return x * x + y * y + z * z + w * w;
}

bool cVector4::FuzzyCompare ( const cVector4& vecCompare, const float& Tolerance ) const
{
    if ( fabsf ( x ) > Tolerance ) return false;
    if ( fabsf ( y ) > Tolerance ) return false;
    if ( fabsf ( z ) > Tolerance ) return false;
    if ( fabsf ( w ) > Tolerance ) return false;

    return true;
}

cVector4& cVector4::Normalize ( void )
{
    float   denom;

    denom = sqrtf ( x * x + y * y + z * z + w * w );

    if ( fabsf ( denom ) < 1e-5f )
		return *this;

    denom = 1.0f / denom;

    x *= denom;
    y *= denom;
    z *= denom;
    w *= denom;

    return *this;
}

cVector3::cVector3 ( )
{
    x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

cVector3::cVector3 ( const cVector3 & vec )
{
    x = vec.x;
	y = vec.y;
	z = vec.z;
}

cVector3::cVector3 ( float _x, float _y, float _z )
{
    x = _x;
	y = _y;
	z = _z;
}

cVector3::operator float * ( )
{
    return ( float* ) &x;
}

cVector3::operator const float * ( ) const
{
    return ( const float* ) &x;
}

cVector3 cVector3::operator + ( ) const
{
    return *this;
}

cVector3 cVector3::operator - ( ) const
{
    return cVector3 ( -x, -y, -z );
}

cVector3& cVector3::operator += ( const cVector3& vec )
{
    x += vec.x;
	y += vec.y;
	z += vec.z;

    return *this;
}

cVector3& cVector3::operator -= ( const cVector3& vec )
{
    x -= vec.x;
	y -= vec.y;
	z -= vec.z;

    return *this;
}

cVector3& cVector3::operator *= ( const float& Value  )
{
    x *= Value;
	y *= Value;
	z *= Value;

    return *this;
}

cVector3& cVector3::operator /= ( const float& Value  )
{
    float fValue = 1.0f / Value;
    
	x *= fValue;
	y *= fValue;
	z *= fValue;

    return *this;
}

cVector3 cVector3::operator + ( const cVector3& vec ) const
{
    return cVector3 ( x + vec.x, y + vec.y, z + vec.z );
}

cVector3 cVector3::operator - ( const cVector3& vec ) const
{
    return cVector3 ( x - vec.x, y - vec.y, z - vec.z );
}

cVector3 cVector3::operator * ( const float& Value ) const
{
    return cVector3 ( x * Value, y * Value, z * Value );
}

cVector3 cVector3::operator * ( const cVector3& vec ) const
{
    return cVector3 ( x * vec.x, y * vec.y, z * vec.z );
}

cVector3 cVector3::operator / ( const float& Value ) const
{
    float fValue = 1.0f / Value;

    return cVector3 ( x * fValue, y * fValue, z * fValue );
}

cVector3& cVector3::operator = ( const cVector3& vec )
{
    x = vec.x;
	y = vec.y;
	z = vec.z;

    return *this;
}

bool cVector3::operator == ( const cVector3& vec ) const
{
    return ( x == vec.x ) && ( y == vec.y ) && ( z == vec.z );
}

bool cVector3::operator != ( const cVector3& vec ) const
{
    return ( x != vec.x ) || ( y != vec.y ) || ( z != vec.z );
}
    
cVector3 operator * ( float Value, const cVector3& vec )
{
    return cVector3 ( vec.x * Value, vec.y * Value, vec.z * Value );
}

bool cVector3::IsEmpty ( void ) const
{
    return ( x == 0.0f ) && ( y == 0.0f ) && ( z == 0.0f );
}

bool cVector3::SetBaryCentric ( const cVector3& V1, const cVector3& V2, const cVector3& V3, const float& f, const float& g )
{
    x = V1.x + f * ( V2.x - V1.x ) + g * ( V3.x - V1.x );
    y = V1.y + f * ( V2.y - V1.y ) + g * ( V3.y - V1.y );
    z = V1.z + f * ( V2.z - V1.z ) + g * ( V3.z - V1.z );
    
    return true;
}

bool cVector3::SetCatmullRom ( const cVector3& V1, const cVector3& V2, const cVector3& V3, const cVector3& V4, const float& s )
{
    float   ss, sss, a, b, c, d;

    ss  = s * s;
    sss = s * ss;

    a = -0.5f * sss + ss - 0.5f * s;
    b =  1.5f * sss - 2.5f * ss + 1.0f;
    c = -1.5f * sss + 2.0f * ss + 0.5f * s;
    d =  0.5f * sss - 0.5f * ss;

    x = a * V1.x + b * V2.x + c * V3.x + d * V4.x;
    y = a * V1.y + b * V2.y + c * V3.y + d * V4.y;
    z = a * V1.z + b * V2.z + c * V3.z + d * V4.z;

    return true;
}

bool cVector3::SetHermite ( const cVector3& V1, const cVector3& T1, const cVector3& V2, const cVector3& T2, const float& s )
{
    float   ss, sss, a, b, c, d;

    ss  = s * s;
    sss = s * ss;

    a =  2.0f * sss - 3.0f * ss + 1.0f;
    b = -2.0f * sss + 3.0f * ss;
    c =  sss - 2.0f * ss + s;
    d =  sss - ss;

    x = a * V1.x + b * V2.x + c * T1.x + d * T2.x;
    y = a * V1.y + b * V2.y + c * T1.y + d * T2.y;
    z = a * V1.z + b * V2.z + c * T1.z + d * T2.z;

    return true;
}

cVector3 cVector3::Lerp ( const cVector3& V1, const float& s ) const
{
    return cVector3 ( x + s * ( V1.x - x ), y + s * ( V1.y - y ), z + s * ( V1.z - z ) );
}

cVector3 cVector3::Maximize ( const cVector3& V1 ) const
{
    return cVector3 ( ( x > V1.x ) ? x : V1.x, ( y > V1.y ) ? y : V1.y, ( z > V1.z ) ? z : V1.z );
}

cVector3 cVector3::Minimize ( const cVector3& V1 ) const
{
    return cVector3 ( ( x < V1.x ) ? x : V1.x, ( y < V1.y ) ? y : V1.y, ( z < V1.z ) ? z : V1.z );
}

cVector3& cVector3::Scale ( const float &Scale )
{
    x *= Scale;
	y *= Scale;
	z *= Scale;

    return *this;
}

cVector3 cVector3::Cross ( const cVector3& V1 ) const
{
    return cVector3 ( y * V1.z - z * V1.y, z * V1.x - x * V1.z, x * V1.y - y * V1.x );
}

float cVector3::Dot ( const cVector3& vec ) const
{
    return x * vec.x + y * vec.y + z * vec.z;
}

float cVector3::Length ( void ) const
{
    return sqrtf ( x * x + y * y + z * z );
}

float cVector3::SquareLength ( void ) const
{
    return x * x + y * y + z * z;
}

bool cVector3::FuzzyCompare ( const cVector3& vecCompare, const float& Tolerance ) const
{
    if ( fabsf ( x - vecCompare.x ) >= Tolerance ) return false;
    if ( fabsf ( y - vecCompare.y ) >= Tolerance ) return false;
    if ( fabsf ( z - vecCompare.z ) >= Tolerance ) return false;

    return true;
}

cVector3& cVector3::Normalize ( void )
{
    float   denom;

    denom = sqrtf ( x * x + y * y + z * z );

    if ( fabsf ( denom ) < 1e-5f )
		return *this;

    denom = 1.0f / denom;

    x *= denom;
    y *= denom;
    z *= denom;

    return *this;
}

float cVector3::DistanceToPlane ( const cPlane3& Plane ) const
{
    return Dot ( Plane.m_vecNormal ) - Plane.m_fDistance;
}

float cVector3::DistanceToPlane ( const cPlane3& Plane, const cVector3& Direction ) const
{
    float fPlaneDistance = Dot ( -Plane.m_vecNormal ) - Plane.m_fDistance;
    return fPlaneDistance * ( 1 / ( -Plane.m_vecNormal ).Dot ( Direction ) );
}

float cVector3::DistanceToLine ( const cVector3 &vecStart, const cVector3& vecEnd ) const
{
    cVector3 c, v;
    float    d, t;

    c = *this  - vecStart;
    v = vecEnd - vecStart;   
    d = v.Length ( );
    
    v.Normalize ( );
    
    t = v.Dot ( c );
   
    if ( t < 0.01f     ) return 99999.0f;
    if ( t > d - 0.01f ) return 99999.0f;
  
    v.x = vecStart.x + ( v.x * t );
    v.y = vecStart.y + ( v.y * t );
    v.z = vecStart.z + ( v.z * t );
  
    return ( ( *this ) - v ).Length ( );
}
