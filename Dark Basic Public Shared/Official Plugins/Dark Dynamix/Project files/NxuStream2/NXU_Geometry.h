#ifndef	NXU_GEOMETRY_H

#define	NXU_GEOMETRY_H


class	NxMat34;
class	NxVec3;

namespace	NXU
{

class	NxuGeometry
{
	 public:
		NxuGeometry(void)
		{
			mVcount	=	0;
			mVertices	=	0;
			mTcount	=	0;
			mIndices = 0;
		}
		~NxuGeometry(void)
		{
			reset();
		}

		void reset(void)
		{
			if (mVertices)
			{
					delete mVertices;
			}
			if (mIndices)
			{
					delete mIndices;
			}
			mVcount	=	0;
			mVertices	=	0;
			mTcount	=	0;
			mIndices = 0;
		}

		unsigned int mVcount;
		float	*mVertices;
		unsigned int mTcount;
		unsigned int *mIndices;
};


bool createBox(NxVec3	&dimensions,
               NxuGeometry &g,
               const NxMat34 *localPose=0,
               float shrink=1.0f,
               unsigned int maxV=32);

bool createSphere(float	radius,
                  NxuGeometry	&g,
                  const	NxMat34	*localPose=0,
                  int slices=16,
                  float shrink=1.0f,
                  unsigned int maxV=256);

bool createCapsule(float radius,
                   float height,
                   NxuGeometry &g,
                   const NxMat34 *localPose=0,
                   int axis=1,
                   int slices=16,
                   float shrink=1.0f,
                   unsigned int maxV=256);

bool createHull(unsigned int vcount,
                const float *points,
                NxuGeometry &g,
                const NxMat34 *localPose=0,
                float shrink=1,
                unsigned int maxV=256);



}

/**************************************************************************
 *																		*
 **************************************************************************/

#endif
