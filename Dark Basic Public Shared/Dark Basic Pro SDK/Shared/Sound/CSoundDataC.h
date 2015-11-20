#ifndef _CSOUNDDATA_H_
#define _CSOUNDDATA_H_

//#define INITGUID
//#include <dxerr8.h>
//#include <cguid.h>
//#include < D3DX8.h >
#include < D3DX9.h >
#include "dsutil.h"

//#define DARKSDK __declspec ( dllexport )

struct sSoundData
{
	CSound*				        pSound;
	IDirectSound3DBuffer8*		pDSBuffer3D;

	bool						b3D;
	bool						bPlaying;
	bool						bLoop;
	bool						bPause;
	int							iVolume;
	int							iPan;
	bool						bRecorded;

	int							iLoopStartPos;
	int							iLoopEndPos;


	D3DXVECTOR3					vecPosition;
	D3DXVECTOR3					vecLast;

	sSoundData ( )
	{
		pSound	     = NULL;
		pDSBuffer3D  = NULL;

		b3D     = false;
		bLoop   = false;
		bPause  = false;
		iVolume = 0;
		iPan    = 0;
		bRecorded = false;

		vecPosition         = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
		vecLast				= D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	}

	~sSoundData ( )
	{
		if(pSound) { delete pSound; pSound=NULL; }
		if(pDSBuffer3D) { pDSBuffer3D->Release(); pDSBuffer3D=NULL; }
	}
};

#endif _CSOUNDDATA_H_