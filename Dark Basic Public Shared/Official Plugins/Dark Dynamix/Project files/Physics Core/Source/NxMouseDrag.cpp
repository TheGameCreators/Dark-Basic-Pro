#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include "NxMouseDrag.h"

#include "NxPhysics.h"
#include "NxScene.h"
#include "NxShape.h"
#include "NxActor.h"
#if NX_USE_SOFTBODY_API
#include "softbody/NxSoftBody.h"
#endif
#include "cloth/NxCloth.h"

namespace SOFTBODY
{

class NxMouseDrag
{
public:
  NxMouseDrag(NxPhysicsSDK *sdk,NxScene *scene,int screenWid,int screenHit)
  {
    mSDK       = sdk;
    mScene     = scene;
    mScreenWid = screenWid;
    mScreenHit = screenHit;
    mInfo.mDragType = NGT_NONE;
    mInfo.mShape    = 0;
    mInfo.mScene    = 0;
    mInfo.mSelect.set(0,0,0);
    mInfo.mIntersect.set(0,0,0);
    mInfo.mRay.orig.set(0,0,0);
		mInfo.mRay.dir.set(0,0,0);
  }

  ~NxMouseDrag(void)
  {
  }

  void doRaycast(NxScene *scene,const NxRay &ray,NxDragType types,float &closest,NxDragInfo &info)
  {
    if ( types & (NGT_STATIC_SHAPE | NGT_DYNAMIC_SHAPE) )
    {
      NxShape *shape = raycastShapes(scene,ray,types,closest,info.mIntersect);
      if ( shape )
      {
        NxActor *a = &shape->getActor();
        if ( a->isDynamic() )
          info.mDragType = NGT_DYNAMIC_SHAPE;
        else
          info.mDragType = NGT_STATIC_SHAPE;
        info.mShape = shape;
      }
    }

    if ( types & NGT_CLOTH )
    {
      NxCloth *cloth = raycastCloth(scene,ray,closest,info.mIntersect,info.mVertexID);
      if ( cloth )
      {
        info.mCloth = cloth;
        info.mDragType = NGT_CLOTH;
      }
    }

    if ( types & NGT_SOFTBODY )
    {
      NxSoftBody *sbody = raycastSoftBodies(scene,ray,closest,info.mIntersect,info.mVertexID);
      if ( sbody )
      {
        info.mSoftBody = sbody;
        info.mDragType = NGT_SOFTBODY;
      }
    }
  }

  NxDragInfo * doRaycast(const NxRay &ray,NxDragType types,NxDragInfo &info)
  {
    NxDragInfo *ret = 0;
    float closest = FLT_MAX;
    info.mDragType = NGT_NONE;
    if ( mScene )
    {
      doRaycast(mScene,ray,types,closest,info);
    }
    else
    {
      NxU32 nbscenes = mSDK->getNbScenes();
      for (NxU32 i=0; i<nbscenes; i++)
      {
        NxScene *scene = mSDK->getScene(i);
        doRaycast(scene,ray,types,closest,info);
      }
    }

    if ( info.mDragType != NGT_NONE )
    {

      if ( info.mDragType == NGT_STATIC_SHAPE || info.mDragType == NGT_DYNAMIC_SHAPE )
      {
        NxMat34 pose = info.mShape->getActor().getGlobalPose();
        pose.multiplyByInverseRT(info.mIntersect,info.mSelect); // put it back into object space of the actor
      }
      else
      {
        info.mSelect = info.mIntersect;
      }



      info.mRay = ray;
      ret = &info;
    }
		return ret;
  }

  void set(NxMat34 &mat,const float *p)
  {
    mat.M.setColumnMajorStride4(p);
    mat.t.x = p[12];
    mat.t.y = p[13];
    mat.t.z = p[14];
  }

