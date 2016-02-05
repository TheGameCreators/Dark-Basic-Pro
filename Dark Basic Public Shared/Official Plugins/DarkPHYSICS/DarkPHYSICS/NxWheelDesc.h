#ifndef NX_WHEEL_DESC
#define NX_WHEEL_DESC

#include <NxVec3.h>

enum NxWheelFlags {
	NX_WF_STEERABLE_INPUT		= (1 << 0),
	NX_WF_STEERABLE_AUTO		= (1 << 1),
	NX_WF_AFFECTED_BY_HANDBRAKE	= (1 << 2),
	NX_WF_ACCELERATED			= (1 << 3),
	
	NX_WF_BUILD_LOWER_HALF		= (1 << 8),
	NX_WF_USE_WHEELSHAPE		= (1 << 9),

	NX_WF_ALL_WHEEL_FLAGS		= NX_WF_STEERABLE_INPUT
								| NX_WF_STEERABLE_AUTO
								| NX_WF_AFFECTED_BY_HANDBRAKE
								| NX_WF_ACCELERATED,

};

class NxWheelDesc {
public:
	NxVec3					position;
	//NxVec3					wheelAxis;
	//NxVec3					downAxis;

	NxReal					wheelRadius;
	NxReal					wheelWidth;

	NxReal					wheelSuspension;

	NxReal					springRestitution;
	NxReal					springDamping;
	NxReal					springBias;

	NxReal					maxBrakeForce;
	NxReal					frictionToSide;
	NxReal					frictionToFront;

	NxU32					wheelApproximation;

	NxU32					wheelFlags;

	

	void*					userData;

	NX_INLINE NxWheelDesc();
	NX_INLINE void setToDefault();
	NX_INLINE bool isValid() const;

};

NX_INLINE NxWheelDesc::NxWheelDesc()	//constructor sets to default
{
	setToDefault();
}

NX_INLINE void NxWheelDesc::setToDefault()
{
	position.set(0,0,0);
	//wheelAxis.set(0,0,1);
	//downAxis.set(0,-1,0);
	userData = NULL;
	wheelFlags = 0;
	wheelRadius = 1;
	wheelWidth = 0.1f;

	springBias = 0;
	springRestitution = 1.f;
	springDamping = 0.f;

	wheelSuspension = 1.f;
	maxBrakeForce = 0.f;
	frictionToSide = 1.0f;
	frictionToFront = 1.0f;
}

NX_INLINE bool NxWheelDesc::isValid() const
{
	//if (NxMath::abs(1-wheelAxis.magnitudeSquared()) > 0.001f)
	//	return false;
	if (wheelApproximation > 0 && wheelApproximation < 4) {
		fprintf(stderr, "wheelApproximation must be either 0 or bigger than 3\n");
		return false;
	}
	if ((wheelFlags & NX_WF_STEERABLE_AUTO) && (wheelFlags & NX_WF_STEERABLE_INPUT)) {
		fprintf(stderr, "NX_WF_STEERABLE_AUTO and NX_WF_STEERABLE_INPUT not permitted at the same time\n");
		return false;
	}
	return true;
}

#endif
