#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// this	attempts to	save out a valid COLLADA graphics	visualiaztion	of the physics for debugging purposes	with some	viewers.

#include "NXU_ColladaExport.h"
#include "NXU_schema.h"
#include "NXU_Geometry.h"
#include "NXU_SchemaStream.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.
#endif

namespace	NXU
{

// const	float	DEG_TO_RAD = ((2.0f	*3.14152654f)	/	360.0f);
const	float	RAD_TO_DEG = (360.0f / (2.0f *3.141592654f));

NxShapeType getShapeType(SCHEMA_CLASS t)
{
	NxShapeType ret = NX_SHAPE_BOX;

	switch (t )
	{
    case SC_NxPlaneShapeDesc: ret = NX_SHAPE_PLANE; break;
    case SC_NxSphereShapeDesc: ret = NX_SHAPE_SPHERE; break;
    case SC_NxBoxShapeDesc: ret = NX_SHAPE_BOX; break;
    case SC_NxCapsuleShapeDesc: ret = NX_SHAPE_CAPSULE; break;
    case SC_NxWheelShapeDesc: ret = NX_SHAPE_WHEEL; break;
    case SC_NxConvexShapeDesc: ret = NX_SHAPE_CONVEX; break;
    case SC_NxTriangleMeshShapeDesc: ret = NX_SHAPE_MESH; break;
    case SC_NxHeightFieldShapeDesc: ret = NX_SHAPE_HEIGHTFIELD; break;
    default: break;
	}

	return ret;
}

NxJointType getJointType(SCHEMA_CLASS t)
{
	NxJointType ret = NX_JOINT_FIXED;

  switch ( t )
  {
   case SC_NxD6JointDesc:
		 ret = NX_JOINT_D6;
     break;
   case SC_NxCylindricalJointDesc:
		 ret = NX_JOINT_CYLINDRICAL;
     break;
   case SC_NxDistanceJointDesc:
		 ret = NX_JOINT_DISTANCE;
     break;
   case SC_NxFixedJointDesc:
		 ret = NX_JOINT_FIXED;
     break;
   case SC_NxPointInPlaneJointDesc:
		 ret = NX_JOINT_POINT_IN_PLANE;
     break;
   case SC_NxPointOnLineJointDesc:
		 ret = NX_JOINT_POINT_ON_LINE;
     break;
   case SC_NxPrismaticJointDesc:
		 ret = NX_JOINT_PRISMATIC;
     break;
   case SC_NxRevoluteJointDesc:
		 ret = NX_JOINT_REVOLUTE;
     break;
   case SC_NxSphericalJointDesc:
		 ret = NX_JOINT_SPHERICAL;
     break;
   case SC_NxPulleyJointDesc:
		 ret = NX_JOINT_PULLEY;
     break;
   default:
     break;
  }

	return ret;
}



struct StrList
{
	StrList	*mNextString;
	const	char *mString;
};


class	StringCollection
{
	public:

		StringCollection(void)
		{
				mRoot	=	0;
				mStringCount = 0;
				mStringMem = 0;
		}

		~StringCollection(void)
		{
				releaseGlobalStr();
		}

		StrList	*mRoot;
		int	mStringCount;
		int	mStringMem;

		const	char *getGlobalStr(const char	*str,	bool &wasNew)
		{
				wasNew = false;
				if (str	== 0)
				{
						return 0;
				}
				int	l	=	(int)strlen(str);
				if (l	== 0)
				{
						return 0;
				}

				if (mRoot)
				{
						StrList	*s = mRoot;

						while	(s)
						{
								if (strcmp(s->mString, str)	== 0)
								{
										return s->mString;
								}
								s	=	s->mNextString;
						}
				}


				l	=	l	+	1+sizeof(StrList);
				mStringCount++;
				mStringMem +=	l;

				char *ret	=	new	char[l];

				StrList	*s = (StrList*)ret;
				s++;
				ret	=	(char*)s;
				s--;

				s->mNextString = mRoot;
				s->mString = ret;
				mRoot	=	s;

				strcpy(ret,	str);
				wasNew = true;

				return ret;
		}

		void releaseGlobalStr(void)
		{
				StrList	*s = mRoot;
				while	(s)
				{
						StrList	*next	=	s->mNextString;
						char *c	=	(char*)s;
						delete c;
						s	=	next;
				}
				mRoot	=	0;
				mStringCount = 0;
				mStringMem = 0;
		}

};

static StringCollection	gActorNames;

class SaveCollada
{
public:

  SaveCollada(void)
  {
  }

  ~SaveCollada(void)
  {
  }

  const	char *NCNAME(const char	*str)
  {
  	static char	nameField[2048];
  	static int nameCount = 0;

  	char *ret	=	&nameField[nameCount *512];
  	nameCount++;
  	if (nameCount	== 4)
  	{
  		nameCount	=	0;
  	}

  	char *dest = ret;

  	while	(*str)
  	{
  		char c =	*str++;
  		if (c	== 32)
  		{
  			c	=	'_';
  		}
  		*dest++	=	c;
  	}
  	*dest	=	0;

  	return ret;
  }


  const char	*TF(NX_BOOL	b)
  {
  	if (b)
  	{
  		return "true";
  	}
  	return "false";
  }

