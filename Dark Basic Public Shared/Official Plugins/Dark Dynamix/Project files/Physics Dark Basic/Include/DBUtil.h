#pragma once
#include "Globals.h"
#include "MathUtil.h"

class MeshData
{
public:
	MeshData(void){}
	~MeshData(void){}

	unsigned int vertexCount_;
	unsigned int indexCount_;
	NxVec3* vertices_;
	NxU32*  indices_;
	NxVec3  scale_;
};

class BoneData
{
public:
    NxMat34 mat_;
    NxVec3  extent_;
	D3DXMATRIX offsetMat_;
};



class DBUtil
{
public:


#ifdef COMPILE_FOR_GDK
static void setNormals(int objectID, int limb){
//--NORMALIZE
	dbMakeVector3(1);
	dbMakeVector3(2);
	dbMakeVector3(3);
	dbMakeVector3(4);
	dbMakeVector3(5);
	dbMakeVector3(6);
	dbLockVertexDataForLimb(objectID, limb);
	int vertices = dbGetVertexDataVertexCount();
	int indices = dbGetVertexDataIndexCount();
    int faces = vertices / 3;
	int p1index = 0;
	int p2index = 0;
	int p3index = 0;
	int count;
	if(indices)
		count = indices;
	else
		count = vertices;
	for(int a = 0; a < count; a = a + 3){
		if(!indices){
           p1index = a;
		   p2index = a + 1;
		   p3index = a + 2;
		}
		else{
           p1index = dbGetIndexData(a);
		   p2index = dbGetIndexData(a + 1);
		   p3index = dbGetIndexData(a + 2);
		}
		float Ax = dbGetVertexDataPositionX(p1index);
		float Ay = dbGetVertexDataPositionY(p1index);
		float Az = dbGetVertexDataPositionZ(p1index);
		dbSetVector3(1, Ax, Ay, Az);

		float Bx = dbGetVertexDataPositionX(p2index);
		float By = dbGetVertexDataPositionY(p2index);
		float Bz = dbGetVertexDataPositionZ(p2index);
		dbSetVector3(2, Bx, By, Bz);

		float Cx = dbGetVertexDataPositionX(p3index);
		float Cy = dbGetVertexDataPositionY(p3index);
		float Cz = dbGetVertexDataPositionZ(p3index);
		dbSetVector3(3, Cx, Cy, Cz);
		dbSubtractVector3(4, 2, 1);
		dbSubtractVector3(5, 3, 1);
		dbCrossProductVector3(6, 4, 5);
		dbNormalizeVector3(6, 6);
		Ax = dbXVector3(6);
		Ay = dbYVector3(6);
		Az = dbZVector3(6);
		dbSetVertexDataNormals(a, Ax, Ay, Az);
		dbSetVertexDataNormals(a + 1, Ax, Ay, Az);
		dbSetVertexDataNormals(a + 2, Ax, Ay, Az);
	}
	dbUnlockVertexData();

	dbDeleteVector3(1);
	dbDeleteVector3(2);
	dbDeleteVector3(3);
	dbDeleteVector3(4);
	dbDeleteVector3(5);
	dbDeleteVector3(6);

	//New object with set vertex data
	int meshID = 1;
	dbMakeMeshFromObject(meshID, objectID);
	dbDeleteObject(objectID);
	dbMakeObject(objectID, meshID, 0);
	dbDeleteMesh(meshID);
}
#endif



	static NxVec3* getVertexData(sMesh* aMesh, NxVec3 scale)
	{
	   DWORD vertexCount = aMesh->dwVertexCount;
	   NxVec3* vertices = new NxVec3[vertexCount];

	   // Create pointer to vertex data of object mesh
	   BYTE* vertexData = aMesh->pVertexData; 
	   // Get FVF size
	   DWORD FVFSize = aMesh->dwFVFSize;  
	   // Use loop to populate physX vertex data with our object mesh vertex data
	   for(unsigned int j = 0; j < vertexCount; j++)
	   {
		  NxVec3* pos = (NxVec3*)vertexData;
		  vertices[j].x = pos->x * scale.x;
		  vertices[j].y = pos->y * scale.y;
		  vertices[j].z = pos->z * scale.z;
		  vertexData += FVFSize;
		}
		return vertices;
	}

