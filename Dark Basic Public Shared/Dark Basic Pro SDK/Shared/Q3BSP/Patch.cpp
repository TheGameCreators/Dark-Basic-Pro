
#include "cBSPFile.h"

#include < vector >

using namespace std;

#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)

D3DXVECTOR2 vec2_interpolate ( D3DXVECTOR2 a, D3DXVECTOR2 b )
{
    return ( a + b ) * 0.5f;
}

D3DXVECTOR3 vec3_interpolate ( D3DXVECTOR3 a, D3DXVECTOR3 b )
{
    return ( a + b ) * 0.5f;
}

int find_level ( D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c )
{
	// mike - 230604
	const int   max_levels = 200;
    const float subdiv     = 200;

    float test = subdiv * subdiv;

    D3DXVECTOR3 s,t,d;
	int level = 0;
    
	for ( level = 0; level < max_levels - 1; level++ )
	{
        s = vec3_interpolate ( a, b );
        t = vec3_interpolate ( b, c );
        c = vec3_interpolate ( s, t );
        d = c - b;

        if ( D3DXVec3Dot ( &d, &d ) < test )
		{
            break;
        }

        b = a;
    }

    return level;
}

void fill_curve ( int step, int size, int stride, CUSTOMVERTEX* p )
{
    while ( step > 0 )
	{
        int halfstep = step / 2;

        for ( int i = 0; i < size - 1; i += step * 2 )
		{
            int left  = i * stride;
            int mid   = ( i + step     ) * stride;
            int right = ( i + step * 2 ) * stride;

            int half_left  = ( i + halfstep     ) * stride;
            int half_right = ( i + halfstep * 3 ) * stride;

            D3DXVECTOR3 a3, b3;
            D3DXVECTOR2 a2 ,b2;

            // vertex
            a3 = vec3_interpolate ( p [ left ].v, p[ mid   ].v );
            b3 = vec3_interpolate ( p [ mid  ].v, p[ right ].v );

            p [ mid ].v = vec3_interpolate ( a3, b3 );

            if ( halfstep > 0 )
			{
                p [ half_left  ].v = a3;
                p [ half_right ].v = b3;
            }

            // texcoords
			a2 = vec2_interpolate ( D3DXVECTOR2 ( p [ left ].tu, p [ left ].tv ), D3DXVECTOR2 ( p [ mid   ].tu, p [ mid   ].tv ) );
            b2 = vec2_interpolate ( D3DXVECTOR2 ( p [ mid  ].tu, p [ mid  ].tv ), D3DXVECTOR2 ( p [ right ].tu, p [ right ].tv ) );
            
			D3DXVECTOR2 tex_st = vec2_interpolate ( a2, b2 );

			p [ mid ].tu = tex_st.x;
			p [ mid ].tv = tex_st.y;

            if ( halfstep > 0 )
			{
                p [ half_left  ].tu = a2.x;
				p [ half_left  ].tv = a2.y;
                p [ half_right ].tu = b2.x;
				p [ half_right ].tv = b2.y;
            }

			D3DCOLOR col1 = p [ left  ].color;
			D3DCOLOR col2 = p [ right ].color;

			int r,g,b,a;

			r = RGBA_GETRED   ( col1 ) + RGBA_GETRED   ( col2 );
			g = RGBA_GETGREEN ( col1 ) + RGBA_GETGREEN ( col2 );
			b = RGBA_GETBLUE  ( col1 ) + RGBA_GETBLUE  ( col2 );
			a = RGBA_GETALPHA ( col1 ) + RGBA_GETALPHA ( col2 );

			p [ mid ].color = D3DCOLOR_RGBA ( r / 2, g / 2, b / 2, a / 2 );

            // lightmap coords
			a2 = vec2_interpolate ( D3DXVECTOR2 ( p [ left ].lu, p [ left ].lv ), D3DXVECTOR2 ( p [ mid   ].lu, p [mid    ].lv ) );
            b2 = vec2_interpolate ( D3DXVECTOR2 ( p [ mid  ].lu, p [ mid  ].lv ), D3DXVECTOR2 ( p [ right ].lu, p [ right ].lv ) );
			
			D3DXVECTOR2 lm_st = vec2_interpolate ( a2, b2 );

			p [ mid ].lu = lm_st.x;
			p [ mid ].lv = lm_st.y;

            if ( halfstep > 0 )
			{
                p [ half_left  ].lu = a2.x;
				p [ half_left  ].lv = a2.y;
                p [ half_right ].lu = b2.x;
				p [ half_right ].lv = b2.y;
            }
		}

        step /= 2;
    }
}

