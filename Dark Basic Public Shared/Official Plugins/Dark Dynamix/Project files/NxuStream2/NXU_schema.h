#ifndef NXU_schema_H
#define NXU_schema_H

//*****************************************************************************
//*****************************************************************************
//*** WARNING!! This is *auto-generated source code! DO NOT HAND EDIT!
//*****************************************************************************
//*****************************************************************************
#include "NXU_string.h"
#include "NXU_SchemaTypes.h"
#include "NxSimpleTypes.h"
#include "NxArray.h"
#include "NxVec3.h"
#include "NxQuat.h"
#include "NxMat33.h"
#include "NxMat34.h"
#include "NxPlane.h"
#include "NxBounds3.h"
#include <NxVersionNumber.h>
#include <NxPhysics.h>
#include <NxPMap.h>
#include <NxSceneStats.h>
#include <NxStream.h>

#define HAVE_SCHEMA 1

namespace NXU
{

class SchemaStream;
class CustomCopy;
class NxCCDSkeletonDesc;
void validateEnums(void);
/**
\brief Parameters enums to be used as the 1st arg to setParameter or getParameter.
*/
enum NxParameter
{
/**
\brief DEPRECATED! Do not use!
*/
  NX_PENALTY_FORCE,
/**
\brief Default value for NxShapeDesc::skinWidth, see for more info. Range: [0, inf) Default: 0.025 Unit: distance.
*/
  NX_SKIN_WIDTH,
/**
\brief The default linear velocity, squared, below which objects start going to sleep. Note: Only makes sense when the NX_BF_ENERGY_SLEEP_TEST is not set. Range: [0, inf) Default: (0.15*0.15)
*/
  NX_DEFAULT_SLEEP_LIN_VEL_SQUARED,
/**
\brief The default angular velocity, squared, below which objects start going to sleep. Note: Only makes sense when the NX_BF_ENERGY_SLEEP_TEST is not set. Range: [0, inf) Default: (0.14*0.14)
*/
  NX_DEFAULT_SLEEP_ANG_VEL_SQUARED,
/**
\brief A contact with a relative velocity below this will not bounce. Range: (-inf, 0] Default: -2
*/
  NX_BOUNCE_THRESHOLD,
/**
\brief This lets the user scale the magnitude of the dynamic friction applied to all objects. Range: [0, inf) Default: 1
*/
  NX_DYN_FRICT_SCALING,
/**
\brief This lets the user scale the magnitude of the static friction applied to all objects. Range: [0, inf) Default: 1
*/
  NX_STA_FRICT_SCALING,
/**
\brief See the comment for NxActor::setMaxAngularVelocity() for details. Range: [0, inf) Default: 7
*/
  NX_MAX_ANGULAR_VELOCITY,
/**
\brief Enable/disable continuous collision detection (0.0f to disable). Range: [0, inf) Default: 0.0
*/
  NX_CONTINUOUS_CD,
/**
\brief This overall visualization scale gets multiplied with the individual scales. Setting to zero turns ignores all visualizations. Default is 0.  The below settings permit the debug visualization of various simulation properties. The setting is either zero, in which case the property is not drawn. Otherwise it is a scaling factor that determines the size of the visualization widgets. Only bodies and joints for which visualization is turned on using setFlag(VISUALIZE) are visualized. Contacts are visualized if they involve a body which is being visualized. Default is 0. Notes: to see any visualization, you have to set NX_VISUALIZATION_SCALE to nonzero first. the scale factor has been introduced because it's difficult (if not impossible) to come up with a good scale for 3D vectors. Normals are normalized and their length is always 1. But it doesn't mean we should render a line of length 1. Depending on your objects/scene, this might be completely invisible or extremely huge. That's why the scale factor is here, to let you tune the length until it's ok in your scene. however, things like collision shapes aren't ambiguous. They are clearly defined for example by the triangles & polygons themselves, and there's no point in scaling that. So the visualization widgets are only scaled when it makes sense. Range: [0, inf) Default: 0
*/
  NX_VISUALIZATION_SCALE,
/**
\brief Visualize the world axes. 
*/
  NX_VISUALIZE_WORLD_AXES,
/**
\brief Visualize a bodies axes.
*/
  NX_VISUALIZE_BODY_AXES,
/**
\brief Visualize a body's mass axes. This visualization is also useful for visualizing the sleep state of bodies. Sleeping bodies are drawn in black, while awake bodies are drawn in white. If the body is sleeping and part of a sleeping group, it is drawn in red.
*/
  NX_VISUALIZE_BODY_MASS_AXES,
/**
\brief Visualize the bodies linear velocity. 
*/
  NX_VISUALIZE_BODY_LIN_VELOCITY,
/**
\brief Visualize the bodies angular velocity.
*/
  NX_VISUALIZE_BODY_ANG_VELOCITY,
/**
\brief Visualize the bodies linear momentum. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_LIN_MOMENTUM,
#endif
/**
\brief Visualize the bodies angular momentum.
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_ANG_MOMENTUM,
#endif
/**
\brief Visualize the bodies linear acceleration. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_LIN_ACCEL,
#endif
/**
\brief Visualize the bodies angular acceleration. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_ANG_ACCEL,
#endif
/**
\brief Visualize linear forces apllied to bodies. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_LIN_FORCE,
#endif
/**
\brief Visualize angular force applied to bodies.
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_ANG_FORCE,
#endif
/**
\brief Visualize bodies, reduced set of visualizations.
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_REDUCED,
#endif
/**
\brief Visualize the bodies linear momentum. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY15,
#endif
/**
\brief Visualize the bodies angular momentum.
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY16,
#endif
/**
\brief Visualize the bodies linear acceleration. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY17,
#endif
/**
\brief Visualize the bodies angular acceleration. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY18,
#endif
/**
\brief Visualize linear forces apllied to bodies. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY19,
#endif
/**
\brief Visualize angular force applied to bodies.
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY20,
#endif
/**
\brief Visualize bodies, reduced set of visualizations.
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY21,
#endif
/**
\brief Visualize joint groups. 
*/
  NX_VISUALIZE_BODY_JOINT_GROUPS,
/**
\brief Visualize the contact list. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_CONTACT_LIST,
#endif
/**
\brief Visualize body joint lists.
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_JOINT_LIST,
#endif
/**
\brief Visualize body damping. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_DAMPING,
#endif
/**
\brief Visualize sleeping bodies. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_BODY_SLEEP,
#endif
/**
\brief Visualize the contact list. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY23,
#endif
/**
\brief Visualize body joint lists.
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY24,
#endif
/**
\brief Visualize body damping. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY25,
#endif
/**
\brief Visualize sleeping bodies. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY26,
#endif
/**
\brief Visualize local joint axes (including drive targets, if any).
*/
  NX_VISUALIZE_JOINT_LOCAL_AXES,
/**
\brief Visualize joint world axes.
*/
  NX_VISUALIZE_JOINT_WORLD_AXES,
/**
\brief Visualize joint limits. 
*/
  NX_VISUALIZE_JOINT_LIMITS,
/**
\brief Visualize joint errors.
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_JOINT_ERROR,
#endif
/**
\brief Visualize joint forces. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_JOINT_FORCE,
#endif
/**
\brief Visualize joints, reduced set of visualizations. 
*/
#if NX_SDK_VERSION_NUMBER < 270 && NX_SDK_VERSION_NUMBER != 263
  NX_VISUALIZE_JOINT_REDUCED,
#endif
/**
\brief Visualize joint errors.
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY30,
#endif
/**
\brief Visualize joint forces. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY31,
#endif
/**
\brief Visualize joints, reduced set of visualizations. 
*/
#if NX_SDK_VERSION_NUMBER >=270
  NX_DUMMY32,
#endif
/**
\brief Visualize contact points. 
*/
  NX_VISUALIZE_CONTACT_POINT,
/**
\brief Visualize contact normals. 
*/
  NX_VISUALIZE_CONTACT_NORMAL,
/**
\brief Visualize contact errors.
*/
  NX_VISUALIZE_CONTACT_ERROR,
/**
\brief Visualize Contact forces.
*/
  NX_VISUALIZE_CONTACT_FORCE,
/**
\brief Visualize actor axes.
*/
  NX_VISUALIZE_ACTOR_AXES,
/**
\brief Visualize bounds (AABBs in world space). 
*/
  NX_VISUALIZE_COLLISION_AABBS,
/**
\brief Shape visualization. 
*/
  NX_VISUALIZE_COLLISION_SHAPES,
/**
\brief Shape axis visualization. 
*/
  NX_VISUALIZE_COLLISION_AXES,
/**
\brief Compound visualization (compound AABBs in world space).
*/
  NX_VISUALIZE_COLLISION_COMPOUNDS,
/**
\brief Mesh & convex vertex normals.
*/
  NX_VISUALIZE_COLLISION_VNORMALS,
/**
\brief Mesh & convex face normals. 
*/
  NX_VISUALIZE_COLLISION_FNORMALS,
/**
\brief Active edges for meshes. 
*/
  NX_VISUALIZE_COLLISION_EDGES,
/**
\brief Bounding spheres. 
*/
  NX_VISUALIZE_COLLISION_SPHERES,
/**
\brief SAP structures.
*/
#if NX_SDK_VERSION_NUMBER < 280
  NX_VISUALIZE_COLLISION_SAP,
#endif
/**
\brief SAP structures.
*/
#if NX_SDK_VERSION_NUMBER >=280
  NX_DUMMY46,
#endif
/**
\brief Static pruning structures. 
*/
  NX_VISUALIZE_COLLISION_STATIC,
/**
\brief Dynamic pruning structures.
*/
  NX_VISUALIZE_COLLISION_DYNAMIC,
/**
\brief 'Free' pruning structures 
*/
  NX_VISUALIZE_COLLISION_FREE,
/**
\brief Visualize the CCD tests. 
*/
  NX_VISUALIZE_COLLISION_CCD,
/**
\brief Visualize CCD Skeletons.
*/
  NX_VISUALIZE_COLLISION_SKELETONS,
/**
\brief Emitter visualization. 
*/
  NX_VISUALIZE_FLUID_EMITTERS,
/**
\brief Particle position visualization.
*/
  NX_VISUALIZE_FLUID_POSITION,
/**
\brief Particle velocity visualization. 
*/
  NX_VISUALIZE_FLUID_VELOCITY,
/**
\brief Particle kernel radius visualization. 
*/
  NX_VISUALIZE_FLUID_KERNEL_RADIUS,
/**
\brief Fluid AABB visualization.
*/
  NX_VISUALIZE_FLUID_BOUNDS,
/**
\brief Fluid Packet visualization. 
*/
  NX_VISUALIZE_FLUID_PACKETS,
/**
\brief Fluid motion limit visualization.
*/
  NX_VISUALIZE_FLUID_MOTION_LIMIT,
/**
\brief Fluid dynamic collision visualization. 
*/
  NX_VISUALIZE_FLUID_DYN_COLLISION,
/**
\brief Fluid static collision visualization. 
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_VISUALIZE_FLUID_STC_COLLISION,
#endif
/**
\brief Fluid available mesh packets visualization. 
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_VISUALIZE_FLUID_MESH_PACKETS,
#endif
/**
\brief Fluid drain shape visualization. 
*/
  NX_VISUALIZE_FLUID_DRAINS,
/**
\brief Cloth mesh visualization. 
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_VISUALIZE_CLOTH_MESH,
#endif
/**
\brief Cloth rigid body collision visualization. 
*/
  NX_VISUALIZE_CLOTH_COLLISIONS,
/**
\brief Cloth cloth collision visualization. 
*/
  NX_VISUALIZE_CLOTH_SELFCOLLISIONS,
/**
\brief Cloth clustering for the PPU simulation visualization.
*/
  NX_VISUALIZE_CLOTH_WORKPACKETS,
/**
\brief Cloth sleeping visualization. 
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_VISUALIZE_CLOTH_SLEEP,
#endif
/**
\brief Used to enable adaptive forces to accelerate convergence of the solver. Range: [0, inf) Default: 1.0
*/
  NX_ADAPTIVE_FORCE,
/**
\brief Controls default filtering for jointed bodies. True means collision is disabled. Range: {true, false} Default: true See also: NX_JF_COLLISION_ENABLED
*/
  NX_COLL_VETO_JOINTED,
/**
\brief Controls whether two touching triggers generate a callback or not. Range: {true, false} Default: true
*/
  NX_TRIGGER_TRIGGER_CALLBACK,
/**
\brief Internal, used for debugging and testing. Not to be used. 
*/
  NX_SELECT_HW_ALGO,
/**
\brief Internal, used for debugging and testing. Not to be used. 
*/
  NX_VISUALIZE_ACTIVE_VERTICES,
/**
\brief Distance epsilon for the CCD algorithm. Range: [0, inf) Default: 0.01
*/
  NX_CCD_EPSILON,
/**
\brief Used to accelerate solver. Range: [0, inf) Default: 0
*/
  NX_SOLVER_CONVERGENCE_THRESHOLD,
/**
\brief Used to accelerate HW Broad Phase. Range: [0, inf) Default: 0.001
*/
  NX_BBOX_NOISE_LEVEL,
/**
\brief Used to set the sweep cache size. Range: [0, inf) Default: 5.0
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_IMPLICIT_SWEEP_CACHE_SIZE,
#endif
/**
\brief The default sleep energy threshold. Objects with an energy below this threshold are allowed to go to sleep. Note: Only used when the NX_BF_ENERGY_SLEEP_TEST flag is set.  Range: [0, inf) Default: 0.005
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_DEFAULT_SLEEP_ENERGY,
#endif
/**
\brief Constant for the maximum number of packets per fluid.  Used to compute the fluid packet buffer size in NxFluidPacketData
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_CONSTANT_FLUID_MAX_PACKETS,
#endif
/**
\brief Constant for the maximum number of new fluid particles per frame
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP,
#endif
/**
\brief Size in distance of a single cell in the HSM paging grid.
*/
#if NX_SDK_VERSION_NUMBER == 250 || NX_SDK_VERSION_NUMBER == 251
  NX_GRID_HASH_CELL_SIZE,
#endif
/**
\brief Cloth tearable vertices visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_CLOTH_TEARABLE_VERTICES,
#endif
/**
\brief Cloth tearing visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_CLOTH_TEARING,
#endif
/**
\brief Cloth attachments visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_CLOTH_ATTACHMENT,
#endif
/**
\brief Soft body mesh visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_MESH,
#endif
/**
\brief Soft body rigid body collision visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_COLLISIONS,
#endif
/**
\brief Soft body clusering for the PPU simulation visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_WORKPACKETS,
#endif
/**
\brief Soft body sleeping visualization
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_SLEEP,
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES,
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_TEARING,
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_SOFTBODY_ATTACHMENT,
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_VISUALIZE_FLUID_PACKET_DATA,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_VISUALIZE_FORCE_FIELDS,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_VISUALIZE_CLOTH_VALIDBOUNDS,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_VISUALIZE_SOFTBODY_VALIDBOUNDS,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_VISUALIZE_CLOTH_SLEEP_VERTEX,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX,
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_ASYNCHRONOUS_MESH_CREATION,
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON,
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  NX_IMPROVED_SPRING_SOLVER,
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  NX_FAST_MASSIVE_BP_VOLUME_DELETION,
#endif
#if NX_SDK_VERSION_NUMBER >= 281
  NX_LEGACY_JOINT_DRIVE,
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_VISUALIZE_CLOTH_HIERARCHY,
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_VISUALIZE_CLOTH_HARD_STRETCHING_LIMITATION,
#endif
/**
\brief Used to designate the correct total number of parameters
*/
  NX_PARAMS_NUM_VALUES
};
const char * EnumToString(NxParameter v);
bool         StringToEnum(const char *str,NxParameter &v);

#if NX_SDK_VERSION_NUMBER >= 260
enum NxCompartmentType
{
  NX_SCT_RIGIDBODY,
  NX_SCT_FLUID,
  NX_SCT_CLOTH
};
const char * EnumToString(NxCompartmentType v);
bool         StringToEnum(const char *str,NxCompartmentType &v);

#endif
#if NX_SDK_VERSION_NUMBER >= 262
enum NxPruningStructure
{
  NX_PRUNING_NONE,
  NX_PRUNING_OCTREE,
  NX_PRUNING_QUADTREE,
  NX_PRUNING_DYNAMIC_AABB_TREE,
  NX_PRUNING_STATIC_AABB_TREE
};
const char * EnumToString(NxPruningStructure v);
bool         StringToEnum(const char *str,NxPruningStructure &v);

#endif
#if NX_SDK_VERSION_NUMBER >= 280
enum NxBroadPhaseType
{
  NX_BP_TYPE_SAP_SINGLE,
  NX_BP_TYPE_SAP_MULTI
};
const char * EnumToString(NxBroadPhaseType v);
bool         StringToEnum(const char *str,NxBroadPhaseType &v);

#endif
#if NX_SDK_VERSION_NUMBER >= 270
enum NxCompartmentFlag
{
  NX_CF_SLEEP_NOTIFICATION       = (1<<0),
#if NX_SDK_VERSION_NUMBER >= 271
  NX_CF_CONTINUOUS_CD            = (1<<1),
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  NX_CF_RESTRICTED_SCENE         = (1<<2),
#endif
#if NX_SDK_VERSION_NUMBER >= 271
  NX_CF_INHERIT_SETTINGS         = (1<<3)
#endif
};
const char * EnumToString(NxCompartmentFlag v);
bool         StringToEnum(const char *str,NxCompartmentFlag &v);

#endif
#if NX_SDK_VERSION_NUMBER >= 260
#endif
/**
\brief   Flags which describe the format and behavior of a convex mesh. 
*/
enum NxConvexFlags
{
/**
\brief Used to flip the normals if the winding order is reversed. The Nx libraries assume that the face normal of a triangle with vertices [a,b,c] can be computed as: edge1 = b-a edge2 = c-a face_normal = edge1 x edge2. Note: this is the same as counterclockwise winding in a right handed graphics coordinate system.
*/
  NX_CF_FLIPNORMALS              = (1<<0),
/**
\brief Denotes the use of 16-bit vertex indices (otherwise, 32-bit indices are used)
*/
  NX_CF_16_BIT_INDICES           = (1<<1),
/**
\brief Automatically recomputes the hull from the vertices. If this flag is not set, you must provide the entire geometry manually.
*/
  NX_CF_COMPUTE_CONVEX           = (1<<2),
/**
\brief Inflates the convex object according to skin width. Note: This flag is only used in combination with NX_CF_COMPUTE_CONVEX. See also: NxCookingParams
*/
  NX_CF_INFLATE_CONVEX           = (1<<3),
/**
\brief Uses legacy hull algorithm, which doesn't inflate convex according to skin width.  Note: This flag is only used in combination with NX_CF_COMPUTE_CONVEX, and must not be used for convex meshes to be simulated in HW. Deprecated: This cooking method is not compatible with the PhysX HW simulation and should not be used.  Warning: Legacy function See also: NxCookingParams
*/
#if NX_SDK_VERSION_NUMBER < 280
  NX_CF_USE_LEGACY_COOKER        = (1<<4),
#endif
/**
\brief placeholder
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_CF_DUMMY4                   = (1<<4),
#endif
/**
\brief Instructs cooking to save normals uncompressed. The cooked hull data will be larger, but will load faster. See also: NxCookingParams
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_CF_USE_UNCOMPRESSED_NORMALS = (1<<5)
#endif
};
const char * EnumToString(NxConvexFlags v);
bool         StringToEnum(const char *str,NxConvexFlags &v);

enum NxHeightFieldAxis
{
  NX_X,
  NX_Y,
  NX_Z
};
const char * EnumToString(NxHeightFieldAxis v);
bool         StringToEnum(const char *str,NxHeightFieldAxis &v);

/**
\brief Enum with flag values to be used in NxSimpleTriangleMesh::flags.
*/
enum NxMeshFlags
{
/**
\brief Specifies if the SDK should flip normals. The Nx libraries assume that the face normal of a triangle with vertices [a,b,c] can be computed as: edge1 = b-a edge2 = c-a face_normal = edge1 x edge2. Note: This is the same as a counterclockwise winding in a right handed coordinate system or alternatively a clockwise winding order in a left handed coordinate system. If this does not match the winding order for your triangles, raise the below flag.
*/
  NX_MF_FLIPNORMALS              = (1<<0),
/**
\brief Indicates the triangles use 16 bit indices
*/
  NX_MF_16_BIT_INDICES           = (1<<1),
/**
\brief Indicates the mesh is cooked for hardware
*/
  NX_MF_HARDWARE_MESH            = (1<<2)
};
const char * EnumToString(NxMeshFlags v);
bool         StringToEnum(const char *str,NxMeshFlags &v);

/**
\brief Enum with flag values to be used in NxHeightFieldDesc.flags.
*/
enum NxHeightFieldFlags
{
/**
\brief Disable collisions with height field with boundary edges. Raise this flag if several terrain patches are going to be placed adjacent to each other, to avoid a bump when sliding across.
*/
  NX_HF_NO_BOUNDARY_EDGES        = (1<<0)
};
const char * EnumToString(NxHeightFieldFlags v);
bool         StringToEnum(const char *str,NxHeightFieldFlags &v);

#if NX_SDK_VERSION_NUMBER >= 250
/**
\brief Cloth mesh flags.
*/
enum NxClothMeshFlags
{
  NX_CLOTH_MESH_DUMMY0           = (1<<0),
  NX_CLOTH_MESH_DUMMY1           = (1<<1),
  NX_CLOTH_MESH_DUMMY2           = (1<<2),
  NX_CLOTH_MESH_DUMMY3           = (1<<3),
  NX_CLOTH_MESH_DUMMY4           = (1<<4),
  NX_CLOTH_MESH_DUMMY5           = (1<<5),
  NX_CLOTH_MESH_DUMMY6           = (1<<6),
  NX_CLOTH_MESH_DUMMY7           = (1<<7),
/**
\brief Specifies whether extra space is allocated for tearing on the PPU. If this flag is not set, less memory is needed on the PPU but tearing is not possible. These flags for clothMeshes extend the set of flags defined in NxMeshFlags.
*/
  NX_CLOTH_MESH_TEARABLE         = (1<<8),
/**
\brief Welds close vertices.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_CLOTH_MESH_WELD_VERTICES    = (1<<9)
#endif
};
const char * EnumToString(NxClothMeshFlags v);
bool         StringToEnum(const char *str,NxClothMeshFlags &v);

#endif
#if NX_SDK_VERSION_NUMBER >= 250
/**
\brief Vertex tearable flag
*/
enum NxClothVertexFlags
{
  NX_CLOTH_VERTEX_DUMMY0         = (1<<0),
  NX_CLOTH_VERTEX_DUMMY1         = (1<<1),
  NX_CLOTH_VERTEX_DUMMY2         = (1<<2),
  NX_CLOTH_VERTEX_DUMMY3         = (1<<3),
  NX_CLOTH_VERTEX_DUMMY4         = (1<<4),
  NX_CLOTH_VERTEX_DUMMY5         = (1<<5),
  NX_CLOTH_VERTEX_DUMMY6         = (1<<6),
/**
\brief Specifies whether or not a vertex can be torn
*/
  NX_CLOTH_VERTEX_TEARABLE       = (1<<7)
};
const char * EnumToString(NxClothVertexFlags v);
bool         StringToEnum(const char *str,NxClothVertexFlags &v);

#endif
enum NxFilterOp
{
  NX_FILTEROP_AND,
  NX_FILTEROP_OR,
  NX_FILTEROP_XOR,
  NX_FILTEROP_NAND,
  NX_FILTEROP_NOR,
  NX_FILTEROP_NXOR,
  NX_FILTEROP_SWAP_AND
};
const char * EnumToString(NxFilterOp v);
bool         StringToEnum(const char *str,NxFilterOp &v);

enum NxTimeStepMethod
{
  NX_TIMESTEP_FIXED,
  NX_TIMESTEP_VARIABLE
};
const char * EnumToString(NxTimeStepMethod v);
bool         StringToEnum(const char *str,NxTimeStepMethod &v);

enum NxSimulationType
{
  NX_SIMULATION_SW,
  NX_SIMULATION_HW
};
const char * EnumToString(NxSimulationType v);
bool         StringToEnum(const char *str,NxSimulationType &v);

#if NX_SDK_VERSION_NUMBER >= 250
enum NxThreadPriority
{
  NX_TP_HIGH,
#if NX_SDK_VERSION_NUMBER >= 271
  NX_TP_ABOVE_NORMAL,
#endif
  NX_TP_NORMAL,
#if NX_SDK_VERSION_NUMBER >= 271
  NX_TP_BELOW_NORMAL,
#endif
  NX_TP_LOW
};
const char * EnumToString(NxThreadPriority v);
bool         StringToEnum(const char *str,NxThreadPriority &v);

#endif
enum NxSceneFlags
{
  NX_SF_DISABLE_SSE              = (1<<0),
  NX_SF_DISABLE_COLLISIONS       = (1<<1),
  NX_SF_SIMULATE_SEPARATE_THREAD = (1<<2),
  NX_SF_ENABLE_MULTITHREAD       = (1<<3),
#if NX_SDK_VERSION_NUMBER >= 250
  NX_SF_ENABLE_ACTIVETRANSFORMS  = (1<<4),
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  NX_SF_RESTRICTED_SCENE         = (1<<5),
#endif
#if NX_SDK_VERSION_NUMBER >= 250
  NX_SF_DISABLE_SCENE_MUTEX      = (1<<6),
#endif
#if NX_SDK_VERSION_NUMBER >= 261
  NX_SF_FORCE_CONE_FRICTION      = (1<<7),
#endif
#if NX_SDK_VERSION_NUMBER >= 263
  NX_SF_SEQUENTIAL_PRIMARY       = (1<<8),
#endif
#if NX_SDK_VERSION_NUMBER >= 263
  NX_SF_FLUID_PERFORMANCE_HINT   = (1<<9)
#endif
};
const char * EnumToString(NxSceneFlags v);
bool         StringToEnum(const char *str,NxSceneFlags &v);

/**
\brief Flag that determines the combine mode. When two actors come in contact with each other, they each have materials with various coefficients, but we only need a single set of coefficients for the pair. Physics doesn't have any inherent combinations because the coefficients are determined empirically on a case by case basis. However, simulating this with a pairwise lookup table is often impractical. For this reason the following combine behaviors are available: NX_CM_AVERAGE NX_CM_MIN NX_CM_MULTIPLY NX_CM_MAX The effective combine mode for the pair is max(material0.combineMode, material1.combineMode).
*/
enum NxCombineMode
{
/**
\brief Average: (a + b)/2.
*/
  NX_CM_AVERAGE,
/**
\brief Minimum: min(a,b). 
*/
  NX_CM_MIN,
/**
\brief Multiply: a*b. 
*/
  NX_CM_MULTIPLY,
/**
\brief Maximum: max(a,b).
*/
  NX_CM_MAX
};
const char * EnumToString(NxCombineMode v);
bool         StringToEnum(const char *str,NxCombineMode &v);

/**
\brief Flags which control the behavior of a material.
*/
enum NxMaterialFlag
{
/**
\brief Flag to enable anisotropic friction computation. For a pair of actors, anisotropic friction is used only if at least one of the two actors' materials are anisotropic. The anisotropic friction parameters for the pair are taken from the material which is more anisotropic (i.e. the difference between its two dynamic friction coefficients is greater). The anisotropy direction of the chosen material is transformed to world space: dirOfAnisotropyWS = shape2world * dirOfAnisotropy Next, the directions of anisotropy in one or more contact planes (i.e. orthogonal to the contact normal) have to be determined. The two directions are: uAxis = (dirOfAnisotropyWS ^ contactNormal).normalize() vAxis = contactNormal ^ uAxis This way [uAxis, contactNormal, vAxis] forms a basis. It may happen, however, that (dirOfAnisotropyWS | contactNormal).magnitude() == 1 and then (dirOfAnisotropyWS ^ contactNormal) has zero length. This happens when the contactNormal is coincident to the direction of anisotopy. In this case we perform isotropic friction.
*/
  NX_MF_ANISOTROPIC              = (1<<0),
  NX_MF_DUMMY1                   = (1<<1),
  NX_MF_DUMMY2                   = (1<<2),
  NX_MF_DUMMY3                   = (1<<3),
/**
\brief If this flag is set, friction computations are always skipped between shapes with this material and any other shape. It may be a good idea to use this when all friction is to be performed using the tire friction model (see NxWheelShape).
*/
  NX_MF_DISABLE_FRICTION         = (1<<4),
/**
\brief The difference between 'normal' and 'strong' friction is that the strong friction feature remembers the 'friction error' between simulation steps. The friction is a force trying to hold objects in place (or slow them down) and this is handled in the solver. But since the solver is only an approximation, the result of the friction calculation can include a small 'error' - e.g. a box resting on a slope should not move at all if the static friction is in action, but could slowly glide down the slope because of a small friction error in each simulation step. The strong friction counter-acts this by remembering the small error and taking it to account during the next simulation step. However, in some cases the strong friction could cause problems, and this is why it is possible to disable the strong friction feature by setting this flag. One example is raycast vehicles, that are sliding fast across the surface, but still need a precise steering behavior. It may be a goodidea to reenable the strong friction when objects are coming to a rest, to prevent them from slowly creeping down inclines. Note: This flag only has an effect if the NX_MF_DISABLE_FRICTION bit is 0.
*/
  NX_MF_DISABLE_STRONG_FRICTION  = (1<<5)
};
const char * EnumToString(NxMaterialFlag v);
bool         StringToEnum(const char *str,NxMaterialFlag &v);

/**
\brief Collection of flags describing the behavior of a dynamic rigid body.
*/
enum NxBodyFlag
{
/**
\brief Set if gravity should not be applied on this body.  See also: NxBodyDesc.flags NxScene.setGravity()
*/
  NX_BF_DISABLE_GRAVITY          = (1<<0),
/**
\brief Enable/disable freezing for this body/actor. A frozen actor becomes temporarily static. Note: this is an experimental feature which doesn't always work on actors which have joints connected to them.
*/
  NX_BF_FROZEN_POS_X             = (1<<1),
  NX_BF_FROZEN_POS_Y             = (1<<2),
  NX_BF_FROZEN_POS_Z             = (1<<3),
  NX_BF_FROZEN_ROT_X             = (1<<4),
  NX_BF_FROZEN_ROT_Y             = (1<<5),
  NX_BF_FROZEN_ROT_Z             = (1<<6),
/**
\brief Enables kinematic mode for the actor. Enable kinematic mode for the body. Kinematic actors are special dynamic actors that are not influenced by forces (such as gravity), and have no momentum. They appear to have infinite mass and can be moved around the world using the moveGlobal*() methods. They will push regular dynamic actors out of the way. Currently they will not collide with static or other kinematic objects. This will change in a later version. Note that if a dynamic actor is squished between a kinematic and a static or two kinematics, then it will have no choice but to get pressed into one of them. Later we will make it possible to have the kinematic motion be blocked in this case. Kinematic actors are great for moving platforms or characters, where direct motion control is desired. You can not connect Reduced joints to kinematic actors. Lagrange joints work ok if the platform is moving with a relatively low, uniform velocity. See also: NxActor NxActor.raiseActorFlag()
*/
  NX_BF_KINEMATIC                = (1<<7),
/**
\brief Enable debug renderer for this body. See also: NxScene.getDebugRenderable() NxDebugRenderable NxParameter
*/
  NX_BF_VISUALIZATION            = (1<<8),
/**
\brief NX_BF_POSE_SLEEP_TEST Deprecated; do not use. Consider using sleep damping instead. See also: NxBodyDesc.sleepDamping
*/
  NX_BF_DUMMY_0                  = (1<<9),
/**
\brief Filter velocities used keep body awake. The filter reduces rapid oscillations and transient spikes.  See also: NxActor.isSleeping()
*/
  NX_BF_FILTER_SLEEP_VEL         = (1<<10),
/**
\brief Enables energy-based sleeping algorithm.  See also: NxActor.isSleeping() NxBodyDesc.sleepEnergyThreshold
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_BF_ENERGY_SLEEP_TEST        = (1<<11)
#endif
};
const char * EnumToString(NxBodyFlag v);
bool         StringToEnum(const char *str,NxBodyFlag &v);

enum NxActorFlag
{
  NX_AF_DISABLE_COLLISION        = (1<<0),
  NX_AF_DISABLE_RESPONSE         = (1<<1),
  NX_AF_LOCK_COM                 = (1<<2),
  NX_AF_FLUID_DISABLE_COLLISION  = (1<<3),
#if NX_SDK_VERSION_NUMBER < 250
  NX_AF_FLUID_ACTOR_REACTION     = (1<<4),
#endif
/**
\brief Contact Modification
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_AF_CONTACT_MODIFICATION     = (1<<4),
#endif
/**
\brief Force Cone Friction
*/
#if NX_SDK_VERSION_NUMBER >= 261
  NX_AF_FORCE_CONE_FRICTION      = (1<<5),
#endif
/**
\brief Enable/disable custom contact filtering
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_AF_USER_ACTOR_PAIR_FILTERING = (1<<6)
#endif
};
const char * EnumToString(NxActorFlag v);
bool         StringToEnum(const char *str,NxActorFlag &v);

enum NxShapeType
{
  NX_SHAPE_PLANE,
  NX_SHAPE_SPHERE,
  NX_SHAPE_BOX,
  NX_SHAPE_CAPSULE,
  NX_SHAPE_WHEEL,
  NX_SHAPE_CONVEX,
  NX_SHAPE_MESH,
  NX_SHAPE_HEIGHTFIELD
};
const char * EnumToString(NxShapeType v);
bool         StringToEnum(const char *str,NxShapeType &v);

#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief Describes the compartment types a rigid body shape might interact with.
*/
enum NxShapeCompartmentType
{
/**
\brief Software rigid body compartment
*/
  NX_COMPARTMENT_SW_RIGIDBODY    = (1<<0),
/**
\brief Hardware rigid body compartment
*/
  NX_COMPARTMENT_HW_RIGIDBODY    = (1<<1),
/**
\brief Software fluid compartment
*/
  NX_COMPARTMENT_SW_FLUID        = (1<<2),
/**
\brief Hardware fluid compartment
*/
  NX_COMPARTMENT_HW_FLUID        = (1<<3),
/**
\brief Software cloth compartment
*/
  NX_COMPARTMENT_SW_CLOTH        = (1<<4),
/**
\brief Hardware cloth compartment
*/
  NX_COMPARTMENT_HW_CLOTH        = (1<<5),
/**
\brief Software softbody compartment
*/
  NX_COMPARTMENT_SW_SOFTBODY     = (1<<6),
/**
\brief Hardware softbody compartment
*/
  NX_COMPARTMENT_HW_SOFTBODY     = (1<<7)
};
const char * EnumToString(NxShapeCompartmentType v);
bool         StringToEnum(const char *str,NxShapeCompartmentType &v);

#endif
/**
\brief Flags which affect the behavior of NxShapes.
*/
enum NxShapeFlag
{
/**
\brief Trigger callback will be called when a shape enters the trigger volume. See also: NxUserTriggerReport NxScene.setUserTriggerReport()
*/
  NX_TRIGGER_ON_ENTER            = (1<<0),
/**
\brief Trigger callback will be called after a shape leaves the trigger volume. See also: NxUserTriggerReport NxScene.setUserTriggerReport()
*/
  NX_TRIGGER_ON_LEAVE            = (1<<1),
/**
\brief Trigger callback will be called while a shape is intersecting the trigger volume. See also:NxUserTriggerReport NxScene.setUserTriggerReport()
*/
  NX_TRIGGER_ON_STAY             = (1<<2),
/**
\brief Enable debug renderer for this shape. See also: NxScene.getDebugRenderable() NxDebugRenderable NxParameter
*/
  NX_SF_VISUALIZATION            = (1<<3),
/**
\brief Disable collision detection for this shape (counterpart of NX_AF_DISABLE_COLLISION). Warning:IMPORTANT: this is only used for compound objects! Use NX_AF_DISABLE_COLLISION otherwise.
*/
  NX_SF_DISABLE_COLLISION        = (1<<4),
/**
\brief Enable feature indices in contact stream. See also:NxUserContactReport NxContactStreamIterator NxContactStreamIterator.getFeatureIndex0()
*/
  NX_SF_FEATURE_INDICES          = (1<<5),
/**
\brief Disable raycasting for this shape. 
*/
  NX_SF_DISABLE_RAYCASTING       = (1<<6),
/**
\brief Enable contact force reporting per contact point in contact stream (otherwise we only report force per actor pair). 
*/
  NX_SF_POINT_CONTACT_FORCE      = (1<<7),
/**
\brief Sets the shape to be a fluid drain. 
*/
  NX_SF_FLUID_DRAIN              = (1<<8),
  NX_SF_FLUID_DUMMY9             = (1<<9),
/**
\brief Disable collision with fluids. 
*/
  NX_SF_FLUID_DISABLE_COLLISION  = (1<<10),
/**
\brief Enables the reaction of the shapes actor on fluid collision. 
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_SF_FLUID_TWOWAY             = (1<<11),
#endif
/**
\brief Disable collision response for this shape (counterpart of NX_AF_DISABLE_RESPONSE). Warning:IMPORTANT: this is only used for compound objects! Use NX_AF_DISABLE_RESPONSE otherwise.
*/
  NX_SF_DISABLE_RESPONSE         = (1<<12),
/**
\brief Enable dynamic-dynamic CCD for this shape. Used only when CCD is globally enabled and shape have a CCD skeleton. 
*/
  NX_SF_DYNAMIC_DYNAMIC_CCD      = (1<<13),
/**
\brief Disable participation in ray casts, overlap tests and sweeps
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_SF_DISABLE_SCENE_QUERIES    = (1<<14),
#endif
/**
\brief Sets the shape to be a cloth drain
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_SF_CLOTH_DRAIN              = (1<<15),
#endif
/**
\brief Disable collision with cloths.
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_SF_CLOTH_DISABLE_COLLISION  = (1<<16),
#endif
/**
\brief Enable the reaction of the shapes actor on cloth collision
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_SF_CLOTH_TWOWAY             = (1<<17),
#endif
/**
\brief Sets the shape to be a soft body drain.
*/
#if NX_USE_SOFTBODY_API
  NX_SF_SOFTBODY_DRAIN           = (1<<18),
#endif
/**
\brief Disable collision with soft bodies
*/
#if NX_USE_SOFTBODY_API
  NX_SF_SOFTBODY_DISABLE_COLLISION = (1<<19),
#endif
/**
\brief Enables the reaction of the shapes actor on soft body collision
*/
#if NX_USE_SOFTBODY_API
  NX_SF_SOFTBODY_TWOWAY          = (1<<20)
#endif
};
const char * EnumToString(NxShapeFlag v);
bool         StringToEnum(const char *str,NxShapeFlag &v);

/**
\brief Flags specific to capsule shapes
*/
enum NxCapsuleShapeFlag
{
/**
\brief If this flag is set, the capsule shape represents a moving sphere, moving along the ray defined by the capsule's positive Y axis.  Currently this behavior is only implemented for points (zero radius spheres).
*/
  NX_SWEPT_SHAPE                 = (1<<0)
};
const char * EnumToString(NxCapsuleShapeFlag v);
bool         StringToEnum(const char *str,NxCapsuleShapeFlag &v);

/**
\brief Descibes how a wheel shape behaves
*/
enum NxWheelShapeFlags
{
/**
\brief Determines whether the suspension axis or the ground contact normal is used for the suspension constraint. 
*/
  NX_WF_WHEEL_AXIS_CONTACT_NORMAL = (1<<0),
/**
\brief If set, the laterial slip velocity is used as the input to the tire function, rather than the slip angle. 
*/
  NX_WF_INPUT_LAT_SLIPVELOCITY   = (1<<1),
/**
\brief If set, the longutudal slip velocity is used as the input to the tire function, rather than the slip ratio. 
*/
  NX_WF_INPUT_LNG_SLIPVELOCITY   = (1<<2),
/**
\brief If set, does not factor out the suspension travel and wheel radius from the spring force computation. This is the legacy behavior from the raycast capsule approach. 
*/
  NX_WF_UNSCALED_SPRING_BEHAVIOR = (1<<3),
/**
\brief If set, the axle speed is not computed by the simulation but is rather expected to be provided by the user every simulation step via NxWheelShape::setAxleSpeed().
*/
  NX_WF_AXLE_SPEED_OVERRIDE      = (1<<4),
/**
\brief If set, the NxWheelShape will emulate the legacy raycast capsule based wheel
*/
#if NX_SDK_VERSION_NUMBER >= 264
  NX_WF_EMULATE_LEGACY_WHEEL     = (1<<5),
#endif
/**
\brief If set, the NxWheelShape will clamp the force in the friction constraints
*/
#if NX_SDK_VERSION_NUMBER >= 264
  NX_WF_CLAMPED_FRICTION         = (1<<6)
#endif
};
const char * EnumToString(NxWheelShapeFlags v);
bool         StringToEnum(const char *str,NxWheelShapeFlags &v);

#if NX_SDK_VERSION_NUMBER >= 250
/**
\brief   Lets the user specify how to handle mesh paging to the PhysX card.  It is always possible to manually upload mesh pages, also in the modes called 
*/
enum NxMeshPagingMode
{
/**
\brief Manual mesh paging is the default, and it is up to the user to see to it that all needed mesh pages are available on the PhysX card when they are needed.  This mode should be the default choice, as it lets the developer prepare for scene switches, by uploading new mesh data continously.
*/
  NX_MESH_PAGING_MANUAL,
/**
\brief [Experimental] An automatic fallback to SW collision testing when mesh pages are missing on the PhysX card. This mode uses SW collision methods when an interaction between a shape and a mesh shape is missing mesh pages. The kind of constraints that are generated are both slower to generate, and takes more time to simulate. Warning: This mesh paging method is still an experimental feature
*/
  NX_MESH_PAGING_FALLBACK,
/**
\brief [Experimental] Automatic paging of missing mesh pages.  This mode checks which mesh pages are needed for each interaction between a mesh shape and another shape. If the page is not available on the PhysX card, then it is uploaded. If it is not possible to upload the mesh page (e.g. because the PhysX card is out of memory), then the SW fallback (see NX_MESH_PAGING_FALLBACK) is used instead.  Warning: This mesh paging method is still an experimental feature
*/
  NX_MESH_PAGING_AUTO
};
const char * EnumToString(NxMeshPagingMode v);
bool         StringToEnum(const char *str,NxMeshPagingMode &v);

#endif
/**
\brief Mesh shape flags to describe behavior of mesh shape collisions
*/
enum NxMeshShapeFlag
{
/**
\brief Select between 'normal' or 'smooth' sphere-mesh/raycastcapsule-mesh contact generation routines.  The 'normal' algorithm assumes that the mesh is composed from flat triangles. When a ball rolls or a raycast capsule slides along the mesh surface, it will experience small, sudden changes in velocity as it rolls from one triangle to the next. The smooth algorithm, on the other hand, assumes that the triangles are just an approximation of a surface that is smooth. It uses the Gouraud algorithm to smooth the triangles' vertex normals (which in this case are particularly important). This way the rolling sphere's/capsule's velocity will change smoothly over time, instead of suddenly. We recommend this algorithm for simulating car wheels on a terrain. See also: NxSphereShape NxTriangleMeshShape NxHeightFieldShape
*/
  NX_MESH_SMOOTH_SPHERE_COLLISIONS = (1<<0),
/**
\brief The mesh is double-sided. This is currently only used for raycasting.
*/
  NX_MESH_DOUBLE_SIDED           = (1<<1)
};
const char * EnumToString(NxMeshShapeFlag v);
bool         StringToEnum(const char *str,NxMeshShapeFlag &v);

enum NxJointType
{
  NX_JOINT_PRISMATIC,
  NX_JOINT_REVOLUTE,
  NX_JOINT_CYLINDRICAL,
  NX_JOINT_SPHERICAL,
  NX_JOINT_POINT_ON_LINE,
  NX_JOINT_POINT_IN_PLANE,
  NX_JOINT_DISTANCE,
  NX_JOINT_PULLEY,
  NX_JOINT_FIXED,
  NX_JOINT_D6
};
const char * EnumToString(NxJointType v);
bool         StringToEnum(const char *str,NxJointType &v);

/**
\brief Joint flags
*/
enum NxJointFlag
{
/**
\brief Raised if collision detection should be enabled between the bodies attached to the joint. By default collision constraints are not generated between pairs of bodies which are connected by joints.
*/
  NX_JF_COLLISION_ENABLED        = (1<<0),
/**
\brief Enable debug renderer for this joint.
*/
  NX_JF_VISUALIZATION            = (1<<1)
};
const char * EnumToString(NxJointFlag v);
bool         StringToEnum(const char *str,NxJointFlag &v);

/**
\brief Used to specify the range of motions allowed for a DOF in a D6 joint.
*/
enum NxD6JointMotion
{
/**
\brief The DOF is locked, it does not allow relative motion. 
*/
  NX_D6JOINT_MOTION_LOCKED,
/**
\brief The DOF is limited, it only allows motion within a specific range.
*/
  NX_D6JOINT_MOTION_LIMITED,
/**
\brief The DOF is free and has its full range of motions. 
*/
  NX_D6JOINT_MOTION_FREE
};
const char * EnumToString(NxD6JointMotion v);
bool         StringToEnum(const char *str,NxD6JointMotion &v);

/**
\brief Used to specify a particular drive method. i.e. Having a position based goal or a velocity based goal
*/
enum NxD6JointDriveType
{
/**
\brief Used to set a position goal when driving
*/
  NX_D6JOINT_DRIVE_POSITION      = (1<<0),
/**
\brief Used to set a velocity goal when driving
*/
  NX_D6JOINT_DRIVE_VELOCITY      = (1<<1),

