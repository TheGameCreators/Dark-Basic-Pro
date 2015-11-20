// StatementList.h: interface for the CStatementList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATEMENTLIST_H__44BDB5FD_62E4_4B69_8950_E09A989E3475__INCLUDED_)
#define AFX_STATEMENTLIST_H__44BDB5FD_62E4_4B69_8950_E09A989E3475__INCLUDED_

#include <DB3Array.h>

#include "Statement.h"
#include "Declaration.h"
#include "InstructionTableEntry.h"

class CStatementList  
{
	public:
		CStatementList();
		virtual ~CStatementList();
	
	public:
		bool			UnfoldIncludes(LPSTR* ppData, DWORD* pSize); 
		bool			MakeStatements(LPSTR pData, DWORD Size);
		bool			AddMiniStatements(LPSTR pData, DWORD Size);

		CStatement*		GetPreScanStatements(void) { return m_pPreScanStatements; }
		CStatement*		GetProgramStatements(void) { return m_pProgramStatements; }
		CStatement*		GetMiniStatements(void) { return m_pMiniStatements; }
		void			ResetParserPointers(void);

		void			SetFileDataPointer(LPSTR pData) { m_pFileDataPointer=pData; }
		void			IncFileDataPointer(void) { if(m_pFileDataPointer<m_pFileDataEnd) m_pFileDataPointer++; }
		LPSTR			GetFileDataPointer(void) { return m_pFileDataPointer; }
		LPSTR			GetFileDataEnd(void) { return m_pFileDataEnd; }
		LPSTR			GetFileDataStart(void) { return m_pFileData; }

		void			UpdateLineDBMData(CStatement* pStatementAt);
		void			SetLineNumber(DWORD line);
		DWORD			GetLineNumber(void) { return m_dwLineNumber; }
		void			IncLineNumber(void);

		void			SetLastCharInDataPosition(DWORD dwCharPos) { m_dwLastCharInDataPosition=dwCharPos; }
		DWORD			GetLastCharInDataPosition(void) { return m_dwLastCharInDataPosition; }
		void			SetLastStampedCharInDataPosition(void) { m_dwLastStampedCharInDataPosition=m_dwLastCharInDataPosition; }
		DWORD			GetLastStampedCharInDataPosition(void) { return m_dwLastStampedCharInDataPosition; }
		
		void			SetTokenLineNumber(DWORD line) { m_dwTokenLineNumber=line; }
		DWORD			GetTokenLineNumber(void) { return m_dwTokenLineNumber; }
		void			SetTempVarIndex(DWORD index) { m_dwTempVariableIndex=index; }
		DWORD			GetTempVarIndex(void) { return m_dwTempVariableIndex; }
		void			IncTempVarIndex(void) { m_dwTempVariableIndex++; }

		void			SetVarOffsetCounter(DWORD value) { m_dwVarOffsetCounter=value; }
		DWORD			GetVarOffsetCounter(void) { return m_dwVarOffsetCounter; }
		void			IncVarOffsetCounter(DWORD size) { m_dwVarOffsetCounter+=size; }

		void			SetDataIndexCounter(DWORD value) { m_dwDataIndexCounter=value; }
		DWORD			GetDataIndexCounter(void) { return m_dwDataIndexCounter; }
		void			IncDataIndexCounter(DWORD size) { m_dwDataIndexCounter+=size; }
		
		void			SetStringIndexCounter(DWORD value) { m_dwStringIndexCounter=value; }
		DWORD			GetStringIndexCounter(void) { return m_dwStringIndexCounter; }
		void			IncStringIndexCounter(DWORD size) { m_dwStringIndexCounter+=size; }

		void			SetDLLIndexCounter(DWORD value) { m_dwDLLIndexCounter=value; }
		DWORD			GetDLLIndexCounter(void) { return m_dwDLLIndexCounter; }
		void			IncDLLIndexCounter(DWORD size) { m_dwDLLIndexCounter+=size; }

		void			SetCommandIndexCounter(DWORD value) { m_dwCommandIndexCounter=value; }
		DWORD			GetCommandIndexCounter(void) { return m_dwCommandIndexCounter; }
		void			IncCommandIndexCounter(DWORD size) { m_dwCommandIndexCounter+=size; }

		void			SetLabelIndexCounter(DWORD value) { m_dwLabelIndexCounter=value; }
		DWORD			GetLabelIndexCounter(void) { return m_dwLabelIndexCounter; }
		void			IncLabelIndexCounter(DWORD size) { m_dwLabelIndexCounter+=size; }

		void			SetVarQtyCounter(DWORD value) { m_dwVarQtyCounter=value; }
		DWORD			GetVarQtyCounter(void) { return m_dwVarQtyCounter; }
		void			IncVarQtyCounter(DWORD size) { m_dwVarQtyCounter+=size; }
		void			SetLabelQtyCounter(DWORD value) { m_dwLabelQtyCounter=value; }
		DWORD			GetLabelQtyCounter(void) { return m_dwLabelQtyCounter; }
		void			IncLabelQtyCounter(DWORD size) { m_dwLabelQtyCounter+=size; }

		void			SetUserFunctionName(LPSTR pUFName) { m_pCurrentUserFunctionName.SetText(pUFName); }
		LPSTR			GetUserFunctionName(void) { return m_pCurrentUserFunctionName.GetStr(); }

