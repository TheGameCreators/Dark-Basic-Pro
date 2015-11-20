//
// BoxCol.h: interface for the C3DCollision class.
//

#include <windows.h>
#include <D3DX9.h>
#include "..\cobjectdatac.h"

// Exposed Internal Functions
int CheckWorldBoxCollision ( D3DXVECTOR3 box1p, D3DXMATRIX matARotation, sCollisionData* pCollisionA, D3DXVECTOR3 box2p, D3DXMATRIX matBRotation, sCollisionData* pCollisionB );

// DBV1 Object Collision Commands
void	DBV1_MakeObjectCollisionBox( sObject* pObj, float x1, float y1, float z1, float x2, float y2, float z2, int iRotationCollisionMode );
void	DBV1_DeleteObjectCollisionBox( sObject* pObj );
int		DBV1_ReturnModelBoxSystemOverlapResult( sObject* pObj, sObject* pSecondObj );
float	DBV1_ObjectCollisionRadius( sObject* pObj );
float	DBV1_ObjectCollisionCenterX( sObject* pObj );
float	DBV1_ObjectCollisionCenterY( sObject* pObj );
float	DBV1_ObjectCollisionCenterZ( sObject* pObj );
float	DBV1_ObjectCollisionXF(void);
float	DBV1_ObjectCollisionYF(void);
float	DBV1_ObjectCollisionZF(void);

// DBV1 Static Box Collision Commands
void	DBV1_MakeStaticCollisionBox(float x1, float y1, float z1, float x2, float y2, float z2);
void	DBV1_DeleteAllStaticCollisionBoxes(void);
int		DBV1_StaticCollisionBoxHit(	float x1, float y1, float z1, float x2, float y2, float z2, float Bx1, float By1, float Bz1, float Bx2, float By2, float Bz2 );
float	DBV1_StaticCollisionXF(void);
float	DBV1_StaticCollisionYF(void);
float	DBV1_StaticCollisionZF(void);

// DBV1 Line of Sight Commands (using static collision boxes)
int		DBV1_StaticLineOfSightHit(float sx, float sy, float sz, float dx, float dy, float dz, float width, float acc );
float	DBV1_LineOfSightXF(void);
float	DBV1_LineOfSightYF(void);
float	DBV1_LineOfSightZF(void);