  NX_D6JOINT_DRIVE_POSITION_AND_VELOCITY = NX_D6JOINT_DRIVE_POSITION | NX_D6JOINT_DRIVE_VELOCITY
};
const char * EnumToString(NxD6JointDriveType v);
bool         StringToEnum(const char *str,NxD6JointDriveType &v);

/**
\brief Joint projection modes. Joint projection is a method for correcting large joint errors. It is also necessary to set the distance above which projection occurs.
*/
enum NxJointProjectionMode
{
/**
\brief don't project this joint
*/
  NX_JPM_NONE,
  NX_JPM_POINT_MINDIST,
#if NX_SDK_VERSION_NUMBER >= 260
  NX_JPM_LINEAR_MINDIST
#endif
};
const char * EnumToString(NxJointProjectionMode v);
bool         StringToEnum(const char *str,NxJointProjectionMode &v);

/**
\brief Flags which control the general behavior of D6 joints.
*/
enum NxD6JointFlag
{
/**
\brief Drive along the shortest spherical arc.
*/
  NX_D6JOINT_SLERP_DRIVE         = (1<<0),
/**
\brief Apply gearing to the angular motion, e.g. body 2s angular motion is twice body 1s etc
*/
  NX_D6JOINT_GEAR_ENABLED        = (1<<1)
};
const char * EnumToString(NxD6JointFlag v);
bool         StringToEnum(const char *str,NxD6JointFlag &v);

/**
\brief Flags which control the behavior of distance joints.
*/
enum NxDistanceJointFlag
{
/**
\brief NX_DJF_MAX_DISTANCE_ENABLED
*/
  NX_DJF_MAX_DISTANCE_ENABLED    = (1<<0),
/**
\brief NX_DJF_MIN_DISTANCE_ENABLED
*/
  NX_DJF_MIN_DISTANCE_ENABLED    = (1<<1),
/**
\brief NX_DJF_SPRING_ENABLED
*/
  NX_DJF_SPRING_ENABLED          = (1<<2)
};
const char * EnumToString(NxDistanceJointFlag v);
bool         StringToEnum(const char *str,NxDistanceJointFlag &v);

/**
\brief   Flags to control the behavior of revolute joints.
*/
enum NxRevoluteJointFlag
{
/**
\brief true if limits is enabled 
*/
  NX_RJF_LIMIT_ENABLED           = (1<<0),
/**
\brief true if the motor is enabled
*/
  NX_RJF_MOTOR_ENABLED           = (1<<1),
/**
\brief true if the spring is enabled. The spring will only take effect if the motor is disabled.
*/
  NX_RJF_SPRING_ENABLED          = (1<<2)
};
const char * EnumToString(NxRevoluteJointFlag v);
bool         StringToEnum(const char *str,NxRevoluteJointFlag &v);

/**
\brief Flags which control the behavior of spherical joints.
*/
enum NxSphericalJointFlag
{
/**
\brief true if the twist limit is enabled 
*/
  NX_SJF_TWIST_LIMIT_ENABLED     = (1<<0),
/**
\brief true if the swing limit is enabled 
*/
  NX_SJF_SWING_LIMIT_ENABLED     = (1<<1),
/**
\brief true if the swing spring is enabled 
*/
  NX_SJF_TWIST_SPRING_ENABLED    = (1<<2),
/**
\brief true if the swing spring is enabled 
*/
  NX_SJF_SWING_SPRING_ENABLED    = (1<<3),
/**
\brief true if the joint spring is enabled
*/
  NX_SJF_JOINT_SPRING_ENABLED    = (1<<4),
/**
\brief Add additional constraints to linear movement
*/
#if NX_SDK_VERSION_NUMBER >= 281
  NX_SJF_PERPENDICULAR_DIR_CONSTRAINTS = (1<<5)
#endif
};
const char * EnumToString(NxSphericalJointFlag v);
bool         StringToEnum(const char *str,NxSphericalJointFlag &v);

/**
\brief    Flags to control the behavior of pulley joints.
*/
enum NxPulleyJointFlag
{
/**
\brief true if the joint also maintains a minimum distance, not just a maximum.
*/
  NX_PJF_IS_RIGID                = (1<<0),
  NX_PJF_MOTOR_ENABLED           = (1<<1)
};
const char * EnumToString(NxPulleyJointFlag v);
bool         StringToEnum(const char *str,NxPulleyJointFlag &v);

enum NxContactPairFlag
{
  NX_IGNORE_PAIR                 = (1<<0),
  NX_NOTIFY_ON_START_TOUCH       = (1<<1),
  NX_NOTIFY_ON_END_TOUCH         = (1<<2),
  NX_NOTIFY_ON_TOUCH             = (1<<3),
  NX_NOTIFY_ON_IMPACT            = (1<<4),
  NX_NOTIFY_ON_ROLL              = (1<<5),
  NX_NOTIFY_ON_SLIDE             = (1<<6),
#if NX_SDK_VERSION_NUMBER >= 250
  NX_NOTIFY_FORCES               = (1<<7),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD = (1<<8),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD = (1<<9),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD = (1<<10),
#endif
/**
\brief 
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_NOTIFY_CONTACT_MODIFICATION = (1<<16)
#endif
};
const char * EnumToString(NxContactPairFlag v);
bool         StringToEnum(const char *str,NxContactPairFlag &v);

#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
enum NxFluidSimulationMethod
{
  NX_F_SPH                       = (1<<0),
  NX_F_NO_PARTICLE_INTERACTION   = (1<<1),
  NX_F_MIXED_MODE                = (1<<2)
};
const char * EnumToString(NxFluidSimulationMethod v);
bool         StringToEnum(const char *str,NxFluidSimulationMethod &v);

#endif
#if NX_USE_FLUID_API
/**
\brief The fluid collision method
*/
enum NxFluidCollisionMethod
{
/**
\brief collide with static objects
*/
  NX_F_STATIC                    = (1<<0),
/**
\brief collide with dynamic objects
*/
  NX_F_DYNAMIC                   = (1<<1)
};
const char * EnumToString(NxFluidCollisionMethod v);
bool         StringToEnum(const char *str,NxFluidCollisionMethod &v);

#endif
#if NX_USE_FLUID_API
/**
\brief Fluid flags.
*/
enum NxFluidFlag
{
/**
\brief Enables debug visualization for the NxFluid. 
*/
  NX_FF_VISUALIZATION            = (1<<0),
/**
\brief Disables scene gravity for the NxFluid. 
*/
  NX_FF_DISABLE_GRAVITY          = (1<<1),
/**
\brief Enable/disable two way collision of fluid with the rigid body scene. In either case, fluid is influenced by colliding rigid bodies. If NX_FF_COLLISION_TWOWAY is not set, rigid bodies are not influenced by colliding pieces of fluid. Use NxFluidDesc.collisionResponseCoefficient to control the strength of the feedback force on rigid bodies.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_FF_COLLISION_TWOWAY         = (1<<2),
#endif
/**
\brief Enable/disable execution of fluid simulation.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_FF_ENABLED                  = (1<<3),
#endif
/**
\brief Defines whether this fluid is simulated on the PPU. 
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_FF_HARDWARE                 = (1<<4),
#endif
/**
\brief Enable/disable particle priority mode.
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NX_FF_PRIORITY_MODE            = (1<<5),
#endif
/**
\brief Enable/disable projection of particles to a plane.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FF_PROJECT_TO_PLANE         = (1<<6),
#endif
/**
\brief Forces fluid static mesh cooking format to parameters given by the fluid descriptor.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FF_FORCE_STRICT_COOKING_FORMAT = (1<<7)
#endif
};
const char * EnumToString(NxFluidFlag v);
bool         StringToEnum(const char *str,NxFluidFlag &v);

#endif
#if NX_USE_FLUID_API
enum NxEmitterType
{
  NX_FE_CONSTANT_PRESSURE        = (1<<0),
  NX_FE_CONSTANT_FLOW_RATE       = (1<<1)
};
const char * EnumToString(NxEmitterType v);
bool         StringToEnum(const char *str,NxEmitterType &v);

#endif
#if NX_USE_FLUID_API
enum NxEmitterShape
{
  NX_FE_RECTANGULAR              = (1<<0),
  NX_FE_ELLIPSE                  = (1<<1)
};
const char * EnumToString(NxEmitterShape v);
bool         StringToEnum(const char *str,NxEmitterShape &v);

#endif
#if NX_USE_FLUID_API
enum NxFluidEmitterFlag
{
  NX_FEF_VISUALIZATION           = (1<<0),
/**
\brief Broken Actor Reference
*/
#if NX_SDK_VERSION_NUMBER < 250
  NX_FEF_BROKEN_ACTOR_REF        = (1<<1),
#endif
/**
\brief placeholder
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NX_FEF_DUMMY1                  = (1<<1),
#endif
/**
\brief Force on Actor
*/
#if NX_SDK_VERSION_NUMBER < 260
  NX_FEF_FORCE_ON_ACTOR          = (1<<2),
#endif
/**
\brief Force on body
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_FEF_FORCE_ON_BODY           = (1<<2),
#endif
/**
\brief Add actor velocity
*/
#if NX_SDK_VERSION_NUMBER < 260
  NX_FEF_ADD_ACTOR_VELOCITY      = (1<<3),
#endif
/**
\brief Add body velocity
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NX_FEF_ADD_BODY_VELOCITY       = (1<<3),
#endif
/**
\brief The emitter is enabled
*/
  NX_FEF_ENABLED                 = (1<<4)
};
const char * EnumToString(NxFluidEmitterFlag v);
bool         StringToEnum(const char *str,NxFluidEmitterFlag &v);

#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
enum NxClothFlag
{
  NX_CLF_PRESSURE                = (1<<0),
  NX_CLF_STATIC                  = (1<<1),
  NX_CLF_DISABLE_COLLISION       = (1<<2),
  NX_CLF_SELFCOLLISION           = (1<<3),
  NX_CLF_VISUALIZATION           = (1<<4),
  NX_CLF_GRAVITY                 = (1<<5),
  NX_CLF_BENDING                 = (1<<6),
  NX_CLF_BENDING_ORTHO           = (1<<7),
  NX_CLF_DAMPING                 = (1<<8),
  NX_CLF_COLLISION_TWOWAY        = (1<<9),
  NX_CLF_DUMMY1                  = (1<<10),
  NX_CLF_TRIANGLE_COLLISION      = (1<<11),
  NX_CLF_TEARABLE                = (1<<12),
  NX_CLF_HARDWARE                = (1<<13),
#if NX_SDK_VERSION_NUMBER >= 250
  NX_CLF_COMDAMPING              = (1<<14),
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_CLF_VALIDBOUNDS             = (1<<15),
#endif
#if NX_SDK_VERSION_NUMBER >= 270
  NX_CLF_FLUID_COLLISION         = (1<<16),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_CLF_DISABLE_DYNAMIC_CCD     = (1<<17),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_CLF_ADHERE                  = (1<<18),
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_CLF_DUMMY19                 = (1<<19),
#endif
/**
\brief Make cloth less stretchy
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_CLF_HARD_STRETCH_LIMITATION = (1<<20),
#endif
/**
\brief Untangle cloth in the neighborhood of vertices
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_CLF_UNTANGLING              = (1<<21),
#endif
/**
\brief Enable collision with other deformables
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_CLF_INTER_COLLISION         = (1<<22)
#endif
};
const char * EnumToString(NxClothFlag v);
bool         StringToEnum(const char *str,NxClothFlag &v);

#if NX_SDK_VERSION_NUMBER >= 270
enum NxForceFieldCoordinates
{
  NX_FFC_CARTESIAN,
  NX_FFC_SPHERICAL,
  NX_FFC_CYLINDRICAL,
  NX_FFC_TOROIDAL
};
const char * EnumToString(NxForceFieldCoordinates v);
bool         StringToEnum(const char *str,NxForceFieldCoordinates &v);

#endif
enum NxForceFieldType
{
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NX_FF_TYPE_DUMMY_0,
#endif
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NX_FF_TYPE_DUMMY_1,
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FF_TYPE_GRAVITATIONAL,
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FF_TYPE_OTHER,
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FF_TYPE_NO_INTERACTION
#endif
};
const char * EnumToString(NxForceFieldType v);
bool         StringToEnum(const char *str,NxForceFieldType &v);

enum NxForceFieldFlags
{
#if NX_SDK_VERSION_NUMBER >= 272
  NX_FFF_DUMMY_0                 = (1<<0),
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_FFF_DUMMY_1                 = (1<<1),
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_FFF_DUMMY_2                 = (1<<2),
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_FFF_DUMMY_3                 = (1<<3),
#endif
#if NX_SDK_VERSION_NUMBER >= 272
  NX_FFF_DUMMY_4                 = (1<<4),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FFF_VOLUMETRIC_SCALING_FLUID = (1<<5),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FFF_VOLUMETRIC_SCALING_CLOTH = (1<<6),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FFF_VOLUMETRIC_SCALING_SOFTBODY = (1<<7),
#endif
#if NX_SDK_VERSION_NUMBER >= 280
  NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY = (1<<8)
#endif
};
const char * EnumToString(NxForceFieldFlags v);
bool         StringToEnum(const char *str,NxForceFieldFlags &v);

#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
#if NX_SDK_VERSION_NUMBER >= 272
#endif
enum NxForceFieldShapeGroupFlags
{
  NX_FFSG_EXCLUDE_GROUP          = (1<<0)
};
const char * EnumToString(NxForceFieldShapeGroupFlags v);
bool         StringToEnum(const char *str,NxForceFieldShapeGroupFlags &v);

#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_USE_SOFTBODY_API
/**
\brief Soft body mesh flags
*/
enum NxSoftBodyMeshFlags
{
  NX_SOFTBODY_MESH_DUMMY         = (1<<0),
/**
\brief Denotes the use of 16-bit vertex indices
*/
  NX_SOFTBODY_MESH_16_BIT_INDICES = (1<<1),
/**
\brief Not supported in the current release. Specifies whether extra space is allocated for tearing on the PPU. If this flag is not set, less memory is needed on the PPU but tearing is not possible.
*/
  NX_SOFTBODY_MESH_TEARABLE      = (1<<2)
};
const char * EnumToString(NxSoftBodyMeshFlags v);
bool         StringToEnum(const char *str,NxSoftBodyMeshFlags &v);

#endif
#if NX_USE_SOFTBODY_API
/**
\brief Soft body vertex flags.
*/
enum NxSoftBodyVertexFlags
{
  NX_SOFTBODY_VERTEX_DUMMY0      = (1<<0),
  NX_SOFTBODY_VERTEX_DUMMY1      = (1<<1),
  NX_SOFTBODY_VERTEX_DUMMY2      = (1<<2),
  NX_SOFTBODY_VERTEX_DUMMY3      = (1<<3),
  NX_SOFTBODY_VERTEX_DUMMY4      = (1<<4),
  NX_SOFTBODY_VERTEX_DUMMY5      = (1<<5),
  NX_SOFTBODY_VERTEX_DUMMY6      = (1<<6),
/**
\brief Specifies whether a cloth vertex can be torn.
*/
  NX_SOFTBODY_VERTEX_TEARABLE    = (1<<7),
  NX_SOFTBODY_VERTEX_DUMMY8      = (1<<8),
  NX_SOFTBODY_VERTEX_DUMMY9      = (1<<9),
  NX_SOFTBODY_VERTEX_DUMMY10     = (1<<10),
  NX_SOFTBODY_VERTEX_DUMMY11     = (1<<11),
  NX_SOFTBODY_VERTEX_DUMMY12     = (1<<12),
/**
\brief Used for clothing.
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SOFTBODY_VERTEX_SECONDARY   = (1<<13)
#endif
};
const char * EnumToString(NxSoftBodyVertexFlags v);
bool         StringToEnum(const char *str,NxSoftBodyVertexFlags &v);

#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
/**
\brief Collection of flags describing the behavior of a soft body.
*/
enum NxSoftBodyFlag
{
  NX_SBF_DUMMY0                  = (1<<0),
/**
\brief Makes the soft body static.
*/
  NX_SBF_STATIC                  = (1<<1),
/**
\brief Disable collision handling with the rigid body scene.
*/
  NX_SBF_DISABLE_COLLISION       = (1<<2),
/**
\brief Enable/disable self-collision handling within a single soft body.
*/
  NX_SBF_SELFCOLLISION           = (1<<3),
/**
\brief Enable/disable debug visualization.
*/
  NX_SBF_VISUALIZATION           = (1<<4),
/**
\brief Enable/disable gravity. If off, the soft body is not subject to the gravitational force of the rigid body scene.
*/
  NX_SBF_GRAVITY                 = (1<<5),
/**
\brief Enable/disable volume conservation. Select volume conservation through NxSoftBodyDesc.volumeStiffness.
*/
  NX_SBF_VOLUME_CONSERVATION     = (1<<6),
/**
\brief Enable/disable damping of internal velocities. Use NxSoftBodyDesc.dampingCoefficient to control damping.
*/
  NX_SBF_DAMPING                 = (1<<7),
/**
\brief Enable/disable two way collision of the soft body with the rigid body scene.
*/
  NX_SBF_COLLISION_TWOWAY        = (1<<8),
/**
\brief Defines whether the soft body is tearable.
*/
  NX_SBF_TEARABLE                = (1<<9),
/**
\brief Defines whether this soft body is simulated on the PPU. To simulate a soft body on the PPU set this flag and create the soft body in a regular software scene.
*/
  NX_SBF_HARDWARE                = (1<<10),
/**
\brief Enable/disable center of mass damping of internal velocities.
*/
  NX_SBF_COMDAMPING              = (1<<11),
/**
\brief If the flag NX_SBF_VALIDBOUNDS is set, soft body particles outside the volume defined by NxSoftBodyDesc.validBounds are automatically removed from the simulation
*/
  NX_SBF_VALIDBOUNDS             = (1<<12),
/**
\brief Enable/disable collision with fluids
*/
  NX_SBF_FLUID_COLLISION         = (1<<13),
/**
\brief Disable CCD with dynamic actors
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_SBF_DISABLE_DYNAMIC_CCD     = (1<<14),
#endif
/**
\brief Enable soft body to rigid body adherence
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NX_SBF_ADHERE                  = (1<<15),
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_DUMMY16                 = (1<<16),
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_DUMMY17                 = (1<<17),
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_DUMMY18                 = (1<<18),
#endif
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_DUMMY19                 = (1<<19),
#endif
/**
\brief Make soft body less stretchy
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_HARD_STRETCH_LIMITATION = (1<<20),
#endif
/**
\brief Enable collision with other deformables
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NX_SBF_INTER_COLLISION         = (1<<21)
#endif
};
const char * EnumToString(NxSoftBodyFlag v);
bool         StringToEnum(const char *str,NxSoftBodyFlag &v);

#endif
#if NX_USE_SOFTBODY_API
/**
\brief Soft body attachment flags.
*/
enum NxSoftBodyAttachmentFlag
{
/**
\brief The default is only object->soft body interaction (one way).
*/
  NX_SOFTBODY_ATTACHMENT_TWOWAY  = (1<<0),
/**
\brief When this flag is set, the attachment is tearable.
*/
  NX_SOFTBODY_ATTACHMENT_TEARABLE = (1<<1)
};
const char * EnumToString(NxSoftBodyAttachmentFlag v);
bool         StringToEnum(const char *str,NxSoftBodyAttachmentFlag &v);

#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
#endif
/**
\brief Very similar to NxMeshFlags used for the NxSimpleTriangleMesh type
*/
enum NxMeshDataFlags
{
/**
\brief Denotes the use of 16-bit vertex indices.
*/
  NX_MDF_16_BIT_INDICES          = (1<<0)
};
const char * EnumToString(NxMeshDataFlags v);
bool         StringToEnum(const char *str,NxMeshDataFlags &v);

enum SCHEMA_CLASS
{
  SC_NXUSTREAM2,
  SC_NxCompartmentFlag,
  SC_NxCompartmentDesc,
  SC_NxParameterDesc,
  SC_NxConvexFlags,
  SC_NxConvexMeshDesc,
  SC_NxMeshFlags,
  SC_NxSimpleTriangleMesh,
  SC_NxCCDSkeletonDesc,
  SC_NxTriangleMeshDesc,
  SC_NxHeightFieldFlags,
  SC_NxHeightFieldDesc,
  SC_NxClothAttachDesc,
  SC_NxConstraintDominanceDesc,
  SC_NxClothMeshFlags,
  SC_NxClothVertexFlags,
  SC_NxClothMeshDesc,
  SC_NxGroupsMask,
  SC_NxActorGroupPair,
  SC_NxSceneLimits,
  SC_NxSceneFlags,
  SC_NxMaterialFlag,
  SC_NxSpringDesc,
  SC_NxMaterialDesc,
  SC_NxBodyFlag,
  SC_NxBodyDesc,
  SC_NxActorFlag,
  SC_NxShapeCompartmentType,
  SC_NxShapeFlag,
  SC_NxCapsuleShapeFlag,
  SC_NxTireFunctionDesc,
  SC_NxWheelShapeFlags,
  SC_NxShapeDesc,
  SC_NxBoxShapeDesc,
  SC_NxPlaneShapeDesc,
  SC_NxSphereShapeDesc,
  SC_NxCapsuleShapeDesc,
  SC_NxMeshShapeFlag,
  SC_NxConvexShapeDesc,
  SC_NxTriangleMeshShapeDesc,
  SC_NxWheelShapeDesc,
  SC_NxHeightFieldShapeDesc,
  SC_NxActorDesc,
  SC_NxJointFlag,
  SC_NxPlaneInfoDesc,
  SC_NxJointDesc,
  SC_NxCylindricalJointDesc,
  SC_NxJointLimitSoftDesc,
  SC_NxJointLimitSoftPairDesc,
  SC_NxD6JointDriveType,
  SC_NxJointDriveDesc,
  SC_NxD6JointFlag,
  SC_NxD6JointDesc,
  SC_NxDistanceJointFlag,
  SC_NxDistanceJointDesc,
  SC_NxFixedJointDesc,
  SC_NxPointInPlaneJointDesc,
  SC_NxPointOnLineJointDesc,
  SC_NxPrismaticJointDesc,
  SC_NxJointLimitDesc,
  SC_NxJointLimitPairDesc,
  SC_NxMotorDesc,
  SC_NxRevoluteJointFlag,
  SC_NxRevoluteJointDesc,
  SC_NxSphericalJointFlag,
  SC_NxSphericalJointDesc,
  SC_NxPulleyJointFlag,
  SC_NxPulleyJointDesc,
  SC_NxContactPairFlag,
  SC_NxPairFlagDesc,
  SC_NxCollisionGroupDesc,
  SC_NxParticleData,
  SC_NxFluidSimulationMethod,
  SC_NxFluidCollisionMethod,
  SC_NxFluidFlag,
  SC_NxEmitterType,
  SC_NxEmitterShape,
  SC_NxFluidEmitterFlag,
  SC_NxFluidEmitterDesc,
  SC_NxFluidDesc,
  SC_NxClothFlag,
  SC_NxClothDesc,
  SC_NxForceFieldFlags,
  SC_NxForceFieldShapeDesc,
  SC_NxBoxForceFieldShapeDesc,
  SC_NxSphereForceFieldShapeDesc,
  SC_NxCapsuleForceFieldShapeDesc,
  SC_NxConvexForceFieldShapeDesc,
  SC_NxForceFieldShapeGroupFlags,
  SC_NxForceFieldShapeGroupDesc,
  SC_NxForceFieldLinearKernelDesc,
  SC_NxForceFieldScaleTableEntry,
  SC_NxForceFieldGroupReference,
  SC_NxForceFieldDesc,
  SC_NxSpringAndDamperEffectorDesc,
  SC_NxSoftBodyMeshFlags,
  SC_NxSoftBodyVertexFlags,
  SC_NxSoftBodyMeshDesc,
  SC_NxSoftBodyFlag,
  SC_NxSoftBodyAttachmentFlag,
  SC_NxSoftBodyAttachDesc,
  SC_NxSoftBodyDesc,
  SC_NxSceneDesc,
  SC_NxSceneInstanceDesc,
  SC_NxMeshDataFlags,
  SC_NxPhysicsSDKDesc,
  SC_NxuPhysicsCollection,
  SC_NxScene,
  SC_LAST
};
const char * EnumToString(SCHEMA_CLASS v);
bool         StringToEnum(const char *str,SCHEMA_CLASS &v);

#if NX_SDK_VERSION_NUMBER >= 260
#endif
#if NX_SDK_VERSION_NUMBER >= 262
#endif
#if NX_SDK_VERSION_NUMBER >= 280
#endif
#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 260
/**
\brief Descriptor class for NxCompartment. A compartment is a portion of the scene that can be simulated on a different hardware device than other parts of the scene.
*/
class NxCompartmentDesc
{
public:
  NxCompartmentDesc(void);
 ~NxCompartmentDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxCompartmentDesc &desc,CustomCopy &cc);
  void copyTo(::NxCompartmentDesc &desc,CustomCopy &cc);
/**
\brief Named lookup for compartment
*/
  const char *                                   mId;
/**
\brief Compartment meant for this type of simulation. Will be removed in 3.0, as comparments will become type-neutral. 
*/
  NxCompartmentType                              type;
/**
\brief A NxDeviceCode, incl. a PPU index from 0 to 31.
*/
  NxU32                                          deviceCode;
/**
\brief Size in distance units of a single cell in the paging grid. Should be set to the size of the largest common dynamic object in this compartment. 
*/
  NxF32                                          gridHashCellSize;
/**
\brief 2-power used to determine size of the hash table that geometry gets hashed into. Hash table size is (1 << gridHashTablePower).
*/
  NxU32                                          gridHashTablePower;
/**
\brief Combination of NxCompartmentFlags
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxCompartmentFlag                              flags;
#endif
/**
\brief It is possible to scale the simulation time steps of this compartment relative to the primary scene.
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxF32                                          timeScale;
#endif
/**
\brief Thread affinity mask for the compartment thread.
*/
#if NX_SDK_VERSION_NUMBER >= 271
  NxU32                                          threadMask;
#endif
  void *mInstance;
};

#endif
/**
\brief Describes a single SDK parameter and its value
*/
class NxParameterDesc
{
public:
  NxParameterDesc(void);
 ~NxParameterDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief The SDK parameter to use
*/
  NxParameter                                    param;
/**
\brief The value to apply
*/
  NxF32                                          value;
  void *mInstance;
};

/**
\brief Descriptor class for NxConvexMesh.
*/
class NxConvexMeshDesc
{
public:
  NxConvexMeshDesc(void);
 ~NxConvexMeshDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxConvexMeshDesc &desc,CustomCopy &cc);
  void copyTo(::NxConvexMeshDesc &desc,CustomCopy &cc);
/**
\brief identifier
*/
  const char *                                   mId;
/**
\brief optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief Array of points
*/
  NxArray< NxVec3 >                              mPoints;
/**
\brief Array of indices
*/
  NxArray< NxTri >                               mTriangles;
/**
\brief Flags bits, combined from values of the enum NxConvexFlags.
*/
  NxConvexFlags                                  flags;
/**
\brief Size of pre-cooked data
*/
  NxU32                                          mCookedDataSize;
/**
\brief Raw contents of cooked data in ASCII HEX form.
*/
  NxArray< NxU8 >                                mCookedData;
  void *mInstance;
// These are used internally to support auto-generated CCD skeletons.
  NxCCDSkeletonDesc *mCCDSkeleton;
};

/**
\brief A structure describing a triangle mesh.
*/
class NxSimpleTriangleMesh
{
public:
  NxSimpleTriangleMesh(void);
 ~NxSimpleTriangleMesh(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSimpleTriangleMesh &desc,CustomCopy &cc);
  void copyTo(::NxSimpleTriangleMesh &desc,CustomCopy &cc);
/**
\brief Array of points
*/
  NxArray< NxVec3 >                              mPoints;
/**
\brief Array of indices
*/
  NxArray< NxTri >                               mTriangles;
/**
\brief Flag bits from values of the enum NxMeshFlags
*/
  NxMeshFlags                                    flags;
  void *mInstance;
};

/**
\brief The description for a CCD skeleton
*/
class NxCCDSkeletonDesc : public NxSimpleTriangleMesh
{
public:
  NxCCDSkeletonDesc(void);
 ~NxCCDSkeletonDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief The named ID for this CCD skeleton
*/
  const char *                                   mId;
/**
\brief The optional user defined properties
*/
  const char *                                   mUserProperties;
  void *mInstance;
// These are used internally to support auto-generated CCD skeletons.
  SCHEMA_CLASS mPrimitive;
  NxVec3       mDimensions;
  NxReal       mRadius;
  NxReal       mHeight;
  void *       mConvexInstance;
};

/**
\brief Descriptor class for NxTriangleMesh. Note that this class is derived from NxSimpleTriangleMesh which contains the members that describe the basic mesh. The mesh data is *copied* when an NxTriangleMesh object is created from this descriptor. After the call the user may discard the triangle data.
*/
class NxTriangleMeshDesc : public NxSimpleTriangleMesh
{
public:
  NxTriangleMeshDesc(void);
 ~NxTriangleMeshDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxTriangleMeshDesc &desc,CustomCopy &cc);
  void copyTo(::NxTriangleMeshDesc &desc,CustomCopy &cc);
/**
\brief The named ID for this triangle mesh
*/
  const char *                                   mId;
/**
\brief The optional user defined properties
*/
  const char *                                   mUserProperties;
/**
\brief Optional pointer to first material index, or NULL. There are NxSimpleTriangleMesh::numTriangles indices in total. Caller may add materialIndexStride bytes to the pointer to access the next triangle.
*/
  NxArray< NxU32 >                               mMaterialIndices;
/**
\brief If treated asa heightfield, this is the vertical axis
*/
  NxHeightFieldAxis                              heightFieldVerticalAxis;
/**
\brief The maximum range of the mesh as heightfield
*/
  NxF32                                          heightFieldVerticalExtent;
/**
\brief The total size in bytes of the PMAP
*/
  NxU32                                          mPmapSize;
/**
\brief The density of the PMAP
*/
  NxU32                                          mPmapDensity;
/**
\brief The raw PMAP data in ASCII hex form.
*/
  NxArray< NxU8 >                                mPmapData;
/**
\brief The SDK computes convex edges of a mesh and use them for collision detection. This parameter allows you to setup a tolerance for the convex edge detector.
*/
  NxF32                                          convexEdgeThreshold;
/**
\brief The size of the pre-cooked data
*/
  NxU32                                          mCookedDataSize;
/**
\brief The raw cooked data in ASCII HEX form.
*/
  NxArray< NxU8 >                                mCookedData;
  void *mInstance;
};

/**
\brief Descriptor class for NxHeightField. The heightfield data is *copied* when a NxHeightField object is created from this descriptor. After the call the user may discard the height data.
*/
class NxHeightFieldDesc
{
public:
  NxHeightFieldDesc(void);
 ~NxHeightFieldDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxHeightFieldDesc &desc,CustomCopy &cc);
  void copyTo(::NxHeightFieldDesc &desc,CustomCopy &cc);
/**
\brief unique id
*/
  const char *                                   mId;
/**
\brief optional custom User properties
*/
  const char *                                   mUserProperties;
/**
\brief Number of sample rows in the height field samples array.
*/
  NxU32                                          nbRows;
/**
\brief Number of sample columns in the height field samples array.
*/
  NxU32                                          nbColumns;
/**
\brief The samples and materials array.
*/
  NxArray< NxU32 >                               mSamples;
/**
\brief Sets how far 'below ground' the height volume extends.
*/
  NxF32                                          verticalExtent;
/**
\brief Sets how thick the heightfield surface is.
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxF32                                          thickness;
#endif
/**
\brief This threshold is used by the collision detection to determine if a height field edge is convex and can generate contact points. Usually the convexity of an edge is determined from the angle (or cosine of the angle) between the normals of the faces sharing that edge. The height field allows a more efficient approach by comparing height values of neighbouring vertices. This parameter offsets the comparison. Smaller changes than 0.5 will not alter the set of convex edges. The rule of thumb is that larger values will result in fewer edge contacts.
*/
  NxF32                                          convexEdgeThreshold;
/**
\brief Flags bits, combined from values of the enum NxHeightFieldFlags.
*/
  NxHeightFieldFlags                             flags;
  void *mInstance;
};

/**
\brief A class to represent shape / cloth attachments
*/
class NxClothAttachDesc
{
public:
  NxClothAttachDesc(void);
 ~NxClothAttachDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief The actor the cloth is attached to.
*/
  const char *                                   mAttachActor;
/**
\brief The shape index of the actor which it is attached to.
*/
  NxU32                                          mAttachShapeIndex;
/**
\brief True if the shape is attached as with two-way interaction.
*/
  NX_BOOL                                        mTwoWay;
/**
\brief True if the verts attached to the shape are tearable or not.
*/
  NX_BOOL                                        mTearable;
/**
\brief True if the actor and its shapes are attached to the 'core' of the cloth simulation
*/
  NX_BOOL                                        mCore;
  void *mInstance;
};

/**
\brief Structure used to store dominance pairs
*/
class NxConstraintDominanceDesc
{
public:
  NxConstraintDominanceDesc(void);
 ~NxConstraintDominanceDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief first group
*/
  NxU16                                          mGroup0;
/**
\brief second group
*/
  NxU16                                          mGroup1;
/**
\brief first group's dominance
*/
  NxF32                                          mDominance0;
/**
\brief second group's dominance
*/
  NxF32                                          mDominance1;
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 250
#endif
#if NX_SDK_VERSION_NUMBER >= 250
#endif
/**
\brief Descriptor class for NxClothMesh.
*/
class NxClothMeshDesc : public NxSimpleTriangleMesh
{
public:
  NxClothMeshDesc(void);
 ~NxClothMeshDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxClothMeshDesc &desc,CustomCopy &cc);
  void copyTo(::NxClothMeshDesc &desc,CustomCopy &cc);
/**
\brief identifier
*/
  const char *                                   mId;
/**
\brief optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief Pointer to first vertex mass.
*/
  NxArray< NxU32 >                               mVertexMasses;
/**
\brief Pointer to first vertex flag. Flags are of type NxClothMeshFlags.
*/
  NxArray< NxU32 >                               mVertexFlags;
/**
\brief Cloth-specific flags; taken from/merged to flags in NxSimpleTriangleMesh
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxClothMeshFlags                               mMeshFlags;
#endif
/**
\brief For welding close vertices.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxReal                                         weldingDistance;
#endif
/**
\brief For creating a mesh hierarchy.
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxU32                                          numHierarchyLevels;
#endif
  void *mInstance;
};

/**
\brief 128-bit mask used for collision filtering. The collision filtering equation for 2 shapes S0 and S1 is: (G0 op0 K0) op2 (G1 op1 K1) == b  with G0 = NxGroupsMask for shape S0. See setGroupsMask  G1 = NxGroupsMask for shape S1. See setGroupsMask  K0 = filtering constant 0. See setFilterConstant0  K1 = filtering constant 1. See setFilterConstant1  b = filtering boolean. See setFilterBool  op0, op1, op2 = filtering operations. See setFilterOps  If the filtering equation is true, collision detection is enabled.
*/
class NxGroupsMask
{
public:
  NxGroupsMask(void);
 ~NxGroupsMask(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxGroupsMask &desc,CustomCopy &cc);
  void copyTo(::NxGroupsMask &desc,CustomCopy &cc);
/**
\brief bit0-31
*/
  NxU32                                          bits0;
/**
\brief bit32-63
*/
  NxU32                                          bits1;
/**
\brief bit64-95
*/
  NxU32                                          bits2;
/**
\brief bit95-127
*/
  NxU32                                          bits3;
  void *mInstance;
};

/**
\brief Structure used to store actor group pair flags. 
*/
class NxActorGroupPair
{
public:
  NxActorGroupPair(void);
 ~NxActorGroupPair(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief first group
*/
  NxU16                                          group0;
/**
\brief second group
*/
  NxU16                                          group1;
/**
\brief The contact pair flags between these two actor groups
*/
  NxContactPairFlag                              flags;
  void *mInstance;
};

/**
\brief Class used to retrieve limits(e.g. max number of bodies) for a scene. The limits are used as a hint to the size of the scene, not as a hard limit (i.e. it will be possible to create more objects than specified in the scene limits). 
*/
class NxSceneLimits
{
public:
  NxSceneLimits(void);
 ~NxSceneLimits(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSceneLimits &desc,CustomCopy &cc);
  void copyTo(::NxSceneLimits &desc,CustomCopy &cc);
/**
\brief Expected max number of actors.
*/
  NxU32                                          maxNbActors;
/**
\brief Expected max number of bodies.
*/
  NxU32                                          maxNbBodies;
/**
\brief Expected max number of static shapes.
*/
  NxU32                                          maxNbStaticShapes;
/**
\brief Expected max number of dynamic shapes.
*/
  NxU32                                          maxNbDynamicShapes;
/**
\brief Expected max number of joints.
*/
  NxU32                                          maxNbJoints;
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 250
#endif
/**
\brief Describes a joint spring. The spring is implicitly integrated, so even high spring and damper coefficients should be robust.
*/
class NxSpringDesc
{
public:
  NxSpringDesc(void);
 ~NxSpringDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSpringDesc &desc,CustomCopy &cc);
  void copyTo(::NxSpringDesc &desc,CustomCopy &cc);
/**
\brief spring coefficient
*/
  NxF32                                          spring;
/**
\brief damper coefficient
*/
  NxF32                                          damper;
/**
\brief target value (angle/position) of spring where the spring force is zero.
*/
  NxF32                                          targetValue;
  void *mInstance;
};

/**
\brief Descriptor of NxMaterial.
*/
class NxMaterialDesc
{
public:
  NxMaterialDesc(void);
 ~NxMaterialDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxMaterialDesc &desc,CustomCopy &cc);
  void copyTo(::NxMaterialDesc &desc,CustomCopy &cc);
/**
\brief Unique identifier for material
*/
  const char *                                   mId;
/**
\brief Optional user properties for this material
*/
  const char *                                   mUserProperties;
/**
\brief True if material has a spring specification
*/
  NX_BOOL                                        mHasSpring;
/**
\brief Index number reference
*/
  NxU16                                          mMaterialIndex;
/**
\brief    coefficient of dynamic friction -- should be in [0, +inf]. If set to greater than staticFriction, the effective value of staticFriction will be increased to match. if flags & NX_MF_ANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)
*/
  NxF32                                          dynamicFriction;
/**
\brief coefficient of static friction -- should be in [0, +inf] if flags & NX_MF_ANISOTROPIC is set, then this value is used for the primary direction of anisotropy (U axis)
*/
  NxF32                                          staticFriction;
/**
\brief coefficient of restitution -- 0 makes the object bounce as little as possible, higher values up to 1.0 result in more bounce. Note that values close to or above 1 may cause stability problems and/or increasing energy. Range: [0,1] Default: 0.0
*/
  NxF32                                          restitution;
/**
\brief anisotropic dynamic friction coefficient for along the secondary (V) axis of anisotropy. This is only used if flags & NX_MF_ANISOTROPIC is set. Range: [0,inf] Default: 0.0
*/
  NxF32                                          dynamicFrictionV;
/**
\brief anisotropic static friction coefficient for along the secondary (V) axis of anisotropy. This is only used if flags & NX_MF_ANISOTROPIC is set. Range: [0,inf] Default: 0.0
*/
  NxF32                                          staticFrictionV;
/**
\brief   Friction combine mode. See the enum NxCombineMode .Default: NX_CM_AVERAGE
*/
  NxCombineMode                                  frictionCombineMode;
/**
\brief Restitution combine mode. See the enum NxCombineMode .Default: NX_CM_AVERAGE
*/
  NxCombineMode                                  restitutionCombineMode;
/**
\brief shape space direction (unit vector) of anisotropy. This is only used if flags & NX_MF_ANISOTROPIC is set. Range: direction vector Default: 1.0f,0.0f,0.0f
*/
  NxVec3                                         dirOfAnisotropy;
/**
\brief Flags, a combination of the bits defined by the enum NxMaterialFlag .Default: 0
*/
  NxMaterialFlag                                 flags;
/**
\brief Currently unused
*/
  NxSpringDesc                                   mSpring;
  NxU16 mInstanceIndex;
};

/**
\brief Descriptor for the optional rigid body dynamic state of NxActor.
*/
class NxBodyDesc
{
public:
  NxBodyDesc(void);
 ~NxBodyDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxBodyDesc &desc,CustomCopy &cc);
  void copyTo(::NxBodyDesc &desc,CustomCopy &cc);
/**
\brief position and orientation of the center of mass 
*/
  NxMat34                                        massLocalPose;
/**
\brief Diagonal mass space inertia tensor in bodies mass frame. 
*/
  NxVec3                                         massSpaceInertia;
/**
\brief Mass of Body
*/
  NxF32                                          mass;
/**
\brief Linear Velocity of the body.
*/
  NxVec3                                         linearVelocity;
/**
\brief Angular velocity of the body.
*/
  NxVec3                                         angularVelocity;
/**
\brief The body's initial wake up counter.
*/
  NxF32                                          wakeUpCounter;
/**
\brief Linear damping applied to the body. 
*/
  NxF32                                          linearDamping;
/**
\brief Angular damping applied to the body.
*/
  NxF32                                          angularDamping;
/**
\brief Maximum allowed angular velocity.
*/
  NxF32                                          maxAngularVelocity;
/**
\brief When CCD is globally enabled, it is still not performed if the motion distance of all points on the body is below this threshold.
*/
  NxF32                                          CCDMotionThreshold;
/**
\brief Combination of NxBodyFlag flags.
*/
  NxBodyFlag                                     flags;
/**
\brief Maximum linear velocity at which body can go to sleep.
*/
  NxF32                                          sleepLinearVelocity;
/**
\brief Maximum angular velocity at which body can go to sleep.
*/
  NxF32                                          sleepAngularVelocity;
/**
\brief Number of solver iterations performed when processing joint/contacts connected to this body. 
*/
  NxU32                                          solverIterationCount;
/**
\brief The force threshold for contact reports.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          contactReportThreshold;
#endif
/**
\brief Threshold for the energy-based sleeping algorithm. Only used when the NX_BF_ENERGY_SLEEP_TEST flag is set.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          sleepEnergyThreshold;
#endif
/**
\brief Damping factor for bodies that are about to sleep.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          sleepDamping;
#endif
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 280
#endif
/**
\brief Class used to describe tire properties for a NxWheelShape.  A tire force function takes a measure for tire slip as input, (this is computed differently for lateral and longitudal directions) and gives a force (or in our implementation, an impulse) as output. The curve is approximated by a two piece cubic Hermite spline. The first section goes from (0,0) to (extremumSlip, extremumValue), at which point the curve's tangent is zero. The second section goes from (extremumSlip, extremumValue) to (asymptoteSlip, asymptoteValue), at which point the curve's tangent is again zero.
*/
class NxTireFunctionDesc
{
public:
  NxTireFunctionDesc(void);
 ~NxTireFunctionDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxTireFunctionDesc &desc,CustomCopy &cc);
  void copyTo(::NxTireFunctionDesc &desc,CustomCopy &cc);
/**
\brief extremal point of curve. Both values must be positive. 
*/
  NxF32                                          extremumSlip;
/**
\brief extremal point of curve. Both values must be positive.
*/
  NxF32                                          extremumValue;
/**
\brief point on curve at which for all x > minumumX, function equals minimumY. Both values must be positive.
*/
  NxF32                                          asymptoteSlip;
/**
\brief point on curve at which for all x > minumumX, function equals minimumY. Both values must be positive.
*/
  NxF32                                          asymptoteValue;
/**
\brief Scaling factor for tire force.
*/
  NxF32                                          stiffnessFactor;
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 250
#endif
/**
\brief Descriptor for NxShape class. Used for saving and loading the shape state. See the derived classes for the different shape types.
*/
class NxShapeDesc
{
public:
  NxShapeDesc(void);
 ~NxShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxShapeDesc &desc,CustomCopy &cc);
/**
\brief Optional custom user properties on this shape
*/
  const char *                                   mUserProperties;
  const char *                                   name;
/**
\brief The pose of the shape in the coordinate frame of the owning actor.
*/
  NxMat34                                        localPose;
/**
\brief A combination of NxShapeFlag values.
*/
  NxShapeFlag                                    shapeFlags;
/**
\brief shape group
*/
  NxU16                                          group;
/**
\brief The material index of the shape.
*/
  NxU16                                          materialIndex;
/**
\brief The name of the CCD skeleton
*/
  const char *                                   mCCDSkeleton;
/**
\brief density of this individual shape when computing mass inertial properties for a rigidbody (unless a valid mass >0.0 is provided). Note that this will only be used if the body has a zero inertia tensor, or if you call NxActor::updateMassFromShapes explicitly. 
*/
  NxF32                                          mShapeDensity;
/**
\brief mass of this individual shape when computing mass inertial properties for a rigidbody. When mass<=0.0 then density and volume determine the mass. Note that this will only be used if the body has a zero inertia tensor, or if you call NxActor::updateMassFromShapes explicitly. 
*/
  NxF32                                          mShapeMass;
/**
\brief Specifies by how much shapes can interpenetrate.
*/
  NxF32                                          skinWidth;
/**
\brief Groups bit mask for collision filtering.
*/
  NxGroupsMask                                   groupsMask;
/**
\brief Defines which compartment types the shape should not interact with.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxShapeCompartmentType                         nonInteractingCompartmentTypes;
#endif
  void *mInstance;
  SCHEMA_CLASS mType;
};

/**
\brief Descriptor class for NxBoxShape.
*/
class NxBoxShapeDesc : public NxShapeDesc
{
public:
  NxBoxShapeDesc(void);
 ~NxBoxShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxBoxShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxBoxShapeDesc &desc,CustomCopy &cc);
/**
\brief Dimensions of the box.
*/
  NxVec3                                         dimensions;
  void *mInstance;
};

/**
\brief Descriptor for plane shape desc
*/
class NxPlaneShapeDesc : public NxShapeDesc
{
public:
  NxPlaneShapeDesc(void);
 ~NxPlaneShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPlaneShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxPlaneShapeDesc &desc,CustomCopy &cc);
/**
\brief Plane normal (unit length!)
*/
  NxVec3                                         normal;
/**
\brief The distance from the origin
*/
  NxF32                                          d;
  void *mInstance;
};

/**
\brief Descriptor class for NxSphereShape. 
*/
class NxSphereShapeDesc : public NxShapeDesc
{
public:
  NxSphereShapeDesc(void);
 ~NxSphereShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSphereShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxSphereShapeDesc &desc,CustomCopy &cc);
/**
\brief radius of shape. Must be positive.
*/
  NxF32                                          radius;
  void *mInstance;
};

/**
\brief Descriptor class for NxCapsuleShape.
*/
class NxCapsuleShapeDesc : public NxShapeDesc
{
public:
  NxCapsuleShapeDesc(void);
 ~NxCapsuleShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxCapsuleShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxCapsuleShapeDesc &desc,CustomCopy &cc);
/**
\brief Radius of the capsule's hemispherical ends and its trunk.
*/
  NxF32                                          radius;
/**
\brief The distance between the two hemispherical ends of the capsule.
*/
  NxF32                                          height;
/**
\brief Combination of NxCapsuleShapeFlag.
*/
  NxCapsuleShapeFlag                             flags;
  void *mInstance;
};

/**
\brief Descriptor class for NxConvexShape.
*/
class NxConvexShapeDesc : public NxShapeDesc
{
public:
  NxConvexShapeDesc(void);
 ~NxConvexShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxConvexShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxConvexShapeDesc &desc,CustomCopy &cc);
/**
\brief Named reference of the triangle mesh we want to instance
*/
  const char *                                   mMeshData;
/**
\brief Combination of NxMeshShapeFlag.
*/
  NxMeshShapeFlag                                meshFlags;
  void *mInstance;
};

