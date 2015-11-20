/*
	BulletPhysics PHYSICS COMMANDS
*/

// Includes
#include "BulletPhysics.h"
#include <stack>
#include <stdio.h>
#include <math.h>

// Bullet specific includes
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

// Ragdoll
#include "Ragdoll/DBPro/Include/DBPro.hpp"
#include "Ragdoll/DBProToBullet.h"
#include "Ragdoll/BaseItem.h"
#include "Ragdoll/BaseItemManager.h"
#include "Ragdoll/DBProRagDollBone.h"
#include "Ragdoll/DBProRagDoll.h"
#include "Ragdoll/DBProRagDollManager.h"
#include "Ragdoll/DBProJointManager.h"

//Dave
#include "LinearMath/btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btAabbUtil2.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
//End Dave

//Dave
void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles );

btCompoundShape*	pImporterComopundShape;
int					pImporterMainObjectID;

// Dave - some commands to use for testing purpopses
typedef sObject*(*t_dbGetObject)(long);
t_dbGetObject dbGetObject;
/*typedef DWORD(*t_dbInkey)(DWORD);

//============================================================================

t_dbInkey dbInkey;

dbInkey=(t_dbInkey)GetProcAddress( g_pGlob->g_Camera3D , "?InkeyS@@YAKK@Z" );*/
// End Dave

// EXPORTED FUNCTIONS //////////////////////////////////////////////
DARKSDK void ReceiveCoreDataPtr ( LPVOID pCore );
DARKSDK void Destructor ( void );

// externals to ragdoll system
extern DBProJointManager* jointManager;
extern DBProRagdollManager* ragdollManager;

// Constants
#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_TERRAIN = BIT(0), //<Collide with terrain
    COL_OBJECT = BIT(1), //<Collide with object
    COL_CAPSULECHAR = BIT(2), //<Collide with capsule(character)
};

// World Scaling (turns 1 unit = 1 inch to 1 unit = 1 meter) (75 inches high character becomes 1.8 high in Bullet)
float gSc = 40.0f;

// Mass Scaling (to produce a mass value from the volume dimensions of the object)
float gfVolumeReducer = 100000.0f;
float gMs = 50.0f;

// Capsule default height
float g_fCapsuleHeight = 45.0f;

// Global gravity values
float fCharacterGravity = 900.0f;
float fCharacterFallSpeed = 5000.0f;
float fCharacterMaxSlope = 70.0f;

// Water Line handling
float g_fWaterLineY = 500.0f;

// Globals
GlobStruct* g_pGlob = NULL;
char errStr [ 256 ];

void HandleError( char* szMessage )
{
	MessageBox( NULL, szMessage, "ODE Error", 0 );
	exit(0);
}

// Main Globals
GlobStruct*						g_pGlobal					= NULL;
pfnGetObject					g_pfnGetObject				= NULL;
pfnGetFVFOffsetMap				g_pfnGetFVFOffsetMap		= NULL;
pfnCalculateMeshBounds			g_pfnCalculateMeshBounds	= NULL;
pfnCalcObjectWorld				g_pfnCalcObjectWorld		= NULL;
pfnMakeMeshFromOtherMesh		g_pfnMakeMeshFromOtherMesh	= NULL;
pfnPositionObject				g_pfnPositionObject			= NULL;
pfnPositionObject				g_pfnRotateObject			= NULL;
pfnMakeObjectBox				g_pfnMakeObjectBox			= NULL;
pfnDeleteObject					g_pfnDeleteObject			= NULL;

// Bullet globals
btDiscreteDynamicsWorld* g_dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> g_collisionShapes;
btAlignedObjectArray<btTriangleIndexVertexArray*> g_indexvertexarrays;
btDefaultCollisionConfiguration* g_collisionConfiguration;
btCollisionDispatcher* g_dispatcher;
btSequentialImpulseConstraintSolver* g_solver;
btPairCachingGhostObject* m_ghostObject = NULL;
btKinematicCharacterController* m_character = NULL;
btAxisSweep3* sweepBP = NULL;
btAxisSweep3* m_overlappingPairCache = NULL;
int g_CharacterControlObject = 0;

// ray detect globals
int g_hitObjectNumber = 0;
btVector3 g_hitPointWorld;
btVector3 g_hitNormalWorld;

// Data to control physics system interaction with DBP
struct sObjectList
{
	int iID;
	//sObject* pObject; - DBP could delete object from under Bullet and cause corrupt ptrs (instead use g_pfnGetObject ( iObjectNumber ) )
	btRigidBody* body;
	bool bDynamicUpdate;
	btDefaultMotionState* myMotionState;
	bool bCapsuleControl;
	bool bBouyant;
	LPVOID pMem1;
	LPVOID pMem2;
	float fScaled;
	float fRaised;
};
std::vector < sObjectList > g_ObjectList;

// Internal functions

void ReceiveCoreDataPtr ( LPVOID pCore )
{
	g_pGlobal = ( GlobStruct* ) pCore;
	g_pGlob = g_pGlobal;
	dbGetObject=(t_dbGetObject)GetProcAddress( g_pGlob->g_Basic3D , "?GetObjectA@@YAPAUsObject@@H@Z" );
}

void ODE_AddObject ( int iID, sObject* pObject, btRigidBody* body, int iIsDynamic, btDefaultMotionState* myMotionState, bool bCapsuleMode = false, bool bBouyant = false, LPVOID pMem1 = NULL, LPVOID pMem2 = NULL, float fScaled = 1.0f, float fRaised = 1.0f )
{
	// add object to list
	sObjectList object;
	object.iID = iID;
	//object.pObject = pObject;
	object.body = body;
	if ( iIsDynamic==1 )
		object.bDynamicUpdate = true;
	else
		object.bDynamicUpdate = false;
	object.myMotionState = myMotionState;
	object.bCapsuleControl = bCapsuleMode;
	object.bBouyant = bBouyant;
	object.pMem1 = pMem1;
	object.pMem2 = pMem2;
	object.fScaled = fScaled;
	object.fRaised = fRaised;
	g_ObjectList.push_back ( object );

	// body settings
	if ( body )
	{
		// default body settings
		body->setSleepingThresholds ( 1.0f, 0.25f );
		body->setFriction ( 0.9f );
		body->setAnisotropicFriction ( btVector3(0.9f,0.9f,0.9f), 1 );
		body->setDamping ( 0.001f, 0.005f );

		// submitted by Frank Boyle (stab in the dark software)
		if(body->getInvMass() > 0.0)
		{
			btVector3 bbDimensions;
			btScalar diameter;
			bbDimensions = ((btBoxShape*)body->getCollisionShape())->getImplicitShapeDimensions();
			diameter = bbDimensions.getX();
			diameter = bbDimensions.getY() > 0.0 && bbDimensions.getY() < diameter ? bbDimensions.getY() : diameter;
			diameter = bbDimensions.getZ() > 0.0 && bbDimensions.getZ() < diameter ? bbDimensions.getZ() : diameter;
			body->setCcdMotionThreshold(diameter / 2.0f );
			body->setCcdSweptSphereRadius(diameter);
		}
	}
}

void ODE_RemoveObject ( int iID )
{
	int iDeleteIndex = -1;
	for ( int i = 0; i < (int)g_ObjectList.size ( ); i++ )
	{
		if ( g_ObjectList [ i ].iID == iID )
		{
			iDeleteIndex = i;
			break;
		}
	}
	if ( iDeleteIndex > -1 )
		g_ObjectList.erase ( g_ObjectList.begin() + iDeleteIndex );
}

sObjectList* ODE_FindID ( int iID )
{
	for ( int i = 0; i < (int)g_ObjectList.size ( ); i++ )
	{
		if ( g_ObjectList [ i ].iID == iID )
			return &g_ObjectList [ i ];
	}
	return NULL;
}

void PostTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	for (int j=g_ObjectList.size()-1; j>=0 ;j--)
	{
		btRigidBody* body = g_ObjectList[j].body;
		if ( body==NULL ) continue;
		//sObject* pObject = g_ObjectList[j].pObject;
		sObject* pObject = g_pfnGetObject ( g_ObjectList[j].iID );
		if ( pObject==NULL ) return;
        if ( body!=NULL && body->isActive() )
        {
            btVector3 velocity = body->getLinearVelocity();
            btScalar speed = velocity.length();
            btScalar maxLinearVelocity = 999999.9f;//((DBProMotionState*)body->getMotionState())->GetMaxLinearVelocity();
            if(speed > maxLinearVelocity)
            {
                velocity *= maxLinearVelocity/speed;
                body->setLinearVelocity(velocity);
            }
        }
    }
}

// Initialisation commands

void ODE_Start ( void )
{
	// ragdoll system
	//SAFE_DELETE_ARRAY ( jointManager );
	SAFE_DELETE ( jointManager );
	jointManager = new DBProJointManager();
	//SAFE_DELETE_ARRAY ( ragdollManager );
	SAFE_DELETE ( ragdollManager );
	ragdollManager = new DBProRagdollManager();

	// link to required Basic3D functions
	g_pfnGetObject				= ( pfnGetObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
	g_pfnGetFVFOffsetMap		= ( pfnGetFVFOffsetMap			) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetFVFOffsetMap@@YA_NPAUsMesh@@PAUsOffsetMap@@@Z" );
	g_pfnCalculateMeshBounds	= ( pfnCalculateMeshBounds		) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalculateMeshBounds@@YA_NPAUsMesh@@@Z" );
	g_pfnCalcObjectWorld		= ( pfnCalcObjectWorld			) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalcObjectWorld@@YA_NPAUsObject@@@Z" );
	g_pfnMakeMeshFromOtherMesh	= ( pfnMakeMeshFromOtherMesh	) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeMeshFromOtherMesh@@YA_N_NPAUsMesh@@1PAUD3DXMATRIX@@@Z" );
	g_pfnPositionObject			= ( pfnPositionObject			) GetProcAddress ( g_pGlobal->g_Basic3D, "?Position@@YAXHMMM@Z" );
	g_pfnRotateObject			= ( pfnRotateObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?Rotate@@YAXHMMM@Z" );
	g_pfnMakeObjectBox			= ( pfnMakeObjectBox			) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeBox@@YAXHMMM@Z" );
	g_pfnDeleteObject			= ( pfnDeleteObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?DeleteEx@@YAXH@Z" );

	// clear physics system data
	g_ObjectList.clear();

	// BULLET

	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	g_collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	g_dispatcher = new	btCollisionDispatcher(g_collisionConfiguration);

	///btAxis3Sweep universe
	//btVector3 worldMin(-5000,-25000,-5000);
	//btVector3 worldMax(57000,57000,57000);
	btVector3 worldMin(-500,-2500,-500);
	btVector3 worldMax(1800,4000,1800);
	sweepBP = new btAxisSweep3(worldMin,worldMax);
	m_overlappingPairCache = sweepBP;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	g_solver = new btSequentialImpulseConstraintSolver;

	// create dynamics world
	g_dynamicsWorld = new btDiscreteDynamicsWorld(g_dispatcher,m_overlappingPairCache,g_solver,g_collisionConfiguration);

	// callback to do some processing after each simulation steo
	g_dynamicsWorld->setInternalTickCallback(&PostTickCallback);

	// character centric?
	//g_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration=0.001f; //0.0001f;
	g_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

	// set gravity
	g_dynamicsWorld->setGravity(btVector3(0,-10.0f,0));

	///-----initialization_end-----
}

void DestroyThisObject ( sObjectList* pPhyObject )
{
	if ( pPhyObject )
	{
		// delete allocations for this object
		if ( pPhyObject->myMotionState ) delete pPhyObject->myMotionState;

		// remove ridig body from world
		if ( pPhyObject->body )
		{
			// store shape from ragdoll initially
			btCollisionShape* shape = pPhyObject->body->getCollisionShape();

			// remove body from world then delete body
			g_dynamicsWorld->removeRigidBody(pPhyObject->body);
			delete pPhyObject->body;

			// delete stored collision shape from collision object list and memory
			for (int j=0;j<g_collisionShapes.size();j++)
			{
				if ( g_collisionShapes[j]==shape )
				{
					g_collisionShapes[j] = 0;
					delete shape;
					break;
				}
			}
		}

		// release any memory created to assign physics object
		if ( pPhyObject->pMem1 ) delete[] pPhyObject->pMem1;
		if ( pPhyObject->pMem2 ) delete[] pPhyObject->pMem2;
		pPhyObject->pMem1 = NULL;
		pPhyObject->pMem2 = NULL;

		// use regular DBPro matrix again
		sObject* pObject = g_pfnGetObject ( pPhyObject->iID );
		if ( pObject ) pObject->position.bCustomWorldMatrix = false;

		// when remove item from middle of list, messes up how objects are referenced here!
		//pPhyObject->iID = 0;
		pPhyObject->body = NULL;
		pPhyObject->myMotionState = NULL;
		//pPhyObject->pObject = NULL;
	}
}

void ODE_End ( void )
{
	// ragdoll system termination
	//SAFE_DELETE_ARRAY ( jointManager );
	//SAFE_DELETE_ARRAY ( ragdollManager );
	SAFE_DELETE ( jointManager );
	SAFE_DELETE ( ragdollManager );

	///-----cleanup_start-----
	if ( g_dynamicsWorld )
	{
		// remove character controller if any
		if (m_character)
		{
			g_dynamicsWorld->removeCollisionObject(m_ghostObject);
			m_ghostObject = NULL;
			delete m_character;
			m_character = NULL;
		}

		// reset links to regular DBP objects in case any missed
		//for (int j=g_dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--)
		//	DestroyThisObject ( &g_ObjectList[j] );
		for (int j=g_ObjectList.size()-1; j>=0 ;j--)
			DestroyThisObject ( &g_ObjectList[j] );

		// remove the collision objects from the dynamics world and delete them (any stragglers not deleted by DestroyThisObject)
		for (int i=g_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
		{
			btCollisionObject* obj = g_dynamicsWorld->getCollisionObjectArray()[i];
			g_dynamicsWorld->removeCollisionObject( obj );
			delete obj;
		}

		//delete collision shapes
		for (int j=0;j<g_collisionShapes.size();j++)
		{
			if ( g_collisionShapes[j]!=0 )
			{
				btCollisionShape* shape = g_collisionShapes[j];
				g_collisionShapes[j] = 0;
				delete shape;
			}
		}

		// clear physics system data (no mallocs in structure, just references)
		g_ObjectList.clear();

		// delete physics system resources
		delete g_dynamicsWorld;

		// free indexvertex arrays (only after world destroyed)
		for (int j=0;j<g_indexvertexarrays.size();j++)
		{
			btTriangleIndexVertexArray* pIndexVertexArray = g_indexvertexarrays[j];
			SAFE_DELETE ( pIndexVertexArray );
		}
		g_indexvertexarrays.clear();

		delete g_solver;
		delete m_overlappingPairCache;
		delete g_dispatcher;
		delete g_collisionConfiguration;

		// free main var
		g_dynamicsWorld = NULL;
	}

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	g_collisionShapes.clear();
}

D3DXQUATERNION BT2DX_QUATERNION(const btQuaternion &q)
{
 return D3DXQUATERNION(q.x(), q.y(), q.z(), q.w());
}

D3DXMATRIX BT2DX_MATRIX(const btTransform &ms)
{
 btQuaternion q = ms.getRotation();
 btVector3 p = ms.getOrigin();

 D3DXMATRIX pos, rot, world;
 D3DXMatrixTranslation(&pos, p.x(), p.y(), p.z());
 D3DXMatrixRotationQuaternion(&rot, &BT2DX_QUATERNION(q));
 D3DXMatrixMultiply(&world, &rot, &pos);

 return world;
}

//Dave
float LinearInterpolate ( float y1, float y2, float mu )
{
   return ( y1 * ( 1 - mu ) + y2 * mu );
}

float CosineInterpolate ( float y1, float y2, float mu )
{
 float mu2 = ( 1 - cos ( mu * 3.141f ) ) / 2;
 return ( y1 * ( 1 - mu2 ) + y2 * mu2 );
}
//End

//float fDirX=2.0f; // smooth test

void Update ( float fTimeStep )
{
	// step physics simulation
	// Dave 27/03/2013 - original line that was the cause of the slowdown on less powerful machines
	// g_dynamicsWorld->stepSimulation ( fTimeStep, 20, 1.0f/120.0f ); // highest 120fps lowest 6fps
	// Dave 27/03/2014 - changed to 5 steps which will cater for 12fps (a huge timestep of 0.083f compared to the usual ideal 0.01f)
	// Might need to put the last param back in if people end up walking through walls etc.
	g_dynamicsWorld->stepSimulation (fTimeStep , 7 , 1.0f/120.0f );

	// update ragdoll simulations
	ragdollManager->Update();

	// character controller update
	if ( g_CharacterControlObject > 0 )
	{
		// position of character
		btTransform characterWorldTrans;
		characterWorldTrans = m_ghostObject->getWorldTransform();

		// update object position vector
		float fHorizontalSmoothing = 0.35f;
		float fVerticalSmoothing = 0.5f;
		sObject* p1 = dbGetObject ( g_CharacterControlObject );
		float fMoveX = ( p1->position.vecPosition.x - (characterWorldTrans.getOrigin().getX()*gSc) );
		float fMoveY = ( p1->position.vecPosition.y - (characterWorldTrans.getOrigin().getY()*gSc) );
		float fMoveZ = ( p1->position.vecPosition.z - (characterWorldTrans.getOrigin().getZ()*gSc) );
		fMoveX = ( fMoveX * fHorizontalSmoothing );
		fMoveY = ( fMoveY * fVerticalSmoothing );
		fMoveZ = ( fMoveZ * fHorizontalSmoothing );
		float newX = p1->position.vecPosition.x - fMoveX;
		float newY = p1->position.vecPosition.y - fMoveY;
		float newZ = p1->position.vecPosition.z - fMoveZ;
		g_pfnPositionObject ( g_CharacterControlObject, newX , newY, newZ );

		// update object rotation which returns status (X=whether can jump)
		float fAngleX = 0.0f;
		if ( m_character->wasOnGround() ) fAngleX = 1.0f;
		g_pfnRotateObject ( g_CharacterControlObject, fAngleX, 0.0f, 0.0f );
	}

	//print positions of all objects
	for (int j=g_ObjectList.size()-1; j>=0 ;j--)
	{		
		// get this object
		btRigidBody* body = g_ObjectList[j].body;
		if ( body==NULL ) continue;
		//sObject* pObject = g_ObjectList[j].pObject;
		sObject* pObject = g_pfnGetObject ( g_ObjectList[j].iID );
		if ( pObject==NULL ) continue;

		// If this body has a physics hinge, apply the swingdoor motor affect now
		/* lee - 150914 - this crashes when ujsed with a character capsule object!
		btTypedConstraint* bodyContraint = body->getConstraintRef(0);
		if (bodyContraint != NULL){
			if (bodyContraint->getConstraintType() == HINGE_CONSTRAINT_TYPE){
				btHingeConstraint* hingeC = (btHingeConstraint*)bodyContraint;
				hingeC->setMotorTarget(0,0.0001f);
				btScalar fImpulse = abs(hingeC->getHingeAngle() * 0.0001f);
				//hingeC->getAppliedImpulse
				hingeC->setMaxMotorImpulse(fImpulse);
			}
		}
		*/

		// handle physics body visual update
		if ( body->getMotionState() && g_ObjectList[j].bDynamicUpdate==true )
		{
			// get position from physics body
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);

			// full object control or bCapsuleControl
			if ( g_ObjectList[j].bCapsuleControl )
			{
				// Update DBP object position so can get coordinate back in DBP
				//float fCapsuleCenterToObjPos = pObject->collision.vecColCenter.y;
				//float fCapsuleCenterToObjPos = pObject->collision.vecCentre.y;
				//if ( fCapsuleCenterToObjPos <= 0.0f ) fCapsuleCenterToObjPos = 36.8518448f;
				//fCapsuleCenterToObjPos -= g_ObjectList[j].fRaised;
				float fCapsuleCenterToObjPos = g_ObjectList[j].fRaised;
				g_pfnPositionObject ( g_ObjectList[j].iID, trans.getOrigin().getX()*gSc, (trans.getOrigin().getY()*gSc)-fCapsuleCenterToObjPos, trans.getOrigin().getZ()*gSc );
			}
			else
			{
				// get body angles
				btQuaternion angles;
				angles = body->getOrientation();

				// make the rotation matrix
				D3DXMATRIX matRotation;
				D3DXQUATERNION qRotation;
				qRotation.x = angles.getX();
				qRotation.y = angles.getY();
				qRotation.z = angles.getZ();
				qRotation.w = angles.getW();
				D3DXMatrixRotationQuaternion(&matRotation, &qRotation );

				// LEE, I think the matrix returned is a ZYX order rotation, but we need XYZ
				// so it translates back properly. I could use ZYX in FPSC but it will screw up all maps..
				if ( pObject->position.bApplyPivot )
					matRotation = pObject->position.matPivot * matRotation;

				// Use physics object position
				D3DXMATRIX matTranslation;
				D3DXMatrixTranslation ( &matTranslation, trans.getOrigin().getX()*gSc, trans.getOrigin().getY()*gSc, trans.getOrigin().getZ()*gSc );

				// Use physics object position				
				D3DXMATRIX matCenterColOffset;				
				// lee - 150914 - use Simon's collision center calc (from CreateBox logic).
				//D3DXMatrixTranslation ( &matCenterColOffset, -pObject->collision.vecColCenter.x, -pObject->collision.vecColCenter.y, -pObject->collision.vecColCenter.z );
				float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
				float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
				float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
				float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize/2.0f);
				float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize/2.0f);
				float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize/2.0f); 
				D3DXMatrixTranslation ( &matCenterColOffset, -fXOffset, -fYOffset, -fZOffset);

				// Update DBP object position so can get coordinate back in DBP
				g_pfnPositionObject ( g_ObjectList[j].iID, trans.getOrigin().getX()*gSc, trans.getOrigin().getY()*gSc, trans.getOrigin().getZ()*gSc );

				// final world matrix for physics object back to dbpro object
				D3DXMATRIX matWorld;
				matWorld = pObject->position.matScale * matCenterColOffset * matRotation * matTranslation;
		
				// use ODE matrix, not regular DBPro matrix
				pObject->position.bCustomWorldMatrix = true;
				pObject->position.matWorld			 = matWorld;

				//Dave - update DBPro object position and rotation to match physics
				pObject->position.vecPosition = D3DXVECTOR3 ( matWorld._41, matWorld._42, matWorld._43 );

				D3DXVECTOR3 vecRotate = D3DXVECTOR3 ( 0, 0, 0 );
				AnglesFromMatrix ( &matWorld, &vecRotate );
				pObject->position.vecRotate = vecRotate;
				pObject->collision.bColCenterUpdated = true;
				//Dave end
			}
		}

		// Handle bouyancy
		if ( g_ObjectList[j].bBouyant==true)
		{
			// water line to control strength of bouyancy
			float fWaterLine = g_fWaterLineY - pObject->position.vecPosition.y;

			// SK: Simplified boyancy for now
			if (fWaterLine > 0.0f)
			{
				float fForce = fWaterLine/g_fWaterLineY;
				if (fForce > 0.03f) fForce = 0.03f;
				body->applyImpulse(btVector3(0.0f, fForce/gSc, 0.0f), btVector3(0,0,0));
				body->activate();
				btVector3 vOldVel = body->getLinearVelocity();
				float fYVel = vOldVel.getY();
				if (fYVel > 2.0f) fYVel = 2.0f; // Slow ascent
				// If we're close to the surface, we provide extra damping on the y velocity to avoid bouncing issues
				if (abs(fWaterLine) < 5) fYVel *= 0.975f;
				body->setLinearVelocity(btVector3(vOldVel.getX()*0.975f,fYVel*0.995f,vOldVel.getZ()*0.975f));
				vOldVel = body->getAngularVelocity();
				body->setAngularVelocity(btVector3(vOldVel.getX()*0.99f,fYVel*0.99f,vOldVel.getZ()*0.99f));
				body->applyDamping(0.5f);

			}
			/*
			// two largest axis (so bouyancy produces flat raft effect)
			float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
			float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
			float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
			int iBiggestAxis = 0;
			float fBiggest = fXSize;
			if ( fYSize > fBiggest ) { iBiggestAxis=1; fBiggest = fYSize; }
			if ( fZSize > fBiggest ) { iBiggestAxis=2; fBiggest = fZSize; }
			int iSecondBiggestAxis = 0;
			float fSecondBiggest = fXSize;
			if ( iBiggestAxis==0 ) { iSecondBiggestAxis=1; fSecondBiggest=fYSize; }
			if ( fZSize > fSecondBiggest && iBiggestAxis!=2 ) { iSecondBiggestAxis=2; fSecondBiggest = fZSize; }
			D3DXVECTOR3 vecFloaters = D3DXVECTOR3(0,0,0);
			if ( iBiggestAxis==0 ) vecFloaters.x = fBiggest;
			if ( iBiggestAxis==1 ) vecFloaters.y = fBiggest;
			if ( iBiggestAxis==2 ) vecFloaters.z = fBiggest;
			if ( iSecondBiggestAxis==0 ) vecFloaters.x = fSecondBiggest;
			if ( iSecondBiggestAxis==1 ) vecFloaters.y = fSecondBiggest;
			if ( iSecondBiggestAxis==2 ) vecFloaters.z = fSecondBiggest;
			int iSmallestAxis = 0;
			if ( iBiggestAxis==0 || iSecondBiggestAxis==0 ) iSmallestAxis = 1;
			if ( iBiggestAxis==1 || iSecondBiggestAxis==1 ) iSmallestAxis = 2;
			float fSmallestDim = fXSize;
			if ( iSmallestAxis==1 ) fSmallestDim = fYSize;
			if ( iSmallestAxis==2 ) fSmallestDim = fZSize;

			// make the rotation matrix
			D3DXMATRIX matRotation;
			D3DXQUATERNION qRotation;
			btQuaternion angles;
			angles = body->getOrientation();
			qRotation.x = angles.getX();
			qRotation.y = angles.getY();
			qRotation.z = angles.getZ();
			qRotation.w = angles.getW();
			D3DXMatrixRotationQuaternion ( &matRotation, &qRotation );
			if ( pObject->position.bApplyPivot )
				matRotation = pObject->position.matPivot * matRotation;

			// four primary points
			for ( int ppi=0; ppi<9; ppi++ )
			{
				// model space floater locations
				D3DXVECTOR3 vecFP;
				if ( ppi==0 ) vecFP = D3DXVECTOR3(vecFloaters.x,vecFloaters.y,vecFloaters.z);
				if ( ppi==1 ) vecFP = D3DXVECTOR3(vecFloaters.x,vecFloaters.y,-vecFloaters.z);
				if ( ppi==2 ) vecFP = D3DXVECTOR3(vecFloaters.x,-vecFloaters.y,vecFloaters.z);
				if ( ppi==3 ) vecFP = D3DXVECTOR3(vecFloaters.x,-vecFloaters.y,-vecFloaters.z);
				if ( ppi==4 ) vecFP = D3DXVECTOR3(-vecFloaters.x,vecFloaters.y,vecFloaters.z);
				if ( ppi==5 ) vecFP = D3DXVECTOR3(-vecFloaters.x,vecFloaters.y,-vecFloaters.z);
				if ( ppi==6 ) vecFP = D3DXVECTOR3(-vecFloaters.x,-vecFloaters.y,vecFloaters.z);
				if ( ppi==7 ) vecFP = D3DXVECTOR3(-vecFloaters.x,-vecFloaters.y,-vecFloaters.z);
				if ( ppi==8 ) vecFP = D3DXVECTOR3(0,0,0);

				// rotate vectors to current object matrix
				D3DXVec3TransformCoord(&vecFP, &vecFP, &matRotation );

				// if beneath water line, apply impulse
				float fPressureStr = (fXSize*fYSize*fZSize)/85000000.0f;
				float fDepthOfThisPoint = (fWaterLine - vecFP.y) + 20.0f;
				if ( fDepthOfThisPoint > 0.0f )
				{
					// locate points on physics body for force application
					btVector3 floatationpoint = btVector3(vecFP.x, vecFP.y, vecFP.z);					
					float fMag = fDepthOfThisPoint / 50.0f;
					if ( fMag > 1.0f ) fMag = 1.0f;
					float fUpwardForce = fPressureStr * (50.0f*fMag);
					//body->applyImpulse(btVector3(0.0f, fUpwardForce, 0.0f), floatationpoint);
					body->applyImpulse(btVector3(0.0f, fUpwardForce/gSc, 0.0f), floatationpoint);
					body->activate();
					btVector3 vOldVel = body->getLinearVelocity();
					body->setLinearVelocity(btVector3(vOldVel.getX()*0.999f,vOldVel.getY()*0.995f,vOldVel.getZ()*0.999f));
					body->setAngularVelocity(body->getAngularVelocity()*0.995f);
					body->applyDamping(0.3f);				
				}
			}
			*/
		}
	}
}

