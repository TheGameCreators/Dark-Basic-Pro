#include "stdafx.h"

Ragdoll::Ragdoll(int id) : PhysicsObject(id),
frozen(false),drawDebugLines(false),particle(0),link(0),numParticles(0),numLinks(0),
damping(0),particleInvMass(1.0f)
{
	useExternalMatrix(true);
}

Ragdoll::~Ragdoll()
{	
}

ePhysError Ragdoll::setRagdollMass(float mass)
{
	if(mass<=0.0f) return PERR_BAD_PARAMETER;

	return PERR_AOK;
}



ePhysError Ragdoll::fixPoint(int pntNo, bool fixed)
{	
	if(pntNo<0 || pntNo>=numParticles) return PERR_BAD_PARAMETER;
	particle[pntNo].fixed = fixed;
	particle[pntNo].lastPos=particle[pntNo].pos; //Need this to remove apparent velocity from particle
	return PERR_AOK;
}


ePhysError Ragdoll::fixPoints(float px, float py, float pz, float radius, bool fixed)
{	
	if(radius<0) return PERR_BAD_PARAMETER;

	float radSq=radius*radius;
	Vector3 stickyPos(px,py,pz);

	RagdollParticle * curParticle = &particle[0];

	for(int i=0;i<numParticles;i++,curParticle++)
	{
		if((curParticle->pos-stickyPos).MagSqr()<=radSq)
		{
			curParticle->fixed=fixed;
			curParticle->lastPos=curParticle->pos; //Need this to remove apparent velocity from particle
		}
	}
	return PERR_AOK;
}

ePhysError Ragdoll::fixPoints(float px, float py, float pz, float sx, float sy, float sz, bool fixed)
{	
	if(sx<=0.0f || sy<=0.0f || sz<=0.0f) return PERR_BAD_PARAMETER;

	Vector3 minp(px-0.5f*sx,py-0.5f*sy,pz-0.5f*sz);
	Vector3 maxp(px+0.5f*sx,py+0.5f*sy,pz+0.5f*sz);

	RagdollParticle * curParticle = &particle[0];

	for(int i=0;i<numParticles;i++,curParticle++)
	{
		if(curParticle->pos.x<maxp.x && curParticle->pos.y<maxp.y && curParticle->pos.z<maxp.z)
		{
			if(curParticle->pos.x>minp.x && curParticle->pos.y>minp.y && curParticle->pos.z>minp.z)
			{
				curParticle->fixed=fixed;
				curParticle->lastPos=curParticle->pos; //Need this to remove apparent velocity from particle
			}
		}
	}
	return PERR_AOK;
}


ePhysError Ragdoll::fixNearestPoint(float px, float py, float pz, bool fixed)
{	
	float minRadSq=1e38f;
	Vector3 stickyPos(px,py,pz);

	RagdollParticle * curParticle = &particle[0];
	RagdollParticle * minParticle = 0;

	for(int i=0;i<numParticles;i++,curParticle++)
	{
		float curRadSq = (curParticle->pos-stickyPos).MagSqr();
		if(curRadSq<minRadSq)
		{
			minParticle=curParticle;
			minRadSq=curRadSq;
		}
	}
	if(minParticle)
	{
		minParticle->fixed=fixed;
		minParticle->lastPos=minParticle->pos; //Need this to remove apparent velocity from particle
	}
	return PERR_AOK;
}


ePhysError Ragdoll::fixPointToObject(int pntNo, int objectID, Vector3 * pos)
{	
	if(pntNo<0 || pntNo>=numParticles) return PERR_BAD_PARAMETER;
	if(!PhysicsManager::DLL_GetExists(objectID)) return PERR_ITEM_DOESNT_EXIST;
	
	particle[pntNo].fixed = true;
	particle[pntNo].objectID=objectID;

	//Check to see if we're already referring to the object
	if(refObjectID.Find(objectID)==-1)
	{
		//Make sure the object notifies us if it gets deleted
		refObjectID.Push(objectID);
		PhysicsManager::DLL_SetDeleteCallback(objectID,onAttachedObjectDeletedRagdollCallback,objId);
	}

	//Give particle the pointer	to the object's position information
	PhysicsManager::DLL_GetPositionData(objectID,&(particle[pntNo].DBProPosition));

	if(!pos)
	{
		//Calculate the local position on the target object
		Matrix mat,invMat;
		mat.convertD3DMatrix(&(particle[pntNo].DBProPosition->matWorld));
		invMat.getInverse3x3Of(mat);
		particle[pntNo].fixedPos=particle[pntNo].pos;
		particle[pntNo].fixedPos-=mat.pos();
		invMat.rotateV3(&particle[pntNo].fixedPos);
	}
	else
	{
		particle[pntNo].fixedPos=*pos;
	}
	particle[pntNo].lastPos=particle[pntNo].pos; //Need this to remove apparent velocity from particle
	return PERR_AOK;
}


