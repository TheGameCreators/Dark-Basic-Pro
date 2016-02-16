#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>

#include "NXU_tinyxml.h"
#include "NXU_Asc2Bin.h"
#include "NXU_GraphicsMesh.h"
#include "NXU_string.h"

#ifdef WIN32
#pragma warning(disable:4996)
#endif

namespace NXU
{
	
#define DEBUG_LOG 0

enum NodeType
{
	NT_NONE,
	NT_SCENE_GRAPH,
	NT_MESH,
	NT_ANIMATION,
	NT_SKELETON,
	NT_BONE,
	NT_MESH_SECTION,
	NT_VERTEX_BUFFER,
	NT_INDEX_BUFFER,
	NT_NODE_TRIANGLE,
	NT_NODE_INSTANCE,
	NT_ANIM_TRACK,
  NT_NODE_TETRA_MESH,
	NT_LAST
};

enum AttributeType
{
	AT_NONE,
	AT_NAME,
	AT_COUNT,
	AT_PARENT,
	AT_MATERIAL,
	AT_CTYPE,
	AT_SEMANTIC,
	AT_POSITION,
	AT_ORIENTATION,
	AT_DURATION,
	AT_DTIME,
	AT_TRACK_COUNT,
	AT_FRAME_COUNT,
	AT_LAST
};

class MaxVertex
{
public:
	float         mPos[3];
	float         mWeight[4];
	unsigned char  mNxuBone[4];
	float         mNormal[3];
	float         mTexel[2];
	float         mTangent[3];
	float         mBinormal[3];
};


class ParsePxl
{
public:
	ParsePxl(void)
	{
		mCallback = 0;
		mType     = NT_NONE;
		mNxuBone     = 0;
		mFrameCount = 0;
		mDuration   = 0;
		mTrackCount = 0;
		mDtime      = 0;
		mTrackIndex = 0;


		mName          = 0;
		mCount         = 0;
		mParent        = 0;
		mCtype         = 0;
		mSemantic      = 0;
		mSkeleton      = 0;
		mNxuBoneIndex     = 0;
		mIndexBuffer   = 0;
		mVertexBuffer  = 0;
		mVertexCount   = 0;
		mIndexCount    = 0;
		mAnimTrack     = 0;
		mAnimation     = 0;

	}

  NodeType getElementType(const char *str)
  {
  	NodeType ret = NT_NONE;

		if ( strcasecmp(str,"SceneGraph") == 0 )  ret = NT_SCENE_GRAPH;
		else if ( strcasecmp(str,"Mesh") == 0 )  ret = NT_MESH;
		else if ( strcasecmp(str,"Animation") == 0 )  ret = NT_ANIMATION;
		else if ( strcasecmp(str,"Skeleton") == 0 )  ret = NT_SKELETON;
		else if ( strcasecmp(str,"Bone") == 0 )  ret = NT_BONE;
		else if ( strcasecmp(str,"MeshSection") == 0 )  ret = NT_MESH_SECTION;
		else if ( strcasecmp(str,"VertexBuffer") == 0 )  ret = NT_VERTEX_BUFFER;
		else if ( strcasecmp(str,"TetraMesh") == 0 )  ret = NT_NODE_TETRA_MESH;
		else if ( strcasecmp(str,"IndexBuffer") == 0 )  ret = NT_INDEX_BUFFER;
		else if ( strcasecmp(str,"NodeTriangle") == 0 )  ret = NT_NODE_TRIANGLE;
		else if ( strcasecmp(str,"NodeInstance") == 0 )  ret = NT_NODE_INSTANCE;
		else if ( strcasecmp(str,"AnimTrack") == 0 )  ret = NT_ANIM_TRACK;

  	return ret;
  }

