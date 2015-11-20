#pragma once

#include "DB3.h"
#include "../Core/globstruct.h"

#include <math.h>

//comment this out if you want fast math
#define DB3_PRECISE 1

DB_ENTER_NS()

#define MAX_TEMPORARY_VECTORS 128

#define PI (3.1415926535897932384626433832795028841971693993751058209)
#define TAU ((PI)*2.0)

#undef LB
#define LB(x) (sizeof(x)*8 - 1)

//infinity
// Thank you LackOfGrace:
// http://www.gamedev.net/topic/465682-visual-studio-2005-and-infinity/
#ifndef INFINITY //this is defined in glibc, but not in MSVC :(
union MSVC_EVIL_FLOAT_HACK
{
   u8  Bytes[4];
   f32 Value;
};
static union MSVC_EVIL_FLOAT_HACK INFINITY_HACK = {{0x00, 0x00, 0x80, 0x7F}};
# define INFINITY (db3::INFINITY_HACK.Value)
#endif

//----------------------------------------------------------------------------------------------------------------------

struct SMatrix {
	f32	m11, m21, m31, m41,
		m12, m22, m32, m42,
		m13, m23, m33, m43,
		m14, m24, m34, m44;
};

struct SVec2 {
	f32	x, y;

	inline SVec2(f32 x=0.0f, f32 y=0.0f): x(x), y(y) {}
};
struct SVec3: public SVec2 {
	f32	z;

	inline SVec3(f32 x=0.0f, f32 y=0.0f, f32 z=0.0f): SVec2(x, y), z(z) {}
};
struct SVec4: public SVec3 {
	f32	w;

	inline SVec4(f32 x=0.0f, f32 y=0.0f, f32 z=0.0f, f32 w=0.0f): SVec3(x, y, z), w(w) {}
};

struct SQuat {
	f32	w;
	f32	x, y, z;
};

//----------------------------------------------------------------------------------------------------------------------

class CMathTable {
public:
#if 0
	//
	//	Memory Usage
	//	------------
	//	TYPE	RES			USAGE
	//	f64		2048		16K
	//	f64		4096		32K
	//	f64		8192		64K
	//	f64		16384		128K
	//	f64		32768		256K
	//	f64		65536		512M
	//	f64		131072		1M
	//	f64		262144		2M
	//	f64		524288		4M
	//	f64		1050576		8M
	//	f64		2101152		16M
	//

	enum {
		//
		// Should be a power-of-two; select one
		//

		//RESOLUTION=2048,
		//RESOLUTION=4096,
		//RESOLUTION=8192,
		//RESOLUTION=16384,
		//RESOLUTION=32768,
		//RESOLUTION=65536,
		//RESOLUTION=131072,
		RESOLUTION=262144,
		//RESOLUTION=524144,
		//RESOLUTION=1050576,
		//RESOLUTION=2101152
	};

	static f64 g_sin[RESOLUTION];
#endif
	enum {
		RESOLUTION = DB3_SIN_TABLE_RESOLUTION
	};

	template<typename T>
	static __forceinline uint Degrees(T f) {
		// NOTE: After optimizations, this is two multiplies, and one bit-and.
		return ((uint)((f/360) * RESOLUTION)) % RESOLUTION;
	}
	template<typename T>
	static __forceinline uint Radians(T f) {
		return ((uint)((f/static_cast<T>(TAU)) * RESOLUTION)) % RESOLUTION;
	}

	__forceinline CMathTable() {
		f64 rad;
		uint i;

		for(i=0; i<RESOLUTION; i++) {
			rad = (((f64)i)/RESOLUTION)*TAU;

			//g_sin[i] = ::sin(rad);
			g_pGlob->f64SinTable[i] = ::sin(rad);
		}
	}
	__forceinline ~CMathTable() {
	}
};

template<typename T>
inline T Radians(T x) { return static_cast<T>(x/180.0*3.1415926535897932384626433832795028841971693993751058209); }
template<typename T>
inline T Degrees(T x) { return static_cast<T>(x/PI*180.0); }

