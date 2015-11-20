#ifndef _H_ODEJOINT
#define _H_ODEJOINT

class sODEJoint
{

private:

	int iID;
	dJointID oJoint;
	static sODEJoint* pODEJointList;

	bool bMovingLimit;
	float fLowStart, fHighStart;
	float fLowEnd, fHighEnd;
	float fMoveTime, fOrigMoveTime;

	sODEJoint *pNextJoint, *pPrevJoint;

public:

	sODEJoint( );	
	~sODEJoint();

	static sODEJoint* AddFixedJoint( int id, dBodyID body1, dBodyID body2 );
	static sODEJoint* AddHingeJoint( int id, dBodyID body1, dBodyID body2, float ax, float ay, float az, float x, float y, float z );
	static sODEJoint* AddHinge2Joint( int id, dBodyID body1, dBodyID body2, float ax, float ay, float az, float ax2, float ay2, float az2, float x, float y, float z );
	static sODEJoint* AddBallJoint( int id, dBodyID body1, dBodyID body2, float x, float y, float z );
	static void RemoveJoint( sODEJoint* pDelJoint );
	static sODEJoint* GetJoint( int iID );

	void SetHingeLimit( float low, float high );
	void SetMovingHingeLimit( float startlow, float starthigh, float endlow, float endhigh, float time );
	void SetHinge2Limit( float axis1low, float axis1high, float axis2low, float axis2high );

	void UpdateJoint( float fTimeDelta );
};

#endif