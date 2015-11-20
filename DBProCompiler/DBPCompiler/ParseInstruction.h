// ParseInstruction.h: interface for the CParseInstruction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEINSTRUCTION_H__998BC1A9_749D_4B46_8D74_49608BD44224__INCLUDED_)
#define AFX_PARSEINSTRUCTION_H__998BC1A9_749D_4B46_8D74_49608BD44224__INCLUDED_

// Common Includes
#include "windows.h"

// Custom Includes
#include "Statement.h"
#include "InstructionTableEntry.h"

class CParseInstruction  
{
	public:
		CParseInstruction();
		virtual ~CParseInstruction();

		void				SetType(DWORD dwType) { m_dwInstructionType = dwType; }
		void				SetValue(DWORD dwValue) { m_dwInstructionValue = dwValue; }
		void				SetParamMax(DWORD dwMax) { m_dwInstructionParamMax = dwMax; }
		void				SetReturnParameter(CStr* pReturn) { m_pReturnParam = pReturn; }
		void				SetLabelParam(CStr* pParam) { m_pLabelParameter = pParam; }

		void				SetLineNumber(DWORD line) { m_dwLineNumber = line; }
		DWORD				GetLineNumber(void) { return m_dwLineNumber; }

		void				SetParameter(CParameter* pParam) { m_pParameter=pParam; }
		CParameter*			GetParameter(void) { return m_pParameter; }
		CStr*				GetReturnParameter(void) { return m_pReturnParam; }
		CStr*				GetLabelParam(void) { return m_pLabelParameter; }

		void				SetInstructionRef(CInstructionTableEntry* pRef) { m_pRefInstructionEntry=pRef; }
		CInstructionTableEntry* GetInstructionRef(void) { return m_pRefInstructionEntry; }

		void				PassStartEndCharForPossibleDebugHook(DWORD dwS, DWORD dwE) { m_dwS=dwS; m_dwS=dwE; }

		bool				ActOnSingleVar(CResultData* pVar, DWORD dwType, int iDisplacement);
		bool				ActOnType(CResultData* pVar, CStr* pTypeName, int iDisplacement);
		bool				ActOnLocalVars(CResultData* pVar);

		bool				WriteDBM(void);
		bool				WriteDBMBit(void);
		bool				WriteDBMHardCode(DWORD dwBuildID, CResultData* pP1, CResultData* pP2, CResultData* pP3);

		bool				WriteASMTask(DWORD dwLine, DWORD dwTask, CStr* pP1, CStr* pP2);
		bool				WriteASMLine(DWORD dwOp, LPSTR pOpStr, CStr* pParam1, CStr* pParam2);

	private:

		// Debug Data
		DWORD						m_dwLineNumber;
		DWORD						m_dwS;
		DWORD						m_dwE;
	
		// Instruction Data
		DWORD						m_dwInstructionType;
		DWORD						m_dwInstructionValue;
		DWORD						m_dwInstructionParamMax;
		CParameter*					m_pParameter;
		CStr*						m_pLabelParameter;
		CStr*						m_pReturnParam;

		CInstructionTableEntry*		m_pRefInstructionEntry;
};

#endif // !defined(AFX_PARSEINSTRUCTION_H__998BC1A9_749D_4B46_8D74_49608BD44224__INCLUDED_)
