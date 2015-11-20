// ParseUserFunction.h: interface for the CParseUserFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEUSERFUNCTION_H__7FF9540A_8B4A_4B3A_B490_A2E4549AD774__INCLUDED_)
#define AFX_PARSEUSERFUNCTION_H__7FF9540A_8B4A_4B3A_B490_A2E4549AD774__INCLUDED_
#include "ParserHeader.h"

// Custom Includes
#include "Statement.h"
#include "Declaration.h"

class CParseUserFunction  
{
	public:
		CParseUserFunction();
		virtual ~CParseUserFunction();

	public:
		void			SetName(CStr *pName) { m_pName = pName; }
		void			SetParamMax(DWORD dwPMax) { m_dwParamMax = dwPMax; }
		void			SetBlock(CStatement *pStatement) { m_pCodeBlock = pStatement; }
		void			SetResultParameter(CParameter *pParam) { m_pParameter = pParam; }
		void			SetDecChainRef(CDeclaration *pDecRef) { m_pDecChainRef = pDecRef; }
		
		CStr*			GetName(void) { return m_pName; }
		DWORD			GetParamMax(void) { return m_dwParamMax; }
		CStatement*		GetBlock(void) { return m_pCodeBlock; }
		CParameter*		GetResultParameter(void) { return m_pParameter; }
		CDeclaration*	GetDecChainRef(void) { return m_pDecChainRef; }

		void			SetStartLineNumber(DWORD line) { m_dwStartLineNumber = line; }
		DWORD			GetStartLineNumber(void) { return m_dwStartLineNumber; }
		void			SetEndLineNumber(DWORD line) { m_dwEndLineNumber = line; }
		DWORD			GetEndLineNumber(void) { return m_dwEndLineNumber; }

		bool			ActOnSingleVar(DWORD dwType, int iDisplacement, DWORD PlacementCode, CStr* pDoNotFree, bool bSpecialRecreate);
		bool			ActOnType(CStr* pTypeName, int iDisplacement, DWORD PlacementCode, CStr* pDoNotFree, bool bSpecialRecreate);
		bool			ActOnLocalVars(DWORD PlacementCode, CStr* pDoNotFree);

		bool			WriteDBM(DWORD PlacementCode);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult);

	private:

		// Debug Data
		DWORD			m_dwStartLineNumber;
		DWORD			m_dwEndLineNumber;

		// User Function Data
		CStr*			m_pName;
		DWORD			m_dwParamMax;
		CStatement*		m_pCodeBlock;
		CParameter*		m_pParameter;

		// Reference Pointer Only
		CDeclaration*	m_pDecChainRef;
};

#endif // !defined(AFX_PARSEUSERFUNCTION_H__7FF9540A_8B4A_4B3A_B490_A2E4549AD774__INCLUDED_)
