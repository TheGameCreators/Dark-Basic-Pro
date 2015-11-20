// ParseJump.h: interface for the CParseJump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEJUMP_H__27BAD33A_43FA_42BC_9069_F91E2E4FDD82__INCLUDED_)
#define AFX_PARSEJUMP_H__27BAD33A_43FA_42BC_9069_F91E2E4FDD82__INCLUDED_
#include "ParserHeader.h"

// Custom Includes
#include "Statement.h"
#include "StatementChain.h"

// Defines
#define JUMPTYPE_IF				1
#define JUMPTYPE_GOTO			2
#define JUMPTYPE_GOSUB			3
#define	JUMPTYPE_SELECT			4

class CParseJump  
{
	public:
		CParseJump();
		virtual ~CParseJump();

	public:
		void			SetType(DWORD dwType) { m_dwJumpType = dwType; }
		void			SetBlockA(CStatement *pStatement) { m_pCodeBlockA = pStatement; }
		void			SetBlockALabel(CStr* pLabelNameA) { m_pCodeBlockLabelA=pLabelNameA; }
		void			SetBlockB(CStatement *pStatement) { m_pCodeBlockB = pStatement; }
		void			SetBlockBLabel(CStr* pLabelNameB) { m_pCodeBlockLabelB=pLabelNameB; }
		void			SetBlockChain(CStatementChain* pStatementChain) { m_pCodeBlockChain = pStatementChain; }
		void			SetConditionParameter(CParameter *pParam) { m_pParameter = pParam; }
		void			SetConditionCaseParameter(CParameter *pParam) { m_pCaseParameter = pParam; }
		void			SetConditionLabelParameter(CParameter *pParam) { m_pLabelParameter = pParam; }
		void			SetExitLabelRefParameterRef(CParameter *pParam) { m_pExitLabelParameterRef = pParam; }
		
		DWORD			GetJumpType(void) { return m_dwJumpType; }
		CStatement*		GetBlockA(void) { return m_pCodeBlockA; }
		CStr*			GetBlockLabelA(void) { return m_pCodeBlockLabelA; }
		CStatement*		GetBlockB(void) { return m_pCodeBlockB; }
		CStr*			GetBlockLabelB(void) { return m_pCodeBlockLabelB; }
		CStatementChain* GetBlockChain(void) { return m_pCodeBlockChain; }
		CParameter*		GetConditionParameter(void) { return m_pParameter; }
		CParameter*		GetConditionCaseParameter(void) { return m_pCaseParameter; }
		CParameter*		GetConditionLabelParameter(void) { return m_pLabelParameter; }
		CParameter*		GetExitLabelRefParameterRef(void) { return m_pExitLabelParameterRef; }

		void			SetStartLineNumber(DWORD line) { m_dwStartLineNumber = line; }
		DWORD			GetStartLineNumber(void) { return m_dwStartLineNumber; }
		void			SetMiddleLineNumber(DWORD line) { m_dwMiddleLineNumber = line; }
		DWORD			GetMiddleLineNumber(void) { return m_dwMiddleLineNumber; }
		void			SetEndLineNumber(DWORD line) { m_dwEndLineNumber = line; }
		DWORD			GetEndLineNumber(void) { return m_dwEndLineNumber; }

		bool			WriteDBM(DWORD PlacementCode);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult);

	private:

		// Debug Data
		DWORD				m_dwStartLineNumber;
		DWORD				m_dwMiddleLineNumber;
		DWORD				m_dwEndLineNumber;

		// Loop Data
		DWORD				m_dwJumpType;
		CStatement*			m_pCodeBlockA;
		CStr*				m_pCodeBlockLabelA;
		CStatement*			m_pCodeBlockB;
		CStr*				m_pCodeBlockLabelB;
		CParameter*			m_pParameter;
		CParameter*			m_pCaseParameter;
		CParameter*			m_pLabelParameter;
		CParameter*			m_pExitLabelParameterRef;

		CStatementChain*	m_pCodeBlockChain;
};

#endif // !defined(AFX_PARSEJUMP_H__27BAD33A_43FA_42BC_9069_F91E2E4FDD82__INCLUDED_)
