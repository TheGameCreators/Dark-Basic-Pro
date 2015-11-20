/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "CharacterDemo.h"


#ifdef DYNAMIC_CHARACTER_CONTROLLER
#include "DynamicCharacterController.h"
#else
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#endif

const int maxProxies = 32766;
const int maxOverlap = 65535;

static int gForward = 0;
static int gBackward = 0;
static int gLeft = 0;
static int gRight = 0;
static int gJump = 0;


typedef unsigned char byte_t;



////////////////////////////////////////////////////////////////////////////////
//
//	static helper methods
//
//	Only used within this file (helpers and terrain generation, etc)
//
////////////////////////////////////////////////////////////////////////////////

static const char *
getTerrainTypeName
(
eTerrainModel model
)
{
	switch (model) {
	case eRadial:
		return "Radial";

	case eFractal:
		return "Fractal";

	default:
		btAssert(!"bad terrain model type");
	}

	return NULL;
}



static const char *
getDataTypeName
(
PHY_ScalarType type
)
{
	switch (type) {
	case PHY_UCHAR:
		return "UnsignedChar";

	case PHY_SHORT:
		return "Short";

	case PHY_FLOAT:
		return "Float";

	default:
		btAssert(!"bad heightfield data type");
	}

	return NULL;
}



static const char *
getUpAxisName
(
int axis
)
{
	switch (axis) {
	case 0:
		return "X";

	case 1:
		return "Y";

	case 2:
		return "Z";

	default:
		btAssert(!"bad up axis");
	}

	return NULL;
}



static btVector3
getUpVector
(
int upAxis,
btScalar regularValue,
btScalar upValue
)
{
	btAssert(upAxis >= 0 && upAxis <= 2 && "bad up axis");

	btVector3 v(regularValue, regularValue, regularValue);
	v[upAxis] = upValue;

	return v;
}



// TODO: it would probably cleaner to have a struct per data type, so
// 	you could lookup byte sizes, conversion functions, etc.
static int getByteSize
(
PHY_ScalarType type
)
{
	int size = 0;

	switch (type) {
	case PHY_FLOAT:
		size = sizeof(float);
		break;

	case PHY_UCHAR:
		size = sizeof(unsigned char);
		break;

	case PHY_SHORT:
		size = sizeof(short);
		break;

	default:
		btAssert(!"Bad heightfield data type");
	}

	return size;
}



static float
convertToFloat
(
const byte_t * p,
PHY_ScalarType type
)
{
	btAssert(p);

	switch (type) {
	case PHY_FLOAT:
		{
			float * pf = (float *) p;
			return *pf;
		}

	case PHY_UCHAR:
		{
			unsigned char * pu = (unsigned char *) p;
			return ((*pu) * s_gridHeightScale);
		}

	case PHY_SHORT:
		{
			short * ps = (short *) p;
			return ((*ps) * s_gridHeightScale);
		}

	default:
		btAssert(!"bad type");
	}

	return 0;
}



static float
getGridHeight
(
byte_t * grid,
int i,
int j,
PHY_ScalarType type
)
{
	btAssert(grid);
	btAssert(i >= 0 && i < s_gridSize);
	btAssert(j >= 0 && j < s_gridSize);

	int bpe = getByteSize(type);
	btAssert(bpe > 0 && "bad bytes per element");

	int idx = (j * s_gridSize) + i;
	long offset = ((long) bpe) * idx;

	byte_t * p = grid + offset;

	return convertToFloat(p, type);
}



static void
convertFromFloat
(
byte_t * p,
float value,
PHY_ScalarType type
)
{
	btAssert(p && "null");

	switch (type) {
	case PHY_FLOAT:
		{
			float * pf = (float *) p;
			*pf = value;
		}
		break;

	case PHY_UCHAR:
		{
			unsigned char * pu = (unsigned char *) p;
			*pu = (unsigned char) (value / s_gridHeightScale);
		}
		break;

	case PHY_SHORT:
		{
			short * ps = (short *) p;
			*ps = (short) (value / s_gridHeightScale);
		}
		break;

	default:
		btAssert(!"bad type");
	}
}



