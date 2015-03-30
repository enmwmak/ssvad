/* File: window.c
   Hamming windowing or Rectangular windowing
   Author: M.W.Mak
   Date: 20/2/95
*/

#include <stdio.h>
#include <math.h>
#include "mmalloc.h"
#include "window.h"
#include "veclib.h"

/***************************************************************/
/*      Hamming window                                         */
/*      Output  : h(n) :vec_t                                  */
/***************************************************************/
#ifndef PI
#define PI 3.141592654
#endif
vec_t hamming(int win_size, int n)
{
     if (n<0 || n>=win_size)
	return(0.0);
     return(0.54-0.46 * cos((vec_t)(2*PI*n)/(vec_t)(win_size-1)));
}

/**************************************************************/
/*	WINDOWING : performing windowing on time domain data  */
/*	Input     : iparray,oparray,window_size,window_type   */
/*		    normalized_factor			      */
/*                  iparray[0..winsize-1]                     */
/*      Output    : oparray[0..winsize-1]                     */
/**************************************************************/

void    windowing(short huge *iparray,       /* input array for windowing */
		  vec_t huge *oparray,       /* ouput array after windowing */
		  int	win_size,	/* window size */
		  int   win_type,	/* Hamming or Rectangular */
		  int   nor_factor,	/* normalizing factor */
		  vec_t prem_factor)    /* pre-emphasis factor á */
{
	int	i;
	vec_t   hamming(int,int);
	/******************************************************/
	/* pre_empahsis by 1-áz^-1                            */
	/*          y(n) = x(n) - áx(n-1)                     */
	/******************************************************/
	oparray[0] = (vec_t)iparray[0];
	for (i=1;i<win_size;i++)
	    oparray[i] = (vec_t)iparray[i] - (vec_t)iparray[i-1] * prem_factor;
	if (win_type == 'H')
	   for (i=0;i<win_size;i++)
	       oparray[i] *= hamming(win_size,i) / (vec_t)nor_factor;
	else
	   for (i=0;i<win_size;i++)
	      oparray[i] = oparray[i] / (vec_t)nor_factor;   /* Rectangular window */
}



void    dewindowing(vec_t huge *iparray,       /* input array for windowing */
		    vec_t huge *oparray,       /* ouput array after windowing */
		    int	win_size,	/* window size */
		    int   win_type,	/* Hamming or Rectangular */
		    int   nor_factor	/* normalizing factor */
		    )
{
	int	i;
	vec_t   hamming(int,int);
	/******************************************************/
	/* pre_empahsis by 1-áz^-1                            */
	/*          y(n) = x(n) - áx(n-1)                     */
	/******************************************************/
	if (win_type == 'H') {
	   for (i=0;i<win_size;i++)
   	       oparray[i] = iparray[i] * nor_factor / hamming(win_size,i);
	} else {
	   for (i=0;i<win_size;i++)
	      oparray[i] = oparray[i] * (vec_t)nor_factor;   /* Rectangular window */
	}
}
