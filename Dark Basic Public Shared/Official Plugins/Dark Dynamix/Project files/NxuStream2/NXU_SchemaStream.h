#ifndef SCHEMA_STREAM_H

#define SCHEMA_STREAM_H

#include "NxSimpleTypes.h"
#include "NxArray.h"
#include "NxVec3.h"
#include "NxQuat.h"
#include "NxMat34.h"
#include "NxBounds3.h"
#include "NXU_tinyxml.h"
#include "NXU_SchemaTypes.h"

namespace NXU
{

#define NXUSTREAM_VERSION 103

class SchemaBlock;
class NXU_FILE;
class NxuPhysicsCollection;
class NxSceneDesc;
class NxTriangleMeshDesc;
class NxConvexMeshDesc;
class	NxHeightFieldDesc;
class NxActorDesc;
class NxShapeDesc;
class NxFluidDesc;
class NxClothDesc;
class NxJointDesc;
class NxSoftBodyMeshDesc;
class NxSoftBodyDesc;

#define HEADER1 0x0ABCDEF
#define HEADER2 0x1234567

class SchemaXML;

class SchemaHeader
{
public:
  SchemaHeader(void)
  {
    mId1 = HEADER1;
    mId2 = HEADER2;
    mType = -1;
    mLen = 0;
  }

  bool isValid(void);

  NxU32 mId1;         // header ID to verify this is what we expect
  NxI32 mType;       // the 'class' type.
  NxU32 mLen;        // the length of the data (distance to skip to the next header)
  NxU32 mId2;         // header ID to verify this is what we expect
};

class SchemaStream
{
public:
  SchemaStream(const char *fname,
               bool isBinary,
               const char *mode,    // "wb" "rb" or "wmem"
               void *mem,
               int len);

  ~SchemaStream(void);

  bool beginHeader(NxI32 headerId,const char *parent=0);

  bool peekHeader(NxI32 headerId,
									NxI32 *peek=0,
                  NxI32 or1=-1,
                  NxI32 or2=-1,
                  NxI32 or3=-1,
                  NxI32 or4=-1,
                  NxI32 or5=-1,
                  NxI32 or6=-1,
                  NxI32 or7=-1,
                  NxI32 or8=-1,
                  NxI32 or9=-1,
                  NxI32 or10=-1);

  void store(NX_BOOL v,const char *name,bool attribute);
  void store(NxU16 v,const char *name,bool attribute);
  void store(NxU32 v,const char *name,bool attribute);
  void store(NxF32 v,const char *name,bool attribute);
  void store(const char *v,const char *name,bool attribute);


  void store(NxVec3 &v,const char *name,bool attribute);
  void store(NxQuat &v,const char *name,bool attribute);
  void store(NxMat34 &v,const char *name,bool attribute);
  void store(NxMat33 &v,const char *name,bool attribute);
  void store(NxBounds3 &v,const char *name,bool attribute);
  void store(NxPlane &v,const char *name,bool attribute);

  void store(NxArray< NxF32 > &v,const char *name,bool attribute);
  void store(NxArray< NxU32 > &v,const char *name,bool attribute);
  void store(NxArray< NxU16 > &v,const char *name,bool attribute);
  void store(NxArray< NxU8 > &v,const char *name,bool attribute);
  void store(NxArray< NxVec3 > &v,const char *name,bool attribute);
  void store(NxArray< NxTri > &v,const char *name,bool attribute);
  void store(NxArray< NxTetra > &v,const char *name,bool attribute);

  void load(NX_BOOL &v,const char *name,bool attribute);
  void load(NxU16 &v,const char *name,bool attribute);
  void load(NxU32 &v,const char *name,bool attribute);
  void load(NxF32 &v,const char *name,bool attribute);
  void load(const char *&v,const char *name,bool attribute);
  void load(NxArray< NxF32 > &v,const char *name,bool attribute);
  void load(NxArray< NxU32 > &v,const char *name,bool attribute);
  void load(NxArray< NxU16 > &v,const char *name,bool attribute);
  void load(NxArray< NxU8 > &v,const char *name,bool attribute);
  void load(NxVec3 &v,const char *name,bool attribute);
  void load(NxQuat &v,const char *name,bool attribute);
  void load(NxMat34 &v,const char *name,bool attribute);
  void load(NxMat33 &v,const char *name,bool attribute);
  void load(NxArray< NxVec3 > &v,const char *name,bool attribute);
  void load(NxArray< NxTri > &v,const char *name,bool attribute);
  void load(NxArray< NxTetra > &v,const char *name,bool attribute);
  void load(NxBounds3 &v,const char *name,bool attribute);
  void load(NxPlane &v,const char *name,bool attribute);


