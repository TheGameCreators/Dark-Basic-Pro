// Declaration.cpp: implementation of the CDeclaration class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"

// Common Includes
#include "Declaration.h"
#include "VarTable.h"

// External Class Pointer
extern CVarTable *g_pVarTable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeclaration::CDeclaration()
{
	m_dwLineNumber=0;

	m_dwArr=0;
	m_pArrValue=NULL;
	m_pName=NULL;
	m_pType=NULL;
	m_pInit=NULL;
	m_dwOffset=0;
	m_dwDataSize=0;

	m_pNext=NULL;
	m_pPrev=NULL;
}

CDeclaration::~CDeclaration()
{
	SAFE_DELETE(m_pArrValue);
	SAFE_DELETE(m_pName);
	SAFE_DELETE(m_pType);
	SAFE_DELETE(m_pInit);

	SAFE_DELETE(m_pNext);
}

void CDeclaration::Add(CDeclaration* pNew)
{
	if(m_pNext==NULL)
	{
		pNew->m_pPrev=this;
		m_pNext = pNew;
	}
	else
		m_pNext->Add(pNew);
}

CDeclaration* CDeclaration::Find(LPSTR pName, DWORD dwArrFlag)
{
	CDeclaration* pCurrent = this;
	while(pCurrent)
	{
		if(stricmp(pName, pCurrent->GetName()->GetStr())==NULL
		&& pCurrent->GetArrFlag()==dwArrFlag)
			return this;

		pCurrent=pCurrent->GetNext();
	}
	return NULL;
}

void CDeclaration::SetDecData(DWORD dwDecArr, LPSTR pDecArrValue, LPSTR pDecName, LPSTR pDecType, LPSTR pDecInit, DWORD LineNumberRef)
{
	// Data values
	CStr* pArrValueStr = new CStr(pDecArrValue);
	CStr* pNameStr = new CStr(pDecName);
	CStr* pTypeStr = new CStr(pDecType);
	CStr* pInitStr = new CStr(pDecInit);

	// Set data
	SetArr(dwDecArr);
	SetArrValue(pArrValueStr);
	SetName(pNameStr);
	SetType(pTypeStr);
	SetInit(pInitStr);
	SetLineNumber(LineNumberRef);
	SetOffset(0);
	SetDataSize(0);
}

bool CDeclaration::GetNumberOfDecsInChain(DWORD* pdwCount)
{
	(*pdwCount)++;
	if(GetNext())
		return GetNext()->GetNumberOfDecsInChain(pdwCount);

	return true;
}

bool CDeclaration::GetTypeStringOfDecsInChain(LPSTR* pTypeString)
{
	// Collect characters rep. types that make up dec chain
	CStr* pStr = new CStr("");
	CDeclaration* pEntry = this;
	while(pEntry)
	{
		LPSTR pTypeNameString = pEntry->GetType()->GetStr();
		DWORD dwTypeValue = g_pVarTable->GetBasicTypeValue(pTypeNameString);
		pStr->AddChar(g_pVarTable->GetCharOfType(dwTypeValue));
		pEntry=pEntry->GetNext();
	}

	// Create Type String
	*pTypeString = new char[pStr->Length()+1];
	strcpy(*pTypeString, pStr->GetStr());
	SAFE_DELETE(pStr);

	// Complete
	return true;
}

bool CDeclaration::WriteDBM(void)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetText("STRUCT@");
	strDBMLine.AddText(m_pName);
	if(GetArrFlag()==1)
	{
		strDBMLine.AddText("(array)");
	}
	strDBMLine.AddText("    TYPE>");
	strDBMLine.AddText(m_pType);
	strDBMLine.AddText("    OFFSET>");
	strDBMLine.AddNumericText(m_dwOffset);

	// Output type details
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// Write next one
	if(GetNext())
	{
		if((GetNext()->WriteDBM())==false) return false;
	}

	// Complete
	return true;
}
