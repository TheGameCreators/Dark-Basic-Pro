#pragma once

#include <DB3Math.h>

DB_EXPORT char *Vector2ToString(char *old, const db3::SVec2 *p);
DB_EXPORT char *Vector3ToString(char *old, const db3::SVec3 *p);
DB_EXPORT char *Vector4ToString(char *old, const db3::SVec4 *p);
DB_EXPORT char *QuatToString(char *old, const db3::SQuat *p);

DB_EXPORT const db3::SVec2 *TempVector2(float x, float y);
DB_EXPORT const db3::SVec3 *TempVector3(float x, float y, float z);
DB_EXPORT const db3::SVec4 *TempVector4(float x, float y, float z, float w);

DB_EXPORT const db3::SVec3 *Vector3Right  ();
DB_EXPORT const db3::SVec3 *Vector3Up     ();
DB_EXPORT const db3::SVec3 *Vector3Forward();

DB_EXPORT db3::SVec2 *NewVector2();
DB_EXPORT db3::SVec3 *NewVector3();
DB_EXPORT db3::SVec4 *NewVector4();
DB_EXPORT void *DeleteVector(void *p);

DB_EXPORT db3::SVec2 *Vector2Copy(db3::SVec2 *dst, const db3::SVec2 *src);
DB_EXPORT db3::SVec3 *Vector3Copy(db3::SVec3 *dst, const db3::SVec3 *src);
DB_EXPORT db3::SVec4 *Vector4Copy(db3::SVec4 *dst, const db3::SVec4 *src);

DB_EXPORT db3::SVec2 *Vector2Duplicate(const db3::SVec2 *src);
DB_EXPORT db3::SVec3 *Vector3Duplicate(const db3::SVec3 *src);
DB_EXPORT db3::SVec4 *Vector4Duplicate(const db3::SVec4 *src);

DB_EXPORT void VectorSetX(db3::SVec2 *dst, float x);
DB_EXPORT void VectorSetY(db3::SVec2 *dst, float y);
DB_EXPORT void VectorSetZ(db3::SVec3 *dst, float z);
DB_EXPORT void VectorSetW(db3::SVec4 *dst, float w);

DB_EXPORT dbReturnFloat_t VectorX(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t VectorY(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t VectorZ(const db3::SVec3 *src);
DB_EXPORT dbReturnFloat_t VectorW(const db3::SVec4 *src);

#define VECTOR_TYPE db3::SVec2 *
#define VECTOR_FUNC(nm) Vector2##nm
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Scale)(VECTOR_TYPE dst, const VECTOR_TYPE a, float scale);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
#undef VECTOR_FUNC
#undef VECTOR_TYPE

#define VECTOR_TYPE db3::SVec3 *
#define VECTOR_FUNC(nm) Vector3##nm
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
#undef VECTOR_FUNC
#undef VECTOR_TYPE

#define VECTOR_TYPE db3::SVec4 *
#define VECTOR_FUNC(nm) Vector4##nm
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Scale)(VECTOR_TYPE dst, const VECTOR_TYPE a, float scale);
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b);
#undef VECTOR_FUNC
#undef VECTOR_TYPE

DB_EXPORT db3::SVec2 *Vector2Lerp(db3::SVec2 *dst, const db3::SVec2 *a, const db3::SVec2 *b, float t);
DB_EXPORT db3::SVec3 *Vector3Lerp(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SVec3 *b, float t);
DB_EXPORT db3::SVec4 *Vector4Lerp(db3::SVec4 *dst, const db3::SVec4 *a, const db3::SVec4 *b, float t);

DB_EXPORT dbReturnFloat_t Vector2Dot(const db3::SVec2 *a, const db3::SVec2 *b);
DB_EXPORT dbReturnFloat_t Vector3Dot(const db3::SVec3 *a, const db3::SVec3 *b);
DB_EXPORT dbReturnFloat_t Vector4Dot(const db3::SVec4 *a, const db3::SVec4 *b);

