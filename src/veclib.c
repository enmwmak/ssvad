/****************************************************************************
 MSDOS Vector Library V1.0
 Filename: veclib.c
 Date: 5/10/92
 Author: M.W.Mak
 Detail: This module do the same things as the HELIOS VECLIB interface
         module.  As MSDOS have no vector library, this module implements
         the actual computation
 Note: All index start from zero
**************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmalloc.h"
#include "veclib.h"
#ifdef _BORLANDC_
   #include <alloc.h>
#endif

/****************************************************************************/
/*  VECznorm: vec[i] = (vec[i]-mean)/stddev                                 */
/****************************************************************************/
void VECznorm(int N, vec_t *vec, vec_t mean, vec_t stddev)
{
    if (N <=0) return;
    int i;
    for (i=0; i<N; i++)
	vec[i] = (vec[i]-mean)/stddev;
}


/****************************************************************************/
/*  VECsubf: substract two vector, vec3[] = vec[1] - vec[2]                 */
/****************************************************************************/
void VECsubf(int length, vec_t *vec1, vec_t *vec2, vec_t *vec3)
{
     if (length <= 0) return;
     while (length--) {
        *vec3 = *vec1++ - *vec2++;
        vec3++;
     }
}

/****************************************************************************/
/*  VECdotf: return the dot product between two vector                      */
/****************************************************************************/
vec_t VECdotf(int length, vec_t *vec1, vec_t *vec2)
{
      vec_t sum=0.0;
      if (length <= 0) return(0.0);
      while (length--)
         sum += *vec1++ * *vec2++;
      return(sum);
}

/****************************************************************************/
/*  VECcopyf: copy vector y to vector x                                     */
/****************************************************************************/
void VECcopyf(int length, vec_t *x, vec_t *y)
{
     if (length <= 0) return;
     while (length--)
         *x++ = *y++;
}
     
/****************************************************************************/
/*  VECmaxf: return the max between elements of a vector                    */
/****************************************************************************/
vec_t VECmaxf(int length, vec_t *vec)
{
     vec_t max = -1e38;
     if (length <= 0) return(0);
     while (length--) {
        if (*vec > max)
           max = *vec;
        vec++;
     }
     return(max);
}

/****************************************************************************/
/*  VECmaxposf: return the max value in a vector and its position           */  
/****************************************************************************/
vec_t VECmaxposf(int length, vec_t *vec, int *pos)
{
     vec_t max = -1e38;
     int   j=0;          /* current position to vec[] */
     if (length <= 0) return(0);
     while (length--) {
        if (*vec > max) {
           max = *vec;
           *pos = j;
        } 
        vec++;
        j++;
     }
     return(max);
}

/****************************************************************************/
/*  VECamaxposf: return the abs max value in a vector and its position      */  
/****************************************************************************/
vec_t VECamaxposf(int length, vec_t *vec, int *pos)
{
     vec_t max = -1e38;
     int   j=0;          /* current position to vec[] */
     if (length <= 0) return(0);
     while (length--) {
        if (fabs(*vec) > max) {
           max = fabs(*vec);
           *pos = j;
        } 
        vec++;
        j++;
     }
     return(max);
}


/****************************************************************************/
/*  VECminposf: return the min value in a vector and its position           */  
/****************************************************************************/
vec_t VECminposf(int length, vec_t *vec, int *pos)
{
     vec_t min = 1e38;
     int   j=0;          /* current position to vec[] */
     if (length <= 0) return(0);
     while (length--) {
        if (*vec < min) {
           min = *vec;
           *pos = j;
        } 
        vec++;
        j++;
     }
     return(min);
}


/****************************************************************************/
/*  VECminf: return the min between elements of a vector                    */
/****************************************************************************/
vec_t VECminf(int length, vec_t *vec)
{
     vec_t min = 1e38;
     if (length <= 0) return(0);
     while (length--) {
	if (*vec<min)
	   min = *vec;
	vec++;
     }
     return(min);
}


/****************************************************************************/
/*  VECamaxf: return the abs max between elements of a vector                */
/****************************************************************************/
vec_t VECamaxf(int length, vec_t *vec)
{
     vec_t max = 0.0;
     vec_t temp;
     if (length <= 0) return(0);
     while (length--) {
        if ((temp=fabs(*vec++)) > max)
           max = temp;
     }
     return(max);
}

