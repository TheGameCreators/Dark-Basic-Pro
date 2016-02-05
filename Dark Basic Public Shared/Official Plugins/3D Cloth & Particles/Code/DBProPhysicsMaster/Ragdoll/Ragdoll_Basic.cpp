#include "stdafx.h"


DECLARE_PLUGIN(Ragdoll_Basic);

Ragdoll_Basic::~Ragdoll_Basic()
{
	SAFEDELETE_ARRAY(particle);
	SAFEDELETE_ARRAY(link);
	SAFEDELETE_ARRAY(rotConstr);
}


void Ragdoll_Basic::onUpdate()
{	
	if(frozen) return;
	int i;
	
	Vector3 tempPos;

	float timeIntSqr = PhysicsManager::iterationInterval;
	timeIntSqr*=timeIntSqr;		

	RagdollParticle * curPar = &particle[0];


	Matrix mat;
	//Integrate particle positions
	for(i=0;i<numParticles;i++,curPar++)
	{
		if(!curPar->fixed)
		{
			//Rewritten like this to minimize floating point inaccuracies
			tempPos = curPar->pos*2.0f - curPar->lastPos - (curPar->pos - curPar->lastPos)*damping + (curPar->force*particleInvMass + gravity)*timeIntSqr;
			curPar->lastPos=curPar->pos;
			curPar->pos=tempPos;
		}
		else if(curPar->objectID!=-1) //Is particle fixed to an object?
		{						
			PhysicsManager::DLL_UpdatePositionData(curPar->DBProPosition); //Causes matrix to be updated if necessary
			curPar->lastPos=curPar->pos;	//Need this here for segment based forces
			mat.convertD3DMatrix(&(curPar->DBProPosition->matWorld));
			mat.rotateAndTransV3(curPar->fixedPos,&curPar->pos);
		}
		curPar->force.x=curPar->force.y=curPar->force.z=0;
		curPar->delta.x=curPar->delta.y=curPar->delta.z=0;
	}

	//Apply link constraints
	RagdollLink * curLink = &link[0];
	for(i=0;i<numLinks;i++,curLink++)
	{
		RagdollParticle& p1=*curLink->p1;
		RagdollParticle& p2=*curLink->p2;
		Vector3& x1 = p1.pos;
		Vector3& x2 = p2.pos;
		Vector3 delta = x2-x1;
		float deltaMagSq = delta.MagSqr();
		
		if((curLink->type==RAGLINK_SOLID) ||
			((curLink->type==RAGLINK_MINIMUM || curLink->type==RAGLINK_MINMAX) && deltaMagSq<curLink->distSq) ||
			(curLink->type==RAGLINK_MAXIMUM && deltaMagSq>curLink->distSq))
		{
			delta*=curLink->distSq/(deltaMagSq+curLink->distSq)-0.5f;
		}
		else if(curLink->type==RAGLINK_MINMAX && deltaMagSq>curLink->maxSq)
		{
			delta*=curLink->maxSq/(deltaMagSq+curLink->maxSq)-0.5f;
		}
		else if(curLink->type==RAGLINK_DISTDIFF)
		{
			RagdollParticle& p3=*curLink->p3;
			Vector3 delta2 = p3.pos-x1;
			float deltaMagSq2 = delta2.MagSqr();

			deltaMagSq2+=curLink->distSq;
			if(deltaMagSq<deltaMagSq2)
			{				
				delta*=deltaMagSq2/(deltaMagSq+deltaMagSq2)-0.5f;
			}
			else continue;
		}
		else continue;

		//Note: signs are reversed from (incorrect) Verlet paper
		if(!p1.fixed) 
		{
			x1 -= delta;
			p1.delta -= delta;
		}
		if(!p2.fixed)
		{
			x2 += delta;
			p2.delta += delta;
		}
	}

	for(i=0;i<numRotConstraints;i++)
	{
		rotConstr[i].Evaluate();
	}

	//Update attached objects
	for(i=0;i<11;i++)
	{
		if(bodyPart[i].active && bodyPart[i].objectId!=-1)
		{		
			Matrix partMat;
			Vector3 v1 = particle[partLinks[i].p1].pos - particle[partLinks[i].pBase].pos;
			Vector3 v2 = particle[partLinks[i].p2].pos - particle[partLinks[i].pBase].pos;
			Vector3 norm = v1*v2;
			norm.Normalise();

			Vector3 avg;

			if(partLinks[i].useTwoBases)
			{
				Vector3 v3 = particle[partLinks[i].p3].pos - particle[partLinks[i].pBase2].pos;
				Vector3 v4 = particle[partLinks[i].p4].pos - particle[partLinks[i].pBase2].pos;
				Vector3 norm2 = v3*v4;
				norm2.Normalise();
				norm+=norm2;
				norm.Normalise();
				avg =(v1+v2+v3+v4)*0.25f;
				if(partLinks[i].flip)
				{
					Vector3 otherPos = particle[partLinks[i].p1].pos +
										particle[partLinks[i].p2].pos +
										particle[partLinks[i].p3].pos +
										particle[partLinks[i].p4].pos;
					partMat.setPos(otherPos*0.25f);
					avg=-avg;
				}
				else
				{
					partMat.setPos((particle[partLinks[i].pBase].pos + particle[partLinks[i].pBase2].pos)*0.5f);
				}
			}
			else
			{
				avg=(v1+v2)*0.5f;
				if(partLinks[i].flip)
				{
					Vector3 otherPos = particle[partLinks[i].p1].pos +
										particle[partLinks[i].p2].pos;

					partMat.setPos(otherPos*0.5f);
					avg=-avg;
				}
				else
				{
					partMat.setPos(particle[partLinks[i].pBase].pos);
				}
			}

			Vector3 right = norm*avg;
			Vector3 up = right*norm;
			right.Normalise();
			up.Normalise();

			partMat.setUnitX(right);
			partMat.setUnitY(norm);
			partMat.setUnitZ(up);
			Vector3 offsetPos;
			partMat.rotateV3(bodyPart[i].offset,&offsetPos);
			partMat.Move(offsetPos);
			
			PhysicsManager::DLL_SetWorldMatrix(bodyPart[i].objectId,partMat);
		}		
	}
}
objectPartLink Ragdoll_Basic::partLinks[11]={
	{false,	RAGBASIC_NECK,			RAGBASIC_HEADLEFT,			RAGBASIC_HEADRIGHT,		false, 0, 0, 0},
	{false,	RAGBASIC_SHOULDERLEFT,	RAGBASIC_UPELBOWLEFT,		RAGBASIC_LOWELBOWLEFT,	false, 0, 0, 0},
	{true,	RAGBASIC_HANDLEFT,		RAGBASIC_LOWELBOWLEFT,		RAGBASIC_UPELBOWLEFT,	false, 0, 0, 0},
	{false,	RAGBASIC_SHOULDERRIGHT,	RAGBASIC_UPELBOWRIGHT,		RAGBASIC_LOWELBOWRIGHT,	false, 0, 0, 0},
	{true,	RAGBASIC_HANDRIGHT,		RAGBASIC_LOWELBOWRIGHT,		RAGBASIC_UPELBOWRIGHT,	false, 0, 0, 0},
	{true,	RAGBASIC_NECK,			RAGBASIC_WAISTLEFT,			RAGBASIC_WAISTRIGHT,	false, 0, 0, 0},
	{true,	RAGBASIC_WAISTLEFT,		RAGBASIC_LEGTOPLEFT,		RAGBASIC_LEGTOPRIGHT,	true,	RAGBASIC_WAISTRIGHT,	RAGBASIC_LEGTOPLEFT,	RAGBASIC_LEGTOPRIGHT},
	{false,	RAGBASIC_LEGTOPLEFT,	RAGBASIC_INKNEELEFT,		RAGBASIC_OUTKNEELEFT,	false, 0, 0, 0},
	{true,	RAGBASIC_FOOTLEFT,		RAGBASIC_OUTKNEELEFT,		RAGBASIC_INKNEELEFT,	false, 0, 0, 0},
	{false,	RAGBASIC_LEGTOPRIGHT,	RAGBASIC_INKNEERIGHT,		RAGBASIC_OUTKNEERIGHT,	false, 0, 0, 0},
	{true,	RAGBASIC_FOOTRIGHT,		RAGBASIC_OUTKNEERIGHT,		RAGBASIC_INKNEERIGHT,	false, 0, 0, 0}
};

