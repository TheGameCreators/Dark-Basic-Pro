/*----------------------------------------------------------------------------*\
|
|						     AGEIA PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

//Test SoftBody Implementation

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		//suppress windows' global min,max macros.
#include <windows.h>	//needed for gl.h
#endif

//#include <GL/glut.h>

#include <stdio.h>
#include "MySoftBody.h"
#include "Stream.h"
//#include "BmpLoader.h"
#include "NxCooking.h"
#include "NxArray.h"
#include "ObjMesh.h"
//#include "DrawObjects.h"
#include "cooking.h"
#include "globals.h"

#define TEAR_MEMORY_FACTOR	2

// -----------------------------------------------------------------------
MySoftBody::MySoftBody(NxScene *scene, NxSoftBodyDesc &desc, char *tetFileName, ObjMesh *surfaceMesh)
{
	mInitDone = false;
	NxSoftBodyMeshDesc meshDesc;
	generateTetMeshDesc(meshDesc, tetFileName);

	// todo: handle failure
	init(scene, desc, meshDesc, surfaceMesh);
}


// -----------------------------------------------------------------------
MySoftBody::MySoftBody(NxScene *scene, NxSoftBodyDesc &desc, SoftBodyType type, 
					   NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines)

{
	mInitDone = false;
	NxSoftBodyMeshDesc meshDesc;
	switch (type) {
		case SOFTBODY_BOX: generateBoxMeshDesc(meshDesc, wx,wy,wz,h, tearLines); break;
		case SOFTBODY_SPHERE: generateSphereMeshDesc(meshDesc, wx,wy,wz,h, tearLines); break;
	}
	init(scene, desc, meshDesc, NULL);
}

// -----------------------------------------------------------------------
void MySoftBody::init(NxScene *scene, NxSoftBodyDesc &desc, NxSoftBodyMeshDesc &meshDesc, ObjMesh *surfaceMesh)
{
	mScene = scene;
	mScale = 1.0f;
	mOffset.zero();
	mSurfaceMesh = NULL;

	// if we want tearing on the PPU we must tell the cooker
	// this way it will generate some space for particles that will be generated during tearing		
	if (desc.flags & NX_SBF_TEARABLE)
		meshDesc.flags |= NX_SOFTBODY_MESH_TEARABLE;

	cookMesh(meshDesc);

	// compute barycentric coords before buffers are deleted again
	if (surfaceMesh != NULL) {
		mSurfaceMesh = surfaceMesh;
		mSurfaceMesh->buildTetraLinks((NxVec3 *)meshDesc.vertices, (NxU32 *)meshDesc.tetrahedra, meshDesc.numTetrahedra);
	}

	releaseMeshDescBuffers(meshDesc);

	allocateReceiveBuffers(meshDesc.numVertices, meshDesc.numTetrahedra);
	desc.softBodyMesh = mSoftBodyMesh;
	desc.meshData = mReceiveBuffers;

	mSoftBody = scene->createSoftBody(desc);
	mInitDone = true;


}

// -----------------------------------------------------------------------
MySoftBody::~MySoftBody()
{
	if (mInitDone) {
		if (mSoftBody) mScene->releaseSoftBody(*mSoftBody);
		if (mSoftBodyMesh) mScene->getPhysicsSDK().releaseSoftBodyMesh(*mSoftBodyMesh);
		releaseReceiveBuffers();
	}
}


// -----------------------------------------------------------------------
bool MySoftBody::generateTetMeshDesc(NxSoftBodyMeshDesc &desc, char *filename)
{
	static const NxU16 MESH_STRING_LEN = 256;
	char s[MESH_STRING_LEN];
	int i0,i1,i2,i3;
	NxVec3 v;

	FILE *f = fopen(filename, "r");
	if (!f) return false;

	NxArray<NxVec3> tempVertices;
	NxArray<NxU32> tempIndices;
	NxBounds3 bounds;
	bounds.setEmpty();
	
	while (!feof(f)) {
		if (fgets(s, MESH_STRING_LEN, f) == NULL) break;

		if (strncmp(s, "v ", 2) == 0) {	// vertex
			sscanf(s, "v %f %f %f", &v.x, &v.y, &v.z);
			tempVertices.push_back(v);
			bounds.include(v);
		}
		else if (strncmp(s, "t ", 2) == 0) {	// tetra
			sscanf(s, "t %i %i %i %i", &i0,&i1,&i2,&i3);
			tempIndices.push_back(i0);
			tempIndices.push_back(i1);
			tempIndices.push_back(i2);
			tempIndices.push_back(i3);
		}
	}

	if(tempVertices.size() == 0) return false;

	desc.numVertices				= tempVertices.size();
	desc.numTetrahedra				= tempIndices.size() / 4;
	desc.vertexStrideBytes			= sizeof(NxVec3);
	desc.tetrahedronStrideBytes		= 4*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.vertices					= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.tetrahedra					= (NxU32*)malloc(sizeof(NxU32)*desc.numTetrahedra*4);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;

	// copy points and indices
	NxReal diag = bounds.min.distance(bounds.max);
	mOffset = (bounds.min + bounds.max) * 0.5f;
	
	NxVec3 *vSrc = (NxVec3*)tempVertices.begin();
	NxVec3 *vDest = (NxVec3*)desc.vertices;
	for (NxU32 i = 0; i < desc.numVertices; i++, vDest++, vSrc++) 
		*vDest = (*vSrc) - mOffset;
	memcpy((NxU32*)desc.tetrahedra, tempIndices.begin(), sizeof(NxU32)*desc.numTetrahedra*4);

	return true;
}

// -----------------------------------------------------------------------
void MySoftBody::releaseMeshDescBuffers(const NxSoftBodyMeshDesc& desc)
{
	NxVec3* p = (NxVec3*)desc.vertices;
	NxU32* t = (NxU32*)desc.tetrahedra;
	NxReal* m = (NxReal*)desc.vertexMasses;
	NxU32* f = (NxU32*)desc.vertexFlags;
	free(p);
	free(t);
	free(m);
	free(f);
}

// -----------------------------------------------------------------------
bool MySoftBody::cookMesh(NxSoftBodyMeshDesc& desc)
{
	// we cook the mesh on the fly through a memory stream
	// we could also use a file stream and pre-cook the mesh
	MemoryWriteBuffer wb;
	if (!CookSoftBodyMesh(desc, wb)) 
		return false;

	MemoryReadBuffer rb(wb.data);
	mSoftBodyMesh = mScene->getPhysicsSDK().createSoftBodyMesh(rb);
	return true;
}

// -----------------------------------------------------------------------
void MySoftBody::allocateReceiveBuffers(int numVertices, int numTetrahedra)
{
	// here we setup the buffers through which the SDK returns the dynamic softbody data
	// we reserve more memory for vertices than the initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears softbodies as long as there is room in these buffers
	
	NxU32 maxVertices = TEAR_MEMORY_FACTOR * numVertices;
	mReceiveBuffers.verticesPosBegin = (NxVec3*)malloc(sizeof(NxVec3)*maxVertices);		
	mReceiveBuffers.verticesPosByteStride = sizeof(NxVec3);
	mReceiveBuffers.maxVertices = maxVertices;
	mReceiveBuffers.numVerticesPtr = (NxU32*)malloc(sizeof(NxU32));

	// the number of tetrahedra is constant, even if the softbody is torn
	NxU32 maxIndices = 4*numTetrahedra;
	mReceiveBuffers.indicesBegin = (NxU32*)malloc(sizeof(NxU32)*maxIndices);
	mReceiveBuffers.indicesByteStride = sizeof(NxU32);
	mReceiveBuffers.maxIndices = maxIndices;
	mReceiveBuffers.numIndicesPtr = (NxU32*)malloc(sizeof(NxU32));

	// init the buffers in case we want to draw the mesh 
	// before the SDK as filled in the correct values
	*mReceiveBuffers.numVerticesPtr = 0;
	*mReceiveBuffers.numIndicesPtr = 0;
}

// -----------------------------------------------------------------------
void MySoftBody::releaseReceiveBuffers()
{
	NxVec3* vp;
	NxU32* up; 
	vp = (NxVec3*)mReceiveBuffers.verticesPosBegin; free(vp);
	up = (NxU32*)mReceiveBuffers.numVerticesPtr; free(up);

	up = (NxU32*)mReceiveBuffers.indicesBegin; free(up);
	up = (NxU32*)mReceiveBuffers.numIndicesPtr; free(up);
}

// -----------------------------------------------------------------------
void MySoftBody::simulateAndDraw(bool shadows, bool tetraMesh, bool tetraWireframe)
{
	
	if (!tetraMesh && mSurfaceMesh) {
		mSurfaceMesh->simulateMesh(mReceiveBuffers);
		mSurfaceMesh->draw(0xFFFFFF, false, shadows);
		return;
	}

	/*
	if (!tetraMesh) return;

	if (tetraWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	NxU32 numVertices = *mReceiveBuffers.numVerticesPtr;
	NxU32 numTetrahedra = *mReceiveBuffers.numIndicesPtr / 4;
	NxVec3 *vertices = (NxVec3*)mReceiveBuffers.verticesPosBegin;

	NxReal scale = 0.9f;
	GLfloat color [] = {0.0f,1.0f,0.0f,1.0f};
	glColor4fv(color);

	NxU32 *idxPtr = (NxU32*)mReceiveBuffers.indicesBegin;
	NxU32 renderVertIdx = 0; 
	NxU32 renderNormalIdx = 0;

	if(mTetraRenderVerts.size() == 0)
	{
		mTetraRenderVerts.resize(12 * numTetrahedra);
		mTetraRenderNormals.resize(12 * numTetrahedra);
	}

	for(NxU32 i = 0; i < numTetrahedra; i++)
	{
		NxVec3 p0(vertices[*idxPtr++]);
		NxVec3 p1(vertices[*idxPtr++]);
		NxVec3 p2(vertices[*idxPtr++]);
		NxVec3 p3(vertices[*idxPtr++]);
		NxVec3 normal, center, v[4];

		center = p0;  v[0] = p0;
		center += p1; v[1] = p1;
		center += p2; v[2] = p2;
		center += p3; v[3] = p3;
		center *= 0.25f;
		v[0] = center + (v[0] - center)*scale;
		v[1] = center + (v[1] - center)*scale;
		v[2] = center + (v[2] - center)*scale;
		v[3] = center + (v[3] - center)*scale;
		NxU8 sides[4][3] = {{2,1,0}, {0,1,3}, {1,2,3}, {2,0,3}};

		for (NxU8 side = 0; side < 4; side ++) {
			NxVec3 &v0 = v[sides[side][0]];
			NxVec3 &v1 = v[sides[side][1]];
			NxVec3 &v2 = v[sides[side][2]];
			
			normal.cross(v1-v0, v2-v0);
			normal.normalize();

			mTetraRenderVerts[renderVertIdx++] = v0;
			mTetraRenderVerts[renderVertIdx++] = v1;
			mTetraRenderVerts[renderVertIdx++] = v2;

			mTetraRenderNormals[renderNormalIdx++] = normal;
			mTetraRenderNormals[renderNormalIdx++] = normal;
			mTetraRenderNormals[renderNormalIdx++] = normal;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mTetraRenderVerts.begin());
	glNormalPointer(GL_FLOAT, 0, mTetraRenderNormals.begin());

	glDrawArrays(GL_TRIANGLES, 0, mTetraRenderVerts.size()); 

	if (shadows) {
		const static NxReal ShadowMat[] = { 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glPushMatrix();
		glMultMatrixf(ShadowMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.05f, 0.1f, 0.15f,1.0f);

		glDrawArrays(GL_TRIANGLES, 0, mTetraRenderVerts.size()); 
		
		glColor4fv(color);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	if (tetraWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/
}

// -----------------------------------------------------------------------
void MySoftBody::generateBoxMeshDesc(NxSoftBodyMeshDesc &desc, NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines)
{
	int numX = (int)(wx / h) + 1;
	int numY = (int)(wy / h) + 1;
	int numZ = (int)(wz / h) + 1;

	desc.numVertices				= (numX+1) * (numY+1) * (numZ+1);
	desc.numTetrahedra				= numX*numY*numZ*5;
	desc.vertexStrideBytes			= sizeof(NxVec3);
	desc.tetrahedronStrideBytes		= 4*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.vertices					= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.tetrahedra					= (NxU32*)malloc(sizeof(NxU32)*desc.numTetrahedra * 4);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;

	int i,j,k;
	NxVec3 offset(h * numX * 0.5f, h * numY * 0.5f, h * numZ * 0.5f);
	NxVec3 *p = (NxVec3*)desc.vertices;
	for (i = 0; i <= numX; i++) {
		for (j = 0; j <= numY; j++) {
			for (k = 0; k <= numZ; k++) {
				p->set(h*i, h*j, h*k); 
				*p -=offset;
				p++;
			}
		}
	}

	int i1,i2,i3,i4,i5,i6,i7,i8;

	NxU32 *id = (NxU32*)desc.tetrahedra;
	for (i = 0; i < numX; i++) {
		for (j = 0; j < numY; j++) {
			for (k = 0; k < numZ; k++) {
		        i5 = (i*(numY+1) + j)*(numZ+1) + k; i1 = i5+1;
		        i6 = ((i+1)*(numY+1) + j)*(numZ+1) + k; i2 = i6+1;
		        i7 = ((i+1)*(numY+1) + (j+1))*(numZ+1) + k; i3 = i7+1;
		        i8 = (i*(numY+1) + (j+1))*(numZ+1) + k; i4 = i8+1;

				if ((i + j + k) % 2 == 1) {
					*id++ = i1; *id++ = i2; *id++ = i6; *id++ = i3;
					*id++ = i6; *id++ = i3; *id++ = i7; *id++ = i8;
					*id++ = i1; *id++ = i8; *id++ = i4; *id++ = i3;
					*id++ = i1; *id++ = i6; *id++ = i5; *id++ = i8;
					*id++ = i1; *id++ = i3; *id++ = i6; *id++ = i8;
				}
				else {
					*id++ = i2; *id++ = i5; *id++ = i1; *id++ = i4;
					*id++ = i2; *id++ = i7; *id++ = i6; *id++ = i5;
					*id++ = i2; *id++ = i4; *id++ = i3; *id++ = i7;
					*id++ = i5; *id++ = i7; *id++ = i8; *id++ = i4;
					*id++ = i2; *id++ = i5; *id++ = i4; *id++ = i7;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------
int MySoftBody::childNr(int i0, int i1, NxVec3 &pos) 
{
	int i,j;
	int num = maxSphereLevels*6;
	if (i0 > i1) { i = i0; i0 = i1; i1 = i; }
	i = i0 * num * 2;
	for (j = 0; j < num; j++, i += 2) {
		if (mSphereDivisions[i] == i1 || mSphereDivisions[i] < 0) 
			break;
	}
	if (j < num && mSphereDivisions[i] == i1)
		return mSphereDivisions[i+1];

	assert(j < num);
	mSphereDivisions[i] = i1;
	mSphereDivisions[i+1] = mSphereVertices.size();
	mSphereVertices.push_back(pos);

	for (j = 0; j < maxSphereLevels*6*2; j++)
		mSphereDivisions.push_back(-1);

	return mSphereDivisions[i+1];
}

//---------------------------------------------------------------------------------------
void MySoftBody::sphereSubdivide(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, 
						   int i0, int i1, int i2, int level) 
{
	if (level == 0) {
		mSphereIndices.push_back(0); 
		mSphereIndices.push_back(i0); 
		mSphereIndices.push_back(i1); 
		mSphereIndices.push_back(i2); 
		return;
	}

	NxVec3 p01 = p0 + p1; p01.normalize();
	NxVec3 p12 = p1 + p2; p12.normalize();
	NxVec3 p20 = p2 + p0; p20.normalize();
	int i01 = childNr(i0,i1,p01);
	int i12 = childNr(i1,i2,p12);
	int i20 = childNr(i2,i0,p20);

	sphereSubdivide(p0, p01, p20, i0,i01,i20, level-1);
	sphereSubdivide(p1, p12, p01, i1,i12,i01, level-1);
	sphereSubdivide(p2, p20, p12, i2,i20,i12, level-1);
	sphereSubdivide(p01,p12, p20, i01,i12,i20, level-1);
}

// -----------------------------------------------------------------------
void MySoftBody::generateSphereMeshDesc(NxSoftBodyMeshDesc &desc, NxReal wx, NxReal wy, NxReal wz, NxReal h, bool tearLines)
{
	int level = 0;
	NxReal d = 5*h;
	NxReal r = (wx + wy + wz) / 3.0f;

	while (d < r && level < maxSphereLevels) {
		d *= 2.0f;
		level++;
	}

	mSphereIndices.clear();
	mSphereVertices.clear();
	mSphereDivisions.clear();

	#define X 0.525731112119133606f
	#define Z 0.850650808352039932f

	static NxReal coords[12][3] = {
		{-X, 0.0f, Z}, {X, 0.0f, Z}, {-X, 0.0f, -Z}, {X, 0.0f, -Z},
		{0.0f, Z, X}, {0.0f, Z, -X}, {0.0f, -Z, X}, {0.0f, -Z, -X},
		{Z, X, 0.0f}, {-Z, X, 0.0f}, {Z, -X, 0.0f}, {-Z, -X, 0.0f}
	};

	static NxU32 indices[20][3] = {
		{1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
		{1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
		{3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
		{10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
	};

	NxVec3 p0,p1,p2;
	NxU32 i;
	// center vertex
	p0.zero();
	mSphereVertices.push_back(p0);

	// first level vertices
	for (i = 0; i < 12; i++) {
		p0.set(coords[i][0], coords[i][1], coords[i][2]);
		mSphereVertices.push_back(p0);
	}

	for (i = 0; i < 13 * maxSphereLevels*6*2; i++)
		mSphereDivisions.push_back(-1);

	for (i = 0; i < 20; i++) {
		NxU32 i0 = indices[i][0];
		NxU32 i1 = indices[i][1];
		NxU32 i2 = indices[i][2];
		p0.set(coords[i0][0], coords[i0][1], coords[i0][2]);
		p1.set(coords[i1][0], coords[i1][1], coords[i1][2]);
		p2.set(coords[i2][0], coords[i2][1], coords[i2][2]);
		sphereSubdivide(p0,p1,p2, i0+1,i1+1,i2+1, level);
	}

	desc.numVertices				= mSphereVertices.size();
	desc.numTetrahedra				= mSphereIndices.size() / 4;
	desc.vertexStrideBytes			= sizeof(NxVec3);
	desc.tetrahedronStrideBytes		= 4*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.vertices					= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.tetrahedra					= (NxU32*)malloc(sizeof(NxU32)*desc.numTetrahedra * 4);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;

	NxVec3 *v = (NxVec3*)desc.vertices;
	NxVec3 scaled;
	for (i = 0; i < mSphereVertices.size(); i++, v++) {
		scaled = mSphereVertices[i];
		scaled.x *= wx; scaled.y *= wy; scaled.z *= wz;
		*v = scaled;
	}

	NxU32 *tetras = (NxU32*)desc.tetrahedra;
	for (i = 0; i < mSphereIndices.size(); i++, tetras++)
		*tetras = mSphereIndices[i];
}
