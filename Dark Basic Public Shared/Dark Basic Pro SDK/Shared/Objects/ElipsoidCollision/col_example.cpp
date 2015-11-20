//Just example code for the collision DLL
typedef CollisionTrace* (*C1)(float* position,float* vel);
typedef void (*C2)(bool grav);
typedef void (*C3)(bool backcul);
typedef void (*C4)(bool resp);
typedef void (*C5)(float UPM);
typedef void (*C6)(float* grav );
typedef void (*C7)(float* radius);
typedef void (*C8)(int numtriangle, float* pool, int stride);

C1 CollideAndSlide;
C2 EnableGravity;
C3 EnableBackCulling;
C4 EnableResponse;
C5 SetUnitsPerMeter;
C6 SetGravity;
C7 SetRadius;
C8 SetTrianglePool;

bool *col_poly=NULL; //Visibility of flat polygons
bool *col_curvepoly=NULL; //Visibility of curved polygons
float *pool=NULL; //Triangle buffer holding vertices

CollisionTrace *trace; //Holds the collision information

void
c_init()
{
	HINSTANCE hLib=LoadLibrary("collision.dll");

	CollideAndSlide=(C1)GetProcAddress((HMODULE)hLib, "collideAndSlide");
	EnableGravity=(C2)GetProcAddress((HMODULE)hLib, "colEnableGravity");
	EnableBackCulling=(C3)GetProcAddress((HMODULE)hLib, "colEnableBackCulling");
	EnableResponse=(C4)GetProcAddress((HMODULE)hLib, "colEnableResponse");
	SetUnitsPerMeter=(C5)GetProcAddress((HMODULE)hLib, "colSetUnitsPerMeter");
	SetGravity=(C6)GetProcAddress((HMODULE)hLib, "colSetGravity");
	SetRadius=(C7)GetProcAddress((HMODULE)hLib, "colSetRadius");
	SetTrianglePool=(C8)GetProcAddress((HMODULE)hLib, "colSetTrianglePool");

	float temp[3]={0,0,0};
	float radius[3]={50,80,50};

	EnableGravity(false);
	EnableBackCulling(false);
	EnableResponse(true);

	SetUnitsPerMeter(200.0f);
	SetGravity((float*)temp);
	SetRadius((float*)radius);	
}

bool
c_tag_polygons(int leaf)
{
	int L1;

	for(L1=0;L1<b_leaf[leaf].nummappolygons;L1++)
	{
		col_poly[b_leaf[leaf].mappolygon[L1]]=true;
	}
	for(L1=0;L1<b_leaf[leaf].numpolygons;L1++)
	{
		col_curvepoly[b_leaf[leaf].polygon[L1]]=true;
	}

	return true;
}

bool
c_push_sphere(int node, dvec3_t v1, dvec3_t v2, dvec3_t center, SCALAR radius)
{
	SCALAR dist;
	bool hit=FALSE;
	
	if(node<0)
		return c_tag_polygons(-node);
	if(node==0)
		return FALSE;

	dist=b_plane[b_colnode[node].plane].normaal.x*center.x+b_plane[b_colnode[node].plane].normaal.y*center.y+b_plane[b_colnode[node].plane].normaal.z*center.z-b_plane[b_colnode[node].plane].dist;
	if(dist>-radius)
	{
		hit=c_push_sphere(b_colnode[node].frontnode,v1,v2,center, radius);
	}
	if(dist<radius)
	{
		hit |= c_push_sphere( b_colnode[node].backnode,v1,v2,center, radius);
	}

	return hit;
}

bool
c_checkpath(int node, dvec3_t v1, dvec3_t v2)
{
	dvec3_t midpoint;
	SCALAR round;

	midpoint.x=(v1.x+v2.x)/2;
	midpoint.y=(v1.y+v2.y)/2;
	midpoint.z=(v1.z+v2.z)/2;
	round= (sqrt( (v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z) ) /2 + 100)*2; //100=Radius

	return c_push_sphere(1, v1, v2,midpoint, round);
}

void
c_setpool(dvec3_t start, dvec3_t end)
{
	int L1, L2;
	int poolnum;

	pool=(float*)m_realloc(pool, sizeof(float)*10240);
	poolnum=0;

	col_poly=(bool*)m_realloc(col_poly, sizeof(bool)*m_numpolygons);
	memset(col_poly, false, sizeof(bool)*m_numpolygons);
	col_curvepoly=(bool*)m_realloc(col_curvepoly, sizeof(bool)*b_numpolygons);
	memset(col_curvepoly, false, sizeof(bool)*b_numpolygons);

	c_checkpath(1, start, end);

	for(L1=0;L1<m_numpolygons;L1++)
	{
		if(!col_poly[L1])continue;
		for(L2=0;L2<m_polygon[L1].numvertex-2;L2++)
		{
			pool[poolnum*9]=m_polygon[L1].vertex[0].x;
			pool[poolnum*9+1]=m_polygon[L1].vertex[0].y;
			pool[poolnum*9+2]=m_polygon[L1].vertex[0].z;

			pool[poolnum*9+3]=m_polygon[L1].vertex[L2+1].x;
			pool[poolnum*9+4]=m_polygon[L1].vertex[L2+1].y;
			pool[poolnum*9+5]=m_polygon[L1].vertex[L2+1].z;

			pool[poolnum*9+6]=m_polygon[L1].vertex[L2+2].x;
			pool[poolnum*9+7]=m_polygon[L1].vertex[L2+2].y;
			pool[poolnum*9+8]=m_polygon[L1].vertex[L2+2].z;

			poolnum++;
		}
	}
	for(L1=0;L1<b_numpolygons;L1++)
	{
		if(!col_curvepoly[L1])continue;
		for(L2=0;L2<b_polygon[L1].numvertex-2;L2++)
		{
			pool[poolnum*9]=b_polygon[L1].vertex[0].x;
			pool[poolnum*9+1]=b_polygon[L1].vertex[0].y;
			pool[poolnum*9+2]=b_polygon[L1].vertex[0].z;

			pool[poolnum*9+3]=b_polygon[L1].vertex[L2+1].x;
			pool[poolnum*9+4]=b_polygon[L1].vertex[L2+1].y;
			pool[poolnum*9+5]=b_polygon[L1].vertex[L2+1].z;

			pool[poolnum*9+6]=b_polygon[L1].vertex[L2+2].x;
			pool[poolnum*9+7]=b_polygon[L1].vertex[L2+2].y;
			pool[poolnum*9+8]=b_polygon[L1].vertex[L2+2].z;

			poolnum++;
		}
	}

	SetTrianglePool(poolnum, pool, 0);
}

bool
c_collide(dvec3_t start, dvec3_t end, collision_t *output)
{
	float point[3];
	float vel[3];

	start.y-=60;end.y-=60;

	point[0]=start.x;point[1]=start.y;point[2]=start.z;
	vel[0]=end.x-start.x;
	vel[1]=end.y-start.y;
	vel[2]=end.z-start.z;

	c_setpool(start, end);
	trace=CollideAndSlide(point, vel);

	bool value=trace->foundCollision;

	m_free(pool);
	pool=NULL;

	output->collision=value;
	set_vector(&output->start, trace->finalPosition[0], trace->finalPosition[1], trace->finalPosition[2]);

	output->start.y+=60;

	return !value;
}
