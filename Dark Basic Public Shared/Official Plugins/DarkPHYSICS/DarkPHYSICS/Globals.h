
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include "NxPhysics.h"
#include "NxCooking.h"
#include "NxController.h"
#include <vector>
#include "..\..\..\..\Dark Basic Pro SDK\Shared\DBOFormat\DBOData.h"
#include "UserAllocator.h"
#include <stack>
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "ControllerManager.h"
#include "..\..\..\..\Dark Basic Pro SDK\Shared\Core\globstruct.h"
#include <d3d9.h>

class DxFluidRenderer;

#ifndef DARKSDK
 #define DARKSDK	__declspec ( dllexport )
#endif
#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

#ifdef DARKSDK_COMPILE
	sObject*	dbGetObject						( int iObject );
	bool		GetFVFOffsetMap					( sMesh* pMesh, sOffsetMap* psOffsetMap );
	void		SetWorldMatrix					( int iID, D3DXMATRIX* pMatrix );
	bool		CreateNewObject					( int iID, LPSTR pName, int iFrame );
	bool		SetupMeshFVFData				( sMesh* pMesh, DWORD dwFVF, DWORD dwSize, DWORD dwIndexCount );
	bool		SetNewObjectFinalProperties		( int iID, float fRadius );
	void		SetTexture						( int iID, int iImageIndex );
	bool		CalculateMeshBounds				( sMesh* pMesh );
#endif


namespace PhysX
{
	typedef sObject*	( *GetObjectPFN						)	( int );
	typedef void		( *DeleteObjectPFN					)	( int );
	typedef void		( *PositionObjectPFN				)	( int, float, float, float );
	typedef void		( *RotateObjectPFN					)	( int, float, float, float );
	typedef bool		( *GetFVFOffsetMapPFN				)	( sMesh*, sOffsetMap* );
	typedef bool		( *CreateNewObjectPFN				)	( int, char*, int );
	typedef bool		( *SetupMeshFVFDataPFN				)	( sMesh*, DWORD, DWORD, DWORD );
	typedef bool		( *CalculateMeshBoundsPFN			)	( sMesh* );
	typedef bool		( *SetNewObjectFinalPropertiesPFN	)	( int, float );
	typedef void		( *SetTexturePFN					)	( int, int );
	typedef void		( *SetObjectTransparencyPFN			)	( int, int );
	typedef void		( *DisableObjectZWritePFN			)	( int );
	typedef void		( *SetObjectLightPFN				)	( int, int );
	typedef int			( *RndPFN							)	( int );
	typedef void		( *GhostObjectOnPFN					)	( int );
	typedef void		( *GhostObjectOn1PFN				)	( int, int );
	typedef D3DXVECTOR3 ( *GetCameraLookPFN					)	( void );
	typedef D3DXVECTOR3 ( *GetCameraPositionPFN				)	( void );
	typedef D3DXVECTOR3 ( *GetCameraUpPFN					)	( void );
	typedef D3DXVECTOR3 ( *GetCameraRightPFN				)	( void );
	typedef void		( *RotateLimbPFN					)	( int, int, float, float, float );
	typedef void		( *OffsetLimbPFN					)	( int, int, float, float, float );
	typedef DWORD		( *GetLimbAnglePFN					)	( int, int );
	typedef DWORD		( *GetLimbOffsetPFN					)	( int, int );
	typedef float		( *GetLimbAngleExPFN				)	( int, int );
	typedef int			( *KeyStatePFN						)	( int );
	typedef int			( *GetDisplayFPSPFN					)	( void );
	typedef DWORD		( *GetLimbPositionPFN				)	( int, int );
	typedef DWORD		( *GetLimbDirectionPFN				)	( int, int );
	typedef IDirect3D9*		  ( *GetDirect3DPFN					)	( void );
	typedef IDirect3DDevice9* ( *GetDirect3DDevicePFN			)	( void );
	typedef void* ( *GetCameraInternalDataPFN ) ( int );

	typedef bool ( *ConvertLocalMeshToTriListPFN ) ( sMesh* pMesh );

	extern ConvertLocalMeshToTriListPFN	ConvertLocalMeshToTriList;

	extern GetCameraInternalDataPFN GetCameraInternalData;

	extern GetDirect3DPFN	GetDirect3D;
	extern GetDirect3DDevicePFN GetDirect3DDevice;

