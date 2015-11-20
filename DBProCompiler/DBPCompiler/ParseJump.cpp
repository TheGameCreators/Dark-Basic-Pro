// ParseJump.cpp: implementation of the CParseJump class.
//
//////////////////////////////////////////////////////////////////////

#include "ParseJump.h"
#include "ParseInstruction.h"
#include "InstructionTable.h"
#include "DBPCompiler.h"

// Externals to assist in math call
extern CInstructionTable* g_pInstructionTable;
extern CDBPCompiler* g_pDBPCompiler;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseJump::CParseJump()
{
	m_dwStartLineNumber=0;
	m_dwMiddleLineNumber=0;
	m_dwEndLineNumber=0;

	m_dwJumpType=0;
	m_pCodeBlockA=NULL;
	m_pCodeBlockLabelA=NULL;
	m_pCodeBlockB=NULL;
	m_pCodeBlockLabelB=NULL;
	m_pParameter=NULL;
	m_pCaseParameter=NULL;
	m_pLabelParameter=NULL;
	m_pExitLabelParameterRef=NULL;//ref only

	m_pCodeBlockChain=NULL;
}

CParseJump::~CParseJump()
{
	// Statements Deleted One by One
	m_pCodeBlockA->Free();
	m_pCodeBlockA=NULL;

	// Statements Deleted One by One
	m_pCodeBlockB->Free();
	m_pCodeBlockB=NULL;

	SAFE_DELETE(m_pCodeBlockLabelA);
	SAFE_DELETE(m_pCodeBlockLabelB);
	SAFE_DELETE(m_pParameter);
	SAFE_DELETE(m_pCaseParameter);
	SAFE_DELETE(m_pLabelParameter);

	SAFE_DELETE(m_pCodeBlockChain);
}

