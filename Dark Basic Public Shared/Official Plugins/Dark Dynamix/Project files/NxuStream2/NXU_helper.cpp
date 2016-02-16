#include "NXU_helper.h"

#include "NXU_PhysicsExport.h"
#include "NXU_schema.h"
#include "NXU_PhysicsInstantiator.h"
#include "NXU_string.h"
#include "NXU_File.h"
#include "NXU_SchemaStream.h"

#include "NXU_ColladaExport.h"
#include "NXU_ColladaImport.h"
#include "NXU_Geometry.h"
#include "NXU_customcopy.h"

#include <NxVersionNumber.h>
#include <NxPhysics.h>
#include <NxPMap.h>
#include <NxSceneStats.h>
#include <NxStream.h>

#ifdef WIN32
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

#if	NX_SDK_VERSION_NUMBER	>= 240
#include <NxCCDSkeleton.h>
#endif

extern NXU_errorReport	*gErrorReport;

//*************************************************************************
//*************************************************************************
// ** Demonstrates how to save any portion of the SDK one object at a time.
//*************************************************************************
//*************************************************************************

#define NXU_SAVE_PARAMETERS 1
#define NXU_SAVE_GROUP_FLAGS 1
#define NXU_SAVE_MESHES 1
#define NXU_SAVE_SCENE_DESC 1
#define NXU_SAVE_COMPARTMENTS 1
#define NXU_SAVE_ACTORS 1
#define NXU_SAVE_JOINTS 1   // if you have save joints on, but save actors off, then the jointed actors will get added to the collection
#define NXU_SAVE_PAIR_FLAGS 1
#if NX_SDK_VERSION_NUMBER >= 272
#define NXU_SAVE_DOMINANCE_PAIRS 1
#endif
#define NXU_SAVE_FLUIDS 1
#define NXU_SAVE_CLOTHES 1
#define NXU_SAVE_SOFTBODIES 1
#define NXU_SAVE_EFFECTORS 1
#define NXU_SAVE_MATERIALS 1
#define NXU_SAVE_FORCE_FIELDS 1


static void saveGroupFlags(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
 	for (NxU32 i = 0; i < 32; ++i )
 	{
 		for ( NxU32 j = i; j <32; ++j )
 		{
 			bool on = scene->getGroupCollisionFlag( i, j );
			if ( !on )
			  NXU::addGroupCollisionFlag(*c,i,j,on);
 		}
 	}

#if NX_SDK_VERSION_NUMBER >= 260
  NxU32 gcount = scene->getNbActorGroupPairs();
  if ( gcount )
  {
  	NxActorGroupPair *agp = new NxActorGroupPair[gcount];
  	NxU32 userIterator=0;
  	NxU32 count = scene->getActorGroupPairArray(agp, gcount, userIterator );
  	assert( count == gcount );
  	for (NxU32 i=0; i<count; i++)
  	{
  		NxActorGroupPair &p = agp[i];
			NXU::addActorGroupFlag(*c, p.group0, p.group1, p.flags );
  	}
  	delete agp;
  }

	bool filter 		= scene->getFilterBool();
	::NxFilterOp op0,op1,op2;
	scene->getFilterOps(op0,op1,op2);

	::NxGroupsMask m1 = scene->getFilterConstant0();
	::NxGroupsMask m2 = scene->getFilterConstant1();


	NXU::setFilterOps(*c,filter,op0,op1,op2,m1,m2);

#endif
}

static void saveParameters(NXU::NxuPhysicsCollection *c,NxPhysicsSDK *sdk)
{
	NXU::addPhysicsSDK(*c,*sdk);
	for	(int i = NX_PENALTY_FORCE; i < NX_PARAMS_NUM_VALUES; i++)
	{
		NxParameter	p	=	(NxParameter)i;
		NxReal v = sdk->getParameter(p);
		NXU::addParameter(*c,p,v);
	}
}

static void addShapeMesh(NXU::NxuPhysicsCollection *c,NxShape *shape)
{
	switch ( shape->getType() )
	{
		case NX_SHAPE_CONVEX:
			if ( 1 )
			{
				NxConvexShape *s = (NxConvexShape *) shape;
				NxConvexMesh &mesh = s->getConvexMesh();
				NXU::addConvexMesh(*c,mesh);
			}
			break;
		case NX_SHAPE_MESH:
			if ( 1 )
			{
				NxTriangleMeshShape *s = (NxTriangleMeshShape *) shape;
				NxTriangleMesh &mesh = s->getTriangleMesh();
				NXU::addTriangleMesh(*c,mesh);
			}
			break;
		case NX_SHAPE_HEIGHTFIELD:
			if ( 1 )
			{
				NxHeightFieldShape *s = (NxHeightFieldShape *) shape;
				NxHeightField &mesh = s->getHeightField();
				NXU::addHeightField(*c,mesh);
			}
			break;
		default: /*nothing*/ break;
  }

	NxCCDSkeleton *skeleton = shape->getCCDSkeleton();
	if ( skeleton )
	{
		NXU::addCCDSkeleton(*c,*skeleton,0);
	}


}

static void saveMeshes(NXU::NxuPhysicsCollection *c,NxScene *scene)
{

  NxU32 nbActors = scene->getNbActors();
  NxActor **alist = scene->getActors();
  for (NxU32 i=0; i<nbActors; i++)
  {
  	NxActor *a = alist[i];
  	NxU32 nbShapes = a->getNbShapes();
   	if ( nbShapes )
   	{
   		NxShape ** slist = (NxShape **)a->getShapes();
   		for (NxU32 j=0; j<nbShapes; j++)
   		{
   			NxShape *s = slist[j];
   			addShapeMesh(c,s);
    	}
    }
  }

  NxPhysicsSDK &sdk = scene->getPhysicsSDK();
#if NX_USE_CLOTH_API
  NxU32 nbClothMeshes = sdk.getNbClothMeshes();
  if ( nbClothMeshes )
  {
  	NxClothMesh **clist = sdk.getClothMeshes();
  	for (NxU32 i=0; i<nbClothMeshes; i++)
  	{
  		NxClothMesh *cmesh = clist[i];
  		addClothMesh(*c,*cmesh);
  	}
  }
#endif

#if NX_USE_SOFTBODY_API
  NxU32 nbSoftBodies = sdk.getNbSoftBodyMeshes();
  if ( nbSoftBodies )
  {
  	NxSoftBodyMesh **slist = sdk.getSoftBodyMeshes();
  	for (NxU32 i=0; i<nbSoftBodies; i++)
  	{
  		NxSoftBodyMesh *sb = slist[i];
  		addSoftBodyMesh(*c,*sb);
  	}
  }
#endif

}