	extern GetObjectPFN						GetObject;
	extern DeleteObjectPFN					DeleteObject;
	extern PositionObjectPFN				PositionObject;
	extern RotateObjectPFN					RotateObject;
	extern GetFVFOffsetMapPFN				GetFVFOffsetMap;
	extern CreateNewObjectPFN				CreateNewObject;
	extern SetupMeshFVFDataPFN				SetupMeshFVFData;
	extern CalculateMeshBoundsPFN			CalculateMeshBounds;
	extern SetNewObjectFinalPropertiesPFN	SetNewObjectFinalProperties;
	extern SetTexturePFN					SetTexture;
	extern SetObjectTransparencyPFN			SetObjectTransparency;
	extern DisableObjectZWritePFN			DisableObjectZWrite;
	extern SetObjectLightPFN				SetObjectLight;
	extern RndPFN							Rnd;
	extern GhostObjectOnPFN					GhostObjectOn;
	extern GhostObjectOn1PFN				GhostObjectOn1;
	extern GetCameraLookPFN					GetCameraLook;
	extern GetCameraPositionPFN				GetCameraPosition;
	extern GetCameraUpPFN					GetCameraUp;
	extern GetCameraRightPFN				GetCameraRight;
	extern RotateLimbPFN					RotateLimb;
	extern OffsetLimbPFN					OffsetLimb;
	extern GetLimbAnglePFN					GetLimbAngleX;
	extern GetLimbAnglePFN					GetLimbAngleY;
	extern GetLimbAnglePFN					GetLimbAngleZ;
	extern GetLimbOffsetPFN					GetLimbOffsetX;
	extern GetLimbOffsetPFN					GetLimbOffsetY;
	extern GetLimbOffsetPFN					GetLimbOffsetZ;
	extern KeyStatePFN						GetKeyState;
	extern GetDisplayFPSPFN					GetDisplayFPS;

	extern GetLimbPositionPFN				GetLimbPositionX;
	extern GetLimbPositionPFN				GetLimbPositionY;
	extern GetLimbPositionPFN				GetLimbPositionZ;
	extern GetLimbDirectionPFN				GetLimbDirectionX;
	extern GetLimbDirectionPFN				GetLimbDirectionY;
	extern GetLimbDirectionPFN				GetLimbDirectionZ;

	extern GetLimbPositionPFN				GetLimbScaleX;
	extern GetLimbPositionPFN				GetLimbScaleY;
	extern GetLimbPositionPFN				GetLimbScaleZ;

	extern GlobStruct*						pGlobStruct;

	enum eType
	{
		eRigidBodyBox,
		eRigidBodySphere,
		eRigidBodyCapsule,
		eRigidBodyMesh,
		eRigidBodyTerrain,
		eRigidBodyConvex,
		eRigidBodyPMAP,
		eRigidBodyRagdoll,
		eBoxTrigger,
		eSphereTrigger,
		eTypeVehicle,
		eBoxCharacterController,
		eCapsuleCharacterController
	};

	enum eJointType
	{
		eUnknownJoint,
		eSphereJoint,
		eRevoluteJoint,
		ePrismaticJoint,
		eCylindricalJoint,
		eFixedJoint,
		eDistanceJoint,
		ePointInPlaneJoint,
		ePointOnLineJoint,
		ePulleyJoint,
		e6DOFJoint
	};

	enum eState
	{
		eStatic,
		eDynamic
	};

	enum eAction
	{
		eActionEnter,
		eActionLeave
	};

	struct sObjectCollisionData
	{
		int			iObjectA;
		int			iObjectB;

		D3DXVECTOR3 vecFrictionForce;
		D3DXVECTOR3 vecNormalForce;

		D3DXVECTOR3 vecContactNormal;
		D3DXVECTOR3 vecContactPoint;
		D3DXVECTOR3 contactForce;
		D3DXVECTOR3 contactArrowForceTip;
		D3DXVECTOR3 contactArrowFrictionTip;
		D3DXVECTOR3 contactArrowPenetrationTip;
	};

	struct sPhysXObject
	{
		sObject* pObject;
		//NxActor* pActor;

		std::vector < NxActor* > pActorList;

		NxScene* pScene;

		int		 iID;
		eState	 state;
		eType	 type;

		int		iLastCollider;

		std::vector < sObjectCollisionData > collisionList;
	};

	struct sBone
	{
		int iMesh;
		int iBone;
		int iObject;
	};

	struct sRagdoll
	{
		sObject* pObject;
		NxActor* pActor;
		int		 iID;
		eState	 state;
		eType	 type;

		std::vector < sBone > boneList;
	};

	class ParticleEmitter;

	struct sPhysXEmitterColorKey
	{
		DWORD		dwColor;
		float		fTime;	
	};

	struct sPhysXEmitter
	{
		sObject*								pObject;
		ParticleEmitter*						pEmitter;
		int										iID;
		float									fSpeed;
		float									fAcceleration;
		int										iMaxParticles;
		float									fTime;
		int										iGroup;
		float									fX;
		float									fY;
		float									fZ;
		int										iFade;
		std::vector < sPhysXEmitterColorKey >	ColorKeys;
		DWORD									dwColor;

