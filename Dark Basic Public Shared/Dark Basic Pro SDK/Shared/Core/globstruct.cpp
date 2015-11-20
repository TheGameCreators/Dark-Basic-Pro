//
// Glob Struct Functions (shared by all DLLs)
//

// Includes
#define _CRT_SECURE_NO_DEPRECATE
#include "windows.h"
#include "globstruct.h"

// Global Function Ptr to CreateString in Core ptr passed in
extern GlobStruct* g_pGlob;

// Checklist creation
void GlobExpandChecklist( DWORD iIndex, DWORD dwImminentStringSize )
{
	// First must make a checklist if new
	if(g_pGlob->checklist==NULL)
	{
		// Create array of items
		g_pGlob->dwChecklistArraySize=256;
		DWORD dwMemSize = g_pGlob->dwChecklistArraySize * sizeof(GlobChecklistStruct);
		g_pGlob->CreateDeleteString((DWORD*)&g_pGlob->checklist, dwMemSize);
		ZeroMemory(g_pGlob->checklist, sizeof(GlobChecklistStruct) * g_pGlob->dwChecklistArraySize);

		// Create strings for each item
		for(DWORD n=0; n<g_pGlob->dwChecklistArraySize; n++)
		{
			// Create default blank string
			g_pGlob->checklist[n].dwStringSize = 2;
			DWORD dwMemSize = g_pGlob->checklist[n].dwStringSize;
			g_pGlob->CreateDeleteString((DWORD*)&g_pGlob->checklist[n].string, dwMemSize);
			strcpy(g_pGlob->checklist[n].string, "");
		}
	}

	// Ensure checklist is big enough
	if(iIndex>g_pGlob->dwChecklistArraySize-2)
	{
		// Double size of array
		DWORD dwArraySize=g_pGlob->dwChecklistArraySize*2;

		// Make new larger checklist
		GlobChecklistStruct* pNewArray = NULL;
		DWORD dwMemSize = dwArraySize * sizeof(GlobChecklistStruct);
		g_pGlob->CreateDeleteString((DWORD*)&pNewArray, dwMemSize);
		ZeroMemory(pNewArray, sizeof(GlobChecklistStruct) * dwArraySize);
		
		// Copy strings over to new array
		DWORD n = 0;
		for(n=0; n<g_pGlob->dwChecklistArraySize; n++)
		{
			// Create default blank string
			pNewArray[n].dwStringSize = g_pGlob->checklist[n].dwStringSize;
			pNewArray[n].string = g_pGlob->checklist[n].string;
		}

		// Create new strings
		for(g_pGlob->dwChecklistArraySize; n<dwArraySize; n++)
		{
			// Create default blank string
			pNewArray[n].dwStringSize=2;
			DWORD dwMemSize = pNewArray[n].dwStringSize;
			g_pGlob->CreateDeleteString((DWORD*)&pNewArray[n].string, dwMemSize);
			strcpy(pNewArray[n].string, "");
		}

		// Transfer pointers and delete old one
		if(g_pGlob->checklist) g_pGlob->CreateDeleteString((DWORD*)&g_pGlob->checklist, 0);
		g_pGlob->dwChecklistArraySize = dwArraySize;
		g_pGlob->checklist = pNewArray;
	}

	// Ensure string being referenced is big enough
	if(dwImminentStringSize>g_pGlob->checklist[iIndex].dwStringSize)
	{
		// Expand string within checklist
		LPSTR pNewString=NULL;
		DWORD dwMemSize = dwImminentStringSize+1;
		g_pGlob->CreateDeleteString((DWORD*)&pNewString, dwMemSize);
		strcpy(pNewString, g_pGlob->checklist[iIndex].string);
		g_pGlob->checklist[iIndex].dwStringSize = dwImminentStringSize;
		g_pGlob->CreateDeleteString((DWORD*)&g_pGlob->checklist[iIndex].string, 0);
		g_pGlob->checklist[iIndex].string=pNewString;
	}
}
