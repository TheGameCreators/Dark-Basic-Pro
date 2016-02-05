
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject );
DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, int iMaterial );
DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile );
DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile );
DARKSDK void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );

DARKSDK void dbPhySetRigidBodyTerrainExtent ( float fExtent );

void dbPhyMakeRigidBodyTerrain ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
