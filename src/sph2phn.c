/* 
   Filename	:wav2phn.c
   Version	:1.0
   Date		:25 Jan 2013
   Author	:M.W. Mak (enmwmak@polyu.edu.hk)
   Description  :Convert SPHERE (TIMIT) .sph file to TIMIT .phn file using SSVAD
                 H.B. Yu and M.W. Mak, "Comparison of Voice Activity Detectors for Interview Speech 
		 in NIST Speaker Recognition Evaluation", Interspeech'11, Florence, Aug. 2011.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sp/sphere.h>
#include "mmalloc.h"
#include "sph_io.h"
#include "cmdline.h"
#include "segment.h"
#include "veclib.h"
#include "silence.h"
#include "denoise.h"
#include "findnoise.h"
#include "winwav.h"




/* Declare global variables here */
/* Default command line input parameters */
char
    *CL_PhnFile="default.phn",        /* .phn file of the TIMIT corpus */
    *CL_SphFile="default.sph",	      /* Default .sph filename */
    *CL_ChannelID="A",                /* In case of SPIDRE corpus, the channel to be read */
    *CL_Denoise="Y",                  /* Apply noise reduction before performing speech detection */
    *CL_DenoiseWavFile=(char *)NULL,  /* Denoised speech file, only if Denoise is Y */
    *CL_ZcrFactor="-1000",            /* Factor for determining zero crossing threshold (<0 means not use) */
    *CL_AvmFactor="0.99";             /* Factor for determining average mag threshold */
                                      /* Th = f*bkg_magnitude+(1-f)mean_peak */

CLINEPARA options[]=
{
    {"-PhnFile", "-phn", &CL_PhnFile},
    {"-SphFile", "-sph", &CL_SphFile},
    {"-ChannelID", "-ch", &CL_ChannelID},
    {"-Denoise", "-dn", &CL_Denoise},
    {"-DenoiseWavFile", "-df", &CL_DenoiseWavFile},
    {"-ZeroCrossingFactor", "-zf", &CL_ZcrFactor},
    {"-AverageAmplitudeFactor", "-af", &CL_AvmFactor}
};  	

int num_options=sizeof(options)/sizeof(CLINEPARA);

#define BKG_FRAC 0.05                 /* Fraction of background frame w.r.t. the whole utterance */
#define FRM_SIZE 512                 /* Number of samples in one frame */   
int main(int argc, char *argv[])
{
     SP_INTEGER bps;                /* Byte per samples */
     SP_INTEGER sr;                 /* Sampling rate in Hz */
     unsigned long num_samples;     /* number of samples to be read from audio device */
     short *spbuf;                  /* buffer storing speech samples */
     SEGMENT *segment;              /* Structure storing information regarding silence 
				       regions*/
     int   errcode;
     SP_INTEGER n_ch;
     double zcr_factor;              /* Factor for determining zero crossing threshold */
     double avm_factor;              /* Factor for determining average mag threshold */
     char *smpcode;
     short *denoiseSph;
     unsigned long framesize = FRM_SIZE;  // Frame size of spectral subtraction. Must be power of 2
     vec_t alphaMax = 4.0;		   // Parameters for spectral subtraction
     vec_t alphaMin = 0.5;		   // with musical noise minimization
     vec_t betaMax = 0.05;
     vec_t betaMin = 0.01;
     unsigned long numOutSmps;		  // Number of output samples. Could be less than numSmps
     vec_t *noiseSpec;			  // Noise spectrum [0...frameSize-1]
     unsigned long j,tot_num_segs,num_sph_segs;

     if (argc==1)
         usage(argv[0],num_options,options);

     /* Get paramters from command line input */	
     get_cmdline(argc, argv, num_options, options);
     zcr_factor = atof(CL_ZcrFactor);
     avm_factor = atof(CL_AvmFactor);

     /* Read the wave file */
     if ((spbuf=read_wav_file(CL_SphFile,&num_samples,&bps,&n_ch,&sr,&smpcode,CL_ChannelID[0],
			      &errcode))==NULL) {
	 fprintf(stderr,"%s: Error in reading %s\n",argv[0],CL_SphFile);
	 exit(EXIT_FAILURE);
     }

     /* Perform spectral subtraction only if spbuf[] contains speech */
     if (CL_Denoise[0] == 'Y' && zero_crossing(spbuf, num_samples)>0) {
	 printf("Performing denoising\n"); fflush(stdout);
	 noiseSpec = findnoise(spbuf, num_samples, framesize, BKG_FRAC);
	 denoiseSph = denoise(spbuf, num_samples, framesize, framesize/4, &numOutSmps,
			      noiseSpec, alphaMax,alphaMin,betaMax,betaMin);
	 if (CL_DenoiseWavFile) {
	     printf("Writing denoised file %s\n", CL_DenoiseWavFile);
             wavwrite(denoiseSph, numOutSmps,sr,bps, CL_DenoiseWavFile);
	 }
     } else {
	 denoiseSph = spbuf;
	 numOutSmps = num_samples;
     }

     /* Determine silence sections */
     printf("Performing speech detection\n"); fflush(stdout);
     segment=detect_silence((short *)denoiseSph,numOutSmps,sr,zcr_factor,avm_factor,1.0);

     /* A normal speech file should have at least 3 segments: <sil><speech><sil>.
	If the number of segments is 1, the speech file could be either all silence
	or all speech. Both situations are undesirable */

    // If there is no speech segment, artificially assign one speech segment and the following
    // one silence segment. This helps to avoid problem in sph2cep.c
    num_sph_segs = 0;
    for (j=0; j<segment[0].num_segs; j++) {
	if (strcmp(segment[j].phoneme,"S")==0)
	    num_sph_segs++;
    }
    tot_num_segs = segment[0].num_segs;
    if (num_sph_segs == 0) {
	segment[0].num_segs = 2;
	segment[0].begin = 0;
	segment[0].end = 512;
	strcpy(segment[0].phoneme,"S");
	segment[1].num_segs = 2;
	segment[1].begin = segment[0].end;
	segment[1].end = numOutSmps-1;
	strcpy(segment[1].phoneme,"h#");
    }


     /* Save segment information to .phn file */
     PhnFileWrite(CL_PhnFile,segment);

     return(0);
}




   


