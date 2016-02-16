#pragma once
#ifndef DARKPHY2DBPRO
#define DARKPHY2DBPRO
// ************************* Dark Physics 2 ************************** //

// DBPro Header file, creates global function pointers for dbPro commands
// to match GDK commands. Exports DarkPhysics2 functions when compiling
// as dll.

//#include "DBOData.h"
#include "Globals.h"


// Easy export of functions
#define EXPORT __declspec(dllexport)
#define EXPORTC extern "C" __declspec(dllexport)


////TESTING
//
//typedef sObject* (__cdecl *dbGetObjectPtr_t) (int);
//typedef void (__cdecl *dbGetObjectWorldMatrixPtr_t) (int, int, D3DXMATRIX*);
//typedef void (__cdecl *dbPositionObjectPtr_t) (int, float, float, float);
//typedef void (__cdecl *dbRotateObjectPtr_t) (int, float, float, float);
//
//class DBPlugin
//{
//public:
//     static GlobStruct* Core;
//     static HMODULE Basic3D;
//
//     static dbGetObjectPtr_t dbGetObject;
//     static dbGetObjectWorldMatrixPtr_t dbGetObjectWorldMatrix;
//     static dbPositionObjectPtr_t dbPositionObject;
//     static dbRotateObjectPtr_t dbRotateObject;
//
//	 static EXPORT void ReceiveCoreDataPtr(LPVOID CorePtr)
//	 {
//		Core = (GlobStruct*)CorePtr;
//		//Basic3D = LoadLibrary( (LPCWSTR)Dependencies[0] );
//		Basic3D = Core->g_Basic3D;
//
//
//		// Populate the function pointers for your DLL's
//		dbGetObject = (dbGetObjectPtr_t)GetProcAddress(Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z");
//		dbGetObjectWorldMatrix = (dbGetObjectWorldMatrixPtr_t)GetProcAddress(Basic3D, "?GetWorldMatrix@@YAXHHPAUD3DXMATRIX@@@Z");
//		dbPositionObject = (dbPositionObjectPtr_t)GetProcAddress(Basic3D, "?Position@@YAXHMMM@Z");
//		dbRotateObject = (dbRotateObjectPtr_t)GetProcAddress(Basic3D, "?Rotate@@YAXHMMM@Z");
//	 }
//}





// A few global variables...
//extern 
GlobStruct* Core = 0;
//extern 
HMODULE Basic3D = 0;
HMODULE Basic2D = 0;
HMODULE CameraMod = 0;
HMODULE VectorsMod = 0;

HMODULE g_hLib = 0;
HMODULE CoreMod = 0;

dbGetDirect3DDevicePtr_t dbGetDirect3DDevice = 0;
dbGetObjectPtr_t dbGetObject = 0;
dbGetObjectWorldMatrixPtr_t dbGetObjectWorldMatrix = 0;
dbSetObjectWorldMatrixPtr_t dbSetObjectWorldMatrix = 0;
dbPositionObjectPtr_t dbPositionObject = 0;
dbRotateObjectPtr_t dbRotateObject = 0;
dbObjectExistPtr_t dbObjectExist = 0;
dbLimbExistPtr_t dbLimbExist = 0;
dbLimbCountPtr_t dbLimbCount = 0;
dbLockVertexDataForLimbPtr_t dbLockVertexDataForLimb = 0;
dbGetVertexDataVertexCountPtr_t dbGetVertexDataVertexCount = 0;
dbGetVertexDataIndexCountPtr_t dbGetVertexDataIndexCount = 0;
dbUnlockVertexDataPtr_t dbUnlockVertexData = 0;
dbSetVertexDataPositionPtr_t dbSetVertexDataPosition = 0;
//NEW
dbGetVertexDataPositionXPtr_t dbGetVertexDataPositionX = 0;
dbGetVertexDataPositionYPtr_t dbGetVertexDataPositionY = 0;
dbGetVertexDataPositionZPtr_t dbGetVertexDataPositionZ = 0;
dbMakeObjectTrianglePtr_t dbMakeObjectTriangle = 0;
dbMakeMeshFromObjectPtr_t dbMakeMeshFromObject = 0;
dbAddLimbPtr_t dbAddLimb = 0;
dbDeleteMeshPtr_t dbDeleteMesh = 0;
dbDeleteObjectPtr_t dbDeleteObject = 0;
dbMakeObjectPtr_t dbMakeObject = 0;
dbGetProjectionMatrixPtr_t dbGetProjectionMatrix = 0;
dbGetViewMatrixPtr_t dbGetViewMatrix = 0;
dbSetVector3Ptr_t dbSetVector3 = 0;

