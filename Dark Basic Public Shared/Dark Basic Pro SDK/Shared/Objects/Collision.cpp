
//
// Collision Functions Implementation
//

// Includes
#include "collision.h"
#include "CommonC.h"

// Defines
#define EPSILON ( 1.0e-05f )

// Prototypes
void CalcNormal  ( D3DXVECTOR3 v [ 3 ], D3DXVECTOR3* out );
void CalcNormal2 ( D3DXVECTOR3* v, int v_count, D3DXVECTOR3* out );
BOOL BoundElipsoidIntersect ( D3DXVECTOR3 mid, D3DXVECTOR3 e, D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos );

// Collision Object
class collision_data 
{
	public:
		collision_data ( ) : found ( false ), dist ( -1.0f ), dir_len ( 0.0f ) { }

		D3DXVECTOR3 src;
		D3DXVECTOR3 dir;
		D3DXVECTOR3 ndir;
		float		dir_len;

		bool		found;
		float		dist;
		float		dist1;
		D3DXVECTOR3 nearest_poly;
		float		tu;
		float		tv;
		DWORD		diffuseid;
};

// Collision Performance Structure
struct collisionperf_data
{
	DWORD	dwCounter0;
	DWORD	dwCounter1;
	DWORD	dwCounter2;
	DWORD	dwCounter3;
	DWORD	dwCounter4;
};

// Global Collision Data
vector < sCollisionPolygon* >	g_PolygonList;
collision_data					g_CollisionData;
collisionperf_data				g_CollisionPerformance;
sCollisionPolygon*				g_pCurrentPoly			= NULL;
static int						cutoff					= 0;
static int						cutoff_max				= 5;
BOOL							clipped					= FALSE; 


//
// Workhorse Functions
//

void CalcNormal ( sCollisionVertex* v, D3DXVECTOR3 *out )
{
	D3DXVECTOR3 v1, v2;
	D3DXVec3Subtract ( &v1, &v [ 1 ].vecPosition, &v [ 0 ].vecPosition );
	D3DXVec3Subtract ( &v2, &v [ 2 ].vecPosition, &v [ 0 ].vecPosition );
	D3DXVec3Cross ( out, &v1, &v2 );
}

BOOL CheckPointInSphere ( D3DXVECTOR3 point, D3DXVECTOR3 sO, double sR )
{
	float d = D3DXVec3Length ( &( point - sO ) );
	
	if ( d <= sR )
		return TRUE;

	return FALSE;	
}

bool point_in_poly ( D3DXVECTOR3& p )
{
    int i = 0;
    
	D3DXVECTOR3 v [ 32 ];
	for ( i = 0; i < 3; i++ )
	{
	    v [ i ] = p - g_pCurrentPoly->triangle [ i ].vecPosition;
		D3DXVec3Normalize ( &v [ i ], &v [ i ] );
	}

	float total = 0.0f;
	for ( i = 0; i < 2; i++ )
	    total += ( float ) acos ( D3DXVec3Dot ( &v [ i ], &v [ i + 1 ] ) );
	
	total += ( float ) acos ( D3DXVec3Dot ( &v [ 2 ], &v [ 0 ] ) );
	
	if ( fabsf ( total - 6.28f ) < 0.01f )
		return true;

	return false;
}

D3DXVECTOR3 closest_on_line ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& p )
{
	D3DXVECTOR3 c = p - a;
    D3DXVECTOR3 v = b - a;

    float d = D3DXVec3Length ( &v );

    if ( d )
		v /= d;

    float t = D3DXVec3Dot ( &v, &c );
    
	if ( t < 0.0f )
		return a;

    if ( t > d )
		return b;

    return a + v * t;
}

D3DXVECTOR3 closest_on_poly ( const D3DXVECTOR3& p, D3DXVECTOR3* line_dir )
{
    float d [ 32 ];
    D3DXVECTOR3 v [ 32 ];
    int i = 0;
    for ( i = 0; i < 2; i++ )
	{
        v [ i ] = closest_on_line ( g_pCurrentPoly->triangle [ i ].vecPosition, g_pCurrentPoly->triangle [ i+1 ].vecPosition, p );
        D3DXVECTOR3 t = p - v [ i ];
		d [ i ] = D3DXVec3Length ( &t );
    }

    i = 2;
    v [ i ] = closest_on_line ( g_pCurrentPoly->triangle [ i ].vecPosition, g_pCurrentPoly->triangle [ 0 ].vecPosition, p );
    
	D3DXVECTOR3 t = p - v [ i ];
    d [ i ] = D3DXVec3Length ( &t );

    float min = d [ 0 ];
    D3DXVECTOR3 r = v [ 0 ];
	int cl_line = 0;

    for ( i = 1; i < 3; i++ ) 
	{
        if ( d [ i ] < min )
		{
            min     = d [ i ];
            r       = v [ i ];
			cl_line = i;
        }
    }

	*line_dir = ( cl_line < 2 ) ? g_pCurrentPoly->triangle [ cl_line+1 ].vecPosition - g_pCurrentPoly->triangle [ cl_line ].vecPosition : g_pCurrentPoly->triangle [ 0 ].vecPosition - g_pCurrentPoly->triangle [ cl_line ].vecPosition;

	return r;
}