  const char	*fstring(NxF32 v)
  {
  	static char	data[64	*16];
  	static int index = 0;

  	char *ret	=	&data[index	*64];
  	index++;
  	if (index	== 16)
  	{
  		index	=	0;
  	}

  	if (v	== FLT_MIN)
  	{
  		return "-INF";
  	}
  	// collada notation	for	FLT_MIN	and	FLT_MAX
  	if (v	== FLT_MAX)
  	{
  		return "INF";
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





  const char	*getName(const char	*key,	int	s, int i)
  {
  	char scratch[512];
  	if (s)
  	{
  		sprintf(scratch, "%s%d_%d",	key, i,	s);
  	}
  	else
  	{
  		sprintf(scratch, "%s%d", key,	i);
  	}

  	bool wasNew;
  	const	char *ret	=	gActorNames.getGlobalStr(scratch,	wasNew);
  	assert(wasNew);
  	return ret;
  }


  void	saveSpotLight(NXU_FILE *fph)
  {
  	//**************************************************************
  	//**************************************************************
  	// Define	a	spot light and a material
  	nxu_fprintf(fph, "	<library_lights>\r\n");
  	nxu_fprintf(fph, "		<light id=\"Spot\" name=\"Spot\">\r\n");
  	nxu_fprintf(fph, "			<technique_common>\r\n");
  	nxu_fprintf(fph, "				<point>\r\n");
  	nxu_fprintf(fph, "					<color>1.0 1.0 0.71397</color>\r\n");
  	nxu_fprintf(fph, "					<constant_attenuation>0.0</constant_attenuation>\r\n");
  	nxu_fprintf(fph, "					<linear_attenuation>0.0</linear_attenuation>\r\n");
  	nxu_fprintf(fph, "					<quadratic_attenuation>0.0</quadratic_attenuation>\r\n");
  	nxu_fprintf(fph, "				</point>\r\n");
  	nxu_fprintf(fph, "			</technique_common>\r\n");
  	nxu_fprintf(fph, "		</light>\r\n");
  	nxu_fprintf(fph, "	</library_lights>\r\n");
  }

  void	saveRigidBodyInstance(NXU_FILE *fph, NxActorDesc	*a)
  {
  	const	char *name = a->mColladaName;
  	nxu_fprintf(fph, "				<instance_rigid_body target=\"#%s-Node\" body=\"%s-RigidBody\">\r\n",	NCNAME(name),	NCNAME(name));

  	if ( a->mHasBody )
  	{
  		const	NxBodyDesc &b	=	a->mBody;
  		nxu_fprintf(fph, "					<technique_common>\r\n");
  		nxu_fprintf(fph, "						<angular_velocity>%s %s	%s</angular_velocity>\r\n",	fstring(b.angularVelocity.x),	fstring(b.angularVelocity.y),	fstring(b.angularVelocity.z));
  		nxu_fprintf(fph, "						<velocity>%s %s	%s</velocity>\r\n",	fstring(b.linearVelocity.x), fstring(b.linearVelocity.y),	fstring(b.linearVelocity.z));
  		nxu_fprintf(fph, "					</technique_common>\r\n");
  	}
  	nxu_fprintf(fph, "				</instance_rigid_body>\r\n");
  }

  void	saveRigidConstraintInstance(NXU_FILE *fph, NxJointDesc	*jdesc)
  {
  	const	char *name = jdesc->mColladaName;

  	nxu_fprintf(fph, "				<instance_rigid_constraint constraint=\"%s-RigidConstraint\"/>\r\n", NCNAME(name));
  }

  void	Indent(NXU_FILE	*fph,	int	indent)
  {
  	for	(int i = 0;	i	<	indent;	i++)
  	{
  		nxu_fprintf(fph, " ");
  	}
  }

  void	saveTransformAxisAngle(int indent, NXU_FILE	*fph,	const	NxMat34	&mat)
  {
  	NxQuat q; q.zero();

  	mat.M.toQuat(q);

  	NxVec3 axis;
  	NxReal angle;

  	q.getAngleAxis(angle,	axis);

  	if (axis.x ==	0	&& axis.y	== 0 &&	axis.z ==	0)
  	{
  		axis.x = 1;
  	}

  	Indent(fph,	indent);
  	nxu_fprintf(fph, "<translate>%s	%s %s</translate>\r\n",	fstring(mat.t.x),	fstring(mat.t.y),	fstring(mat.t.z));
  	Indent(fph,	indent);
  	nxu_fprintf(fph, "<rotate>%s %s	%s	 %s</rotate>\r\n", fstring(axis.x),	fstring(axis.y), fstring(axis.z),	fstring(angle));
  }

  void	saveTransform(int	indent,	NXU_FILE *fph, const NxMat34 &mat)
  {
  	float	m[9];
  	mat.M.getRowMajor(m);
  	Indent(fph,	indent);

  	nxu_fprintf(fph, "<matrix	sid=\"transform\"> %s	%s %s	%s	%s %s	%s %s	 %s	%s %s	%s 0 0 0 1</matrix>\r\n",	fstring(m[0]), fstring(m[1]),	fstring(m[2]), fstring(mat.t.x),

  	fstring(m[3]), fstring(m[4]),	fstring(m[5]), fstring(mat.t.y),

  	fstring(m[6]), fstring(m[7]),	fstring(m[8]), fstring(mat.t.z));
  }

  void	saveVisualInstance(NXU_FILE	*fph,	NxActorDesc *a)
  {
  	//**************************************************************
  	//**************************************************************

  	const	char *name = a->mColladaName;

  	nxu_fprintf(fph, "			<node	id=\"%s-Node\" name=\"%s\" type=\"NODE\">\r\n",	NCNAME(name),	name);

  	saveTransformAxisAngle(8,	fph, a->globalPose);

  	if (a->mShapes.size() ==	1)
  	{
  		// bool wasNew	=	true;
  		NxShapeDesc	*s = a->mShapes[0];
  		switch ( getShapeType(s->mType) )
  		{
  			case NX_SHAPE_PLANE:
  			case NX_SHAPE_SPHERE:
  			case NX_SHAPE_BOX:
  			case NX_SHAPE_CAPSULE:
  				break;
  			case NX_SHAPE_CONVEX:
  				if (1)
  				{
  					NxConvexShapeDesc *c	=	(NxConvexShapeDesc*)s;
  					NxConvexMeshDesc	*cm	=	locateConvexMeshDesc(mCollection,c->mMeshData);
  					if (cm)
  					{
  						name = cm->mId;
  					}
  				}
  				break;
  			case NX_SHAPE_MESH:
  				if (1)
  				{
  					NxTriangleMeshShapeDesc *c	=	(NxTriangleMeshShapeDesc*)s;
  					NxTriangleMeshDesc	*cm	=	locateTriangleMeshDesc(mCollection,c->mMeshData);
  					if (cm)
  					{
  						name = cm->mId;
  					}
  				}
  				break;
			default:
				break;
  		}
  	}

  	nxu_fprintf(fph, "			</node>\r\n");

  }

  void	savePairFlag(NXU_FILE	*fph,	NxPairFlagDesc	*pd, unsigned	int	i,NxSceneDesc *scene)
  {
  	NxActorDesc *a0 = locateActorDesc(scene,pd->mActor0);
  	NxActorDesc *a1 = locateActorDesc(scene,pd->mActor1);
  	if ( a0 && a1 )
  	{
    	nxu_fprintf(fph, "					<disable_collision id=\"PairFlag_%d\"	body1=\"%s-RigidBody\" body2=\"%s-RigidBody\"/>\r\n",	i, NCNAME(a0->mColladaName),	NCNAME(a1->mColladaName));
    }
  }

  bool savePhysicsScene(NXU_FILE	*fph,	NxSceneDesc *s, int scene)
  {
		bool ret = true;

  	nxu_fprintf(fph, "		<physics_model id=\"Scene%d-PhysicsInstance\">\r\n", scene);
  	nxu_fprintf(fph, "			<instance_physics_model	url=\"#Scene%d-PhysicsModel\">\r\n", scene);

  	if (1)
  	{
  		int	acount = s->mActors.size();
  		for	(int j = 0;	j	<	acount;	j++)
  		{
  			NxActorDesc *a	=	s->mActors[j];
  			saveRigidBodyInstance(fph, a);
  		}
  	}

  	if (1)
  	{
  		int	jcount = s->mJoints.size();
  		for	(int j = 0;	j	<	jcount;	j++)
  		{
  			NxJointDesc *joint	=	s->mJoints[j];
  			saveRigidConstraintInstance(fph, joint);
  		}
  	}


  	if (1)
  	{
  		nxu_fprintf(fph, "			<extra>\r\n");
  		nxu_fprintf(fph, "				<technique profile=\"PhysX\">\r\n");
  		unsigned int pcount	=	s->mPairFlags.size();
  		for	(unsigned	int	i	=	0; i < pcount; i++)
  		{
  			NxPairFlagDesc	*pd	=	s->mPairFlags[i];
  			savePairFlag(fph,	pd,	i, s);
  		}
  		nxu_fprintf(fph, "				</technique>\r\n");
  		nxu_fprintf(fph, "			</extra>\r\n");
  	}

  	nxu_fprintf(fph, "			</instance_physics_model>\r\n");
  	nxu_fprintf(fph, "		</physics_model>\r\n");
		return ret;
  }

  void	saveVisualScene(NXU_FILE *fph, NxSceneDesc	*s,	int	sno)
  {
  	nxu_fprintf(fph, "		<visual_scene	id=\"Scene%d-Visual\"	name=\"Scene%d-Visual\">\r\n", sno,	sno);

  	if (1)
  	{
  		int	acount = s->mActors.size();
  		for	(int j = 0;	j	<	acount;	j++)
  		{
  			NxActorDesc *a	=	s->mActors[j];
  			saveVisualInstance(fph,	a);
  		}
  	}


  	nxu_fprintf(fph, "			</visual_scene>\r\n");
  }

  void	saveShape(NXU_FILE *fph, NxShapeDesc *s, int scene)
  {
  	NxShapeType	type = getShapeType(s->mType);

  	nxu_fprintf(fph, "					<shape>\r\n");
  	saveTransformAxisAngle(12, fph,	s->localPose);

  	nxu_fprintf(fph, "						<instance_physics_material url=\"#pmat%d_%d-PhysicsMaterial\"/>\r\n",	s->materialIndex,	scene);

  	if (s->mShapeMass	>	0)
  	{
  		nxu_fprintf(fph, "					 <mass>%s</mass>\r\n", fstring(s->mShapeMass));
  	}
  	else
  	{
  		nxu_fprintf(fph, "					 <density>%s</density>\r\n", fstring(s->mShapeDensity));
  	}

  	nxu_fprintf(fph, "					 <extra>\r\n");
  	nxu_fprintf(fph, "						 <technique	profile=\"PhysX\">\r\n");
  	nxu_fprintf(fph, "							 <group>%d</group>\r\n", s->group);
  	nxu_fprintf(fph, "							 <skinWidth>%s</skinWidth>\r\n", fstring(s->skinWidth));
  	nxu_fprintf(fph, "							 <disable_collision>%s</disable_collision>\r\n", TF(s->shapeFlags	&NX_SF_DISABLE_COLLISION));
  	nxu_fprintf(fph, "						 </technique>\r\n");
  	nxu_fprintf(fph, "					 </extra>\r\n");


  	switch (type)
  	{
  		case NX_SHAPE_CONVEX:
  			if (1)
  			{
  				NxConvexShapeDesc *c	=	(NxConvexShapeDesc*)s;
  				NxConvexMeshDesc	*cm	=	locateConvexMeshDesc(mCollection,c->mMeshData);
  				if (cm)
  				{
  					nxu_fprintf(fph, "						<instance_geometry url=\"#%s-Mesh\"/>\r\n",	cm->mId);
  				}
  			}
  			break;
  		case NX_SHAPE_MESH:
  			if (1)
  			{
  				NxTriangleMeshShapeDesc *t	=	(NxTriangleMeshShapeDesc*)s;
  				NxTriangleMeshDesc	*tm	=	locateTriangleMeshDesc(mCollection,t->mMeshData);
  				if (tm)
  				{
  					nxu_fprintf(fph, "						<instance_geometry url=\"#%s-Mesh\"/>\r\n",	tm->mId);
  				}
  			}
  			break;
  		case NX_SHAPE_PLANE:
  			if (1)
  			{
  				// <plane>
  				//	 <equation>A B C D</equation>
  				// </plane>
  				NxPlaneShapeDesc *p	=	(NxPlaneShapeDesc*)s;
  				nxu_fprintf(fph, "						<plane>\r\n");
  				nxu_fprintf(fph, "							<equation>%s %s	%s %s</equation>\r\n", fstring(p->normal.x), fstring(p->normal.y), fstring(p->normal.z), fstring(p->d));
  				nxu_fprintf(fph, "						</plane>\r\n");

  			}
  			break;
  		case NX_SHAPE_SPHERE:
  			if (1)
  			{
  				//<sphere>
  				//	<radius>1</radius>
  				//</sphere>
  				NxSphereShapeDesc	*p = (NxSphereShapeDesc*)s;
  				nxu_fprintf(fph, "						<sphere>\r\n");
  				nxu_fprintf(fph, "							<radius>%s</radius>\r\n",	fstring(p->radius));
  				nxu_fprintf(fph, "						</sphere>\r\n");
  			}
  			break;
  		case NX_SHAPE_BOX:
  			if (1)
  			{
  				NxBoxShapeDesc *b	=	(NxBoxShapeDesc*)s;
  				nxu_fprintf(fph, "						<box>\r\n");
  				nxu_fprintf(fph, "							<half_extents>%s %s	%s</half_extents>\r\n",	fstring(b->dimensions.x),	fstring(b->dimensions.y),	fstring(b->dimensions.z));
  				nxu_fprintf(fph, "						</box>\r\n");
  			}
  			break;
  		case NX_SHAPE_CAPSULE:
  			if (1)
  			{
  				// <capsule>
  				//	 <radius>1</radius>
  				//	 <height>1 1</height>
  				//	</capsule>
  				NxCapsuleShapeDesc *c	=	(NxCapsuleShapeDesc*)s;
  				nxu_fprintf(fph, "						<capsule>\r\n");
  				nxu_fprintf(fph, "							<radius>%s %s</radius>\r\n", fstring(c->radius), fstring(c->radius));
  				nxu_fprintf(fph, "							<height>%s</height>\r\n",	fstring(c->height));
  				nxu_fprintf(fph, "						</capsule>\r\n");
  			}
  			break;
  		case NX_SHAPE_WHEEL:
  			if (1)
  			{
  				// NxWheelShapeDesc *w	=	(NxWheelShapeDesc*)s;
  			}
  			break;
		default:
			break;
  	}



  	nxu_fprintf(fph, "					 </shape>\r\n");

  }

  void	saveRigidBody(NXU_FILE *fph, NxActorDesc	*a,	int	scene)
  {
  	const	char *name = a->mColladaName;

  	nxu_fprintf(fph, "			<rigid_body	sid=\"%s-RigidBody\" name=\"%s\">\r\n",	NCNAME(name),	a->name);
  	nxu_fprintf(fph, "				<technique_common>\r\n");

  	if (a->mShapes.size())
  	{
  		NxShapeDesc	*s = a->mShapes[0];
  		nxu_fprintf(fph, "					<instance_physics_material url=\"#pmat%d_%d-PhysicsMaterial\"/>\r\n",	s->materialIndex,	scene);
  	}

  	int	scount = a->mShapes.size();
  	for	(int i = 0;	i	<	scount;	i++)
  	{
  		NxShapeDesc	*s = a->mShapes[i];
  		saveShape(fph, s,	scene);
  	}

  	NX_BOOL	hasBody	=	a->mHasBody;

  	if (hasBody)
  	{
		nxu_fprintf(fph, "					 <dynamic>%s</dynamic>\r\n",TF(! (a->mBody.flags & NX_BF_KINEMATIC)));

  		if (a->mBody.mass	>	0)
  		{
  			nxu_fprintf(fph, "					 <mass>%s</mass>\r\n", fstring(a->mBody.mass));
  		}
  		else
  		{
  			nxu_fprintf(fph, "					 <density>%s</density>\r\n", fstring(a->density));
  		}
  		nxu_fprintf(fph, "					 <mass_frame>\r\n");

  		saveTransformAxisAngle(12, fph,	a->mBody.massLocalPose);

  		nxu_fprintf(fph, "					</mass_frame>\r\n");
  		nxu_fprintf(fph, "					<inertia>%s	%s %s</inertia>\r\n",	fstring(a->mBody.massSpaceInertia.x),	fstring(a->mBody.massSpaceInertia.y),	fstring(a->mBody.massSpaceInertia.z));

  	}
  	else
  	{
  		nxu_fprintf(fph, "					<dynamic>false</dynamic>\r\n");
  		nxu_fprintf(fph, "					<mass>1</mass>\r\n");
  	}

  	nxu_fprintf(fph, "					</technique_common>\r\n");
  	nxu_fprintf(fph, "			<extra>\r\n");
  	nxu_fprintf(fph, "				<technique profile=\"PhysX\">\r\n");

  	if (hasBody)
  	{
  		nxu_fprintf(fph, "						<wakeUpCounter>%s</wakeUpCounter>\r\n",	fstring(a->mBody.wakeUpCounter));
  		nxu_fprintf(fph, "						<linearDamping>%s</linearDamping>\r\n",	fstring(a->mBody.linearDamping));
  		nxu_fprintf(fph, "						<angularDamping>%s</angularDamping>\r\n",	fstring(a->mBody.angularDamping));
  		nxu_fprintf(fph, "						<maxAngularVelocity>%s</maxAngularVelocity>\r\n",	fstring(a->mBody.maxAngularVelocity));
  		nxu_fprintf(fph, "						<sleepLinearVelocity>%s</sleepLinearVelocity>\r\n",	fstring(a->mBody.sleepLinearVelocity));
  		nxu_fprintf(fph, "						<sleepAngularVelocity>%s</sleepAngularVelocity>\r\n",	fstring(a->mBody.sleepAngularVelocity));
  		nxu_fprintf(fph, "						<solverIterationCount>%d</solverIterationCount>\r\n",	a->mBody.solverIterationCount);
  		nxu_fprintf(fph, "						<disable_gravity>%s</disable_gravity>\r\n",	TF(a->mBody.flags	&NX_BF_DISABLE_GRAVITY));
  		nxu_fprintf(fph, "						<kinematic>%s</kinematic>\r\n",	TF(a->mBody.flags	&NX_BF_KINEMATIC));
#if NX_SDK_VERSION_NUMBER < 281
  		nxu_fprintf(fph, "						<pose_sleep_test>%s</pose_sleep_test>\r\n",	TF(a->mBody.flags	&NX_BF_POSE_SLEEP_TEST));
#endif
		nxu_fprintf(fph, "						<filter_sleep_velocity>%s</filter_sleep_velocity>\r\n",	TF(a->mBody.flags	&NX_BF_FILTER_SLEEP_VEL));
  	}

  	nxu_fprintf(fph, "					<group>%d</group>\r\n",	a->group);
  	nxu_fprintf(fph, "					<disable_collision>%s</disable_collision>\r\n",	TF(a->flags	&NX_AF_DISABLE_COLLISION));
  	nxu_fprintf(fph, "					<disable_response>%s</disable_response>\r\n",	TF(a->flags	&NX_AF_DISABLE_RESPONSE));
  	nxu_fprintf(fph, "					<lock_com>%s</lock_com>\r\n",	TF(a->flags	&NX_AF_LOCK_COM));
  	nxu_fprintf(fph, "				</technique>\r\n");
  	nxu_fprintf(fph, "			</extra>\r\n");
  	nxu_fprintf(fph, "			 </rigid_body>\r\n");
  }

  const char	*getDriveType(NxBitField32 type)
  {
  	const	char *ret	=	"UNKNOWN";

  	switch (type)
  	{
  		case NX_D6JOINT_DRIVE_POSITION:
  			ret	=	"NX_D6JOINT_DRIVE_POSITION";
  			break;
  		case NX_D6JOINT_DRIVE_VELOCITY:
  			ret	=	"NX_D6JOINT_DRIVE_VELOCITY";
  			break;
  			case (NX_D6JOINT_DRIVE_POSITION	|	NX_D6JOINT_DRIVE_VELOCITY):	ret	=	"NX_D6JOINT_DRIVE_POSITION+NX_D6JOINT_DRIVE_VELOCITY";
  			break;
  	}

  	return ret;
  }

  void	saveJointDrive(NXU_FILE	*fph,	const	char *item,	const	NxJointDriveDesc &d)
  {
  	nxu_fprintf(fph, "					<NxJointDriveDesc	id=\"%s\">\r\n", item);
  	nxu_fprintf(fph, "						<driveType>NX_D6JOINT_DRIVE_POSITION</driveType>\r\n", getDriveType(d.driveType));
  	nxu_fprintf(fph, "						<spring>%s</spring>\r\n",	fstring(d.spring));
  	nxu_fprintf(fph, "						<damping>%s</damping>\r\n",	fstring(d.damping));
  	nxu_fprintf(fph, "						<forceLimit>%s</forceLimit>\r\n",	fstring(d.forceLimit));
  	nxu_fprintf(fph, "					</NxJointDriveDesc>\r\n");
  }


  void	initMotion(NxD6JointMotion motion, NxReal	&m1, NxReal	&m2, NxReal	lmin,	NxReal lmax, bool	degrees)
  {
  	switch (motion)
  	{
  		case NX_D6JOINT_MOTION_LOCKED:
  			m1 = m2	=	0;
  			break;
  		case NX_D6JOINT_MOTION_LIMITED:
  			m1 = lmin;
  			m2 = lmax;
  			if (degrees)
  			{
  				m1 *=	RAD_TO_DEG;
  				m2 *=	RAD_TO_DEG;
  			}
  			break;
  		case NX_D6JOINT_MOTION_FREE:
  			m1 = FLT_MIN;
  			m2 = FLT_MAX;
  			break;
  	}
  }

  void	saveRigidConstraint(NXU_FILE *fph, NxJointDesc	*j,	int	scene,NxSceneDesc *sdesc)
  {


  	NxJointDesc	*jdesc = j;

  	const	char *name = j->mColladaName;


  	nxu_fprintf(fph, "			<rigid_constraint	sid=\"%s-RigidConstraint\" name=\"%s\">\r\n",	NCNAME(name),	jdesc->name);

  	NxActorDesc *a1= locateActorDesc(sdesc,j->mActor0);
  	NxActorDesc *a2= locateActorDesc(sdesc,j->mActor1);

  	if (a1)
  	{
  		nxu_fprintf(fph, "				<ref_attachment	rigid_body=\"%s-RigidBody\"	name=\"%s\">\r\n", NCNAME(a1->mColladaName), a1->name);
  	}
  	else
  	{
  		nxu_fprintf(fph, "				 <ref_attachment node=\"@world\">\r\n");
  	}

  	NxMat34	mat;

  	mat.M.setColumn(0, jdesc->localNormal0 );
  	mat.M.setColumn(1, jdesc->localAxis0 ^ jdesc->localNormal0);
  	mat.M.setColumn(2, jdesc->localAxis0);
  	mat.t	=	jdesc->localAnchor0;

  	saveTransformAxisAngle(10, fph,	mat);

  	nxu_fprintf(fph, "				</ref_attachment>\r\n");


  	if (a2)
  	{
  		nxu_fprintf(fph, "				<attachment	rigid_body=\"%s-RigidBody\"	name=\"%s\">\r\n", NCNAME(a2->mColladaName), a2->name);
  	}
  	else
  	{
  		nxu_fprintf(fph, "				 <attachment node=\"@world\">\r\n");
  	}


  	mat.M.setColumn(0, jdesc->localNormal1);
  	mat.M.setColumn(1, jdesc->localAxis1 ^ jdesc->localNormal1);
  	mat.M.setColumn(2, jdesc->localAxis1);
  	mat.t	=	jdesc->localAnchor1;

  	saveTransformAxisAngle(10, fph,	mat);

  	nxu_fprintf(fph, "				</attachment>\r\n");


  	nxu_fprintf(fph, "				<technique_common>\r\n");

  	const	char *ip = "true"; //	interpenetration

  	if (jdesc->jointFlags	&NX_JF_COLLISION_ENABLED)
  	{
  		ip = "false";
  	}


  	NxVec3 lmin(0, 0,	0);	// linear	minimum
  	NxVec3 lmax(0, 0,	0);	// linear	maximum
  	NxVec3 smin(0, 0,	0);	// swing,	cone and twist min
  	NxVec3 smax(0, 0,	0);
  	NxVec3 lspring(1,	0, 0);
  	NxVec3 aspring(1,	0, 0);

  	NxD6JointDesc	*jd	=	0;

  	if ( getJointType(jdesc->mType) ==	NX_JOINT_D6)
  	{
  		jd = (NxD6JointDesc*)jdesc;

  		initMotion(jd->xMotion,	lmin.x,	lmax.x,	 - jd->linearLimit.value,	jd->linearLimit.value, false);
  		initMotion(jd->yMotion,	lmin.y,	lmax.y,	 - jd->linearLimit.value,	jd->linearLimit.value, false);
  		initMotion(jd->zMotion,	lmin.z,	lmax.z,	 - jd->linearLimit.value,	jd->linearLimit.value, false);

  		initMotion(jd->swing1Motion, smin.x, smax.x,	-	jd->swing1Limit.value, jd->swing1Limit.value,	true);
  		initMotion(jd->swing2Motion, smin.y, smax.y,	-	jd->swing1Limit.value, jd->swing1Limit.value,	true);
  		initMotion(jd->twistMotion,	smin.z,	smax.z,	jd->twistLimit.low.value,	jd->twistLimit.high.value, true);

  		lspring.x	=	jd->linearLimit.spring;
  		lspring.y	=	jd->linearLimit.damping;
  		lspring.z	=	jd->linearLimit.value;
  		//
  		if (jd->swing2Motion ==	NX_D6JOINT_MOTION_LIMITED)
  		{
  			aspring.x	=	jd->swing2Limit.spring;
  			aspring.y	=	jd->swing2Limit.damping;
  			aspring.z	=	jd->swing2Limit.restitution;
  		}
  		if (jd->swing1Motion ==	NX_D6JOINT_MOTION_LIMITED)
  		{
  			aspring.x	=	jd->swing1Limit.spring;
  			aspring.y	=	jd->swing1Limit.damping;
  			aspring.z	=	jd->swing1Limit.restitution;
  		}
  		if (jd->twistMotion	== NX_D6JOINT_MOTION_LIMITED)
  		{
  			aspring.x	=	jd->twistLimit.low.spring;
  			aspring.y	=	jd->twistLimit.low.damping;
  			aspring.z	=	jd->twistLimit.low.restitution;

  		}
  	}

  	nxu_fprintf(fph, "					<enabled>1</enabled>\r\n");
  	nxu_fprintf(fph, "					<interpenetrate>%s</interpenetrate>\r\n",	ip);
  	nxu_fprintf(fph, "					<limits>\r\n");
  	nxu_fprintf(fph, "						<linear>\r\n");
  	nxu_fprintf(fph, "							<min>%s	%s %s</min>\r\n",	fstring(lmin.x), fstring(lmin.y),	fstring(lmin.z));
  	nxu_fprintf(fph, "							<max>%s	%s %s</max>\r\n",	fstring(lmax.x), fstring(lmax.y),	fstring(lmax.z));
  	nxu_fprintf(fph, "						 </linear>\r\n");
  	nxu_fprintf(fph, "						<swing_cone_and_twist>\r\n");
  	nxu_fprintf(fph, "							<min>%s	%s %s</min>\r\n",	fstring(smin.x), fstring(smin.y),	fstring(smin.z));
  	nxu_fprintf(fph, "							<max>%s	%s %s</max>\r\n",	fstring(smax.x), fstring(smax.y),	fstring(smax.z));
  	nxu_fprintf(fph, "						</swing_cone_and_twist>\r\n");
  	nxu_fprintf(fph, "					</limits>\r\n");
  	nxu_fprintf(fph, "					<spring>\r\n");
  	nxu_fprintf(fph, "						<linear>\r\n");
  	nxu_fprintf(fph, "							<stiffness>%s</stiffness>\r\n",	fstring(lspring.x));
  	nxu_fprintf(fph, "							<damping>%s</damping>\r\n",	fstring(lspring.y));
  	nxu_fprintf(fph, "							<target_value>%s</target_value>\r\n",	fstring(lspring.z));
  	nxu_fprintf(fph, "						</linear>\r\n");
  	nxu_fprintf(fph, "						<angular>\r\n");
  	nxu_fprintf(fph, "							<stiffness>%s</stiffness>\r\n",	fstring(aspring.x));
  	nxu_fprintf(fph, "							<damping>%s</damping>\r\n",	fstring(aspring.y));
  	nxu_fprintf(fph, "							<target_value>%s</target_value>\r\n",	fstring(aspring.z));
  	nxu_fprintf(fph, "						</angular>\r\n");
  	nxu_fprintf(fph, "					</spring>\r\n");
  	nxu_fprintf(fph, "				</technique_common>\r\n");



  	if (jd)
  	{
  		nxu_fprintf(fph, "				<extra>\r\n");
  		nxu_fprintf(fph, "					<technique profile=\"PhysX\">\r\n");


  		saveJointDrive(fph,	"xDrive",	jd->xDrive);
  		saveJointDrive(fph,	"yDrive",	jd->yDrive);
  		saveJointDrive(fph,	"zDrive",	jd->zDrive);
  		saveJointDrive(fph,	"swingDrive",	jd->swingDrive);
  		saveJointDrive(fph,	"twistDrive",	jd->twistDrive);
  		saveJointDrive(fph,	"slerpDrive",	jd->slerpDrive);


  		nxu_fprintf(fph, "					<drivePosition>%s	%s %s</drivePosition>\r\n",	fstring(jd->drivePosition.x),	fstring(jd->drivePosition.y),	fstring(jd->drivePosition.z));
  		nxu_fprintf(fph, "					<driveOrientation>%s %s	%s %s</driveOrientation>\r\n",	fstring(jd->driveOrientation.x), fstring(jd->driveOrientation.y),	fstring(jd->driveOrientation.z), fstring(jd->driveOrientation.w));
  		nxu_fprintf(fph, "					<driveLinearVelocity>%s	%s %s</driveLinearVelocity>\r\n",	fstring(jd->driveLinearVelocity.x),	fstring(jd->driveLinearVelocity.y),	fstring(jd->driveLinearVelocity.z));
  		nxu_fprintf(fph, "					<driveAngularVelocity>%s %s	%s</driveAngularVelocity>\r\n",	fstring(jd->driveAngularVelocity.x), fstring(jd->driveAngularVelocity.y),	fstring(jd->driveAngularVelocity.z));

  		if (jd->projectionMode ==	NX_JPM_POINT_MINDIST)
  		{
  			nxu_fprintf(fph, "						<projectionMode>NX_JPM_POINT_MINDIST</projectionMode>\r\n");
  		}
  		else
  		{
  			nxu_fprintf(fph, "						<projectionMode>NX_JPM_NONE</projectionMode>\r\n");
  		}

  		nxu_fprintf(fph, "						<projectionDistance>%s</projectionDistance>\r\n",	fstring(jd->projectionDistance));
  		nxu_fprintf(fph, "						<projectionAngle>%s</projectionAngle>\r\n",	fstring(jd->projectionAngle));

  		nxu_fprintf(fph, "					<gearRatio>%s</gearRatio>\r\n",	fstring(jd->gearRatio));

  		nxu_fprintf(fph, "					</technique>\r\n");
  		nxu_fprintf(fph, "				</extra>\r\n");
  	}

  	nxu_fprintf(fph, "			</rigid_constraint>\r\n");
  }

  void	savePhysicsMaterial(NXU_FILE *fph, NxMaterialDesc *m, int scene,	int	matid)
  {
  	char scratch[512];
  	sprintf(scratch, "pmat%d_%d-PhysicsMaterial",	matid, scene);

  	nxu_fprintf(fph, "		<physics_material	id=\"%s\"	name=\"%s\">\r\n", scratch,	scratch);
  	nxu_fprintf(fph, "			<technique_common>\r\n");
  	nxu_fprintf(fph, "				<dynamic_friction>%s</dynamic_friction>\r\n",	fstring(m->dynamicFriction));
  	nxu_fprintf(fph, "				<restitution>%s</restitution>\r\n",	fstring(m->restitution));
  	nxu_fprintf(fph, "				<static_friction>%s</static_friction>\r\n",	fstring(m->staticFriction));
  	nxu_fprintf(fph, "			</technique_common>\r\n");
  	nxu_fprintf(fph, "		</physics_material>\r\n");
  }

  void	saveDefaultCamera(NXU_FILE *fph)
  {
  	nxu_fprintf(fph, "	<library_cameras>\r\n");
  	nxu_fprintf(fph, "		<camera	id=\"Camera-Camera\" name=\"Camera-Camera\">\r\n");
  	nxu_fprintf(fph, "			<optics>\r\n");
  	nxu_fprintf(fph, "				<technique_common>\r\n");
  	nxu_fprintf(fph, "					<perspective>\r\n");
  	nxu_fprintf(fph, "						<yfov>49.13434</yfov>\r\n");
  	nxu_fprintf(fph, "						<znear>0.1</znear>\r\n");
  	nxu_fprintf(fph, "						<zfar>100.0</zfar>\r\n");
  	nxu_fprintf(fph, "					</perspective>\r\n");
  	nxu_fprintf(fph, "				</technique_common>\r\n");
  	nxu_fprintf(fph, "			</optics>\r\n");
  	nxu_fprintf(fph, "		</camera>\r\n");
  	nxu_fprintf(fph, "	</library_cameras>\r\n");
  }

  void	saveLibraryEffects(NXU_FILE	*fph)
  {
  	nxu_fprintf(fph, "	 <library_materials>\r\n");
  	nxu_fprintf(fph, "			<material	id=\"Material\"	name=\"Material\">\r\n");
  	nxu_fprintf(fph, "				 <instance_effect	url=\"#Material-fx\"></instance_effect>\r\n");
  	nxu_fprintf(fph, "			</material>\r\n");
  	nxu_fprintf(fph, "	 </library_materials>\r\n");
  	nxu_fprintf(fph, "	 <library_effects>\r\n");
  	nxu_fprintf(fph, "			<effect	id=\"Material-fx\" name=\"Material\">\r\n");
  	nxu_fprintf(fph, "				 <profile_COMMON>\r\n");
  	nxu_fprintf(fph, "						<technique id=\"Material-fx-COMMON\" sid=\"COMMON\">\r\n");
  	nxu_fprintf(fph, "							 <phong>\r\n");
  	nxu_fprintf(fph, "									<ambient>\r\n");
  	nxu_fprintf(fph, "										 <color>0.803922 0.588235	0.92549	1</color>\r\n");
  	nxu_fprintf(fph, "									</ambient>\r\n");
  	nxu_fprintf(fph, "									<diffuse>\r\n");
  	nxu_fprintf(fph, "										 <color>0.803922 0.588235	0.92549	1</color>\r\n");
  	nxu_fprintf(fph, "									</diffuse>\r\n");
  	nxu_fprintf(fph, "									<specular>\r\n");
  	nxu_fprintf(fph, "										 <color>0.631373 0.631373	0.631373 1</color>\r\n");
  	nxu_fprintf(fph, "									</specular>\r\n");
  	nxu_fprintf(fph, "									<shininess>\r\n");
  	nxu_fprintf(fph, "										 <float>1</float>\r\n");
  	nxu_fprintf(fph, "									</shininess>\r\n");
  	nxu_fprintf(fph, "									<reflective>\r\n");
  	nxu_fprintf(fph, "										 <color>0	0	0	1</color>\r\n");
  	nxu_fprintf(fph, "									</reflective>\r\n");
  	nxu_fprintf(fph, "									<transparent>\r\n");
  	nxu_fprintf(fph, "										 <color>1	1	1	1</color>\r\n");
  	nxu_fprintf(fph, "									</transparent>\r\n");
  	nxu_fprintf(fph, "									<transparency>\r\n");
  	nxu_fprintf(fph, "										 <float>0</float>\r\n");
  	nxu_fprintf(fph, "									</transparency>\r\n");
  	nxu_fprintf(fph, "							 </phong>\r\n");
  	nxu_fprintf(fph, "						</technique>\r\n");
  	nxu_fprintf(fph, "				 </profile_COMMON>\r\n");
  	nxu_fprintf(fph, "			</effect>\r\n");
  	nxu_fprintf(fph, "	 </library_effects>\r\n");
  }

  void writeMesh(NXU_FILE	*fph,	const	char *name,NxArray< NxVec3 > &_vertices,NxArray< NxTri > &_indices,bool isConvex)
  {
  	unsigned int vcount         = _vertices.size();
  	unsigned int tcount         = _indices.size();
  	if ( vcount && tcount )
  	{
    	const float *vertices       = &_vertices[0].x;
    	const unsigned int *indices = &_indices[0].a;

    	if (isConvex)
    	{
    		nxu_fprintf(fph, "			<convex_mesh>\r\n");
    	}
    	else
    	{
    		nxu_fprintf(fph, "			<mesh>\r\n");
    	}

    	nxu_fprintf(fph, "				<source	id=\"%s-Position\">\r\n",	name);
    	nxu_fprintf(fph, "					<float_array count=\"%d\"	id=\"%s-Position-array\">\r\n",	vcount *3, name);
    	nxu_fprintf(fph, "						");

    	for	(unsigned	int	i	=	0; i < vcount; i++)
    	{
    		const	float	*p = &vertices[i *3];
    		nxu_fprintf(fph, "%s %s	%s	", fstring(p[0]),	fstring(p[1]), fstring(p[2]));

    		if (((i	+	1) &3) ==	0	&& (i	+	1) < vcount)
    		{
    			nxu_fprintf(fph, "\r\n");
    			nxu_fprintf(fph, "						");
    		}
    	}

    	nxu_fprintf(fph, "\r\n");
    	nxu_fprintf(fph, "					</float_array>\r\n");
    	nxu_fprintf(fph, "					<technique_common>\r\n");
    	nxu_fprintf(fph, "						<accessor	count=\"%d\" source=\"#%s-Position-array\" stride=\"3\">\r\n", vcount, name);
    	nxu_fprintf(fph, "							<param name=\"X\"	type=\"float\"/>\r\n");
    	nxu_fprintf(fph, "							<param name=\"Y\"	type=\"float\"/>\r\n");
    	nxu_fprintf(fph, "							<param name=\"Z\"	type=\"float\"/>\r\n");
    	nxu_fprintf(fph, "						</accessor>\r\n");
    	nxu_fprintf(fph, "					 </technique_common>\r\n");
    	nxu_fprintf(fph, "				 </source>\r\n");


    	nxu_fprintf(fph, "				<vertices	id=\"%s-Vertex\">\r\n",	name);
    	nxu_fprintf(fph, "					<input semantic=\"POSITION\" source=\"#%s-Position\"/>\r\n", name);
    	nxu_fprintf(fph, "				</vertices>\r\n");
    	nxu_fprintf(fph, "				<triangles material=\"Material\" count=\"%d\">\r\n", tcount);
    	nxu_fprintf(fph, "					<input offset=\"0\"	semantic=\"VERTEX\"	source=\"#%s-Vertex\"/>\r\n",	name);
    	nxu_fprintf(fph, "				 <p>");

    	if ( 1 )
    	{
      	for	(unsigned	int	i	=	0; i < tcount; i++)
      	{
      		const	unsigned int *idx	=	&indices[i *3];
      		nxu_fprintf(fph, "%d %d	%d	", idx[0], idx[1], idx[2]);
      		if (((i	+	1) &3) ==	0)
      		{
      			nxu_fprintf(fph, "\r\n");
      			nxu_fprintf(fph, "				 ");
      		}
      	}
      }
    	nxu_fprintf(fph, "</p>\r\n");
    	nxu_fprintf(fph, "				</triangles>\r\n");

    	if (isConvex)
    	{
    		nxu_fprintf(fph, "			</convex_mesh>\r\n");
    	}
    	else
    	{
    		nxu_fprintf(fph, "			</mesh>\r\n");
    	}
    }
  }

  void saveNode(NXU_FILE *fph, NxSceneInstanceDesc	*nsi,	unsigned int i)
  {
  	nxu_fprintf(fph, "			<node	id=\"SceneInstance%d-Node\"	name=\"%s\"	type=\"NODE\">\r\n", i,	nsi->mSceneName);
  	saveTransformAxisAngle(8,	fph, nsi->mRootNode);
  	nxu_fprintf(fph, "			</node>\r\n");
  }

  void saveGeometry(NXU_FILE *fph, NxActorDesc	*a)
  {
  }


  void saveGeometry(NXU_FILE *fph, NxTriangleMeshDesc *d, int i)
  {
  	nxu_fprintf(fph, "		<geometry	id=\"%s-Mesh\" name=\"%s-Mesh\">\r\n", d->mId, d->mId);
  	writeMesh(fph,d->mId, d->mPoints,d->mTriangles,false);
  	nxu_fprintf(fph, "		</geometry>\r\n");
  }

  void saveGeometry(NXU_FILE *fph, NxConvexMeshDesc *d, int i)
  {
  	nxu_fprintf(fph, "		<geometry	id=\"%s-Mesh\" name=\"%s-Mesh\">\r\n", d->mId, d->mId);
  	writeMesh(fph,d->mId,d->mPoints, d->mTriangles, true);
  	nxu_fprintf(fph, "		</geometry>\r\n");
  }

  bool saveColladaFile(NxuPhysicsCollection	*c,	NXU_FILE *fph, const char	*fname)
  {
		mCollection = c;

  	nxu_fprintf(fph, "<?xml	version=\"1.0\"	encoding=\"utf-8\"?>\r\n");
  	nxu_fprintf(fph, "<COLLADA version=\"1.4.0\" xmlns=\"http://www.collada.org/2005/11/COLLADASchema\">\r\n");
  	nxu_fprintf(fph, "	<asset>\r\n");
  	nxu_fprintf(fph, "		<contributor>\r\n");
  	nxu_fprintf(fph, "			<author>NxuStream2 converter - http://www.nvidia.com</author>\r\n");
  	nxu_fprintf(fph, "			<authoring_tool>PhysX	Rocket,	PhysX	Viewer,	or CreateDynamics</authoring_tool>\r\n");
  	nxu_fprintf(fph, "			<comments>questions	to:	jratcliff@ageia.com</comments>\r\n");
  	//	nxu_fprintf(fph,"			 <copyright></copyright>\r\n");
  	nxu_fprintf(fph, "			<source_data>%s</source_data>\r\n",	fname);
  	nxu_fprintf(fph, "		</contributor>\r\n");
  	//	nxu_fprintf(fph,"		 <created></created>\r\n");
  	//	nxu_fprintf(fph,"		 <modified></modified>\r\n");
  	nxu_fprintf(fph, "		<unit	meter=\"1\"	name=\"meter\"/>\r\n");
  	nxu_fprintf(fph, "		<up_axis>Y_UP</up_axis>\r\n");
  	nxu_fprintf(fph, "	</asset>\r\n");


  	// first we	assign the collada name	fields we	plan to	use.
  	// ok..now have	to save	out	the	graphics representation	of each	object.
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			NxSceneDesc *s = c->mScenes[i];
  			if (1)
  			{
  				int	acount = s->mActors.size();
  				for	(int j = 0;	j	<	acount;	j++)
  				{
  					NxActorDesc *a	=	s->mActors[j];
  					a->mColladaName	=	getName("Actor", i,	j);
  				}
  			}
  			if (1)
  			{
  				int	jcount = s->mJoints.size();
  				for	(int j = 0;	j	<	jcount;	j++)
  				{
  					NxJointDesc *joint	=	s->mJoints[j];
  					joint->mColladaName	=	getName("Joint", i,	j);
  				}
  			}
  		}
  	}


  	saveLibraryEffects(fph);


  	nxu_fprintf(fph, "	<library_geometries>\r\n");

  	// ok..first we	have to	save out all of	the	convex meshes	and	triangle meshes...
  	if (1)
  	{
  		for	(unsigned	int	i	=	0; i < c->mConvexMeshes.size(); i++)
  		{
  			NxConvexMeshDesc	*d = c->mConvexMeshes[i];
  			saveGeometry(fph,	d, i);
  		}
  	}

  	if (1)
  	{
  		for	(unsigned	int	i	=	0; i < c->mTriangleMeshes.size(); i++)
  		{
  			NxTriangleMeshDesc	*d = c->mTriangleMeshes[i];
  			saveGeometry(fph,	d, i);
  		}
  	}

  	// ok..now have	to save	out	the	graphics representation	of each	object.
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			NxSceneDesc *s = c->mScenes[i];
  			int	acount = s->mActors.size();
  			for	(int j = 0;	j	<	acount;	j++)
  			{
  				NxActorDesc *a	=	s->mActors[j];
  				saveGeometry(fph,	a);
  			}
  		}
  	}