/**
\brief Descriptor class for NxTriangleMeshShape. 
*/
class NxTriangleMeshShapeDesc : public NxShapeDesc
{
public:
  NxTriangleMeshShapeDesc(void);
 ~NxTriangleMeshShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxTriangleMeshShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxTriangleMeshShapeDesc &desc,CustomCopy &cc);
/**
\brief Named reference of the triangle mesh we want to instance
*/
  const char *                                   mMeshData;
/**
\brief Combination of NxMeshShapeFlag.
*/
  NxMeshShapeFlag                                meshFlags;
/**
\brief Mesh paging scheme
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxMeshPagingMode                               meshPagingMode;
#endif
  void *mInstance;
};

/**
\brief Descriptor for an NxWheelShape.
*/
class NxWheelShapeDesc : public NxShapeDesc
{
public:
  NxWheelShapeDesc(void);
 ~NxWheelShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxWheelShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxWheelShapeDesc &desc,CustomCopy &cc);
/**
\brief distance from wheel axle to a point on the contact surface.
*/
  NxF32                                          radius;
/**
\brief maximum extension distance of suspension along shape's -Y axis.
*/
  NxF32                                          suspensionTravel;
/**
\brief data intended for car wheel suspension effects. 
*/
  NxSpringDesc                                   suspension;