// creates a radially-varying heightfield
static void
setRadial
(
byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
float phase = 0.0
)
{
	btAssert(grid);
	btAssert(bytesPerElement > 0);

	// min/max
	float period = 0.5 / s_gridSpacing;
	float floor = 0.0;
	float min_r = 3.0 * sqrt(s_gridSpacing);
	float magnitude = 50.0 * sqrt(s_gridSpacing);

	// pick a base_phase such that phase = 0 results in max height
	//   (this way, if you create a heightfield with phase = 0,
	//    you can rely on the min/max heights that result)
	float base_phase = (0.5 * SIMD_PI) - (period * min_r);
	phase += base_phase;

	// center of grid
	float cx = 0.5 * s_gridSize * s_gridSpacing;
	float cy = cx;		// assume square grid
	byte_t * p = grid;
	for (int i = 0; i < s_gridSize; ++i) {
		float x = i * s_gridSpacing;
		for (int j = 0; j < s_gridSize; ++j) {
			float y = j * s_gridSpacing;

			float dx = x - cx;
			float dy = y - cy;

			float r = sqrt((dx * dx) + (dy * dy));

			float z = period;
			if (r < min_r) {
				r = min_r;
			}
			z = (1.0 / r) * sin(period * r + phase);
			if (z > period) {
				z = period;
			} else if (z < -period) {
				z = -period;
			}
			z = floor + magnitude * z;

			convertFromFloat(p, z, type);
			p += bytesPerElement;
		}
	}
}



static float
randomHeight
(
int step
)
{
	return (0.33 * s_gridSpacing * s_gridSize * step * (rand() - (0.5 * RAND_MAX))) / (1.0 * RAND_MAX * s_gridSize);
}



static void
dumpGrid
(
const byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
int max
)
{
	//std::cerr << "Grid:\n";

	char buffer[32];

	for (int j = 0; j < max; ++j) {
		for (int i = 0; i < max; ++i) {
			long offset = j * s_gridSize + i;
			float z = convertToFloat(grid + offset * bytesPerElement, type);
			sprintf(buffer, "%6.2f", z);
			//std::cerr << "  " << buffer;
		}
		//std::cerr << "\n";
	}
}



static void
updateHeight
(
byte_t * p,
float new_val,
PHY_ScalarType type
)
{
	float old_val = convertToFloat(p, type);
	if (!old_val) {
		convertFromFloat(p, new_val, type);
	}
}