		bool									bLinearVelocityRange;
		float									fMinX;
		float									fMinY;
		float									fMinZ;
		float									fMaxX;
		float									fMaxY;
		float									fMaxZ;

		float									fUpdate;
	};

	struct sPhysXJoint
	{
		int		   iID;
		eJointType type;
		NxJoint*   pJoint;

		sPhysXObject*	pA;
		sPhysXObject*	pB;

		union
		{
			NxSphericalJoint*		pSphereJoint;
			NxRevoluteJoint*		pRevoluteJoint;
			NxPrismaticJoint*		pPrismaticJoint;
			NxCylindricalJoint*		pCylindricalJoint;
			NxFixedJoint*			pFixedJoint;
			NxDistanceJoint*		pDistanceJoint;
			NxPointInPlaneJoint*	pPointInPlaneJoint;
			NxPointOnLineJoint*		pPointOnLineJoint;
			NxPulleyJoint*			pPulleyJoint;
			NxD6Joint*				pD6Joint;
		};

		NxSphericalJointDesc*		pSphericalDesc;
		NxRevoluteJointDesc*		pRevoluteDesc;
		NxPrismaticJointDesc*		pPrismaticDesc;
		NxCylindricalJointDesc*		pCylindricalDesc;
		NxFixedJointDesc*			pFixedDesc;
		NxDistanceJointDesc*		pDistanceDesc;
		NxPointInPlaneJointDesc*	pPointInPlaneDesc;
		NxPointOnLineJointDesc*		pPointOnLineDesc;
		NxPulleyJointDesc*			pPulleyDesc;
		NxD6JointDesc*				pD6Desc;
	};

	struct sPhysXMaterial
	{
		NxMaterialDesc  matMaterial;
		NxMaterialIndex	matIndex;
		NxMaterial*		pMaterial;
		int				iID;
	};

	struct CarWheelContact
	{
		NxActor * car;
		NxShape * wheel;
		NxVec3 contactPoint;
	};

	class cContactReport : public NxUserContactReport
	{
		void onContactNotify ( NxContactPair& pair, NxU32 events );
	};

	class cTriggerReport : public NxUserTriggerReport
	{
		void onTrigger ( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status );
	};

	class cRayCastReport : public NxUserRaycastReport
	{
		public:
			bool		 m_bHit;
			NxRaycastHit m_HitData;
			int			 m_iID;

			bool onHit ( const NxRaycastHit& hit );
	};

	

	struct sCollision
	{
		int	    iObjectA;
		int	    iObjectB;

		float	fNormalForceX;
		float	fNormalForceY;
		float	fNormalForceZ;

		float	fFrictionForceX;
		float	fFrictionForceY;
		float	fFrictionForceZ;

		float	fPointX;
		float	fPointY;
		float	fPointZ;

		float	fNormalX;
		float	fNormalY;
		float	fNormalZ;

		eAction action;
	};

	struct sSceneContainer
	{
		NxScene*	pScene;
		int			iID;
	};

	extern NxPhysicsSDK*					pPhysicsSDK;
	extern NxScene*							pScene;

	//extern std::vector < NxScene* >			pSceneList;
	extern std::vector < sSceneContainer* >			pSceneList;

	extern NxVec3							vecGravity;
	extern std::vector < sRagdoll* >		pRagdollList;
	extern std::vector < sPhysXObject* >	pObjectList;
	extern std::vector < sPhysXObject* >	pTriggerList;
	extern std::vector < sPhysXMaterial* >	pMaterialList;
	extern std::vector < sPhysXJoint* >		pJointList;
	extern std::vector < sPhysXEmitter* >	pEmitterList;
	extern UserAllocator*					pAllocator;
	extern cContactReport					ContactReport;
	extern cTriggerReport					TriggerReport;
	extern cRayCastReport					RayCastReport;

	extern NxMaterialIndex					SceneMaterialIndex;

	extern std::stack < sCollision >		CollisionStack;
	extern sCollision						CollisionData;

	extern std::stack < sCollision >		TriggerStack;
	extern sCollision						TriggerData;

	extern ControllerManager				CharacterController;

	extern int								iTimingMethod;
	extern float							fFixedTime;

	extern int								iSimulationType;
	extern int 								iSimulationSceneType;
	extern int 								iSimulationFlags;

	extern int								iDebug;

	extern bool							bEnableGroundPlane;

	extern float							fHeightFieldVerticalExtent;

	extern NxSceneQuery*					pSceneQueryObject;

	extern DxFluidRenderer *               fluidSurfaceRenderer;
}

#endif _GLOBALS_H_