/**
\brief cubic hermite spline coefficients describing the longitudal tire force curve.
*/
  NxTireFunctionDesc                             longitudalTireForceFunction;
/**
\brief cubic hermite spline coefficients describing the lateral tire force curve.
*/
  NxTireFunctionDesc                             lateralTireForceFunction;
/**
\brief inverse mass of the wheel.
*/
  NxF32                                          inverseWheelMass;
/**
\brief flags from NxWheelShapeFlags
*/
  NxWheelShapeFlags                              wheelFlags;
/**
\brief Sum engine torque on the wheel axle.
*/
  NxF32                                          motorTorque;
/**
\brief The amount of torque applied for braking.
*/
  NxF32                                          brakeTorque;
/**
\brief steering angle, around shape Y axis.
*/
  NxF32                                          steerAngle;
  void *mInstance;
};

/**
\brief Descriptor class for NxHeightFieldShape.
*/
class NxHeightFieldShapeDesc : public NxShapeDesc
{
public:
  NxHeightFieldShapeDesc(void);
 ~NxHeightFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxHeightFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxHeightFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Id of HeightField to instance
*/
  const char *                                   mHeightField;
/**
\brief Multiplier to transform sample height values to shape space y coordinates.
*/
  NxF32                                          heightScale;
/**
\brief Multiplier to transform height field rows to shape space x coordinates.
*/
  NxF32                                          rowScale;
/**
\brief Multiplier to transform height field columns to shape space z coordinates.
*/
  NxF32                                          columnScale;
/**
\brief The high 9 bits of this number are used to complete the material indices in the samples.
*/
  NxU16                                          materialIndexHighBits;
/**
\brief The the material index that designates holes in the height field.
*/
  NxU16                                          holeMaterial;
/**
\brief Combination of NxMeshShapeFlag. So far the only value permitted here is 0 or NX_MESH_SMOOTH_SPHERE_COLLISIONS.
*/
  NxMeshShapeFlag                                meshFlags;
  void *mInstance;
};

