/*
  File		:segment.h
  Author	:M.W. Mak
  Date		:30 April 96
  Last update	:
  Description	:This module extracts the non-silence speech from the sample[]
 		 array using the seg structure.
*/

#ifndef __SEGMENT_INCLUDED__
#define __SEGMENT_INCLUDED__

#include "veclib.h"

typedef struct {
	int	begin;			/* Position of begnining of segment */
	int	end;			/* Position of end of segment */
	int	num_samples;		/* Number of samples in the segment */
	int	num_segs;
	char	phoneme[30];            /* Need more space because of SPIDRE .mrk files */
        vec_t   mean_namp;              /* Mean normalized amplitude of current segment */
}SEGMENT;



short *extract_sample(SEGMENT *seg, short *sample, unsigned long *num_samples);

SEGMENT *PhnFileRead(char *PhnFileName);
void PhnFileWrite(char *phnfilename, SEGMENT *seg);

SEGMENT *MrkFileRead(char *MrkFileName, char channel, int freq);

int remove_silence_frames(SEGMENT *seg, int frame_size, int frame_adv, int N, int D, vec_t **x);

vec_t **get_sph_frames(vec_t **Y, SEGMENT *seg, unsigned int *num_frms, int sampPeriod, int sampleFreq);

void print_seg_info(SEGMENT *seg, int wininc, unsigned long num_samples);

short *extract_sph_sample(SEGMENT *seg, short *sample, unsigned long *num_samples);

short *extract_bkg_sample(SEGMENT *seg, short *sample, unsigned long *num_samples);


#endif
