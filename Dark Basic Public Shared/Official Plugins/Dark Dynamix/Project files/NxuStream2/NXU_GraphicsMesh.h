#ifndef NXU_GRAPHICS_MESH_H
#define NXU_GRAPHICS_MESH_H

#include <float.h>
#include <math.h>
#include "NXU_string.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif


namespace NXU
{

class NxuSkeleton;
class NxuAnimation;

enum NxuVertexType
{
	GVT_RIGID,          // for rigid bodies, two sets of texture channels and a vertex normal
	GVT_DEFORM,         // for deformed objects, up to 4 bone weightings and 4 bone indices
	GvT_THIN            // for fluids, just a position and normal since the texture coordinates are generated.
};

class NxuVertexThin
{
public:
	float        mPos[3];
	float        mNormal[3];
};

class NxuVertex;

class NxuDeformVertex
{
public:
	NxuDeformVertex(void) { };

	inline NxuDeformVertex(const NxuVertex &v);
	inline void Lerp(const NxuVertex &a,const NxuVertex &b,float d);

	float        mPos[3];
	float        mNormal[3];
	float        mTexel1[2];
	float        mTexel2[2];
	float        mWeight[4];
	unsigned short mNxuBone[4];
};


class NxuVertex
{
public:
  NxuVertex(void) { };

	NxuVertex(const NxuDeformVertex &v)
	{
		mPos[0] = v.mPos[0];
		mPos[1] = v.mPos[1];
		mPos[2] = v.mPos[2];
		mNormal[0] = v.mNormal[0];
		mNormal[1] = v.mNormal[1];
		mNormal[2] = v.mNormal[2];
		mTexel1[0] = v.mTexel1[0];
		mTexel1[1] = v.mTexel1[1];
		mTexel2[0] = v.mTexel2[0];
		mTexel2[1] = v.mTexel2[1];
	}


	void Lerp(const NxuVertex &a,const NxuVertex &b,float d)
	{
		mPos[0] = ((b.mPos[0] - a.mPos[0]) * d) + a.mPos[0];
		mPos[1] = ((b.mPos[1] - a.mPos[1]) * d) + a.mPos[1];
		mPos[2] = ((b.mPos[2] - a.mPos[2]) * d) + a.mPos[2];

		mNormal[0] = ((b.mNormal[0] - a.mNormal[0])*d) + a.mNormal[0];
		mNormal[1] = ((b.mNormal[1] - a.mNormal[1])*d) + a.mNormal[1];
		mNormal[2] = ((b.mNormal[2] - a.mNormal[2])*d) + a.mNormal[2];

		mTexel1[0] = ((b.mTexel1[0] - a.mTexel1[0])*d) + a.mTexel1[0];
		mTexel1[1] = ((b.mTexel1[1] - a.mTexel1[1])*d) + a.mTexel1[1];

		mTexel2[0] = ((b.mTexel2[0] - a.mTexel2[0])*d) + a.mTexel2[0];
		mTexel2[1] = ((b.mTexel2[1] - a.mTexel2[1])*d) + a.mTexel2[1];

	}

	float        mPos[3];
	float        mNormal[3];
	float        mTexel1[2];
	float        mTexel2[2];
};



class NxuGraphicsInterface
{
public:

	virtual ~NxuGraphicsInterface() 
	{
	}

	virtual void NodeMaterial(const char * /*name*/,const char * /*info*/)
	{
	}

	virtual void NodeMesh(const char * /*name*/,const char * /*info*/)
	{
	}

	virtual void NodeTriangleList(int /*vcount*/,const NxuVertex * /*vertex*/,int /*icount*/,const unsigned short * /*indices*/)
	{
	}

	virtual void NodeTriangleList(int /*vcount*/,const NxuDeformVertex * /*vertex*/,int /*icount*/,const unsigned short * /*indices*/)
	{
	}


	virtual void NodeTriangle(const NxuVertex * /*v1*/,const NxuVertex * /*v2*/,const NxuVertex * /*v3*/)
	{
	}

	virtual void NodeTriangle(const NxuDeformVertex * /*v1*/,const NxuDeformVertex * /*v2*/,const NxuDeformVertex * /*v3*/)
	{
	}

	// add a NxuSkeleton
	virtual void NodeSkeleton(const NxuSkeleton * /*NxuSkeleton*/)
	{
	}

