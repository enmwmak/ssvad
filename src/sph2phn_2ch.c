/* 
   Filename	:sph2phn_2ch.c (VAD with crosstalk removal)
   Version	:1.0
   Date		:Jan 13
   Author	:M.W. Mak (enmwmak@polyu.edu.hk)
   Description  :Convert TIMIT wave file to TIMIT phn file. Use information of 2 channels to remove
                 cross talk.
   Change logs  :29-1-13. The method remove_crosstalk has been moved to rm_crosstalk.c

                 The strategy to remove crosstalk depends on the year of SRE, because
		 different years treat the channel B differently.

		 For SRE06, there is no crosstalk (in theory). So, this program should not be used.
		 If it is used, the crosstalk removal should be turned off.

		 For SRE08, there are two situations.
		 (1) Interview segments in which Channel B contains all zeros. So, it is not 
		 necessary to remove the crosstalk. Again, this program should not be used. 
		 If it is used, the crosstalk removal should be turned off.
		 (2) two-channel tel conversations with speech recorded by tel handsets or
		 by a microphone. There is also no crosstalk in this case.

		 For SRE10, there are two situations.
		 (1) Based on the doc of eval plan, Channel B contains interviewer's head-mounted close-talking
		 microphone, with some level of noise added to mask any residual speech of the
		 target speaker (Channel A) in it. So, only Ch B will cross talk to Ch A, not
		 the other way round. So, whenever both ChA and ChB contain speech, the segment in ChA is 
		 considered to be caused by cross talk and should be assigned a "h#".
		 (2) Channel A and Channel B could couple to each other, e.g. 
		 /corpus/nist10/data/interview/3min/euhxw.sph

		 For SRE12, crosstalk will occur in both channels. Therefore, energy
		 in both channels is used to determine the crosstalk regions.  


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
#include "rm_crosstalk.h"


/* Declare global variables here */
/* Default command line input parameters */
char *CL_PhnFile="default.phn",        /* .phn file for ch A */
     *CL_SphFile="default.sph",	       /* Default .sph filename */
     *CL_ChannelID="A",                /* In case of SPIDRE corpus, the channel to be read */
     *CL_Denoise="Y",                  /* Apply noise reduction before performing speech detection */
     *CL_DenoiseWavFile=(char *)NULL,  /* Denoised speech file for output, only if Denoise is Y */
     *CL_Corpus="nist12",              /* Corpus, if "nist12", "nist12_8k" or "nist12_16k", use post-SRE12 crosstalk rm */
     *CL_AlphaMax="4.0",               /* Hyper-parameters for spectral subtraction algorithm */ 
     *CL_AlphaMin="0.5",
     *CL_BetaMax="0.05",
     *CL_BetaMin="0.01",
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
	{"-Corpus", "-c", &CL_Corpus},
	{"-AlphaMax","-amax", &CL_AlphaMax},
	{"-AlphaMin","-amin", &CL_AlphaMin},
	{"-BetaMax","-bmax", &CL_BetaMax},
	{"-BetaMin","-bmin", &CL_BetaMin},
	{"-ZeroCrossingFactor", "-zf", &CL_ZcrFactor},
	{"-AverageAmplitudeFactor", "-af", &CL_AvmFactor}
};  	

int num_options=sizeof(options)/sizeof(CLINEPARA);

