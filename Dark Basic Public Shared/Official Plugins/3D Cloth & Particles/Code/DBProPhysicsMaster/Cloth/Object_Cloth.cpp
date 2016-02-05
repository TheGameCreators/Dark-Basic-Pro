#include "stdafx.h"

const int Cloth::classID='CLTH';

void ClothSegment::Set(ClothParticle * p0, ClothParticle * p1, ClothParticle * p2, ClothParticle * p3)
{
	p0->segmentWeighting+=1.0f;
	p1->segmentWeighting+=1.0f;
	p2->segmentWeighting+=1.0f;
	if(p3) p3->segmentWeighting+=1.0f;

	p0->triWeighting+=1.0f;
	p1->triWeighting+=1.0f;
	p2->triWeighting+=1.0f;
	if(p3) 
	{
		p1->triWeighting+=1.0f;
		p2->triWeighting+=1.0f;
		p3->triWeighting+=1.0f;
	}


	particle[0]=p0;
	particle[1]=p1;
	particle[2]=p2;
	particle[3]=p3;

	if(!particle[3]) numParticles=3; 
	else  numParticles=4;

	Relax();
	//int nMinusOne = numParticles-1;
	//int count=0;
	//for(int i=0;i<nMinusOne;i++)
	//{
	//	for(int j=i+1;j<numParticles;j++)
	//	{
	//		distSq[count] = (particle[j]->pos-particle[i]->pos).MagSqr();			
	//		count++;
	//	}
	//}
}

void ClothSegment::Relax()
{
	int nMinusOne = numParticles-1;
	int count=0;
	for(int i=0;i<nMinusOne;i++)
	{
		for(int j=i+1;j<numParticles;j++)
		{
			distSq[count] = (particle[j]->pos-particle[i]->pos).MagSqr();			
			count++;
		}
	}
}

Cloth::Cloth(int id) : PhysicsObject(id),
hasMesh(false),frozen(false),segment(0),particle(0),numSegs(0),numParticles(0),
numTriangles(0),damping(0),elastic_USER(1.0f),mass_USER(1.0f),particleInvMass(1.0f),clothStiffness(1.0f),
fsu(1.0f),fsv(1.0f),fmu(0.0f),fmv(0.0f),bsu(1.0f),bsv(1.0f),bmu(0.0f),bmv(0.0f)
{
	useExternalMatrix(true);
}

Cloth::~Cloth()
{	
	if(hasMesh)
	{
		SAFEDELETE_ARRAY(segment);
		SAFEDELETE_ARRAY(particle);
	}
}

void Cloth::onUpdate()
{
	if(!hasMesh || frozen) return;
	int i;
	
	Vector3 tempPos;
	ClothParticle * curPar = &particle[0];

	float timeIntSqr = PhysicsManager::iterationInterval;
	timeIntSqr*=timeIntSqr;		

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
	ClothSegment * curSeg = &segment[0];
	for(i=0;i<numSegs;i++,curSeg++)
	{
		int nMinusOne = curSeg->numParticles-1;
		int count=0;
		for(int j=0;j<nMinusOne;j++)
		{
			for(int k=j+1;k<curSeg->numParticles;k++)
			{
				ClothParticle& p1=*curSeg->particle[j];
				ClothParticle& p2=*curSeg->particle[k];
				Vector3& x1 = p1.pos;
				Vector3& x2 = p2.pos;
				Vector3 delta = x2-x1;
				delta*=curSeg->distSq[count]/(delta.MagSqr()+curSeg->distSq[count])-0.5f;
				//Note: signs are reversed from (incorrect) Verlet paper
				delta*=clothStiffness;
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
				
				count++;
			}
		}		
	}
}


