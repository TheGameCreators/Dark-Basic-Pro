
#ifndef __AUDIO_H
#define __AUDIO_H

#include "OggVorbisEncoderEncode.h"

struct input_format
{
	int ( *id_func ) ( unsigned char *buf, int len );
	int id_data_len;
	int  ( *open_func ) ( FILE* in, oe_enc_opt* opt, unsigned char * buf, int buflen );
	void ( *close_func ) ( void* );
	char* format;
	char* description;
};

struct wav_fmt
{
	short	format;
	short	channels;
	int		samplerate;
	int		bytespersec;
	short	align;
	short	samplesize;
};

struct wavfile
{
	short	channels;
	short	samplesize;
	long	totalsamples;
	long	samplesread;
	FILE*	f;
	short	bigendian;
};

input_format* open_audio_file ( FILE* in, oe_enc_opt* opt );

int  wav_open  ( FILE* in, oe_enc_opt* opt, unsigned char* buf, int buflen );
int  wav_id    ( unsigned char* buf, int len );
void wav_close ( void* );

long wav_read        ( void*, float** buffer, int samples );
long wav_ieee_read   ( void*, float** buffer, int samples );
long raw_read_stereo ( void*, float** buffer, int samples );

#endif 
