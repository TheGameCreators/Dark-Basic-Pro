// StatementChain.cpp: implementation of the CStatementChain class.
//
//////////////////////////////////////////////////////////////////////

#include "StatementChain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatementChain::CStatementChain()
{
	m_FirstStatmentBlock=NULL;
	m_pNext=NULL;
}

CStatementChain::~CStatementChain()
{
	SAFE_DELETE(m_FirstStatmentBlock);
	SAFE_DELETE(m_pNext);
}

void CStatementChain::Add(CStatementChain *pNext)
{
	if(m_pNext==NULL)
	{
		m_pNext=pNext;
	}
	else
	{	
		m_pNext->Add(pNext);
	}
}