	AttributeType getAttributeType(const char *str)
	{
		AttributeType ret = AT_NONE;

		if ( strcasecmp(str,"name") == 0 ) ret = AT_NAME;
		else if ( strcasecmp(str,"count") == 0 ) ret = AT_COUNT;
		else if ( strcasecmp(str,"parent") == 0 ) ret = AT_PARENT;
		else if ( strcasecmp(str,"material") == 0 ) ret = AT_MATERIAL;
		else if ( strcasecmp(str,"ctype") == 0 ) ret = AT_CTYPE;
		else if ( strcasecmp(str,"semantic") == 0 ) ret = AT_SEMANTIC;
		else if ( strcasecmp(str,"position") == 0 ) ret = AT_POSITION;
		else if ( strcasecmp(str,"orientation") == 0 ) ret = AT_ORIENTATION;
		else if ( strcasecmp(str,"duration") == 0 ) ret = AT_DURATION;
		else if ( strcasecmp(str,"dtime") == 0 ) ret = AT_DTIME;
		else if ( strcasecmp(str,"trackcount") == 0 ) ret = AT_TRACK_COUNT;
		else if ( strcasecmp(str,"framecount") == 0 ) ret = AT_FRAME_COUNT;

    return ret;
	}

  bool LoadMesh(const char *fname,NxuGraphicsInterface *callback)
	{
		bool ret = false;
		mCallback = callback;
		TiXmlDocument *doc = new TiXmlDocument;
		bool ok = doc->LoadFile(fname);
		if ( ok )
		{
			Traverse(doc,0);
			ret = true;
		}

		if ( mAnimation )
		{
			mCallback->NodeAnimation(mAnimation);
			delete mAnimation;
			mAnimation = 0;
		}

		delete doc;

		return ret;
	}

	void Traverse(TiXmlNode *node,int depth)
	{

		Process(node,depth);

		node = node->FirstChild();

		while (node )
		{

			if ( node->NoChildren() )
			{
				Process(node,depth);
			}
			else
			{
				Traverse(node,depth+1);
			}

			node = node->NextSibling();
		}

	}

	void Process(TiXmlNode *node,int depth)
	{

		const char *value = node->Value();

		ProcessNode(node->Type(),value,depth);

		TiXmlElement *element = node->ToElement(); // is there an element?  Yes, traverse it's attribute key-pair values.

		if ( element )
		{
			TiXmlAttribute *atr = element->FirstAttribute();
			while ( atr )
			{
				const char *aname  = atr->Name();
				const char *avalue = atr->Value();
				ProcessAttribute( node->Type(), value, depth, aname, avalue );
				atr = atr->Next();
			}
		}
	}


