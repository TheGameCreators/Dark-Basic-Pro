// ParseInit.h: interface for the CParseInit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEINIT_H__5AE26014_3544_4337_9662_4B439E57C853__INCLUDED_)
#define AFX_PARSEINIT_H__5AE26014_3544_4337_9662_4B439E57C853__INCLUDED_

// Common Includes
#include "windows.h"

// Custom Includes
#include "Statement.h"

class CParseInit  
{
	public:
		CParseInit();
		virtual ~CParseInit();

		void			SetLineNumber(DWORD line) { m_dwLineNumber = line; }
		DWORD			GetLineNumber(void) { return m_dwLineNumber; }

		void			SetVariableNameMathOp(CMathOp* pNameMath) { m_pMathOp=pNameMath; }
		CMathOp*		GetVariableNameMathOp(void) { return m_pMathOp; }
		void			SetVariableParamList(CParameter* pParam) { m_pDataParamList=pParam; }
		CParameter*		GetParameter(void) { return m_pDataParamList; }

		bool			WriteDBM(void);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText);

	private:

		// Debug Data
		DWORD			m_dwLineNumber;

		// Initialisation Data
		CMathOp*		m_pMathOp;
		CParameter*		m_pDataParamList;
};

#endif // !defined(AFX_PARSEINIT_H__5AE26014_3544_4337_9662_4B439E57C853__INCLUDED_)
