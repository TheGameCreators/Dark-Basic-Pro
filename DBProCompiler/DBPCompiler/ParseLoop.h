// ParseLoop.h: interface for the CParseLoop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSELOOP_H__750474B4_5487_4E6E_B8F7_8639AC8BF24B__INCLUDED_)
#define AFX_PARSELOOP_H__750474B4_5487_4E6E_B8F7_8639AC8BF24B__INCLUDED_
#include "ParserHeader.h"

// Custom Includes
#include "Statement.h"

// Defines
#define LOOPTYPE_DO				1
#define LOOPTYPE_WHILE			2
#define LOOPTYPE_REPEAT			3
#define LOOPTYPE_FORNEXT		4

class CParseLoop  
{
	public:
		CParseLoop();
		virtual ~CParseLoop();

	public:
		void			SetType(DWORD dwType) { m_dwLoopType = dwType; }
		void			SetBlock(CStatement *pStatement) { m_pCodeBlock = pStatement; }
		void			SetConditionParameter(CParameter *pParam) { m_pParameter = pParam; }
		void			SetSLabelParameter(CParameter *pParam) { m_pStartLabelParameter = pParam; }
		void			SetELabelParameter(CParameter *pParam) { m_pEndLabelParameter = pParam; }
		
		void			SetForNextInitParameter(CParameter *pParam) { m_pForNextInitParameter = pParam; }
		void			SetForNextIncParameter(CParameter *pParam) { m_pForNextIncParameter = pParam; }
		void			SetForNextCheckParameter(CParameter *pParam) { m_pForNextCheckParameter = pParam; }

		CStatement*		GetBlock(void) { return m_pCodeBlock; }
		CParameter*		GetConditionParameter(void) { return m_pParameter; }
		CParameter*		GetSLabelParameter(void) { return m_pStartLabelParameter; }
		CParameter*		GetELabelParameter(void) { return m_pEndLabelParameter; }

		CParameter*		GetForNextInitParameter(void) { return m_pForNextInitParameter; }
		CParameter*		GetForNextIncParameter(void) { return m_pForNextIncParameter; }
		CParameter*		GetForNextCheckParameter(void) { return m_pForNextCheckParameter; }

		void			SetStartLineNumber(DWORD line) { m_dwStartLineNumber = line; }
		DWORD			GetStartLineNumber(void) { return m_dwStartLineNumber; }
		void			SetEndLineNumber(DWORD line) { m_dwEndLineNumber = line; }
		DWORD			GetEndLineNumber(void) { return m_dwEndLineNumber; }

		void			PassStartEndCharForPossibleDebugHook(DWORD dwS, DWORD dwE) { m_dwS=dwS, m_dwE=dwE; }

		bool			WriteDBM(DWORD PlacementCode);
		bool			WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult);

	private:

		// Debug Data
		DWORD			m_dwStartLineNumber;
		DWORD			m_dwEndLineNumber;
		DWORD			m_dwS;
		DWORD			m_dwE;

		// Loop Data
		DWORD			m_dwLoopType;
		CStatement*		m_pCodeBlock;
		CParameter*		m_pParameter;
		CParameter*		m_pStartLabelParameter;
		CParameter*		m_pEndLabelParameter;

		// Additional FORNEXT Data
		CParameter*		m_pMiddleLabelParameter;
		CParameter*		m_pForNextInitParameter;
		CParameter*		m_pForNextIncParameter;
		CParameter*		m_pForNextCheckParameter;
};

#endif // !defined(AFX_PARSELOOP_H__750474B4_5487_4E6E_B8F7_8639AC8BF24B__INCLUDED_)
