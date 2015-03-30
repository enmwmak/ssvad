/**********************************************************************
 MEMORY ALLOCATION LIBRARY :
    This library contains routines which allocate and free vector or
    array of any data object.  The matrix or array size can be greater
    than 64Kbyte provided that huge pointer is used and compile under
    compact memory model.
    The library is designed to run on either MSDOS/Helios/Unix OS.
    if MSDOS Brolandc is used, a statement #define _BORLANDC_ should
    be added before the statement #include "malloc.h".
 Filename : mmalloc.c
 Memory model : compact
 Author : M.W.Mak
 Date   : 15/2/92
 Version : V2.0
 Modified(2/11/92): Bug fix in alloc_farmatrix, row should be int rather
                    than unsigned long so that matrix can start and
                    end with negative index
 Modified(22/11/92): Heap check in free_vector()
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mmalloc.h"

#ifdef   _BORLANDC_
#include <alloc.h> 			/* Borlandc support far data */
#endif

/**************** Start of Library Routines ***********************/

/*****************************************************************
 This is the entry point of allocating matrix. Depending on the
 OS type "HELIOS or MSDOS", appropriate memory allocating routine
 will be called.  If MSDOS, far data will be used
*******************************************************************/

char huge **matrix(row_start,row_fsh,col_start,col_fsh,obj_size,ptr_size)
int row_start;
int row_fsh;
int col_start;
int col_fsh;
int obj_size; 
int ptr_size;
{
#ifdef  _BORLANDC_
	return(alloc_farmatrix(row_start,row_fsh,col_start,col_fsh,
			       obj_size, ptr_size));
#else
	return((char **)alloc_matrix(row_start,row_fsh,col_start,col_fsh,
			    obj_size, ptr_size));
#endif
}


/*****************************************************************
 This is the entry point of allocating vector. Depending on the
 OS type "HELIOS or MSDOS", appropriate memory allocating routine
 will be called. If MSDOS, far data will be used
*******************************************************************/

char huge *vector(start_index,end_index,obj_size)
long start_index,end_index,obj_size;
{
#ifdef _BORLANDC_
	return(alloc_farvector(start_index,end_index,obj_size));
#else
	return((char *)alloc_vector((int)start_index,(int)end_index,
					(int)obj_size));
#endif
}


/************************************************************/
/*	FREE_MATRIX					    */
/*  free memory allocated by matrix()			    */
/************************************************************/
void  free_matrix(matrix, rs, cs, obj_size)
char  huge **matrix;
int   rs,cs,obj_size;
{
#ifdef _BORLANDC_
      free_farmatrix(matrix,rs,cs,obj_size);
#else
      free((char huge *) (&matrix[rs][cs*obj_size]));
      free((char huge *) (&matrix[rs]));
#endif
}

/***************************************************************
 Entry point for FREE_VECTOR
****************************************************************/
void free_vector(v,nl,obj_size)
char huge *v;
long nl,obj_size;
{
#ifdef  _BORLANDC_
	free_farvector(v,nl,obj_size);
	if (farheapcheck()==_HEAPCORRUPT || heapcheck()==_HEAPCORRUPT)
	    put_error("\nHeap is corrupted");
#else
	free((char*) (v+nl*obj_size));
#endif
}



#ifdef _BORLANDC_

char huge *alloc_farvector(nl,nh,obj_size)
long nl,nh,obj_size;
{
	char huge *v;
	v=(char huge *)x_farcalloc((unsigned long)(nh-nl+1)*sizeof(char)
				   * obj_size);
	return ((char huge *)v-nl*obj_size);
}

void free_farvector(v,nl,obj_size)
char huge *v;
long nl,obj_size;
{
	farfree((char far *) (v+nl*obj_size));
}

/*******************************************************/
/*	X_FARMALLOC				       */
/*******************************************************/
char huge *x_farmalloc(unsigned long size)
{
      char huge *p;
      p = (char huge *)farmalloc((unsigned long)size);
      if (p==(char huge *)NULL) {
	  printf("\nInsufficient memory in x_farmalloc");
	  exit(EXIT_FAILURE);
      }
      return(p);
}

/*******************************************************/
/*	X_FARCALLOC				       */
/*******************************************************/
char  huge  *x_farcalloc(unsigned long size)
{
      char huge *p;
      p = (char huge *)farcalloc((unsigned long)size,sizeof(char));
      if (p==(char huge *)NULL) {
	  printf("\nInsufficient memory to allocate %ld Bytes in x_farcalloc",size);
	  exit(1);
      }
      return(p);
}