#define BKG_FRAC 0.1                 /* Fraction of background frame w.r.t. the whole utterance */
#define FRM_SIZE 512                 /* Frame size for computing noise spectrum and spectral subtraction */
int main(int argc, char *argv[])
{
     SP_INTEGER bps;                 /* Byte per samples */
     SP_INTEGER sr;                  /* Sampling rate in Hz */
     unsigned long num_samples;      /* number of samples to be read from audio device */
     short *spbuf1,*spbuf2;          /* buffer storing speech samples in both channels */
     SEGMENT *seg1,*seg2,*seg3;      /* Structure storing information regarding silence regions
					seg3[] stores the segmentation (VAD) information after crosstalk removal */
     int   errcode;
     SP_INTEGER n_ch;
     double zcr_factor;              /* Factor for determining zero crossing threshold */
     double avm_factor;              /* Factor for determining average mag threshold */
     char *smpcode;
     short *denoiseSph1,*denoiseSph2;     // Waveform after spectral subtraction */
     unsigned long framesize = FRM_SIZE;  // Frame size of spectral subtraction. Must be power of 2
     unsigned long numOutSmps1;		  // Number of output samples in Channel A. Could be less than numOutSmps
     unsigned long numOutSmps2;		  // Number of output samples in Channel B. Could be less than numOutSmps
     unsigned long numOutSmps;            // The less of numOutSmps1 and numOutSmps2
     vec_t *noiseSpec1,*noiseSpec2;	  // Noise spectrum [0...frameSize-1] for Channels A and B
     vec_t *denoiseSpec1,*denoiseSpec2;	  // Noise spectrum [0...frameSize-1] after spectral subtraction

     vec_t alphaMax = atof(CL_AlphaMax);		   // Parameters for spectral subtraction
     vec_t alphaMin = atof(CL_AlphaMin);		   // with musical noise minimization
     vec_t betaMax = atof(CL_BetaMax);
     vec_t betaMin = atof(CL_BetaMin);

     if (argc==1)
         usage(argv[0],num_options,options);

     /* Get paramters from command line input */	
     get_cmdline(argc, argv, num_options, options);
     zcr_factor = atof(CL_ZcrFactor);
     avm_factor = atof(CL_AvmFactor);
     alphaMax = atof(CL_AlphaMax);		   // Parameters for spectral subtraction
     alphaMin = atof(CL_AlphaMin);		   // with musical noise minimization
     betaMax = atof(CL_BetaMax);
     betaMin = atof(CL_BetaMin);

     /* Read Channel A and Channel B from wave file */
     if ((spbuf1=read_wav_file(CL_SphFile,&num_samples,&bps,&n_ch,&sr,&smpcode,'A',&errcode))==NULL) {
	 fprintf(stderr,"%s: Error in reading %s\n",argv[0],CL_SphFile);
	 exit(EXIT_FAILURE);
     }
     if ((spbuf2=read_wav_file(CL_SphFile,&num_samples,&bps,&n_ch,&sr,&smpcode,'B',&errcode))==NULL) {
	 fprintf(stderr,"%s: Error in reading %s\n",argv[0],CL_SphFile);
	 exit(EXIT_FAILURE);
     }

     /* Perform spectral subtraction if speech exists. Estimate noise spectrum before and after 
        spectral subtraction */
     if (CL_Denoise[0] == 'Y') {
	 if (zero_crossing(spbuf1, num_samples)>0) {
	     printf("Performing denoising on channel A\n"); fflush(stdout);
	     noiseSpec1 = findnoise(spbuf1, num_samples, framesize, BKG_FRAC);
	     denoiseSph1 = denoise(spbuf1, num_samples, framesize, framesize/4, &numOutSmps1,
				   noiseSpec1, alphaMax,alphaMin,betaMax,betaMin);
	     denoiseSpec1 = findnoise(denoiseSph1, numOutSmps1, framesize, BKG_FRAC);
	     printf("Channel A Noise Energy = %f\n",sqrt(VECL2normf(framesize, denoiseSpec1))/framesize);
	 } else {
	     denoiseSph1 = spbuf1;
	     denoiseSpec1 = (vec_t*)calloc(framesize,sizeof(vec_t));
	     numOutSmps1 = num_samples;
	 }
	 if (zero_crossing(spbuf2, num_samples)>0) {
	     printf("Performing denoising on channel B\n"); fflush(stdout);
	     noiseSpec2 = findnoise(spbuf2, num_samples, framesize, BKG_FRAC);
	     denoiseSph2 = denoise(spbuf2, num_samples, framesize, framesize/4, &numOutSmps2,
				   noiseSpec2, alphaMax,alphaMin,betaMax,betaMin);
	     denoiseSpec2 = findnoise(denoiseSph2, numOutSmps2, framesize, BKG_FRAC);
	     printf("Channel B Noise Energy = %f\n",sqrt(VECL2normf(framesize, denoiseSpec2))/framesize);
	 } else {
	     denoiseSph2 = spbuf2;
	     denoiseSpec2 = (vec_t*)calloc(framesize,sizeof(vec_t));
	     numOutSmps2 = num_samples;
	 }

     } else {
	 denoiseSph1 = spbuf1;
	 denoiseSph2 = spbuf2;
	 numOutSmps1 = num_samples;
	 numOutSmps2 = num_samples;
     }
     // /Spectral subtraction may truncate speech samples at the end of file
     numOutSmps = (numOutSmps1<numOutSmps2) ? numOutSmps1 : numOutSmps2;


     /* Save denoised waveform as MS wave file */
     if (CL_Denoise[0] == 'Y' && CL_DenoiseWavFile!=NULL) {
	 if (CL_ChannelID[0] == 'A') {
	     printf("Writing channel A to denoised WAVE file %s\n", CL_DenoiseWavFile);
	     wavwrite(denoiseSph1, numOutSmps,sr,bps, CL_DenoiseWavFile);
	 } else {
	     printf("Writing channel B to denoised WAVE file %s\n", CL_DenoiseWavFile);
	     wavwrite(denoiseSph2, numOutSmps,sr,bps, CL_DenoiseWavFile);
	 }
     }

     /* Determine silence segments */
     printf("Performing speech detection on channel A\n"); fflush(stdout);
     seg1=detect_silence((short *)denoiseSph1,numOutSmps,sr,zcr_factor,avm_factor,
			 sqrt(VECL2normf(framesize, denoiseSpec1))/framesize);
     printf("Performing speech detection on channel B\n"); fflush(stdout);
     seg2=detect_silence((short *)denoiseSph2,numOutSmps,sr,zcr_factor,avm_factor,
			 sqrt(VECL2normf(framesize, denoiseSpec2))/framesize);

     /* Perform crosstalk removal */
     if (strcmp(CL_Corpus,"nist12")==0 || strcmp(CL_Corpus,"nist12_8k")==0 || strcmp(CL_Corpus,"nist12_16k")==0) {
	 printf("Performing SRE12 crosstalk removal\n");
	 seg3 = remove_crosstalk_SRE12(seg1, seg2, numOutSmps, CL_ChannelID[0]);
     } else {
	 if (strcmp(CL_Corpus,"nist10")==0 || strcmp(CL_Corpus, "nist10_8k")==0 || strcmp(CL_Corpus, "nist10_16k")==0) {
	     printf("Performing SRE10 crosstalk removal\n");
	     seg3 = remove_crosstalk_SRE10(seg1, seg2, numOutSmps, CL_ChannelID[0]);
	 }  else {
	     printf("Crosstalk removal not necessary for pre-SRE10\n");
	     if (CL_ChannelID[0] == 'A')
		 seg3 = seg1;                // We want VAD info of ChA
	     else
		 seg3 = seg2;                // We want VAD info of ChB
	 }
     }
     print_seg_info(seg3, sr/FRAME_RATE, numOutSmps);

     /* Save segment information to .phn file */
     printf("Saving segment of Channel %c info to %s\n",CL_ChannelID[0],CL_PhnFile);
     PhnFileWrite(CL_PhnFile,seg3);

     /* Save the crosstalk-removed speech to .sph file */
     //cx_rm_smp = extract_sample(seg3, spbuf1, &numOutSmps);
     //write_wav_file("/tmp/cx_rm_smp.sph",cx_rm_smp,(SP_INTEGER)numOutSmps,(SP_INTEGER)2,(SP_INTEGER)sr);     

     return(0);
}




