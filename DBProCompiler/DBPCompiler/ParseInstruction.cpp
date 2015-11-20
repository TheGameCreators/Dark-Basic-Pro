// ParseInstruction.cpp: implementation of the CParseInstruction class.
//
//////////////////////////////////////////////////////////////////////

// Includes
#include "VarTable.h"
#include "StructTable.h"
#include "InstructionTable.h"
#include "InstructionTableEntry.h"
#include "ParserHeader.h"
#include "ParseInstruction.h"
#include "ParseuserFunction.h"
#include "ASMWriter.h"
#include "DebugInfo.h"
#include "DBPCompiler.h"

// External class pointers
extern CVarTable* g_pVarTable;
extern CStructTable* g_pStructTable;
extern CInstructionTable* g_pInstructionTable;
extern CDebugInfo g_DebugInfo;
extern CDBPCompiler* g_pDBPCompiler;

// External Scope Ptrs
extern CParseUserFunction* g_pUserFunctionWithin;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseInstruction::CParseInstruction()
{
	m_dwLineNumber=0;
	m_dwInstructionType=0;
	m_dwInstructionValue=0;
	m_dwInstructionParamMax=0;
	m_pParameter=NULL;
	m_pReturnParam=NULL;
	m_pLabelParameter=NULL;

	// Reference Only
	m_pRefInstructionEntry=NULL;
}

CParseInstruction::~CParseInstruction()
{
	SAFE_DELETE(m_pParameter);
	SAFE_DELETE(m_pReturnParam);
	SAFE_DELETE(m_pLabelParameter);
}

bool CParseInstruction::ActOnSingleVar ( CResultData* pVar, DWORD dwType, int iDisplacement)
{
	// Only strings
	if(dwType==3)
	{
		// Write offset out
		CStr* pData = new CStr("");
		pData->AddNumericText(iDisplacement);

		// Local Var - not part of param-in data
		CStr* pNull = new CStr("0");

		// Determine natural mode, and make sure its an offset to get at the UDT element
		DWORD dwAccessMode = g_pASMWriter->DetMode(pVar->m_pStringToken, pVar->m_dwType, pVar->m_dwDataOffset);
		if ( dwAccessMode==PMODE_MEM ) dwAccessMode=PMODE_MEMOFF;
		if ( dwAccessMode==PMODE_EBP ) dwAccessMode=PMODE_EBPOFF;

		// PUSH STRING FROM UDT TO STACK
		g_pASMWriter->WriteASMXtoEAX(dwAccessMode, pVar->m_pStringToken, pVar->m_pAdditionalOffset, 3, iDisplacement);
		g_pASMWriter->WriteASMEAXtoX(PMODE_STACK, NULL, NULL, 3, iDisplacement);
		g_pASMWriter->WriteASMComment("PUSH TO STACK", "", "", "");

		// Pass DEST + CURRENT STRING (same address)
		g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSH, pNull, 7);
		SAFE_DELETE(pNull);

		// CALL EQUATE to create a NEW STRING from CURRENT STRING
		g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?EquateSS@@YAKKK@Z");

		// Put EAX overwrites DEST
		g_pASMWriter->WriteASMEAXtoX(dwAccessMode, pVar->m_pStringToken, pVar->m_pAdditionalOffset, 3, iDisplacement);
		g_pASMWriter->WriteASMComment("ASSIGN EAX TO X", "", "", "");

		// Pop param data
		g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);
		g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);

		// Free usages
		SAFE_DELETE(pNull);
		SAFE_DELETE(pData);
	}

	// Complete
	return true;
}

bool CParseInstruction::ActOnType ( CResultData* pVar, CStr* pTypeName, int iDisplacement)
{
	int iGlobalDisplacement = iDisplacement;
	CStructTable* pStruct = g_pStructTable->DoesTypeEvenExist(pTypeName->GetStr());
	if(pStruct)
	{
		CDeclaration* pCurrent = pStruct->GetDecChain();
		while(pCurrent)
		{
			// Determine local func-mem from local var name
			DWORD dwOffset=0, dwSizeOfData=0;
			if(g_pStructTable->FindOffsetFromField(pTypeName->GetStr(), pCurrent->GetName()->GetStr(), &dwOffset, &dwSizeOfData))
			{
				// Determine type of Field and Offset
				DWORD dwFieldType = g_pVarTable->GetBasicTypeValue(pCurrent->GetType()->GetStr());
				int iDisplacement = (iGlobalDisplacement + dwOffset);
		
				// Process Element
				if(dwFieldType==1001)
					ActOnType(pVar, pCurrent->GetType(), iDisplacement);
				else
					ActOnSingleVar(pVar, dwFieldType, iDisplacement);
			}

			// Next declaration
			pCurrent = pCurrent->GetNext();
		}
	}

	// Complete
	return true;
}