void Cloth::onUpdateGraphics()
{
	if(!hasMesh || frozen) return;

	for(int i=0;i<numParticles;i++)
	{
		particle[i].normal.Init();
	}
	
	//Calculate the face normals
	Vector3 normal;
	// mike
	//for(int i=0;i<numSegs;i++)
	for( i=0;i<numSegs;i++)
	{
		ClothSegment * curSeg = &segment[i];
		Vector3 a,b;
		a=curSeg->particle[1]->pos - curSeg->particle[0]->pos;
		b=curSeg->particle[2]->pos - curSeg->particle[0]->pos;
		normal=a * b;
		normal.Normalise();
		curSeg->particle[0]->normal+=normal;
		curSeg->particle[1]->normal+=normal;
		curSeg->particle[2]->normal+=normal;
		if(curSeg->particle[3])
		{
			a=curSeg->particle[3]->pos - curSeg->particle[1]->pos;
			b=curSeg->particle[2]->pos - curSeg->particle[1]->pos;
			normal=a*b;
			normal.Normalise();
			curSeg->particle[1]->normal+=normal;
			curSeg->particle[2]->normal+=normal;
			curSeg->particle[3]->normal+=normal;
		}
	}

	PhysicsManager::DLL_LockVertexData(objId,0);

	// mike
	//for(int i=0;i<numParticles;i++)
	for( i=0;i<numParticles;i++)
	{		
		ClothParticle& p = particle[i];
		p.normal*=p.triWeighting;
		PhysicsManager::DLL_SetVertexPosition(i,p.pos.x,p.pos.y,p.pos.z);
		PhysicsManager::DLL_SetVertexNormal(i,p.normal.x,p.normal.y,p.normal.z);
		if(doubleSided)
		{
			PhysicsManager::DLL_SetVertexPosition(i+numParticles,p.pos.x,p.pos.y,p.pos.z);
			PhysicsManager::DLL_SetVertexNormal(i+numParticles,-p.normal.x,-p.normal.y,-p.normal.z);
		}

	}
	PhysicsManager::DLL_UnlockVertexData();
}

