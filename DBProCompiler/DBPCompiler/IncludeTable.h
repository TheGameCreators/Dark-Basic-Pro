// IncludeTable.h: interface for the CIncludeTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INCLUDETABLE_H__D33B3943_5954_4196_B917_9DA3ACD7978D__INCLUDED_)
#define AFX_INCLUDETABLE_H__D33B3943_5954_4196_B917_9DA3ACD7978D__INCLUDED_
#include "ParserHeader.h"
#include "Task.h"

class CIncludeTable  
{
	public:
		CIncludeTable();
		virtual ~CIncludeTable();
		void Add(CIncludeTable* pNew);
		CIncludeTable* GetNext(void) { return m_pNext; }

		bool				FindInclude(LPSTR pFilename);
		void				SetFilename(CStr* pFile) { m_pFilename=pFile; }
		CStr*				GetFilename(void) { return m_pFilename; }
		void				SetFirstByte(DWORD dwByte) { m_dwFirstByte=dwByte; }
		DWORD				GetFirstByte(void) { return m_dwFirstByte; }

	private:

		// Include Data
		CStr*				m_pFilename;
		DWORD				m_dwFirstByte;

		// Hierarchy Data
		CIncludeTable*		m_pNext;

		// Safe Access
		db3::CLock			m_Lock;
};

#endif // !defined(AFX_INCLUDETABLE_H__D33B3943_5954_4196_B917_9DA3ACD7978D__INCLUDED_)