DB_EXPORT db3::SVec3 *Vector3Cross(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SVec3 *b);
DB_EXPORT db3::SVec4 *Vector4Cross(db3::SVec4 *dst, const db3::SVec4 *a, const db3::SVec4 *b, const db3::SVec4 *c);

DB_EXPORT dbReturnFloat_t Vector2Magnitude(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t Vector3Magnitude(const db3::SVec3 *src);
DB_EXPORT dbReturnFloat_t Vector4Magnitude(const db3::SVec4 *src);

DB_EXPORT dbReturnFloat_t Vector2Length(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t Vector3Length(const db3::SVec3 *src);
DB_EXPORT dbReturnFloat_t Vector4Length(const db3::SVec4 *src);

DB_EXPORT dbReturnFloat_t Vector2LengthFast(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t Vector3LengthFast(const db3::SVec3 *src);
DB_EXPORT dbReturnFloat_t Vector4LengthFast(const db3::SVec4 *src);

DB_EXPORT dbReturnFloat_t Vector2InvLengthFast(const db3::SVec2 *src);
DB_EXPORT dbReturnFloat_t Vector3InvLengthFast(const db3::SVec3 *src);
DB_EXPORT dbReturnFloat_t Vector4InvLengthFast(const db3::SVec4 *src);

DB_EXPORT db3::SVec2 *Vector2Normalize(db3::SVec2 *dst, const db3::SVec2 *src);
DB_EXPORT db3::SVec3 *Vector3Normalize(db3::SVec3 *dst, const db3::SVec3 *src);
DB_EXPORT db3::SVec4 *Vector4Normalize(db3::SVec4 *dst, const db3::SVec4 *src);

DB_EXPORT db3::SVec2 *Vector2NormalizeSelf(db3::SVec2 *dst);
DB_EXPORT db3::SVec3 *Vector3NormalizeSelf(db3::SVec3 *dst);
DB_EXPORT db3::SVec4 *Vector4NormalizeSelf(db3::SVec4 *dst);

DB_EXPORT db3::SVec2 *Vector2NormalizeFast(db3::SVec2 *dst, const db3::SVec2 *src);
DB_EXPORT db3::SVec3 *Vector3NormalizeFast(db3::SVec3 *dst, const db3::SVec3 *src);
DB_EXPORT db3::SVec4 *Vector4NormalizeFast(db3::SVec4 *dst, const db3::SVec4 *src);

DB_EXPORT db3::SVec2 *Vector2NormalizeSelfFast(db3::SVec2 *dst);
DB_EXPORT db3::SVec3 *Vector3NormalizeSelfFast(db3::SVec3 *dst);
DB_EXPORT db3::SVec4 *Vector4NormalizeSelfFast(db3::SVec4 *dst);

DB_EXPORT dbReturnFloat_t Vector2Distance(const db3::SVec2 *a, const db3::SVec2 *b);
DB_EXPORT dbReturnFloat_t Vector3Distance(const db3::SVec3 *a, const db3::SVec3 *b);
DB_EXPORT dbReturnFloat_t Vector4Distance(const db3::SVec4 *a, const db3::SVec4 *b);

DB_EXPORT dbReturnFloat_t Vector2DistanceFast(const db3::SVec2 *a, const db3::SVec2 *b);
DB_EXPORT dbReturnFloat_t Vector3DistanceFast(const db3::SVec3 *a, const db3::SVec3 *b);
DB_EXPORT dbReturnFloat_t Vector4DistanceFast(const db3::SVec4 *a, const db3::SVec4 *b);

DB_EXPORT db3::SVec3 *Vector3Transform4(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SMatrix *b);
DB_EXPORT db3::SVec3 *Vector3Transform3(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SMatrix *b);

DB_EXPORT db3::SVec3 *Vector3EulerLookAt(db3::SVec3 *dst, const db3::SVec3 *origin, const db3::SVec3 *target);
DB_EXPORT db3::SVec3 *Vector3EulerLookAtFast(db3::SVec3 *dst, const db3::SVec3 *origin, const db3::SVec3 *target);

DB_EXPORT db3::SVec3 *Vector3TriangleNormal(db3::SVec3 *dst, const db3::SVec3 *vA, const db3::SVec3 *vB,
	const db3::SVec3 *vC);
DB_EXPORT db3::SVec3 *Vector3TriangleSlide(db3::SVec3 *dst, const db3::SVec3 *src, const db3::SVec3 *norm,
	float speed);

DB_EXPORT db3::SVec3 *Vector3RotateByQuat(db3::SVec3 *dst, const db3::SVec3 *src, const db3::SQuat *rot);


DB_EXPORT db3::SQuat *TempQuat(float w, float x, float y, float z);
DB_EXPORT db3::SQuat *NewQuat();
DB_EXPORT db3::SQuat *QuatCopy(db3::SQuat *dst, const db3::SQuat *src);
DB_EXPORT db3::SQuat *QuatDuplicate(const db3::SQuat *src);

DB_EXPORT db3::SQuat *QuatSet(db3::SQuat *dst, float w, float x, float y, float z);
DB_EXPORT db3::SQuat *QuatSetW(db3::SQuat *dst, float w);
DB_EXPORT db3::SQuat *QuatSetX(db3::SQuat *dst, float w);
DB_EXPORT db3::SQuat *QuatSetY(db3::SQuat *dst, float w);
DB_EXPORT db3::SQuat *QuatSetZ(db3::SQuat *dst, float w);

DB_EXPORT dbReturnFloat_t QuatW(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatX(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatY(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatZ(const db3::SQuat *src);

DB_EXPORT db3::SQuat *QuatLoadIdentity(db3::SQuat *dst);
DB_EXPORT const db3::SQuat *QuatIdentity();

DB_EXPORT db3::SQuat *QuatAdd(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b);
DB_EXPORT db3::SQuat *QuatSub(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b);
DB_EXPORT db3::SQuat *QuatMul(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b);
DB_EXPORT db3::SQuat *QuatScale(db3::SQuat *dst, const db3::SQuat *q, float scale);

DB_EXPORT db3::SQuat *QuatAxis(db3::SQuat *dst, float angle, float x, float y, float z);
DB_EXPORT db3::SQuat *QuatRotationX(db3::SQuat *dst, float angle);
DB_EXPORT db3::SQuat *QuatRotationY(db3::SQuat *dst, float angle);
DB_EXPORT db3::SQuat *QuatRotationZ(db3::SQuat *dst, float angle);

DB_EXPORT db3::SQuat *QuatFromEuler(db3::SQuat *dst, const db3::SVec3 *src);

DB_EXPORT dbReturnFloat_t QuatDot(const db3::SQuat *a, const db3::SQuat *b);

DB_EXPORT db3::SQuat *QuatLerp(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b, float t);
DB_EXPORT db3::SQuat *QuatSlerp(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b, float t);

DB_EXPORT dbReturnFloat_t QuatMagnitude(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatLength(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatLengthFast(const db3::SQuat *src);
DB_EXPORT dbReturnFloat_t QuatInvLengthFast(const db3::SQuat *src);

DB_EXPORT db3::SQuat *QuatNormalize(db3::SQuat *dst, const db3::SQuat *src);
DB_EXPORT db3::SQuat *QuatNormalizeSelf(db3::SQuat *quat);
DB_EXPORT db3::SQuat *QuatNormalizeFast(db3::SQuat *dst, const db3::SQuat *src);
DB_EXPORT db3::SQuat *QuatNormalizeSelfFast(db3::SQuat *quat);

DB_EXPORT db3::SQuat *QuatConjugate(db3::SQuat *dst, const db3::SQuat *src);
DB_EXPORT db3::SQuat *QuatConjugateSelf(db3::SQuat *quat);
DB_EXPORT db3::SQuat *QuatInverse(db3::SQuat *dst, const db3::SQuat *src);
DB_EXPORT db3::SQuat *QuatInverseFast(db3::SQuat *dst, const db3::SQuat *src);