bool CParseJump::WriteDBM(DWORD PlacementCode)
{
	if(GetJumpType()==JUMPTYPE_IF)
	{
		// IF Statement
		// Use parameters if valid
		if(m_pParameter)
		{
			// Write out parameter traversals
			m_pParameter->WriteDBM();

			// Set the CMP Instruction for the Condition
			g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_CONDITION, m_pParameter->GetMathItem()->FindResultData());

			// Set the JE Instruction for the Condition
			CStr* pJumpToLabel=GetBlockLabelB();
			if(pJumpToLabel==NULL) pJumpToLabel=GetBlockLabelA();
			if ( g_pASMWriter->GetCondToggle() )
			{
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CONDJUMPNE, pJumpToLabel, 10);
				g_pASMWriter->SetCondToggle(false);
			}
			else
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CONDJUMPE, pJumpToLabel, 10);
		}
		if(GetBlockA())
		{
			// BLOCK A
			GetBlockA()->WriteDBM();

			// Jump if ELSE
			if(GetBlockB())
			{
				// Set the JMP Instruction for the Condition
				CStr* pJumpToLabel=GetBlockLabelA();
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_JUMP, pJumpToLabel, 10);
			}
		}
		if(GetBlockB())
		{
			// BLOCK B
			GetBlockB()->WriteDBM();
		}
	}
	if(GetJumpType()==JUMPTYPE_GOTO)
	{
		// LEEFIX - Nono - 111002 - GOT is dangerous and fast - fast main loop option!!
		// LEEFIX - 101102 - Add Compiler Switch STABLE/FAST option
		if(g_pDBPCompiler->GetSpeedOverStabilityFlag()==false)
		{
			// As GOTO can be used to bypass a loop (causing freezes), insert a message call
			if ( g_pDBPCompiler->m_bRemoveSafetyCode==false )
			{
				CParseInstruction* pTemp = new CParseInstruction;
				pTemp->SetLineNumber(GetStartLineNumber());
				pTemp->PassStartEndCharForPossibleDebugHook(0, 0);
				pTemp->WriteDBMHardCode(BUILD_SYNC, NULL, NULL, NULL);
				SAFE_DELETE(pTemp);
			}
		}

		// Get Label to Jump To
		LPSTR pLabelStr = NULL;
		if(GetExitLabelRefParameterRef())
			pLabelStr = GetExitLabelRefParameterRef()->GetMathItem()->GetResultStringToken()->GetStr();
		else
			pLabelStr = m_pParameter->GetMathItem()->GetResultStringToken()->GetStr();

		// Set the JMP Instruction
		CStr* pData = new CStr(pLabelStr);
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_JUMP, pData, 10);
		SAFE_DELETE(pData);

	}
	if(GetJumpType()==JUMPTYPE_GOSUB)
	{
		// Get Label to Jump To
		LPSTR pLabelStr = m_pParameter->GetMathItem()->GetResultStringToken()->GetStr();

		// Set the JMP Instruction
		CStr* pData = new CStr(pLabelStr);
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_JUMPSUBROUTINE, pData, 10);
		SAFE_DELETE(pData);
	}
	if(GetJumpType()==JUMPTYPE_SELECT)
	{
		// SELECT Statement
		m_pParameter->WriteDBM();

		// Create Condition Jump Chain
		CParameter* pCaseLabel=m_pLabelParameter;
		CParameter* pCaseCondition=m_pCaseParameter;

		// If string select, use string comparison chain
		// LEEFIX - 201102 - Added support for 103 which is a sting 'array'! && GetResultType to FindResultTypeValueForDBM
		if(m_pParameter->GetMathItem()->FindResultTypeValueForDBM()==3
		|| m_pParameter->GetMathItem()->FindResultTypeValueForDBM()==103)
		{
			// Compares strings against string pointed to by m_pParameter
			while(pCaseCondition)
			{
				// Push Strings to stack
				// LEEFIX - 081102 - SELECT in Functions did not resolve from decorated function-var-name
//				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_PUSH, m_pParameter->GetMathItem()->GetResultData());
				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_PUSH, m_pParameter->GetMathItem()->FindResultData());
				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_PUSH, pCaseCondition->GetMathItem()->FindResultData());

				// CALL String Comparison, result in EAX
				CInstructionTableEntry* pRef=g_pInstructionTable->GetRef(IT_INTERNAL_EQUALLSS);
				LPSTR pMathCommand=pRef->GetDecoratedName()->GetStr();
				g_pASMWriter->WriteASMCall(GetStartLineNumber(), "dbprocore.dll", pMathCommand);

				// Free stack
				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_POPEBX, NULL);
				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_POPEBX, NULL);

				// If EAX is one, jump to the label colding the case code
				CStr* pOne = new CStr("1");
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CONDITIONDATA, pOne, 7);
				CStr* pJumpToLabel=pCaseLabel->GetMathItem()->FindResultStringTokenForDBM();
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CONDJUMPE, pJumpToLabel, 10);
				SAFE_DELETE(pOne);

				// Next in chain
				pCaseCondition=pCaseCondition->GetNext();
				pCaseLabel=pCaseLabel->GetNext();
			}
		}
		else
		{
			// Move Variable into EAX
			g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_ASSIGNTOEAX, m_pParameter->GetMathItem()->FindResultData());

			// Compares numeric values against EAX
			while(pCaseCondition)
			{
				// Set the CMP Instruction for the Condition
				g_pASMWriter->WriteASMTaskP1(GetStartLineNumber(), ASMTASK_CONDITIONDATA, pCaseCondition->GetMathItem()->FindResultData());

				// Set the JE Instruction for the Condition
				CStr* pJumpToLabel=pCaseLabel->GetMathItem()->FindResultStringTokenForDBM();
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CONDJUMPE, pJumpToLabel, 10);

				// Next in chain
				pCaseCondition=pCaseCondition->GetNext();
				pCaseLabel=pCaseLabel->GetNext();
			}
		}

		// Add case default block if present
		if(GetBlockB())
		{
			// Write Out Case Default Block
			GetBlockB()->WriteDBM();
		}

		// Set the JMP Instruction to skip all case code
		CStr* pJumpToLabel=GetBlockLabelA();
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_JUMP, pJumpToLabel, 10);

		// Write Out Case Blocks
		CStatementChain* pStatementBlock = GetBlockChain();
		while(pStatementBlock)
		{
			// Get Statement
			CStatement* pStatementRef = pStatementBlock->GetStatementBlock();

			// Write out block
			pStatementRef->WriteDBM();

			// Set the JMP Instruction to skip all case code
			CStr* pJumpToLabel=GetBlockLabelA();
			g_pASMWriter->WriteASMTaskCoreP1(pStatementRef->GetLineNumber(), ASMTASK_JUMP, pJumpToLabel, 10);

			// Next in chain
			pStatementBlock=pStatementBlock->GetNext();
		}
	}
	return true;
}

bool CParseJump::WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult)
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
