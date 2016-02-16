#ifndef TETRA_GRAPHICS_H

#define TETRA_GRAPHICS_H

#include <string.h>
#include "NxPhysics.h"

class NxScene;

class SoftFileInterface
{
public:
  virtual const char * getSoftFileName(const char *name,bool read_access) { return name; }; // translate a source file to its full relative path location on disk.  If read_access is true it should search for the file location
  virtual void         softMessageReport(const char *str) { }; // optional.  Trap this routine to receive log messages.
  virtual void         menuCommand(const char *) { };
  virtual void         scriptCommand(const char *) { };

  virtual const char * getSaveFileName(const char *fileType,const char *initial,const char *description) { return initial; }; // allows the application the opportunity to present a file save dialog box.
  virtual const char * getLoadFileName(const char *extension,const char *description) { return 0; }; //
  virtual NxScene *    resetSDK(bool groundPlane) { return 0; }; // reset the SDK, return the current scene, and create a ground plane if requested.
  virtual void         fetchResults(void) { };
};


namespace SOFTBODY
{

class TetraGraphicsVertex
{
public:
  float	       mPos[3];
  float        mNormal[3];
  float        mTexel[2];
};

class TetraDeformVertex
{
public:
  float	         mPos[3];
  float          mNormal[3];
  float          mTexel1[2];
  float          mTexel2[2];
  float          mWeight[4];
  unsigned short mBone[4];
};

class TetraMaterial
{
public:
  TetraMaterial(const char *name,const char *texture,unsigned int ambient,unsigned int diffuse,unsigned int specular,float specularPower)
  {
    mName[0] = 0;
  	mTexture[0] = 0;

    if ( name )
      strncpy(mName,name,512);

    if ( texture )
      strncpy(mTexture,texture,512);

  	mHandle = 0;
    setDiffuseColor(diffuse);
    setAmbientColor(ambient);
    setSpecularColor(specular);
    setSpecularPower(specularPower);
  }

  TetraMaterial(const char *name)
  {
    strcpy(mName,name);
  	mTexture[0] = 0;
  	mHandle = 0;
    setDiffuseColor(0xFFFFFFFF);
    setAmbientColor(0xFF606060);
    setSpecularColor(0xFFFFFFFF);
    setSpecularPower(32.0f);
  }

	TetraMaterial(const TetraMaterial &tm)
	{
	  strcpy(mName,tm.mName);
	  strcpy(mTexture,tm.mTexture);
	  mSpecularPower = tm.mSpecularPower;
	  for (int i=0; i<4; i++)
	  {
	  	mAmbientColor[i] = tm.mAmbientColor[i];
	  	mDiffuseColor[i] = tm.mDiffuseColor[i];
	  	mSpecularColor[i] = tm.mSpecularColor[i];
	  }
	  mHandle = 0;
	}

	bool operator==(const TetraMaterial &tm) const
	{

		bool ret = false;

		if ( strcmp(mName,tm.mName) == 0 &&
			   strcmp(mTexture,tm.mTexture) == 0 &&
			   mSpecularPower == tm.mSpecularPower &&
			   mAmbientColor[0] == tm.mAmbientColor[0] &&
			   mAmbientColor[1] == tm.mAmbientColor[1] &&
			   mAmbientColor[2] == tm.mAmbientColor[2] &&
			   mAmbientColor[3] == tm.mAmbientColor[3] &&
			   mDiffuseColor[0] == tm.mDiffuseColor[0] &&
			   mDiffuseColor[1] == tm.mDiffuseColor[1] &&
			   mDiffuseColor[2] == tm.mDiffuseColor[2] &&
			   mDiffuseColor[3] == tm.mDiffuseColor[3] &&
			   mSpecularColor[0] == tm.mSpecularColor[0] &&
			   mSpecularColor[1] == tm.mSpecularColor[1] &&
			   mSpecularColor[2] == tm.mSpecularColor[2] &&
			   mSpecularColor[3] == tm.mSpecularColor[3] ) ret = true;

	  return ret;
	}