void ODE_Update ( void )
{
	Update ( 1.0f/60.0f );
}

void ODE_Update ( float fManualStep )
{
	Update ( fManualStep );
}

// Creation commands

void CreateBox ( int iObjectNumber, int isDynamic, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution,
				 bool blnManual, float fXIN, float fYIN, float fZIN, float fXSizeIN, float fYSizeIN, float fZSizeIN, float fXAngIN, float fYAngIN, float fZAngIN)
{
	// defaults
	if ( fWeight==-1 ) fWeight = 100.0f;
	if ( fFriction==-1 ) fFriction = 100.0f;
	if ( fRestitution==-1 ) fRestitution = 0.0f;

	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	if (blnManual)
	{
		fXPos = fXIN;
		fYPos = fYIN;
		fZPos = fZIN;
		fXSize = fXSizeIN;
		fYSize = fYSizeIN;
		fZSize = fZSizeIN;
		fXRot = D3DXToRadian (fXAngIN);
		fYRot = D3DXToRadian (fYAngIN);
		fZRot = D3DXToRadian (fZAngIN);
	}
	else
	{
		fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
		fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
		fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
		fXPos	 = pObject->position.vecPosition.x;
		fYPos	 = pObject->position.vecPosition.y;
		fZPos	 = pObject->position.vecPosition.z;
		fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
		fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
		fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

		// adjust for center of object as all physics objects coords are at center
		// SK: Existing collision.vecColCenter appears buggy and does not correctly report collision centre in all cases	
		float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize/2.0f);
		float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize/2.0f);
		float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize/2.0f); 
		D3DXVECTOR3 vColOffset = D3DXVECTOR3(fXOffset,fYOffset,fZOffset);
		//pObject->collision.vecColCenter = vColOffset; // override colcenter so Update() uses exact reverse of this physics offset :)
		D3DXMATRIX matRot;
		D3DXMatrixRotationX(&matRot,fXRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
		D3DXMatrixRotationY(&matRot,fYRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
		D3DXMatrixRotationZ(&matRot,fZRot); D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
		fXPos += vColOffset.x;
		fYPos += vColOffset.y;
		fZPos += vColOffset.z;
	}
	
	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// if limb number specified
	if ( iLimbNumber!=-1 )
	{
		// change boundbox to that of the limb
		sFrame* pLimbFrame = pRealObject->ppFrameList[iLimbNumber];
		if ( pLimbFrame )
		{
			sMesh* pLimbMesh = pLimbFrame->pMesh;
			if ( pLimbMesh )
			{
				fXSize = ( pLimbMesh->Collision.vecMax.x - pLimbMesh->Collision.vecMin.x );
				fYSize = ( pLimbMesh->Collision.vecMax.y - pLimbMesh->Collision.vecMin.y );
				fZSize = ( pLimbMesh->Collision.vecMax.z - pLimbMesh->Collision.vecMin.z );
				D3DXVECTOR3 vLimbCenter = D3DXVECTOR3(pLimbMesh->Collision.vecMin.x+(fXSize/2),pLimbMesh->Collision.vecMin.y+(fYSize/2),pLimbMesh->Collision.vecMin.z+(fZSize/2));
				D3DXVec3TransformCoord ( &vLimbCenter, &vLimbCenter, &pLimbFrame->matAbsoluteWorld );
				fXPos = vLimbCenter.x;
				fYPos = vLimbCenter.y;
				fZPos = vLimbCenter.z;
				fXSize = fXSize * pObject->position.vecScale.x;
				fYSize = fYSize * pObject->position.vecScale.y;
				fZSize = fZSize * pObject->position.vecScale.z;
			}
		}
	}

	// ensure very small objects are not allowed below this size
	if ( fXSize < 3 ) fXSize = 3;
	if ( fYSize < 6 ) fYSize = 6;
	if ( fZSize < 6 ) fZSize = 6;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// modify with weight modifier
	fVolume *= (fWeight/100.0f);

	// can specify speciaal collision type through iTipOverMode
	bool bDoNotMoveAnyOtherObjects = false;
	if ( iTipOverMode==11 ) { bDoNotMoveAnyOtherObjects=true; iTipOverMode = 0; }

	// create a bullet shape
	btCollisionShape* boxShape = NULL;
	if ( iTipOverMode==0 )
	{
		// regular box
		boxShape = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));
	}
	else
	{
		// create additional shape for a TipOver shape (balancing on gun nozzle)
		btCompoundShape* pCompound = new btCompoundShape();
		btTransform localTrans;
		localTrans.setIdentity();
		btCollisionShape* pShape1 = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));
		float fXSize2 = fXSize * 1.5f;
		float fYSize2 = fYSize * 1.5f;
		float fZSize2 = fZSize * 1.5f;
		if ( iTipOverMode==1 ) fXSize2 = 0.005f/gSc;
		if ( iTipOverMode==2 ) fYSize2 = 0.005f/gSc;
		if ( iTipOverMode==3 ) fZSize2 = 0.005f/gSc;
		btCollisionShape* pShape2 = new btBoxShape(btVector3(btScalar(fXSize2/2.0f),btScalar(fYSize2/2.0f),btScalar(fZSize2/2.0f)));
		pCompound->addChildShape(localTrans, pShape1);
		pCompound->addChildShape(localTrans, pShape2);
		boxShape = (btCollisionShape*)pCompound;
	}

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(boxShape);

	// mass calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) boxShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,boxShape,localInertia);
	rbInfo.m_restitution = fRestitution/100.0f;
	rbInfo.m_friction = fFriction/50.0f;
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// also remove all contact response
	if ( bDoNotMoveAnyOtherObjects==true )
	{
		body->setCollisionFlags ( btCollisionObject::CF_NO_CONTACT_RESPONSE );
	}

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL );
}

void StartStaticObject ( int iObjectNumber )
{
	pImporterComopundShape = new btCompoundShape;
	pImporterMainObjectID = iObjectNumber;
}

void EndStaticObject ( int iObjectNumber, int iTerrainMaterial )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// adjust for center of object as all physics objects coords are at center
	// SK: Existing collision.vecColCenter appears buggy and does not correctly report collision centre in all cases	
	/*float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize/2.0f);
	float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize/2.0f);
	float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize/2.0f); 
	D3DXVECTOR3 vColOffset = D3DXVECTOR3(fXOffset,fYOffset,fZOffset);*/
	D3DXVECTOR3 vColOffset = D3DXVECTOR3(0,0,0);
	D3DXMATRIX matRot;
	D3DXMatrixRotationX(&matRot,fXRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	D3DXMatrixRotationY(&matRot,fYRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	D3DXMatrixRotationZ(&matRot,fZRot); D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	fXPos += vColOffset.x;
	fYPos += vColOffset.y;
	fZPos += vColOffset.z;
	
	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// ensure very small objects are not allowed below this size
	if ( fXSize < 6 ) fXSize = 6;
	if ( fYSize < 6 ) fYSize = 6;
	if ( fZSize < 6 ) fZSize = 6;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(pImporterComopundShape);

	// mass/volume calculation
	btScalar mass(0);
	btVector3 localInertia(0,0,0);

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,pImporterComopundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	if ( iTerrainMaterial==1 )
		g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);
	else
		g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	pObject = NULL;

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, 0, myMotionState, false, bBouyant, NULL, NULL );

}

int CreateSwingDoorHinge(int iObjectNumber, float fX, float fY, float fZ, float fAng)
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	btHingeConstraint *hingeC = new btHingeConstraint(*(pPhyObject->body), 
												      btVector3(fX/gSc,fY/gSc,fZ/gSc),
													  btVector3(0,1,0),
													  true);
	//hingeC->setLimit(D3DXToRadian(-135),D3DXToRadian(135),0.5f,0.3f,0.5f);
	//hingeC->enableMotor(true);
	//hingeC->setMotorTarget(0,0.001f); // Also set every frame in update function	
	g_dynamicsWorld->addConstraint(hingeC, true);
	return 1;
}

void AddStaticObjectBox ( int iObjectNumber , int iStaticBoxObject)
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iStaticBoxObject );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// adjust for center of object as all physics objects coords are at center
	// SK: Existing collision.vecColCenter appears buggy and does not correctly report collision centre in all cases	
	float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize/2.0f);
	float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize/2.0f);
	float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize/2.0f); 
	D3DXVECTOR3 vColOffset = D3DXVECTOR3(fXOffset,fYOffset,fZOffset);
	D3DXMATRIX matRot;
	D3DXMatrixRotationX(&matRot,fXRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	D3DXMatrixRotationY(&matRot,fYRot);	D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	D3DXMatrixRotationZ(&matRot,fZRot); D3DXVec3TransformCoord ( &vColOffset, &vColOffset, &matRot);
	fXPos += vColOffset.x;
	fYPos += vColOffset.y;
	fZPos += vColOffset.z;
	
	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// ensure very small objects are not allowed below this size
	if ( fXSize < 6 ) fXSize = 6;
	if ( fYSize < 6 ) fYSize = 6;
	if ( fZSize < 6 ) fZSize = 6;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// create a bullet shape
	btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(boxShape);

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));
	pImporterComopundShape->addChildShape    ( startTransform, boxShape );
}

void CreateSphere ( int iObjectNumber, int isDynamic )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fRadius = pObject->collision.fRadius * pObject->position.vecScale.x;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// adjust for center of object as all physics objects coords are at center
	fXPos += pObject->collision.vecColCenter.x;
	fYPos += pObject->collision.vecColCenter.y;
	fZPos += pObject->collision.vecColCenter.z;

	// just before shape creation, reduce scale of size and position
	fRadius /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fRadiusCubed = fRadius*fRadius*fRadius;
	float fVolume = ((4.0f/3.0f) * fRadiusCubed) * 3.14159f;

	// create a dynamic rigidbody
	btCollisionShape* colShape = new btSphereShape(btScalar(fRadius));
	g_collisionShapes.push_back(colShape);

	// mass/volume calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) colShape->calculateLocalInertia(mass,localInertia);

	// physics object pos and rotation
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL );
}

void CreateCylinder ( int iObjectNumber, int isDynamic, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot)
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;	

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// create a bullet shape
	btCollisionShape* cylinderShape = new btCylinderShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(cylinderShape);

	// mass/volume calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) cylinderShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,cylinderShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL );
}

void CreateCylinder ( int iObjectNumber, int isDynamic )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// adjust for center of object as all physics objects coords are at center
	// SK: Existing collision.vecColCenter appears buggy and does not correctly report collision centre in all cases
	fXPos += pObject->collision.vecMin.x * pObject->position.vecScale.x + fXSize/2.0f;
	fYPos += pObject->collision.vecMin.y * pObject->position.vecScale.y + fYSize/2.0f;
	fZPos += pObject->collision.vecMin.z * pObject->position.vecScale.z + fZSize/2.0f;

	CreateCylinder(iObjectNumber, isDynamic, fXPos, fYPos, fZPos, fXSize, fYSize, fZSize, fXRot, fYRot, fZRot);
}