	virtual void NodeAnimation(const NxuAnimation * /*NxuAnimation*/)
	{
	}

	virtual void NodeInstance(const char * /*name*/,const float * /*transform*/)
	{
	}

  virtual void NodeTetrahedron(const float * /*p1*/,const float * /*p2*/,const float * /*p3*/,const float * /*p4*/)
  {
  }

private:
};

inline NxuDeformVertex::NxuDeformVertex(const NxuVertex &v)
{
	mPos[0] = v.mPos[0];
	mPos[1] = v.mPos[1];
	mPos[2] = v.mPos[2];

	mNormal[0] = v.mNormal[0];
	mNormal[1] = v.mNormal[1];
	mNormal[2] = v.mNormal[2];

	mTexel1[0] = v.mTexel1[0];
	mTexel1[1] = v.mTexel1[1];

	mTexel2[0] = v.mTexel2[0];
	mTexel2[1] = v.mTexel2[1];

	mWeight[0] = 1;
	mWeight[1] = 0;
	mWeight[2] = 0;
	mWeight[3] = 0;

	mNxuBone[0] = 0;
	mNxuBone[1] = 0;
	mNxuBone[2] = 0;
	mNxuBone[3] = 0;
}

inline void NxuDeformVertex::Lerp(const NxuVertex &a,const NxuVertex &b,float d)
{
	mPos[0] = ((b.mPos[0] - a.mPos[0]) * d) + a.mPos[0];
	mPos[1] = ((b.mPos[1] - a.mPos[1]) * d) + a.mPos[1];
	mPos[2] = ((b.mPos[2] - a.mPos[2]) * d) + a.mPos[2];

	mNormal[0] = ((b.mNormal[0] - a.mNormal[0])*d) + a.mNormal[0];
	mNormal[1] = ((b.mNormal[1] - a.mNormal[1])*d) + a.mNormal[1];
	mNormal[2] = ((b.mNormal[2] - a.mNormal[2])*d) + a.mNormal[2];

	mTexel1[0] = ((b.mTexel1[0] - a.mTexel1[0])*d) + a.mTexel1[0];
	mTexel1[1] = ((b.mTexel1[1] - a.mTexel1[1])*d) + a.mTexel1[1];

	mTexel2[0] = ((b.mTexel2[0] - a.mTexel2[0])*d) + a.mTexel2[0];
	mTexel2[1] = ((b.mTexel2[1] - a.mTexel2[1])*d) + a.mTexel2[1];

}

bool NxuLoadGraphicsMesh(const char *meshName,NxuGraphicsInterface *iface);

//********************************************************
//****************** NxuSkeleton
//********************************************************

#define MAXSTRLEN 256

class NxuBone
{
public:
	NxuBone(void)
	{
		mParentIndex = -1;
		strcpy(mName,"");
		Identity();
	}

	void Identity(void)
	{
		mPosition[0] = 0;
		mPosition[1] = 0;
		mPosition[2] = 0;

		mOrientation[0] = 0;
		mOrientation[1] = 0;
		mOrientation[2] = 0;
		mOrientation[3] = 1;
		IdentityTransform();
	}

