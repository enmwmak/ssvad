/* 
   Filename	:sph_io.h (formerly wav_io.h)
   Version	:1.0
   Date		:25 April 96
   Author	:M.W.Mak
   Description	:Function prototypes for sph_io.c	
*/   
#ifndef __WAVIO_INCLUDE__
#define __WAVIO_INCLUDE__

#ifndef MTRF_ON
#include <sp/sphere.h>
#endif
/*
#include "segment.h"
*/
#define SP_EOF_ERR  -1
#define SP_FILE_IO_ERR -2
#define SP_FILE_OPEN_ERR -3
#define SP_GET_HFIELD_ERR -4


short *read_wav_file(char *wavfilename, unsigned long *sample_read,
		     SP_INTEGER *byte_per_sample, SP_INTEGER *num_channels,
		     SP_INTEGER *sample_rate, SP_STRING *sample_coding, 
		     char channel_id, int *err_code);


long write_wav_file(char *wavfilename, void *sample, SP_INTEGER num_samples,
		    SP_INTEGER byte_per_sample, SP_INTEGER s_rate);

long write_2ch_wav_file(char *wavfilename, short **sample, SP_INTEGER num_samples,
			SP_INTEGER byte_per_sample, SP_INTEGER s_rate);

void dump_sample(char *pcmfilename, short *sample, unsigned long num_samples);



#endif