/****************************************************************************/
/*  VECmulaphaf: constant vector multiplicaton   xi=a * xi                   */
/****************************************************************************/
void VECmulalphaf(int length, vec_t a, vec_t *vec)
{
    if (length <= 0) return;
    while (length--) {
       *vec *= a;
       vec++;
    }
}

/****************************************************************************/
/*  VECmetricf: return the L1-norm product of vector x with vector y         */
/*             L1 = sum(|xi-yi|)					    */
/****************************************************************************/
vec_t VECmetricf(int length, vec_t *x, vec_t *y)
{
    vec_t sum=0.0;
    if (length <= 0) return(0);
    while (length--)
       sum += fabs(*x++ - *y++);
    return(sum);
}

/****************************************************************************/
/*  VECedistf: return the Eclidean distance between vector x with vector y  */
/*             L2 = sqrt(SUM((xi-yi)^2))                                    */
/****************************************************************************/
vec_t VECedistf(int length, vec_t *x, vec_t *y)
{
    vec_t sum=0.0;
    vec_t temp;
    if (length <= 0) return(0);
    while (length--) {
       temp = (*x++ - *y++);
       sum += temp*temp;
    }
    return(sqrt(sum));
}

/****************************************************************************/
/*  VECsqedistf: return the square of the Eclidean distance between vector x*/
/*             with vector y                                                */
/*             L2 = (SUM((xi-yi)^2))                                        */
/****************************************************************************/
vec_t VECsqedistf(int length, vec_t *x, vec_t *y)
{
    vec_t sum=0.0;
    vec_t temp;
    if (length <= 0) return(0);
    while (length--) {
       temp = (*x++ - *y++);
       sum += temp*temp;
    }
    return(sum);
}

/****************************************************************************/
/*  VECsqmdistf: return the square of the Mahalinobis distance between      */
/*               vectors x and y, given the inverse of covariance matrix C  */
/*             with vector y                                                */
/*             MD = (x-y)'C(x-y)                                            */
/****************************************************************************/
vec_t VECsqmdistf(int length, vec_t *x, vec_t *y, vec_t *C)
{
      vec_t  mdist;			/* Mahalanobis distance */
      vec_t  *v,*d;                     /* temp storage */

      v = (vec_t *)vector(0,length-1,sizeof(vec_t));
      d = (vec_t *)vector(0,length-1,sizeof(vec_t));

      /* Find d = x - y */
      VECsubf(length,x,y,d);

      /* Find v = C . d */ 
      VECmvmulf(length,length,v,C,d);

      /* Find (x-y).v = (x-y)'C(x-y) */
      mdist=VECdotf(length,d,v);

      free_vector((char *)v,0,sizeof(vec_t));
      free_vector((char *)d,0,sizeof(vec_t));
      return(mdist);
}


/****************************************************************************/
/*  VECmatmultf: Matrix multiplication (Index start from 1)                 */
/*               C = AB                                                     */
/*               A is an MxN matrix                                         */
/*               B is an NxP matrix                                         */
/*               C is an MxP matrix                                         */
/*                          N                                               */
/*               C[i][j] = sum (a_ik * b_kj)      i=1..M; j=1..P            */
/*                         k=1                                              */
/****************************************************************************/
void VECmatmultf(int M, int N, int P, vec_t **A, vec_t **B, vec_t **C)
{
  int i,j,k;
  vec_t s;

  for (i=1; i<=M; i++) {
      for (j=1; j<=P; j++) {
          s=0.0;
	  for (k=1; k<=N; k++) {
	      s += A[i][k]*B[k][j];
	  }
	  C[i][j]=s;
      }
  }
}

/****************************************************************************/
/*  VECmataddf: Matrix addition (Index start from 1)                        */
/*               C = A+B                                                    */
/*               A is an MxN matrix                                         */
/*               B is an MxN matrix                                         */
/*               C is an MxN matrix                                         */
/*               C[i][j] = a[i][j] + b[i][j]      i=1..M; j=1..N            */
/****************************************************************************/
void VECmataddf(int M, int N, vec_t **A, vec_t **B, vec_t **C)
{
  int i,j;

  for (i=1; i<=M; i++) {
      for (j=1; j<=N; j++) {
	  C[i][j] = A[i][j] + B[i][j];
      }
  }
}

