#pragma once

#include <windows.h>
#include "DB3.h"

#if _MSC_VER
# include <intrin.h>

# pragma intrinsic(_InterlockedIncrement)
# pragma intrinsic(_InterlockedDecrement)
# pragma intrinsic(_InterlockedExchange)
# pragma intrinsic(_InterlockedCompareExchange)
# pragma intrinsic(_InterlockedAnd)
# pragma intrinsic(_InterlockedOr)
# pragma intrinsic(_InterlockedXor)

# if _WIN64
#  pragma intrinsic(_InterlockedIncrement64)
#  pragma intrinsic(_InterlockedDecrement64)
#  pragma intrinsic(_InterlockedExchange64)
#  pragma intrinsic(_InterlockedCompareExchange64)
#  pragma intrinsic(_InterlockedAnd64)
#  pragma intrinsic(_InterlockedOr64)
#  pragma intrinsic(_InterlockedXor64)
# else
#  define _InterlockedIncrement64 InterlockedIncrement64
#  define _InterlockedDecrement64 InterlockedDecrement64
#  define _InterlockedExchange64 InterlockedExchange64
#  define _InterlockedCompareExchange64 InterlockedCompareExchange64
#  define _InterlockedAnd64 InterlockedAnd64
#  define _InterlockedOr64 InterlockedOr64
#  define _InterlockedXor64 InterlockedXor64
# endif

# if 0
#  pragma intrinsic(_InterlockedExchangePointer)
#  pragma intrinsic(_InterlockedCompareExchangePointer)
# else
#  define _InterlockedExchangePointer InterlockedExchangePointer
#  define _InterlockedCompareExchangePointer InterlockedCompareExchangePointer
# endif
#endif

DB_ENTER_NS()

//------------------------------------------------------------------------------
// 32-bit

__forceinline u32 atomic_inc(volatile u32 *x) {
#if _MSC_VER
	return _InterlockedIncrement((volatile LONG *)x);
#else
	return __sync_fetch_and_add(x, 1);
#endif
}
__forceinline u32 atomic_dec(volatile u32 *x) {
#if _MSC_VER
	return _InterlockedDecrement((volatile LONG *)x);
#else
	return __sync_fetch_and_sub(x, 1);
#endif
}
__forceinline u32 atomic_set(volatile u32 *x, u32 y) {
#if _MSC_VER
	return _InterlockedExchange((volatile LONG *)x, y);
#else
	return __sync_lock_test_and_set(x, y);
#endif
}
__forceinline u32 atomic_set_eq(volatile u32 *dst, u32 src, u32 cmp) {
#if _MSC_VER
	return _InterlockedCompareExchange((volatile LONG *)dst, src, cmp);
#else
	return __sync_val_compare_and_swap(dst, cmp, src);
#endif
}
__forceinline u32 atomic_and(volatile u32 *x, u32 y) {
#if _MSC_VER
	return _InterlockedAnd((volatile LONG *)x, y);
#else
	return __sync_fetch_and_and(x, y);
#endif
}
__forceinline u32 atomic_or(volatile u32 *x, u32 y) {
#if _MSC_VER
	return _InterlockedOr((volatile LONG *)x, y);
#else
	return __sync_fetch_and_or(x, y);
#endif
}
__forceinline u32 atomic_xor(volatile u32 *x, u32 y) {
#if _MSC_VER
	return _InterlockedXor((volatile LONG *)x, y);
#else
	return __sync_fetch_and_xor(x, y);
#endif
}

__forceinline u32 atomic_get(volatile u32 *x) {
	return atomic_or(x, 0);
}

//------------------------------------------------------------------------------
// 64-bit

__forceinline u64 atomic_inc(volatile u64 *x) {
#if _MSC_VER
	return _InterlockedIncrement64((volatile LONGLONG *)x);
#else
	return __sync_fetch_and_add(x, 1);
#endif
}
__forceinline u64 atomic_dec(volatile u64 *x) {
#if _MSC_VER
	return _InterlockedDecrement64((volatile LONGLONG *)x);
#else
	return __sync_fetch_and_sub(x, 1);
#endif
}
__forceinline u64 atomic_set(volatile u64 *x, u64 y) {
#if _MSC_VER
	return _InterlockedExchange64((volatile LONGLONG *)x, y);
#else
	return __sync_lock_test_and_set(x, y);
#endif
}
__forceinline u64 atomic_set_eq(volatile u64 *dst, u64 src, u64 cmp) {
#if _MSC_VER
	return _InterlockedCompareExchange64((volatile LONGLONG *)dst, src, cmp);
#else
	return __sync_val_compare_and_swap(dst, cmp, src);
#endif
}
__forceinline u64 atomic_and(volatile u64 *x, u64 y) {
#if _MSC_VER
	return _InterlockedAnd64((volatile LONGLONG *)x, y);
#else
	return __sync_fetch_and_and(x, y);
#endif
}
__forceinline u64 atomic_or(volatile u64 *x, u64 y) {
#if _MSC_VER
	return _InterlockedOr64((volatile LONGLONG *)x, y);
#else
	return __sync_fetch_and_or(x, y);
#endif
}
__forceinline u64 atomic_xor(volatile u64 *x, u64 y) {
#if _MSC_VER
	return _InterlockedXor64((volatile LONGLONG *)x, y);
#else
	return __sync_fetch_and_xor(x, y);
#endif
}

