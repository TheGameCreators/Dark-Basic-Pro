#include "stdafx.h"


DECLARE_PLUGIN(Effector_WrapRound);

const int Effector_WrapRound::classID='WRAP';

Effector_WrapRound::Effector_WrapRound(int id)
:Effector(id),region(1.0f,1.0f,1.0f)
{
}

void Effector_WrapRound::setRegion(float sx, float sy, float sz)
{
	region.x=sx;
	region.y=sy;
	region.z=sz;
}

void Effector_WrapRound::updateInit()
{
	minb_Upd = m.pos()-region*0.5f;
	maxb_Upd = m.pos()+region*0.5f;
}

#ifdef USINGMODULE_P
void Effector_WrapRound::onUpdateParticles()
{
	if(emitterList.gotoFirstItem())
	{
		emitterInfo.gotoFirstItem();

		ParticleList * alive;
		ParticleList * dead;
		
		do
		{
			EffectorTargetInfo * ei = emitterInfo.getCurItem();

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
						if(particle->pos.x<minb_Upd.x)
						{
							particle->pos.x+=region.x;
							particle->lastPos.x+=region.x;
						}
						else if(particle->pos.x>maxb_Upd.x)
						{
							particle->pos.x-=region.x;
							particle->lastPos.x-=region.x;
						}

						if(particle->pos.y<minb_Upd.y)
						{
							particle->pos.y+=region.y;
							particle->lastPos.y+=region.y;
						}
						else if(particle->pos.y>maxb_Upd.y)
						{
							particle->pos.y-=region.y;
							particle->lastPos.y-=region.y;
						}

						if(particle->pos.z<minb_Upd.z)
						{
							particle->pos.z+=region.z;
							particle->lastPos.z+=region.z;
						}
						else if(particle->pos.z>maxb_Upd.z)
						{
							particle->pos.z-=region.z;
							particle->lastPos.z-=region.z;
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

//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------


DLLEXPORT void makeWrapRoundEffector(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addEffector(id,EFFECTOR_PLUGIN(Effector_WrapRound)),PCMD_MAKE_WRAPROUND_EFFECTOR);
}


DLLEXPORT void setWrapRoundEffector(int id, float sx, float sy, float sz)
{
	EffectorPtr tmp;
	RETURN_ON_PERR(physics->getEffector(id,tmp),PCMD_SET_WRAPROUND_EFFECTOR);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Effector::classID,Effector_WrapRound::classID),PCMD_SET_WRAPROUND_EFFECTOR);

	Effector_WrapRound * wrp = reinterpret_cast<Effector_WrapRound *>(&(*tmp));
	wrp->setRegion(sx,sy,sz);
}