  bool   screenToWorld(int sx,      // screen x position
                       int sy,      // screen y position
											 int wid,
											 int hit,
                       const float *_view,
                       const float *_projection,
                       NxRay &ray)
  {
    bool ret = false;


    if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
    {
    	NxVec3 v;
    	v.x =  ( ( ( 2.0f * sx ) / wid  ) - 1 ) / _projection[0*4+0];
    	v.y = -( ( ( 2.0f * sy ) / hit ) - 1 )  / _projection[1*4+1];
    	v.z =  1.0f;

    	// Get the inverse view matrix
      NxMat34 view;
      set(view,_view);            // copy the 4x4 graphics transform to an NxMat34
    	NxMat34 m;
      view.getInverse(m); // invert the matrix

      float matrix[16];
      m.M.getColumnMajorStride4(matrix); // get it back into graphics format.
			matrix[12] = m.t.x;
			matrix[13] = m.t.x;
			matrix[14] = m.t.x;


      ray.dir.x = v.x*matrix[0*4+0] + v.y*matrix[1*4+0] + v.z*matrix[2*4+0];
      ray.dir.y = v.x*matrix[0*4+1] + v.y*matrix[1*4+1] + v.z*matrix[2*4+1];
      ray.dir.z = v.x*matrix[0*4+2] + v.y*matrix[1*4+2] + v.z*matrix[2*4+2];

      ray.dir.normalize();
    	ray.orig = m.t;

      ret = true;
    }

    return ret;
  }

  NxDragInfo *  mouseDrag(const float *view,const float *projection,int screenX,int screenY,NxDragType types,float dtime)
  {
    NxDragInfo *ret = 0;

    if ( mInfo.mDragType == NGT_NONE )
    {
      NxRay ray;
      bool ok = screenToWorld(screenX,screenY,mScreenWid,mScreenHit,view,projection,ray);
      if ( ok )
      {
        ret = doRaycast(ray,types,mInfo);
        if ( mInfo.mDragType != NGT_NONE )
        {

          mInfo.mSelectDistance = mInfo.mRay.orig.distance(mInfo.mIntersect);
          mInfo.mDragPosition = mInfo.mIntersect;

          if ( mInfo.mDragType == NGT_DYNAMIC_SHAPE )
          {
            NxActor &a = mInfo.mShape->getActor();
            mInfo.mLinearDamping = a.getLinearDamping();
            mInfo.mAngularDamping = a.getAngularDamping();
            a.setLinearDamping(1.0f);
            a.setAngularDamping(1.0f);
          }
        }
      }
    }
    else
    {
      NxRay ray;
      bool ok = screenToWorld(screenX,screenY,mScreenWid,mScreenHit,view,projection,ray);
      if ( ok )
      {
        mInfo.mRay = ray; // the new ray..
        switch ( mInfo.mDragType )
        {
          case NGT_STATIC_SHAPE: // can't drag static shapes, though we could consider moving them kinematically!
            break;
          case NGT_DYNAMIC_SHAPE:
            if ( 1 )
            {
              NxVec3 spos;
              NxVec3 pos   = ray.orig + (ray.dir*mInfo.mSelectDistance); // ok, now project where we are dragging to!

              NxActor& a   = mInfo.mShape->getActor();

              NxMat34 pose = a.getGlobalPose();
              pose.multiply(mInfo.mSelect,spos);

							float mass = a.getMass();

              NxVec3 currVel   = a.getLinearVelocity();
              NxVec3 targetVel = (pos - spos)*16.0f;

              targetVel-=currVel;

              float dragForce = 1500.0f;

              NxVec3 force = mass*targetVel*dtime*dragForce;

              a.addForceAtPos(force,spos,NX_FORCE);

              mInfo.mIntersect = spos;
              mInfo.mDragPosition = pos;

            }
            break;
          case NGT_CLOTH:
            if ( 1 )
            {
              NxVec3 pos = ray.orig + (ray.dir*mInfo.mSelectDistance); // ok, now project where we are dragging to!
              mInfo.mCloth->attachVertexToGlobalPosition(mInfo.mVertexID,pos);
              mInfo.mIntersect = pos;
              mInfo.mDragPosition = pos;
            }
            break;
          case NGT_SOFTBODY:
            if ( 1 )
            {
              NxVec3 pos = ray.orig + (ray.dir*mInfo.mSelectDistance); // ok, now project where we are dragging to!
#if NX_USE_SOFTBODY_API
              mInfo.mSoftBody->attachVertexToGlobalPosition(mInfo.mVertexID,pos);
#endif
              mInfo.mIntersect = pos;
              mInfo.mDragPosition = pos;
            }
            break;
        }
      }

      ret = &mInfo;

    }

    return ret;
  }

	bool isMouseSelected(void)
	{
		bool ret = false;

    if ( mInfo.mDragType != NGT_NONE )
    	ret = true;

    return ret;
  }

