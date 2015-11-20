// Declaration.h: interface for the CDeclaration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECLARATION_H__105B07AA_795F_45E5_8648_CA2399C29110__INCLUDED_)
#define AFX_DECLARATION_H__105B07AA_795F_45E5_8648_CA2399C29110__INCLUDED_

// Common includes
#include "windows.h"
#include "macros.h"
#include "Str.h"

class CDeclaration  
{
	public:
		CDeclaration();
		virtual ~CDeclaration();
		void Add(CDeclaration* pNew);
		CDeclaration* GetNext(void) { return m_pNext; }
		CDeclaration* GetPrev(void) { return m_pPrev; }
		CDeclaration* Find(LPSTR pName, DWORD dwArrFlag);

		void SetLineNumber(DWORD dwLine) { m_dwLineNumber=dwLine; }
		DWORD GetLineNumber(void) { return m_dwLineNumber; }

		void SetArr(DWORD dwArr) { m_dwArr = dwArr; }
		void SetArrValue(CStr* pArrValue) { m_pArrValue = pArrValue; }
		void SetName(CStr* pName) { m_pName = pName; }
		void SetType(CStr* pType) { m_pType = pType; }
		void SetInit(CStr* pInit) { m_pInit = pInit; }
		void SetOffset(DWORD dwOffset) { m_dwOffset = dwOffset; }
		void SetDataSize(DWORD dwSize) { m_dwDataSize = dwSize; }
		void SetDecData(DWORD dwDecArr, LPSTR pDecArrValue, LPSTR pDecName, LPSTR pDecType, LPSTR pDecInit, DWORD LineNumberRef);

		bool GetNumberOfDecsInChain(DWORD* pdwCount);
		bool GetTypeStringOfDecsInChain(LPSTR* pTypeString);

		CStr* GetName(void) { return m_pName; }
		CStr* GetType(void) { return m_pType; }
		CStr* GetArrValue(void) { return m_pArrValue; }
		DWORD GetArrFlag(void) { return m_dwArr; }
		DWORD GetOffset(void) { return m_dwOffset; }
		DWORD GetDataSize(void) { return m_dwDataSize; }

		bool WriteDBM(void);

	private:

		// Debug Data
		DWORD			m_dwLineNumber;

		// Declaration Data
		DWORD			m_dwArr;
		CStr*			m_pArrValue;
		CStr*			m_pName;
		CStr*			m_pType;
		CStr*			m_pInit;
		DWORD			m_dwOffset;
		DWORD			m_dwDataSize;

		// Hierarchy Data
		CDeclaration*	m_pNext;
		CDeclaration*	m_pPrev;
};

#endif // !defined(AFX_DECLARATION_H__105B07AA_795F_45E5_8648_CA2399C29110__INCLUDED_)
