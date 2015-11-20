
#include "OggVorbisEncoderPlatform.h"
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <stdio.h>

void setbinmode ( FILE* f )
{
	_setmode ( _fileno ( f ), _O_BINARY );
}
