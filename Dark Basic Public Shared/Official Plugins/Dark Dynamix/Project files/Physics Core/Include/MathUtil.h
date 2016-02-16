#pragma once
#define NOMINMAX
#include <d3dx9.h>
#include "NxPhysics.h"

#pragma comment(lib, "d3dx9.lib")

class MathUtil
{
public:
    static void quaternionToEuler(NxQuat &quaternion, D3DXVECTOR3 &euler)
	{
		//quaternion.normalize();
		float GRAD_PI = 180.0f / D3DX_PI;
		NxReal w, x, y, z;
		w = quaternion.w;
		x = quaternion.x;
		y = quaternion.y;
		z = quaternion.z;
		double sqw = w * w;    
		double sqx = x * x;    
		double sqy = y * y;    
		double sqz = z * z;	
		euler.z = (NxReal) (atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))
							* GRAD_PI);	  
		euler.x = (NxReal) (atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))
							* GRAD_PI);  
		euler.y = (NxReal) (asinf(NxMath::clamp(-2.0f * (x*z - y*w), (NxF32)1.0f, (NxF32)-1.0f))  
							* GRAD_PI);
	}

	static D3DXMATRIX getScaledMatrix(D3DXMATRIX aMatrix, NxVec3 scale)
	{
		D3DXMATRIX scaleMat;
		D3DXMatrixScaling(&scaleMat, scale.x, scale.y, scale.z);
		D3DXMATRIX scaleMat_and_physXMat;
		D3DXMatrixMultiply(&scaleMat_and_physXMat, &scaleMat, &aMatrix);
		return scaleMat_and_physXMat;
	}

	static void setMatrix(D3DXMATRIX &matOut, float posX, float posY, float posZ, float rotX, float rotY, float rotZ){
        D3DXMATRIX matRotateX;
        D3DXMATRIX matRotateY;
        D3DXMATRIX matRotateZ;
		D3DXMATRIX matTranslate;
		D3DXMatrixRotationX(&matRotateX, D3DXToRadian(rotX));
		D3DXMatrixRotationY(&matRotateY, D3DXToRadian(rotY));
		D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(rotZ)); 
		D3DXMatrixTranslation(&matTranslate, posX,
			                                 posY,
                                             posZ);
		matOut = matRotateX * matRotateY * matRotateZ * matTranslate;
		
		//matOut = matTranslate * matRotateX * matRotateY * matRotateZ;
		//NOTE: limbs on models have issues when scaled non uniformly
		////Scaled: If y scale is *1.5, z rotation seems to work as / 2
		////        If y scale is * 2, z rotation seems to work as / 4
		//// 2^n
		////D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(dbLimbDirectionZ(object_, limb_) / 4));
	}

	static void setMatrix(D3DXMATRIX &matOut, float rotX, float rotY, float rotZ){
        D3DXMATRIX matRotateX;
        D3DXMATRIX matRotateY;
        D3DXMATRIX matRotateZ;
		//D3DXMATRIX matTranslate;
		D3DXMatrixRotationX(&matRotateX, D3DXToRadian(rotX));
		D3DXMatrixRotationY(&matRotateY, D3DXToRadian(rotY));
		D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(rotZ)); 
		//D3DXMatrixTranslation(&matTranslate, posX,
		//	                                 posY,
        //                                     posZ);
		matOut = matRotateX * matRotateY * matRotateZ;// * matTranslate;

		//NOTE: limbs on models have issues when scaled non uniformly
		////Scaled: If y scale is *1.5, z rotation seems to work as / 2
		////        If y scale is * 2, z rotation seems to work as / 4
		//// 2^n
		////D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(dbLimbDirectionZ(object_, limb_) / 4));
	}

	static void transformVertices(NxVec3 *vertices, unsigned int vertexCount, D3DXMATRIX &mat){
       // Create pointer to vertex data of object mesh
	  // BYTE* vertexData = aMesh->pVertexData; 
	   // Get FVF size
	  // DWORD FVFSize = aMesh->dwFVFSize;  
	   // Use loop to populate physX vertex data with our object mesh vertex data
	    D3DXVECTOR3 d3dVec;	
	    for(unsigned int j = 0; j < vertexCount; j++)
	    {
           d3dVec.x = vertices[j].x;
           d3dVec.y = vertices[j].y;
           d3dVec.z = vertices[j].z;
		   D3DXVec3TransformCoord(&d3dVec, &d3dVec, &mat);
		  // D3DXVec3Transform(&d3dVec, &d3dVec, &mat);
		   vertices[j].x = d3dVec.x;
		   vertices[j].y = d3dVec.y;
		   vertices[j].z = d3dVec.z;
		}    
	}

	static void QuatToMatrix(D3DXQUATERNION *quat, D3DXMATRIX &mat)//float m[4][4])
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		// calculate coefficients used for building the matrix
		x2 = quat->x + quat->x; y2 = quat->y + quat->y;
		z2 = quat->z + quat->z;
		xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
		yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
		wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;

		mat.m[0][0] = 1.0f - (yy + zz);
		mat.m[1][0] = xy - wz;
		mat.m[2][0] = xz + wy;
		mat.m[3][0] = 0.0f;
		mat.m[0][1] = xy + wz;
		mat.m[1][1] = 1.0f - (xx + zz);
		mat.m[2][1] = yz - wx;
		mat.m[3][1] = 0.0f;
		mat.m[0][2] = xz - wy;
		mat.m[1][2] = yz + wx;
		mat.m[2][2] = 1.0f - (xx + yy);
		mat.m[3][2] = 0.0f;
		mat.m[0][3] = 0;
		mat.m[1][3] = 0;
		mat.m[2][3] = 0;
		mat.m[3][3] = 1;
	}

	static void RemoveScaleFromMatrix(D3DXMATRIX &mat){
		D3DXVECTOR3 vecScale;
		D3DXQUATERNION quatRotation;
		D3DXVECTOR3 vecTranslation;
		D3DXMatrixDecompose(&vecScale, &quatRotation, &vecTranslation, &mat);
		D3DXMATRIX matRotate;
		D3DXMATRIX matTranslate;
		//D3DXMATRIX matResult;
		MathUtil::QuatToMatrix(&quatRotation, matRotate);
		D3DXMatrixTranslation(&matTranslate, vecTranslation.x, vecTranslation.y, vecTranslation.z);
		mat = matRotate * matTranslate;
		//mat = matResult;
	}

};
