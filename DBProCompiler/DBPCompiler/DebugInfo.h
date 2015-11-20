// DebugInfo.h: interface for the CDebugInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGINFO_H__9766630D_BC56_4F47_8995_F83233070B49__INCLUDED_)
#define AFX_DEBUGINFO_H__9766630D_BC56_4F47_8995_F83233070B49__INCLUDED_

#include "windows.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDebugInfo  
{
	public:
		CDebugInfo();
		~CDebugInfo();

		bool		DebugModeOn(void) { return m_bDebugModeOn; }
		void		SetDebugMode(bool bState, HINSTANCE hInstance) { m_bDebugModeOn=bState; m_hInstance=hInstance; }
		void		SetProgramSize(DWORD dwSize) { m_dwProgramSize=dwSize; }
		void		SetProgramPtr(LPSTR pPtr) { m_dwProgramPtr=pPtr; }
		DWORD		GetProgramSize(void) { return m_dwProgramSize; }
		LPSTR		GetProgramPtr(void) { return m_dwProgramPtr; }
		HINSTANCE	GetInstance(void) { return m_hInstance; }
		void		SetParsingMain(bool bFlag) { m_bParsingMain=bFlag; }
		bool		GetParsingMain(void) { return m_bParsingMain; }
		void		SetLastBreakPoint(DWORD dwPos) { m_dwLastBytePosOfBreakPoint=dwPos; }
		DWORD		GetLastBreakPoint(void) { return m_dwLastBytePosOfBreakPoint; }

		void		SetMessageArrived(bool bFlag) { m_bMessageArrived=bFlag; }
		bool		MessageArrived(void) { return m_bMessageArrived; }
		void		SetCLIText(LPSTR bPtr) { m_pCliText=bPtr; }
		LPSTR		GetCLIText(void) { return m_pCliText; }
		void		SetCLISize(DWORD dwSize) { m_dwCliTextSize=dwSize; }
		DWORD		GetCLISize(void) { return m_dwCliTextSize; }

	private:
		bool		m_bDebugModeOn;
		HINSTANCE	m_hInstance;
		DWORD		m_dwProgramSize;
		LPSTR		m_dwProgramPtr;
		bool		m_bParsingMain;
		DWORD		m_dwLastBytePosOfBreakPoint;

		bool		m_bMessageArrived;
		DWORD		m_dwCliTextSize;
		LPSTR		m_pCliText;
};

#endif // !defined(AFX_DEBUGINFO_H__9766630D_BC56_4F47_8995_F83233070B49__INCLUDED_)