	void ProcessNode(int ntype,const char *svalue,int depth)
	{
		char value[43];
		value[39] = '.';
		value[40] = '.';
		value[41] = '.';
		value[42] = 0;

		strncpy(value,svalue,39);

		switch ( ntype )
		{
			case TiXmlNode::ELEMENT:
			case TiXmlNode::DOCUMENT:
				if ( 1 )
				{
					if ( ntype == TiXmlNode::DOCUMENT )
						Display(depth,"Node(DOCUMENT): %s\n", value);
					else
					{
						mType = (NodeType)getElementType(svalue);
						switch ( mType )
						{
							case NT_ANIMATION:
								if ( mAnimation )
								{
									mCallback->NodeAnimation(mAnimation);
									delete mAnimation;
									mAnimation = 0;
								}
								mName       = 0;
								mFrameCount = 0;
								mDuration   = 0;
								mTrackCount = 0;
								mDtime      = 0;
								mTrackIndex = 0;
								break;
							case NT_ANIM_TRACK:
								if ( mAnimation == 0 )
								{
									if ( mName && mFrameCount && mDuration && mTrackCount && mDtime )
									{
										int framecount = atoi( mFrameCount );
										float duration = (float) atof( mDuration );
										int trackcount = atoi(mTrackCount);
										float dtime = (float) atof(mDtime);
										if ( trackcount >= 1 && framecount >= 1 )
										{
											mAnimation = new NxuAnimation(mName, trackcount, framecount, duration, dtime );
										}
									}
								}
								if ( mAnimation )
								{
									mAnimTrack = mAnimation->GetTrack(mTrackIndex);
									mTrackIndex++;
								}
								break;
							case NT_SKELETON:
								if ( 1 )
								{
									delete mSkeleton;
									mSkeleton = new NxuSkeleton("bip01");
								}
							case NT_BONE:
								if ( mSkeleton )
								{
									mNxuBone = mSkeleton->GetNxuBonePtr(mNxuBoneIndex);
								}
								break;
							default:
								break;
						}
						Display(depth,"Node(ELEMENT): %s\n", value);
					}
				}
				break;
			case TiXmlNode::TEXT:
				Display(depth,"Node(TEXT): %s\n", value);
				switch ( mType )
				{
					case NT_ANIM_TRACK:
						if ( mAnimTrack )
						{
							mAnimTrack->SetName(mName);
							int count = atoi( mCount );
							if ( count == mAnimTrack->GetFrameCount() )
							{
								float *buff = (float *) malloc(sizeof(float)*7*count);
								Asc2Bin(svalue, count, "fff ffff", buff );
								for (int i=0; i<count; i++)
								{
									NxuAnimPose *p = mAnimTrack->GetPose(i);
									const float *src = &buff[i*7];
									p->mPos[0]  = src[0];
									p->mPos[1]  = src[1];
									p->mPos[2]  = src[2];
									p->mQuat[0] = src[3];
									p->mQuat[1] = src[4];
									p->mQuat[2] = src[5];
									p->mQuat[3] = src[6];
								}

							}
						}
						break;
					case NT_NODE_INSTANCE:
						if ( mName )
						{
							float transform[4*4];
							Asc2Bin(svalue, 4, "ffff", transform );
							mCallback->NodeInstance(mName,transform);
							mName = 0;
						}
						break;
					case NT_NODE_TRIANGLE:
						if ( mCtype && mSemantic )
						{
							if ( strcasecmp(mSemantic,"position normal texcoord texcoord blendweights blendindices") == 0 )
							{
								NxuDeformVertex vtx[3];
								Asc2Bin(svalue, 3, mCtype, vtx );
								mCallback->NodeTriangle( &vtx[0], &vtx[1], &vtx[2] );
							}
							else if ( strcasecmp(mSemantic,"position normal texcoord texcoord") == 0 )
							{
								NxuVertex vtx[3];
								Asc2Bin(svalue, 3, mCtype, vtx );
								mCallback->NodeTriangle( &vtx[0], &vtx[1], &vtx[2] );
							}
							mCtype = 0;
							mSemantic = 0;

						}
						break;
          case NT_NODE_TETRA_MESH:
            if ( mCtype && mCount )
            {
							unsigned int tcount  = atoi(mCount);
							if ( tcount > 0 )
							{
								void *tetraMesh = Asc2Bin(svalue, tcount, mCtype, 0 );
                if ( tetraMesh && mCallback )
                {
                  const float *tmesh = (const float *)tetraMesh;
                  for (unsigned int i=0; i<tcount; i++)
                  {
                    mCallback->NodeTetrahedron(tmesh,tmesh+3,tmesh+6,tmesh+9);
                    tmesh+=12;
                  }
                }
                delete (char*)tetraMesh;
							}
							mCtype = 0;
							mCount = 0;
            }
            break;
					case NT_VERTEX_BUFFER:
						if ( mCtype && mCount )
						{
							mVertexCount  = atoi(mCount);
							if ( mVertexCount > 0 )
							{
								mVertexBuffer = Asc2Bin(svalue, mVertexCount, mCtype, 0 );
							}
							mCtype = 0;
							mCount = 0;
						}
						break;
					case NT_INDEX_BUFFER:
						if ( mCount )
						{
							mIndexCount = atoi(mCount);
							if ( mIndexCount > 0 )
							{
								mIndexBuffer = Asc2Bin(svalue, mIndexCount, "hhh", 0 );
							}
						}

						if ( mIndexBuffer && mVertexBuffer )
						{
							if ( strcasecmp(mSemantic,"position normal texcoord texcoord blendweights blendindices") == 0 )
							{
								NxuDeformVertex *buff = (NxuDeformVertex *) mVertexBuffer;
								unsigned short *idx = (unsigned short *) mIndexBuffer;
								mCallback->NodeTriangleList(mVertexCount, buff, mIndexCount*3, idx );
							}
							else if ( strcasecmp(mSemantic,"position blendweights blendindices normal texcoord tangent binormal") == 0 )
							{
								NxuDeformVertex *vtx;
								vtx = new NxuDeformVertex[mVertexCount];
								MaxVertex            *source = (MaxVertex *) mVertexBuffer;
								NxuDeformVertex *dest = vtx;

								for (int i=0; i<mVertexCount; i++)
								{

									dest->mPos[0]     = source->mPos[0];
									dest->mPos[1]     = source->mPos[1];
									dest->mPos[2]     = source->mPos[2];

									dest->mNormal[0]  = source->mNormal[0];
									dest->mNormal[1]  = source->mNormal[1];
									dest->mNormal[2]  = source->mNormal[2];

									dest->mTexel1[0]  = source->mTexel[0];
									dest->mTexel1[1]  = source->mTexel[1];

									dest->mTexel2[0]  = source->mTexel[0];
									dest->mTexel2[1]  = source->mTexel[1];

									dest->mWeight[0] = source->mWeight[0];
									dest->mWeight[1] = source->mWeight[1];
									dest->mWeight[2] = source->mWeight[2];
									dest->mWeight[3] = source->mWeight[3];

									dest->mNxuBone[0]   = (unsigned short) source->mNxuBone[0];
									dest->mNxuBone[1]   = (unsigned short) source->mNxuBone[1];
									dest->mNxuBone[2]   = (unsigned short) source->mNxuBone[2];
									dest->mNxuBone[3]   = (unsigned short) source->mNxuBone[3];

									dest++;
									source++;

								}

								unsigned short *idx = (unsigned short *) mIndexBuffer;
								mCallback->NodeTriangleList(mVertexCount, vtx, mIndexCount*3, idx );

							}
							else if ( strcasecmp(mSemantic,"position normal texcoord texcoord") == 0 )
							{
								NxuVertex *buff = (NxuVertex *) mVertexBuffer;
								unsigned short *idx = (unsigned short *) mIndexBuffer;
								mCallback->NodeTriangleList(mVertexCount, buff, mIndexCount*3, idx );
							}
						}

						free( mIndexBuffer);
			      free( mVertexBuffer);
						mIndexBuffer = 0;
						mVertexBuffer = 0;
						mIndexCount = 0;
						mVertexCount = 0;
						break;
					default:
						break;
				}
				break;
			case TiXmlNode::COMMENT:
				Display(depth,"Node(COMMENT): %s\n", value);
				break;
			case TiXmlNode::DECLARATION:
				Display(depth,"Node(DECLARATION): %s\n", value);
				break;
			case TiXmlNode::UNKNOWN:
				Display(depth,"Node(UNKNOWN): %s\n", value);
				break;
			default:
				Display(depth,"Node(?): %s\n", value);
				break;
		}
	}

