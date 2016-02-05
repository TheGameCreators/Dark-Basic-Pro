#include "stdafx.h"


DECLARE_PLUGIN(Cloth_Basic);

const int Cloth_Basic::classID='CBSC';

ePhysError Cloth_Basic::generateRectangularCloth(float sx, float sz, int resx, int resz, bool useDoubleSided)
{
	if(hasMesh) return PERR_CLOTH_ALREADY_GENERATED;
	if(resx<1 || resz<1 || sx<=0 || sz<=0) return PERR_BAD_PARAMETER;

	doubleSided = useDoubleSided;

	getEngineMatrix();

	float segx = sx/resx;
	float segz = sz/resz;

	numSegs = resx*resz;
	numParticles = (resx+1)*(resz+1);
	numTriangles = numSegs*2;	

	segment = new ClothSegment[numSegs];
	particle = new ClothParticle[numParticles];

	float offsetX=sx*-0.5f;
	float offsetZ=sz*-0.5f;

	int count=0;
	for(int z=0;z<resz;z++)
	{
		for(int x=0;x<resx;x++)
		{
			int pBase = x+z*(resx+1);
			
			particle[pBase].Set(x*segx+offsetX,0,z*segz+offsetZ);
			particle[pBase+1].Set((x+1)*segx+offsetX,0,z*segz+offsetZ);
			particle[pBase+resx+1].Set(x*segx+offsetX,0,(z+1)*segz+offsetZ);
			particle[pBase+resx+2].Set((x+1)*segx+offsetX,0,(z+1)*segz+offsetZ);
			segment[count].Set(&particle[pBase],&particle[pBase+resx+1],&particle[pBase+1],&particle[pBase+resx+2]);
			count++;
		}
	}

	//Adjust segment weighting, and transform particles by object matrix
	for(int i=0;i<numParticles;i++)
	{
		particle[i].segmentWeighting=1.0f/particle[i].segmentWeighting;
		particle[i].triWeighting=1.0f/particle[i].triWeighting;
		m.rotateAndTransV3(&(particle[i].pos));
		particle[i].lastPos=particle[i].pos;
	}
		
	allocateGraphicsAndInit();

	hasMesh=true;
	return PERR_AOK;
}

