// DebugInfo.cpp: implementation of the CDebugInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "DebugInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugInfo::CDebugInfo()
{
	// All Ref Only
	m_bDebugModeOn=false;
	m_dwProgramSize=0;
	m_dwProgramPtr=NULL;
	m_bParsingMain=true;

	m_bMessageArrived=false;
	m_pCliText=NULL;
}

CDebugInfo::~CDebugInfo()
{
	if(m_pCliText)
	{
		delete m_pCliText;
		m_pCliText=NULL;
	}
}