float intersect ( const D3DXVECTOR3& r0, const D3DXVECTOR3& rn, const D3DXVECTOR3& p0, const D3DXVECTOR3& pn )
{
    float d = -D3DXVec3Dot ( &pn, &p0 );

    float numer = D3DXVec3Dot ( &pn, &r0 ) + d;
    float denom = D3DXVec3Dot ( &pn, &rn );

// before EAT
//	if ( denom <= -0.001f || denom >= 0.001f )
	if ( denom != 0.0f )
	    return -( numer / denom );
    
    return -1.0f;
}

bool intersect_plane ( const D3DXVECTOR3& start, const D3DXVECTOR3& dir,  const D3DXVECTOR3& pt, D3DXVECTOR3& ret )
{
	const D3DXVECTOR3& n = g_pCurrentPoly->normal;
	float denom = D3DXVec3Dot ( &n, &dir );

	if ( denom <= -EPSILON || denom >= EPSILON )
	{
		float numer = D3DXVec3Dot ( &n, &( pt - start ) );
		float u = numer / denom;

		if ( u > 0 && u <= 1.0f )
		{
			ret = start + dir * u;
			return true;
		}
	}
	return false;
}

float intersect_sphere ( const D3DXVECTOR3& r, const D3DXVECTOR3& rv, const D3DXVECTOR3& s, float sr )
{
    D3DXVECTOR3 q = s - r;
    float c = D3DXVec3Length ( &q );
    float v = D3DXVec3Dot ( &q, &rv );
    float d = sr * sr - ( c * c - v * v );

    if ( d < 0.0f )
		return -3.0f;

    return v - ( float ) sqrt ( d );
}

bool check_collision ( collision_data& coldat, float radius )
{
	// required initial data
    D3DXVECTOR3*	ppt			= &g_pCurrentPoly->triangle [ 0 ].vecPosition;
	D3DXVECTOR3*	pn			= &g_pCurrentPoly->normal;
	D3DXVECTOR3		s			= coldat.src - *pn * radius;
    D3DXVECTOR3		r;

	// calculate distance from closest point of old-sphere-pos (s) to plane of polygon
	float t = D3DXVec3Dot ( &( s - *ppt ), pn );

	// no collision if sphere not near enough to plane
    if ( t > coldat.dir_len )
		return false;

    // also no collision if sphere is so far through the plane the opposite side of sphere does not intersect either
    if ( t < -2 * radius )
		return false;
    
	// sphere gone beyond sliding-collision
	if ( t < 0.0f ) 
	{
		// closest point on sphere (S) has penetrated plane so we check backwards back to source
		if ( !intersect_plane ( s, *pn * radius, *ppt, r ) )
			return false;
    }
    else
	{
		// clostest point on sphere (S) is hovering just above plane (need a final intersect test)
		if ( !intersect_plane ( s, coldat.dir, *ppt, r ) )
			return false;
    }

	// R has been created; the vector at the point the sphere touches the plane

	// determine if ray intersects actual polygon represented so far by plane
	if ( !point_in_poly ( r ) )
	{
		// no, it misses so find out nearest vector on polygon
		D3DXVECTOR3 line_dir;
        r = closest_on_poly ( r, &line_dir );

		// work out if new position and nearest vector within range (t!=1.0f if so)
		t = intersect_sphere ( r, -coldat.ndir, coldat.src, radius );
	}
	else
	{
		// yes, ray goes right through polygon (t!=1.0f if so)
		t = intersect_sphere ( r, -coldat.ndir, coldat.src, radius );
	}

	// T has been updated; represents the actual distance from the center of the sphere to intersection point at R

	// distance must still be valid
	if ( t >= 0.0f && t <= coldat.dir_len )
	{
		// choose best 'closest' collision to store
		if ( !coldat.found || t < fabs(coldat.dist) )
		{
			// basic collision found
			coldat.found        = true;
			coldat.dist         = t;
			coldat.nearest_poly = r;

			// collision data from polygon pool
			coldat.diffuseid	= g_pCurrentPoly->diffuseid;

			// additional collision data
			D3DXIntersectTri ( 	&g_pCurrentPoly->triangle [ 0 ].vecPosition,
								&g_pCurrentPoly->triangle [ 1 ].vecPosition,
								&g_pCurrentPoly->triangle [ 2 ].vecPosition,
								&s,
								&coldat.ndir,
								&coldat.tu,
								&coldat.tv,
								&coldat.dist1 );

			// polygon collision detected
			return true;
		}
	}
	
	// no collision with this polygon
	return false;
}