void RagdollRotConstraint::Evaluate()
{
	Vector3 pav=(p1->pos+p2->pos+p3->pos+p4->pos)*0.25f; //Get average position
	Vector3 rot = (p1->pos-p1->lastPos)*(p1->pos-pav);
	rot += (p2->pos-p2->lastPos)*(p2->pos-pav);
	rot += (p3->pos-p3->lastPos)*(p3->pos-pav);
	rot += (p4->pos-p4->lastPos)*(p4->pos-pav);
	rot*=scale;
	//Reapply it in the opposite direction
	p1->pos+=rot*(p1->pos-pav);
	p2->pos+=rot*(p2->pos-pav);
	p3->pos+=rot*(p3->pos-pav);
	p4->pos+=rot*(p4->pos-pav);
}


void Ragdoll_Basic::onUpdateGraphics()
{
	if(drawDebugLines)
	{
		RagdollLink * curlink;
		int n = numLinks;
		for(int i=0;i<n;i++)
		{
			curlink = &link[i];
			if(curlink->type==RAGLINK_SOLID)
			{
				D3D_DebugLines::getInst()->setLine(curlink->p1->pos,curlink->p2->pos,0xffffff);
			}
		}
	}
}

//Delete object callback
void onRagdollObjectDeletedCallback(int id, int userData)
{
	RagdollPtr tmp;
	if(physics->getRagdoll(id,tmp)==PERR_ITEM_DOESNT_EXIST) return;
	if(!tmp->isValid()) return;

	tmp->removeRagdollObjectPart(userData);
}

