// ParseType.h: interface for the CParseType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSETYPE_H__B7DDAA01_DA88_4BFB_AF03_8C18F1EA34CD__INCLUDED_)
#define AFX_PARSETYPE_H__B7DDAA01_DA88_4BFB_AF03_8C18F1EA34CD__INCLUDED_

// Common Includes
#include "windows.h"

// Custom Includes
#include "Statement.h"

class CParseType  
{
	public:
		CParseType();
		virtual ~CParseType();

	public:
		void			SetStartLineNumber(DWORD line) { m_dwStartLineNumber = line; }
		DWORD			GetStartLineNumber(void) { return m_dwStartLineNumber; }
		void			SetEndLineNumber(DWORD line) { m_dwEndLineNumber = line; }
		DWORD			GetEndLineNumber(void) { return m_dwEndLineNumber; }

		bool			WriteDBM(void);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText);

	private:

		// Debug Data
		DWORD			m_dwStartLineNumber;
		DWORD			m_dwEndLineNumber;
};

#endif // !defined(AFX_PARSETYPE_H__B7DDAA01_DA88_4BFB_AF03_8C18F1EA34CD__INCLUDED_)
