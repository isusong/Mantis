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


#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <cstddef>

/**
 * Returns an integer in the range [lower, upper]
 * at random, each integer in the interval having
 * equal probability of being returned.
 */
size_t randomInRange(std::size_t lower, std::size_t upper);

double standardNormal();

/**
 * Returns a draw from U[0,1).
 */
double random01();

/**
 * Set the seed for the pseudorandom number generator.
 */
void setSeed(unsigned long s);

/**
 * A functor which returns an integer in the union of ranges,
 * [a,b] U [c,d]. If b < a, takes [a,b] to be
 * empty (and so will never return a point in the range [a,b]).
 * Similarly for [c,d].
 * Each integer in the union has equal probability
 * of being returned.
 *
 * To get repeatable behavior, call setSeed(.) with
 * a constant value before creating a particular instance.
 */
class RandomInSplitRange {
 public:
  /**
   * Throws an invalid_argument exception if both
   * [a,b] and [c,d] are empty.
   */
  RandomInSplitRange(int a, int b, int c, int d);

  /**
   * Returns the next int from the range.
   */
  int operator()();

 private:
  int a;
  int c;
  std::size_t ablen;
  std::size_t lenMinus1;
};

/**
 * A functor that returns random integers in the range [0,n), each
 * with uniform probability. A model for
 * http://www.sgi.com/tech/stl/RandomNumberGenerator.html
 */
class RandomUpTo {
 public:
  std::size_t operator()(std::size_t n) { return static_cast<std::size_t>(n * random01());}
};

/**
 * Returns an integer uniform "at random" in the range [0, n-1].
 * The results are undefined for n <= 0.
 */
int randomUpTo(int n);

/**
 * SAMPLE WITHOUT REPLACEMENT:
 * Sets the range [result, result + sampleSize) equal
 * to sampleSize pseudorandom draws without replacement from
 * the integers (size_t) in [0, n), each integer
 * having an equal chance of getting drawn.
 *
 * For repeatable results, call setSeed(.) prior
 * to calling this method.
 *
 * Returns result + sampleSize.
 */
template<typename OutputIterator>
inline OutputIterator randomSample(std::size_t n, std::size_t sampleSize, OutputIterator result)
{
  //I looked at the following code while implementing this:
  //http://www.swarm.org/pipermail/support/1996-August/000620.html, which 
  // is copied below,
  //as well as Knuth's "The Art of Computer Programming: Volume II"
  // -- Algorithm S under random sampling.

   //copied from the above link
   /*

At 2:40 PM -0400 8/21/96, Rick Riolo wrote:
>I'd like to know the recommended way to get
>some random samples without replacement from a collection (in particular
>a list if it matters).

Hi Rick! :)
Knuth gives 2 algorithms (v. 2, pp. 137ff.) for sampling n items without
replacement from a collection of N items, with probability of n/N for each
item.  One algorithm assumes N is known, the second doesn't.  Here's a C
implementation of the first algorithm.  This way you don't have to remove
agents from the collection, if that matters to you.
-Ted


void Select(int n, int N, int* result) {
	// Algorithm S, Knuth v. 2, p. 137
	// select random combination

	// select n items at random from a set of N, 0 < n <= N
	// each item is selected (in order) with prob n/N
	// items are numbered [0, ..., N-1]
	// returns result in array int result[0,...,n-1]
	// (caller must allocate space for array)

	// initialize
	int t = 0; // number of items dealt with
	int m = 0; // number of items selected
	float U;

	while (m < n) {
		U = rnd(0,1); // uniformly generated random number
			      // on interval (0,1)

		if ( ((N - t) * U) >= (n - m) ) {
			// skip this item
			t++;
		}
		else {
			// select next item and add it to list
			result[m++] = t++;
		}
	}
}
--============_-1371463997==_============
	*/

  std::size_t t = 0;
  std::size_t m = 0;
  while (m < sampleSize) {
    double u = random01();
    if ((n - t) * u < (sampleSize - m)) {
      *result = t;
      ++result;
      ++m;
    }
    ++t;
  }
  return result;
}

/**
 * Returns an integer in the union of the ranges
 * [a,b] U [c,d]. Requires that a <= b <= c <= d.
 * Each integer in the union has equal probability
 * of being returned.
 */
int randomInSplitRange(int a, int b, int c, int d);

/* get a seed set by current time */
unsigned long getSeedByTime();

#endif

