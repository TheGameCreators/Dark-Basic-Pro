
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "OggVorbisEncoderPlatform.h"
#include "OggVorbisEncoderEncode.h"
#include "OggVorbisEncoderAudio.h"

int Encode ( char* szInput, char* szOutput, float fQuality )
{
	oe_options OggOptions =	{
								"ISO-8859-1",
								NULL,
								0,
								NULL,
								0,
								NULL,
								0,
								NULL,
								0,
								NULL, 
								0,
								NULL,
								0,
								NULL,
								0,
								0,
								0,
								16,
								44100,
								2,
								NULL,
								DEFAULT_NAMEFMT_REMOVE, 
								DEFAULT_NAMEFMT_REPLACE,
								NULL,
								-1,
								-1,
								-1,
								-1,0
							};

	int  i;
	char infiles [ 10 ] [ 256 ];
	int	 numfiles;
	int  errors = 0;

	setlocale ( LC_ALL, "" );

	OggOptions.outfile         = szOutput;
	OggOptions.raw_samplerate  = 44100;
	OggOptions.serial          = 0;
	OggOptions.nominal_bitrate = -1;
	OggOptions.min_bitrate     = -1;
	OggOptions.max_bitrate     = -1;
	OggOptions.quality         = 0.1f;
	numfiles            = 1;

	strcpy ( &infiles [ 0 ] [ 0 ], szInput );

	if ( OggOptions.serial == 0 )
		OggOptions.serial = rand ( );
	
	for ( i = 0; i < numfiles; i++ )
	{
		oe_enc_opt      enc_opts;
		vorbis_comment  vc;
		char*			out_fn = NULL;
		FILE* in			= NULL;
		FILE* out	= NULL;
		int foundformat = 0;
		int closeout = 0, closein = 0;
		char *artist=NULL, *album=NULL, *title=NULL, *track=NULL;
        char *date=NULL, *genre=NULL;
		input_format *format;

		enc_opts.serialno        = OggOptions.serial++;
		
		memset ( &vc, 0, sizeof ( vc ) );

		in = fopen ( infiles [ i ], "rb" );

		if ( in == NULL )
		{
			free ( out_fn );
			errors++;
			continue;
		}

		closein = 1;
		
		format = open_audio_file ( in, &enc_opts );
		
		if ( format )
			foundformat = 1;
		
		if ( !foundformat )
		{
			if ( closein )
				fclose ( in );

			errors++;
			continue;
		}

		if ( OggOptions.outfile )
			out_fn = strdup ( OggOptions.outfile );
		
		out = fopen ( out_fn, "wb" );

		if ( out == NULL )
		{
			if ( closein )
				fclose ( in );

			errors++;
			free ( out_fn );
			continue;
		}

		closeout = 1;

		enc_opts.out         = out;
		enc_opts.comments    = &vc;
		enc_opts.filename    = out_fn;
		enc_opts.infilename  = infiles [ i ];
		enc_opts.bitrate     = OggOptions.nominal_bitrate; 
		enc_opts.min_bitrate = OggOptions.min_bitrate;
		enc_opts.max_bitrate = OggOptions.max_bitrate;
		enc_opts.quality     = OggOptions.quality;

		if ( oe_encode ( &enc_opts ) )
			errors++;

		if ( out_fn )
			free ( out_fn );

		vorbis_comment_clear ( &vc );

		if ( !OggOptions.rawmode ) 
			format->close_func ( enc_opts.readdata );

		if ( closein )
			fclose ( in );

		if ( closeout )
			fclose ( out );
	}

	return errors ? 1 : 0;
}