#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <float.h>

#include "NXU_ColladaImport.h"
#include "NXU_tinyxml.h"
#include "NXU_Asc2Bin.h"
#include "NXU_string.h"
#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_SchemaStream.h"
#include "NXU_hull.h"

#ifdef WIN32
#pragma warning(disable:4996)
#endif


namespace	NXU
{

const	float	DEG_TO_RAD = ((2.0f	*3.14152654f)	/	360.0f);
// const	float	RAD_TO_DEG = (360.0f / (2.0f *3.141592654f));

static bool	getTF(const	char *str)
{
	bool ret = false;
	if (str)
	{
		if (strcasecmp(str, "true") ==	0	|| strcasecmp(str,	"1") ==	0)
		{
			ret	=	true;
		}
	}
	return ret;
}

#define	NODE_STACK 64

// list	of collada elements	we even	care about.
enum CELEMENT
{
	CE_LIBRARY_GEOMETRIES, CE_GEOMETRY,	 //	attribute	'id'
	CE_MESH, CE_CONVEX_MESH, CE_SOURCE,	 //	attribute	'id'
	CE_FLOAT_ARRAY,	 //	attribute	'count'	attribute	'id'
	CE_INT_ARRAY,	CE_BOOL_ARRAY, CE_NAME_ARRAY,	CE_IDREF_ARRAY,	CE_TECHNIQUE_COMMON,	//
	CE_ACCESSOR,	// attribute 'count' attribute 'source'	attribute	'stride'
	CE_PARAM,	 //	parameter	in the accessor	attribute	'name' and 'type'
	CE_VERTICES,	// attribute 'id'
	CE_INPUT,	 //	attribute	'semantic' attribute 'source'
	CE_POLYGONS,	// attribute 'material'	attribute	'count'
	CE_TRIANGLES,	CE_POLYLIST, CE_VCOUNT,	CE_P,	 //	individual polygon.
	CE_LIBRARY_VISUAL_SCENES,	 //
	CE_VISUAL_SCENE,	// attribute 'id'
	CE_NODE,	// attribute 'id'	attribute	'type'
	CE_MATRIX,	// attribute 'sid'
	CE_INSTANCE_GEOMETRY,	 //	attribute	'url'
	CE_TRANSLATE,	CE_ROTATE, CE_LIBRARY_PHYSICS_MATERIALS,	//
	CE_PHYSICS_MATERIAL,	// attribute 'id'
	CE_DYNAMIC_FRICTION, CE_RESTITUTION, CE_STATIC_FRICTION, CE_LIBRARY_PHYSICS_MODELS,	CE_PHYSICS_MODEL,
	// attribute 'id'
	CE_RIGID_BODY,	// attribute 'sid'
	CE_INSTANCE_PHYSICS_MATERIAL,	 //	attribute	'url'
	CE_SHAPE,	CE_BOX,	CE_HALF_EXTENTS, CE_DYNAMIC, CE_MASS,	CE_MASS_FRAME, CE_INERTIA, CE_LIBRARY_PHYSICS_SCENES,	CE_PHYSICS_SCENE,	 //	attribute	'id'
	CE_INSTANCE_PHYSICS_MODEL,	// attribute 'url'
	CE_INSTANCE_RIGID_BODY,	 //	attribute	'target' attribute 'body'
	CE_VELOCITY, CE_ANGULAR_VELOCITY,	CE_GRAVITY,	CE_TIME_STEP,	CE_SCENE,	CE_INSTANCE_VISUAL_SCENE,	 //	attribute	'url'
	CE_INSTANCE_PHYSICS_SCENE,	// attribute 'url'
	CE_DENSITY,	CE_EQUATION, CE_RADIUS,	CE_HEIGHT, CE_CAPSULE, CE_PLANE, CE_SPHERE,	CE_CYLINDER, CE_FORCE_FIELD, CE_INSTANCE_FORCE_FIELD,	CE_LIBRARY_FORCE_FIELDS, CE_RIGID_CONSTRAINT,	CE_INSTANCE_RIGID_CONSTRAINT,	CE_TAPERED_CAPSULE,	CE_TAPERED_CYLINDER, CE_RADIUS1, CE_RADIUS2, CE_DISABLE_COLLISION, CE_MIN, CE_MAX, CE_STIFFNESS, CE_DAMPING, CE_TARGET_VALUE,	CE_ENABLED,	CE_INTERPENETRATE, CE_REF_ATTACHMENT,	CE_ATTACHMENT, CE_LINEAR,	CE_SWING_CONE_AND_TWIST, CE_ANGULAR, CE_GROUP, CE_SKIN_WIDTH,	CE_WAKEUP_COUNTER, CE_LINEAR_DAMPING,	CE_ANGULAR_DAMPING,	CE_MAX_ANGULAR_VELOCITY, CE_SLEEP_LINEAR_VELOCITY, CE_SLEEP_ANGULAR_VELOCITY,	CE_SOLVER_ITERATION_COUNT, CE_DISABLE_GRAVITY, CE_KINEMATIC, CE_POSE_SLEEP_TEST, CE_FILTER_SLEEP_VELOCITY, CE_DISABLE_RESPONSE,	CE_LOCK_COM, CE_PROJECTION_MODE, CE_PROJECTION_DISTANCE, CE_PROJECTION_ANGLE,
	//
	CE_NX_JOINT_DRIVE_DESC,	CE_DRIVE_TYPE, CE_SPRING,	CE_FORCE_LIMIT,	CE_DRIVE_POSITION, CE_DRIVE_ORIENTATION, CE_DRIVE_LINEAR_VELOCITY, CE_DRIVE_ANGULAR_VELOCITY,	CE_GEAR_RATIO,
	//
	CE_IGNORE, CE_LAST
};

static const char	*CE_NAMES[CE_LAST] =
{
	"library_geometries",	"geometry",	"mesh",	"convex_mesh", "source", "float_array",	"int_array", "bool_array", "Name_array", "IDREF_array",	"technique_common",	"accessor",	"param", "vertices", "input",	"polygons",	"triangles", "polylist", "vcount", "p",	"library_visual_scenes", "visual_scene", "node", "matrix", "instance_geometry",	"translate", "rotate", "library_physics_materials",	"physics_material",	"dynamic_friction",	"restitution", "static_friction",	"library_physics_models",	"physics_model", "rigid_body", "instance_physics_material",	"shape", "box",	"half_extents",	"dynamic", "mass", "mass_frame", "inertia",	"library_physics_scenes",	"physics_scene", "instance_physics_model", "instance_rigid_body",	"velocity",	"angular_velocity",	"gravity", "time_step",	"scene", "instance_visual_scene",	"instance_physics_scene",	"density", "equation", "radius", "height", "capsule",	"plane", "sphere", "cylinder", "force_field",	"instance_force_field",	"library_force_fields",	"rigid_constraint",	"instance_rigid_constraint", "tapered_capsule",	"tapered_cylinder",	"radius1", "radius2",	"disable_collision", "min",	"max", "stiffness",	"damping", "target_value", "enabled",	"interpenetrate",	"ref_attachment",	"attachment",	"linear",	"swing_cone_and_twist",	"angular", "group",	"skinWidth",	"wakeupCounter", "linearDamping",	"angularDamping",	"maxAngularVelocity",	"sleepLinearVelocity", "sleepAngularVelocity", "solverIterationCount", "disable_gravity",	"kinematic", "pose_sleep_test",	"filter_sleep_velocity", "disable_response", "lock_com", "projectionMode", "projectionDistance", "projectionAngle",	"NxJointDriveDesc",	"driveType", "spring", "forceLimit", "drivePosition",	"driveOrientation",	"driveLinearVelocity", "driveAngularVelocity", "gearRatio",	"ignore"
};


class	C_InstancePhysicsModel;
class	C_Geometry;
class	C_Mesh;
class	C_Source;
class	C_FloatArray;
class	C_Accessor;
class	C_Param;
class	C_Vertices;
class	C_Input;
class	C_Polygons;
class	C_Polygon;
class	C_Scene;
class	C_VisualScene;
class	C_Node;
class	C_PhysicsMaterial;
class	C_PhysicsModel;
class	C_RigidBody;
class	C_PhysicsScene;
class	C_Shape;
class	C_LibraryPhysicsScenes;
class	C_LibraryPhysicsModels;
class	C_LibraryPhysicsMaterials;
class	C_LibraryVisualScenes;
class	C_LibraryGeometries;
class	C_RigidConstraint;
class	C_InstanceRigidConstraint;

static float getFloat(const	char *s)
{
		float	ret	=	0;
		if (s)
		{
				if (strcasecmp(s, "FLT_MIN")	== 0 ||	strcasecmp(s, "-INF") ==	0)
				{
						ret	=	FLT_MIN;
				}
				else if	(strcasecmp(s,	"FLT_MAX") ==	0	|| strcasecmp(s,	"INF") ==	0)
				{
						ret	=	FLT_MAX;
				}
				else
				{
						ret	=	(float)atof(s);
				}
		}
		return ret;
}

static const char	*getAttribute(TiXmlElement *element, const char	*v)
{
	const	char *ret	=	0;

	TiXmlAttribute *atr	=	element->FirstAttribute();
	while	(atr)
	{
		const	char *aname	=	atr->Name();
		const	char *avalue = atr->Value();
		if (strcmp(v,	aname) ==	0)
		{
			ret	=	avalue;
			break;
		}
		atr	=	atr->Next();
	}

	return ret;
}


class	C_Base
{
	public:
		C_Base(TiXmlElement	*element)
		{
				mId	=	getAttribute(element,	"id");
				mSid = getAttribute(element, "sid");
				mName	=	getAttribute(element,	"name");
				mUrl = getAttribute(element, "url");
				mCount = 0;
				const	char *c	=	getAttribute(element,	"count");
				if (c)
				{
						mCount = atoi(c);
				}
		}

		virtual ~C_Base() {}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_IGNORE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		const	char *mId;
		const	char *mSid;
		const	char *mName;
		const	char *mUrl;
		int	mCount;
};


/*
static const char	*TF(bool s)
{
	if (s)
	{
		return "true";
	}
	return "false";
}
*/

/*
static const char	*fstring(NxF32 v)
{
	static char	data[64	*16];
	static int index = 0;

	char *ret	=	&data[index	*64];
	index++;
	if (index	== 16)
	{
		index	=	0;
	}

	if (v	== 1)
	{
		strcpy(ret,	"1");
	}
	else if	(v ==	0)
	{
		strcpy(ret,	"0");
	}
	else if	(v ==	 - 1)
	{
		strcpy(ret,	"-1");
	}
	else
	{
		sprintf(ret, "%.9f", v);
		const	char *dot	=	strstr(ret,	".");
		if (dot)
		{
			int	len	=	(int)strlen(ret);
			char *foo	=	&ret[len - 1];
			while	(*foo	== '0')
			{
				foo--;
			}
			if (*foo ==	'.')
			{
				*foo = 0;
			}
			else
			{
				foo[1] = 0;
			}
		}
	}

	return ret;
}
*/

#define	WARN(x)	reportWarning(x)

static bool	colladaMatrix(const	char *m, NxMat34 &mat)
{
	bool ret = false;

	mat.id();

	float	matrix[16];

	void *mem	=	Asc2Bin(m, 16, "f",	matrix);

	if (mem)
	{
		float	m[9];

		m[0] = matrix[0];
		m[1] = matrix[1];
		m[2] = matrix[2];
		mat.t.x	=	matrix[3];

		m[3] = matrix[4];
		m[4] = matrix[5];
		m[5] = matrix[6];
		mat.t.y	=	matrix[7];

		m[6] = matrix[8];
		m[7] = matrix[9];
		m[8] = matrix[10];
		mat.t.z	=	matrix[11];

		mat.M.setRowMajor(m);


		ret	=	true;
	}
	return ret;
}

class	C_Query
{
	public:
		virtual ~C_Query() {}
		virtual	C_VisualScene	*locateVisualScene(const char	*str)	=	0;
		virtual	C_PhysicsScene *locatePhysicsScene(const char	*str)	=	0;
		virtual	C_PhysicsModel *locatePhysicsModel(const char	*str)	=	0;
		virtual	C_Node *locateNode(const char	*str)	=	0;
		virtual	C_Geometry *locateGeometry(const char	*str)	=	0;
		virtual	int	getMaterialIndex(const char	*mat)	=	0;
		virtual	NxConvexMeshDesc	*locateConvexMeshDesc(const	char *str) = 0;
		virtual	NxTriangleMeshDesc	*locateTriangleMeshDesc(const	char *str) = 0;
};

/*
static TiXmlNode *getElement(const char	*name, TiXmlNode *root)
{
	TiXmlNode	*ret = 0;

	TiXmlNode	*node	=	root->NextSibling();

	while	(node)
	{
		if (node->Type() ==	TiXmlNode::ELEMENT)
		{
			if (strcmp(node->Value(),	name)	== 0)
			{
				ret	=	node;
				break;
			}
		}

		if (node->NoChildren())
		{
			assert(node);
			while	(node->NextSibling() ==	NULL &&	node !=	root)
			{
				node = node->Parent();
			}
			if (node ==	root)
			{
				break;
			}
			assert(node);
			node = node->NextSibling();
		}
		else
		{
			assert(node);
			node = node->FirstChild();
		}

	}

	return ret;
}
*/

/*
static const char	*getElementText(TiXmlElement *element, const char	*name)
{
	const	char *ret	=	0;

	TiXmlNode	*node	=	getElement(name, element);
	if (node)
	{
		TiXmlNode	*child = node->FirstChild();
		if (child	&& child->Type() ==	TiXmlNode::TEXT)
		{
			ret	=	child->Value();
		}
	}

	return ret;
}
*/

class	C_TriangleMesh
{
	public:

		unsigned int vlookup(const NxVec3	&v)
		{
				const	float	EPSILON	=	0.0001f;
				unsigned int ret;
				for	(unsigned	int	i	=	0; i < mVertices.size(); i++)
				{
						const	NxVec3 &c	=	mVertices[i];
						float	dx = fabsf(c.x - v.x);
						if (dx < EPSILON)
						{
								float	dy = fabsf(c.y - v.y);
								if (dy < EPSILON)
								{
										float	dz = fabsf(c.z - v.z);
										if (dz < EPSILON)
										{
												float	d	=	dx * dx	+	dy * dy	+	dz * dz;
												if (d	<	(EPSILON *EPSILON))
												{
														return i;
												}
										}
								}
						}
				}

				ret	=	mVertices.size();

				mVertices.push_back(v);

				return ret;
		}

		void addTri(const	NxVec3 &v1,	const	NxVec3 &v2,	const	NxVec3 &v3)
		{
				unsigned int i1	=	vlookup(v1);
				unsigned int i2	=	vlookup(v2);
				unsigned int i3	=	vlookup(v3);
				mIndices.push_back(i1);
				mIndices.push_back(i2);
				mIndices.push_back(i3);
		}



		NxArray	<	NxVec3 > mVertices;
		NxArray	<	unsigned int > mIndices;
};

class	C_Spring
{
	public:

		C_Spring(void)
		{
				mStiffness = 1;
				mDamping = 0;
				mTargetValue = 0;
		}

		float	mStiffness;
		float	mDamping;
		float	mTargetValue;
};

enum JointDriveType
{
	JDT_POSITION,	JDT_VELOCITY,	JDT_POSITION_VELOCITY,
};

class	C_JointDrive
{
	public:
		C_JointDrive(void)
		{
				driveType	=	JDT_POSITION;
				spring = 0;
				damping	=	0;
				forceLimit = FLT_MAX;
		}