bool CParseInstruction::ActOnLocalVars ( CResultData* pVar )
{
	// Apply any data offset to ensure correct place in UDT structure from pVar
	DWORD dwDataOffset=pVar->m_dwDataOffset;

	// Duplicate strings in UDT
	CDeclaration* pCurrent = pVar->m_pStruct->GetDecChain();
	while(pCurrent)
	{
		// Determine type of Local Variable
		DWORD dwDecType = g_pVarTable->GetBasicTypeValue(pCurrent->GetType()->GetStr());
		if(pCurrent->GetArrFlag()==1) dwDecType+=100;
		if(dwDecType==3 || dwDecType>=1001)
		{
			// Determine local func-mem from local var name
			DWORD dwOffset=0, dwSizeOfData=0;
			if(g_pStructTable->FindOffsetFromField(pVar->m_pStruct->GetTypeName()->GetStr(), pCurrent->GetName()->GetStr(), &dwOffset, &dwSizeOfData))
			{
				// Process Element
				if(dwDecType==1001)
				{
					// Element is a user type
					ActOnType(pVar, pCurrent->GetType(), dwDataOffset+dwOffset);
				}
				else
				{
					// Element is a basic type
					ActOnSingleVar(pVar, dwDecType, dwDataOffset+dwOffset);
				}
			}
		}
		pCurrent=pCurrent->GetNext();
	}
	
	// Complete
	return true;
}

bool CParseInstruction::WriteDBM(void)
{
	// Write out param chain
	if(m_pParameter)
	{
		if(m_pParameter->WriteDBM()==false) return false;
	}

	// Write out function call
	WriteDBMBit();

	return true;
}

