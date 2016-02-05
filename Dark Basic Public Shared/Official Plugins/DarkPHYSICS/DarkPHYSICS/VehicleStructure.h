
#ifndef _VEHICLE_STRUCTURE_H_
#define _VEHICLE_STRUCTURE_H_

namespace PhysXVehicle
{
	struct sVehicleWheelData
	{
		float fRadius;
		float fHeight;
		float fWheelX;
		float fWheelY;
		float fWheelZ;
		int  iDirection;
		int  iLimb;
		bool bFront;
	};

	struct sVehicle
	{
		int									iID;
		bool								bAutoDrive;
		bool								bFrontWheelIsPowered;
		bool								bRearWheelIsPowered;
		NxReal								fMaxMotorPower;
		NxReal								fMotorForce;
		NxReal								fSteeringDelta;
		NxReal								fMaxSteeringAngle;
		NxReal								fSuspensionSpring;
		NxReal								fSuspensionDamping;
		NxReal								fSuspensionBias;
		NxSpringDesc						spring;
		NxReal								fMass;
		NxMaterialDesc						material;
		NxMaterialIndex 					materialIndex;
		sObject*							pObject;
		NxActor*							pActor;
		NxBodyDesc							bodyDesc;
		NxBoxShapeDesc						boxDesc [ 12 ];
		int									iBoxCount;
		NxWheelShapeDesc					wheelDesc [ 4 ];
		NxWheelShape*						wheelptr [ 4 ];
		NxActorDesc							actorDesc;
		sVehicleWheelData					wheels [ 4 ];
		int									iWheelCount;
		float								fOldPosX;
		float								fOldPosY;
		float								fOldPosZ;
		float								fSteeringValue;
		float								fSteeringAngle;
		float								fWheelMultiplier;
		NxArray < PhysX::CarWheelContact >	wheelContactPoints;

		bool								bBuilt;
	};

	extern std::vector < sVehicle* > pVehicleList;
}

PhysXVehicle::sVehicle* dbPhyGetVehicle ( int iID, bool bDisplayError = false );

#endif _VEHICLE_STRUCTURE_H_