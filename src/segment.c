/*
  File		:segment.c
  Author	:M.W. Mak
  Date		:30 April 96
  Last update	:
  Description	:This module extracts the non-silence speech from the sample[]
 		 array using the seg structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mmalloc.h"
#include "segment.h"
#include "veclib.h"

/*
  PhnFileRead: read the .phn file and return an array of SEGMENT
  	       structure containing begining and end of segments and
	       the phonetic symbols corresponding to the segments
  Input: (char *)PhnFileName
  Output: return an array of SEGMENT structure on success [0..num_seg-1]
	  return NULL on failure
*/
SEGMENT *PhnFileRead(char *PhnFileName)
{
     FILE 	*phnfile;
     SEGMENT 	*seg;
     int	num_segs;		/* Number of segment in .phn file */
     int	i;
     char	buf[10];
     int	dummy;

     if ((phnfile=fopen(PhnFileName,"r"))==NULL) {
	 fprintf(stderr,"Error in opening %s\n",PhnFileName);
         exit(EXIT_FAILURE);
     }

     /* Parse .phn file and determine the number of segment */
     num_segs = 0;
     while (fscanf(phnfile,"%d %d %s\n",&dummy,&dummy,buf)!=EOF)
	 num_segs++;
     rewind(phnfile);

     /* Check whether there is any segments */
     if (num_segs<1) {
         fprintf(stderr,"Warning: No segment is defined in %s\n",PhnFileName);
	 return((SEGMENT *)NULL);
     }

     if (num_segs==1) {
         fprintf(stderr,"Warning: %s contains only one segment, possibly silence or non-speech\n",PhnFileName);
	 //return((SEGMENT *)NULL);
     }

     /* Allocate memory for the segs[] array */	
     seg = (SEGMENT *)vector(0,num_segs-1,sizeof(SEGMENT));

     /* Read the .phn file again to get segment information */
     for (i=0; i<num_segs; i++){
	 if (fscanf(phnfile,"%d %d %s\n", &seg[i].begin, &seg[i].end, 
		    seg[i].phoneme)!=3){
	     fprintf(stderr,"Warning: incorrect .phn file format, no segment is defined");
	     return((SEGMENT *)NULL);	
	 }
	 /* Last samples specified in the .phn file are regarded as the begining
	    samples of the next segment. So, the last sample in the current
	    segment must be substracted by one */
	 seg[i].end--;	
	 seg[i].num_samples = seg[i].end-seg[i].begin+1;
	 seg[i].num_segs = num_segs;
     }
     return(seg);
}


/*
  PhnFileWrite:write the .phn file 
  Input: (char *)PhnFileName: .phn file to be written
         (SEGMENT *)segment: array of SEGMENT structure
*/

void PhnFileWrite(char *phnfilename, SEGMENT *seg)
{
     int s;
     int num_segs;
     FILE *phnfile;
     
     if ((phnfile=fopen(phnfilename,"w"))==NULL) {
	 fprintf(stderr,"Unable to open %s for write\n",phnfilename);
	 exit(EXIT_FAILURE);
     }

     num_segs = seg[0].num_segs;
     for (s=0; s<num_segs; s++)
         fprintf(phnfile,"%d %d %s\n",seg[s].begin,seg[s].end,seg[s].phoneme);
     fclose(phnfile);
}


