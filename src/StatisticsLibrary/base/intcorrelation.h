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

#ifndef __INTCORRELATION_H__
#define __INTCORRELATION_H__

#include <cassert>
#include "corloc.h"
#include <cmath>
#include <vector>
using namespace std;

template<typename ForwardIter1, typename ForwardIter2>

//Ru He comments:
//return the squred corrlation with its sign
double intCompCorr(ForwardIter1 y1, ForwardIter2 y2, int window)
{
    long long s1 = 0;
    long long s2 = 0;
    long long s11 = 0;
    long long s22 = 0;
    long long s12 = 0;
  
    ForwardIter1 end = y1 + window;
    while (y1 != end) {
        const int v1 = *y1++;
        const int v2 = *y2++;
        
        s1 += v1;
        s11 += v1 * v1;
    
        s2 += v2;
        s22 += v2 * v2;
    
        s12 += v1 * v2;
    }
  
    double cs11 = double(window) * s11 - s1 * s1;
    double cs22 = double(window) * s22 - s2 * s2;
    double cs12 = double(window) * s12 - s1 * s2;
    return cs12 * fabs(cs12) / (cs11 * cs22);
}

//XXX! We could probably do a little template trickery
//using numeric_limits<T>::is_integer to generalize
//this method to deal with more int types, falling
//back on a doubleing-type implementation otherwise.

/**
 * x -- first sample.
 * y -- second sample.
 * window -- size of the windows over which to compute correlations.
 *
 * Requires: x.size() == y.size()
 *           x.size() >= window
 */
CorLoc maxCorrelation(const vector<int>& x,
                      const vector<int>& y,
                      size_t window)
{
  
    class IntYStuff {
    public:
      long long ySum;
      double yVar;
    };
    
  
    assert(x.size() == y.size());
    assert(x.size() >= window);
  
    const size_t len = y.size();
    
    typedef vector<int>::const_iterator Iter;
    const Iter xBegin = x.begin();
    const Iter yBegin = y.begin();
    const Iter xEnd = xBegin + (len - window + 1);
    const Iter yEnd = yBegin + (len - window + 1);
  
    double currMax = -9.0f;
    size_t loc1 = 1000000;
    size_t loc2 = 1000000;
    bool maxGreaterThan0 = false;
  
    const long w = static_cast<long>(window);
  
    //To save recalculating the sums for y,
    //we make a special case of the pairs between
    //the first subwindow in x and the subwindows in y,
    //storing the values computed for y as we go.
  
    IntYStuff* ystuff = new IntYStuff[len];
  
    long long sumX = 0;
    long long sumXSq = 0;
    const Iter xEnd2 = xBegin + window;
    for (Iter xi2(xBegin); xi2 != xEnd2; ++xi2) {
        const int xv = *xi2;
        sumX += xv;
        sumXSq += xv * xv;
    }
    const double xVar = w * sumXSq - static_cast<double>(sumX) * sumX;
    
    size_t i = 0;
    for (Iter yi(yBegin); yi != yEnd; ++yi, ++i) {
        long long sumY = 0;
        long long sumYSq = 0;
        long long wSumXY = 0;
        Iter yi2(yi);
        for (Iter xi2(xBegin); xi2 != xEnd2; ++xi2, ++yi2) {
            const int yv = *yi2;
            sumY += yv;
            sumYSq += yv * yv;
            
            const int xv = *xi2;
            wSumXY += xv * yv;
      }
      wSumXY *= w;
      
      const double top = wSumXY - static_cast<double>(sumX) * sumY;
      const double varY = w * sumYSq - static_cast<double>(sumY) * sumY;
  
      ystuff[i].ySum = sumY;
      ystuff[i].yVar = varY;
  
      if (top > 0) {
          double sqCor = static_cast<double>(top)*top / (xVar * varY);
          if (sqCor > currMax) {
              maxGreaterThan0 = true;
              currMax = sqCor;
              loc1 = 0;
              loc2 = i;
          }
      }
      else if (maxGreaterThan0) { }
      else {
          double sqCor = -static_cast<double>(top)*top /  (xVar * varY);
          if (sqCor > currMax) {
              currMax = sqCor;
              loc1 = 0;
              loc2 = i;
          }
      }
    }
  
    //Now having stored the sums for y, we continue on with
    //the subsequent subwindows in x.
  
    for (Iter xi(xBegin+1); xi != xEnd; ++xi) {
        long long sumX = 0;
        long long sumXSq = 0;
        const Iter xEnd2 = xi + window;
        for (Iter xi2 = xi; xi2 != xEnd2; ++xi2) {
            const int xv = *xi2;
            sumX += xv;
            sumXSq += xv * xv;
        }
        const double xVar = w * sumXSq - static_cast<double>(sumX) * sumX;
    
        size_t i = 0;
        const IntYStuff* ystuffIter = ystuff;
        for (Iter yi(yBegin); yi != yEnd; ++yi, ++i, ++ystuffIter) {
            long long wSumXY = 0;
            Iter yi2(yi);
            for (Iter xi2 = xi; xi2 != xEnd2; ++xi2, ++yi2) {
                const int yv = *yi2;
                const int xv = *xi2;
                wSumXY += xv * yv;
            }
            wSumXY *= w;
            
            const double top = static_cast<double>( wSumXY - sumX * (ystuffIter->ySum) );
            if (top > 0) {
                const double sqCor = static_cast<double>(top)*top / (xVar * (ystuffIter->yVar));
                if (sqCor > currMax) {
                    maxGreaterThan0 = true;
                    currMax = sqCor;
                    loc1 = xi - xBegin;
                    loc2 = i;
                }
            }
            else if (maxGreaterThan0) { }
            else {
                const double sqCor = -static_cast<double>(top)*top / (xVar * (ystuffIter->yVar));
                if (sqCor > currMax) {
                    currMax = sqCor;
                    loc1 = xi - xBegin;
                    loc2 = i;
                }
            }
        }
    }
    
    delete[] ystuff;
    if (maxGreaterThan0) {
         return CorLoc(sqrt(currMax), loc1, loc2);
    } else {
         return CorLoc(-sqrt(-currMax), loc1, loc2);
    }
 }
  

