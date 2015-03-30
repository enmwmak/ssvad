/* ------------------------------------------------------------------------ */
/*  TITLE : winwav.c                                                         */
/*  Author: M.W. Mak                                                        */
/*  DESCRIPTION :                                                           */
/*     This file reads and writes MS windows .wav file to and from memory   */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "winwav.h"

INT16 *WavRead(char *WavFileName, WAV_HDR *WavHdr, unsigned long *num_smps)
{
    FILE    *wavfile;
    INT16   *sample;
    INT32   num_samples,i;

    if ((wavfile=fopen(WavFileName,"rb"))==NULL) {
        fprintf(stderr,"%s not found\n",WavFileName);
        return ((INT16 *)NULL);
    }

    if (fread(WavHdr,sizeof(WAV_HDR),1,wavfile)!=1) {
        fprintf(stderr,"Error in reading %s file header\n",WavFileName);
        return((INT16 *)NULL);
    }

    num_samples = WavHdr->dsize/(WavHdr->channel*WavHdr->bps/8);
    *num_smps = num_samples;
    sample=(INT16 *)malloc(num_samples*sizeof(INT16));

    if (!sample) {
        fprintf(stderr,"Insufficient memory to allocate sample[]\n");
	exit(EXIT_FAILURE);
    }

    if (WavHdr->bps==8) {           /* 8 bits per sample*/
        for (i=0;i<num_samples;i+=WavHdr->channel)
        {
            if (fread((unsigned char *)&sample[i],1,1,wavfile)!=1) {
                fprintf(stderr,"\nError in reading %s data",WavFileName);
                return((INT16 *)NULL);
            }
            sample[i]-=128;
        }
    }

    if (WavHdr->bps==16) {          /* 16 bits per sample*/
        for (i=0; i<num_samples; i+=WavHdr->channel)
	{
            if (fread(&sample[i],sizeof(INT16),1,wavfile)!=1) {
                fprintf(stderr,"Error in reading %s data\n",WavFileName);
                return((INT16 *)NULL);
	    }
        }
    }
    fclose(wavfile);
    return((INT16 *)sample);
}

void WavWrite(char *WavFileName, WAV_HDR *WavHdr, void *sample)
{
     FILE *wavfile;
     INT32 num_samples;

     num_samples = WavHdr->dsize/(WavHdr->channel*WavHdr->bps/8);
     if ((wavfile=fopen(WavFileName,"wb"))==NULL) {
        fprintf(stderr,"Error in opening %s\n",WavFileName);
	exit(EXIT_FAILURE);
     }
     fwrite((WAV_HDR *)WavHdr,sizeof(WAV_HDR),1,wavfile);
     if (WavHdr->bps==16)
        fwrite((INT16 *)sample,2,num_samples,wavfile);
     else
        fwrite((unsigned char *)sample,1,num_samples,wavfile);
     fclose(wavfile);
}


/* Same format as Matlab wavwrite */
void wavwrite(short *sample, unsigned long num_samples, unsigned long sr, 
	      unsigned long bps, char *wavfilename)
{
    WAV_HDR wh;
    strncpy(wh.riff,"RIFF",4);
    strncpy(wh.type,"WAVE",4);
    wh.format = bps*8;
    strncpy(wh.fmt,"fmt ",4);
    wh.fmtag = 1;
    wh.channel = 1;
    wh.srate = sr;
    wh.drate = sr*bps;
    wh.align = 2;
    wh.bps = bps*8;
    strncpy(wh.data,"data",4);
    wh.dsize = num_samples*bps;
    wh.filesize = wh.dsize + sizeof(WAV_HDR);
    WavWrite(wavfilename, &wh, sample);
}

/* End of file */

 