static void saveSceneDesc(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NXU::addScene(*c,*scene); // save out the scene descriptor and sets this as the 'current' scene we are adding to.
}

static void	saveActors(NXU::NxuPhysicsCollection *c,NxScene *scene, NXU_userNotify *un = 0)
{
	NxU32 nbActors = scene->getNbActors();
	if ( nbActors )
	{
		NxActor **alist = scene->getActors();
		for (NxU32 i=0; i<nbActors; i++)
		{
			const char* pUserProperties = 0;

			if (un)
				un->NXU_notifySaveActor(alist[i], &pUserProperties);

			NXU::addActor(*c,*alist[i], pUserProperties);
		}
	}
}

static void			saveJoints(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	jointCount = scene->getNbJoints();
	if (jointCount)
	{
		NxArray< NxJoint * > joints;

		scene->resetJointIterator();
		for	(NxU32 i = 0;	i	<	jointCount;	++i)
		{
			NxJoint	*j = scene->getNextJoint();
			joints.push_back(j);
		}
		for (NxU32 i=0; i<jointCount; i++)
		{
			NxU32 index = (jointCount-1)-i;
			NxJoint *j = joints[index];
			NXU::addJoint(*c,*j);
		}
	}
}

static void	savePairFlags(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	pairCount	=	scene->getNbPairs();
	if (pairCount)
	{
		NxPairFlag *array	=	new	NxPairFlag[pairCount];
		memset(array,	0, pairCount *sizeof(NxPairFlag));
		scene->getPairFlagArray(array, pairCount);
		for	(NxU32 i = 0;	i	<	pairCount; ++i)
		{
			addPairFlag(*c,array[i]);
		}

		delete array;
	}
}

#if NX_SDK_VERSION_NUMBER >= 272
static void	saveDominancePairs(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	for (NxU16 i = 0; i < 32; i++)
	{
		for (NxU16 j = 0; j <= i; j++)
		{
			NxConstraintDominance dominance = scene->getDominanceGroupPair(i, j);
			//NxU32 temp = (i << 16) | j;
			addDominancePair(*c, i, j, dominance);
		}
	}
}
#endif

#if NX_USE_FLUID_API
static void      saveFluids(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	fluidCount = scene->getNbFluids();
	if (fluidCount)
	{
		NxFluid	**fluidList	=	scene->getFluids();
		for	(NxU32 i = 0;	i	<	fluidCount;	++i)
		{
			addFluid(*c,*fluidList[i]);
		}
	}
}
#endif

#if NX_USE_CLOTH_API
static void      saveClothes(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	clothCount = scene->getNbCloths();
	NxCloth	**clothList	=	scene->getCloths();
	for	(NxU32 i = 0;	i	<	clothCount;	i++)
	{
		addCloth(*c,*clothList[i]);
	}
}
#endif

#if NX_USE_SOFTBODY_API
static void      saveSoftBodies(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	softBodyCount = scene->getNbSoftBodies();
	NxSoftBody	**softBodyList	=	scene->getSoftBodies();
	for	(NxU32 i = 0;	i	<	softBodyCount;	i++)
	{
		addSoftBody(*c,*softBodyList[i]);
	}
}
#endif

static void		  saveEffectors(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	// now write effectors
	NxU32	effectorCount	=	scene->getNbEffectors();
	if (effectorCount)
	{
		NxArray< NxEffector * > effectors;
		scene->resetEffectorIterator();
		for	(NxU32 i = 0;	i	<	effectorCount; ++i)
		{
			NxEffector *e	=	scene->getNextEffector();
			effectors.push_back(e);
		}
		for (NxU32 i=0; i<effectorCount; ++i)
		{
			NxU32 index = (effectorCount-1)-i;
			NxEffector *e = effectors[index];
			NXU::addEffector(*c,*e);
		}
	}
}

