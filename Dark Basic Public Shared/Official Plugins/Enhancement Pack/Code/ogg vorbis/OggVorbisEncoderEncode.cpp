
#include "OggVorbisEncoderPlatform.h"
#include "vorbis/vorbisenc.h"
#include "OggVorbisEncoderEncode.h"

#define READSIZE 1024

int oe_write_page ( ogg_page* page, FILE* fp );

int oe_encode ( oe_enc_opt* opt )
{
	ogg_stream_state os;
	ogg_page 		 og;
	ogg_packet 		 op;

	vorbis_dsp_state vd;
	vorbis_block     vb;
	vorbis_info      vi;

	long	samplesdone   = 0;
    int		eos;
	long	bytes_written = 0;
	long	packetsdone   = 0;
	int		ret           = 0;
	
	vorbis_info_init ( &vi );

	if ( opt->quality >= 0.0f )
	{
		if ( vorbis_encode_init_vbr ( &vi, opt->channels, opt->rate, opt->quality ) )
		{
			vorbis_info_clear ( &vi );
			return 1;
		}
	}
	else
	{
		if ( vorbis_encode_init ( 
									&vi,
									opt->channels,
									opt->rate,
									opt->max_bitrate > 0 ? opt->max_bitrate * 1000 : -1,
									opt->bitrate * 1000, 
									opt->min_bitrate > 0 ? opt->min_bitrate * 1000 : -1
								) )
		{
			vorbis_info_clear ( &vi );
			return 1;
		}
	}

	vorbis_analysis_init ( &vd, &vi );
	vorbis_block_init    ( &vd, &vb );

	ogg_stream_init ( &os, opt->serialno );

	ogg_packet header_main;
	ogg_packet header_comments;
	ogg_packet header_codebooks;
	int result;

	vorbis_analysis_headerout ( &vd,opt->comments, &header_main, &header_comments, &header_codebooks );

	ogg_stream_packetin ( &os, &header_main );
	ogg_stream_packetin ( &os, &header_comments );
	ogg_stream_packetin ( &os, &header_codebooks );

	while ( ( result = ogg_stream_flush ( &os, &og ) ) )
	{
		if ( !result )
			break;
		
		ret = oe_write_page ( &og, opt->out );

		if ( ret != og.header_len + og.body_len )
		{
			ret = 1;
			goto cleanup;
		}
		else
			bytes_written += ret;
	}
	
	eos = 0;

	while ( !eos )
	{
		float** buffer       = vorbis_analysis_buffer ( &vd, READSIZE );
		long    samples_read = opt->read_samples ( opt->readdata, buffer, READSIZE );

		if ( samples_read == 0 )
			vorbis_analysis_wrote ( &vd, 0 );
		else
		{
			samplesdone += samples_read;

			vorbis_analysis_wrote ( &vd, samples_read );
		}

		while ( vorbis_analysis_blockout ( &vd, &vb ) == 1 )
		{
			vorbis_analysis         ( &vb, NULL );
			vorbis_bitrate_addblock ( &vb );

			while ( vorbis_bitrate_flushpacket ( &vd, &op ) )
			{
				ogg_stream_packetin ( &os,&op );
				packetsdone++;

				while ( !eos )
				{
					int result = ogg_stream_pageout ( &os, &og );

					if ( !result )
						break;

					ret = oe_write_page ( &og, opt->out );

					if ( ret != og.header_len + og.body_len )
					{
						ret = 1;
						goto cleanup;
					}
					else
						bytes_written += ret; 
	
					if ( ogg_page_eos ( &og ) )
						eos = 1;
				}
			}
		}
	}

	ret = 0;

cleanup:

	ogg_stream_clear ( &os );

	vorbis_block_clear ( &vb );
	vorbis_dsp_clear   ( &vd );
	vorbis_info_clear  ( &vi );

	return ret;
}

int oe_write_page ( ogg_page* page, FILE* fp )
{
	int written;

	written  = fwrite ( page->header, 1, page->header_len, fp );
	written += fwrite ( page->body,   1, page->body_len,   fp );

	return written;
}