dbScreenWidthPtr_t dbScreenWidth = 0;
dbScreenHeightPtr_t dbScreenHeight = 0;

dbMoveObjectDownPtr_t dbMoveObjectDown = 0;
dbTurnObjectRightPtr_t dbTurnObjectRight = 0;
dbPitchObjectDownPtr_t dbPitchObjectDown = 0;

dbXVector3Ptr_t dbXVector3 = 0;
dbYVector3Ptr_t dbYVector3 = 0;
dbZVector3Ptr_t dbZVector3 = 0;

dbSetVertexDataNormalsPtr_t dbSetVertexDataNormals = 0;
dbSetVertexDataUVPtr_t dbSetVertexDataUV = 0;
dbSetIndexDataPtr_t dbSetIndexData = 0;
dbGetVertexDataUPtr_t dbGetVertexDataU = 0;
dbGetVertexDataVPtr_t dbGetVertexDataV = 0;

//new
dbMakeObjectPlainPtr_t dbMakeObjectPlain = 0;
dbCreateMeshForObjectPtr_t dbCreateMeshForObject = 0;
dbConvertObjectFVFPtr_t dbConvertObjectFVF = 0;
dbSetVertexDataDiffusePtr_t dbSetVertexDataDiffuse = 0;
dbRGBPtr_t dbRGB = 0;
dbMeshExistPtr_t dbMeshExist = 0;
dbAddMeshToVertexDataPtr_t dbAddMeshToVertexData = 0;

//newest
dbCameraPositionXPtr_t dbCameraPositionX = 0;
dbCameraPositionYPtr_t dbCameraPositionY = 0;
dbCameraPositionZPtr_t dbCameraPositionZ = 0;
dbPointObjectPtr_t dbPointObject = 0;
dbShowObjectPtr_t dbShowObject = 0;
dbHideObjectPtr_t dbHideObject = 0;
dbFadeObjectPtr_t dbFadeObject = 0;

dbScaleObjectPtr_t dbScaleObject = 0;

dbSetAlphaMappingOnPtr_t dbSetAlphaMappingOn = 0;
dbPointCameraPtr_t dbPointCamera = 0;
dbPositionCameraPtr_t dbPositionCamera = 0;

//EXPORT void ReceiveCoreDataPtr(LPVOID CorePtr);

