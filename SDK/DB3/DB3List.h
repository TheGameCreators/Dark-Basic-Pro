#pragma once

#include "DB3.h"

DB_ENTER_NS()

//
//	The TListBase<T> and TListLink<T> classes are useful for lower level TList
//	management. They allow a finer level of control than the TList<T> system.
//
//	The TList<T> system is for general ease-of-use.
//

template<class T> class TListBase;

template<class T>
class TListLink {
protected:
	friend class TListBase<T>;

	T *m_Obj;
	TListLink<T> *m_Prev, *m_Next;
	TListBase<T> *m_Base;

public:
	inline TListLink(T *obj=nullptr, TListBase<T> *base=nullptr): m_Obj(obj), m_Prev(nullptr), m_Next(nullptr),
	m_Base(nullptr) {
		if (base)
			base->AddLast(this);
	}
	inline ~TListLink() {
		Remove();
	}

	inline bool IsLinked() const { return m_Base != nullptr; }

	inline void Unlink() {
		if __unlikely(!m_Base)
			return;

		m_Base->Unlink(this);
	}

	inline void Remove(bool delObj=false, bool delLnk=false) {
		if __unlikely(!m_Base)
			return;

		m_Base->Remove(this, delObj, delLnk);
	}

	inline void SetObject(T *obj) {
		m_Obj = obj;
	}

	inline const TListLink<T> *LinkBefore() const { return m_Prev; }
	inline const TListLink<T> *LinkAfter () const { return m_Next; }
	inline       TListLink<T> *LinkBefore()       { return m_Prev; }
	inline       TListLink<T> *LinkAfter ()       { return m_Next; }

	inline const TListBase<T> *Base      () const { return m_Base; }
	inline       TListBase<T> *Base      ()       { return m_Base; }

	inline const           T  *Object    () const { return m_Obj ; }
	inline                 T  *Object    ()       { return m_Obj ; }

	inline const           T  *Before    () const { return m_Prev ? m_Prev->m_Obj : nullptr; }
	inline const           T  *After     () const { return m_Next ? m_Next->m_Obj : nullptr; }
	inline                 T  *Before    ()       { return m_Prev ? m_Prev->m_Obj : nullptr; }
	inline                 T  *After     ()       { return m_Next ? m_Next->m_Obj : nullptr; }

	inline const TListLink<T> *PrevLink  () const { return m_Prev; }
	inline const TListLink<T> *NextLink  () const { return m_Next; }
	inline       TListLink<T> *PrevLink  ()       { return m_Prev; }
	inline       TListLink<T> *NextLink  ()       { return m_Next; }

	inline const           T  *Prev      () const { return m_Prev ? m_Prev->m_Obj : nullptr; }
	inline const           T  *Next      () const { return m_Next ? m_Next->m_Obj : nullptr; }
	inline                 T  *Prev      ()       { return m_Prev ? m_Prev->m_Obj : nullptr; }
	inline                 T  *Next      ()       { return m_Next ? m_Next->m_Obj : nullptr; }

	inline void InsertBefore(TListLink<T> *that) {
		assert(that != nullptr);
		assert(that->m_Base != nullptr);

		that->m_Base->InsertBefore(this, that);
	}
	inline void InsertAfter(TListLink<T> *that) {
		assert(that != nullptr);
		assert(that->m_Base != nullptr);

		that->m_Base->InsertAfter(this, that);
	}
};

//----------------------------------------------------------------------------------------------------------------------

template<class T>
class TListBase {
protected:
	friend class TListLink<T>;

	bool m_DelObjOnRm;
	bool m_DelLnkOnRm;

	TListLink<T> *m_Head, *m_Tail;

	mutable TListLink<T> *m_Curr;
	mutable uint m_CurrIndex;
	uint m_Count;

	inline const TListLink<T> *GetLink_(uint i) const {
		//
		//	TODO: This can really be improved. This should be investigated further.
		//
#if _DEBUG
		assert_msg(i < m_Count, "Requested index out of range in list!");
#else
		if __unlikely(i >= m_Count)
			return nullptr;
#endif

		if (!m_Curr) {
			m_Curr = m_Head;
			m_CurrIndex = 0;
		}

		while(m_CurrIndex < i && m_Curr) {
			m_CurrIndex++;
			m_Curr = m_Curr->m_Next;
		}

		while(m_CurrIndex > i && m_Curr) {
			m_CurrIndex--;
			m_Curr = m_Curr->m_Prev;
		}

		return m_CurrIndex==i ? m_Curr : nullptr;
	}
	inline const T *GetObject_(uint i) const {
		auto link = GetLink(i);
		return link ? link->m_Obj : nullptr;
	}

public:
	inline TListBase(bool delObjOnRm=false, bool delLnkOnRm=false)
	: m_DelObjOnRm(delObjOnRm), m_DelLnkOnRm(delLnkOnRm), m_Head(nullptr), m_Tail(nullptr), m_Curr(nullptr),
	  m_CurrIndex(0), m_Count(0) {
	}
	inline ~TListBase() {
		RemoveAll();
	}

	inline void SetDeleteObjectOnRemove(bool delObjOnRm) {
		m_DelObjOnRm = delObjOnRm;
	}
	inline void SetDeleteLinkOnRemove(bool delLnkOnRm) {
		m_DelLnkOnRm = delLnkOnRm;
	}
	inline bool GetDeleteObjectOnRemove() const {
		return m_DelObjOnRm;
	}
	inline bool GetDeleteLinkOnRemove() const {
		return m_DelLnkOnRm;
	}

