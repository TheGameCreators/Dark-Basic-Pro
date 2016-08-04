#include "stdafx.h"
#include < windows.h >
#include < iostream.h >

#include "DBOFile.h"

int main ( int argc, char* argv [ ] )
{
	sObject* pObject = NULL;

	if ( !LoadDBOEx ( "test.dbo", &pObject ) )
		return 0;

	delete pObject;
	pObject = NULL;

	return 0;
}