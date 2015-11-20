#ifndef _H_ODEBONE
#define _H_ODEBONE

class sODEBone
{

private:

	dBodyID body;
	dGeomID geom;
	int iFrameOwner;
	int iGroupID;

	sObject* pDebugObject;
	int iDebugID;

	float fOffsetX,fOffsetY,fOffsetZ;

	float fTravelDistancePerUpdateX;
	float fTravelDistancePerUpdateY;
	float fTravelDistancePerUpdateZ;

public:
	float fLastPositionX;
	float fLastPositionY;
	float fLastPositionZ;

public:
	D3DXMATRIX matWorld;
	float fSExtentX;
	float fSExtentY;
	float fSExtentZ;
	bool bResponsiveToLinearVelocity;

public:

	sODEBone( );
	~sODEBone( );

	float GetTravelDistancePerUpdateX ( void ) { return fTravelDistancePerUpdateX; }
	float GetTravelDistancePerUpdateY ( void ) { return fTravelDistancePerUpdateY; }
	float GetTravelDistancePerUpdateZ ( void ) { return fTravelDistancePerUpdateZ; }

	void CreateBone( const D3DXMATRIX *pObjMatrix, const sMesh *pMesh, int iBoneID, float fCenterX, float fCenterY, float fCenterZ, float fExtentX, float fExtentY, float fExtentZ, int group, int iDebugObject );
	void UpdateBone( bool bRagdollMoving, float fScale );

	dBodyID GetBody( );
	int GetOwner( );
	int GetGroupID( );
	D3DXMATRIX* GetMatrix( );

};

#endif