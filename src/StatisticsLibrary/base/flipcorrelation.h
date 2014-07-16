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
 * Authors: Max Morris (mmorris@iastate.edu), Laura Ekstrand (ldmil@iastate.edu)
 */

#ifndef __FLIPCORRELATION_H__
#define __FLIPCORRELATION_H__

#include <algorithm>
#include <cmath>
#include "FlippableCorLoc.h"
#include <iterator>
#include <stdexcept>
#include <sstream>
#include "mydebug.h"
#include <iostream>

using std::runtime_error;
using std::cout;
using std::endl;

/**
 * Note: In order to fix a linking issue,
 * I have had to split the definition and declaration
 * of FlippableCorLoc into separate files
 * (as probably should've been done from the beginning
 * anyway). Older code should switch to including
 * FlippableCorLoc.h and compiling against
 * FlippableCorLoc.cpp.
 */

template <typename RandomAccessIter>
class MaxCorrelationWithFlips {
   public:
  
   /**
    * Finds the pair of windows with max correlation
    * out of all windows of the given length
    * between y1 and y2 and y1 (by default) and y2 reversed.
    *
    * y1 -- iterator pointing to a sequence of ints.
    * y2 -- iterator pointing to a sequence of ints.
    * length1 -- length of the first sequences
    * length2 -- length of the second sequences
    * window -- size of the windows over which to compute correlations.
    * checkFlip -- also compare y1 and y2 reversed.
    * 
    * this function is changed by Maverick to accommodate 
    * different lengths for the two traces 
    *
    *  XXX! it does not support flipping any more (by Maverick)  
    *  
    */
	//Laura Ekstrand (March 2013) - added maxShiftPercentage as leash for 
	//Opposite End Problem - see flipcorrelation.h:maxCorVaryingSecond().
   FlippableCorLoc operator()(RandomAccessIter y1,
                              RandomAccessIter y2,
                              int length1,
                              int length2,
                              int window,
							  float maxShiftPercentage,
                              bool checkFlip = false) const 
   {
       if (length1 < 0 || length2 < 0) {
           std::ostringstream what;
           what << "length1 or length2 < 0: [length1=" << length1 << ", length2=" << length2 << "]" ;
           throw std::out_of_range(what.str());
       }
       if (window < 0) {
           std::ostringstream what;
           what << "window < 0: " << window;
           throw std::out_of_range(what.str());
       }
       if (window > length1 || window> length2) {
           std::ostringstream what;
           what << "window > length1 or length2: " << window << " > " << "[lenght1=" << length1 << ", length2=" << length2 << "]";
           throw std::out_of_range(what.str());
       }
     
       /**
        * One past the leftmost index of the rightmost
        * subwindow, ie, the number of subwindows
        * that will fit within a single sequence.
        */
       const int numWindows1 = length1 - window + 1;
       const int numWindows2 = length2 - window + 1;
     
       /* why do we need to make another copy? (Q by Maverick)  
        * probably for the reversed copy of trace 2 (A by Maverick)
        */
       int* y1Copy = new int[length1];
       std::copy(y1, y1 + length1, y1Copy);
     
       int* y2Copy = new int[length2];
       std::copy(y2, y2 + length2, y2Copy);
       
       //XXX! We could make this code more "exception-safe"
       //by wrapping these in auto_ptrs or at least dropping
       //extra copies of the pointers into auto_ptrs
       //and using those to manage the memory.
       SumVar* y1Table = new SumVar[numWindows1];
       SumVar* y2Table = new SumVar[numWindows2];
     
       //Precompute the sum and variance of each
       //subwindow. We might combine this with
       //the first loop of the code to find the
       //correlations, but separating it thus
       //allows for more reuse/simpler code.
       
       //Initialization:
       int* y1Old(y1Copy);
       int* y2Old(y2Copy);
       long long y1Sum = 0;
       long long y1SqSum = 0;
       long long y2Sum = 0;
       long long y2SqSum = 0;
       int* y1Next(y1Old);
       int* y2Next(y2Old);
       for (int i = 0; i != window; ++i) {
           const int v1 = *y1Next++;
           y1Sum += v1;
           y1SqSum += v1 * v1;
     
           const int v2 = *y2Next++;
           y2Sum += v2;
           y2SqSum += v2 * v2;
       }
       y1Table[0].resetWith(window, y1Sum, y1SqSum);
     
       y2Table[0].resetWith(window, y2Sum, y2SqSum);
     
	   //Ru He comments:
	   //The following each-time-update-one strategy is used to reduce the computation costs
	   //Since 
       for (int i = 1; ; ++i) {
            if (i >= numWindows1 && i >= numWindows2)  break; 
            if (i < numWindows1) { 
                 const long long old1 = (long long) *y1Old++;
                 y1Sum -= old1;
                 y1SqSum -= old1 * old1;
                 const long long next1 = (long long) *y1Next++;
                 y1Sum += next1;
                 y1SqSum += next1 * next1;
                 y1Table[i].resetWith(window, y1Sum, y1SqSum);
				// cout << "i=" << i << ", y1sum=" << y1Sum << ", y1sqsum=" << y1SqSum << endl;
            } 
            if (i < numWindows2) { 
                const long long old2 = (long long) *y2Old++;
                y2Sum -= old2;
                y2SqSum -= old2 * old2;
                const long long next2 = (long long) *y2Next++;
                y2Sum += next2;
                y2SqSum += next2 * next2;
                y2Table[i].resetWith(window, y2Sum, y2SqSum);
		//		cout << "i=" << i << ", y2sum=" << y2Sum << ", y2sqsum=" << y2SqSum << endl;
            } 
       }
       
	   //Ru He comments: -2.0f is small enough since corr is in [-1, 1]
       SqCorLoc c = maxCorrelation(y1Copy, y2Copy, length1, length2, window, maxShiftPercentage, y1Table, y2Table, -2.0f);
	   // cout << "[" << c.loc1() << c.loc2() << "]";
     
       /* I suppose the following code is used to compute the correlations 
        * for the [trace 1] and [the reverse of trace 2]  
        * comments added by Maverick 
        *
        * And for the time being Maverick would comment all of the following 
        * so that it does not support flip and would not take 
        * the maximum of [y1 vs y2] and [y1 vs y2 reversed] 
        */
       /* 
       SqCorLoc cReversed(-10,-10,-10);
       int* y2Reversed = 0;
       SumVar* y2TableReversed = 0;
       if (checkFlip) {
          y2Reversed = new int[length2];
          std::reverse_copy(y2Copy, y2Copy + length, y2Reversed);
          
          y2TableReversed = new SumVar[numWindows];
          std::reverse_copy(y2Table, y2Table + numWindows, y2TableReversed);
          
          cReversed = maxCorrelation(y1Copy, y2Reversed, length1, length2, window, y1Table, y2TableReversed, c.sqCor());
       }
       */ // commented out by Maverick 
     
       delete[] y1Copy;
       delete[] y2Copy;
       delete[] y1Table;
       delete[] y2Table; 

       /*
       delete[] y2Reversed;
       delete[] y2TableReversed;
       */ // commented out by maverick 
       /*
     
       if (!checkFlip || c > cReversed) {
         return FlippableCorLoc(c.cor(), c.loc1(), c.loc2(), false);
       }
       else {
         return FlippableCorLoc(cReversed.cor(), cReversed.loc1(), cReversed.loc2(), true);
       }
       */ // commented out by Maverick 
       return FlippableCorLoc(c.cor(), c.loc1(), c.loc2(), false); //added by Maverick 
  }
  