void CreateMesh ( int iObjectNumber, int isDynamic, int iLimbNumber, int iTerrainMesh, int iCollisionScaling, int iHullReduction )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// real object for mesh data
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;

	// scaling modifier
	float fScalingFactor = (float)iCollisionScaling / 100.0f;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x * fScalingFactor;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y * fScalingFactor;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z * fScalingFactor;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// determine if using LOD, if so, choose lowest
	int iChooseLowLOD = -1;
	int iBestLowLOD = 0;
	for ( int iFrame=0; iFrame<pRealObject->iFrameCount; iFrame++ )
	{
		sFrame* pFrame = pRealObject->ppFrameList[iFrame];
		if ( pFrame )
		{
			if ( stricmp ( pFrame->szName, "lod_0" )==NULL && iBestLowLOD<1 ) { iChooseLowLOD = iFrame; iBestLowLOD=1; }
			if ( stricmp ( pFrame->szName, "lod_1" )==NULL && iBestLowLOD<2 ) { iChooseLowLOD = iFrame; iBestLowLOD=2; }
			if ( stricmp ( pFrame->szName, "lod_2" )==NULL && iBestLowLOD<3 ) { iChooseLowLOD = iFrame; iBestLowLOD=3; }
		}
	}

	// do I need to create new data, or can I use mesh data directly in mesh builder? (save memory)
	int totalVerts = 0;
	int totalIndices = 0;
	for ( int iFrame=0; iFrame<pRealObject->iFrameCount; iFrame++ )
	{
		if ( iChooseLowLOD==-1 || iChooseLowLOD==iFrame )
		{
			sFrame* pFrame = pRealObject->ppFrameList[iFrame];
			if ( pFrame)
			{
				sMesh* pMesh = pFrame->pMesh;
				if ( pMesh )
				{
					totalVerts = totalVerts + pMesh->dwVertexCount;
					totalIndices = totalIndices + pMesh->dwIndexCount;
				}
			}
		}
	}

	// if more vertices than a WORD can index, use 32bit indices
	bool bDWORDSizeIndices = false;
	if ( totalVerts >= 0xFFFF )
	{
		bDWORDSizeIndices = true;
	}

	float* gVertices = new float[totalVerts*4];
	memset ( gVertices, 0, sizeof(float)*totalVerts*4 );
	LPVOID gIndices = NULL;
	if ( bDWORDSizeIndices==true )
	{
		gIndices = (LPVOID)new DWORD[totalIndices];
	 	memset ( gIndices, 0, sizeof(DWORD)*totalIndices );
	}
	else
	{
		gIndices = (LPVOID)new WORD[totalIndices];
	 	memset ( gIndices, 0, sizeof(WORD)*totalIndices );
	}
	int vcount = 0, icount = 0;
	for ( int iFrame=0; iFrame<pRealObject->iFrameCount; iFrame++ )
	{
		if ( iChooseLowLOD==-1 || iChooseLowLOD==iFrame )
		{
			sFrame* pFrame = pRealObject->ppFrameList[iFrame];
			if ( pFrame)
			{
				sMesh* pMesh = pFrame->pMesh;
				if ( pMesh )
				{
					// transform matrices for frame/mesh
					D3DXMATRIX matFinal;
					if ( pMesh->pBones )
					{
						matFinal = pMesh->pBones[0].matTranslation;
						D3DXMatrixMultiply ( &matFinal, &matFinal, &pFrame->matCombined );
					}
					else
					{
						matFinal = pFrame->matCombined;
					}

					// apply scaling
					D3DXMATRIX matScale;
					D3DXMatrixScaling ( &matScale, (pObject->position.vecScale.x*fScalingFactor)/gSc, (pObject->position.vecScale.y*fScalingFactor)/gSc, (pObject->position.vecScale.z*fScalingFactor)/gSc );
					D3DXMatrixMultiply ( &matFinal, &matFinal, &matScale );

					// copy vertex to physics mesh buffer
					float* pVertData = (float*)pMesh->pVertexData;
					DWORD dwSkip = pMesh->dwFVFSize/sizeof(float);
					for ( int v=0; v<(int)pMesh->dwVertexCount; v++ )
					{
						D3DXVECTOR3 vec = D3DXVECTOR3(*(pVertData+0),*(pVertData+1),*(pVertData+2));
						D3DXVec3TransformCoord ( &vec, &vec, &matFinal );
						*(gVertices+((vcount+v)*4)+0) = vec.x;
						*(gVertices+((vcount+v)*4)+1) = vec.y;
						*(gVertices+((vcount+v)*4)+2) = vec.z;
						*(gVertices+((vcount+v)*4)+3) = 1.0f;
						pVertData+=dwSkip;
					}
					WORD* pIndexData = (WORD*)pMesh->pIndices;
					for ( int i=0; i<(int)pMesh->dwIndexCount; i++ )
					{
						if ( bDWORDSizeIndices==true )
							((DWORD*)gIndices)[icount+i] = vcount + (*pIndexData);
						else
							((WORD*)gIndices)[icount+i] = vcount + (*pIndexData);
						pIndexData++;
					}
					vcount+=pMesh->dwVertexCount;
					icount+=pMesh->dwIndexCount;
				}
			}
		}
	}

	// index based mesh or verts only
	if ( totalIndices==0 )
	{
		// verts only, so CREATE index list
		totalIndices = totalVerts;
		gIndices = (LPVOID)new DWORD[totalIndices];
		memset ( gIndices, 0, sizeof(DWORD)*totalIndices );
		for ( int n=0; n<totalIndices; n++ ) ((DWORD*)gIndices)[n] = n;
		bDWORDSizeIndices = true;
	}

	// index based mesh
	const int totalTriangles = totalIndices / 3;
	int vertStride = sizeof(float)*4;
	btIndexedMesh btMesh;
	btMesh.m_numVertices = totalVerts;
	btMesh.m_vertexBase=(const unsigned char*)gVertices;
	btMesh.m_vertexStride = vertStride;
	btMesh.m_numTriangles = totalTriangles;
	btMesh.m_triangleIndexBase=(const unsigned char*)gIndices;
	btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray();
	if ( bDWORDSizeIndices==true )
	{
		btMesh.m_indexType = PHY_INTEGER;
		btMesh.m_triangleIndexStride = 3*sizeof(DWORD);
		m_indexVertexArrays->addIndexedMesh(btMesh,PHY_INTEGER);
	}
	else
	{
		btMesh.m_indexType = PHY_SHORT;
		btMesh.m_triangleIndexStride = 3*sizeof(WORD);
		m_indexVertexArrays->addIndexedMesh(btMesh,PHY_SHORT);
	}

	// these are deleted ONLY when world is destroyed
	g_indexvertexarrays.push_back(m_indexVertexArrays);

	// create a rigidbody
	bool useQuantizedAabbCompression = true;
	btVector3 aabbMin(pObject->collision.vecMin.x/gSc,pObject->collision.vecMin.y/gSc,pObject->collision.vecMin.z/gSc),aabbMax(pObject->collision.vecMax.x/gSc,pObject->collision.vecMax.y/gSc,pObject->collision.vecMax.z/gSc);
	bool buildBvh = false; // will make raycasting/etc slower!
	btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression,aabbMin,aabbMax,buildBvh);
	trimeshShape->buildOptimizedBvh(); // done manually here for slightly faster build time?!?

	// convex hull optimization if flagged
	//btCollisionShape pMyShape = trimeshShape;
	//if ( iHullReduction > 0 )
	//{
	//	btAlignedObjectArray<btVector3> &vetexBuffer = NULL;
	//	btConvexHullShape* collShape = new btConvexHullShape(&(vetexBuffer[0].getX()),vetexBuffer.size());
	//	collShape->setMargin(CONVEX_MARGIN);
    //    btShapeHull* hull = new btShapeHull(collShape);
    //    btScalar margin = collShape->getMargin();
    //    hull->buildHull(margin);
    //    btConvexHullShape* simplifiedConvexShape = new btConvexHullShape(&(hull->getVertexPointer()[0].getX()),hull->numVertices());
    //    SAFE_DELETE(collShape);
    //    pMyShape = (btCollisionShape)simplifiedConvexShape;
	//}

	//bool buildBvh = false; // will make raycasting/etc slower!
	//btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression,aabbMin,aabbMax,false);
	//trimeshShape->buildOptimizedBvh(); // done manually here for slightly faster build time?!?
	g_collisionShapes.push_back(trimeshShape);

	// mass/volume calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) trimeshShape->calculateLocalInertia(mass,localInertia);

	// physics object pos and rotation
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,trimeshShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	if ( iTerrainMesh==1 )
		g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);
	else
		g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	/* yes this works, might need it in the future - going to use stripped out OBJECTS for now (quicker)
	// experiment to view vertex data in trianglemesh so I can interoggate it later
	// when I need to send physics collision goemetry to occlusion system for depth render floor
	// the user pointer shouldn't already be used for other purposes
	btBvhTriangleMeshShape* getTrimeshShape = (btBvhTriangleMeshShape*)body->getCollisionShape();
	if ( getTrimeshShape )
	{
		// get mesh interface and scaling
		btStridingMeshInterface* meshInterface = trimeshShape->getMeshInterface();
		const btVector3& meshScaling = meshInterface->getScaling();

		// go through all sub parts of triangle mesh
		for (int partId = 0; partId< meshInterface->getNumSubParts();partId++)
		{
			const unsigned char *vertexbase = 0;
			int numverts = 0;
			PHY_ScalarType type = PHY_INTEGER;
			int stride = 0;
			const unsigned char *indexbase = 0;
			int indexstride = 0;
			int numfaces = 0;
			PHY_ScalarType indicestype = PHY_INTEGER;
			meshInterface->getLockedReadOnlyVertexIndexBase(&vertexbase,numverts,type,stride,&indexbase,indexstride,numfaces,indicestype,partId);
			for (int triangleIndex = 0 ; triangleIndex < numfaces;triangleIndex++)
			{
				// get triangle data
				unsigned int* gfxbase = (unsigned int*)(indexbase+triangleIndex*indexstride);
				for (int j=2;j>=0;j--)
				{
					int graphicsindex = indicestype==PHY_SHORT?((unsigned short*)gfxbase)[j]:gfxbase[j];
					if (type == PHY_FLOAT)
					{
						float* graphicsbase = (float*)(vertexbase+graphicsindex*stride);
						float fX = graphicsbase[0]*meshScaling.getX();
						float fY = graphicsbase[1]*meshScaling.getY();
						float fZ = graphicsbase[2]*meshScaling.getZ();
					}
					else
					{
						double* graphicsbase = (double*)(vertexbase+graphicsindex*stride);
						float fX = btScalar(graphicsbase[0]*meshScaling.getX());
						float fY = btScalar(graphicsbase[1]*meshScaling.getY());
						float fZ = btScalar(graphicsbase[2]*meshScaling.getZ());
					}
				}
			}

			// unlock when finished
			meshInterface->unLockReadOnlyVertexBase ( partId );
		}
	}
	*/

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, gVertices, gIndices );
}

void CreateTerrain ( int iObjectNumber, int isDynamic, int iWidth, int iLength, DWORD dwMemBlockPtr )
{
	// problem is that Blitzwerks uses alternating quad edge order which means polygon slope cannot be
	// predicted by Bullet height map shape (pity as it would have been low memory fast performance

	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// create a bullet shape
	int s_gridSize = iWidth; // 1024x1024
	float m_minHeight = 0.0f;
	float m_maxHeight = 50000.0f/gSc;
	int m_upAxis = 1;
	bool flipQuadEdges = true;
	float* m_rawHeightfieldData = new float[s_gridSize*s_gridSize];
	memset ( m_rawHeightfieldData, 0, sizeof(float)*s_gridSize*s_gridSize );
	float* pMemBlockPtr = (float*)dwMemBlockPtr;
	for ( int t=0; t<s_gridSize*s_gridSize; t++ )
	{
		m_rawHeightfieldData[t] = (*pMemBlockPtr) / gSc;
		pMemBlockPtr++;
	}
	btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(	s_gridSize, s_gridSize,
																					m_rawHeightfieldData,
																					0.0f,
																					m_minHeight, m_maxHeight,
																					m_upAxis, PHY_FLOAT, flipQuadEdges);
	btVector3 localScaling = btVector3(50.0f/gSc,1.0f,50.0f/gSc);
	terrainShape->setLocalScaling(localScaling);

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(terrainShape);

	// fully static is our ground terrain!!
	btScalar mass(0/gMs);
	btVector3 localInertia(0,0,0);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,terrainShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, (LPVOID)m_rawHeightfieldData, NULL );
}

void UpdateTerrain ( int iObjectNumber, int isDynamic, int iWidth, int iLength, DWORD dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	// NOTE: Terrain collision replaced with sector meshes (which will need to be updated instead!!)
	float* m_rawHeightfieldData = (float*)pPhyObject->pMem1;
	if ( m_rawHeightfieldData==NULL ) return;

	// create a bullet shape
	int s_gridSize = iWidth; // 1024x1024
	float m_minHeight = 0.0f;
	float m_maxHeight = 50000.0f/gSc;
	int m_upAxis = 1;
	bool flipQuadEdges = true;
	float* pMemBlockPtr = (float*)dwMemBlockPtr;
	for ( int y=iZ1; y<=iZ2; y++ )
	{
		for ( int x=iX1; x<=iX2; x++ )
		{
			int t = (y*s_gridSize)+x;
			m_rawHeightfieldData[t] = (*pMemBlockPtr) / gSc;
			pMemBlockPtr++;
		}
	}

	// remove body from simulation
	g_dynamicsWorld->removeRigidBody(pPhyObject->body);

	// remove collision shape
	btHeightfieldTerrainShape* terrainOLDShape = (btHeightfieldTerrainShape*)pPhyObject->body->getCollisionShape();
	delete terrainOLDShape;

	// create new shape
	btHeightfieldTerrainShape* terrainNEWShape = new btHeightfieldTerrainShape(	s_gridSize, s_gridSize,
																					m_rawHeightfieldData,
																					0.0f,
																					m_minHeight, m_maxHeight,
																					m_upAxis, PHY_FLOAT, flipQuadEdges);

	// prepare new shape intertia
	btVector3 localScaling = btVector3(50.0f/gSc,1.0f,50.0f/gSc);
	terrainNEWShape->setLocalScaling(localScaling);

	// assign new shape
	pPhyObject->body->setCollisionShape(terrainNEWShape);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	pPhyObject->body->setUserPointer((void*)iArbValue);

	// add new body back to simulation
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( pPhyObject->body, COL_TERRAIN, sCollidesWith);

	// replace shape in collision shapes list
	for (int j=0;j<g_collisionShapes.size();j++)
	{
		if ( g_collisionShapes[j]==terrainOLDShape )
		{
			g_collisionShapes[j] = terrainNEWShape;
			break;
		}
	}
}

void CreateCapsule ( int iObjectNumber, int isDynamic, float fScaleModifier, float fRaised )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// instead of fixed capsule height, calc proportion so a character 73 units high gives capsule of 80(half=40)
	float fProportionalCapsuleHeight = (fYSize*gSc)*0.55f;

	// apply modifier
	if ( fScaleModifier==0.0f ) fScaleModifier = 1.0f;
	float fCapsuleHeight = fProportionalCapsuleHeight/gSc;
	fCapsuleHeight = fCapsuleHeight * fScaleModifier;

	// somenow affects final height of capsule in simulation, find out metrics!!
	// also use proportional radius for capsule (fat animals; cows)
	float fProportionalCapsuleRadius = (((fXSize+fZSize)/2.0f/2.0f)*gSc)*0.967f;

	// will offset real object by HALF of total height of capsule (sphere halves+cylinder)
	fRaised = fRaised + ((fProportionalCapsuleRadius*2)+fProportionalCapsuleHeight)/2.0f;

	// create a bullet shape
	//float fCapsuleWidth = 15.0f/gSc;
	float fCapsuleWidth = fProportionalCapsuleRadius/gSc;
	btCollisionShape* capsuleShape = new btCapsuleShape(fCapsuleWidth, fCapsuleHeight);

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(capsuleShape);

	// mass/volume calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) capsuleShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,capsuleShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// ensure capsule stands upright
	body->setAngularFactor(0.0f);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_CAPSULECHAR, sCollidesWith);

	// add physics object details to pbject list
	bool bBouyant = false;
	ODE_AddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, true, bBouyant, NULL, NULL, fScaleModifier, fRaised );
}

void ODE_CreateDynamicCharacterController ( int iObjectNumber, float fGravity, float fFallSpeed, float fMaxSlope )
{
	// set new global defaults for character controller
	fCharacterGravity = fGravity;
	fCharacterFallSpeed = fFallSpeed;
	fCharacterMaxSlope = fMaxSlope;

	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// set position transform for physics object
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fXPos,fYPos,fZPos));
	D3DXQUATERNION QuatAroundX(1,0,0,fXRot);
	D3DXQUATERNION QuatAroundY(0,1,0,fYRot);
	D3DXQUATERNION QuatAroundZ(0,0,1,fZRot);
	D3DXQuaternionRotationAxis(&QuatAroundX,&D3DXVECTOR3(1,0,0),fXRot);
	D3DXQuaternionRotationAxis(&QuatAroundY,&D3DXVECTOR3(0,1,0),fYRot);
	D3DXQuaternionRotationAxis(&QuatAroundZ,&D3DXVECTOR3(0,0,1),fZRot);
	D3DXQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	startTransform.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));

	// create a character controller shape (if not already created)
	if ( m_ghostObject==NULL )
	{
		// create a new
		m_ghostObject = new btPairCachingGhostObject();
		m_ghostObject->setWorldTransform(startTransform);
		sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		btScalar characterWidth  = 15.0f/gSc;
		btScalar characterHeight = 40.0f/gSc;
		btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
		m_ghostObject->setCollisionShape (capsule);
		m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
		btScalar stepHeight = btScalar(10.35/gSc);
		m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight);

		// only collide with static for now (no interaction with dynamic objects)
		g_dynamicsWorld->addCollisionObject ( m_ghostObject, COL_OBJECT, COL_OBJECT | COL_TERRAIN );
		g_dynamicsWorld->addAction(m_character);
	}
	else
	{
		// simply reposition existing
		m_ghostObject->setWorldTransform(startTransform);
	}

	// required for character
	g_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(),g_dynamicsWorld->getDispatcher());

	// setup character settings
	m_character->reset ();
	m_character->resetFallStuff();
	m_character->setGravity(fCharacterGravity/gSc); 
	m_character->setFallSpeed(fCharacterFallSpeed/gSc);
	m_character->setMaxSlope(D3DXToRadian(fCharacterMaxSlope));
	m_character->warp (btVector3(fXPos,fYPos,fZPos));
	m_character->m_bCurrentlyDucked = false;

	// add physics object details to pbject list
	bool bBouyant = false;
	ODE_AddObject ( iObjectNumber, pObject, NULL, 1, NULL, false, bBouyant, NULL, NULL );
}

void ODE_SetDynamicCharacterController ( int iObjectNumber, float fWaterLineY, float fRes1, float fRes2, float fRes3, float fRes4, float fRes5, float fRes6 )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;

	// set properties for the character on the fly
	m_character->setWaterLineY ( fWaterLineY );
}

void ODE_SetWaterLine ( float fWaterLineY )
{
	// set properties for the character on the fly
	g_fWaterLineY = fWaterLineY;
	if ( m_character ) m_character->setWaterLineY ( fWaterLineY );
}

