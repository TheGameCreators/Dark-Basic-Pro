#include "OptimizedMath.h"

#include <stdio.h>
#include <string.h>

#define __alignvec __declspec(align(16))

#if _MSC_VER
# pragma warning(disable:4351) //new behavior: elements of array will be initialized
#endif

template<typename T>
class TCircularPool {
private:
	TCircularPool(const TCircularPool &);// = delete;
	TCircularPool &operator=(const TCircularPool &);// = delete;

protected:
	__alignvec	T			m_pool[MAX_TEMPORARY_VECTORS];
				db3::uint	m_curr;

public:
	TCircularPool(): m_pool(), m_curr(0) {}
	~TCircularPool() {}

	T *Get() {
		T *p;

		p = &m_pool[m_curr++];
		m_curr %= MAX_TEMPORARY_VECTORS;

		return p;
	}
};

static TCircularPool<db3::SVec2> g_vec2Pool;
static TCircularPool<db3::SVec3> g_vec3Pool;
static TCircularPool<db3::SVec4> g_vec4Pool;

#define VECTOR2_OPERATION(dst,a,b,op)\
	dst = dst ? dst : g_vec2Pool.Get();\
	dst->x = a->x op b->x; dst->y = a->y op b->y;\
	return dst
#define VECTOR3_OPERATION(dst,a,b,op)\
	dst = dst ? dst : g_vec3Pool.Get();\
	dst->x = a->x op b->x; dst->y = a->y op b->y; dst->z = a->z op b->z;\
	return dst
#define VECTOR4_OPERATION(dst,a,b,op)\
	dst = dst ? dst : g_vec4Pool.Get();\
	dst->x = a->x op b->x; dst->y = a->y op b->y; dst->z = a->z op b->z; dst->w = a->w op b->w;\
	return dst

__forceinline float ReturnFloatToFloat(dbReturnFloat_t x) {
	return *(float *)&x;
}
__forceinline dbReturnFloat_t Sqrt(dbReturnFloat_t x) {
	return dbReturnFloat(sqrtf(ReturnFloatToFloat(x)));
}
__forceinline dbReturnFloat_t SqrtFast(dbReturnFloat_t x) {
	return dbReturnFloat(db3::SqrtFast(ReturnFloatToFloat(x)));
}
__forceinline dbReturnFloat_t InvSqrtFast(dbReturnFloat_t x) {
	return dbReturnFloat(db3::InvSqrtFast(ReturnFloatToFloat(x)));
}

//
//	String conversion routines
//

DB_EXPORT char *Vector2ToString(char *old, const db3::SVec2 *p) {
	char buf[32];

	sprintf_s(buf, "(%g, %g)", p->x, p->y);

	return dbReturnString(old, buf);
}
DB_EXPORT char *Vector3ToString(char *old, const db3::SVec3 *p) {
	char buf[64];

	sprintf_s(buf, "(%g, %g, %g)", p->x, p->y, p->z);

	return dbReturnString(old, buf);
}
DB_EXPORT char *Vector4ToString(char *old, const db3::SVec4 *p) {
	char buf[128];

	sprintf_s(buf, "(%g, %g, %g, %g)", p->x, p->y, p->z, p->w);

	return dbReturnString(old, buf);
}
DB_EXPORT char *QuatToString(char *old, const db3::SQuat *p) {
	char buf[128];

	sprintf_s(buf, "(%g; %g, %g, %g)", p->w, p->x, p->y, p->z);

	return dbReturnString(old, buf);
}

//
//	Temporary Vectors
//

DB_EXPORT const db3::SVec2 *TempVector2(float x, float y) {
	db3::SVec2 *p;

	p = g_vec2Pool.Get();

	p->x = x;
	p->y = y;

	return p;
}
DB_EXPORT const db3::SVec3 *TempVector3(float x, float y, float z) {
	db3::SVec3 *p;

	p = g_vec3Pool.Get();

	p->x = x;
	p->y = y;
	p->z = z;

	return p;
}
DB_EXPORT const db3::SVec4 *TempVector4(float x, float y, float z, float w) {
	db3::SVec4 *p;

	p = g_vec4Pool.Get();

	p->x = x;
	p->y = y;
	p->z = z;
	p->w = w;

	return p;
}

//
//	Permanent Vectors
//

