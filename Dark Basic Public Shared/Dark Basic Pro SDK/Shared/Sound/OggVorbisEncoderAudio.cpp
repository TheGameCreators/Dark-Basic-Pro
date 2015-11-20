
#include <stdlib.h>
#include <string.h>
#include "OggVorbisEncoderAudio.h"
#include "OggVorbisEncoderPlatform.h"

#define WAV_HEADER_SIZE 44

#define READ_U32_LE(buf) (((buf)[3]<<24)|((buf)[2]<<16)|((buf)[1]<<8)|((buf)[0]&0xff))
#define READ_U16_LE(buf) (((buf)[1]<<8)|((buf)[0]&0xff))
#define READ_U32_BE(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|((buf)[3]&0xff))
#define READ_U16_BE(buf) (((buf)[0]<<8)|((buf)[1]&0xff))


input_format formats [ ] =
							{
								{ wav_id, 12, wav_open, wav_close, "wav", "WAV file reader" },
								{ NULL,    0, NULL,     NULL,      NULL,  NULL }
							};

input_format* open_audio_file ( FILE* in, oe_enc_opt* opt )
{
	int				j          = 0;
	unsigned char*	buf        = NULL;
	int				buf_size   = 0;
	int				buf_filled = 0;
	int				size;
	int				ret;

	while ( formats [ j ].id_func )
	{
		size = formats [ j ].id_data_len;

		if ( size >= buf_size )
		{
			buf      = ( unsigned char* ) realloc ( buf, size );
			buf_size = size;
		}

		if ( buf_size > buf_filled )
		{
			ret         = fread ( buf + buf_filled, 1, buf_size - buf_filled, in );
			buf_filled += ret;

			if ( buf_filled != buf_size )
			{
				buf_size = buf_filled;
				j++;

				continue;
			}
		}

		if ( formats [ j ].id_func ( buf, size ) )
		{
			if ( formats [ j ].open_func ( in, opt, buf, size ) )
				return &formats [ j ];
		}

		j++;
	}

	return NULL;
}

static int seek_forward ( FILE* in, int length )
{
	if ( fseek ( in, length, SEEK_CUR ) )
	{
		unsigned char	buf [ 1024 ];
		int				seek_needed = length;
		int				seeked;

		while ( seek_needed > 0 )
		{
			seeked = fread ( buf, 1, seek_needed > 1024 ? 1024 : seek_needed, in );
			
			if ( !seeked )
				return 0;
			else
				seek_needed -= seeked;
		}
	}

	return 1;
}

static int find_wav_chunk ( FILE* in, char* type, unsigned int* len )
{
	unsigned char buf [ 8 ];

	while ( 1 )
	{
		if ( fread ( buf, 1, 8, in ) < 8 )
			return 0;
		
		if ( memcmp ( buf, type, 4 ) )
		{
			*len = READ_U32_LE ( buf + 4 );

			if ( !seek_forward ( in, *len ) )
				return 0;

			buf [ 4 ] = 0;
		}
		else
		{
			*len = READ_U32_LE ( buf + 4 );
			return 1;
		}
	}
}

int wav_id ( unsigned char* buf, int len )
{
	unsigned int flen;
	
	if ( len < 12 )
		return 0;

	if ( memcmp ( buf, "RIFF", 4 ) )
		return 0;

	flen = READ_U32_LE ( buf + 4 );

	if ( memcmp ( buf + 8, "WAVE",4 ) )
		return 0;

	return 1;
}