/****************************************************************/
/*  This function allocates storage for a matrix of dimensions  */
/*  [row_start..row_finish][col_start..col_finish] stored as a  */
/*  pointer to a pointer.  The rows of the matrix are stored    */
/*  contiguously in memory so that matrix[row_start] is a       */
/*  pointer to the start of the first row of the matrix, and to */
/*  successive rows stored from start to finish.                */
/*  Modified : this version support all data type, including    */
/*	       structure define by user				*/
/*  Date : 5/2/92 by M.W.Mak					*/
/*  Example :							*/
/*  mat = (IMAGE **)alloc_matrix(1,10,1,10,sizeof(IMAGE),sizeof(IMAGE *) */
/*  where IMAGE is structure type defined by user, the function	*/
/*  return 2-D array of IMAGE type				*/
/*  The size of matrix can be greater than 64K			*/
/****************************************************************/
char huge **alloc_farmatrix(int row_start,
			    int row_finish,
			    int col_start,
			    int col_finish,
			    int obj_size,		/* object size */
			    int obj_psize)		/* size of pointer to object */
{
  char huge **matrix;
  unsigned long row_size,col_size,mat_size;
  int      row;                                 /* bug fix, use int */

  col_size=(row_finish-row_start+1);
  row_size=(col_finish-col_start+1);
  mat_size=row_size*col_size;

  matrix=(char huge **)farcalloc((unsigned long)(col_size * obj_psize),sizeof(char));
  if (!matrix)
      put_error("\nError 1 in alloc_matrix : Insufficient memory");
  matrix=matrix-row_start;
  matrix[row_start]=(char huge *)farcalloc((unsigned long)(mat_size * obj_size),sizeof(char));
  if (!matrix[row_start])
      put_error("\nError 2 in alloc_matrix : Insufficient memory");
  matrix[row_start]=matrix[row_start]-col_start * obj_size;
  for (row=row_start+1;row<=row_finish;row++)
      matrix[row]=matrix[row-1]+row_size * obj_size;
  return((char huge **) matrix);
}


/************************************************************/
/*	FREE_FARMATRIX					    */
/*  free memory allocated by alloc_farmatrix()		    */
/************************************************************/
void  free_farmatrix(matrix, rs, cs, obj_size)
char  huge **matrix;
int   rs,cs,obj_size;
{
      farfree((char far *) (&matrix[rs][cs*obj_size]));
      farfree((char far *) (&matrix[rs]));
}

#endif			/* end of Borland C far data */

char   *alloc_vector(nl,nh,obj_size)
int nl,nh,obj_size;
{
	char *v;

	v=(char *)x_calloc((unsigned) (nh-nl+1)*sizeof(char) * obj_size);
	return (v-nl*obj_size);
}

/*******************************************************/
/* Function : put_error				       */
/* Purpose  : display error message on stderr device   */
/* Date     : 17/11/91				       */
/*******************************************************/
void    put_error(err_msg)
char *err_msg;
{
    fprintf(stderr,"%s",err_msg);
    exit(1);
}

/*******************************************************/
/*	X_MALLOC				       */
/*******************************************************/
char    *x_malloc(size)
unsigned int size;
{
      char  *p;
      p = (char *)malloc((unsigned)size);
      if (p==(char *)NULL) {
	  printf("\nInsufficient memory in x_malloc");
	  exit(1);
      }
      return(p);
}



/*******************************************************/
/*	X_CALLOC				       */
/*******************************************************/
char    *x_calloc(size)
unsigned int size;
{
      char  *p;
      p = (char *)calloc((size_t)size,sizeof(char));
      if (p==(char *)NULL) {
	  printf("Insufficient memory in x_calloc\n");
	  exit(1);
      }
      return(p);
}


/****************************************************************/
/*  This function allocates storage for a matrix of dimensions  */
/*  [row_start..row_finish][col_start..col_finish] stored as a  */
/*  pointer to a pointer.  The rows of the matrix are stored    */
/*  contiguously in memory so that matrix[row_start] is a       */
/*  pointer to the start of the first row of the matrix, and to */
/*  successive rows stored from start to finish.                */
/****************************************************************/
float **fmatrix(row_start,row_finish,col_start,col_finish)
int row_start,row_finish,col_start,col_finish;
{
  float **matrix;
  int row,row_size,col_size,mat_size;

  col_size=(row_finish-row_start+1);
  row_size=(col_finish-col_start+1);
  mat_size=row_size*col_size;

  matrix=(float **)x_calloc(col_size * sizeof(float *));
  matrix=matrix-row_start;
  matrix[row_start]=(float *)x_calloc(mat_size * sizeof(float));
  matrix[row_start]=matrix[row_start]-col_start;
  for (row=row_start+1;row<=row_finish;row++)
      matrix[row]=matrix[row-1]+row_size;
  return(matrix);
}



