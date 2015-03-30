/* Filename	:phninfo.c
   Version	:1.0
   Date		:13 Jan 09
   Author	:M.W. Mak
   Purpose      :Display the info of .phn file
*/

#include <stdio.h>
#include <stdlib.h>
#include <veclib.h>
#include <math.h>
#include <string.h>
#include "segment.h"
#include "cmdline.h"

/* Declare global variables here */
/* Default command line input parameters */
char *CL_PhnFile=(char *)NULL,	/* default phoneme file .phn */
     *CL_WinSize="200",
     *CL_SamplePerFrame="80";

CLINEPARA options[]=
{
    {"-PhnFile", "-phn", &CL_PhnFile},
    {"-WinSize", "-ws", &CL_WinSize},
    {"-SamplePerFrame (FrameShift)", "-spf", &CL_SamplePerFrame}
};  	

int num_options=sizeof(options)/sizeof(CLINEPARA);


#define DCEP_WSIZE	7		/* window size for calculating delta cepstrum */    
int main(int argc, char *argv[])
{
     SEGMENT	*seg;			/* Structure storing segment and transcrition info */ 	
     unsigned long num_samples;         /* number of samples */
     int i;
     unsigned long spf;
     unsigned long tot_num_samples;
     float sp_ratio;
     unsigned long num_frms,N,win_size,win_adv;
     char *phoneme;

     if (argc==1) usage(argv[0],num_options,options);

     /* Get paramters from command line input */	
     get_cmdline(argc, argv, num_options, options);
     spf = atol(CL_SamplePerFrame);
     win_size = atol(CL_WinSize);
     win_adv = spf;

     /* Read .phn or .mrk file to define speech/nonspeech segments */
     seg = (SEGMENT *)PhnFileRead(CL_PhnFile);

     /* Print segment information in <#frames #segments #samples %speech> */
     num_samples = 0;
     tot_num_samples = 0;
     num_frms = 0;
     for (i=0; i<seg[0].num_segs; i++) {
	 tot_num_samples += seg[i].num_samples;
	 phoneme = seg[i].phoneme;
	 if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	     continue;  				/* skip silence segments */
	 N = seg[i].end - seg[i].begin + 1;
	 if (N < win_size)
	     continue;
	 num_frms += (N-win_size)/win_adv+1;	 
	 num_samples += seg[i].num_samples;
     }
     sp_ratio = (float)num_samples/(float)tot_num_samples;
     printf("%10ld %10d %10ld %6.2f%%",num_frms,seg[0].num_segs,num_samples,sp_ratio*100);

     return(0);
}

   