DB_EXPORT const db3::SVec3 *Vector3Right  () { static db3::SVec3 v(1,0,0); return &v; }
DB_EXPORT const db3::SVec3 *Vector3Up     () { static db3::SVec3 v(0,1,0); return &v; }
DB_EXPORT const db3::SVec3 *Vector3Forward() { static db3::SVec3 v(0,0,1); return &v; }

//
//	Vector Allocation
//

DB_EXPORT db3::SVec2 *NewVector2() {
	db3::SVec2 *vec;

	vec = reinterpret_cast<db3::SVec2 *>(malloc(sizeof(*vec)));
	if (!vec)
		return nullptr;

	vec->x = 0;
	vec->y = 0;

	return vec;
}
DB_EXPORT db3::SVec3 *NewVector3() {
	db3::SVec3 *vec;

	vec = reinterpret_cast<db3::SVec3 *>(malloc(sizeof(*vec)));
	if (!vec)
		return nullptr;

	vec->x = 0;
	vec->y = 0;
	vec->z = 0;

	return vec;
}
DB_EXPORT db3::SVec4 *NewVector4() {
	db3::SVec4 *vec;

	vec = reinterpret_cast<db3::SVec4 *>(malloc(sizeof(*vec)));
	if (!vec)
		return nullptr;

	vec->x = 0;
	vec->y = 0;
	vec->z = 0;
	vec->w = 0;

	return vec;
}
DB_EXPORT void *DeleteVector(void *p) {
	if (!p)
		return nullptr;

	free(p);
	return nullptr;
}

//
//	Vector Management
//
DB_EXPORT db3::SVec2 *Vector2Copy(db3::SVec2 *dst, const db3::SVec2 *src) {
	dst = dst ? dst : g_vec2Pool.Get();

	dst->x = src->x;
	dst->y = src->y;

	return dst;
}
DB_EXPORT db3::SVec3 *Vector3Copy(db3::SVec3 *dst, const db3::SVec3 *src) {
	dst = dst ? dst : g_vec3Pool.Get();

	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return dst;
}
DB_EXPORT db3::SVec4 *Vector4Copy(db3::SVec4 *dst, const db3::SVec4 *src) {
	dst = dst ? dst : g_vec4Pool.Get();

	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->w = src->w;

	return dst;
}

DB_EXPORT db3::SVec2 *Vector2Duplicate(const db3::SVec2 *src) {
	db3::SVec2 *dst;

	dst = NewVector2();
	if (!dst)
		return nullptr;

	return Vector2Copy(dst, src);
}
DB_EXPORT db3::SVec3 *Vector3Duplicate(const db3::SVec3 *src) {
	db3::SVec3 *dst;

	dst = NewVector3();
	if (!dst)
		return nullptr;

	return Vector3Copy(dst, src);
}
DB_EXPORT db3::SVec4 *Vector4Duplicate(const db3::SVec4 *src) {
	db3::SVec4 *dst;

	dst = NewVector4();
	if (!dst)
		return nullptr;

	return Vector4Copy(dst, src);
}

DB_EXPORT void VectorSetX(db3::SVec2 *dst, float x) {
	dst->x = x;
}
DB_EXPORT void VectorSetY(db3::SVec2 *dst, float y) {
	dst->y = y;
}
DB_EXPORT void VectorSetZ(db3::SVec3 *dst, float z) {
	dst->z = z;
}
DB_EXPORT void VectorSetW(db3::SVec4 *dst, float w) {
	dst->w = w;
}

//
//	Vector Information
//

DB_EXPORT dbReturnFloat_t VectorX(const db3::SVec2 *src) {
	return dbReturnFloat(src->x);
}
DB_EXPORT dbReturnFloat_t VectorY(const db3::SVec2 *src) {
	return dbReturnFloat(src->y);
}
DB_EXPORT dbReturnFloat_t VectorZ(const db3::SVec3 *src) {
	return dbReturnFloat(src->z);
}
DB_EXPORT dbReturnFloat_t VectorW(const db3::SVec4 *src) {
	return dbReturnFloat(src->w);
}

//
//	Vector2 Arithmetic
//

#define VECTOR_TYPE db3::SVec2 *
#define VECTOR_FUNC(nm) Vector2##nm
#define VECTOR_OPERATION VECTOR2_OPERATION
#define TEMP_VECTOR(scale) TempVector2(scale,scale)
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, +);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, -);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Scale)(VECTOR_TYPE dst, const VECTOR_TYPE a, float scale) {
	const VECTOR_TYPE b; b = TEMP_VECTOR(scale);
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, /);
}
#undef TEMP_VECTOR
#undef VECTOR_OPERATION
#undef VECTOR_FUNC
#undef VECTOR_TYPE