/**
 * Return the location and correlation of the pair of windows
 * of the given size with the maximum Pearson correlation coefficient.
 */
CorLoc leveledMaxCorrelation(const vector<int>& x,
                             const vector<int>& y,
                             size_t window)
{
  size_t nn = x.size();  
  assert(nn == y.size());
  assert(nn >= window);
  assert(window > 0);

  size_t end = nn - window + 1;

  typedef vector<int>::const_iterator Iter;
  const Iter xBegin = x.begin();
  const Iter yBegin = y.begin();
  const Iter xEnd = xBegin + end;
  const Iter yEnd = yBegin + end;

  double* xSubwindow = new double[window];

  //meanX == 1/n * Sum(j=0,n-1)j = 1/n * (n * ((n-1) + 0) / 2)
  // == (n-1)/2.
  const double meanZ =(window-1)/2.0f;

  //Sum(j=0,n-1)(j^2) == Sum(j=1,n-1)(j^2) == (n-1)n(2(n-1)+1)/6
  // == n * (n-1)*(2n-1)/6 from the formula for the sum of the first n squares.
  //
  //ssZZ == Sum(j=0,n-1)(j^2) - n * meanZ^2
  // == n * ((n-1)(2n-1)/6 - meanZ^2)
  const double ssZZ = window * ((window-1)*(2*window-1)/6.0f - meanZ*meanZ);

  double currentMax = -10.0f;
  size_t loc1 = 1000000;
  size_t loc2 = 1000000;
  bool maxGreaterThan0 = false;

  const double w = (double) window;

  long lwindow = static_cast<long>(window);
  for (Iter xi = xBegin; xi != xEnd; ++xi) {
    long long sumX = 0;
    long long sumXZ = 0;
    Iter xii(xi);
    for (long j = 0; j != lwindow; ++j) {
      const int v = *xii++;
      sumX += v;
      sumXZ += v * j;
    }

    const double xSlope = (sumXZ - meanZ * sumX) / ssZZ;
    const double xIntercept = sumX/w - xSlope * meanZ;

    //Compute residuals
    double xrSqSum = 0.0f;
    xii = xi;
    double* xSub = xSubwindow;
    for (size_t j = 0; j != window; ++j, ++xii, ++xSub) {
      const double xr = *xii - xSlope * j;
      xrSqSum += xr * xr;
      *xSub = xr;
    }

    const double wXIntercept = w * xIntercept;
    const double xVar = xrSqSum - wXIntercept * xIntercept;

    for (Iter yi = yBegin; yi != yEnd; ++yi) {
      long long sumY = 0;
      long long sumYZ = 0;
      Iter yii(yi);
      for (long j = 0; j != lwindow; ++j) {
        const int v = *yii++;
        sumY += v;
        sumYZ += v * j;
      }

      const double ySlope = (sumYZ - meanZ * sumY) / ssZZ;
      const double yIntercept = sumY/w - ySlope * meanZ;

      //Compute Sum(yr * yr) and Sum(xr * yr)
      double yrSqSum = 0.0f;
      double xrYrSum = 0.0f;
      xSub = xSubwindow;
      yii = yi;
      for (long j = 0; j != lwindow; ++j) {
        const double yr = *yii++ - ySlope * j;
        yrSqSum += yr * yr;
        xrYrSum += yr * (*xSub++);
      }

      const double top = xrYrSum - wXIntercept * yIntercept;

      if (xrYrSum > 0.0f) {
        const double corSquared =  top*top / (xVar * (yrSqSum - w * yIntercept * yIntercept));
        if (corSquared > currentMax) {
          currentMax = corSquared;
          loc1 = xi - xBegin;
          loc2 = yi - yBegin;
          maxGreaterThan0 = true;
        }
      }
      else if (maxGreaterThan0) {
      }
      else {
        const double corSquared = top * (-top) / (xVar * (yrSqSum - w * yIntercept * yIntercept));
        if (corSquared > currentMax) {
          currentMax = corSquared;
          loc1 = xi - xBegin;
          loc2 = yi - yBegin;
        }
      }
    }
  }

  delete[] xSubwindow;
  if (maxGreaterThan0) {
    return CorLoc(sqrt(currentMax), loc1, loc2);
  }
  else {
    return CorLoc(-sqrt(-currentMax), loc1, loc2);
  }
}