/****************************************************************************/
/*  VECmatsubf: Matrix subtraction (Index start from 1)                     */
/*               C = A-B                                                    */
/*               A is an MxN matrix                                         */
/*               B is an MxN matrix                                         */
/*               C is an MxN matrix                                         */
/*               C[i][j] = a[i][j] - b[i][j]      i=1..M; j=1..P            */
/****************************************************************************/
void VECmatsubf(int M, int N, vec_t **A, vec_t **B, vec_t **C)
{
  int i,j;

  for (i=1; i<=M; i++) {
      for (j=1; j<=N; j++) {
	  C[i][j] = A[i][j] - B[i][j];
      }
  }
}

/****************************************************************************/
/*  VECmattracef: Trace of a matrix subtraction (Index start from 1)        */
/*                tr(A)                                                     */
/*               A is an MxM matrix                                         */
/*               tr(A) = sum_ii A[i][i], i = 1,..,M                         */
/****************************************************************************/
vec_t VECmattracef(int M, vec_t **A)
{
  int i;
  vec_t tr;

  tr = 0.0;
  for (i=1; i<=M; i++) {
      tr += A[i][i];
  }
  return(tr);
}



/****************************************************************************/
/*  VECmuladdf: multiply a vector by a const and add the result to another  */
/*              vector    xi=xi+a*yi                                        */
/****************************************************************************/
void VECmuladdf(int length, vec_t a, vec_t *x, vec_t *y)
{
    if (length <= 0) return;
    while (length--) {
       *x += a * *y++;
       x++;
    }
}

/****************************************************************************/
/*  VECmtransposef: matrix transpose function    			    */
/*                 xij := yji                                               */
/****************************************************************************/
void VECmtransposef(int numrows, int numcols, vec_t *x, vec_t *y)
{
    int  i,j,temp;
    for (i=0;i<numrows;i++) {
       temp = i*numrows;
       for (j=0;j<numcols;j++)
          x[j*numrows+i] = y[temp+j];
    }
}


/****************************************************************************/
/*  VECpostmultn: postmultiplies the row vector u_t by the matrix A to      */
/*  produce the row vector v_t                                              */
/*                       v_t = u_t * A                                      */
/*  Since helios VECLIB only got pre-multiplies routines, we use            */
/*                       v = A_t * u                                        */
/*  where v_t is the transpose of v                                         */
/****************************************************************************/
void VECpostmultn(int nrows, int ncols, vec_t *v_t, vec_t *A, vec_t *u_t)
{
     vec_t *A_t;

     if ((A_t=(vec_t *)malloc(nrows*ncols*sizeof(vec_t)))==NULL) {
     	printf("\nInsufficient memory in VECpostmultn");
     	exit(EXIT_FAILURE);
     }
     /* make A_t from A */
     VECmtransposef(nrows,ncols,A_t,A);
     VECmvmulf(ncols,nrows,v_t,A_t,u_t);     /* nrows & ncols reverse after transpose */
     free(A_t);
}

/****************************************************************************/
/* VECmvmulf: pre-multiplies a vector u by a matrix A to give the result  */
/*              vector v						    */
/*                            v = A*u					    */
/*                            vi = sum(Aij * uj)                             */
/*                                  j                                       */
/*            A[] contains consecutive rows vectors of numcols elements     */
/****************************************************************************/
void VECmvmulf(int numrows, int numcols, vec_t *v, vec_t *A, vec_t *u)
{
     int i,j,temp;
     for (i=0;i<numrows;i++) {
        temp = i*numcols;
        v[i] = 0.0;
        for (j=0;j<numcols;j++)
            v[i] += (A[temp+j] * u[j]);
     }
}

/****************************************************************************/
/* VECaddalphaf: add a constant to every element of a vector                 */
/*                            xi = a + xi				    */
/****************************************************************************/
void VECaddalphaf(int length, vec_t a, vec_t *x)
{
     if (length <= 0) return;
     while (length--) {
        *x += a;
        x++;
     }
}

/****************************************************************************/
/* VECaddf: Add two vector element by element and store in the first vector */
/*                            xi :=  xi + yi				    */
/****************************************************************************/
void VECaddf(int length, vec_t *x, vec_t *y)
{
     if (length <= 0) return;
     while (length--) {
        *x += *y++;
        x++;
     }
}

/********************************************************************/
/* VECfillf: xi := a	                                            */
/********************************************************************/
void VECfillf(int n, vec_t a, vec_t *x)
{
     if (n<=0) return;
     while (n--)
	*x++ = a;
}