ePhysError Ragdoll::freeAllPoints()
{
	for(int i=0;i<numParticles;i++)
	{
		if(particle[i].fixed)
		{
			particle[i].fixed=false;
			particle[i].objectID=-1;
			particle[i].DBProPosition=0;			
		}
	}
	refObjectID.Clear();
	return PERR_AOK;
}

ePhysError Ragdoll::freePointsOnObject(int objectID)
{
	int objIndex = refObjectID.Find(objectID);
	if(objIndex==-1) return PERR_ITEM_DOESNT_EXIST;
	
	for(int i=0;i<numParticles;i++)
	{
		if(particle[i].fixed && particle[i].objectID==objectID)
		{
			particle[i].fixed=false;
			particle[i].objectID=-1;
			particle[i].DBProPosition=0;			
		}
	}
	refObjectID.Remove(objIndex);
	return PERR_AOK;
}

ePhysError Ragdoll::freePointsOnAllObjects()
{	
	if(refObjectID.Count()==0) return PERR_AOK;
	for(int i=0;i<numParticles;i++)
	{
		if(particle[i].fixed && particle[i].objectID!=-1)
		{
			particle[i].fixed=false;
			particle[i].objectID=-1;
			particle[i].DBProPosition=0;			
		}
	}
	refObjectID.Clear();
	return PERR_AOK;
}

ePhysError Ragdoll::applyForce(int pointId, float fx, float fy, float fz)
{
	if(pointId<0 || pointId>=numParticles) return PERR_BAD_PARAMETER;
	particle[pointId].force.x+=fx;
	particle[pointId].force.y+=fy;
	particle[pointId].force.z+=fz;
	return PERR_AOK;
}


ePhysError Ragdoll::applyImpulse(int pointId, float ix, float iy, float iz)
{
	if(pointId<0 || pointId>=numParticles) return PERR_BAD_PARAMETER;
	particle[pointId].pos.x+=ix*PhysicsManager::iterationInterval;
	particle[pointId].pos.y+=iy*PhysicsManager::iterationInterval;
	particle[pointId].pos.z+=iz*PhysicsManager::iterationInterval;
	return PERR_AOK;
}


