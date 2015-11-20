// Error.h: interface for the CError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__F0A3CDF6_50BC_4230_92B8_2FA62212053E__INCLUDED_)
#define AFX_ERROR_H__F0A3CDF6_50BC_4230_92B8_2FA62212053E__INCLUDED_

// Common Includes
#ifndef DARKEXE
# include "Str.h"
#else
typedef void CStr;
#endif
#include "DB3Task.h"

// Error Value Defines
#define ERR_INTERNAL	0
#define ERR_SYNTAX		100000
#define ERR_COMPILER	200000

#define __UNKNOWN_ERR_STR__ "??" "?"
class CError  
{
	public:
		CError();
		virtual ~CError();

	public:
		bool IsError(void) { return m_bErrorExist; }
		bool IsParserError(void) { return m_bParserErrorExist; }
#ifdef DARKEXE
		LPSTR GetErrorString() { return __UNKNOWN_ERR_STR__; }
		LPSTR GetParserErrorString() { return __UNKNOWN_ERR_STR__; }
#else
		LPSTR GetErrorString(void) { if(m_pErrorString) return m_pErrorString->GetStr(); else return __UNKNOWN_ERR_STR__; }
		LPSTR GetParserErrorString(void) { if(m_pParserErrorString) return m_pParserErrorString->GetStr(); else return __UNKNOWN_ERR_STR__; }
#endif
		void PrepareVerboseErrorHeader(DWORD LineNumber, LPSTR ErrorString);
		void AddErrorString(LPSTR ErrorString);
		void SetParserError(DWORD LineNumber, LPSTR ErrorString);
		void OutputInternalErrorReport(void);

	public:
		DWORD CountDatabaseSubset(LPSTR pSection, LPSTR pErrorFilename);
		LPSTR* CreateDatabaseSubset(LPSTR pSection, DWORD dwMax, LPSTR pErrorFilename);
		LPSTR* CreateRuntimeDatabaseSubset(LPSTR pSection, DWORD dwMax, LPSTR pErrorFilename);
		void LoadErrorDatabase(LPSTR pErrorFilename);
		void FreeDatabaseSubset(LPSTR* pDatabase, DWORD dwMax);
		void FreeErrorDatabase(void);

		DWORD GetRuntimeErrorStringMax(void) { return m_dwRuntimeErrorsMax; }
		LPSTR GetRuntimeErrorString(int iIndex) { return m_pRuntimeError[iIndex]; }

	public:
		void GetErrorConstruction(DWORD dwLine, DWORD dwErrCode, CStr** pRawErrorString);
		DWORD GetTokenIndex(CStr* pTokenFieldString);
		LPSTR CreateAndReword ( LPSTR pI );
		void ConstructError(DWORD dwLine, DWORD dwErrCode, LPSTR pA, LPSTR pB, LPSTR pC);
		void SetError(DWORD dwLine, DWORD dwErrCode);
		void SetError(DWORD dwLine, DWORD dwErrCode, DWORD dw1);
		void SetError(DWORD dwLine, DWORD dwErrCode, LPSTR lp1);
		void SetError(DWORD dwLine, DWORD dwErrCode, LPSTR lp1, LPSTR lp2);

	public:
		void ProgressReport(LPSTR lpString, DWORD dwValue);
		DWORD GetPerc(DWORD pPerc) { return (DWORD)((m_dwMaxLines/100.0f)*pPerc); }
		void SetMaxLines(DWORD dwMaxLines) { m_dwMaxLines=dwMaxLines;; }


	private:
		bool		m_bParserErrorExist;
		CStr*		m_pParserErrorString;

	private:
		bool		m_bErrorExist;
		CStr*		m_pErrorString;

	private:
		bool		m_bEstablishedConnectionToMonitor;
		HANDLE		m_hMonitorFileMap;
		LPVOID		m_lpVoidMonitor;
//		HWND		m_hWndMonitor;
		DWORD		m_dwMaxLines;

	private:
		DWORD		m_dwInternalErrorsMax;
		LPSTR*		m_pInternalError;
		DWORD		m_dwParserErrorsMax;
		LPSTR*		m_pParserError;
		DWORD		m_dwRuntimeErrorsMax;
		LPSTR*		m_pRuntimeError;

	private:
		db3::CLock	m_Lock;
};

#endif // !defined(AFX_ERROR_H__F0A3CDF6_50BC_4230_92B8_2FA62212053E__INCLUDED_)