void SetLength ( D3DXVECTOR3& v, float l )
{
	float len = sqrt ( v.x * v.x + v.y * v.y + v.z * v.z );

	v.x *= l / len;
	v.y *= l / len;
	v.z *= l / len;
}

D3DXVECTOR3 check_collisions ( D3DXVECTOR3 src, D3DXVECTOR3 dir, D3DXVECTOR3 eRadius, float fScale )
{
	cutoff++;
// before EAT
//	if ( cutoff > cutoff_max )
//		return src;

	// clear collision data store for polygon pass
	collision_data coldat;
	memset(&coldat,0,sizeof(coldat));
    coldat.src     = src;
    coldat.dir     = dir;
	coldat.dir_len = D3DXVec3Length ( &dir );

	// used to know when require no more passes
// before EAT
//	if ( coldat.dir_len < 0.001f )
//		return src;

	// calculate normal for volume movement determination
    coldat.ndir           = coldat.dir / coldat.dir_len;

	// go through every polygon in pool
	for ( int i = 0; i < g_PolygonList.size ( ); i++ )
	{
		// check each polygon against volume
		g_pCurrentPoly = g_PolygonList [ i ];
		if ( check_collision ( coldat, fScale ) )
		{
			// poly collision found, store it
			g_CollisionData = coldat;
		}
	}

	// if collision occuring
	if ( coldat.found )
	{
		// if deemed to have gone beyond sliding-collision range
		D3DXVECTOR3 newdir;
		D3DXVECTOR3 s = src;

		// calc advanced location of moving-volume
// before EAT
//		if ( coldat.dist >= 0.001f )
//			s += coldat.ndir * ( coldat.dist - 0.001f );
		s += coldat.ndir * coldat.dist;

		// work out new direction of moving-volume
		D3DXVECTOR3 dst = src + dir;
		D3DXVECTOR3 n = s - coldat.nearest_poly;
		D3DXVec3Normalize ( &n, &n );
		float t = intersect ( dst, n, coldat.nearest_poly, n );
		D3DXVECTOR3 newdst = dst + n * t;
		newdir = newdst - coldat.nearest_poly;

		// register collision with global state
		clipped = TRUE;

		// enter another pass to resolve new moving-volume collision
		return check_collisions ( s, newdir, eRadius, fScale );
	}

	// return with correct sliding-collision position
// before EAT
//	return src + ( coldat.ndir * ( coldat.dir_len - 0.001f ) );
	return src + ( coldat.ndir * coldat.dir_len );
}

BOOL Collision::World ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos, int cut, float fScale )
{
	// set globals
	clipped = FALSE;
	cutoff = 0;
	cutoff_max = cut;

	// used to store final collision feedback
	memset ( &g_CollisionData, 0, sizeof ( g_CollisionData ) );
	memset ( &g_CollisionPerformance, 0, sizeof ( g_CollisionPerformance ) );

	// ellipse denom
	o_pos.x /= eRadius.x;
	o_pos.y /= eRadius.y;
	o_pos.z /= eRadius.z;
	n_pos.x /= eRadius.x;
	n_pos.y /= eRadius.y;
	n_pos.z /= eRadius.z;

	// find collision
	g_DBPROCollisionResult.fClosestDist = 99999.0f;
	DWORD dwT3=timeGetTime();
	D3DXVECTOR3 pos = check_collisions ( o_pos, n_pos - o_pos, eRadius, fScale );
	DWORD dwT4=timeGetTime();

	// ellipse denom
	pos.x *= eRadius.x;
	pos.y *= eRadius.y;
	pos.z *= eRadius.z;
	o_pos.x *= eRadius.x;
	o_pos.y *= eRadius.y;
	o_pos.z *= eRadius.z;

	// fill collision feedback data to result
	if ( clipped )
	{
		// basic collision feedback
		g_DBPROCollisionResult.vecPos = pos;
		g_DBPROCollisionResult.vecDifference = pos - o_pos;
		g_DBPROCollisionResult.iPolysChecked = g_PolygonList.size();

		// additional collision feedback
		g_DBPROCollisionResult.iTextureIndex = 0;//this data needs to be added higher up
		g_DBPROCollisionResult.fTextureU = g_CollisionData.tu;
		g_DBPROCollisionResult.fTextureV = g_CollisionData.tv;
		g_DBPROCollisionResult.vecNormal = g_CollisionData.ndir;
		g_DBPROCollisionResult.dwArbitaryValue = g_CollisionData.diffuseid;
	}

	// return new position if requested
	if ( out_pos != NULL )
		*out_pos = pos;

	return clipped;
}