/********************************************************************/
/* VECsoftmaxf: yi := Softmax(xi)                                   */
/*                         exp(xi)                                  */
/*              yi := -----------------------                       */
/*                     sum {exp(xj)}                                */
/*                      j                                           */
/********************************************************************/
#include <math.h>
void VECsoftmaxf(int n, vec_t *x, vec_t *y)
{
     int i,j;
     vec_t sum;
     
     if (n<=0) return;
     for (sum=0.0,j=0; j<n; j++)
          sum += exp(x[j]);
     for (i=0; i<n; i++)
          y[i] = exp(x[i]) / sum;
}

/********************************************************************
    VECmeanf: return the mean of vector x
*********************************************************************/
vec_t VECmeanf(int n, vec_t *x)
{
     int i;
     vec_t sum;
     if (n<=0) return 0.0;
     for (sum=0.0,i=0; i<n; i++)
         sum += x[i];
     return(sum/n);
}

/********************************************************************
    VECstddevf: return the standard deviation of vector x
*********************************************************************/
vec_t VECstddevf(int n, vec_t *x)
{
      int i;
      vec_t mean,sum,temp;
      if (n<=0) return 0.0;
      mean = VECmeanf(n,x);
      for (sum=0.0,i=0; i<n; i++) {
	  temp = x[i] - mean;
	  sum += temp*temp;
      }
      return(sqrt(sum/n));
}

/********************************************************************
    VECstatf: return mean & standard deviation of vector x
*********************************************************************/
void  VECstatf(int n, vec_t *x, vec_t *mean, vec_t *stddev)
{
      int i;
      vec_t sum,temp;
      if (n<=0) return;
      *mean = VECmeanf(n,x);
      for (sum=0.0,i=0; i<n; i++) {
	  temp = x[i] - *mean;
	  sum += temp*temp;
      }
      *stddev = sqrt(sum/n);
}

/********************************************************************
    VECskewf: return the skewness of an array
*********************************************************************/
vec_t  VECskewf(int n, vec_t *x)
{
      int i;
      vec_t sum,temp;
      vec_t mean,std;

      if (n<=0) return 0;
      VECstatf(n, x, &mean, &std);
      for (sum=0.0,i=0; i<n; i++) {
	  temp = (x[i] - mean)/std;
	  sum += temp*temp*temp;
      }
      return(sum/n);
}



/****************************************************************************/
/*  VECL2normf: return the L2-norm product of vector x                      */
/*             L1 = sum(xi)^2					            */
/****************************************************************************/
vec_t VECL2normf(int n, vec_t *x)
{
    vec_t sum=0.0;
    int   i;   
    if (n <= 0) return(0);
    for (i=0; i<n; i++)
        sum += x[i]*x[i];
    return(sum);
}

/****************************************************************************/
/*  VECpermutef: return the permuted vector of s[0..N-1]                    */
/****************************************************************************/
void VECpermutef(int N, vec_t *s)
{
    int *p;
    int i,k,index,repeat;
    vec_t *temp;
   
    p = (int *)vector(0,N-1,sizeof(int));
    temp = (vec_t *)vector(0,N-1,sizeof(vec_t));

    VECcopyf(N, temp, s);
    srand((int)time((time_t *)NULL));
    for (i=0;i<N;i++) {
	do {
	    repeat = 0;		/* hasn't repeat for the first time */
            index = (int)((float)rand()*(float)N/(float)RAND_MAX);
	    p[i] = index;
	    for (k=0;k<i;k++) {
		if (p[k]==index) {
		    repeat = 1;
		    break;
		}
	    }
	} while (repeat == 1);
    }
     
    for (i=0;i<N;i++)         /* for each center */
        s[i] = temp[p[i]];

    free_vector((char *)p,0,sizeof(int));
    free_vector((char *)temp,0,sizeof(vec_t));
}

/****************************************************************************/
/*  VECswapf: swap the elements of s[0..N-1]                                */
/*            s[i] = s[N-i]                                                 */ 
/****************************************************************************/
void VECswapf(int N, vec_t *s)
{
    int i;
    vec_t *temp;
   
    temp = (vec_t *)vector(0,N-1,sizeof(vec_t));
    VECcopyf(N, temp, s);
    for (i=0; i<N ;i++) {
	s[i] = temp[N-i-1];
    }
    free_vector((char *)temp,0,sizeof(vec_t));
}



/*============ End of vec_t point version of Veclib for MSDOS ==============*/

