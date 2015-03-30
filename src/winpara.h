/* Filename: winpara.h (formerly speech.h)
   Version: 1.0
   Date: 10 Nov. 95
   Author:M.W.Mak
*/   
#ifndef __WINPARA_INCLUDE__
#define __WINPARA_INCLUDE__

/* Define delta cepstrum window size */
#define DCEP_WIZE 7

typedef struct {
	 int	win_size;		/* window size */
	 char   win_type;		/* window type, 'H' for hamming, 'R' for rectangular */
	 int    win_adv;		/* number of sample points to be advanced in the next frame */
	 float  pre_emp;		/* pre-emphasis factor */
}WINPARA;


#endif