	void IdentityTransform(void)
	{
		mElement[0][0] = 1;
		mElement[1][1] = 1;
		mElement[2][2] = 1;
		mElement[3][3] = 1;

		mElement[1][0] = 0;
		mElement[2][0] = 0;
		mElement[3][0] = 0;

		mElement[0][1] = 0;
		mElement[2][1] = 0;
		mElement[3][1] = 0;

		mElement[0][2] = 0;
		mElement[1][2] = 0;
		mElement[3][2] = 0;

		mElement[0][3] = 0;
		mElement[1][3] = 0;
		mElement[2][3] = 0;
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void Set(const char *name,int parent,const float *transform)
	{
		strncpy(mName,name,MAXSTRLEN);
		mParentIndex = parent;
		memcpy(mElement,transform,sizeof(float)*16);

		ExtractOrientation(mOrientation);

		mPosition[0] = mElement[3][0];
		mPosition[1] = mElement[3][1];
		mPosition[2] = mElement[3][2];

	}

	void Set(const char *name,int parent,const float *pos,const float *rot)
	{
		strncpy(mName,name,MAXSTRLEN);

		mParentIndex = parent;

		mPosition[0] = pos[0];
		mPosition[1] = pos[1];
		mPosition[2] = pos[2];

		mOrientation[0] = rot[0];
		mOrientation[1] = rot[1];
		mOrientation[2] = rot[2];
		mOrientation[3] = rot[3];

		ComposeTransform();

	}

	void GetPos(float *pos) const
	{
		pos[0] = mElement[3][0];
		pos[1] = mElement[3][1];
		pos[2] = mElement[3][2];
	}

	const char * GetName(void) const { return mName; };

	const float * GetTransform(void) const { return &mElement[0][0]; };

	void ExtractOrientation(float *rot)
	{
		float tr = mElement[0][0] + mElement[1][1] + mElement[2][2];
		if (tr > 0.0f )
		{
			float s = (float) sqrtf( tr + 1.0f);
			rot[3] = s * 0.5f;
			s = 0.5f / s;
			rot[0] = (mElement[1][2] - mElement[2][1]) * s;
			rot[1] = (mElement[2][0] - mElement[0][2]) * s;
			rot[2] = (mElement[0][1] - mElement[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			int nxt[3] = {1, 2, 0};
			float  qa[4];

			int i = 0;

			if (mElement[1][1] > mElement[0][0]) i = 1;
			if (mElement[2][2] > mElement[i][i]) i = 2;

			int j = nxt[i];
			int k = nxt[j];

			float s = sqrtf ( ((mElement[i][i] - (mElement[j][j] + mElement[k][k])) + 1.0f) );

			qa[i] = s * 0.5f;

			if (s != 0.0f ) s = 0.5f / s;

			qa[3] = (mElement[j][k] - mElement[k][j]) * s;
			qa[j] = (mElement[i][j] + mElement[j][i]) * s;
			qa[k] = (mElement[i][k] + mElement[k][i]) * s;

			rot[0] = qa[0];
			rot[1] = qa[1];
			rot[2] = qa[2];
			rot[3] = qa[3];
		}
	}

	void ComposeTransform(void)
	{
		IdentityTransform();
		mElement[3][0] = mPosition[0];
		mElement[3][1] = mPosition[1];
		mElement[3][2] = mPosition[2];
		ComposeOrientation();
	}

	void ComposeOrientation(void)
	{
		float w = mOrientation[3];

		float xx = mOrientation[0]*mOrientation[0];
		float yy = mOrientation[1]*mOrientation[1];
		float zz = mOrientation[2]*mOrientation[2];
		float xy = mOrientation[0]*mOrientation[1];
		float xz = mOrientation[0]*mOrientation[2];
		float yz = mOrientation[1]*mOrientation[2];
		float wx = w*mOrientation[0];
		float wy = w*mOrientation[1];
		float wz = w*mOrientation[2];

		mElement[0][0] = 1 - 2 * ( yy + zz );
		mElement[1][0] =     2 * ( xy - wz );
		mElement[2][0] =     2 * ( xz + wy );

		mElement[0][1] =     2 * ( xy + wz );
		mElement[1][1] = 1 - 2 * ( xx + zz );
		mElement[2][1] =     2 * ( yz - wx );

		mElement[0][2] =     2 * ( xz - wy );
		mElement[1][2] =     2 * ( yz + wx );
		mElement[2][2] = 1 - 2 * ( xx + yy );

	}

	int GetParentIndex(void) const { return mParentIndex; };
	const float * GetPosition(void) const { return mPosition; };
	const float * GetOrientation(void) const { return mOrientation; };

	char          mName[MAXSTRLEN];
	int           mParentIndex;          // array index of parent bone
	float         mPosition[3];
	float         mOrientation[4];
	float         mElement[4][4];
};

class NxuSkeleton
{
public:
	NxuSkeleton(const char *name)
	{
		strcpy(mName,name);
		mNxuBoneCount = 0;
		mNxuBones = 0;
	}

	NxuSkeleton(unsigned int bonecount)
	{
		mName[0] = 0;
		mNxuBoneCount = bonecount;
		mNxuBones = new NxuBone[bonecount];
	}

	NxuSkeleton(const NxuSkeleton &sk)
	{
		strcpy(mName, sk.mName );
		mNxuBoneCount = sk.mNxuBoneCount;
		mNxuBones = 0;
		if ( mNxuBoneCount )
		{
			mNxuBones = new NxuBone[mNxuBoneCount];
			memcpy(mNxuBones,sk.mNxuBones,sizeof(NxuBone)*mNxuBoneCount);
		}
	}

	~NxuSkeleton(void)
	{
		delete []mNxuBones;
	}

	void SetName(const char *name)
	{
		strcpy(mName,name);
	}

	void SetNxuBones(int bcount,NxuBone *bones)
	{
		mNxuBoneCount = bcount;
		mNxuBones     = bones;
	}

	int GetNxuBoneCount(void) const { return mNxuBoneCount; };

	const NxuBone& GetNxuBone(int index) const { return mNxuBones[index]; };

	NxuBone * GetNxuBonePtr(int index) const { return &mNxuBones[index]; };

	void SetNxuBone(int index,const NxuBone &b) { mNxuBones[index] = b; };

	const char * GetName(void) const { return mName; };

  void FindFourClosest(const float *pos,float *closest,int *bones,float &fifthbone) const
  {
  	closest[0] = FLT_MAX;
  	closest[1] = FLT_MAX;
  	closest[2] = FLT_MAX;
  	closest[3] = FLT_MAX;

  	fifthbone  = FLT_MAX;

  	bones[0]   = 0;
  	bones[1]   = 0;
  	bones[2]   = 0;
  	bones[3]   = 0;

  	NxuBone *b = mNxuBones;
  	for (int i=0; i<mNxuBoneCount; i++,b++)
  	{
  		float bpos[3];

  		b->GetPos(bpos);

  		float dx = bpos[0] - pos[0];
  		float dy = bpos[1] - pos[1];
  		float dz = bpos[2] - pos[2];

  		float distance = dx*dx+dy*dy+dz*dz;

  		if ( distance < closest[0] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2] = closest[1];
  			bones[2]   = bones[1];

  			closest[1] = closest[0];
  			bones[1]   = bones[0];

  			closest[0] = distance;
  			bones[0]   = i;

  		}
  		else if ( distance < closest[1] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2] = closest[1];
  			bones[2]   = bones[1];

  			closest[1] = distance;
  			bones[1]   = i;
  		}
  		else if ( distance < closest[2] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2]  = distance;
  			bones[2]    = i;
  		}
  		else if ( distance < closest[3] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = distance;
  			bones[3]   = i;
  		}
  	}

  	closest[0] = sqrtf( closest[0] );
  	closest[1] = sqrtf( closest[1] );
  	closest[2] = sqrtf( closest[2] );
  	closest[3] = sqrtf( closest[3] );

  	fifthbone  = sqrtf(fifthbone);
  }