// creates a random, fractal heightfield
static void
setFractal
(
byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
int step
)
{
	btAssert(grid);
	btAssert(bytesPerElement > 0);
	btAssert(step > 0);
	btAssert(step < s_gridSize);

	int newStep = step / 2;
//	std::cerr << "Computing grid with step = " << step << ": before\n";
//	dumpGrid(grid, bytesPerElement, type, step + 1);

	// special case: starting (must set four corners)
	if (s_gridSize - 1 == step) {
		// pick a non-zero (possibly negative) base elevation for testing
		float base = randomHeight(step / 2);

		convertFromFloat(grid, base, type);
		convertFromFloat(grid + step * bytesPerElement, base, type);
		convertFromFloat(grid + step * s_gridSize * bytesPerElement, base, type);
		convertFromFloat(grid + (step * s_gridSize + step) * bytesPerElement, base, type);
	}

	// determine elevation of each corner
	float c00 = convertToFloat(grid, type);
	float c01 = convertToFloat(grid + step * bytesPerElement, type);
	float c10 = convertToFloat(grid + (step * s_gridSize) * bytesPerElement, type);
	float c11 = convertToFloat(grid + (step * s_gridSize + step) * bytesPerElement, type);

	// set top middle
	updateHeight(grid + newStep * bytesPerElement, 0.5 * (c00 + c01) + randomHeight(step), type);

	// set left middle
	updateHeight(grid + (newStep * s_gridSize) * bytesPerElement, 0.5 * (c00 + c10) + randomHeight(step), type);

	// set right middle
	updateHeight(grid + (newStep * s_gridSize + step) * bytesPerElement, 0.5 * (c01 + c11) + randomHeight(step), type);

	// set bottom middle
	updateHeight(grid + (step * s_gridSize + newStep) * bytesPerElement, 0.5 * (c10 + c11) + randomHeight(step), type);

	// set middle
	updateHeight(grid + (newStep * s_gridSize + newStep) * bytesPerElement, 0.25 * (c00 + c01 + c10 + c11) + randomHeight(step), type);

//	std::cerr << "Computing grid with step = " << step << ": after\n";
//	dumpGrid(grid, bytesPerElement, type, step + 1);

	// terminate?
	if (newStep < 2) {
		return;
	}

	// recurse
	setFractal(grid, bytesPerElement, type, newStep);
	setFractal(grid + newStep * bytesPerElement, bytesPerElement, type, newStep);
	setFractal(grid + (newStep * s_gridSize) * bytesPerElement, bytesPerElement, type, newStep);
	setFractal(grid + ((newStep * s_gridSize) + newStep) * bytesPerElement, bytesPerElement, type, newStep);
}



static byte_t *
getRawHeightfieldData
(
eTerrainModel model,
PHY_ScalarType type,
btScalar& minHeight,
btScalar& maxHeight
)
{
//	std::cerr << "\nRegenerating terrain\n";
//	std::cerr << "  model = " << model << "\n";
//	std::cerr << "  type = " << type << "\n";

	long nElements = ((long) s_gridSize) * s_gridSize;
//	std::cerr << "  nElements = " << nElements << "\n";

	int bytesPerElement = getByteSize(type);
//	std::cerr << "  bytesPerElement = " << bytesPerElement << "\n";
	btAssert(bytesPerElement > 0 && "bad bytes per element");

	long nBytes = nElements * bytesPerElement;
//	std::cerr << "  nBytes = " << nBytes << "\n";
	byte_t * raw = new byte_t[nBytes];
	btAssert(raw && "out of memory");

	// reseed randomization every 30 seconds
//	srand(time(NULL) / 30);

	// populate based on model
	switch (model) {
	case eRadial:
		setRadial(raw, bytesPerElement, type);
		break;

	case eFractal:
		for (int i = 0; i < nBytes; i++)
		{
			raw[i] = 0;
		}
		setFractal(raw, bytesPerElement, type, s_gridSize - 1);
		break;

	default:
		btAssert(!"bad model type");
	}

	if (0) {
		// inside if(0) so it keeps compiling but isn't
		// 	exercised and doesn't cause warnings
//		std::cerr << "final grid:\n";
		dumpGrid(raw, bytesPerElement, type, s_gridSize - 1);
	}

	// find min/max
	for (int i = 0; i < s_gridSize; ++i) {
		for (int j = 0; j < s_gridSize; ++j) {
			float z = getGridHeight(raw, i, j, type);
//			std::cerr << "i=" << i << ", j=" << j << ": z=" << z << "\n";

			// update min/max
			if (!i && !j) {
				minHeight = z;
				maxHeight = z;
			} else {
				if (z < minHeight) {
					minHeight = z;
				}
				if (z > maxHeight) {
					maxHeight = z;
				}
			}
		}
	}

	if (maxHeight < -minHeight) {
		maxHeight = -minHeight;
	}
	if (minHeight > -maxHeight) {
		minHeight = -maxHeight;
	}

//	std::cerr << "  minHeight = " << minHeight << "\n";
//	std::cerr << "  maxHeight = " << maxHeight << "\n";

	return raw;
}



/// TERRAIN CONSTANTS END