// We want the core pointer, just because...
EXPORT void ReceiveCoreDataPtr(LPVOID CorePtr)
{
    Core = (GlobStruct*)CorePtr;

	
	//Access to D3D Device
	char cPath[_MAX_PATH];
	strcpy(cPath, Core->pEXEUnpackDirectory);
	strcat(cPath, "\\DBProSetupDebug.dll");
	g_hLib = LoadLibrary(cPath);
	
    // Get the Basic3D handle here
    //Basic3D = LoadLibrary(Dependencies[0] );
	//Basic3D = LoadLibrary("DBProBasic3DDebug.dll");

	//Access to Basic3D handle
	char cPath2[_MAX_PATH];
	strcpy(cPath2, Core->pEXEUnpackDirectory);
	strcat(cPath2, "\\DBProBasic3DDebug.dll");
	Basic3D = LoadLibrary(cPath2);

	//Access to Camera handle
	char cPath3[_MAX_PATH];
	strcpy(cPath3, Core->pEXEUnpackDirectory);
	strcat(cPath3, "\\DBProCameraDebug.dll");
	CameraMod = LoadLibrary(cPath3);

	//Access to Vectors handle
	char cPath4[_MAX_PATH];
	strcpy(cPath4, Core->pEXEUnpackDirectory);
	strcat(cPath4, "\\DBProVectorsDebug.dll");
	VectorsMod = LoadLibrary(cPath4);

	char cPath5[_MAX_PATH];
	strcpy(cPath5, Core->pEXEUnpackDirectory);
	strcat(cPath5, "\\DBProCore.dll");
	CoreMod = LoadLibrary(cPath5);

	//Access to Basic2D handle
	char cPath6[_MAX_PATH];
	strcpy(cPath6, Core->pEXEUnpackDirectory);
	strcat(cPath6, "\\DBProBasic2DDebug.dll");
	Basic2D = LoadLibrary(cPath6);

	//Basic3D = Core->g_Basic3D;
    // NOTE, this particular library is also available via Core->g_Basic3D
    // If you use that instead, then you don't need to unload the DLL later.
    // However, since this is a general example that you can use to load any
    // DLL, I'll stick with LoadLibrary.

    // Populate the function pointers for your DLL's
    dbGetDirect3DDevice = (dbGetDirect3DDevicePtr_t)GetProcAddress(g_hLib, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ");
	dbGetObject = (dbGetObjectPtr_t)GetProcAddress(Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z");
    dbGetObjectWorldMatrix = (dbGetObjectWorldMatrixPtr_t)GetProcAddress(Basic3D, "?GetWorldMatrix@@YAXHHPAUD3DXMATRIX@@@Z");
    dbSetObjectWorldMatrix = (dbSetObjectWorldMatrixPtr_t)GetProcAddress(Basic3D, "?SetWorldMatrix@@YAXHPAUD3DXMATRIX@@@Z");
	dbPositionObject = (dbPositionObjectPtr_t)GetProcAddress(Basic3D, "?Position@@YAXHMMM@Z");
    dbRotateObject = (dbRotateObjectPtr_t)GetProcAddress(Basic3D, "?Rotate@@YAXHMMM@Z");
    dbLimbExist = (dbLimbExistPtr_t)GetProcAddress(Basic3D, "?GetLimbExist@@YAHHH@Z");
    dbObjectExist = (dbObjectExistPtr_t)GetProcAddress(Basic3D, "?GetExist@@YAHH@Z");
	dbLimbCount = (dbLimbCountPtr_t)GetProcAddress(Basic3D, "?GetLimbCount@@YAHH@Z");

	dbLockVertexDataForLimb = (dbLockVertexDataForLimbPtr_t)GetProcAddress(Basic3D, "?LockVertexDataForLimb@@YAXHH@Z");
	dbGetVertexDataVertexCount = (dbGetVertexDataVertexCountPtr_t)GetProcAddress(Basic3D, "?GetVertexDataVertexCount@@YAHXZ");
	dbGetVertexDataIndexCount = (dbGetVertexDataIndexCountPtr_t)GetProcAddress(Basic3D, "?GetVertexDataIndexCount@@YAHXZ");
	dbUnlockVertexData = (dbUnlockVertexDataPtr_t)GetProcAddress(Basic3D, "?UnlockVertexData@@YAXXZ");
	dbSetVertexDataPosition = (dbSetVertexDataPositionPtr_t)GetProcAddress(Basic3D, "?SetVertexDataPosition@@YAXHMMM@Z");

	dbGetVertexDataPositionX  = (dbGetVertexDataPositionXPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionX@@YAKH@Z");
	dbGetVertexDataPositionY  = (dbGetVertexDataPositionYPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionY@@YAKH@Z");
	dbGetVertexDataPositionZ  = (dbGetVertexDataPositionZPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionZ@@YAKH@Z");
	dbMakeObjectTriangle  = (dbMakeObjectTrianglePtr_t)GetProcAddress(Basic3D, "?MakeTriangle@@YAXHMMMMMMMMM@Z");
	dbMakeMeshFromObject = (dbMakeMeshFromObjectPtr_t)GetProcAddress(Basic3D, "?MakeMeshFromObject@@YAXHHH@Z");
	dbAddLimb = (dbAddLimbPtr_t)GetProcAddress(Basic3D, "?AddLimb@@YAXHHH@Z");
	dbDeleteMesh = (dbDeleteMeshPtr_t)GetProcAddress(Basic3D, "?DeleteMesh@@YAXH@Z"); 
	dbDeleteObject = (dbDeleteObjectPtr_t)GetProcAddress(Basic3D, "?DeleteObject@@YA_NH@Z");
	dbMakeObject = (dbMakeObjectPtr_t)GetProcAddress(Basic3D, "?MakeObject@@YAXHHH@Z");

    dbGetProjectionMatrix = (dbGetProjectionMatrixPtr_t)GetProcAddress(CameraMod, "?GetProjectionMatrix@@YA?AUD3DXMATRIX@@H@Z");
    dbGetViewMatrix = (dbGetViewMatrixPtr_t)GetProcAddress(CameraMod, "?GetViewMatrix@@YA?AUD3DXMATRIX@@H@Z");
	dbSetVector3 = (dbSetVector3Ptr_t)GetProcAddress(VectorsMod, "?SetVector3@@YAXHMMM@Z");

	dbScreenWidth = (dbScreenWidthPtr_t)GetProcAddress(g_hLib, "?GetDisplayWidth@@YAHXZ");
	dbScreenHeight = (dbScreenHeightPtr_t)GetProcAddress(g_hLib, "?GetDisplayHeight@@YAHXZ");
    
	dbMoveObjectDown = (dbMoveObjectDownPtr_t)GetProcAddress(Basic3D, "?MoveDown@@YAXHM@Z");
	dbTurnObjectRight = (dbTurnObjectRightPtr_t)GetProcAddress(Basic3D, "?TurnRight@@YAXHM@Z");
	dbPitchObjectDown = (dbPitchObjectDownPtr_t)GetProcAddress(Basic3D, "?PitchDown@@YAXHM@Z");

    dbXVector3 = (dbXVector3Ptr_t)GetProcAddress(VectorsMod, "?GetXVector3@@YAKH@Z"); 
	dbYVector3 = (dbYVector3Ptr_t)GetProcAddress(VectorsMod, "?GetYVector3@@YAKH@Z"); 
	dbZVector3 = (dbZVector3Ptr_t)GetProcAddress(VectorsMod, "?GetZVector3@@YAKH@Z"); 

	dbSetVertexDataNormals = (dbSetVertexDataNormalsPtr_t)GetProcAddress(Basic3D,"?SetVertexDataNormals@@YAXHMMM@Z");
	dbSetVertexDataUV = (dbSetVertexDataUVPtr_t)GetProcAddress(Basic3D, "?SetVertexDataUV@@YAXHMM@Z");
	dbSetIndexData = (dbSetIndexDataPtr_t)GetProcAddress(Basic3D, "?SetIndexData@@YAXHH@Z");
	dbGetVertexDataU = (dbGetVertexDataUPtr_t)GetProcAddress(Basic3D, "?GetVertexDataU@@YAKH@Z");
	dbGetVertexDataV = (dbGetVertexDataVPtr_t)GetProcAddress(Basic3D, "?GetVertexDataV@@YAKH@Z"); 

	dbMakeObjectPlain = (dbMakeObjectPlainPtr_t)GetProcAddress(Basic3D, "?MakePlane@@YAXHMM@Z");
	dbCreateMeshForObject = (dbCreateMeshForObjectPtr_t)GetProcAddress(Basic3D, "?CreateMeshForObject@@YAXHKKK@Z");
	dbConvertObjectFVF = (dbConvertObjectFVFPtr_t)GetProcAddress(Basic3D, "?ConvertToFVF@@YAXPAUsMesh@@K@Z");
	dbSetVertexDataDiffuse = (dbSetVertexDataDiffusePtr_t)GetProcAddress(Basic3D, "?SetVertexDataDiffuse@@YAXHK@Z");
	dbRGB = (dbRGBPtr_t)GetProcAddress(Basic2D, "?Rgb@@YAKHHH@Z");
	dbMeshExist = (dbMeshExistPtr_t)GetProcAddress(Basic3D, "?GetMeshExist@@YAHH@Z");
	dbAddMeshToVertexData = (dbAddMeshToVertexDataPtr_t)GetProcAddress(Basic3D, "?AddMeshToVertexData@@YAXH@Z");

	//new
    dbCameraPositionX = (dbCameraPositionXPtr_t)GetProcAddress(CameraMod, "?GetXPositionEx@@YAKH@Z");  
	dbCameraPositionY = (dbCameraPositionYPtr_t)GetProcAddress(CameraMod, "?GetYPositionEx@@YAKH@Z"); 
	dbCameraPositionZ = (dbCameraPositionZPtr_t)GetProcAddress(CameraMod, "?GetZPositionEx@@YAKH@Z"); 
	dbPointObject = (dbPointObjectPtr_t)GetProcAddress(Basic3D, "?Point@@YAXHMMM@Z"); 
	dbShowObject = (dbShowObjectPtr_t)GetProcAddress(Basic3D, "?Show@@YAXH@Z");  
	dbHideObject = (dbHideObjectPtr_t)GetProcAddress(Basic3D, "?Hide@@YAXH@Z"); 
	dbFadeObject = (dbFadeObjectPtr_t)GetProcAddress(Basic3D, "?Fade@@YAXHM@Z"); 

    dbScaleObject = (dbScaleObjectPtr_t)GetProcAddress(Basic3D, "?Scale@@YAXHMMM@Z");

	dbSetAlphaMappingOn = (dbSetAlphaMappingOnPtr_t)GetProcAddress(Basic3D, "?SetAlphaFactor@@YAXHM@Z");
	//dbTimer = (dbTimerPtr_t)GetProcAddress(g_hLib, "?Timer@@YGMW4TIMER_COMMAND@@@Z"); 
	dbPointCamera = (dbPointCameraPtr_t)GetProcAddress( CameraMod, "?Point@@YAXHMMM@Z");
    dbPositionCamera = (dbPositionCameraPtr_t)GetProcAddress(CameraMod, "?Position@@YAXHMMM@Z");
	// OV getD3DDevice = (OV) GetProcAddress(g_hLib, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ");
    // Using the UNDNAME facility on the string '?GetObjectA@@YAPAUsObject@@H@Z' returns this:
    // struct sObject * __cdecl GetObjectA(int)
    // This matches the function pointer type GetObjectPtr_t defined above.
}












// Instruct DBPro that we need the Basic3D plugin for this one to work
//const char* Dependencies[] =
//{
//    "DBProBasic3DDebug.dll",
//};


//EXPORT int GetNumDependencies( void )
//{
//    return sizeof(Dependencies) / sizeof(const char*);
//}


//EXPORT char const* GetDependencyID(int ID)
//{
//    return Dependencies[ID];
//}


//EXPORT void PreDestructor()
//{
//    // Free the external DLL's here if you have loaded any
//    // FreeLibrary( Basic3D );
//}



//EXPORT int GetLimbCount(int ObjectId)
//{
//    sObject* Obj = dbGetObject(ObjectId);
//
//    // NULL pointer = no object
//    if (! Obj)
//        return -1;
//
//    return Obj->iFrameCount;  // Number of limbs
//}



//EXPORT inline int   start(void);
//EXPORT inline void  stop(void);
//EXPORT inline int   getCurrentScene(void);
//EXPORT inline void  setCurrentScene(int sceneID);
//EXPORT inline void  makeScene(int sceneID);
//EXPORT inline void  deleteScene(int sceneID);
//EXPORT inline int   getSceneCount(void);

//EXPORT void Constructor ( void )
//{
//   //phy = new Physics(); // now done in start()
//   // Create memory here
//}

//EXPORT void Destructor ( void )
//{
//   //If user does not call stop, release physics
//   //phy->stop();
//   //delete phy;   //now done in stop()
//}

#endif