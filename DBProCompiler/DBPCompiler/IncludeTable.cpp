// IncludeTable.cpp: implementation of the CIncludeTable class.
//
//////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE
#include "IncludeTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIncludeTable::CIncludeTable()
{
	m_pFilename=NULL;
	m_dwFirstByte=0;
	m_pNext=NULL;
}

CIncludeTable::~CIncludeTable()
{
	SAFE_DELETE(m_pFilename);
	SAFE_DELETE(m_pNext);
}

void CIncludeTable::Add(CIncludeTable* pNew)
{
	if(m_pNext==NULL)
		m_pNext=pNew;
	else
		m_pNext->Add(pNew);
}

bool CIncludeTable::FindInclude(LPSTR pFilename)
{
	CIncludeTable* pCurrent = this;
	while(pCurrent)
	{
		if(stricmp(pFilename, pCurrent->GetFilename()->GetStr())==NULL)
			return true;

		pCurrent=pCurrent->GetNext();
	}

	// Could not find soft fail
	return false;
}
