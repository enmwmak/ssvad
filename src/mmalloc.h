/**********************************************************************
 MEMORY ALLOCATION LIBRARY (header file):
    This library contains routines which allocate and free vector or
    array of any data object.  The matrix or array size can be greater
    than 64Kbyte provided that huge pointer is used and compile under
    compact memory model.
    The library is designed to run on either MSDOS/Helios/Unix OS.
    if MSDOS Brolandc is used, a statement #define _BORLANDC_ should
    be added before the statement #include "malloc.h".
 Filename : mmalloc.h
 Memory model : compact
 Author : M.W.Mak
 Date   : 15/2/92
 Version : V2.0
************************************************************************/

/*#define _BORLANDC_*/			/* compile under BC */
#define _ANSI_                     /* compile under ANSI gcc in UNIX */

#ifndef _MMALLOC_INCLUDED_
  #define _MMALLOC_INCLUDED_
  #ifdef  _BORLANDC_		/* borland C support far data */
    #include <alloc.h>              /* farmalloc need it */
    char huge *x_farmalloc(unsigned long size);
    char huge *x_farcalloc(unsigned long size);
    char huge **alloc_farmatrix(int,int,int,int,int,int);
    void free_farmatrix(char huge **, int,int,int);
    char huge *alloc_farvector(long,long,long);
    void free_farvector(char huge *,long,long);
  #else				/* Helios OS, don't need to used far data */
    #ifndef  far
       #define  far
       #define  huge
    #endif   			/* end if far */
    #define  farmalloc(a) malloc(a)
    #define  farfree(a) free(a)
  #endif			/* end if _BORLANDC_ */


#if defined _ANSI_ || defined _BORLANDC_
float **fmatrix(int,int,int,int);
char huge **matrix(int,int,int,int,int,int);
char huge *vector(long, long, long);
void free_matrix(char huge **, int,int,int);
void free_vector(char huge *,long,long);
float huge **convert_matrix(float huge *a, int nrl, int nrh, int ncl, int nch);
float huge **submatrix(float huge **a, int oldrl,int oldrh,int oldcl, int oldch,int newrl, int newcl);
void free_submatrix(float huge **m, int nrl, int nrh, int ncl, int nch);
void free_convert_matrix(float huge **m, int nrl, int nrh, int ncl, int nch);
void nrerror(char *err_text);
void    put_error(char *err_msg);
char    *x_malloc(unsigned int size);
char    *x_calloc(unsigned int size);
char    *alloc_vector(int,int,int);
char    **alloc_matrix(int,int,int,int,int,int);
#endif    /* end if _ANSI_ */

#endif	  /* end if _MMALLOC_INCLUDE_ */


/**************** End of Function Prototypes **********************/
