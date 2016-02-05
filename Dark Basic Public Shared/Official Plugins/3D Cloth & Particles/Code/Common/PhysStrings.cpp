#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "PhysErrors.h"


#if defined(BUILD_PC)
#include "../DBProPhysicsMaster/Resource_PC/Definitions.h"
#elif defined(BUILD_P)
#include "../DBProPhysicsMaster/Resource_P/Definitions.h"
#elif defined(BUILD_C)
#include "../DBProPhysicsMaster/Resource_C/Definitions.h"
#endif

char ** PERR_messages=0;
char ** PCMD_names=0;

int __numErrorMsgs=0;
int __numCommandNames=0;

bool __errorMessagesAvailable=false;

int countItems(char * buffer,int bufferSize, const char * marker)
{	
	char *curPos,*lastPos,*start,*end;

	curPos = strstr(buffer,marker);
	if(!curPos) return -1;
	start = strchr(curPos,'"');
	if(!start) return -1;
	
	lastPos = strchr(start,'[');
	if(!lastPos) lastPos = buffer+bufferSize;
	if(start>lastPos) return -1;

	int numItems=0;
	while(start && start<lastPos)
	{	
		numItems++;
		start++;
		end = strchr(start,'"');
		start = strchr(end+1,'"');
	}
	return numItems;
}

bool loadItems(char * buffer,int bufferSize , const char * marker, char ** strPtrList)
{	
	char *curPos,*lastPos,*start,*end;

	curPos = strstr(buffer,marker);
	if(!curPos) return false;
	start = strchr(curPos,'"');
	if(!start) return false;
	
	lastPos = strchr(start,'[');
	if(!lastPos) lastPos = buffer+bufferSize;
	if(start>lastPos) return false;

	int length=0;
	int count=0;
	while(start && start<lastPos)
	{			
		start++;
		end = strchr(start,'"');
		length = (int)(end-start);
		strPtrList[count] = new char[length+1];
		strncpy(strPtrList[count],start,length);
		strPtrList[count][length]=0;
		count++;
		start = strchr(end+1,'"');
	}
	return true;
}

bool stringLoadError(const char * msg)
{
#ifdef _DEBUG
	MessageBox(0,msg,"Physics Resource String Error",MB_OK);
#endif
	return false;
}

bool loadLocalisationStrings()
{	
	__errorMessagesAvailable=false;

	try
	{

	HKEY hKey;	
	unsigned long datatype;
	unsigned long bufferlength = 1024;
	char filename[1024];

	// mike - 300305 - don't display error messages for loc files

	long res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Dark Basic\\Dark Basic Pro"), 0, KEY_QUERY_VALUE, &hKey);
	if(res!=ERROR_SUCCESS)
	{
		RegCloseKey ( hKey );
		return false; //stringLoadError("Code 1");
	}


	res = RegQueryValueEx(hKey, "INSTALL-PATH", NULL, &datatype, (BYTE*)&filename, &bufferlength);
	if(res!=ERROR_SUCCESS)
	{
		RegCloseKey ( hKey );
		return false; //stringLoadError("Code 2"); 
	}


	res = RegCloseKey(hKey);
	if(res!=ERROR_SUCCESS) return false; //stringLoadError("Code 3");



#if defined(BUILD_PC)
	strcat(filename,"\\Compiler\\plugins-user\\PhysicsMainLang_PC.loc");
#elif defined(BUILD_P)
	strcat(filename,"\\Compiler\\plugins-user\\PhysicsMainLang_P.loc");
#elif defined(BUILD_C)
	strcat(filename,"\\Compiler\\plugins-user\\PhysicsMainLang_C.loc");
#endif

	//Load whole file into memory
	FILE * file = fopen(filename,"rb");

	if(!file) return stringLoadError("Code 4");


	fseek(file,0,SEEK_END);
	int fsize = ftell(file);
	fseek(file,0,SEEK_SET);
	char * buffer = new char[fsize];
	fread(buffer,fsize,1,file);	
	fclose(file);	
	

	__numErrorMsgs = countItems(buffer,fsize,"[errors]");
	if(__numErrorMsgs!=MAX_PERR_VALUE+1) 
	{
		char msg[256];
		sprintf(msg,"Needed %i error messages, received %i",MAX_PERR_VALUE+1,__numErrorMsgs);
		//MessageBox(0,msg,"Resource String Error",MB_OK);
		SAFEDELETE_ARRAY(buffer);

		return stringLoadError(msg);
	}

	PERR_messages = new char*[__numErrorMsgs];

	if(!loadItems(buffer,fsize,"[errors]", PERR_messages))
	{
		SAFEDELETE_ARRAY(PERR_messages);
		SAFEDELETE_ARRAY(buffer);
		return stringLoadError("Code 5");
	}	

	__numCommandNames = countItems(buffer,fsize,"[commands]");
	if(__numCommandNames!=MAX_PCMD_VALUE+1)
	{
		char msg[256];
		sprintf(msg,"Needed %i command strings, received %i",MAX_PCMD_VALUE+1,__numCommandNames);
		//MessageBox(0,msg,"Resource String Error",MB_OK);
		
		SAFEDELETE_ARRAY(PERR_messages);
		SAFEDELETE_ARRAY(buffer);
		return stringLoadError(msg);
	}

	PCMD_names = new char*[__numCommandNames];

	if(!loadItems(buffer,fsize,"[commands]", PCMD_names))
	{
		for(int i=0;i<__numErrorMsgs;i++)
		{
			SAFEDELETE_ARRAY(PERR_messages[i]);
		}
		SAFEDELETE_ARRAY(PERR_messages);
		SAFEDELETE_ARRAY(PCMD_names);
		SAFEDELETE_ARRAY(buffer);
		return stringLoadError("Code 6");
	}


	SAFEDELETE_ARRAY(buffer);
	
	

	}
	catch(...)
	{
		return stringLoadError("Unknown Exception");
	}

	

	__errorMessagesAvailable=true;
	return true;
}
