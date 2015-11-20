#include "collision.h"

#include "cQ2BSP.h"

_col_opt col_opt_Q12;
BOOL onfloor, onceil;

extern void CalcNormal  ( D3DXVECTOR3 v [ 3 ], D3DXVECTOR3* out );
extern void CalcNormal2 ( D3DXVECTOR3* v, int v_count, D3DXVECTOR3* out );
BOOL BoundElipsoidIntersect ( D3DXVECTOR3 mid, D3DXVECTOR3 e, D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos );

extern D3DXVECTOR3 ConvertVectorFromQ3 ( D3DXVECTOR3 src );
extern D3DXVECTOR3 ConvertVectorFromQ3 ( float src [ 3 ] );
extern D3DXVECTOR3 ConvertVectorFromQ3 ( int src [ 3 ] );
extern D3DXVECTOR3 ConvertVectorFromQ3 ( short src [ 3 ] );
extern D3DXVECTOR3 ConvertVectorToQ3 ( D3DXVECTOR3 src );
extern float       ConvertVectorToQ3 ( float src );


void FreeColOpt(void)
{
	if(col_opt_Q12.check)
	{
		free ( col_opt_Q12.check );
		col_opt_Q12.check=NULL;
	}
}

BOOL BoundElipsoidIntersect ( D3DXVECTOR3 mid, D3DXVECTOR3 e, D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos )
{
	mid.x /= e.x;
	mid.y /= e.y;
	mid.z /= e.z;

	o_pos.x /= e.x;
	o_pos.y /= e.y;
	o_pos.z /= e.z;

	n_pos.x /= e.x;
	n_pos.y /= e.y;
	n_pos.z /= e.z;

	if ( D3DXSphereBoundProbe ( &mid, 1, &o_pos, &( n_pos - o_pos ) ) &&
		D3DXSphereBoundProbe ( &mid, 1, &n_pos, &( o_pos - n_pos ) ) )
			return TRUE;

	return FALSE;
}

BOOL CheckPointInSphere ( D3DXVECTOR3 point, D3DXVECTOR3 sO, double sR )
{
	float d = D3DXVec3Length ( &( point - sO ) );
	
	if ( d <= sR )
		return TRUE;

	return FALSE;	
}

struct _cache 
{
	D3DXVECTOR3 vertex [ 32 ];
	D3DXVECTOR3 normal;
	int count;
} cache;

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
		D3DXVECTOR3 nearest_poly;

		D3DXVECTOR3 BoundingSphere;
};

static bool point_in_poly ( const D3DXVECTOR3& p )
{
    D3DXVECTOR3 v [ 32 ];

    for ( int i = 0; i < cache.count; i++ )
	{
        v[i] = p - cache.vertex[i];
        D3DXVec3Normalize(&v[i],&v[i]);
    }

    float total = 0.0f;

    for ( i = 0; i < cache.count - 1; i++ )
	{
		total += ( float ) acos ( D3DXVec3Dot ( &v [ i ], &v [ i + 1 ] ) );
	}

    total += ( float ) acos ( D3DXVec3Dot ( &v [ cache.count - 1 ], &v [ 0 ] ) );

    if ( fabsf ( total - 6.28f ) < 0.01f )
	{
        return true;
    }

    return false;
}

static D3DXVECTOR3 closest_on_line ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& p )
{
    D3DXVECTOR3 c = p - a;
    D3DXVECTOR3 v = b - a;

    float d = D3DXVec3Length ( &v );

    if ( d )
		v /= d;

    float t = D3DXVec3Dot ( &v, &c );
    
	if (t < 0.0f )
		return a;

    if ( t > d )
		return b;

    return a + v * t;
}

static D3DXVECTOR3 closest_on_poly ( const D3DXVECTOR3& p, D3DXVECTOR3* line_dir )
{
    D3DXVECTOR3 v [ 32 ];
    float d [ 32 ];

    for ( int i = 0; i < cache.count - 1; i++ )
	{
        v [ i ] = closest_on_line ( cache.vertex [ i ], cache.vertex [ i + 1 ], p );

        D3DXVECTOR3 t = p - v [ i ];
        
		d [ i ] = D3DXVec3Length ( &t );
    }

    i = cache.count-1;

    v [ i ] = closest_on_line ( cache.vertex [ i ], cache.vertex [ 0 ], p );
    
	D3DXVECTOR3 t = p - v [ i ];

    d [ i ] = D3DXVec3Length ( &t );

    float min = d [ 0 ];
    D3DXVECTOR3 r = v [ 0 ];
	int cl_line = 0;

    for ( i = 1; i < cache.count; i++ ) 
	{
        if ( d [ i ] < min )
		{
            min     = d [ i ];
            r       = v [ i ];
			cl_line = i;
        }
    }

	// calculate direction vector of closest line
	if ( cl_line < cache.count - 1)  
		*line_dir = cache.vertex [ cl_line + 1 ] - cache.vertex [ cl_line ];
	else
		*line_dir = cache.vertex [ 0 ] - cache.vertex [ cl_line ];

    return r;
}

