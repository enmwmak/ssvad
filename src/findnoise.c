/////////////////////////////////////////////////////////////////
//	Project: denoise
//	Module: findnoise.c
//	Author: Berton LEE, student helper of Dr. M. W. Mak
//	Date: 21/09/2000
//	Purpose:	Finding the average noise spectrum in frequency domain
//				The program processes wave files of 16-bit mono window_PCM wave file format.
//
////////////////////////////////////////////////////////////////////////



#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "findnoise.h"
#include "fft.h"
#include "veclib.h"
#include "window.h"


vec_t* findnoise(const short* inpwave,		// Input wave file, noise.wav
		 unsigned long num_smps,	// Number of samples in the input wave file
		 const unsigned long frameSize, // Size of speech frame
		 const vec_t bkg_frac)          // Fraction of background frames w.r.t. the whole utt
{
        unsigned long i,k,t,j;			// Index variable
	unsigned long numFrames;		// Number of frames in the noisy speech file
	vec_t *y;				// Background noise in time-domain [0...frameSize-1]
	vec_t *Y;				// Noise spectrum (including real & imaginary parts) 
	vec_t *aveMagY;				// Average magnitude of backgound noise [0...frameSize-1]
	short *s;                               // Buffer storing one frame of speech
	unsigned long num_bkg_frms;             // =500 before 2010; =100 before 2012 Dec; =250 on 2013 Jan.
	                                        // Determined by bkg_frac after March 2013
	short *bkgwav;
	vec_t *a;
	vec_t minamp;
	int minpos;

	/* Find the background frames by looking for nonspeech frames (no frame overlapping).
	   Making sure the no. of bkg frames will not be larger than half the no. of frames */
	numFrames = (num_smps/frameSize);

	/* Determine the no. of background frames based on background fraction (bkg_frac) */
	num_bkg_frms = (unsigned long)((vec_t)numFrames*bkg_frac);
	if (num_bkg_frms > numFrames/2) {
	    num_bkg_frms = numFrames/2;
	}
	bkgwav = (short *)vector(0,num_bkg_frms*frameSize-1,sizeof(short));
	a = (vec_t *)vector(0,numFrames-1,sizeof(vec_t));
	for (i=0; i<numFrames; i++) {
	    j = i*frameSize;
	    a[i] = 0.0;
	    for (t=0; t<frameSize; t++)
		a[i] += (vec_t)abs(inpwave[j+t]);
	    a[i] = a[i]/frameSize;
	}
	for (i=0; i<num_bkg_frms; i++) {
	    minamp = VECminposf(numFrames, a, &minpos);
	    for (t=0; t<frameSize; t++) {
		j = i*frameSize;
		bkgwav[j+t] = inpwave[minpos*frameSize+t];
	    }
	    a[minpos] = 1e38;
	}
	free_vector((char *)a,0,sizeof(vec_t));


	//**************************************************************
	/*
	Objectives:
		- Calculate the number of frames.
		- Allocate memory for background noise in time-domain, background noise 
		  in frequency domain, average spectral magnitude and average spectral phase 
		  angle. Note that the contents of these arrays are different from frame to frame. 
	Hints:
		- Number of frames = number of samples over frame advancement.
		- Use the function "*calloc()" in "stdlib.h" or the "new" operator to allocate memory.
		- The array for storing noise spectra contains both real and imaginary values.
		- Cast all the memory pointers to the correct types.
	*/
	// Allocate background noise array, y[]
	y = (vec_t*)calloc(frameSize,sizeof(vec_t));
	s = (short *)calloc(frameSize,sizeof(short));

	// Allocate background spectrum array, Y[]
	Y = (vec_t*)calloc(frameSize*2,sizeof(vec_t));

	// Allocate array for averaged magnitude of noise spectrum, aveMagY[]
	aveMagY = (vec_t*)calloc(frameSize,sizeof(vec_t));

	//**********************************************************
	/*
	Objectives:
		For each frame, 
			- Apply FFT to compute the magnitude spectrum.
			- Store the average noise magnitude in aveMagY[].
		Deallocate the memory and return the average noise.
	Hints:
		- Use frame advancement (frameAdv) to define the location of each frame.
		  (starting point of each frame)
		- For each frame store the INT16 samples in an array of double.
		- Return 0 if the result of FFT() is false.
		- Compute the magnitude for all frequencies in a frame.
		- Use the defined function "SQR()" in "denoise.h" to compute
		  the magnitude. e.g. SQR(2) = 4. 
		- In Y[], two consecutive elements constitute
		  a complex number and the real part is followed by the imaginary part. 
		  e.g.		Y[0] = real part of Y(0)
					Y[1] = imaginary part of Y(0)
					Y[2] = real part of Y(1)
					  .
					  .
					  .
					Y[k] = real part of Y(k/2)
					Y[k+1] = imaginary part of Y(k/2)
					  .
					  .
					  .
		- Store the average value into an array and sum them up for each frame.
		- Use the function "free()" in "stdlib.h" or the operator delete to deallocate memory.
	*/

	for (i=0; i<num_bkg_frms; i++)
	{
	    // Get one frame
	    for (t=0; t<frameSize; t++)
		s[t] = bkgwav[i*frameSize+t];

	    // Apply Hamming window
	    windowing(s,y,frameSize,'H',1.0,0.0);

	    // Compute FFT-based spectrum
	    if (!(FFT(y, Y, frameSize))) {
		return 0;
	    }

	    // Compute noise spectral magnitude and phase. Note the phase is not necessary
	    for (k=0; k<frameSize; k++) {
		aveMagY[k] += sqrt(SQR(Y[2*k])+SQR(Y[2*k+1]));
	    }
	}
	for (k=0; k<frameSize; k++) {
	    aveMagY[k] /= num_bkg_frms;
	    //printf("%f\n",aveMagY[k]);
	}

	free(y);
	free(Y);
	return aveMagY;

}


