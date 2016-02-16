//#include "DarkPhy2(dbPro).h"
//
//
// //We want the core pointer, just because...
//EXPORT void ReceiveCoreDataPtr(LPVOID CorePtr)
//{
//    Core = (GlobStruct*)CorePtr;
//
//	Access to D3D Device
//	char cPath[_MAX_PATH];
//	strcpy(cPath, Core->pEXEUnpackDirectory);
//	strcat(cPath, "\\DBProSetupDebug.dll");
//	g_hLib = LoadLibrary(cPath);
//	
//     Get the Basic3D handle here
//    Basic3D = LoadLibrary(Dependencies[0] );
//	Basic3D = LoadLibrary("DBProBasic3DDebug.dll");
//
//	Access to Basic3D handle
//	char cPath2[_MAX_PATH];
//	strcpy(cPath2, Core->pEXEUnpackDirectory);
//	strcat(cPath2, "\\DBProBasic3DDebug.dll");
//	Basic3D = LoadLibrary(cPath2);
//
//	Access to Camera handle
//	char cPath3[_MAX_PATH];
//	strcpy(cPath3, Core->pEXEUnpackDirectory);
//	strcat(cPath3, "\\DBProCameraDebug.dll");
//	CameraMod = LoadLibrary(cPath3);
//
//	Access to Vectors handle
//	char cPath4[_MAX_PATH];
//	strcpy(cPath4, Core->pEXEUnpackDirectory);
//	strcat(cPath4, "\\DBProVectorsDebug.dll");
//	VectorsMod = LoadLibrary(cPath4);
//
//
//	Basic3D = Core->g_Basic3D;
//     NOTE, this particular library is also available via Core->g_Basic3D
//     If you use that instead, then you don't need to unload the DLL later.
//     However, since this is a general example that you can use to load any
//     DLL, I'll stick with LoadLibrary.
//
//     Populate the function pointers for your DLL's
//    dbGetDirect3DDevice = (dbGetDirect3DDevicePtr_t)GetProcAddress(g_hLib, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ");
//	dbGetObject = (dbGetObjectPtr_t)GetProcAddress(Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z");
//    dbGetObjectWorldMatrix = (dbGetObjectWorldMatrixPtr_t)GetProcAddress(Basic3D, "?GetWorldMatrix@@YAXHHPAUD3DXMATRIX@@@Z");
//    dbSetObjectWorldMatrix = (dbSetObjectWorldMatrixPtr_t)GetProcAddress(Basic3D, "?SetWorldMatrix@@YAXHPAUD3DXMATRIX@@@Z");
//	dbPositionObject = (dbPositionObjectPtr_t)GetProcAddress(Basic3D, "?Position@@YAXHMMM@Z");
//    dbRotateObject = (dbRotateObjectPtr_t)GetProcAddress(Basic3D, "?Rotate@@YAXHMMM@Z");
//    dbLimbExist = (dbLimbExistPtr_t)GetProcAddress(Basic3D, "?GetLimbExist@@YAHHH@Z");
//    dbObjectExist = (dbObjectExistPtr_t)GetProcAddress(Basic3D, "?GetExist@@YAHH@Z");
//	dbLimbCount = (dbLimbCountPtr_t)GetProcAddress(Basic3D, "?GetLimbCount@@YAHH@Z");
//
//	dbLockVertexDataForLimb = (dbLockVertexDataForLimbPtr_t)GetProcAddress(Basic3D, "?LockVertexDataForLimb@@YAXHH@Z");
//	dbGetVertexDataVertexCount = (dbGetVertexDataVertexCountPtr_t)GetProcAddress(Basic3D, "?GetVertexDataVertexCount@@YAHXZ");
//	dbGetVertexDataIndexCount = (dbGetVertexDataIndexCountPtr_t)GetProcAddress(Basic3D, "?GetVertexDataIndexCount@@YAHXZ");
//	dbUnlockVertexData = (dbUnlockVertexDataPtr_t)GetProcAddress(Basic3D, "?UnlockVertexData@@YAXXZ");
//	dbSetVertexDataPosition = (dbSetVertexDataPositionPtr_t)GetProcAddress(Basic3D, "?SetVertexDataPosition@@YAXHMMM@Z");
//
//	dbGetVertexDataPositionX  = (dbGetVertexDataPositionXPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionX@@YAKH@Z");
//	dbGetVertexDataPositionY  = (dbGetVertexDataPositionYPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionY@@YAKH@Z");
//	dbGetVertexDataPositionZ  = (dbGetVertexDataPositionZPtr_t)GetProcAddress(Basic3D, "?GetVertexDataPositionZ@@YAKH@Z");
//	dbMakeObjectTriangle  = (dbMakeObjectTrianglePtr_t)GetProcAddress(Basic3D, "?MakeTriangle@@YAXHMMMMMMMMM@Z");
//	dbMakeMeshFromObject = (dbMakeMeshFromObjectPtr_t)GetProcAddress(Basic3D, "?MakeMeshFromObject@@YAXHHH@Z");
//	dbAddLimb = (dbAddLimbPtr_t)GetProcAddress(Basic3D, "?AddLimb@@YAXHHH@Z");
//	dbDeleteMesh = (dbDeleteMeshPtr_t)GetProcAddress(Basic3D, "?DeleteMesh@@YAXH@Z"); 
//	dbDeleteObject = (dbDeleteObjectPtr_t)GetProcAddress(Basic3D, "?DeleteObject@@YA_NH@Z");
//	dbMakeObject = (dbMakeObjectPtr_t)GetProcAddress(Basic3D, "?MakeObject@@YAXHHH@Z");
//
//    dbGetProjectionMatrix = (dbGetProjectionMatrixPtr_t)GetProcAddress(CameraMod, "?GetProjectionMatrix@@YA?AUD3DXMATRIX@@H@Z");
//    dbGetViewMatrix = (dbGetViewMatrixPtr_t)GetProcAddress(CameraMod, "?GetViewMatrix@@YA?AUD3DXMATRIX@@H@Z");
//
//	dbSetVector3 = (dbSetVector3Ptr_t)GetProcAddress(VectorsMod, "?SetVector3@@YAXHMMM@Z");
//
//     OV getD3DDevice = (OV) GetProcAddress(g_hLib, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ");
//
//     Using the UNDNAME facility on the string '?GetObjectA@@YAPAUsObject@@H@Z' returns this:
//     struct sObject * __cdecl GetObjectA(int)
//     This matches the function pointer type GetObjectPtr_t defined above.
//}