	void ProcessAttribute(int         ntype,          // enumerated type of the node
												const char *nvalue, // The node value / key
												int         depth,          // how deeply nested we are in the XML hierachy
												const char *aname,  // the name of the attribute
												const char *savalue) // the value of the attribute
	{
		char avalue[43];

		avalue[39] = '.';
		avalue[40] = '.';
		avalue[41] = '.';
		avalue[42] = 0;

		strncpy(avalue,savalue,39);
		Display(depth,"  ### Attribute(%s,%s)\n", aname, avalue );

		AttributeType attrib = (AttributeType) getAttributeType(aname);
		switch ( attrib )
		{
			case AT_POSITION:
				if ( mType == NT_BONE && mNxuBone )
				{
					Asc2Bin(savalue,1,"fff", mNxuBone->mPosition );
					mNxuBone->ComposeTransform();
					mNxuBoneIndex++;

					if ( mNxuBoneIndex == mSkeleton->GetNxuBoneCount() )
					{
						mCallback->NodeSkeleton(mSkeleton);
						delete mSkeleton;
						mSkeleton = 0;
						mNxuBoneIndex = 0;
					}

				}
				break;
			case AT_ORIENTATION:
				if ( mType == NT_BONE && mNxuBone )
				{
					Asc2Bin(savalue,1,"ffff", mNxuBone->mOrientation );
				}
				break;

			case AT_DURATION:
				mDuration = savalue;
				break;

			case AT_DTIME:
				mDtime = savalue;
				break;

			case AT_TRACK_COUNT:
				mTrackCount = savalue;
				break;

			case AT_FRAME_COUNT:
				mFrameCount = savalue;
				break;

			case AT_NAME:
				mName = savalue;

				switch ( mType )
				{
					case NT_MESH:
						mCallback->NodeMesh(savalue,0);
						break;
					case NT_SKELETON:
						if ( mSkeleton )
						{
							mSkeleton->SetName(savalue);
						}
						break;
					case NT_BONE:
						if ( mNxuBone )
						{
							mNxuBone->SetName(savalue);
						}
						break;
					default:
						break;

				}
				break;
			case AT_COUNT:
				mCount = savalue;
				if ( mType == NT_SKELETON )
				{
					if ( mSkeleton )
					{
						int count = atoi( savalue );
						if ( count > 0 )
						{
							NxuBone *bones;
							bones = new NxuBone[count];
							mSkeleton->SetNxuBones(count,bones);
							mNxuBoneIndex = 0;
						}
					}
				}
				break;
			case AT_PARENT:
				mParent = savalue;
				if ( mNxuBone )
				{
					for (int i=0; i<mNxuBoneIndex; i++)
					{
						const NxuBone &b = mSkeleton->GetNxuBone(i);
						if ( strcmp(mParent,b.mName) == 0 )
						{
							mNxuBone->mParentIndex = i;
							break;
						}
					}
				}
				break;
			case AT_MATERIAL:
				if ( mType == NT_MESH_SECTION )
				{
					mCallback->NodeMaterial(savalue,0);
				}
				break;
			case AT_CTYPE:
				mCtype = savalue;
				break;
			case AT_SEMANTIC:
				mSemantic = savalue;
				break;
			default:
				break;
	
		}

	}