/*============ Start of Huge floating point version ========================*/
/****************************************************************************/
/*  VECsubfHuge: substract two vector, vec3[] = vec[1] - vec[2]             */
/****************************************************************************/
void VECsubfHuge(int length, vec_t huge *vec1, vec_t huge *vec2, vec_t huge *vec3)
{
     if (length <= 0) return;
     while (length--) {
        *vec3 = *vec1++ - *vec2++;
        vec3++;
     }
}

/****************************************************************************/
/*  VECdotfHuge: return the dot product between two vector                  */
/****************************************************************************/
vec_t VECdotfHuge(int length, vec_t huge *vec1, vec_t huge *vec2)
{
      vec_t sum=0.0;
      if (length <= 0) return(0.0);
      while (length--)
         sum += *vec1++ * *vec2++;
      return(sum);
}

/****************************************************************************/
/*  VECcopyfHuge: copy vector y to vector x                                 */
/****************************************************************************/
void VECcopyfHuge(int length, vec_t huge *x, vec_t huge *y)
{
     if (length <= 0) return;
     while (length--)
	 *x++ = *y++;
}
     
/****************************************************************************/
/*  VECmaxfHuge: return the max between elements of a vector                */
/****************************************************************************/
vec_t VECmaxfHuge(int length, vec_t huge *vec)
{
     vec_t max = -1e38;
     if (length <= 0) return(0);
     while (length--) {
        if (*vec > max)
           max = *vec;
        vec++;
     }
     return(max);
}

/****************************************************************************/
/*  VECminfHuge: return the min between elements of a vector                */
/****************************************************************************/
vec_t VECminfHuge(int length, vec_t huge *vec)
{
     vec_t min = 1e38;
     if (length <= 0) return(0);
     while (length--) {
	if (*vec < min)
	   min = *vec;
	vec++;
     }
     return(min);
}


/****************************************************************************/
/*  VECamaxfHuge: return the abs max between elements of a vector           */
/****************************************************************************/
vec_t VECamaxfHuge(int length, vec_t huge *vec)
{
     vec_t max = 0.0;
     vec_t temp;
     if (length <= 0) return(0);
     while (length--) {
        if ((temp=fabs(*vec++)) > max)
	   max = temp;
     }
     return(max);
}


/****************************************************************************/
/*  VECmulaphafHuge: constant vector multiplicaton   xi=a * xi              */
/****************************************************************************/
void VECmulalphafHuge(int length, vec_t a, vec_t huge *vec)
{
    if (length <= 0) return;
    while (length--) {
       *vec *= a;
       vec++;
    }
}

/****************************************************************************/
/*  VECmetricfHuge: return the L1-norm product of vector x with vector y    */
/*             L1 = sum(|xi-yi|)					    */
/****************************************************************************/
vec_t VECmetricfHuge(int length, vec_t huge *x, vec_t huge *y)
{
    vec_t sum=0.0;
    if (length <= 0) return(0);
    while (length--)
       sum += fabs(*x++ - *y++);
    return(sum);
}

/****************************************************************************/
/*  VECmuladdfHuge: multiply a vector by a const and add the result to another*/
/*              vector    xi=xi+a*yi                                        */
/****************************************************************************/
void VECmuladdfHuge(int length, vec_t a, vec_t huge *x, vec_t huge *y)
{
    if (length <= 0) return;
    while (length--) {
       *x += a * *y++;
       x++;
    }
}

/****************************************************************************/
/*  VECmtransposefHuge: matrix transpose function                           */
/*                 xij := yji                                               */
/****************************************************************************/
void VECmtransposefHuge(int numrows, int numcols, vec_t huge *x, vec_t huge *y)
{
    int  i,j,temp;
    for (i=0;i<numrows;i++) {
       temp = i*numrows;
       for (j=0;j<numcols;j++)
          x[j*numrows+i] = y[temp+j];
    }
}


