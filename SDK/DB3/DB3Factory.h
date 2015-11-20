#pragma once

#include "DB3.h"
#include "DB3List.h"
#include "DB3Task.h"

#ifndef DB_NO_OPERATOR_NEW
template<typename T> void *operator new(size_t, T &x) {
	return &x;
}
#endif

DB_ENTER_NS()

namespace Mem
{
	
inline void *Alloc(uint n)
{
	void *q;
	if (!n)
		return nullptr;

	q = malloc(n);
	assert_msg(q != nullptr, "malloc() failed");

	return q;
}
inline nullptr_t Free(void *p)
{
	if (!!p)
		free(p);

	return nullptr;
}
inline void *Realloc(void *p, uint n)
{
	void *q;

	if (!n)
		return Free(p);

	if (!p)
		q = malloc(n);
	else
		q = realloc(p, n);

	assert_msg(q != nullptr, "realloc() failed");

	return q;
}

}

/// Manage resources via object pooling.
///
/// The factory allows you to place a bunch of memory objects into two lists called pools. There's the live list, which
/// represents active (in-use) objects. And there's the dead list, which represents inactive (unused) objects. To
/// improve allocation speed, an existing object (from the dead pool) will be used if possible for an allocation. If it
/// is not available, then the object will be allocated with Mem::Alloc().
template<class T, bool CallConstructor=true>
class TFactory {
protected:
	struct SObject {
		T					Object;
		TListLink<SObject>	Link;
	};

	TListBase<SObject>		m_LiveList,
							m_DeadList;
	CLock					m_LiveLock,
							m_DeadLock;

	SObject *PopDead() {
		SObject *obj;

		CAutolock autoDeadLock(m_DeadLock);
		obj = m_DeadList.First();
		if (!obj)
			return (SObject *)NULL;

		obj->Link.Unlink();
		return obj;
	}
	void PushLive(SObject *obj) {
		{
			CAutolock autoLiveLock(m_LiveLock);
			m_LiveList.PushBack(&obj->Link);
		}

#pragma warning(push)
#pragma warning(disable:4291)
		if (CallConstructor)
			::new(obj->Object) T;
#pragma warning(pop)
	}
	void PushDead(SObject *obj, bool callDestructor) {
		{
			CAutolock autoDeadLock(m_DeadLock);
			m_DeadList.PushBack(&obj->Link);
		}

		if (callDestructor)
			obj->Object.~T();
	}

public:
	/// Basic constructor.
	TFactory(): m_LiveList(), m_DeadList(), m_LiveLock(), m_DeadLock() {
		m_LiveList.SetDeleteObjectOnRemove(false);
		m_LiveList.SetDeleteLinkOnRemove(false);

		m_DeadList.SetDeleteObjectOnRemove(false);
		m_DeadList.SetDeleteLinkOnRemove(false);
	}
	/// Basic destructor.
	///
	/// Performs a Purge() operation to remove all used memory.
	~TFactory() {
		Purge();
	}

	/// Make an allocation from this pool. (Interacts with IHeap via ee::heap.)
	///
	/// If an item is available in the dead pool it will be "resurrected" (added to the live pool). Otherwise, if the
	/// item is not available in the dead pool, it will be allocated. In either case, assuming successful allocation in
	/// the latter, the constructor will be run on the object prior to returning the handle.
	///
	/// \return An object allocated from the pool if successful, or null on error.
	T *Alloc() {
		SObject *obj;

		obj = PopDead();
		if (!obj) {
			obj = (SObject *)Mem::Alloc(sizeof(*obj));
			if __unlikely(!obj)
				return nullptr;

			memset(&obj->Link, 0, sizeof(obj->Link));
			obj->Link.SetObject(obj);
		}

		PushLive(obj);
		return &obj->Object;
	}
	/// Free an object allocated through this pool.
	///
	/// \param p Handle to an object allocated through the pool or NULL.
	/// \return NULL.
	nullptr_t Free(T *p, bool callDestructor=CallConstructor) {
		SObject *obj;
		
		obj = reinterpret_cast<SObject *>(p);
		if __unlikely(!obj)
			return nullptr;

		if (obj->Link.Base()==&m_DeadList) {
			{
				CAutolock autoDeadLock(m_DeadLock);
				obj->Link.Unlink();
			}
			return Mem::Free((void *)obj);
		}

		PushDead(obj, callDestructor);
		return nullptr;
	}

	/// Remove all living and dead pool items.
	///
	/// This function first runs through the list of "live" objects and invokes Free() on them, causing their
	/// destructors to be run and them to be added to the dead pool. Then all of the dead items are run through, with
	/// Free() being invoked again. This causes the items to be removed from memory.
	void Purge() {
		CAutolock autoLiveLock(m_LiveLock);
		while(m_LiveList.FirstLink())
			Free(&m_LiveList.First()->Object, true);

		Clean();
	}
	/// Scrub all dead objects from memory.
	///
	/// A dead object is an object which has been destructed and added to the dead pool, but has not yet been removed
	/// from memory. This function will go through the list of "dead" objects and invoke Free() on them, causing them to
	/// be removed from memory.
	void Clean() {
		CAutolock autoDeadLock(m_DeadLock);
		while(m_DeadList.FirstLink())
			Free(&m_DeadList.First()->Object);
	}
};

//----------------------------------------------------------------------------------------------------------------------

/// Base class for objects to be managed by a factory (See TFactory<>.)
///
/// When you use TObject<> as a base class, you don't have to define TFactory<> anywhere, because it's already taken
/// care of when you inherit from TObject<>.
template<class T> class TObject {
protected:
	static TFactory<T, false> g_Pool;

public:
	TObject() {}
	virtual ~TObject() {}

	inline void *operator new(uint n) {
		assert_msg(n == sizeof(T), "Attempted to allocate an object other than TObject<T>!");

		return reinterpret_cast<void *>(g_Pool.Alloc());
	}
	inline void operator delete(void *p) {
#ifdef DB_TOBJECT_USE_DYNAMIC_CAST
		g_Pool.Free(dynamic_cast<T *>(p), false);
#else
		g_Pool.Free(reinterpret_cast<T *>(p), false);
#endif
	}

	/// Remove all "dead" items from the pool.
	static inline void Clean() {
		g_Pool.Clean();
	}
	/// Remove all "live" items from the pool, then all "dead" items.
	static inline void Purge() {
		g_Pool.Purge();
	}
};

template<class T> TFactory<T, false> TObject<T>::g_Pool;

DB_LEAVE_NS()
