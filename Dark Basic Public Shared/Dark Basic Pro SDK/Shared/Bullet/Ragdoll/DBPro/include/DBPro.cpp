//
// DBPro Mappings 
//

#include "DBPro.hpp"

// need proper resource handling here!!
int iStartObjectOffset = 1000;
int iStartMeshOffset = 1000;

namespace DBPro
{
	int FindFreeObject ( void )
	{
		int iObj = iStartObjectOffset;
		while ( iObj )
		{
			if ( ObjectExist ( iObj )==NULL )
				return iObj;
			iObj++;
		}
		return -1;
	}

	int FindFreeMesh ( void )
	{
		iStartMeshOffset++;
		return iStartMeshOffset;
	}

	void ReportError ( LPCSTR pErr1, LPSTR pErr2 )
	{
		MessageBox ( NULL, pErr1, pErr2, MB_OK );
	}

	int VectorExist ( int iVector )
	{
		typedef int(*t_dbVectorExist)(int);
		return ((t_dbVectorExist)GetProcAddress( g_pGlob->g_Vectors , "?GetExist@@YAHH@Z" ))(iVector);
	}

	float XVector2 ( int iVector )
	{
		typedef DWORD(*t_dbXVector2)(int);
		DWORD dwReturn = ((t_dbXVector2)GetProcAddress( g_pGlob->g_Vectors , "?GetXVector2@@YAKH@Z" ))(iVector);
		return *( float* ) &dwReturn;
	}

	float YVector2 ( int iVector )
	{
		typedef DWORD(*t_dbYVector2)(int);
		DWORD dwReturn = ((t_dbYVector2)GetProcAddress( g_pGlob->g_Vectors , "?GetYVector2@@YAKH@Z" ))(iVector);
		return *( float* ) &dwReturn;
	}

	float XVector3 ( int iVector )
	{
		typedef DWORD(*t_dbXVector3)(int);
		DWORD dwReturn = ((t_dbXVector3)GetProcAddress( g_pGlob->g_Vectors , "?GetXVector3@@YAKH@Z" ))(iVector);
		return *( float* ) &dwReturn;
	}

	float YVector3 ( int iVector )
	{
		typedef DWORD(*t_dbXVector3)(int);
		DWORD dwReturn = ((t_dbXVector3)GetProcAddress( g_pGlob->g_Vectors , "?GetYVector3@@YAKH@Z" ))(iVector);
		return *( float* ) &dwReturn;
	}

	float ZVector3 ( int iVector )
	{
		typedef DWORD(*t_dbXVector3)(int);
		DWORD dwReturn = ((t_dbXVector3)GetProcAddress( g_pGlob->g_Vectors , "?GetZVector3@@YAKH@Z" ))(iVector);
		return *( float* ) &dwReturn;
	}

	void PositionObject ( int oObj, float fX, float fY, float fZ )
	{
		typedef void(*t_dbPositionObject)(int,float,float,float);
		((t_dbPositionObject)GetProcAddress( g_pGlob->g_Basic3D , "?Position@@YAXHMMM@Z" ))(oObj, fX, fY, fZ);
	}

	void RotateObject ( int oObj, float fX, float fY, float fZ )
	{
		typedef void(*t_dbRotateObject)(int,float,float,float);
		((t_dbRotateObject)GetProcAddress( g_pGlob->g_Basic3D , "?Rotate@@YAXHMMM@Z" ))(oObj, fX, fY, fZ);
	}

	void HideObject ( int oObj )
	{
		typedef void(*t_dbHideObject)(int);
		((t_dbHideObject)GetProcAddress( g_pGlob->g_Basic3D , "?Hide@@YAXH@Z" ))(oObj);
	}

	void ShowObject ( int oObj )
	{
		typedef void(*t_dbShowObject)(int);
		((t_dbShowObject)GetProcAddress( g_pGlob->g_Basic3D , "?Show@@YAXH@Z" ))(oObj);
	}