/**
 * Compute the signed squared correlation of the sequences
 * [y1, y1 + window) and [y2, y2 + window). That is, return
 * the square of the correlation times its sign.
 */
template <typename ForwardIter1, typename ForwardIter2>
double doubleCompSqCorr(ForwardIter1 y1, ForwardIter2 y2, size_t window)
{
  double s1 = 0.0f;
  double s2 = 0.0f;
  double s11 = 0.0f;
  double s22 = 0.0f;
  double s12 = 0.0f;

  ForwardIter1 end = y1 + window;
  while (y1 != end) {
    const double v1 = *y1++;
    const double v2 = *y2++;

    s1 += v1;
    s2 += v2;
    s11 += v1 * v1;
    s22 += v2 * v2;
    s12 += v1 * v2;
  }
  double cs11 = window * s11 - s1*s1;
  double cs22 = window * s22 - s2*s2;
  double cs12 = window * s12 - s1*s2;
  return cs12 * fabs(cs12) / (cs11 * cs22);
}

/**
 * Computes the Pearson correlation of the segments of length window
 * beginning at i1 and i2 in y1 and y2 respectively, levelled
 * by subtracting a line fit to each segment vs. its indeces.
 */
template<typename ForwardIterator1, typename ForwardIterator2>
double intCompSqCorrLeveled(ForwardIterator1 y1, ForwardIterator2 y2, size_t window)
{
  const size_t maxWindowSize = 9600;

  double residuals1[maxWindowSize];
  double residuals2[maxWindowSize];

  assert(window <= maxWindowSize);

  long long int sumY1 = 0;
  long long int sumY2 = 0;

  long long int sumProdY1 = 0;
  long long int sumProdY2 = 0;

  const long int swindow = static_cast<long int>(window);
  for (long int w = 0; w != swindow; ++w) {
    const int v1 = *y1++;
    const int v2 = *y2++;

    sumY1 += v1;
    sumY2 += v2;

    //XXX! It's a waste to carry out this operation
    //for w = 0.
    sumProdY1 += v1 * w;
    sumProdY2 += v2 * w;

    //We copy the values of y1 and y2 so as not to have to violate
    //the contract of the ForwardIterators.
    residuals1[w] = (double) v1;
    residuals2[w] = (double) v2;
  }

  const double meanY1 = static_cast<double>(sumY1) / window;
  const double meanY2 = static_cast<double>(sumY2) / window;

  //meanX == 1/n * Sum(j=0,n-1)j = 1/n * (n * ((n-1) + 0) / 2)
  // == (n-1)/2.
  const double meanX = (window-1)/2.0f;

  //Sum(j=0,n-1)(j^2) == Sum(j=1,n-1)(j^2) == (n-1)n(2(n-1)+1)/6
  // == n * (n-1)*(2n-1)/6 from the formula for the sum of the first n squares.
  //
  //ssXX == Sum(j=0,n-1)(j^2) - n * meanX^2
  // == n * ((n-1)(2n-1)/6 - meanX ^2)
  const double ssXX = window * ((window-1)*(2*window-1)/6.0f - meanX*meanX);

  const double ssXY1 = sumProdY1 - window * meanX * meanY1;
  const double ssXY2 = sumProdY2 - window * meanX * meanY2;

  const double b1 = ssXY1 / ssXX;
  const double a1 = meanY1 - b1 * meanX;

  const double b2 = ssXY2 / ssXX;
  const double a2 = meanY2 - b2 * meanX;

  for (size_t i = 0; i != window; ++i) {
    residuals1[i] -= (b1 * i + a1);
    residuals2[i] -= (b2 * i + a2);
  }
  return doubleCompSqCorr(residuals1, residuals2, window);
}

template<typename ForwardIterator>
double intCompCorrLeveled(ForwardIterator y1, ForwardIterator y2, size_t window)
{
  double sqCor = intCompSqCorrLeveled(y1, y2, window);
  if (sqCor >= 0.0) {
     return sqrt(sqCor);
  } else {
     return -sqrt(-sqCor);
  }
}

double intCompCorrLeveled(vector<int>& y1, vector<int>& y2)
{
  return intCompCorrLeveled(y1.begin(), y2.begin(), y1.size());
}


#endif