__forceinline u64 atomic_get(volatile u64 *x) {
	return atomic_or(x, 0);
}

//------------------------------------------------------------------------------
// pointer

template<typename T>
__forceinline T *atomic_set(T *volatile *x, T *y) {
#if _MSC_VER
	return _InterlockedExchangePointer((void *volatile *)x, (void *)y);
#else
	return __sync_lock_test_and_set(x, y);
#endif
}
template<typename T>
__forceinline T *atomic_set_eq(T *volatile *dst, T *src, T *cmp) {
#if _MSC_VER
	return _InterlockedCompareExchangePointer((void *volatile *)dst,
		(void *)src, (void *)cmp);
#else
	return __sync_val_compare_and_swap(dst, cmp, src);
#endif
}

template<typename T>
__forceinline T *atomic_get(T *volatile *x) {
#if AXTEK_ARCH_BITS==64
	return (T *)atomic_get((volatile u64 *)x);
#elif AXTEK_ARCH_BITS==32
	return (T *)atomic_get((volatile u32 *)x);
#else
	if (sizeof(T *)==8)
		return (T *)atomic_get((volatile u64 *)x);

	return (T *)atomic_get((volatile u32 *)x);
#endif
}

class CLock {
protected:
#if _WIN32
	CRITICAL_SECTION m_cs;
#else
	pthread_mutex_t m_mutex;
#endif

public:

#if _WIN32
	inline CLock(): m_cs()
#else
	inline CLock()
#endif
	{
#if _WIN32
		InitializeCriticalSectionAndSpinCount(&m_cs, 4096);
#else
		static pthread_mutexattr_t attr;
		static bool didInit = false;

		if unlikely (!didInit) {
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

			didInit = true;
		}

		pthread_mutex_init(&m_mutex, &attr);
#endif
	}
	inline ~CLock() {
#if _WIN32
		DeleteCriticalSection(&m_cs);
#else
		pthread_mutex_destroy(&m_mutex);
#endif
	}

	inline void Lock() {
#if _WIN32
		EnterCriticalSection(&m_cs);
#else
		pthread_mutex_lock(&m_mutex);
#endif
	}
	inline bool TryLock() {
#if _WIN32
		return (bool)(TryEnterCriticalSection(&m_cs) & true);
#else
		if (!pthread_mutex_trylock(&m_mutex))
			return true;

		return false;
#endif
	}
	inline void Unlock() {
#if _WIN32
		LeaveCriticalSection(&m_cs);
#else
		pthread_mutex_unlock(&m_mutex);
#endif
	}
};

class CAutolock {
protected:
	CLock &m_lock;

public:
	inline CAutolock(CLock &l): m_lock(l) {
		m_lock.Lock();
	}
	inline ~CAutolock() {
		m_lock.Unlock();
	}
};

class CEvent {
protected:
#if _WIN32
	HANDLE m_ev;
#else
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
#endif

public:
#if _WIN32
	inline CEvent(): m_ev(nullptr)
#else
	inline CEvent()
#endif
	{
#if _WIN32
		m_ev = CreateEvent(0, TRUE, FALSE, 0);
#else
		pthread_mutex_init(&m_mutex, 0);
		pthread_cond_init(&m_cond, 0);
#endif
	}
	inline ~CEvent() {
#if _WIN32
		if (!m_ev)
			return;

		CloseHandle(m_ev);
		m_ev = nullptr;
#else
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_mutex);
#endif
	}

	inline void Signal() {
#if _WIN32
		SetEvent(m_ev);
#else
		pthread_cond_signal(&m_cond);
#endif
	}
	inline void Reset() {
#if _WIN32
		ResetEvent(m_ev);
#else
		/* TODO */
#endif
	}

	inline void Sync() {
#if _WIN32
		WaitForSingleObject(m_ev, INFINITE);
#else
		pthread_mutex_lock(&m_mutex);
		pthread_cond_wait(&m_cond, &m_mutex);
		pthread_mutex_unlock(&m_mutex);
#endif
	}
	inline bool Wait(u32 milliseconds) {
#if _WIN32
		if (WaitForSingleObject(m_ev, milliseconds) != WAIT_OBJECT_0)
			return false;

		return true;
#else
		struct timespec ts = {
			milliseconds/1000, (milliseconds%1000)*1000000
		};
		bool r = false;

		pthread_mutex_lock(&m_mutex);
		if (pthread_cond_timedwait(&m_cond, &m_mutex, &ts) == 0)
			r = true;
		pthread_mutex_unlock(&m_mutex);

		return r;
#endif
	}
	inline bool TryWait() {
		return Wait(0);
	}
};