CharacterDemo::CharacterDemo()
:
m_indexVertexArrays(0),
m_vertices(0),
m_cameraHeight(4.f),
m_minCameraDistance(3.f),
m_maxCameraDistance(10.f)
{
	m_character = 0;
	m_cameraPosition = btVector3(30,30,30);
}




void CharacterDemo::initPhysics()
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(50,3,50));
	m_collisionShapes.push_back(groundShape);
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
	m_overlappingPairCache = sweepBP;

	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);
	m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration=0.0001f;
	
#ifdef DYNAMIC_CHARACTER_CONTROLLER
	m_character = new DynamicCharacterController ();
#else
	
	btTransform startTransform;
	startTransform.setIdentity ();
	//startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
	startTransform.setOrigin (btVector3(10.210098,-1.6433364,16.453260));


	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight=1.75;
	btScalar characterWidth =1.75;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
	m_ghostObject->setCollisionShape (capsule);
	m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight);
#endif

	////////////////

	/// Create some basic environment from a Quake level

	//m_dynamicsWorld->setGravity(btVector3(0,0,0));
	btTransform tr;
	tr.setIdentity();

	if ( 0 )
	{

	const char* bspfilename = "BspDemo.bsp";
	void* memoryBuffer = 0;

	FILE* file = fopen(bspfilename,"r");
	if (!file)
	{
		//cmake generated visual studio projects need 4 levels back
		bspfilename = "../../../../BspDemo.bsp";
		file = fopen(bspfilename,"r");
	}
	if (!file)
	{
		//visual studio leaves the current working directory in the projectfiles folder
		bspfilename = "../../BspDemo.bsp";
		file = fopen(bspfilename,"r");
	}
	if (!file)
	{
		//visual studio leaves the current working directory in the projectfiles folder
		bspfilename = "BspDemo.bsp";
		file = fopen(bspfilename,"r");
	}

	if (file)
	{
		BspLoader bspLoader;
		int size=0;
		if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET)) {        /* File operations denied? ok, just close and return failure */
			printf("Error: cannot get filesize from %s\n", bspfilename);
		} else
		{
			//how to detect file size?
			memoryBuffer = malloc(size+1);
			fread(memoryBuffer,1,size,file);
			bspLoader.loadBSPFile( memoryBuffer);

			BspToBulletConverter bsp2bullet(this);
			float bspScaling = 0.1f;
			bsp2bullet.convertBsp(bspLoader,bspScaling);

		}
		fclose(file);
	}

	}

	///only collide with static for now (no interaction with dynamic objects)
	m_dynamicsWorld->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	m_dynamicsWorld->addAction(m_character);





	///////////////
	///////////////
	// ATTEMPT TO ADD TERRAIN IN
	if ( 1 )
	{

	m_upAxis=1;
	m_type = PHY_FLOAT;
	m_model = eFractal;


	// get new heightfield of appropriate type
	m_rawHeightfieldData =
	    getRawHeightfieldData(m_model, m_type, m_minHeight, m_maxHeight);
	btAssert(m_rawHeightfieldData && "failed to create raw heightfield");
	bool flipQuadEdges = false;
	btHeightfieldTerrainShape * heightfieldShape =
	    new btHeightfieldTerrainShape(s_gridSize, s_gridSize,
					  m_rawHeightfieldData,
					  s_gridHeightScale,
					  m_minHeight, m_maxHeight,
					  m_upAxis, m_type, flipQuadEdges);
	btAssert(heightfieldShape && "null heightfield");

	// scale the shape
	btVector3 localScaling = getUpVector(m_upAxis, s_gridSpacing, 1.0);
	heightfieldShape->setLocalScaling(localScaling);

	// stash this shape away
	m_collisionShapes.push_back(heightfieldShape);

	// set origin to middle of heightfield
	//btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,-20,0));

	// create ground object
	float mass = 0.0;
	localCreateRigidBody(mass, tr, heightfieldShape);

	}

	///////////////
	///////////////


	///////////////

	clientResetScene();

	setCameraDistance(56.f);

}