  void endHeader(void);

	bool isBinary(void) const { return mBinary; };
  bool isValid(void) const { return mIsValid; };

  void * getMemBuffer(size_t &outputLength);

  void myprintf(const char *fmt,...);


  void invalidate(const char *fmt,...);

  void setCurrentCollection(NxuPhysicsCollection *c);
  void setCurrentScene(NxSceneDesc *s);
  void setCurrentJoint(NxJointDesc *j);
  void setCurrentActor(NxActorDesc *a);
  void setCurrentShape(NxShapeDesc *s);
  void setCurrentFluid(NxFluidDesc *f);
  void setCurrentCloth(NxClothDesc *c);

  NxuPhysicsCollection * getCurrentCollection(void) const { return mCurrentCollection; };
  NxSceneDesc * getCurrentScene(void) const { return mCurrentScene; };
  NxJointDesc * getCurrentJoint(void) const { return mCurrentJoint; };
  NxActorDesc * getCurrentActor(void) const { return mCurrentActor; };
  NxShapeDesc * getCurrentShape(void) const { return mCurrentShape; };
  NxFluidDesc * getCurrentFluid(void) const { return mCurrentFluid; };
  NxClothDesc * getCurrentCloth(void) const { return mCurrentCloth; };

//**************
  void write(NxU8  data);
  void write(NxU16 data);
  void write(NxU32 data);
  void write(NxI32 data);
  void write(NxF32 data);

  void write(const NxVec3 &data);
  void write(const NxF32 *data,NxU32 count);
  void write(const NxU32 *data,NxU32 count);
  void write(const NxU16 *data,NxU32 count);
  void write(const NxVec3 *data,NxU32 count);
  void write(const char *mem,NxU32 size);
  void write(const NxU8 *mem,NxU32 count);
  void write(const NxTri *tris,NxU32 count);
  void write(const NxTetra *data,NxU32 count);

  void read(NxU8 &data);
  void read(NxU16 &data);
  void read(NxU32 &data);
  void read(NxI32 &data);
  void read(NxF32 &data);
  void read(NxVec3 &v);
  void read(NxTri &data);
  void read(NxTetra &data);

  void read(NxF32 *data,NxU32 count);
  void read(NxU32 *data,NxU32 count);
  void read(NxU16 *data,NxU32 count);
  void read(NxVec3 *data,NxU32 count);
  void read(char *mem,NxU32 size);
  void read(NxU8 *mem,NxU32 count);
  void read(NxTri *tris,NxU32 count);
  void read(NxTetra *data,NxU32 count);

//**************

  void writeMem(const void *mem,NxU32 size);
  void readMem(void *mem,NxU32 size);

  void peekReadMem(void *mem,NxU32 size);
  void peekRead(NxU32 &v);
  void peekRead(NxI32 &v);

private:

  size_t ftell(void);

  void endFlush(void);
	void closeLast(void);
	void pushLast(size_t loc);


  SchemaBlock             *mCurrentBlock;
  NxArray< SchemaBlock * > mBlocks;
  NXU_FILE                *mFph;
  int                      mStackPtr;
  size_t                   mStack[32];
  bool                     mBinary;
  bool                     mReadMode; // true if reading data
  bool                     mIsValid;
  bool                     mFlipEndian;

  SchemaHeader             mCurrentHeader;
  int                      mHeaderStackPtr;
  SchemaHeader             mHeaderStack[32];

  SchemaXML               *mSchemaXML;

  NxuPhysicsCollection    *mCurrentCollection;

  NxSceneDesc             *mCurrentScene;
  NxActorDesc             *mCurrentActor;
  NxJointDesc             *mCurrentJoint;
  NxShapeDesc             *mCurrentShape;
  NxClothDesc             *mCurrentCloth;
  NxFluidDesc             *mCurrentFluid;

};

const char         *getElement(const char *name);

NxSceneDesc        *locateSceneDesc(NxuPhysicsCollection *c,const char *name,NxU32 &index);
NxConvexMeshDesc   *locateConvexMeshDesc(NxuPhysicsCollection *c,const char *name);
NxTriangleMeshDesc *locateTriangleMeshDesc(NxuPhysicsCollection *c,const char *name);
NxHeightFieldDesc  *locateHeightFieldDesc(NxuPhysicsCollection *c,const char *name);
NxActorDesc        *locateActorDesc(NxSceneDesc *s,const char *name);


}

#endif
