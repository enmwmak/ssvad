/*   Module        : wavio.h (Function prototypes for module WAVIO.C)
     Author        : M.W. Mak (H.K. PolyU)

     Last modified : 24. Dec 1995

     This module contains the subroutines to record and playback .wav
     file

     The following functions are provided within the module WAVIO.C:
*/
/* Structure defininng the wav header */

#ifndef __WINWAV_INCLUDE__
#define __WINWAV_INCLUDE__

#ifndef __WORDLENGTH__
#define __WORDLENGTH__
typedef short INT16;
typedef int   INT32;
#endif

typedef struct {
        char    riff[4];              /* Usually: RIFF */
        INT32   filesize;
        char    type[4];              /* RIFF form type, usually WAVE */
        char    fmt[4];              /* Start of <fmt-ck>, usually 'fmt ' */
        INT32   format;              /* Default is 16 */
        INT16    fmtag;               /* Format tag, 1 for PCM */
        INT16    channel;             /* number of channels */
        INT32   srate;               /* sampling freq */
        INT32   drate;               /* Average data rate */
        INT16    align;               /* Block alignment, 1 for mono */
        INT16    bps;                 /* bits per sample */
        char    data[4];             /* start of <data-ck> */
        INT32   dsize;               /* size of sampled data in byte */
} WAV_HDR;


/*
        drate = channel*srate*bps/8
	align = channel*bps/8
	dsize = num_samples*bps/8
*/

/* Function to read and write winwav (.wwv) file */
INT16 *WavRead(char *WavFileName, WAV_HDR *WavHdr,unsigned long *num_smps);
void WavWrite(char *WavFileName, WAV_HDR *WavHdr, void *samples);
void wavwrite(short *sample, unsigned long num_samples, unsigned long sr, 
	      unsigned long bps, char *wavfilename);

#endif



























