/////////////////////////////////////////////////////////////////
//	Project: denoise
//	Module: findnoise.h
//	Author: Berton LEE, student helper of Dr. M. W. Mak
//	Date: 21/09/2000
//	Purpose:	Finding the average noise spectrum in frequency domain
//				The program processes wave files of 16-bit mono window_PCM wave file format.
//
////////////////////////////////////////////////////////////////////////


#ifndef __FINDNOISE_H__
#define __FINDNOISE_H__

#ifndef  SQR
   #define SQR(a) ((a) * (a))
#endif
#ifndef PI
#define PI 3.141592654
#endif

#include "veclib.h"

vec_t* findnoise(const short* inpwave,		// Input wave file, noise.wav
		 unsigned long num_smps,	// Number of samples in the input wave file
		 const unsigned long frameSize, // Size of speech frame
		 const vec_t bkg_frac);         // Fraction of background frames w.r.t. the whole utt


vec_t* findnoise_from_file_start(const short* inpwave,unsigned long num_smps,const unsigned long frameSize); 


#endif   //__FINDNOISE_H__