class CSignal {
protected:
	uint m_cur, m_exp;
	CEvent m_event;

public:
	inline CSignal(uint exp=0): m_cur(0), m_exp(exp), m_event() {
	}
	inline ~CSignal() {
	}

	inline void Raise() {
		uint x;

		if ((x = atomic_inc(&m_cur)) >= atomic_get(&m_exp) - 1)
			m_event.Signal();
	}
	inline void Lower() {
		uint x;

		if ((x = atomic_dec(&m_cur)) < atomic_get(&m_exp))
			m_event.Reset();
	}

	inline void Clear() {
		atomic_set(&m_cur, 0);
		m_event.Reset();
	}
	inline void Reset(uint exp) {
		atomic_set(&m_exp, exp);
		atomic_set(&m_cur, 0);
		m_event.Reset();
	}
	inline void IncreaseTarget()
	{
		atomic_inc(&m_exp);
		if (atomic_get(&m_cur) < atomic_get(&m_exp))
			m_event.Reset();
	}
	inline void Sync() {
		m_event.Sync();
	}
	inline bool Wait(uint milliseconds) {
		return m_event.Wait(milliseconds);
	}
	inline bool TryWait() {
		return m_event.TryWait();
	}
};

inline void yield() {
#if _WIN32
	Sleep(0);
#else
	sched_yield(); //use nanosleep() instead?
#endif
}

#define THREADPROCAPI __stdcall
typedef DWORD result_t;

class CWorkQueue
{
public:
	enum
	{
		kMinimumThreads = 1,
		kInitialReserve = 128,
		kMaximumStreams = 4096
	};
	typedef void(*work_function_type)(void *parm);

	struct SWork
	{
		work_function_type Func;
		void *Parm;

		CSignal *Sync;
		SWork *Prev, *Next;
	};

protected:
	template<class T> struct remove_const          { typedef T type; };
	template<class T> struct remove_const<const T> { typedef T type; };

	HANDLE m_semaphore;
	bool m_terminate;

	CRITICAL_SECTION m_lock;

	SWork *m_liveWork;
	SWork *m_deadWork;

	HANDLE m_threads[32];
	uint m_numThreads;

	inline void AddWork(SWork *&queue, SWork *item)
	{
		if (queue!=nullptr)
		{
			item->Next = queue;
			item->Prev = queue->Prev;
			queue->Prev->Next = item;
			queue->Prev = item;
		}
		else
		{
			queue = item;
			queue->Prev = item;
			queue->Next = item;
		}
	}
	inline SWork *RemoveWork(SWork *&queue)
	{
		SWork *item;

		if (queue==nullptr)
			return nullptr;

		item = queue;

		if (queue==queue->Next)
		{
			queue = nullptr;
		}
		else
		{
			queue->Next->Prev = queue->Prev;
			queue->Prev->Next = queue->Next;
			queue = queue->Next;
		}

		return item;
	}
	inline void ReserveDeadWork(uint count)
	{
		uint i;

		for(i=0; i<count; i++)
			AddWork(m_deadWork, new SWork);
	}

	static inline DWORD __stdcall ThreadFunc_f(void *p)
	{
		CWorkQueue *q;
		SWork *w;

		q = reinterpret_cast<decltype(q)>(p);
#if _DEBUG
		if __unlikely(!q)
			return 1;
#endif

		while(true)
		{
			if __unlikely(WaitForSingleObject(q->m_semaphore, INFINITE)!=WAIT_OBJECT_0)
				break;
			if __unlikely(q->m_terminate)
			{
				ReleaseSemaphore(q->m_semaphore, 1, nullptr);
				break;
			}

			EnterCriticalSection(&q->m_lock);
			w = q->RemoveWork(q->m_liveWork);
			LeaveCriticalSection(&q->m_lock);

#if _DEBUG
			if __unlikely(!w)
				return 2;
#endif

			w->Prev = nullptr;
			w->Next = nullptr;

#if _DEBUG
			if __unlikely(!w->Func)
				return 3;
#endif

			w->Func(w->Parm);

			if (w->Sync)
				w->Sync->Raise();

			EnterCriticalSection(&q->m_lock);
			q->AddWork(q->m_deadWork, w);
			LeaveCriticalSection(&q->m_lock);
		}

		return 0;
	}

public:
#pragma warning(push)
#pragma warning(disable:4351)
	inline CWorkQueue()
	: m_semaphore(), m_terminate(false), m_lock(), m_liveWork(nullptr), m_deadWork(nullptr), m_threads(),
	  m_numThreads(0)
	{
		InitializeCriticalSectionAndSpinCount(&m_lock, 4096);
	}
#pragma warning(pop)
	inline ~CWorkQueue()
	{
		Fini();
	}

