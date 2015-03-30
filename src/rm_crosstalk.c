/* 
   Filename	:rm_crosstalk.c
   Version	:1.0
   Date		:Jan 13
   Author	:M.W. Mak (enmwmak@polyu.edu.hk)
   Description  : Remove crosstalk: Return the crosstalk-removed segmentation info

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmalloc.h"
#include "segment.h"
#include "veclib.h"

#define WIN_SIZE 200       // Min frame size (assume 8kHz)
#define WIN_ADV 80         // Min frame shift

/* 
   Remove crosstalk in nist10 files: Only ChB will crosstalk to ChA, not the other way round
*/
SEGMENT *remove_crosstalk_SRE10(SEGMENT *seg1, SEGMENT *seg2, unsigned long num_samples, char channel)
{
    unsigned long i,j,t,tot_num_segs,num_sph_segs,num_sph_samples,num_sph_frms;
    char *phoneme;
    int *label1,*label2,*label3; 
    SEGMENT *seg3;
    int max_num_segs = 50000;

    seg3 = (SEGMENT *)vector(0,max_num_segs-1,sizeof(SEGMENT));
    label1 = (int *)vector(0, num_samples, sizeof(int));
    label2 = (int *)vector(0, num_samples, sizeof(int));
    label3 = (int *)vector(0, num_samples, sizeof(int));

    // Set label1[t]=1 if sample t of channel A falls on speech segment
    for (i=0; i<seg1[0].num_segs; i++) {
	 phoneme = seg1[i].phoneme;
	 if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	     continue;
	 for (t=seg1[i].begin; t<=seg1[i].end; t++) {
	     label1[t] = 1;
	 }
    }
    
    // Set label2[t]=1 if sample t of channel B falls on speech segment
    for (i=0; i<seg2[0].num_segs; i++) {
	 phoneme = seg2[i].phoneme;
	 if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	     continue;
	 for (t=seg2[i].begin; t<=seg2[i].end; t++) {
	     label2[t] = 1;
	 }
    }

    if (channel == 'A') {
	//Frame t falls on a crosstalk region if label2[t]==1 
	for (t=0; t<num_samples; t++) {
	    label3[t] = label1[t];
	    if (label2[t]==1) {
		label3[t] = 0;
	    }
	}
    } else {
	//Frame t falls on a crosstalk region if label1[t]==1 
	for (t=0; t<num_samples; t++) {
	    label3[t] = label2[t];
	    if (label1[t]==1) {
		label3[t] = 0;
	    }
	}
    }

    // Assign values to seg3 according to label3[]
    j = 0;
    t = 0;
    tot_num_segs = 0;
    num_sph_samples = 0;
    num_sph_frms = 0;
    while (t<num_samples) {
	seg3[j].begin = t;
	if (label3[t] == 0) {
	    strcpy(seg3[j].phoneme,"h#");            // Crosstalk or silence
	    while (label3[t] == 0) {                 // Search for next segment
		t++;
	    }
	    seg3[j].end = t;
	    seg3[j].num_samples = t-seg3[j].begin+1;
	} else {
	    strcpy(seg3[j].phoneme,"S");             // Speech segment
	    while (label3[t] == 1) {                 // Search for next segment
		t++;
	    }
	    seg3[j].end = t;
	    seg3[j].num_samples = t-seg3[j].begin+1;
	    if (seg3[j].num_samples >= WIN_SIZE) {        // Assume minimum frame size is 200
		num_sph_samples += seg3[j].num_samples;
		num_sph_frms += (seg3[j].num_samples-WIN_SIZE)/WIN_ADV+1;;
	    }	    
	}
	j++;                                         // next segment
	tot_num_segs++;
    }
    for (j=0; j<tot_num_segs; j++)
	seg3[j].num_segs = tot_num_segs;

    // If there is no speech segment, artificially assign one speech segment and the following
    // one silence segment. This helps to avoid problem in sph2cep.c
    num_sph_segs = 0;
    for (j=0; j<tot_num_segs; j++) {
	if (strcmp(seg3[j].phoneme,"S")==0)
	    num_sph_segs++;
    }
    if (num_sph_segs == 0 || num_sph_frms == 0 || num_sph_samples == 0) {
	printf("No speech segment, artificially assign one to help sph2cep.c\n");
	seg3[0].num_segs = 2;
	seg3[0].begin = 0;
	seg3[0].end = 512;
	strcpy(seg3[0].phoneme,"S");
	seg3[1].num_segs = 2;
	seg3[1].begin = seg3[0].end;
	seg3[1].end = seg1[tot_num_segs-1].end;
	strcpy(seg3[1].phoneme,"h#");
    }

    free_vector((char *)label1,0,sizeof(int));
    free_vector((char *)label2,0,sizeof(int));
    free_vector((char *)label3,0,sizeof(int));
    return seg3;
}
  


