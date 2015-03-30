/////////////////////////////////////////////////////////////////
//	Project: denoise
//	Module: denoise.h
//	Author: Berton LEE, student helper of Dr. M. W. Mak
//	Date: 21/09/2000
//	Purpose: Recovering clean speech in a noisy speech by spectral subtraction
//			and overlaying rectangular windowing techniques.
//			The program processes wave files of 16-bit mono window_PCM wave file format.
//
////////////////////////////////////////////////////////////////////////


#ifndef __DENOISE_H__
#define __DENOISE_H__

#ifndef  SQR
   #define SQR(a) ((a) * (a))
#endif
#ifndef PI
#define PI 3.141592654
#endif

short* denoise(const short* nspeech, const unsigned long num_smps, const unsigned long frameSize, 
			   const unsigned long frameAdv, unsigned long* nOutSmps, const double* noise,
			   const double alphaMax, const double alphaMin,
			   const double betaMax, const double betaMin);

#endif	//__DENOISE_H__