vec_t* findnoise_from_file_start(const short* inpwave,		// Input wave file, noise.wav
				 unsigned long num_smps,	// Number of samples in the input wave file
				 const unsigned long frameSize) // Size of speech frame
{
        unsigned long i,k,t;			// Index variable
	vec_t *y;				// Background noise in time-domain [0...frameSize-1]
	vec_t *Y;				// Noise spectrum (including real & imaginary parts) 
	vec_t *aveMagY;				// Average magnitude of backgound noise [0...frameSize-1]
	short *s;                               // Buffer storing one frame of speech
	unsigned long num_bkg_frms = 25;

	// Allocate background noise array, y[]
	y = (vec_t*)calloc(frameSize,sizeof(vec_t));
	s = (short *)calloc(frameSize,sizeof(short));

	// Allocate background spectrum array, Y[]
	Y = (vec_t*)calloc(frameSize*2,sizeof(vec_t));

	// Allocate array for averaged magnitude of noise spectrum, aveMagY[]
	aveMagY = (vec_t*)calloc(frameSize,sizeof(vec_t));

	for (i=0; i<num_bkg_frms; i++)
	{
	    // Get one frame
	    for (t=0; t<frameSize; t++)
		s[t] = inpwave[i*frameSize+t];

	    // Apply Hamming window
	    windowing(s,y,frameSize,'H',1.0,0.0);

	    // Compute FFT-based spectrum
	    if (!(FFT(y, Y, frameSize))) {
		return 0;
	    }

	    // Compute noise spectral magnitude and phase. Note the phase is not necessary
	    for (k=0; k<frameSize; k++) {
		aveMagY[k] += sqrt(SQR(Y[2*k])+SQR(Y[2*k+1]));
	    }
	}
	for (k=0; k<frameSize; k++) {
	    aveMagY[k] /= num_bkg_frms;
	    printf("%f\n",aveMagY[k]);
	}

	free(y);
	free(Y);
	return aveMagY;

}