	static NxU32* getIndexData(sMesh* aMesh)
	{
		DWORD indexCount = aMesh->dwIndexCount;
		// Create array to hold indices in mesh
		NxU32* indices = new NxU32[indexCount]; 
		// Get array of indices from mesh
		WORD* indexArray = aMesh->pIndices;     
		// Use loop to populate physX index array with our object mesh index array
		for(unsigned int i = 0; i < indexCount; i++)
		{
		   indices[i] = indexArray[i];
		}
		return indices;
	}

	static void getMeshData(sObject *obj, MeshData &mesh){
		mesh.scale_       = (NxVec3)obj->position.vecScale;
		
		//Limb
		mesh.vertexCount_ = obj->ppFrameList[0]->pMesh->dwVertexCount;
		mesh.indexCount_  = obj->ppFrameList[0]->pMesh->dwIndexCount;
		mesh.vertices_    = DBUtil::getVertexData(obj->ppFrameList[0]->pMesh, mesh.scale_);
		mesh.indices_     = DBUtil::getIndexData(obj->ppFrameList[0]->pMesh);
	
		//Mesh
		//obj->ppMeshList[0]->
		//mesh.vertexCount_ = obj->ppMeshList[0]->dwVertexCount;
		//mesh.indexCount_  = obj->ppMeshList[0]->dwIndexCount;
		//mesh.vertices_ = DBUtil::getVertexData(obj->ppMeshList[0], mesh.scale_);
		//mesh.indices_ = DBUtil::getIndexData(obj->ppMeshList[0]);
	
	}

	static int getBoneVertexCount(sMesh* mesh, int boneID){
        return mesh->pBones[boneID].dwNumInfluences;
	}
	static int getBoneVertexCount(sObject* obj, int meshID, int boneID){
       	//-- Get mesh
		sMesh* mesh = 0;
		mesh = obj->ppMeshList[meshID];
		if(!mesh){
            return 0;
		}
	    return mesh->pBones[boneID].dwNumInfluences;
	}
	static NxVec3* getBoneVerts(sObject* obj, int meshID, int boneID){
        //-- Get scale
		NxVec3 scale;
		scale.x = obj->position.vecScale.x;
		scale.y = obj->position.vecScale.y;
		scale.z = obj->position.vecScale.z;
		
		//-- Get mesh
		sMesh* mesh = 0;
		mesh = obj->ppMeshList[meshID];
		if(!mesh){
            return 0;
		}
		if(!mesh->pBones[boneID].pVertices){
            return 0;
		}

		//-- Get complete mesh vertex array
		int totalVertCount = mesh->dwVertexCount;
		NxVec3 *verts = new NxVec3[totalVertCount];
		verts = DBUtil::getVertexData(mesh, scale);	
		
		//-- Get bone vertices only from complete mesh
		int bvCount = getBoneVertexCount(mesh, boneID);
		NxVec3* outVecArray = new NxVec3[bvCount];
	    DWORD* pVerts = mesh->pBones[boneID].pVertices;
		for(int i = 0; i < bvCount; i++){
			int index = *pVerts;
			outVecArray[i] = verts[index];
			pVerts++;
		}
		//-- Don't need complete verts, user must release bone verts.
		delete verts;
		return outVecArray;
	}

	static NxVec3* getBoneVerts(sMesh* mesh, int boneID, NxVec3 scale){
        //-- Get complete mesh vertex array
		int totalVertCount = mesh->dwVertexCount;
		NxVec3 *verts = new NxVec3[totalVertCount];
		verts = DBUtil::getVertexData(mesh, scale);	
		
		//-- Get bone vertices only from complete mesh
		int bvCount = getBoneVertexCount(mesh, boneID);
		NxVec3* outVecArray = new NxVec3[bvCount];
	    DWORD* pVerts = mesh->pBones[boneID].pVertices;
		for(int i = 0; i < bvCount; i++){
			int index = *pVerts;
			outVecArray[i] = verts[index];
			pVerts++;
		}
		//-- Don't need complete verts, user must release bone verts.
		delete verts;
		return outVecArray;
	}

