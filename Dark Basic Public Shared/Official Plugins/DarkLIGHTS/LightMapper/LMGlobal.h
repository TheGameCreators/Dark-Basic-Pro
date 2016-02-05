#ifndef LMGLOBAL_H
#define LMGLOBAL_H

#include <windows.h>
#include <stdio.h>

#include "SharedData.h"

//#include <stdlib.h>
//#include <time.h>

extern SharedData *g_pShared;
extern int g_iLightmapFileFormat;

inline int FtoI( float f )
{
	int iValue;
	
	__asm {
		fld f;
		fistp iValue;
	}

	return iValue;
}

#endif