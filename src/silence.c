/* 
   Filename	:silence.c
   Author	:M.W. Mak (enmwmak@polyu.edu.hk)
   Description  :Detect the silence region of a given speech buffer
   Bug Fixed    :(9/4/99) The decision in detect_silence() changed to <=, i.e.,
	          if (zcr[i] <= zcr_th && avm[i] <= avm_th)
	             silence[i]=SILENCE;
	          else
	             silence[i]=NONSILENCE;
		 This avoids the case where all samples in the background regions
		 are zeros. However, it is likely that the resulting .phn file
		 will have two segments only
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mmalloc.h"
#include "segment.h"
#include "silence.h"
#include "veclib.h"
#include "qsortfunc.h"

int sgn(short y);
vec_t median(unsigned int num_pk_frms, vec_t *peak);

int sgn(short y)
{
    if (y>0) 
	  return 1;
    else
          return -1;
}


vec_t median(unsigned int num_pk_frms, vec_t *peak)
{
    int (*fp)();
    fp = qsortcomparef;
    qsort((vec_t *)peak, num_pk_frms ,sizeof(vec_t), fp);
    return(peak[(num_pk_frms/2)]);
}


/***************************************************************************
  zero_crossing(): Determine the zero crossing of an input frame of speech
  Input:
        short *x: a frame of speech [0..n-1]
	int   n: number of samples in a frame
  Return:
        vec_t   zero crossing rate
*******************************************************************************/
vec_t zero_crossing(short *x, unsigned long n)
{
    unsigned long i;
    unsigned long sum=0;
    for (i=1; i<n; i++)
        sum+=abs(sgn(x[i])-sgn(x[i-1]));
    return (vec_t)sum/(2*(vec_t)n);
}


/***************************************************************************
  average_magnitude(): Determine the average magnitude of a frame of speech
  Input:
        short *x: a frame of speech [0..n-1]
	int   n: number of samples in a frame
  Return:
        vec_t amag: average magnitude
*******************************************************************************/
vec_t average_magnitude(short *x, unsigned long n)
{
      unsigned long i;
      vec_t amag=0.0;

      for (i=0; i<n; i++)
	  amag += (vec_t)abs(x[i]);
      return(amag/n);
}


/***************************************************************************
  remove_offset(): Remove the DC offset of speech
  Input:
        short *x: speech signals [0..num_samples-1]
	unsigned long num_samples: number of samples in x[]
  Output:
        short *x: input speech vector updated with DC offset removed
*******************************************************************************/
void remove_offset(short *x, unsigned long num_samples)
{
     unsigned long i;
     vec_t offset=0.0;

     for (i=0; i<num_samples; i++)
         offset += (vec_t)x[i];
     offset /= (vec_t)num_samples;

     for (i=0; i<num_samples; i++)
         x[i] -= (short)offset;
}

/***************************************************************************
  FIR_filter(): Implementation of an FIR filter
  Input:
        vec_t *x: Signal to be filtered [0..n-1]
	unsigned long N: number of samples
	vec_t a0,a1,a2: filter coefficients
  Output:
        vec_t *y: y(n)=a0x(n)+a1x(n-1)+a2x(n-2)
*******************************************************************************/
void FIR_filtering(vec_t *x, unsigned long N, vec_t a0, vec_t a1, vec_t a2)
{
     unsigned long n;
     vec_t *y = (vec_t *)vector(0,N-1,sizeof(vec_t));

     for (n=2; n<N; n++) {
         y[n]=a0*x[n]+a1*x[n-1]+a2*x[n-2];
     }
     for (n=0; n<N; n++)
         x[n]=y[n];
     free_vector((char *)y,0,sizeof(vec_t));
}