void ODE_ControlDynamicCharacterController ( int iObjectNumber, float fAngleY, float fAngleX, float fSpeed, float fJump, float fDucking, float fPushAngle, float fPushForce, float fThrustUpwards )
{
	// get reference object
	sObject* pObject = g_pfnGetObject ( iObjectNumber );
	if ( pObject==NULL ) return;
	if ( m_character==NULL ) return;

	// if physics world present
	if ( g_dynamicsWorld!=NULL )
	{
		D3DXMATRIX matRotX;
		D3DXMatrixRotationX ( &matRotX, D3DXToRadian(fAngleX) );
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY ( &matRotY, D3DXToRadian(fAngleY) );
		D3DXMATRIX matRot;
		D3DXMatrixMultiply ( &matRot, &matRotX, &matRotY );
		D3DXVECTOR3 vecForward = D3DXVECTOR3(0,0,1);
		D3DXVec3TransformCoord ( &vecForward, &vecForward, &matRot );
		if ( fAngleX<0 ) vecForward.y *= -1;
		btVector3 walkDirection = btVector3(vecForward.x,vecForward.y,vecForward.z);
		btScalar walkVelocity = fSpeed;
		btScalar walkSpeed = walkVelocity;

		// control character controller jumping
		if ( m_character->canJump() )
		{
			if ( fJump > 0.0f )
			{
				m_character->setJumpSpeed(fJump/gSc);
				m_character->jump();
			}
		}
						
		// push character if forced in a specific direction
		if ( fPushForce>0.0f )
		{
			// cannot walk when being pushed
			float fPushAngleRad = D3DXToRadian(fPushAngle);
			btVector3 vecPush = btVector3(sin(fPushAngleRad),0.0,cos(fPushAngleRad));
			m_character->setWalkDirection((vecPush*fPushForce)/gSc);
		}
		else
		{
			// control character controller for walk direction
			m_character->setWalkDirection((walkDirection*walkSpeed)/gSc);
		}

		// Apply thrust value to system
		m_character->m_fThrustUpwards = fThrustUpwards;

		// Ducking control
		float fXPos = pObject->position.vecPosition.x;
		float fYPos = pObject->position.vecPosition.y+1;
		float fZPos = pObject->position.vecPosition.z;
		if ( fDucking==1.0f )
		{
			if ( m_character->m_bCurrentlyDucked==false )
			{
				// change to small capsule
				btCollisionShape* oldrigidbodyshape = m_ghostObject->getCollisionShape();
				btScalar characterWidth  = 15.0f/gSc;
				btScalar characterHeight = 22.0f/gSc;
				btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
				m_ghostObject->setCollisionShape ( capsule );
				m_character->SetConvexShape ( capsule );
				delete oldrigidbodyshape;
				m_character->m_bCurrentlyDucked = true;

				// reposition character for ducking (otherwise it jitters)
				m_character->warp (btVector3(fXPos/gSc,fYPos/gSc,fZPos/gSc));
			}
		}
		else
		{
			if ( m_character->m_bCurrentlyDucked==true )
			{
				// do raycast to see if we can unduck 
				bool bHitSomething = false;
				for ( int allcorners=0; allcorners<5; allcorners++ )
				{
					btVector3 Start, End, Normal;
					if ( allcorners==0 ) { Start = btVector3(fXPos,fYPos,fZPos); End = btVector3(fXPos,fYPos+40,fZPos); }
					if ( allcorners==1 ) { Start = btVector3(fXPos-15,fYPos,fZPos-15); End = btVector3(fXPos-15,fYPos+40,fZPos-15); }
					if ( allcorners==2 ) { Start = btVector3(fXPos+15,fYPos,fZPos-15); End = btVector3(fXPos+15,fYPos+40,fZPos-15); }
					if ( allcorners==3 ) { Start = btVector3(fXPos-15,fYPos,fZPos+15); End = btVector3(fXPos-15,fYPos+40,fZPos+15); }
					if ( allcorners==4 ) { Start = btVector3(fXPos+15,fYPos,fZPos+15); End = btVector3(fXPos+15,fYPos+40,fZPos+15); }
					Start.setX(Start.getX() / gSc);
					Start.setY(Start.getY() / gSc);
					Start.setZ(Start.getZ() / gSc);
					End.setX(End.getX() / gSc);
					End.setY(End.getY() / gSc);
					End.setZ(End.getZ() / gSc);
					btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
					g_dynamicsWorld->rayTest(Start, End, RayCallback);
					if(RayCallback.hasHit())
					{
						//End = RayCallback.m_hitPointWorld;
						//Normal = RayCallback.m_hitNormalWorld;
						bHitSomething = true;
					}
				}

				// if hit, don't get up yet!
				if ( bHitSomething==false )
				{
					// change back to regular capsule
					btCollisionShape* oldrigidbodyshape = m_ghostObject->getCollisionShape();
					btScalar characterWidth  = 15.0f/gSc;
					btScalar characterHeight = 40.0f/gSc;
					btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
					m_ghostObject->setCollisionShape ( capsule );
					m_character->SetConvexShape ( capsule );
					delete oldrigidbodyshape;
					m_character->m_bCurrentlyDucked = false;
				}
			}
		}

		// ensure object associated with controller is updated in ODE_UPDATE
		g_CharacterControlObject = iObjectNumber;
	}
}

DWORD ODE_GetCharacterControllerDucking ( int iObjectNumber )
{
	if ( m_character->m_bCurrentlyDucked==true )
		return 1;
	else
		return 0;
}

int ODE_GetCharacterHitFloor ( void )
{
	// returns value passed in for this collider (material type)
	int iHitFloorMaterialValue = m_character->iHitFloor;
	m_character->iHitFloor = 0;
	return iHitFloorMaterialValue;
}

DWORD ODE_GetCharacterFallDistance ( void )
{
	float fReturnValue = m_character->fFallDistance * gSc;
	fReturnValue = fReturnValue / 2.0f; //seems too sensitive, check with debug later!
	m_character->fFallDistance = 0.0f;
	return *(DWORD*)&fReturnValue;
}

void ODE_CreateStaticSphere	( int iObjectNumber )
{
	CreateSphere ( iObjectNumber, 0 );
}
void ODE_CreateStaticBox ( int iObjectNumber, int iLimbNumber )
{
	CreateBox ( iObjectNumber, 0, iLimbNumber, 0, -1, -1, -1, false, 0,0,0,0,0,0,0,0,0 );
}
void ODE_CreateStaticBox ( int iObjectNumber )
{
	CreateBox ( iObjectNumber, 0, -1, 0, -1, -1, -1, false, 0,0,0,0,0,0,0,0,0);	            
}
void ODE_CreateStaticCylinder ( int iObjectNumber )
{
	CreateCylinder ( iObjectNumber, 0 );
}
void ODE_CreateStaticCylinder ( int iObjectNumber, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot )
{
	CreateCylinder ( iObjectNumber, 0, fXPos, fYPos, fZPos, fXSize, fYSize, fZSize, fXRot, fYRot, fZRot );
}
void ODE_CreateStaticTerrainMesh ( int iObjectNumber )
{
	CreateMesh ( iObjectNumber, 0, -1, 1, 100, 0 );
}
void ODE_CreateStaticTriangleMesh ( int iObjectNumber )
{
	CreateMesh ( iObjectNumber, 0, -1, 0, 100, 0 );
}
void ODE_CreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, 100, 0 );
}
void ODE_CreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber, int iCollisionScaling )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, iCollisionScaling, 0 );
}
void ODE_CreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, iCollisionScaling, iHullReductionMode );
}
void ODE_CreateStaticTerrain ( int iObjectNumber, int iWidth, int iLength, DWORD dwMemBlockPtr )
{
	CreateTerrain ( iObjectNumber, 0, iWidth, iLength, dwMemBlockPtr );
}
void ODE_UpdateStaticTerrain ( int iObjectNumber, int iWidth, int iLength, DWORD dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 )
{
	UpdateTerrain ( iObjectNumber, 0, iWidth, iLength, dwMemBlockPtr, iX1, iZ1, iX2, iZ2 );
}
void ODE_CreateStaticCapsule ( int iObjectNumber )
{
	CreateCapsule ( iObjectNumber, 0, 1.0f, 0.0f );
}
void ODE_CreateDynamicSphere ( int iObjectNumber )
{
	CreateSphere ( iObjectNumber, 1 );
}
void ODE_CreateDynamicBox ( int iObjectNumber )
{
	CreateBox ( iObjectNumber, 1, -1, 0, -1, -1, -1, false,0,0,0,0,0,0,0,0,0 );
}
void ODE_CreateDynamicBox ( int iObjectNumber, int iLimbNumber, int iTipOverMode )
{
	CreateBox ( iObjectNumber, 1, iLimbNumber, iTipOverMode, -1, -1, -1 , false, 0,0,0,0,0,0,0,0,0);
}
void ODE_CreateDynamicBox ( int iObjectNumber, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution )
{
	CreateBox ( iObjectNumber, 1, iLimbNumber, iTipOverMode, fWeight, fFriction, fRestitution, false, 0,0,0,0,0,0,0,0,0 );
}
void ODE_CreateDynamicBoxManual( int iObjectNumber, float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize, float fXAng, float fYAng, float fZAng){
	CreateBox(iObjectNumber, 1, -1, 0, -1, -1, -1, true, fX,fY,fZ,fXSize,fYSize,fZSize,fXAng,fYAng,fZAng);
}
void ODE_CreateDynamicCylinder ( int iObjectNumber )
{
	CreateCylinder ( iObjectNumber, 1 );
}
void ODE_CreateDynamicTriangleMesh ( int iObjectNumber )
{
	CreateMesh ( iObjectNumber, 1, -1, 0, 100, 0 );
}
void ODE_CreateDynamicCapsule ( int iObjectNumber, float fScaleModifier, float fRaised )
{
	CreateCapsule ( iObjectNumber, 1, fScaleModifier, fRaised );
}
void ODE_StartStaticObject ( int iObjectNumber )
{
	StartStaticObject ( iObjectNumber );
}
void ODE_EndStaticObject ( int iObjectNumber, int iTerrainMaterial )
{
	EndStaticObject ( iObjectNumber, iTerrainMaterial );
}
void ODE_AddStaticObjectBox ( int iObjectNumber , int iStaticBoxObject )
{
	AddStaticObjectBox ( iObjectNumber , iStaticBoxObject);
}
int ODE_CreateSwingDoorHinge(int iObjectNumber, float fX, float fY, float fZ, float fAng){
	return CreateSwingDoorHinge(iObjectNumber, fX, fY, fZ, fAng);
}
void ODE_DestroyObject ( int iObjectNumber )
{
	// get reference object
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;
	DestroyThisObject ( pPhyObject );
	ODE_RemoveObject ( iObjectNumber );
}

// Does not work at the moment
// The idea is to have a dynamic entity that does not response to gravity or hitting the terrain
// The no gravity works, but without setting collision flags to no response it bounces off terrain
// Not always consistant. No contact response does seem to work, but then nothing collides with the object either (so it gives no response and has no respone either)
void ODE_SetNoGravity( int iObjectNumber, int iGravity )
{
/*
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	if ( iGravity )
	{
		btVector3 acceleration(0,0,0);
		pPhyObject->body->setGravity( acceleration);
		//pPhyObject->body->setLinearVelocity(acceleration);
		//pPhyObject->body->setAngularVelocity(acceleration);
		//pPhyObject->body->setMassProps(0,acceleration);
		pPhyObject->body->setFlags ( BT_DISABLE_WORLD_GRAVITY );
		//COL_OBJECT | COL_CAPSULECHAR | 
		pPhyObject->body->setCollisionFlags( btCollisionObject::CF_NO_CONTACT_RESPONSE) ;
		pPhyObject->body->forceActivationState(1);
	}
	else
	{
		pPhyObject->body->setFlags ( !BT_DISABLE_WORLD_GRAVITY );
	}*/
}

// Ragdoll commands

void DBProReportError ( LPSTR pErr1, LPSTR pErr2 )
{
	MessageBox ( NULL, pErr1, pErr2, MB_OK );
}

void BPhys_RagDollBegin(int objectID, float objTotalWeight)
{
	currentDBProRagDoll = new DBProRagDoll(objectID);
	currentDBProRagDoll->SetTotalWeight(objTotalWeight);	
}

int BPhys_RagDollGetIDFromBoneObject(int objectID)
{
	return ragdollManager->GetIDFromBoneObject(objectID);
}

int BPhys_RagDollAddBone(int startLimbID, int endLimbID, float diameter, int collisionGroup, int collisionMask)
{
	if(currentDBProRagDoll != NULL)
	{
		return currentDBProRagDoll->AddBone(currentDBProRagDoll->GetID(), startLimbID, endLimbID, diameter, collisionGroup, collisionMask);
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
	return -1;
} 

int BPhys_RagDollAddBone2(int startLimbID, int endLimbID, float diameter, float lengthmod, int collisionGroup, int collisionMask)
{
	if(currentDBProRagDoll != NULL)
	{
		return currentDBProRagDoll->AddBone(currentDBProRagDoll->GetID(), startLimbID, endLimbID, diameter, lengthmod, collisionGroup, collisionMask);
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
	return -1;
} 

void BPhys_RagDollBoneAddLimbID(int boneID, int limbID) 
{
	if(currentDBProRagDoll != NULL)
	{
		currentDBProRagDoll->AssignLimbIDToBone( boneID, limbID);
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
}

void BPhys_RagDollAddHingeJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec2)
{
	if(currentDBProRagDoll!=NULL)
	{
		DBProToBullet::AssertValidVector(jointRotationVec3, "Parameter Passed To BPhys_RagDollAddHingeJoint Invalid");
		DBProToBullet::AssertValidVector(limitsVec2, "Parameter Passed To BPhys_RagDollAddHingeJoint Invalid");
		currentDBProRagDoll->AddHingeJoint(boneAID, boneBID, limbID, DBProToBullet::GetVector3(jointRotationVec3), 
																		 btScalar(DBPro::XVector2(limitsVec2)), 
																			btScalar(DBPro::YVector2(limitsVec2)));
	}
	else
	{
		DBProReportError("Can not call BPhys_RagDollAddHingeJoint before BPhys_RagDollBegin", "Bullet Physics Wrapper");
	}
}

void BPhys_RagDollAddTwistJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec3)
{
	if(currentDBProRagDoll!=NULL)
	{
		DBProToBullet::AssertValidVector(jointRotationVec3, "Parameter Passed To BPhys_RagDollAddTwistJoint Invalid");
		DBProToBullet::AssertValidVector(limitsVec3, "Parameter Passed To BPhys_RagDollAddTwistJoint Invalid");
		currentDBProRagDoll->AddTwistConstraint(boneAID, boneBID, limbID, DBProToBullet::GetVector3(jointRotationVec3),
																															DBProToBullet::GetVector3(limitsVec3));
	}
	else
	{
		DBProReportError("Can not call BPhys_RagDollAddTwistJoint before BPhys_RagDollBegin", "Bullet Physics Wrapper");
	}
}

int BPhys_RagdollGetBoneObjID(int ragdollID, int boneIndex)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	return ragdoll ? ragdoll->GetBoneObjID(boneIndex) : -1;
}

void BPhys_RagDollEnd()
{
	ragdollManager->AddRagdoll(currentDBProRagDoll);
	currentDBProRagDoll->Finalize();
	currentDBProRagDoll->Activate();
	currentDBProRagDoll = NULL;
}

void BPhys_RagDollApplyForce(int ragdollID,int iLimbNumber,float fFX,float fFY,float fFZ,float fTX,float fTY,float fTZ,float fForce)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	if ( ragdoll )
	{
		ragdoll->Activate();
		ragdoll->ApplyForce(iLimbNumber,btVector3(fFX,fFY,fFZ),btVector3(fTX,fTY,fTZ),fForce);
	}
}

int BPhys_RagdollExist(int ragdollID)
{
	return ragdollManager->GetRagdoll(ragdollID) != NULL;
}

int BPhys_DeleteRagdoll(int ragdollID)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	char data[2048];
	sprintf(data, "BPhys_DeleteRagdoll: Ragdoll %d does not exist", ragdollID);
	DBProRagdollManager::AssertRagdollExist(ragdollID, data);
	if(ragdoll)
	{
		ragdollManager->DeleteRagdoll(ragdollID);
	}
	else
	{
		return 0;
	}
	return 1;
}

int BPhys_RagdollIsStatic(int ragdollID)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	if(ragdoll)
	{
		return ragdoll->IsStatic();
	}
}

int BPhys_RagdollSetStatic(int ragdollID, int isStatic)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	if(ragdoll)
	{
		if ( isStatic==0 )
			ragdoll->SetStatic(false);
		else
			ragdoll->SetStatic(true);
	}
	else
	{
		return 0;
	}
	return 1;
}

int BPhys_RagDollHideBones(int ragdollID)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	if(ragdoll)
	{
		ragdoll->HideBones();
	}
	else
	{
		return 0;
	}
	return 1;
}

int BPhys_RagDollShowBones(int ragdollID)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	if(ragdoll)
	{
		ragdoll->ShowBones();
	}
	else
	{
		return 0;
	}
	return 1;
}

//FHB:Can only call these function before Ragdoll END.
void BPhys_RagDollSetDamping(float linear, float angular)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetDamping(btScalar(linear), btScalar(angular));
	}
	else
	{
		//TODO: Determine if an error message needs to be sent to DBPro
	}
}

void BPhys_RagDollSetSleepingThresholds(float linear, float angular)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetSleepingThresholds(btScalar(linear), btScalar(angular));
	}
	else
	{
		//TODO: Determine if an error message needs to be sent to DBPro
	}
}

void BPhys_RagDollSetDeactivationTime(float time)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetDeactivationTime(btScalar(time));
	}
	else
	{
		//TODO: Determine if an error message needs to be sent to DBPro
	}
}

// Force commands

void ODE_SetLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	// get reference object
	sObjectList* pPhyObject = ODE_FindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply linear velocity
	pPhyObject->body->activate();
	pPhyObject->body->setLinearVelocity(btVector3(fX/gSc,fY/gSc,fZ/gSc));
}

void ODE_AddBodyForce ( int iObject, float fX, float fY, float fZ, float fPX, float fPY, float fPZ )
{
	// get reference object
	sObjectList* pPhyObject = ODE_FindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply force
	pPhyObject->body->activate();
	pPhyObject->body->applyForce(btVector3(fX/gSc,fY/gSc,fZ/gSc), btVector3(fPX/gSc,fPY/gSc,fPZ/gSc));
}

void ODE_SetAngularVelocity ( int iObject, float fX, float fY, float fZ )
{
	// get reference object
	sObjectList* pPhyObject = ODE_FindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply linear velocity
	pPhyObject->body->activate();
	pPhyObject->body->setAngularVelocity(btVector3(fX/gSc,fY/gSc,fZ/gSc));
}

int ODE_RayTerrainEx ( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType )
{
	// Cast a ray against the terrain object and see if hit
	btVector3 Start, End, Normal;
    Start = btVector3(fX/gSc,fY/gSc,fZ/gSc);
	End = btVector3(fToX/gSc,fToY/gSc,fToZ/gSc);
	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	RayCallback.m_collisionFilterGroup = iCollisionType;
	RayCallback.m_collisionFilterMask = iCollisionType;
	g_dynamicsWorld->rayTest(Start, End, RayCallback);
	if(RayCallback.hasHit())
	{
		End = RayCallback.m_hitPointWorld;
		Normal = RayCallback.m_hitNormalWorld;
		g_hitObjectNumber = 0;
		g_hitPointWorld = End * gSc;
		g_hitNormalWorld = Normal;
		return 1;
	}
	else
		return 0;
}

int ODE_RayTerrain ( float fX, float fY, float fZ, float fToX, float fToY, float fToZ )
{
	return ODE_RayTerrainEx ( fX, fY, fZ, fToX, fToY, fToZ, COL_TERRAIN );
}

int ODE_RayForce ( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, float fForceValue )
{
	// Cast a ray, and apply force to what it hits first
	btVector3 Start, End, Normal;
    Start = btVector3(fX/gSc,fY/gSc,fZ/gSc);
	End = btVector3(fToX/gSc,fToY/gSc,fToZ/gSc);
	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	g_dynamicsWorld->rayTest(Start, End, RayCallback);
	if(RayCallback.hasHit())
	{
		End = RayCallback.m_hitPointWorld;
		Normal = RayCallback.m_hitNormalWorld;
		g_hitObjectNumber = 0;
		g_hitPointWorld = End * gSc;
		g_hitNormalWorld = Normal;
		if ( RayCallback.m_collisionObject->getInternalType()==btCollisionObject::CO_RIGID_BODY )
		{
			// apply force to rigid body
			RayCallback.m_collisionObject->activate();
			btRigidBody* pBody = (btRigidBody*)RayCallback.m_collisionObject;
			btVector3 thisPos = pBody->getWorldTransform().getOrigin();
			btVector3 relPos = End - thisPos;
			btVector3 force = (End-Start);
			force = force / force.length() / gSc;
			force = force * (fForceValue/gSc);
			pBody->applyForce(force, relPos);

			// find which object we hit
			sObject* pFound = NULL;
			for (int j=g_ObjectList.size()-1; j>=0 ;j--)
			{
				btRigidBody* findbody = g_ObjectList[j].body;
				if ( findbody==pBody )
				{
					//pFound = g_ObjectList[j].pObject;
					pFound = g_pfnGetObject ( g_ObjectList[j].iID );
					break;
				}
			}
			if ( pFound )
			{
				g_hitObjectNumber = pFound->dwObjectNumber;
			}
		}
		return 1;
	}
	else
		return 0;
}

