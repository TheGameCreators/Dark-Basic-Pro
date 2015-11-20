// ParseInit.cpp: implementation of the CParseInit class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"

#include "ParseInit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseInit::CParseInit()
{
	m_dwLineNumber=0;
	m_pMathOp=NULL;
	m_pDataParamList=NULL;
}

CParseInit::~CParseInit()
{
	SAFE_DELETE(m_pMathOp);
	SAFE_DELETE(m_pDataParamList);
}

bool CParseInit::WriteDBM(void)
{
	WriteDBMBit(GetLineNumber(), "");
	return true;
}

bool CParseInit::WriteDBMBit(DWORD dwLineNumber, LPSTR pText)
{
	// Write out function param list
	CStr strDBMParamList(1);
	if(m_pDataParamList)
		m_pDataParamList->MakeParamList(&strDBMParamList);

	// Write out text
	/* leefix - u71 - 061108 - messed up external debugger scanning of DBM
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(dwLineNumber);
	strDBMLine.AddText(" Fill ");
	strDBMLine.AddText(m_pMathOp->FindResultStringTokenForDBM());
	strDBMLine.AddText(" with ");
	strDBMLine.AddText(&strDBMParamList);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;
	*/
	// leefix - u71 - 121108 - but it did some good, so just call the functions silently
	m_pMathOp->FindResultStringTokenForDBM();

	// Do not fill if array
	DWORD dwType=m_pMathOp->FindResultTypeValueForDBM();
	if(dwType<100 || dwType==1001)
	{
		// Simple Init
		if(dwType==3)
		{
			// Dynamic String Creation
			CResultData* pVarItem = m_pMathOp->FindResultData();
			CResultData* pDataItem = m_pDataParamList->GetMathItem()->FindResultData();
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_PUSH, pDataItem);
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_PUSH, pVarItem);
			g_pASMWriter->WriteASMCall(dwLineNumber, "dbprocore.dll", "?EquateSS@@YAKKK@Z");
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_ASSIGN, pVarItem);
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_POPEBX, NULL);
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_POPEBX, NULL);
		}
		else
		{
			// Simple Value Assignment
			if ( m_pDataParamList )
				if(g_pASMWriter->WriteASMTaskP2(dwLineNumber, ASMTASK_ASSIGN, m_pMathOp->GetResultData(), m_pDataParamList->GetMathItem()->FindResultData())==false) return false;
		}
	}

	return true;
}