static float intersect ( const D3DXVECTOR3& r0, const D3DXVECTOR3& rn, const D3DXVECTOR3& p0, const D3DXVECTOR3& pn )
{
    float d = -D3DXVec3Dot ( &pn, &p0 );

    float numer = D3DXVec3Dot ( &pn, &r0 ) + d;
    float denom = D3DXVec3Dot ( &pn, &rn );
  
    if ( denom <= -0.001f || denom >= 0.001f )
	{
        return -( numer / denom );
    }
    return -1.0f;
}

#define EPSILON (1.0e-05f)

bool intersect_plane ( const D3DXVECTOR3& start, const D3DXVECTOR3& dir,  const D3DXVECTOR3& pt, D3DXVECTOR3& ret )
{
	const D3DXVECTOR3& n = cache.normal;
	float denom = D3DXVec3Dot ( &n,&dir );

	if ( denom <= -EPSILON || denom >= EPSILON )
	{
		float numer = D3DXVec3Dot ( &n,&( pt - start ) );
		float u = numer / denom;

		if ( u > 0 && u <= 1.0f )
		{
			ret = start + dir * u;
			return true;
		}
	}
	return false;
}

static float intersect_sphere ( const D3DXVECTOR3& r, const D3DXVECTOR3& rv, const D3DXVECTOR3& s, float sr )
{
    D3DXVECTOR3 q = s - r;
    float c = D3DXVec3Length ( &q );
    float v = D3DXVec3Dot ( &q, &rv );
    float d = sr * sr - (c * c - v * v );

    if ( d < 0.0f )
		return -1.0f;

    return v - sqrt ( d );
}

D3DXVECTOR3 thepos;
static void check_collision ( collision_data& coldat )
{
	float radius;

	if ( coldat.BoundingSphere.x == coldat.BoundingSphere.y && coldat.BoundingSphere.x == coldat.BoundingSphere.z )
	{
		radius = coldat.BoundingSphere.x;
	}
	else
	{
		radius = 1.0f;
		
		// scale polygon
		for ( small x = 0; x < cache.count; x++ )
		{
			cache.vertex [ x ].x /= coldat.BoundingSphere.x;
			cache.vertex [ x ].y /= coldat.BoundingSphere.y;
			cache.vertex [ x ].z /= coldat.BoundingSphere.z;
		}

		CalcNormal ( cache.vertex, &cache.normal );
		D3DXVec3Normalize ( &cache.normal, &cache.normal );
	}


    D3DXVECTOR3 r;
    D3DXVECTOR3 pt = cache.vertex [ 0 ];
    D3DXVECTOR3 n  = cache.normal;
    D3DXVECTOR3 s  = coldat.src - n * radius;
	
	float t = D3DXVec3Dot ( &( s - pt ), &n );

    if ( t > coldat.dir_len )
	{
        return;
    }

    if ( t < -2 * radius )
	{
        return;
    }

    if ( t < 0.0f ) 
	{
        if ( !intersect_plane ( s, n * radius, pt, r ) )
			return;
    }
    else 
	{
     	if ( !intersect_plane ( s, coldat.dir, pt, r ) )
			return;
    }

    if ( !point_in_poly ( r ) )
	{
		D3DXVECTOR3 line_dir;
        r = closest_on_poly ( r, &line_dir );
		
		// mj change - sticky collision fix 27/08/02
		D3DXVECTOR3 ndir;
		//D3DXVec3Cross ( &ndir, &coldat.ndir, &line_dir );
		//D3DXVec3Cross ( &ndir, &line_dir, &ndir );
		//D3DXVec3Normalize ( &ndir, &ndir );

		ndir = coldat.ndir;

		t = intersect_sphere ( r, -ndir, coldat.src, radius );
	}
	else
	{
		t = intersect_sphere ( r, -coldat.ndir, coldat.src, radius );
	}

	    if ( t >= 0.0f && t <= coldat.dir_len )
		{
			if ( !coldat.found || t < coldat.dist )
			{
				coldat.found        = true;
				coldat.dist         = t;
				coldat.nearest_poly = r;
        }
    }
}

static int cutoff, cutoff_max;

BOOL clipped; 

float g_fCollisionThreshold = 0.003f;