#if NX_SDK_VERSION_NUMBER >= 280
static void saveForceFieldScaleTable(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	// find max table entries
	NxForceFieldVariety maxVar = scene->getHighestForceFieldVariety();
	NxForceFieldMaterial maxMat = scene->getHighestForceFieldMaterial();

	for(NxU16 var = 0; var <= maxVar; var++)
	{
		for(NxU16 mat = 0;  mat <= maxMat ; mat++)
		{
			NxReal scale = scene->getForceFieldScale(var,mat);
			if(scale != 1.0f)
				NXU::addForceFieldScale(*c, var, mat, scale);
		}
	}
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
static void saveForceFieldShapeGroups(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32 groupsCount =	scene->getNbForceFieldShapeGroups();
	scene->resetForceFieldShapeGroupsIterator();
	while(groupsCount--)
	{
		NxForceFieldShapeGroup* group = scene->getNextForceFieldShapeGroup();
		NXU::addForceFieldShapeGroup(*c, *group);
	}
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
static void saveForceFieldLinearKernels(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32 kernelCount =	scene->getNbForceFieldLinearKernels();
	scene->resetForceFieldLinearKernelsIterator();
	while(kernelCount--)
	{
		NxForceFieldLinearKernel* kernel = scene->getNextForceFieldLinearKernel();
		NXU::addForceFieldLinearKernel(*c, *kernel);
	}
}
#endif

#if NX_SDK_VERSION_NUMBER >= 272
static void		  saveForceFields(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
#if NX_SDK_VERSION_NUMBER >= 280
	// save the scaling table
	saveForceFieldScaleTable(c, scene);
	// save all groups
	saveForceFieldShapeGroups(c, scene);
	// save all linear kernels
	saveForceFieldLinearKernels(c, scene);
#endif
	// now write force fields
	NxU32	fieldCount	=	scene->getNbForceFields();
	if (fieldCount)
	{
		NxForceField** fields = scene->getForceFields();
		for	(NxU32 i = 0;	i	<	fieldCount; ++i)
		{
			NXU::addForceField(*c,*(fields[i]));
		}
	}
}
#endif

static void		  saveMaterials(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	mcount = scene->getNbMaterials();
	if (mcount)
	{
		NxMaterial **materials = new NxMaterial	*[mcount];
		memset(materials,	0, sizeof(NxMaterial*) * mcount);
		NxU32	iterator = 0;
		scene->getMaterialArray(&materials[0],	mcount,	iterator);
		for (NxU32 i=0; i<mcount; i++)
		{
		  NXU::addMaterial(*c, *materials[i] );
		}
		delete []	materials;
	}
}

#if NX_SDK_VERSION_NUMBER >= 260
static void		  saveCompartments(NXU::NxuPhysicsCollection *c,NxScene *scene)
{
	NxU32	nb = scene->getNbCompartments();
	if ( nb )
	{
		NxCompartment **compartments = new NxCompartment	*[nb];
		memset(compartments,	0, sizeof(NxCompartment*) * nb);
		NxU32	iterator = 0;
		scene->getCompartmentArray(&compartments[0],	nb,	iterator);
		for (NxU32 i=0; i<nb; i++)
		{
		  NXU::addCompartment(*c, *compartments[i] );
		}
		delete []	compartments;
	}
}
#endif


namespace	NXU
{


extern bool  gSaveDefaults;

bool gSaveCooked=false;

static bool gAutoGenerateSkeletons=false;
static NxReal gShrinkRatio = 0.9f;
static NxU32  gMaxSkeletonVertices = 16;
static NxuPhysicsCollection *gSkeletons=0; // used just to create CCD skeletons on the fly.

static bool isEmptyString(const char *str)
{
	bool ret = true;

	if ( str && strlen(str) ) ret = false;

  return ret;
}


void setId(NxuPhysicsCollection *c,const char *id,const char *userProperties,bool replaceId)
{
	if ( id )
	{
		if ( replaceId || isEmptyString(c->mId) )
		{
      c->mId = getGlobalString(id);
    }
  }

  if ( userProperties )
  {
  	if ( replaceId || isEmptyString(c->mUserProperties) )
  	{
      c->mUserProperties = getGlobalString(userProperties);
    }
  }

  c->mSdkVersion = NX_SDK_VERSION_NUMBER;
  c->mNxuVersion = NXUSTREAM_VERSION;
}

bool saveCOLLADA(NxuPhysicsCollection	*c,	NXU_FILE *fph,const	char *asset_name)
// save	the	contents of	this collection	in the COLLADA format.
{
	bool ret = false;

	assert(c);

	if (c)
	{
		ret	=	saveColladaFile(c, fph,	asset_name);
	}

	return ret;
}


bool saveCOLLADA(NxuPhysicsCollection	*c,	const	char *fname)
// save	the	contents of	this collection	in the COLLADA format.
{
	bool ret = false;

	NXU_FILE *fph	=	nxu_fopen(fname, "wb");
	if (fph)
	{
		ret	=	saveCOLLADA(c, fph,	fname);
		nxu_fclose(fph);
	}
	else
	{
		reportError("Failed to	open file	'%s' for write access.	COLLADA	file not written.",	fname);
	}

	return ret;
}


NxuPhysicsCollection *loadCollection(const char	*fname,	NXU_FileType type,void *mem,int len)
{
	NxuPhysicsCollection *ret	=	0;

	NXU_FILE *fph = nxu_fopen(fname,"rb",mem,len);

	if ( fph )
	{
		nxu_fclose(fph);
		if (type ==	FT_COLLADA)
		{
			ret	=	colladaImport(fname,mem,len);
		}
		else
		{
			// insert load code here
			bool binary = type == FT_BINARY;

			SchemaStream ss(fname,binary,"rb",mem,len);
			if ( ss.isValid() )
			{
				ret = new NxuPhysicsCollection;
				ret->load(ss);
				if ( !ss.isValid() )
				{
          reportError("Failed to load collection '%s'", fname );
					delete ret;
					ret = 0;
				}
        else
        {
      		if ( gAutoGenerateSkeletons )
      		{
      			createCCDSkeletons(*ret,gShrinkRatio,gMaxSkeletonVertices);
      		}
        }
			}

		}
	}
	else
	{
		reportError("Failed to open physics data file '%s' for read access", fname );
	}

#if 0
	if ( ret )
	{
		NXU::saveCollection(ret,"debug.xml",FT_XML);
	}
#endif

	return ret;
}

bool saveCollection(NxuPhysicsCollection *c, const char	*fname,	NXU_FileType type, bool	save_defaults, bool	cook)
{

	bool ret     = false;

  if ( c )
  {

    setId(c,fname,0,false);

		if ( gAutoGenerateSkeletons )
		{
			createCCDSkeletons(*c,gShrinkRatio,gMaxSkeletonVertices);
		}

  	gSaveDefaults= save_defaults;
  	gSaveCooked  = cook;

  	if (type ==	FT_COLLADA)
  	{
  		ret = saveCOLLADA(c, fname);
  	}
  	else
  	{
		if (type ==	FT_BINARY)
		{
			gSaveDefaults = true;
			SchemaStream ss(fname,true,"wb",0,0);
			if ( ss.isValid() )
			{
				c->store(ss);
				ret = true;
			}
		}
		else
		{
			SchemaStream ss(fname,false,"wb",0,0);
			if ( ss.isValid() )
			{
				c->store(ss);
				ret = true;
			}
		}
  	}
  }


	return ret;
}

bool instantiateCollection(NxuPhysicsCollection	*c,	NxPhysicsSDK &sdk, NxScene *scene,const NxMat34 *rootNode,	NXU_userNotify *ur)
{
	bool ret = false;

	if ( c )
	{
  	NxuPhysicsInstantiator Instantiator(c);
	  Instantiator.instantiate(sdk,	ur,	scene, rootNode);
	  ret = true;
	}

	return ret;
}


bool instantiateScene(NxuPhysicsCollection	*c,const char *sceneName,NxPhysicsSDK &sdk, NxScene *scene, NxMat34 *rootNode,	NXU_userNotify *un)
{
	bool ret = false;

  if ( c )
  {
  	NxuPhysicsInstantiator Instantiator(c);

    NxSceneInstanceDesc nsi;

    nsi.mSceneName = getGlobalString(sceneName);

#if 0 // todo todo todo
		NxU32 index;
  	NxSceneDesc *sdesc	 =	c->locateSceneDesc(nsi.sceneName,	index);
 		NxSceneInstance *si =  c->locateSceneInstance(nsi.sceneName,index);
 		if ( sdesc || si )
 		{
			Instantiator.setInstanceScene(scene);
		  Instantiator.instantiateSceneInstance(&nsi,sdk, un, rootNode, 1 );
		  ret = true;
		}
#endif
  }

  return ret;
}

void releaseCollection(NxuPhysicsCollection	*c)
{
	delete c;
}


bool coreDump(NxPhysicsSDK *sdk, const char	*fname,	NXU_FileType type, bool	save_default,	bool cook, NXU_userNotify *un)
{
	bool ret = false;

	NxuPhysicsCollection *c	=	extractCollectionSDK(sdk, 0, un);
	if (c)
	{
		ret	=	saveCollection(c,	fname, type, save_default, cook);
		releaseCollection(c);
	}

	return ret;
}

void releasePersistentMemory(void) //	do this	when you exit	the	application	or do	a	reset	of the Physics SDK
{
	NX_DELETE_SINGLE(gSkeletons);
	NXU::releaseGlobalStrings();
	NXU::releaseGlobalInstances();
}


NxuPhysicsCollection *extractCollectionSDK(NxPhysicsSDK	*sdk,const char *collectionId, NXU_userNotify *un)// extract the contents	of the SDK into	this collection.
{

	NxuPhysicsCollection *c =	createCollection(collectionId);

	#if NXU_SAVE_PARAMETERS
	saveParameters(c,sdk);
	#endif

	NxU32	scount = sdk->getNbScenes();
	for	(NxU32 i = 0;	i	<	scount;	i++)
	{
		NxScene	*scene = sdk->getScene(i);
		addEntireScene(*c,*scene, 0, 0, un);
	}

#if 0 // debugging code
	NxMat34 root;
	root.id();

	addToCurrentSceneInstance(*c,"SceneInstances", "", root, false, true, "UserData" );


	for (int i=0; i<4; i++)
	{
    root.t.x+=10.0f;
    char scratch[512];
    sprintf(scratch,"SceneInstance%d", i+1);
    bool ignorePlane = true;
    if ( i== 0 ) ignorePlane = false;
	  addSceneInstance(*c,scratch, "Scene_0", root, false, true, "More User Data" );
	}

	closeCurrentSceneInstance(*c);

	addSceneInstance(*c,"SceneInstances2", "SceneInstances", root, false, true, "UserData" );
#endif

	return c;

}

NxuPhysicsCollection *extractCollectionScene(NxScene *scene,const char *collectionId,const char *sceneId) // extract the contents	a	single scene into	this collection.
{

	NxuPhysicsCollection *c = createCollection(collectionId);

	if ( c && scene )
	{
		addEntireScene(*c,*scene,sceneId);
	}

	return c;
}

NxuPhysicsCollection *createCollection(const char *collectionId,const char *userProperties)	// create	an empty physics collection.
{
	NxuPhysicsCollection *ret	=	new	NxuPhysicsCollection;

  setId(ret,collectionId,userProperties,true);

	return ret;
}

bool addParameter(NxuPhysicsCollection &c,              unsigned int param,float value)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
 	ret = exporter.Write( (NxParameter) param, (NxReal) value );

	return ret;
}

bool addScene(NxuPhysicsCollection &c,                  NxScene &s,const char *userProperties,const char *sceneId)
{
	bool ret = false;


	::NxSceneDesc desc;
	s.saveToDesc(desc);

	desc.groundPlane = false; // to avoid duplicate planes on load.

 	NxuPhysicsExport exporter(&c);
 	ret = exporter.Write(&desc, getGlobalString(userProperties), getGlobalString(sceneId) );

	return ret;
}

bool addEntireScene(NxuPhysicsCollection &c,                  NxScene &scene,const char *userProperties,const char *sceneId, NXU_userNotify *un)
{
	bool ret = false;


	#if NXU_SAVE_MESHES
	saveMeshes(&c,&scene);
	#endif

	#if NXU_SAVE_SCENE_DESC
	saveSceneDesc(&c,&scene);					 // save the scene descriptor
	#endif

	#if NXU_SAVE_GROUP_FLAGS
	saveGroupFlags(&c,&scene);
	#endif

	#if NXU_SAVE_MATERIALS
	saveMaterials(&c,&scene);          // save the materials
	#endif

	#if NXU_SAVE_COMPARTMENTS && NX_SDK_VERSION_NUMBER >= 260
	saveCompartments(&c,&scene);
	#endif

	#if NXU_SAVE_ACTORS
	saveActors(&c,&scene, un);            // save the actors
	#endif

	#if NXU_SAVE_JOINTS
	saveJoints(&c,&scene);             // save the joints
	#endif

	#if NXU_SAVE_PAIR_FLAGS	// save body pair flags, collision filters, etc. etc.
	savePairFlags(&c,&scene);
	#endif

	#if NXU_SAVE_DOMINANCE_PAIRS
	saveDominancePairs(&c,&scene);
	#endif

	#if NXU_SAVE_FLUIDS && NX_USE_FLUID_API
	saveFluids(&c,&scene);    // save the fluids
	#endif

	#if NXU_SAVE_CLOTHES && NX_USE_CLOTH_API
	saveClothes(&c,&scene);    // save the clothes
	#endif

	#if NXU_SAVE_SOFTBODIES && NX_USE_SOFTBODY_API
	saveSoftBodies(&c,&scene);
	#endif

	#if NXU_SAVE_EFFECTORS
	saveEffectors(&c,&scene); // save the effectors.
	#endif

	#if NXU_SAVE_FORCE_FIELDS && NX_SDK_VERSION_NUMBER >= 272
	saveForceFields(&c,&scene);
	#endif

	return ret;
}

bool addActor(NxuPhysicsCollection &c,                  NxActor &a,const char *userProperties,const char *actorId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&a, getGlobalString(userProperties), getGlobalString(actorId) );

	return ret;
}

bool addJoint(NxuPhysicsCollection &c,                  NxJoint &j,const char *userProperties,const char *jointId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&j, getGlobalString(userProperties), getGlobalString(jointId) );

	return ret;
}