void Cloth::allocateGraphicsAndInit(bool calcUVs) //Defaults to true
{	
	if(doubleSided)
	{
		PhysicsManager::DLL_CreateMesh(objId, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, numParticles*2, numTriangles*6);
	}
	else
	{
		PhysicsManager::DLL_CreateMesh(objId, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, numParticles, numTriangles*3);
	}

	PhysicsManager::DLL_LockVertexData(objId,0);

	if(calcUVs)
	{
		//Make 2D bounding box of cloth vertices and use it to calculate UVs
		Vector3 bbmin(particle[0].pos);
		Vector3 bbmax(particle[0].pos);

		for(int i=1;i<numParticles;i++)
		{
			Vector3& p = particle[i].pos;
			if(p.x<bbmin.x) bbmin.x=p.x;
			if(p.z<bbmin.z) bbmin.z=p.z;
			if(p.x>bbmax.x) bbmax.x=p.x;
			if(p.z>bbmax.z) bbmax.z=p.z;
		}

		Vector3 bbSize = bbmax-bbmin;

		//Assign vertex positions and UVs
		//for(int i=0;i<numParticles;i++)
		// mike
		for(i=0;i<numParticles;i++)
		{
			Vector3& p = particle[i].pos;
			particle[i].u = (p.x-bbmin.x)/bbSize.x;
			particle[i].v = (p.z-bbmin.z)/bbSize.z;
			PhysicsManager::DLL_SetVertexUV(i,particle[i].u,particle[i].v);
			PhysicsManager::DLL_SetVertexPosition(i,p.x,p.y,p.z);

			if(doubleSided)
			{
				PhysicsManager::DLL_SetVertexUV(i+numParticles,particle[i].u,particle[i].v);
				PhysicsManager::DLL_SetVertexPosition(i+numParticles,p.x,p.y,p.z);
			}
		}
	}
	else
	{
		//Assign vertex positions and UVs
		for(int i=0;i<numParticles;i++)
		{
			Vector3& p = particle[i].pos;
			PhysicsManager::DLL_SetVertexUV(i,particle[i].u*fsu+fmu,particle[i].v*fsv+fmv);
			PhysicsManager::DLL_SetVertexPosition(i,p.x,p.y,p.z);
			
			if(doubleSided)
			{
				PhysicsManager::DLL_SetVertexUV(i+numParticles,particle[i].u*bsu+bmu,particle[i].v*bsv+bmv);
				PhysicsManager::DLL_SetVertexPosition(i+numParticles,p.x,p.y,p.z);
			}
		}
	}
	//Create face indices
	int index=0;
	ClothSegment * curSeg;
	for(int i=0;i<numSegs;i++)
	{
		//Triangle 01
		curSeg = &segment[i];
		int pId0 = (int) (curSeg->particle[0]-&particle[0]);		
		PhysicsManager::DLL_SetIndex(index++,pId0);		
		int pId1 = (int) (curSeg->particle[1]-&particle[0]);
		PhysicsManager::DLL_SetIndex(index++,pId1);
		int pId2 = (int) (curSeg->particle[2]-&particle[0]);		
		PhysicsManager::DLL_SetIndex(index++,pId2);
		
		if(doubleSided)
		{
			PhysicsManager::DLL_SetIndex(index++,pId0+numParticles);
			PhysicsManager::DLL_SetIndex(index++,pId2+numParticles);
			PhysicsManager::DLL_SetIndex(index++,pId1+numParticles);
		}

		//If the fourth vertex exists, create second triangle
		if(curSeg->particle[3])
		{
			//Triangle 02
			PhysicsManager::DLL_SetIndex(index++,pId1);
			int pId3 = (int) (curSeg->particle[3]-&particle[0]);
			PhysicsManager::DLL_SetIndex(index++,pId3);
			PhysicsManager::DLL_SetIndex(index++,pId2);

			if(doubleSided)
			{
				PhysicsManager::DLL_SetIndex(index++,pId1+numParticles);
				PhysicsManager::DLL_SetIndex(index++,pId2+numParticles);
				PhysicsManager::DLL_SetIndex(index++,pId3+numParticles);
			}
		}
	}

	//for(int i=0;i<numParticles;i++)
	// mike
	for(i=0;i<numParticles;i++)
	{
		particle[i].normal.Init();
	}
	
	//Calculate the face normals
	Vector3 normal;
	// mike
	//for(int i=0;i<numSegs;i++)
	for(i=0;i<numSegs;i++)
	{
		ClothSegment * curSeg = &segment[i];
		Vector3 a,b;
		a=curSeg->particle[1]->pos - curSeg->particle[0]->pos;
		b=curSeg->particle[2]->pos - curSeg->particle[0]->pos;
		normal=a * b;
		normal.Normalise();
		curSeg->particle[0]->normal+=normal;
		curSeg->particle[1]->normal+=normal;
		curSeg->particle[2]->normal+=normal;
		if(curSeg->particle[3])
		{
			a=curSeg->particle[3]->pos - curSeg->particle[1]->pos;
			b=curSeg->particle[2]->pos - curSeg->particle[1]->pos;
			normal=a*b;
			normal.Normalise();
			curSeg->particle[1]->normal+=normal;
			curSeg->particle[2]->normal+=normal;
			curSeg->particle[3]->normal+=normal;
		}
	}
	
	//Calculate the smoothed vertex normals
	// mike
	//for(int i=0;i<numParticles;i++)
	for(i=0;i<numParticles;i++)
	{
		ClothParticle& p = particle[i];
		p.normal*=p.triWeighting;
		PhysicsManager::DLL_SetVertexNormal(i,p.normal.x,p.normal.y,p.normal.z);
		if(doubleSided)
		{
			PhysicsManager::DLL_SetVertexNormal(i+numParticles,-p.normal.x,-p.normal.y,-p.normal.z);
		}
	}

	PhysicsManager::DLL_UnlockVertexData();
}


