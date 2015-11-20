#ifndef _H_ODERAGDOLL
#define _H_ODERAGDOLL

#include "sODEBone.h"
#include "sODEJoint.h"

class sODERagdoll
{

	// X10 - 051007 - quick hack to get the ragdoll list so we can delete it when ODE quits
public:

		static sODERagdoll *pODERagdollList;

private:

	sODEBone *pODEBones;
	sODEJoint **pODEJoints;

	DWORD dwNumBones, dwNumJoints;

	sObject* pObject;
	int iID;
	int iCenterBone;

	float		fTravelDistancePerUpdateX;
	float		fTravelDistancePerUpdateY;
	float		fTravelDistancePerUpdateZ;

	int			m_iRagdollCounterNumber;
	DWORD		m_dwRagdollCounterTime;

	sODEBone* FindBoneFromOwner( int iFrame );
	void AttachBones( sFrame *pParentFrame, sFrame *pFrame );
	void CreateJoint( sFrame *pParentLimb, sFrame *pChildLimb );

public:

		sODERagdoll *pNextRagdoll, *pPrevRagdoll;


public:

	int			iResponseMode;
	int			iFreezeDelay;
	float		fContactMU;
	float		fContactMU2;
	float		fContactBounce;
	float		fContactBounceVelocity;
	float		fContactSoftERP;
	float		fContactSoftCFM;
	float		fContactMotion1;
	float		fContactMotion2;
	float		fContactSlip1;
	float		fContactSlip2;


public:

	sODERagdoll( );
	~sODERagdoll( );

	void AddTravelDistanceX ( float fDistance ) { fTravelDistancePerUpdateX += fDistance; }
	void AddTravelDistanceY ( float fDistance ) { fTravelDistancePerUpdateY += fDistance; }
	void AddTravelDistanceZ ( float fDistance ) { fTravelDistancePerUpdateZ += fDistance; }
	float GetTravelDistancePerUpdateX ( void ) { return fTravelDistancePerUpdateX; }
	float GetTravelDistancePerUpdateY ( void ) { return fTravelDistancePerUpdateY; }
	float GetTravelDistancePerUpdateZ ( void ) { return fTravelDistancePerUpdateZ; }
	void AdjustMass ( float fMass );
	void SetGravity ( int iGravityMode );
	void SetActive ( int iNudgeIntoActivityMode );
	void SetLinearVelocity ( float fX, float fY, float fZ );
	void SetAngularVelocity ( float fX, float fY, float fZ );

	static sODERagdoll* AddRagdoll( int id, sObject *pObj, float fAdjust, int iDebugObject );
	static void RemoveRagdoll( sODERagdoll* pDelRagdoll );
	static sODERagdoll* GetRagdoll( int iID );
	static void DisableSilentRagdollsEachUpdate( void );
	static void UpdateRagdolls( float fTimeDelta );
	static sODERagdoll* GetRagdoll( dBodyID body );

	void Update( float fTimeDelta );
	bool IsBodyBone( dBodyID checkbody );
	bool CheckBoneGroups( dBodyID body1, dBodyID body2 );
	int GetRagdollID( void ) { return iID; }
};

#endif