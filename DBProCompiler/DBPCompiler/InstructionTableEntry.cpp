// InstructionTableEntry.cpp: implementation of the CInstructionTableEntry class.
//
//////////////////////////////////////////////////////////////////////

// Common Includes
#include "macros.h"

// Custom Includes
#include "Declaration.h"
#include "InstructionTableEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstructionTableEntry::CInstructionTableEntry()
{
	m_dwInternalID=0;
	m_dwReturnParam=0;
	m_dwParamMax=0;
	m_pName=NULL;
	m_pDLL=NULL;
	m_pDecoratedName=NULL;
	m_pParamTypes=NULL;
	m_pParamDesc=NULL;
	m_dwHardcoreInternalValue=0;
	m_dwBuildID=0;
	m_pDecChain=NULL;

	m_pPrev=NULL;
	m_pNext=NULL;
}

CInstructionTableEntry::~CInstructionTableEntry()
{
	SAFE_DELETE(m_pName);
	SAFE_DELETE(m_pDLL);
	SAFE_DELETE(m_pDecoratedName);
	SAFE_DELETE(m_pParamTypes);
	SAFE_DELETE(m_pParamDesc);
	SAFE_DELETE(m_pDecChain);
}

void CInstructionTableEntry::Free(void)
{
	CInstructionTableEntry* pCurrent = this;
	while(pCurrent)
	{
		CInstructionTableEntry* pNext = pCurrent->GetNext();
		delete pCurrent;
		pCurrent = pNext;
	}
}

void CInstructionTableEntry::Add(CInstructionTableEntry* pNew)
{
	CInstructionTableEntry* pCurrent = this;
	while(pCurrent->m_pNext)
		pCurrent=pCurrent->m_pNext;
	pCurrent->m_pNext=pNew;
	pNew->m_pPrev=pCurrent;
}

void CInstructionTableEntry::Insert(CInstructionTableEntry *pNew)
{
	// Get neighbors
	CInstructionTableEntry* pNeighA = m_pPrev;
	CInstructionTableEntry* pNeighB = this;

	// Instruct neighbours to point to me
	if(pNeighA) pNeighA->m_pNext = pNew;
	pNeighB->m_pPrev = pNew;

	// Insruct new to point to neighbors
	pNew->m_pNext = pNeighB;
	pNew->m_pPrev = pNeighA;
}

void CInstructionTableEntry::SetData(DWORD InternalID, CStr* pStr, CStr* pDLL, CStr* pDecoratedName, CStr* pParamTypes, DWORD returnparam, DWORD param, DWORD dwInternalId, DWORD dwBuildID)
{
	// Set Instruction Data
	m_dwInternalID = InternalID;
	m_dwReturnParam = returnparam;
	m_dwParamMax = param;
	m_pName = pStr;
	m_pDLL = pDLL;
	m_pDecoratedName = pDecoratedName;
	m_pParamTypes = pParamTypes;
	m_dwHardcoreInternalValue=dwInternalId;
	m_dwBuildID=dwBuildID;
}