bool addCloth(NxuPhysicsCollection &c,                  NxCloth &cloth,const char *userProperties,const char *clothId)
{
	bool ret = false;

#if NX_USE_CLOTH_API
  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&cloth, getGlobalString(userProperties), getGlobalString(clothId) );
#endif

	return ret;
}

bool addFluid(NxuPhysicsCollection &c,                  NxFluid &f,const char *userProperties,const char *fluidId)
{
	bool ret = false;

#if NX_USE_FLUID_API
  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&f, getGlobalString(userProperties), getGlobalString(fluidId) );
#endif

	return ret;
}

bool addMaterial(NxuPhysicsCollection	&c,	              NxMaterial &m,const char *userProperties,const char *materialId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&m, getGlobalString(userProperties), getGlobalString(materialId) );

	return ret;
}

bool addConvexMesh(NxuPhysicsCollection &c,             NxConvexMesh &mesh,const char *userProperties,const char *convexId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&mesh, getGlobalString(userProperties), getGlobalString(convexId) );

	return ret;
}

bool addTriangleMesh(NxuPhysicsCollection &c,           NxTriangleMesh &mesh,const char *userProperties,const char *meshId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&mesh, getGlobalString(userProperties), getGlobalString(meshId) );

	return ret;
}

bool addHeightField(NxuPhysicsCollection &c,            NxHeightField &mesh,const char *userProperties,const char *heightFieldId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&mesh, getGlobalString(userProperties), getGlobalString(heightFieldId) );

	return ret;
}