bool CParseInstruction::WriteDBMBit(void)
{
	// Write Out parameters to Stack if jump or call instruction
	int iRemoveInParam=0;
	bool bAddParamsToStack=false;
	if(m_dwInstructionType==3) bAddParamsToStack=true;
	if(m_dwInstructionType==2)
	{
		// Instruction Requires Stack Items
		if(m_pRefInstructionEntry)
		{
			if(m_pRefInstructionEntry->GetDLL()->Length()>0)
			{
				bAddParamsToStack=true;
			}

			// Except in cases where instruction input params are output (input a)
			if(m_pRefInstructionEntry->GetReturnParam()>=11
			&& m_pRefInstructionEntry->GetReturnParam()<=19)
			{
				iRemoveInParam=m_pRefInstructionEntry->GetReturnParamPlace();
			}
		}
	}

	// Prepare Return Param Flag
	CResultData* pPutEAXReturn = NULL;
	CResultData* pPutEAXReturnDynCreated = NULL;
	CParameter* pInputParamToUseAsOutput = NULL;

	// When userfunctions return a string/array, return var must be freed/cleared
	if(m_dwInstructionType==3)
	{
		// Handle Return Value later..
		if(m_pRefInstructionEntry)
		{
			if(GetReturnParameter())
			{
				pPutEAXReturnDynCreated = new CResultData;
				pPutEAXReturn = pPutEAXReturnDynCreated;
				pPutEAXReturn->m_pStringToken = GetReturnParameter();
				pPutEAXReturn->m_dwType = m_pRefInstructionEntry->GetReturnParam();
				pPutEAXReturn->m_pAdditionalOffset = NULL;
				pPutEAXReturn->m_dwDataOffset = 0;

				// Ensure Return Param From UserFunction is freed if string or array
				if(pPutEAXReturn->m_dwType==3)
				{
					if(pPutEAXReturn->m_pStringToken) pPutEAXReturn->m_pStringToken->TranslateForDBM(pPutEAXReturn);
					if(pPutEAXReturn->m_pAdditionalOffset) pPutEAXReturn->m_pAdditionalOffset->TranslateForDBM(pPutEAXReturn);

					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pPutEAXReturn);
					CInstructionTableEntry* pRef=g_pInstructionTable->GetRef(IT_INTERNAL_STRFREE);
					LPSTR pMathCommand=pRef->GetDecoratedName()->GetStr();
					g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", pMathCommand);
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);
				}
			}
		}
	}

	DWORD dwMustPopStack=0;
	if(bAddParamsToStack==true)
	{
		// Add to Stack - In Reverse Order
		if(GetParameter())
		{
			// How many params
			int iThisPosition=m_pRefInstructionEntry->GetParamMax();
			CParameter* pCurrent = GetParameter()->GetLast();
			if(pCurrent)
			{
				while(pCurrent)
				{
					// Add to stack or not
					DWORD dwDataType = pCurrent->GetMathItem()->FindResultTypeValueForDBM();
					if(iRemoveInParam!=iThisPosition || dwDataType==103 || dwDataType==13 || dwDataType==3)
					{
						// Finally push result to stack for main instruction
						if ( dwDataType==1001 )
						{
							// UDT is variable size
							CResultData* pResultData = pCurrent->GetMathItem()->FindResultData();
							DWORD dwUDTSize = 4;
							if ( pResultData->m_pStruct ) dwUDTSize = pResultData->m_pStruct->GetTypeSize();
							DWORD dwStackSize = dwUDTSize/4;
							if ( dwStackSize*4!=dwUDTSize )	dwStackSize++;
							DWORD dwStackSizeInBytes=dwStackSize*4;
							CStr* pStackSize = new CStr();
							pStackSize->SetNumericText(dwStackSizeInBytes);

							// Push entire UDT data onto stack (force data through resultstructure)
							CResultData* pSizeData = new CResultData();
							pSizeData->m_pStringToken = pStackSize;
							pSizeData->m_dwDataOffset = dwStackSize;
							g_pASMWriter->WriteASMTaskP2(m_dwLineNumber, ASMTASK_PUSHUDT, pResultData, pSizeData);

							// Record size on stack (for later removal)
							dwMustPopStack+=dwStackSize;

							// free result with string
							SAFE_DELETE(pSizeData);
						}
						else
						{
							// regular DWORD(x2) push
							g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pCurrent->GetMathItem()->FindResultData());
							if(dwDataType==8 || dwDataType==9 || dwDataType==108 || dwDataType==109)
								dwMustPopStack+=2;
							else
								dwMustPopStack++;
						}
					}

					// Record param for output purposes...
					if(iRemoveInParam>0 && iRemoveInParam==iThisPosition)
						pInputParamToUseAsOutput = pCurrent;
					
					// Prev param
					pCurrent=pCurrent->GetPrev();
					iThisPosition--;
				}
			}
		}
	}

	if(m_dwInstructionType==2)
	{
		// Called Instructions
		if(m_pRefInstructionEntry)
		{
			if(m_pRefInstructionEntry->GetDLL()->Length()>0)
			{
				// Prepare return result
				pPutEAXReturnDynCreated=NULL;
				if(m_pRefInstructionEntry->GetReturnParam()>0)
				{
					// If input-specified output present, use it (except if require non-typical datatypes)
					if(pInputParamToUseAsOutput && m_pRefInstructionEntry->GetReturnParam()!=18 && m_pRefInstructionEntry->GetReturnParam()!=19)
					{
						pPutEAXReturn = pInputParamToUseAsOutput->GetMathItem()->FindResultData();
					}
					else
					{
						// Use Return Param, else last param of expression chain
						if(GetReturnParameter())
						{
							pPutEAXReturnDynCreated = new CResultData;
							pPutEAXReturn = pPutEAXReturnDynCreated;
							pPutEAXReturn->m_pStringToken = GetReturnParameter();
							pPutEAXReturn->m_dwType = m_pRefInstructionEntry->GetReturnParam();
							if(pPutEAXReturn->m_dwType>10 && pPutEAXReturn->m_dwType<20) pPutEAXReturn->m_dwType-=10;//INPUT marks param as an output var by setting type as 11-19
							pPutEAXReturn->m_pAdditionalOffset = NULL;
							pPutEAXReturn->m_dwDataOffset = 0;
						}
						else
						{
							if(GetParameter()) pPutEAXReturn = GetParameter()->GetMathItem()->FindResultData();
						}
					}
					if(pPutEAXReturn->m_dwType==0) pPutEAXReturn=NULL;
				}

				// Ensure result is translated
				if(pPutEAXReturn)
				{
					if(pPutEAXReturn->m_pStringToken) pPutEAXReturn->m_pStringToken->TranslateForDBM(pPutEAXReturn);
					if(pPutEAXReturn->m_pAdditionalOffset) pPutEAXReturn->m_pAdditionalOffset->TranslateForDBM(pPutEAXReturn);
				}

				// A Return String must be passed (so it can be freed) All xxxxx[%S
				if(m_pRefInstructionEntry->GetReturnParam()==3
				&& m_pRefInstructionEntry->GetHardcoreInternalValue()!=IT_INTERNAL_ASSIGNSS)
				{
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pPutEAXReturn);
					dwMustPopStack++;
				}

				// DLL Call
				g_pASMWriter->WriteASMCall(m_dwLineNumber, m_pRefInstructionEntry->GetDLL()->GetStr(), m_pRefInstructionEntry->GetDecoratedName()->GetStr());
			}
			else
			{
				DWORD dwBuildID=m_pRefInstructionEntry->GetBuildID();
				if(dwBuildID==0)
				{
					// HARDCODED INTERNAL MATHS AND CASTS
					CParameter* pCurrent = GetParameter();
					if(pCurrent)
					{
						g_pASMWriter->WriteASMTaskP2(m_dwLineNumber, ASMTASK_ASSIGN, pCurrent->GetMathItem()->FindResultData(), pCurrent->GetNext()->GetMathItem()->FindResultData());
					}
				}
				else
				{
					// HARDCODED INSTRUCTIONS (NO CORE CALLS)
					CParameter* pCurrent = GetParameter();
					if(pCurrent)
					{
						if(pCurrent->GetMathItem())
						{
							if(pCurrent->GetNext()==NULL)
							{
								// With 1 Param
								WriteDBMHardCode(dwBuildID, pCurrent->GetMathItem()->FindResultData(), NULL, NULL);
							}
							else
							{
								if(pCurrent->GetNext()->GetNext()==NULL)
								{
									// With 2 Params
									WriteDBMHardCode(dwBuildID, pCurrent->GetMathItem()->FindResultData(), pCurrent->GetNext()->GetMathItem()->FindResultData(), NULL);
								}
								else
								{
									// With 3 Params
									WriteDBMHardCode(dwBuildID, pCurrent->GetMathItem()->FindResultData(), pCurrent->GetNext()->GetMathItem()->FindResultData(), pCurrent->GetNext()->GetNext()->GetMathItem()->FindResultData());
								}
							}
						}
					}
					else
					{
						// No Params
						WriteDBMHardCode(dwBuildID, NULL, NULL, NULL);
					}
				}
			}
		}
	}
	if(m_dwInstructionType==3)
	{
		// User Defined CALL to User Function
		if(m_pRefInstructionEntry)
		{
			// Handle Return Value later..
			if(pPutEAXReturn)
			{
				pPutEAXReturn->m_pStringToken = GetReturnParameter();
				pPutEAXReturn->m_dwType = m_pRefInstructionEntry->GetReturnParam();
			}

			// Set the JMP Instruction
			CStr* pLabelParam=GetLabelParam();
			if(pLabelParam) g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_JUMPSUBROUTINE, pLabelParam, 10);
			
			// Return stack pointer to normal
			if(dwMustPopStack>0)
			{
				CStr* pData = new CStr;
				pData->SetNumericText(dwMustPopStack*4);
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_ADDESP, pData, 7);
				SAFE_DELETE(pData);
			}

			// No need to pop stack
			dwMustPopStack=0;
		}
	}

	// Handle Return Value from EAX to Return Param
	if(pPutEAXReturn)
	{
		// Ensure return value has been translated (ie @fs  to :-19)
		if(pPutEAXReturn->m_pStringToken) pPutEAXReturn->m_pStringToken->TranslateForDBM(pPutEAXReturn);

		// Copy EAX/EDX contents to Required Return Param
		g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_ASSIGN, pPutEAXReturn);
	}

	// Free usages
	SAFE_DELETE(pPutEAXReturnDynCreated);

	// Optional function return param
	if(dwMustPopStack>0)
	{
		while(dwMustPopStack)
		{
			g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_POPEBX, NULL, 0);
			dwMustPopStack--;
		}
	}

	return true;
}

