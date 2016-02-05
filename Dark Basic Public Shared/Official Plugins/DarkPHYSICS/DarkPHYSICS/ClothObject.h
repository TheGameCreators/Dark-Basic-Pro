// ===============================================================================
//						  AGEIA PHYSX SDK SAMPLE PROGRAMS
//					        TEST CLOTH IMPLEMENTATION
//
//		            Written by Matthias Müller-Fischer 03-08-05
// ===============================================================================

/*
#ifndef CCLOTHOBJECT_H
#define CCLOTHOBJECT_H

#include "NxPhysics.h"

class cClothObject {
public:
	
	cClothObject(NxScene *scene, NxClothDesc &desc, char *objFileName, NxReal scale);
	
	cClothObject(NxScene *scene, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, char *texFilename = NULL);

	~cClothObject();

	void draw(bool shadows);
	NxCloth *getNxCloth() { return mCloth; }

public:
	void init(NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc);
	bool generateObjMeshDesc(NxClothMeshDesc &desc, char *filename, NxReal scale);
	void generateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords);
	void releaseMeshDescBuffers(const NxClothMeshDesc& desc);

	bool cookMesh(NxClothMeshDesc& desc);

	void allocateReceiveBuffers(int numVertices, int numTriangles);
	void releaseReceiveBuffers();

	bool createTexture(char *filename);
	void updateTextureCoordinates();

	bool mInitDone;

	NxMeshData mReceiveBuffers;
	NxScene *mScene;
	NxCloth *mCloth;
	NxClothMesh *mClothMesh;

	float *mTexCoords;
	NxU32   mNumTexCoords;
	int  mTexId;
};

#endif
*/


#ifndef MYCLOTH_H
#define MYCLOTH_H

#include "NxPhysics.h"
#include "globals.h"

class cClothObject {
public:
	/* create mesh from obj file */
	cClothObject(sObject* pObject, NxScene *scene, NxClothDesc &desc, char *objFileName, NxReal scale);
	/* create regular mesh */
	cClothObject(NxScene *scene, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, char *texFilename = NULL, bool tearLines = false);

	~cClothObject();

	void draw(bool shadows);
	NxCloth *getNxCloth() { return mCloth; }

public:
	void init(NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc);
	bool generateObjMeshDesc(sObject* pObject, NxClothMeshDesc &desc, char *filename, NxReal scale);
	void generateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords, bool tearLines = false);
	void generateTearLines(NxClothMeshDesc& desc, NxU32 w, NxU32 h);
	void releaseMeshDescBuffers(const NxClothMeshDesc& desc);

	bool cookMesh(NxClothMeshDesc& desc);

	void allocateReceiveBuffers(int numVertices, int numTriangles);
	void releaseReceiveBuffers();

	bool createTexture(char *filename);
	void updateTextureCoordinates();

	bool mInitDone;

	NxMeshData mReceiveBuffers;
	NxScene *mScene;
	NxCloth *mCloth;
	NxClothMesh *mClothMesh;

	float *mTexCoords;
	NxU32   mNumTexCoords;
	int  mTexId;
};

#endif