//to be implemented by the demo
void CharacterDemo::renderme()
{
	updateCamera();

	DemoApplication::renderme();
}



void	CharacterDemo::debugDrawContacts()
{
//	printf("numPairs = %d\n",m_customPairCallback->getOverlappingPairArray().size());
	{
		btManifoldArray	manifoldArray;
		btBroadphasePairArray& pairArray = m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i=0;i<numPairs;i++)
		{
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];
			
			btBroadphasePair* collisionPair = m_overlappingPairCache->getOverlappingPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
			if (!collisionPair)
				continue;

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			for (int j=0;j<manifoldArray.size();j++)
			{
				btPersistentManifold* manifold = manifoldArray[j];
				for (int p=0;p<manifold->getNumContacts();p++)
				{
					const btManifoldPoint&pt = manifold->getContactPoint(p);

					btVector3 color(255,255,255);
					m_dynamicsWorld->getDebugDrawer()->drawContactPoint(pt.getPositionWorldOnB(),pt.m_normalWorldOnB,pt.getDistance(),pt.getLifeTime(),color);
				}
			}
		}
	}

}

void CharacterDemo::clientMoveAndDisplay()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float dt = getDeltaTimeMicroseconds() * 0.000001f;

	/* Character stuff &*/
	if (m_character)
	{
		
	}

	debugDrawContacts();


	if (m_dynamicsWorld)
	{
		//during idle mode, just run 1 simulation step maximum
		int maxSimSubSteps = m_idle ? 1 : 2;
		if (m_idle)
			dt = 1.0/420.f;

		///set walkDirection for our character
		btTransform xform;
		xform = m_ghostObject->getWorldTransform ();

		btVector3 forwardDir = xform.getBasis()[2];
	//	printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
		btVector3 upDir = xform.getBasis()[1];
		btVector3 strafeDir = xform.getBasis()[0];
		forwardDir.normalize ();
		upDir.normalize ();
		strafeDir.normalize ();

		btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
		btScalar walkVelocity = btScalar(1.1) * 40.0; // 4 km/h -> 1.1 m/s
		btScalar walkSpeed = walkVelocity * dt;

		//rotate view
		if (gRight)
		{
			btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
			orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0),0.01));
			m_ghostObject->getWorldTransform ().setBasis(orn);
		}

		if (gLeft )
		{
			btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
			orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0),-0.01));
			m_ghostObject->getWorldTransform ().setBasis(orn);
		}

		if (gForward)
			walkDirection += forwardDir;

		if (gBackward)
			walkDirection -= forwardDir;	

		if (gJump)
		{
			m_character->setJumpSpeed(10.0f);
			m_character->jump();
		}

		m_character->setWalkDirection(walkDirection*walkSpeed);


		int numSimSteps = m_dynamicsWorld->stepSimulation(dt,maxSimSubSteps);

		//optional but useful: debug drawing
		if (m_dynamicsWorld)
			m_dynamicsWorld->debugDrawWorld();

//#define VERBOSE_FEEDBACK
#ifdef VERBOSE_FEEDBACK
		if (!numSimSteps)
			printf("Interpolated transforms\n");
		else
		{
			if (numSimSteps > maxSimSubSteps)
			{
				//detect dropping frames
				printf("Dropped (%i) simulation steps out of %i\n",numSimSteps - maxSimSubSteps,numSimSteps);
			} else
			{
				printf("Simulated (%i) steps\n",numSimSteps);
			}
		}
#endif //VERBOSE_FEEDBACK

	}







#ifdef USE_QUICKPROF
        btProfiler::beginBlock("render");
#endif //USE_QUICKPROF


	renderme();

#ifdef USE_QUICKPROF
        btProfiler::endBlock("render");
#endif


	glFlush();
	glutSwapBuffers();

}