	static void getMeshData(sObject *obj, int limb, float scale, MeshData &mesh){
		//Object scale * limb scale
		
		mesh.scale_       = (NxVec3)obj->position.vecScale;
		NxVec3 limbScale = (NxVec3)obj->ppFrameList[limb]->vecScale;
		//scale_.arrayMultiply((NxVec3)obj->ppFrameList[limb]->vecScale, scale_);
		//User scale adjust * model scale * limb scale
		//scale = scale / 100;
		mesh.scale_.x = scale * mesh.scale_.x * limbScale.x;
		mesh.scale_.y = scale * mesh.scale_.y * limbScale.y;
		mesh.scale_.z = scale * mesh.scale_.z * limbScale.z;

		//Limb
		mesh.vertexCount_ = obj->ppFrameList[limb]->pMesh->dwVertexCount;
		mesh.indexCount_  = obj->ppFrameList[limb]->pMesh->dwIndexCount;
		mesh.vertices_ = DBUtil::getVertexData(obj->ppFrameList[limb]->pMesh, mesh.scale_);
		mesh.indices_ = DBUtil::getIndexData(obj->ppFrameList[limb]->pMesh);
	
		////Mesh
		//mesh.vertexCount_ = obj->ppMeshList[limb]->dwVertexCount;
		//mesh.indexCount_  = obj->ppMeshList[limb]->dwIndexCount;
		//mesh.vertices_ = DBUtil::getVertexData(obj->ppMeshList[limb], mesh.scale_);
		//mesh.indices_ = DBUtil::getIndexData(obj->ppMeshList[limb]);
	
	}

	static NxMat34 getMatrixFromLimb(int object, int limb){
	    D3DXMATRIX mat;
	    dbGetObjectWorldMatrix(object, limb, &mat);
	    MathUtil::RemoveScaleFromMatrix(mat);
		NxMat34 phyMat;
	    phyMat.setColumnMajor44(mat.m);
		return phyMat;
	}

	static NxMat34 getMatrixFromObject(int object){
	    D3DXMATRIX mat;
	    dbGetObjectWorldMatrix(object, 0, &mat);
	    MathUtil::RemoveScaleFromMatrix(mat);
		NxMat34 phyMat;
	    phyMat.setColumnMajor44(mat.m);
		return phyMat;
	}