		void get(NxJointDriveDesc	&d)const
		{
				switch (driveType)
				{
						case JDT_POSITION:
							d.driveType	=	NX_D6JOINT_DRIVE_POSITION;
							break;
						case JDT_VELOCITY:
							d.driveType	=	NX_D6JOINT_DRIVE_VELOCITY;
							break;
						case JDT_POSITION_VELOCITY:
							d.driveType	=	NX_D6JOINT_DRIVE_POSITION_AND_VELOCITY;
							break;
				}

				d.spring = spring;
				d.damping	=	damping;
				d.forceLimit = forceLimit;
		}

		JointDriveType driveType;
		float	spring;
		float	damping;
		float	forceLimit;
};

class	C_RigidConstraint: C_Base
{
	public:
		C_RigidConstraint(TiXmlElement *element):	C_Base(element)
		{
				mRef = true;
				mLinear	=	true;

				mBody1 = 0;
				mBody2 = 0;
				mNode1 = 0;
				mNode2 = 0;
				mName1 = 0;
				mName2 = 0;
				mMatrix1.id();
				mMatrix2.id();
				mProjectionMode	=	0;
				mProjectionDistance	=	0;
				mProjectionAngle = 0;
				mDrive = 0;
				drivePosition.set(0, 0,	0);
				driveOrientation.setXYZW(0,	0, 0,	1);
				driveLinearVelocity.set(0, 0,	0);
				driveAngularVelocity.set(0,	0, 0);
				gearRatio	=	1;
		}

		virtual ~C_RigidConstraint()	{}

		void setJointDriveDesc(TiXmlElement	*element)
		{
				const	char *id = getAttribute(element, "id");
				if (id)
				{
						if (strcasecmp(id,	"xDrive")	== 0)
						{
								mDrive = &xDrive;
						}
						else if	(strcasecmp(id, "yDrive") ==	0)
						{
								mDrive = &yDrive;
						}
						else if	(strcasecmp(id, "zDrive") ==	0)
						{
								mDrive = &zDrive;
						}
						else if	(strcasecmp(id, "swingDrive") ==	0)
						{
								mDrive = &swingDrive;
						}
						else if	(strcasecmp(id, "twistDrive") ==	0)
						{
								mDrive = &twistDrive;
						}
						else if	(strcasecmp(id, "slerpDrive") ==	0)
						{
								mDrive = &slerpDrive;
						}
				}
				else
				{
						mDrive = 0;
				}
		}

		virtual	void setText(CELEMENT	operation, const char	*data)
		{
				switch (operation)
				{
						case CE_DRIVE_TYPE:
							if (mDrive)
							{
									if (strcasecmp(data,	"NX_D6JOINT_DRIVE_POSITION") ==	0)
									{
											mDrive->driveType	=	JDT_POSITION;
									}
									else if	(strcasecmp(data, "NX_D6JOINT_DRIVE_VELOCITY")	== 0)
									{
											mDrive->driveType	=	JDT_VELOCITY;
									}
									else if	(strcasecmp(data, "NX_D6JOINT_DRIVE_POSITION+NX_D6JONT_DRIVE_VELOCITY") ==	0)
									{
											mDrive->driveType	=	JDT_POSITION_VELOCITY;
									}
							}
							break;
						case CE_SPRING:
							if (mDrive)
							{
									mDrive->spring = getFloat(data);
							}
							break;
						case CE_FORCE_LIMIT:
							if (mDrive)
							{
									mDrive->forceLimit = getFloat(data);
							}
							break;
						case CE_DRIVE_POSITION:
							Asc2Bin(data,	3, "f",	&drivePosition.x);
							break;
						case CE_DRIVE_ORIENTATION:
							if (1)
							{
									float	quat[4]	=
									{
											0, 0,	0, 1
									}
									;
									Asc2Bin(data,	4, "f",	quat);
									driveOrientation.setXYZW(quat[0],	quat[1], quat[2],	quat[3]);
							}
							break;
						case CE_DRIVE_LINEAR_VELOCITY:
							Asc2Bin(data,	3, "f",	&driveLinearVelocity.x);
							break;
						case CE_DRIVE_ANGULAR_VELOCITY:
							Asc2Bin(data,	3, "f",	&driveAngularVelocity.x);
							break;
						case CE_GEAR_RATIO:
							gearRatio	=	getFloat(data);
							break;

						case CE_PROJECTION_MODE:
							mProjectionMode	=	data;
							break;
						case CE_PROJECTION_DISTANCE:
							mProjectionDistance	=	getFloat(data);
							break;
						case CE_PROJECTION_ANGLE:
							mProjectionAngle = getFloat(data);
							break;
						case CE_ENABLED:
							mEnabled = getTF(data);
							break;
						case CE_INTERPENETRATE:
							mInterpenetrate	=	getTF(data);
							break;
						case CE_TRANSLATE:
							if (1)
							{
									NxVec3 t(0,	0, 0);
									Asc2Bin(data,	3, "f",	&t.x);
									if (mRef)
									{
											mMatrix1.t +=	t;
									}
									else
									{
											mMatrix2.t +=	t;
									}
							}
							break;
						case CE_ROTATE:
							if (1)
							{
									float	aa[4]	= {1, 0, 0, 0};

									Asc2Bin(data,	4, "f",	aa);
									NxVec3 axis(aa[0], aa[1],	aa[2]);
									NxReal angle = aa[3];
									NxQuat q;
									q.fromAngleAxis(angle, axis);
									NxMat33	m(q);
									if (mRef)
									{
											mMatrix1.M *=	m;
									}
									else
									{
											mMatrix2.M *=	m;
									}
							}
							break;
						case CE_MATRIX:
							if (mRef)
							{
									colladaMatrix(data,	mMatrix1);
							}
							else
							{
									colladaMatrix(data,	mMatrix2);
							}
							break;
						case CE_MIN:
							if (mLinear)
							{
									Asc2Bin(data,	3, "f",	&mLinearMin.x);
							}
							else
							{
									Asc2Bin(data,	3, "f",	&mAngularMin.x);
							}
							break;
						case CE_MAX:
							if (mLinear)
							{
									Asc2Bin(data,	3, "f",	&mLinearMax.x);
							}
							else
							{
									Asc2Bin(data,	3, "f",	&mAngularMax.x);
							}
							break;
						case CE_STIFFNESS:
							if (mLinear)
							{
									mLinearSpring.mStiffness = getFloat(data);
							}
							else
							{
									mAngularSpring.mStiffness	=	getFloat(data);
							}
							break;
						case CE_DAMPING:
							if (mDrive)
							{
									mDrive->damping	=	getFloat(data);
							}
							else
							{
									if (mLinear)
									{
											mLinearSpring.mDamping = getFloat(data);
									}
									else
									{
											mAngularSpring.mDamping	=	getFloat(data);
									}
							}
							break;
						case CE_TARGET_VALUE:
							if (mLinear)
							{
									mLinearSpring.mTargetValue = getFloat(data);
							}
							else
							{
									mAngularSpring.mTargetValue	=	getFloat(data);
							}
							break;
						default:
							break;
				}
		}



		void setRefAttachment(TiXmlElement *element)
		{
				mBody1 = getAttribute(element, "rigid_body");
				mNode1 = getAttribute(element, "node");
				mName1 = getAttribute(element, "name");
				mRef = true;
		}

		void setAttachment(TiXmlElement	*element)
		{
				mBody2 = getAttribute(element, "rigid_body");
				mNode2 = getAttribute(element, "node");
				mName2 = getAttribute(element, "name");
				mRef = false;
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_RIGID_CONSTRAINT;
		};

		NxD6JointMotion	getMotion(float	a, float b)
		{
				NxD6JointMotion	ret;
				if (a	== 0 &&	b	== 0)
				{
						ret	=	NX_D6JOINT_MOTION_LOCKED;
				}
				else if	(a ==	FLT_MIN	&& b ==	FLT_MAX)
				{
						ret	=	NX_D6JOINT_MOTION_FREE;
				}
				else
				{
						ret	=	NX_D6JOINT_MOTION_LIMITED;
				}
				return ret;
		}

		float	getMeanRad(float a,	float	b)const
		{
				a	=	fabsf(a);
				b	=	fabsf(b);
				if (b	>	a)
				{
						a	=	b;
				}
				return a *DEG_TO_RAD;
		}

		void getJoint(NxSceneDesc *scene, C_Query *q, C_PhysicsModel	*pmodel);

		bool match(const char	*str)const
		{
				bool ret = false;
				if (mSid &&	str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(mSid, str)	== 0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		bool mRef;
		bool mLinear;

		const	char *mBody1;
		const	char *mBody2;

		const	char *mNode1;
		const	char *mNode2;

		const	char *mName1;
		const	char *mName2;

		bool mEnabled;
		bool mInterpenetrate;
		NxMat34	mMatrix1;
		NxMat34	mMatrix2;
		NxVec3 mLinearMin;
		NxVec3 mLinearMax;
		NxVec3 mAngularMin;
		NxVec3 mAngularMax;
		C_Spring mLinearSpring;
		C_Spring mAngularSpring;

		const	char *mProjectionMode;
		float	mProjectionDistance;
		float	mProjectionAngle;

		C_JointDrive *mDrive;	// current drive..

		C_JointDrive xDrive;
		C_JointDrive yDrive;
		C_JointDrive zDrive;
		C_JointDrive swingDrive;
		C_JointDrive twistDrive;
		C_JointDrive slerpDrive;
		NxVec3 drivePosition;
		NxQuat driveOrientation;
		NxVec3 driveLinearVelocity;
		NxVec3 driveAngularVelocity;
		float	gearRatio;

};

class	C_InstanceRigidConstraint: C_Base
{
	public:
		C_InstanceRigidConstraint(TiXmlElement *element):	C_Base(element)
		{
				mConstraint	=	getAttribute(element,	"constraint");
		}

		virtual ~C_InstanceRigidConstraint() {}

		virtual	void setText(CELEMENT	operation, const char	*data){}

		void getJoints(NxSceneDesc	*scene,	C_Query	*q,	C_PhysicsModel *pmodel);

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INSTANCE_RIGID_CONSTRAINT;
		};

		const	char *mConstraint;

};
enum ArrayType
{
	AT_BOOL, AT_INT, AT_FLOAT, AT_NAME,	AT_IDREF,	AT_LAST
};

class	C_Array: public	C_Base
{
	public:
		C_Array(TiXmlElement *element):	C_Base(element){}
		virtual ~C_Array() {}
		virtual	ArrayType	getArrayType(void)const	=	0;
		virtual	void setText(CELEMENT	operation, const char	*data) = 0;
};

// attributes:
//													 count	:	required
//															id	:	optional
//														 name	:	optional
class	C_BoolArray: C_Array
{
	public:
		C_BoolArray(TiXmlElement *element):	C_Array(element){}

		virtual ~C_BoolArray() 
		{
		}

		virtual	ArrayType	getArrayType(void)const
		{
				return AT_BOOL;
		};

		virtual	void setText(CELEMENT	operation, const char	*data){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_BOOL_ARRAY;
		};

};

// Attributes:
//								count		 (required)
//									 id		 (optional)
//								 name		 (optional)
//						minInclusive (optional)
//						maxInclusive (optional)
class	C_IntArray:	public C_Array
{
	public:
		C_IntArray(TiXmlElement	*element): C_Array(element)
		{
				mData	=	0;
				mMinInclusive	=	getAttribute(element,	"minInclusive");
				mMaxInclusive	=	getAttribute(element,	"maxInclusive");
		}

		virtual ~C_IntArray(void)
		{
				delete mData;
		}

		virtual	ArrayType	getArrayType(void)const
		{
				return AT_INT;
		};

		virtual	void setText(CELEMENT	operation, const char	*data)
		{
				if (mCount &&	!mData)
				{
						mData	=	(int*)Asc2Bin(data,	mCount,	"d", 0);
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INT_ARRAY;
		};

		const	char *mMinInclusive;
		const	char *mMaxInclusive;
		int	*mData;
};

// Attributes:
//									count		(required)
//									 name		(optional)
//										 id		(optional)
//									digits	(optional)
//								 magnitude (optional)
//
class	C_FloatArray:	public C_Array
{
	public:
		C_FloatArray(TiXmlElement	*element): C_Array(element)
		{
				mDigits	=	getAttribute(element,	"digits");
				mMagnitude = getAttribute(element, "magnitude");
				mData	=	0;
		}

		virtual ~C_FloatArray(void)
		{
				delete mData;
		}

		virtual	ArrayType	getArrayType(void)const
		{
				return AT_FLOAT;
		};

		virtual	void setText(CELEMENT	operation, const char	*data)
		{
				if (mCount &&	!mData)
				{
						mData	=	(float*)Asc2Bin(data,	mCount,	"f", 0);
				}
		}

		void getV(NxVec3 &v, unsigned	int	i)
		{
				v.set(0, 0,	0);
				assert(i < (unsigned int)(mCount / 3));
				if (i	<	(unsigned	int)(mCount	/	3))
				{
						v.x	=	mData[i	*3+0];
						v.y	=	mData[i	*3+1];
						v.z	=	mData[i	*3+2];
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_FLOAT_ARRAY;
		};

		const	char *mDigits;
		const	char *mMagnitude;
		float	*mData;
};

class	C_NameArray: public	C_Array
{
	public:
		C_NameArray(TiXmlElement *element):	C_Array(element){}

		virtual ~C_NameArray() 
		{
		}

		virtual	ArrayType	getArrayType(void)const
		{
				return AT_NAME;
		};

		virtual	void setText(CELEMENT	operation, const char	*data){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_NAME_ARRAY;
		};

};

class	C_IDREFArray:	public C_Array
{
	public:
		C_IDREFArray(TiXmlElement	*element): C_Array(element){}
		
		virtual ~C_IDREFArray() 
		{
		}

		virtual	ArrayType	getArrayType(void)const
		{
				return AT_IDREF;
		};

		virtual	void setText(CELEMENT	operation, const char	*data){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_IDREF_ARRAY;
		};

};

// attributes:
//														 name	 : optional
//															sid	 : optional	but	if present must	be unique
//														 type	 : required
//													sematnic : optional
//
class	C_Param: public	C_Base
{
	public:
		C_Param(TiXmlElement *element):	C_Base(element)
		{
				mType	=	getAttribute(element,	"type");
				mSemantic	=	getAttribute(element,	"semantic");
		}

		virtual ~C_Param() 
		{
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_PARAM;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		const	char *mType;
		const	char *mSemantic;

};

// attributes:
//														count		 : required
//														offset	 : optional	(default is	0)
//														source	 : required
//														stride	 : optional	(default is	1)
class	C_Accessor:	public C_Base
{
	public:
		C_Accessor(TiXmlElement	*element): C_Base(element)
		{
				mOffset	=	0;
				const	char *o	=	getAttribute(element,	"offset");
				if (o)
				{
						mOffset	=	atoi(o);
				}
				mSource	=	getAttribute(element,	"source");
				mStride	=	1;
				const	char *s	=	getAttribute(element,	"stride");
				if (s)
				{
						mStride	=	atoi(s);
				}
		}