bool addCCDSkeleton(NxuPhysicsCollection &c,            NxCCDSkeleton &skel,const char *userProperties,const char *skeletonId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  const char *name = exporter.Write(&skel, getGlobalString(userProperties), getGlobalString(skeletonId) );
	if ( name )
		ret = true;

	return ret;
}

bool addEffector(NxuPhysicsCollection &c,NxEffector &e,const char *userProperties,const char *effectorId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&e, getGlobalString(userProperties), getGlobalString(effectorId));

	return ret;
}

#if NX_SDK_VERSION_NUMBER >= 270
bool addForceField(NxuPhysicsCollection &c,NxForceField &f,const char *userProperties,const char *forceFieldId)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
	ret = exporter.Write(&f, getGlobalString(userProperties), getGlobalString(forceFieldId));

	return ret;
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
bool addForceFieldScale(NxuPhysicsCollection &c, NxForceFieldVariety v, NxForceFieldMaterial mat, NxReal scale)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
	ret = exporter.addForFieldScaleTableEntry(v, mat, scale);
	
	return ret;
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
bool addForceFieldShapeGroup(NxuPhysicsCollection &c,NxForceFieldShapeGroup &g,const char *userProperties,const char *forceFieldId)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
	ret = exporter.Write(&g, getGlobalString(userProperties), getGlobalString(forceFieldId));

	return ret;
}
#endif

#if NX_SDK_VERSION_NUMBER >= 280
bool addForceFieldLinearKernel(NxuPhysicsCollection &c,NxForceFieldLinearKernel &k,const char *userProperties,const char *forceFieldId)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
	ret = exporter.Write(&k, getGlobalString(userProperties), getGlobalString(forceFieldId));

	return ret;
}
#endif

bool addPairFlag(NxuPhysicsCollection &c,               NxPairFlag &f,const char *userProperties,const char *pairFlagId)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&f, getGlobalString(userProperties), getGlobalString(pairFlagId) );

	return ret;
}

#if NX_SDK_VERSION_NUMBER >= 272
bool addDominancePair(NxuPhysicsCollection &c,       	NxU16 group0, NxU16 group1, NxConstraintDominance &f)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);

	ret = exporter.addDominancePair(group0,group1,f);

	return ret;
}
#endif

void setErrorReport(NXU_errorReport *er)
{
	gErrorReport = er;
}

void * saveCollectionToMemory(NxuPhysicsCollection *c,const char	*collectionId,NXU_FileType type, bool	save_defaults, bool	cook,void *mem,int len,size_t &outputLength)
{
	void * ret = 0;

  if ( c )
  {
    setId(c,collectionId,0,true);

		if ( gAutoGenerateSkeletons )
		{
			createCCDSkeletons(*c,gShrinkRatio,gMaxSkeletonVertices);
		}

  	gSaveDefaults = save_defaults;
  	gSaveCooked  = cook;

  	if (type ==	FT_COLLADA)
  	{

  		NXU_FILE *fph	=	nxu_fopen(collectionId, "wmem", mem, len );
  		if (fph)
  		{
  		  saveColladaFile(c, fph,	collectionId);

  			ret = nxu_getMemBuffer( fph, outputLength );

  			nxu_fclose(fph);
  		}
  	}
  	else
  	{

  		if (type ==	FT_BINARY)
  		{
        gSaveDefaults = true;
  			SchemaStream ss(collectionId,true,"wmem",mem,len);
        if ( ss.isValid() )
        {
    			c->store(ss);
          if ( ss.isValid() )
    	  	  ret = ss.getMemBuffer(outputLength );
        }
  		}
  		else
  		{
  			SchemaStream ss(collectionId,false,"wmem",mem,len);
        if ( ss.isValid() )
        {
    			c->store(ss);
          if ( ss.isValid() )
          {
      		  ret = ss.getMemBuffer(outputLength );
          }
        }
  		}
  	}
  }

  return ret;
}

void releaseCollectionMemory(void *mem)
{
	delete (char *) mem;
}

bool addGroupCollisionFlag(NxuPhysicsCollection &c,NxU32 group1,NxU32 group2,bool enable)
{
	bool ret = false;


  NxuPhysicsExport exporter(&c);
  ret = exporter.addGroupCollisionFlag(group1,group2,enable);

  return ret;
}

bool addActorGroupFlag(NxuPhysicsCollection &c,NxU32	group1,NxU32 group2,NxU32 flags)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);

  ret = exporter.addActorGroupFlag(group1,group2,flags);

  return ret;
}

bool setFilterOps(NxuPhysicsCollection &c,bool active,::NxFilterOp op0,::NxFilterOp op1,::NxFilterOp op2,const ::NxGroupsMask &filterConstant0,const ::NxGroupsMask &filterConstant1)
{
	bool ret = false;


  NxuPhysicsExport exporter(&c);

  ret = exporter.setFilterOps(active,op0,op1,op2,filterConstant0,filterConstant1);

  return ret;
}


bool addSceneInstance(NxuPhysicsCollection &c,const char *sceneId,const char *sceneName,const NxMat34 &rootNode,bool ignorePlane,const char *userProperties)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);

	ret = exporter.addSceneInstance(sceneId,sceneName,rootNode,ignorePlane,userProperties);

  return ret;
}

bool addToCurrentSceneInstance(NxuPhysicsCollection &c,const char *instanceId,const char *sceneName,const NxMat34 &rootNode,bool ignorePlane,const char *userProperties)
{
	bool ret = false;

  NxuPhysicsExport exporter(&c);

	ret = exporter.addToCurrentSceneInstance(instanceId,sceneName,rootNode,ignorePlane,userProperties);

  return ret;
}

bool closeCurrentSceneInstance(NxuPhysicsCollection &c)
{
	if ( c.mCurrentSceneInstance )
	{
		c.mCurrentSceneInstance = c.mCurrentSceneInstance->mParent;
	}
	return true;
}


NxU32 getSceneCount(NxuPhysicsCollection &c)
{
	return c.mScenes.size();
}

const char * getSceneName(NxuPhysicsCollection &c,NxU32 sceneNo)
{
	const char * ret = 0;
	NxU32 scount = getSceneCount(c);
	if ( sceneNo >= 0 && sceneNo < scount )
	{
		ret = c.mScenes[sceneNo]->mId;
	}
	return ret;
}

NxU32 getSceneInstanceCount(NxuPhysicsCollection &c)
{
	return c.mSceneInstances.size();
}

const char * getSceneInstanceName(NxuPhysicsCollection &c,NxU32 sceneNo)
{
	const char * ret = 0;
	NxU32 scount = getSceneInstanceCount(c);
	if ( sceneNo >= 0 && sceneNo < scount )
	{
		ret = c.mSceneInstances[sceneNo]->mId;
	}
	return ret;
}