	static NxVec3 getObjectDimensions(int objectID){
		sObject *obj = dbGetObject(objectID);
		NxVec3 scale        = (NxVec3)obj->position.vecScale;
		NxVec3 dim          = (NxVec3)(obj->collision.vecMax - obj->collision.vecMin);
		dim.arrayMultiply(dim, scale);
		return dim;
	}

//class BoneData
//{
//public:
//    NxMat34 mat_;
//    NxVec3 extent_;
//};

static void getBoneData(BoneData &dataOut, int objectID, int meshID, int boneID, NxVec3 &scaleMultiplier){
	sObject* pObject = dbGetObject(objectID);

	sMesh *pMesh = pObject->ppMeshList [ meshID ];
	BYTE* pVertPtr = pMesh->pOriginalVertexData;
	if ( pVertPtr == NULL ) pVertPtr = pMesh->pVertexData;	//if pOriginalVertexData has not been filled, assume pVertexData contains original vertices

	sBone *pBone = &pMesh->pBones[boneID];

	//values to store the bounds of the bone
	float minx=0,miny=0,minz=0;
	float maxx=0,maxy=0,maxz=0;

	//for all vertices, calculate the bounding box
	for ( int vert = 0; vert < ( int ) pBone->dwNumInfluences; vert++ )
	{
		int iOffset = pBone->pVertices [ vert ];
		float fWeight = pBone->pWeights [ vert ];

		if ( fWeight > 0.3 )
		{
			float* pVertexBase = (float*)(pVertPtr + ( pMesh->dwFVFSize * iOffset ));

			D3DXVECTOR3 vec = D3DXVECTOR3 ( *(pVertexBase+0), *(pVertexBase+1), *(pVertexBase+2) );
			
			//translate vertex onto x-axis using the bone translation and check bounds
			D3DXVECTOR3 vecFinal;
			vecFinal.x = vec.x * pBone->matTranslation._11 + vec.y * pBone->matTranslation._21 + vec.z * pBone->matTranslation._31 + pBone->matTranslation._41;
			vecFinal.y = vec.x * pBone->matTranslation._12 + vec.y * pBone->matTranslation._22 + vec.z * pBone->matTranslation._32 + pBone->matTranslation._42;
			vecFinal.z = vec.x * pBone->matTranslation._13 + vec.y * pBone->matTranslation._23 + vec.z * pBone->matTranslation._33 + pBone->matTranslation._43;

			if ( vecFinal.x < minx || vert == 0 ) minx = vecFinal.x;
			if ( vecFinal.y < miny || vert == 0 ) miny = vecFinal.y;
			if ( vecFinal.z < minz || vert == 0 ) minz = vecFinal.z;

			if ( vecFinal.x > maxx || vert == 0 ) maxx = vecFinal.x;
			if ( vecFinal.y > maxy || vert == 0 ) maxy = vecFinal.y;
			if ( vecFinal.z > maxz || vert == 0 ) maxz = vecFinal.z;
		}
	}
	
	//calculate the center of the bounding box, this is the offset from te bone's center of rotation
	float fCenterX = (minx+maxx) / 2.0f;
	float fCenterY = (miny+maxy) / 2.0f;
	float fCenterZ = (minz+maxz) / 2.0f;
	
	//adjust bounding box to be smaller than the actual bone, may need tweaking.
	//float fAdjust = 0.5f; //0.85f;
	float fExtentX = (maxx-minx) * scaleMultiplier.x;
	float fExtentY = (maxy-miny) * scaleMultiplier.y;
	float fExtentZ = (maxz-minz) * scaleMultiplier.z;	

	if ( fExtentX < 0.1f ) fExtentX = 0.1f;
	if ( fExtentY < 0.1f ) fExtentY = 0.1f;
	if ( fExtentZ < 0.1f ) fExtentZ = 0.1f;

	fExtentX *= pObject->position.vecScale.x;
	fExtentY *= pObject->position.vecScale.y;
	fExtentZ *= pObject->position.vecScale.z;

	D3DXMATRIX matOffset;
	D3DXMatrixTranslation( &matOffset, fCenterX, fCenterY, fCenterZ );

	D3DXMATRIX matWorld;
	D3DXMatrixMultiply( &matWorld, &matOffset, pMesh->pFrameMatrices [ boneID ] );
	D3DXMatrixMultiply( &matWorld, &matWorld, &pObject->position.matWorld);
    NxMat34 physicsMat;
	physicsMat.setColumnMajor44(matWorld.m);

	dataOut.extent_ = NxVec3(fExtentX, fExtentY, fExtentZ);
	dataOut.mat_ = physicsMat;
	dataOut.offsetMat_ = matOffset;
	dataOut.offsetMat_._41 *= -1;
	dataOut.offsetMat_._42 *= -1;
	dataOut.offsetMat_._43 *= -1;


}





	static NxVec3 getCollisionCentre(int objectID){
		sObject *obj = dbGetObject(objectID);
		float x = obj->collision.vecMin.x + obj->collision.vecMax.x;
		float y = obj->collision.vecMin.y + obj->collision.vecMax.y;
		float z = obj->collision.vecMin.z + obj->collision.vecMax.z;
		NxVec3 centre(x / 2.0f, y / 2.0f, z / 2.0f);
		return centre;
	}

	static NxVec3 getLimbScale(int object, int limb, float scale){
		sObject *obj = dbGetObject(object);
		NxVec3 objectScale = (NxVec3)obj->position.vecScale;
		NxVec3 limbScale    = (NxVec3)obj->ppFrameList[limb]->vecScale;
		return NxVec3(scale * objectScale.x * limbScale.x,
			          scale * objectScale.y * limbScale.y,
                      scale * objectScale.z * limbScale.z);
	}
};