  	nxu_fprintf(fph, "	</library_geometries>\r\n");

  	nxu_fprintf(fph, "	<library_visual_scenes>\r\n");
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			NxSceneDesc *s = c->mScenes[i];
  			saveVisualScene(fph, s,	i);
  		}
  	}
  	if (c->mSceneInstances.size())
  	{
  		nxu_fprintf(fph, "		<visual_scene	id=\"SceneInstances\"	name=\"SceneInstances\">\r\n");
  		for	(unsigned	int	i	=	0; i < c->mSceneInstances.size();	i++)
  		{
  			NxSceneInstanceDesc *nsi	=	c->mSceneInstances[i];
  			saveNode(fph,	nsi, i);
  		}
  		nxu_fprintf(fph, "			</visual_scene>\r\n");
  	}
  	nxu_fprintf(fph, "	</library_visual_scenes>\r\n");


  	nxu_fprintf(fph, "	<library_physics_materials>\r\n");
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			NxSceneDesc *s = c->mScenes[i];
  			int	mcount = s->mMaterials.size();
  			for	(int j = 0;	j	<	mcount;	j++)
  			{
  				NxMaterialDesc	*m = s->mMaterials[j];
  				savePhysicsMaterial(fph, m,	i, j);
  			}
  		}
  	}
  	nxu_fprintf(fph, "	</library_physics_materials>\r\n");



  	nxu_fprintf(fph, "	<library_physics_models>\r\n");

  	// ok..now have	to save	out	the	graphics representation	of each	object.
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			nxu_fprintf(fph, "		<physics_model id=\"Scene%d-PhysicsModel\">\r\n",	i);
  			NxSceneDesc *s = c->mScenes[i];

  			if (1)
  			{
  				int	acount = s->mActors.size();
  				for	(int j = 0;	j	<	acount;	j++)
  				{
  					NxActorDesc *a	=	s->mActors[j];
  					saveRigidBody(fph, a,	i);
  				}
  			}

  			if (1)
  			{
  				int	jcount = s->mJoints.size();
  				for	(int j = 0;	j	<	jcount;	j++)
  				{
  					NxJointDesc *joint	=	s->mJoints[j];
  					saveRigidConstraint(fph, joint,	i,s);
  				}
  			}
  			nxu_fprintf(fph, "		</physics_model>\r\n");
  		}

  		if (1)
  		{
  			int	scount = c->mScenes.size();
  			for	(int i = 0;	i	<	scount;	i++)
  			{
  				NxSceneDesc *s = c->mScenes[i];
  				savePhysicsScene(fph,	s, i);
  			}
  		}

  	}

  	nxu_fprintf(fph, "	</library_physics_models>\r\n");



  	nxu_fprintf(fph, "	<library_physics_scenes>\r\n");

  	nxu_fprintf(fph, "		<physics_scene id=\"SceneInstances-Physics\">\r\n");

  	if (c->mSceneInstances.size())
  	{
  		for	(unsigned	int	i	=	0; i < c->mSceneInstances.size();	i++)
  		{
  			NxSceneInstanceDesc *nsi	=	c->mSceneInstances[i];
  			NxU32	index;
  			NxSceneDesc *sd = locateSceneDesc(mCollection, nsi->mSceneName, index);
  			if (sd)
  			{
  				nxu_fprintf(fph, "			<instance_physics_model	url=\"#Scene%d-PhysicsInstance\" parent=\"#SceneInstance%d-Node\"/>\r\n",	index, i);
  			}
  		}
  	}
  	else
  	{
  		for	(unsigned	int	i	=	0; i < c->mScenes.size(); i++)
  		{
  			// NxSceneDesc *sd = c->mScenes[i];
  			nxu_fprintf(fph, "			<instance_physics_model	url=\"#Scene%d-PhysicsInstance\"/>\r\n", i);
  		}
  	}


  	NxVec3 gravity(0,	 - 9.8f, 0);

  	if (c->mScenes.size())
  	{
  		gravity	=	c->mScenes[0]->gravity;
  	}

  	nxu_fprintf(fph, "			<technique_common>\r\n");
  	nxu_fprintf(fph, "				<gravity>%s	%s %s</gravity>\r\n",	fstring(gravity.x),	fstring(gravity.y),	fstring(gravity.z));
  	nxu_fprintf(fph, "			</technique_common>\r\n");

  	nxu_fprintf(fph, "		</physics_scene>\r\n");


  	nxu_fprintf(fph, "	</library_physics_scenes>\r\n");

  	nxu_fprintf(fph, "	<scene>\r\n");
  	nxu_fprintf(fph, "		<instance_physics_scene	url=\"#SceneInstances-Physics\"/>\r\n");
  	nxu_fprintf(fph, "	</scene>\r\n");

  	// set all of	the	COLLADA	name fields	back to	null.
  	if (1)
  	{
  		int	scount = c->mScenes.size();
  		for	(int i = 0;	i	<	scount;	i++)
  		{
  			NxSceneDesc *s = c->mScenes[i];
  			int	acount = s->mActors.size();
  			for	(int j = 0;	j	<	acount;	j++)
  			{
  				NxActorDesc *a	=	s->mActors[j];
  				a->mColladaName	=	0;
  			}
  		}
  	}

  	nxu_fprintf(fph, "</COLLADA>\r\n");

  	gActorNames.releaseGlobalStr();

  	return true;
  }

private:
  NxuPhysicsCollection	*mCollection;
};


bool saveColladaFile(NxuPhysicsCollection	*c,	NXU_FILE *fph, const char	*asset_name)
{
  SaveCollada scf;
  return scf.saveColladaFile(c,fph,asset_name);
}

};
