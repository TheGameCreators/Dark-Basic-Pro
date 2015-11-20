// LabelTable.h: interface for the CLabelTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LABELTABLE_H__9DE5F86C_7DE6_4787_B7A5_696E44DC3260__INCLUDED_)
#define AFX_LABELTABLE_H__9DE5F86C_7DE6_4787_B7A5_696E44DC3260__INCLUDED_
#include "ParserHeader.h"
#include "Task.h"

#include "PerfMacros.h"

#ifdef __AARON_LBLTBLPERF__
# include <DB3Dict.h>
#endif

class CLabelTable  
{
	public:
		CLabelTable();
		CLabelTable(LPSTR pChar);
		virtual ~CLabelTable();
		void Free(void);

		void Add(CLabelTable* pNew);
		void Insert(CLabelTable* pNew);
		void AddInOrder(LPSTR pName, CLabelTable* pNew);
		CLabelTable* Advance(DWORD dwCountdown);
		CLabelTable* Subtract(DWORD dwCountdown);
		CLabelTable* GetNext(void) { return m_pNext; }
		CLabelTable* GetPrev(void) { return m_pPrev; }

		bool			AddLabel(LPSTR pStrName, DWORD dwCodeIndex, DWORD dwDataIndex, CStatement* pSRef);
		CLabelTable*	FindLabel(LPSTR pLabelName);
		bool			UpdateLabel(LPSTR pStrName, DWORD dwCodeIndex, DWORD dwDataIndex, CStatement* pSRef);

		void			SetName(CStr* pName) { m_pName=pName; }
		void			SetCodeIndex(DWORD dwIndex) { m_dwCodeIndex=dwIndex; }
		void			SetDataIndex(DWORD dwIndex) { m_dwDataIndex=dwIndex; }
		void			SetBytePosition(DWORD dwIndex) { m_dwBytePos=dwIndex; }
		void			SetSRef(CStatement* pRef) { m_pSRef=pRef; }

		CStr*			GetName(void) { return m_pName; }
		DWORD			GetCodeIndex(void) { return m_dwCodeIndex; }
		DWORD			GetDataIndex(void) { return m_dwDataIndex; }
		DWORD			GetBytePosition(void) { return m_dwBytePos; }
		CStatement*		GetSRef(void) { return m_pSRef; }

		void			UpdateDataIndexOfLabelsAtLine(CStatement* pStatementRef, DWORD dwData);

		bool			WriteDBMHeader(void);
		bool			WriteDBM(void);

	private:

		// Data
		CStr*					m_pName;
		DWORD					m_dwCodeIndex;
		DWORD					m_dwDataIndex;
		DWORD					m_dwBytePos;
		CStatement*				m_pSRef;

		// Hierarchy Data
		CLabelTable*			m_pNext;
		CLabelTable*			m_pPrev;

		// Safe Access
		db3::CLock				m_Lock;

#ifdef __AARON_LBLTBLPERF__
		static db3::TDictionary<CLabelTable> g_Table;
#endif
};

#endif // !defined(AFX_LABELTABLE_H__9DE5F86C_7DE6_4787_B7A5_696E44DC3260__INCLUDED_)
