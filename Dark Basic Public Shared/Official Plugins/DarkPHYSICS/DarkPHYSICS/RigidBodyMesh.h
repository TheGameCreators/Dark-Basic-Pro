
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject );
DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyStaticMesh		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyStaticMeshes		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyDynamicMesh		( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyDynamicMeshes	( int iObject, char* szFile, int iMaterial );

void dbPhyMakeRigidBodyMesh ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile, PhysX::sPhysXObject* pPhysXObject );
void dbPhyMakeRigidBodyPMAP ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile );
void dbPhySetupHardwareMesh ( NxActor* pActor );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
