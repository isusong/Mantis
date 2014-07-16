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


#ifndef __INTNOLEV_FUNCTORS_H__
#define __INTNOLEV_FUNCTORS_H__

#include "random.h"
#include "intcorrelation.h"
#include <memory>
#include <vector>

/**
 * Functors for code the version of the algorithm
 * that doesn't do levelling.
 */

class IntNonrigidCorSampWithFlips {
public:
  std::auto_ptr<std::vector<double> >
    operator()(const std::vector<int>& y1,
	       const std::vector<int>& y2,
	       size_t pairs,
	       size_t window)
    {
      std::auto_ptr<std::vector<double> >
	result(new std::vector<double>(pairs));
      size_t r = y1.size() - window + 1;
      
      for (size_t j = 0; j < pairs; ++j) {
	std::vector<int>::const_iterator y1Begin =
	  y1.begin() + static_cast<size_t>(r*random01());
	
	//An offset into y2.
	size_t i2 = static_cast<size_t>(r*random01());
	
	if (random01() < 0.5) {
	  (*result)[j] = intCompCorr(y1Begin, y2.begin() + i2, window);
	}
	else {
	  //y2.rbegin() returns a reverse iterator pointing
	  //to the last element in y2. Adding i2 to it
	  //makes it point to an element i2 elements from the
	  //end. And as a reverse iterator, the other operations
	  //(++) will also act in reverse, hence the usefulness
	  //of this type of iterator here.
	  (*result)[j] = intCompCorr(y1Begin, y2.rbegin() + i2, window);
	}
      }
      return result;
    }
};

class IntRigidCorSampExcludeSearch {
 public:
  /**
   * Returns at most pairs correlations at rigid shifts about
   * the indices given in loc in y1 and y2. window is the length
   * of the segments for which the correlations are computed.
   * Exclude the search window, ie, the regions with the highest correlation
   * from consideration.
   * (l1, l2), for which we have the maximum correlation
   */
  std::auto_ptr<std::vector<double> >
    operator()(const std::vector<int>& y1,
	       const std::vector<int>& y2,
	       int l1,
	       int l2,
	       size_t searchWindow,
	       size_t pairs,
	       size_t window)
    {
      std::auto_ptr<std::vector<double> >
	  result(new std::vector<double>(pairs));
      size_t nn1 = y1.size();
	  size_t nn2 = y2.size();
      
      //i is a random offset from both locations, l1 and l2.
      //We need a window with lower index at offset i to
      //lie totally within each sample, y1 and y2.
      //Then i + min(l1, l2) >= 0 and so i >= -min(l1, l2).
      //And we need i + max(l1, l2) <= nn - window and so
      //i <= nn - window - max(l1, l2). (To see that nn - window
      //is the max allowable lower index, just consider
      //if nn = 3 and window = 3. Clearly, then only the first
      //index, 0, is allowable.)

	  // added by maverick:
      // since the lengths for the two traces might be different,
	  // some adjustment is added here.
      int ilower = -std::min(l1, l2);
	  int iupper = std::min(nn1 - l1, nn2 - l2) - window; 
      //int iupper = nn - std::max(l1, l2) - window;
    
      int sw = static_cast<int>(searchWindow);
      int w = static_cast<int>(window);
    
	  //Ru He comments:
	  //in order to get the rigid window on the left side of both search window, ilower <= -w, i.e., -ilower >= w
	  //in order to get the rigid window on the right side of both search window, iupper >= sw
      if (ilower > -w && iupper < sw) {
	     (*result).resize(0);
	     return result;
      }
    
	  // somehow we do not want to get a rigid pair in the windows where 
	  // we have the maximum, but maverick does not know 
	  // the specific ristrictiion.
	  // added by maverick
      RandomInSplitRange splitRand(ilower, -w, sw, iupper);

      std::vector<int>::const_iterator y1Begin = y1.begin();
      std::vector<int>::const_iterator y2Begin = y2.begin();
      for (size_t j = 0; j < pairs; ++j) {
		//Ru He comments: i will take range from [ilower, -w] U [sw, iupper]
     	int i = splitRand();
    	(*result)[j] = intCompCorr(y1Begin + l1 + i, y2Begin + l2 + i, window);
      }
      return result;
    }
};



