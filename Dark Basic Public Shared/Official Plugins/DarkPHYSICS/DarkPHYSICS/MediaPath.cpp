

#if defined(WIN32) || defined(WIN64)
#define NOMINMAX
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "MediaPath.h"

#if defined(_XBOX)

char* FindMediaFile(const char *fname,char *buff)
{
	strcpy(buff,"d:\\");
	strcat(buff,fname);

	return buff;
}

char* GetTempFilePath(char *buff)
{
	strcpy(buff,"d:\\");
	return buff;
}

#elif defined(__CELLOS_LV2__)

#include "sys/paths.h"

char* FindMediaFile(const char *fname,char *buff)
{
	strcpy(buff,SYS_APP_HOME);
	strcat(buff,"/");
	strcat(buff,fname);
	return buff;
}

char* GetTempFilePath(char *buff)
{
	strcpy(buff,SYS_APP_HOME);
	strcat(buff,"/");
	return buff;
}

#elif defined(WIN32) || defined(WIN64)

static bool MediaFileExists(const char *fname)
{
	FILE *fp=fopen(fname,"rb");
	if(fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}

char* FindMediaFile(const char *fname,char *buff)
{
	strcpy(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	GetModuleFileName(NULL, buff, MAX_PATH);
	char* pTmp = strrchr(buff, '\\');
    if(pTmp != NULL){
        buff[pTmp-buff+1]=0;
		strcat(buff, fname);
		if(MediaFileExists(buff))
			return buff;
	}
	
	strcpy(buff,"..\\..\\Samples\\Media\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff,"..\\..\\..\\Media\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff,"..\\..\\Media\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	strcpy(buff,"Samples\\Media\\");
	strcat(buff,fname);
	if(MediaFileExists(buff))
		return buff;

	printf("Can't find media file: %s\n", fname);

	strcpy(buff,fname);
	return buff;
}

char* GetTempFilePath(char *buff)
{
	strcpy(buff,"");
	return buff;
}

#endif
