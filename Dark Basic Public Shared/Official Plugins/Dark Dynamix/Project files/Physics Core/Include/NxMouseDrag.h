#ifndef NX_MOUSE_DRAG_H

#define NX_MOUSE_DRAG_H

#include "NxVec3.h"
#include "NxRay.h"

class NxPhysicsSDK;
class NxScene;
class NxShape;
class NxActor;
class NxSoftBody;
class NxCloth;

namespace SOFTBODY
{

class NxMouseDrag;



enum NxDragType
{
  NGT_NONE             = 0,
  NGT_STATIC_SHAPE     = (1<<0),
  NGT_DYNAMIC_SHAPE    = (1<<1),
  NGT_CLOTH            = (1<<2),
  NGT_SOFTBODY         = (1<<3),
  NGT_ALL              = (NGT_STATIC_SHAPE | NGT_DYNAMIC_SHAPE | NGT_CLOTH | NGT_SOFTBODY)
};

class NxDragInfo
{
public:
  NxDragInfo(void)
  {
    mDragType = NGT_NONE;
    mShape    = 0;
    mScene    = 0;
    mSelect.set(0,0,0);
    mIntersect.set(0,0,0);
    mRay.orig.set(0,0,0);
    mRay.dir.set(0,0,0);
    mDragPosition.set(0,0,0);
    mVertexID = 0;
    mLinearDamping = 0;
    mAngularDamping = 0;
    mSelectDistance = 0;
  }

  NxDragType  mDragType;
union
{
  NxShape    *mShape; // shape on an actor that is being dragged.
  NxCloth    *mCloth; // cloth that is being dragged.
  NxSoftBody *mSoftBody; // soft body that is being dragged.
};
  NxScene    *mScene;    // the scene that the object is from.
  NxVec3      mSelect;    // the original selection location (in object space if a shape)
  NxVec3      mIntersect; // the intersection location in worldspace
  NxVec3      mDragPosition; // the position we last 'dragged' to.
  NxRay       mRay;       // raycast in world space.
  NxU32       mVertexID;
  float       mLinearDamping;
  float       mAngularDamping;
  float       mSelectDistance;
};

NxMouseDrag * createMouseDrag(NxPhysicsSDK *sdk,  // sdk instance to do mouse dragging on
                              NxScene *scene,     // if only dragging on a specific scene, null means all scenes
                              int screenWid,      // width of the graphics screen
                              int screenHit);     // height of the graphics screen

void          releaseMouseDrag(NxMouseDrag *drag); // relase the mouse drag interface

NxDragInfo *  mouseDrag(NxMouseDrag *drag,        // the mouse drag interface
                        const float *view,        // the view matrix
                        const float *projection,  // the projection matrix
                        int screenX,              // the screen x position of the mouse
                        int screenY,              // the screen y position of the mouse
                        NxDragType types,         // the types of objects it is ok to drag
                        float dtime);             // delta time on last frame.

bool          isMouseSelected(NxMouseDrag *drag); // true if something is currently being dragged.

void          mouseDragDeselect(NxMouseDrag *drag); // indicate that a deselection event has occured.

void         mouseDragWheel(NxMouseDrag *drag,int delta); // moves the selected object closer to or further away from the camera based on the sign of the mouse wheel value.

};

#endif