/****************************************************************************/
/*  VECpostmultnHuge: postmultiplies the row vector u_t by the matrix A to  */
/*  produce the row vector v_t                                              */
/*                       v_t = u_t * A                                      */
/*  Since helios VECLIB only got pre-multiplies routines, we use            */
/*                       v = A_t * u                                        */
/*  where v_t is the transpose of v                                         */
/****************************************************************************/
void VECpostmultnHuge(int nrows, int ncols, vec_t huge *v_t, vec_t huge *A, vec_t huge *u_t)
{
     vec_t huge *A_t;

     if ((A_t=(vec_t huge *)farmalloc(nrows*ncols*sizeof(vec_t)))==NULL) {
     	printf("\nInsufficient memory in VECpostmultn");
     	exit(1);
     }
     /* make A_t from A */
     VECmtransposefHuge(nrows,ncols,A_t,A);
     VECmvmulfHuge(ncols,nrows,v_t,A_t,u_t);     /* nrows & ncols reverse after transpose */
     farfree((char far *)A_t);
#ifdef _BORLANDC_
     if (farheapcheck()==_HEAPCORRUPT || heapcheck()==_HEAPCORRUPT) {
	printf("\nFar heap is corrupted");
	exit(1);
     }
#endif
}

/****************************************************************************/
/* VECmvmulfHuge: pre-multiplies a vector u by a matrix A to give the result*/
/*              vector v						    */
/*                            v = A*u					    */
/*                            vi = sum(Aij * uj                             */
/*                                  i                                       */
/****************************************************************************/
void VECmvmulfHuge(int numrows, int numcols, vec_t huge *v, vec_t huge *A, vec_t huge *u)
{
     int i,j,temp;
     for (i=0;i<numrows;i++) {
        temp = i*numcols;
        v[i] = 0.0;
	for (j=0;j<numcols;j++)
            v[i] += (A[temp+j] * u[j]);
     }
}

/****************************************************************************/
/* VECaddalphafHuge: add a constant to every element of a vector            */
/*                            xi = a + xi				    */
/****************************************************************************/
void VECaddalphafHuge(int length, vec_t a, vec_t huge *x)
{
     if (length <= 0) return;
     while (length--) {
        *x += a;
        x++;
     }
}

/****************************************************************************/
/* VECaddfHuge: Add two vector element by element and store in the first vector */
/*                            xi :=  xi + yi				    */
/****************************************************************************/
void VECaddfHuge(int length, vec_t huge *x, vec_t huge *y)
{
     if (length <= 0) return;
     while (length--) {
        *x += *y++;
        x++;
     }
}

/********************************************************************/
/* VECfillfHuge: xi := a                                            */
/********************************************************************/
void VECfillfHuge(int n, vec_t a, vec_t huge *x)
{
     if (n<=0) return;
     while (n--)
	*x++ = a;
}

/********************************************************************/
/* VECprintmatrixfHuge                                              */
/* print a matrix start from x[rs][cs] to x[cf][cf] that is allocated */
/* by matrix(rs,rf,cs,cf,...) in mmalloc.c				*/
/********************************************************************/
void VECprintmatrixfHuge(int rs, int rf, int cs, int cf, vec_t huge **x)
{
     int i;
     if (cf<cs) return;
     for (i=rs;i<=rf;i++) {
	 printf("\n");
	 VECprintvectorfHuge(cf-cs+1, &x[i][cs]);
     }
     printf("\n");
}

/********************************************************************/
/* VECprintvectorfHuge: print n element of vector x[] (start from x[0] */
/********************************************************************/
void VECprintvectorfHuge(int n, vec_t huge *x)
{
     int j;
     printf(": ");
     for (j=0;j<n;j++)
	 printf("%.3f ",x[j]);
}

/*================== End of huge vec_t point version =======================*/

/*================== Start of Byte version, default is huge ================*/

/****************************************************************************/
/* VECcopyb: copy BYTE vector                                               */
/*           xi := yi                                                       */
/****************************************************************************/
void VECcopyb(int n, VBYTE huge *x, VBYTE huge *y)
{
     if (n <=0) return;
     while (n--)
         *x++ = *y++;
}

/********************************************************************/
/* VECmaxb: VBYTE version of max of vector                           */
/********************************************************************/
VBYTE VECmaxb(int n, VBYTE huge *x)
{
     VBYTE max = 0;
     if (n <= 0) return(0);
     while (n--) {
        if (*x > max)
           max = *x;
        x++;
     }
     return(max);
}

/********************************************************************/
/* VECasubb: VBYTE version of vector absolute substraction            */
/*          xi:=|xi-yi|                                             */
/********************************************************************/
void VECasubb(int n, VBYTE huge *x, VBYTE huge *y)
{
     if (n<=0) return;
     while (n--) {
        *x = (VBYTE)abs((int)(*x - *y++));
        x++;
     }
}