void CharacterDemo::displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderme();

	//optional but useful: debug drawing
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();

	debugDrawContacts();

	glFlush();
	glutSwapBuffers();
}

void CharacterDemo::clientResetScene()
{
	m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(),getDynamicsWorld()->getDispatcher());

	m_character->reset ();
	///WTF
	m_character->warp (btVector3(10.210001,-2.0306311,16.576973));
	
}

void CharacterDemo::specialKeyboardUp(int key, int x, int y)
{
   switch (key)
    {
    case GLUT_KEY_UP:
	{
		gForward = 0;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		gBackward = 0;
	}
	break;
	case GLUT_KEY_LEFT:
	{
		gLeft = 0;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		gRight = 0;
	}
	break;
	default:
		DemoApplication::specialKeyboardUp(key,x,y);
        break;
    }
}


void CharacterDemo::specialKeyboard(int key, int x, int y)
{

//	printf("key = %i x=%i y=%i\n",key,x,y);

	gJump=0;

    switch (key)
    {
    case GLUT_KEY_UP:
	{
		gForward = 1;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		gBackward = 1;
	}
	break;
	case GLUT_KEY_LEFT:
	{
		gLeft = 1;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		gRight = 1;
	}
	break;
	case GLUT_KEY_F1:
	{
		if (m_character && m_character->canJump())
			gJump = 1;
	}
	break;
	default:
		DemoApplication::specialKeyboard(key,x,y);
        break;
    }

//	glutPostRedisplay();


}

void	CharacterDemo::updateCamera()
{

//#define DISABLE_CAMERA 1
#ifdef DISABLE_CAMERA
	DemoApplication::updateCamera();
	return;
#endif //DISABLE_CAMERA

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	btTransform characterWorldTrans;

	//look at the vehicle
	characterWorldTrans = m_ghostObject->getWorldTransform();
	btVector3 up = characterWorldTrans.getBasis()[1];
	btVector3 backward = -characterWorldTrans.getBasis()[2];
	up.normalize ();
	backward.normalize ();

	m_cameraTargetPosition = characterWorldTrans.getOrigin();
	m_cameraPosition = m_cameraTargetPosition + up * 10.0 + backward * 12.0;
	
	//use the convex sweep test to find a safe position for the camera (not blocked by static geometry)
	btSphereShape cameraSphere(0.2f);
	btTransform cameraFrom,cameraTo;
	cameraFrom.setIdentity();
	cameraFrom.setOrigin(characterWorldTrans.getOrigin());
	cameraTo.setIdentity();
	cameraTo.setOrigin(m_cameraPosition);
	
	btCollisionWorld::ClosestConvexResultCallback cb( characterWorldTrans.getOrigin(), cameraTo.getOrigin() );
	cb.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
		
	m_dynamicsWorld->convexSweepTest(&cameraSphere,cameraFrom,cameraTo,cb);
	if (cb.hasHit())
	{

		btScalar minFraction  = cb.m_closestHitFraction;//btMax(btScalar(0.3),cb.m_closestHitFraction);
		m_cameraPosition.setInterpolate3(cameraFrom.getOrigin(),cameraTo.getOrigin(),minFraction);
	}




	//update OpenGL camera settings
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    gluLookAt(m_cameraPosition[0],m_cameraPosition[1],m_cameraPosition[2],
		      m_cameraTargetPosition[0],m_cameraTargetPosition[1], m_cameraTargetPosition[2],
			  m_cameraUp.getX(),m_cameraUp.getY(),m_cameraUp.getZ());



}


CharacterDemo::~CharacterDemo()
{
	//cleanup in the reverse order of creation/initialization
	if (m_character)
	{
		m_dynamicsWorld->removeCollisionObject(m_ghostObject);
	}
	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	delete m_indexVertexArrays;
	delete m_vertices;

	//delete dynamics world
	delete m_dynamicsWorld;

	//delete solver
	delete m_constraintSolver;

	//delete broadphase
	delete m_overlappingPairCache;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;

}

