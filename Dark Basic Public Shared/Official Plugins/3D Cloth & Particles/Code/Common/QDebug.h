#ifndef __QUICKDEBUG_H
#define __QUICKDEBUG_H

#include "stdio.h"


#ifdef DEBUG_TEXT_OUTPUT
#define DEBUG_OUT(x) OutputDebugString(x)
#else
#define DEBUG_OUT(x)
#endif


void consoleDebugMsg(const char* format, ...);
void fileDebugMsg(const char* format, ...);

class AutoFile
{
public:
	AutoFile();
	~AutoFile();
	FILE * file;
};

#endif