  void setColor(unsigned int color,float *c)
  {
    unsigned int a = (color>>24)&0xFF;
    unsigned int r = (color>>16)&0xFF;
    unsigned int g = (color>>8)&0xFF;
    unsigned int b = color&0xFF;

    c[0] = (float)r/255.0f;
    c[1] = (float)g/255.0f;
    c[2] = (float)b/255.0f;
    c[3] = (float)a/255.0f;
  }

  void setDiffuseColor(unsigned int color)
  {
    setColor(color,mDiffuseColor);
  }

  void setAmbientColor(unsigned int color)
  {
    setColor(color,mAmbientColor);
  }

  void setSpecularColor(unsigned int color)
  {
    setColor(color,mSpecularColor);
  }


  void setSpecularPower(float power)
  {
    mSpecularPower = power;
  }

	char  mName[512];
  char  mTexture[512];
  float mSpecularPower;
  float mAmbientColor[4];  // ARGB color
  float mDiffuseColor[4];
  float mSpecularColor[4];
  void *mHandle; // handle to the device version

};


class SoftMeshInterface
{
public:
	virtual void softMeshMaterial(const TetraMaterial &tm) = 0;
	virtual void softMeshTriangle(const TetraGraphicsVertex &v1,const TetraGraphicsVertex &v2,const TetraGraphicsVertex &v3) = 0;
	virtual void softMeshTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4) = 0;
    virtual void softMeshIndices(NxU32 *i1, NxU32 *i2, NxU32 *i3, NxU32 *i4) = 0;
	virtual void softMeshPosition(NxVec3 *pos) = 0;
};


// This pure-virtual interface binds to a graphics rendering interface.
class TetraGraphicsInterface
{
public:

  virtual void   setDevice(void *d3device,void *hwnd)=0;
  virtual void   onDeviceReset(void *d3device)=0;

  virtual bool   releaseMaterial(TetraMaterial *tm) = 0; // releases device handle for this material if one is present
  virtual void   setWireFrame(bool state) = 0;
  virtual void   setShowBackfaces(bool state) = 0;

	virtual void   setViewProjectionMatrix(const void *view,const void *projection) = 0;
  virtual void   setWorldMatrix(const void *world) = 0;

  virtual void * createVertexBuffer(unsigned int vcount,const TetraGraphicsVertex *vertices) = 0; // allocate a VB for this vertex data.
  virtual bool   releaseVertexBuffer(void *buffer)                    = 0;
  virtual void * lockVertexBuffer(void *buffer)                       = 0;
  virtual bool   unlockVertexBuffer(void *buffer)                     = 0;

  virtual void * createIndexBuffer(unsigned int icount,const unsigned int *indices)   = 0;
  virtual bool   releaseIndexBuffer(void *ibuffer)                    = 0;
  virtual void * lockIndexBuffer(void *buffer)                        = 0;
  virtual bool   unlockIndexBuffer(void *buffer)                      = 0;

  virtual bool   renderSection(TetraMaterial *material,void *buffer,void *ibufferm,unsigned int vcount,unsigned int tcount)            = 0;
  virtual bool   renderSection(TetraMaterial *material,void *vbuffer,unsigned int vcount) = 0;

  virtual void   addLine(unsigned int color,const float *p1,const float *p2) = 0;
  virtual void   flushLines(void) = 0;

  virtual bool   screenToWorld(int sx,      // screen x position
                               int sy,      // screen y position
                               float *world, // world position of the eye
                               float *direction) = 0; // direction vector into the world

  // render using UP data.
  virtual bool   renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int *ibuffer,unsigned int vcount,unsigned int tcount) = 0;
  virtual bool   renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int vcount) = 0;

  virtual void   wireBox(unsigned int color,const float *bmin,const float *bmax) = 0;

  virtual void   getEyePos(float *eye)=0;

};

extern TetraGraphicsInterface *gGraphicsInterface;
extern SoftFileInterface *gFileInterface;

}; // END OF SOFTBODY NAMESPACE




#endif