template<typename T>
__forceinline T SinFast(T x) { return static_cast<T>(g_pGlob->f64SinTable[CMathTable::Degrees(x)]); }
template<typename T>
__forceinline T CosFast(T x) { return static_cast<T>(g_pGlob->f64SinTable[CMathTable::Degrees(x + 90)]); }

template<typename T>
__forceinline T SinPrec(T x) { return ::sin(Radians(x)); }
template<typename T>
__forceinline T CosPrec(T x) { return ::cos(Radians(x)); }

#if DB3_PRECISE
template<typename T>
__forceinline T Cos(T x) { return CosPrec(x); }
template<typename T>
__forceinline T Sin(T x) { return SinPrec(x); }
#else
template<typename T>
__forceinline T Cos(T x) { return CosFast(x); }
template<typename T>
__forceinline T Sin(T x) { return SinFast(x); }
#endif

template<typename T>
__forceinline T Tan(T x) { return Sin(x)/Cos(x); }

template<typename T>
__forceinline T ACos(T x) { return Degrees(acos(x)); }
template<typename T>
__forceinline T ASin(T x) { return Degrees(asin(x)); }
template<typename T>
__forceinline T ATan(T x) { return Degrees(atan(x)); }
template<typename T>
__forceinline T ATan2(T y, T x) { return Degrees(atan2(y, x)); }

//----------------------------------------------------------------------------------------------------------------------

__forceinline i32 Sign(i32 a) {
	return (1 | (a>>31));
}
__forceinline f32 Sign(f32 a) {
	return (f32)(1 | (((i32)a)>>31));
}
__forceinline i32 Abs(i32 x) {
	return (x+(x>>31))^(x>>31);
}
__forceinline f32 Abs(f32 x) {
	union { f32 f; u32 i; } v;

	v.f = x;
	v.i &= 0x7FFFFFFFUL;

	return v.f;
}
__forceinline i32 FloatToIntFast(f32 x) {
	union { f32 f; i32 i; } v;

	v.f   = x + (f32)(3 << 21);
	v.i  -= 0x4AC00000;
	v.i >>= 1;

	return v.i;
}
__forceinline u32 FloatToUIntFast(f32 x) {
	union { f32 f; i32 i; } v;

	v.f   = x + (f32)(3 << 21);
	v.i  -= 0x4AC00000;
	v.i >>= 1;
	v.i  &= 0x7FFFFFFF;

	return *(u32 *)&v.i;
}
__forceinline f32 SqrtFast(f32 x) {
	union { f32 f; i32 i; } v;

	v.f   = x;
	v.i  -= 0x3F800000;
	v.i >>= 1;
	v.i  += 0x3F800000;

	return v.f;
}
__forceinline f32 InvSqrtFast(f32 x) {
	union { f32 f; i32 i; } v;

	v.f = x;
	v.i = 0x5F3759DF - (v.i >> 1);

	return v.f * (1.5f - 0.5f * x * v.f * v.f);
}

// Copy the sign of 'b' into 'a'
inline i32 CopySign(i32 a, i32 b) {
	i32 difmsk;

	// absolute
	//a = (a + (a >> (sizeof(a)*8 - 1))) ^ (a >> (sizeof(a)*8 - 1));

	difmsk = (((a ^ b) & (1<<LB(a)))>>LB(a));
	//printf("[difmsk=0x%.8X]", *(u32 *)&difmsk);

	return (-a & difmsk) | (a & ~difmsk);
}
inline i64 CopySign(i64 a, i64 b) {
	i64 difmsk;

	difmsk = (((a ^ b) & (1ULL<<LB(a)))>>LB(a));

	return (-a & difmsk) | (a & ~difmsk);
}
inline f32 CopySign(f32 a, f32 b) {
	i32 i;

	i = ((*(i32 *)&a)&~(1<<LB(a)))|((*(i32 *)&b)&(1<<LB(b)));

	return *(f32 *)&i;
}
inline f64 CopySign(f64 a, f64 b) {
	i64 i;

	i = ((*(i64 *)&a)&~(1ULL<<LB(a)))|((*(i64 *)&b)&(1ULL<<LB(b)));

	return *(f64 *)&i;
}