/**
\brief Actor Descriptor. This structure is used to save and load the state of NxActor objects. 
*/
class NxActorDesc
{
public:
  NxActorDesc(void);
 ~NxActorDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxActorDesc &desc,CustomCopy &cc);
  void copyTo(::NxActorDesc &desc,CustomCopy &cc);
/**
\brief The unique ID field for the actor
*/
  const char *                                   mId;
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief True if actor has a body
*/
  NX_BOOL                                        mHasBody;
/**
\brief The otpional name of the actor
*/
  const char *                                   name;
/**
\brief The pose of the actor in the world. 
*/
  NxMat34                                        globalPose;
/**
\brief Body descriptor, null for static actors. 
*/
  NxBodyDesc                                     mBody;
/**
\brief Density used during mass/inertia computation
*/
  NxF32                                          density;
/**
\brief Combination of NxActorFlag flags
*/
  NxActorFlag                                    flags;
/**
\brief The actors group
*/
  NxU16                                          group;
/**
\brief The compartment to place the actor in. Must be either a pointer to an NxCompartment of type NX_SCT_RIGIDBODY, or NULL. A NULL compartment means creating the actor in the scene proper.
*/
#if NX_SDK_VERSION_NUMBER >= 260
  const char *                                   mCompartment;
#endif
  NxArray< NxShapeDesc *>                        mShapes;