DWORD ODE_GetRayCollisionX()
{
	float value = g_hitPointWorld.getX();
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionY ( void )
{
	float value = g_hitPointWorld.getY();
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionZ ( void )
{
	float value = g_hitPointWorld.getZ();
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalX	( void )
{
	float value = g_hitNormalWorld.getX();
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalY	( void )
{
	float value = g_hitNormalWorld.getY();
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalZ	( void )
{
	float value = g_hitNormalWorld.getZ();
	return *( (DWORD*) &value );
}

int ODE_GetRayObjectHit ( void )
{
	return g_hitObjectNumber;
}

//
// Set commands
//

void ODE_SetActive ( int iObjectNumber, int iMode )
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	/* How do you temporarily disable a body from the simulation?
	if ( iMode==0 )
		pPhyObject->body->forceActivationState(pPhyObject->body->
	else
		pPhyObject->body->forceActivationState(
	*/
}

void ODE_SetBodyPosition ( int iObjectNumber, float fX, float fY, float fZ )
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	// change position of physics object
	if ( pPhyObject->body )
	{
		btTransform worldTrans;
		worldTrans.setOrigin(btVector3(fX/gSc,fY/gSc,fZ/gSc));
		pPhyObject->body->setWorldTransform(worldTrans);
	}
	else
	{
		// or character if no body (need further qualification here)
		if ( m_character )
		{
			m_character->warp(btVector3(fX/gSc,fY/gSc,fZ/gSc));
		}
	}
}

void ODE_SetBodyMass ( int iObject, float fPercentage )
{
	sObjectList* pPhyObject = ODE_FindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body )
	{
		fPercentage=fPercentage/100.0f;
		float fOld = pPhyObject->body->getInvMass() * fPercentage;
		btScalar mass(fOld);
		btVector3 localInertia(0,0,0);
		//pPhyObject->body->setMassProps(mass,localInertia);
	}
}

void ODE_SetBodyFriction ( int iObject, float fFriction )
{
	btScalar friction(fFriction);
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(iObject);
	if ( ragdoll )
	{
		// ragdoll object
		ragdoll->SetFriction ( friction );
	}
	else
	{
		// regular object
		sObjectList* pPhyObject = ODE_FindID ( iObject );
		if ( pPhyObject==NULL ) return;
		if ( pPhyObject->body )
		{
			fFriction = fFriction / 100.0f;
			//pPhyObject->body->setFriction(friction);
			//pPhyObject->body->setAnisotropicFriction ( btVector3(fFriction,fFriction,fFriction), 1 );
		}
	}
}

//
// Get commands
//
DWORD ODE_GetBodyLinearVelocityX ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getX() * gSc;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyLinearVelocityY ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getY() * gSc;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyLinearVelocityZ ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODE_FindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getZ() * gSc;
	return *(DWORD*)&fValue;
}


// world functions

void ODE_SetWorldGravity ( float fX, float fY, float fZ )
{
	// set player gravity (assuming 20 is default)
	float fRelative = fabs(fY/20.0f);
	m_character->setGravity((fCharacterGravity*fRelative)/gSc);
	m_character->setFallSpeed((fCharacterFallSpeed*fRelative)/gSc);
}


/* ODE OLD CODE

dWorldID						g_ODEWorld;
dSpaceID						g_ODESpace;
dJointGroupID					g_ODEContactGroup;
std::vector < sODEObject >		g_ODEObjectList;
std::vector < sODEObject >		g_ODEStaticObjectList;
std::vector < sODEBody >		g_ODEBodyList;
float							g_fODEStep;
int								g_iODEMode;
int								g_iODERagdollMode = 1;
sODECar							g_ODECarList [ 64 ];
std::stack < sODECollision >	g_ODECollision;
sODECollision					g_ODECollisionMessage;
int								g_iCountDynamicEntityJoints = 0;
int								g_iCountDynamicEntityJointsMax = STACKOVERFLOWCAP; // any higher and too many joints are created - crash!

__int64 i64TimeFreq;
__int64 i64CurrentTime;
__int64 i64LastTime;
float fTimeDelta;

dVector3 vCollidePos, vCollideNormal;
float fCollideDepth = 0;;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

int	ODE_CollisionMessageExists ( void )
{
	memset ( &g_ODECollisionMessage, 0, sizeof ( g_ODECollisionMessage ) );
	if ( g_ODECollision.size ( ) )
		return 1;

	return 0;
}

void ODE_CollisionGetMessage ( void )
{
	g_iCountDynamicEntityJoints=0;
	g_ODECollisionMessage = g_ODECollision.top ( );
	g_ODECollision.pop ( );
}

int	ODE_GetObjectA ( void )
{
	return g_ODECollisionMessage.iObjectA;
}

int	ODE_GetObjectB ( void )
{
	return g_ODECollisionMessage.iObjectB;
}

DWORD ODE_GetObjectAContact ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAContact;
}

DWORD ODE_GetObjectAVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityX;
}

DWORD ODE_GetObjectAVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityY;
}

DWORD ODE_GetObjectAVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityZ;
}

DWORD ODE_GetObjectAAngularVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityX;
}

DWORD ODE_GetObjectAAngularVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityY;
}

DWORD ODE_GetObjectAAngularVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityZ;
}

DWORD ODE_GetObjectBContact ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBContact;
}

DWORD ODE_GetObjectBVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityX;
}

DWORD ODE_GetObjectBVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityY;
}

DWORD ODE_GetObjectBVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityZ;
}

DWORD ODE_GetObjectBAngularVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityX;
}

DWORD ODE_GetObjectBAngularVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityY;
}

DWORD ODE_GetObjectBAngularVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityZ;
}

void ODE_CreateStaticUniverse ( void )
{
	// create a static triangle mesh from the universe

	// local variable declarations
	cUniverse*			pUniverse = NULL;			// universe pointer
	vector < sMesh* >	pMeshList;					// mesh list
	DWORD				dwVertexCount	= 0;		// total number of vertices
	DWORD				dwIndexCount	= 0;		// total number of indices
	int					iMesh			= 0;		// temporary mesh counter
	float*				vertices		= NULL;		// vertex list
	int					vertexCount		= 0;		// vertex count
	int*				triangles		= NULL;		// triangle list
	int					triangleCount	= 0;		// triangle count
	int					iPos			= 0;
	dTriMeshDataID		triangleData	= 0;
	dBodyID				body;
	dGeomID				geom;
	int					iTriPos			= 0;
	int					iTriOffset		= 0;
	dMass				mass;
	sOffsetMap			offsetMap;

	// check the get universe function pointer is valid
	if ( !g_pfnGetUniverseMeshList )
	{
		return;
	}
	
	// make sure the list is cleared first
	pMeshList.clear ( );

	// attempt to get the master mesh list,
	g_pfnGetUniverseMeshList ( &pMeshList );

	// check to see if we have some meshes
	if ( pMeshList.size ( ) < 1 )
		return;

	// find the total number of vertices and indices
	for ( iMesh = 0; iMesh < (int)pMeshList.size ( ); iMesh++ )
	{
		dwVertexCount += pMeshList [ iMesh ]->dwVertexCount;
		dwIndexCount  += pMeshList [ iMesh ]->dwIndexCount;
	}

	// allocate arrays for vertices and indices
	vertices		= new float [ dwVertexCount * 3 ];
	vertexCount		= dwVertexCount;
	triangles		= new int [ dwIndexCount ];
	triangleCount	= dwIndexCount;
	iPos			= 0;
	triangleData	= 0;
	
	// check pointers are valid
	if ( !vertices || !triangles )
	{
		// memory allocation failed
		return;
	}

	// check the offset map function as a precaution
	if ( !g_pfnGetFVFOffsetMap )
	{
		// invalid function pointer
		return;
	}
	
	// go through each mesh in the list
	for ( iMesh = 0; iMesh < (int)pMeshList.size ( ); iMesh++ )
	{
		// get the current mesh
		sMesh* pMesh = pMeshList [ iMesh ];

		// check the mesh is valid
		if ( !pMesh )
			continue;
		
		// get the offset map
		g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
		
		// copy each vertex position across to the new vertex array
		for ( int i = 0; i < (int)pMesh->dwVertexCount; i++ )
		{
			BYTE* pVertex = pMesh->pVertexData;

			D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
														*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
												);

			vertices [ iPos++ ] = vecPosition.x;
			vertices [ iPos++ ] = vecPosition.y;
			vertices [ iPos++ ] = vecPosition.z;
		}

		//paul missing "int i" declaration
		// copy each index value across to the new index array
		for ( int i = 0; i < (int)pMesh->dwIndexCount; i++ )
			triangles [ iTriPos++ ] = pMesh->pIndices [ i ] + iTriOffset;

		// increment the triangle offset
		iTriOffset += pMesh->dwIndexCount;
	}

	// now create a rigid body
	body = dBodyCreate ( g_ODEWorld );

	// create the triangle mesh data
	triangleData = dGeomTriMeshDataCreate ( );

	// pass in the data
	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									dwVertexCount,
									( int* ) &triangles [ 0 ],
									dwIndexCount,
									3 * sizeof ( int )
								);
	
	// finally create the big triangle mesh
	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );
}

void ODE_Start ( void )
{
	#ifndef DARKSDK_COMPILE
#ifdef X10
		g_pfnGetObject				= ( pfnGetObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetObjectW@@YAPAUsObject@@H@Z" );
#else
		g_pfnGetObject				= ( pfnGetObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
#endif
		g_pfnGetFVFOffsetMap		= ( pfnGetFVFOffsetMap			) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetFVFOffsetMap@@YA_NPAUsMesh@@PAUsOffsetMap@@@Z" );
		g_pfnCalculateMeshBounds	= ( pfnCalculateMeshBounds		) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalculateMeshBounds@@YA_NPAUsMesh@@@Z" );
		g_pfnCalcObjectWorld		= ( pfnCalcObjectWorld			) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalcObjectWorld@@YA_NPAUsObject@@@Z" );
		g_pfnMakeMeshFromOtherMesh	= ( pfnMakeMeshFromOtherMesh	) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeMeshFromOtherMesh@@YA_N_NPAUsMesh@@1PAUD3DXMATRIX@@@Z" );
		g_pfnPositionObject			= ( pfnPositionObject			) GetProcAddress ( g_pGlobal->g_Basic3D, "?Position@@YAXHMMM@Z" );
		g_pfnGetUniverseMeshList	= ( pfnGetUniverseMeshList		) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetUniverseMeshList@@YAXPAV?$vector@PAUsMesh@@V?$allocator@PAUsMesh@@@std@@@std@@@Z" );
		g_pfnMakeObjectBox			= ( pfnMakeObjectBox			) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeBox@@YAXHMMM@Z" );
		g_pfnDeleteObject			= ( pfnDeleteObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?DeleteEx@@YAXH@Z" );
	#else
		g_pfnGetObject				= GetObject;
		g_pfnGetFVFOffsetMap		= GetFVFOffsetMap;
		g_pfnCalculateMeshBounds	= CalculateMeshBounds;
		g_pfnCalcObjectWorld		= CalcObjectWorld;
		g_pfnMakeMeshFromOtherMesh	= MakeMeshFromOtherMesh;
		g_pfnPositionObject			= dbPositionObject;
		g_pfnRotateObject			= dbRotateObject;
		g_pfnGetUniverseMeshList	= GetUniverseMeshList;
	#endif

	g_ODEWorld = dWorldCreate ( );

	// type of space to detect collision within
	if ( 1 )
	{
		// I think some optimisation potential here (a big flaky once in my proto-staticobj miss)
		// ie such as the circle windows (WHEN STATIC GEOM) of size 60,60,5 which could not be detected!
		g_ODESpace = dHashSpaceCreate  ( 0 );
	}
	else
	{
		// Absolutely no collision culling, everything Vs everything approach (not for final release)
		g_ODESpace = dSimpleSpaceCreate  ( 0 );
	}

	g_ODEContactGroup = dJointGroupCreate ( 0 );
	g_iODEMode		  = 0;
	g_fODEStep		  = 0.05f;

	// world defaults
	dWorldSetGravity ( g_ODEWorld, 0, -9.81f, 0 );
	ODE_SetAutoDisableFlag ( 1 );
// 140307 - ragdolls jitter and do not stop with this;	ODE_SetAutoDisableLinearThreshold ( 0.2f );
//	ODE_SetAutoDisableAngularThreshold ( 0.2f );
// 240407 - big alien jittering
	ODE_SetAutoDisableLinearThreshold ( 3.0f );
	ODE_SetAutoDisableAngularThreshold ( 3.0f );
	ODE_SetAutoDisableSteps( 20 );

	QueryPerformanceFrequency ( (LARGE_INTEGER*) &i64TimeFreq );
    QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
    i64LastTime = i64CurrentTime;
}

void ODE_End ( void )
{
	// X10 - 051008 - delete any ragdolls still in the scene
	sODERagdoll* pRagdoll = sODERagdoll::pODERagdollList;
	while ( pRagdoll )
	{
		sODERagdoll* pNextOne = pRagdoll->pNextRagdoll;
		sODERagdoll::RemoveRagdoll( pRagdoll );
		pRagdoll = pNextOne;
	}
	sODERagdoll::pODERagdollList = NULL;

	// X10 - 051008 - delete any objects still in the scene
	for ( int iObjectID=1; iObjectID<65535; iObjectID++ )
		ODE_DestroyObject ( iObjectID );
	g_ODEObjectList.clear();

	// close ode
	dCloseODE();
}

void ODE_DestroyObject ( int iObject )
{
	// scan dynamic objects
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].iID == iObject )
		{
			sObject* pObject = g_pfnGetObject ( g_ODEObjectList [ i ].iID );
			if ( pObject ) pObject->position.bCustomWorldMatrix = false;

			if ( g_ODEObjectList [ i ].body )
				dBodyDestroy ( g_ODEObjectList [ i ].body );

			if ( g_ODEObjectList [ i ].geom )
				dGeomDestroy ( g_ODEObjectList [ i ].geom );

			if ( g_ODEObjectList [ i ].geom2 )
				dGeomDestroy ( g_ODEObjectList [ i ].geom2 );

			//paull - erase() needs an index
			g_ODEObjectList.erase ( g_ODEObjectList.begin() + i );
		}
	}

	// scan static objects
	for ( int i = 0; i < (int)g_ODEStaticObjectList.size ( ); i++ )
	{
		if ( g_ODEStaticObjectList [ i ].iID == iObject )
		{
			if ( g_ODEStaticObjectList [ i ].geom )
				dGeomDestroy ( g_ODEStaticObjectList [ i ].geom );

			if ( g_ODEStaticObjectList [ i ].geom2 )
				dGeomDestroy ( g_ODEStaticObjectList [ i ].geom2 );

			//paul - erase() needs an index
			g_ODEStaticObjectList.erase ( g_ODEStaticObjectList.begin() + i );
		}
	}
}

void MakeRotationMatrix ( const dReal* fRotate, D3DXMATRIX* pMatrix )
{
	pMatrix->_11 = fRotate [  0 ]; 
	pMatrix->_12 = fRotate [  4 ]; 
	pMatrix->_13 = fRotate [  8 ]; 
	pMatrix->_14 = 0; 

	pMatrix->_21 = fRotate [  1 ]; 
	pMatrix->_22 = fRotate [  5 ]; 
	pMatrix->_23 = fRotate [  9 ]; 
	pMatrix->_24 = 0; 

	pMatrix->_31 = fRotate [  2 ]; 
	pMatrix->_32 = fRotate [  6 ]; 
	pMatrix->_33 = fRotate [ 10 ]; 
	pMatrix->_34 = 0;
	
	pMatrix->_41 = 0;
	pMatrix->_42 = 0;
	pMatrix->_43 = 0;
	pMatrix->_44 = 1;
}

void DBPAnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
	float m00 = pmatMatrix->_11;
	float m01 = pmatMatrix->_12;
	float m02 = pmatMatrix->_13;
	float m12 = pmatMatrix->_23;
	float m22 = pmatMatrix->_33;
	float heading = (float)atan2(m01,m00);
	float attitude = (float)atan2(m12,m22);
	float bank = (float)asin(-m02);

	// check for gimbal lock
	if ( fabs ( m02 ) > 1.0f )
	{
		// looking straight up or down
		float PI = D3DX_PI / 2.0f;
		pVecAngles->x = 0.0f;
		pVecAngles->y = D3DXToDegree ( PI * m02 );
		pVecAngles->z = 0.0f;
	}
	else
	{
		pVecAngles->x = D3DXToDegree ( attitude );
		pVecAngles->y = D3DXToDegree ( bank );
		pVecAngles->z = D3DXToDegree ( heading );
	}
}

int ODE_RayCast( int iObject, float sx, float sy, float sz, float ex, float ey, float ez )
{
	float diffx = ex - sx;
	float diffy = ey - sy;
	float diffz = ez - sz;
	float length = sqrt( diffx*diffx + diffy*diffy + diffz*diffz );

	dGeomID ray = dCreateRay( g_ODESpace, length );
	dGeomRaySet( ray, sx,sy,sz, diffx,diffy,diffz );

	fCollideDepth = -1;

	try
	{
		if ( iObject > 0 )
		{
			dContact contact;//dContactGeom
			sODEObject *pObj= ODE_FindID( iObject );

			if ( !pObj ) throw 0;

			if ( dCollide( ray, pObj->geom, 1, &contact.geom, sizeof(contact) ) )
			{
				vCollidePos [ 0 ] = contact.geom.pos [ 0 ];
				vCollidePos [ 1 ] = contact.geom.pos [ 1 ];
				vCollidePos [ 2 ] = contact.geom.pos [ 2 ];

				vCollideNormal [ 0 ] = contact.geom.normal [ 0 ];
				vCollideNormal [ 1 ] = contact.geom.normal [ 1 ];
				vCollideNormal [ 2 ] = contact.geom.normal [ 2 ];
				
				fCollideDepth = contact.geom.depth;
				throw 1;
			}
		}
		else
		{
			dContact contact [ 10 ];//dContactGeom
			int objs = dSpaceGetNumGeoms( g_ODESpace );

			for (int i = 0; i < objs; i++ ) 
			{
				dGeomID geom = dSpaceGetGeom( g_ODESpace, i );
				int numContacts = dCollide( ray, geom, 10, &contact [ 0 ].geom, sizeof(dContact) );//dContactGeom
				for ( int c = 0; c < numContacts && c < 10; c++ )
				{
					if ( fCollideDepth < 0 || contact [ c ].geom.depth < fCollideDepth )
					{
						vCollidePos [ 0 ] = contact [ c ].geom.pos [ 0 ];
						vCollidePos [ 1 ] = contact [ c ].geom.pos [ 1 ];
						vCollidePos [ 2 ] = contact [ c ].geom.pos [ 2 ];

						vCollideNormal [ 0 ] = contact [ c ].geom.normal [ 0 ];
						vCollideNormal [ 1 ] = contact [ c ].geom.normal [ 1 ];
						vCollideNormal [ 2 ] = contact [ c ].geom.normal [ 2 ];

						fCollideDepth = contact [ c ].geom.depth;
					}
				}
			}
		}
	}
	catch( int num )
	{
		dSpaceRemove( g_ODESpace, ray );
		dGeomDestroy( ray ); 
		return num;
	}

	dSpaceRemove( g_ODESpace, ray );
	dGeomDestroy( ray ); 

	return fCollideDepth >= 0;
}

DWORD ODE_GetRayCollisionX( )
{
	float value = vCollidePos [ 0 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionY( )
{
	float value = vCollidePos [ 1 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionZ( )
{
	float value = vCollidePos [ 2 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalX( )
{
	float value = vCollideNormal [ 0 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalY( )
{
	float value = vCollideNormal [ 1 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalZ( )
{
	float value = vCollideNormal [ 2 ];
	return *( (DWORD*) &value );
}

void Update ( float fStep )
{
	// 150607 - ragdolls using response mode are disabled each update
	sODERagdoll::DisableSilentRagdollsEachUpdate();

	//raycast check for small objects
	for ( int i = 0; i < (int)g_ODEObjectList.size(); i++ )
	{
		if ( g_ODEObjectList [ i ].bRayCast )
		{
			float fRadius = 1.0;
			if ( dGeomGetClass( g_ODEObjectList [ i ].geom ) == dSphereClass ) fRadius = dGeomSphereGetRadius( g_ODEObjectList [ i ].geom );
			if ( dGeomGetClass( g_ODEObjectList [ i ].geom ) == dBoxClass ) 
			{
				dVector3 vSize;
				dGeomBoxGetLengths( g_ODEObjectList [ i ].geom, vSize );
				fRadius = (vSize[0] + vSize[1] + vSize[2]) / 3.0f;
			}

			const dReal *pos = dGeomGetPosition( g_ODEObjectList [ i ].geom );
			const dReal *vel = dBodyGetLinearVel( g_ODEObjectList [ i ].body );
			dVector3 vGrav;
			dWorldGetGravity( g_ODEWorld, vGrav );
			float fNewVelX = (vel[0] + vGrav[0]*fStep)*0.99f;	//added gravity manually and damp speed
			float fNewVelY = (vel[1] + vGrav[1]*fStep)*0.99f;
			float fNewVelZ = (vel[2] + vGrav[2]*fStep)*0.99f;
			
			float fLength = sqrt( fNewVelX*fNewVelX + fNewVelY*fNewVelY + fNewVelZ*fNewVelZ );
			if ( fLength*fStep < 0.001f ) continue;

			dGeomID ray = dCreateRay( g_ODESpace, fLength*fStep );
			dGeomRaySet( ray, pos[0],pos[1],pos[2], fNewVelX/fLength,fNewVelY/fLength,fNewVelZ/fLength );

			fCollideDepth = -1;

			dContact contact [ 10 ];//dContactGeom
			int iNumGeoms = dSpaceGetNumGeoms( g_ODESpace );

			//check against all geoms in the space
			for (int obj = 0; obj < iNumGeoms; obj++ ) 
			{
				dGeomID geom = dSpaceGetGeom( g_ODESpace, obj );
				if ( geom == g_ODEObjectList [ i ].geom || geom == ray ) continue;

				//check ray against geom, record at most 10 contacts - unordered
				int numContacts = dCollide( ray, geom, 10, &contact[0].geom, sizeof(dContact) );//dContactGeom

				//find the closest contact and store it
				for ( int c = 0; c < numContacts && c < 10; c++ )
				{
					if ( fCollideDepth < 0 || (contact [ c ].geom.depth < fCollideDepth && contact [ c ].geom.depth > 0) )
					{
						vCollidePos [ 0 ] = contact [ c ].geom.pos [ 0 ];
						vCollidePos [ 1 ] = contact [ c ].geom.pos [ 1 ];
						vCollidePos [ 2 ] = contact [ c ].geom.pos [ 2 ];

						vCollideNormal [ 0 ] = contact [ c ].geom.normal [ 0 ];
						vCollideNormal [ 1 ] = contact [ c ].geom.normal [ 1 ];
						vCollideNormal [ 2 ] = contact [ c ].geom.normal [ 2 ];

						fCollideDepth = contact [ c ].geom.depth;
					}
				}
			}

			dSpaceRemove( g_ODESpace, ray );
			dGeomDestroy( ray );

			//object collided on route, place at closest collision
			//(small chance of error here due to only 10 contacts being checked)
			if ( fCollideDepth > 0 )
			{
				//add a collision message for 
				sODECollision collision = { 0 };
				collision.iObjectA = g_ODEObjectList [ i ].iID; // 201007 - paul, corrected from just 'i'
				const dReal* pA = dBodyGetLinearVel ( g_ODEObjectList [ i ].body );
				collision.fObjectAVelocityX = pA [ 0 ];
				collision.fObjectAVelocityY = pA [ 1 ];
				collision.fObjectAVelocityZ = pA [ 2 ];
				g_ODECollision.push ( collision );
						
				//dGeomSetPosition( g_ODEObjectList [ i ].geom, vCollidePos [ 0 ], vCollidePos [ 1 ], vCollidePos [ 2 ] );
				float fNewX = pos[0] + (fNewVelX/fLength)*(fCollideDepth-fRadius*0.9);
				float fNewY = pos[1] + (fNewVelY/fLength)*(fCollideDepth-fRadius*0.9);
				float fNewZ = pos[2] + (fNewVelZ/fLength)*(fCollideDepth-fRadius*0.9);
				dGeomSetPosition( g_ODEObjectList [ i ].geom, fNewX, fNewY, fNewZ );
				//dBodyDisable( g_ODEObjectList [ i ].body );

				float fNormLength = sqrt(vCollideNormal[0]*vCollideNormal[0] + vCollideNormal[1]*vCollideNormal[1] + vCollideNormal[2]*vCollideNormal[2]);
				vCollideNormal[0] = vCollideNormal[0]/fNormLength;
				vCollideNormal[1] = vCollideNormal[1]/fNormLength;
				vCollideNormal[2] = vCollideNormal[2]/fNormLength;

				float fBounce = 1.6f;	//This sets the bounciness of the object, 2.0 = perfect bounce (loses no height), 1.0 = no bounce

				float fDot = fNewVelX*vCollideNormal[0] + fNewVelY*vCollideNormal[1] + fNewVelZ*vCollideNormal[2];
				fNewVelX = fNewVelX - vCollideNormal[0]*fDot*fBounce;
				fNewVelY = fNewVelY - vCollideNormal[1]*fDot*fBounce;
				fNewVelZ = fNewVelZ - vCollideNormal[2]*fDot*fBounce;
				dBodySetLinearVel( g_ODEObjectList [ i ].body, fNewVelX, fNewVelY, fNewVelZ );
			}
			else
			{
				float fNewX = pos[0] + fNewVelX*fStep;
				float fNewY = pos[1] + fNewVelY*fStep;
				float fNewZ = pos[2] + fNewVelZ*fStep;
				dGeomSetPosition( g_ODEObjectList [ i ].geom, fNewX, fNewY, fNewZ );

				dBodySetLinearVel( g_ODEObjectList [ i ].body, fNewVelX, fNewVelY, fNewVelZ );
			}

			//completely taking control of the object
			dBodyDisable( g_ODEObjectList [ i ].body );
			dGeomDisable( g_ODEObjectList [ i ].geom );
		}
	}

	dSpaceCollide ( g_ODESpace, 0, &ODE_Callback );
	sODERagdoll::DisableSilentRagdollsEachUpdate();

	if ( g_iODEMode == 0 )
	{
		dWorldQuickStep ( g_ODEWorld, fStep );
	}
	else if ( g_iODEMode == 1 )
	{
		dWorldStep ( g_ODEWorld, fStep );
	}

	dJointGroupEmpty ( g_ODEContactGroup );

	sODERagdoll::UpdateRagdolls( fStep );

	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		// FPSC specific
		if ( dBodyIsEnabled  ( g_ODEObjectList [ i ].body ) )
		{
			// this stops crash, caused by angular velocity exponentially accelerating and objs going numb and failing ALL collision
			const dReal* pAA = dBodyGetAngularVel ( g_ODEObjectList [ i ].body );
			dBodySetAngularVel ( g_ODEObjectList [ i ].body, pAA[0]*0.9f, pAA[1]*0.9f, pAA[2]*0.9f );
			// this ensures objects never go TOO fast to penetrate geom
		}

		// X10 - 300707 - doors tend to move very slowly
		if ( g_ODEObjectList [ i ].iResponseMode==2 )
		{
			// ensures pseudo static are completely none-moving
			dBodySetLinearVel ( g_ODEObjectList [ i ].body, 0, 0, 0 );
		}
		
		D3DXMATRIX		matTranslation,
						matRotation,
						matRotate,
						matWorld;
		const dReal*	fPosition	= dBodyGetPosition ( g_ODEObjectList [ i ].body );
		const dReal*	fRotate		= dBodyGetRotation ( g_ODEObjectList [ i ].body );
		sObject*		pObject		= g_pfnGetObject ( g_ODEObjectList [ i ].iID );

		// make the rotation matrix
		MakeRotationMatrix ( fRotate, &matRotate );	

		// Use physics object position
		g_pfnPositionObject ( g_ODEObjectList [ i ].iID, fPosition [ 0 ], fPosition [ 1 ], fPosition [ 2 ] );
		D3DXMatrixTranslation ( &matTranslation, fPosition [ 0 ], fPosition [ 1 ], fPosition [ 2 ] );

		// Apply pivot if any
		matRotation = matRotate;
		if ( pObject->position.bApplyPivot )
		{
			// modify current rotation
			matRotation = pObject->position.matPivot * matRotation;
		}

		// final world matrix for physics object back to dbpro object
		matWorld    = pObject->position.matScale * matRotation * matTranslation;

		// verify object ptr
		sODEObject* pODEObject = ODE_FindID ( g_ODEObjectList [ i ].iID );
		if ( pODEObject )
		{
			// this info used if the adjustment values are read (or height value)
			// used for adjustment for small objects with boundbox margins and height for shadow calc.
			pODEObject->bNoNeedToRecalculateShape = false;
		}		
		
		// use ODE matrix, not regular DBPro matrix
		pObject->position.bCustomWorldMatrix = true;
		pObject->position.matWorld			 = matWorld;

		// also copy rotation matrix for orient command to use
		pObject->position.matRotation = matRotate;
		DBPAnglesFromMatrix ( &matRotate, &pObject->position.vecRotate );
	}
}

void ODE_Update ( void )
{
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	fTimeDelta = ( i64CurrentTime - i64LastTime ) / ( (float) i64TimeFreq );
	i64LastTime = i64CurrentTime;

	// if time step is too big objects will tunnel through each other.
	// lee - 170407 - objects drop out at 10fps, let alone 1fps - adjust max to Xfps
	float fMaxForStableSimulation = 1.0f / 10.0f;
	// multiplier because we multiply later on
	fMaxForStableSimulation /= 3.0f;
	// if time leap too great, bring it back to min
	if ( fTimeDelta > fMaxForStableSimulation ) fTimeDelta = fMaxForStableSimulation;
	if ( fTimeDelta < 0.0001f ) fTimeDelta=0.0001f;

	//on first update i64LastTime is undefined, step a fixed amount
	static bool bInitialUpdate = true;

	// skip intro setup delay
	if ( bInitialUpdate )
	{
		// startup update
		Update( 0.001f );
		bInitialUpdate = false;
	}
	else
	{
		fTimeDelta *= 3.0f;
		Update( fTimeDelta );
	}	
}

void ODE_Update ( float fManualStep )
{
	// X10 - 221007 - if call a manualy update of 0.001 (pause physics), then make all ragdolls static
	if ( fManualStep==0.001f )
	{
		// this ensures all ragdolls joints are destroyed (prevents the ragdoll jump and corrupt bug when pause physics)
		g_iRagdollCreationCounter += 10;
	}

	// performs  manual update, and also resets the i64LastTime to solve physics mid-run physics jump in time deltas
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	i64LastTime = i64CurrentTime;
	Update( fManualStep );
}

void ODE_SetContact ( int iID, int iMode, float fValue )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iID );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		switch ( iMode )
		{
			case 0: pRagdoll->fContactMU				= fValue; break;
			case 1: pRagdoll->fContactMU2				= fValue; break;
			case 2: pRagdoll->fContactBounce			= fValue; break;
			case 3: pRagdoll->fContactBounceVelocity	= fValue; break;
			case 4: pRagdoll->fContactSoftERP			= fValue; break;
			case 5: pRagdoll->fContactSoftCFM			= fValue; break;
			case 6: pRagdoll->fContactMotion1			= fValue; break;
			case 7: pRagdoll->fContactMotion2			= fValue; break;
			case 8: pRagdoll->fContactSlip1				= fValue; break;
			case 9: pRagdoll->fContactSlip1				= fValue; break;
		}
	}
	if ( pObject )
	{
		switch ( iMode )
		{
			case 0: pObject->fContactMU				= fValue; break;
			case 1: pObject->fContactMU2			= fValue; break;
			case 2: pObject->fContactBounce			= fValue; break;
			case 3: pObject->fContactBounceVelocity = fValue; break;
			case 4: pObject->fContactSoftERP		= fValue; break;
			case 5: pObject->fContactSoftCFM		= fValue; break;
			case 6: pObject->fContactMotion1		= fValue; break;
			case 7: pObject->fContactMotion2		= fValue; break;
			case 8: pObject->fContactSlip1			= fValue; break;
			case 9: pObject->fContactSlip1			= fValue; break;
		}
	}
}

void ODE_SetContactMu2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 1, fValue );
}

void ODE_SetContactFDir1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 0, fValue );
}

void ODE_SetContactBounce ( int iID, float fValue )
{
	ODE_SetContact ( iID, 2, fValue );
}

void ODE_SetContactBounceVelocity ( int iID, float fValue )
{
	ODE_SetContact ( iID, 3, fValue );
}

void ODE_SetContactSoftERP ( int iID, float fValue )
{
	ODE_SetContact ( iID, 4, fValue );
}

void ODE_SetContactSoftCFM ( int iID, float fValue )
{
	ODE_SetContact ( iID, 5, fValue );
}

void ODE_SetContactMotion1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 6, fValue );
}

void ODE_SetContactMotion2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 7, fValue );
}

void ODE_SetContactSlip1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 8, fValue );
}

void ODE_SetContactSlip2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 9, fValue );
}

void ODE_SetSurfaceMode ( int iID, int iMode, int iState )
{
	int			iModeList [ ]	=	{ 
										dContactMu2,
										dContactFDir1,
										dContactBounce,
										dContactSoftERP,
										dContactSoftCFM,
										dContactMotion1,
										dContactMotion2,
										dContactSlip1,
										dContactSlip2,
										dContactApprox0,
										dContactApprox1_1,
										dContactApprox1_2,
										dContactApprox1
									};

	sODEObject* pObject			= ODE_FindID ( iID );

	if ( !pObject )
		return;
	
	if ( iState )
		pObject->iSurfaceMode |= iModeList [ iMode ];
	else
		pObject->iSurfaceMode &= ~iModeList [ iMode ];
}

void ODE_SetSurfaceModeContactMu2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 0, iState );
}

void ODE_SetSurfaceModeContactFDir1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 1, iState );
}

void ODE_SetSurfaceModeContactBounce ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 2, iState );
}

void ODE_SetSurfaceModeContactSoftERP ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 3, iState );
}

void ODE_SetSurfaceModeContactSoftCFM ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 4, iState );
}

void ODE_SetSurfaceModeContactMotion1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 5, iState );
}

void ODE_SetSurfaceModeContactMotion2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 6, iState );
}

void ODE_SetSurfaceModeContactSlip1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 7, iState );
}

void ODE_SetSurfaceModeContactSlip2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 8, iState );
}

void ODE_SetSurfaceModeContactApprox0 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 9, iState );
}

void ODE_SetSurfaceModeContactApprox11 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 10, iState );
}

void ODE_SetSurfaceModeContactApprox12 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 11, iState );
}

void ODE_SetSurfaceModeContactApprox1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 12, iState );
}

static void ODE_Callback ( void* data, dGeomID o1, dGeomID o2 )
{
	// get bodies of this collision
	dBodyID b1 = dGeomGetBody ( o1 );
	dBodyID b2 = dGeomGetBody ( o2 );

	//if bodies are directly connected by a joint then ignore collisions
	//moved downif (b1 && b2 && dAreConnected (b1,b2)) return;

	// reset IDs
	int	iID = 0;//-1 dangerous id to default with 060307
	int	iID2 = 0; //-1

	// paul - add ragdoll ptr check
	sODERagdoll *pRagdoll1 = sODERagdoll::GetRagdoll( b1 );
	sODERagdoll *pRagdoll2 = sODERagdoll::GetRagdoll( b2 );
	if ( pRagdoll1 || pRagdoll2 )
	{
		//if bodies are directly connected by a joint then ignore collisions
		if (b1 && b2 && dAreConnected (b1,b2)) return;

		//if different limbs of the same ragdoll are colliding check if this will cause problems. e.g. spine1<->spine2 collision
		if ( pRagdoll1 == pRagdoll2 )
		{
			//check if bones are in the same group, ignore collision
			if ( pRagdoll1->CheckBoneGroups( b1, b2 ) ) return;
		}

		// get ID of colliding ragdoll
		if ( pRagdoll1 ) iID = pRagdoll1->GetRagdollID();
		if ( pRagdoll2 ) iID2 = pRagdoll2->GetRagdollID();

		// if body is a pseudo-static against a NULL, no collision required at all
		// this means if plr obj hits static, plr walks thourgh ragdoll pseudo static - no no 
		// if ( pRagdoll1 && pRagdoll2==NULL ) if ( pRagdoll1->iResponseMode==2 ) return;
		// if ( pRagdoll2 && pRagdoll1==NULL ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if pseudo static ragdoll body is colliding against no other body, go no further
		if ( pRagdoll1 && !b2 ) if ( pRagdoll1->iResponseMode==2 ) return;
		if ( pRagdoll2 && !b1 ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if body is a pseudo-static against another pseudo-static, no collision required at all
		if ( pRagdoll1 && pRagdoll2 ) if ( pRagdoll1->iResponseMode==2 && pRagdoll2->iResponseMode==2 ) return;
		if ( pRagdoll1 && pRagdoll2 ) if ( pRagdoll1->iResponseMode==2 && pRagdoll2->iResponseMode==2 ) return;

		// X10 - 141107 - if psuedo static used, so guns and plr can pass through ragdoll, ignore collision with non-ragdoll OBJ
		if ( pRagdoll1 && pRagdoll2==NULL && b1 && b2 ) if ( pRagdoll1->iResponseMode==2 ) return;
		if ( pRagdoll2 && pRagdoll1==NULL && b1 && b2 ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if b2 NULL must be static geom - find it's id
		if ( !b2 )
		{
			// this actually gets the ObjectID back, 
			iID2 = ODE_FindStaticBodyID ( o2 );
		}

		// reset collision structure
		sODECollision collision = { 0 };
		collision.iObjectA = iID;
		collision.iObjectB = iID2;
		collision.fObjectAContact = 0;
		collision.fObjectBContact = 0;

		// accuracy of contact points (more the better)
		const int	N = 32;
		dContact	contacts [ N ];
		const int	n = dCollide ( o1, o2, N, &contacts [ 0 ].geom, sizeof ( dContact ) );

		// if obj1 present
		if ( b1 )
		{
			if ( n!=0 ) collision.fObjectAContact = 1.0f;
			const dReal* pA = dBodyGetLinearVel ( b1 );
			collision.fObjectAVelocityX = pA [ 0 ];
			collision.fObjectAVelocityY = pA [ 1 ];
			collision.fObjectAVelocityZ = pA [ 2 ];
			const dReal* pAA = dBodyGetAngularVel ( b1 );
			collision.fObjectAAngularVelocityX = pAA [ 0 ];
			collision.fObjectAAngularVelocityY = pAA [ 1 ];
			collision.fObjectAAngularVelocityZ = pAA [ 2 ];
		}

		// if obj2 present
		if ( b2 )
		{
			if ( n!=0 ) collision.fObjectBContact = 1.0f;
			const dReal* pB = dBodyGetLinearVel ( b2 );
			collision.fObjectBVelocityX = pB [ 0 ];
			collision.fObjectBVelocityY = pB [ 1 ];
			collision.fObjectBVelocityZ = pB [ 2 ];
			const dReal* pBB = dBodyGetAngularVel ( b2 );
			collision.fObjectBAngularVelocityX = pBB [ 0 ];
			collision.fObjectBAngularVelocityY = pBB [ 1 ];
			collision.fObjectBAngularVelocityZ = pBB [ 2 ];
		}

		// add to collision list (for getcollision loop in dbpro - which empties it)
		g_ODECollision.push ( collision );

		// determine if A-B connection, or if A/B is pseudo-static, connect other to static environment
		dBodyID bTreatBodyAsStatic = 0;
		if ( pRagdoll1 != pRagdoll2 )
		{
			// ragdoll is pseudo static, only when a dynamic object is moving at it
			if ( pRagdoll1 )
				if ( pRagdoll1->iResponseMode==2 )
					if ( b2 )
						bTreatBodyAsStatic = b1;

			if ( pRagdoll2 )
				if ( pRagdoll2->iResponseMode==2 )
					if ( b1 )
						bTreatBodyAsStatic = b2;
		}

		// ragdoll could brush against a pseudo-static like a door, make the door SOLID (NULL)
		int iObjID = 0;
		int iObjID2 = 0;
		if ( b1 ) iObjID = ODE_FindBodyID ( b1 );
		if ( b2 ) iObjID2 = ODE_FindBodyID ( b2 );
		sODEObject* pObject = ODE_FindID ( iObjID );
		sODEObject* pObject2 = ODE_FindID ( iObjID2 );
		if ( pObject ) if ( pObject->iResponseMode==2 ) bTreatBodyAsStatic = b1;
		if ( pObject2 ) if ( pObject2->iResponseMode==2 ) bTreatBodyAsStatic = b2;

		// go through all contact points for this collision, add to contactgroup list
		sODERagdoll *pRagdoll = pRagdoll1;
		if ( pRagdoll==NULL ) pRagdoll = pRagdoll2;
		for ( int i = 0; i < n; ++i )
		{
			if ( 0 )
			{
				// pauls fixed - slides about
				contacts [ i ].surface.mode			= 0x00000010;
				contacts [ i ].surface.mu			= 150.0f;
				contacts [ i ].surface.mu2			= 0.0f;
				contacts [ i ].surface.bounce		= 0;
				contacts [ i ].surface.bounce_vel	= 0;
				contacts [ i ].surface.soft_erp		= 0;
				contacts [ i ].surface.soft_cfm		= 0;
				contacts [ i ].surface.motion1		= 0;
				contacts [ i ].surface.motion2		= 0;
				contacts [ i ].surface.slip1		= 0;
				contacts [ i ].surface.slip2		= 0;
			}
			else
			{
				contacts [ i ].surface.mode			= dContactSoftERP | dContactSoftCFM | 0x00000010;
				//contacts [ i ].surface.mu			= pRagdoll->fContactMU;
				//contacts [ i ].surface.soft_erp	= pRagdoll->fContactSoftERP;
				//contacts [ i ].surface.soft_cfm	= pRagdoll->fContactSoftCFM;
				contacts [ i ].surface.mu			= 10000.0f;
				contacts [ i ].surface.soft_erp		= 0.3f;
				contacts [ i ].surface.soft_cfm		= 0.0001f;
				contacts [ i ].surface.mu2			= 0.0f;
				contacts [ i ].surface.bounce		= pRagdoll->fContactBounce;
				contacts [ i ].surface.bounce_vel	= pRagdoll->fContactBounceVelocity;
				contacts [ i ].surface.motion1		= pRagdoll->fContactMotion1;
				contacts [ i ].surface.motion2		= pRagdoll->fContactMotion2;
				contacts [ i ].surface.slip1		= pRagdoll->fContactSlip1;
				contacts [ i ].surface.slip2		= pRagdoll->fContactSlip2;
			}
			dJointID c = dJointCreateContact ( g_ODEWorld, g_ODEContactGroup, &contacts [ i ] );
			dBodyID bodyA = dGeomGetBody ( contacts [ i ].geom.g1 );
			dBodyID bodyB = dGeomGetBody ( contacts [ i ].geom.g2 );
			if ( bodyA==bTreatBodyAsStatic ) bodyA=0;
			if ( bodyB==bTreatBodyAsStatic ) bodyB=0;

			// add joint to physics pool for resolving
			if ( g_iCountDynamicEntityJoints < g_iCountDynamicEntityJointsMax || iID==65534 || iID2==65534 )
			{
				dJointAttach ( c, bodyA, bodyB );
				g_iCountDynamicEntityJoints++;
			}
		}
	}
	else
	{
		// get obj1 and 2 ptr
		sODEObject* pObject = ODE_FindObjPtr ( b1 );
		sODEObject* pObject2 = ODE_FindObjPtr ( b2 );
		if ( !pObject ) return;

		// if objects excluded because they are totally disabled, ignore this contact
		if ( pObject ) if ( pObject->bEnabled==false ) return;
		if ( pObject2 ) if ( pObject2->bEnabled==false ) return;

		// get IDs
		if ( b1 ) iID = ODE_FindBodyID ( b1 );
		if ( b2 ) iID2 = ODE_FindBodyID ( b2 );

		// leelee
		if ( iID==10102 || iID2==10102 )
		{
			int ibarrelcollidingwithsomething=42;
		}

		// if body is a pseudo-static against a NULL, no collision required at all
		if ( pObject && pObject2==NULL ) if ( pObject->iResponseMode==2 ) return;
		if ( pObject2 && pObject==NULL ) if ( pObject->iResponseMode==2 ) return;

		// if body is a pseudo-static against another pseudo-static, no collision required at all
		if ( pObject && pObject2 ) if ( pObject->iResponseMode==2 && pObject2->iResponseMode==2 ) return;
		if ( pObject && pObject2 ) if ( pObject->iResponseMode==2 && pObject2->iResponseMode==2 ) return;

		// if b2 NULL must be static geom - find it's id
		if ( !b2 )
		{
			// this actually gets the ObjectID back, 
			iID2 = ODE_FindStaticBodyID ( o2 );

			// if the response of objA is to ignore static one (the universe mesh), ignore this callback
			if ( pObject->iResponseMode!=0)
			{
				// is it universe mesh (body zero)
				if ( iID2==0 )
					return;
			}
		}

		// reset collision structure
		sODECollision collision = { 0 };
		collision.iObjectA = iID;
		collision.iObjectB = iID2;
		collision.fObjectAContact = 0;
		collision.fObjectBContact = 0;

		// accuracy of contact points (more the better)
		const int	N = 32;
		dContact	contacts [ N ];
		const int	n = dCollide ( o1, o2, N, &contacts [ 0 ].geom, sizeof ( dContact ) );

		// if obj1 present
		if ( b1 )
		{
			if ( n!=0 ) collision.fObjectAContact = 1.0f;
			const dReal* pA = dBodyGetLinearVel ( b1 );
			collision.fObjectAVelocityX = pA [ 0 ];
			collision.fObjectAVelocityY = pA [ 1 ];
			collision.fObjectAVelocityZ = pA [ 2 ];
			const dReal* pAA = dBodyGetAngularVel ( b1 );
			collision.fObjectAAngularVelocityX = pAA [ 0 ];
			collision.fObjectAAngularVelocityY = pAA [ 1 ];
			collision.fObjectAAngularVelocityZ = pAA [ 2 ];
		}

		// if obj2 present
		if ( b2 )
		{
			if ( n!=0 ) collision.fObjectBContact = 1.0f;
			const dReal* pB = dBodyGetLinearVel ( b2 );
			collision.fObjectBVelocityX = pB [ 0 ];
			collision.fObjectBVelocityY = pB [ 1 ];
			collision.fObjectBVelocityZ = pB [ 2 ];
			const dReal* pBB = dBodyGetAngularVel ( b2 );
			collision.fObjectBAngularVelocityX = pBB [ 0 ];
			collision.fObjectBAngularVelocityY = pBB [ 1 ];
			collision.fObjectBAngularVelocityZ = pBB [ 2 ];
		}

		// add to collision list (for getcollision loop in dbpro - which empties it)
		g_ODECollision.push ( collision );

		// determine if A-B connection, or if A/B is pseudo-static, connect other to static environment
		dBodyID bTreatBodyAsStatic = 0;
		if ( pObject  )	if ( pObject->iResponseMode==2 ) bTreatBodyAsStatic = b1;
		if ( pObject2 )	if ( pObject2->iResponseMode==2 ) bTreatBodyAsStatic = b2;

		// go through all contact points for this collision, add to contactgroup list
		for ( int i = 0; i < n; ++i )
		{
			contacts [ i ].surface.mu			= pObject->fContactMU;
			contacts [ i ].surface.mu2			= 0.0f;
			contacts [ i ].surface.bounce		= pObject->fContactBounce;
			contacts [ i ].surface.bounce_vel	= pObject->fContactBounceVelocity;

			// lee - 160307 - resolve this properly at some point (ERP, sheesh!)
			if ( iID==65534 || iID2==65534 )
			{
				// X10 hack - player borble needs to have very quick ERP (correct error quickly to avoid going through floor)
				contacts [ i ].surface.mode			= dContactSoftERP | pObject->iSurfaceMode;

				// U76 - 290710 - when hit surface that will push borble DOWN, it's not a great character controlling move
				// so as we're already hacking 65534, we will add a hack to ensure we cannot be pushed down through a floor
//				contacts [ i ].surface.soft_erp		= 2.0f; // was0.8 but could run into table and fall through floor
				contacts [ i ].surface.soft_erp		= 1.1f;
				if ( contacts [ i ].geom.normal[1] < 0.0f )
				{
					// and to help smooth things out, we even off a slight upward force so we can climb it
					contacts [ i ].geom.normal[1] = 0.001f;
				}
			}
			else
			{
				contacts [ i ].surface.mode			= pObject->iSurfaceMode;
				contacts [ i ].surface.soft_erp		= pObject->fContactSoftERP;
			}
			contacts [ i ].surface.soft_cfm		= pObject->fContactSoftCFM;
			contacts [ i ].surface.motion1		= pObject->fContactMotion1;
			contacts [ i ].surface.motion2		= pObject->fContactMotion2;
			contacts [ i ].surface.slip1		= pObject->fContactSlip1;
			contacts [ i ].surface.slip2		= pObject->fContactSlip2;

			dJointID c = dJointCreateContact ( g_ODEWorld, g_ODEContactGroup, &contacts [ i ] );
			dBodyID bodyA = dGeomGetBody ( contacts [ i ].geom.g1 );
			dBodyID bodyB = dGeomGetBody ( contacts [ i ].geom.g2 );
			if ( bodyA==bTreatBodyAsStatic ) bodyA=0;
			if ( bodyB==bTreatBodyAsStatic ) bodyB=0;

			// add joint to physics pool for resolving
			if ( g_iCountDynamicEntityJoints < g_iCountDynamicEntityJointsMax || iID==65534 || iID2==65534 )
			{
				dJointAttach ( c, bodyA, bodyB );
				g_iCountDynamicEntityJoints++;
			}
		}
	}
}

void ODE_AddObject ( dBodyID body, dGeomID geom, int iID, int iFrame )
{
	sODEObject object = { 0 };

	object.bEnabled = true;
	object.body = body;
	object.geom = geom;
	object.iID  = iID;
	object.iFrame  = iFrame;

	g_ODEObjectList.push_back ( object );
}

void ODE_SetWorldERP ( float fValue )
{
//	dWorldSetERP ( g_ODEWorld, 0.2f );
	dWorldSetERP ( g_ODEWorld, fValue );
}

void ODE_SetWorldCFM ( float fValue )
{
//	dWorldSetCFM ( g_ODEWorld, 0.00001f ); // 130607 - removes ragdoll jiggle
	dWorldSetCFM ( g_ODEWorld, fValue );
}

void ODE_SetWorldStep ( float fStep )
{
	//U75 - 150210 - interferes with FPSC compile process
	//static bool bWarned = false;
	static bool bWarned = true;
	if ( !bWarned ) MessageBox( NULL, "The command 'ODE Set World Step' is no longer supported, use 'ODE Update <step value>' instead", "Warning", 0 );
	bWarned = true;

	g_fODEStep = fStep;
}

void ODE_SetWorldQuickStepNumIterations ( int iValue )
{
	dWorldSetQuickStepNumIterations ( g_ODEWorld, iValue );
}

void ODE_SetStepMode ( int iMode )
{
	g_iODEMode = iMode;
}

void ODE_SetRagdollMode	( int iOneIsOn )
{
	// 201107 - so we can control TEEN rating ragdoll freeze in here
	g_iODERagdollMode = iOneIsOn;
}

float ODE_GetWorldGravityX ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 0 ];
}

float ODE_GetWorldGravityY ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 1 ];
}

float ODE_GetWorldGravityZ ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 2 ];
}

float ODE_GetWorldERP ( void )
{
	return dWorldGetERP ( g_ODEWorld );
}

float ODE_GetWorldCFM ( void )
{
	return dWorldGetCFM ( g_ODEWorld );
}

float ODE_GetWorldStep ( void )
{
	return g_fODEStep;
}

int ODE_GetWorldQuickStepNumIterations ( void )
{
	return dWorldGetQuickStepNumIterations ( g_ODEWorld );
}

void ODE_SetAutoDisableFlag ( int iFlag )
{
	dWorldSetAutoDisableFlag ( g_ODEWorld, iFlag );
}

void ODE_SetAutoDisableLinearThreshold ( float fThreshold )
{
	dWorldSetAutoDisableLinearThreshold ( g_ODEWorld, fThreshold );
}

void ODE_SetAutoDisableAngularThreshold ( float fThreshold )
{
	dWorldSetAutoDisableAngularThreshold ( g_ODEWorld, fThreshold );
}

void ODE_SetAutoDisableSteps ( int iSteps )
{
	dWorldSetAutoDisableSteps ( g_ODEWorld, iSteps );
}

void ODE_SetAutoDisableTime ( float fTime )
{
	dWorldSetAutoDisableTime ( g_ODEWorld, fTime );
}

int ODE_GetAutoDisableFlag ( void )
{
	return dWorldGetAutoDisableFlag ( g_ODEWorld );
}

float ODE_GetAutoDisableLinearThreshold ( void )
{
	return dWorldGetAutoDisableLinearThreshold ( g_ODEWorld );
}

float ODE_GetAutoDisableAngularThreshold ( void )
{
	return dWorldGetAutoDisableAngularThreshold ( g_ODEWorld );
}

int ODE_GetAutoDisableSteps ( void )
{
	return dWorldGetAutoDisableSteps ( g_ODEWorld );
}

float ODE_GetAutoDisableTime ( void )
{
	return dWorldGetAutoDisableTime ( g_ODEWorld );
}

void ODE_SetWorldContactSurfaceLayer ( float fDepth )
{
	dWorldSetContactSurfaceLayer ( g_ODEWorld, fDepth );
}

float ODE_GetWorldContactSurfaceLayer ( void )
{
	return dWorldGetContactSurfaceLayer ( g_ODEWorld );
}

sODEBody* ODE_FindBodyID ( int iID )
{
	for ( int i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			return &g_ODEBodyList [ i ];
		}
	}

	return NULL;
}

int ODE_FindBodyID ( dBodyID body )
{
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].body == body )
		{
			return g_ODEObjectList [ i ].iID;
		}
	}
	return 0;
}

sODEObject* ODE_FindObjPtr ( dBodyID body )
{
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].body == body )
		{
			return &(g_ODEObjectList [ i ]);
		}
	}
	return 0;
}

int ODE_FindStaticBodyID ( dGeomID geom )
{
	for ( int i = 0; i < (int)g_ODEStaticObjectList.size ( ); i++ )
	{
		if ( g_ODEStaticObjectList [ i ].geom == geom )
		{
			return g_ODEStaticObjectList [ i ].iID;
		}
	}
	return 0;
}

void ODE_BodyCreate	( int iID )
{
	sODEBody	body    = { 0 };
	int			i		= 0;

	body.iID     = iID;
	body.dBodyID = dBodyCreate ( g_ODEWorld );

	for ( i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			// error - id already exists
		}
	}

	g_ODEBodyList.push_back ( body );
}

void ODE_BodyDestroy ( int iID )
{
	//paul - erase() needs an index 
	for ( int i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			g_ODEBodyList.erase( g_ODEBodyList.begin() + i );
		}
	}

}

void ODE_SetBodyPosition ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	dBodySetPosition ( pBody->dBodyID, fX, fY, fZ );
}

void ODE_SetBodyResponse ( int iID, int iResponseMode )
{
	// ode object
	sODEObject* pObject = ODE_FindID ( iID );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		// set respnse (0-default,1-ignorestatic,2-isstatic)
		pRagdoll->iResponseMode = iResponseMode;
	}
	if ( pObject )
	{
		// set respnse (0-default,1-ignorestatic,2-isstatic)
		pObject->iResponseMode = iResponseMode;
	}
}

void ODE_SetBodyRotation ( int iID, float fX, float fY, float fZ )
{
	// ode object
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return;

	// internal clearing value (for hacked Yrotation solution)
	if ( fX==-1.0f && fY==-1.0f && fZ==-1.0f )
	{
		pObject->iItemCarryGrab=0;
		return;
	}

	// vars needed
	dMatrix3 y, w;
	D3DXMATRIX matY, matW, matOLDW;

	// ode matrices
	dRSetIdentity ( y );
	dRSetIdentity ( w );

	// get ode W rotation
	if ( pObject->iItemCarryGrab==0 )
	{	
 		// store 'held' position
		pObject->iItemCarryGrab=1;
		const dReal* pRot = dBodyGetRotation ( pObject->body );
		if ( pRot )
		{
			for ( int i=0; i<12; i++ )
			{
				w[i] = pRot[i];
				pObject->wItemCarryGrab[i] = w[i];
			}
		}
	}
	else
	{
		// restore grabbed W
		for ( int i=0; i<12; i++ )
			w[i] = pObject->wItemCarryGrab[i];
	}

	// convert odeW to dxW
	convertMatrix ( matOLDW, w );

	// dx matrix for Y only
	dRFromEulerAngles( y, 0, D3DXToRadian ( -fY ), 0);

	// ode to dx Y only
	convertMatrix ( matY, y );

	// work out final DX matrux
	matW = matY * matOLDW;

	// dx to ode
	w [  0 ] = matW._11;
	w [  1 ] = matW._12;
	w [  2 ] = matW._13;
	w [  4 ] = matW._21;
	w [  5 ] = matW._22;
	w [  6 ] = matW._23;
	w [  8 ] = matW._31;
	w [  9 ] = matW._32;
	w [ 10 ] = matW._33;

	// final rotation
	dBodySetRotation(pObject->body, w);

	// done for temp solution.
	return;
}

void ODE_SetBodyLinearVelocity ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	// not the DBPro one!
	dBodySetLinearVel ( pBody->dBodyID, fX, fY, fZ );
}

void ODE_SetBodyAngularVelocity ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	dBodySetAngularVel ( pBody->dBodyID, fX, fY, fZ );
}

DWORD ODE_GetBodyTravelDistanceX ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateX();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

DWORD ODE_GetBodyTravelDistanceY ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateY();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

DWORD ODE_GetBodyTravelDistanceZ ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateZ();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

float ODE_GetBodyPositionX ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyPositionY ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyPositionZ ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyRotationX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fXAngle;
}

float ODE_GetBodyRotationY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fYAngle;
}

float ODE_GetBodyRotationZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fZAngle;
}

DWORD ODE_GetBodyLinearVelocityX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateX();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 0 ];
	}
}

DWORD ODE_GetBodyLinearVelocityY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateY();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 1 ];
	}
}

DWORD ODE_GetBodyLinearVelocityZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateZ();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 2 ];
	}
}

DWORD ODE_GetBodyAngularVelocityX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 0 ];
}

DWORD ODE_GetBodyAngularVelocityY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 1 ];
}

DWORD ODE_GetBodyAngularVelocityZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 2 ];
}

void CalculateShapeOfObjAfterRotation ( sODEObject* pObject )
{
	// Work out if need to recalculate
	if ( pObject->bNoNeedToRecalculateShape==false )
	{
		// Object rotation matrix
		float fRev=0;
		D3DXMATRIX matObjRot;
		const dReal* fRotate = dBodyGetRotation ( pObject->body );
		MakeRotationMatrix ( fRotate, &matObjRot );
		D3DXMatrixInverse ( &matObjRot, &fRev, &matObjRot );

		// Default ground direction
		D3DXVECTOR3 vecDir = D3DXVECTOR3( 0, -1, 0 );
		D3DXVec3TransformNormal ( &vecDir, &vecDir, &matObjRot );

		// Default size of object
		sObject* pActualObject = g_pfnGetObject ( pObject->iID );
		float fOrigXSize = ( pActualObject->collision.vecMax.x - pActualObject->collision.vecMin.x ) * pActualObject->position.vecScale.x;
		float fOrigYSize = ( pActualObject->collision.vecMax.y - pActualObject->collision.vecMin.y ) * pActualObject->position.vecScale.y;
		float fOrigZSize = ( pActualObject->collision.vecMax.z - pActualObject->collision.vecMin.z ) * pActualObject->position.vecScale.z;
		D3DXVECTOR3 vecBodySize = D3DXVECTOR3 ( pObject->fXSize, pObject->fYSize, pObject->fZSize );

		// Margins of object
		D3DXVECTOR3 vecMargin = vecBodySize - D3DXVECTOR3 ( fOrigXSize, fOrigYSize, fOrigZSize );
		if ( vecMargin.x < 0.0f ) vecMargin.x = 0.0f; else vecMargin.x/=2.0f;
		if ( vecMargin.y < 0.0f ) vecMargin.y = 0.0f; else vecMargin.y/=2.0f;
		if ( vecMargin.z < 0.0f ) vecMargin.z = 0.0f; else vecMargin.z/=2.0f;
		pObject->fActualMarginX = vecMargin.x * vecDir.x;
		pObject->fActualMarginY = vecMargin.y * vecDir.y;
		pObject->fActualMarginZ = vecMargin.z * vecDir.z;

		// After used size above, rotate against obj to get current body height
		D3DXVec3TransformCoord ( &vecBodySize, &vecBodySize, &matObjRot );
		pObject->fActualHeight = fabs ( (double)vecBodySize.y );

		// recalculated
		pObject->bNoNeedToRecalculateShape=true;
	}
}

DWORD ODE_GetBodyHeight ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get height of current body
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualHeight;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentX ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginX;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentY ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginY;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentZ ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginZ;
	return *(DWORD*)&fValue;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void convertMatrix ( D3DXMATRIX &t, const dReal *bMat )
{ 

   t._11 = bMat[0];
   t._12 = bMat[1];
   t._13 = bMat[2];
   t._14 = 0;
   t._21 = bMat[4];
   t._22 = bMat[5];
   t._23 = bMat[6];
   t._24 = 0;
   t._31 = bMat[8];
   t._32 = bMat[9];
   t._33 = bMat[10];
   t._34 = 0;


   t._41 = 0;
   t._42 = 0;
   t._43 = 0;
   t._44 = 0;
} 

void ODE_CreateStaticTriangleMesh ( int iObject )
{
	// get object 
	sObject* pObject = g_pfnGetObject ( iObject );

	// only accept Vertex only meshes (not indexed as they are not big enough in 16bit)
	DWORD dwVertexCount = 0;
	for ( int i = 0; i < pObject->iMeshCount; i++ )
		dwVertexCount += pObject->ppMeshList [ i ]->dwVertexCount;

	float*			vertices		= new float [ dwVertexCount * 3 ];
	int				vertexCount		= dwVertexCount;
	int*			triangles		= new int [ dwVertexCount * 3 ];
	int				triangleCount	= dwVertexCount * 3;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	sOffsetMap		offsetMap;
	dBodyID			body;
	dGeomID			geom;

	//paul missing "int i" declaration
	// collect triangles
	int iTriPos = 0;
	for ( int i = 0; i < pObject->iMeshCount; i++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ i ];

		// transform FVF with frame matrix
		g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
		TransformVertices ( pObject, pMesh, pObject->ppFrameList [ i ]->matTransformed );

		// go through verts of mesh
		BYTE* pVertex = pMesh->pVertexData;
		for ( int k = 0; k < (int)pMesh->dwVertexCount; k+=3 )
		{
			// get vertex data
			D3DXVECTOR3 vec0 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+0) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+0) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+0) ) ) );
			D3DXVECTOR3 vec1 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+1) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+1) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+1) ) ) );
			D3DXVECTOR3 vec2 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+2) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+2) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+2) ) ) );

			vertices [ iPos++ ] = vec0.x;
			vertices [ iPos++ ] = vec0.y;
			vertices [ iPos++ ] = vec0.z;
			vertices [ iPos++ ] = vec1.x;
			vertices [ iPos++ ] = vec1.y;
			vertices [ iPos++ ] = vec1.z;
			vertices [ iPos++ ] = vec2.x;
			vertices [ iPos++ ] = vec2.y;
			vertices [ iPos++ ] = vec2.z;

			// test and see poly size
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+0) ) ) = vec0;
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+1) ) ) = vec1;
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+2) ) ) = vec2;
		}

		//paul missing "int k" declaration
		// simple incremental indexes
		for ( int k = 0; k < (int)pMesh->dwVertexCount; k++ )
		{
			triangles [ iTriPos ] = iTriPos;
			iTriPos++;
		}
	
		// ODE controls transform now
		pObject->bDisableTransform = true;
	}

	// construct ODE static body
	body = dBodyCreate ( g_ODEWorld );
	triangleData = dGeomTriMeshDataCreate ( );
	dGeomTriMeshDataBuildSingle (	triangleData,
									// vertices
									&vertices [ 0 ],
									3 * sizeof ( float ),
									dwVertexCount,
									// faces
									( int* ) &triangles [ 0 ],
									dwVertexCount,
									3 * sizeof ( int )
									);

	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );
}

void ODE_SetRayCast( int iObject, int iMode )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	if ( !pObject ) return;

	dBodyDisable( pObject->body );

	pObject->bRayCast = iMode > 0;
}

void ODE_SetBodyMass ( int iObject, float fMass )
{
	dMass		mass;
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->AdjustMass ( fMass );
	}
	if ( pObject )
	{
		dMassSetBox  ( &mass, 1, pObject->fXSize, pObject->fYSize, pObject->fZSize );
		dMassAdjust  ( &mass, fMass );
		dBodySetMass ( pObject->body, &mass );
	}
}

void ODE_CreateDynamicCylinder ( int iObject )
{
	// init object
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// total bound size
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	// modify radius of object to reflect new cylinder identity HACK (ODE needs z Z-axis tube)
	pObject->collision.fRadius = object.fXSize;
	if ( object.fYSize > pObject->collision.fRadius )
		pObject->collision.fRadius = object.fYSize;

	// radius and length
	int direction = 3; // z-axis
	float fRad = pObject->collision.fRadius / 2.0f;
	float flen = object.fZSize;

	// ALAS ODE CANNOT CHECK COLLISION OF CYLINDER AGAINST TRI-MESH (NOR TRIMESH vs TRIMESH) :(
//	object.geom   = dCreateCCylinder  ( g_ODESpace, fRad, flen );

	// Fake a cylinder using a composite of a sphere and a box
	float fBulgeRadius = (object.fXSize+object.fZSize)/4;
	object.geom = dCreateSphere  ( g_ODESpace, fBulgeRadius * 1.1f );
	object.geom2 = dCreateBox  ( g_ODESpace, object.fXSize*0.95f, object.fYSize, object.fZSize*0.95f );

	object.body   = dBodyCreate ( g_ODEWorld );
	dMassSetSphere  ( &mass, 1, object.fXSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	dGeomSetBody     ( object.geom, object.body );
	dGeomSetBody     ( object.geom2, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	// X10 - 131007 - PAULs rotation code from DBP to ODE
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	// add physics object
	ODE_AddObject ( object, iObject, -1 );

	// lee - 170407
	pObject->position.bCustomWorldMatrix = true;
}

void ODE_CreateDynamicBox ( int iObject )
{
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// work out size based on collision bounds
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	object.geom   = dCreateBox  ( g_ODESpace, object.fXSize, object.fYSize, object.fZSize );
	object.body   = dBodyCreate ( g_ODEWorld );

	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	//paul 190207 - ODE matrix is formed using XYZ order, DBPro matrices are ZYX order, dRFromEulerAngles is incorrectly interpreting the angles
	//Rotation matrix needs to be copied, and transposed.
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	ODE_AddObject ( object, iObject, -1 );

	pObject->position.bCustomWorldMatrix = true;
}

void ODE_CreateDynamicBox ( int iObject, float fXSize, float fYSize, float fZSize )
{
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// work out size based on collision bounds
	object.fXSize = fXSize;
	object.fYSize = fYSize;
	object.fZSize = fZSize;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	object.geom   = dCreateBox  ( g_ODESpace, object.fXSize, object.fYSize, object.fZSize );
	object.body   = dBodyCreate ( g_ODEWorld );

	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	//paul 190207 - ODE matrix is formed using XYZ order, DBPro matrices are ZYX order, dRFromEulerAngles is incorrectly interpreting the angles
	//Rotation matrix needs to be copied, and transposed.
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	ODE_AddObject ( object, iObject, -1 );
	pObject->position.bCustomWorldMatrix = true;
}

void ODE_CreateHingeJoint ( int id, int iObj1, int iObj2, float axisX, float axisY, float axisZ, float posX, float posY, float posZ )
{
	sODEObject *pODEObject1 = ODE_FindID( iObj1 );
	sODEObject *pODEObject2 = ODE_FindID( iObj2 );

	if ( !pODEObject1 )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - Object (%d) has not been created in ODE", iObj1 );
		HandleError( errStr );
	}

	if ( !pODEObject2 )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - Object (%d) has not been created in ODE", iObj2 );
		HandleError( errStr );
	}

	if ( sODEJoint::GetJoint( id ) )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - hinge id (%d) already exists", id );
		HandleError( errStr );
	}

	sODEJoint::AddHingeJoint( id, pODEObject1->body, pODEObject2->body, axisX, axisY, axisZ, posX, posY, posZ );
}

void ODE_CreateBallJoint ( int id, int iObj1, int iObj2, float posX, float posY, float posZ )
{
	sODEObject *pODEObject1 = ODE_FindID( iObj1 );
	sODEObject *pODEObject2 = ODE_FindID( iObj2 );

	if ( !pODEObject1 )
	{
		sprintf_s( errStr, 256, "Cannot create ball joint - Object (%d) has not been created in ODE", iObj1 );
		HandleError( errStr );
	}

	if ( !pODEObject2 )
	{
		sprintf_s( errStr, 256, "Cannot create ball joint - Object (%d) has not been created in ODE", iObj2 );
		HandleError( errStr );
	}

	if ( sODEJoint::GetJoint( id ) )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - hinge id (%d) already exists", id );
		HandleError( errStr );
	}

	sODEJoint::AddBallJoint( id, pODEObject1->body, pODEObject2->body, posX, posY, posZ );
}

void ODE_SetHingeJointLimit ( int id, float low, float high )
{
	sODEJoint *pJoint = sODEJoint::GetJoint( id );
	if ( !pJoint )
	{
		sprintf_s( errStr, 256, "Cannot set hinge limit - hinge id (%d) does not exist", id );
		HandleError( errStr );
	}

	pJoint->SetHingeLimit( low, high );
}

void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale, int iDebugObject )
{
	sObject *pObject = g_pfnGetObject ( iObjID );

	if ( !pObject ) 
	{
		sprintf_s( errStr, 256, "Cannot create ragdoll - object (%d) does not exist", iObjID );
		HandleError( errStr );
	}

	if ( !pObject->ppMeshList )
	{
		sprintf_s( errStr, 256, "Cannot create ragdoll - object (%d) has no mesh", iObjID );
		HandleError( errStr );
	}

	// lee - 040707 - too many ragdolls overload the simulation and cause.
	// hmm, lets find out if its ragdoll joints or limbs in simulation

	// lee - 030307 - the rag doll uses the matCombined data of the object, but we
	// must update this at the point of rag doll conversion or the physical = 
	// achieved by set object frame with the update bounds parameter (,1)
	
	// now create the ragdoll
	sODERagdoll::AddRagdoll( iObjID, pObject, fScale, iDebugObject );

	// and now wipe out the objects local matrix as it is not inside the ragdoll limbs
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	pObject->position.vecPosition = D3DXVECTOR3(0,0,0);

	// 201007 - ensure no rotation is associated with object holding ragdoll (ragdoll uses custom matrix for each bone)
	for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
	{
		// find frame within object
		sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
		if ( pFrame ) D3DXMatrixIdentity ( &pFrame->matAbsoluteWorld );
	}

	// and ensure object is set as ragdoll (this skips the matCombined calculation normally done to animating/animframe objects)
	// 250607 - this also skips parts of the regular object processing for faster performance
	pObject->position.bCustomBoneMatrix = true;

	// U75 - 210210 - ensure cannot be culled
	pObject->collision.fRadius = 0;
	pObject->collision.fScaledRadius = 0;
	pObject->collision.fScaledLargestRadius = 0;

	// trigger an anim frame refresh (needed paul?)
	pObject->bAnimUpdateOnce = true;
}

void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale )
{
	ODE_CreateRagdollFromBoneModel( iObjID, fScale, 0 );
}

void ODE_DestroyRagdoll( int iRagdollID )
{
	// X10 - 051007 - silent fail if no object
	sObject *pObject = g_pfnGetObject ( iRagdollID );
	if ( pObject==NULL )
		return;

	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iRagdollID );
	if ( pRagdoll==NULL )
		return;

	// ensure object restored from ragdoll (this reincludes the matCombined calculation normally done to animating/animframe objects)
	pObject->position.bCustomBoneMatrix = false;

	// reset matrices to wipe out old ragdoll influemces
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	D3DXMatrixIdentity ( &pObject->position.matTranslation );
	D3DXMatrixIdentity ( &pObject->position.matObjectNoTran );

	// and now wipe out the objects local matrix as it is not inside the ragdoll limbs
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	pObject->position.vecPosition = D3DXVECTOR3(0,0,0);

	// remove from physics simulation
	pRagdoll->SetActive ( 0 );

	// and remove ragdoll
	sODERagdoll::RemoveRagdoll( pRagdoll );
}

void ODE_SetLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetLinearVelocity ( fX, fY, fZ );
	}
	if ( pObject )
	{
		dBodyEnable ( pObject->body );
		dBodySetLinearVel ( pObject->body, fX, fY, fZ );
	}
}

void ODE_SetAngularVelocity ( int iObject, float fX, float fY, float fZ )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetAngularVelocity ( fX, fY, fZ );
	}
	if ( pObject )
	{
		dBodyEnable ( pObject->body );
		dBodySetAngularVel ( pObject->body, fX, fY, fZ );
	}
}

void ODE_SetGravity ( int iObject, int iMode )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetGravity ( iMode );
	}
	if ( pObject )
	{
		dBodySetGravityMode ( pObject->body, iMode );
		dBodyEnable ( pObject->body );
	}
}

void ODE_SetActive ( int iObject, int iMode )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetActive ( iMode );
	}
	if ( pObject )
	{
		if ( iMode==0 )
			dBodyDisable ( pObject->body );
		else
			dBodyEnable ( pObject->body );
	}
}

void ODE_SetEnabled	( int iObject, int iEnabled )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
	}
	if ( pObject )
	{
		if ( iEnabled==0 )
			pObject->bEnabled = false;
		else
			pObject->bEnabled = true;
	}
}

void TransformVertices ( sObject* pObject, sMesh* pMesh, D3DXMATRIX matrix )
{
	sOffsetMap	offsetMap;
	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	for ( int iVertex = 0; iVertex < (int)pMesh->dwVertexCount; iVertex++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											  );

		D3DXVec3TransformCoord ( &vecPosition, &vecPosition, &matrix );

		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
		*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
		*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
	}
}

void ODE_CreateDynamicTriangleMesh	( int iObject )
{
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	dMass		mass;

	// work out size based on collision bounds
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	// GEOM creation
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];
	sOffsetMap		offsetMap;
	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;
		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);
		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ ) triangles [ i ] = pMesh->pIndices [ i ];
	triangleData = dGeomTriMeshDataCreate ( );
	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	object.geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );

	object.body   = dBodyCreate ( g_ODEWorld );
	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 400 );
	dBodySetMass ( object.body, &mass );
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	ODE_AddObject ( object, iObject, -1 );
	pObject->position.bCustomWorldMatrix = true;
}

bool GetODETriangleMesh ( sODECar* pCar, sObject* pObject, sMesh* pMesh )
{
	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	dReal			sides [ 3 ];
	sOffsetMap		offsetMap;

	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	TransformVertices ( pObject, pMesh, pObject->position.matWorld );

	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);

		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul - missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ )
		triangles [ i ] = pMesh->pIndices [ i ];

	pCar->bodies [ 0 ] = dBodyCreate ( g_ODEWorld );

	triangleData = dGeomTriMeshDataCreate ( );

	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	
	pCar->geom [ 0 ] = dCreateTriMesh ( 0, triangleData, 0, 0, 0 );

	sides [ 0 ] = 14;
	sides [ 1 ] = 5;
	sides [ 2 ] = 14;

	dGeomSetData ( pCar->geom [ 0 ], triangleData );
	dMassSetBox  ( &m, pCar->fMass, sides [ 0 ], sides [ 1 ], sides [ 2 ] );
	dGeomSetBody ( pCar->geom [ 0 ], pCar->bodies [ 0 ] );

	return true;
}

void ODE_CreateCar ( int iCar )
{
	memset ( &g_ODECarList [ iCar ], 0, sizeof ( sODECar ) );
}

void ODE_SetCarPosition ( int iCar, float fX, float fY, float fZ )
{
	g_ODECarList [ iCar ].fX = fX;
	g_ODECarList [ iCar ].fY = fY;
	g_ODECarList [ iCar ].fZ = fZ;
}

void ODE_SetCarMass ( int iCar, float fMass )
{
	g_ODECarList [ iCar ].fMass = fMass;
}

void ODE_SetCarMesh ( int iCar, int iObject, int iMode )
{
	sObject*	pObject = g_pfnGetObject ( iObject );
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];

	g_ODECarList [ iCar ].iID [ 0 ] = iObject;

	switch ( iMode )
	{
		case 0:
		{
			// sphere
		}
		break;

		case 1:
		{
			// triangle
			if ( ! GetODETriangleMesh ( &g_ODECarList [ iCar ], pObject, pMesh ) )
				return;
		}
		break;

		case 2:
		{
			// box
		}
		break;
	}
}

void ODE_SetCarWheel ( int iCar, int iWheel, int iObject, float fRadius )
{
	g_ODECarList [ iCar ].bodies [ iWheel + 1 ] = dBodyCreate ( g_ODEWorld );
	g_ODECarList [ iCar ].geom   [ iWheel + 1 ] = dCreateSphere ( 0, fRadius );
	g_ODECarList [ iCar ].iID    [ iWheel + 1 ] = iObject;
	g_ODECarList [ iCar ].fWheelRadius			= fRadius;

	dGeomSetBody ( g_ODECarList [ iCar ].geom [ iWheel + 1 ], g_ODECarList [ iCar ].bodies [ iWheel + 1 ] );

	sObject* pObject = g_pfnGetObject ( iObject );
	TransformVertices ( pObject, pObject->ppMeshList [ 0 ], pObject->position.matWorld );

}

void ODE_SetCarWheelMass ( int iCar, int iWheel, float fMass )
{
	dMass mass;

	dMassSetSphere ( &mass, 1, g_ODECarList [ iCar ].fWheelRadius );
	dMassAdjust	   ( &mass, fMass );
}

void ODE_SetCarWheelRotation ( int iCar, int iWheel, float fX, float fY, float fZ )
{
	dMatrix3 r;

	dRFromEulerAngles ( r, fX, fY, fZ );
	dBodySetRotation  ( g_ODECarList [ iCar ].bodies [ iWheel + 1 ], r );
}

void ODE_SetCarWheelPosition ( int iCar, int iWheel, float fX, float fY, float fZ )
{
	dBodySetPosition ( g_ODECarList [ iCar ].bodies [ iWheel + 1 ], fX, fY, fZ );
}

void ODE_SetCarDefaultJoints ( int iCar )
{
	for ( int i = 0; i < 4; ++i )
	{
		g_ODECarList [ iCar ].joints [ i ] = dJointCreateHinge2 ( g_ODEWorld, 0 );
		dJointAttach ( g_ODECarList [ iCar ].joints [ i ], g_ODECarList [ iCar ].bodies [ 0 ], g_ODECarList [ iCar ].bodies [ i + 1 ] );
		
		const dReal* const wPos = dBodyGetPosition ( g_ODECarList [ iCar ].bodies [ i + 1 ] );

		dJointSetHinge2Anchor ( g_ODECarList [ iCar ].joints [ i ], wPos [ 0 ], wPos [ 1 ], wPos [ 2 ] );
		dJointSetHinge2Axis1  ( g_ODECarList [ iCar ].joints [ i ], 0, 1, 0 );
		dJointSetHinge2Axis2  ( g_ODECarList [ iCar ].joints [ i ], 0, 0, ( ( i % 2 ) == 0 ) ? -1 : 1 );
		
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamLoStop, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamHiStop, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamFMax, 50 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamVel2, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamFMax2, 80 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamSuspensionERP, 0.25 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamSuspensionCFM, 0.004f * 50 );
	}
}


void ODE_UpdateCar ( int iCar )
{
	g_ODECarList [ iCar ].space = dHashSpaceCreate ( g_ODESpace );

	dSpaceSetCleanup ( g_ODECarList [ iCar ].space, 0 );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 0 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 1 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 2 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 3 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 4 ] );

	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 0 ], g_ODECarList [ iCar ].geom [ 0 ], g_ODECarList [ iCar ].iID [ 0 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 1 ], g_ODECarList [ iCar ].geom [ 1 ], g_ODECarList [ iCar ].iID [ 1 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 2 ], g_ODECarList [ iCar ].geom [ 2 ], g_ODECarList [ iCar ].iID [ 2 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 3 ], g_ODECarList [ iCar ].geom [ 3 ], g_ODECarList [ iCar ].iID [ 3 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 4 ], g_ODECarList [ iCar ].geom [ 4 ], g_ODECarList [ iCar ].iID [ 4 ], -1 );
}

void ODE_CreateStaticBox ( int iObject, float fXSize, float fYSize, float fZSize )
{
	sODEObject box     = { 0 };
	sObject*   pObject = g_pfnGetObject ( iObject );

	if ( !pObject )
		return;

	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	
	if ( fXSize == 0.0f ) fXSize = 0.01f;
	if ( fYSize == 0.0f ) fYSize = 0.01f;
	if ( fZSize == 0.0f ) fZSize = 0.01f;

	

	box.geom = dCreateBox ( g_ODESpace, fXSize, fYSize, fZSize );

	dGeomSetPosition ( box.geom, fXPos, fYPos, fZPos );	
}


void ODE_CreateStaticTriangleMesh ( int iObject, float fX, float fY, float fZ )
{
	sObject*	pObject = g_pfnGetObject ( iObject );
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];

	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	sOffsetMap		offsetMap;

	dBodyID		body;
	dGeomID		geom;

	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	TransformVertices ( pObject, pMesh, pObject->position.matWorld );

	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);

		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul - missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ )
		triangles [ i ] = pMesh->pIndices [ i ];

	body = dBodyCreate ( g_ODEWorld );

	triangleData = dGeomTriMeshDataCreate ( );

	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	
	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );

	pObject->bDisableTransform = true;
}

*/

void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
 // from http://www.martinb.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
 float m00 = pmatMatrix->_11;
 float m01 = pmatMatrix->_12;
 float m02 = pmatMatrix->_13;
 float m12 = pmatMatrix->_23;
 float m22 = pmatMatrix->_33;
 float heading = (float)atan2(m01,m00);
 float attitude = (float)atan2(m12,m22);
 float bank = (float)asin(-m02);

 // check for gimbal lock
 if ( fabs ( m02 ) > 1.0f )
 {
  // looking straight up or down
  float PI = D3DX_PI / 2.0f;
  pVecAngles->x = 0.0f;
  pVecAngles->y = D3DXToDegree ( PI * m02 );
  pVecAngles->z = 0.0f;
 }
 else
 {
  pVecAngles->x = D3DXToDegree ( attitude );
  pVecAngles->y = D3DXToDegree ( bank );
  pVecAngles->z = D3DXToDegree ( heading );
 }
}