ePhysError Cloth_Basic::generateClothFromBitmap(int bitmapID, float sx, float sz, bool autoTriangles, bool useDoubleSided)
{
	if(hasMesh) return PERR_CLOTH_ALREADY_GENERATED;
	if(bitmapID<=0 || sx<=0 || sz<=0) return PERR_BAD_PARAMETER;

	doubleSided = useDoubleSided;

	getEngineMatrix();

	TextureAccess texture(bitmapID);

	if(!texture.getTexture()) return PERR_IMAGE_DOESNT_EXIST;
	int w, h;
	w=texture.Width();
	h=texture.Height();

	int numPix = w*h;
	ClothBitmapElement ** segArray = new ClothBitmapElement*[numPix];
	if(!segArray) return PERR_OUT_OF_MEMORY;	
	for(int i=0;i<numPix;i++)
	{			
		segArray[i]=0;
	}

	float segx = sx/w;
	float segz = sz/h;
	float offsetX=sx*-0.5f;
	float offsetZ=sz*-0.5f;

	try
	{
		numParticles=0;
		numSegs=0;
		//Count number of vertices and segments needed
		for(int y=0;y<h;y++)
		{
			for(int x=0;x<w;x++)
			{		
				int pixVal = texture.getRed(x,y);
				if(pixVal>127)
				{
					int makeVert=7;
					if(texture.isRedAboveHalf(x-1,y-1)) makeVert&=6;
					if(texture.isRedAboveHalf(x,y-1)) makeVert&=4;
					if(texture.isRedAboveHalf(x+1,y-1)) makeVert&=5;
					if(texture.isRedAboveHalf(x-1,y)) makeVert&=2;
					numParticles++;
					if(makeVert & 1) numParticles++;
					if(makeVert & 2) numParticles++;
					if(makeVert & 4) numParticles++;
					
					//Need quad segment
					segArray[x+y*w] = new ClothBitmapElement;
					if(!segArray[x+y*w]) throw;
					numSegs++;
				}
				else
				{	
					if(autoTriangles || pixVal>5)
					{
						int vertExists=0;
						if(texture.isRedAboveHalf(x-1,y-1)) vertExists|=1;
						if(texture.isRedAboveHalf(x,y-1)) vertExists|=3;
						if(texture.isRedAboveHalf(x+1,y-1)) vertExists|=2;
						if(texture.isRedAboveHalf(x-1,y)) vertExists|=5;
						if(texture.isRedAboveHalf(x+1,y)) vertExists|=10;
						if(texture.isRedAboveHalf(x-1,y+1)) vertExists|=4;
						if(texture.isRedAboveHalf(x,y+1)) vertExists|=12;
						if(texture.isRedAboveHalf(x+1,y+1)) vertExists|=8;
						int num=0;
						if(vertExists & 1) num++;
						if(vertExists & 2) num++;
						if(vertExists & 4) num++;
						if(vertExists & 8) num++;
						if(num==3) 
						{
							//Need triangular segment
							segArray[x+y*w] = new ClothBitmapElement;
							if(!segArray[x+y*w]) throw;
							segArray[x+y*w]->numParticles=3;
							numSegs++;
						}
					}
				}
			}
		}
		texture.releaseTexture();

		if(numSegs==0)
		{
			for(int i=0;i<numPix;i++)
			{			
				if(segArray[i]) 
				{
					SAFEDELETE(segArray[i]);
				}
			}
			SAFEDELETE_ARRAY(segArray);			
			return PERR_NO_VALID_PIXELS_IN_IMAGE;
		}

		segment=0;
		particle=0;
		segment = new ClothSegment[numSegs];
		particle = new ClothParticle[numParticles];
	}
	catch(...)
	{
		for(int i=0;i<numPix;i++)
		{			
			if(segArray[i]) 
			{
				SAFEDELETE(segArray[i]);
			}
		}
		SAFEDELETE_ARRAY(segArray);
		SAFEDELETE_ARRAY(segment);
		SAFEDELETE_ARRAY(particle);
		return PERR_OUT_OF_MEMORY;
	}


	//Point each valid array element to a cloth segment
	int currentSegNum=0;

	// mike
	//for(int i=0;i<numPix;i++)
	for(i=0;i<numPix;i++)
	{			
		if(segArray[i])
		{
			segArray[i]->seg=&segment[currentSegNum];
			segment[currentSegNum].numParticles=segArray[i]->numParticles;
			segment[currentSegNum].InitPointers();
			currentSegNum++;
		}
	}


	currentSegNum=0;
	int currentParticleNum=0;
	numTriangles=0;

	//Create particles for segments with 4 particles
	for(int y=0;y<h;y++)
	{
		for(int x=0;x<w;x++)
		{	
			int arrPos = x + y*w;
			if(segArray[arrPos])
			{
				ClothSegment * centerseg = segArray[arrPos]->seg;
				ClothSegment * curseg;
				if(centerseg)
				{		
					if(centerseg->numParticles==4)
					{				
						if(x>0 && y>0 && segArray[arrPos-1-w])//0
						{
							curseg = segArray[arrPos-1-w]->seg;
							if(curseg->particle[3]) centerseg->particle[0] = curseg->particle[3];
						}
						if(y>0 && segArray[arrPos-w])//1
						{
							curseg = segArray[arrPos-w]->seg;
							if(curseg->particle[2]) centerseg->particle[0] = curseg->particle[2];
							if(curseg->particle[3]) centerseg->particle[1] = curseg->particle[3];
						}
						if(x<w-1 && y>0 && segArray[arrPos+1-w])//2
						{
							curseg = segArray[arrPos+1-w]->seg;
							if(curseg->particle[2]) centerseg->particle[1] = curseg->particle[2];
						}
						if(x>0 && segArray[arrPos-1])//3
						{
							curseg = segArray[arrPos-1]->seg;
							if(curseg->particle[1]) centerseg->particle[0] = curseg->particle[1];
							if(curseg->particle[3]) centerseg->particle[2] = curseg->particle[3];
						}
						
						//Create any particles that haven't been created already by previous segments
						for(int i=0;i<4;i++)
						{
							if(!centerseg->particle[i]) 
							{
								if(i==0) particle[currentParticleNum].Set(x*segx+offsetX,0,y*segz+offsetZ);
								else if(i==1) particle[currentParticleNum].Set((x+1)*segx+offsetX,0,y*segz+offsetZ);
								else if(i==2) particle[currentParticleNum].Set(x*segx+offsetX,0,(y+1)*segz+offsetZ);
								else particle[currentParticleNum].Set((x+1)*segx+offsetX,0,(y+1)*segz+offsetZ);
								centerseg->particle[i] = &particle[currentParticleNum];
								currentParticleNum++;
							}
						}
						segment[currentSegNum++].Set(centerseg->particle[0],centerseg->particle[1],centerseg->particle[2],centerseg->particle[3]);
						numTriangles+=2;
					}
					else
					{
						currentSegNum++;						
					}
				}
			}
		}
	}

	//Assign particles to segments with only three particles

	// mike
	//for(int y=0;y<h;y++)
	for(y=0;y<h;y++)
	{
		for(int x=0;x<w;x++)
		{	
			int arrPos = x + y*w;
			if(segArray[arrPos])
			{
				ClothSegment * centerseg = segArray[arrPos]->seg;
				ClothSegment * curseg;
				if(centerseg && centerseg->numParticles==3)
				{							
					if(x>0 && y>0 && segArray[arrPos-1-w])//0
					{
						curseg = segArray[arrPos-1-w]->seg;
						if(curseg->particle[3]) centerseg->particle[0] = curseg->particle[3];
					}
					if(y>0 && segArray[arrPos-w])//1
					{
						curseg = segArray[arrPos-w]->seg;
						if(curseg->particle[2]) centerseg->particle[0] = curseg->particle[2];
						if(curseg->particle[3]) centerseg->particle[1] = curseg->particle[3];
					}
					if(x<w-1 && y>0 && segArray[arrPos+1-w])//2
					{
						curseg = segArray[arrPos+1-w]->seg;
						if(curseg->particle[2]) centerseg->particle[1] = curseg->particle[2];
					}
					if(x>0 && segArray[arrPos-1])//3
					{
						curseg = segArray[arrPos-1]->seg;
						if(curseg->particle[1]) centerseg->particle[0] = curseg->particle[1];
						if(curseg->particle[3]) centerseg->particle[2] = curseg->particle[3];
					}
					if(x<w-1 && segArray[arrPos+1])//5
					{
						curseg = segArray[arrPos+1]->seg;
						if(curseg->particle[0]) centerseg->particle[1] = curseg->particle[0];
						if(curseg->particle[2]) centerseg->particle[3] = curseg->particle[2];
					}
					if(x>0 && y<h-1 && segArray[arrPos-1+w])//6
					{
						curseg = segArray[arrPos-1+w]->seg;
						if(curseg->particle[1]) centerseg->particle[2] = curseg->particle[1];
					}
					if(y<h-1 && segArray[arrPos+w])//7
					{
						curseg = segArray[arrPos+w]->seg;
						if(curseg->particle[0]) centerseg->particle[2] = curseg->particle[0];
						if(curseg->particle[1]) centerseg->particle[3] = curseg->particle[1];						
					}
					if(x<w-1 && y<h-1 && segArray[arrPos+1+w])//8
					{
						curseg = segArray[arrPos+1+w]->seg;
						if(curseg->particle[0]) centerseg->particle[3] = curseg->particle[0];
					}					
				}
			}
		}
	}

	currentSegNum=0;

	//Reorder the particles in the array for segments with 3 particles so that particle[3]=0
	//for(int y=0;y<h;y++)
	// mike
	for(y=0;y<h;y++)
	{
		for(int x=0;x<w;x++)
		{	
			int arrPos = x + y*w;
			if(segArray[arrPos])
			{
				ClothSegment * centerseg = segArray[arrPos]->seg;
				if(centerseg)
				{		
					if(centerseg->numParticles==3)
					{
						//Particle[3] must be null					
						if(!centerseg->particle[0])
						{
							//Need to preserve creation order
							centerseg->particle[0]=centerseg->particle[1];
							centerseg->particle[1]=centerseg->particle[3];
							centerseg->particle[3]=0;
						}
						else if(!centerseg->particle[1])
						{
							centerseg->particle[1]=centerseg->particle[3];
							centerseg->particle[3]=0;
						}
						else if(!centerseg->particle[2])
						{
							centerseg->particle[2]=centerseg->particle[3];
							centerseg->particle[3]=0;
						}

						segment[currentSegNum++].Set(centerseg->particle[0],centerseg->particle[1],centerseg->particle[2],centerseg->particle[3]);
						numTriangles+=1;
					}
					else
					{
						currentSegNum++;
					}
				}
			}
		}
	}


	//for(int i=0;i<numPix;i++)
	// mike
	for(i=0;i<numPix;i++)
	{			
		if(segArray[i]) 
		{
			SAFEDELETE(segArray[i]);
		}
	}

	SAFEDELETE_ARRAY(segArray);	


	//Adjust segment weighting, and transform particles by object matrix
	//for(int i=0;i<numParticles;i++)
	// mike
	for(i=0;i<numParticles;i++)
	{
		particle[i].segmentWeighting=1.0f/particle[i].segmentWeighting;
		particle[i].triWeighting=1.0f/particle[i].triWeighting;
		m.rotateAndTransV3(&(particle[i].pos));
		particle[i].lastPos=particle[i].pos;
	}
		
	allocateGraphicsAndInit();

	hasMesh=true;
	return PERR_AOK;
}