/**
\brief The actor's dominance group
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxU16                                          dominanceGroup;
#endif
/**
\brief Combination of ::NxContactPairFlag flags
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU32                                          contactReportFlags;
#endif
/**
\brief force field material index for the scaling table
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU16                                          forceFieldMaterial;
#endif
  void *mInstance;
  const char *mColladaName;
};

/**
\brief Plane limit information
*/
class NxPlaneInfoDesc
{
public:
  NxPlaneInfoDesc(void);
 ~NxPlaneInfoDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief Normal of plane equation
*/
  NxVec3                                         mPlaneNormal;
/**
\brief D co-efficient of plane equation
*/
  NxF32                                          mPlaneD;
/**
\brief limit plane restitution
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxF32                                          restitution;
#endif
  void *mInstance;
};

/**
\brief Descriptor class for the NxJoint class. Joint descriptors for all the different joint types are derived from this class.
*/
class NxJointDesc
{
public:
  NxJointDesc(void);
 ~NxJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointDesc &desc,CustomCopy &cc);
/**
\brief the unique Id of this joint
*/
  const char *                                   mId;
/**
\brief optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief Id of actor 0
*/
  const char *                                   mActor0;
/**
\brief Id of actor 1
*/
  const char *                                   mActor1;
/**
\brief X axis of joint space, in actor[i]'s space, orthogonal to localAxis[i].
*/
  NxVec3                                         localNormal0;
/**
\brief X axis of joint space, in actor[i]'s space, orthogonal to localAxis[i].
*/
  NxVec3                                         localNormal1;
/**
\brief Z axis of joint space, in actor[i]'s space. This is the primary axis of the joint.
*/
  NxVec3                                         localAxis0;
/**
\brief Z axis of joint space, in actor[i]'s space. This is the primary axis of the joint.
*/
  NxVec3                                         localAxis1;
/**
\brief Attachment point of joint in actor[i]'s space. 
*/
  NxVec3                                         localAnchor0;
/**
\brief Attachment point of joint in actor[i]'s space. 
*/
  NxVec3                                         localAnchor1;
/**
\brief Maximum force on the joint before it can break (if breakable set)
*/
  NxF32                                          maxForce;
/**
\brief Maximum angular force (torque) that the joint can withstand before breaking, must be positive.
*/
  NxF32                                          maxTorque;
/**
\brief Extrapolation factor for solving joint constraints.
*/
#if NX_SDK_VERSION_NUMBER >= 281
  NxF32                                          solverExtrapolationFactor;
#endif
/**
\brief Switch to acceleration based spring.
*/
#if NX_SDK_VERSION_NUMBER >= 281
  NxU32                                          useAccelerationSpring;
#endif
/**
\brief This is a combination of the bits defined by NxJointFlag .
*/
  NxJointFlag                                    jointFlags;
/**
\brief Point of plane limit
*/
  NxVec3                                         mPlaneLimitPoint;
/**
\brief not sure
*/
  NX_BOOL                                        mOnActor2;
/**
\brief List of plane limits
*/
  NxArray< NxPlaneInfoDesc *>                    mPlaneInfo;
  void *mInstance;
  SCHEMA_CLASS mType;
  const char *mColladaName;
};

/**
\brief Desc class for sliding joint. See NxCylindricalJoint.
*/
class NxCylindricalJointDesc : public NxJointDesc
{
public:
  NxCylindricalJointDesc(void);
 ~NxCylindricalJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxCylindricalJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxCylindricalJointDesc &desc,CustomCopy &cc);
  void *mInstance;
};

/**
\brief Describes a joint limit.
*/
class NxJointLimitSoftDesc
{
public:
  NxJointLimitSoftDesc(void);
 ~NxJointLimitSoftDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointLimitSoftDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointLimitSoftDesc &desc,CustomCopy &cc);
/**
\brief the angle / position beyond which the limit is active.
*/
  NxF32                                          value;
/**
\brief Controls the amount of bounce when the joint hits a limit.
*/
  NxF32                                          restitution;
/**
\brief if greater than zero, the limit is soft, i.e. a spring pulls the joint back to the limit
*/
  NxF32                                          spring;
/**
\brief if spring is greater than zero, this is the damping of the spring
*/
  NxF32                                          damping;
  void *mInstance;
};

/**
\brief Describes a pair of joint limits.
*/
class NxJointLimitSoftPairDesc
{
public:
  NxJointLimitSoftPairDesc(void);
 ~NxJointLimitSoftPairDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointLimitSoftPairDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointLimitSoftPairDesc &desc,CustomCopy &cc);
/**
\brief The low limit (smaller value).
*/
  NxJointLimitSoftDesc                           low;
/**
\brief the high limit (larger value)
*/
  NxJointLimitSoftDesc                           high;
  void *mInstance;
};

/**
\brief Class used to describe drive properties for a NxD6Joint.
*/
class NxJointDriveDesc
{
public:
  NxJointDriveDesc(void);
 ~NxJointDriveDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointDriveDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointDriveDesc &desc,CustomCopy &cc);
/**
\brief The drive type bit flags
*/
  NxD6JointDriveType                             driveType;
/**
\brief spring coefficient
*/
  NxF32                                          spring;
/**
\brief damper coefficient
*/
  NxF32                                          damping;
/**
\brief The maximum force (or torque) the drive can exert.
*/
  NxF32                                          forceLimit;
  void *mInstance;
};

/**
\brief Descriptor class for the D6Joint. See NxD6Joint. 
*/
class NxD6JointDesc : public NxJointDesc
{
public:
  NxD6JointDesc(void);
 ~NxD6JointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxD6JointDesc &desc,CustomCopy &cc);
  void copyTo(::NxD6JointDesc &desc,CustomCopy &cc);
/**
\brief Define the linear degrees of freedom.
*/
  NxD6JointMotion                                xMotion;
/**
\brief Define the linear degrees of freedom.
*/
  NxD6JointMotion                                yMotion;
/**
\brief Define the linear degrees of freedom.
*/
  NxD6JointMotion                                zMotion;
/**
\brief Define the angular degrees of freedom. 
*/
  NxD6JointMotion                                swing1Motion;
/**
\brief Define the angular degrees of freedom. 
*/
  NxD6JointMotion                                swing2Motion;
/**
\brief Define the angular degrees of freedom. 
*/
  NxD6JointMotion                                twistMotion;
/**
\brief If some linear DOF are limited, linearLimit defines the characteristics of these limits.
*/
  NxJointLimitSoftDesc                           linearLimit;
/**
\brief If swing1Motion is NX_D6JOINT_MOTION_LIMITED, swing1Limit defines the characteristics of the limit.
*/
  NxJointLimitSoftDesc                           swing1Limit;
/**
\brief If swing2Motion is NX_D6JOINT_MOTION_LIMITED, swing2Limit defines the characteristics of the limit.
*/
  NxJointLimitSoftDesc                           swing2Limit;
/**
\brief If twistMotion is NX_D6JOINT_MOTION_LIMITED, twistLimit defines the characteristics of the limit.
*/
  NxJointLimitSoftPairDesc                       twistLimit;
/**
\brief Drive the three linear DOF.
*/
  NxJointDriveDesc                               xDrive;
/**
\brief Drive the three linear DOF.
*/
  NxJointDriveDesc                               yDrive;
/**
\brief Drive the three linear DOF.
*/
  NxJointDriveDesc                               zDrive;
/**
\brief These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.
*/
  NxJointDriveDesc                               swingDrive;
/**
\brief These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set.
*/
  NxJointDriveDesc                               twistDrive;
/**
\brief This drive is used if the flag NX_D6JOINT_SLERP_DRIVE is set.
*/
  NxJointDriveDesc                               slerpDrive;
/**
\brief If the type of xDrive (yDrive,zDrive) is NX_D6JOINT_DRIVE_POSITION, drivePosition defines the goal position.
*/
  NxVec3                                         drivePosition;
/**
\brief If the type of swingDrive or twistDrive is NX_D6JOINT_DRIVE_POSITION, driveOrientation defines the goal orientation.
*/
  NxQuat                                         driveOrientation;
/**
\brief If the type of xDrive (yDrive,zDrive) is NX_D6JOINT_DRIVE_VELOCITY, driveLinearVelocity defines the goal linear velocity.
*/
  NxVec3                                         driveLinearVelocity;
/**
\brief If the type of swingDrive or twistDrive is NX_D6JOINT_DRIVE_VELOCITY, driveAngularVelocity defines the goal angular velocity.
*/
  NxVec3                                         driveAngularVelocity;
/**
\brief If projectionMode is NX_JPM_NONE, projection is disabled. If NX_JPM_POINT_MINDIST, bodies are projected to limits leaving an linear error of projectionDistance and an angular error of projectionAngle.
*/
  NxJointProjectionMode                          projectionMode;
/**
\brief The distance above which to project the joint.
*/
  NxF32                                          projectionDistance;
/**
\brief The angle above which to project the joint.
*/
  NxF32                                          projectionAngle;
/**
\brief when the flag NX_D6JOINT_GEAR_ENABLED is set, the angular velocity of the second actor is driven towards the angular velocity of the first actor times gearRatio (both w.r.t. their primary axis) 
*/
  NxF32                                          gearRatio;
/**
\brief This is a combination of the bits defined by NxD6JointFlag.
*/
  NxD6JointFlag                                  flags;
  void *mInstance;
};

/**
\brief Desc class for distance joint. See NxDistanceJoint.
*/
class NxDistanceJointDesc : public NxJointDesc
{
public:
  NxDistanceJointDesc(void);
 ~NxDistanceJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxDistanceJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxDistanceJointDesc &desc,CustomCopy &cc);
/**
\brief The maximum rest length of the rope or rod between the two anchor points.
*/
  NxF32                                          maxDistance;
/**
\brief The minimum rest length of the rope or rod between the two anchor points.
*/
  NxF32                                          minDistance;
/**
\brief makes the joint springy. The spring.targetValue field is not used.
*/
  NxSpringDesc                                   spring;
/**
\brief This is a combination of the bits defined by NxDistanceJointFlag.
*/
  NxDistanceJointFlag                            flags;
  void *mInstance;
};

/**
\brief Desc class for fixed joint.
*/
class NxFixedJointDesc : public NxJointDesc
{
public:
  NxFixedJointDesc(void);
 ~NxFixedJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxFixedJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxFixedJointDesc &desc,CustomCopy &cc);
  void *mInstance;
};

/**
\brief Desc class for point-in-plane joint. See NxPointInPlaneJoint. 
*/
class NxPointInPlaneJointDesc : public NxJointDesc
{
public:
  NxPointInPlaneJointDesc(void);
 ~NxPointInPlaneJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPointInPlaneJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxPointInPlaneJointDesc &desc,CustomCopy &cc);
  void *mInstance;
};

/**
\brief Desc class for point-on-line joint.
*/
class NxPointOnLineJointDesc : public NxJointDesc
{
public:
  NxPointOnLineJointDesc(void);
 ~NxPointOnLineJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPointOnLineJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxPointOnLineJointDesc &desc,CustomCopy &cc);
  void *mInstance;
};

/**
\brief Desc class for NxPrismaticJoint.
*/
class NxPrismaticJointDesc : public NxJointDesc
{
public:
  NxPrismaticJointDesc(void);
 ~NxPrismaticJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPrismaticJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxPrismaticJointDesc &desc,CustomCopy &cc);
  void *mInstance;
};

/**
\brief Describes a joint limit.
*/
class NxJointLimitDesc
{
public:
  NxJointLimitDesc(void);
 ~NxJointLimitDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointLimitDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointLimitDesc &desc,CustomCopy &cc);
/**
\brief The angle / position beyond which the limit is active.
*/
  NxF32                                          value;
/**
\brief limit bounce
*/
  NxF32                                          restitution;
/**
\brief [not yet implemented!] limit can be made softer by setting this to less than 1.
*/
  NxF32                                          hardness;
  void *mInstance;
};

/**
\brief Describes a pair of joint limits.
*/
class NxJointLimitPairDesc
{
public:
  NxJointLimitPairDesc(void);
 ~NxJointLimitPairDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxJointLimitPairDesc &desc,CustomCopy &cc);
  void copyTo(::NxJointLimitPairDesc &desc,CustomCopy &cc);
/**
\brief The low limit (smaller value).
*/
  NxJointLimitDesc                               low;
/**
\brief the high limit (larger value)
*/
  NxJointLimitDesc                               high;
  void *mInstance;
};

/**
\brief Describes a joint motor. Some joints can be motorized, this allows them to apply a force to cause attached actors to move. Joints which can be motorized: NxPulleyJoint NxRevoluteJoint NxJointDriveDesc is used for a similar purpose with NxD6Joint.
*/
class NxMotorDesc
{
public:
  NxMotorDesc(void);
 ~NxMotorDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxMotorDesc &desc,CustomCopy &cc);
  void copyTo(::NxMotorDesc &desc,CustomCopy &cc);
/**
\brief The relative velocity the motor is trying to achieve.
*/
  NxF32                                          velTarget;
/**
\brief The maximum force (or torque) the motor can exert.
*/
  NxF32                                          maxForce;
/**
\brief If true, motor will not brake when it spins faster than velTarget.
*/
  NX_BOOL                                        freeSpin;
  void *mInstance;
};

/**
\brief Desc class for NxRevoluteJoint.
*/
class NxRevoluteJointDesc : public NxJointDesc
{
public:
  NxRevoluteJointDesc(void);
 ~NxRevoluteJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxRevoluteJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxRevoluteJointDesc &desc,CustomCopy &cc);
/**
\brief Optional limits for the angular motion of the joint.
*/
  NxJointLimitPairDesc                           limit;
/**
\brief Optional motor.
*/
  NxMotorDesc                                    motor;
/**
\brief Optional spring.
*/
  NxSpringDesc                                   spring;
/**
\brief The distance beyond which the joint is projected.
*/
  NxF32                                          projectionDistance;
/**
\brief The angle beyond which the joint is projected.
*/
  NxF32                                          projectionAngle;
/**
\brief This is a combination of the bits defined by NxRevoluteJointFlag.
*/
  NxRevoluteJointFlag                            flags;
/**
\brief use this to enable joint projection 
*/
  NxJointProjectionMode                          projectionMode;
  void *mInstance;
};

/**
\brief Desc class for an NxSphericalJoint.
*/
class NxSphericalJointDesc : public NxJointDesc
{
public:
  NxSphericalJointDesc(void);
 ~NxSphericalJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSphericalJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxSphericalJointDesc &desc,CustomCopy &cc);
/**
\brief swing limit axis defined in the joint space of actor 0.
*/
  NxVec3                                         swingAxis;
/**
\brief Distance above which to project joint.
*/
  NxF32                                          projectionDistance;
/**
\brief limits rotation around twist axis
*/
  NxJointLimitPairDesc                           twistLimit;
/**
\brief limits swing of twist axis
*/
  NxJointLimitDesc                               swingLimit;
/**
\brief spring that works against twisting
*/
  NxSpringDesc                                   twistSpring;
/**
\brief spring that works against swinging
*/
  NxSpringDesc                                   swingSpring;
/**
\brief spring that lets the joint get pulled apart 
*/
  NxSpringDesc                                   jointSpring;
/**
\brief This is a combination of the bits defined by NxSphericalJointFlag
*/
  NxSphericalJointFlag                           flags;
/**
\brief use this to enable joint projection
*/
  NxJointProjectionMode                          projectionMode;
  void *mInstance;
};

/**
\brief Desc class for NxPulleyJoint. 
*/
class NxPulleyJointDesc : public NxJointDesc
{
public:
  NxPulleyJointDesc(void);
 ~NxPulleyJointDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPulleyJointDesc &desc,CustomCopy &cc);
  void copyTo(::NxPulleyJointDesc &desc,CustomCopy &cc);
/**
\brief suspension points of two bodies in world space.
*/
  NxVec3                                         pulley0;
/**
\brief suspension points of two bodies in world space.
*/
  NxVec3                                         pulley1;
/**
\brief the rest length of the rope connecting the two objects.
*/
  NxF32                                          distance;
/**
\brief how stiff the constraint is, between 0 and 1 (stiffest)
*/
  NxF32                                          stiffness;
/**
\brief transmission ratio
*/
  NxF32                                          ratio;
/**
\brief This is a combination of the bits defined by NxPulleyJointFlag.
*/
  NxPulleyJointFlag                              flags;
/**
\brief Optional joint motor. 
*/
  NxMotorDesc                                    motor;
  void *mInstance;
};

/**
\brief Struct used by NxScene::getPairFlagArray(). The high bit of each 32 bit flag field denotes whether a pair is a shape or an actor pair. The flags are defined by the enum NxContactPairFlag.
*/
class NxPairFlagDesc
{
public:
  NxPairFlagDesc(void);
 ~NxPairFlagDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief Id of this pair flag
*/
  const char *                                   mId;
/**
\brief Optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief Whether this is an actor pair or a shape pair
*/
  NX_BOOL                                        mIsActorPair;
/**
\brief Contact flags
*/
  NxContactPairFlag                              mFlags;
  const char *                                   mActor0;
  NxU32                                          mShapeIndex0;
  const char *                                   mActor1;
  NxU32                                          mShapeIndex1;
  void *mInstance;
};

/**
\brief Defines a set of paired collisions
*/
class NxCollisionGroupDesc
{
public:
  NxCollisionGroupDesc(void);
 ~NxCollisionGroupDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief Collision Group A
*/
  NxU32                                          mCollisionGroupA;
/**
\brief Collision Group B
*/
  NxU32                                          mCollisionGroupB;
/**
\brief Indicates whether collisions are enabled or disabled between these two groups.
*/
  NX_BOOL                                        mEnable;
  void *mInstance;
};

#if NX_USE_FLUID_API
/**
\brief Descriptor-like user-side class describing a set of fluid particles. NxParticleData is used to submit particles to the simulation and to retrieve information about particles in the simulation. Each particle is described by its position, velocity, lifetime, density, and a set of (NxParticleFlag) flags. The memory for the particle data is allocated by the application, making this class a 
*/
class NxParticleData
{
public:
  NxParticleData(void);
 ~NxParticleData(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxParticleData &desc,CustomCopy &cc);
  void copyTo(::NxParticleData &desc,CustomCopy &cc);
/**
\brief Optional debug name
*/
  const char *                                   name;
/**
\brief Specifies the maximal number of particle elements the buffers can hold.
*/
#if NX_SDK_VERSION_NUMBER < 270
  NxU32                                          maxParticles;
#endif
/**
\brief The pointer to the user-allocated buffer for particle positions. 
*/
  NxArray< NxVec3 >                              mBufferPos;
/**
\brief The pointer to the user-allocated buffer for particle velocities.
*/
  NxArray< NxVec3 >                              mBufferVel;
/**
\brief The pointer to the user-allocated buffer for particle lifetimes.
*/
  NxArray< NxF32 >                               mBufferLife;
/**
\brief The pointer to the user-allocated buffer for particle densities.
*/
  NxArray< NxF32 >                               mBufferDensity;
/**
\brief The pointer to the user-allocated buffer for particle IDs
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxArray< NxU32 >                               mBufferId;
#endif
/**
\brief The pointer to the user-allocated buffer for particle flags.
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxArray< NxU32 >                               mBufferFlag;
#endif
/**
\brief Keeps track of the exact number of particles in this buffer.
*/
  NxU32                                          mParticleCount;
  void *mInstance;
};

#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
#endif
#if NX_USE_FLUID_API
/**
\brief Descriptor for NxFluidEmitter class. Used for saving and loading the emitter state
*/
class NxFluidEmitterDesc
{
public:
  NxFluidEmitterDesc(void);
 ~NxFluidEmitterDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxFluidEmitterDesc &desc,CustomCopy &cc);
  void copyTo(::NxFluidEmitterDesc &desc,CustomCopy &cc);
/**
\brief Id of emitter
*/
  const char *                                   mId;
/**
\brief user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief The emitter's pose relative to the frameShape.
*/
  NxMat34                                        relPose;
