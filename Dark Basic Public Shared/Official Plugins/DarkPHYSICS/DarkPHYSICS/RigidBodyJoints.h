
/////////////////////////////////////////////////////////////////////////////////////
// RIGID BODY COMMANDS //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
DARKSDK void dbPhyMakeSphereJoint		( int iJoint, int iA, int iB, float fX, float fY, float fZ );
DARKSDK void dbPhyMakeRevoluteJoint		( int iJoint, int iA, int iB, float fAxisX, float fAxisY, float fAxisZ, float fX, float fY, float fZ );
DARKSDK void dbPhyMakePrismaticJoint	( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakeCylindricalJoint	( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakeFixedJoint		( int iJoint, int iA, int iB );
DARKSDK void dbPhyMakeDistanceJoint		( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fMinDistance, float fMaxDistance );
DARKSDK void dbPhyMakePointInPlaneJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ );
DARKSDK void dbPhyMakePointOnLineJoint	( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ  );
DARKSDK void dbPhyMakePulleyJoint		( int iJoint, int iA, int iB, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float fX3, float fY3, float fZ3, float fX4, float fY4, float fZ4, float fDistance, float fStiffness, float fRatio );


DARKSDK void dbPhyMakeSphereJoint		( int iJoint );
DARKSDK void dbPhyMakeRevoluteJoint		( int iJoint );
DARKSDK void dbPhyMakePrismaticJoint	( int iJoint );
DARKSDK void dbPhyMakeCylindricalJoint	( int iJoint );
DARKSDK void dbPhyMakeFixedJoint		( int iJoint );
DARKSDK void dbPhyMakeDistanceJoint		( int iJoint );
DARKSDK void dbPhyMakePointInPlaneJoint ( int iJoint );
DARKSDK void dbPhyMakePointOnLineJoint	( int iJoint );
DARKSDK void dbPhyMakePulleyJoint		( int iJoint );
*/

DARKSDK void dbPhyDeleteJoint			( int iJoint );
DARKSDK int  dbPhyGetJointExist			( int iJoint );
DARKSDK int  dbPhyGetJointType			( int iJoint );
DARKSDK void dbPhySetJointBreakLimits   ( int iJoint, float fMaxForce, float fMaxTorque );

DARKSDK int dbPhyGetJointState ( int iJoint );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


PhysX::sPhysXJoint* dbPhyGetJoint ( int iID, PhysX::eJointType eJoint = PhysX::eUnknownJoint, bool bDisplayError = false );
bool dbPhyCheckJoint ( int iID );
bool dbPhySetupJoint ( int iJoint, int iA, int iB, NxJointDesc* pDescription );

/*
virtual void  getActors (NxActor **actor1, NxActor **actor2)=0 
  Retrieves the Actors involved. 
 
virtual void  setGlobalAnchor (const NxVec3 &vec)=0 
  Sets the point where the two actors are attached, specified in global coordinates. 
 
virtual void  setGlobalAxis (const NxVec3 &vec)=0 
  Sets the direction of the joint's primary axis, specified in global coordinates. 
 
virtual NxVec3  getGlobalAnchor () const =0 
  Retrieves the joint anchor. 
 
virtual NxVec3  getGlobalAxis () const =0 
  Retrieves the joint axis. 
 
virtual NxJointState  getState ()=0 
  Returns the state of the joint. 
 
virtual void  setBreakable (NxReal maxForce, NxReal maxTorque)=0 
  Sets the maximum force magnitude that the joint is able to withstand without breaking. 
 
virtual void  getBreakable (NxReal &maxForce, NxReal &maxTorque)=0 
  Retrieves the max forces of a breakable joint. See setBreakable(). 
 
virtual NxJointType  getType () const =0 
  Retrieve the type of this joint. 
 
virtual void  setName (const char *name)=0 
  Sets a name string for the object that can be retrieved with getName(). 
 
virtual const char *  getName () const =0 
  Retrieves the name string set with setName(). 
 
virtual NxScene &  getScene ()=0 
  Retrieves owner scene. 
 
Limits 
virtual void  setLimitPoint (const NxVec3 &point, bool pointIsOnActor2=true)=0 
  Sets the limit point. 
 
virtual bool  getLimitPoint (NxVec3 &worldLimitPoint)=0 
  Retrieves the global space limit point. 
 
virtual bool  addLimitPlane (const NxVec3 &normal, const NxVec3 &pointInPlane)=0 
  Adds a limit plane. 
 
virtual void  purgeLimitPlanes ()=0 
  deletes all limit planes added to the joint. 
 
virtual void  resetLimitPlaneIterator ()=0 
  Restarts the limit plane iteration. 
 
virtual bool  hasMoreLimitPlanes ()=0 
  Returns true until the iterator reaches the end of the set of limit planes. 
 
virtual bool  getNextLimitPlane (NxVec3 &planeNormal, NxReal &planeD)=0 
  Returns the next element pointed to by the limit plane iterator, and increments the iterator 
*/