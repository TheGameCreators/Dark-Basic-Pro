// ParseType.cpp: implementation of the CParseType class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"
#include "ParseType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseType::CParseType()
{
	m_dwStartLineNumber=0;
	m_dwEndLineNumber=0;
}

CParseType::~CParseType()
{
}

bool CParseType::WriteDBM(void)
{
	WriteDBMBit(GetEndLineNumber(), "");
	return true;
}

bool CParseType::WriteDBMBit(DWORD dwLineNumber, LPSTR pText)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(dwLineNumber);
	strDBMLine.AddText(" ?");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}