/*
ePhysError Cloth::saveClothState(const char * filename)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;

	//Append .clt to the end if it's not already there
	int len = (int)strlen(filename);
	char fname[512];
	if(strcmpi(&filename[len-4],".clt")!=0)
	{
		sprintf(fname,"%s.clt",filename);
	}
	else strcpy(fname,filename);

	FILE * file;
	file = fopen(fname,"wb");

	if(!file) return PERR_CANT_CREATE_SAVE_FILE;

	//Write out a header
	char fileID[19] = "PHYSICS:CLOTH001.0";
	if(!fwrite(fileID,19,1,file)) {fclose(file);return PERR_IO_FILE_ERROR;}

	if(!saveParams(file)) {fclose(file);return PERR_IO_FILE_ERROR;}
	for(int i=0;i<numParticles;i++)
	{
		if(!particle[i].saveParams(file)) {fclose(file);return PERR_IO_FILE_ERROR;}
	}
	for(int i=0;i<numSegs;i++)
	{
		if(!segment[i].saveParams(file,&particle[0])) {fclose(file);return PERR_IO_FILE_ERROR;}
	}
	fclose(file);
	return PERR_AOK;
}

ePhysError Cloth::loadClothState(const char * filename, bool useVelocities)
{
	if(hasMesh) return PERR_CLOTH_ALREADY_GENERATED;

	//Append .clt to the end if it's not already there
	int len = (int)strlen(filename);
	char fname[512];
	if(strcmpi(&filename[len-4],".clt")!=0)
	{
		sprintf(fname,"%s.clt",filename);
	}
	else strcpy(fname,filename);

	FILE * file;
	file = fopen(fname,"rb");
	if(!file) return PERR_CANT_LOAD_FILE;

	//Read and check the header
	char fileID[19];
	fread(fileID,19,1,file);
	if(strcmp("PHYSICS:CLOTH001.0",fileID)!=0) {fclose(file);return PERR_INCORRECT_FILE_FORMAT;}

	if(!loadParams(file)) {fclose(file);return PERR_IO_FILE_ERROR;}
	particle = new ClothParticle[numParticles];
	segment = new ClothSegment[numSegs];

	for(int i=0;i<numParticles;i++)
	{
		if(!particle[i].loadParams(file)) {fclose(file);return PERR_IO_FILE_ERROR;}
		if(particle[i].fixed && particle[i].objectID!=-1)
		{
			//Check to see if we're already referring to the object
			if(refObjectID.Find(particle[i].objectID)==-1)
			{
				//Make sure the object notifies us if it gets deleted
				refObjectID.Push(particle[i].objectID);
				PhysicsManager::DLL_SetDeleteCallback(particle[i].objectID,onAttachedObjectDeletedCallback,0);
			}
		}
	}
	for(int i=0;i<numSegs;i++)
	{
		if(!segment[i].loadParams(file,&particle[0])) {fclose(file);return PERR_IO_FILE_ERROR;}
	}
	fclose(file);

	if(!useVelocities)
	{
		for(int i=0;i<numParticles;i++)
		{
			particle[i].lastPos=particle[i].pos;
		}
	}
	
	allocateGraphicsAndInit(false);
	hasMesh=true;
	return PERR_AOK;
}



bool Cloth::saveParams(FILE * file)
{
	if(!fwrite(&numParticles,sizeof(int),1,file)) return false;
	if(!fwrite(&numTriangles,sizeof(int),1,file)) return false;
	if(!fwrite(&numSegs,sizeof(int),1,file)) return false;
	if(!fwrite(&elastic_USER,sizeof(float),1,file)) return false;
	if(!fwrite(&mass_USER,sizeof(float),1,file)) return false;
	if(!fwrite(&particleInvMass,sizeof(float),1,file)) return false;
	if(!fwrite(&clothStiffness,sizeof(float),1,file)) return false;
	return true;
}

bool Cloth::loadParams(FILE * file)
{
	if(!fread(&numParticles,sizeof(int),1,file)) return false;
	if(!fread(&numTriangles,sizeof(int),1,file)) return false;
	if(!fread(&numSegs,sizeof(int),1,file)) return false;
	if(!fread(&elastic_USER,sizeof(float),1,file)) return false;
	if(!fread(&mass_USER,sizeof(float),1,file)) return false;
	if(!fread(&particleInvMass,sizeof(float),1,file)) return false;
	if(!fread(&clothStiffness,sizeof(float),1,file)) return false;
	return true;
}

bool ClothParticle::saveParams(FILE * file)
{
	if(!fwrite(&pos,sizeof(Vector3),1,file)) return false;
	if(!fwrite(&lastPos,sizeof(Vector3),1,file)) return false;
	if(!fwrite(&segmentWeighting,sizeof(float),1,file)) return false;
	if(!fwrite(&triWeighting,sizeof(float),1,file)) return false;
	if(!fwrite(&fixed,sizeof(bool),1,file)) return false;
	if(!fwrite(&objectID,sizeof(int),1,file)) return false;
	if(!fwrite(&fixedPos,sizeof(Vector3),1,file)) return false;
	if(!fwrite(&u,sizeof(float),1,file)) return false;
	if(!fwrite(&v,sizeof(float),1,file)) return false;
	return true;
}

bool ClothParticle::loadParams(FILE * file)
{
	if(!fread(&pos,sizeof(Vector3),1,file)) return false;
	if(!fread(&lastPos,sizeof(Vector3),1,file)) return false;
	if(!fread(&segmentWeighting,sizeof(float),1,file)) return false;
	if(!fread(&triWeighting,sizeof(float),1,file)) return false;
	if(!fread(&fixed,sizeof(bool),1,file)) return false;
	if(!fread(&objectID,sizeof(int),1,file)) return false;
	if(!fread(&fixedPos,sizeof(Vector3),1,file)) return false;
	if(!fread(&u,sizeof(float),1,file)) return false;
	if(!fread(&v,sizeof(float),1,file)) return false;

	if(fixed && objectID!=-1)
	{
		//Reassign cloth to object
		if(PhysicsManager::DLL_GetExists(objectID))
		{
			PhysicsManager::DLL_GetPositionData(objectID,&DBProPosition);
		}
		else
		{
			objectID=-1;
			DBProPosition=0;
		}
	}

	return true;
}

bool ClothSegment::saveParams(FILE * file, ClothParticle * base)
{
	int p[4];
	p[0]=(int)(particle[0]-base);
	p[1]=(int)(particle[1]-base);
	p[2]=(int)(particle[2]-base);
	if(particle[3]) p[3]=(int)(particle[3]-base);
	else p[3]=-1;

	if(!fwrite(p,sizeof(int)*4,1,file)) return false;
	if(!fwrite(distSq,sizeof(float)*6,1,file)) return false;
	if(!fwrite(&numParticles,sizeof(int),1,file)) return false;

	return true;
}

bool ClothSegment::loadParams(FILE * file, ClothParticle * base)
{
	int p[4];
	if(!fread(p,sizeof(int)*4,1,file)) return false;
	if(!fread(distSq,sizeof(float)*6,1,file)) return false;
	if(!fread(&numParticles,sizeof(int),1,file)) return false;

	particle[0] = base+p[0];
	particle[1] = base+p[1];
	particle[2] = base+p[2];
	if(p[3]!=-1) particle[3] = base+p[3];
	else particle[3]=0;

	return true;
}
*/

/*
void removeClothObjectReferences(ClothPtr& cloth, void * userData)
{
	//Gets called for each cloth
	int id = *((int*)userData);
	cloth->freePointsOnObject(id);		
}

void onAttachedObjectDeletedCallback(int id, int userData)
{
	physics->callForEachCloth(removeClothObjectReferences,&id);
}
*/


void onAttachedObjectDeletedRagdollCallback(int id, int userData)
{
	RagdollPtr ragdoll;
	if(physics->getRagdoll(userData,ragdoll)!=PERR_AOK) return;
	if(!ragdoll->isValid()) return;
	ragdoll->freePointsOnObject(id);	
}