ePhysError Ragdoll_Basic::fixObjectToRagdoll(int objectId, int partNo)
{	
	if(partNo<0 || partNo>10) return PERR_BAD_PARAMETER;
	if(!PhysicsManager::DLL_GetExists(objectId)) return PERR_ITEM_DOESNT_EXIST;	
	bodyPart[partNo].active=true;
	bodyPart[partNo].objectId=objectId;
	
	PhysicsManager::DLL_SetDeleteCallback(objectId,onRagdollObjectDeletedCallback,objId);
	return PERR_AOK;
}


ePhysError Ragdoll_Basic::removeRagdollObjectPart(int id)
{
	bool found = false;
	for(int i=0;i<11;i++)	
	{
		if(id==bodyPart[i].objectId)
		{
			bodyPart[i].active=false;
			bodyPart[i].objectId=-1;
			found=true;
		}
	}
	if(found) return PERR_AOK;	
	return PERR_ITEM_DOESNT_EXIST;
}

ePhysError Ragdoll_Basic::setRagdollObjectOffset(int partNo, float ox, float oy, float oz)
{
	if(partNo<0 || partNo>10) return PERR_BAD_PARAMETER;
	if(bodyPart[partNo].objectId==-1) return PERR_OBJECT_NOT_LINKED;	
	bodyPart[partNo].offset.Set(ox,oy,oz);
	return PERR_AOK;
}

ePhysError Ragdoll_Basic::setRagdollScales(float globalScale, 
										  float upperArmScale,float lowerArmScale,
										  float upperLegScale,float lowerLegScale,
										  float chestHeightScale, float waistHeightScale)
{
	if(globalScale<0.01f || upperArmScale<0.01f || lowerArmScale<0.01f
		|| upperLegScale<0.01f || lowerLegScale<0.01f || chestHeightScale<0.01f || waistHeightScale<0.01f) return PERR_BAD_PARAMETER;

	scaleBasicDoll(globalScale,upperArmScale,lowerArmScale,upperLegScale,lowerLegScale, chestHeightScale, waistHeightScale);

	return PERR_AOK;
}



//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeRagdoll(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addRagdoll(id,RAGDOLL_PLUGIN(Ragdoll_Basic)),PCMD_MAKE_RAGDOLL);
}

DLLEXPORT void fixObjectToRagdoll(int id, int objectID, int partNo)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_FIX_OBJECT_TO_RAGDOLL);

	RETURN_ON_PERR(tmp->fixObjectToRagdoll(objectID,partNo),PCMD_FIX_OBJECT_TO_RAGDOLL);
}

DLLEXPORT void setRagdollScales(int id, float globalScale, 
										  float upperArmScale,float lowerArmScale,
										  float upperLegScale,float lowerLegScale,
										  float chestHeightScale, float waistHeightScale)
{
	RagdollPtr tmp;
	RETURN_ON_PERR(physics->getRagdoll(id,tmp),PCMD_SET_RAGDOLL_SCALES);
	RETURN_ON_PERR(tmp->checkType(Ragdoll::classID,Ragdoll_Basic::classID),PCMD_SET_RAGDOLL_SCALES);

	Ragdoll_Basic * rag = reinterpret_cast<Ragdoll_Basic *>(&(*tmp));

	RETURN_ON_PERR(rag->setRagdollScales(globalScale*0.01f,upperArmScale*0.01f,lowerArmScale*0.01f,upperLegScale*0.01f,lowerLegScale*0.01f,chestHeightScale*0.01f,waistHeightScale*0.01f),PCMD_SET_RAGDOLL_SCALES);
}
