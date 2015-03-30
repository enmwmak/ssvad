/* 
   Filename	:silence.h
   Version	:1.0
   Date		:13 Nov. 97
   Author	:M.W. Mak (enmwmak@polyu.edu.hk)
   Description  :Detect the silence region of a given speech buffer
*/


#ifndef __SILENCE_INCLUDE__
#define __SILENCE_INCLUDE__

#include "veclib.h"
#include "segment.h"

#define FRAME_WIDTH 0.01                   /* Frame width in second */
#define FRAME_RATE 1000                     /* How many frame per second */
#define BACKGROUND_PERIOD 0.1              /* The first 0.1 seconds have no speech */
#define SILENCE     1
#define NONSILENCE -1

vec_t zero_crossing(short *x, unsigned long n);
vec_t average_magnitude(short *x, unsigned long n);
void remove_offset(short *x, unsigned long num_samples);
void FIR_filtering(vec_t *x, unsigned long N, vec_t a0, vec_t a1, vec_t a2);
void moving_average(vec_t *x, unsigned long N, int M);
SEGMENT *detect_silence(short *x, unsigned long num_samples, int sample_rate,
			vec_t zcr_factor, vec_t avm_factor, vec_t noise_energy);

#endif



