/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 * Author: Max Morris (mmorris@iastate.edu)
 */

#include <cassert>
#include <cmath>
#include "mtrandom.h"
#include "random.h"
#include <stdexcept>
#include <time.h>

//BUG: cxx (Compaq's C++ compiler) barfs on the following
//include, leaving ostringstream undefined and so
//preventing this file from compiling.
#include <sstream>
using namespace std;

/**
 * Throws an invalid_argument exception if both
 * [a,b] and [c,d] are empty.
 */
RandomInSplitRange::RandomInSplitRange(int a, int b, int c, int d) : a(a), c(c)
{
  if (a <= b) {
    ablen = b - a + 1;
  }
  else {
    ablen = 0;
  }

  if (c <= d) {
	  //Ru He comments:
	  //lenMinus1 means the length - 1
    lenMinus1 = ablen + d - c;
  }
  else {
    if (ablen > 0) {
      lenMinus1 = ablen - 1;
    }
    else {
      ostringstream what;
      what << "Empty range: [" << a << "," << b << ","
	   << c << "," << d << "]";
      throw invalid_argument(what.str());
    }
  }
}

/**
 * Returns the next int from the range.
 */
int RandomInSplitRange::operator()()
{
  size_t i = randomInRange(0, lenMinus1);
  if (i < ablen) {
    return a + static_cast<int>(i);
  }
  else {
    return c + static_cast<int>(i - ablen);
  }
}

/**
 * Returns an integer in the union of the ranges
 * [a,b] U [c,d]. Requires that a <= b <= c <= d.
 * Each integer in the union has equal probability
 * of being returned.
 */
int randomInSplitRange(int a, int b, int c, int d)
{
  assert(a <= b);
  assert(b <= c);
  assert(c <= d);

  size_t ablen = b - a + 1;
  size_t lenMinus1 = d - c + ablen;
  size_t i = static_cast<size_t>(lenMinus1 * genrand_real2());
  if (i < ablen) {
    return a + static_cast<int>(i);
  }
  else {
    return c + static_cast<int>(i - ablen);
  }
}

/**
 * Returns an integer in the range [lower, upper]
 * at random, each integer in the interval having
 * equal probability of being returned.
 */
size_t randomInRange(size_t lower, size_t upper)
{
  assert(lower <= upper);
  size_t range = upper - lower + 1;
  return static_cast<size_t>(floor(range * genrand_real2()) + lower);
}

/**
 * Returns a draw from U[0,1).
 */
double random01()
{
  return genrand_real2();
}

/**
 * Returns an integer uniform "at random" in the range [0, n-1].
 */
int randomUpTo(int n)
{
  return static_cast<int>(n * random01());
}

double standardNormal()
{
  const double twoPi =  6.283185;

  //Use Box-Muller transformation:
  double u1 = genrand_real3();
  double u2 = genrand_real3();
  return sqrt(-2 * log(u1)) * cos(twoPi * u2);
}

/**
 * Set the seed for the pseudorandom number generator.
 */
void setSeed(unsigned long s)
{
  init_genrand(s);
}

/* to generate a seed using time.
 */
unsigned long getSeedByTime() 
{   
	time_t t0;
	time(&t0);
	return (unsigned long) t0;
}