//Ru He adds:
//Replace the original code to do the random-window-pair comparisons to avoid the possible overlap of search window and random window
class IntRandomCorSampExcludeSearch {
 public:
  /**
   * Returns at most pairs correlations at random shifts about
   * the indices given in loc in y1 and y2. randomWindow is the length
   * of the segments for which the correlations are computed.
   * Exclude the search window, ie, the regions with the highest correlation
   * from consideration.
   * (l1, l2), for which we have the maximum correlation
   */
  std::auto_ptr<std::vector<double> >
    operator()
	      (const std::vector<int>& y1,
	       const std::vector<int>& y2,
	       int l1,
	       int l2,
	       size_t searchWindow,
	       size_t pairs,
	       size_t randomWindow)
    {
      std::auto_ptr<std::vector<double> >
	  result(new std::vector<double>(pairs));
      size_t nn1 = y1.size();
	  size_t nn2 = y2.size();

	  int sw = static_cast<int>(searchWindow);
      int w = static_cast<int>(randomWindow);

	  //Ru He comments:
	  //leftShiftUB is the left shift upper bound from l1 (or l2)
	  //rightShiftLB is the right shift lower bound from l1 (or l2)
	  int leftShiftUB = -w;
	  int rightShiftLB = sw;

	  //Ru He comments:
	  //leftShiftLBTrace1 is the left shift lower bound from l1 
	  //rightShiftUBTrace1 is the right shift upper bound from l1 
	  int leftShiftLBTrace1 = -l1;
	  int rightShiftUBTrace1 = nn1 - l1 - randomWindow;

	  //leftShiftLBTrace2 is the left shift lower bound from l2 
	  //rightShiftUBTrace2 is the right shift upper bound from l2 
	  int leftShiftLBTrace2 = -l2;
	  int rightShiftUBTrace2 = nn2 - l2 - randomWindow;

	  //Ru He comments:
	  //in order to get the random window on the left side of the search window in Trace1, leftShiftLBTrace1 <= leftShiftUB ;
	  //in order to get the random window on the right side of the search window in Trace1, rightShiftLB <= rightShiftUBTrace1;
	  //in order to get the random window on the left side of the search window in Trace2, leftShiftLBTrace2 <= leftShiftUB ;
	  //in order to get the random window on the right side of the search window in Trace2, rightShiftLB <= rightShiftUBTrace2;

	  //if random window can not be sampled from Trace 1 or random window can not be sampled from Trace 2, then return empty vector
	  if((leftShiftLBTrace1 > leftShiftUB && rightShiftLB > rightShiftUBTrace1) 
		  || (leftShiftLBTrace2 > leftShiftUB && rightShiftLB > rightShiftUBTrace2 )){
	     (*result).resize(0);
	     return result;
      }

      RandomInSplitRange splitRandTrace1(leftShiftLBTrace1, leftShiftUB, rightShiftLB, rightShiftUBTrace1);
	  RandomInSplitRange splitRandTrace2(leftShiftLBTrace2, leftShiftUB, rightShiftLB, rightShiftUBTrace2);

      std::vector<int>::const_iterator y1Begin = y1.begin();
      std::vector<int>::const_iterator y2Begin = y2.begin();
	        
	  //Ru He comments:
      //shift1 is a random offset from l1, location of trace1.
	  //shift2 is a random offset from l2, location of trace2.
	  int shift1, shift2;
      for (size_t j = 0; j < pairs; ++j) {
		//Ru He comments: 
		//shift1 will take range from [leftShiftLBTrace1, leftShiftUB] U [rightShiftLB, rightShiftUBTrace1]
		//shift2 will take range from [leftShiftLBTrace2, leftShiftUB] U [rightShiftLB, rightShiftUBTrace2]
     	shift1 = splitRandTrace1();
		shift2 = splitRandTrace2();
    	(*result)[j] = intCompCorr(y1Begin + l1 + shift1, y2Begin + l2 + shift2, randomWindow);
      }
      return result;
    }
};//class IntRandomCorSampExcludeSearch 


#endif


