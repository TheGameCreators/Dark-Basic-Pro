#include "stdafx.h"


DECLARE_PLUGIN(Collider_Box);

const int Collider_Box::classID='CBOX';

Collider_Box::Collider_Box(int id) : Collider(id)
{
}

void Collider_Box::updateInit()
{
	invM_Upd=m;
	invM_Upd.Inverse3x3This();
}

#ifdef USINGMODULE_P
void Collider_Box::onUpdateParticles()
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
						do
						{
							particle = alive->getCurLink()->data;
							Vector3 pos = particle->pos-m.pos();

							//Do bounding radius check
							if(pos.MagSqr()<radiusSq)
							{
								Vector3 lastPos = particle->lastPos-m.pos();
								invM_Upd.rotateV3(&pos);
								invM_Upd.rotateV3(&lastPos);
								
								Vector3 hitPoint,norm,dir;
								norm.Set(0,1,0);
								if(HitBoundingBoxExterior(lastPos,pos,&hitPoint,&norm))
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
											m.rotateAndTransV3(&hitPoint);
											m.rotateV3(&norm);
															
											//Work out velocities here to prevent energy loss
											Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
											Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
											particle->pos = hitPoint;
											particle->lastPos=particle->pos-vpar*ei->smoothness+vperp*ei->bounce;								
											//Extract both lastPos and pos from box
											Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
											particle->lastPos+=tmp;
											particle->pos+=tmp;
										}
										if(ei->useColor) particle->color=ei->color;
									}
								}
							}
						}while(alive->gotoNextItem());
					}
					else
					{
						do
						{
							particle = alive->getCurLink()->data;
							Vector3 pos = particle->pos-m.pos();

							//Do bounding radius check
							if(pos.MagSqr()<radiusSq)
							{
								Vector3 lastPos = particle->lastPos-m.pos();
								invM_Upd.rotateV3(&pos);
								invM_Upd.rotateV3(&lastPos);
								
								Vector3 hitPoint,norm,dir;
								norm.Set(0,1,0);
								if(HitBoundingBoxInterior(lastPos,pos,&hitPoint,&norm))
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
											m.rotateAndTransV3(&hitPoint);
											m.rotateV3(&norm);
															
											//Work out velocities here to prevent energy loss
											Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
											Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
											particle->pos = hitPoint;
											particle->lastPos=particle->pos-vpar*ei->smoothness+vperp*ei->bounce;								
											//Extract both lastPos and pos from box
											Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
											particle->lastPos+=tmp;
											particle->pos+=tmp;
										}
										if(ei->useColor) particle->color=ei->color;
									}
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
void Collider_Box::onUpdateCloth()
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
					//Solid Box
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 pos = particle->pos-m.pos();

						//Do bounding radius check
						if(pos.MagSqr()<radiusSq)
						{
							Vector3 lastPos = particle->lastPos-m.pos();
							invM_Upd.rotateV3(&pos);
							invM_Upd.rotateV3(&lastPos);
							
							Vector3 hitPoint,norm,dir;
							norm.Set(0,1,0);
							if(HitBoundingBoxExterior(lastPos,pos,&hitPoint,&norm))
							{
								//ci->ticksSinceLastHit=0;
								//ci->numCollisions++;
								//particle->flags|=PARTICLEDATA_COLLIDED;
								//particle->colliderID=id;

								if(!ci->noResolve)
								{
									m.rotateAndTransV3(&hitPoint);
									m.rotateV3(&norm);
													
									//Work out velocities here to prevent energy loss
									Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = hitPoint;
									particle->lastPos=particle->pos-vpar*ci->smoothness+vperp*ci->bounce;								
									//Extract both lastPos and pos from box
									Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
									particle->lastPos+=tmp;
									particle->pos+=tmp;
								}
							}
						}
					}
				}
				else
				{
					//Box cavity
					for(int i=0;i<numParticles;i++,particle++)
					{						
						Vector3 pos = particle->pos-m.pos();
						if(pos.MagSqr()>invertedRadiusSq)
						{
							Vector3 lastPos = particle->lastPos-m.pos();
							invM_Upd.rotateV3(&pos);
							invM_Upd.rotateV3(&lastPos);
							
							Vector3 hitPoint,norm,dir;
							norm.Set(0,1,0);
							if(HitBoundingBoxInterior(lastPos,pos,&hitPoint,&norm))
							{

							//ci->ticksSinceLastHit=0;
							//ci->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

								if(!ci->noResolve)
								{
									m.rotateAndTransV3(&hitPoint);
									m.rotateV3(&norm);
													
									//Work out velocities here to prevent energy loss
									Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = hitPoint;
									particle->lastPos=particle->pos-vpar*ci->smoothness+vperp*ci->bounce;
									//Extract both lastPos and pos from box
									Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
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
void Collider_Box::onUpdateRagdoll()
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
					//Solid Box
					for(int i=0;i<numParticles;i++,particle++)
					{
						Vector3 pos = particle->pos-m.pos();

						//Do bounding radius check
						if(pos.MagSqr()<radiusSq)
						{
							Vector3 lastPos = particle->lastPos-m.pos();
							invM_Upd.rotateV3(&pos);
							invM_Upd.rotateV3(&lastPos);
							
							Vector3 hitPoint,norm,dir;
							norm.Set(0,1,0);
							if(HitBoundingBoxExterior(lastPos,pos,&hitPoint,&norm))
							{
								//ri->ticksSinceLastHit=0;
								//ri->numCollisions++;
								//particle->flags|=PARTICLEDATA_COLLIDED;
								//particle->colliderID=id;

								if(!ri->noResolve)
								{
									m.rotateAndTransV3(&hitPoint);
									m.rotateV3(&norm);
													
									//Work out velocities here to prevent energy loss
									Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = hitPoint;
									particle->lastPos=particle->pos-vpar*ri->smoothness+vperp*ri->bounce;								
									//Extract both lastPos and pos from box
									Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
									particle->lastPos+=tmp;
									particle->pos+=tmp;
								}
							}
						}
					}
				}
				else
				{
					//Box cavity
					for(int i=0;i<numParticles;i++,particle++)
					{						
						Vector3 pos = particle->pos-m.pos();
						if(pos.MagSqr()>invertedRadiusSq)
						{
							Vector3 lastPos = particle->lastPos-m.pos();
							invM_Upd.rotateV3(&pos);
							invM_Upd.rotateV3(&lastPos);
							
							Vector3 hitPoint,norm,dir;
							norm.Set(0,1,0);
							if(HitBoundingBoxInterior(lastPos,pos,&hitPoint,&norm))
							{

							//ri->ticksSinceLastHit=0;
							//ri->numCollisions++;
							//particle->flags|=PARTICLEDATA_COLLIDED;
							//particle->colliderID=id;

								if(!ri->noResolve)
								{
									m.rotateAndTransV3(&hitPoint);
									m.rotateV3(&norm);
													
									//Work out velocities here to prevent energy loss
									Vector3 vperp= norm * norm.Dot(particle->pos-particle->lastPos);
									Vector3 vpar = ((particle->pos-particle->lastPos)-vperp);
									particle->pos = hitPoint;
									particle->lastPos=particle->pos-vpar*ri->smoothness+vperp*ri->bounce;
									//Extract both lastPos and pos from box
									Vector3 tmp = norm*((particle->pos-particle->lastPos).Dot(norm)+0.00001f);
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

ePhysError Collider_Box::setBox(float sx, float sy, float sz)
{
	if(sx<=0.0f || sy<=0.0f || sz<=0.0f) return PERR_BAD_PARAMETER;
	size.x=sx;
	size.y=sy;
	size.z=sz;
	hsize.x=sx*0.5f;
	hsize.y=sy*0.5f;
	hsize.z=sz*0.5f;
	radiusSq = hsize.MagSqr();

	//Use minimum dimension as bounding radius for inverted box
	if(sx<sy && sx<sz) invertedRadiusSq = sx*0.5f;
	else if(sy<sz && sy<sz) invertedRadiusSq = sy*0.5f;
	else invertedRadiusSq = sz*0.5f;
	
	invertedRadiusSq *= invertedRadiusSq;

	return PERR_AOK;
}


bool Collider_Box::HitBoundingBoxExterior(const Vector3& origin, const Vector3& dest, Vector3 * coord, Vector3 * norm)
{
	Vector3 dir = dest-origin;

	int boxSide[3];
	float colPlane[3];
	float pDist[3];
	register int i;

	for(i=0;i<3;i++)
	{
		if(dir.c[i]>0)
		{
			colPlane[i]=-hsize.c[i];
			boxSide[i]=-1;
		}
		else if(dir.c[i]<0)
		{				
			colPlane[i]=hsize.c[i];
			boxSide[i]=1;
		}
		else
		{
			boxSide[i]=0;
		}
	}

	for(i=0;i<3;i++)
	{
		if(boxSide[i]!=0)
		{
			pDist[i]=(colPlane[i]-origin.c[i]) / dir.c[i];
		}
		else pDist[i]=-1e-30f;
	}

	int hitPlane = 0;
	for(i=1;i<3;i++)
	{
		if (pDist[hitPlane] < pDist[i]) hitPlane = i;
	}

	if(pDist[hitPlane]<0.0f)
	{
		//Origin is inside the hit plane, so return if origin is not inside the box
		if(origin.x<=-hsize.x || origin.x>=hsize.x) return false;
		if(origin.y<=-hsize.y || origin.y>=hsize.y) return false;
		if(origin.z<=-hsize.z || origin.z>=hsize.z) return false;
	}

	//Following statement should return if all elements of dir are zero
	if(boxSide[hitPlane]==0) return false;

	if(dir.c[hitPlane]!=0)
	{
		//Return if the ray is too short to intersect with hit plane
		float farDist=(colPlane[hitPlane]-dest.c[hitPlane]) / dir.c[hitPlane];
		if(farDist>0 && pDist[hitPlane]>0) return false;
	}

	for(i=0;i<3;i++)
	{
		if(hitPlane!=i) 
		{
			coord->c[i] = origin.c[i]+pDist[hitPlane]*dir.c[i];
			if (coord->c[i] < -hsize.c[i] || coord->c[i] > hsize.c[i])	return false;
			norm->c[i]=0.0f;
		} 
		else
		{
			coord->c[i] = colPlane[i];
			norm->c[i] = float(boxSide[hitPlane]);
		}		
	}
	
	return true;
}

bool Collider_Box::HitBoundingBoxInterior(const Vector3& origin, const Vector3& dest, Vector3 * coord, Vector3 * norm)
{
	Vector3 dir = dest-origin;

	int boxSide[3];
	float colPlane[3];
	float pDist[3];
	register int i;

	for(i=0;i<3;i++)
	{
		if(dir.c[i]>0)
		{
			colPlane[i]=hsize.c[i];
			boxSide[i]=-1;
		}
		else if(dir.c[i]<0)
		{				
			colPlane[i]=-hsize.c[i];
			boxSide[i]=1;
		}
		else
		{
			boxSide[i]=0;
		}
	}

	for(i=0;i<3;i++)
	{
		if(boxSide[i]!=0)
		{
			pDist[i]=(colPlane[i]-origin.c[i]) / dir.c[i];
		}
		else pDist[i]=1e30f;
	}

	int hitPlane = 0;
	for(i=1;i<3;i++)
	{
		if (pDist[hitPlane] > pDist[i]) hitPlane = i;
	}

	if(dir.c[hitPlane]!=0)
	{
		//Return if the ray is too short to intersect with hit plane
		float farDist=(colPlane[hitPlane]-dest.c[hitPlane]) / dir.c[hitPlane];
		if(farDist>0 && pDist[hitPlane]>0) return false;
	}

	//Following statement should return if all elements of dir are zero
	if(boxSide[hitPlane]==0) return false;

	for(i=0;i<3;i++)
	{
		if(hitPlane!=i) 
		{
			coord->c[i] = origin.c[i]+pDist[hitPlane]*dir.c[i];
			if (coord->c[i] < -hsize.c[i] || coord->c[i] > hsize.c[i])	return false;
			norm->c[i]=0.0f;
		} 
		else
		{
			coord->c[i] = colPlane[i];
			norm->c[i] = float(boxSide[hitPlane]);
		}		
	}
	
	return true;
}


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeCollisionBox(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addCollider(id,COLLIDER_PLUGIN(Collider_Box)),PCMD_MAKE_COLLISION_BOX);
}


DLLEXPORT void setCollisionBox(int id, float sx, float sy, float sz)
{
	ColliderPtr tmp;
	RETURN_ON_PERR(physics->getCollider(id,tmp),PCMD_SET_COLLISION_BOX);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Collider::classID,Collider_Box::classID),PCMD_SET_COLLISION_BOX);

	Collider_Box * box = reinterpret_cast<Collider_Box *>(&(*tmp));
	RETURN_ON_PERR(box->setBox(sx,sy,sz),PCMD_SET_COLLISION_BOX);
}