/* 
   Remove crosstalks in nist12 files
*/
SEGMENT *remove_crosstalk_SRE12(SEGMENT *seg1, SEGMENT *seg2, unsigned long num_samples, char channel)
{
    unsigned long i,j,t,tot_num_segs,num_sph_segs,num_sph_samples,num_sph_frms;
    char *phoneme;
    int *label1,*label2,*label3; 
    SEGMENT *seg3;
    int max_num_segs = 50000;
    vec_t *amp1,*amp2;                            // Mean amplitude of segments, replicated on every time point

    seg3 = (SEGMENT *)vector(0,max_num_segs-1,sizeof(SEGMENT));
    label1 = (int *)vector(0, num_samples, sizeof(int));
    label2 = (int *)vector(0, num_samples, sizeof(int));
    label3 = (int *)vector(0, num_samples, sizeof(int));
    amp1 = (vec_t *)vector(0, num_samples, sizeof(vec_t));
    amp2 = (vec_t *)vector(0, num_samples, sizeof(vec_t));

    // Set label1[t]=1 if sample t of channel A falls on speech segment
    for (i=0; i<seg1[0].num_segs; i++) {
	 phoneme = seg1[i].phoneme;
	 if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	     continue;
	 for (t=seg1[i].begin; t<=seg1[i].end; t++) {
	     label1[t] = 1;
	     amp1[t] = seg1[i].mean_namp;
	 }
    }
    
    // Set label2[t]=1 if sample t of channel B falls on speech segment
    for (i=0; i<seg2[0].num_segs; i++) {
	 phoneme = seg2[i].phoneme;
	 if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	     continue;
	 for (t=seg2[i].begin; t<=seg2[i].end; t++) {
	     label2[t] = 1;
	     amp2[t] = seg2[i].mean_namp;
	 }
    }

    
    if (channel == 'A') {
	//Frame t falls on a crosstalk region of ChA if label2[t]==1 and ChB's norm amplitude > ChA's norm amplitude
	for (t=0; t<num_samples; t++) {
	    label3[t] = label1[t];
	    //printf("%ld: %d %d %f %f\n",t,label1[t],label2[t],amp1[t],amp2[t]);
	    if ((label2[t]==1) && (amp2[t]>amp1[t])) {
		label3[t] = 0;
	    }
	}
    } else {
	//Frame t falls on a crosstalk region of ChB if label1[t]==1 and ChA's norm amplitude > ChB's norm amplitude
	for (t=0; t<num_samples; t++) {
	    label3[t] = label2[t];
	    if ((label1[t]==1) && (amp1[t]>amp2[t])) {
		label3[t] = 0;
	    }
	}
    }

    // Assign values to seg3 according to label3[]
    j = 0;
    t = 0;
    tot_num_segs = 0;
    num_sph_samples = 0;
    num_sph_frms = 0;
    while (t<num_samples) {
	seg3[j].begin = t;
	if (label3[t] == 0) {
	    strcpy(seg3[j].phoneme,"h#");            // Crosstalk or silence
	    while (label3[t] == 0) {                 // Search for next segment
		t++;
	    }
	    seg3[j].end = t;
	    seg3[j].num_samples = t-seg3[j].begin+1;
	} else {
	    strcpy(seg3[j].phoneme,"S");             // Speech segment
	    while (label3[t] == 1) {                 // Search for next segment
		t++;
	    }
	    seg3[j].end = t;
	    seg3[j].num_samples = t-seg3[j].begin+1;
	    if (seg3[j].num_samples >= WIN_SIZE) {   // Assume minimum frame size is 200
		num_sph_samples += seg3[j].num_samples;
		num_sph_frms += (seg3[j].num_samples-WIN_SIZE)/WIN_ADV+1;;
	    }	    
	}
	j++;                                         // next segment
	tot_num_segs++;
    }
    for (j=0; j<tot_num_segs; j++)
	seg3[j].num_segs = tot_num_segs;

    // If there is no speech segment, artificially assign one speech segment using the following
    // one silence segment. This helps to avoid problem in sph2cep.c
    num_sph_segs = 0;
    for (j=0; j<tot_num_segs; j++) {
	if (strcmp(seg3[j].phoneme,"S")==0)
	    num_sph_segs++;
    }
    if (num_sph_segs == 0 || num_sph_frms == 0 || num_sph_samples == 0) {
	printf("No speech segment, artificially assign one to help sph2cep.c\n");
	seg3[0].num_segs = 2;
	seg3[0].begin = 0;
	seg3[0].end = 512;
	strcpy(seg3[0].phoneme,"S");
	seg3[1].num_segs = 2;
	seg3[1].begin = seg3[0].end;
	seg3[1].end = seg1[tot_num_segs-1].end;
	strcpy(seg3[1].phoneme,"h#");
    }

    free_vector((char *)label1,0,sizeof(int));
    free_vector((char *)label2,0,sizeof(int));
    free_vector((char *)label3,0,sizeof(int));
    free_vector((char *)amp1,0,sizeof(vec_t));
    free_vector((char *)amp2,0,sizeof(vec_t));
    return seg3;
}
 