int create_patch ( int face_idx )
{
	const face_t3& f = faces3 [ face_idx ];
    int	  cpx_count  = f.mesh_cp [ 0 ];
    int   cpy_count  = f.mesh_cp [ 1 ];

    vector < D3DXVECTOR3 > cpoints;

    {
        for ( int i = 0; i < f.vert_count; i++ )
	    {
            D3DXVECTOR3 p = vertices3 [ f.vert_start + i ].point;
            cpoints.push_back ( p );
        }
    }

    int u, v;
    
    D3DXVECTOR3 a,b,c;

    bool found = false;

    for ( v = 0; v < cpy_count; v++ )
	{
        for ( u = 0; u < cpx_count - 1; u += 2 )
		{
            a = cpoints [ v * cpx_count + u     ];
            b = cpoints [ v * cpx_count + u + 1 ];
            c = cpoints [ v * cpx_count + u + 2 ];

            if ( a != c )
			{
                found = true;
                break;
            }
        }

        if ( found )
			break;
    }

    if ( !found )
	    return -1;
    
    int level  = find_level ( a, b, c );
    int u_size = ( LEVEL_WIDTH ( level ) - 1 ) * ( ( cpx_count - 1 ) / 2 ) + 1;
    
    found = false;

    for ( u = 0; u < cpx_count; u++ )
	{
        for ( v = 0; v < cpy_count-1; v += 2 )
		{
            a = cpoints [ v * cpx_count + u         ];
            b = cpoints [ ( v + 1 ) * cpx_count + u ];
            c = cpoints [ ( v + 2 ) * cpx_count + u ];

            if ( a != c ) 
			{
                found = true;
                break;
            }
        }

        if ( found )
			break;
    }

    if ( !found )
        return -1;

    level = find_level ( a, b, c );
    int v_size = ( LEVEL_WIDTH ( level ) - 1 ) * ( ( cpy_count - 1 ) / 2 ) + 1;
    

    mesh_t3 mesh;
    mesh.verts_count = u_size * v_size;
	mesh.verts       = ( CUSTOMVERTEX* ) malloc ( sizeof ( CUSTOMVERTEX ) * mesh.verts_count );

    int u_step = ( u_size - 1 ) / ( cpx_count - 1 );
    int v_step = ( v_size - 1 ) / ( cpy_count - 1 );

    // fill sparse control points
    const vertex_t3* s = vertices3 + f.vert_start;

    CUSTOMVERTEX* d = &mesh.verts [ 0 ];

	int i = 0;

    for ( v = 0; v < v_size; v += v_step )
	{
        for ( u = 0; u < u_size; u += u_step )
		{
            i = v * u_size + u;

			d [ i ].v   = ConvertVectorFromQ3 ( s->point );
            d [ i ].tu  = s->texture  [ 0 ];
			d [ i ].tv  = s->texture  [ 1 ];
            d [ i ].lu  = s->lightmap [ 0 ];
			d [ i ].lv  = s->lightmap [ 1 ];
            d [ i ].n.x = s->normal   [ 0 ];
			d [ i ].n.z = s->normal   [ 1 ];
			d [ i ].n.y = s->normal   [ 2 ];

			D3DCOLOR col = s->color;
			int r, g, b, a;

			r = RGBA_GETRED   ( col );
			g = RGBA_GETGREEN ( col );
			b = RGBA_GETBLUE  ( col );
			a = RGBA_GETALPHA ( col );

			d [ i ].color = D3DCOLOR_RGBA ( b, g, r, a );

            s++;
        }
    }

    // fill curves
    CUSTOMVERTEX* p = &mesh.verts [ 0 ];

    for ( u = 0; u < u_size; u += u_step )
	{
        fill_curve ( v_step, v_size, u_size, p + u );
    }

    for ( v = 0; v < v_size; v++ )
	{
        fill_curve ( u_step, u_size, 1, p + v * u_size );
    }

    // fill elements
    mesh.elems_count = ( u_size - 1 ) * ( v_size - 1 ) * 6;
	mesh.elems       = ( UINT16* ) malloc ( sizeof ( UINT16 ) * mesh.elems_count );
    i = 0;

    for ( v = 0; v < v_size-1; ++v )
	{
	    for ( u = 0; u < u_size - 1; ++u )
		{
	        mesh.elems [ i++ ] = v * u_size + u;
	        mesh.elems [ i++ ] = ( v + 1) * u_size + u;
	        mesh.elems [ i++ ] = v * u_size + u + 1;
	        mesh.elems [ i++ ] = v * u_size + u + 1;
	        mesh.elems [ i++ ] = ( v + 1 ) * u_size + u;
	        mesh.elems [ i++ ] = ( v + 1 ) * u_size + u + 1;
	    }
    }
	
	Q3MAP.meshes [ Q3MAP.mesh_count ] = mesh;
	Q3MAP.mesh_count++;

    return Q3MAP.mesh_count - 1;
}


