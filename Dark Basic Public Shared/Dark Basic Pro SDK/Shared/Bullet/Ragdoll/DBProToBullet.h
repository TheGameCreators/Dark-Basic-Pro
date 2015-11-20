

#pragma once

#include "Windows.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath\btVector3.h"
#include "LinearMath\btTransform.h"

namespace DBProToBullet
{
	class DBProVertexData
	{
	public:
		DBProVertexData()
		{

		}
		~DBProVertexData()
		{
			vertexBuffer.clear();
			indexBuffer.clear();
			normals.clear();
			uvData.clear();
		}
		btAlignedObjectArray<btVector3> vertexBuffer;
		btAlignedObjectArray<int> indexBuffer;
		btAlignedObjectArray<btVector3> normals;
		btAlignedObjectArray<btVector3> uvData;
	};
	btScalar GetObjectDiameter(int objectID);
	btVector3 GetObjectSize(int objectID);
	DBProToBullet::DBProVertexData* GetVertexData(int objectID, btScalar scaleFactor, bool bTransform, bool bReverseVertexOrder, bool bMirrorOnXAxis); 
	//void GetVertexData(int objectID, btAlignedObjectArray<btVector3> & outObjVerts,
	//btAlignedObjectArray<btVector3> &outObjUVData,  btAlignedObjectArray<btVector3> & outObjNormals, 
	//bool bTransform, bool bMirrorXAxis /*=false*/);
	//void GetIndexData(int objectID, btAlignedObjectArray<int> & outObjIndices, bool bReverseOrder);
	btVector3 GetScale(int objectID); 
	btTransform GetTransform(int objectID, btScalar scaleFactor = 1.0);
	btVector3 GetVector3(int vectorID);
	void AssertValidVector(int vectorID, LPCSTR message);
	void AssertValidObject(int objectID, LPCSTR message);
}