//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeCloth(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addCloth(id,CLOTH_PLUGIN(Cloth_Basic)),PCMD_MAKE_CLOTH);
}


DLLEXPORT void generateRectangularCloth(int id, float sx, float sy, int resx, int resy, DWORD doubleSided)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_GENERATE_RECTANGULAR_CLOTH);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Cloth::classID,Cloth_Basic::classID),PCMD_GENERATE_RECTANGULAR_CLOTH);

	Cloth_Basic * clt = reinterpret_cast<Cloth_Basic *>(&(*tmp));
	RETURN_ON_PERR(clt->generateRectangularCloth(sx,sy,resx,resy,(doubleSided>0?true:false)),PCMD_GENERATE_RECTANGULAR_CLOTH);
}

DLLEXPORT void generateClothFromBitmap(int id, int bitmapID, float sx, float sy, DWORD autoTriangles, DWORD doubleSided)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_GENERATE_CLOTH_FROM_BITMAP);
	// mike
	//RETURN_ON_PERR(tmp->checkType(Cloth::classID,Cloth_Basic::classID),PCMD_GENERATE_CLOTH_FROM_BITMAP);

	Cloth_Basic * clt = reinterpret_cast<Cloth_Basic *>(&(*tmp));	
	RETURN_ON_PERR(clt->generateClothFromBitmap(bitmapID,sx,sy,(autoTriangles>0?true:false),(doubleSided>0?true:false)),PCMD_GENERATE_CLOTH_FROM_BITMAP);
}