	inline bool Init(uint numThreads=0)
	{
		uint i;

		m_liveWork = nullptr;
		m_deadWork = nullptr;

		if (!numThreads)
		{
			SYSTEM_INFO si;

			GetSystemInfo(&si);
			numThreads  = static_cast<uint>(si.dwNumberOfProcessors);
			numThreads += numThreads/2; //optimal number of threads is about ~1.5 times the number of logical cores
		}

		if (numThreads > 32)
			numThreads = 32;
		else if(numThreads < kMinimumThreads)
			numThreads = kMinimumThreads;

		m_semaphore = CreateSemaphore(nullptr, 0, kMaximumStreams, nullptr);
		if __unlikely(!m_semaphore)
			return false;

		m_terminate = false;

		m_numThreads = numThreads;
		for(i=0; i<numThreads; i++)
		{
			m_threads[i] = CreateThread(nullptr, 0, &ThreadFunc_f, reinterpret_cast<void *>(this), 0, nullptr);
			if __unlikely(!m_threads[i])
			{
				Fini();
				return false;
			}
		}

		ReserveDeadWork(kInitialReserve);
		return true;
	}
	inline void Fini()
	{
		uint i;

		if (!m_semaphore)
			return;

		m_terminate = true;
		ReleaseSemaphore(m_semaphore, 1, nullptr); //pretty much guaranteed...
		ReleaseSemaphore(m_semaphore, m_numThreads, nullptr); //this should help speed the process up, but may fail

		for(i=0; i<m_numThreads; i++)
		{
			WaitForSingleObject(m_threads[i], 250);
			CloseHandle(m_threads[i]);
		}

		CloseHandle(m_semaphore);
		m_semaphore = nullptr;

		EnterCriticalSection(&m_lock); //other threads might be trying to do stuff...
		SWork *const *heads[] = { &m_liveWork, &m_deadWork };
		uint headIdx;

		for(headIdx=0; headIdx<sizeof(heads)/sizeof(heads[0]); headIdx++)
		{
			SWork *item, *next;

			item = *heads[headIdx];
			while(item != nullptr)
			{
				next = item->Next;
				delete item;
				item = next!=*heads[headIdx] ? next : nullptr;
			}
		}
		LeaveCriticalSection(&m_lock);

		DeleteCriticalSection(&m_lock);
	}

	template<typename T>
	inline bool Enqueue(void(*func)(T *), T *parm=nullptr, CSignal *signal=nullptr)
	{
		SWork *item;
		bool r = false;

		// don't insert anything while "not alive" or in the process of terminating
		if __unlikely(!m_semaphore || m_terminate)
			return false;

		EnterCriticalSection(&m_lock);
		if __likely(m_deadWork) //this is more likely because of an initial reserve and corpse recycling
			item = RemoveWork(m_deadWork);
		else
			item = new SWork;

		if __likely(item != nullptr)
		{
			item->Func = reinterpret_cast<work_function_type>(func);
			//item->Parm = reinterpret_cast<void *>(const_cast<remove_const<decltype(parm)>::type>(parm));
			item->Parm = (void *)parm; //TODO: C casts are considered bad, but the above is ugly and doesn't work.
			item->Sync = signal;
			AddWork(m_liveWork, item);

			r = true;
		}
		LeaveCriticalSection(&m_lock);

		if (!r)
			return false;

		if (signal)
			signal->IncreaseTarget();

		while(!ReleaseSemaphore(m_semaphore, 1, nullptr) && !m_terminate)
			Sleep(10);

		return !m_terminate;
	}
	inline void Reserve(uint count)
	{
		EnterCriticalSection(&m_lock);
		ReserveDeadWork(count);
		LeaveCriticalSection(&m_lock);
	}

	inline uint GetThreadCount() const
	{
		return m_numThreads;
	}
};

DB_LEAVE_NS()
