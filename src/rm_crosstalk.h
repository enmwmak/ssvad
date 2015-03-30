/* 
   Filename	:rm_crosstalk.h
   Date		:29-1-2013
   Author	:M.W.Mak
   Description	:Function prototypes for rm_crosstalk.c	
*/   

#ifndef __RM_CROSSTALK_INCLUDE__
#define __RM_CROSSTALK_INCLUDE__

#include "veclib.h"

SEGMENT *remove_crosstalk_SRE10(SEGMENT *seg1, SEGMENT *seg2, unsigned long num_samples, char channel);
SEGMENT *remove_crosstalk_SRE12(SEGMENT *seg1, SEGMENT *seg2, unsigned long num_samples, char channel);

#endif