const char * getSceneInstanceSceneName(NxuPhysicsCollection &c,NxU32 sceneNo)
{
	const char * ret = 0;
	NxU32 scount = getSceneInstanceCount(c);
	if ( sceneNo >= 0 && sceneNo < scount )
	{
		ret = c.mSceneInstances[sceneNo]->mSceneName;
	}
	return ret;
}


const char *            getCollectionName(NxuPhysicsCollection &c)
{
	const char * ret = 0;

  ret = c.mId;

  return ret;
}

void setUseClothActiveState(bool state)
{
#if NX_SDK_VERSION_NUMBER >= 260
  gUseClothActiveState = state;
#endif
}


void setUseSoftBodyActiveState(bool state)
{
#if NX_SDK_VERSION_NUMBER >= 270
  gUseSoftBodyActiveState = state;
#endif
}


bool 										addPhysicsSDK(NxuPhysicsCollection &c,NxPhysicsSDK &sdk)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
 	ret = exporter.Write(&sdk);

	return ret;

}

#if NX_SDK_VERSION_NUMBER >= 260
/**
\brief Adds a compartment to the physics collection

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the compartment to be added to the current scene in this collection.
\return true if successful.
*/
bool                    addCompartment(NxuPhysicsCollection &c,NxCompartment &cmp,const char *compartmentId)
{
	bool ret = false;

	NxuPhysicsExport exporter(&c);
  const char *name = exporter.Write(&cmp,compartmentId);
  if ( name )
    ret = true;

	return ret;

}
#endif


NxU32 createCCDSkeletons(NxuPhysicsCollection &c,NxReal shrink,NxU32 maxv)
{
	NxU32 ret = 0;

	for (NxU32 i=0; i<c.mScenes.size(); i++)
	{

		NxSceneDesc *sd = c.mScenes[i];

		for (NxU32 j=0; j<sd->mActors.size(); j++)
		{

			NxActorDesc *ad = sd->mActors[j];

			if ( ad->mHasBody ) // only for dynamic actors
			{
  			for (NxU32 k=0; k<ad->mShapes.size(); k++)
	  		{
		  		NxShapeDesc *shape = ad->mShapes[k];

			  	// if it already has a CCD skeleton we leave it alone..
  				if ( shape->mCCDSkeleton && strlen(shape->mCCDSkeleton) )
  				{
						// skeleton already defined...
					}
					else
					{



  					NxuGeometry g;
  					NxVec3 dimensions(0,0,0);
  					NxReal radius=0;
  					NxReal height=0;
  					NxConvexMeshDesc *cmesh=0;

  					bool   found = false;

  					switch ( shape->mType )
  					{

  						case SC_NxBoxShapeDesc:
  							if ( 1 )
  							{
  								bool found = false;
   								NxBoxShapeDesc *b = (NxBoxShapeDesc *) shape;
  								for (NxU32 i=0; i<c.mSkeletons.size(); i++)
  								{
  									NxCCDSkeletonDesc *sd = c.mSkeletons[i];
  									if ( sd->mPrimitive == SC_NxBoxShapeDesc && sd->mDimensions == b->dimensions )
  									{
  										shape->mCCDSkeleton = sd->mId;
  										found = true;
  										break;
  									}
  								}
  								if ( !found )
  								{
    								createBox(b->dimensions,g,0,shrink,maxv);
    								dimensions = b->dimensions;
    							}
  							}
  							break;

  						case SC_NxSphereShapeDesc:
  							if ( 1 )
  							{
  								bool found = false;
   								NxSphereShapeDesc *b = (NxSphereShapeDesc *) shape;
  								for (NxU32 i=0; i<c.mSkeletons.size(); i++)
  								{
  									NxCCDSkeletonDesc *sd = c.mSkeletons[i];
  									if ( sd->mPrimitive == SC_NxSphereShapeDesc && sd->mRadius == b->radius )
  									{
  										shape->mCCDSkeleton = sd->mId;
  										found = true;
  										break;
  									}
  								}
  								if ( !found )
  								{
    								createSphere(b->radius,g,0,16,shrink,maxv);
    								radius = b->radius;
    							}
  							}
  							break;

  						case SC_NxCapsuleShapeDesc:
  							if ( 1 )
  							{
  								bool found = false;
   								NxCapsuleShapeDesc *b = (NxCapsuleShapeDesc *) shape;
  								for (NxU32 i=0; i<c.mSkeletons.size(); i++)
  								{
  									NxCCDSkeletonDesc *sd = c.mSkeletons[i];
  									if ( sd->mPrimitive == SC_NxCapsuleShapeDesc && sd->mRadius == b->radius && sd->mHeight == b->height )
  									{
  										shape->mCCDSkeleton = sd->mId;
  										found = true;
  										break;
  									}
  								}
  								if ( !found )
  								{
    								createCapsule(b->radius,b->height,g,0,1,16,shrink,maxv);
    								radius = b->radius;
    								height = b->height;
    							}
  							}
  							break;

  						case SC_NxConvexShapeDesc:
  							if ( 1 )
  							{
  								NxConvexShapeDesc *cv = (NxConvexShapeDesc *) shape;
   								CustomCopy cc(&c,0);
   								cmesh = cc.getConvexMeshDescFromName(cv->mMeshData);
 	  							if ( cmesh && cmesh->mPoints.size() )
 		  						{
										if ( cmesh->mCCDSkeleton )
										{
											shape->mCCDSkeleton = cmesh->mCCDSkeleton->mId;
											found = true;
										}
										else
										{
 			  						  createHull( cmesh->mPoints.size(), &cmesh->mPoints[0].x, g, 0,shrink, maxv);
										}
  				  			}
  							}
  							break;
						default: /*nothing*/ break;
  					}
  					if ( !found && g.mVcount ) // if not found already and we have a geometry.
  					{
  						::NxSimpleTriangleMesh stm;
  						stm.numVertices  = g.mVcount;
  						stm.numTriangles = g.mTcount;
  						stm.points       = g.mVertices;
  						stm.triangles    = g.mIndices;
  						stm.pointStrideBytes = sizeof(float)*3;
  						stm.triangleStrideBytes = sizeof(unsigned int)*3;
  						stm.flags = 0;

  						NxCCDSkeletonDesc *skel = new NxCCDSkeletonDesc;
  						char scratch[512];
  						sprintf(scratch,"AutoCCD_%d", ret );
  						shape->mCCDSkeleton = skel->mId = getGlobalString(scratch);

  						CustomCopy cc(&c,0);

  						skel->copyFrom(stm,cc);

							c.mSkeletons.push_back(skel); // add the skeleton

							skel->mPrimitive = shape->mType;

    					switch ( shape->mType )
    					{
    						case SC_NxBoxShapeDesc:
    							skel->mDimensions = dimensions;
    							break;
    						case SC_NxSphereShapeDesc:
    							skel->mRadius = radius;
    							break;
    						case SC_NxCapsuleShapeDesc:
    							skel->mRadius = radius;
    							skel->mHeight = height;
    							break;
    						case SC_NxConvexShapeDesc:
    							if ( cmesh )
    							{
    								cmesh->mCCDSkeleton = skel;
    							}
    							break;
							default: /*nothing*/ break;
    					}


  						ret++;

  					}
  				}
	  		}
	  	}
		}
	}

  return ret;
}