/*
  MrkFileRead: read the .mrk file and return an array of SEGMENT
  	       structure containing begining and end of segments and
	       the word corresponding to the segments
  Input: (char *)MrkFileName
         int freq: sampling frequency of the corresponding wave file
  Output: return an array of SEGMENT structure on success [0..num_seg-1]
	  return NULL on failure
*/
SEGMENT *MrkFileRead(char *MrkFileName, char channel, int freq)
{
     FILE 	*mrkfile;
     SEGMENT 	*seg;
     int	num_segs;		/* Number of segment in .phn file */
     int	i;
     char	buf1[5],buf2[10],buf3[10],buf4[30],*p;

     if ((mrkfile=fopen(MrkFileName,"r"))==NULL) {
	 fprintf(stderr,"Warning: Error in opening %s in segment.c\n",MrkFileName);
	 fprintf(stderr,"         No segment is defined\n");         
         return((SEGMENT *)NULL);
     }

     //**************************************************************** 
     // Parse .mrk file and determine the number of segments that corresponds to
     // the specified channel. Exclude those non-speech segments
     // We need to check whether the last column in the .mrk file contains
     // spaces or not. If yes, we need special code to read the space and the
     // characters after it. We also need to insert '\0' at the end of string 
     //*****************************************************************
     num_segs = 0;
     while (fscanf(mrkfile,"%s %s %s %s",buf1,buf2,buf3,buf4)!=EOF) {
         p = &buf4[strlen(buf4)];                   // Point to the null terminator
         while((*p++ = getc(mrkfile))!='\n')        // Read space and beyond if there is any
	    ;       
	 *(--p) = '\0';                             // Make null terminate string
         if (buf1[0]==channel && buf2[0]!='*')
	     num_segs++;
     }
     rewind(mrkfile);

     /* Check whether there is any segments */
     if (num_segs<1) {
         fprintf(stderr,"Warning: No segment is defined in %s\n",MrkFileName);
	 return((SEGMENT *)NULL);
     }

     if (num_segs==1) {
         fprintf(stderr,"Warning: Only one segment, possibly silence or non-speech\n");
	 return((SEGMENT *)NULL);
     }

     /* Allocate memory for the segs[] array */	
     seg = (SEGMENT *)vector(0,num_segs-1,sizeof(SEGMENT));

     /* Read the .mrk file again to get segment information */
     i=0;
     while (fscanf(mrkfile,"%s %s %s %s",buf1,buf2,buf3,buf4)!=EOF) 
     {
         p = &buf4[strlen(buf4)];                   // Point to the null terminator
         while((*p++ = getc(mrkfile))!='\n')        // Read space and beyond if there is any
	    ;       
	 *(--p) = '\0';                             // Make null terminate string
//	 printf("%d %s\n",i+1,buf4);fflush(stdout);

	 /* Last samples specified in the .mrk file are regarded as the begining
	    samples of the next segment. So, the last sample in the current
	    segment must be substracted by one */
         if (buf1[0]==channel && buf2[0]!='*') {
	     seg[i].begin = (int)ceil(freq*atof(buf2));
	     seg[i].num_samples = (int)ceil((atof(buf3)*freq));
	     seg[i].end = seg[i].begin+seg[i].num_samples-1;
	     seg[i].num_segs = num_segs;
	     strcpy(seg[i].phoneme,buf4);
	     i++;
         }
     }
     return(seg);
}



/*
  Extract from the input array sample[] those non-silence samples according
  to the segment information
  Input:  segment structure, and array containing time domain speech samples
  Return: array containing non-silence samples, actual number of non-silence
          samples [0..num_samples-1]
*/
short *extract_sample(SEGMENT *seg, short *sample, unsigned long *num_samples)
{
      int	i,j,k;
      short	*ssample;
      char	*phoneme;	
		
      /* Find the total number of non-silence samples */
      *num_samples=0;
      for (i=0; i<seg[0].num_segs; i++) {
	  phoneme = seg[i].phoneme;
          if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 ||
	      strcmp(phoneme,"epi")==0)
	      continue;  
	  *num_samples += seg[i].num_samples;
      }

      /* For each segment, copy non-silence samples from sample[] to ssample[] */	
      ssample = (short *)vector(0,*num_samples-1,sizeof(short));	
      for (i=0,j=0; i<seg[0].num_segs; i++) {
	  phoneme = seg[i].phoneme;
          if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 ||
	      strcmp(phoneme,"epi")==0)
	      continue;  				/* skip silence segments */
	  for (k=0; k<seg[i].num_samples; k++)
	        ssample[j+k]=sample[seg[i].begin+k];
	  j += seg[i].num_samples;		/* adjust index for sample[] */
      }	
      return(ssample);	
}

short *extract_sph_sample(SEGMENT *seg, short *sample, unsigned long *num_samples)
{
    return(extract_sample(seg, sample, num_samples));
}



/*
  Extract from the input array sample[] those silence (non-speech) samples according
  to the segment information
  Input:  segment structure, and array containing time domain speech samples
  Return: array containing silence samples, actual number of silence
          samples [0..num_samples-1]
*/
short *extract_bkg_sample(SEGMENT *seg, short *sample, unsigned long *num_samples)
{
      int	i,j,k;
      short	*bsample;
      char	*phoneme;	
		
      /* Find the total number of silence samples */
      *num_samples=0;
      for (i=0; i<seg[0].num_segs; i++) {
	  phoneme = seg[i].phoneme;
          if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	      *num_samples += seg[i].num_samples;
      }

      /* For each segment, copy silence samples from sample[] to ssample[] */	
      bsample = (short *)vector(0,*num_samples-1,sizeof(short));	
      for (i=0,j=0; i<seg[0].num_segs; i++) {
	  phoneme = seg[i].phoneme;
          if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0) {
	      for (k=0; k<seg[i].num_samples; k++)
		  bsample[j+k]=sample[seg[i].begin+k];
	      j += seg[i].num_samples;		/* adjust index for sample[] */
	  }
      }	
      return(bsample);	
}



