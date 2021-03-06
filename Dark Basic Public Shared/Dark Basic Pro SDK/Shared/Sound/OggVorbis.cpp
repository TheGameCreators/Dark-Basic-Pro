
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	OGG VORBIS COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )
#define WIN32_LEAN_AND_MEAN

//#include "core.h"
#include <windows.h>
#include "CSoundDataC.h"

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/vorbisenc.h"

//#pragma comment ( lib, "vorbis/lib/ogg_static.lib"        )
//#pragma comment ( lib, "vorbis/lib/vorbis_static.lib"     )
//#pragma comment ( lib, "vorbis/lib/vorbisfile_static.lib" )
//#pragma comment ( lib, "vorbis/lib/vorbisenc_static.lib"  )

// mike - 051005
#pragma comment ( lib, "ogg_static.lib"        )
#pragma comment ( lib, "vorbis_static.lib"     )
#pragma comment ( lib, "vorbisfile_static.lib" )
#pragma comment ( lib, "vorbisenc_static.lib"  )

#define MIN_SOUND	0
#define MAX_SOUND	65535

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

LPDIRECTSOUNDBUFFER* LoadOggVorbis ( DWORD dwFileName );





////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
