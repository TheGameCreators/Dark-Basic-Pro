#include "stdafx.h"


DECLARE_PLUGIN(Collider_Plane);

const int Collider_Plane::classID='PLAN';

Collider_Plane::Collider_Plane(int id) : Collider(id)
{
	plane.Set(0,0,1.0f,0);
}

void Collider_Plane::updateInit()
{
	worldPlane_Upd = plane;
	worldPlane_Upd.rotateAndTrans(m);
}

#ifdef USINGMODULE_P
void Collider_Plane::onUpdateParticles()
{
	if(emitterList.gotoFirstItem())
	{
		emitterInfo.gotoFirstItem();

		ParticleList * alive;
		ParticleList * dead;
		
		do
		{
			ColliderTargetInfo * ei = emitterInfo.getCurItem();
			ei->ticksSinceLastHit++;

			if(!emitterList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!emitterList.getCurItem_WARNING()->isFrozen() && ei->enabled)
			{
				emitterList.getCurItem_WARNING()->getParticleLists(&alive,&dead);

				if(alive->gotoFirstItem())
				{
					Particle * particle;
					if(!ei->invert)
					{
						//Normal points out of solid surface
						do
						{
							particle = alive->getCurLink()->data;
							float dist;		//-ve distances are inside the plane
							if(worldPlane_Upd.isPointInside(&particle->pos,&dist))
							{
								ei->ticksSinceLastHit=0;
								ei->numCollisions++;
								if(!(particle->flags&PARTICLEDATA_COLLIDED)) particle->baseFrameCount=particle->frameCount;
								particle->flags|=PARTICLEDATA_COLLIDED;								
								particle->colliderID=objId;

								if(ei->killer) particle->energy=0;
								else
								{
									if(!ei->noResolve)
									{
										Vector3 tmp = worldPlane_Upd.n*dist;
										particle->pos -= tmp;
										particle->lastPos -= tmp;
										Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
										Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
										particle->lastPos=particle->pos-vpar*ei->smoothness+vperp*ei->bounce;
									}
									if(ei->useColor) particle->color=ei->color;
								}
							}
						}while(alive->gotoNextItem());					
					}
					else
					{
						//Normal points into solid surface
						do
						{
							particle = alive->getCurLink()->data;
							float dist;		//-ve distances are inside the plane
							if(!worldPlane_Upd.isPointInside(&particle->pos,&dist))
							{
								ei->ticksSinceLastHit=0;
								ei->numCollisions++;
								if(!(particle->flags&PARTICLEDATA_COLLIDED)) particle->baseFrameCount=particle->frameCount;
								particle->flags|=PARTICLEDATA_COLLIDED;								
								particle->colliderID=objId;

								if(ei->killer) particle->energy=0;
								else
								{
									if(!ei->noResolve)
									{
										Vector3 tmp = worldPlane_Upd.n*dist;
										particle->pos -= tmp;
										particle->lastPos -= tmp;
										Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
										Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
										particle->lastPos=particle->pos-vpar*ei->smoothness+vperp*ei->bounce;
									}
									if(ei->useColor) particle->color=ei->color;
								}
							}
						}while(alive->gotoNextItem());					
					}
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_C
void Collider_Plane::onUpdateCloth()
{
	if(clothList.gotoFirstItem())
	{
		clothInfo.gotoFirstItem();
		
		do
		{
			ColliderTargetInfo * ci = clothInfo.getCurItem();
			//ci->ticksSinceLastHit++;

			if(!clothList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!clothList.getCurItem_WARNING()->isFrozen() && ci->enabled)
			{
				ClothParticle * particle;
				clothList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = clothList.getCurItem_WARNING()->getNumParticles();

				if(!ci->invert)
				{
					//Normal points out of solid surface
					for(int i=0;i<numParticles;i++,particle++)
					{
						float dist;		//-ve distances are inside the plane
						if(worldPlane_Upd.isPointInside(&particle->pos,&dist))
						{
							//ci->ticksSinceLastHit=0;
							//ci->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

							if(!ci->noResolve)
							{
								Vector3 tmp = worldPlane_Upd.n*dist;
								particle->pos -= tmp;
								particle->lastPos -= tmp;
								Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
								Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
								particle->lastPos=particle->pos-vpar*ci->smoothness+vperp*ci->bounce;
							}
						}
					}
				}
				else
				{
					//Normal points into solid surface
					for(int i=0;i<numParticles;i++,particle++)
					{						
						float dist;		//-ve distances are inside the plane
						if(!worldPlane_Upd.isPointInside(&particle->pos,&dist))
						{
							//ci->ticksSinceLastHit=0;
							//ci->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

							if(!ci->noResolve)
							{
								Vector3 tmp = worldPlane_Upd.n*dist;
								particle->pos -= tmp;
								particle->lastPos -= tmp;
								Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
								Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
								particle->lastPos=particle->pos-vpar*ci->smoothness+vperp*ci->bounce;
							}
						}
					}
				}
				
			}
			clothInfo.gotoNextItem();
		}
		while(clothList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_R
void Collider_Plane::onUpdateRagdoll()
{
	if(ragdollList.gotoFirstItem())
	{
		ragdollInfo.gotoFirstItem();
		
		do
		{
			ColliderTargetInfo * ri = ragdollInfo.getCurItem();
			//ri->ticksSinceLastHit++;

			if(!ragdollList.getCurItem_WARNING()->isValid())
			{
				objectHasGarbage();				
			}
			else if(!ragdollList.getCurItem_WARNING()->isFrozen() && ri->enabled)
			{
				RagdollParticle * particle;
				ragdollList.getCurItem_WARNING()->getParticles(&particle);
				int numParticles = ragdollList.getCurItem_WARNING()->getNumParticles();

				if(!ri->invert)
				{
					//Normal points out of solid surface
					for(int i=0;i<numParticles;i++,particle++)
					{
						float dist;		//-ve distances are inside the plane
						if(worldPlane_Upd.isPointInside(&particle->pos,&dist))
						{
							//ri->ticksSinceLastHit=0;
							//ri->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

							if(!ri->noResolve)
							{
								Vector3 tmp = worldPlane_Upd.n*dist;
								particle->pos -= tmp;
								particle->lastPos -= tmp;
								Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
								Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
								particle->lastPos=particle->pos-vpar*ri->smoothness+vperp*ri->bounce;
							}
						}
					}
				}
				else
				{
					//Normal points into solid surface
					for(int i=0;i<numParticles;i++,particle++)
					{						
						float dist;		//-ve distances are inside the plane
						if(!worldPlane_Upd.isPointInside(&particle->pos,&dist))
						{
							//ri->ticksSinceLastHit=0;
							//ri->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

							if(!ri->noResolve)
							{
								Vector3 tmp = worldPlane_Upd.n*dist;
								particle->pos -= tmp;
								particle->lastPos -= tmp;
								Vector3 vperp= worldPlane_Upd.n * worldPlane_Upd.n.Dot(particle->pos-particle->lastPos);
								Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
								particle->lastPos=particle->pos-vpar*ri->smoothness+vperp*ri->bounce;
							}
						}
					}
				}
				
			}
			ragdollInfo.gotoNextItem();
		}
		while(ragdollList.gotoNextItem());
	}
}
#endif

/*
ePhysError Collider_Plane::setPlane(float nx, float ny, float nz, float dist)
{
	if(nx==0.0f && ny==0.0f && nz==0.0f) return PERR_NULL_VECTOR;
	plane.Set(nx,ny,nz,dist);
	return PERR_AOK;
}
*/


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeCollisionPlane(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addCollider(id,COLLIDER_PLUGIN(Collider_Plane)),PCMD_MAKE_COLLISION_PLANE);	
}

/*
DLLEXPORT void setCollisionPlane(int id, float nx, float ny, float nz, float dist)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(id,tmp),PCMD_SET_COLLISION_PLANE);
	RETURN_ON_PERR(tmp->checkType(Collider::classID,Collider_Plane::classID),PCMD_SET_COLLISION_PLANE);

	Collider_Plane * pln = reinterpret_cast<Collider_Plane *>(&(*tmp));
	RETURN_ON_PERR(pln->setPlane(nx,ny,nz,dist),PCMD_SET_COLLISION_PLANE);
}
*/

