/* 
   Filename: sph_io.c (formerly wav_io.c) 
   Version: 1.0
   Date: 9 Nov. 95
   Last update: 10 July 03
   Author: M.W.Mak
   Modified: On 18 May 01. To change read_wav_file so that wav2cep can read SPIDRE corpus
   Modified: 10 July 02. Bug in parameter "sample_coding" fixed. Now, we past pointer
                         to SP_STRING to read_wav_file() for the parameter sample_coding.
                         Default option for sp_set_data_mod() in read_wav_file() has been
			 added.
*/   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sp/sphere.h>
#include "mmalloc.h"
#include "sph_io.h"

/*******************************************************************
   Read the wave file in the PCM-2 or RAW format of the TIMIT database.
   On success, it returns a short int array containing num_samples samples;
   otherwise, it return a NULL pointer. It also return an error code if
   an NULL pointer is returned
   
   Error code:
       -1: if there is an EOF error.
       -2: file error occured
       -3: file cannot be opened.
       -4: error on getting the header field.
   
   Input parameters:
   char *wavefilename: name of the wave file to be read
   char channel_id: 'A' (channel 1) or 'B' (channel 2)

   Output parameters:
   unsigned long *sample_read: number of sample read
   int  *err_code: error code to be returned if NULL pointer is returned.
   SP_INTEGER *byte_per_sample: number of byte per sample
   SP_INTEGER *num_channels: number of channels in wave file
   SP_INTEGER *sample_rate: sampling rate in Hz.
********************************************************************/   
short *read_wav_file(char *wavfilename, unsigned long *tot_sample_read,
		     SP_INTEGER *byte_per_sample, SP_INTEGER *num_channels,
		     SP_INTEGER *sample_rate, SP_STRING *sample_coding, 
		     char channel_id, int *err_code)
{
    SP_FILE *wavfile;
    unsigned long total_samples, sample_read, k, i, blksize, num_blks;
    SP_INTEGER sample_count;
    short *waveform = (short *)NULL;
    int err=0;
 
    *err_code = 0;
    //************************************************ 
    //  Open SPHERE wave file
    //************************************************
    if ((wavfile = sp_open(wavfilename,"r"))==(SP_FILE *)0){
        fprintf(stderr,"Error: Unable to open SPHERE file %s\n",wavfilename);
	sp_print_return_status(stderr);
	*err_code=SP_FILE_OPEN_ERR;
	sp_close(wavfile);
	return((short *)NULL);
    }

    //************************************************ 
    // Get sample coding type from wave file header
    //************************************************
    if (sp_h_get_field(wavfile,"sample_coding",T_STRING,(void **)sample_coding)>0){
	sp_print_return_status(stderr);
        *err_code=SP_GET_HFIELD_ERR;
	sp_close(wavfile);
        return((short *)NULL);
    }

    //******************************************************************* 
    // Set sample coding type in memory after reading from file to PCM-2.
    // See /usr/local/nist/doc/sphere.doc for format information.
    //******************************************************************* 
    switch(channel_id) {
	case 'A': err=sp_set_data_mode(wavfile,"SE-PCM-2:CH-1");break;
        case 'B': err=sp_set_data_mode(wavfile,"SE-PCM-2:CH-2");break;
	default : err=sp_set_data_mode(wavfile,"SE-PCM:SBF-N");break;
    }
    if (err!=0) {
        fprintf(stderr,"Error: Unable to set data_mode to SE-PCM-2:CH-%c in %s\n",channel_id,wavfilename);
	sp_print_return_status(stderr);
	sp_close(wavfile);
	return((short *)NULL);
    }

    //******************************************************************* 
    // Get other field from the file header
    //******************************************************************* 
    if (sp_h_get_field(wavfile,"sample_count",T_INTEGER,(void **)&sample_count)>0){
	sp_print_return_status(stderr);
        *err_code=SP_GET_HFIELD_ERR;
	sp_close(wavfile);
        return((short *)NULL);
    }
    if (sp_h_get_field(wavfile,"sample_n_bytes",T_INTEGER,(void **)byte_per_sample)>0){
	sp_print_return_status(stderr);
        *err_code=SP_GET_HFIELD_ERR;
	sp_close(wavfile);
        return((short *)NULL);
    }
    if (sp_h_get_field(wavfile,"channel_count",T_INTEGER,(void **)num_channels)>0){
	sp_print_return_status(stderr);
        *err_code=SP_GET_HFIELD_ERR;
	sp_close(wavfile);
        return((short *)NULL);
    }
    if (sp_h_get_field(wavfile,"sample_rate",T_INTEGER,(void **)sample_rate)>0){
	sp_print_return_status(stderr);
        *err_code=SP_GET_HFIELD_ERR;
	sp_close(wavfile);
        return((short *)NULL);
    }

    //******************************************************************* 
    // Allocate sufficient data for storing samples
    // Note: only need to allocate data for one channel
    // Note: Need to read a block at a time and not to the end of file
    //       for nist02 .sph files
    //******************************************************************* 
    total_samples = sample_count;
    if ((waveform = (short *)sp_data_alloc(wavfile,total_samples))==(short *)0) {
        fprintf(stderr, "Fetal Error: Unable to allocate memory for storing waveform in %s\n",wavfilename);
	sp_print_return_status(stderr);
	sp_close(wavfile);
	exit(EXIT_FAILURE);
    }

    *tot_sample_read = 0;
    blksize = 1024;
    num_blks = sample_count/blksize;
    for (i=0; i<num_blks; i++) {
	k = i*blksize;
        sample_read = sp_read_data(&waveform[k], blksize, wavfile);
	*tot_sample_read += sample_read;
    }
    if (sp_error(wavfile)){
	   printf("IO error\n");fflush(stdout); 
	   *err_code = SP_FILE_IO_ERR;
	   sp_close(wavfile);
	   return((short *)NULL);
    }

    /* Close wavfile */
    sp_close(wavfile);

    return(waveform);
}

