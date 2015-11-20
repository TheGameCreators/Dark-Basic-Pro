#ifndef __ENCODE_H
#define __ENCODE_H

#include <stdio.h>
#include "vorbis/codec.h"

typedef long ( *audio_read_func ) ( void* src, float** buffer, int samples );

struct oe_options
{
	char*	encoding;

	char**	title;
	int		title_count;
	char**	artist;
	int		artist_count;
	char**	album;
	int		album_count;
	char**	comments;
	int		comment_count;
	char**	tracknum;
	int		track_count;
	char**	dates;
	int		date_count;
	char**	genre;
	int		genre_count;

	int		quiet;

	int		rawmode;
	int		raw_samplesize;
	int		raw_samplerate;
	int		raw_channels;

	char*	namefmt;
    char*	namefmt_remove;
    char*	namefmt_replace;
	char*	outfile;

	int		min_bitrate;
	int		nominal_bitrate;
	int		max_bitrate;

	float	quality;

	unsigned int serial;
};

struct oe_enc_opt
{
	vorbis_comment*	comments;
	unsigned int	serialno;

	audio_read_func		read_samples;
	
	void*				readdata;

	long				total_samples_per_channel;
	int					channels;
	long				rate;
	int					samplesize;

	int					bitrate;
	int					min_bitrate;
	int					max_bitrate;
	float				quality;

	FILE*				out;
	char*				filename;
	char*				infilename;
};

int oe_encode ( oe_enc_opt* opt );

#endif