	void Display(int depth,const char *fmt,...)
	{
#if DEBUG_LOG
		for (int i=0; i<depth; i++)
		{
			gLog->Display("  ");
		}
		char wbuff[8192];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(wbuff, fmt, ap);
		va_end(ap);
		gLog->Display("%s", wbuff);
#endif
	}





private:
	NxuGraphicsInterface     *mCallback;
	NodeType               mType;

	const char * mName;
	const char * mCount;
	const char * mParent;
	const char * mCtype;
	const char * mSemantic;

	const char * mFrameCount;
	const char * mDuration;
	const char * mTrackCount;
	const char * mDtime;

	int          mTrackIndex;
	int          mNxuBoneIndex;
	NxuBone       * mNxuBone;

	NxuAnimation  * mAnimation;
	NxuAnimTrack  * mAnimTrack;
	NxuSkeleton   * mSkeleton;
	int          mVertexCount;
	int          mIndexCount;
	void       * mVertexBuffer;
	void       * mIndexBuffer;


};


bool NxuLoadGraphicsMesh(const char *meshName,NxuGraphicsInterface *iface)
{
	bool ret = false;

	ParsePxl pp;

	ret = pp.LoadMesh(meshName,iface);

  return ret;
}


}; // END OF NXU NAMESPACE

