//
// External Text Reader
//

// Includes
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)

#include "stdafx.h"
#include "ExternalText.h"
#include "direct.h"

// String Data
char g_pTGCWord [ EXTWORDSMAX ][ 1024 ];

//
// Functions
//

void GatherAllExternalWords(LPSTR pWordsFile)
{
	// Read All External Words From Words File
	int id=0;
	while(id<EXTWORDSMAX)
	{
		// create id
		char idstr[8];
		itoa(id, idstr, 10);

		// get word from id
		GetPrivateProfileString("GENERAL", idstr, "", g_pTGCWord[id], 1024, pWordsFile);

		// next word
		id++;
	}
}

bool FindExternalWords(void)
{
	// Temp Strings
	char root[_MAX_PATH];
	char path[_MAX_PATH];
	char inifile[_MAX_PATH];
	char textfiles[_MAX_PATH];

	// already in current folder
	getcwd(root, _MAX_PATH);

	// Get path from root filename to find setup.ini
	strcpy(inifile, root);
	strcat(inifile, "\\SETUP.INI");

	// Get Path to WORDS File
	GetPrivateProfileString("SETTINGS", "TEXTLANGUAGE", "ENGLISH", textfiles, 256, inifile);
	strcpy(path, root);
	strcat(path, "\\LANG");
	strcat(path, "\\");
	strupr(textfiles);
	strcat(path, textfiles);
	strcat(path, "\\TGC.TXT");

	// GATHER ALL EXTERNAL WORDS
	GatherAllExternalWords(path);

	// Complete
	return true;
}