template<typename T, typename T2>
__forceinline T Lerp(const T &x, const T &y, const T2 &t) { return x + (y - x)*t; }

__forceinline f64 NormalizeAngle360(f64 angle) {
	if (((u32 *)&angle)[1] > 0x40768000)
		angle -= floor(angle/360.0)*360.0;

	return angle;
}
__forceinline f32 NormalizeAngle360(f32 angle) {
	// This simultaneously checks whether angle is above 360.0f or below 0.0
	// (negatives get the sign-bit set). 0x43B40000 == 360.0f.
	if ((*(u32 *)&angle) > 0x43B40000)
		angle -= floorf(angle/360.0f)*360.0f;

	//if (angle < 0.0 || angle > 360.0)
	//	angle -= floor(angle/360.0)*360.0;

	return angle;
}

template<typename T>
__forceinline T NormalizeAngle180(T angle) {
	angle = NormalizeAngle360(angle);
	/*
	 * No branching version (32-bit)
	 * -----------------------------
	 * union { f32 f; i32 i; } v;
	 *
	 * v.f  = angle;
	 * v.i -= <bit-vector of 180.0f>;
	 * v.i  = -((v.i&0x80000000)>>31) & <bit-vector of 360.0>
	 *
	 * return angle - v.f;
	 *
	 * Assembly (dst, src)
	 * -------------------
	 * mov r1, [angle] //although, r1 will already be angle from the call above
	 * mov r2, r1
	 * sub r2, 180.0f
	 * and r2, 1<<31
	 * shr r2, 31
	 * neg r2
	 * and r2, 360.0f
	 * sub r1, r2
	 *
	 * Analysis
	 * --------
	 * Each of the instructions above should take one cycle. Ignoring the first
	 * mov, that would be a total of 7 cycles.
	 *
	 * If a branch misprediction costs 50 cycles then on average we should
	 * optimistically expect 9 cycles lost due to branch misprediction. The
	 * branching version would then cost around 11~15 cycles. (Worst case at 60
	 * cycles.)
	 *
	 * The non-branching version is better, but GCC does not produce optimal
	 * code matching the assembly. Since this function is not called very often,
	 * there's no point in going through the trouble of writing the assembly due
	 * to portability issues.
	 *
	 * Still, this is an interesting observation.
	 */
	return angle > 180.0 ? angle - 360.0 : angle;
}

template<typename T>
__forceinline T AngleDelta(T a, T b) {
	return NormalizeAngle180(a - b);
}
// Round 'x' to the nearest fitting power-of-two */
__forceinline i32 NextPowerOfTwo(i32 x) {
	if ((x & (x - 1)) != 0) {
		x--;
		x |= x>>1;
		x |= x>>2;
		x |= x>>4;
		x |= x>>8;
		x |= x>>16;
		x++;
	}

	return x;
}
__forceinline i64 NextPowerOfTwo(i64 x) {
	if ((x & (x - 1)) != 0) {
		x--;
		x |= x>>1;
		x |= x>>2;
		x |= x>>4;
		x |= x>>8;
		x |= x>>16;
		x |= x>>32;
		x++;
	}

	return x;
}
__forceinline i32 NextSquarePowerOfTwo(i32 x, i32 y) {
	return NextPowerOfTwo(x > y ? x : y);
}
__forceinline i64 NextSquarePowerOfTwo(i64 x, i64 y) {
	return NextPowerOfTwo(x > y ? x : y);
}

