
#include "globals.h"

namespace PhysX
{
	GetDirect3DPFN					GetDirect3D;
	GetDirect3DDevicePFN			GetDirect3DDevice;

	GetCameraInternalDataPFN		GetCameraInternalData;


	GetObjectPFN					GetObject;
	DeleteObjectPFN					DeleteObject;
	PositionObjectPFN				PositionObject;
	RotateObjectPFN					RotateObject;
	GetFVFOffsetMapPFN				GetFVFOffsetMap;
	CreateNewObjectPFN				CreateNewObject;
	SetupMeshFVFDataPFN				SetupMeshFVFData;
	CalculateMeshBoundsPFN			CalculateMeshBounds;
	SetNewObjectFinalPropertiesPFN	SetNewObjectFinalProperties;
	SetTexturePFN					SetTexture;
	SetObjectTransparencyPFN		SetObjectTransparency;
	DisableObjectZWritePFN			DisableObjectZWrite;
	SetObjectLightPFN				SetObjectLight;
	RndPFN							Rnd;
	GhostObjectOnPFN				GhostObjectOn;
	GhostObjectOn1PFN				GhostObjectOn1;
	GetCameraLookPFN				GetCameraLook;
	GetCameraPositionPFN			GetCameraPosition;
	GetCameraUpPFN					GetCameraUp;
	GetCameraRightPFN				GetCameraRight;
	RotateLimbPFN					RotateLimb;
	OffsetLimbPFN					OffsetLimb;
	GetLimbAnglePFN					GetLimbAngleX;
	GetLimbAnglePFN					GetLimbAngleY;
	GetLimbAnglePFN					GetLimbAngleZ;
	GetLimbOffsetPFN				GetLimbOffsetX;
	GetLimbOffsetPFN				GetLimbOffsetY;
	GetLimbOffsetPFN				GetLimbOffsetZ;
	KeyStatePFN						GetKeyState;
	GetDisplayFPSPFN				GetDisplayFPS;

	GetLimbPositionPFN				GetLimbPositionX;
	GetLimbPositionPFN				GetLimbPositionY;
	GetLimbPositionPFN				GetLimbPositionZ;
	GetLimbDirectionPFN				GetLimbDirectionX;
	GetLimbDirectionPFN				GetLimbDirectionY;
	GetLimbDirectionPFN				GetLimbDirectionZ;

	GetLimbPositionPFN				GetLimbScaleX;
	GetLimbPositionPFN				GetLimbScaleY;
	GetLimbPositionPFN				GetLimbScaleZ;

	GlobStruct*						pGlobStruct;

	NxPhysicsSDK*					pPhysicsSDK	= NULL;
	NxScene*						pScene		= NULL;

	ConvertLocalMeshToTriListPFN	ConvertLocalMeshToTriList;

	

	//std::vector < NxScene* >			pSceneList;
	std::vector < sSceneContainer* >			pSceneList;

	NxVec3							vecGravity ( 0, -9.8f, 0 );
	std::vector < sRagdoll* >	pRagdollList;
	std::vector < sPhysXObject* >	pObjectList;
	std::vector < sPhysXObject* >	pTriggerList;
	std::vector < sPhysXMaterial* >	pMaterialList;
	std::vector < sPhysXJoint* >	pJointList;
	std::vector < sPhysXEmitter* >	pEmitterList;
	NxMaterialIndex					SceneMaterialIndex = 0;
	UserAllocator*					pAllocator = NULL;
	cContactReport					ContactReport;
	cTriggerReport					TriggerReport;
	cRayCastReport					RayCastReport;

	std::stack < sCollision >		CollisionStack;
	sCollision						CollisionData;

	std::stack < sCollision >		TriggerStack;
	sCollision						TriggerData;

	ControllerManager				CharacterController;

	int								iTimingMethod;
	float							fFixedTime;

	bool							bEnableGroundPlane = false;

	int iDebug = 0;

	int iSimulationType;
	int iSimulationSceneType;
	int iSimulationFlags;

	float							fHeightFieldVerticalExtent = -1024.0f;

	NxSceneQuery*					pSceneQueryObject = NULL;

	// shaders for fluid rendering

	//IDirect3DVertexShader9 *        particleVS;
	//IDirect3DPixelShader9 *         particleThicknessPS;

	DxFluidRenderer *               fluidSurfaceRenderer;
}