  void ComputeDefaultWeighting(const float *pos,float *weight,unsigned short &b1,unsigned short &b2,unsigned short &b3,unsigned short &b4) const
  {

  	float closest[4];
  	int   bones[4];
  	float furthest;

  	FindFourClosest(pos,closest,bones,furthest);

  	float recip = 1.0f / furthest;

  	weight[0] = (furthest-closest[0]) * recip;
  	weight[1] = (furthest-closest[1]) * recip;
  	weight[2] = (furthest-closest[2]) * recip;
  	weight[3] = (furthest-closest[3]) * recip;

		weight[0]*=4.0f;
		weight[1]*=2.0f;

  	float total = weight[0] + weight[1] + weight[2] + weight[3];

  	recip = 1.0f / total;

  	weight[0] = weight[0]*recip;
  	weight[1] = weight[1]*recip;
  	weight[2] = weight[2]*recip;
  	weight[3] = weight[3]*recip;

  	total = weight[0] + weight[1] + weight[2] + weight[3];

  	b1 = (unsigned short)bones[0];
  	b2 = (unsigned short)bones[1];
  	b3 = (unsigned short)bones[2];
  	b4 = (unsigned short)bones[3];

  }


  void DebugReport(void) const
  {
  	for (int i=0; i<mNxuBoneCount; i++)
  	{
  		const NxuBone &b = mNxuBones[i];
  		const char *foo = "no parent";
  		if ( b.GetParentIndex() >= 0 )
  		{
  			foo = mNxuBones[ b.GetParentIndex() ].GetName();
  		}
  		printf("NxuBone%d : %s  ParentIndex(%d) Parent(%s)\n", i, b.GetName(), b.GetParentIndex(), foo );
  	}
  }


private:
	char                   mName[MAXSTRLEN];
	int                    mNxuBoneCount;
	NxuBone               *mNxuBones;
};


//********************************************************
//******************* END OF NxuSkeleton
//********************************************************


//********************************************************
//******************* NxuAnimation
//********************************************************

class NxuAnimPose
{
public:

	void SetPose(const float *pos,const float *quat)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
	};

	void Sample(float *pos,float *quat) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
	}

	float mPos[3];
	float mQuat[4];
};

class NxuAnimTrack
{
public:

	NxuAnimTrack(int framecount,
						float duration,
						float dtime)
	{
		mName[0] = 0;
		mFrameCount = framecount;
		mPose = new NxuAnimPose[mFrameCount];
		mDuration   = duration;
		mDtime      = dtime;
	}

	NxuAnimTrack(const NxuAnimTrack &c)
	{
		strcpy(mName, c.mName );
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mPose = new NxuAnimPose[mFrameCount];
		for (int i=0; i<mFrameCount; i++)
		{
			mPose[i] = c.mPose[i];
		}
	}

	~NxuAnimTrack(void)
	{
		delete []mPose;
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,256);
	}

	void SetPose(int frame,const float *pos,const float *quat)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat);
	}

	const char * GetName(void) const { return mName; };

	void SampleNxuAnimation(int frame,float *pos,float *quat) const
	{
		mPose[frame].Sample(pos,quat);
	}

	int GetFrameCount(void) const { return mFrameCount; };

	NxuAnimPose * GetPose(int index) { return &mPose[index]; };

private:
	char      mName[256]; // name of the track.
	int       mFrameCount;
	float     mDuration;
	float     mDtime;
	NxuAnimPose *mPose;
};

class NxuAnimation
{
public:
	NxuAnimation(const char *name,int trackcount,int framecount,float duration,float dtime)
	{
		strncpy(mName,name,256);
		mTrackCount = trackcount;
		mFrameCount = framecount;
		mTracks = (NxuAnimTrack **) malloc(sizeof(NxuAnimTrack*)*mTrackCount);
		mDuration  = duration;
		mDtime     = dtime;
		for (int i=0; i<trackcount; i++)
		{
			mTracks[i] = new NxuAnimTrack(framecount,duration,dtime);
		}
	}

	NxuAnimation(const NxuAnimation &c) // construct NxuAnimation by copying an existing one
	{
		strcpy(mName, c.mName );
		mTrackCount = c.mTrackCount;
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mTracks     = (NxuAnimTrack **) malloc(sizeof(NxuAnimTrack*)*mTrackCount);
		for (int i=0; i<mTrackCount; i++)
		{
			mTracks[i] = new NxuAnimTrack( *c.mTracks[i] );
		}
	}

	~NxuAnimation(void)
	{
		for (int i=0; i<mTrackCount; i++)
		{
			NxuAnimTrack *at = mTracks[i];
			delete at;
		}
		free(mTracks);
	}

	void SetName(const char *name)
	{
		strcpy(mName,name);
	}

	void SetTrackName(int track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(int track,int frame,const float *pos,const float *quat)
	{
		mTracks[track]->SetPose(frame,pos,quat);
	}

	const char * GetName(void) const { return mName; };

	const NxuAnimTrack * LocateTrack(const char *name) const
	{
		const NxuAnimTrack *ret = 0;
		for (int i=0; i<mTrackCount; i++)
		{
			const NxuAnimTrack *t = mTracks[i];
			if ( strcasecmp(t->GetName(),name) == 0 )
			{
				ret = t;
				break;
			}
		}
		return ret;
	}

	int GetFrameIndex(float t) const
	{
		t = fmodf( t, mDuration );
		int index = int(t / mDtime);
		return index;
	}

	int GetTrackCount(void) const { return mTrackCount; };
	float GetDuration(void) const { return mDuration; };

	NxuAnimTrack * GetTrack(int index)
	{
		NxuAnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	int GetFrameCount(void) const { return mFrameCount; };
	float GetDtime(void) const { return mDtime; };

private:
	char        mName[256];
	int         mTrackCount;
	int         mFrameCount;
	float       mDuration;
	float       mDtime;
	NxuAnimTrack **mTracks;
};

//********************************************************
//***************  END OF NxuAnimATION CODE
//********************************************************


} // END OF NAMESPACE

#ifdef WIN32
#pragma warning(pop)
#endif

#endif