/*
  Extract from input vector sequence x[0..N-1][1..D] the non-silence frames,
  where N is the number of frames and D is the vector dimension. 
  Output the non-silence vector sequence in x[0..M-1][1..D] where M is the
  number of non-silence frames.
  The function returns the number of non-silence frames
*/
int remove_silence_frames(SEGMENT *seg, int frame_size, int frame_adv, int N, int D, vec_t **x)
{
    int   i,j,k,r,s;
    int   M;                   // Total number of nonsilence frames
    int   K;                   // No. of nonsilence frames in the current segment
    char  *phoneme;            // Phoneme of the current segment
    vec_t **y = (vec_t **)matrix(0,N-1,1,D,sizeof(vec_t),sizeof(vec_t  *));
		
    /* For each segment, copy the nonsilence frame from x[][] to y[][] */
    r=s=0;
    for (i=0; i<seg[0].num_segs; i++) {
	phoneme = seg[i].phoneme;
        if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0) {
	    continue;                           /* skip silence segments */
	}  				        
	k = seg[i].begin/frame_adv+1;           /* Determine the starting frame */
	K = (seg[i].num_samples-frame_size)/frame_adv+1;     // No. of frames in current segment
	if (K < 1) {
	    continue;                           /* Samples less than one frame */
	}
	//printf("Frame starts at %d, %d.  Frame end at %d %d\n", k, seg[i].begin, k+K, seg[i].end);
	for (s=0; s<K; s++) {                   /* Copy K nonsilence frames from x[][] to y[][] */
	    if (k+s>=N)
		break;                          /* Just in case the .phn file has more frames than x[][] */
	    for (j=1; j<=D; j++) {
		y[r+s][j] = x[k+s][j];
	    }
	}
	if (k+K<N)
	    r += K;
	else {
	    r += s;
	    break;
	}
    }
    M = r;

    /* Update x[][] */
    VECcopyf(N*D, &x[0][1], &y[0][1]);
    free_matrix((char **)y, 0, 1, sizeof(vec_t));
    return(M);	
}


/* 
 * Extract speech from from feature matrix Y[][].
 * sampPeriod = Frame rate in 100ns, e.g., 10ms frame adv => sampPeriod=100000. See HTK book p. 91 
*/
vec_t **get_sph_frames(vec_t **Y, SEGMENT *seg, unsigned int *num_frms, int sampPeriod, int sampleFreq)
{
    char *phoneme;
    int i;
    unsigned int frameAdv;
    unsigned int frameStart;
    unsigned int frameEnd;
    vec_t **Z;
    unsigned int num_sph_frms = 0;
    unsigned int n = 1;
    unsigned int f;

    frameAdv = (unsigned int)(((float)(sampleFreq)/(float)sampPeriod)*1000);
    Z = (vec_t **)vector(1,*num_frms,sizeof(vec_t *));

    for (i=0; i<seg[0].num_segs; i++) {
	phoneme = seg[i].phoneme;
	if (strcmp(phoneme,"h#")==0 || strcmp(phoneme,"pau")==0 || strcmp(phoneme,"epi")==0)
	    continue;  				/* skip silence segments */
	frameStart = seg[i].begin/frameAdv + 1;
	frameEnd = (seg[i].end-frameAdv)/frameAdv + 1;
	if (frameStart > *num_frms)
	    frameStart = *num_frms;
	if (frameEnd > *num_frms)
	    frameEnd = *num_frms;
	num_sph_frms += frameEnd-frameStart+1;
	for (f=frameStart; f<=frameEnd; f++) {
	    Z[n] = Y[f];
	    n++;
	}
    }
    *num_frms = num_sph_frms;
    return(Z);
}


void print_seg_info(SEGMENT *seg, int wininc, unsigned long num_samples)
{
    unsigned long num_sph_samples = 0;
    int i;
    vec_t sph_ratio;

    for (i=0; i<seg[0].num_segs; i++) {
	if (seg[i].phoneme[0] == 'S')
	     num_sph_samples += seg[i].num_samples;
    }
    sph_ratio = (vec_t)num_sph_samples/(vec_t)num_samples;
    printf("Num_sph_frames=%ld, Num_segments=%d, Num_sph_samples=%ld, sph_ratio=%.2f%%\n",
           num_sph_samples/wininc,seg[0].num_segs,num_sph_samples,sph_ratio*100);
}