bool CParseInstruction::WriteDBMHardCode(DWORD dwBuildID, CResultData* pP1, CResultData* pP2, CResultData* pP3)
{
	switch(dwBuildID)
	{
		case BUILD_RET:
			if ( g_pUserFunctionWithin && g_pDBPCompiler->m_bRemoveSafetyCode==false )
			{
				// U74 - 110509 - ensure RETURN commands not inside a user function
				char err[512];
				wsprintf ( err, "Cannot use RETURN command inside a function at line %d.", m_dwLineNumber );
				g_pErrorReport->AddErrorString(err);
				return false;
			}
			else
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_RETURN, NULL, 0);
			break;

		case BUILD_PURERET:
			g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PURERETURN, NULL, 0);
			break;

		case BUILD_END:
			{
				// Always jump to END OF PROGRAM
				CStr* pAlwaysJump = new CStr("$labelend");
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_JUMP, pAlwaysJump, 10);
				SAFE_DELETE(pAlwaysJump);
			}
			break;

		case BUILD_ENDERROR:
			{
				// Report An Error (user hit the function declaration mid-program)
				g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?EarlyEnd@@YAXXZ");

				// Always jump to END OF PROGRAM
				CStr* pAlwaysJump = new CStr("$labelend");
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_JUMP, pAlwaysJump, 10);
				SAFE_DELETE(pAlwaysJump);
			}
			break;

		case BUILD_SYNC:
			{
				// If Debug Mode, always push code position to stack
				if(g_DebugInfo.DebugModeOn())
				{
					// Calculate Current Position Of Program
					CStr* pData = new CStr("");
					DWORD dwPosition=g_pASMWriter->GetCurrentMCPosition();
					g_DebugInfo.SetLastBreakPoint(dwPosition);
					pData->SetNumericText(dwPosition);
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSH, pData, 7);
					SAFE_DELETE(pData);

					// Call PROCESSMESSAGES Function with Debug Prog-Position
					g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?ProcessMessages@@YAKK@Z");

					// Removes Position DWORD back off stack
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_POPEBX, NULL, 0);

					// Check if EAX=1 (got a message to QUIT)
					pData = new CStr("1");
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_CONDITIONDATA, pData, 7);
					SAFE_DELETE(pData);

					// Only snapshot if parsing main, not mini
					if(g_DebugInfo.GetParsingMain())
					{
						// LEAP-FORWARDS Marker OpCode
						g_pASMWriter->WriteASMLeapMarkerJumpNotEqual(1);

// No Stack save - data would be useless when new m/c executed
//						// Push all registers to stack before snapshot
//						g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSHREGISTERS, NULL, 0);

//						// Push stack location to stack
//						g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSHESP, NULL, 0);

//						// Ending program - Snapshot everything on stack back to beginning of program run
//						g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?StackSnapshotStore@@YAXK@Z");

						// Jump to END OF PROGRAM
						CStr* pJumpToLabel = new CStr("$labelend");
						g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_JUMP, pJumpToLabel, 10);
						SAFE_DELETE(pJumpToLabel);

						// Complete LEAP-FORWARD Marker
						g_pASMWriter->WriteASMLeapMarkerEnd(1);
					}
					else
					{
						// Jump to END OF PROGRAM
						CStr* pJumpToLabel = new CStr("$labelend");
						g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_CONDJUMPE, pJumpToLabel, 10);
						SAFE_DELETE(pJumpToLabel);
					}
				}
				else
				{
					// Call PROCESSMESSAGES Function for fullspeed mode
					g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?ProcessMessages@@YAKXZ");

					// Check if EAX=1 (got a message to QUIT)
					CStr* pData = new CStr("1");
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_CONDITIONDATA, pData, 7);
					SAFE_DELETE(pData);

					// If so, jump to END OF PROGRAM
					CStr* pJumpToLabel = new CStr("$labelend");
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_CONDJUMPE, pJumpToLabel, 10);
					SAFE_DELETE(pJumpToLabel);
				}
			}							
			break;

		case BUILD_STARTPROGRAM:
			{
				// Store Registers before program begins
				CStr* pParam1 = new CStr("@$_ESP_");
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSHREGISTERS, NULL, 0);
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_STOREESP, pParam1, 7);
				SAFE_DELETE(pParam1);

				// If Debug Mode, special jump if breakpoint needs to be jumped to
				if(g_DebugInfo.DebugModeOn() && g_DebugInfo.GetParsingMain())
				{
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_BREAKPOINTRESUME, NULL, 0);
				}
			}
			break;
			
		case BUILD_ENDPROGRAMANDQUIT:
			{
				// Call QUIT Function (to close message loop only for main program)
				if(g_DebugInfo.GetParsingMain())
				{
					// leefix - 120108 - U71 - correct line number
					g_pASMWriter->m_dwLineNumber = m_dwLineNumber;

					// Check if breakpoint active
					g_pASMWriter->WriteASMCheckBreakPointVar();

					// LEAP-FORWARDS Marker OpCode to skip breakpoint-resume
					g_pASMWriter->WriteASMLeapMarkerJumpNotEqual(0);

					// Call Quit which closes the app window
					g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?Quit@@YAKXZ");

					// Complete LEAP-FORWARD Marker (jump here if skip breakpoint resume)
					g_pASMWriter->WriteASMLeapMarkerEnd(0);
				}

				// Compare _ESP_ store with stack
				if(g_DebugInfo.DebugModeOn())
				{
					// If they are different, broke from function or stack leak
					// set special escapecode so cannot resume this program
					CStr* pParam1 = new CStr("@$_ESP_");
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_SETNORETURNIFESPLEAK, pParam1, 7);
					SAFE_DELETE(pParam1);
				}

				// Restore STACK Pointer
				CStr* pParam1 = new CStr("@$_ESP_");
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_RESTOREESP, pParam1, 7);
				g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_POPREGISTERS, NULL, 0);
				SAFE_DELETE(pParam1);
			}
			break;

		case BUILD_INC:
		case BUILD_DEC:
			{
				// By 1 value, or Qty
				if(pP2==NULL)
				{
					// Inc or Dec
					DWORD dwASMToBuild=0;
					if(dwBuildID==BUILD_INC) dwASMToBuild=ASMTASK_INCVAR;
					if(dwBuildID==BUILD_DEC) dwASMToBuild=ASMTASK_DECVAR;

					// Call hard code builder (leefix-260603-passing in all data now as INC can change to ADD)
//					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, dwASMToBuild, pP1->m_pStringToken, pP1->m_dwType);
					g_pASMWriter->WriteASMTaskCore(	m_dwLineNumber, dwASMToBuild, pP1->m_pStringToken, pP1->m_pAdditionalOffset, pP1->m_dwType, pP1->m_dwDataOffset, NULL, NULL, 0, 0, NULL, NULL, 0, 0 );
				}
				else
				{
					// Add or Sub
					DWORD dwASMToBuild=0;
					if(dwBuildID==BUILD_INC) dwASMToBuild=ASMTASK_ADD;
					if(dwBuildID==BUILD_DEC) dwASMToBuild=ASMTASK_SUB;

					// Call hard code builder
					g_pASMWriter->WriteASMTaskCore(	m_dwLineNumber, dwASMToBuild,
													pP1->m_pStringToken, pP1->m_pAdditionalOffset, pP1->m_dwType, pP1->m_dwDataOffset,
													pP2->m_pStringToken, pP2->m_pAdditionalOffset, pP2->m_dwType, pP2->m_dwDataOffset,
													pP1->m_pStringToken, pP1->m_pAdditionalOffset, pP1->m_dwType, pP1->m_dwDataOffset);
				}
			}
			break;

		case BUILD_INCADD:
		case BUILD_DECADD:
			{
				// Inc or Dec
				DWORD dwMathSymbol=0;
				if(dwBuildID==BUILD_INCADD) dwMathSymbol=4;
				if(dwBuildID==BUILD_DECADD) dwMathSymbol=5;

				// If pP2 is empty, assume 1
				CResultData* pDynValue = NULL;
				CResultData* pValue = pP2;
				if(pValue==NULL)
				{
					pDynValue = new CResultData();
					ZeroMemory(pDynValue, sizeof(CResultData));
					pDynValue->m_pStringToken = new CStr("1");
					pDynValue->m_dwType = pP1->m_dwType % 100;
					pValue = pDynValue;
				}

				// Push Params for Add to stack (reverse order)
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pValue);
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pP1);

				// Internal Math Call to DLL
