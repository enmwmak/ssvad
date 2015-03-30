/////////////////////////////////////////////////////////////////
//  File        : denoise.c
//  Author      : Dr. M. W. Mak
//  Date        : 21/09/2000
//  Purpose     : Recovering clean speech in a noisy speech by spectral subtraction
//	 	  and overlaying rectangular windowing techniques.
//		  The program processes wave files of 16-bit mono window_PCM wave file format.
//
//  Notes       : The parameter 'alpha' is an overestimation factor and 'beta' is
//                the noise floor controlling the musical noise. See labsheets for details 
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "fft.h"
#include "denoise.h"
#include "veclib.h"
#include "window.h"

#define FS 512
#define PI 3.141592654

short* denoise(const short* noisySpeech,	// Input wave file, noisy.wav
	       const unsigned long num_smps,	// Number of samples in the input wave file
	       const unsigned long frameSize,	// No. of samples per frame
	       const unsigned long frameAdv,	// No. of samples for frame shift
	       unsigned long* nOutSmps,		// Number of output samples
	       const vec_t* noise,		// Array for storing the noise spectrum
	       const vec_t alphaMax,		// Parameters for spectral subtraction (see lab sheets for details)
	       const vec_t alphaMin,		
	       const vec_t betaMax, 
	       const vec_t betaMin)
{
        unsigned long t,k;			// Index variable
	unsigned long frame;			// Index to the current frame
	unsigned long frameStart;		// Index to the start of current frame
	unsigned long frameEnd;			// Index to the end of current frame
	unsigned long numFrames;		// Number of frames in the noisy speech file
	vec_t *y;				// Noisy speech in time-domain [0...frameSize-1]
 	vec_t *xhat;				// Reconstructed speech in time-domain [0...frameSize-1]
	vec_t *Y;				// Noisy speech spectrum (including real & imaginary parts)
	vec_t *Xhat;				// Reconstructed spectrum (including real & imaginary parts)
	vec_t *magY, *phaseY;			// Magnitude & phase of each freq bin [0...frameSize-1]
	vec_t *tempOut;				// Temp output containing the middle section of each frame
	vec_t norm;				// Normalising factor
	short *_16bitData;			// 16-bit wave data array for output to wave file [0...nOutSmps-1]
	vec_t alpha, beta;			// Parameters for spectral subtraction
	vec_t snr;				// Signal to background noise ratio
	vec_t signalEnergy, noiseEnergy;        // Energy of signal and noise
	short *s;                               // Buffer storing one frame of speech
	short sig_peak;                         // Peak amplitude of signal
	unsigned long offset;                   // To compensate for the offset due to frame processing

	//*******************************************************
	/*
	Objectives:
		- Calculate the number of frames and the number of output
		  samples.
		- Allocate memory for storing (1) background noise & reconstructed signal in 
		  time-domain, (2) background noise spectrum & reconstructed spectrum, (3) spectral 
		  magnitude and spectral phase angle, and (4) temporary output wave signal for 
		  packing the reconstructed frames. Note that the contents of these arrays are 
		  different from frame to frame.
	*/
	numFrames = num_smps/frameAdv-((frameSize/frameAdv)-1);

	// Allocate array for storing the current frame of noisy speech, y[]
	y = (vec_t *)calloc(frameSize,sizeof(vec_t));
	s = (short *)calloc(frameSize,sizeof(short));

	// Allocate array for storing the current reconstructed signal, xhat[]
	xhat = (vec_t*)calloc(frameSize,sizeof(vec_t));

	// Allocate array for storing the background spectrum of the current frame, Y[]
	Y = (vec_t*)calloc(frameSize*2,sizeof(vec_t));

	// Allocate array for storing the reconstructed spectrum of the current frame, Xhat[] 
	Xhat = (vec_t*)calloc(frameSize*2,sizeof(vec_t));

	// Allocate array for storing the magnitude of noise spectrum, magY[]
	magY = (vec_t*)calloc(frameSize,sizeof(vec_t));

	// Allocate array for storing the phase angle of noise spectrum, phaseY[]
	phaseY = (vec_t*)calloc(frameSize,sizeof(vec_t));

	offset = frameSize/2 - frameAdv/2;
	*nOutSmps = numFrames*frameAdv+offset;

	// Allocate temporary output signal, tempOut[]
	tempOut = (vec_t*)calloc(*nOutSmps,sizeof(vec_t));

	//******************************************************
	/*
	Objectives:
		- Apply a Hamming window and FFT to each frame, then compute the
		  magnitude spectrum and the phase spectrum for each frame.
	Hints:
		- Use frame advancement (frameAdv) to define the location of
		  each frame. (starting point of each frame)
		- For each frame store the INT16 samples in an array of double.
		- Use the following code to apply hamming window
		  "windowing(y,y,frameSize,'H',1.0,0.0,1);" where y[] contains the time-domain
		  samples of the current frame.
		- Return null if the result of FFT() is false.
		- Compute the magnitude and the phase angle for all frequencies in a frame.
		- Use the function "SQR()" in "denoise.h" to compute the magnitude 
		  for all frequencies in a frame. e.g. SQR(2) = 4. 
		- In Y[], the two consecutive
		  elements constitute a complex number, and the real part is followed by the
		  imaginary part.
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
				Y[frameSize*2-2] = real part of Y(frameSize-1)
				Y[frameSize*2-1] = imagineary part of Y(frameSize-1)
		- To compute the phase angle of complex number z=a+jb, apply atan|b/a|. Then, we
		  shift the phase angle back to the correct quadrant. This can be determined by 
		  the sign of the real and the imaginary parts of Y.
		  e.g. real < 0 & imaginary > 0 means that the angle is in the 2nd quadrant. 
		  Therefore, the correct phase = PI - phase. Note that the phase angle equals
		  to pi/2 if the real part is zero and the imaginary part is greater than zero. 
	*/
	*nOutSmps = 0;
	noiseEnergy = VECsumf(frameSize,noise);
	for (frame=0; frame<numFrames; frame++)
	{
	    frameStart = frame*frameAdv;
	    frameEnd=frameStart+frameSize-1;

	    // Get one frame
	    for (k=0; k<frameSize; k++)
		s[k] = noisySpeech[frameStart+k];

	    // Apply Hamming window
	    windowing(s,y,frameSize,'H',1.0,0.0);

	    // Compute FFT-based spectrum
	    if (!(FFT(y, Y, frameSize))) {
		return NULL;
	    }

	    // Compute magnitude and phase
	    for (k=0; k<frameSize; k++) {
		magY[k] = sqrt(SQR(Y[2*k])+SQR(Y[2*k+1]));
		if (Y[2*k]) {
		    phaseY[k] = atan(fabs(Y[2*k+1])/fabs(Y[2*k]));
		    if (Y[2*k]<0 && Y[2*k+1]>0)			// 2nd quadrant
			phaseY[k] = PI - phaseY[k];
		    if (Y[2*k]<0 && Y[2*k+1]<0)			// 3rd quadrant
			phaseY[k] = phaseY[k] - PI;
		    if (Y[2*k]>0 && Y[2*k+1]<0)			// 4rd quadrant
			phaseY[k] = -phaseY[k];
		} else {
		    if (Y[2*k+1]>0)
			phaseY[k] = PI/2;
		    else if (Y[2*k+1]<0)
			phaseY[k] = -PI/2;
		    else
			phaseY[k] = 0;
		}
	    }

	    //***********************************************************
	    /*
		Objectives:
			- Determine the SNR for each frame.
			- Determine the over subtraction factor based on the SNR.
		Hints:
			- Use the function "VECsumf()" in "vector.cpp" to calculate 
			  the signal & noise energy.
			- Beta = betaMin if SNR < 1. Otherwise, beta = betaMax
			- Given alpha = -0.5*SNR + 4.5
			- The range of alpha is between alphaMax and alphaMin. 
			  The value of alpha is set to alphaMax if it is larger than
			  alphaMax.
	    */
	    signalEnergy = VECsumf(frameSize,magY);
	    if (noiseEnergy)
		snr = signalEnergy/noiseEnergy;
	    else
		snr = 0;
	    if (snr < 1.0)
		beta = betaMin;
	    else
		beta = betaMax;
	    alpha = -0.5*snr+4.5;		// Note that using 4.5 produce better sound quality than using 2.5
	    
	    if (alpha > alphaMax) alpha = alphaMax;
	    if (alpha < alphaMin) alpha = alphaMin;

	    //************************************************************
	    /*
		Objectives:
			- Apply spectral subtraction with musical noise minimization
			  for each frame.
		Hints:
			- Implement the magnitude part of Equation 2 in the lab sheet.
			- You may use magY[] to store the noise-subtracted magnitude
			  spectrum
	    */

	    // Perform spectral subtraction, with musical noise minimization
	    for (k=0; k<frameSize; k++) {
		if (magY[k] > (alpha+beta)*noise[k]) {
		    magY[k] = magY[k] - alpha*noise[k];
		    //printf("Frame %ld: upper f=%ld snr=%.2f a=%.2f b=%.2f ",frame,k,snr,alpha,beta);
		} else {
		    magY[k] = beta*noise[k];
		    //printf("Frame %ld: lower f=%ld snr=%.2f a=%.2f b=%.2f ",frame,k,snr,alpha,beta);
		}
		if (magY[k]<0) {
		    magY[k] = 0;
		}
		//printf("%f\n",magY[k]);
	    }

	    //***************************************************
	    /*
		Objectives:
			For each frame,
				- Reconstruct the complex frequency spectrum of denoised speech (Xhat[])
				  for IFFT.
				- Apply IFFT and dehamming window.
				- Pack the reconstructed time-domain signal and count the number of 
				  samples packed.
		Hints:
			- Return NULL if the IFFT value is false.
			- Use the following code to apply dehamming window 
			  "windowing(xhat,xhat,frameSize,'H',1.0,0.0,0);" where xhat[] contains
			  the denoise samples of the current frame.
			- Pack the frames based on the amount of frame overlapping (see Lab sheet
			  for details).
			- Sum up the size of all the packed frame, it's the size for the 16-bit 
			  wave data array in the next section.
	    */

	    // reconstructing the complex array
	    for (k=0; k<frameSize; k++) {
		Xhat[2*k]=magY[k]*cos(phaseY[k]);
		Xhat[2*k+1]=magY[k]*sin(phaseY[k]);
	    }

	    // carrying out IFFT
	    if (!(IFFT(Xhat, xhat, frameSize))) {
		return NULL;
	    }

	    // Dehamming
	    dewindowing(xhat,xhat,frameSize,'H',1.0);

	    // Packing frames
	    for (k=0; k<frameAdv; k++)
		tempOut[frame*frameAdv+k+offset] = xhat[k+(frameSize/2)-(frameAdv/2)];

	    // Number of samples packed so far
	    *nOutSmps += frameAdv;
	}
	
	//**********************************************
	/*
	Objectives:
		- Determine the normalising factor
		- Store the temporary output signal to a 16-bit array.
		- Deallocate memory and return the 16-bit array.
	Hints:
		- Allocate the requested memroy for the 16-bit wave
		  data by using the function "*calloc()" in "stdlib.h". The 
		  size of this array is the number of output samples 
		  Remember to cast this array to the correct types.
		- The 16-bit data wave is the temporary output signal 
		  times the normalising factor.
		- Use the function "free()" in "stdlib.h" or the operator "delete" 
		  to deallocate the requested memory.
	*/
	/* Determine the peak value of noisy speech */
	for (sig_peak=0,t=0; t<num_smps; t++) {
	    if (abs(noisySpeech[t])>sig_peak) {
		sig_peak=abs(noisySpeech[t]);
	    }
	}
	// normalising factor
	norm = sig_peak/VECamaxf((int)*nOutSmps,tempOut);

	// allocating the array for 16-bit wave data
	_16bitData = (short*)calloc(*nOutSmps, sizeof(short));

	// casting all vec_ts to short for writing 16-bit wave file
	for (k=0; k<(*nOutSmps); k++)
		_16bitData[k] = (short)(tempOut[k]*norm);

	free(y);
	free(s);
	free(xhat);
	free(Y);
	free(Xhat);
	free(magY);
	free(phaseY);
	free(tempOut);
	return _16bitData;
}
