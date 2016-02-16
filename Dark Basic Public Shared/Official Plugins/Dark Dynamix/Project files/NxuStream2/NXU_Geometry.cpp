#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#include "NXU_Geometry.h"
#include "NxMat34.h"
#include "NXU_hull.h"

namespace	NXU
{

static void	set(float	*p,	int	index, float x,	float	y, float z,const NxMat34 *localPose)
{
	NxVec3 v(x,y,z);
	if ( localPose )
	{
		NxVec3 t;
		localPose->multiply(v,t);
		v = t;
	}
	p	=	&p[index *3];
	p[0] = v.x;
	p[1] = v.y;
	p[2] = v.z;
}

static unsigned	int	*addTri(unsigned int i1, unsigned	int	i2,	unsigned int i3, unsigned	int	*idx)
{
	idx[0] = i1;
	idx[1] = i2;
	idx[2] = i3;
	idx	+= 3;
	return idx;
}

bool createBox(NxVec3	&dimensions,NxuGeometry &g,const NxMat34 *localPose, float shrink,unsigned int maxV)
{

	g.reset();

	g.mVertices	=	new	float[8	*3];
	g.mVcount	=	8;
	g.mTcount	=	12;
	g.mIndices = new unsigned	int[12 *3];

	float	bmin[3];
	float	bmax[3];

	bmin[0]	=	 -dimensions.x*shrink;
	bmin[1]	=	 -dimensions.y*shrink;
	bmin[2]	=	 -dimensions.z*shrink;

	bmax[0]	=	dimensions.x*shrink;
	bmax[1]	=	dimensions.y*shrink;
	bmax[2]	=	dimensions.z*shrink;



	set(g.mVertices, 0,	bmin[0], bmin[1],	bmin[2], localPose);
	set(g.mVertices, 1,	bmax[0], bmin[1],	bmin[2], localPose);
	set(g.mVertices, 2,	bmax[0], bmax[1],	bmin[2], localPose);
	set(g.mVertices, 3,	bmin[0], bmax[1],	bmin[2], localPose);
	set(g.mVertices, 4,	bmin[0], bmin[1],	bmax[2], localPose);
	set(g.mVertices, 5,	bmax[0], bmin[1],	bmax[2], localPose);
	set(g.mVertices, 6,	bmax[0], bmax[1],	bmax[2], localPose);
	set(g.mVertices, 7,	bmin[0], bmax[1],	bmax[2], localPose);

	unsigned int *indices	=	g.mIndices;

	indices	=	addTri(0,	3, 2,	indices);
	indices	=	addTri(0,	2, 1,	indices);
	indices	=	addTri(2,	6, 5,	indices);
	indices	=	addTri(1,	2, 5,	indices);
	indices	=	addTri(6,	7, 4,	indices);
	indices	=	addTri(5,	6, 4,	indices);
	indices	=	addTri(4,	7, 3,	indices);
	indices	=	addTri(4,	3, 0,	indices);
	indices	=	addTri(5,	4, 0,	indices);
	indices	=	addTri(5,	0, 1,	indices);
	indices	=	addTri(3,	7, 6,	indices);
	indices	=	addTri(2,	3, 6,	indices);

	return true;
}

static float *createSphere(float r,	int	stepsize,	unsigned int &vcount)
{
	float	a	=	0;
	float	astep	=	(3.141592654f	*2.0f) / (float)stepsize;

	vcount = stepsize	*	stepsize;

	float	*points	=	new	float[stepsize *stepsize * 3];

	for	(int i = 0;	i	<	stepsize;	i++)
	{
		float	b	=	0;
		float	bstep	=	(3.141592654f	*2.0f) / stepsize;

		for	(int j = 0;	j	<	stepsize;	j++)
		{

			float	p[3];

			p[0] = r * sinf(a) *cosf(b);
			p[1] = r * sinf(a) *sinf(b);
			p[2] = r * cosf(a);

			int	index	=	(i *stepsize)	+	j;
			points[index *3+0] = p[0];
			points[index *3+1] = p[1];
			points[index *3+2] = p[2];

			b	+= bstep;
		}

		a	+= astep;
	}
	return points;
}

bool createSphere(float	r,NxuGeometry &g,const NxMat34 *localPose,int stepsize,NxReal shrink,unsigned int maxV)
{

	unsigned int vcount = 0;

	float	*points	=	createSphere(r,	stepsize,	vcount);

	bool ret = createHull(vcount,	points,		g,localPose, shrink, maxV);

	delete points;

	return ret;
}

static void	SwapYZ(float *p)
{
	float	t	=	p[1];
	p[1] = p[2];
	p[2] = t;
}

static void	SwapXZ(float *p)
{
	float	t	=	p[0];
	p[0] = p[2];
	p[2] = t;
}

bool createCapsule(float radius, float height, NxuGeometry &g,const NxMat34 *localPose,int axis, int stepsize,  NxReal shrink,unsigned int maxV)
{
	unsigned int vcount = 0;

	float	*points	=	createSphere(radius, stepsize, vcount);

	float	spread = height	*	0.5f;

	for	(unsigned	int	i	=	0; i < vcount; i++)
	{
		float	*p = &points[i *3];

		if (p[2] < 0)
		{
			p[2] -=	spread;
		}
		else
		{
			p[2] +=	spread;
		}


		if (axis ==	1)
		{
			SwapYZ(p);
		}
		else if	(axis	== 0)
		{
			SwapXZ(p);
		}

	}

	bool ret = createHull(vcount,	points,	g,localPose, shrink, maxV);

	delete points;

	return ret;
}


static void computeAABB(unsigned int vcount,const float *points,unsigned int pstride,float *bmin,float *bmax)
{

  const unsigned char *source = (const unsigned char *) points;

	bmin[0] = points[0];
	bmin[1] = points[1];
	bmin[2] = points[2];

	bmax[0] = points[0];
	bmax[1] = points[1];
	bmax[2] = points[2];


  for (unsigned int i=1; i<vcount; i++)
  {
  	source+=pstride;
  	const float *p = (const float *) source;

  	if ( p[0] < bmin[0] ) bmin[0] = p[0];
  	if ( p[1] < bmin[1] ) bmin[1] = p[1];
  	if ( p[2] < bmin[2] ) bmin[2] = p[2];

		if ( p[0] > bmax[0] ) bmax[0] = p[0];
		if ( p[1] > bmax[1] ) bmax[1] = p[1];
		if ( p[2] > bmax[2] ) bmax[2] = p[2];

  }
}

bool createHull(unsigned int vcount, const float *points,NxuGeometry &g,const NxMat34 *localPose,NxReal shrink,unsigned int maxv)
{
	bool ret = false;

	g.reset();

	HullLibrary	hl;

	HullDesc desc(QF_TRIANGLES,	vcount,	points,	sizeof(float)	*3);

	desc.mMaxVertices	=	maxv;

	HullResult result;

	HullError	err	=	hl.CreateConvexHull(desc,	result);

	if (err	== QE_OK)
	{
		ret	=	true;

		g.mVertices	=	new	float[result.mNumOutputVertices	*3];

		float bmin[3];
		float bmax[3];
		computeAABB( result.mNumOutputVertices, result.mOutputVertices, sizeof(float)*3, bmin, bmax );

    NxVec3 center;

    center.x = (bmax[0]-bmin[0])*0.5f + bmin[0];
    center.y = (bmax[1]-bmin[1])*0.5f + bmin[1];
    center.z = (bmax[2]-bmin[2])*0.5f + bmin[2];

		const float *source = result.mOutputVertices;
		float *dest = g.mVertices;

		for (NxU32 i=0; i<result.mNumOutputVertices; i++)
		{
			NxVec3 v(source[0],source[1],source[2]);

			v-=center;
			v*=shrink;
			v+=center;

			if ( localPose )
			{
				NxVec3 t;
				localPose->multiply(v,t);
				v = t;
			}

			dest[0] = v.x;
			dest[1] = v.y;
			dest[2] = v.z;

			source+=3;
			dest+=3;
		}

		g.mIndices = new unsigned	int[result.mNumIndices];

		memcpy(g.mIndices, result.mIndices,	sizeof(unsigned	int) *result.mNumIndices);

		g.mVcount	=	result.mNumOutputVertices;
		g.mTcount	=	result.mNumFaces;

		hl.ReleaseResult(result);
	}

	return ret;
}

};