//
//	Vector 3 Arithmetic
//
#define VECTOR_TYPE db3::SVec3 *
#define VECTOR_FUNC(nm) Vector3##nm
#define VECTOR_OPERATION VECTOR3_OPERATION
#define TEMP_VECTOR(scale) TempVector3(scale,scale,scale)
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, +);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, -);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Scale)(VECTOR_TYPE dst, const VECTOR_TYPE a, float scale) {
	const VECTOR_TYPE b; b = TEMP_VECTOR(scale);
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, /);
}
#undef TEMP_VECTOR
#undef VECTOR_OPERATION
#undef VECTOR_FUNC
#undef VECTOR_TYPE

//
//	Vector 4 Arithmetic
//
#define VECTOR_TYPE db3::SVec4 *
#define VECTOR_FUNC(nm) Vector4##nm
#define VECTOR_OPERATION VECTOR4_OPERATION
#define TEMP_VECTOR(scale) TempVector4(scale,scale,scale,scale)
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Add)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, +);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Sub)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, -);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Mul)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Scale)(VECTOR_TYPE dst, const VECTOR_TYPE a, float scale) {
	const VECTOR_TYPE b; b = TEMP_VECTOR(scale);
	VECTOR_OPERATION(dst, a, b, *);
}
DB_EXPORT VECTOR_TYPE VECTOR_FUNC(Div)(VECTOR_TYPE dst, const VECTOR_TYPE a, const VECTOR_TYPE b) {
	VECTOR_OPERATION(dst, a, b, /);
}
#undef TEMP_VECTOR
#undef VECTOR_OPERATION
#undef VECTOR_FUNC
#undef VECTOR_TYPE

//
//	Linear Interpolation
//
DB_EXPORT db3::SVec2 *Vector2Lerp(db3::SVec2 *dst, const db3::SVec2 *a, const db3::SVec2 *b, float t) {
	dst = dst ? dst : dst = g_vec2Pool.Get();

	dst->x = a->x + (b->x - a->x)*t;
	dst->y = a->y + (b->y - a->y)*t;

	return dst;
}
DB_EXPORT db3::SVec3 *Vector3Lerp(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SVec3 *b, float t) {
	dst = dst ? dst : dst = g_vec3Pool.Get();

	dst->x = a->x + (b->x - a->x)*t;
	dst->y = a->y + (b->y - a->y)*t;
	dst->z = a->z + (b->z - a->z)*t;

	return dst;
}
DB_EXPORT db3::SVec4 *Vector4Lerp(db3::SVec4 *dst, const db3::SVec4 *a, const db3::SVec4 *b, float t) {
	dst = dst ? dst : dst = g_vec4Pool.Get();

	dst->x = a->x + (b->x - a->x)*t;
	dst->y = a->y + (b->y - a->y)*t;
	dst->z = a->z + (b->z - a->z)*t;
	dst->w = a->w + (b->w - a->w)*t;

	return dst;
}

//
//	Dot Product
//
DB_EXPORT dbReturnFloat_t Vector2Dot(const db3::SVec2 *a, const db3::SVec2 *b) {
	return dbReturnFloat(a->x*b->x + a->y*b->y);
}
DB_EXPORT dbReturnFloat_t Vector3Dot(const db3::SVec3 *a, const db3::SVec3 *b) {
	return dbReturnFloat(a->x*b->x + a->y*b->y + a->z*b->z);
}
DB_EXPORT dbReturnFloat_t Vector4Dot(const db3::SVec4 *a, const db3::SVec4 *b) {
	return dbReturnFloat(a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w);
}