void	setAutoGenerateCCDSkeletons(bool state,NxReal shrink,NxU32 maxV)
{
	gAutoGenerateSkeletons = state;
	gShrinkRatio = shrink;
	gMaxSkeletonVertices = maxV;
}


void  setCollectionID(NxuPhysicsCollection &c,const char *collectionId)
{
	if ( collectionId )
	{
		c.mId = getGlobalString(collectionId);
	}
}

// on the fly auto-generate a CCD skeleton
bool autoGenerateCCDSkeletons(NxActor *actor,NxReal shrink,NxU32 maxv)
{
	bool ret = false;

	if ( gSkeletons == 0 )
	{
		gSkeletons = createCollection("@skeletons",0);
	}

	if ( actor->isDynamic() )
	{
		NxU32 scount = actor->getNbShapes();
		if ( scount )
		{
			NxShape *const* slist = actor->getShapes();
			for (NxU32 i=0; i<scount; i++)
			{
				NxShape *shape = slist[i];
				if ( shape->getCCDSkeleton() == 0 ) // only if it doesn't already have one!
				{

 					NxuGeometry g;

 					NxVec3 dimensions(0,0,0);
 					NxReal radius=0;
 					NxReal height=0;

 					NxConvexMesh *cmesh=0;

					SCHEMA_CLASS sctype = SC_LAST;

 					bool   found = false;

 					switch ( shape->getType() )
 					{

 						case NX_SHAPE_BOX:
 							if ( 1 )
 							{
								sctype = SC_NxBoxShapeDesc;
 								bool found = false;
 								NxBoxShape *bs = (NxBoxShape *) shape;
 								::NxBoxShapeDesc b;
 								bs->saveToDesc(b);
 								for (NxU32 i=0; i<gSkeletons->mSkeletons.size(); i++)
 								{
 									NxCCDSkeletonDesc *sd = gSkeletons->mSkeletons[i];
 									if ( sd->mPrimitive == SC_NxBoxShapeDesc && sd->mDimensions == b.dimensions )
 									{
 										shape->setCCDSkeleton( (NxCCDSkeleton *)sd->mInstance );
 										found = true;
 										break;
 									}
 								}
 								if ( !found )
 								{
   								createBox(b.dimensions,g,0,shrink,maxv);
   								dimensions = b.dimensions;
   							}
 							}
 							break;
 						case NX_SHAPE_SPHERE:
 							if ( 1 )
 							{
								sctype = SC_NxSphereShapeDesc;
 								bool found = false;
 								NxSphereShape *bs = (NxSphereShape *) shape;
 								::NxSphereShapeDesc b;
 								bs->saveToDesc(b);
 								for (NxU32 i=0; i<gSkeletons->mSkeletons.size(); i++)
 								{
 									NxCCDSkeletonDesc *sd = gSkeletons->mSkeletons[i];
 									if ( sd->mPrimitive == SC_NxSphereShapeDesc && sd->mRadius == b.radius )
 									{
 										shape->setCCDSkeleton( (NxCCDSkeleton *)sd->mInstance );
 										found = true;
 										break;
 									}
 								}
 								if ( !found )
 								{
   								createSphere(b.radius,g,0,16,shrink,maxv);
   								radius = b.radius;
   							}
 							}
 							break;
 						case NX_SHAPE_CAPSULE:
 							if ( 1 )
 							{
								sctype = SC_NxCapsuleShapeDesc;
 								bool found = false;
 								NxCapsuleShape *bs = (NxCapsuleShape *) shape;
 								::NxCapsuleShapeDesc b;
 								bs->saveToDesc(b);
 								for (NxU32 i=0; i<gSkeletons->mSkeletons.size(); i++)
 								{
 									NxCCDSkeletonDesc *sd = gSkeletons->mSkeletons[i];
 									if ( sd->mPrimitive == SC_NxCapsuleShapeDesc && sd->mRadius == b.radius && sd->mHeight == b.height )
 									{
 										shape->setCCDSkeleton( (NxCCDSkeleton *)sd->mInstance );
 										found = true;
 										break;
 									}
 								}
 								if ( !found )
 								{
   								createCapsule(b.radius,b.height,g,0,1,16,shrink,maxv);
   								radius = b.radius;
   								height = b.height;
   							}
 							}
 							break;
 						case NX_SHAPE_CONVEX:
 							if ( 1 )
 							{
								sctype = SC_NxConvexShapeDesc;
								NxConvexShape *bs = (NxConvexShape *) shape;
								::NxConvexShapeDesc cv;
								bs->saveToDesc(cv);
								cmesh = cv.meshData;
 								for (NxU32 i=0; i<gSkeletons->mSkeletons.size(); i++)
 								{
 									NxCCDSkeletonDesc *sd = gSkeletons->mSkeletons[i];
 									if ( sd->mPrimitive == SC_NxConvexShapeDesc && sd->mConvexInstance == cmesh )
 									{
 										shape->setCCDSkeleton( (NxCCDSkeleton *)sd->mInstance );
 										found = true;
 										break;
 									}
 								}
 								if ( !found )
 								{
   								CustomCopy cc(gSkeletons,0);
   								NxConvexMeshDesc temp;
									::NxConvexMeshDesc sdkmesh;
									cmesh->saveToDesc(sdkmesh);

   								temp.copyFrom(sdkmesh,cc);

    							if ( temp.mPoints.size() )
  	  						{
		  						  createHull( temp.mPoints.size(), &temp.mPoints[0].x, g, 0, shrink, maxv);
  								}
 				  			}
 							}
 							break;
						default: /*nothing*/ break;
 					}
 					if ( !found && g.mVcount ) // if not found already and we have a geometry.
 					{
 						::NxSimpleTriangleMesh stm;
 						stm.numVertices  = g.mVcount;
 						stm.numTriangles = g.mTcount;
 						stm.points       = g.mVertices;
 						stm.triangles    = g.mIndices;
 						stm.pointStrideBytes = sizeof(float)*3;
 						stm.triangleStrideBytes = sizeof(unsigned int)*3;
 						stm.flags = 0;

 						NxCCDSkeletonDesc *skel = new NxCCDSkeletonDesc;
 						char scratch[512];
 						sprintf(scratch,"AutoCCD_%d", ret );

						skel->mId = getGlobalString(scratch);

 						CustomCopy cc(gSkeletons,0);

 						skel->copyFrom(stm,cc);

						// ok..now let's actually create it on the SDK.
						NxScene &scene = actor->getScene();
						NxPhysicsSDK &sdk = scene.getPhysicsSDK();

            NxCCDSkeleton *ccdskel = sdk.createCCDSkeleton(stm);
            skel->mInstance = ccdskel;
            shape->setCCDSkeleton(ccdskel);

						gSkeletons->mSkeletons.push_back(skel); // add the skeleton

						skel->mPrimitive = sctype;

   					switch ( sctype )
   					{
   						case SC_NxBoxShapeDesc:
   							skel->mDimensions = dimensions;
   							break;
   						case SC_NxSphereShapeDesc:
   							skel->mRadius = radius;
   							break;
   						case SC_NxCapsuleShapeDesc:
   							skel->mRadius = radius;
   							skel->mHeight = height;
   							break;
   						case SC_NxConvexShapeDesc:
   							skel->mConvexInstance = cmesh;
   							break;
						default: /*nothing*/ break;
   					}
 						ret = true;
 					}
				}
			}
		}
	}

  return ret;
}


