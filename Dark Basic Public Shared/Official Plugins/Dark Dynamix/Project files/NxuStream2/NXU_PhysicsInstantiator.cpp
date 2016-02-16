/*----------------------------------------------------------------------
Copyright	(c)	2010 NVIDIA Corporation

NxuPhysicsInstantiator.cpp

This is	the	common import	implementation that	contains format	independent	import logic.

Changelist
----------

 */

#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

#include "NXU_PhysicsInstantiator.h"
#include "NXU_cooking.h"
#include "NXU_Streaming.h"
#include "NXU_string.h"
#include "NXU_customcopy.h"

namespace	NXU
{

static const char * getStr(const char *c)
{
  const char *ret = "null";
  if ( c && strlen(c) ) ret = c;
  return ret;
}

NxShape * getShapeFromIndex(NxActor *a,NxU32 index)
{
	NxShape *ret = 0;
	NxU32 nbShapes = a->getNbShapes();
	if ( index >= 0 && index < nbShapes )
	{
		NxShape *const* shapes = a->getShapes();
		ret = shapes[index];
	}
	return ret;
}

NxuPhysicsInstantiator::NxuPhysicsInstantiator(NxuPhysicsCollection	*coll)
{
	mCollection	=	coll;
	instanceDefaultScene = 0;
	mCurrentScene = 0;
}

NxuPhysicsInstantiator::NxuPhysicsInstantiator(void)
{
	mCollection	=	0;
	instanceDefaultScene = 0;
	mCurrentScene = 0;
}

NxuPhysicsInstantiator::~NxuPhysicsInstantiator()
{
}

void NxuPhysicsInstantiator::instanceSkeletons(NxPhysicsSDK	&sdk,NXU_userNotify *callback)
{
  CustomCopy cc(mCollection,0);

	NxU32	size = mCollection->mSkeletons.size();
	for	(NxU32 i = 0;	i	<	size;	++i)
	{
		NxCCDSkeleton	*skel	=	0;
		NxCCDSkeletonDesc *sd = mCollection->mSkeletons[i];
		if (sd)
		{
			char tempString[512];
			sprintf(tempString,	"%s+%s", getStr(mCollection->mId), getStr(sd->mId));
			skel = (NxCCDSkeleton*)findInstance(tempString);
			if (skel ==	0)
			{
				bool ok = true;

        NxSimpleTriangleMesh *stm = static_cast< NxSimpleTriangleMesh *>(sd);
				::NxSimpleTriangleMesh desc;
        stm->copyTo(desc,cc);

				if ( callback )
				{
					ok = callback->NXU_preNotifyCCDSkeleton(desc,sd->mUserProperties);
				}
				if ( ok )
				{
					skel = sdk.createCCDSkeleton(desc);
				}
				if (skel)
				{
					if ( callback )
						callback->NXU_notifyCCDSkeleton(skel,sd->mUserProperties);
					setInstance(tempString,	skel);
				}
				else
				{
					if ( callback )
						callback->NXU_notifyCCDSkeletonFailed(desc,sd->mUserProperties);
					reportWarning("Failed to create CCD skeleton.");
				}
			}
			sd->mInstance	=	skel;
		}
	}
}

void NxuPhysicsInstantiator::instanceConvexes(NxPhysicsSDK &sdk,NXU_userNotify *callback)
{

  CustomCopy cc(mCollection,0);

	// now load	 up	any	tri	meshes
	NxU32	convexSize = mCollection->mConvexMeshes.size();

	for	(NxU32 i = 0;	i	<	convexSize;	++i)
	{

		NxConvexMesh *cm = 0;
		NxConvexMeshDesc	*cmd = mCollection->mConvexMeshes[i];

		if (cmd)
		{
			char tempString[512];
			sprintf(tempString,	"%s+%s", getStr(mCollection->mId), getStr(cmd->mId) );
			cm = (NxConvexMesh*)findInstance(tempString);

			if (cm ==	0)
			{
        cm = instantiateConvexMesh(sdk,*cmd,tempString,callback,cc);
			}
			cmd->mInstance = cm;
		}
	}
}

#if NX_USE_CLOTH_API

void NxuPhysicsInstantiator::instanceClothMeshes(NxPhysicsSDK &sdk,NXU_userNotify *callback)
{

  CustomCopy cc(mCollection,0);

	NxU32	clothSize = mCollection->mClothMeshes.size();

	for	(NxU32 i = 0;	i	<	clothSize;	++i)
	{
		NxClothMesh *cm = 0;
		NxClothMeshDesc	*cmd = mCollection->mClothMeshes[i];
		if (cmd)
		{
			char tempString[512];
			sprintf(tempString,	"%s+%s", getStr(mCollection->mId), getStr(cmd->mId) );
			cm = (NxClothMesh*)findInstance(tempString);

			if (cm ==	0)
			{
				bool ok = true;
        ::NxClothMeshDesc desc;
        cmd->copyTo(desc,cc);

				if ( callback )
				{
					ok = callback->NXU_preNotifyClothMesh(desc,cmd->mUserProperties);
				}
				if ( ok )
				{
					bool status	=	true;

					InitCooking();

					ImportMemoryWriteBuffer	writeBuffer;

					status = CookClothMesh(desc,	writeBuffer);

					CloseCooking();

					if (status)
					{
						ImportMemoryReadBuffer importMemoryReadBuffer(writeBuffer.data);
						cm = sdk.createClothMesh(importMemoryReadBuffer);
					}
					else
					{
						reportWarning("Failed to cook cloth mesh");
					}

					if (cm)
					{
						if ( callback )
							callback->NXU_notifyClothMesh(cm,cmd->mUserProperties);
						setInstance(tempString,	cm);
					}
					else
					{
						if ( callback )
						{
							callback->NXU_notifyClothMeshFailed(desc,cmd->mUserProperties);
						}
						reportWarning("Cloth Mesh failed to be created.");
					}
				}
			}

			cmd->mInstance = cm;

		}
	}
}


#endif

#if NX_USE_SOFTBODY_API

void NxuPhysicsInstantiator::instanceSoftBodyMeshes(NxPhysicsSDK &sdk,NXU_userNotify *callback)
{

  CustomCopy cc(mCollection,0);

	NxU32	softBodySize = mCollection->mSoftBodyMeshes.size();

	for	(NxU32 i = 0;	i	<	softBodySize;	++i)
	{
		NxSoftBodyMesh *cm = 0;
		NxSoftBodyMeshDesc	*cmd = mCollection->mSoftBodyMeshes[i];

		if (cmd)
		{
			char tempString[512];
			sprintf(tempString,	"%s+%s", getStr(mCollection->mId), getStr(cmd->mId) );
			cm = (NxSoftBodyMesh*)findInstance(tempString);

			if (cm ==	0)
			{
				bool ok = true;

        ::NxSoftBodyMeshDesc desc;

        cmd->copyTo(desc,cc);

				if ( callback )
				{
					ok = callback->NXU_preNotifySoftBodyMesh(desc,cmd->mUserProperties);
				}
				if ( ok )
				{
					bool status	=	true;

					InitCooking();

					ImportMemoryWriteBuffer	writeBuffer;

					status = CookSoftBodyMesh(desc,	writeBuffer);

					CloseCooking();

					if (status)
					{
						ImportMemoryReadBuffer importMemoryReadBuffer(writeBuffer.data);
						cm = sdk.createSoftBodyMesh(importMemoryReadBuffer);
					}
					else
					{
						reportWarning("Failed to cook softBody mesh");
					}

					if (cm)
					{
						if ( callback )
							callback->NXU_notifySoftBodyMesh(cm,cmd->mUserProperties);
						setInstance(tempString,	cm);
					}
					else
					{
						if ( callback )
						{
							callback->NXU_notifySoftBodyMeshFailed(desc,cmd->mUserProperties);
						}
						reportWarning("SoftBody Mesh failed to be created.");
					}
				}
			}

			cmd->mInstance = cm;

		}
	}
}


#endif

NxTriangleMesh * NxuPhysicsInstantiator::instantiateTriangleMesh(NxPhysicsSDK &sdk,NxTriangleMeshDesc &desc,const char *collectionId,NXU_userNotify	*callback,CustomCopy &cc)
{
	NxTriangleMesh *tm = 0;
  NxTriangleMeshDesc *tmd = &desc;

	char tempString[512];
	sprintf(tempString,	"%s+%s", getStr(collectionId), getStr(tmd->mId));
	tm = (NxTriangleMesh*)findInstance(tempString);
	if (tm ==	0)
	{
    ::NxTriangleMeshDesc desc;
		tmd->copyTo(desc,cc);
		bool ok = true;
		if (callback)
		{
			ok = callback->NXU_preNotifyTriangleMesh(desc,tmd->mUserProperties);
		}
		if ( ok )
		{
			if (tmd->mCookedDataSize == 0 )
			{
				desc.pmap	=	0;
				bool status	=	true;
				InitCooking();
				ImportMemoryWriteBuffer	writeBuffer;
				status = CookTriangleMesh(desc,	writeBuffer);
				CloseCooking();
				if (status)
				{
					tm = sdk.createTriangleMesh(ImportMemoryReadBuffer(writeBuffer.data));
				}
  		}
			else
			{
				ImportMemoryReadBuffer readBuffer((const NxU8*)&tmd->mCookedData[0]);
				tm = sdk.createTriangleMesh(readBuffer);
			}
      if ( tm )
      {
        if ( tmd->mPmapData.size() )
        {
          NxU8 *data = &tmd->mPmapData[0];
		  		NxPMap p;
					p.data = data;
					p.dataSize = tmd->mPmapData.size();
					tm->loadPMap(p);
        }
      }

  		if (tm)
			{
				if ( callback )
					callback->NXU_notifyTriangleMesh(tm,0);
				setInstance(tempString,	tm);
			}
			else
			{
				if ( callback )
					callback->NXU_notifyTriangleMeshFailed(desc,tmd->mUserProperties);
				reportWarning("Failed to create TriangleMesh");
			}
		}
	}

	tmd->mInstance = tm;

  return tm;
}

void NxuPhysicsInstantiator::instanceTrimeshes(NxPhysicsSDK &sdk, NXU_userNotify	*callback)
{
	// now load	 up	any	tri	meshes
  CustomCopy cc(mCollection,0);

	NxU32	triSize	=	mCollection->mTriangleMeshes.size();

	for	(NxU32 i = 0;	i	<	triSize; ++i)
	{
		NxTriangleMeshDesc	*tmd = mCollection->mTriangleMeshes[i];
    instantiateTriangleMesh(sdk,*tmd,mCollection->mId,callback,cc);
	}
}

void NxuPhysicsInstantiator::instanceHeightFields(NxPhysicsSDK &sdk,NXU_userNotify *callback)
{
	NxU32	size = mCollection->mHeightFields.size();
	for	(NxU32 i = 0;	i	<	size;	++i)
	{
		NxHeightFieldDesc *hfd	=	mCollection->mHeightFields[i];
		if (hfd)
		{
			NxHeightField	*hf;
			char tempString[512];
			sprintf(tempString,	"%s+%s", getStr(mCollection->mId), getStr(hfd->mId));
			hf = (NxHeightField*)findInstance(tempString);
			if (hf ==	0)
			{
				::NxHeightFieldDesc desc;
				CustomCopy cc(mCollection,0);
				hfd->copyTo(desc,cc);
				bool ok = true;
				if ( callback )
				{
					ok = callback->NXU_preNotifyHeightField(desc,hfd->mUserProperties);
				}
				if ( ok )
				{
					hf = sdk.createHeightField(desc);
					if (hf)
					{
						if ( callback )
							callback->NXU_notifyHeightField(hf,hfd->mUserProperties);
						setInstance(tempString,	hf);
					}
					else
					{
						if ( callback )
							callback->NXU_notifyHeightFieldFailed(desc,hfd->mUserProperties);
						reportWarning("Failed to create HeightField");
					}
				}
			}
			hfd->mInstance = hf;
		}
	}
}

#define THRESHOLD 0.00001f

bool same(NxF32 a,NxF32 b)
{
  NxF32 diff = fabsf(a-b);
  return (diff<THRESHOLD);
}

bool same(const NxVec3 &a,const NxVec3 &b)
{
  bool ret = false;
  if ( same(a.x,b.x) &&
       same(a.y,b.y) &&
       same(a.z,b.z) ) ret = true;
  return ret;
}

bool sameMaterial(NxMaterialDesc &m1,::NxMaterialDesc &m2)
{
  bool ret = false;

  if ( same(m1.dynamicFriction,m2.dynamicFriction) &&
       same(m1.staticFriction,m2.staticFriction) &&
       same(m1.restitution,m2.restitution) &&
       same(m1.dynamicFrictionV,m2.dynamicFrictionV) &&
       same(m1.staticFrictionV,m2.staticFrictionV) &&
       (NxU32)m1.frictionCombineMode==(NxU32)m2.frictionCombineMode &&
       (NxU32)m1.restitutionCombineMode==(NxU32)m2.restitutionCombineMode &&
       same(m1.dirOfAnisotropy,m2.dirOfAnisotropy) &&
       (NxU32)m1.flags == m2.flags )
  {
    ret = true;
  }

  return ret;
}

#if NX_SDK_VERSION_NUMBER >= 260
bool sameCompartment(NxCompartmentDesc &c1,::NxCompartmentDesc &c2)
{
	bool ret = false;

	if ( c1.deviceCode == c2.deviceCode &&
#if NX_SDK_VERSION_NUMBER >= 270
		(NxU32)c1.flags == c2.flags &&
		same(c1.timeScale, c2.timeScale) &&
#endif
		same(c1.gridHashCellSize, c2.gridHashCellSize) &&
		c1.gridHashTablePower == c2.gridHashTablePower &&
		(NxU32)c1.type == (NxU32)c2.type )
	{
		ret = true;
	}

	return ret;
}
#endif

void NxuPhysicsInstantiator::instanceModel(NxScene &scene, NxSceneDesc	&model,	NxMat34	&pose, bool	isHSM, NXU_userNotify	*callback, NX_BOOL ignorePlane)
{

  CustomCopy cc(mCollection,&model);

//*** Instance materials

#if 0
	NxU32	count	=	model.mMaterials.size();

	for	(NxU32 i = 0;	i	<	count; ++i)
	{

		NxMaterialDesc	*mat = model.mMaterials[i];
    NxU32 nbMaterials = scene.getNbMaterials();

    bool match = false;
    for (NxU32 i=0; i<nbMaterials; i++)
    {
      NxMaterial *m = scene.getMaterialFromIndex(i);
      ::NxMaterialDesc md;
      m->saveToDesc(md);
      if ( sameMaterial(*mat,md) )
      {
        match = true;
        mat->mInstanceIndex = i;
        break;
      }
    }
    if ( !match )
    {
      ::NxMaterialDesc md;
      mat->copyTo(md,cc);
      NxMaterial *mm = scene.createMaterial(md);
      if ( mm )
      {
        mat->mInstanceIndex = nbMaterials;
      }

    }
	}
#else
	NxU32	count	=	model.mMaterials.size();

	for	(NxU32 i = 0;	i	<	count; ++i)
	{
		NxMaterialDesc	*mat = model.mMaterials[i];
		::NxMaterialDesc desc;
		mat->mInstanceIndex = mat->mMaterialIndex;
		mat->copyTo(desc,cc);

		bool ok = desc.isValid();

		if (callback)
			ok = callback->NXU_preNotifyMaterial(desc, mat->mUserProperties);

		if (ok)
		{
			NxMaterial *instance;

			NxU32 nbMaterials = scene.getNbMaterials();
			if ( mat->mMaterialIndex >= nbMaterials )
			{
    			for (NxU32 j=nbMaterials; j<=mat->mMaterialIndex; j++)
    			{
    				instance = scene.createMaterial(desc);
					if (!instance)
					{
						ok = false;
						break;
					}
    			}
			}
			else
			{
					instance = scene.getMaterialFromIndex(i);
					instance->loadFromDesc(desc);
			}

			if (callback)
			{
				if (ok)
					callback->NXU_notifyMaterial(instance, mat->mUserProperties);
				else
					callback->NXU_notifyMaterialFailed(desc, mat->mUserProperties);
			}
		}
	}
#endif

//*** Instance actors
	count	=	model.mActors.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{
		NxActorDesc *actor	=	model.mActors[i];
		actor->mInstance = 0;
    ::NxActorDesc desc;
    actor->copyTo(desc,cc);

		bool isOk = false;

    ::NxBodyDesc body;
    if ( actor->mHasBody )
    {
      actor->mBody.copyTo(body,cc);
      desc.body = &body;
    }

		bool hasPlane	=	false;

		NxArray< ::NxShapeDesc * > slist;

		for	(NxU32 j = 0;	j	<	actor->mShapes.size();	j++)
		{
      ::NxShapeDesc *shapeDesc=0;

			NxShapeDesc	*shape = actor->mShapes[j];

			switch (shape->mType )
			{
				case SC_NxPlaneShapeDesc:
					{
						::NxPlaneShapeDesc *d1 = new ::NxPlaneShapeDesc;
						NxPlaneShapeDesc *s	=	static_cast<NxPlaneShapeDesc*>(shape);
            s->copyTo(*d1,cc);
            shapeDesc = d1;
						isOk = d1->isValid();
						hasPlane = true;
					}
					break;
				case SC_NxSphereShapeDesc:
					{
						::NxSphereShapeDesc *d2 = new ::NxSphereShapeDesc;
						NxSphereShapeDesc *s	=	static_cast<NxSphereShapeDesc*>(shape);
            s->copyTo(*d2,cc);
            shapeDesc = d2;
						isOk = d2->isValid();
					}
					break;
				case SC_NxBoxShapeDesc:
					{
						::NxBoxShapeDesc *d3 = new ::NxBoxShapeDesc;
						NxBoxShapeDesc *s	=	static_cast<NxBoxShapeDesc*>(shape);
            s->copyTo(*d3,cc);
            shapeDesc = d3;
						isOk = d3->isValid();
					}
					break;
				case SC_NxCapsuleShapeDesc:
					{
						::NxCapsuleShapeDesc *d4 = new ::NxCapsuleShapeDesc;
						NxCapsuleShapeDesc *s	=	static_cast<NxCapsuleShapeDesc*>(shape);
            s->copyTo(*d4,cc);
            shapeDesc = d4;
						isOk = d4->isValid();
					}
					break;
				case SC_NxWheelShapeDesc:
					{
						::NxWheelShapeDesc *d5 = new ::NxWheelShapeDesc;
						NxWheelShapeDesc *s	=	static_cast<NxWheelShapeDesc*>(shape);
            s->copyTo(*d5,cc);
            shapeDesc = d5;
						isOk = d5->isValid();
					}
					break;
				case SC_NxConvexShapeDesc:
					{
						::NxConvexShapeDesc *d6 = new ::NxConvexShapeDesc;
						NxConvexShapeDesc *s = static_cast<NxConvexShapeDesc*>(shape);
						s->copyTo(*d6,cc);
            shapeDesc = d6;
						isOk = d6->isValid();
					}
					break;
				case SC_NxTriangleMeshShapeDesc:
					{
						::NxTriangleMeshShapeDesc *d7 = new ::NxTriangleMeshShapeDesc;
						NxTriangleMeshShapeDesc *s = static_cast<NxTriangleMeshShapeDesc*>(shape);
						s->copyTo(*d7,cc);
            shapeDesc = d7;
						isOk = d7->isValid();
					}
					break;
				case SC_NxHeightFieldShapeDesc:
					{
						::NxHeightFieldShapeDesc *d8 = new ::NxHeightFieldShapeDesc;
						NxHeightFieldShapeDesc *s = static_cast<NxHeightFieldShapeDesc*>(shape);
						s->copyTo(*d8,cc);
            shapeDesc = d8;
						isOk = d8->isValid();
					}
					break;
				default:
					NX_ASSERT(false);	//Unknown	shape	type
			}

      if ( shapeDesc )
      {
				if ( isOk )
				{
					shapeDesc->ccdSkeleton = cc.getSkeletonFromName(shape->mCCDSkeleton);
  			  shapeDesc->userData = (void *)shape->mUserProperties;
          desc.shapes.push_back(shapeDesc);
          slist.push_back(shapeDesc);
				}
				else
				{
					reportWarning("Failed to construct valid shape descriptor type(%s) for Actor (%s)", EnumToString( shape->mType) , actor->mId );
				}
      }
			else
			{
				reportWarning("Unable to construct valid shape descriptor type(%s) for Actor (%s)", EnumToString( shape->mType) , actor->mId );
			}

			
		}

		if (hasPlane &&	ignorePlane)
	  {
    }
		else
		{
			desc.globalPose.multiply(pose, desc.globalPose);

			bool ok	=	true;

			ok = desc.isValid();
			if ( !ok )
			{
				ok = desc.isValid();
			}

			if (callback)
			{
				ok = callback->NXU_preNotifyActor(desc,	actor->mUserProperties);
			}

//			assert( desc.isValid() );
//			desc.isValid();

			if (ok)
			{

				NxActor *a = scene.createActor(desc);
				actor->mInstance = a;

				if ( a )
				{
					NxU32 nbShapes = a->getNbShapes();
					if ( nbShapes && actor->mShapes.size() == nbShapes )
					{
						NxShape *const* shapes = a->getShapes();
						for (NxU32 i=0; i<nbShapes; i++)
						{
							NxShapeDesc *sd = actor->mShapes[i];
              sd->mInstance = shapes[i];
						}
					}
					if ( callback )
					{
  					callback->NXU_notifyActor(a,	actor->mUserProperties);
  				}
				}
				else
				{
					if ( callback )
						callback->NXU_notifyActorFailed(desc, actor->mUserProperties);
					reportWarning("Failed to create actor '%s'", desc.name );
				}
			}
		}


		for (unsigned int i=0; i<slist.size(); i++)
		{
			::NxShapeDesc *s = slist[i];
			delete s;
		}

	}


//********************************************** do collision pair flags

	count	=	model.mPairFlags.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{

		NxPairFlagDesc	*d = model.mPairFlags[i];

		NxActor *a0 = cc.getActorFromName( d->mActor0 );
		NxActor *a1 = cc.getActorFromName( d->mActor1 );

		if ( a0 && a1 )
		{
  		if ( d->mIsActorPair )
	  	{
	  		scene.setActorPairFlags(*a0,*a1,d->mFlags );
  		}
			else
			{

				NxShape *s0 = getShapeFromIndex(a0,d->mShapeIndex0);
				NxShape *s1 = getShapeFromIndex(a1,d->mShapeIndex1);

				if ( s0 && s1 )
				{
	  			scene.setShapePairFlags(*s0,*s1,d->mFlags );
				}
			}
		}
	}

//******************************************* do contraints
	count	=	model.mJoints.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{
		NxJointDesc *v	=	model.mJoints[i];
		::NxD6JointDesc 					j1;
		::NxCylindricalJointDesc  j2;
		::NxDistanceJointDesc 		j3;
		::NxFixedJointDesc    		j4;
		::NxPointInPlaneJointDesc j5;
		::NxPointOnLineJointDesc 	j6;
		::NxPrismaticJointDesc 		j7;
		::NxRevoluteJointDesc 		j8;
		::NxSphericalJointDesc 		j9;
		::NxPulleyJointDesc 			j10;

    ::NxJointDesc *jdesc = 0;

    switch ( v->mType )
    {
      case SC_NxD6JointDesc:
        if ( 1 )
        {
          NxD6JointDesc *p = static_cast<NxD6JointDesc *>(v);
          p->copyTo(j1,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j1);
        }
        break;
      case SC_NxCylindricalJointDesc:
        if ( 1 )
        {
          NxCylindricalJointDesc *p = static_cast<NxCylindricalJointDesc *>(v);
          p->copyTo(j2,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j2);
        }
        break;
      case SC_NxDistanceJointDesc:
        if ( 1 )
        {
          NxDistanceJointDesc *p = static_cast<NxDistanceJointDesc *>(v);
          p->copyTo(j3,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j3);
        }
        break;
      case SC_NxFixedJointDesc:
        if ( 1 )
        {
          NxFixedJointDesc *p = static_cast<NxFixedJointDesc *>(v);
          p->copyTo(j4,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j4);
        }
        break;
      case SC_NxPointInPlaneJointDesc:
        if ( 1 )
        {
          NxPointInPlaneJointDesc *p = static_cast<NxPointInPlaneJointDesc *>(v);
          p->copyTo(j5,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j5);
        }
        break;
      case SC_NxPointOnLineJointDesc:
        if ( 1 )
        {
          NxPointOnLineJointDesc *p = static_cast<NxPointOnLineJointDesc *>(v);
          p->copyTo(j6,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j6);
        }
        break;
      case SC_NxPrismaticJointDesc:
        if ( 1 )
        {
          NxPrismaticJointDesc *p = static_cast<NxPrismaticJointDesc *>(v);
          p->copyTo(j7,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j7);
        }
        break;
      case SC_NxRevoluteJointDesc:
        if ( 1 )
        {
          NxRevoluteJointDesc *p = static_cast<NxRevoluteJointDesc *>(v);
          p->copyTo(j8,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j8);
        }
        break;
      case SC_NxSphericalJointDesc:
        if ( 1 )
        {
          NxSphericalJointDesc *p = static_cast<NxSphericalJointDesc *>(v);
          p->copyTo(j9,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j9);
        }
        break;
      case SC_NxPulleyJointDesc:
        if ( 1 )
        {
          NxPulleyJointDesc *p = static_cast<NxPulleyJointDesc *>(v);
          p->copyTo(j10,cc);
          jdesc = static_cast< ::NxJointDesc *>(&j10);
        }
        break;
      default:
	break;
    }


		assert(jdesc);

		if ( jdesc )
		{
			// If one attachment is the world frame, it must be adjusted by the scene transform.
			if (jdesc->actor[0] == 0)
			{
				pose.multiply(jdesc->localAnchor[0], jdesc->localAnchor[0]);
				pose.M.multiply(jdesc->localAxis[0], jdesc->localAxis[0]);
				pose.M.multiply(jdesc->localNormal[0], jdesc->localNormal[0]);
			}
			if (jdesc->actor[1] == 0)
			{
				pose.multiply(jdesc->localAnchor[1], jdesc->localAnchor[1]);
				pose.M.multiply(jdesc->localAxis[1], jdesc->localAxis[1]);
				pose.M.multiply(jdesc->localNormal[1], jdesc->localNormal[1]);
			}
  		bool ok	=	true;

			jdesc->isValid();

  		if (callback)
  		{
  			ok = callback->NXU_preNotifyJoint(*jdesc,	v->mUserProperties);
  		}
  		if (ok)
  		{
  			NxJoint *jt = scene.createJoint(*jdesc);
  			if ( jt)
  			{

				NxU32	planes = v->mPlaneInfo.size();
				if (planes)
				{
					jt->setLimitPoint(v->mPlaneLimitPoint, v->mOnActor2	?	true : false);
				}
				for	(NxU32 p = 0;	p	<	planes;	++p)
				{
					NxPlaneInfoDesc *pInfo = v->mPlaneInfo[p];

					// determine a point on the limit plane
					// planeD = -(planeNormal) DOT (pointOnPlane)
					// for pointOnPlane[i] == pointOnPlane[j] == 0
					// pointOnPlane[k] = -planeD / planeNormal[k]
					NxVec3 pointOnPlane(0,0,0);
					const NxU32 k = pInfo->mPlaneNormal.closestAxis();
					assert(fabsf(pInfo->mPlaneNormal[k]) > 0.001f);
					pointOnPlane[k] = -pInfo->mPlaneD / pInfo->mPlaneNormal[k];

#if NX_SDK_VERSION_NUMBER >= 272
					jt->addLimitPlane(pInfo->mPlaneNormal, pointOnPlane, pInfo->restitution);
#else
					jt->addLimitPlane(pInfo->mPlaneNormal, pointOnPlane);
#endif
				}


  				jt->setName(jdesc->name);
  				v->mInstance = jt;
					if ( callback )
  					callback->NXU_notifyJoint(jt,	v->mUserProperties);
  			}
  			else
  			{
					if ( callback )
						callback->NXU_notifyJointFailed(*jdesc,v->mUserProperties);
					reportWarning("Failed to create joint '%s'", v->name );
  			}
  		}
		}
	}


	count	=	model.mEffectors.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{
    ::NxSpringAndDamperEffectorDesc desc;
		NxSpringAndDamperEffectorDesc *d	=	model.mEffectors[i];
    d->copyTo(desc,cc);
    bool ok = true;
    if ( callback )
    {
      ok = callback->NXU_preNotifySpringAndDamperEffector(desc,d->mUserProperties);
    }
    if ( ok )
    {
  		d->mInstance = scene.createSpringAndDamperEffector(desc);
      if ( d->mInstance )
      {
        if ( callback )
        {
          callback->NXU_notifySpringAndDamperEffector( (NxSpringAndDamperEffector *) d->mInstance, d->mUserProperties);
        }
      }
      else
      {
        if ( callback )
          callback->NXU_notifySpringAndDamperEffectorFailed(desc,d->mUserProperties);
        reportError("Failed to create SpringAndDamper Effector '%s'\r\n", getStr(d->mId) );
      }
    }
	}

// force field scale table
#if NX_SDK_VERSION_NUMBER >= 280
  {
	NxForceFieldVariety maxVar = 0;
	NxForceFieldMaterial maxMat = 0;
	count = model.mForceFieldScaleTable.size();
	for (NxU32 i=0; i<count; i++)
	{
		NxForceFieldScaleTableEntry& entry = *model.mForceFieldScaleTable[i];
		while(entry.var > maxVar)
		{
			maxVar = scene.createForceFieldVariety();
		}
		while(entry.mat > maxMat)
		{
			maxMat = scene.createForceFieldMaterial();
		}
		scene.setForceFieldScale(entry.var, entry.mat, entry.scale);
	}
  }
#endif

// force field linear kernels
#if NX_SDK_VERSION_NUMBER >= 280
  {
	count =	model.mForceFieldLinearKernels.size();
	for	(NxU32 i = 0; i<count; i++)
	{
		::NxForceFieldLinearKernelDesc desc;
		NxForceFieldLinearKernelDesc *d	=	model.mForceFieldLinearKernels[i];
		d->copyTo(desc,cc);

		NxForceFieldLinearKernel *newKernel = scene.createForceFieldLinearKernel(desc);
		d->mInstance = newKernel;
		if(!newKernel)
			reportError("Failed to create ForceField LinearKernel '%s'\r\n", getStr(d->mId));
	}
  }
#endif

// force field groups
#if NX_SDK_VERSION_NUMBER >= 280
  {
	count =	model.mForceFieldShapeGroups.size();
	for	(NxU32 i = 0; i<count; i++)
	{
		::NxForceFieldShapeGroupDesc desc;
		NxForceFieldShapeGroupDesc *d =	model.mForceFieldShapeGroups[i];
		d->copyTo(desc,cc);
		// translate shapes to instance pose
		for(NxU32 j = 0; j < desc.shapes.size(); j++)
		{
			NxMat34& shapePose = desc.shapes[j]->pose;
			shapePose.multiply(pose, shapePose);
		}

		NxForceFieldShapeGroup *newGroup = scene.createForceFieldShapeGroup(desc);
		d->mInstance = newGroup;
		// delete shape descriptors
		for(NxU32 j = 0; j < desc.shapes.size(); j++)
		{
			delete desc.shapes[j];
		}
		desc.shapes.clear();
		if(!newGroup)
			reportError("Failed to create ForceField shape group '%s'\r\n", getStr(d->mId));
	}
  }
#endif

#if NX_SDK_VERSION_NUMBER >= 272
	count	=	model.mForceFields.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{
		::NxForceFieldDesc desc;
			NxForceFieldDesc *d	=	model.mForceFields[i];
		d->copyTo(desc,cc);
	    
		if (desc.actor == 0) 
		{
			desc.pose.multiply(pose, desc.pose);
		}

		bool ok = true;
		if ( callback )
		{
			ok = callback->NXU_preNotifyForceField(desc,d->mUserProperties);
		}
		if ( ok )
		{
			NxForceField *newField = scene.createForceField(desc);
			d->mInstance = newField;
			if ( d->mInstance )
			{
				if ( callback )
				{
				  callback->NXU_notifyForceField( (NxForceField *) d->mInstance, d->mUserProperties);
				}
			}
			else
			{
				if ( callback )
					callback->NXU_notifyForceFieldFailed(desc,d->mUserProperties);
				reportError("Failed to create ForceField Effector '%s'\r\n", getStr(d->mId) );
			}

			//cleanup: delete include group shapes
			for(NxU32 j = 0; j < desc.includeGroupShapes.size(); j++)
			{
				delete desc.includeGroupShapes[j];
			}
			desc.includeGroupShapes.clear();

#if NX_SDK_VERSION_NUMBER < 280
			// Now for the force field shapes
			int nbShapes = d->mShapes.size();
			for (int j = 0; j < nbShapes; j++)
			{
				bool isOk = false;
				NxForceFieldShapeDesc *fsd = d->mShapes[j];
				::NxForceFieldShapeDesc *shapeDesc=0;


				switch (fsd->mType)
				{
					case SC_NxSphereForceFieldShapeDesc:
					{
						::NxSphereForceFieldShapeDesc *d2 = new ::NxSphereForceFieldShapeDesc;
						NxSphereForceFieldShapeDesc *s	=	static_cast<NxSphereForceFieldShapeDesc*>(fsd);
						s->copyTo(*d2,cc);
						shapeDesc = d2;
						isOk = d2->isValid();
					}
					break;
					case SC_NxBoxForceFieldShapeDesc:
					{
						::NxBoxForceFieldShapeDesc *d3 = new ::NxBoxForceFieldShapeDesc;
						NxBoxForceFieldShapeDesc *s	=	static_cast<NxBoxForceFieldShapeDesc*>(fsd);
						s->copyTo(*d3,cc);
						shapeDesc = d3;
						isOk = d3->isValid();
					}
					break;
					case SC_NxCapsuleForceFieldShapeDesc:
					{
						::NxCapsuleForceFieldShapeDesc *d4 = new ::NxCapsuleForceFieldShapeDesc;
						NxCapsuleForceFieldShapeDesc *s	=	static_cast<NxCapsuleForceFieldShapeDesc*>(fsd);
						s->copyTo(*d4,cc);
						shapeDesc = d4;
						isOk = d4->isValid();
					}
					break;
					case SC_NxConvexForceFieldShapeDesc:
					{
						::NxConvexForceFieldShapeDesc *d6 = new ::NxConvexForceFieldShapeDesc;
						NxConvexForceFieldShapeDesc *s = static_cast<NxConvexForceFieldShapeDesc*>(fsd);
						s->copyTo(*d6,cc);
						shapeDesc = d6;
						isOk = d6->isValid();
					}
					break;
					default:
						NX_ASSERT(false);	//Unknown	shape	type
				}

				if ( shapeDesc )
				{
					if ( isOk )
					{
						shapeDesc->userData = (void *)fsd->mUserProperties;

						newField->createShape(*shapeDesc);
					}
					else
					{
						reportWarning("Failed to construct valid force field shape descriptor type(%s) for Field (%s)", EnumToString( fsd->mType) , d->mId );
					}
					delete shapeDesc;
				}
				else
				{
					reportWarning("Unable to construct valid shape descriptor type(%s) for Actor (%s)", EnumToString( fsd->mType) , d->mId );
				}
			}
#endif // FF shape loop
		}
	}
#endif

#if NX_USE_CLOTH_API
	NxU32	clothCount = model.mCloths.size();
	for	(NxU32 i = 0;	i	<	clothCount;	++i)
	{
		NxClothDesc *d	=	model.mCloths[i];
		if (d)
		{
      NxClothMesh *cmesh = cc.getClothMeshFromName(d->mClothMesh);
			if ( cmesh )
			{
  			bool ok	=	true;
  			::NxClothDesc	cdesc;
        d->copyTo(cdesc,cc);

  			if (callback)
  			{
  				ok = callback->NXU_preNotifyCloth(cdesc, d->mUserProperties);
  			}

  			if (ok)
  			{

  				d->mInstance	=	scene.createCloth(cdesc);

  				if ( d->mInstance)
  				{
  					bool okActive = true;
#if NX_SDK_VERSION_NUMBER < 260
            okActive = false;
#else
           if ( d->mActiveState.size() == 0 ) okActive = false; // no active data buffer saved out!
#endif
            if ( d->mAttachments.size() )
            {
            	NxU32 numShapes = d->mAttachments.size();

            	NxShape **shapes = new NxShape*[numShapes];

              //bool shapeOk = true;

            	for (NxU32 i=0; i<numShapes; i++)
            	{

            		NxClothAttachDesc *att = d->mAttachments[i];
                NxActor *a = cc.getActorFromName( att->mAttachActor);
                shapes[i] = 0;

                if ( a )
                {
                  if ( att->mCore ) // if this actor is being attached to the core!
                  {
                    okActive = false;
                    NxCloth *cm = (NxCloth *) d->mInstance;
#if NX_SDK_VERSION_NUMBER >= 262
                    cm->attachToCore(a,10.0f,1.1f);
#endif
                  }
                  else
                  {
                    NxU32 nbShapes = a->getNbShapes();
                    NxShape *const* slist = a->getShapes();
                    if ( att->mAttachShapeIndex >= 0 && att->mAttachShapeIndex < nbShapes )
                    {
                      shapes[i] = slist[att->mAttachShapeIndex];
                    }
                    else
                    {
                      okActive = false;
                    }
                  }
                }
                else
                  okActive = false;
              }

#if NX_SDK_VERSION_NUMBER < 270
							okActive = false;
#endif
  						if ( okActive )
  						{
#if NX_SDK_VERSION_NUMBER >= 270
								NxCloth *cm = (NxCloth *) d->mInstance;
             		cm->setShapePointers(shapes, numShapes );
#endif
  						}
  						else
  						{
    						for (NxU32 i=0; i<numShapes; i++)
    						{
            			NxClothAttachDesc *att = d->mAttachments[i];
    							if ( shapes[i] )
    							{
    								NxU32 flags = 0;
#if NX_SDK_VERSION_NUMBER >= 250
    								if ( att->mTwoWay ) flags|=NX_CLOTH_ATTACHMENT_TWOWAY;
    								if ( att->mTearable ) flags|=NX_CLOTH_ATTACHMENT_TEARABLE;
										NxCloth *cm = (NxCloth *) d->mInstance;
    								cm->attachToShape(shapes[i],flags);
#endif
    							}
    						}
             	}
         		  delete shapes;
            }
#if NX_SDK_VERSION_NUMBER >= 260
  					if ( okActive )
  					{
    					ImportMemoryReadBuffer readBuffer(&d->mActiveState[0]);
							NxCloth *cm = (NxCloth *)d->mInstance;
    					cm->loadStateFromStream(readBuffer);
  					}
#endif

  					if ( callback )
  					{
  					  callback->NXU_notifyCloth((NxCloth *)d->mInstance, d->mUserProperties);
  					}
  				}
  				else
  				{
  					if ( callback )
  						callback->NXU_notifyClothFailed(cdesc, d->mUserProperties );
  					reportWarning("Failed to create cloth");
  				}
  			}
  		}
		}
	}
#endif


#if NX_USE_SOFTBODY_API
	NxU32	softBodyCount = model.mSoftBodies.size();
	for	(NxU32 i = 0;	i	<	softBodyCount;	++i)
	{

		NxSoftBodyDesc *d	=	model.mSoftBodies[i];

		if (d)
		{

      NxSoftBodyMesh *cmesh = cc.getSoftBodyMeshFromName(d->mSoftBodyMesh);

			if ( cmesh )
			{
  			bool ok	=	true;
  			::NxSoftBodyDesc	cdesc;
        d->copyTo(cdesc,cc);

  			if (callback)
  			{
  				ok = callback->NXU_preNotifySoftBody(cdesc, d->mUserProperties);
  			}

  			if (ok)
  			{

				cdesc.isValid();

  				d->mInstance	=	scene.createSoftBody(cdesc);

  				if ( d->mInstance)
  				{
  					bool okActive = true; //gUseSoftBodyActiveState;
            if ( d->mActiveState.size() == 0 ) okActive = false; // no active data buffer saved out!
            if ( d->mAttachments.size() )
            {
            	NxU32 numShapes = d->mAttachments.size();
            	NxShape **shapes = new NxShape*[numShapes];
							NxU32    *flags  = new NxU32[numShapes];

							memset(shapes,0,sizeof(NxShape *)*numShapes);
              // bool shapeOk = true;

            	for (NxU32 i=0; i<numShapes; i++)
            	{

            		NxSoftBodyAttachDesc *att = d->mAttachments[i];
                NxActor *a = cc.getActorFromName( att->mAttachActor);
								flags[i] = att->flags;

                if ( a )
                {
                  NxU32 nbShapes = a->getNbShapes();
                  NxShape *const* slist = a->getShapes();
                  if ( att->mAttachShapeIndex >= 0 && att->mAttachShapeIndex < nbShapes )
                  {
                    shapes[i] = slist[att->mAttachShapeIndex];
                  }
                  else
                  {
                    okActive = false;
                  }
                }
                else
                  okActive = false;
              }
  						if ( okActive )
  						{
								NxSoftBody *cm = (NxSoftBody *) d->mInstance;
             		cm->setShapePointers(shapes, numShapes);
  						}
  						else
  						{
    						for (NxU32 i=0; i<numShapes; i++)
    						{
            			NxSoftBodyAttachDesc *att = d->mAttachments[i];
    							if ( shapes[i] )
    							{
										NxSoftBody *cm = (NxSoftBody *) d->mInstance;
    								cm->attachToShape(shapes[i],att->flags);
    							}
    						}
          		  delete shapes;
             	}
            }
  					if ( okActive )
  					{
    					ImportMemoryReadBuffer readBuffer(&d->mActiveState[0]);
							NxSoftBody *cm = (NxSoftBody *)d->mInstance;
    					cm->loadStateFromStream(readBuffer);
  					}
  					if ( callback )
  					{
  					  callback->NXU_notifySoftBody((NxSoftBody *)d->mInstance, d->mUserProperties);
  					}
  				}
  				else
  				{
  					if ( callback )
  						callback->NXU_notifySoftBodyFailed(cdesc, d->mUserProperties );
  					reportWarning("Failed to create soft body");
  				}
  			}
  		}
		}
	}
#endif



//********************************** do fluids
#if NX_USE_FLUID_API
	count	=	model.mFluids.size();
	for	(NxU32 i = 0;	i	<	count; ++i)
	{

		NxFluidDesc *d	=	model.mFluids[i];
		::NxFluidDesc desc;
		d->copyTo(desc,cc);
#ifdef __PPCGEKKO__
		desc.flags &= ~NX_FF_HARDWARE;
#endif

		for (NxU32 j = 0; j < desc.emitters.size(); j++)
		{
#if NX_SDK_VERSION_NUMBER >= 260
			NxActor *frameActor = cc.getActorFromName(d->mEmitters[j]->mFrameActor);

			if (frameActor)
				desc.emitters[j].frameShape = frameActor->getShapes()[d->mEmitters[j]->mFrameShape];
#else
			desc.emitters[j].frameActor = cc.getActorFromName(d->mEmitters[j]->mFrameActor);
#endif
		}

		bool ok	=	true;

		if (callback)
		{
			ok = callback->NXU_preNotifyFluid(desc, d->mUserProperties);
		}
		if (ok)
		{
			desc.isValid();
			d->mInstance = scene.createFluid(desc);
			if ( d->mInstance)
			{
#if NX_SDK_VERSION_NUMBER >= 280
				if (d->mCurrentParticleLimit == 0xffffffff)
				{
					// To work with old serialization files we check whether the current particle limit
					// parameter could be loaded. If not (the value is still set to default), then we
					// set it equal the max particle limit.
					d->mCurrentParticleLimit = ((NxFluid *)d->mInstance)->getMaxParticles();
				}
   				((NxFluid *)d->mInstance)->setCurrentParticleLimit(d->mCurrentParticleLimit);
#endif

				if ( callback )
				  callback->NXU_notifyFluid((NxFluid *)d->mInstance,	d->mUserProperties);
			}
			else
			{
				if ( callback )
					callback->NXU_notifyFluidFailed(desc,d->mUserProperties);
				reportWarning("Failed to create fluid.");
			}
		}
	}
#endif

  count = model.mCollisionGroups.size();
  for (NxU32 i=0; i<count; i++)
  {
  	NxCollisionGroupDesc &g = *model.mCollisionGroups[i];
		bool enable = false;
		if ( g.mEnable ) enable = true;
  	scene.setGroupCollisionFlag( g.mCollisionGroupA, g.mCollisionGroupB, enable );
  }


  count = model.mActorGroups.size();
  for (NxU32 i=0; i<count; i++)
  {
  	NxActorGroupPair &g = *model.mActorGroups[i];
  	scene.setActorGroupPairFlags( g.group0, g.group1, g.flags );
  }
#if NX_SDK_VERSION_NUMBER >= 272
  count = model.mDominanceGroupPairs.size();
  for (NxU32 i=0; i<count; i++)
  {
	  NxConstraintDominanceDesc &g = *model.mDominanceGroupPairs[i];
	  NxConstraintDominance dom(g.mDominance0, g.mDominance1);
	  scene.setDominanceGroupPair( g.mGroup0, g.mGroup1, dom );
  }
#endif
}


void NxuPhysicsInstantiator::instantiateSceneInstance(NxSceneInstanceDesc *nsi,
																											NxPhysicsSDK	&sdk,	 //	SDK	to load	the	collection into.
																											NXU_userNotify *callback,	 //	notification events	to the application as	the	scene	is loaded.
																											const NxMat34	*defaultSceneOffset,
																											int depth)
{
	NxMat34	mat	=	nsi->mRootNode;

	if (defaultSceneOffset)
	{
		mat.multiply(mat,*defaultSceneOffset);
	}

  bool ok = true;

 	NxU32	index;
 	NxSceneDesc *sdesc	     = CustomCopy::locateSceneDesc(mCollection,nsi->mSceneName,	index);
	NxSceneInstanceDesc *si  = CustomCopy::locateSceneInstanceDesc(mCollection,nsi->mSceneName,index);


  if ( callback && (sdesc || si || depth ) )
  {
  	ok = callback->NXU_preNotifySceneInstance( nsi->mId, nsi->mSceneName, nsi->mUserProperties, mat, depth );
  }

  if ( ok )
  {
  	if (sdesc)
  	{
  		instanceDefaultScene = instantiateScene(nsi->mSceneName,	nsi->mIgnorePlane, sdk,	callback, instanceDefaultScene,	&mat);
  	}
  	else
  	{
  		if ( si )
  		{
  			instantiateSceneInstance(si,sdk,callback,&mat,depth+1);
  		}
  	}

  	if ( depth )
  	{
  		for (NxU32 i=0; i<nsi->mSceneInstances.size(); i++)
  		{
  			NxSceneInstanceDesc *si = nsi->mSceneInstances[i];
  			instantiateSceneInstance(si,sdk,callback,&mat,depth);
  		}
  	}
  }
}

void NxuPhysicsInstantiator::instantiate(NxPhysicsSDK	&sdk,	 //	SDK	to load	the	collection into.
																				NXU_userNotify *callback,	 //	notification events	to the application as	the	scene	is loaded.
																				NxScene	*_defaultScene,
																				const NxMat34	*defaultSceneOffset)
{
	if (mCollection)
	{
    CustomCopy cc(mCollection);

		cc.initCollection();

		instanceTrimeshes(sdk, callback);    //	won't	instantiate	them unless	it is	the	first	time.
		instanceConvexes(sdk,callback);
		instanceSkeletons(sdk,callback);
		instanceHeightFields(sdk,callback);
#if NX_USE_CLOTH_API
		instanceClothMeshes(sdk,callback);
#endif
#if NX_USE_SOFTBODY_API
		instanceSoftBodyMeshes(sdk,callback);
#endif

		if (mCollection->mSceneInstances.size() )
		{
			instanceDefaultScene = _defaultScene;
			NxU32 scount = mCollection->mSceneInstances.size();
			for (NxU32 i=0; i<scount; i++)
			{
				NxSceneInstanceDesc *nsi = mCollection->mSceneInstances[i];
				instantiateSceneInstance(nsi,sdk,callback,defaultSceneOffset,0);
			}
		}
		else
		{
			for	(unsigned	int	i	=	0; i < mCollection->mScenes.size(); i++)
			{
				NxSceneDesc *p	=	mCollection->mScenes[i];
				instantiateScene(p->mId, false,	sdk, callback, _defaultScene, defaultSceneOffset);
			}
		}
	}
}


#if NX_SDK_VERSION_NUMBER >= 260
void NxuPhysicsInstantiator::instanceCompartments(NxPhysicsSDK &sdk,NxSceneDesc &sd, NXU_userNotify *callback, bool existingScene)
{
	if ( sd.mInstance && sd.mCompartments.size() )
	{
		NxScene *scene = (NxScene *) sd.mInstance;

		int nCompartments = scene->getNbCompartments();
		NxCompartment** buffer = new NxCompartment*[nCompartments];
		NxU32 temp = 0;
		NxArray<NxCompartment *>sceneCompartments(nCompartments);
		if (nCompartments > 0)
		{
			scene->getCompartmentArray(buffer, nCompartments, temp);
			for (int i = 0; i < nCompartments; i++)
				sceneCompartments[i] = buffer[i];
		}
		delete buffer;

		CustomCopy cc(mCollection,&sd);
		for (unsigned int i=0; i<sd.mCompartments.size(); i++)
		{
			NxCompartment *nc = 0;

			NxCompartmentDesc *c = sd.mCompartments[i];

#if NX_SDK_VERSION_NUMBER > 262
			if (existingScene) 
			{
				// Instantiating into existing scene; map each compartment to 
				// first matching (by descriptor), create only if no match.
				::NxCompartmentDesc existingCompartment;
				if (nCompartments)
				{
					for (int j = 0; j < nCompartments; j++)
					{
						if (sceneCompartments[j] == 0)
							continue;
						sceneCompartments[j]->saveToDesc(existingCompartment);
						if (sameCompartment(*c, existingCompartment))
						{
							nc = sceneCompartments[j];
							sceneCompartments[j] = 0;
						}
					}
				}
			}
			else // Clean scene instance; create all compartments
#endif
			{
				char tempString[512];

				sprintf(tempString,	"%s+%s+%s", getStr(mCollection->mId), getStr( sd.mId ), getStr(c->mId));			
				nc = (NxCompartment *) findInstance(tempString);
			}

			if ( nc  )
			{
				c->mInstance = nc;
			}
			else
			{
				::NxCompartmentDesc desc;
				c->copyTo(desc,cc);
				

				bool ok = true;

				if (callback)
					ok = callback->NXU_preNotifyCompartment(desc);

				if (ok)
				{
					NxCompartment *nc = scene->createCompartment(desc);

					if ( nc )
					{
						if (callback)
							callback->NXU_notifyCompartment(nc);
						c->mInstance = nc;
						sceneCompartments.push_back(nc);
						nCompartments++;
						//setInstance(tempString,	nc);
					}
					else
					{
						if (callback)
							callback->NXU_notifyCompartmentFailed(desc);
						reportWarning("Failed to create NxCompartment");
					}
				}
			}
		}
	}
}
#endif

NxScene * NxuPhysicsInstantiator::instantiateScene(const	char *sceneName,	// null	means	the	'root' scene.
																							NX_BOOL	ignore_plane,	 //	don't	instantiate	'planes'
																							NxPhysicsSDK &sdk,
																							NXU_userNotify	*callback,
																							NxScene *defaultScene,
																							const NxMat34	*defaultSceneOffset)
{
	bool ret = false;

	NxScene	*newScene	=	0;

	if (mCollection)
	{

		NxU32	index;

		NxSceneDesc *p	=	CustomCopy::locateSceneDesc(mCollection,sceneName,	index);

    CustomCopy cc(mCollection,p);
    cc.initScene();

		if (p)
		{
			unsigned int sno = 0;

			::NxSceneDesc sdesc;
			p->copyTo(sdesc,cc);

			if (defaultScene)
			{
				newScene = defaultScene;
			}
			else
			{

      	if ( sdesc.maxTimestep < 0.00001f )
      	{
      		reportWarning("Scene Descriptor had an invalid maxTimestep of %f; changing it to 0.02", sdesc.maxTimestep);
      		sdesc.maxTimestep = 0.02f;
      	}

      	if ( sdesc.maxIter < 1 || sdesc.maxIter > 1024 )
      	{
      		reportWarning("Scene Descriptor had an invalid maxIter value of %d; changing it to the default of 8", sdesc.maxIter );
      		sdesc.maxIter = 8;
      	}

				bool isValid = sdesc.isValid();
				if ( !isValid )
				{
					reportWarning("Supplied scene descriptor is invalid.  Switching to the default settings.");
					::NxSceneDesc def;
					sdesc = def;
				}


				if (callback)
				{
					sno	=	sdk.getNbScenes();
					newScene = callback->NXU_preNotifyScene(sno, sdesc,	p->mUserProperties);
				}

				if (newScene ==	0)
				{
					NxU32	numParams	=	mCollection->mParameters.size();
					for	(NxU32 i = 0;	i	<	numParams; ++i)
					{
						NxParameterDesc *p	=	mCollection->mParameters[i];
						sdk.setParameter((::NxParameter)p->param, p->value);
					}

					newScene = sdk.createScene(sdesc);

				}
			}

			p->mInstance = newScene;
#if NX_SDK_VERSION_NUMBER >= 260
			instanceCompartments(sdk,*p, callback, defaultScene != 0);
#endif

			if (newScene)
			{
				if ( defaultScene ==	0 && callback )	// only	notify the app if	*we* created the scene, not if he provided it.
				{
 					callback->NXU_notifyScene(sno, newScene, p->mUserProperties);
				}

				NxMat34	xform;
				xform.id();
				if (defaultSceneOffset)
				{
					xform	=	 *defaultSceneOffset;
				}

				newScene->setFilterBool( p->mFilterBool != 0 );
				newScene->setFilterOps( (::NxFilterOp)p->mFilterOp0, (::NxFilterOp)p->mFilterOp1, (::NxFilterOp)p->mFilterOp2 );
				::NxGroupsMask c0;
				::NxGroupsMask c1;
				p->mGroupMask0.copyTo(c0,cc);
				p->mGroupMask1.copyTo(c1,cc);
				newScene->setFilterConstant0(c0);
				newScene->setFilterConstant1(c1);

				instanceModel(*newScene, *p, xform,	false, callback, ignore_plane);

				ret	=	true;

			}
			else
			{
				if ( defaultScene == 0 )
				{
					if ( callback )
						callback->NXU_notifySceneFailed(sno, sdesc, p->mUserProperties );
					reportWarning("Failed to create scene '%s'", p->mId );
				}
			}
		}
	}


	return newScene;
}


NxConvexMesh   * NxuPhysicsInstantiator::instantiateConvexMesh(NxPhysicsSDK &sdk,NxConvexMeshDesc &cdesc,const char *collectionId,NXU_userNotify	*callback,CustomCopy &cc)
{
  NxConvexMesh *ret = 0;

	::NxConvexMeshDesc desc;

  NxConvexMeshDesc *cmd = &cdesc;

	cmd->copyTo(desc,cc);

  bool ok = true;

  if ( callback )
	{
		ok = callback->NXU_preNotifyConvexMesh(desc,cmd->mUserProperties);
	}
	if ( ok )
	{
		if ( cmd->mCookedDataSize == 0 )
		{
			bool status	=	true;
			InitCooking();
			ImportMemoryWriteBuffer	writeBuffer;
			status = CookConvexMesh(desc,	writeBuffer);
			CloseCooking();
			if (status)
			{
				ret = sdk.createConvexMesh(ImportMemoryReadBuffer(writeBuffer.data));
			}
		}
		else
		{
			ImportMemoryReadBuffer readBuffer((const NxU8*)&cmd->mCookedData[0]);
			ret = sdk.createConvexMesh(readBuffer);
	  }
		if (ret)
		{
  		if ( callback )
    		callback->NXU_notifyConvexMesh(ret,cmd->mUserProperties);
  		setInstance(collectionId,	ret);
		}
		else
		{
			if ( callback )
  			callback->NXU_notifyConvexMeshFailed(desc,cmd->mUserProperties);
  		reportWarning("Failed to create convex mesh");
  	}
  }

  return ret;
}



};

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