ePhysError Cloth::setClothElasticity(float elast)
{
	if(elast<=0.0f) return PERR_BAD_PARAMETER;
	elastic_USER=elast;
	clothStiffness=1.0f/(mass_USER*elastic_USER);
	return PERR_AOK;
}

ePhysError Cloth::setClothMass(float mass)
{
	if(mass<=0.0f) return PERR_BAD_PARAMETER;
	mass_USER=mass;

	particleInvMass=1.0f/mass_USER;
	clothStiffness=1.0f/(mass_USER*elastic_USER);
	return PERR_AOK;
}



ePhysError Cloth::fixPoint(int pntNo, bool fixed)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(pntNo<0 || pntNo>=numParticles) return PERR_BAD_PARAMETER;
	particle[pntNo].fixed = fixed;
	particle[pntNo].lastPos=particle[pntNo].pos; //Need this to remove apparent velocity from particle
	return PERR_AOK;
}


ePhysError Cloth::fixPoints(float px, float py, float pz, float radius, bool fixed)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(radius<0) return PERR_BAD_PARAMETER;

	float radSq=radius*radius;
	Vector3 stickyPos(px,py,pz);

	ClothParticle * curParticle = &particle[0];

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

ePhysError Cloth::fixPoints(float px, float py, float pz, float sx, float sy, float sz, bool fixed)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(sx<=0.0f || sy<=0.0f || sz<=0.0f) return PERR_BAD_PARAMETER;

	Vector3 minp(px-0.5f*sx,py-0.5f*sy,pz-0.5f*sz);
	Vector3 maxp(px+0.5f*sx,py+0.5f*sy,pz+0.5f*sz);

	ClothParticle * curParticle = &particle[0];

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


ePhysError Cloth::fixNearestPoint(float px, float py, float pz, bool fixed)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	
	float minRadSq=1e38f;
	Vector3 stickyPos(px,py,pz);

	ClothParticle * curParticle = &particle[0];
	ClothParticle * minParticle = 0;

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


ePhysError Cloth::fixPointToObject(int pntNo, int objectID, Vector3 * pos)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(pntNo<0 || pntNo>=numParticles) return PERR_BAD_PARAMETER;
	if(!PhysicsManager::DLL_GetExists(objectID)) return PERR_ITEM_DOESNT_EXIST;
	
	particle[pntNo].fixed = true;
	particle[pntNo].objectID=objectID;

	//Check to see if we're already referring to the object
	if(refObjectID.Find(objectID)==-1)
	{
		//Make sure the object notifies us if it gets deleted
		refObjectID.Push(objectID);
		if(objectID!=objId) //Don't set the callback if we are fixing to ourselves
		{
			PhysicsManager::DLL_SetDeleteCallback(objectID,onAttachedObjectDeletedClothCallback,objId);
		}
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


ePhysError Cloth::freeAllPoints()
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;

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

ePhysError Cloth::freePointsOnObject(int objectID)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;

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

ePhysError Cloth::freePointsOnAllObjects()
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;

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
	// mike
	//for(int i=0;i<numSegs;i++)
	for(i=0;i<numSegs;i++)
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
				if(particle[i].objectID!=objId)
				{
					PhysicsManager::DLL_SetDeleteCallback(particle[i].objectID,onAttachedObjectDeletedClothCallback,objId);
				}
			}
		}
	}
	// mike
	//for(int i=0;i<numSegs;i++)
	for(i=0;i<numSegs;i++)
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

ePhysError Cloth::Relax()
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	for(int i=0;i<numSegs;i++)
	{
		segment[i].Relax();
	}
	return PERR_AOK;
}