		void			SetUserFunctionDecChain(CDeclaration* pDec) { m_pCurrentUserFunctionDecChain=pDec; }
		CDeclaration*	GetUserFunctionDecChain(void) { return m_pCurrentUserFunctionDecChain; }

		void			SetInstructionRef(CInstructionTableEntry* pRef) { m_pInstructionRef=pRef; }
		CInstructionTableEntry* GetInstructionRef(void) { return m_pInstructionRef; }
		void			SetInstructionType(DWORD type) { m_dwInstructionType=type; }
		DWORD			GetInstructionType(void) { return m_dwInstructionType; }
		void			SetInstructionValue(DWORD value) { m_dwInstructionValue=value; }
		DWORD			GetInstructionValue(void) { return m_dwInstructionValue; }
		void			SetInstructionParamMax(DWORD max) { m_dwInstructionParamMax=max; }
		DWORD			GetInstructionParamMax(void) { return m_dwInstructionParamMax; }

		void			SetVariableAddParse(bool bState) { m_bParseVariableAdds = bState; }
		bool			GetVariableAddParse(void) { return m_bParseVariableAdds; }
		void			SetImplementationParse(bool bState) { m_bParseImplementation = bState; }
		bool			GetImplementationParse(void) { return m_bParseImplementation; }
		void			SetDisableParsingToCR(bool bState) { m_bDisableParsingToCR = bState; }
		bool			GetDisableParsingToCR(void) { return m_bDisableParsingToCR; }
		void			SetDisableParsingFull(bool bState) { m_bDisableParsingFull = bState; }
		bool			GetDisableParsingFull(void) { return m_bDisableParsingFull; }
		void			SetAllowLabelAsValue(bool bState) { m_bPermitLabelAsValue = bState; }
		bool			GetAllowLabelAsValue(void) { return m_bPermitLabelAsValue; }
		void			SetLocalVarUsageAsGlobal(bool bState) { m_bLocalVarUsage = bState; }
		bool			GetLocalVarUsageAsGlobal(void) { return m_bLocalVarUsage; }
		
		void			SetLastLine(DWORD line) { m_dwLastLineNumber=line; }
		DWORD			GetLastLine(void) { return m_dwLastLineNumber; }

		void			SetLatestLoopExitLabel(CParameter* pLabel) { m_pLatestLoopExitLabelRef=pLabel; }
		CParameter*		GetLatestLoopExitLabel(void) { return m_pLatestLoopExitLabelRef; }

		bool			FindStartOfFileDataProgramLine(DWORD dwFindLineNumber, LPSTR* pReturnText);
		size_t			GetLineText(DWORD dwLineNumber, char *pDst, size_t DstLen);

		void			SetWriteStarted(bool bState) { m_bWriteStarted=bState; }
		bool			GetWriteStarted(void) { return m_bWriteStarted; }
		CStatement*		GetRefStatement(void) { return m_pRefStatementDuringWrite; }
		bool			WriteDBM(void);

	public:
		int				m_iNestCount;

	private:

		// Original DBA FileData
		LPSTR			m_pFileData;
		LPSTR			m_pFileDataEnd;
		LPSTR			m_pFileDataPointer;
		DWORD			m_FileDataSize;

		// Tracking Data
		DWORD			m_dwLastCharInDataPosition;
		DWORD			m_dwLastStampedCharInDataPosition;
		DWORD			m_dwTokenLineNumber;
		DWORD			m_dwLineNumber;
		DWORD			m_dwTempVariableIndex;

		// Variable Creation Tracking
		DWORD			m_dwVarOffsetCounter;
		CStr			m_pCurrentUserFunctionName;
		CDeclaration*	m_pCurrentUserFunctionDecChain;

		// Data/String Creation Tracking
		DWORD			m_dwDataIndexCounter;
		DWORD			m_dwStringIndexCounter;

		// DLL/Command Creation Tracking
		DWORD			m_dwDLLIndexCounter;
		DWORD			m_dwCommandIndexCounter;

		// Label INTERNAL Creation Tracking
		DWORD			m_dwLabelIndexCounter;

		// Label TOTAL Creation Tracking
		DWORD			m_dwLabelQtyCounter;

		// Variable Creation Tracking
		DWORD			m_dwVarQtyCounter;

		// Instruction Workdata
		CInstructionTableEntry* m_pInstructionRef;
		DWORD			m_dwInstructionType;
		DWORD			m_dwInstructionValue;
		DWORD			m_dwInstructionParamMax;

		// Parsing Control
		bool			m_bParseVariableAdds;
		bool			m_bParseImplementation;
		bool			m_bDisableParsingToCR;
		bool			m_bDisableParsingFull;
		bool			m_bPermitLabelAsValue;
		bool			m_bLocalVarUsage;

		CStatement*		m_pRefStatementDuringWrite;
		bool			m_bWriteStarted;

		// Global Used When building Label Byte Offsets (for nonlinenumstatements)
		DWORD			m_dwLastLineNumber;

		// Track Loop Parsing (so EXIT knows the jump-to label)
		CParameter*		m_pLatestLoopExitLabelRef;

		// Start of Program Block
		CStatement*		m_pPreScanStatements;
		CStatement*		m_pProgramStatements;

		// MiniProgram Block
		CStatement*		m_pMiniStatements;

		// Track all lines
		db3::TArray<char *> m_LinePtrs;
};

#endif // !defined(AFX_STATEMENTLIST_H__44BDB5FD_62E4_4B69_8950_E09A989E3475__INCLUDED_)