static D3DXVECTOR3 check_collisions ( D3DXVECTOR3 src, D3DXVECTOR3 dir, D3DXVECTOR3 eRadius )
{
	cutoff++;

    if ( cutoff > cutoff_max )
		return src;

	collision_data coldat;
    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = D3DXVec3Length ( &dir );
	
	//if ( coldat.dir_len < 0.001f )
	//if ( coldat.dir_len < 0.01f )
	if ( coldat.dir_len < g_fCollisionThreshold )
		return src;

    coldat.ndir = coldat.dir / coldat.dir_len;

	coldat.BoundingSphere = eRadius;

	for ( int x = 0; x < numfaces; x++ )
	{
		if ( !col_opt_Q12.check [ x ] )
			continue;

		cache.count  = 3;
		cache.normal = QMAP.v [ QMAP.faces [ x ].first ].n;

		for ( int y = 0; y < QMAP.faces [ x ].count - 2; y++ )
		{
			cache.vertex [ 0 ] = QMAP.v [ QMAP.faces [ x ].first         ].v;
			cache.vertex [ 1 ] = QMAP.v [ QMAP.faces [ x ].first + y + 1 ].v;
			cache.vertex [ 2 ] = QMAP.v [ QMAP.faces [ x ].first + y + 2 ].v;

			check_collision ( coldat );
		}
	}

    if ( coldat.found )
	{
		D3DXVECTOR3 s = src;

		if ( coldat.dist >= 0.001f )
		{
            s += coldat.ndir * ( coldat.dist - 0.001f );
        }

		D3DXVECTOR3 dst = src + dir;
        D3DXVECTOR3 n = s - coldat.nearest_poly;

        D3DXVec3Normalize ( &n, &n );
		float t = intersect ( dst, n, coldat.nearest_poly, n );

        D3DXVECTOR3 newdst = dst + n * t;
        D3DXVECTOR3 newdir = newdst - coldat.nearest_poly;

		clipped = TRUE;

		if ( n.y > 0.3f )
			onfloor = TRUE;
		
		if ( n.y < -0.3f )
			onceil = TRUE;

		if ( onfloor )
		{
			float fMin = -0.002f;
			float fMax =  0.002f;

			float fMinX = -0.0002f;
			float fMaxX =  0.0002f;

			/*
			if ( newdir.y > fMin && newdir.y < fMax )
			{
				if ( newdir.x > fMin && newdir.x < fMax )
					newdir.x = 0.0f;

				//if ( newdir.z > fMin && newdir.z < fMax )
				//	newdir.z = 0.0f;
				
				newdir.y = 0.0f;
			}
			*/

			/*
			if ( newdir.y > fMin && newdir.y < fMax )
			{
				if ( newdir.x > fMinX && newdir.x < fMaxX )
				{
					newdir.x = 0.0f;
					newdir.y = 0.0f;
				}

				//newdir.y = 0.0f;
			}
			*/

			//if ( ( newdir.y > fMin && newdir.y < fMax ) && ( newdir.x > fMinX && newdir.x < fMaxX ) )
			//{
			//	newdir.y = 0.0f;
			//}
		}

		return check_collisions ( s, newdir, eRadius );
    }

	return src + ( coldat.ndir * ( coldat.dir_len - 0.001f ) );
}



BOOL Collision::Player ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 mid, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos )
{
	mid += D3DXVECTOR3 ( 0, 10.0, 0 );

	if ( BoundElipsoidIntersect ( mid, eRadius, o_pos, n_pos ) )
	{
		if ( out_pos != NULL )
			*out_pos=o_pos;

		return TRUE;
	}

	if ( out_pos != NULL )
		*out_pos = n_pos;

	return FALSE;
}

BOOL Collision::Box ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere, D3DXVECTOR3 box1, D3DXVECTOR3 box2, float* dist, D3DXVECTOR3* out_pos )
{
	return FALSE;
}

void optQ1 ( int n, D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere )
{
	D3DXVECTOR3 min, max;

	if ( n >= 0 )
	{
		min = ConvertVectorFromQ3 ( node_list1 [ n ].mins );
		max = ConvertVectorFromQ3 ( node_list1 [ n ].maxs );
	}
	else
	{
		min = ConvertVectorFromQ3 ( leaf_list1 [ -n - 1 ].mins );
		max = ConvertVectorFromQ3 ( leaf_list1 [ -n - 1 ].maxs );
	}

	float c = BoundingSphere;

	min -= D3DXVECTOR3 ( c, c, c );
	max += D3DXVECTOR3 ( c, c, c );

	if ( 
			D3DXBoxBoundProbe ( &min, &max, &o_pos, &( n_pos - o_pos ) ) &&
			D3DXBoxBoundProbe ( &min, &max, &n_pos, &( o_pos - n_pos ) )
	   )
	{
		if ( n < 0 )
		{
			for ( int x = 0; x < leaf_list1 [ -n - 1 ].nummarksurfaces; x++ )
			{
				int face_idx = marksurfaces_list [ leaf_list1 [ -n - 1 ].firstmarksurface + x ];

				if ( QMAP.faces [ face_idx ].flags & ( QSURF_DRAWSKY | QSURF_DRAWTURB | QSURF_UNDERWATER ) )
					continue;

				col_opt_Q12.check [ face_idx ] = TRUE;
			}

			return;
		}

		optQ1 ( node_list1 [ n ].children [ 0 ], o_pos, n_pos, BoundingSphere );
		optQ1 ( node_list1 [ n ].children [ 1 ], o_pos, n_pos, BoundingSphere );
	}
}