	int ObjectExist ( int oObj )
	{
		typedef int(*t_dbObjectExist)(int);
		return ((t_dbObjectExist)GetProcAddress( g_pGlob->g_Basic3D , "?GetExist@@YAHH@Z" ))(oObj);
	}

	sObject* GetObjectData ( int iObj )
	{
		typedef sObject*(*t_dbGetObject)(long);
		return ((t_dbGetObject)GetProcAddress( g_pGlob->g_Basic3D , "?GetObjectA@@YAPAUsObject@@H@Z" ))(iObj);
	}

	void MakeObject ( int iObj, int iData1, int iData2 )
	{
		typedef void(*t_dbMakeObject)(int,int,int);
		((t_dbMakeObject)GetProcAddress( g_pGlob->g_Basic3D , "?MakeObject@@YAXHHH@Z" ))(iObj, iData1, iData2);
	}

	void MakeObjectSphere ( int iObj, float fS, int i1, int i2 )
	{
		typedef void(*t_dbMakeObjectSphere)(int,float,int,int);
		((t_dbMakeObjectSphere)GetProcAddress( g_pGlob->g_Basic3D , "?MakeSphere@@YAXHMHH@Z" ))(iObj, fS, i1, i2);
	}

	void MakeMeshFromObject ( int iMeshID, int iObj )
	{
		typedef void(*t_dbMakeMeshFromObject)(int,int);
		((t_dbMakeMeshFromObject)GetProcAddress( g_pGlob->g_Basic3D , "?MakeMeshFromObject@@YAXHH@Z" ))(iMeshID, iObj);
	}

	void DeleteObject ( int iObj )
	{
		typedef void(*t_dbDeleteObject)(int);
		((t_dbDeleteObject)GetProcAddress( g_pGlob->g_Basic3D , "?DeleteEx@@YAXH@Z" ))(iObj);
	}

	void PointObject ( int iObj, float f1, float f2, float f3 )
	{
		typedef void(*t_dbPointObject)(int,float,float,float);
		((t_dbPointObject)GetProcAddress( g_pGlob->g_Basic3D , "?Point@@YAXHMMM@Z" ))(iObj, f1, f2, f3);
	}

	void DeleteMesh ( int iMeshID )
	{
		typedef void(*t_dbDeleteMesh)(int);
		((t_dbDeleteMesh)GetProcAddress( g_pGlob->g_Basic3D , "?DeleteMesh@@YAXH@Z" ))(iMeshID);
	}

