
/////////////////////////////////////////////////////////////////////////////////////
// MATERIAL COMMANDS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// give material name property
DARKSDK void  dbPhyMakeMaterial						( int iID, char* szName );
DARKSDK void  dbPhyDeleteMaterial					( int iID );
DARKSDK void  dbPhySetMaterialDynamicFriction		( int iID, float fValue );
DARKSDK void  dbPhySetMaterialStaticFriction		( int iID, float fValue );
DARKSDK void  dbPhySetMaterialRestitution			( int iID, float fValue );
DARKSDK void  dbPhySetMaterialDynamicFrictionV		( int iID, float fValue );
DARKSDK void  dbPhySetMaterialStaticFrictionV		( int iID, float fValue );
DARKSDK void  dbPhySetMaterialDirOfAnisotropy		( int iID, float fX, float fY, float fZ );
DARKSDK void  dbPhySetMaterialFrictionCombine		( int iID, int iMode );
DARKSDK void  dbPhySetMaterialRestitutionCombine	( int iID, int iMode );
DARKSDK void  dbPhyBuildMaterial					( int iID );

DARKSDK int   dbPhyGetMaterialExist					( int iID );

DARKSDK float dbPhyGetMaterialDynamicFriction		( int iID );
DARKSDK float dbPhyGetMaterialStaticFriction		( int iID );
DARKSDK float dbPhyGetMaterialRestitution			( int iID );
DARKSDK float dbPhyGetMaterialDynamicFrictionV		( int iID );
DARKSDK float dbPhyGetMaterialStaticFrictionV		( int iID );
DARKSDK float dbPhyGetMaterialDirOfAnisotropyX		( int iID );
DARKSDK float dbPhyGetMaterialDirOfAnisotropyY		( int iID );
DARKSDK float dbPhyGetMaterialDirOfAnisotropyZ		( int iID );
DARKSDK DWORD dbPhyGetMaterialDynamicFrictionEx		( int iID );
DARKSDK DWORD dbPhyGetMaterialStaticFrictionEx		( int iID );
DARKSDK DWORD dbPhyGetMaterialRestitutionEx			( int iID );
DARKSDK DWORD dbPhyGetMaterialDynamicFrictionVEx	( int iID );
DARKSDK DWORD dbPhyGetMaterialStaticFrictionVEx		( int iID );
DARKSDK DWORD dbPhyGetMaterialDirOfAnisotropyXEx	( int iID );
DARKSDK DWORD dbPhyGetMaterialDirOfAnisotropyYEx	( int iID );
DARKSDK DWORD dbPhyGetMaterialDirOfAnisotropyZEx	( int iID );

DARKSDK int   dbPhyGetMaterialFrictionCombine		( int iID );
DARKSDK int   dbPhyGetMaterialRestitutionCombine	( int iID );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// MATERIAL COMMANDS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void					dbPhySetMaterialCombine		( int iID, int iType, int iMode );

PhysX::sPhysXMaterial*	dbPhyGetMaterial			( int iID, bool bDisplayError = false );
int						dbPhyGetMaterialIndex		( int iID );

void					dbPhyClearMaterials			( void );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