/*******************************************************************
   Write the wave file in the PCM-2 format or in the ORIG format 
   of the TIMIT database.
   On success, it returns the number of samples written;
   otherwise, it return -1. 
   
   Input parameters:
   char *wavefilename: name of the wave file to be written
   short *sample:      buffer storing PCM samples
   int  num_samples:   number of samples to be written

   Output parameters:
   int *err_code

   Return: 
   unsigned long sample_written: number of sample actually written
********************************************************************/   
long write_wav_file(char *wavfilename, void *sample, SP_INTEGER num_samples,
		    SP_INTEGER byte_per_sample, SP_INTEGER s_rate)
{
    SP_FILE *wavfile;
    SP_INTEGER c_count=1;    
    long status;

    if ((wavfile = sp_open(wavfilename,"w"))==(SP_FILE *)0){
        fprintf(stderr,"Error: Unable to open SPHERE file %s\n",wavfilename);
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_count",T_INTEGER,&num_samples)>0){
        fprintf(stderr,"Error: Unable to set sample count\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_n_bytes",T_INTEGER,&byte_per_sample)>0){
        fprintf(stderr,"Error: Unable to set sample_n_bytes\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_rate",T_INTEGER,&s_rate)>0){
        fprintf(stderr,"Error: Unable to set sample_rate\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (byte_per_sample==1) {
        if (sp_h_set_field(wavfile,"sample_byte_format",T_STRING,(void *)"1")>0){
            fprintf(stderr,"Error: Unable to set sample_byte_format to 1\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
        if (sp_h_set_field(wavfile,"sample_coding",T_STRING,(void *)"raw")>0){
	    fprintf(stderr,"Error: Unable to set sample_coding to raw\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
    }
    if (byte_per_sample==2) {
        if (sp_h_set_field(wavfile,"sample_byte_format",T_STRING,(void *)"01")>0){
            fprintf(stderr,"Error: Unable to set sample_byte_format to 01\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
        if (sp_h_set_field(wavfile,"sample_coding",T_STRING,(void *)"pcm")>0){
	    fprintf(stderr,"Error: Unable to set sample_coding to pcm\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    }
    if (sp_h_set_field(wavfile,"channel_count",T_INTEGER,&c_count)>0){
        fprintf(stderr,"Error: Unable to set channel_count\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (byte_per_sample==1) {
        if (sp_set_data_mode(wavfile,"SE-ORIG")!=0) {
            fprintf(stderr,"Error: Unable to set data_mode to SE-ORIG\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    }
    if (byte_per_sample==2) {
        if (sp_set_data_mode(wavfile,"SE-PCM-2")!=0) {
            fprintf(stderr,"Error: Unable to set data_mode to SE-PCM-2\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    } 
    if (byte_per_sample!=1 && byte_per_sample!=2)
    {
	fprintf(stderr,"Error: Byte per sample must be either 1 or 2\n");
	exit(EXIT_FAILURE);
    }

    /* Write sample[] to wavfile */
    status = sp_write_data((void *)sample, num_samples, wavfile);
    if (status!=num_samples) {
        fprintf(stderr,"Unable to write NIST waveform file: %s\n",wavfilename);
	sp_print_return_status(stderr);
	status=sp_error(wavfile);
	sp_print_return_status(stderr);
	sp_close(wavfile);
	exit(EXIT_FAILURE);
    }
    
    /* Close wavfile */
    if (sp_close(wavfile)!=0) {
        fprintf(stderr,"Unable to close NIST waveform file: %s\n",wavfilename);
	sp_print_return_status(stderr);
	exit(EXIT_FAILURE);
    }
    return(num_samples);
}


/*******************************************************************
   Write 2ch data to wave file in the PCM-2 format or in the ORIG format 
   of the TIMIT database.
   On success, it returns the number of samples written;
   otherwise, it return -1. 
   
   Input parameters:
   char *wavefilename: name of the wave file to be written
   short **sample:     buffer storing PCM samples
   int  num_samples:   number of samples to be written

   Output parameters:
   int *err_code

   Return: 
   unsigned long sample_written: number of sample actually written
********************************************************************/   
long write_2ch_wav_file(char *wavfilename, short **sample, SP_INTEGER num_samples,
			SP_INTEGER byte_per_sample, SP_INTEGER s_rate)
{
    SP_FILE *wavfile;
    SP_INTEGER c_count=2;    
    long status;

    if ((wavfile = sp_open(wavfilename,"w"))==(SP_FILE *)0){
        fprintf(stderr,"Error: Unable to open SPHERE file %s\n",wavfilename);
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_count",T_INTEGER,&num_samples)>0){
        fprintf(stderr,"Error: Unable to set sample count\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_n_bytes",T_INTEGER,&byte_per_sample)>0){
        fprintf(stderr,"Error: Unable to set sample_n_bytes\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (sp_h_set_field(wavfile,"sample_rate",T_INTEGER,&s_rate)>0){
        fprintf(stderr,"Error: Unable to set sample_rate\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (byte_per_sample==1) {
        if (sp_h_set_field(wavfile,"sample_byte_format",T_STRING,(void *)"1")>0){
            fprintf(stderr,"Error: Unable to set sample_byte_format to 1\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
        if (sp_h_set_field(wavfile,"sample_coding",T_STRING,(void *)"raw")>0){
	    fprintf(stderr,"Error: Unable to set sample_coding to raw\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
    }
    if (byte_per_sample==2) {
        if (sp_h_set_field(wavfile,"sample_byte_format",T_STRING,(void *)"01")>0){
            fprintf(stderr,"Error: Unable to set sample_byte_format to 01\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
	}
        if (sp_h_set_field(wavfile,"sample_coding",T_STRING,(void *)"pcm")>0){
	    fprintf(stderr,"Error: Unable to set sample_coding to pcm\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    }
    if (sp_h_set_field(wavfile,"channel_count",T_INTEGER,&c_count)>0){
        fprintf(stderr,"Error: Unable to set channel_count\n");
	sp_print_return_status(stderr); exit(EXIT_FAILURE);
    }
    if (byte_per_sample==1) {
        if (sp_set_data_mode(wavfile,"SE-ORIG")!=0) {
            fprintf(stderr,"Error: Unable to set data_mode to SE-ORIG\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    }
    if (byte_per_sample==2) {
        if (sp_set_data_mode(wavfile,"SE-PCM-2")!=0) {
            fprintf(stderr,"Error: Unable to set data_mode to SE-PCM-2\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
        if (sp_set_data_mode(wavfile,"DF-ARRAY")!=0) {
            fprintf(stderr,"Error: Unable to set data_mode to DF-ARRAY\n");
	    sp_print_return_status(stderr); exit(EXIT_FAILURE);
        }
    } 
    if (byte_per_sample!=1 && byte_per_sample!=2)
    {
	fprintf(stderr,"Error: Byte per sample must be either 1 or 2\n");
	exit(EXIT_FAILURE);
    }

    /* Write sample[] to wavfile */
    status = sp_write_data((void *)sample, num_samples, wavfile);
    if (status!=num_samples) {
        fprintf(stderr,"Unable to write NIST waveform file: %s\n",wavfilename);
	sp_print_return_status(stderr);
	status=sp_error(wavfile);
	sp_print_return_status(stderr);
	sp_close(wavfile);
	exit(EXIT_FAILURE);
    }
    
    /* Close wavfile */
    if (sp_close(wavfile)!=0) {
        fprintf(stderr,"Unable to close NIST waveform file: %s\n",wavfilename);
	sp_print_return_status(stderr);
	exit(EXIT_FAILURE);
    }
    return(num_samples);
}



void dump_sample(char *pcmfilename, short *sample, unsigned long num_samples)
{
     FILE *fp;
     int i;

     if ((fp = fopen(pcmfilename,"wb"))==NULL) {
         fprintf(stderr,"Error in dump_sample in convert.c: File open error\n");
	 exit(EXIT_FAILURE);
     }
     printf("Dumping %ld samples to %s...\n",num_samples,pcmfilename);
     for (i=0; i<num_samples; i++) {
         if ((fwrite(&sample[i],sizeof(short),1,fp))!=1) {
            fprintf(stderr,"Error in dump_sample in convert.c: File write error\n");
	    exit(EXIT_FAILURE);
         }
     }
     fclose(fp);
}
 