/***************************************************************************
  moving_average(): Compute the moving average
  Input:
        vec_t *x: Signal to be filtered [0..N-1]
	unsigned long N: number of samples
	int   M: moving average window size
  Output:
                             M-1
        vec_t *y: y(n)=(1/M)sum x(n-j)
                            j=0
*******************************************************************************/
void moving_average(vec_t *x, unsigned long N, int M)
{
     unsigned long n;
     vec_t *y = (vec_t *)vector(0,N-1,sizeof(vec_t));
     vec_t sum;
     int j;
     for (n=M-1; n<N; n++) {
	 sum = 0.0;
	 for (j=0; j<M; j++)
	     sum += x[n-j];
	 y[n] = sum/M;
     }
     for (n=M-1; n<N; n++) {
	 x[n] = y[n];
     }
     free_vector((char *)y,0,sizeof(vec_t));
}



/***************************************************************************
  detect_silence(): Determine the silence regions of a given speech signals
  Input:
        x             : speech signal [0..num_samples-1]
	num_samples   : number of samples in x[]
	sample_rate   : sampling rate in Hz
        zcr_factor    : factor of mean bkg zero-crossing rate 
        avm_factor    : factor of mean bkg amplitude ($\nu$ in Eq. 9 of CSL paper)
        noise_energy  : Energy of background noise, for crosstalk removal
  Return:
        seg           : array of SEGMENT structure containing the beginning and end 
	                of silence regions
*******************************************************************************/
#define BKG_AMP_FLOOR 5
#define BKG_ZCR_FLOOR 0.3
#define BKG_RATIO 0.1                      /* Assume that 10% of the speech file contain background */
#define PEAK_RATIO 0.05                    /* Assume that 5% of the speech file contain signal peaks */
SEGMENT *detect_silence(short *x, unsigned long num_samples, int sample_rate,
			vec_t zcr_factor, vec_t avm_factor, vec_t noise_energy)
{
     unsigned long i,j;
     SEGMENT *seg;                         /* Array of segment to be returned */
     int      s;                           /* Segment number, index to access seg[] */
     int      num_segs;                    /* Number of segment */
     vec_t    *zcr,*z;                     /* Number of zero crossing */
     vec_t    *avm,*a;                     /* Average magnitude */
     vec_t    *bk_zcr;                     /* Zero crossing rate of background */
     vec_t    *bk_avm;                     /* Average magnitude of background */
     vec_t    zcr_th;                      /* Threshold for zero crossing rate */
     vec_t    avm_th;                      /* Threshold for average magnitude */
     unsigned long num_frms;               /* Number of frames */
     unsigned int winsize;                 /* Number of samples in a frame */
     unsigned int wininc;                  /* Number of samples to advance for each frame */
     short    *silence;                    /* Array indicating which frame contain voice and
					      which frame contain silence. [0..num_frms-1] */
     unsigned int num_bk_frms;             /* Number of frames in the background period */
     unsigned int num_pk_frms;             /* Number of frames containing peak amplitude */
     vec_t    mean_zcr,std_zcr;            /* The mean and standard derivation of zero crossing 
					      rate during the background period */
     vec_t    mean_avm,std_avm;            /* The mean and standard derivation of average magnitude
					      rate during the background period */
     int      sig_peak;                    /* the peak of the signal */
     vec_t    minamp,maxamp;
     int      minpos,maxpos;
     vec_t    mean_peak;                   // Mean of signal peaks
     vec_t    *peak;                       // Amplitude of frames containing peaks [0..num_bkg_frms-1]
     vec_t    *p;                          // [0..num_frms-1]
     vec_t    median_peak;
     vec_t    min_peak;
     vec_t    *norm_avm;                   // Z-normalized average amplitude of the utt
     unsigned long start_frm,end_frm;      // Start and end frame indexes of the current segment

     winsize = FRAME_WIDTH*sample_rate;
     wininc = sample_rate/FRAME_RATE;
     num_frms = (num_samples-winsize)/wininc+1;
     printf("No. of frames = %ld, ",num_frms); fflush(stdout);

     /* Remove DC offset */
     remove_offset(x,num_samples);

     /* Determine the peak value of x[] */
     for (sig_peak=0,i=0; i<num_samples; i++)
        if (abs(x[i])>sig_peak) sig_peak=abs(x[i]);
     
     /* 
        Find the statistic of background period by looking for nonspeech frames 
        We consider 'num_bk_frms' frames with the lowest amplitude, flooring the
	lowest average amplitude to BKG_AMP_FLOOR.
	We assume that about 10% of the speech file is non-speech.
	We assume that 5% of the speech contain peaks. 
     */

     num_bk_frms = BKG_RATIO*num_frms;  
     if (num_bk_frms < 1) 
	 num_bk_frms = 1;
     num_pk_frms = PEAK_RATIO*num_frms; 
     if (num_pk_frms < 1)
	 num_pk_frms = 1;
     bk_zcr = (vec_t *)vector(0,num_bk_frms-1,sizeof(vec_t));
     bk_avm = (vec_t *)vector(0,num_bk_frms-1,sizeof(vec_t));
     peak = (vec_t *)vector(0,num_pk_frms-1,sizeof(vec_t));
     a = (vec_t *)vector(0,num_frms-1,sizeof(vec_t));
     p = (vec_t *)vector(0,num_frms-1,sizeof(vec_t));
     z = (vec_t *)vector(0,num_frms-1,sizeof(vec_t));
     for (i=0; i<num_frms; i++) {
         j=i*wininc;
	 a[i] = average_magnitude(&x[j],winsize);
	 z[i] = zero_crossing(&x[j],winsize);
     }
     VECcopyf(num_frms, p, a);
     for (i=0; i<num_bk_frms; i++) {
         minamp = VECminposf(num_frms, a, &minpos);
	 if (minamp < BKG_AMP_FLOOR) {
	     bk_avm[i] = BKG_AMP_FLOOR;
	     bk_zcr[i] = BKG_ZCR_FLOOR;
	 } else {
	     bk_avm[i] = a[minpos];
	     bk_zcr[i] = z[minpos];
	 }
	 a[minpos] = 1e38;
     }
     for (i=0; i<num_pk_frms; i++) {
	 maxamp = VECmaxposf(num_frms, p, &maxpos);
	 peak[i] = p[maxpos];
	 p[maxpos] = 0;
     }

     VECstatf(num_bk_frms,bk_zcr,&mean_zcr,&std_zcr);
     VECstatf(num_bk_frms,bk_avm,&mean_avm,&std_avm);
     mean_peak = VECmeanf(num_pk_frms,peak);
     median_peak = median(num_pk_frms,peak);
     min_peak = VECminf(num_pk_frms,peak);

     /* 
        Determine the thresholds based on background statistics and average magnitude and
        zero crossing of the speech signal. Limit the mag threshold within 20% of mean signal peak.
	Magnitude threshold is a linear combination between minimum of peaks and mean background magnitude.
     */
     if (zcr_factor > 0)
	 zcr_th = zcr_factor*mean_zcr + 2.0*std_zcr;
     else
	 zcr_th = -1;    // Do not use zero-crossing
     avm_th = avm_factor*mean_avm + (1-avm_factor)*min_peak;
     if (avm_th==0.0 || avm_th > 0.2*mean_peak) {
        avm_th = 0.2 * mean_peak;
     }
     
     printf("mean_bkg_avm=%.2lf, std_bkg_avm=%.2lf, mean_bkg_zcr=%.2lf, std_bkg_zcr=%.2lf, min_peak=%.2lf, sig_peak=%d, median_peak=%.2lf, mean_peak=%.2lf, avm_th=%.2lf, zcr_th=%.2lf, mean_zcr=%.2lf\n",mean_avm,std_avm,mean_zcr,std_avm,min_peak,sig_peak,median_peak,mean_peak,avm_th,zcr_th,mean_zcr);

     /* Determine silence frames. Information store in silence[] */
     silence = (short *)vector(0,num_frms-1,sizeof(short));
     zcr = (vec_t *)vector(0,num_frms-1,sizeof(vec_t));
     avm = (vec_t *)vector(0,num_frms-1,sizeof(vec_t));
     for (i=0; i<num_frms; i++) {
         j=i*wininc;
	 zcr[i]=zero_crossing(&x[j],winsize);
	 avm[i]=average_magnitude(&x[j],winsize);
     }

     /* Smooth the profile of average amplitude and zero crossing using moving averaging */
     moving_average(zcr,num_frms,40);
     moving_average(avm,num_frms,40);

     /* Normalize the energy so that the energy in the 2 channels can be compared. This is
	important for crosstalk removal */
     norm_avm = (vec_t *)vector(0, num_frms-1, sizeof(vec_t));
     VECcopyf(num_frms, norm_avm, avm);

     // Perform Z-norm: This is not good for some files
     //VECznorm(num_frms, norm_avm, VECmeanf(num_frms, avm), VECstddevf(num_frms, avm));

     // Normalized by median: Not good if large part of the file contains silence
     //VECmulalphaf(num_frms, 1/(median(num_frms,norm_avm)+1e-38), norm_avm);

     // Normalized by the square root of the norm of background spectrum: Seems to be the best option
     VECmulalphaf(num_frms, 1/(noise_energy+1e-38), norm_avm);

     /* Use zero crossing only if backgroud frames do not contain all zeros */
     if (zcr_th > 0.0) {
        for (i=0; i<num_frms; i++) {
	    if (zcr[i] <= zcr_th && avm[i] <= avm_th)
	        silence[i]=SILENCE;
	    else
	        silence[i]=NONSILENCE;
        }
     } else {
        for (i=0; i<num_frms; i++) {
	    if (avm[i]<=avm_th)
	       silence[i]=SILENCE;
	    else
	       silence[i]=NONSILENCE;
	    if (zcr[i] < mean_zcr*0.1) {
	    	silence[i]=SILENCE;           // Set frames with extremely low zero crossing rate to silence
	    }        
	}
     }

     /* Determine the number of segments */
     num_segs=0;
     for (i=1; i<num_frms; i++)
        num_segs+=abs(sgn(silence[i])-sgn(silence[i-1]));
     num_segs=num_segs/2 + 1;

     /* Allocate memory for storing segment */
     seg = (SEGMENT *)vector(0,num_segs,sizeof(SEGMENT));

     /* Find the beginning and the end of each segment */
     s=0;i=0;
     while (i<num_frms) {
	 start_frm = i;
         seg[s].begin = i*wininc;
	 while (i<num_frms-1 && silence[i]*silence[i+1]>0)
	     i++;
	 seg[s].end = (i+1)*wininc;
	 if (silence[i]==SILENCE)
	     strcpy(seg[s].phoneme,"h#");
	 else
	     strcpy(seg[s].phoneme,"S");
	 seg[s].num_segs = num_segs;
	 seg[s].num_samples = seg[s].end-seg[s].begin+1;
	 end_frm = i;
	 seg[s].mean_namp = VECmeanf(end_frm-start_frm+1, &norm_avm[start_frm]);
	 i++;
	 s++;
     }

     /* Print segment information in <#frames #segments #samples %speech> */
     print_seg_info(seg, wininc, num_samples);

     free_vector((char *)silence,0,sizeof(short));
     free_vector((char *)bk_zcr,0,sizeof(short));
     free_vector((char *)bk_avm,0,sizeof(short));
     free_vector((char *)zcr,0,sizeof(vec_t));
     free_vector((char *)avm,0,sizeof(vec_t));
     free_vector((char *)peak,0,sizeof(vec_t));
     free_vector((char *)p,0,sizeof(vec_t));
     free_vector((char *)a,0,sizeof(vec_t));
     free_vector((char *)z,0,sizeof(vec_t));
     free_vector((char *)norm_avm,0,sizeof(vec_t));

     return(seg);
}


         