//
//	Cross Product (Vector Product)
//
DB_EXPORT db3::SVec3 *Vector3Cross(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SVec3 *b) {
	dst = dst ? dst : dst = g_vec3Pool.Get();

	dst->x = a->y*b->z - a->z*b->y;
	dst->y = a->z*b->x - a->x*b->z;
	dst->z = a->x*b->y - a->y*b->x;

	return dst;
}
DB_EXPORT db3::SVec4 *Vector4Cross(db3::SVec4 *dst, const db3::SVec4 *a, const db3::SVec4 *b, const db3::SVec4 *c) {
	dst = dst ? dst : dst = g_vec4Pool.Get();

#define x1 a->x
#define y1 a->y
#define z1 a->z
#define w1 a->w
#define x2 b->x
#define y2 b->y
#define z2 b->z
#define w2 b->w
#define x3 c->x
#define y3 c->y
#define z3 c->z
#define w3 c->w
	dst->x =   y1*(z2*w3 - z3*w2) - z1*(y2*w3 - y3*w2) + w1*(y2*z3 - z2*y3);
	dst->y = -(x1*(z2*w3 - z3*w2) - z1*(x2*w3 - x3*w2) + w1*(x2*z3 - x3*z2));
	dst->z =   x1*(y2*w3 - y3*w2) - y1*(x2*w3 - x3*w2) + w1*(x2*y3 - x3*y2);
	dst->w = -(x1*(y2*z3 - y3*z2) - y1*(x2*z3 - x3*z2) + z1*(x2*y3 - x3*y2));
#undef w3
#undef z3
#undef y3
#undef x3
#undef w2
#undef z2
#undef y2
#undef x2
#undef w1
#undef z1
#undef y1
#undef x1

	return dst;
}

//
//	Magnitude (Length Squared)
//
DB_EXPORT dbReturnFloat_t Vector2Magnitude(const db3::SVec2 *src) {
	return Vector2Dot(src, src);
}
DB_EXPORT dbReturnFloat_t Vector3Magnitude(const db3::SVec3 *src) {
	return Vector3Dot(src, src);
}
DB_EXPORT dbReturnFloat_t Vector4Magnitude(const db3::SVec4 *src) {
	return Vector4Dot(src, src);
}

