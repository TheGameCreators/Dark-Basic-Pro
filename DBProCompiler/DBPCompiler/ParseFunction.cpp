// ParseFunction.cpp: implementation of the CParseFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "ParseFunction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseFunction::CParseFunction()
{
	m_dwLineNumber=0;
	m_pResultStringToken=NULL;
	m_pParameter=NULL;
}

CParseFunction::~CParseFunction()
{
	SAFE_DELETE(m_pResultStringToken);
	SAFE_DELETE(m_pParameter);
}

bool CParseFunction::WriteDBM(void)
{
	// Write out param chain
	if(m_pParameter)
	{
		if(m_pParameter->WriteDBM()==false) return false;
	}

	// Write out function call
	WriteDBMBit(GetLineNumber());

	return true;
}

bool CParseFunction::WriteDBMBit(DWORD dwLineNumber)
{
	// Write out function param list
	CStr strDBMParamList(1);
	GetParameter()->MakeParamList(&strDBMParamList);

	// Write Out What Needs To Be Done
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText("Use Function ");
	strDBMLine.AddText("FUNCTION");
	strDBMLine.AddText(" using params ");
	strDBMLine.AddText(&strDBMParamList);
	strDBMLine.AddText(". Produce ");
	strDBMLine.AddText(m_pResultStringToken);
	strDBMLine.AddText(".");

	// Output final function call
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}