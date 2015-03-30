/* 
   Filename	:window.h
   Version	:1.0
   Date		:25 April 96
   Author	:M.W.Mak
   Description	:Function prototypes for window.c	
*/   

#ifndef __WINDOW_INCLUDE__
#define __WINDOW_INCLUDE__

#include "veclib.h"

void windowing(short *iparray,vec_t *oparray,int win_size,
	       int win_type,int nor_factor,vec_t prem_factor);   

vec_t hamming(int win_size, int num_points);

void    dewindowing(vec_t huge *iparray,       /* input array for windowing */
		    vec_t huge *oparray,       /* ouput array after windowing */
		    int	win_size,	/* window size */
		    int   win_type,	/* Hamming or Rectangular */
		    int   nor_factor	/* normalizing factor */
		    );


#endif