/**
\brief actor id it is attached to
*/
  const char *                                   mFrameActor;
/**
\brief Shape index into frame actor above
*/
  NxU32                                          mFrameShape;
/**
\brief The emitter's mode of operation.
*/
  NxEmitterType                                  type;
/**
\brief The maximum number of particles which are emitted from this emitter.
*/
  NxU32                                          maxParticles;
/**
\brief The emitter's shape can either be rectangular or elliptical.
*/
  NxEmitterShape                                 shape;
/**
\brief The sizes of the emitter in the directions of the first and the second axis of its orientation frame (relPose).
*/
  NxF32                                          dimensionX;
/**
\brief The sizes of the emitter in the directions of the first and the second axis of its orientation frame (relPose). 
*/
  NxF32                                          dimensionY;
/**
\brief Random vector with values for each axis direction of the emitter orientation.
*/
  NxVec3                                         randomPos;
/**
\brief Random angle deviation from emission direction. 
*/
  NxF32                                          randomAngle;
/**
\brief The velocity magnitude of the emitted fluid particles.
*/
  NxF32                                          fluidVelocityMagnitude;
/**
\brief The rate specifies how many particles are emitted per second.
*/
  NxF32                                          rate;
/**
\brief This specifies the time in seconds an emitted particle lives. 
*/
  NxF32                                          particleLifetime;
/**
\brief Defines a factor for the impulse transfer from attached emitter to body
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxF32                                          repulsionCoefficient;
#endif
/**
\brief A combination of NxFluidEmitterFlags. 
*/
  NxFluidEmitterFlag                             flags;
  void *mInstance;
};

#endif
#if NX_USE_FLUID_API
/**
\brief Describes an NxFluid.
*/
class NxFluidDesc
{
public:
  NxFluidDesc(void);
 ~NxFluidDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxFluidDesc &desc,CustomCopy &cc);
  void copyTo(::NxFluidDesc &desc,CustomCopy &cc);
/**
\brief Id tag of fluid
*/
  const char *                                   mId;
/**
\brief optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief Possible debug name. The string is not copied by the SDK, only the pointer is stored.
*/
  const char *                                   name;
/**
\brief Describes the particles which are added to the fluid initially.
*/
  NxParticleData                                 mInitialParticleData;
/**
\brief Sets the maximal number of particles for the fluid used in the simulation.
*/
  NxU32                                          mMaxFluidParticles;
/**
\brief Sets a limit on the number of particles for the fluid.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU32                                          mCurrentParticleLimit;
#endif
/**
\brief Defines the number of particles which are reserved for creation at runtime.
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxU32                                          numReserveParticles;
#endif
/**
\brief Target density for the fluid (water is about 1000).
*/
  NxF32                                          restDensity;
/**
\brief The particle resolution given as particles per linear meter measured when the fluid is in its rest state (relaxed)
*/
  NxF32                                          restParticlesPerMeter;
/**
\brief Radius of sphere of influence for particle interaction. 
*/
  NxF32                                          kernelRadiusMultiplier;
/**
\brief Maximal distance a particle is allowed to travel within one timestep.
*/
  NxF32                                          motionLimitMultiplier;
/**
\brief Defines the distance between particles and collision geometry, which is maintained during simulation.
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxF32                                          collisionDistanceMultiplier;
#endif
/**
\brief This parameter controls the parallelization of the fluid.
*/
  NxU32                                          packetSizeMultiplier;
/**
\brief The stiffness of the particle interaction related to the pressure.
*/
  NxF32                                          stiffness;
/**
\brief The viscosity of the fluid defines its viscous behaviour.
*/
  NxF32                                          viscosity;
/**
\brief Velocity damping constant, which is globally applied to each particle.
*/
  NxF32                                          damping;
/**
\brief Defines a timespan for the particle to fade in
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxF32                                          fadeInTime;
#endif
/**
\brief Acceleration (m/s^2) applied to all particles at all time steps.
*/
  NxVec3                                         externalAcceleration;
/**
\brief Defines plane particles are projected to
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxPlane                                        projectionPlane;
#endif
/**
\brief Defines the restitution coefficient used for collisions of the fluid particles with static shapes.
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxF32                                          staticCollisionRestitution;
#endif
/**
\brief Defines the restitution coefficient used for collisions of the fluid particles with static shapes.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          restitutionForStaticShapes;
#endif
/**
\brief Defines the 'friction' of the fluid regarding the surface of a static shape.
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxF32                                          staticCollisionAdhesion;
#endif
/**
\brief Defines the dynamic friction of the fluid regarding the surface of a static shape.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          dynamicFrictionForStaticShapes;
#endif
/**
\brief Defines the static friction of the fluid regarding the surface of a static shape.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          staticFrictionForStaticShapes;
#endif
/**
\brief Defines the attraction of the fluid regarding the surface of a static shape.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          attractionForStaticShapes;
#endif
/**
\brief Defines the restitution coefficient used for collisions of the fluid particles with dynamic shapes.
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxF32                                          dynamicCollisionRestitution;
#endif
/**
\brief Defines the restitution coefficient used for collisions of the fluid particles with dynamic shapes.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          restitutionForDynamicShapes;
#endif
/**
\brief Defines the 'friction' of the fluid regarding the surface of a dynamic shape.
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxF32                                          dynamicCollisionAdhesion;
#endif
/**
\brief Defines the dynamic friction coefficient used for collisions of the fluid particles with dynamic shapes.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          dynamicFrictionForDynamicShapes;
#endif
/**
\brief Defines the static friction coefficient used for collisions of the fluid particles with dynamic shapes.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          staticFrictionForDynamicShapes;
#endif
/**
\brief Defines the attraction coefficient used for collisions of the fluid particles with dynamic shapes.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          attractionForDynamicShapes;
#endif
/**
\brief Defines a factor for the impulse transfer from fluid to colliding rigid bodies in the range of 0 to 1.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          collisionResponseCoefficient;
#endif
/**
\brief NxFluidSimulationMethod flags. Defines whether or not particle interactions are considered in the simulation.
*/
  NxFluidSimulationMethod                        simulationMethod;
/**
\brief NxFluidCollisionMethod flags. Selects whether static collision and/or dynamic collision with the environment is performed.
*/
  NxFluidCollisionMethod                         collisionMethod;
/**
\brief Sets which collision group this fluid is part of.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxU16                                          collisionGroup;
#endif
/**
\brief Sets the 128-bit mask used for collision filtering.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxGroupsMask                                   groupsMask;
#endif
/**
\brief Flags defining certain properties of the fluid.
*/
  NxFluidFlag                                    flags;
/**
\brief Name of compartment
*/
#if NX_SDK_VERSION_NUMBER >= 260
  const char *                                   mCompartment;
#endif
/**
\brief Emitters for this fluid
*/
  NxArray< NxFluidEmitterDesc *>                 mEmitters;
/**
\brief force field material index for the scaling table
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU16                                          forceFieldMaterial;
#endif
/**
\brief The surfaceTension of the fluid defines an attractive force between particles.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxF32                                          surfaceTension;
#endif
  void *mInstance;
};

#endif
/**
\brief Descriptor class for NxCloth.
*/
class NxClothDesc
{
public:
  NxClothDesc(void);
 ~NxClothDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxClothDesc &desc,CustomCopy &cc);
  void copyTo(::NxClothDesc &desc,CustomCopy &cc);
/**
\brief Id
*/
  const char *                                   mId;
/**
\brief User properties field
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief The name of the cloth mesh to use
*/
  const char *                                   mClothMesh;
/**
\brief The global pose of the cloth in the world.
*/
  NxMat34                                        globalPose;
/**
\brief Thickness of the cloth.
*/
  NxF32                                          thickness;
/**
\brief Density of the cloth (mass per area).
*/
  NxF32                                          density;
/**
\brief Bending stiffness of the cloth in the range 0 to 1.
*/
  NxF32                                          bendingStiffness;
/**
\brief Stretching stiffness of the cloth in the range 0 to 1.
*/
  NxF32                                          stretchingStiffness;
/**
\brief Spring damping of the cloth in the range 0 to 1.
*/
  NxF32                                          dampingCoefficient;
/**
\brief Friction coefficient in the range 0 to 1.
*/
  NxF32                                          friction;
/**
\brief If the flag NX_CLF_PRESSURE is set, this variable defines the volume of air inside the mesh as volume = pressure * restVolume.:
*/
  NxF32                                          pressure;
/**
\brief If the flag NX_CLF_TEARABLE is set, this variable defines the elongation factor that causes the cloth to tear.
*/
  NxF32                                          tearFactor;
/**
\brief Defines a factor for the impulse transfer from cloth to colliding rigid bodies.
*/
  NxF32                                          collisionResponseCoefficient;
/**
\brief Defines a factor for the impulse transfer from cloth to attached rigid bodies.
*/
  NxF32                                          attachmentResponseCoefficient;
/**
\brief If the flag NX_CLOTH_ATTACHMENT_TEARABLE is set in the attachment method of NxCloth, this variable defines the elongation factor that causes the attachment to tear.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          attachmentTearFactor;
#endif
/**
\brief Defines a factor for the impule transfer fromt his cloth to colliding fluids
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxF32                                          toFluidResponseCoefficient;
#endif
/**
\brief Defines a factor for the impule transfer from colliding fluids to this cloth
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxF32                                          fromFluidResponseCoefficient;
#endif
/**
\brief Number of solver iterations.
*/
  NxU32                                          solverIterations;
/**
\brief External acceleration which affects all non attached particles of the cloth.
*/
  NxVec3                                         externalAcceleration;
/**
\brief Acceleration which acts normal to the cloth surface at each vertex.
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxVec3                                         windAcceleration;
#endif
/**
\brief The cloth wake up counter.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          wakeUpCounter;
#endif
/**
\brief Maximum linear velocity at which cloth can go to sleep.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxF32                                          sleepLinearVelocity;
#endif
/**
\brief Sets which collision group this cloth is part of.
*/
  NxU16                                          collisionGroup;
/**
\brief Sets the 128-bit mask used for collision filtering.
*/
  NxGroupsMask                                   groupsMask;
/**
\brief Cloth flag bits
*/
  NxClothFlag                                    flags;
/**
\brief Name of compartment
*/
#if NX_SDK_VERSION_NUMBER >= 260
  const char *                                   mCompartment;
#endif
/**
\brief Binary data of cloth in simulated state
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxArray< NxU8 >                                mActiveState;
#endif
/**
\brief Cloth attachments
*/
  NxArray< NxClothAttachDesc *>                  mAttachments;
