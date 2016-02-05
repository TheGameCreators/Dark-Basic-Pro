// ===============================================================================
//						  AGEIA PHYSX SDK SAMPLE PROGRAMS
//					        TEST SOFT BODY IMPLEMENTATION
//
//		            Written by Matthias Müller-Fischer 03-08-05
// ===============================================================================

#ifndef MYSOFTBODY_H
#define MYSOFTBODY_H

#include "NxPhysics.h"

class ObjMesh;

enum SoftBodyType
{
	SOFTBODY_BOX = 0,
	SOFTBODY_SPHERE	= 1,
};

class MySoftBody {
public:
	/* create mesh from tet file */
	MySoftBody(NxScene *scene, NxSoftBodyDesc &desc, char *tetFileName, ObjMesh *surfaceMesh);
	/* regular mesh */
	MySoftBody(NxScene *scene, NxSoftBodyDesc &desc, SoftBodyType type, NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines = false);

	~MySoftBody();

	void simulateAndDraw(bool shadows, bool tetraMesh, bool tetraWireframe);
	NxSoftBody *getNxSoftBody() { return mSoftBody; }

	const NxMeshData* getMeshData() const { return &mReceiveBuffers; }


//private:
public:
	void init(NxScene *scene, NxSoftBodyDesc &desc, NxSoftBodyMeshDesc &meshDesc, ObjMesh *surfaceMesh);
	bool generateTetMeshDesc(NxSoftBodyMeshDesc &desc, char *filename);

	void releaseMeshDescBuffers(const NxSoftBodyMeshDesc& desc);
	bool cookMesh(NxSoftBodyMeshDesc& desc);

	void allocateReceiveBuffers(int numVertices, int numTetrahedra);
	void releaseReceiveBuffers();

	void generateBoxMeshDesc(NxSoftBodyMeshDesc &desc, NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines = false);

	int childNr(int i0, int i1, NxVec3 &pos);
	void sphereSubdivide(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, int i0, int i1, int i2, int level);
	void generateSphereMeshDesc(NxSoftBodyMeshDesc &desc, NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines = false);
	static const int maxSphereLevels = 4;
	NxArray<NxU32> mSphereIndices;
	NxArray<NxVec3> mSphereVertices;
	NxArray<int> mSphereDivisions;

	bool mInitDone;

	NxMeshData mReceiveBuffers;
	NxScene *mScene;
	NxSoftBody *mSoftBody;
	NxSoftBodyMesh *mSoftBodyMesh;

	ObjMesh *mSurfaceMesh;

	NxArray<NxVec3> mTetraRenderVerts;
	NxArray<NxVec3> mTetraRenderNormals;

	NxReal  mScale;
	NxVec3  mOffset;
};

#endif