/****************************************************************/
/*  This function allocates storage for a matrix of dimensions  */
/*  [row_start..row_finish][col_start..col_finish] stored as a  */
/*  pointer to a pointer.  The rows of the matrix are stored    */
/*  contiguously in memory so that matrix[row_start] is a       */
/*  pointer to the start of the first row of the matrix, and to */
/*  successive rows stored from start to finish.                */
/*  Modified : this version support all data type, including    */
/*	       structure define by user				*/
/*  Date : 5/2/92 by M.W.Mak					*/
/*  Example :							*/
/*  mat = (IMAGE **)alloc_matrix(1,10,1,10,sizeof(IMAGE),sizeof(IMAGE *) */
/*  where IMAGE is structure type defined by user, the function	*/
/*  return 2-D array of IMAGE type				*/
/****************************************************************/
char **alloc_matrix(row_start,
		    row_finish,
		    col_start,
		    col_finish,
		    obj_size,		/* object size */
		    obj_psize)		/* size of pointer to object */
int row_start,row_finish,col_start,col_finish,obj_size,obj_psize;
{
  char **matrix;
  int row,row_size,col_size,mat_size;

  col_size=(row_finish-row_start+1);
  row_size=(col_finish-col_start+1);
  mat_size=row_size*col_size;

  matrix=(char **)x_calloc((unsigned)col_size * obj_psize);
  matrix=matrix-row_start;
  matrix[row_start]=(char *)x_calloc((unsigned)mat_size * obj_size);
  matrix[row_start]=matrix[row_start]-col_start * obj_size;
  for (row=row_start+1;row<=row_finish;row++)
      matrix[row]=matrix[row-1]+row_size * obj_size;
  return(matrix);
}




void nrerror(error_text)
char error_text[];
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}


float huge **submatrix(a,oldrl,oldrh,oldcl,oldch,newrl,newcl)
float huge **a;
int oldrl,oldrh,oldcl,oldch,newrl,newcl;
{
	int i,j;
	float huge **m;
#ifdef  _BORLANDC_
	m=(float huge **)farmalloc((unsigned long) (oldrh-oldrl+1)*sizeof(float*));
#else
	m=(float **)malloc((unsigned) (oldrh-oldrl+1)*sizeof(float*));
#endif
	if (!m) nrerror("allocation failure in submatrix()");
	m -= newrl;

	for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+oldcl-newcl;

	return m;
}


void free_submatrix(b,nrl,nrh,ncl,nch)
float huge **b;
int nrl,nrh,ncl,nch;
{
#ifdef  _BORLANDC_
	farfree((char far *) (b+nrl));
#else
	free((char*) (b+nrl));
#endif
}



float huge **convert_matrix(a,nrl,nrh,ncl,nch)
float huge *a;
int nrl,nrh,ncl,nch;
{
	int i,j,nrow,ncol;
	float huge **m;

	nrow=nrh-nrl+1;
	ncol=nch-ncl+1;
#ifdef  _BORLANDC_
	m = (float huge **)farmalloc((unsigned long) (nrow)*sizeof(float*));
#else
	m = (float **) malloc((unsigned) (nrow)*sizeof(float*));
#endif
	if (!m) nrerror("allocation failure in convert_matrix()");
	m -= nrl;
	for(i=0,j=nrl;i<=nrow-1;i++,j++) m[j]=a+ncol*i-ncl;
	return m;
}



void free_convert_matrix(b,nrl,nrh,ncl,nch)
float huge **b;
int nrl,nrh,ncl,nch;
{
#ifdef  _BORLANDC_
	farfree((char far *) (b+nrl));
#else
	free((char*) (b+nrl));
#endif
}








/* #define _MATRIXTEST_ */
#ifdef  _MATRIXTEST_
void main(int argc, char *argv[])
{
	int rs = atoi(argv[1]);
	int rf = atoi(argv[2]);
	int cs = atoi(argv[3]);
	int cf = atoi(argv[4]);
	double  huge **mat;
	double  sum;
	int i,j;
	if (argc!=5) exit(1);
	mat=(double huge **)matrix(rs,rf,cs,cf,
				   sizeof(double) ,sizeof(double huge *));
	sum = 0;
	for (i=rs;i<=rf;i++)
	{
	    for (j=cs;j<=cf;j++)
	    {
		mat[i][j] = 1;
		sum += mat[i][j];
	    };
	    printf("\n%f",(double)sum);
	}
	printf("\nSum = %f",(double)sum);
	free_matrix((char huge **)mat,rs,cs,sizeof(double));
}
#elif defined(_VECTORTEST_)

void  main(int argc, char *argv[])
{
      float huge *vec;
      float sum;
      long  rs,rf,i;
      if (argc!=3) exit(1);
      rs=atol(argv[1]);
      rf=atol(argv[2]);

      vec = (float huge *)alloc_farvector(rs,rf,sizeof(float huge));
      sum=0;
      for (i=rs;i<=rf;i++)
      {
	  vec[i]=1;
	  sum += vec[i];
	  putchar('.');
      }
      printf("\nSum = %f",sum);
      free_farvec((char *)vec,rs,sizeof(float huge));
}
#endif