bool 	addSoftBody(NxuPhysicsCollection &c,NxSoftBody &softBody,const char *userProperties,const char *softBodyId)
{
	bool ret = false;


#if NX_USE_SOFTBODY_API
  NxuPhysicsExport exporter(&c);
  ret = exporter.Write(&softBody, getGlobalString(userProperties), getGlobalString(softBodyId) );
#endif

  return ret;
}

/**
\brief Adds a single cloth mesh to the collection

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the NxClothMesh
\param clothMeshIdThe optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool                    addClothMesh(NxuPhysicsCollection &c,NxClothMesh &cmp,const char *clothMeshId)
{
	bool ret = false;

#if NX_USE_CLOTH_API
  NxuPhysicsExport exporter(&c);
  exporter.Write(&cmp, getGlobalString(clothMeshId));
#endif

  return ret;
}


/**
\brief Adds a single soft body mesh to the collection

\param c The NxuPhysicsCollection to add data to.
\param cmp A reference to the NxSoftBodyMesh
\param softBodyhMeshIdThe optional unique Id.  If null, one will be provided.
\return true if successful.
*/
bool addSoftBodyMesh(NxuPhysicsCollection &c,NxSoftBodyMesh &cmp,const char *softBodyMeshId)
{
	bool ret = false;

#if NX_USE_SOFTBODY_API
  NxuPhysicsExport exporter(&c);
  exporter.Write(&cmp,softBodyMeshId);
#endif

  return ret;
}


bool addSoftBody(NxuPhysicsCollection &c,const ::NxSoftBodyDesc &softBody,const ::NxSoftBodyMeshDesc &softMesh,const char *userProperties,const char *softBodyId)
{
  bool ret = false;

#if NX_USE_SOFTBODY_API
  NxuPhysicsExport exporter(&c);
  exporter.Write(softBody,softMesh,getGlobalString(userProperties),getGlobalString(softBodyId));
#endif

  return ret;
}

#define MAXKEYVALUE 256
#define MAXBUFFER   2048
static char *keyValue[MAXKEYVALUE*2];
static char  keyBuffer[MAXBUFFER];

static char *skipSoft(char *scan,bool &quote)
{
	quote = false;

  while ( *scan )
  {
  	if ( *scan == 34 )
  	{
  		quote = true;
  		scan++;
  		break;
  	}
  	else if ( *scan > 32 && *scan < 127 ) // a valid character to process
  	{
  		break;
  	}
  	scan++;
  }
  return scan;
}

const char ** parseUserProperties(const char *userProperties,unsigned int &count)
{
	const char **ret = 0;
	count = 0;

	if ( userProperties && strlen(userProperties) < MAXBUFFER )
	{
		strcpy(keyBuffer,userProperties);
		char *scan = keyBuffer;
		bool quote;

		while ( *scan ) // while there are characters to process..
		{

			scan = skipSoft(scan,quote); // skip leading spaces if any, note if it begins with a quote

			if ( *scan )  // if still data to process, copy the key pointer and search for the termination character.
			{
   			keyValue[count++] = scan; // store the key pointer and advance the counter.
 	  		while ( *scan  )  // search until we hit an 'equal' sign.
 		  	{
 			  	if ( *scan == 34 ) // if we hit a quote mark, treat that as an end of string termination, but still look for the equal sign.
 			  	{
   					*scan = 0;
   				}
   				else if ( *scan == '=' ) // if we hit the equal sign, terminate the key string by stomping on the equal and continue to value processing
   				{
   					*scan = 0; // stomp and EOS marker.
   					scan++;    // advance to the next location.
   					break;     // break out of the loop
   				}
   				scan++;
   			}
 			  if ( *scan ) // if there is still data to process.
   			{
 	  			scan = skipSoft(scan,quote); // skip leading spaces, etc. note if there is a key.
   			  keyValue[count++] = scan;  // assign the value.
   			  while ( *scan )
   			  {
   			  	if ( quote )  // if we began with a quote, then only a quote can terminate
   			  	{
   			  		if ( *scan == 34 ) // if it is a quote, then terminate the string and continue to the next key
   			  		{
   			  			*scan = 0;
   			  			scan++;
   			  			break;
   			  		}
						}
						else
						{
 			  			// acceptable seperators are a space, a comma, or a semi-colon
 			  			if ( *scan == ';' || *scan == ',' || *scan == '=' || *scan == 32 )
 			  			{
 			  				*scan = 0;
 			  				scan++;
 			  				break;
   			  		}
   			  	}
   			  	scan++;
   			  }
 	  		}
 	  	}
		}
	}

	count = count/2;

	if ( count )
		ret = (const char **)keyValue;

	return ret;
}


const char *  getKeyValue(const char *userProperties,const char *_key)  //
{
  const char *ret = 0;

  unsigned int count=0;
  const char **slist = parseUserProperties(userProperties,count);
  if ( count )
  {
    for (unsigned int i=0; i<count; i++)
    {
      const char *key = slist[i*2];
      const char *value = slist[i*2+1];
      if ( strcmp(key,_key) == 0 )
      {
        ret = value;
        break;
      }
    }
  }
  return ret;
}

void  setEndianMode(bool processorEndian,bool endianSave)
{
	gProcessorBigEndian = processorEndian;
	gSaveBigEndian      = endianSave;
}

};
