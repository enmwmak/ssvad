/**********************************************************************
 Header file for MSDOS VECLIB routines
 This file should be compiled with veclib.c
 Filename: veclib.h
 Date: 5/20/92
 Author: M.W.Mak
 Modified (30/3/99): define vec_t as float
***********************************************************************/
#ifndef  __VECLIB_INCLUDED__
#define  __VECLIB_INCLUDED__
#include "mmalloc.h"

#ifndef VBYTE
   typedef unsigned char VBYTE;
#endif

/* Define vector type */
#ifndef vec_t
  typedef VECTOR_TYPE vec_t;
#endif

/* Function Prototype for floating point version */
void VECznorm(int N, vec_t *vec, vec_t mean, vec_t stddev);
void VECcopyf(int length, vec_t *x, vec_t *y);
void VECmulalphaf(int length, vec_t alpha, vec_t *vec);
vec_t VECdotf(int length, vec_t *vec1,vec_t *vec2);
vec_t VECmaxf(int length,vec_t *vec);
vec_t VECmaxposf(int length, vec_t *vec, int *);
vec_t VECamaxposf(int length, vec_t *vec, int *pos);
vec_t VECminposf(int length, vec_t *vec, int *pos);
vec_t VECamaxf(int length, vec_t *vec);
void VECsubf(int length, vec_t *vec1,vec_t *vec2, vec_t *vec3);
vec_t VECmetricf(int length, vec_t *x, vec_t *y);
vec_t VECedistf(int length, vec_t *x, vec_t *y);
void VECmuladdf(int length, vec_t a, vec_t *x, vec_t *y);
void VECmtransposef(int numrows, int numcols, vec_t *A_t, vec_t *A);
void VECpostmultn(int numrows, int numcols, vec_t *v_t, vec_t *A, vec_t *u_t);
void VECmvmulf(int numrows, int numcols, vec_t *v, vec_t *A, vec_t *u);
void VECaddalphaf(int length, vec_t a, vec_t *vec);
void VECaddf(int length, vec_t *x, vec_t *y);
vec_t VECminf(int length, vec_t *x);
void VECfillf(int length, vec_t a, vec_t *x);
void VECsoftmaxf(int length, vec_t *x, vec_t *y);
vec_t VECmeanf(int n, vec_t *x);
vec_t VECstddevf(int n, vec_t *x);
void  VECstatf(int n, vec_t *x, vec_t *mean, vec_t *stddev);
vec_t  VECskewf(int n, vec_t *x);
vec_t VECL2normf(int length, vec_t *x);
vec_t VECsqedistf(int length, vec_t *x, vec_t *y);
vec_t VECsqmdistf(int length, vec_t *x, vec_t *y, vec_t *C);
void VECmatmultf(int M, int N, int P, vec_t **A, vec_t **B, vec_t **C);
void VECmataddf(int M, int N, vec_t **A, vec_t **B, vec_t **C);
void VECmatsubf(int M, int N, vec_t **A, vec_t **B, vec_t **C);
vec_t VECmattracef(int M, vec_t **A);
void VECpermutef(int N, vec_t *s);
void VECswapf(int N, vec_t *s);
vec_t VECsumf(int length, const vec_t *x);
vec_t VECasumf(int length, const vec_t *x);


/* Function Prototype for huge floating point version */
void  VECcopyfHuge(int length, vec_t huge *x, vec_t huge *y);
void  VECmulalphafHuge(int length, vec_t alpha, vec_t huge *vec);
vec_t VECdotfHuge(int length, vec_t huge *vec1,vec_t huge *vec2);
vec_t VECmaxfHuge(int length,vec_t huge *vec);
vec_t VECamaxfHuge(int length, vec_t huge *vec);
void  VECsubfHuge(int length, vec_t huge *vec1,vec_t huge *vec2, vec_t huge *vec3);
vec_t VECmetricfHuge(int length, vec_t huge *x, vec_t huge *y);
void  VECmuladdfHuge(int length, vec_t a, vec_t huge *x, vec_t huge *y);
void  VECmtransposefHuge(int numrows, int numcols, vec_t huge *A_t, vec_t huge *A);
void  VECpostmultnHuge(int numrows, int numcols, vec_t huge *v_t, vec_t huge *A, vec_t huge *u_t);
void  VECmvmulfHuge(int numrows, int numcols, vec_t huge *v, vec_t huge *A, vec_t huge *u);
void  VECaddalphafHuge(int length, vec_t a, vec_t huge *vec);
void  VECaddfHuge(int length, vec_t huge *x, vec_t huge *y);
void  VECfillfHuge(int n, vec_t a, vec_t huge *x);
void  VECprintmatrixfHuge(int rs, int rf, int cs, int cf, vec_t huge **x);
void  VECprintvectorfHuge(int n, vec_t huge *);
vec_t  VECminfHuge(int n, vec_t huge *);

/* Function prototypes for unsigned char version */
void VECcopyb(int n, VBYTE huge *x, VBYTE huge *y);
VBYTE VECmaxb(int n, VBYTE huge *x);
void VECasubb(int n, VBYTE huge *x, VBYTE huge *y);
VBYTE VECminb(int n, VBYTE huge *x);
void VECmulalphab(int n, VBYTE a, VBYTE huge *x);
void VECaddalphab(int n, VBYTE a, VBYTE huge *x);
void VECfillb(int n, VBYTE a, VBYTE huge *);

/* Function prototpypes for int version */
int  VECmaxi(int n, int huge *);
int  VECmini(int n, int huge *);
void VECfilli(int n, int a, int huge *x);
int  VECamaxiHuge(int length, int huge *vec);

/* Function prototypes for short int version */
short VECamaxShortHuge(int length, short huge *vec);
vec_t VECL2norms(int n, short *x);


#endif    /* end of veclib.h */
