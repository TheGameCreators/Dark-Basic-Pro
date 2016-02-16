#include "Defines.h"
#ifdef COMPILE_FOR_GDK
#include"DarkGDK.h"
#else

#include "globstruct.h"
#include "DBOData.h"

// A few global variables...
extern GlobStruct* Core;// = 0;
extern HMODULE Basic3D;// = 0;
extern HMODULE CameraMod;// = 0;
extern HMODULE VectorsMod;// = 0;
extern HMODULE CoreMod;

typedef IDirect3DDevice9* (__cdecl *dbGetDirect3DDevicePtr_t) ();
extern dbGetDirect3DDevicePtr_t dbGetDirect3DDevice;

typedef sObject* (__cdecl *dbGetObjectPtr_t) (int);
extern dbGetObjectPtr_t dbGetObject;// = 0;

typedef void (__cdecl *dbGetObjectWorldMatrixPtr_t) (int, int, D3DXMATRIX*);
extern dbGetObjectWorldMatrixPtr_t dbGetObjectWorldMatrix;// = 0;

typedef void (__cdecl *dbSetObjectWorldMatrixPtr_t) (int, D3DXMATRIX*);
extern dbSetObjectWorldMatrixPtr_t dbSetObjectWorldMatrix;

typedef void (__cdecl *dbPositionObjectPtr_t) (int, float, float, float);
extern dbPositionObjectPtr_t dbPositionObject;

typedef void (__cdecl *dbRotateObjectPtr_t) (int, float, float, float);
extern dbRotateObjectPtr_t dbRotateObject;

typedef bool (__cdecl *dbObjectExistPtr_t) (int);
extern dbObjectExistPtr_t dbObjectExist;

typedef bool (__cdecl *dbLimbExistPtr_t) (int, int);
extern dbLimbExistPtr_t dbLimbExist;



typedef int (__cdecl *dbLimbCountPtr_t) (int);
extern dbLimbCountPtr_t dbLimbCount;

typedef void (__cdecl *dbLockVertexDataForLimbPtr_t) (int, int);
extern dbLockVertexDataForLimbPtr_t dbLockVertexDataForLimb;

typedef int (__cdecl *dbGetVertexDataVertexCountPtr_t) (void);
extern dbGetVertexDataVertexCountPtr_t dbGetVertexDataVertexCount;

typedef int (__cdecl *dbGetVertexDataIndexCountPtr_t) (void);
extern dbGetVertexDataIndexCountPtr_t dbGetVertexDataIndexCount;

typedef void (__cdecl *dbUnlockVertexDataPtr_t) (void);
extern dbUnlockVertexDataPtr_t dbUnlockVertexData;

typedef void (__cdecl *dbSetVertexDataPositionPtr_t) (int, float, float, float);
extern dbSetVertexDataPositionPtr_t dbSetVertexDataPosition;

//NEW
typedef DWORD (__cdecl *dbGetVertexDataPositionXPtr_t) (int);
extern dbGetVertexDataPositionXPtr_t dbGetVertexDataPositionX;

typedef DWORD (__cdecl *dbGetVertexDataPositionYPtr_t) (int);
extern dbGetVertexDataPositionYPtr_t dbGetVertexDataPositionY;

typedef DWORD (__cdecl *dbGetVertexDataPositionZPtr_t) (int);
extern dbGetVertexDataPositionZPtr_t dbGetVertexDataPositionZ;

typedef void (__cdecl *dbMakeObjectTrianglePtr_t) (int, float, float, float, float, float, float, float, float, float);
extern dbMakeObjectTrianglePtr_t dbMakeObjectTriangle;

typedef void (__cdecl *dbMakeMeshFromObjectPtr_t) (int, int);
extern dbMakeMeshFromObjectPtr_t dbMakeMeshFromObject;

typedef void (__cdecl *dbAddLimbPtr_t) (int, int, int);
extern dbAddLimbPtr_t dbAddLimb;

typedef void (__cdecl *dbDeleteMeshPtr_t) (int);
extern dbDeleteMeshPtr_t dbDeleteMesh;

typedef void (__cdecl *dbDeleteObjectPtr_t) (int);
extern dbDeleteObjectPtr_t dbDeleteObject;

typedef void (__cdecl *dbMakeObjectPtr_t) (int, int, int);
extern dbMakeObjectPtr_t dbMakeObject;

typedef D3DXMATRIX (__cdecl *dbGetProjectionMatrixPtr_t) (int);
extern dbGetProjectionMatrixPtr_t dbGetProjectionMatrix;

typedef D3DXMATRIX (__cdecl *dbGetViewMatrixPtr_t) (int);
extern dbGetViewMatrixPtr_t dbGetViewMatrix;

typedef void (__cdecl *dbSetVector3Ptr_t) (int, float, float, float);
extern dbSetVector3Ptr_t dbSetVector3;

