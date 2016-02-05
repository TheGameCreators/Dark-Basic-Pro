#include "stdafx.h"


DECLARE_PLUGIN(Collider_Disc);

const int Collider_Disc::classID='DISC';

Collider_Disc::Collider_Disc(int id) : Collider(id),radiusSq(1.0f)
{
}

void Collider_Disc::updateInit()
{	
	worldPlane_Upd.Set(&m.unitY(),m.unitY().Dot(m.pos()),true);
}

#ifdef USINGMODULE_P
void Collider_Disc::onUpdateParticles()
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
					
					do
					{
						particle = alive->getCurLink()->data;
						Vector3 pos = particle->pos-m.pos();

						//-ve distances are inside the plane
						float distA,distB;						
						
						//Is the line segment crossing the plane
						if(worldPlane_Upd.isPointInside(&particle->pos,&distA)!=worldPlane_Upd.isPointInside(&particle->lastPos,&distB))
						{
							if(distA<0) distA=-distA;
							if(distB<0) distB=-distB;
							if((distA+distB)!=0)
							{
								float t=distB/(distA+distB);
								Vector3 iPos=particle->lastPos+((particle->pos-particle->lastPos)*t);
								iPos-=m.pos();
								if(iPos.MagSqr()<radiusSq)
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
											iPos+=m.pos();
											//Work out velocities here to prevent energy loss
											Vector3 vperp= m.unitY() * m.unitY().Dot(particle->pos-particle->lastPos);
											Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
											particle->pos = iPos;
											particle->lastPos=particle->pos-vpar*ei->smoothness+vperp*ei->bounce;								
											//Extract both lastPos and pos from shape
											Vector3 tmp = m.unitY()*((particle->pos-particle->lastPos).Dot(m.unitY())+0.0000001f);
											particle->lastPos+=tmp;
											particle->pos+=tmp;
										}
										if(ei->useColor) particle->color=ei->color;
									}
								}
							}
						}
					}while(alive->gotoNextItem());					
				}
			}
			emitterInfo.gotoNextItem();
		}
		while(emitterList.gotoNextItem());
	}
}
#endif

#ifdef USINGMODULE_C
void Collider_Disc::onUpdateCloth()
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
				
				for(int i=0;i<numParticles;i++,particle++)
				{
					Vector3 pos = particle->pos-m.pos();

					//-ve distances are inside the plane
					float distA,distB;						
					
					//Is the line segment crossing the plane
					if(worldPlane_Upd.isPointInside(&particle->pos,&distA)!=worldPlane_Upd.isPointInside(&particle->lastPos,&distB))
					{
						if(distA<0) distA=-distA;
						if(distB<0) distB=-distB;
						if((distA+distB)!=0)
						{
							float t=distB/(distA+distB);
							Vector3 iPos=particle->lastPos+((particle->pos-particle->lastPos)*t);
							iPos-=m.pos();
							if(iPos.MagSqr()<radiusSq)
							{
								//ci->ticksSinceLastHit=0;
								//ci->numCollisions++;
								//particle->flags|=PARTICLEDATA_COLLIDED;
								//particle->colliderID=id;

								if(!ci->noResolve)
								{
									iPos+=m.pos();
									//Work out velocities here to prevent energy loss
									Vector3 vperp= m.unitY() * m.unitY().Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = iPos;
									particle->lastPos=particle->pos-vpar*ci->smoothness+vperp*ci->bounce;								
									//Extract both lastPos and pos from shape
									Vector3 tmp = m.unitY()*((particle->pos-particle->lastPos).Dot(m.unitY())+0.0000001f);

									Vector3 dperp= m.unitY() * m.unitY().Dot(particle->delta);
									if(dperp.Dot(m.unitY())<0)
									{
										particle->delta-=dperp;
										tmp-=dperp;	
									}

									particle->lastPos+=tmp;
									particle->pos+=tmp;

								}
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
void Collider_Disc::onUpdateRagdoll()
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
				
				for(int i=0;i<numParticles;i++,particle++)
				{
					Vector3 pos = particle->pos-m.pos();

					//-ve distances are inside the plane
					float distA,distB;						
					
					//Is the line segment crossing the plane
					if(worldPlane_Upd.isPointInside(&particle->pos,&distA)!=worldPlane_Upd.isPointInside(&particle->lastPos,&distB))
					{
						if(distA<0) distA=-distA;
						if(distB<0) distB=-distB;
						if((distA+distB)!=0)
						{
							float t=distB/(distA+distB);
							Vector3 iPos=particle->lastPos+((particle->pos-particle->lastPos)*t);
							iPos-=m.pos();
							if(iPos.MagSqr()<radiusSq)
							{
								//ri->ticksSinceLastHit=0;
								//ri->numCollisions++;
								//particle->flags|=PARTICLEDATA_COLLIDED;
								//particle->colliderID=id;

								if(!ri->noResolve)
								{
									iPos+=m.pos();
									//Work out velocities here to prevent energy loss
									Vector3 vperp= m.unitY() * m.unitY().Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = iPos;
									particle->lastPos=particle->pos-vpar*ri->smoothness+vperp*ri->bounce;								
									//Extract both lastPos and pos from shape
									Vector3 tmp = m.unitY()*((particle->pos-particle->lastPos).Dot(m.unitY())+0.0000001f);

									Vector3 dperp= m.unitY() * m.unitY().Dot(particle->delta);
									if(dperp.Dot(m.unitY())<0)
									{
										particle->delta-=dperp;
										tmp-=dperp;	
									}

									particle->lastPos+=tmp;
									particle->pos+=tmp;

								}
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

ePhysError Collider_Disc::setDisc(float r)
{
	if(r<0.0f) return PERR_BAD_PARAMETER;
	radiusSq=r*r;
	return PERR_AOK;
}


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeCollisionDisc(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addCollider(id,COLLIDER_PLUGIN(Collider_Disc)),PCMD_MAKE_COLLISION_DISC);
}


DLLEXPORT void setCollisionDisc(int id, float r)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(id,tmp),PCMD_SET_COLLISION_DISC);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Collider::classID,Collider_Disc::classID),PCMD_SET_COLLISION_DISC);

	Collider_Disc * dsc = reinterpret_cast<Collider_Disc *>(&(*tmp));
	RETURN_ON_PERR(dsc->setDisc(r),PCMD_SET_COLLISION_DISC);
}