   private:
    /**
     * Nested struct for storing the sum and variance
     * corresponding to a given position.
     */
    struct SumVar {
      double sum;
      double var;
      /**
       * Sets the fields based off of the given window size, sum, and squared sum.
       */
      inline void resetWith(int window, long long newSum, long long sqSum) {
        sum = (double) newSum;
        var = 1.0 * window * sqSum - (newSum) * newSum;
        /*
        so for y_1, y_2, ... y_n,
        var (defined right above) = n * \sum_i y_2^2 - n^2 * ybar^2
         */
      }
    };
  
  
    /**
     * Nested class for storing the value and location, which has two values for the two traces,
     * of the max signed squared correlation found.
     */
    class SqCorLoc {
    public:
      SqCorLoc(double sqCor, int loc1, int loc2) : _sqCor(sqCor), _loc1(loc1), _loc2(loc2) {}
      int loc1() const { return _loc1; }
      int loc2() const { return _loc2; }
  
      /**
       * Returns the signed squared correlation.
       */
      double sqCor() const { return _sqCor; }
      
      /**
       * Returns the correlation.
       */
      double cor() const {
        if (_sqCor >= 0.0f) {
            return std::sqrt(_sqCor);
        }
        else {
            return -std::sqrt(-_sqCor);
        }
      }
  