/**
\brief If the flag NX_CLF_VALIDBOUNDS is set, this variable defines the volume outside of which cloth particles are automatically removed from the simulation
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxBounds3                                      validBounds;
#endif
/**
\brief Defines the size of grid cells for collision detection
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxReal                                         relativeGridSpacing;
#endif
/**
\brief If the NX_CLF_ADHERE flag is set the cloth moves partially in the frame of the attached actor.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxReal                                         minAdhereVelocity;
#endif
/**
\brief force field material index for the scaling table
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU16                                          forceFieldMaterial;
#endif
/**
\brief number of solver iterations of the hierarchical solver
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxU32                                          hierarchicalSolverIterations;
#endif
/**
\brief Size of the particle diameters used for self collision and inter-collision
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxReal                                         selfCollisionThickness;
#endif
/**
\brief Defines the hard stretch elongation limit
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxReal                                         hardStretchLimitationFactor;
#endif
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 270
#endif
#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief Descriptor class for NxForceFieldShape
*/
class NxForceFieldShapeDesc
{
public:
  NxForceFieldShapeDesc(void);
 ~NxForceFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxForceFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxForceFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief World to shape or force field to shape transform
*/
  NxMat34                                        pose;
/**
\brief Various force field shape flags
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxU32                                          flags;
#endif
  void *mInstance;
  SCHEMA_CLASS mType;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief Descriptor class for NxBoxForceFieldShape
*/
class NxBoxForceFieldShapeDesc : public NxForceFieldShapeDesc
{
public:
  NxBoxForceFieldShapeDesc(void);
 ~NxBoxForceFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxBoxForceFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxBoxForceFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Dimensions of the box
*/
  NxVec3                                         dimensions;
  void *mInstance;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief Descriptor class for NxSphereForceFieldShape
*/
class NxSphereForceFieldShapeDesc : public NxForceFieldShapeDesc
{
public:
  NxSphereForceFieldShapeDesc(void);
 ~NxSphereForceFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSphereForceFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxSphereForceFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Radius of the capsule
*/
  NxReal                                         radius;
  void *mInstance;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief Descriptor class for NxCapsuleForceFieldShape
*/
class NxCapsuleForceFieldShapeDesc : public NxForceFieldShapeDesc
{
public:
  NxCapsuleForceFieldShapeDesc(void);
 ~NxCapsuleForceFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxCapsuleForceFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxCapsuleForceFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Radius of the capsule
*/
  NxReal                                         radius;
/**
\brief Height of the capsule
*/
  NxReal                                         height;
  void *mInstance;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 272
/**
\brief Descriptor class for NxConvexForceFieldShape
*/
class NxConvexForceFieldShapeDesc : public NxForceFieldShapeDesc
{
public:
  NxConvexForceFieldShapeDesc(void);
 ~NxConvexForceFieldShapeDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxConvexForceFieldShapeDesc &desc,CustomCopy &cc);
  void copyTo(::NxConvexForceFieldShapeDesc &desc,CustomCopy &cc);
/**
\brief Named reference of the convex mesh we want to instance
*/
  const char *                                   mMeshData;
  void *mInstance;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief Descriptor class for NxForceFieldShapeGroup
*/
class NxForceFieldShapeGroupDesc
{
public:
  NxForceFieldShapeGroupDesc(void);
 ~NxForceFieldShapeGroupDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxForceFieldShapeGroupDesc &desc,CustomCopy &cc);
  void copyTo(::NxForceFieldShapeGroupDesc &desc,CustomCopy &cc);
/**
\brief Name of the group
*/
  const char *                                   mId;
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief Force field group flags
*/
  NxForceFieldShapeGroupFlags                    flags;
/**
\brief Force field group's shapes
*/
  NxArray<NxForceFieldShapeDesc*>                mShapes;
  void *mInstance;
};

#endif
#if NX_SDK_VERSION_NUMBER >= 280
/**
\brief Descriptor class for NxForceFieldLinearKernelDesc
*/
class NxForceFieldLinearKernelDesc
{
public:
  NxForceFieldLinearKernelDesc(void);
 ~NxForceFieldLinearKernelDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxForceFieldLinearKernelDesc &desc,CustomCopy &cc);
  void copyTo(::NxForceFieldLinearKernelDesc &desc,CustomCopy &cc);
/**
\brief Name of the group
*/
  const char *                                   mId;
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
  const char *                                   name;
/**
\brief Constant part of force field function
*/
  NxVec3                                         constant;
/**
\brief coefficient of force field function position term
*/
  NxMat33                                        positionMultiplier;
/**
\brief force field position target
*/
  NxVec3                                         positionTarget;
/**
\brief coefficient of force field function velocity term
*/
  NxMat33                                        velocityMultiplier;
/**
\brief force field velocity target
*/
  NxVec3                                         velocityTarget;
/**
\brief radius for NX_FFC_TOROIDAL type coordinates
*/
  NxF32                                          torusRadius;
/**
\brief Linear falloff component
*/
  NxVec3                                         falloffLinear;
/**
\brief Quadratic falloff component
*/
  NxVec3                                         falloffQuadratic;
/**
\brief noise scaling
*/
  NxVec3                                         noise;
  void *mInstance;
};

#endif
/**
\brief Force Field Scale Table Entry
*/
class NxForceFieldScaleTableEntry
{
public:
  NxForceFieldScaleTableEntry(void);
 ~NxForceFieldScaleTableEntry(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief Force Field Variety
*/
  NxU16                                          var;
/**
\brief Force Field Material
*/
  NxU16                                          mat;
/**
\brief Force Field Scale
*/
  NxReal                                         scale;
  void *mInstance;
};

/**
\brief NxForceFieldGroupReference
*/
class NxForceFieldGroupReference
{
public:
  NxForceFieldGroupReference(void);
 ~NxForceFieldGroupReference(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief NxForceFieldGroupReference
*/
  const char*                                    mRef;
  void *mInstance;
};

#if NX_SDK_VERSION_NUMBER >= 270
/**
\brief Descriptor class for NxForceField
*/
class NxForceFieldDesc
{
public:
  NxForceFieldDesc(void);
 ~NxForceFieldDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxForceFieldDesc &desc,CustomCopy &cc);
  void copyTo(::NxForceFieldDesc &desc,CustomCopy &cc);
/**
\brief Name of effector
*/
  const char *                                   mId;
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
#if NX_SDK_VERSION_NUMBER >= 272
  const char *                                   name;
#endif
/**
\brief optional actor to attach the force field too
*/
  const char *                                   mActor;
/**
\brief Global or actor relative transformation of the force field
*/
  NxMat34                                        pose;
/**
\brief Coordinate space of the field
*/
  NxForceFieldCoordinates                        coordinates;
/**
\brief Constant part of force field function
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         constant;
#endif
/**
\brief scaling for white noise component of force field function
*/
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         noisiness;
#endif
/**
\brief linear falloff
*/
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         linearFalloff;
#endif
/**
\brief offset of force field singularity
*/
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         offset;
#endif
/**
\brief force field scaling factor
*/
#if NX_SDK_VERSION_NUMBER == 270 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         magnitude;
#endif
/**
\brief coefficient of force field function position term
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxMat33                                        positionMultiplier;
#endif
/**
\brief coefficient of force field function velocity term
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxMat33                                        velocityMultiplier;
#endif
/**
\brief radius for NX_FFC_TOROIDAL type coordinates
*/
#if NX_SDK_VERSION_NUMBER < 280
  NxF32                                          torusRadius;
#endif
/**
\brief force field position target
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         positionTarget;
#endif
/**
\brief force field velocity target
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         velocityTarget;
#endif
/**
\brief noise scaling
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         noise;
#endif
/**
\brief Force field's shapes
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxArray<NxForceFieldShapeDesc*>                mFFshapes;
#endif
/**
\brief Collision group used for collision filtering
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxU16                                          group;
#endif
/**
\brief Groups mask used for collision filtering
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxGroupsMask                                   groupsMask;
#endif
/**
\brief Linear falloff component
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         falloffLinear;
#endif
/**
\brief Quadratic falloff component
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxVec3                                         falloffQuadratic;
#endif
/**
\brief Force scale factor for fluids
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxReal                                         fluidScale;
#endif
/**
\brief Force scale factor for cloth
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxReal                                         clothScale;
#endif
/**
\brief Force scale factor for soft bodies
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxReal                                         softBodyScale;
#endif
/**
\brief Force scale factor for rigid bodies
*/
#if NX_SDK_VERSION_NUMBER >= 272 && NX_SDK_VERSION_NUMBER < 280
  NxReal                                         rigidBodyScale;
#endif
/**
\brief Force field flags
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxForceFieldFlags                              flags;
#endif
/**
\brief Force field type for fluids
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxForceFieldType                               fluidType;
#endif
/**
\brief Force field type for cloth
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxForceFieldType                               clothType;
#endif
/**
\brief Force field type for soft bodies
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxForceFieldType                               softBodyType;
#endif
/**
\brief Force field type for rigid bodies
*/
#if NX_SDK_VERSION_NUMBER >= 273 && NX_SDK_VERSION_NUMBER != 274
  NxForceFieldType                               rigidBodyType;
#endif
/**
\brief force field variety index for the scaling table
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU16                                          forceFieldVariety;
#endif
/**
\brief kernel function of the force field
*/
#if NX_SDK_VERSION_NUMBER >= 280
  const char *                                   mKernel;
#endif
/**
\brief A special shape group which moves with the force field.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxArray<NxForceFieldShapeDesc*>                mIncludeShapes;
#endif
/**
\brief Shape groups which do not move with the force field
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxArray<NxForceFieldGroupReference*>           mShapeGroups;
#endif
  void *mInstance;
};

#endif
/**
\brief Desc class for NxSpringAndDamperEffector.
*/
class NxSpringAndDamperEffectorDesc
{
public:
  NxSpringAndDamperEffectorDesc(void);
 ~NxSpringAndDamperEffectorDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSpringAndDamperEffectorDesc &desc,CustomCopy &cc);
  void copyTo(::NxSpringAndDamperEffectorDesc &desc,CustomCopy &cc);
/**
\brief Name of effector
*/
  const char *                                   mId;
/**
\brief Optional custom user properties
*/
  const char *                                   mUserProperties;
/**
\brief optional name field
*/
#if NX_SDK_VERSION_NUMBER >= 270
  const char *                                   name;
#endif
/**
\brief First actor attached to
*/
  const char *                                   mBody1;
/**
\brief Second actor attached to
*/
  const char *                                   mBody2;
/**
\brief First attachment point
*/
  NxVec3                                         pos1;
/**
\brief Second attachment point
*/
  NxVec3                                         pos2;
/**
\brief The distance at which the maximum repulsive force is attained (at shorter distances it remains the same).
*/
  NxF32                                          springDistCompressSaturate;
/**
\brief The distance at which the spring force is zero.
*/
  NxF32                                          springDistRelaxed;
/**
\brief The distance at which the attractive spring force attains its maximum (farther away it remains the same).
*/
  NxF32                                          springDistStretchSaturate;
/**
\brief The maximum repulsive spring force, attained at distance springDistCompressSaturate.
*/
  NxF32                                          springMaxCompressForce;
/**
\brief The maximum attractive spring force, attained at distance springDistStretchSaturate.
*/
  NxF32                                          springMaxStretchForce;
/**
\brief The relative velocity (negative) at which the repulsive damping force attains its maximum.
*/
  NxF32                                          damperVelCompressSaturate;
/**
\brief The relative velocity at which the attractive damping force attains its maximum.
*/
  NxF32                                          damperVelStretchSaturate;
/**
\brief The maximum repulsive damping force, attained at relative velocity damperVelCompressSaturate.
*/
  NxF32                                          damperMaxCompressForce;
/**
\brief The maximum attractive damping force, attained at relative velocity damperVelStretchSaturate.
*/
  NxF32                                          damperMaxStretchForce;
  void *mInstance;
};

#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
/**
\brief The mesh data is *copied* when an NxSoftBodyMesh object is created from this descriptor. After the creation the user may discard the basic mesh data.
*/
class NxSoftBodyMeshDesc
{
public:
  NxSoftBodyMeshDesc(void);
 ~NxSoftBodyMeshDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSoftBodyMeshDesc &desc,CustomCopy &cc);
  void copyTo(::NxSoftBodyMeshDesc &desc,CustomCopy &cc);
/**
\brief Id
*/
  const char *                                   mId;
/**
\brief User properties field
*/
  const char *                                   mUserProperties;
/**
\brief List of input vertices
*/
  NxArray< NxVec3 >                              mVertices;
/**
\brief List of input tetrahedra
*/
  NxArray< NxTetra >                             mTetrahedra;
/**
\brief SoftBody mesh flags
*/
  NxU32                                          flags;
/**
\brief List of per-vertex mass assignments
*/
  NxArray< NxF32 >                               mVertexMasses;
/**
\brief List of per-vertex flags
*/
  NxArray< NxU32 >                               mVertexFlags;
  void *mInstance;
};

#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
#endif
#if NX_USE_SOFTBODY_API
/**
\brief A class to represent shape / soft body attachments
*/
class NxSoftBodyAttachDesc
{
public:
  NxSoftBodyAttachDesc(void);
 ~NxSoftBodyAttachDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief The actor the cloth is attached to.
*/
  const char *                                   mAttachActor;
/**
\brief The shape index of the actor which it is attached to.
*/
  NxU32                                          mAttachShapeIndex;
/**
\brief The attachment flags
*/
  NxSoftBodyAttachmentFlag                       flags;
  void *mInstance;
};

#endif
#if NX_USE_SOFTBODY_API
/**
\brief Descriptor class for NxSoftBody.
*/
class NxSoftBodyDesc
{
public:
  NxSoftBodyDesc(void);
 ~NxSoftBodyDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSoftBodyDesc &desc,CustomCopy &cc);
  void copyTo(::NxSoftBodyDesc &desc,CustomCopy &cc);
/**
\brief Id
*/
  const char *                                   mId;
/**
\brief User properties field
*/
  const char *                                   mUserProperties;
/**
\brief User name field
*/
  const char *                                   name;
/**
\brief The name of the soft body mesh
*/
  const char *                                   mSoftBodyMesh;
/**
\brief The global pose of the soft body in the world.
*/
  NxMat34                                        globalPose;
/**
\brief ize of the particles used for collision detection.
*/
  NxF32                                          particleRadius;
/**
\brief Density of the soft body (mass per volume).
*/
  NxF32                                          density;
/**
\brief Volume stiffness of the soft body in the range 0 to 1.
*/
  NxF32                                          volumeStiffness;
/**
\brief Stretching stiffness of the soft body in the range 0 to 1.
*/
  NxF32                                          stretchingStiffness;
/**
\brief Spring damping of the soft body in the range 0 to 1.
*/
  NxF32                                          dampingCoefficient;
/**
\brief Friction coefficient in the range 0 to 1.
*/
  NxF32                                          friction;
/**
\brief If the flag NX_SBF_TEARABLE is set, this variable defines the elongation factor that causes the soft body to tear.
*/
  NxF32                                          tearFactor;
/**
\brief Defines a factor for the impulse transfer from the soft body to colliding rigid bodies. Only has an effect if NX_SBF_COLLISION_TWOWAY is set.
*/
  NxF32                                          collisionResponseCoefficient;
/**
\brief Defines a factor for the impulse transfer from the soft body to attached rigid bodies. Only has an effect if the mode of the attachment is set to NX_SBF_COLLISION_TWOWAY.
*/
  NxF32                                          attachmentResponseCoefficient;
/**
\brief  If the flag NX_SOFTBODY_ATTACHMENT_TEARABLE is set in the attachment method of NxSoftBody, this variable defines the elongation factor that causes the attachment to tear.
*/
  NxF32                                          attachmentTearFactor;
/**
\brief Defines a factor for the impulse transfer from this soft body to colliding fluids.
*/
  NxF32                                          toFluidResponseCoefficient;
/**
\brief Defines a factor for the impulse transfer from fluids to this soft body.
*/
  NxF32                                          fromFluidResponseCoefficient;
/**
\brief Number of solver iterations.
*/
  NxU32                                          solverIterations;
/**
\brief  External acceleration which affects all non attached particles of the soft body.
*/
  NxVec3                                         externalAcceleration;
/**
\brief  The soft body wake up counter.
*/
  NxF32                                          wakeUpCounter;
/**
\brief  Maximum linear velocity at which the soft body can go to sleep.
*/
  NxF32                                          sleepLinearVelocity;
/**
\brief  Sets which collision group this soft body is part of.
*/
  NxU16                                          collisionGroup;
/**
\brief Sets the 128-bit mask used for collision filtering.
*/
  NxGroupsMask                                   groupsMask;
/**
\brief If the flag NX_SBF_VALIDBOUNDS is set, this variable defins the volume outside of which soft body particles are automatically removed from the simulation
*/
  NxBounds3                                      validBounds;
/**
\brief Flag bits.
*/
  NxSoftBodyFlag                                 flags;
/**
\brief The compartment to place the soft body in. Must be either a pointer to an NxCompartment of type NX_SCT_SOFTBODY, or NULL.
*/
  const char *                                   mCompartment;
/**
\brief Binary data of soft body in simulated state
*/
  NxArray< NxU8 >                                mActiveState;
/**
\brief Soft Body attachments
*/
  NxArray< NxSoftBodyAttachDesc *>               mAttachments;
/**
\brief Defines the size of grid cells for collision detection
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxReal                                         relativeGridSpacing;
#endif
/**
\brief If the NX_SBF_ADHERE flag is set the soft body moves partially in the frame of the attached actor.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxReal                                         minAdhereVelocity;
#endif
/**
\brief force field material index for the scaling table
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU16                                          forceFieldMaterial;
#endif
/**
\brief Size of the particle diameters used for self collision and inter-collision
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxReal                                         selfCollisionThickness;
#endif
/**
\brief Defines the hard stretch elongation limit
*/
#if NX_SDK_VERSION_NUMBER >= 283
  NxReal                                         hardStretchLimitationFactor;
#endif
  void *mInstance;
};

#endif
/**
\brief Descriptor class for scenes. See NxScene.
*/
class NxSceneDesc
{
public:
  NxSceneDesc(void);
 ~NxSceneDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxSceneDesc &desc,CustomCopy &cc);
  void copyTo(::NxSceneDesc &desc,CustomCopy &cc);
/**
\brief Identifier for scene
*/
  const char *                                   mId;
/**
\brief Optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief True if scene has specific bounds
*/
  NX_BOOL                                        mHasMaxBounds;
/**
\brief True if the scene has specific limits
*/
  NX_BOOL                                        mHasLimits;
/**
\brief True if there are specific filter settings
*/
  NX_BOOL                                        mHasFilter;
  NX_BOOL                                        mFilterBool;
/**
\brief Filter operation 0
*/
  NxFilterOp                                     mFilterOp0;
/**
\brief Filter operation 1
*/
  NxFilterOp                                     mFilterOp1;
/**
\brief Filter operation 1
*/
  NxFilterOp                                     mFilterOp2;
/**
\brief Group mask 0
*/
  NxGroupsMask                                   mGroupMask0;
/**
\brief Group mask 1
*/
  NxGroupsMask                                   mGroupMask1;
/**
\brief Gravity vector.
*/
  NxVec3                                         gravity;
/**
\brief Maximum substep size.
*/
  NxF32                                          maxTimestep;
/**
\brief Maximum number of substeps to take.
*/
  NxU32                                          maxIter;
/**
\brief Integration method.
*/
  NxTimeStepMethod                               timeStepMethod;
/**
\brief Max scene bounds.
*/
  NxBounds3                                      mMaxBounds;
/**
\brief Expected scene limits (or NULL).
*/
  NxSceneLimits                                  mLimits;
/**
\brief Used to specify if the scene is a master hardware or software scene.
*/
  NxSimulationType                               simType;
/**
\brief Enable/disable default ground plane.
*/
  NX_BOOL                                        groundPlane;
/**
\brief Enable/disable 6 planes around maxBounds (if available).
*/
  NX_BOOL                                        boundsPlanes;
/**
\brief Flags used to select scene options. 
*/
  NxSceneFlags                                   flags;
/**
\brief Sets the number of SDK managed thread used when running the simulation in parallel.
*/
  NxU32                                          internalThreadCount;
/**
\brief Sets the number of SDK managed threads which will be processing background tasks.
*/
  NxU32                                          backgroundThreadCount;
/**
\brief Allows the user to specify which (logical) processor to allocate SDK internal threads to. 
*/
  NxU32                                          threadMask;
/**
\brief Allows the user to specify which (logical) processor to allocate SDK background threads.
*/
  NxU32                                          backgroundThreadMask;
/**
\brief Allows the user to specify the stack size for the main simulation thread.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxU32                                          simThreadStackSize;
#endif
/**
\brief Sets the thread priority of the main simulation thread.
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxThreadPriority                               simThreadPriority;
#endif
/**
\brief Allows the user to specify which (logical) processor to allocate the simulation thread to.
*/
#if NX_SDK_VERSION_NUMBER >= 271
  NxU32                                          simThreadMask;
#endif
/**
\brief Allows the user to specify the stack size for the worker threads created by the SDK. 
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxU32                                          workerThreadStackSize;
#endif
/**
\brief Sets the thread priority of the SDK created worker threads.
*/
#if NX_SDK_VERSION_NUMBER >= 250
  NxThreadPriority                               workerThreadPriority;
#endif
/**
\brief Sets the thread priority of the SDK created background threads.
*/
#if NX_SDK_VERSION_NUMBER >= 281
  NxThreadPriority                               backgroundThreadPriority;
#endif
/**
\brief Defines the up axis for your world.  This is used to accelerate scene queries like raycasting or sweep tests.  This is only used when 'maxBounds' are defined
*/
#if NX_SDK_VERSION_NUMBER >= 262
  NxU32                                          upAxis;
#endif
/**
\brief Defines the subdivision level for acceleration structures used in scene queries.  Warning, this is only used when maxBounds are defined.
*/
#if NX_SDK_VERSION_NUMBER >= 262
  NxU32                                          subdivisionLevel;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  NxPruningStructure                             staticStructure;
#endif
#if NX_SDK_VERSION_NUMBER >= 262
  NxPruningStructure                             dynamicStructure;
#endif
/**
\brief Hint for how much work should be done per simulation frame to rebuild the pruning structure.
*/
#if NX_SDK_VERSION_NUMBER >= 281
  NxU32                                          dynamicTreeRebuildRateHint;
#endif
/**
\brief Selects a broadphase type.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxBroadPhaseType                               bpType;
#endif
/**
\brief Number of grid cells along one axis for multi grid broadphase. Must be power of two.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU32                                          nbGridCellsX;
#endif
/**
\brief Number of grid cells along one axis for multi grid broadphase. Must be power of two.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU32                                          nbGridCellsY;
#endif
/**
\brief Number of actors required to spawn a separate rigid body solver thread.
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxU32                                          solverBatchSize;
#endif
/**
\brief Collection of materials in the scene
*/
  NxArray< NxMaterialDesc * >                    mMaterials;
/**
\brief Collection of actors in the scene
*/
  NxArray< NxActorDesc *>                        mActors;
/**
\brief collection of joints in the scene
*/
  NxArray< NxJointDesc *>                        mJoints;
/**
\brief Collection of pair flags in the scene
*/
  NxArray< NxPairFlagDesc *>                     mPairFlags;
/**
\brief Collection of effectors in the scene
*/
  NxArray< NxSpringAndDamperEffectorDesc *>      mEffectors;
/**
\brief Collection of force fields in the scene
*/
#if NX_SDK_VERSION_NUMBER >= 270
  NxArray< NxForceFieldDesc *>                   mForceFields;
#endif
/**
\brief Collection of collision groups in the scene
*/
  NxArray< NxCollisionGroupDesc *>               mCollisionGroups;
/**
\brief Collection actor group pair flags
*/
  NxArray< NxActorGroupPair *>                   mActorGroups;
/**
\brief collection of compartments in the scene
*/
#if NX_SDK_VERSION_NUMBER >= 260
  NxArray< NxCompartmentDesc *>                  mCompartments;
#endif
/**
\brief Collection of fluids in the scene
*/
#if NX_USE_FLUID_API
  NxArray< NxFluidDesc *>                        mFluids;
#endif
/**
\brief Collection of cloths in the scene
*/
  NxArray< NxClothDesc *>                        mCloths;
/**
\brief Collection of soft bodies in the scene
*/
#if NX_USE_SOFTBODY_API
  NxArray< NxSoftBodyDesc *>                     mSoftBodies;
#endif
/**
\brief Collection of dominance settings
*/
#if NX_SDK_VERSION_NUMBER >= 272
  NxArray< NxConstraintDominanceDesc * >         mDominanceGroupPairs;
#endif
/**
\brief Force field scale table entries
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxArray< NxForceFieldScaleTableEntry * >       mForceFieldScaleTable;
#endif
/**
\brief Force field shape groups
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxArray< NxForceFieldShapeGroupDesc * >        mForceFieldShapeGroups;
#endif
/**
\brief linear force field kernels
*/
#if NX_SDK_VERSION_NUMBER >= 280
  NxArray< NxForceFieldLinearKernelDesc * >      mForceFieldLinearKernels;
#endif
  void *mInstance;
};

/**
\brief Describes an instantiation of a scene
*/
class NxSceneInstanceDesc
{
public:
  NxSceneInstanceDesc(void);
 ~NxSceneInstanceDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief id of this scene instance
*/
  const char *                                   mId;
/**
\brief optional user properties
*/
  const char *                                   mUserProperties;
/**
\brief name of scene to instance
*/
  const char *                                   mSceneName;
/**
\brief root node transform
*/
  NxMat34                                        mRootNode;
/**
\brief whether or not to ignore planes when instantiating the scene
*/
  NX_BOOL                                        mIgnorePlane;
/**
\brief Indicates the number of child scene instances.
*/
  NxU32                                          mNumSceneInstances;
/**
\brief The list of scene instances to instantiate
*/
  NxArray< NxSceneInstanceDesc *>                mSceneInstances;
  void *mInstance;
 NxSceneInstanceDesc *mParent;
};

/**
\brief Descriptor class for NxPhysicsSDK, primarily used for defining PhysX hardware limits for data shared between scenes.
*/
class NxPhysicsSDKDesc
{
public:
  NxPhysicsSDKDesc(void);
 ~NxPhysicsSDKDesc(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
  void copyFrom(const ::NxPhysicsSDKDesc &desc,CustomCopy &cc);
  void copyTo(::NxPhysicsSDKDesc &desc,CustomCopy &cc);
/**
\brief size of hardware mesh pages. Currently only the value 65536 is supported.
*/
  NxU32                                          hwPageSize;
/**
\brief maximum number of hardware pages supported concurrently on hardware. The valid value must be power of 2.
*/
  NxU32                                          hwPageMax;
/**
\brief maximum number of convex meshes which will be resident on hardware. The valid value must be power of 2.
*/
  NxU32                                          hwConvexMax;
  void *mInstance;
};

/**
\brief The base container class that holds of of the physics data assets
*/
class NxuPhysicsCollection
{
public:
  NxuPhysicsCollection(void);
 ~NxuPhysicsCollection(void);
  void load(SchemaStream &stream,const char *parent=0);
  void store(SchemaStream &stream,const char *parent=0);
/**
\brief Id attribute
*/
  const char *                                   mId;
/**
\brief Custom user properties'
*/
  const char *                                   mUserProperties;
/**
\brief SDK version number saved with
*/
  NxU32                                          mSdkVersion;
/**
\brief Version number of NxuStream saved with
*/
  NxU32                                          mNxuVersion;
/**
\brief The SDK descriptor
*/
  NxPhysicsSDKDesc                               mSDK;
/**
\brief Collection of parameters
*/
  NxArray< NxParameterDesc *>                    mParameters;
/**
\brief Collection of convex meshes
*/
  NxArray< NxConvexMeshDesc *>                   mConvexMeshes;
/**
\brief Collection of triangle meshes
*/
  NxArray< NxTriangleMeshDesc *>                 mTriangleMeshes;
/**
\brief Collection of heightfields
*/
  NxArray< NxHeightFieldDesc *>                  mHeightFields;
/**
\brief Collection of CCD skeletons
*/
  NxArray< NxCCDSkeletonDesc *>                  mSkeletons;
/**
\brief Collection of cloth meshes
*/
  NxArray< NxClothMeshDesc *>                    mClothMeshes;
/**
\brief Collection of soft-body meshes
*/
#if NX_USE_SOFTBODY_API
  NxArray< NxSoftBodyMeshDesc *>                 mSoftBodyMeshes;
#endif
/**
\brief Collection of scenes
*/
  NxArray< NxSceneDesc *>                        mScenes;
/**
\brief Collection of scene instances
*/
  NxArray< NxSceneInstanceDesc *>                mSceneInstances;
  void *mInstance;
  NxSceneDesc *mCurrentScene;
  NxSceneInstanceDesc *mCurrentSceneInstance;
  NxU32 mReferenceCount;
};

} // End namespace NXU
#endif  // end of include guard