  void mouseDragDeselect(void)
  {

    switch ( mInfo.mDragType )
    {
      case NGT_DYNAMIC_SHAPE:
        if ( 1 )
        {
          NxActor &a = mInfo.mShape->getActor();
          a.setLinearDamping(mInfo.mLinearDamping);
          a.setAngularDamping(mInfo.mAngularDamping);
        }
        break;
      case NGT_CLOTH:
        mInfo.mCloth->freeVertex(mInfo.mVertexID);
        break;
      case NGT_SOFTBODY:
#if NX_USE_SOFTBODY_API
        mInfo.mSoftBody->freeVertex(mInfo.mVertexID);
#endif
        break;
    }

    mInfo.mDragType = NGT_NONE;
    mInfo.mShape    = 0;
    mInfo.mVertexID = 0;
  }

  NxShape *raycastShapes(NxScene *scene,const NxRay &ray,NxDragType types,float &closest,NxVec3 &intersect)
  {
    NxShape *ret = 0;

    NxShapesType raycastType = NX_ALL_SHAPES;
		NxRaycastHit rayImpact;
		ret = mScene->raycastClosestShape(ray, raycastType, rayImpact);
		if ( ret )
		{
			intersect = rayImpact.worldImpact;
		}

    return ret;
  }

  NxSoftBody * raycastSoftBodies(NxScene *scene,const NxRay &ray,float &closest,NxVec3 &intersect,NxU32 &_vertexID)
  {
    NxSoftBody *ret = 0;
#if NX_USE_SOFTBODY_API
		NxSoftBody **softBodies = scene->getSoftBodies();
    NxU32 sbcount = scene->getNbSoftBodies();
		for (NxU32 j=0; j <sbcount; ++j )
		{
			NxSoftBody *softBody = softBodies[j];
      NxU32 vertexId;
      NxVec3 hit;
      if ( softBody && softBody->raycast( ray, hit, vertexId ) )
			{
        float dist = ray.orig.distanceSquared(hit);
        if ( dist < closest )
        {
          closest = dist;
          ret = softBody;
          intersect = hit;
          _vertexID = vertexId;
        }
      }
    }
#endif
    return ret;
  }

  NxCloth * raycastCloth(NxScene *scene,const NxRay &ray,float &closest,NxVec3 &intersect,NxU32 &_vertexID)
  {
    NxCloth *ret = 0;
		NxCloth **clothBodies = scene->getCloths();
    NxU32 count = scene->getNbCloths();
		for (NxU32 j=0; j <count; ++j )
		{
			NxCloth *clothBody = clothBodies[j];
      NxU32 vertexId;
      NxVec3 hit;
      if ( clothBody && clothBody->raycast( ray, hit, vertexId ) )
			{
        float dist = ray.orig.distanceSquared(hit);
        if ( dist < closest )
        {
          closest = dist;
          intersect = hit;
          ret = clothBody;
          _vertexID = vertexId;
        }
      }
    }
    return ret;
  }

	void processMouseWheel(int delta)
	{
		if ( delta >= 0 )
			mInfo.mSelectDistance*=1.02f;
		else
			mInfo.mSelectDistance*=0.98f;
	}

private:
  NxDragInfo    mInfo;
  NxPhysicsSDK *mSDK;
  NxScene      *mScene;
  int           mScreenWid;
  int           mScreenHit;

};

NxMouseDrag * createMouseDrag(NxPhysicsSDK *sdk,NxScene *scene,int screenWid,int screenHit)
{
  NxMouseDrag *ret = 0;

  ret = new NxMouseDrag(sdk,scene,screenWid,screenHit);

  return ret;
}

void          releaseMouseDrag(NxMouseDrag *drag)
{
  delete drag;
}

NxDragInfo *  mouseDrag(NxMouseDrag *drag,const float *view,const float *projection,int screenX,int screenY,NxDragType types,float dtime)
{
  NxDragInfo *ret = 0;

  ret = drag->mouseDrag(view,projection,screenX,screenY,types,dtime);

  return ret;
}

void          mouseDragDeselect(NxMouseDrag *drag)
{
  drag->mouseDragDeselect();
}

void mouseDragWheel(NxMouseDrag *drag,int delta) // moves the selected object closer to or further away from the camera based on the sign of the mouse wheel value.
{
	drag->processMouseWheel(delta);
}

bool          isMouseSelected(NxMouseDrag *drag) // true if something is currently being dragged.
{
	return drag->isMouseSelected();
}

};