	float ObjectPositionX ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetXPositionEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectPositionY ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetYPositionEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectPositionZ ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetZPositionEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectAngleX ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetXRotationEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectAngleY ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetYRotationEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectAngleZ ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetZRotationEx@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectScaleX ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetXSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectScaleY ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetYSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectScaleZ ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetZSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectSizeX ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetXSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectSizeY ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetYSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float ObjectSizeZ ( int iObj )
	{
		typedef DWORD(*t_dbObjectPosition)(int);
		DWORD dwReturn = ((t_dbObjectPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetZSize@@YAKH@Z" ))(iObj);
		return *( float* ) &dwReturn;
	}

	float LimbPositionX ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbXPosition@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	float LimbPositionY ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbYPosition@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	float LimbPositionZ ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbZPosition@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	float LimbDirectionX ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbXDirection@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	float LimbDirectionY ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbYDirection@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	float LimbDirectionZ ( int iObj, int iLimbID )
	{
		typedef DWORD(*t_dbLimbPosition)(int,int);
		DWORD dwReturn = ((t_dbLimbPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbZDirection@@YAKHH@Z" ))(iObj,iLimbID);
		return *( float* ) &dwReturn;
	}

	void LockVertexdataForLimb ( int i1, int i2, int i3 )
	{
		typedef void(*t_dbLockVertexdataForLimb)(int,int,int);
		((t_dbLockVertexdataForLimb)GetProcAddress( g_pGlob->g_Basic3D , "?LockVertexDataForLimbCore@@YAXHHH@Z" ))(i1, i2, i3);
	}

	void LockVertexdataForLimb ( int i1, int i2 )
	{
		typedef void(*t_dbLockVertexdataForLimb)(int,int);
		((t_dbLockVertexdataForLimb)GetProcAddress( g_pGlob->g_Basic3D , "?LockVertexDataForLimb@@YAXHH@Z" ))(i1, i2);
	}

	void SetVertexdataPosition ( int iI, float f1, float f2, float f3 )
	{
		typedef void(*t_dbSetVertexdataPosition)(int,float,float,float);
		((t_dbSetVertexdataPosition)GetProcAddress( g_pGlob->g_Basic3D , "?SetVertexDataPosition@@YAXHMMM@Z" ))(iI, f1, f2, f3);
	}

	int GetVertexdataVertexCount ( void )
	{
		typedef int(*t_dbGetVertexdataVertexCount)(void);
		return ((t_dbGetVertexdataVertexCount)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataVertexCount@@YAHXZ" ))();
	}

	float GetVertexdataPositionX ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataPosition)(int);
		DWORD dwReturn = ((t_dbGetVertexdataPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataPositionX@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataPositionY ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataPosition)(int);
		DWORD dwReturn = ((t_dbGetVertexdataPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataPositionY@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataPositionZ ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataPosition)(int);
		DWORD dwReturn = ((t_dbGetVertexdataPosition)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataPositionZ@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataU ( int iI, int i2 )
	{
		typedef DWORD(*t_dbGetVertexdata)(int,int);
		DWORD dwReturn = ((t_dbGetVertexdata)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataU@@YAKH@Z" ))(iI,i2);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataV ( int iI, int i2 )
	{
		typedef DWORD(*t_dbGetVertexdata)(int,int);
		DWORD dwReturn = ((t_dbGetVertexdata)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataV@@YAKH@Z" ))(iI,i2);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataNormalsX ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataNormals)(int);
		DWORD dwReturn = ((t_dbGetVertexdataNormals)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataNormalsX@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataNormalsY ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataNormals)(int);
		DWORD dwReturn = ((t_dbGetVertexdataNormals)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataNormalsY@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	float GetVertexdataNormalsZ ( int iI )
	{
		typedef DWORD(*t_dbGetVertexdataNormals)(int);
		DWORD dwReturn = ((t_dbGetVertexdataNormals)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataNormalsZ	@@YAKH@Z" ))(iI);
		return *( float* ) &dwReturn;
	}

	int GetVertexdataIndexCount ( void )
	{
		typedef int(*t_dbGetVertexdataIndexCount)(void);
		return ((t_dbGetVertexdataIndexCount)GetProcAddress( g_pGlob->g_Basic3D , "?GetVertexDataIndexCount@@YAHXZ" ))();
	}

	int GetIndexdata ( int i1 )
	{
		typedef int(*t_dbGetIndexdata)(int);
		return ((t_dbGetIndexdata)GetProcAddress( g_pGlob->g_Basic3D , "?GetIndexData@@YAHH@Z" ))(i1);
	}

	void UnlockVertexdata ( void )
	{
		typedef void(*t_dbUnlockVertexdata)(void);
		((t_dbUnlockVertexdata)GetProcAddress( g_pGlob->g_Basic3D , "?UnlockVertexData@@YAXXZ" ))();
	}

	int GetLimbCount ( int i1 )
	{
		typedef int(*t_dbGetLimbCount)(int);
		return ((t_dbGetLimbCount)GetProcAddress( g_pGlob->g_Basic3D , "?GetLimbCount@@YAHH@Z" ))(i1);
	}

	bool CalculateMeshBounds ( sMesh* pMesh )
	{
		typedef bool(*t_dbCalculateMeshBounds)(sMesh*);
		return ((t_dbCalculateMeshBounds)GetProcAddress( g_pGlob->g_Basic3D , "?CalculateMeshBounds@@YA_NPAUsMesh@@@Z" ))(pMesh);
	}

}