void optQ2 ( int n, D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere )
{
	D3DXVECTOR3 min, max;
	
	if ( n >= 0 )
	{
		min = ConvertVectorFromQ3 ( node_list2 [ n ].mins );
		max = ConvertVectorFromQ3 ( node_list2 [ n ].maxs );
	}
	else
	{
		min = ConvertVectorFromQ3 ( leaf_list2 [ -n - 1 ].mins );
		max = ConvertVectorFromQ3 ( leaf_list2 [ -n - 1 ].maxs );
	}

	float c = BoundingSphere;

	min -= D3DXVECTOR3 ( c, c, c );
	max += D3DXVECTOR3 ( c, c, c );

	if ( 
			D3DXBoxBoundProbe ( &min, &max, &o_pos, &( n_pos - o_pos ) ) &&
			D3DXBoxBoundProbe ( &min, &max, &n_pos, &( o_pos - n_pos ) )
	   )
	{
		if ( n < 0 )
		{
			for ( int x = 0; x < leaf_list2 [ -n - 1 ].numleaffaces; x++ )
			{
				int face_idx = leafface_list [ leaf_list2 [ -n - 1 ].firstleafface + x ];

				if ( QMAP.faces [ face_idx ].flags & ( QSURF_NODRAW | QSURF_HINT | QSURF_SKIP | QSURF_SKY ) )
					continue;

				col_opt_Q12.check [ face_idx ] = TRUE;
			}

			return;
		}

		optQ2 ( node_list2 [ n ].children [ 0 ], o_pos, n_pos, BoundingSphere );
		optQ2 ( node_list2 [ n ].children [ 1 ], o_pos, n_pos, BoundingSphere );
	}
}

BOOL Collision::Opt ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius )
{
	float BoundingSphere = eRadius.x;

	if ( eRadius.y > eRadius.x && eRadius.y > eRadius.z ) BoundingSphere = eRadius.y;
	if ( eRadius.z > eRadius.x && eRadius.z > eRadius.y ) BoundingSphere = eRadius.z;

	if ( numfaces != col_opt_Q12.check_count )
	{
		col_opt_Q12.check       = ( BOOL* ) realloc ( col_opt_Q12.check, sizeof ( BOOL ) * numfaces );
		col_opt_Q12.check_count = numfaces;
	}
	
	ZeroMemory ( col_opt_Q12.check, sizeof ( BOOL ) * numfaces );

	if ( QMAP.type == QUAKE1 )
		optQ1 ( 0, o_pos, n_pos, BoundingSphere );

	if ( QMAP.type == QUAKE2 )
		optQ2 ( 0, o_pos, n_pos, BoundingSphere );

	return TRUE;
}

BOOL Collision::World ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, D3DXVECTOR3 eRadius, D3DXVECTOR3* out_pos, int cut, float fThreshold )
{
	g_fCollisionThreshold = fThreshold;

	clipped = FALSE;
	onfloor = FALSE, onceil = FALSE;
	cutoff     = 0;
	cutoff_max = cut;

	Collision::Opt ( o_pos, n_pos, eRadius );

	if ( eRadius.x == eRadius.y && eRadius.x == eRadius.z )
	{
	
	}
	else
	{
		o_pos.x /= eRadius.x;
		o_pos.y /= eRadius.y;
		o_pos.z /= eRadius.z;
		n_pos.x /= eRadius.x;
		n_pos.y /= eRadius.y;
		n_pos.z /= eRadius.z;
	}

	
	D3DXVECTOR3 pos = check_collisions ( o_pos, n_pos - o_pos, eRadius );

	if ( eRadius.x == eRadius.y && eRadius.x == eRadius.z )
	{

	}
	else
	{
		pos.x *= eRadius.x;
		pos.y *= eRadius.y;
		pos.z *= eRadius.z;
	}

	if ( out_pos != NULL )
		*out_pos = pos;

	return clipped;
}
