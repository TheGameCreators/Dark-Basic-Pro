
#include "OggVorbisEncoderPlatform.h"
#include <fcntl.h>
#include <io.h>

void setbinmode ( FILE* f )
{
	_setmode ( _fileno ( f ), _O_BINARY );
}