ePhysError Cloth::TransformUVs(float scaleu, float scalev, float moveu, float movev, int mode)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(scaleu<0 || scalev<0 || mode<0 || mode>2) return PERR_BAD_PARAMETER;
	
	//mode=0    -   Transform both sides	
	//mode=1    -   Transform cloth front
	//mode=2    -   Transform cloth back
	
	if(mode<2)
	{
		fsu*=scaleu;
		fsv*=scalev;
		fmu*=scaleu;
		fmv*=scaleu;

		fmu+=moveu;
		fmv+=movev;
	}
	if(doubleSided && (mode==0 || mode==2))
	{
		bsu*=scaleu;
		bsv*=scalev;
		bmu*=scaleu;
		bmv*=scaleu;

		bmu+=moveu;
		bmv+=movev;
	}

	PhysicsManager::DLL_LockVertexData(objId,0);

	//Assign vertex positions and UVs
	for(int i=0;i<numParticles;i++)
	{
		if(mode<2)
		{
			PhysicsManager::DLL_SetVertexUV(i,particle[i].u*scaleu+moveu,particle[i].v*scalev+movev);
		}
		if(doubleSided && (mode==0 || mode==2))
		{
			PhysicsManager::DLL_SetVertexUV(i+numParticles,particle[i].u*scaleu+moveu,particle[i].v*scalev+movev);
		}
	}
	
	PhysicsManager::DLL_UnlockVertexData();

	return PERR_AOK;
}

ePhysError Cloth::ResetUVs(int mode)
{
	if(!hasMesh) return PERR_CLOTH_NOT_GENERATED;
	if(mode<0 || mode>3) return PERR_BAD_PARAMETER;
	
	//mode=0    -   Reset both sides	
	//mode=1    -   Reset cloth front
	//mode=2    -   Reset cloth back
		
	if(mode<2)
	{
		fsu=1.0f;
		fsv=1.0f;
		fmu=0.0f;
		fmv=0.0f;
	}
	if(doubleSided && (mode==0 || mode==2))
	{
		bsu=1.0f;
		bsv=1.0f;
		bmu=0.0f;
		bmv=0.0f;
	}

	PhysicsManager::DLL_LockVertexData(objId,0);

	//Assign vertex positions and UVs
	for(int i=0;i<numParticles;i++)
	{
		if(mode<2)
		{
			PhysicsManager::DLL_SetVertexUV(i,particle[i].u,particle[i].v);
		}
		if(doubleSided && (mode==0 || mode==2))
		{
			PhysicsManager::DLL_SetVertexUV(i+numParticles,particle[i].u,particle[i].v);
		}
	}
	
	PhysicsManager::DLL_UnlockVertexData();

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
	if(!fwrite(&doubleSided,sizeof(bool),1,file)) return false;
	if(!fwrite(&fsu,sizeof(float),1,file)) return false;
	if(!fwrite(&fsv,sizeof(float),1,file)) return false;
	if(!fwrite(&fmu,sizeof(float),1,file)) return false;
	if(!fwrite(&fmv,sizeof(float),1,file)) return false;
	if(!fwrite(&bsu,sizeof(float),1,file)) return false;
	if(!fwrite(&bsv,sizeof(float),1,file)) return false;
	if(!fwrite(&bmu,sizeof(float),1,file)) return false;
	if(!fwrite(&bmv,sizeof(float),1,file)) return false;
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
	if(!fread(&doubleSided,sizeof(bool),1,file)) return false;
	if(!fread(&fsu,sizeof(float),1,file)) return false;
	if(!fread(&fsv,sizeof(float),1,file)) return false;
	if(!fread(&fmu,sizeof(float),1,file)) return false;
	if(!fread(&fmv,sizeof(float),1,file)) return false;
	if(!fread(&bsu,sizeof(float),1,file)) return false;
	if(!fread(&bsv,sizeof(float),1,file)) return false;
	if(!fread(&bmu,sizeof(float),1,file)) return false;
	if(!fread(&bmv,sizeof(float),1,file)) return false;
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


void onAttachedObjectDeletedClothCallback(int id, int userData)
{
	ClothPtr cloth;
	if(physics->getCloth(userData,cloth)!=PERR_AOK) return;
	if(!cloth->isValid()) return;
	cloth->freePointsOnObject(id);	
}