	inline void PushFront(TListLink<T> *link) {
		if __unlikely(!link)
			return;

		link->Unlink();
		link->m_Base = this;
		m_Count++;

		if ((link->m_Next = m_Head) != nullptr)
			m_Head->m_Prev = link;
		else
			m_Tail = link;
		m_Head = link;
	}
	inline void PushBack(TListLink<T> *link) {
		if __unlikely(!link)
			return;

		link->Unlink();
		link->m_Base = this;
		m_Count++;

		if ((link->m_Prev = m_Tail) != nullptr)
			m_Tail->m_Next = link;
		else
			m_Head = link;
		m_Tail = link;
	}

	inline TListLink<T> *PopFront(bool rm=false) {
		TListLink<T> *link;

		if ((link = m_Head) != nullptr) {
			if (rm)
				link->Remove();
			else
				link->Unlink();
		}

		return link;
	}
	inline TListLink<T> *PopBack(bool rm=false) {
		TListLink<T> *link;

		if ((link = m_Tail) != nullptr) {
			if (rm)
				link->Remove();
			else
				link->Unlink();
		}

		return link;
	}

	inline void AddFirst(TListLink<T> *link) { PushFront(link); }
	inline void AddLast (TListLink<T> *link) { PushBack (link); }

	inline void InsertBefore(TListLink<T> *link, TListLink<T> *before) {
		if __unlikely(!link)
			return;

		if __unlikely(!before) {
			AddFirst(link);
			return;
		}

		assert_msg(link != before, "Cannot insert a link before itself!");
		assert_msg(before->m_Base==this, "Tried to InsertBefore() with wrong link base (before arg)");

		link->Unlink();

		link->m_Base = this;
		m_Count++;

		if ((link->m_Prev = before->m_Prev) != nullptr)
			link->m_Prev->m_Next = link;
		else
			m_Head = link;
		before->m_Prev = link;
		link->m_Next = before;
	}
	inline void InsertAfter(TListLink<T> *link, TListLink<T> *after) {
		if __unlikely(!link)
			return;

		if __unlikely(!after) {
			AddLast(link);
			return;
		}

		assert_msg(link != after, "Cannot insert a link after itself!");
		assert_msg(after->m_Base==this, "Tried to InsertAfter() with wrong link base (after arg)");

		link->Unlink();

		link->m_Base = this;
		m_Count++;

		if ((link->m_Next = after->m_Next) != nullptr)
			link->m_Next->m_Prev = link;
		else
			m_Tail = link;
		after->m_Next = link;
		link->m_Prev = after;
	}

	inline const TListLink<T> *FirstLink() const { return m_Head; }
	inline const TListLink<T> * LastLink() const { return m_Tail; }
	inline       TListLink<T> *FirstLink()       { return m_Head; }
	inline       TListLink<T> * LastLink()       { return m_Tail; }

	inline const           T  *First    () const { return m_Head ? m_Head->Object() : nullptr; }
	inline const           T  * Last    () const { return m_Tail ? m_Tail->Object() : nullptr; }

	inline                 T  *First    ()       { return m_Head ? m_Head->Object() : nullptr; }
	inline                 T  * Last    ()       { return m_Tail ? m_Tail->Object() : nullptr; }

	inline const TListLink<T> *HeadLink () const { return m_Head; }
	inline const TListLink<T> *TailLink () const { return m_Tail; }
	inline       TListLink<T> *HeadLink ()       { return m_Head; }
	inline       TListLink<T> *TailLink ()       { return m_Tail; }

	inline const           T  *Head     () const { return First(); }
	inline const           T  *Tail     () const { return Last (); }

	inline                 T  *Head     ()       { return First(); }
	inline                 T  *Tail     ()       { return Last (); }

	inline void Unlink(TListLink<T> *link) {
		if __unlikely(!link)
			return;

		if __unlikely(!link->m_Base)
			return;

		assert_msg(link->m_Base == this, "Tried to remove link from unexpected base.");

		m_Count--;

		m_Curr = nullptr;
		m_CurrIndex = 0;

		if (link->m_Prev)
			link->m_Prev->m_Next = link->m_Next;
		else
			m_Head               = link->m_Next;

		if (link->m_Next)
			link->m_Next->m_Prev = link->m_Prev;
		else
			m_Tail               = link->m_Prev;

		link->m_Base = nullptr;
		link->m_Prev = nullptr;
		link->m_Next = nullptr;
	}
	inline void Remove(TListLink<T> *link, bool delObj=false, bool delLnk=false) {
		if __unlikely(!link)
			return;

		if __unlikely(!link->m_Base)
			return;

		link->Unlink();

		if (m_DelObjOnRm | delObj)
			delete link->m_Obj;

		if (m_DelLnkOnRm | delLnk)
			delete link;
	}

	inline void RemoveAll() {
		while(m_Head)
			Remove(m_Head);
	}

	inline uint                GetCount (      ) const { return                            m_Count       ; }
	inline uint                   Count (      ) const { return                            m_Count       ; }

	inline const TListLink<T> *GetLink  (uint i) const { return                            GetLink_  (i) ; }
	inline       TListLink<T> *GetLink  (uint i)       { return const_cast<TListLink<T> *>(GetLink_  (i)); }
	inline const TListLink<T> *   Link  (uint i) const { return                            GetLink   (i) ; }
	inline       TListLink<T> *   Link  (uint i)       { return                            GetLink   (i) ; }

	inline const           T  *GetObject(uint i) const { return                            GetObject_(i) ; }
	inline                 T  *GetObject(uint i)       { return const_cast<          T  *>(GetObject_(i)); }
	inline const           T  *   Object(uint i) const { return                            GetObject (i) ; }
	inline                 T  *   Object(uint i)       { return                            GetObject (i) ; }
};

DB_LEAVE_NS()
