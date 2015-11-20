#pragma once

#include <Windows.h>
#include "../global.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <utility>

#define DB3_NS db3

#define DB_ENTER_NS() namespace DB3_NS {
#define DB_LEAVE_NS() }

DB_ENTER_NS()

typedef ::int8_t		s8 , i8 ;
typedef ::int16_t		s16, i16;
typedef ::int32_t		s32, i32;
typedef ::int64_t		s64, i64;
typedef ::uint8_t		u8 ;
typedef ::uint16_t		u16;
typedef ::uint32_t		u32;
typedef ::uint64_t		u64;

namespace detail
{
template<size_t _Size> struct _select_size {};
template<> struct _select_size<2> { typedef u16 uint_type; typedef s16 sint_type; };
template<> struct _select_size<4> { typedef u32 uint_type; typedef s32 sint_type; };
template<> struct _select_size<8> { typedef u64 uint_type; typedef s64 sint_type; };
}

typedef detail::_select_size<sizeof(void *)>::uint_type uint;
typedef detail::_select_size<sizeof(void *)>::sint_type sint, iint;

typedef float			f32;
typedef double			f64;

#define TEST_BIT_SIZE(n,x) static_assert(sizeof(n##x)==x/8, #n #x " != " #x)
TEST_BIT_SIZE(s, 8);	TEST_BIT_SIZE(u, 8);
TEST_BIT_SIZE(s,16);	TEST_BIT_SIZE(u,16);
TEST_BIT_SIZE(s,32);	TEST_BIT_SIZE(u,32);	TEST_BIT_SIZE(f,32);
TEST_BIT_SIZE(s,64);	TEST_BIT_SIZE(s,64);	TEST_BIT_SIZE(f,64);
#undef TEST_BIT_SIZE

static_assert(sizeof(uint)==sizeof(void *), "uint != pointer");
static_assert(sizeof(sint)==sizeof(void *), "sint != pointer");

#ifndef __likely
# if __GNUC__||__clang__||__INTEL_COMPILER
#  define __likely(x) (__builtin_expect(!!(x), 1))
#  define __unlikely(x) (__builtin_expect(!!(x), 0))
# else
#  define __likely(x) (x)
#  define __unlikely(x) (x)
# endif
#endif
#ifndef __align32
# if _MSC_VER
#  define __align32 __declspec(align(32))
# endif
#endif
#ifndef __aligned
# define __aligned //used for aesthetic decoration; no real purpose
#endif

#define __DB_CONCAT(x,y) x##y
#define DB_CONCAT(x,y) __DB_CONCAT(x,y)

#ifdef __COUNTER__
# define DB_ANONVAR(x) __DB_CONCAT(x,__COUNTER__)
#else
# define DB_ANONVAR(x) __DB_CONCAT(x,__LINE__)
#endif

template<class _Functor>
class CScopeGuard
{
protected:
//	enum class State
	enum State
	{
		Valid,
		Committed
	};

	_Functor m_Func;
	State m_State;

public:
	inline CScopeGuard(const _Functor &func): m_Func(func), m_State(State::Valid) {}
	inline CScopeGuard(CScopeGuard<_Functor> &&r): m_Func(r.m_Func), m_State(r.m_State) { r.Commit(); }
	inline ~CScopeGuard() { if (m_State==State::Valid) m_Func(); }

	inline void Commit() { m_State = State::Committed; }
};

template<class _Functor>
inline CScopeGuard<_Functor> MakeScopeGuard(const _Functor &func)
{
	return std::forward<CScopeGuard<_Functor>>(func);
}
#define DB_SCOPE_GUARD(lambda) auto DB_ANONVAR(__scopeGuard__) = ::DB3_NS::MakeScopeGuard([&]()lambda)

#define DB_UNCOPYABLE(C)\
	private:\
		C(const C &) = delete;\
		C &operator=(const C &) = delete

#define DB3_CRASH()\
	::db3::crash(__FILE__,__LINE__,__FUNCTION__,"About to crash!")
#define DB3_CRASH_MSG(msg)\
	::db3::crash(__FILE__,__LINE__,__FUNCTION__,msg)
inline void crash(const char *file, unsigned int line, const char *func, const char *msg)
{
#if defined(_DEBUG)||defined(DEBUG)||defined(__debug__)
	static bool stopAsking = false;

	if (stopAsking)
		return;

	char buf[512];
	char m[256];

	const char *p;
	p = strchr(msg, '\0');
	if (static_cast<size_t>(p - msg) >= sizeof(m))
		p = msg + (sizeof(m) - 1);
	memcpy(m, msg, p - msg);
	m[p - msg] = '\0';
	sprintf_s(buf, sizeof(buf),
		"%s(%u) %s\nDEBUG BUILD: %s\nContinue with crash? (Cancel to stop asking)",
		file, line, func, m);

	int r = MessageBoxA(0, buf, "About to crash", MB_ICONEXCLAMATION|MB_YESNOCANCEL);

	if (r==IDCANCEL)
	{
		stopAsking = true;
		return;
	}

	if (r==IDYES)
	{
		volatile int *p = nullptr;
		*p = 5;
	}
#endif
}

#if _MSC_VER
# ifndef __func__
#  define __func__ __FUNCTION__
# endif
#endif

#ifdef _DEBUG
inline void HandleAssert(const char *file, uint line, const char *func, const char *expr, const char *msg)
{
	char buf[2048];

	sprintf_s(buf, "[%s(%u) %s] %s: %s", file, static_cast<u32>(line), func, expr, msg);

	fprintf(stderr, "ERROR %s\n", buf);
	fflush(stderr);

	char boxbuf[2048+256];

	sprintf_s(boxbuf, "%s\n\nDo you want to debug?", buf);

	int r = MessageBoxA(GetActiveWindow(), boxbuf, "Assert Error", MB_ICONERROR|MB_YESNO);
	if (r==IDYES)
	{
		volatile int *p = nullptr;
		*p = 7;

		return;
	}

	ExitProcess(1);
}
# define assert_msg(x,m) if(!(x))::DB3_NS::HandleAssert(__FILE__,__LINE__,__func__,#x,(m))
#else
# define assert_msg(x,m) (void)0
#endif

#undef assert
#define assert(x) assert_msg((x),"assert() failed!")

DB_LEAVE_NS()
