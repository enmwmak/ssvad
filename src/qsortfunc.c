/*
  Function for the qsort() routine in stdlib
*/

#include "qsortfunc.h"

int   qsortcomparef(const vec_t *x, const vec_t *y)
{
      if (*x < *y)
           return -1;
      if (*x == *y)
           return 0;
      else
           return 1;
}
