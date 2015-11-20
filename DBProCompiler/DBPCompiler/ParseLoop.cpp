// ParseLoop.cpp: implementation of the CParseLoop class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"
#include "ParseLoop.h"

#include "ParseInstruction.h"
#include "InstructionTable.h"
#include "DBPCompiler.h"

// Externals to assist in math call
extern CDBPCompiler* g_pDBPCompiler;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseLoop::CParseLoop()
{
	m_dwStartLineNumber=0;
	m_dwEndLineNumber=0;

	m_dwLoopType=0;
	m_pCodeBlock=NULL;
	m_pParameter=NULL;
	m_pStartLabelParameter=NULL;
	m_pEndLabelParameter=NULL;

	m_pMiddleLabelParameter=NULL;
	m_pForNextInitParameter=NULL;
	m_pForNextIncParameter=NULL;
	m_pForNextCheckParameter=NULL;
}

CParseLoop::~CParseLoop()
{
	// Statements Deleted One by One
	m_pCodeBlock->Free();
	m_pCodeBlock=NULL;

	SAFE_DELETE(m_pParameter);
	SAFE_DELETE(m_pStartLabelParameter);
	SAFE_DELETE(m_pEndLabelParameter);

	SAFE_DELETE(m_pMiddleLabelParameter);
	SAFE_DELETE(m_pForNextInitParameter);
	SAFE_DELETE(m_pForNextIncParameter);
	SAFE_DELETE(m_pForNextCheckParameter);
}

bool CParseLoop::WriteDBM(DWORD PlacementCode)
{
	// Determine if parameters used
	CParameter* pParameter = NULL;
	if(PlacementCode==DBMPLACEMENT_TOP && m_dwLoopType==LOOPTYPE_WHILE) pParameter=GetConditionParameter();
	if(PlacementCode==DBMPLACEMENT_BOTTOM && m_dwLoopType==LOOPTYPE_REPEAT) pParameter=GetConditionParameter();
	if(PlacementCode==DBMPLACEMENT_TOP && m_dwLoopType==LOOPTYPE_FORNEXT) pParameter=GetConditionParameter();

	// All Loops have a start/end-loop label
	LPSTR pSLabelStr = GetSLabelParameter()->GetMathItem()->GetResultStringToken()->GetStr();
	LPSTR pELabelStr = GetELabelParameter()->GetMathItem()->GetResultStringToken()->GetStr();

	// Line Responsible
	DWORD dwLineToReport = GetStartLineNumber();
	if(PlacementCode==DBMPLACEMENT_BOTTOM) dwLineToReport = GetEndLineNumber();

	// All Loops Require a Process Messages Call (or freeze will occur)
	// [But only loops that are potentially infinite by nature = suggested safe mode]
	if(PlacementCode==DBMPLACEMENT_BOTTOM && m_dwLoopType!=LOOPTYPE_FORNEXT)
	{
		// only if safety code allowed
		if ( g_pDBPCompiler->m_bRemoveSafetyCode==false || m_dwLoopType==LOOPTYPE_DO )
		{
			CParseInstruction* pTemp = new CParseInstruction;
			pTemp->SetLineNumber(m_dwStartLineNumber);
			pTemp->PassStartEndCharForPossibleDebugHook(m_dwS, m_dwE);
			pTemp->WriteDBMHardCode(BUILD_SYNC, NULL, NULL, NULL);
			SAFE_DELETE(pTemp);
		}
	}

	// Process Loop Code
	if(m_dwLoopType==LOOPTYPE_FORNEXT)
	{
		// ForNext has code, rather than simple condition check
		if(PlacementCode==DBMPLACEMENT_TOP)
		{
			// Process Init Code in FORNEXT Init Param
			if(GetForNextInitParameter()) GetForNextInitParameter()->WriteDBM();

			// Process Inc Code in FORNEXT Inc Param
			if(GetForNextIncParameter()) GetForNextIncParameter()->WriteDBM();

			// Process Check Code in FORNEXT Check Param
			if(GetForNextCheckParameter())
			{
				// Set the CMP Instruction for the Condition
				GetForNextCheckParameter()->WriteDBM();
				g_pASMWriter->WriteASMTaskP1(dwLineToReport, ASMTASK_CONDITION, GetForNextCheckParameter()->GetMathItem()->FindResultData());

				// Set the JE Instruction for the Condition
				CStr* pData = new CStr(pELabelStr);
				g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_CONDJUMPE, pData, 10);
				SAFE_DELETE(pData);
			}
		}
		if(PlacementCode==DBMPLACEMENT_BOTTOM)
		{
			// Set the JMP Instruction for the Condition
			CStr* pData = new CStr(pSLabelStr);
			g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_JUMP, pData, 10);
			SAFE_DELETE(pData);
		}
	}
	else
	{
		// Use parameters if valid
		if(pParameter)
		{
			// Write out parameter traversals
			pParameter->WriteDBM();

			// Set the CMP Instruction for the Condition
			g_pASMWriter->WriteASMTaskP1(dwLineToReport, ASMTASK_CONDITION, pParameter->GetMathItem()->FindResultData());

			// Set the JE Instruction for the Condition
			if(PlacementCode==DBMPLACEMENT_TOP)
			{
				CStr* pData = new CStr(pELabelStr);
				if ( g_pASMWriter->GetCondToggle() )
				{
					g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_CONDJUMPNE, pData, 10);
					g_pASMWriter->SetCondToggle(false);
				}
				else
					g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_CONDJUMPE, pData, 10);
				SAFE_DELETE(pData);
			}
			if(PlacementCode==DBMPLACEMENT_BOTTOM)
			{
				CStr* pData = new CStr(pSLabelStr);
				if ( g_pASMWriter->GetCondToggle() )
				{
					g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_CONDJUMPNE, pData, 10);
					g_pASMWriter->SetCondToggle(false);
				}
				else
					g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_CONDJUMPE, pData, 10);
				SAFE_DELETE(pData);
			}
		}
		else
		{
			// Loop Instruction (no param)
			if(PlacementCode==DBMPLACEMENT_BOTTOM)
			{
				// Set the JMP Instruction
				CStr* pData = new CStr(pSLabelStr);
				g_pASMWriter->WriteASMTaskCoreP1(dwLineToReport, ASMTASK_JUMP, pData, 10);
				SAFE_DELETE(pData);
			}		
		}
	}
	return true;
}

bool CParseLoop::WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pText);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pResult);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}
