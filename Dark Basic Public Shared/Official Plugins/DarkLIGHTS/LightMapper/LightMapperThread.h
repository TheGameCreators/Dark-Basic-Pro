#ifndef LIGHTMAPPERTHREAD_H
#define LIGHTMAPPERTHREAD_H

#include "Thread.h"
#include <time.h>

DLLEXPORT extern void LMBuildLightMaps( int iTexSize, float fQuality, int iBlur, int iNumThreads );

class LightMapperThread : public Thread
{
	
private:

public:

	int iTexSize;
	float fQuality;
	int iBlur;
	int iNumThreads;

	LightMapperThread( ) { iTexSize = 0; fQuality = 1.0f; iBlur = 1; iNumThreads = 0; }
	~LightMapperThread( ) { }

	unsigned Run( );

};

unsigned LightMapperThread::Run( )
{
	srand( (unsigned int) time(0) );
	LMBuildLightMaps( iTexSize, fQuality, iBlur, iNumThreads );

	return 0;
}

#endif