// get INC A,5.0 to parse as an INTEGER command, not a FLOAT (FFF) command...
				DWORD dwUseNewInstruction=g_pInstructionTable->DetermineInternalCommandCode(dwMathSymbol, pP1->m_dwType);
				if(dwUseNewInstruction==0)
				{
					// Command not yet implemented
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_UNKNOWN, NULL);
					SAFE_DELETE(pDynValue);
					return true;
				}
				DWORD dwInstructionValue=g_pInstructionTable->GetIIValue(dwUseNewInstruction);
				CInstructionTableEntry* pRef = g_pInstructionTable->GetRef(dwUseNewInstruction);
				LPSTR pMathDLL=pRef->GetDLL()->GetStr();
				LPSTR pMathCommand=pRef->GetDecoratedName()->GetStr();
				g_pASMWriter->WriteASMCall(m_dwLineNumber, pMathDLL, pMathCommand);

				// Copy EAX (holding call result) to temp var used to hold return value
				if(pP1) g_pASMWriter->WriteASMTaskP2(m_dwLineNumber, ASMTASK_ASSIGN, pP1, NULL);

				// Pop params after calc
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);

				// Free usages
				SAFE_DELETE(pDynValue);
			}
			break;

		case BUILD_COPYUDT:
			{
				// P1 = P2 using UDT values (use a memcpy to perform the copy) P3 is Size
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pP3);

				// Variable Or Array
				if ( pP2->m_dwType==1101 )
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pP2);
				else
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSHADDRESS, pP2);

				if ( pP1->m_dwType==1101 )
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pP1);
				else
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSHADDRESS, pP1);

				// Call MEMCPY equivilant
				g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?CopyByteMemory@@YAXKKH@Z");

				// Pop params after calc - leefix - 170403 - stupidy stupidy stupidy
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_POPEBX, NULL);

				// After block copy (for values, run through UDT area to recreate strings
				ActOnLocalVars ( pP1 );
			}
			break;

		case BUILD_USERFUNCTIONEXIT:

			// Not in function so ignore command
			if(g_pUserFunctionWithin==NULL)
				break;

			// Establish Return Param
			CStr* pReturnData = NULL;
			DWORD dwReturnDataType = 0;
			if(pP1)
			{
				pReturnData = pP1->m_pStringToken;
				dwReturnDataType = pP1->m_dwType;
			}

			// check if returned string is global (leefix - 210703 - CODE FROM ENDFUNCTION!)
			bool bDuplicateReturnString=false;
			CResultData* pResultData = NULL;
			if(pP1)
			{
				pResultData = pP1;
				if ( dwReturnDataType==3 || dwReturnDataType==103 )
				{
					DWORD dwArr=0;
					if ( dwReturnDataType==103 ) dwArr=1;
					CVarTable* pVar = g_pVarTable->FindVariable ( "", pReturnData->GetStr()+1, dwArr );
					if ( pVar )
					{
						// return is a global var or array string, must create a duplicate string to send back
						bDuplicateReturnString=true;
					}
				}
			}

			// Free any allocated dynamic memory (strings and arrays)
			g_pUserFunctionWithin->ActOnLocalVars(DBMPLACEMENT_BOTTOM, pReturnData);

			// If user function returns value, store in EAX/EDX/ST0
			if(pP1)
			{
				// Direct or 'Duplicate copy of string'
				if ( bDuplicateReturnString )
				{
					// Source global string array
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, pResultData);

					// Blank String - no thing to free
					CStr* pNull = new CStr("0");
					g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PUSH, pNull, 7);
					SAFE_DELETE(pNull);

					// Put new string address in EAX for return passing
					g_pASMWriter->WriteASMCall(m_dwLineNumber, "dbprocore.dll", "?EquateSS@@YAKKK@Z");
				}
				else
				{
					// direct copy of var ref to outside EAX
					g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_ASSIGNTOEAX, pP1);
				}
			}

			/* does not account for global return strings (see abive replacement code)
			// Free any allocated dynamic memory (strings and arrays)
			g_pUserFunctionWithin->ActOnLocalVars(DBMPLACEMENT_BOTTOM, pReturnData);

			// If user function returns value, store in EAX/EDX/ST0
			if(pP1)
			{
				g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_ASSIGNTOEAX, pP1);
			}
			*/

			// Restore ESP from EBP Register
			g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_MOVESPEBP, NULL, 0);

			// Restore EBP Register
			g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_POPEBP, NULL, 0);

			// RETurn
			g_pASMWriter->WriteASMTaskCoreP1(m_dwLineNumber, ASMTASK_PURERETURN, NULL, 0);
			break;
	}

	// Complete
	return true;
}