      SqCorLoc flipLocations() const {
          return SqCorLoc(_sqCor, _loc2, _loc1);
      }
  
      bool operator>(const SqCorLoc& c) const {
        return sqCor() > c.sqCor();
      }
  
    private:
      double _sqCor;
      int _loc1;
      int _loc2;
    };
  

	//Ru He comments:
	//get the max correlation and its corresponding locs for trace 1 and trace 2
	//Laura Ekstrand (March 2013) - added maxShiftPercentage as leash for 
	//Opposite End Problem - see flipcorrelation.h:maxCorVaryingSecond().
    SqCorLoc maxCorrelation(const int* y1,
                const int* y2,
                int length1,
                int length2,
                int window,
				float maxShiftPercentage,
                const SumVar* y1Table,
                const SumVar* y2Table,
                float priorMaxSqCor) const
      {
        // a little be confused here: whey we need two by Mav
        // figured it out because maxCorVaryingSecond only find the max of one direction. 
	    //Ru He question: is the following two lines are enough?
		//Ru He ans: Yes. It will really run (length1 - window) * (length2 - window) iterations

		//  cout << "leng1=" << length1 << "--length2=" << length2 << endl;
        SqCorLoc c1 = maxCorVaryingSecond(0, y1, y2, length1, length2, window, maxShiftPercentage, y1Table, y2Table, priorMaxSqCor);
		//cout << c1.loc1() << "+" << c1.loc2() << "[cor=]" << c1.cor() << endl;

		//Ru He comments: 1 vs. 0 in previous function, 1 is set to avoid the min(length1, length2) duplicate comparisons

        SqCorLoc c2 = maxCorVaryingSecond(1, y2, y1, length2, length1, window, maxShiftPercentage, y2Table, y1Table, c1.sqCor());

		//cout << c2.loc1() << "+---+" << c2.loc2() << "<cor>=" << c2.cor() <<  endl;

        if (c1 > c2) {
              return c1;
        } else {
              return c2.flipLocations();
        }
      }//SqCorLoc maxCorrelation
  
    // added by maverick 
    // a function that support different lengths for the two traces 
    // length1 -- length of the second sequences
    // length2 -- length of the second sequences
  
    /* comments added by Maverick
     * I suppose first the function is called 'xxxSecond' because 
     * in this function the start point of the second trace 
     * is always greater than the start point of the first one. 
     * As a result, this function needs to be called twice to 
     * get the total max.  In the second call, we exchange trace1 and trace 2. 
     * And that is exactly what is done in ``maxCorrelation'' 
     *
     * the change maverick did in this function is to make to accommodate 
     * different lengths of the two traces. First, the shift needs to be 
     * changed.  Second, when doing the loop over the shift, we need also
     * to be careful about that we are running out of windows for the first 
     * trace.
     */

