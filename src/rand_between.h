/*
  rand_between.h
  rand_between functional prototype

  programmer  rmcinnis
  date        july 06, 1997
*/
#ifndef RAND_BETWEEN_H
#define RAND_BETWEEN_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int rand_between( int lo, int hi ) 
{ 
  if (hi == lo) return lo ; 
  if (lo > hi)
  { // swap
    lo ^= hi ;
    hi ^= lo ;
    lo ^= hi ;
  }
  return rand() % (hi-lo) + lo ; 
} // :: rand_between

#ifdef __cplusplus
} ;
#endif

#endif