		virtual ~C_Accessor(void)
		{
				for	(unsigned	int	i	=	0; i < mParams.size(); i++)
				{
						C_Param	*p = mParams[i];
						delete p;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_ACCESSOR;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		NxArray	<	C_Param	*	>	mParams;
		unsigned int mOffset;
		const	char *mSource;
		unsigned int mStride;

};

//	 attributes:
//													 id		:	required
//												 name		:	optional
class	C_Source:	public C_Base
{
	public:
		C_Source(TiXmlElement	*element): C_Base(element)
		{
				mAccessor	=	0;
				mArray = 0;
		}

		virtual ~C_Source() 
		{
		}

		bool match(const char	*str)
		{
				bool ret = false;
				if (str	&& mId)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(str,	mId) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_SOURCE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_Accessor *mAccessor;
		C_Array	*mArray; //	the	source array.
};

class	C_Input: public	C_Base
{
	public:
		C_Input(TiXmlElement *element):	C_Base(element)
		{
				mOffset	=	0;
				mSet = 0;
				const	char *o	=	getAttribute(element,	"offset");
				if (o)
				{
						mOffset	=	atoi(o);
				}
				mSemantic	=	getAttribute(element,	"semantic");
				mSource	=	getAttribute(element,	"source");
				const	char *s	=	getAttribute(element,	"set");
				if (s)
				{
						mSet = atoi(s);
				}
		}

		virtual ~C_Input() 
		{
		}

		bool match(const char	*str)
		{
				bool ret = false;

				if (str	&& mSemantic)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(str,	mSemantic) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INPUT;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		int	mOffset;
		const	char *mSemantic;
		const	char *mSource;
		int	mSet;

};

// id
// name
class	C_Vertices:	public C_Base
{
	public:
		C_Vertices(TiXmlElement	*element): C_Base(element){}

		virtual ~C_Vertices(void)
		{
				for	(unsigned	int	i	=	0; i < mInputs.size(); i++)
				{
						C_Input	*ip	=	mInputs[i];
						delete ip;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_VERTICES;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_Input	*locateInput(const char	*semantic)
		{
				C_Input	*ret = 0;
				for	(unsigned	int	i	=	0; i < mInputs.size(); i++)
				{
						C_Input	*ip	=	mInputs[i];
						if (ip->match(semantic))
						{
								ret	=	ip;
								break;
						}
				}
				return ret;
		}

		bool match(const char	*str)
		{
				bool ret = false;
				if (mId	&& str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(mId,	str) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		NxArray	<	C_Input	*	>	mInputs;
};


enum InputType
{
	IT_TRIANGLES,	IT_POLYGONS, IT_POLYLIST,	IT_LAST
};

// count,	material,	name
class	C_Triangles: public	C_Base
{
	public:
		C_Triangles(TiXmlElement *element, InputType type):	C_Base(element)
		{
				mInputType = type;
				mMaterial	=	getAttribute(element,	"material");
		}

		virtual ~C_Triangles(void)
		{
				for	(unsigned	int	i	=	0; i < mInputs.size(); i++)
				{
						C_Input	*ip	=	mInputs[i];
						delete ip;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_TRIANGLES;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void addVcount(const char	*c)
		{
				int	count;
				unsigned int *points = (unsigned int*)Asc2Bin(c, count,	"d");
				if (points)
				{
						for	(int i = 0;	i	<	count; i++)
						{
								mVcount.push_back(points[i]);
						}
						delete points;
				}
		}

		void addPoints(const char	*c)
		{
				int	count;
				unsigned int *points = (unsigned int*)Asc2Bin(c, count,	"d");
				if (points)
				{
						if (mInputType ==	IT_POLYGONS)
						{
								assert(count >=	3);
								mVcount.push_back(count);	// number	of points	int	he polygon.
						}
						else
						{
								assert((count	%	3) ==	0);
						}
						for	(int i = 0;	i	<	count; i++)
						{
								mIndices.push_back(points[i]);
						}
						delete points;
				}

		}

		unsigned int getIndex(unsigned int index,	const	unsigned int *indices, unsigned	int	offset,	unsigned int stride)
		{
				unsigned int base	=	stride * index;
				return indices[base	+	offset];
		}

		void addTri(unsigned int i1, unsigned	int	i2,	unsigned int i3, C_TriangleMesh	&t,	C_Source *vertices)
		{
				// hurrah!
				C_Accessor *accessor = vertices->mAccessor;
				C_Array	*array = vertices->mArray;
				if (array	&& array->getArrayType() ==	AT_FLOAT &&	accessor &&	accessor->mParams.size() ==	3	&& accessor->mStride ==	3)
				{

						unsigned int vcount	=	accessor->mCount;

						i1 +=	accessor->mOffset;
						i2 +=	accessor->mOffset;
						i3 +=	accessor->mOffset;

						assert(i1	<	vcount);
						assert(i2	<	vcount);
						assert(i3	<	vcount);

						if (i1 < vcount	&& i2	<	vcount &&	i3 < vcount)
						{
								C_FloatArray *farray = (C_FloatArray*)array;
								unsigned int fcount	=	farray->mCount / 3;
								assert(fcount	== vcount);
								if (fcount ==	vcount)
								{
										NxVec3 v1, v2, v3;

										farray->getV(v1, i1);
										farray->getV(v2, i2);
										farray->getV(v3, i3);

										t.addTri(v1, v2, v3);

								}
						}
				}

		}

		// we	have already computed	the	'position' source.
		// now we	iterate	through	the	indices
		void getTriangleMesh(C_TriangleMesh	&t,	C_Source *psource, C_Vertices	*vertices)
		{
				int	offset = 0;
				bool found = false;

				// things	get	really wacky here!
				for	(unsigned	int	i	=	0; i < mInputs.size(); i++)
				{
						C_Input	*ip	=	mInputs[i];
						if (ip->mSemantic	&& strcmp(ip->mSemantic, "VERTEX") ==	0)
						{
								if (vertices->match(ip->mSource))
								{
										offset = ip->mOffset;
										found	=	true;
										break;
								}
								else
								{
										// what	the	hell!??
								}
						}
				}

				if (found)
				// ok..	we found a mapping beetween	the
				{
						unsigned int icount	=	0;
						for	(unsigned	int	j	=	0; j < mInputs.size(); j++)
						{
								C_Input	*cp	=	mInputs[j];
								if ((unsigned	int)cp->mOffset	>	icount)
								{
										icount = cp->mOffset;
								}
						}

						icount++;


						unsigned int *istart = &mIndices[0];
						unsigned int *indices	=	istart;

						if (mInputType ==	IT_POLYGONS	|| mInputType	== IT_POLYLIST)
						{
								assert((unsigned int)mCount	== mVcount.size());
								if ((unsigned int)mCount > mVcount.size())
								{
										mCount = mVcount.size();
								}
						}

						for	(int i = 0;	i	<	mCount;	i++)
						{
								if (mInputType ==	IT_POLYGONS	|| mInputType	== IT_POLYLIST)
								{
										unsigned int pcount	=	mVcount[i];

										if (mInputType ==	IT_POLYGONS)
										{
												pcount = pcount	/	icount;
										}

										assert(pcount	>= 3);
										if (pcount < 3)
										{
												return ;
										}
										// something is	really screwed up!

										unsigned int i1	=	getIndex(0,	indices, offset, icount);
										unsigned int i2	=	getIndex(1,	indices, offset, icount);
										unsigned int i3	=	getIndex(2,	indices, offset, icount);

										addTri(i1, i2, i3, t,	psource);

										for	(unsigned	int	i	=	3; i < pcount; i++)
										{
												i2 = i3;
												i3 = getIndex(i, indices,	offset,	icount);
												addTri(i1, i2, i3, t,	psource);
										}
										indices	+= pcount	*	icount;
								}
								else
								{

										unsigned int i1	=	getIndex(0,	indices, offset, icount);
										unsigned int i2	=	getIndex(1,	indices, offset, icount);
										unsigned int i3	=	getIndex(2,	indices, offset, icount);

										addTri(i1, i2, i3, t,	psource);

										indices	+= 3 * icount;
								}

								unsigned int dist	=	(unsigned int)(indices	-	istart);

								assert(dist	<= mIndices.size());

								if (dist > mIndices.size())
								{
										break;
								}
						}
				}
		}

		InputType	mInputType;
		NxArray	<	C_Input	*	>	mInputs;
		const	char *mMaterial;
		NxArray	<	unsigned int > mIndices;
		NxArray	<	unsigned int > mVcount;
};


class	C_Mesh:	public C_Base
{
	public:

		C_Mesh(TiXmlElement	*element,	bool isConvex):	C_Base(element)
		{
				mConvex	=	isConvex;
				mVertices	=	0;
				mConvexHullOf	=	getAttribute(element,	"convex_hull_of");
		}

		virtual ~C_Mesh(void)
		{
				for	(unsigned	int	i	=	0; i < mSources.size();	i++)
				{
						C_Source *s	=	mSources[i];
						delete s;
				}
				for	(unsigned	int	i	=	0; i < mTriangles.size();	i++)
				{
						C_Triangles	*t = mTriangles[i];
						delete t;
				}
				delete mVertices;
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_MESH;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_Source *locateSource(const char	*str)
		{
				C_Source *ret	=	0;
				for	(unsigned	int	i	=	0; i < mSources.size();	i++)
				{
						C_Source *c	=	mSources[i];
						if (c->match(str))
						{
								ret	=	c;
								break;
						}
				}
				return ret;
		}

		void getTriangleMesh(C_TriangleMesh	&t,	C_Query	*q);

		bool isConvex(void)const
		{
				return mConvex;
		};

		const	char *mConvexHullOf;
		bool mConvex;	// true	if this	is a 'convex'	mesh.
		NxArray	<	C_Source * > mSources;
		C_Vertices *mVertices;
		NxArray	<	C_Triangles	*	>	mTriangles;
};

class	C_Geometry:	public C_Base
{
	public:
		C_Geometry(TiXmlElement	*element): C_Base(element)
		{
				mIndex = 0;	// 'array' index.
		};

		virtual ~C_Geometry(void)
		{
				for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
				{
						C_Mesh *m	=	mMeshes[i];
						delete m;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_GEOMETRY;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		bool match(const char	*str)
		{
				bool ret = false;
				if (str	&& mId)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(str,	mId) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		bool isConvex(void)
		{
				bool ret = true;
				for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
				{
						C_Mesh *m	=	mMeshes[i];
						if (!m->isConvex())
						{
								ret	=	false;
								break;
						}
				}
				return ret;
		}

		int	getIndex(void)const
		{
				return mIndex;
		};

		void getMesh(NxuPhysicsCollection	*c,	C_Query	*q)
		{
			C_TriangleMesh t;
			getTriangleMesh(t, q);

			if (isConvex())
			{
				HullDesc desc(QF_TRIANGLES,	t.mVertices.size(),	&t.mVertices[0].x, sizeof(NxVec3));
				desc.mMaxVertices	=	32;
				HullLibrary	hl;
				HullResult result;
				HullError	ok = hl.CreateConvexHull(desc, result);

				if (ok ==	QE_OK)
				{
					const	char *name = getGlobalString(mId); //	convert	it into	a	persistent string!
					NxConvexMeshDesc	*convex	=	new	NxConvexMeshDesc;
					convex->mId = name;

					for (NxU32 i=0; i<result.mNumOutputVertices; i++)
					{
						NxVec3 v( &result.mOutputVertices[i*3] );
						convex->mPoints.push_back(v);
					}
					for (NxU32 i=0; i<result.mNumFaces; i++)
					{
						unsigned int *t = &result.mIndices[i*3];
						NxTri tri(t[0],t[1],t[2]);
						convex->mTriangles.push_back(tri);
					}

					c->mConvexMeshes.push_back(convex);

					hl.ReleaseResult(result);
				}
			}
			else
			{
				const	char *name = getGlobalString(mId); //	convert	it into	a	persistent string!
				NxTriangleMeshDesc	*tmesh = new NxTriangleMeshDesc;
				tmesh->mId = name;


	      for (NxU32 i=0; i<t.mVertices.size(); i++)
	      {
	      	NxVec3 &v = t.mVertices[i];
	      	tmesh->mPoints.push_back(v);					
	      }

	      NxU32 tcount = t.mIndices.size()/3;
	      for (NxU32 i=0; i<tcount; i++)
	      {
	      	NxU32 *tt = &t.mIndices[i*3];
	      	NxTri tri(tt[0],tt[1],tt[2]);
	      	tmesh->mTriangles.push_back(tri);
	      }
				c->mTriangleMeshes.push_back(tmesh);
			}
		}

		void getTriangleMesh(C_TriangleMesh	&t,	C_Query	*q)
		{
				for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
				{
						C_Mesh *m	=	mMeshes[i];
						m->getTriangleMesh(t,	q);
				}
		}

		int	mIndex;
		NxArray	<	C_Mesh * > mMeshes;

};

enum C_ShapeType
{
	CST_PLANE, CST_BOX,	CST_SPHERE,	CST_CYLINDER,	CST_TAPERED_CYLINDER,	CST_CAPSULE, CST_TAPERED_CAPSULE,	CST_MESH,	CST_CONVEX_MESH, CST_UNKNOWN
};

class	C_Shape: public	C_Base
{
	public:
		C_Shape(TiXmlElement *element):	C_Base(element)
		{
				mHollow	=	false;
				mMass	=	1;
				mDensity = 0;
				mTransform.id();
				mInstancePhysicsMaterial = 0;
				mShapeType = CST_UNKNOWN;
				mGeometry	=	0;
				mHalfExtents.set(1,	1, 1);
				mRadius1 = 1;
				mRadius2 = 1;
				mHeight	=	1;
				mPlane.normal.set(0, 1,	0);
				mPlane.d = 0;
				//
				mGroup = 0;
				mSkinWidth = 0.005f; //	default
				mDisableCollision	=	false;
		}

		virtual ~C_Shape(void){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_SHAPE;
		};

		virtual	void setText(CELEMENT	operation, const char	*svalue)
		{
				switch (operation)
				{
						case CE_GROUP:
							if (svalue)
							{
									mGroup = atoi(svalue);
							}
							break;
						case CE_SKIN_WIDTH:
							mSkinWidth = getFloat(svalue);
							break;
						case CE_DISABLE_COLLISION:
							mDisableCollision	=	getTF(svalue);
							break;
						case CE_RADIUS:
							setRadius(svalue);
							break;
						case CE_HEIGHT:
							setHeight(svalue);
							break;
						case CE_MASS:
							setMass(svalue);
							break;
						case CE_DENSITY:
							setDensity(svalue);
							break;
						case CE_EQUATION:
							setEquation(svalue);
							break;
						case CE_TRANSLATE:
							if (1)
							{
									NxVec3 t(0,	0, 0);
									Asc2Bin(svalue,	3, "f",	&t.x);
									mTransform.t +=	t;
							}
							break;
						case CE_ROTATE:
							if (1)
							{
									float	aa[4]	=
									{
											1, 0,	0, 0
									}
									;
									Asc2Bin(svalue,	4, "f",	aa);
									NxVec3 axis(aa[0], aa[1],	aa[2]);
									NxReal angle = aa[3];
									NxQuat q;
									q.fromAngleAxis(angle, axis);
									NxMat33	m(q);
									mTransform.M *=	m;
							}
							break;
						case CE_MATRIX:
							setMatrix(svalue);
							break;
						case CE_HALF_EXTENTS:
							setHalfExtents(svalue);
							break;
						default:
							break;
				}
		}

		void setHalfExtents(const	char *v)
		{
				Asc2Bin(v, 3,	"f", &mHalfExtents.x);
		}

		void setEquation(const char	*v)
		{
				float	p[4];
				void *ok = Asc2Bin(v,	4, "f",	p);
				if (ok)
				{
						mPlane.normal.set(p[0],	p[1],	p[2]);
						mPlane.d = p[3];
				}
		}

		void setMatrix(const char	*m)
		{
				colladaMatrix(m, mTransform);
		}

		void getShape(NxActorDesc *a, C_Query *query)
		{
				if (mGeometry)
				{
						C_Geometry *g	=	query->locateGeometry(mGeometry);
						if (g	&& g->isConvex())
						{
								mShapeType = CST_CONVEX_MESH;
						}
						else
						{
								mShapeType = CST_MESH;
						}
				}

				NxShapeDesc	*s = 0;

				switch (mShapeType)
				{
						case CST_PLANE:
							if (1)
							{
									NxPlaneShapeDesc	*ps	=	new	NxPlaneShapeDesc;
									ps->normal = mPlane.normal;
									ps->d	=	mPlane.d;
									s	=	(NxShapeDesc*)ps;
							}
							break;
						case CST_BOX:
							if (1)
							{
									NxBoxShapeDesc	*bs	=	new	NxBoxShapeDesc;
									bs->dimensions = mHalfExtents;
									s	=	(NxShapeDesc*)bs;
							}
							break;
						case CST_SPHERE:
							if (1)
							{
									NxSphereShapeDesc *sd = new NxSphereShapeDesc;
									sd->radius = mRadius1;
									s	=	(NxShapeDesc*)sd;
							}
							break;
						case CST_CYLINDER:
						case CST_TAPERED_CYLINDER:
						case CST_CAPSULE:
						case CST_TAPERED_CAPSULE:
							if (1)
							{
									NxCapsuleShapeDesc	*cd	=	new	NxCapsuleShapeDesc;
									cd->radius = mRadius1;
									cd->height = mHeight;
									s	=	(NxShapeDesc*)cd;
							}
							break;
						case CST_MESH:
							if (1)
							{
									NxTriangleMeshDesc	*tmesh = query->locateTriangleMeshDesc(mGeometry);
									if (tmesh)
									{
											NxTriangleMeshShapeDesc *desc = new NxTriangleMeshShapeDesc;
											desc->mMeshData	=	tmesh->mId;
											s	=	(NxShapeDesc*)desc;
									}
							}
							break;
						case CST_CONVEX_MESH:
							if (1)
							{
									NxConvexMeshDesc	*convex	=	query->locateConvexMeshDesc(mGeometry);
									if (convex)
									{
											NxConvexShapeDesc *desc = new NxConvexShapeDesc;
											desc->mMeshData	=	convex->mId;
											s	=	(NxShapeDesc*)desc;
									}
							}
							break;
						default:
							break;
				}

				if (s)
				{
						s->group = mGroup;
						s->skinWidth = mSkinWidth;

						if (mDisableCollision)
						{
							s->shapeFlags = (NxShapeFlag) (s->shapeFlags | NX_SF_DISABLE_COLLISION);
						}

						s->localPose = mTransform;
						s->mShapeMass	=	mMass;
						s->mShapeDensity = mDensity;
						s->materialIndex = query->getMaterialIndex(mInstancePhysicsMaterial);
						a->mShapes.push_back(s);
				}

		}
		void setRadius(const char	*r)
		{
				float	rd[2];
				if (Asc2Bin(r, 2,	"f", rd))
				{
						mRadius1 = rd[0];
						mRadius2 = rd[1];
				}
				else
				{
						mRadius1 = mRadius2	=	getFloat(r);
				}
		}

		void setHeight(const char	*h)
		{
				mHeight	=	getFloat(h);
		}

		void setMass(const char	*m)
		{
				mMass	=	getFloat(m);
		}

		void setDensity(const	char *d)
		{
				mDensity = getFloat(d);
		}

		void setInstancedGeometry(const	char *str)
		{
				mGeometry	=	str;
		}

		bool mHollow;
		float	mMass;
		float	mDensity;
		NxMat34	mTransform;
		const	char *mInstancePhysicsMaterial;
		C_ShapeType	mShapeType;
		const	char *mGeometry;
		NxVec3 mHalfExtents;
		float	mRadius1;
		float	mRadius2;
		float	mHeight;
		NxPlane	mPlane;
		unsigned int mGroup;
		float	mSkinWidth;
		bool mDisableCollision;
};



class	C_RigidBody: public	C_Base
{
	public:
		C_RigidBody(TiXmlElement *element):	C_Base(element)
		{
				mDynamic = false;
				mMass	=	1;
				mMassFrame.id();
				mInertia.set(0,	0, 0);
				mInstancePhysicsMaterial = 0;
				mActorDesc = 0;
				mDisableCollision	=	false;
				mDisableResponse = false;
				mWakeupCounter = 20.0f * 0.02f;
				mLinearDamping = 0;
				mAngularDamping	=	0;
				mMaxAngularVelocity	=	 - 1;
				mSleepLinearVelocity =	-	1;
				mSleepAngularVelocity	=	 - 1;
				mSolverIterationCount	=	4;
				mKinematic = false;
				mPoseSleepTest = false;
				mFilterSleepVelocity = false;
				mGroup = 0;
				mDensity = 0;
				mLockCOM = false;
				mDisableGravity	=	false;
		}

		virtual ~C_RigidBody(void)
		{
				for	(unsigned	int	i	=	0; i < mShapes.size(); i++)
				{
						C_Shape	*s = mShapes[i];
						delete s;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_RIGID_BODY;
		};

		virtual	void setText(CELEMENT	operation, const char	*svalue)
		{
				switch (operation)
				{
						case CE_MASS:
							mMass	=	getFloat(svalue);
							break;
						case CE_DISABLE_GRAVITY:
							mDisableGravity	=	getTF(svalue);
							break;
						case CE_DISABLE_COLLISION:
							mDisableCollision	=	getTF(svalue);
							break;
						case CE_DISABLE_RESPONSE:
							mDisableResponse = getTF(svalue);
							break;
						case CE_WAKEUP_COUNTER:
							mWakeupCounter = getFloat(svalue);
							break;
						case CE_LINEAR_DAMPING:
							mLinearDamping = getFloat(svalue);
							break;
						case CE_ANGULAR_DAMPING:
							mAngularDamping	=	getFloat(svalue);
							break;
						case CE_MAX_ANGULAR_VELOCITY:
							mMaxAngularVelocity	=	getFloat(svalue);
							break;
						case CE_SLEEP_LINEAR_VELOCITY:
							mSleepLinearVelocity = getFloat(svalue);
							break;
						case CE_SLEEP_ANGULAR_VELOCITY:
							mSleepAngularVelocity	=	getFloat(svalue);
							break;
						case CE_SOLVER_ITERATION_COUNT:
							mSolverIterationCount	=	atoi(svalue);
							if (mSolverIterationCount	<	1)
							{
									mSolverIterationCount	=	1;
							}
							if (mSolverIterationCount	>	65536)
							{
									mSolverIterationCount	=	65536;
							}
							break;
						case CE_KINEMATIC:
							mKinematic = getTF(svalue);
							break;
						case CE_POSE_SLEEP_TEST:
							mPoseSleepTest = getTF(svalue);
							break;
						case CE_FILTER_SLEEP_VELOCITY:
							mFilterSleepVelocity = getTF(svalue);
							break;
						case CE_LOCK_COM:
							mLockCOM = getTF(svalue);
							break;
						case CE_GROUP:
							mGroup = atoi(svalue);
							break;
						case CE_DENSITY:
							mDensity = getFloat(svalue);
							break;
						case CE_DYNAMIC:
							setDynamic(svalue);
							break;
						case CE_INERTIA:
							setInertia(svalue);
							break;
						default:
							break;
				}
		}

		void setDynamic(const	char *v)
		{
				mDynamic = getTF(v);
		}

		void setInertia(const	char *v)
		{
				Asc2Bin(v, 3,	"f", &mInertia.x);
		}

		void setMassFrame(const	char *v)
		{
				colladaMatrix(v, mMassFrame);
		}

		void setMassFrameTranslate(const char	*v)
		{
				NxVec3 t(0,	0, 0);
				Asc2Bin(v, 3,	"f", &t.x);
				mMassFrame.t +=	t;
		}

		void setMassFrameRotate(const	char *v)
		{
				float	aa[4]	=
				{
						1, 0,	0, 0
				};
				Asc2Bin(v, 4,	"f", aa);
				NxReal angle = aa[3];
				NxVec3 axis(aa[0], aa[1],	aa[2]);
				NxQuat q(angle,	axis);
				NxMat33	m;
				m.fromQuat(q);
				mMassFrame.M *=	m;
		}

		void getActor(NxSceneDesc *scene, C_Query *q, const NxMat34 &mat, const NxVec3	&velocity, const NxVec3	&angularVelocity)
		{

				NxActorDesc *a	=	new	NxActorDesc;

				mActorDesc = a;

				a->globalPose	=	mat;
				a->name	=	getGlobalString(mName);
				a->mId  =	getGlobalString(mSid);

				if (mDisableCollision)
				{
					a->flags = (NxActorFlag) (a->flags|NX_AF_DISABLE_COLLISION);
				}

				if (mDynamic)
				{
						a->mHasBody = true;
						a->mBody.massLocalPose = mMassFrame;
						a->mBody.massSpaceInertia	=	mInertia;
						a->mBody.mass	=	mMass;
						a->mBody.linearVelocity	=	velocity;
						a->mBody.angularVelocity = angularVelocity;
						// extra fields
						a->mBody.wakeUpCounter = mWakeupCounter;
						a->mBody.linearDamping = mLinearDamping;
						a->mBody.angularDamping	=	mAngularDamping;
						a->mBody.maxAngularVelocity	=	mMaxAngularVelocity;
						a->mBody.sleepLinearVelocity = mSleepLinearVelocity;
						a->mBody.sleepAngularVelocity	=	mSleepAngularVelocity;
						a->mBody.solverIterationCount	=	mSolverIterationCount;
						if (mDisableGravity)
						{
								a->mBody.flags = (NxBodyFlag) (a->mBody.flags |	NX_BF_DISABLE_GRAVITY);
						}
						if (mKinematic)
						{
							a->mBody.flags = (NxBodyFlag) (a->mBody.flags |	NX_BF_KINEMATIC);
						}
#if NX_SDK_VERSION_NUMBER < 281
						if (mPoseSleepTest)
						{
							a->mBody.flags = (NxBodyFlag) (	a->mBody.flags |	NX_BF_POSE_SLEEP_TEST);
						}
#endif
						if (mFilterSleepVelocity)
						{
							a->mBody.flags = (NxBodyFlag) (	a->mBody.flags |	NX_BF_FILTER_SLEEP_VEL);
						}
				}
				else
				{
						a->mHasBody = false;
				}

				a->group = mGroup;

				if (mDisableCollision)
				{
					a->flags = (NxActorFlag) (a->flags |	NX_AF_DISABLE_COLLISION);
				}
				if (mDisableResponse)
				{
					a->flags = (NxActorFlag) (a->flags |	NX_AF_DISABLE_RESPONSE);
				}
				if (mLockCOM)
				{
					a->flags = (NxActorFlag) (a->flags |	NX_AF_LOCK_COM);
				}


				for	(unsigned	int	i	=	0; i < mShapes.size(); i++)
				{
						C_Shape	*s = mShapes[i];
						s->getShape(a, q);
				}

				scene->mActors.push_back(a);
		}

		bool match(const char	*str)
		{
				bool ret = false;
				if (str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (mSid &&	strcmp(mSid, str)	== 0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		NxActorDesc *mActorDesc;
		bool mDynamic;
		float	mMass;
		NxMat34	mMassFrame;
		NxVec3 mInertia;
		const	char *mInstancePhysicsMaterial;
		NxArray	<	C_Shape	*	>	mShapes;

		// PhysX specific	items.
		bool mDisableCollision;
		bool mDisableResponse;
		float	mWakeupCounter;
		float	mLinearDamping;
		float	mAngularDamping;
		float	mMaxAngularVelocity;
		float	mSleepLinearVelocity;
		float	mSleepAngularVelocity;
		unsigned int mSolverIterationCount;
		bool mKinematic;
		bool mPoseSleepTest;
		bool mFilterSleepVelocity;
		bool mLockCOM;
		bool mDisableGravity;
		unsigned int mGroup;
		float	mDensity;
};

class	C_DisableCollision:	public C_Base
{
	public:
		C_DisableCollision(TiXmlElement	*element): C_Base(element)
		{
				mBody1 = getAttribute(element, "body1");
				mBody2 = getAttribute(element, "body2");
		}

		virtual ~C_DisableCollision() 
		{
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_DISABLE_COLLISION;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void getBodyPairFlag(NxSceneDesc	*scene,	C_Query	*query,	C_PhysicsModel *pmodel);

		const	char *mBody1;
		const	char *mBody2;
};

class	C_PhysicsModel:	public C_Base
{
	public:
		C_PhysicsModel(TiXmlElement	*element): C_Base(element){}

		virtual ~C_PhysicsModel(void);

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_PHYSICS_MODEL;
		};

		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;


		C_RigidBody	*locateRigidBody(const char	*str)
		{
				C_RigidBody	*ret = 0;

				if (str)
				{
						for	(unsigned	int	i	=	0; i < mRigidBodies.size();	i++)
						{
								C_RigidBody	*rb	=	mRigidBodies[i];
								if (rb->match(str))
								{
										ret	=	rb;
										break;
								}
						}
				}
				return ret;
		}

		C_RigidConstraint	*locateRigidConstraint(const char	*str)
		{
				C_RigidConstraint	*ret = 0;
				if (str)
				{
						for	(unsigned	int	i	=	0; i < mRigidConstraints.size(); i++)
						{
								C_RigidConstraint	*rc	=	mRigidConstraints[i];
								if (rc->match(str))
								{
										ret	=	rc;
										break;
								}
						}
				}
				return ret;
		}

		void getRigidBodyElements(NxSceneDesc *scene, C_Query *query, const NxMat34 &parent);

		NxArray	<	C_RigidBody	*	>	mRigidBodies;
		NxArray	<	C_RigidConstraint	*	>	mRigidConstraints;
		NxArray	<	C_InstancePhysicsModel * > mInstancePhysicsModels;
};

class	C_LibraryPhysicsModels:	public C_Base
{
	public:
		C_LibraryPhysicsModels(TiXmlElement	*element): C_Base(element){}

		virtual ~C_LibraryPhysicsModels(void)
		{
				for	(unsigned	int	i	=	0; i < mPhysicsModels.size();	i++)
				{
						C_PhysicsModel *m	=	mPhysicsModels[i];
						delete m;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_LIBRARY_PHYSICS_MODELS;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_PhysicsModel *locatePhysicsModel(const char	*str)
		{
				C_PhysicsModel *ret	=	0;
				if (str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						for	(unsigned	int	i	=	0; i < mPhysicsModels.size();	i++)
						{
								C_PhysicsModel *pm = mPhysicsModels[i];
								if (pm->mId	&& strcmp(pm->mId, str)	== 0)
								{
										ret	=	pm;
										break;
								}
						}
				}
				return ret;
		}

		NxArray	<	C_PhysicsModel * > mPhysicsModels;
};


// Note..needs to	support	overrides!
class	C_InstanceRigidBody: public	C_Base
{
	public:

		C_InstanceRigidBody(TiXmlElement *element):	C_Base(element)
		{
				mBody	=	getAttribute(element,	"body");
				mTarget	=	getAttribute(element,	"target");
				mAngularVelocity.set(0,	0, 0);
				mVelocity.set(0, 0,	0);
		}

		virtual ~C_InstanceRigidBody(void){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INSTANCE_RIGID_BODY;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void setVelocity(const char	*v)
		{
				Asc2Bin(v, 3,	"f", &mVelocity.x);
		}

		void setAngularVelocity(const	char *v)
		{
				Asc2Bin(v, 3,	"f", &mAngularVelocity.x);
		}

		void getActors(NxSceneDesc	*scene,	C_Query	*q,	C_PhysicsModel *pmodel,	const	NxMat34	&parent);

		const	char *mBody;
		const	char *mTarget;
		NxVec3 mAngularVelocity;
		NxVec3 mVelocity;
};

class	C_InstancePhysicsModel:	public C_Base
{
	public:
		C_InstancePhysicsModel(TiXmlElement	*element): C_Base(element)
		{
				mParent	=	getAttribute(element,	"parent");
		}

		virtual ~C_InstancePhysicsModel(void)
		{
				for	(unsigned	int	i	=	0; i < mInstanceRigidBodies.size();	i++)
				{
						C_InstanceRigidBody	*rb	=	mInstanceRigidBodies[i];
						delete rb;
				}
				for	(unsigned	int	i	=	0; i < mInstanceRigidConstraints.size(); i++)
				{
						C_InstanceRigidConstraint	*irc = mInstanceRigidConstraints[i];
						delete irc;
				}
				for	(unsigned	int	i	=	0; i < mDisableCollisions.size();	i++)
				{
						C_DisableCollision *dc = mDisableCollisions[i];
						delete dc;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INSTANCE_PHYSICS_MODEL;
		};

		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void getRigidBodyElements(NxSceneDesc *scene, C_Query *query, const NxMat34 &root);

		const	char *mParent;
		NxArray	<	C_InstanceRigidBody	*	>	mInstanceRigidBodies;
		NxArray	<	C_InstanceRigidConstraint	*	>	mInstanceRigidConstraints;
		NxArray	<	C_DisableCollision * > mDisableCollisions;

};

class	C_PhysicsScene:	public C_Base
{
	public:
		C_PhysicsScene(TiXmlElement	*element): C_Base(element)
		{
				mTimeStep	=	0;
				mGravity.set(0,	 - 9.8f, 0);
		}

		virtual ~C_PhysicsScene(void)
		{
				for	(unsigned	int	i	=	0; i < mInstancePhysicsModels.size();	i++)
				{
						C_InstancePhysicsModel *ip = mInstancePhysicsModels[i];
						delete ip;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_PHYSICS_SCENE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;


		void getRigidBodyElements(NxSceneDesc *scene, C_Query *query)
		{
				NxMat34	root;
				root.id();
				for	(unsigned	int	i	=	0; i < mInstancePhysicsModels.size();	i++)
				{
						C_InstancePhysicsModel *ipm	=	mInstancePhysicsModels[i];
						ipm->getRigidBodyElements(scene, query,	root);
				}
		}

		bool match(const char	*str)
		{
				bool ret = false;
				if (str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (mId	&& strcmp(mId, str)	== 0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		void setGravity(const	char *g)
		{
				Asc2Bin(g, 3,	"f", &mGravity.x);
		}

		void setTimeStep(const char	*t)
		{
				mTimeStep	=	getFloat(t);
		}

		NxVec3 mGravity;
		float	mTimeStep;

		NxArray	<	C_InstancePhysicsModel * > mInstancePhysicsModels;
};

class	C_LibraryPhysicsScenes:	public C_Base
{
	public:
		C_LibraryPhysicsScenes(TiXmlElement	*element): C_Base(element){}

		virtual ~C_LibraryPhysicsScenes(void)
		{
				for	(unsigned	int	i	=	0; i < mPhysicsScenes.size();	i++)
				{
						C_PhysicsScene *cp = mPhysicsScenes[i];
						delete cp;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_LIBRARY_PHYSICS_SCENES;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;


		C_PhysicsScene *locatePhysicsScene(const char	*str)
		{
				C_PhysicsScene *ret	=	0;
				for	(unsigned	int	i	=	0; i < mPhysicsScenes.size();	i++)
				{
						C_PhysicsScene *cp = mPhysicsScenes[i];
						if (cp->match(str))
						{
								ret	=	cp;
								break;
						}
				}
				return ret;
		}

		NxArray	<	C_PhysicsScene * > mPhysicsScenes;
};

class	C_LibraryGeometries: public	C_Base
{
	public:

		C_LibraryGeometries(TiXmlElement *element):	C_Base(element){}

		virtual ~C_LibraryGeometries(void)
		{
				for	(unsigned	int	i	=	0; i < mGeometries.size(); i++)
				{
						C_Geometry *c	=	mGeometries[i];
						delete c;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_LIBRARY_GEOMETRIES;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_Geometry *locateGeometry(const char	*str)
		{
				C_Geometry *ret	=	0;
				for	(unsigned	int	i	=	0; i < mGeometries.size(); i++)
				{
						C_Geometry *g	=	mGeometries[i];
						if (g->match(str))
						{
								ret	=	g;
								break;
						}
				}
				return ret;
		}

		NxArray	<	C_Geometry * > mGeometries;
};

class	C_Node:	public C_Base
{
	public:

		C_Node(TiXmlElement	*element): C_Base(element)
		{
				mType	=	getAttribute(element,	"type");
				mLayer = getAttribute(element, "layer");
				mTransform.id();
				mInstanceGeometry	=	0;
		}
		
		virtual ~C_Node() 
		{
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_NODE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		bool setMatrix(const char	*v)	// sets	the	matrix transform from	this raw source	text.
		{
				return colladaMatrix(v,	mTransform);
		}

		void translate(const char	*v)
		{
				NxVec3 t;

				if (Asc2Bin(v, 3,	"f", &t.x))
				{
						mTransform.t +=	t; //	accumulate the translation
				}
		}

		void rotate(const	char *r)
		{
				float	aa[4];

				if (Asc2Bin(r, 4,	"f", aa))
				{
						NxVec3 axis(aa[0], aa[1],	aa[2]);
						NxReal angle = aa[3];
						NxQuat q(angle,	axis);
						NxMat33	m;
						m.fromQuat(q);
						mTransform.M *=	m;
				}

		}

		void setInstanceGeometry(const char	*geom)
		{
				mInstanceGeometry	=	geom;
		}

		bool match(const char	*str)
		{
				bool ret = false;
				if (str	&& mId)
				//&& mType &&	strcmp(mType,"NODE") ==	0	)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(str,	mId) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		const	char *mLayer;
		const	char *mType;
		NxMat34	mTransform;
		const	char *mInstanceGeometry;
};

class	C_VisualScene: public	C_Base
{
	public:
		C_VisualScene(TiXmlElement *element):	C_Base(element){}

		virtual ~C_VisualScene(void)
		{
				for	(unsigned	int	i	=	0; i < mNodes.size();	i++)
				{
						C_Node *n	=	mNodes[i];
						delete n;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_VISUAL_SCENE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		bool match(const char	*str)
		{
				bool ret = false;
				if (str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (mId	&& strcmp(str, mId)	== 0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		C_Node *locateNode(const char	*str)
		{
				C_Node *ret	=	0;
				for	(unsigned	int	i	=	0; i < mNodes.size();	i++)
				{
						C_Node *n	=	mNodes[i];
						if (n->match(str))
						{
								ret	=	n;
								break;
						}
				}
				return ret;
		}

		NxArray	<	C_Node * > mNodes;
};

class	C_LibraryVisualScenes: public	C_Base
{
	public:
		C_LibraryVisualScenes(TiXmlElement *element):	C_Base(element){}

		virtual ~C_LibraryVisualScenes(void)
		{
				for	(unsigned	int	i	=	0; i < mVisualScenes.size(); i++)
				{
						C_VisualScene	*s = mVisualScenes[i];
						delete s;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_LIBRARY_VISUAL_SCENES;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		C_VisualScene	*locateVisualScene(const char	*str)
		{
				C_VisualScene	*ret = 0;
				for	(unsigned	int	i	=	0; i < mVisualScenes.size(); i++)
				{
						C_VisualScene	*vs	=	mVisualScenes[i];
						if (vs->match(str))
						{
								ret	=	vs;
								break;
						}
				}
				return ret;
		}


		C_Node *locateNode(const char	*str)
		{
				C_Node *ret	=	0;
				for	(unsigned	int	i	=	0; i < mVisualScenes.size(); i++)
				{
						C_VisualScene	*vs	=	mVisualScenes[i];
						ret	=	vs->locateNode(str);
						if (ret)
						{
								break;
						}
				}
				return ret;
		}

		NxArray	<	C_VisualScene	*	>	mVisualScenes;
};


class	C_PhysicsMaterial: public	C_Base
{
	public:
		C_PhysicsMaterial(TiXmlElement *element, int index): C_Base(element)
		{
				mIndex = index;
				mDynamicFriction = 0.5f;
				mRestitution = 0;
				mStaticFriction	=	0.5f;
		}

		virtual ~C_PhysicsMaterial() 
		{
		}

		NxMaterialDesc	*getMaterialDesc(void)
		{
				NxMaterialDesc	*d = new NxMaterialDesc;
				d->mId = getGlobalString(mId);
				d->mMaterialIndex = mIndex;
				d->dynamicFriction = mDynamicFriction;
				d->restitution = mRestitution;
				d->staticFriction	=	mStaticFriction;
				return d;
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_PHYSICS_MATERIAL;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		bool match(const char	*str)
		{
				bool ret = false;
				if (str	&& mId)
				{
						if (*str ==	'#')
						{
								str++;
						}
						if (strcmp(str,	mId) ==	0)
						{
								ret	=	true;
						}
				}
				return ret;
		}

		int	mIndex;
		float	mDynamicFriction;
		float	mRestitution;
		float	mStaticFriction;
};

class	C_LibraryPhysicsMaterials: public	C_Base
{
	public:

		C_LibraryPhysicsMaterials(TiXmlElement *element):	C_Base(element){}

		virtual ~C_LibraryPhysicsMaterials(void)
		{
				for	(unsigned	int	i	=	0; i < mPhysicsMaterials.size(); i++)
				{
						C_PhysicsMaterial	*m = mPhysicsMaterials[i];
						delete m;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_LIBRARY_PHYSICS_MATERIALS;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void getMaterials(NxSceneDesc *scene)
		{
				for	(unsigned	int	i	=	0; i < mPhysicsMaterials.size(); ++i)
				{
						C_PhysicsMaterial	*p = mPhysicsMaterials[i];
						NxMaterialDesc	*md	=	p->getMaterialDesc();
						scene->mMaterials.push_back(md);
				}
		}

		int	getMaterialIndex(const char	*mat)
		{
				int	ret	=	0;
				for	(unsigned	int	i	=	0; i < mPhysicsMaterials.size(); i++)
				{
						C_PhysicsMaterial	*m = mPhysicsMaterials[i];
						if (m->match(mat))
						{
								ret	=	i;
								break;
						}
				}
				return ret;
		}

		NxArray	<	C_PhysicsMaterial	*	>	mPhysicsMaterials;
};

class	C_InstancePhysicsScene:	public C_Base
{
	public:
		C_InstancePhysicsScene(TiXmlElement	*element): C_Base(element){}

		virtual ~C_InstancePhysicsScene(void){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INSTANCE_PHYSICS_SCENE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void getRigidBodyElements(NxSceneDesc *scene, C_Query *query)
		{
				if (mUrl)
				{
						C_PhysicsScene *ps = query->locatePhysicsScene(mUrl);
						if (ps)
						{
								ps->getRigidBodyElements(scene,	query);
						}
				}
		}

};

class	C_InstanceVisualScene: public	C_Base
{
	public:
		C_InstanceVisualScene(TiXmlElement *element):	C_Base(element){}

		virtual ~C_InstanceVisualScene(void){}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_INSTANCE_VISUAL_SCENE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

};

class	C_Scene: public	C_Base
{
	public:
		C_Scene(TiXmlElement *element):	C_Base(element){}

		virtual ~C_Scene(void)
		{
				for	(unsigned	int	i	=	0; i < mInstanceVisualScenes.size(); i++)
				{
						C_InstanceVisualScene	*iv	=	mInstanceVisualScenes[i];
						delete iv;
				}
				for	(unsigned	int	i	=	0; i < mInstancePhysicsScenes.size();	i++)
				{
						C_InstancePhysicsScene *ip = mInstancePhysicsScenes[i];
						delete ip;
				}
		}

		virtual	CELEMENT getBaseType(void)const
		{
				return CE_SCENE;
		};
		virtual	void setText(CELEMENT	operation, const char	*txt){}
		;

		void getRigidBodyElements(NxSceneDesc *scene, C_Query *query)
		{
				for	(unsigned	int	i	=	0; i < mInstancePhysicsScenes.size();	i++)
				{
						C_InstancePhysicsScene *ip = mInstancePhysicsScenes[i];
						ip->getRigidBodyElements(scene,	query);
				}
		}

		NxArray	<	C_InstanceVisualScene	*	>	mInstanceVisualScenes;
		NxArray	<	C_InstancePhysicsScene * > mInstancePhysicsScenes;
};

class	StackEntry
{
	public:
		StackEntry(void)
		{
				mNode	=	0;
				mElement = CE_IGNORE;
		}

		TiXmlNode	*mNode;
		CELEMENT mElement;
};


class	ColladaImport: public	C_Query
{
	public:
		ColladaImport(const	char *fname,void *mem,int len)
		{
				mStackPtr	=	0;
				mCurrent = CE_IGNORE;
				mOperation = CE_IGNORE;

				mScene = 0;
				mLibraryGeometries = 0;
				mLibraryVisualScenes = 0;
				mLibraryPhysicsMaterials = 0;
				mLibraryPhysicsModels	=	0;
				mLibraryPhysicsScenes	=	0;


				mGeometry	=	0;
				mVertices	=	0;
				mTriangles = 0;
				mMesh	=	0;
				mSource	=	0;
				mArray = 0;
				mAccessor	=	0;
				mParam = 0;
				mVisualScene = 0;
				mNode	=	0;
				mPhysicsMaterial = 0;
				mPhysicsModel	=	0;
				mRigidBody = 0;
				mShape = 0;
				mInstancePhysicsModel	=	0;
				mInstanceRigidBody = 0;
				mInstanceVisualScene = 0;
				mInstancePhysicsScene	=	0;
				mDisableCollision	=	0;
				mPhysicsScene	=	0;
				mInstanceCount = 0;
				mBodyPairCount = 0;
				mJointCount	=	0;
				mInstanceRigidConstraint = 0;
				mRigidConstraint = 0;

				mXML = new TiXmlDocument;
				mCollection	=	0;
				mFph = 0;

				mConvexCount = 0;
				mMeshCount = 0;

				if (mXML->LoadFile(fname,mem,len))
				{
						#if	0	// only	for	debugging	help
						mFph = nxu_fopen("collada.txt",	"wb");
						#endif

						mCurrent = CE_IGNORE;
						traverse(mXML, 0);

						// ok..now we	have to	save out triangle	meshes and convex	hulls!
						for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
						{
								const	char *geom = mMeshes[i];
								C_Geometry *g	=	locateGeometry(geom);
								if (g)
								{
										if (g->isConvex())
										{
												g->mIndex	=	mConvexCount;
												mConvexCount++;
										}
										else
										{
												g->mIndex	=	mMeshCount;
												mMeshCount++;
										}
								}
						}

						if (mFph)
						{
								nxu_fclose(mFph);
						}

						createCollection(fname);

				}
				else
				{
					reportError("Error	parsing	file '%s'	as XML", fname);
					reportError("XML	parse	error(%d)	on Row:	%d Column: %d",	mXML->ErrorId(), mXML->ErrorRow(), mXML->ErrorCol());
					reportError("XML	error	description: \"%s\"",	mXML->ErrorDesc());
					delete mXML;
					mXML = 0;
				}



		}

		virtual ~ColladaImport(void)
		{
				if (mFph)
				{
						nxu_fclose(mFph);
				}

				delete mScene;
				delete mLibraryGeometries;
				delete mLibraryVisualScenes;
				delete mLibraryPhysicsMaterials;
				delete mLibraryPhysicsModels;
				delete mLibraryPhysicsScenes;

				delete mXML;
				delete mCollection;

		}

		CELEMENT getELEMENT(const	char *str)
		{
				CELEMENT ret = CE_IGNORE;

				for	(int i = 0;	i	<	CE_LAST; i++)
				{
						if (strcmp(CE_NAMES[i],	str) ==	0)
						{
								ret	=	(CELEMENT)i;
								break;
						}
				}

				return ret;
		}


		// here	we will	convert	the	COLLADA	physics	data into	an NxuStream compatible	PhysicsCollection
		void createCollection(const	char *fname)
		{
				delete mCollection;

				mCollection	=	new	NxuPhysicsCollection;
				mCollection->mId = getGlobalString(fname);

				// write out the convex	hulls
				if (mConvexCount)
				{
						for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
						{
								const	char *geom = mMeshes[i];
								C_Geometry *g	=	locateGeometry(geom);
								if (g)
								{
										if (g->isConvex())
										{
												g->getMesh(mCollection,	this);
										}
								}
						}
				}

				if (mMeshCount)
				{
						for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
						{
								const	char *geom = mMeshes[i];
								C_Geometry *g	=	locateGeometry(geom);
								if (g)
								{
										if (!g->isConvex())
										{
												g->getMesh(mCollection,	this);
										}
								}
						}
				}

				NxSceneDesc *scenedesc	=	new	NxSceneDesc;
				scenedesc->mId = "collada_scene";

				if (mLibraryPhysicsScenes)
				{
						if (mLibraryPhysicsScenes->mPhysicsScenes.size())
						{
								C_PhysicsScene *p	=	mLibraryPhysicsScenes->mPhysicsScenes[0];
								scenedesc->gravity = p->mGravity;
						}
				}

				mCollection->mScenes.push_back(scenedesc);

				NxSceneDesc *scene	=	scenedesc;

				if (mLibraryPhysicsMaterials)
				{
						mLibraryPhysicsMaterials->getMaterials(scene);
				}
				if (mScene)
				{
						mScene->getRigidBodyElements(scene,	this);
				}

		}

		NxuPhysicsCollection *getCollection(void)const
		{
				return mCollection;
		};
		
		void setCollection(NxuPhysicsCollection	*c)
		{
				mCollection	=	c;
		};



		void Display(int depth,	const	char *fmt, ...)
		{
				if (mFph)
				{
						for	(int i = 0;	i	<	depth; i++)
						{
								nxu_fprintf(mFph,	"	 ");
						}
						char wbuff[8192];
#if defined(__PPCGEKKO__) || defined(LINUX)
						va_list va;
						va_start(va, fmt);
							vsprintf(wbuff,	fmt, va);
						va_end(va);
#else						
						vsprintf(wbuff,	fmt, (char*)(&fmt	+	1));
#endif						
						nxu_fprintf(mFph,	"%s",	wbuff);
				}
		}

		void ProcessElement(CELEMENT etype,	TiXmlElement *element, int depth)
		{
				switch (etype)
				{
						case CE_SCENE:
							if (mScene)
							{
									WARN("Encountered	unexpected second	'scene'	element	in the document.");
							}
							else
							{
									mScene = new C_Scene(element);
							}
							break;
						case CE_LIBRARY_GEOMETRIES:
							if (mLibraryGeometries)
							{
									WARN("Encountered	unexpected library_geometries	in the document.");
							}
							else
							{
									mLibraryGeometries = new C_LibraryGeometries(element);
							}
							break;
						case CE_LIBRARY_PHYSICS_SCENES:
							if (mLibraryPhysicsScenes)
							{
									WARN("Encountered	unexpected second	'library_physics_scenes' element in	the	document.");
							}
							else
							{
									mLibraryPhysicsScenes	=	new	C_LibraryPhysicsScenes(element);
							}
							break;
						case CE_LIBRARY_VISUAL_SCENES:
							if (mLibraryVisualScenes)
							{
									WARN("Encountered	unexpected second	'library_visual_scenes'	element	in the document.");
							}
							else
							{
									mLibraryVisualScenes = new C_LibraryVisualScenes(element);
							}
							break;
						case CE_LIBRARY_PHYSICS_MATERIALS:
							if (mLibraryPhysicsMaterials)
							{
									WARN("Encountered	unexpected second	'library_physics_materials'	element	in the document.");
							}
							else
							{
									mLibraryPhysicsMaterials = new C_LibraryPhysicsMaterials(element);
							}
							break;
						case CE_LIBRARY_PHYSICS_MODELS:
							if (mLibraryPhysicsModels)
							{
									WARN("Encountered	unexpected second	'library_physics_models' element in	the	document.");
							}
							else
							{
									mLibraryPhysicsModels	=	new	C_LibraryPhysicsModels(element);
							}
							break;
						case CE_GEOMETRY:
							if (mLibraryGeometries)
							{
									mGeometry	=	new	C_Geometry(element);
									mLibraryGeometries->mGeometries.push_back(mGeometry);
							}
							else
							{
									WARN("Encountered	'geometry' but not inside	a	geometries library!?");
							}
							break;
						case CE_CONVEX_MESH:
							if (mGeometry)
							{
									mMesh	=	new	C_Mesh(element,	true);
									mGeometry->mMeshes.push_back(mMesh);
							}
							break;
						case CE_MESH:
							if (mGeometry)
							{
									mMesh	=	new	C_Mesh(element,	false);
									mGeometry->mMeshes.push_back(mMesh);
							}
							break;
						case CE_SOURCE:
							if (mMesh)
							{
									mSource	=	new	C_Source(element);
									mMesh->mSources.push_back(mSource);
							}
							break;
						case CE_FLOAT_ARRAY:
							if (mSource)
							{
									if (mSource->mArray	== 0)
									{
											C_FloatArray *a	=	new	C_FloatArray(element);
											mArray = (C_Array*)a;
											mSource->mArray	=	mArray;
											mOperation = etype;
									}
							}
							break;
						case CE_INT_ARRAY:
							if (mSource)
							{
									if (mSource->mArray	== 0)
									{
											C_IntArray *a	=	new	C_IntArray(element);
											mArray = (C_Array*)a;
											mSource->mArray	=	mArray;
											mOperation = etype;
									}
							}
							break;
						case CE_BOOL_ARRAY:
							if (mSource)
							{
									if (mSource->mArray	== 0)
									{
											C_BoolArray	*a = new C_BoolArray(element);
											mArray = (C_Array*)a;
											mSource->mArray	=	mArray;
											mOperation = etype;
									}
							}
							break;
						case CE_NAME_ARRAY:
							if (mSource)
							{
									if (mSource->mArray	== 0)
									{
											C_NameArray	*a = new C_NameArray(element);
											mArray = (C_Array*)a;
											mSource->mArray	=	mArray;
											mOperation = etype;
									}
							}
							break;
						case CE_IDREF_ARRAY:
							if (mSource)
							{
									if (mSource->mArray	== 0)
									{
											C_IDREFArray *a	=	new	C_IDREFArray(element);
											mArray = (C_Array*)a;
											mSource->mArray	=	mArray;
											mOperation = etype;
									}
							}
							break;
						case CE_ACCESSOR:
							if (mSource)
							{
									if (mSource->mAccessor ==	0)
									{
											mAccessor	=	new	C_Accessor(element);
											mSource->mAccessor = mAccessor;
									}
							}
							break;
						case CE_PARAM:
							if (mAccessor)
							{
									mParam = new C_Param(element);
									mAccessor->mParams.push_back(mParam);
							}
							break;

						case CE_VERTICES:
							if (mMesh)
							{
									if (mMesh->mVertices ==	0)
									{
											mVertices	=	new	C_Vertices(element);
											mMesh->mVertices = mVertices;
									}
							}
							break;
						case CE_INPUT:
							if (1)
							{
									C_Input	*i = new C_Input(element);
									if (mTriangles)
									{
											mTriangles->mInputs.push_back(i);
									}
									else if	(mVertices)
									{
											mVertices->mInputs.push_back(i);
									}
							}
							break;
						case CE_POLYGONS:
							if (mMesh)
							{
									mTriangles = new C_Triangles(element,	IT_POLYGONS);
									mMesh->mTriangles.push_back(mTriangles);
							}
							break;
						case CE_TRIANGLES:
							if (mMesh)
							{
									mTriangles = new C_Triangles(element,	IT_TRIANGLES);
									mMesh->mTriangles.push_back(mTriangles);
							}
							break;
						case CE_POLYLIST:
							if (mMesh)
							{
									mTriangles = new C_Triangles(element,	IT_POLYLIST);
									mMesh->mTriangles.push_back(mTriangles);
							}
							break;
						case CE_P:
						case CE_VCOUNT:
							mOperation = etype;
							break;
						case CE_VISUAL_SCENE:
							if (mLibraryVisualScenes)
							{
									mVisualScene = new C_VisualScene(element);
									mLibraryVisualScenes->mVisualScenes.push_back(mVisualScene);
							}
							else
							{
									WARN("Encountered	a	visual scene element but not inside	a	library	visual scene?");
							}
							break;
						case CE_NODE:
							if (mVisualScene)
							{
									mNode	=	new	C_Node(element);
									mVisualScene->mNodes.push_back(mNode);
							}
							else
							{
									// It	*is* valid to	have a 'node'	within a Node	or 'library_nodes'
									WARN("Encountered	a	node element but not processing	a	VisualScene");
							}
							break;
						case CE_MATRIX:
						case CE_TRANSLATE:
						case CE_ROTATE:
							mOperation = etype;
							break;
						case CE_INSTANCE_GEOMETRY:
							if (1)
							{
									const	char *mesh = getAttribute(element, "url");
									if (mesh)
									{
											if (mCurrent ==	CE_NODE	&& mNode)
											{
													mNode->setInstanceGeometry(mesh);
											}
											if (mCurrent ==	CE_SHAPE &&	mShape)
											{

													mShape->setInstancedGeometry(mesh);

													bool found = false;

													for	(unsigned	int	i	=	0; i < mMeshes.size(); i++)
													{
															const	char *m	=	mMeshes[i];
															if (strcmp(m,	mesh)	== 0)
															{
																	found	=	true;
																	break;
															}
													}
													if (!found)
													{
															mMeshes.push_back(mesh); //	save this	in the list	of
													}
											}
									}
							}
							break;
						case CE_PHYSICS_MATERIAL:
							if (mLibraryPhysicsMaterials)
							{
									mPhysicsMaterial = new C_PhysicsMaterial(element,	mLibraryPhysicsMaterials->mPhysicsMaterials.size());
									mLibraryPhysicsMaterials->mPhysicsMaterials.push_back(mPhysicsMaterial);
							}
							else
							{
									WARN("Encountered	a	PhysicsMaterial	element	outside	of the PhysicsLibrary");
							}
							break;
						case CE_PHYSICS_MODEL:
							if (mLibraryPhysicsModels)
							{
									mPhysicsModel	=	new	C_PhysicsModel(element);
									mLibraryPhysicsModels->mPhysicsModels.push_back(mPhysicsModel);
							}
							else
							{
									WARN("Encountered	a	PhysicsModel element outside of	the	PhysicsModelLibrary");
							}
							break;
						case CE_RIGID_CONSTRAINT:
							if (mPhysicsModel)
							{
									mRigidConstraint = new C_RigidConstraint(element);
									mPhysicsModel->mRigidConstraints.push_back(mRigidConstraint);
							}
							break;
						case CE_RIGID_BODY:
							if (mPhysicsModel)
							{
									mRigidBody = new C_RigidBody(element);
									mPhysicsModel->mRigidBodies.push_back(mRigidBody);
							}
							else
							{
									WARN("Encountered	a	RigidBody	element	but	not	inside a PhysicsModel");
							}
							break;
						case CE_INSTANCE_PHYSICS_MATERIAL:
							if (1)
							{
									const	char *url	=	getAttribute(element,	"url");
									if (url)
									{
											switch (mCurrent)
											{
												case CE_SHAPE:
													if (mShape)
													{
															mShape->mInstancePhysicsMaterial = url;
													}
													break;
												case CE_RIGID_BODY:
													if (mRigidBody)
													{
															mRigidBody->mInstancePhysicsMaterial = url;
													}
													break;
												default:
													break;
											}
									}
							}
							break;

							// material	values
						case CE_DYNAMIC_FRICTION:
						case CE_RESTITUTION:
						case CE_STATIC_FRICTION:
							mOperation = etype;
							break;
							// rigid body	values
						case CE_DYNAMIC:
							mOperation = etype;
							break;
						case CE_MASS:
						case CE_MASS_FRAME:
						case CE_INERTIA:
							// shape values.
						case CE_DENSITY:
						case CE_HALF_EXTENTS:
						case CE_EQUATION:
						case CE_HEIGHT:
							mOperation = etype;
							break;
						case CE_RADIUS:
							mOperation = etype;
							break;
						case CE_SHAPE:
							if (mRigidBody)
							{
									mShape = new C_Shape(element);
									mRigidBody->mShapes.push_back(mShape);
							}
							else
							{
									WARN("Encountered	a	Shape	element	outside	of the RigidBody");
							}
							break;
						case CE_CAPSULE:
						case CE_CYLINDER:
						case CE_TAPERED_CAPSULE:
						case CE_TAPERED_CYLINDER:
							if (mShape)
							{
									mShape->mShapeType = CST_CAPSULE;
							}
							else
							{
									WARN("Encountered	a	Capsule	element	outside	of a Shape");
							}
							break;
						case CE_SPHERE:
							if (mShape)
							{
									mShape->mShapeType = CST_SPHERE;
							}
							else
							{
									WARN("Encountered	a	Sphere element outside of	a	Shape");
							}
							break;
						case CE_PLANE:
							if (mShape)
							{
									mShape->mShapeType = CST_PLANE;
							}
							else
							{
									WARN("Encountered	a	Plane	element	outside	of a Shape");
							}
							break;
						case CE_BOX:
							if (mShape)
							{
									mShape->mShapeType = CST_BOX;
							}
							else
							{
									WARN("Encountered	a	Box	element	outside	of a Shape");
							}
							break;
						case CE_PHYSICS_SCENE:
							if (mLibraryPhysicsScenes)
							{
									mPhysicsScene	=	new	C_PhysicsScene(element);
									mLibraryPhysicsScenes->mPhysicsScenes.push_back(mPhysicsScene);
							}
							else
							{
									WARN("Encountered	a	PhysicsScene element outside of	a	LibraryPhysicsScene");
							}
							break;
						case CE_INSTANCE_PHYSICS_MODEL:
							if (mPhysicsScene)
							{
									mInstancePhysicsModel	=	new	C_InstancePhysicsModel(element);
									mPhysicsScene->mInstancePhysicsModels.push_back(mInstancePhysicsModel);
							}
							else if	(mPhysicsModel)
							{
									mInstancePhysicsModel	=	new	C_InstancePhysicsModel(element);
									mPhysicsModel->mInstancePhysicsModels.push_back(mInstancePhysicsModel);
							}
							else
							{
									WARN("Encountered	an InstancePhysicsModel	element	outside	of a PhysicsScene	or PhysicsModel");
							}
							break;
						case CE_INSTANCE_RIGID_CONSTRAINT:
							if (mInstancePhysicsModel)
							{
									mInstanceRigidConstraint = new C_InstanceRigidConstraint(element);
									mInstancePhysicsModel->mInstanceRigidConstraints.push_back(mInstanceRigidConstraint);
									mJointCount++;
							}
							break;
						case CE_DISABLE_COLLISION:
							if (mInstancePhysicsModel)
							{
									mDisableCollision	=	new	C_DisableCollision(element);
									mInstancePhysicsModel->mDisableCollisions.push_back(mDisableCollision);
									mBodyPairCount++;
							}
							else
							{
									mOperation = etype;
							}
							break;
						case CE_INSTANCE_RIGID_BODY:
							if (mInstancePhysicsModel)
							{
									mInstanceRigidBody = new C_InstanceRigidBody(element);
									mInstancePhysicsModel->mInstanceRigidBodies.push_back(mInstanceRigidBody);
									mInstanceCount++;
							}
							else
							{
									WARN("Encountered	an InstanceRigidBody element outside of	an InstancePhysicsModel");
							}
							break;
						case CE_VELOCITY:
							if (mInstanceRigidBody)
							{
									mOperation = etype;
							}
							else
							{
									WARN("Encountered	a	velocity element outside of	an InstanceRigidBody");
							}
							break;
						case CE_ANGULAR_VELOCITY:
							if (mInstanceRigidBody)
							{
									mOperation = etype;
							}
							else
							{
									WARN("Encountered	an angular velocity	element	outside	of an	InstanceRigidBody");
							}
							break;
						case CE_GRAVITY:
							if (mPhysicsScene)
							{
									mOperation = etype;
							}
							else
							{
									WARN("Encountered	a	gravity	element	outside	of a PhysicsScene");
							}
							break;
						case CE_TIME_STEP:
							if (mPhysicsScene)
							{
									mOperation = etype;
							}
							else
							{
									WARN("Encountered	a	time_step	element	outside	of a PhysicsScene");
							}
							break;
						case CE_INSTANCE_VISUAL_SCENE:
							if (mScene)
							{
									mInstanceVisualScene = new C_InstanceVisualScene(element);
									mScene->mInstanceVisualScenes.push_back(mInstanceVisualScene);
							}
							break;
						case CE_INSTANCE_PHYSICS_SCENE:
							if (mScene)
							{
									mInstancePhysicsScene	=	new	C_InstancePhysicsScene(element);
									mScene->mInstancePhysicsScenes.push_back(mInstancePhysicsScene);
							}
							break;
						case CE_REF_ATTACHMENT:
							if (mRigidConstraint)
							{
									mRigidConstraint->setRefAttachment(element);
							}
							break;
						case CE_ATTACHMENT:
							if (mRigidConstraint)
							{
									mRigidConstraint->setAttachment(element);
							}
							break;
						case CE_LINEAR:
							if (mRigidConstraint)
							{
									mRigidConstraint->mLinear	=	true;
							}
							break;
						case CE_ANGULAR:
						case CE_SWING_CONE_AND_TWIST:
							if (mRigidConstraint)
							{
									mRigidConstraint->mLinear	=	false;
							}
							break;
						case CE_NX_JOINT_DRIVE_DESC:
							if (mRigidConstraint)
							{
									mRigidConstraint->setJointDriveDesc(element);
							}
							break;
						case CE_MIN:
						case CE_MAX:
						case CE_STIFFNESS:
						case CE_DAMPING:
						case CE_TARGET_VALUE:
						case CE_ENABLED:
						case CE_INTERPENETRATE:
						case CE_GROUP:
						case CE_SKIN_WIDTH:
						case CE_WAKEUP_COUNTER:
						case CE_LINEAR_DAMPING:
						case CE_ANGULAR_DAMPING:
						case CE_MAX_ANGULAR_VELOCITY:
						case CE_SLEEP_LINEAR_VELOCITY:
						case CE_SLEEP_ANGULAR_VELOCITY:
						case CE_DISABLE_GRAVITY:
						case CE_KINEMATIC:
						case CE_POSE_SLEEP_TEST:
						case CE_FILTER_SLEEP_VELOCITY:
						case CE_DISABLE_RESPONSE:
						case CE_LOCK_COM:
						case CE_PROJECTION_MODE:
						case CE_PROJECTION_ANGLE:
						case CE_PROJECTION_DISTANCE:
						case CE_SOLVER_ITERATION_COUNT:
						case CE_DRIVE_TYPE:
						case CE_SPRING:
						case CE_FORCE_LIMIT:
						case CE_DRIVE_POSITION:
						case CE_DRIVE_ORIENTATION:
						case CE_DRIVE_LINEAR_VELOCITY:
						case CE_DRIVE_ANGULAR_VELOCITY:
						case CE_GEAR_RATIO:
							mOperation = etype;
							break;
						default:
							break;
				}
		}


		void ProcessNode(int ntype,	const	char *svalue,	int	depth, TiXmlNode *node)
		{
				char value[43];
				value[39]	=	'.';
				value[40]	=	'.';
				value[41]	=	'.';
				value[42]	=	0;

				strncpy(value, svalue, 39);

				switch (ntype)
				{
						case TiXmlNode::ELEMENT: case	TiXmlNode::DOCUMENT: if	(ntype ==	TiXmlNode::DOCUMENT)
						{
								Display(depth, "Node(DOCUMENT):	%s\n", value);
						}
						else
						{
								Display(depth, "Node(ELEMENT): %s\n",	value);
								CELEMENT e = getELEMENT(svalue);
								if (e	!= CE_IGNORE)
								{
										TiXmlElement *element	=	node->ToElement(); //	is there an	element?	Yes, traverse	it's attribute key-pair	values.
										ProcessElement(e,	element, depth);
								}
						}
						break;
						case TiXmlNode::TEXT:

						Display(depth, "Node(TEXT):	%s\n", value);

						switch (mCurrent)
						{
								case CE_RIGID_CONSTRAINT:
								case CE_NX_JOINT_DRIVE_DESC:
									if (mRigidConstraint)
									{
											mRigidConstraint->setText(mOperation,	svalue);
									}
									break;
								case CE_TRIANGLES:
								case CE_POLYGONS:
								case CE_POLYLIST:
									if (mTriangles &&	mOperation ==	CE_P)
									{
											mTriangles->addPoints(svalue);
									}
									if (mTriangles &&	mOperation ==	CE_VCOUNT)
									{
											mTriangles->addVcount(svalue);
									}
									break;
								case CE_FLOAT_ARRAY:
								case CE_INT_ARRAY:
								case CE_BOOL_ARRAY:
								case CE_NAME_ARRAY:
								case CE_IDREF_ARRAY:
									if (mArray)
									{
											mArray->setText(mOperation,	svalue);
									}
									break;
								case CE_PHYSICS_SCENE:
									if (mPhysicsScene)
									{
											switch (mOperation)
											{
												case CE_GRAVITY:
													mPhysicsScene->setGravity(svalue);
													break;
												case CE_TIME_STEP:
													mPhysicsScene->setTimeStep(svalue);
													break;
												default:
													break;
											}
									}
									break;
								case CE_INSTANCE_RIGID_BODY:
									if (mInstanceRigidBody)
									{
											switch (mOperation)
											{
												case CE_VELOCITY:
													mInstanceRigidBody->setVelocity(svalue);
													break;
												case CE_ANGULAR_VELOCITY:
													mInstanceRigidBody->setAngularVelocity(svalue);
													break;
												default:
													break;
											}
									}
									break;
								case CE_SHAPE:
									if (mShape)
									{
											mShape->setText(mOperation,	svalue);

									}
									break;
								case CE_MASS_FRAME:
									if (mRigidBody)
									{
											switch (mOperation)
											{
												case CE_TRANSLATE:
													mRigidBody->setMassFrameTranslate(svalue);
													break;
												case CE_ROTATE:
													mRigidBody->setMassFrameRotate(svalue);
													break;
												case CE_MATRIX:
													mRigidBody->setMassFrame(svalue);
													break;
												default:
													break;
											}
									}
									break;
								case CE_RIGID_BODY:
									if (mRigidBody)
									{
											mRigidBody->setText(mOperation,	svalue);
									}
									break;
								case CE_NODE:
									if (mNode)
									{
											switch (mOperation)
											{
												case CE_MATRIX:
													mNode->setMatrix(svalue);
													break;
												case CE_TRANSLATE:
													mNode->translate(svalue);
													break;
												case CE_ROTATE:
													mNode->rotate(svalue);
													break;
												default:
													break;
											}
									}
									break;
								case CE_PHYSICS_MATERIAL:
									if (mPhysicsMaterial)
									{
											float	v	=	getFloat(svalue);
											switch (mOperation)
											{
												case CE_DYNAMIC_FRICTION:
													mPhysicsMaterial->mDynamicFriction = v;
													break;
												case CE_STATIC_FRICTION:
													mPhysicsMaterial->mStaticFriction	=	v;
													break;
												case CE_RESTITUTION:
													mPhysicsMaterial->mRestitution = v;
													break;
												default:
													break;
											}
									}
									break;
								default:
									break;
							}

							break;
							case TiXmlNode::COMMENT: Display(depth,	"Node(COMMENT):	%s\n", value);
							break;
							case TiXmlNode::DECLARATION: Display(depth,	"Node(DECLARATION):	%s\n", value);
							break;
							case TiXmlNode::UNKNOWN: Display(depth,	"Node(UNKNOWN):	%s\n", value);
							break;
						default:
							Display(depth, "Node(?): %s\n",	value);
							break;
				}
		}


		void traverse(TiXmlNode	*node, int depth)
		{

				ProcessNode(node->Type(),	node->Value(), depth,	node);

				node = node->FirstChild();
				while	(node)
				{
						if (node->NoChildren())
						{
								ProcessNode(node->Type(),	node->Value(), depth,	node);
						}
						else
						{
								push(node);

								traverse(node, depth + 1);
								closeNode(node); //

								pop(node);
						}
						node = node->NextSibling();
				}
		}

		void closeNode(TiXmlNode *node)
		{
				if (node &&	node->Type() ==	TiXmlNode::ELEMENT)
				{

						CELEMENT from	=	getELEMENT(node->Value());

						switch (from)
						{
								case CE_NX_JOINT_DRIVE_DESC:
									if (mRigidConstraint)
									{
											mRigidConstraint->mDrive = 0;
									}
									// cancel	out	the	drive	pointer.
									break;
								case CE_TRIANGLES:
								case CE_POLYGONS:
								case CE_POLYLIST:
									mTriangles = 0;
									break;
								case CE_VERTICES:
									mVertices	=	0;
									break;
								case CE_GEOMETRY:
									mGeometry	=	0;
									break;
								case CE_VISUAL_SCENE:
									mVisualScene = 0;
									break;
								case CE_NODE:
									mNode	=	0;
									break;
								case CE_PHYSICS_MATERIAL:
									mPhysicsMaterial = 0;
									break;
								case CE_PHYSICS_MODEL:
									mPhysicsModel	=	0;
									break;
								case CE_RIGID_BODY:
									mRigidBody = 0;
									break;
								case CE_SHAPE:
									mShape = 0;
									break;
								case CE_INSTANCE_PHYSICS_MODEL:
									mInstancePhysicsModel	=	0;
									break;
								case CE_INSTANCE_RIGID_BODY:
									mInstanceRigidBody = 0;
									break;
								case CE_INSTANCE_VISUAL_SCENE:
									mInstanceVisualScene = 0;
									break;
								case CE_INSTANCE_PHYSICS_SCENE:
									mInstancePhysicsScene	=	0;
									break;
								case CE_PHYSICS_SCENE:
									mPhysicsScene	=	0;
									break;
								case CE_MESH:
								case CE_CONVEX_MESH:
									mMesh	=	0;
									break;
								case CE_SOURCE:
									mSource	=	0;
									break;
								case CE_FLOAT_ARRAY:
								case CE_INT_ARRAY:
								case CE_BOOL_ARRAY:
								case CE_NAME_ARRAY:
								case CE_IDREF_ARRAY:
									mArray = 0;
									break;
								case CE_ACCESSOR:
									mAccessor	=	0;
									break;
								case CE_PARAM:
									mParam = 0;
									break;
								case CE_RIGID_CONSTRAINT:
									mRigidConstraint = 0;
									break;
								default:
									break;
						}
				}
				mOperation = CE_IGNORE;
		}


		virtual	C_Geometry *locateGeometry(const char	*str)
		{
				C_Geometry *ret	=	0;
				if (mLibraryGeometries)
				{
						ret	=	mLibraryGeometries->locateGeometry(str);
				}
				return ret;
		}

		virtual	C_VisualScene	*locateVisualScene(const char	*str)
		{
				C_VisualScene	*ret = 0;

				if (mLibraryVisualScenes)
				{
						ret	=	mLibraryVisualScenes->locateVisualScene(str);
				}

				return ret;
		}

		virtual	C_PhysicsScene *locatePhysicsScene(const char	*str)
		{
				C_PhysicsScene *ret	=	0;

				if (mLibraryPhysicsScenes	&& str)
				{
						ret	=	mLibraryPhysicsScenes->locatePhysicsScene(str);
				}
				return ret;
		}

		virtual	C_PhysicsModel *locatePhysicsModel(const char	*str)
		{
				C_PhysicsModel *ret	=	0;
				if (mLibraryPhysicsModels	&& str)
				{
						ret	=	mLibraryPhysicsModels->locatePhysicsModel(str);
				}
				return ret;
		}


		virtual	int	getMaterialIndex(const char	*mat)
		{
				int	ret	=	0;

				if (mLibraryPhysicsMaterials)
				{
						ret	=	mLibraryPhysicsMaterials->getMaterialIndex(mat);
				}
				return ret;
		}

		C_Node *locateNode(const char	*str)
		{
				C_Node *ret	=	0;
				if (mLibraryVisualScenes)
				{
						ret	=	mLibraryVisualScenes->locateNode(str);
				}
				return ret;
		}

	private:

		void push(TiXmlNode	*node)
		{
				assert(mStackPtr < NODE_STACK);
				CELEMENT e = getELEMENT(node->Value());
				if (isBlock(e))
				{
						if (mStackPtr	<	NODE_STACK)
						{
								mStack[mStackPtr].mNode	=	node;
								mStack[mStackPtr].mElement = e;
								mStackPtr++;
						}
						mCurrent = e;
				}
		}

		void pop(TiXmlNode *node)
		{
				assert(mStackPtr < NODE_STACK);
				CELEMENT e = getELEMENT(node->Value());
				if (isBlock(e))
				{
						if (mStackPtr)
						{
								mStackPtr--;
								if (mStackPtr)
								{
										mStack[mStackPtr].mElement = CE_IGNORE;
										mStack[mStackPtr].mNode	=	0;
										mCurrent = mStack[mStackPtr	-	1].mElement;
								}
								else
								{
										mCurrent = CE_IGNORE;
								}
						}
				}
		}

		void printStack(const	char *action)
		{
				Display(mStackPtr, "STACK	REPORT:	%s\r\n", action);
				for	(int i = 0;	i	<	mStackPtr; i++)
				{
						const	char *v	=	mStack[i].mNode->Value();
						int	j	=	mStack[i].mElement;
						const	char *str	=	CE_NAMES[j];
						Display(mStackPtr, "%d : %s	 : %s\r\n",	i	+	1, str,	v);
				}
				Display(mStackPtr, "\r\n");
		}

		bool isBlock(CELEMENT	e)const	 //	if it	a	'block'	section	we push/pop	off	of the stack.
		{
				bool ret = false;
				switch (e)
				{
						case CE_NX_JOINT_DRIVE_DESC:
						case CE_SCENE:
						case CE_LIBRARY_GEOMETRIES:
						case CE_LIBRARY_PHYSICS_SCENES:
						case CE_LIBRARY_VISUAL_SCENES:
						case CE_LIBRARY_PHYSICS_MATERIALS:
						case CE_LIBRARY_PHYSICS_MODELS:
						case CE_GEOMETRY:
						case CE_VISUAL_SCENE:
						case CE_NODE:
						case CE_PHYSICS_MATERIAL:
						case CE_PHYSICS_MODEL:
						case CE_RIGID_BODY:
						case CE_SHAPE:
						case CE_PHYSICS_SCENE:
						case CE_INSTANCE_PHYSICS_MODEL:
						case CE_INSTANCE_VISUAL_SCENE:
						case CE_INSTANCE_PHYSICS_SCENE:
						case CE_INSTANCE_RIGID_BODY:
						case CE_MASS_FRAME:
						case CE_CONVEX_MESH:
						case CE_MESH:
						case CE_SOURCE:
						case CE_FLOAT_ARRAY:
						case CE_INT_ARRAY:
						case CE_BOOL_ARRAY:
						case CE_NAME_ARRAY:
						case CE_IDREF_ARRAY:
						case CE_ACCESSOR:
						case CE_PARAM:
						case CE_TRIANGLES:
						case CE_POLYLIST:
						case CE_POLYGONS:
						case CE_VERTICES:
						case CE_RIGID_CONSTRAINT:
							ret	=	true;
							break;
						default:
							break;
				}
				return ret;
		}

		NxConvexMeshDesc	*locateConvexMeshDesc(const	char *str)
		{
				NxConvexMeshDesc	*ret = 0;

				if (mCollection	&& str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						for	(unsigned	int	i	=	0; i < mCollection->mConvexMeshes.size(); i++)
						{
								NxConvexMeshDesc	*cd	=	mCollection->mConvexMeshes[i];
								if (strcmp(cd->mId,	str) ==	0)
								{
										ret	=	cd;
										break;
								}
						}
				}

				return ret;
		}

		NxTriangleMeshDesc	*locateTriangleMeshDesc(const	char *str)
		{
				NxTriangleMeshDesc	*ret = 0;

				if (mCollection	&& str)
				{
						if (*str ==	'#')
						{
								str++;
						}
						for	(unsigned	int	i	=	0; i < mCollection->mTriangleMeshes.size(); i++)
						{
								NxTriangleMeshDesc	*cd	=	mCollection->mTriangleMeshes[i];
								if (strcmp(cd->mId,	str) ==	0)
								{
										ret	=	cd;
										break;
								}
						}
				}

				return ret;
		}

		NXU_FILE *mFph;
		TiXmlDocument	*mXML;
		NxuPhysicsCollection *mCollection;

		int	mStackPtr;
		StackEntry mStack[NODE_STACK];

		CELEMENT mCurrent; //	current	major	element	type we	are	parsing.
		CELEMENT mOperation;

		//
		C_Geometry *mGeometry; //	current	geometry being processed.
		C_Mesh *mMesh;
		C_Triangles	*mTriangles;
		C_Vertices *mVertices;
		C_Source *mSource;
		C_Array	*mArray;
		C_Accessor *mAccessor;
		C_Param	*mParam;
		C_VisualScene	*mVisualScene;
		C_Node *mNode;
		C_PhysicsMaterial	*mPhysicsMaterial;
		C_PhysicsModel *mPhysicsModel;
		C_RigidBody	*mRigidBody;
		C_Shape	*mShape;
		C_InstancePhysicsModel *mInstancePhysicsModel;
		C_InstanceRigidBody	*mInstanceRigidBody;
		C_InstanceVisualScene	*mInstanceVisualScene;
		C_InstancePhysicsScene *mInstancePhysicsScene;
		C_PhysicsScene *mPhysicsScene;
		C_RigidConstraint	*mRigidConstraint;
		C_InstanceRigidConstraint	*mInstanceRigidConstraint;
		C_DisableCollision *mDisableCollision;
		int	mInstanceCount;
		int	mJointCount;
		int	mBodyPairCount;
		//


		C_Scene	*mScene; //	describes	the	instancing of	visual and physics scenes.

		C_LibraryGeometries	*mLibraryGeometries; //	all	of the geometries	in the file
		C_LibraryPhysicsMaterials	*mLibraryPhysicsMaterials; //	all	of the physics materials in	the	file.

		C_LibraryVisualScenes	*mLibraryVisualScenes; //	all	of the visual	scenes.
		C_LibraryPhysicsModels *mLibraryPhysicsModels; //	all	of the physics models.
		C_LibraryPhysicsScenes *mLibraryPhysicsScenes; //	all	of the physics scenes.

		int	mConvexCount;	// number	of unique	convex hulls instantiated.
		int	mMeshCount;	// number	of unique	triangle meshes	instantiated.
		NxArray	<	const	char * > mMeshes;	// list	of meshes	that are instantiated	by the physics as	either or	convex hulls.


};

NxuPhysicsCollection *colladaImport(const	char *fname,void *mem,int len)
{
	NxuPhysicsCollection *ret	=	0;

	ColladaImport	ci(fname,mem,len);

	ret	=	ci.getCollection();

	ci.setCollection(0); //	don't	free up	the	memory we	are	going	to take	ownership	now.

	return ret;
}


void C_InstanceRigidBody::getActors(NxSceneDesc *scene, C_Query *q, C_PhysicsModel	*pmodel, const NxMat34 &parent)
{
	C_RigidBody	*rb	=	0;

	if (pmodel)
	{
		rb = pmodel->locateRigidBody(mBody);

		if (rb)
		{
			NxMat34	mat;
			mat.id();
			if (mTarget)
			{
				C_Node *node = q->locateNode(mTarget);
				assert(node);
				if (node)
				{
					mat	=	node->mTransform;
					mat.multiply(parent, mat);
				}
			}
			rb->getActor(scene,	q, mat,	mVelocity, mAngularVelocity);
		}
	}
}

void C_InstanceRigidConstraint::getJoints(NxSceneDesc *scene, C_Query *q, C_PhysicsModel	*pmodel)
{
	C_RigidConstraint	*rc	=	0;

	if (pmodel &&	mConstraint)
	{
		rc = pmodel->locateRigidConstraint(mConstraint);
		if (rc)
		{
			rc->getJoint(scene,	q, pmodel);
		}
	}
}

void C_RigidConstraint::getJoint(NxSceneDesc	*scene,	C_Query	*q,	C_PhysicsModel *pmodel)
{
	NxD6JointDesc *j	=	new	NxD6JointDesc;

	j->name	=	getGlobalString(mName);

	C_RigidBody *rb1 = pmodel->locateRigidBody(mBody1);
	C_RigidBody *rb2 = pmodel->locateRigidBody(mBody2);

	if ( rb1 )
	{
		j->mActor0 = getGlobalString( rb1->mActorDesc->mId );
	}
	if ( rb2 )
	{
		j->mActor1 = getGlobalString( rb2->mActorDesc->mId );
	}


	mMatrix1.M.getColumn(0,	j->localNormal0);
	mMatrix1.M.getColumn(2,	j->localAxis0);
	j->localAnchor0	=	mMatrix1.t;

	mMatrix2.M.getColumn(0,	j->localNormal1);
	mMatrix2.M.getColumn(2,	j->localAxis1);
	j->localAnchor1	=	mMatrix2.t;

	if (!mInterpenetrate)
	// if	not	allowed	to interpentrate
	{
		j->jointFlags = (NxJointFlag) (j->jointFlags	| NX_JF_COLLISION_ENABLED);
	}

	j->xMotion = getMotion(mLinearMin.x, mLinearMax.x);
	j->yMotion = getMotion(mLinearMin.y, mLinearMax.y);
	j->zMotion = getMotion(mLinearMin.z, mLinearMax.z);

	j->swing1Motion	=	getMotion(mAngularMin.x, mAngularMax.x);
	j->swing2Motion	=	getMotion(mAngularMin.y, mAngularMax.y);
	j->twistMotion = getMotion(mAngularMin.z,	mAngularMax.z);

	float	dist = 0;
	if (j->xMotion ==	NX_D6JOINT_MOTION_LIMITED)
	{
		dist = fabsf(mLinearMax.x	-	mLinearMin.x)	*0.5f;
	}
	if (j->yMotion ==	NX_D6JOINT_MOTION_LIMITED)
	{
		float	d	=	fabsf(mLinearMax.y - mLinearMin.y) *0.5f;
		if (d	>	dist)
		{
			dist = d;
		}
	}
	if (j->zMotion ==	NX_D6JOINT_MOTION_LIMITED)
	{
		float	d	=	fabsf(mLinearMax.z - mLinearMin.z) *0.5f;
		if (d	>	dist)
		{
			dist = d;
		}
	}

	j->linearLimit.value = dist;
	j->linearLimit.spring	=	mLinearSpring.mStiffness;
	j->linearLimit.damping = mLinearSpring.mDamping;
	j->linearLimit.restitution = mLinearSpring.mTargetValue;

	j->swing1Limit.value = getMeanRad(mAngularMax.x, mAngularMin.x);
	j->swing1Limit.spring	=	mAngularSpring.mStiffness;
	j->swing1Limit.damping = mAngularSpring.mDamping;
	j->swing1Limit.restitution = mAngularSpring.mTargetValue;

	j->swing2Limit.value = getMeanRad(mAngularMax.y, mAngularMin.y);
	j->swing2Limit.spring	=	mAngularSpring.mStiffness;
	j->swing2Limit.damping = mAngularSpring.mDamping;
	j->swing2Limit.restitution = mAngularSpring.mTargetValue;

	j->twistLimit.low.value	=	mAngularMin.z	*	DEG_TO_RAD;
	j->twistLimit.low.spring = mAngularSpring.mStiffness;
	j->twistLimit.low.damping	=	mAngularSpring.mDamping;
	j->twistLimit.low.restitution	=	mAngularSpring.mTargetValue;

	j->twistLimit.high.value = mAngularMax.z * DEG_TO_RAD;
	j->twistLimit.high.spring	=	mAngularSpring.mStiffness;
	j->twistLimit.high.damping = mAngularSpring.mDamping;
	j->twistLimit.high.restitution = mAngularSpring.mTargetValue;
	#if	1
	if (mProjectionMode)
	{
		if (strcasecmp(mProjectionMode, "NX_JPM_POINT_MINDIST") ==	0)
		{
			j->projectionMode	=	NX_JPM_POINT_MINDIST;
			j->projectionDistance	=	mProjectionDistance;
			j->projectionAngle = mProjectionAngle;
		}
	}
	#endif

	#if	1
	// time	to fill	out	the	joint	drive	data.
	j->drivePosition = drivePosition;
	j->driveOrientation	=	driveOrientation;
	j->driveLinearVelocity = driveLinearVelocity;
	j->driveAngularVelocity	=	driveAngularVelocity;
	j->gearRatio = gearRatio;
	xDrive.get(j->xDrive);
	yDrive.get(j->yDrive);
	zDrive.get(j->zDrive);
	#endif

	swingDrive.get(j->swingDrive);
	slerpDrive.get(j->slerpDrive);
	twistDrive.get(j->twistDrive);

	scene->mJoints.push_back(j);

}

void C_DisableCollision::getBodyPairFlag(NxSceneDesc	*scene,	C_Query	*query,	C_PhysicsModel *pmodel)
{
	NxPairFlagDesc	*bpair = new NxPairFlagDesc;

	bpair->mActor0 = getGlobalString(mBody1);
	bpair->mActor1 = getGlobalString(mBody2);


	bpair->mFlags = NX_IGNORE_PAIR;

	scene->mPairFlags.push_back(bpair);

}

void C_Mesh::getTriangleMesh(C_TriangleMesh	&t,	C_Query	*q)
{
	if (mConvexHullOf)
	{
		C_Geometry *geom = q->locateGeometry(mConvexHullOf);
		if (geom)
		{
			geom->getTriangleMesh(t, q);
		}
	}
	if (mVertices)
	// if	we have	'vertices' defined.
	{
		C_Input	*input = mVertices->locateInput("POSITION"); //	locate the source	input	that uses	this semantic!
		if (input)
		// ok, we	have the 'position'	input.
		{
			C_Source *source = locateSource(input->mSource);
			// ok..so	far	we have	identified that	the	vertices specification has a 'position'	field	and	we have	located	the	source for it.
			if (source)
			{
				for	(unsigned	int	i	=	0; i < mTriangles.size();	i++)
				{
					C_Triangles	*tris	=	mTriangles[i];
					tris->getTriangleMesh(t, source, mVertices);
				}
			}
		}
	}
}

void C_PhysicsModel::getRigidBodyElements(NxSceneDesc *scene, C_Query *query, const NxMat34 &parent)
{
	for	(unsigned	int	i	=	0; i < mInstancePhysicsModels.size();	i++)
	{
		C_InstancePhysicsModel *ipm	=	mInstancePhysicsModels[i];
		ipm->getRigidBodyElements(scene, query,	parent);
	}
}


C_PhysicsModel::~C_PhysicsModel(void)
{
	for	(unsigned	int	i	=	0; i < mRigidBodies.size();	i++)
	{
		C_RigidBody	*c = mRigidBodies[i];
		delete c;
	}
	for	(unsigned	int	i	=	0; i < mRigidConstraints.size(); i++)
	{
		C_RigidConstraint	*rc	=	mRigidConstraints[i];
		delete rc;
	}

	for	(unsigned	int	i	=	0; i < mInstancePhysicsModels.size();	i++)
	{
		C_InstancePhysicsModel *ip = mInstancePhysicsModels[i];
		delete ip;
	}
}

void C_InstancePhysicsModel::getRigidBodyElements(NxSceneDesc *scene, C_Query *query, const NxMat34 &root)
{

	C_PhysicsModel *pm = query->locatePhysicsModel(mUrl);

	if (pm)
	{

		NxMat34	parent = root;
		if (mParent)
		{
			C_Node *n	=	query->locateNode(mParent);
			if (n)
			{
				parent.multiply(root,	n->mTransform);
			}
		}

		for	(unsigned	int	i	=	0; i < mInstanceRigidBodies.size();	i++)
		{
			C_InstanceRigidBody	*rb	=	mInstanceRigidBodies[i];
			rb->getActors(scene, query,	pm,	parent);
		}
		for	(unsigned	int	i	=	0; i < mInstanceRigidConstraints.size(); i++)
		{
			C_InstanceRigidConstraint	*rb	=	mInstanceRigidConstraints[i];
			rb->getJoints(scene, query,	pm);
		}
		for	(unsigned	int	i	=	0; i < mDisableCollisions.size();	i++)
		{
			C_DisableCollision *rb = mDisableCollisions[i];
			rb->getBodyPairFlag(scene, query,	pm);
		}
		pm->getRigidBodyElements(scene,	query, parent);
	}
}


}; //	End	NXU	namespace