	 //comment from Laura Ekstrand (March 2013)
	 //float maxShiftPercentage added as input argument
	 //This is a value between 0.1 and 1.0 that the full 
	 //maxShift = length2 - window is multiplied by 
	 //to avoid the Opposite End Problem.
    SqCorLoc maxCorVaryingSecond(int minShift,
                     const int* y1,
                     const int* y2,
                     int length1,
                     int length2,
                     int window,
					 float maxShiftPercentage,
                     const SumVar* y1Table,
                     const SumVar* y2Table,
                     double priorMaxSqCor) const
      {
        bool maxGreaterThan0 = priorMaxSqCor > 0.0f;
        //cout << "yalenght1=" << length1 << "--->yalength2=" << length2 << endl;
        double currMax = -10.0f;
        int loc1 = -1;
        int loc2 = -1;
  
        //shift = leftmost index of the window in the 2nd sequence
        //minus the leftmost index of the window in the 1st sequence
        //Ie, shift = loc2 - loc1
        //Thus, loc2 = loc1 + shift.
  
        const int maxShift = maxShiftPercentage*(length2 - window);

		//cout << "maxshift=" << maxShift << endl;
		
        // this is the only change by Maverick inside this function 
		//Ru He comments: here only set the move range of the second search window
        for (int shift = minShift; shift <= maxShift; ++shift) {
			
          //Initialization
          const int* y1Old(y1);
          const int* y2Old(y2 + shift);


          const int* y1Next(y1Old);
          const int* y2Next(y2Old);
          const int* y2NextEnd = y2 + length2;
          const int* y1NextEnd = y1 + length1;
      
          const SumVar* y1TableIter = y1Table;
          const SumVar* y2TableIter = y2Table + shift;
          long long prodSum = 0;
		  long long s1, s2, s1q, s2q;
		  s1=0; s2=0; s1q=0; s2q=0;
      
		  //Ru He comments:
		  //Before loop of while (y2Next != y2NextEnd && y1Next != y1NextEnd) 
          for (int j = 0; j != window; ++j) {
            const int v1 = *y1Next++;
            const int v2 = *y2Next++;
			//Ru He question: why still need the following 4 lines
			s1 += v1;
			s2 += v2;		
			s1q += v1*v1;
			s2q += v2*v2;
			//
            prodSum += v1 * v2;
  
          }
		  //cout << s1 << "|" << s2 << "|" << s1q << "|" << s2q << endl;
          /* to compute the correlation 
           * ref: http://upload.wikimedia.org/math/c/a/6/ca68fbe94060a2591924b380c9bc4e27.png
           */
		  //Ru He comments:
		  //top is n^2 * sample covariance
          const double top = window * prodSum - y1TableIter->sum * y2TableIter->sum;
		  //if Cov is positive
          if (top > 0.0f) {
            const double sqCor = top * top / (y1TableIter->var * y2TableIter->var);
            if (sqCor > currMax) {
              currMax = sqCor;
			  //Ru He comments: so loc1 value range starts from 0
              loc1 = y1TableIter - y1Table;
              loc2 = loc1 + shift;
              maxGreaterThan0 = true;
            }
          }
		  //if Cov is not positive and maxGreaterThan0
          else if (maxGreaterThan0) {}
          else {
            const double sqCor = -top * top / (y1TableIter->var * y2TableIter->var);
            if (sqCor > currMax) {
              currMax = sqCor;
              loc1 = y1TableIter - y1Table;
              loc2 = loc1 + shift;
            }
          }
  
          //Update sums
		  //Ru He comments: y2Next != y2NextEnd && y1Next != y1NextEnd safeguard the end of each trace
          while (y2Next != y2NextEnd && y1Next != y1NextEnd) {
            const int old1 = *y1Old++;
            const int old2 = *y2Old++;
            prodSum -= old1 * old2;
        
            const int next1 = *y1Next++;
            const int next2 = *y2Next++;
            prodSum += next1 * next2;
  
            ++y1TableIter;
            ++y2TableIter;
            const double top = window * prodSum - y1TableIter->sum * y2TableIter->sum;
#ifdef IMPOSSIBLEDEFINED 
#endif


            if (top > 0.0f) {
              const double sqCor = top * top / (y1TableIter->var * y2TableIter->var);
              if (sqCor > currMax) {
                currMax = sqCor;
                loc1 = y1TableIter - y1Table;
                loc2 = loc1 + shift;
                maxGreaterThan0 = true;
              }
            }
            else if (maxGreaterThan0) {}
            else {
              const double sqCor = -top * top / (y1TableIter->var * y2TableIter->var);
              if (sqCor > currMax) {
                currMax = sqCor;
                loc1 = y1TableIter - y1Table;
                loc2 = loc1 + shift;
              }
            }
          }
		  //cout << "CURR=" << currMax << endl;
		  if (currMax > 1) {
			  std::cout << std::endl << "l1=" << loc1 << "--l2=" << loc2;
		  }
		  //cout << endl;

        }//while (y2Next != y2NextEnd && y1Next != y1NextEnd) 

		//cout << "CURR=" << currMax << endl;
        return SqCorLoc(currMax, loc1, loc2);
      }////SqCorLoc maxCorVaryingSecond()