int wav_open ( FILE* in, oe_enc_opt* opt, unsigned char* oldbuf, int buflen )
{
	unsigned char	buf [ 16 ];
	unsigned int	len;
	int				samplesize;
	wav_fmt			format;
	wavfile*		wav = ( wavfile* ) malloc ( sizeof ( wavfile ) );

	if ( !find_wav_chunk ( in, "fmt ", &len ) )
		return 0;

	if ( len < 16 )
		return 0;
	
	if ( fread ( buf, 1, 16, in ) < 16 )
		return 0;
	
	if ( len - 16 > 0 && !seek_forward ( in, len-16 ) )
	    return 0;

	format.format      = READ_U16_LE ( buf      ); 
	format.channels    = READ_U16_LE ( buf + 2  ); 
	format.samplerate  = READ_U32_LE ( buf + 4  );
	format.bytespersec = READ_U32_LE ( buf + 8  );
	format.align       = READ_U16_LE ( buf + 12 );
	format.samplesize  = READ_U16_LE ( buf + 14 );

	if ( !find_wav_chunk ( in, "data", &len ) )
		return 0;

	if ( format.format == 1 )
	{
		samplesize        = format.samplesize / 8;
		opt->read_samples = wav_read;
	}
	else if ( format.format == 3 )
	{
		samplesize        = 4;
		opt->read_samples = wav_ieee_read;
	}
	else
		return 0;
	
	if ( format.align == format.channels * samplesize && format.samplesize == samplesize * 8 )
	{
		opt->rate        = format.samplerate;
		opt->channels    = format.channels;

		wav->f           = in;
		wav->samplesread = 0;
		wav->bigendian   = 0;
		wav->channels    = format.channels; 
		wav->samplesize  = format.samplesize;

		if ( len )
        {
			opt->total_samples_per_channel = len / ( format.channels * samplesize );
		}
		else
		{
			long pos = ftell ( in );

			if ( fseek ( in, 0, SEEK_END ) == -1 )
			{
				opt->total_samples_per_channel = 0;
			}
			else
			{
				opt->total_samples_per_channel = ( ftell ( in ) - pos ) / ( format.channels * samplesize );
				fseek ( in, pos, SEEK_SET );
			}
		}
		
		wav->totalsamples = opt->total_samples_per_channel;
		opt->readdata     = ( void* ) wav;
		
		return 1;
	}
	else
	{
		return 0;
	}
}

long wav_read ( void* in, float** buffer, int samples )
{
	wavfile*		f          = ( wavfile* ) in;
	int				sampbyte   = f->samplesize / 8;
	signed char*	buf        = new signed char [ samples * sampbyte * f->channels ];

	long			bytes_read = fread ( buf, 1, samples * sampbyte * f->channels, f->f );
	int				i;
	int				j;
	long			realsamples;

	if ( f->totalsamples && f->samplesread + bytes_read / ( sampbyte * f->channels ) > f->totalsamples )
		bytes_read = sampbyte * f->channels * ( f->totalsamples - f->samplesread );

	realsamples     = bytes_read / ( sampbyte * f->channels );
	f->samplesread += realsamples;
		
	if ( f->samplesize == 8 )
	{
		unsigned char* bufu = ( unsigned char* ) buf;
		
		for ( i = 0; i < realsamples; i++ )
		{
			for ( j = 0; j < f->channels; j++ )
			{
				buffer [ j ] [ i ] = ( ( int ) ( bufu [ i * f->channels + j ] )-128 ) / 128.0f;
			}
		}
	}
	else
	{
		if ( !f->bigendian )
		{
			for ( i = 0; i < realsamples; i++ )
			{
				for ( j = 0; j < f->channels; j++ )
				{
					buffer [ j ] [ i ] = ( ( buf [ i * 2 * f->channels + 2 * j + 1 ] << 8 ) | ( buf [ i * 2 * f->channels + 2 * j ] & 0xff ) ) / 32768.0f;
				}
			}
		}
		else
		{
			for ( i = 0; i < realsamples; i++ )
			{
				for ( j = 0; j < f->channels; j++ )
				{
					buffer [ j ] [ i ] = ( ( buf [ i * 2 * f->channels + 2 * j ] << 8 ) | ( buf [ i * 2 * f->channels + 2 * j + 1 ] & 0xff ) ) / 32768.0f;
				}
			}
		}
	}

	return realsamples;
}

long wav_ieee_read ( void* in, float** buffer, int samples )
{
	wavfile*	f          = ( wavfile* ) in;
	float*		buf        = new float [ samples * 4 * f->channels ];

	long		bytes_read = fread ( buf, 1, samples * 4 * f->channels, f->f );
	int			i;
	int			j;
	long		realsamples;

	if ( f->totalsamples && f->samplesread + bytes_read / ( 4 * f->channels ) > f->totalsamples )
		bytes_read = 4 * f->channels * ( f->totalsamples - f->samplesread );

	realsamples     = bytes_read / ( 4 * f->channels );
	f->samplesread += realsamples;

	for ( i = 0; i < realsamples; i++ )
	{
		for ( j = 0; j < f->channels; j++ )
			buffer [ j ] [ i ] = buf [ i * f->channels + j ];
	}

	return realsamples;
}

void wav_close ( void* info )
{
	wavfile* f = ( wavfile* )info;

	free ( f );
}