/********************************************************************/
/* VECminb: VBYTE version of min of vector                           */
/********************************************************************/
VBYTE VECminb(int n, VBYTE huge *x)
{
     VBYTE min = 255;
     if (n <= 0) return(255);
     while (n--) {
        if (*x < min)
           min = *x;
        x++;
     }
     return(min);
}

/********************************************************************/
/* VECmulalphab: VBYTE version of multiplied a vector by a constant  */
/*               xi := xi * a                                       */
/********************************************************************/
void VECmulalphab(int n, VBYTE a, VBYTE huge *x)
{
    if (n <= 0) return;
    while (n--) {
       *x *= a ;
       x++;
    }
}

/********************************************************************/
/* VECaddalphab:  xi := a + xi                                      */
/********************************************************************/
void VECaddalphab(int n, VBYTE a, VBYTE huge *x)
{
     if (n<=0) return;
     while (n--) {
        *x += a;
        x++;
     }
}

/********************************************************************/
/* VECfillb: xi := a                                                */
/********************************************************************/
void VECfillb(int n, VBYTE a, VBYTE huge *x)
{
     if (n<=0) return;
     while (n--)
	*x++ = a;
}

/********************************************************************/
/* VECmaxi: int version of max of vector                           */
/********************************************************************/
int  VECmaxi(int n, int huge *x)
{
     int max = 0;
     if (n <= 0) return(0);
     while (n--) {
	if (*x > max)
	   max = *x;
	x++;
     }
     return(max);
}


/********************************************************************/
/* VECmini: int version of min of vector                           */
/********************************************************************/
int  VECmini(int n, int huge *x)
{
     int min = 255;
     if (n <= 0) return(255);
     while (n--) {
	if (*x < min)
	   min = *x;
	x++;
     }
     return(min);
}

/****************************************************************************/
/*  VECamaxiHuge: return the abs max between elements of an int vector      */
/****************************************************************************/
int  VECamaxiHuge(int length, int huge *vec)
{
     int max = 0;
     int temp;
     if (length <= 0) return(0);
     while (length--) {
	if ((temp=abs(*vec++)) > max)
	   max = temp;
     }
     return(max);
}


/********************************************************************/
/* VECfilli: xi := a                                                */
/********************************************************************/
void VECfilli(int n, int a, int huge *x)
{
     if (n<=0) return;
     while (n--)
	*x++ = a;
}


/****************************************************************************/
/*  VECamaxShortHuge: return the abs max between elements of a short int vector      */
/****************************************************************************/
short VECamaxShortHuge(int length, short huge *vec)
{
     short max = 0;
     short temp;
     if (length <= 0) return(0);
     while (length--) { 
	if ((temp=abs(*vec++)) > max)
	   max = temp;
     }
     return(max);
}


/****************************************************************************/
/*  VECsumf:        Sum all elements of a vector			                */
/****************************************************************************/
vec_t VECsumf(int length, const vec_t *x)
{
    vec_t sum=0.0;
    if (length <= 0) return 0;
    while (length--) {
        sum += *x;
        x++;
    }
    return sum;
}

/****************************************************************************/
/*  VECasumf:        Sum all absolute values of elements of a vector			                */
/****************************************************************************/
vec_t VECasumf(int length, const vec_t *x)
{
    vec_t sum=0.0;
    if (length <= 0) return 0;
    while (length--) {
        sum += fabs(*x);
        x++;
    }
    return sum;
}


/*================== Start of short int version ================*/

/****************************************************************************/
/*  VECL2norms: return the L2-norm product of vector x                      */
/*               L1 = sum(xi)^2					            */
/****************************************************************************/
vec_t VECL2norms(int n, short *x)
{
    vec_t sum=0.0;
    int   i;   
    if (n <= 0) return(0.0);
    for (i=0; i<n; i++)
        sum += (vec_t)x[i]*(vec_t)x[i];
    return(sum);
}



/*
main()
{
    vec_t A[3][2] = {3.0,2.0,
		    1.0,-5.0,
		    -1.0,2.0};
    vec_t B[2][3] = {1.0,2.0,3.0,
                     4.0,5.0,6.0};
    vec_t C[3][3];
    int i,j;

    VECmatmultf(3,2,3,A,B,C);
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++)
	printf("%5f",C[i][j]);
      printf("\n");
    }
}
*/

/*
main()
{
   static VBYTE x[3] = {2,40,10};
   static VBYTE y[3] = {7,22,230};
   VECasubb(3,x,y);
}
*/