  /*
    // commented out by mav
    SqCorLoc maxCorVaryingSecond(int minShift,
                     const int* y1,
                     const int* y2,
                     int length,
                     int window,
                     const SumVar* y1Table,
                     const SumVar* y2Table,
                     float priorMaxSqCor) const
      {
        bool maxGreaterThan0 = priorMaxSqCor > 0.0f;
  
        float currMax = -10.0f;
        int loc1 = -1;
        int loc2 = -1;
  
        //shift = leftmost index of the window in the 2nd sequence
        //minus the leftmost index of the window in the 1st sequence
        //Ie, shift = loc2 - loc1
        //Thus, loc2 = loc1 + shift.
        const int maxShift = length - window;
        for (int shift = minShift; shift <= maxShift; ++shift) {
          //Initialization
          const int* y1Old(y1);
          const int* y2Old(y2 + shift);
          const int* y1Next(y1Old);
          const int* y2Next(y2Old);
          const int* y2NextEnd = y2 + length;
      
          const SumVar* y1TableIter = y1Table;
          const SumVar* y2TableIter = y2Table + shift;
          long long prodSum = 0;
      
          for (int j = 0; j != window; ++j) {
            const int v1 = *y1Next++;
            const int v2 = *y2Next++;
            prodSum += v1 * v2;
  
          }
          * to compute the correlation 
           * ref: http://upload.wikimedia.org/math/c/a/6/ca68fbe94060a2591924b380c9bc4e27.png
           
          const float top = window * prodSum - y1TableIter->sum * y2TableIter->sum;
          if (top > 0.0f) {
            const float sqCor = top * top / (y1TableIter->var * y2TableIter->var);
            if (sqCor > currMax) {
              currMax = sqCor;
              loc1 = y1TableIter - y1Table;
              loc2 = loc1 + shift;
              maxGreaterThan0 = true;
            }
          }
          else if (maxGreaterThan0) {}
          else {
            const float sqCor = -top * top / (y1TableIter->var * y2TableIter->var);
            if (sqCor > currMax) {
              currMax = sqCor;
              loc1 = y1TableIter - y1Table;
              loc2 = loc1 + shift;
            }
          }
  
          //Update sums
          while (y2Next != y2NextEnd) {
            const int old1 = *y1Old++;
            const int old2 = *y2Old++;
            prodSum -= old1 * old2;
        
            const int next1 = *y1Next++;
            const int next2 = *y2Next++;
            prodSum += next1 * next2;
  
            ++y1TableIter;
            ++y2TableIter;
            const float top = window * prodSum - y1TableIter->sum * y2TableIter->sum;
            if (top > 0.0f) {
              const float sqCor = top * top / (y1TableIter->var * y2TableIter->var);
              if (sqCor > currMax) {
                currMax = sqCor;
                loc1 = y1TableIter - y1Table;
                loc2 = loc1 + shift;
                maxGreaterThan0 = true;
              }
            }
            else if (maxGreaterThan0) {}
            else {
              const float sqCor = -top * top / (y1TableIter->var * y2TableIter->var);
              if (sqCor > currMax) {
                currMax = sqCor;
                loc1 = y1TableIter - y1Table;
                loc2 = loc1 + shift;
              }
            }
          }
        }
        return SqCorLoc(currMax, loc1, loc2);
      }
	  */
};
//class MaxCorrelationWithFlips


/**
 * Convenience function.
 *
 * modified by Maverick to support different lengths for the two 
 * traces 
 */
//Laura Ekstrand (March 2013) - added maxShiftPercentage as leash for 
//Opposite End Problem - see flipcorrelation.h:maxCorVaryingSecond().
template<typename RandomAccessIter>
FlippableCorLoc maxCorWithFlips(RandomAccessIter y1, RandomAccessIter y2, int length1, int length2, int window, float
	maxShiftPercentage, bool checkFlip = true)
{
    MaxCorrelationWithFlips<RandomAccessIter> f;
    try {
         return f(y1, y2, length1, length2, window, maxShiftPercentage, checkFlip);
    } catch (runtime_error err) { 
#ifdef MYDEBUG
		cout << "I am going to throw an error.\n"; 
		cout << err.what() << endl;
#endif
		throw err; 
	}
}

#endif