__forceinline i32 Clamp(i32 x, i32 l, i32 h) {
	return x < l ? l : x > h ? h : x;
}
__forceinline i64 Clamp(i64 x, i64 l, i64 h) {
	return x < l ? l : x > h ? h : x;
}
__forceinline f32 Clamp(f32 x, f32 l, f32 h) {
#if DB3_USE_CORE_SIMD
	_mm_store_ss(&x, _mm_min_ss(_mm_max_ss(_mm_set_ss(x),_mm_set_ss(l)), _mm_set_ss(h)));

	return x;
#else
	x = (x + h - abs(x - h))*0.5f;
	x = (x + l + abs(x - l))*0.5f;

	return x;
#endif
}
__forceinline f64 Clamp(f64 x, f64 l, f64 h) {
#if DB3_USE_CORE_SIMD
	_mm_store_sd(&x, _mm_min_sd(_mm_max_sd(_mm_set_sd(x),_mm_set_sd(l)), _mm_set_sd(h)));

	return x;
#else
	x = (x + h - abs(x - h))*0.5;
	x = (x + l + abs(x - l))*0.5;

	return x;
#endif
}

template<typename T>
__forceinline T ClampSNorm(T x) {
	return Clamp(x, static_cast<T>(-1), static_cast<T>(+1));
}

template<typename T>
__forceinline T ClampUNorm(T x) {
	return Clamp(x, static_cast<T>(0), static_cast<T>(+1));
}

#define __DB3_FSEL(T)\
	f##T FSel(f##T f, f##T x, f##T y) {\
		u##T m, r;\
		m = -(((*(u##T *)&f)>>(T - 1)) & 1);\
		r = (m&*(u##T *)&y)|(~m&*(u##T *)&x);\
		return *(f##T *)&r;\
	}
#if _MSC_VER
# pragma warning(push)
# pragma warning(disable:4146) //unary minus operator applied to unsigned type, result still unsigned
#endif
__forceinline __DB3_FSEL(32);
__forceinline __DB3_FSEL(64);
#if _MSC_VER
# pragma warning(pop)
#endif

#ifndef DB3_NO_MINMAX
__forceinline f32 Min(f32 x, f32 y) {
#if DB3_USE_CORE_SIMD
	_mm_store_ss(&x, _mm_min_ss(_mm_set_ss(x), _mm_set_ss(y)));
	return x;
#else
	return FSel(y - x, x, y);
#endif
}
__forceinline f64 Min(f64 x, f64 y) {
#if DB3_USE_CORE_SIMD
	_mm_store_sd(&x, _mm_min_sd(_mm_set_sd(x), _mm_set_sd(y)));
	return x;
#else
	return FSel(y - x, x, y);
#endif
}

__forceinline f32 Max(f32 x, f32 y) {
#if DB3_USE_CORE_SIMD
	_mm_store_ss(&x, _mm_max_ss(_mm_set_ss(x), _mm_set_ss(y)));
	return x;
#else
	return FSel(x - y, x, y);
#endif
}
__forceinline f64 Max(f64 x, f64 y) {
#if DB3_USE_CORE_SIMD
	_mm_store_sd(&x, _mm_max_sd(_mm_set_sd(x), _mm_set_sd(y)));
	return x;
#else
	return FSel(x - y, x, y);
#endif
}
#endif

#define __DB3_NEGATE_IF_SIGNED(T,X)\
	f##T NegateIfSigned(f##T f, f##T x) {\
		u##T r;\
		r = (*(u##T *)&f)^((*(u##T *)&x) & (1##X<<(T - 1)));\
		return *(f##T *)&r;\
	}
__forceinline __DB3_NEGATE_IF_SIGNED(32,UL);
__forceinline __DB3_NEGATE_IF_SIGNED(64,ULL);

#define __DB3_NEGATE_IF_UNSIGNED(T,X)\
	f##T NegateIfUnsigned(f##T f, f##T x) {\
		u##T r;\
		r = (*(u##T *)&f)^(~(*(u##T *)&x) & (1##X<<(T - 1)));\
		return *(f##T *)&r;\
	}
__forceinline __DB3_NEGATE_IF_UNSIGNED(32,UL);
__forceinline __DB3_NEGATE_IF_UNSIGNED(64,ULL);

//----------------------------------------------------------------------------------------------------------------------

DB_LEAVE_NS()