//
//	Length
//
DB_EXPORT dbReturnFloat_t Vector2Length(const db3::SVec2 *src) {
	return Sqrt(Vector2Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector3Length(const db3::SVec3 *src) {
	return Sqrt(Vector3Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector4Length(const db3::SVec4 *src) {
	return Sqrt(Vector4Magnitude(src));
}

//
//	Fast Length
//
DB_EXPORT dbReturnFloat_t Vector2LengthFast(const db3::SVec2 *src) {
	return SqrtFast(Vector2Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector3LengthFast(const db3::SVec3 *src) {
	return SqrtFast(Vector3Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector4LengthFast(const db3::SVec4 *src) {
	return SqrtFast(Vector4Magnitude(src));
}

//
//	Fast Inverse Length
//
DB_EXPORT dbReturnFloat_t Vector2InvLengthFast(const db3::SVec2 *src) {
	return InvSqrtFast(Vector2Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector3InvLengthFast(const db3::SVec3 *src) {
	return InvSqrtFast(Vector3Magnitude(src));
}
DB_EXPORT dbReturnFloat_t Vector4InvLengthFast(const db3::SVec4 *src) {
	return InvSqrtFast(Vector4Magnitude(src));
}

//
//	Vector Normalization
//
DB_EXPORT db3::SVec2 *Vector2Normalize(db3::SVec2 *dst, const db3::SVec2 *src) {
	return Vector2Scale(dst, src, 1.0f/Vector2Length(src));
}
DB_EXPORT db3::SVec3 *Vector3Normalize(db3::SVec3 *dst, const db3::SVec3 *src) {
	return Vector3Scale(dst, src, 1.0f/Vector3Length(src));
}
DB_EXPORT db3::SVec4 *Vector4Normalize(db3::SVec4 *dst, const db3::SVec4 *src) {
	return Vector4Scale(dst, src, 1.0f/Vector4Length(src));
}

//
//	Vector Self Normalization
//
DB_EXPORT db3::SVec2 *Vector2NormalizeSelf(db3::SVec2 *dst) {
	return Vector2Scale(dst, dst, 1.0f/Vector2Length(dst));
}
DB_EXPORT db3::SVec3 *Vector3NormalizeSelf(db3::SVec3 *dst) {
	return Vector3Scale(dst, dst, 1.0f/Vector3Length(dst));
}
DB_EXPORT db3::SVec4 *Vector4NormalizeSelf(db3::SVec4 *dst) {
	return Vector4Scale(dst, dst, 1.0f/Vector4Length(dst));
}

//
//	Fast Vector Normalization
//
DB_EXPORT db3::SVec2 *Vector2NormalizeFast(db3::SVec2 *dst, const db3::SVec2 *src) {
	return Vector2Scale(dst, src, 1.0f/Vector2Length(src));
}
DB_EXPORT db3::SVec3 *Vector3NormalizeFast(db3::SVec3 *dst, const db3::SVec3 *src) {
	return Vector3Scale(dst, src, 1.0f/Vector3Length(src));
}
DB_EXPORT db3::SVec4 *Vector4NormalizeFast(db3::SVec4 *dst, const db3::SVec4 *src) {
	return Vector4Scale(dst, src, 1.0f/Vector4Length(src));
}

//
//	Fast Vector Self Normalization
//
DB_EXPORT db3::SVec2 *Vector2NormalizeSelfFast(db3::SVec2 *dst) {
	return Vector2Scale(dst, dst, ReturnFloatToFloat(Vector2InvLengthFast(dst)));
}
DB_EXPORT db3::SVec3 *Vector3NormalizeSelfFast(db3::SVec3 *dst) {
	return Vector3Scale(dst, dst, ReturnFloatToFloat(Vector3InvLengthFast(dst)));
}
DB_EXPORT db3::SVec4 *Vector4NormalizeSelfFast(db3::SVec4 *dst) {
	return Vector4Scale(dst, dst, ReturnFloatToFloat(Vector4InvLengthFast(dst)));
}

//
//	Vector Distance
//
DB_EXPORT dbReturnFloat_t Vector2Distance(const db3::SVec2 *a, const db3::SVec2 *b) {
	return Vector2Length(Vector2Sub(nullptr, a, b));
}
DB_EXPORT dbReturnFloat_t Vector3Distance(const db3::SVec3 *a, const db3::SVec3 *b) {
	return Vector3Length(Vector3Sub(nullptr, a, b));
}
DB_EXPORT dbReturnFloat_t Vector4Distance(const db3::SVec4 *a, const db3::SVec4 *b) {
	return Vector4Length(Vector4Sub(nullptr, a, b));
}

//
//	Fast Vector Distance
//
DB_EXPORT dbReturnFloat_t Vector2DistanceFast(const db3::SVec2 *a, const db3::SVec2 *b) {
	return Vector2LengthFast(Vector2Sub(nullptr, a, b));
}
DB_EXPORT dbReturnFloat_t Vector3DistanceFast(const db3::SVec3 *a, const db3::SVec3 *b) {
	return Vector3LengthFast(Vector3Sub(nullptr, a, b));
}
DB_EXPORT dbReturnFloat_t Vector4DistanceFast(const db3::SVec4 *a, const db3::SVec4 *b) {
	return Vector4LengthFast(Vector4Sub(nullptr, a, b));
}

//
//	Vector Transformation
//
DB_EXPORT db3::SVec3 *Vector3Transform4(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SMatrix *b) {
	float x, y, z;

	dst = dst ? dst : g_vec3Pool.Get();

	x = a->x*b->m11 + a->y*b->m12 + a->z*b->m13 + b->m14;
	y = a->x*b->m21 + a->y*b->m22 + a->z*b->m23 + b->m24;
	z = a->x*b->m31 + a->y*b->m32 + a->z*b->m33 + b->m34;

	dst->x = x;
	dst->y = y;
	dst->z = z;

	return dst;
}
DB_EXPORT db3::SVec3 *Vector3Transform3(db3::SVec3 *dst, const db3::SVec3 *a, const db3::SMatrix *b) {
	float x, y, z;

	dst = dst ? dst : g_vec3Pool.Get();

	x = a->x*b->m11 + a->y*b->m12 + a->z*b->m13;
	y = a->x*b->m21 + a->y*b->m22 + a->z*b->m23;
	z = a->x*b->m31 + a->y*b->m32 + a->z*b->m33;

	dst->x = x;
	dst->y = y;
	dst->z = z;

	return dst;
}

//
//	Vector Look At
//	Create a Euler rotation vector based on a source point and a target point
//
DB_EXPORT db3::SVec3 *Vector3EulerLookAt(db3::SVec3 *dst, const db3::SVec3 *origin, const db3::SVec3 *target) {
	db3::SVec3 diff, vec;
	float x, y, s, c;

	Vector3Sub(&diff, target, origin);
	vec.x=diff.x; vec.y=0.0f; vec.z=diff.z;

	if (ReturnFloatToFloat(Vector3Length(&vec)) > 0.001f) {
		y = db3::ACos(db3::ClampSNorm(Vector3NormalizeSelf(&vec)->z));
		y = db3::NegateIfSigned(y, vec.x);
	} else
		y = 0.0f;

	s = db3::Sin(-y);
	c = db3::Cos(-y);

	vec.x = diff.x*c  + diff.z*s;
	vec.y = diff.y;
	vec.z = diff.x*-s + diff.z*c;

	if (ReturnFloatToFloat(Vector3Length(&vec)) > 0.001f) {
		x = db3::ACos(db3::ClampSNorm(Vector3NormalizeSelf(&vec)->z));
		x = db3::NegateIfUnsigned(x, vec.y - 0.001f);
	} else
		x = 0.0f;

	dst = dst ? dst : g_vec3Pool.Get();

	dst->x = x;
	dst->y = y;
	dst->z = 0.0f;

	return dst;
}
DB_EXPORT db3::SVec3 *Vector3EulerLookAtFast(db3::SVec3 *dst, const db3::SVec3 *origin, const db3::SVec3 *target) {
	db3::SVec3 diff, vec;
	float x, y, s, c;

	Vector3Sub(&diff, target, origin);
	vec.x=diff.x; vec.y=0.0f; vec.z=diff.z;

	if (ReturnFloatToFloat(Vector3LengthFast(&vec)) > 0.001f) {
		y = db3::ACos(db3::ClampSNorm(Vector3NormalizeSelfFast(&vec)->z));
		y = db3::NegateIfSigned(y, vec.x);
	} else
		y = 0.0f;

	s = db3::Sin(-y);
	c = db3::Cos(-y);

	vec.x = diff.x*c  + diff.z*s;
	vec.y = diff.y;
	vec.z = diff.x*-s + diff.z*c;

	if (ReturnFloatToFloat(Vector3LengthFast(&vec)) > 0.001f) {
		x = db3::ACos(db3::ClampSNorm(Vector3NormalizeSelfFast(&vec)->z));
		x = db3::NegateIfUnsigned(x, vec.y - 0.001f);
	} else
		x = 0.0f;

	dst = dst ? dst : g_vec3Pool.Get();

	dst->x = x;
	dst->y = y;
	dst->z = 0.0f;

	return dst;
}

//
//	Vector3 Triangle Fun
//
DB_EXPORT db3::SVec3 *Vector3TriangleNormal(db3::SVec3 *dst, const db3::SVec3 *vA, const db3::SVec3 *vB,
const db3::SVec3 *vC) {
	db3::SVec3 a, b;

	return Vector3NormalizeSelf(Vector3Cross(dst, Vector3Sub(&a, vB, vA), Vector3Sub(&b, vC, vA)));
}
DB_EXPORT db3::SVec3 *Vector3TriangleSlide(db3::SVec3 *dst, const db3::SVec3 *src, const db3::SVec3 *norm,
float speed) {
	db3::SVec3 objDir, slideDir;
	float d;

	d = ReturnFloatToFloat(Vector3Dot(norm, Vector3Normalize(&objDir, src)));
	return Vector3Scale(dst, Vector3NormalizeSelf(Vector3Cross(&slideDir, norm, Vector3Up())), speed*d);
}

//
//	Vector3 Rotate a point by a quaternion
//
DB_EXPORT db3::SVec3 *Vector3RotateByQuat(db3::SVec3 *dst, const db3::SVec3 *src, const db3::SQuat *rot) {
	db3::SQuat p, i, t;

	p.w = 0;
	p.x = src->x;
	p.y = src->y;
	p.z = src->z;

	QuatMul(&p, rot, QuatMul(&t, QuatInverse(&i, rot), &p));

	dst = dst ? dst : g_vec3Pool.Get();

	dst->x = p.x;
	dst->y = p.y;
	dst->z = p.z;

	return dst;
}

//----------------------------------------------------------------------------------------------------------------------

static TCircularPool<db3::SQuat> g_quatPool;

DB_EXPORT db3::SQuat *TempQuat(float w, float x, float y, float z) {
	db3::SQuat *q;

	q = g_quatPool.Get();

	q->w = w;
	q->x = x;
	q->y = y;
	q->z = z;

	return q;
}
DB_EXPORT db3::SQuat *NewQuat() {
	db3::SQuat *q;

	q = (db3::SQuat *)malloc(sizeof(*q));
	if (!q)
		return nullptr;

	q->w = 1;
	q->x = 0;
	q->y = 0;
	q->z = 0;

	return q;
}
DB_EXPORT db3::SQuat *QuatCopy(db3::SQuat *dst, const db3::SQuat *src) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = src->w;
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatDuplicate(const db3::SQuat *src) {
	db3::SQuat *q;

	q = NewQuat();
	if (!q)
		return nullptr;

	return QuatCopy(q, src);
}

DB_EXPORT db3::SQuat *QuatSet(db3::SQuat *dst, float w, float x, float y, float z) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = w;
	dst->x = x;
	dst->y = y;
	dst->z = z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatSetW(db3::SQuat *dst, float w) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = w;

	return dst;
}
DB_EXPORT db3::SQuat *QuatSetX(db3::SQuat *dst, float x) {
	dst = dst ? dst : g_quatPool.Get();

	dst->x = x;

	return dst;
}
DB_EXPORT db3::SQuat *QuatSetY(db3::SQuat *dst, float y) {
	dst = dst ? dst : g_quatPool.Get();

	dst->y = y;

	return dst;
}
DB_EXPORT db3::SQuat *QuatSetZ(db3::SQuat *dst, float z) {
	dst = dst ? dst : g_quatPool.Get();

	dst->z = z;

	return dst;
}

DB_EXPORT dbReturnFloat_t QuatW(const db3::SQuat *src) {
	return dbReturnFloat(src->w);
}
DB_EXPORT dbReturnFloat_t QuatX(const db3::SQuat *src) {
	return dbReturnFloat(src->x);
}
DB_EXPORT dbReturnFloat_t QuatY(const db3::SQuat *src) {
	return dbReturnFloat(src->y);
}
DB_EXPORT dbReturnFloat_t QuatZ(const db3::SQuat *src) {
	return dbReturnFloat(src->z);
}

DB_EXPORT db3::SQuat *QuatLoadIdentity(db3::SQuat *dst) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = 1;
	dst->x = 0;
	dst->y = 0;
	dst->z = 0;

	return dst;
}
DB_EXPORT const db3::SQuat *QuatIdentity() { static const db3::SQuat q = { 1,0,0,0 }; return &q; }

DB_EXPORT db3::SQuat *QuatAdd(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = a->w + b->w;
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatSub(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = a->w - b->w;
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
	dst->z = a->z - b->z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatMul(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b) {
	float w,x,y,z;

	dst = dst ? dst : g_quatPool.Get();

	w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
	x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;
	y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
	z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;

	dst->w = w;
	dst->x = x;
	dst->y = y;
	dst->z = z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatScale(db3::SQuat *dst, const db3::SQuat *q, float scale) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = q->w*scale;
	dst->x = q->x*scale;
	dst->y = q->y*scale;
	dst->z = q->z*scale;

	return dst;
}

DB_EXPORT db3::SQuat *QuatAxis(db3::SQuat *dst, float angle, float x, float y, float z) {
	float c, s, t;

	dst = dst ? dst : g_quatPool.Get();

	t = 0.5f*angle;
	c = db3::Cos(t);
	s = db3::Sin(t);

	dst->w = c;
	dst->x = s*x;
	dst->y = s*y;
	dst->z = s*z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatRotationX(db3::SQuat *dst, float angle) {
	return QuatAxis(dst, angle, 1, 0, 0);
}
DB_EXPORT db3::SQuat *QuatRotationY(db3::SQuat *dst, float angle) {
	return QuatAxis(dst, angle, 0, 1, 0);
}
DB_EXPORT db3::SQuat *QuatRotationZ(db3::SQuat *dst, float angle) {
	return QuatAxis(dst, angle, 0, 0, 1);
}

// euler angles (x=pitch, y=yaw, z=roll)
DB_EXPORT db3::SQuat *QuatFromEuler(db3::SQuat *dst, const db3::SVec3 *src) {
	db3::SQuat qx, qy, qz, qt;

	dst = dst ? dst : g_quatPool.Get();

	QuatRotationX(&qx, src->x);
	QuatRotationY(&qy, src->y);
	QuatRotationZ(&qz, src->z);

	QuatMul(&qt, &qz, &qx);
	QuatMul(dst, &qt, &qy);

	QuatNormalizeSelf(dst);

	return dst;
}

DB_EXPORT dbReturnFloat_t QuatDot(const db3::SQuat *a, const db3::SQuat *b) {
	return dbReturnFloat(a->w*b->w + a->x*b->x + a->y*b->y + a->z*b->z);
}

DB_EXPORT db3::SQuat *QuatLerp(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b, float t) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w = a->w + (b->w - a->w)*t;
	dst->x = a->x + (b->x - a->x)*t;
	dst->y = a->y + (b->y - a->y)*t;
	dst->z = a->z + (b->z - a->z)*t;

	return QuatNormalizeSelf(dst);
}
DB_EXPORT db3::SQuat *QuatSlerp(db3::SQuat *dst, const db3::SQuat *a, const db3::SQuat *b, float t) {
	db3::SQuat q;
	float d, theta, c,s;

	d = ReturnFloatToFloat(QuatDot(a, b));
	if (d > 0.999f)
		return QuatLerp(dst, a, b, t);

	d = db3::ClampSNorm(d);
	theta = db3::ACos(d)*t;

	q.w = b->w - a->w*d;
	q.x = b->x - a->x*d;
	q.y = b->y - a->y*d;
	q.z = b->z - a->z*d;
	QuatNormalizeSelf(&q);

	dst = dst ? dst : g_quatPool.Get();

	c = db3::Cos(theta);
	s = db3::Sin(theta);

	dst->w = a->w*c + q.w*s;
	dst->x = a->x*c + q.x*s;
	dst->y = a->y*c + q.y*s;
	dst->z = a->z*c + q.z*s;

	return dst;
}

DB_EXPORT dbReturnFloat_t QuatMagnitude(const db3::SQuat *src) {
	return QuatDot(src, src);
}
DB_EXPORT dbReturnFloat_t QuatLength(const db3::SQuat *src) {
	return Sqrt(QuatMagnitude(src));
}
DB_EXPORT dbReturnFloat_t QuatLengthFast(const db3::SQuat *src) {
	return SqrtFast(QuatMagnitude(src));
}
DB_EXPORT dbReturnFloat_t QuatInvLengthFast(const db3::SQuat *src) {
	return InvSqrtFast(QuatMagnitude(src));
}

DB_EXPORT db3::SQuat *QuatNormalize(db3::SQuat *dst, const db3::SQuat *src) {
	float r;

	dst = dst ? dst : g_quatPool.Get();

	r = 1.0f/ReturnFloatToFloat(QuatLength(src));

	dst->w = src->w*r;
	dst->x = src->x*r;
	dst->y = src->y*r;
	dst->z = src->z*r;

	return dst;
}
DB_EXPORT db3::SQuat *QuatNormalizeSelf(db3::SQuat *quat) {
	if (!quat)
		return nullptr;

	return QuatNormalize(quat, quat);
}
DB_EXPORT db3::SQuat *QuatNormalizeFast(db3::SQuat *dst, const db3::SQuat *src) {
	float r;

	dst = dst ? dst : g_quatPool.Get();

	r = ReturnFloatToFloat(QuatInvLengthFast(src));

	dst->w = src->w*r;
	dst->x = src->x*r;
	dst->y = src->y*r;
	dst->z = src->z*r;

	return dst;
}
DB_EXPORT db3::SQuat *QuatNormalizeSelfFast(db3::SQuat *quat) {
	if (!quat)
		return nullptr;

	return QuatNormalizeFast(quat, quat);
}

DB_EXPORT db3::SQuat *QuatConjugate(db3::SQuat *dst, const db3::SQuat *src) {
	dst = dst ? dst : g_quatPool.Get();

	dst->w =  src->w;
	dst->x = -src->x;
	dst->y = -src->y;
	dst->z = -src->z;

	return dst;
}
DB_EXPORT db3::SQuat *QuatConjugateSelf(db3::SQuat *quat) {
	if (!quat)
		return nullptr;

	quat->x = -quat->x;
	quat->y = -quat->y;
	quat->z = -quat->z;

	return quat;
}
DB_EXPORT db3::SQuat *QuatInverse(db3::SQuat *dst, const db3::SQuat *src) {
	dst = dst ? dst : g_quatPool.Get();

	QuatConjugate(dst, src);
	QuatNormalizeSelf(dst);

	return dst;
}
DB_EXPORT db3::SQuat *QuatInverseFast(db3::SQuat *dst, const db3::SQuat *src) {
	dst = dst ? dst : g_quatPool.Get();

	QuatConjugate(dst, src);
	QuatNormalizeSelfFast(dst);

	return dst;
}
