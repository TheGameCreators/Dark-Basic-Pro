#include "stdafx.h"
#include "QDebug.h"
#include "Windows.h"

//Two routines for outputting debug messages


#define _DEBUG_FILE_OUTPUT "C:\\Documents and Settings\\Andy Nicholas\\Desktop\\Debug.txt"

//Debug messages to the output window
void consoleDebugMsg(const char* format, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, format);

	static char buffer[2048];
	_vsnprintf(buffer, sizeof(buffer), format, args);
	OutputDebugString(buffer);

	va_end(args);
#endif
}


//Debug messages to a file
void fileDebugMsg(const char* format, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, format);

	static char buffer[2048];
	_vsnprintf(buffer, sizeof(buffer), format, args);
	
	static AutoFile autofile;

	fprintf(autofile.file,buffer);	

	va_end(args);
#endif
}


AutoFile::AutoFile()
{
	file=fopen(_DEBUG_FILE_OUTPUT,"w+");
}

AutoFile::~AutoFile()
{
	fclose(file);
}
