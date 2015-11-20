
// BulletPhysicsWrapper for DarkBasic Professional
//Stab In The Dark Software 
//Copyright (c) 2002-2014
//http://stabinthedarksoftware.com

///#include "StdAfx.h"
#include "DBProToBullet.h"
#include "DBPro.hpp"

namespace DBProToBullet
{
	btScalar DBProToBullet::GetObjectDiameter(int objectID)
	{
		btVector3 sizeVec = GetObjectSize(objectID);
		btScalar diameter = sizeVec.getX();
		if(diameter < sizeVec.getY())
			diameter = sizeVec.getY();
		else if (diameter < sizeVec.getZ())
			diameter = sizeVec.getZ();
		return diameter;
	}
	btVector3 DBProToBullet::GetObjectSize(int objectID)
	{
		return btVector3(DBPro::ObjectSizeX(objectID), DBPro::ObjectSizeY(objectID), DBPro::ObjectSizeZ(objectID));
	}

	DBProToBullet::DBProVertexData* GetVertexData(int objectID, btScalar scaleFactor, bool bTransform, bool bReverseVertexOrder, bool bMirrorOnXAxis) 
	{
		DBProToBullet::DBProVertexData* vertexData = new DBProToBullet::DBProVertexData();
		int prevIndexCount = 0;
		btTransform objectTrans = DBProToBullet::GetTransform(objectID);
		btVector3 objectScale = DBProToBullet::GetScale(objectID); 
		int limbCount = DBPro::GetLimbCount(objectID);
		for(int limbNumber = 0; limbNumber < limbCount; limbNumber++)
		{
			DBPro::LockVertexdataForLimb(objectID,limbNumber);
			int vertexCount = DBPro::GetVertexdataVertexCount();
			int indexCount = DBPro::GetVertexdataIndexCount();
			for(int i = 0; i < vertexCount; i++)
			{
				btVector3 vertex(DBPro::GetVertexdataPositionX(i), DBPro::GetVertexdataPositionY(i), DBPro::GetVertexdataPositionZ(i));
				vertex = vertex * objectScale;//handle DBPro scaling
				vertex = vertex / scaleFactor;
				vertex = bTransform ? objectTrans * vertex : vertex;
				vertex = bMirrorOnXAxis ? vertex * btVector3(-1.0,1.0,1.0) : vertex;
				//
				btVector3 normal(DBPro::GetVertexdataNormalsX(i), DBPro::GetVertexdataNormalsY(i), DBPro::GetVertexdataNormalsZ(i));	
				//
				btVector3 uv(DBPro::GetVertexdataU(i,0), DBPro::GetVertexdataV(i,0), 0.0);
				//
				vertexData->vertexBuffer.push_back(vertex);
				vertexData->normals.push_back(normal);
				vertexData->uvData.push_back(uv);
			}
			for(int i = 0; i < indexCount; i +=3)
			{
				if(bReverseVertexOrder)
				{
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i+2)+prevIndexCount);
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i+1)+prevIndexCount);
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i)+prevIndexCount);
				}
				else
				{
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i)+prevIndexCount);
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i+1)+prevIndexCount);
					vertexData->indexBuffer.push_back(DBPro::GetIndexdata(i+2)+prevIndexCount);
				}
			}
			prevIndexCount += indexCount;
			DBPro::UnlockVertexdata();
		}
		return vertexData;
	}

	//void GetVertexData(int objectID, btAlignedObjectArray<btVector3> & outObjVerts,
	//	btAlignedObjectArray<btVector3> &outObjUVData,  btAlignedObjectArray<btVector3> & outObjNormals, 
	//	bool bTransform, bool bMirrorXAxis /*=false*/)
	//{
	//	btTransform objectTrans;
	//	objectTrans = DBProToBullet::GetTransform(objectID);

	//	for(int limb = 0; limb < DBPro::GetLimbCount(objectID); limb++)
	//	{
	//		DBPro::LockVertexdataForLimb(objectID,limb);
	//		for(int i = 0; i < DBPro::GetVertexdataVertexCount(); i++)
	//		{
	//			btVector3 positionVec;
	//			btVector3 normalVec;
	//			btVector3 uvVec;
	//			if(bMirrorXAxis)
	//			{
	//				positionVec = btVector3(DBPro::GetVertexdataPositionX(i)*-1, DBPro::GetVertexdataPositionY(i), DBPro::GetVertexdataPositionZ(i));
	//			}
	//			else
	//			{
	//				positionVec = btVector3(DBPro::GetVertexdataPositionX(i), DBPro::GetVertexdataPositionY(i), DBPro::GetVertexdataPositionZ(i));
	//			}

	//			normalVec = btVector3(DBPro::GetVertexdataNormalsX(i), DBPro::GetVertexdataNormalsY(i), DBPro::GetVertexdataNormalsZ(i));				
	//			uvVec = btVector3(DBPro::GetVertexdataU(i,0), DBPro::GetVertexdataV(i,0), 0.0);
	//			if(bTransform == true)
	//			{
	//				positionVec = objectTrans * positionVec;
	//			}
	//			outObjVerts.push_back(positionVec);
	//			outObjNormals.push_back(normalVec);
	//			outObjUVData.push_back(uvVec);
	//		}
	//		DBPro::UnlockVertexdata();
	//	}
	//}

	/*void GetIndexData(int objectID, btAlignedObjectArray<int> & outObjIndices, bool bReverseOrder)
	{
		int prevIndexCount = 0;
		for(int limb = 0; limb < DBPro::GetLimbCount(objectID); limb++)
		{
			DBPro::LockVertexdataForLimb(objectID,limb);
			for(int i = 0; i < DBPro::GetVertexdataIndexCount(); i +=3)
			{
				if(bReverseOrder == true)
				{
					outObjIndices.push_back(DBPro::GetIndexdata(i+2)+prevIndexCount);
					outObjIndices.push_back(DBPro::GetIndexdata(i+1)+prevIndexCount);
					outObjIndices.push_back(DBPro::GetIndexdata(i)+prevIndexCount);
				}
				else
				{
					outObjIndices.push_back(DBPro::GetIndexdata(i)+prevIndexCount);
					outObjIndices.push_back(DBPro::GetIndexdata(i+1)+prevIndexCount);
					outObjIndices.push_back(DBPro::GetIndexdata(i+2)+prevIndexCount);
				}
			}
			prevIndexCount += DBPro::GetVertexdataIndexCount();
			DBPro::UnlockVertexdata();
		}
	}*/

	btVector3 DBProToBullet::GetScale(int objectID)
	{
		return btVector3(DBPro::ObjectScaleX(objectID)/100.0, DBPro::ObjectScaleY(objectID)/100.0, DBPro::ObjectScaleZ(objectID)/100.0);
	}

	//Note: Transform does not include scaling
	btTransform DBProToBullet::GetTransform(int objectID, btScalar scaleFactor/* = 1.0*/)
	{
		btTransform objTransform;
		objTransform.setIdentity();
		objTransform.setOrigin(btVector3(DBPro::ObjectPositionX(objectID)/scaleFactor, DBPro::ObjectPositionY(objectID)/scaleFactor, DBPro::ObjectPositionZ(objectID)/scaleFactor));
		//Using a btMatrix setEulerYPR must take it ZYX for correct rotations in DBPro.
		btMatrix3x3 btMatrix;
		btMatrix.setEulerYPR(btScalar(btRadians(DBPro::ObjectAngleZ(objectID))),
										 btScalar(btRadians(DBPro::ObjectAngleY(objectID))),
										 btScalar(btRadians(DBPro::ObjectAngleX(objectID))));
		objTransform.setBasis(btMatrix);
		return objTransform;
	}

	btVector3 DBProToBullet::GetVector3(int vectorID)
	{
		return btVector3(btScalar(DBPro::XVector3(vectorID)), btScalar(DBPro::YVector3(vectorID)), btScalar(DBPro::ZVector3(vectorID)));
	}


	void DBProToBullet::AssertValidVector(int vectorID, LPCSTR message)
	{
		if(DBPro::VectorExist(vectorID) == false)
		{
			DBPro::ReportError(message,"Bullet Physics Wrapper");
		}
	}

	void DBProToBullet::AssertValidObject(int objectID, LPCSTR message)
	{
		if(DBPro::ObjectExist(objectID) == false)
		{
			DBPro::ReportError(message,"Bullet Physics Wrapper");
		}
	}
}