/*
#include "cBSPFile.h"

#include < vector >

using namespace std;

#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)

D3DXVECTOR2 vec2_interpolate ( D3DXVECTOR2 a, D3DXVECTOR2 b )
{
    return ( a + b ) * 0.5f;
}

D3DXVECTOR3 vec3_interpolate ( D3DXVECTOR3 a, D3DXVECTOR3 b )
{
    return ( a + b ) * 0.5f;
}

int find_level ( D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c )
{
	// MIKE FIX 190303 - higher resolution for patches
	//const int   max_levels = 4;
    //const float subdiv     = 10;

	//const int   max_levels = 2;
    //const float subdiv     = 2;

	const int   max_levels = 400;
    const float subdiv     = 400;

    float test = subdiv * subdiv;

    D3DXVECTOR3 s,t,d;
    
	for ( int level = 0; level < max_levels - 1; level++ )
	{
        s = vec3_interpolate ( a, b );
        t = vec3_interpolate ( b, c );
        c = vec3_interpolate ( s, t );
        d = c - b;

        if ( D3DXVec3Dot ( &d, &d ) < test )
		{
            break;
        }

        b = a;
    }

    return level;
}

void fill_curve ( int step, int size, int stride, CUSTOMVERTEX* p )
{
    while ( step > 0 )
	{
        int halfstep = step / 2;

        for ( int i = 0; i < size - 1; i += step * 2 )
		{
            int left  = i * stride;
            int mid   = ( i + step     ) * stride;
            int right = ( i + step * 2 ) * stride;

            int half_left  = ( i + halfstep     ) * stride;
            int half_right = ( i + halfstep * 3 ) * stride;

            D3DXVECTOR3 a3, b3;
            D3DXVECTOR2 a2 ,b2;

            // vertex
            a3 = vec3_interpolate ( p [ left ].v, p[ mid   ].v );
            b3 = vec3_interpolate ( p [ mid  ].v, p[ right ].v );

            p [ mid ].v = vec3_interpolate ( a3, b3 );

            if ( halfstep > 0 )
			{
                p [ half_left  ].v = a3;
                p [ half_right ].v = b3;
            }

            // texcoords
			a2 = vec2_interpolate ( D3DXVECTOR2 ( p [ left ].tu, p [ left ].tv ), D3DXVECTOR2 ( p [ mid   ].tu, p [ mid   ].tv ) );
            b2 = vec2_interpolate ( D3DXVECTOR2 ( p [ mid  ].tu, p [ mid  ].tv ), D3DXVECTOR2 ( p [ right ].tu, p [ right ].tv ) );
            
			D3DXVECTOR2 tex_st = vec2_interpolate ( a2, b2 );

			p [ mid ].tu = tex_st.x;
			p [ mid ].tv = tex_st.y;

            if ( halfstep > 0 )
			{
                p [ half_left  ].tu = a2.x;
				p [ half_left  ].tv = a2.y;
                p [ half_right ].tu = b2.x;
				p [ half_right ].tv = b2.y;
            }

			D3DCOLOR col1 = p [ left  ].color;
			D3DCOLOR col2 = p [ right ].color;

			int r,g,b,a;

			r = RGBA_GETRED   ( col1 ) + RGBA_GETRED   ( col2 );
			g = RGBA_GETGREEN ( col1 ) + RGBA_GETGREEN ( col2 );
			b = RGBA_GETBLUE  ( col1 ) + RGBA_GETBLUE  ( col2 );
			a = RGBA_GETALPHA ( col1 ) + RGBA_GETALPHA ( col2 );

			p [ mid ].color = D3DCOLOR_RGBA ( r / 2, g / 2, b / 2, a / 2 );

            // lightmap coords
			a2 = vec2_interpolate ( D3DXVECTOR2 ( p [ left ].lu, p [ left ].lv ), D3DXVECTOR2 ( p [ mid   ].lu, p [mid    ].lv ) );
            b2 = vec2_interpolate ( D3DXVECTOR2 ( p [ mid  ].lu, p [ mid  ].lv ), D3DXVECTOR2 ( p [ right ].lu, p [ right ].lv ) );
			
			D3DXVECTOR2 lm_st = vec2_interpolate ( a2, b2 );

			p [ mid ].lu = lm_st.x;
			p [ mid ].lv = lm_st.y;

            if ( halfstep > 0 )
			{
                p [ half_left  ].lu = a2.x;
				p [ half_left  ].lv = a2.y;
                p [ half_right ].lu = b2.x;
				p [ half_right ].lv = b2.y;
            }
		}

        step /= 2;
    }
}

int create_patch ( int face_idx )
{
	const face_t3& f = faces3 [ face_idx ];
    int	  cpx_count  = f.mesh_cp [ 0 ];
    int   cpy_count  = f.mesh_cp [ 1 ];

    vector < D3DXVECTOR3 > cpoints;

    for ( int i = 0; i < f.vert_count; i++ )
	{
        D3DXVECTOR3 p = vertices3 [ f.vert_start + i ].point;
        cpoints.push_back ( p );
    }

    int u, v;
    
    D3DXVECTOR3 a,b,c;

	bool found = false;

    for ( v = 0; v < cpy_count; v++ )
	{
        for ( u = 0; u < cpx_count - 1; u += 2 )
		{
            a = cpoints [ v * cpx_count + u     ];
            b = cpoints [ v * cpx_count + u + 1 ];
            c = cpoints [ v * cpx_count + u + 2 ];

            if ( a != c )
			{
                found = true;
                break;
            }
        }

        if ( found )
			break;
    }

    if ( !found )
	    return -1;
    
    int level  = find_level ( a, b, c );
    int u_size = ( LEVEL_WIDTH ( level ) - 1 ) * ( ( cpx_count - 1 ) / 2 ) + 1;
    
    found = false;

    for ( u = 0; u < cpx_count; u++ )
	{
        for ( v = 0; v < cpy_count-1; v += 2 )
		{
            a = cpoints [ v * cpx_count + u         ];
            b = cpoints [ ( v + 1 ) * cpx_count + u ];
            c = cpoints [ ( v + 2 ) * cpx_count + u ];

            if ( a != c ) 
			{
                found = true;
                break;
            }
        }

        if ( found )
			break;
    }

    if ( !found )
        return -1;

    level = find_level ( a, b, c );
    int v_size = ( LEVEL_WIDTH ( level ) - 1 ) * ( ( cpy_count - 1 ) / 2 ) + 1;
    /////////////////////// HERE

    mesh_t3 mesh;
    mesh.verts_count = u_size * v_size;
	mesh.verts       = ( CUSTOMVERTEX* ) malloc ( sizeof ( CUSTOMVERTEX ) * mesh.verts_count );

    int u_step = ( u_size - 1 ) / ( cpx_count - 1 );
    int v_step = ( v_size - 1 ) / ( cpy_count - 1 );

    // fill sparse control points
    const vertex_t3* s = vertices3 + f.vert_start;

    CUSTOMVERTEX* d = &mesh.verts [ 0 ];

    for ( v = 0; v < v_size; v += v_step )
	{
        for ( u = 0; u < u_size; u += u_step )
		{
            int i = v * u_size + u;

			d [ i ].v   = ConvertVectorFromQ3 ( s->point );
            d [ i ].tu  = s->texture  [ 0 ];
			d [ i ].tv  = s->texture  [ 1 ];
            d [ i ].lu  = s->lightmap [ 0 ];
			d [ i ].lv  = s->lightmap [ 1 ];
            d [ i ].n.x = s->normal   [ 0 ];
			d [ i ].n.z = s->normal   [ 1 ];
			d [ i ].n.y = s->normal   [ 2 ];

			D3DCOLOR col = s->color;
			int r, g, b, a;

			r = RGBA_GETRED   ( col );
			g = RGBA_GETGREEN ( col );
			b = RGBA_GETBLUE  ( col );
			a = RGBA_GETALPHA ( col );

			d [ i ].color = D3DCOLOR_RGBA ( b, g, r, a );

            s++;
        }
    }

    // fill curves
    CUSTOMVERTEX* p = &mesh.verts [ 0 ];

    for ( u = 0; u < u_size; u += u_step )
	{
        fill_curve ( v_step, v_size, u_size, p + u );
    }

	for ( v = 0; v < v_size; v++ )
	{
        fill_curve ( u_step, u_size, 1, p + v * u_size );
    }

    // fill elements
    mesh.elems_count = ( u_size - 1 ) * ( v_size - 1 ) * 6;
	mesh.elems       = ( UINT16* ) malloc ( sizeof ( UINT16 ) * mesh.elems_count );
    i = 0;

    for ( v = 0; v < v_size-1; ++v )
	{
	    for ( u = 0; u < u_size - 1; ++u )
		{
	        mesh.elems [ i++ ] = v * u_size + u;
	        mesh.elems [ i++ ] = ( v + 1) * u_size + u;
	        mesh.elems [ i++ ] = v * u_size + u + 1;
	        mesh.elems [ i++ ] = v * u_size + u + 1;
	        mesh.elems [ i++ ] = ( v + 1 ) * u_size + u;
	        mesh.elems [ i++ ] = ( v + 1 ) * u_size + u + 1;
	    }
    }
	
	Q3MAP.meshes [ Q3MAP.mesh_count ] = mesh;
	Q3MAP.mesh_count++;

    return Q3MAP.mesh_count - 1;
}
*/