typedef int (__cdecl *dbScreenWidthPtr_t) ();
extern dbScreenWidthPtr_t dbScreenWidth;

typedef int (__cdecl *dbScreenHeightPtr_t) ();
extern dbScreenHeightPtr_t dbScreenHeight;

typedef void (__cdecl *dbMoveObjectDownPtr_t) (int, float);
extern dbMoveObjectDownPtr_t dbMoveObjectDown;

typedef int (__cdecl *dbTurnObjectRightPtr_t) (int, float);
extern dbTurnObjectRightPtr_t dbTurnObjectRight;

typedef int (__cdecl *dbPitchObjectDownPtr_t) (int, float);
extern dbPitchObjectDownPtr_t dbPitchObjectDown;

typedef DWORD (__cdecl *dbXVector3Ptr_t) (int);
extern dbXVector3Ptr_t dbXVector3;

typedef DWORD (__cdecl *dbYVector3Ptr_t) (int);
extern dbYVector3Ptr_t dbYVector3;

typedef DWORD (__cdecl *dbZVector3Ptr_t) (int);
extern dbZVector3Ptr_t dbZVector3;

//NEW
typedef void (__cdecl *dbSetVertexDataNormalsPtr_t) (int, float, float, float);
extern dbSetVertexDataNormalsPtr_t dbSetVertexDataNormals;

typedef void (__cdecl *dbSetVertexDataUVPtr_t) (int, float, float);
extern dbSetVertexDataUVPtr_t dbSetVertexDataUV;

typedef void (__cdecl *dbSetIndexDataPtr_t) (int, int);
extern dbSetIndexDataPtr_t dbSetIndexData;

typedef DWORD (__cdecl *dbGetVertexDataUPtr_t) (int);
extern dbGetVertexDataUPtr_t dbGetVertexDataU;

typedef DWORD (__cdecl *dbGetVertexDataVPtr_t) (int);
extern dbGetVertexDataVPtr_t dbGetVertexDataV;

//new
typedef void (__cdecl *dbMakeObjectPlainPtr_t) (int, float, float);
extern dbMakeObjectPlainPtr_t dbMakeObjectPlain;

typedef void (__cdecl *dbCreateMeshForObjectPtr_t) (int, DWORD, DWORD, DWORD);
extern dbCreateMeshForObjectPtr_t dbCreateMeshForObject;

typedef void (__cdecl *dbConvertObjectFVFPtr_t) (int, DWORD);
extern dbConvertObjectFVFPtr_t dbConvertObjectFVF;

typedef void (__cdecl *dbSetVertexDataDiffusePtr_t) (int, DWORD);
extern dbSetVertexDataDiffusePtr_t dbSetVertexDataDiffuse;

typedef DWORD (__cdecl *dbRGBPtr_t) (int, int, int);
extern dbRGBPtr_t dbRGB;

typedef int (__cdecl *dbMeshExistPtr_t) (int);
extern dbMeshExistPtr_t dbMeshExist;

typedef void (__cdecl *dbAddMeshToVertexDataPtr_t) (int);
extern dbAddMeshToVertexDataPtr_t dbAddMeshToVertexData;

//new
typedef DWORD (__cdecl *dbCameraPositionXPtr_t) (int);
extern dbCameraPositionXPtr_t dbCameraPositionX;

typedef DWORD (__cdecl *dbCameraPositionYPtr_t) (int);
extern dbCameraPositionYPtr_t dbCameraPositionY;

typedef DWORD (__cdecl *dbCameraPositionZPtr_t) (int);
extern dbCameraPositionZPtr_t dbCameraPositionZ;

typedef void (__cdecl *dbPointObjectPtr_t) (int, float, float, float);
extern dbPointObjectPtr_t dbPointObject;

typedef void (__cdecl *dbShowObjectPtr_t) (int);
extern dbShowObjectPtr_t dbShowObject;

typedef void (__cdecl *dbHideObjectPtr_t) (int);
extern dbHideObjectPtr_t dbHideObject;

typedef void (__cdecl *dbFadeObjectPtr_t) (int, float);
extern dbFadeObjectPtr_t dbFadeObject;

typedef void (__cdecl *dbScaleObjectPtr_t) (int, float, float, float);
extern dbScaleObjectPtr_t dbScaleObject;

typedef void (__cdecl *dbSetAlphaMappingOnPtr_t) (int, float);
extern dbSetAlphaMappingOnPtr_t dbSetAlphaMappingOn;

typedef void (__cdecl *dbPositionCameraPtr_t) (int, float, float, float);
extern dbPositionCameraPtr_t dbPositionCamera;

typedef void (__cdecl *dbPointCameraPtr_t) (int, float, float, float);
extern dbPointCameraPtr_t dbPointCamera;

#endif








