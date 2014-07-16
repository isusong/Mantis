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

#ifndef __DISTLOC_H__
#define __DISTLOC_H__

#include <iosfwd>

/**
 * Class representing the value of a function of
 * two sequences at the given pair of
 * locations.
 */
class ValueLoc {
  //I had planned to call this DistLoc for "distance location"
  //but it occurred to me that the correlation coefficient
  //is not a measure of distance (at least without transformation).

 public:
  ValueLoc(double value, int loc1, int loc2, bool flipped=false);

  /**
   * The distance between the two sequences at the given
   * point.
   */
  double value() const;

  /**
   * Index into the first sequence.
   */
  int loc1() const;

  /**
   * Index into the second sequence.
   * If flipped() is true, is an index into
   * the reverse of the second sequence. Ie,
   * for a sequence of length n, if flipped() == true,
   * loc2() == 0 corresponds to index n-1.
   */
  int loc2() const;

  /**
   * Indicates whether loc2 is in the coordinates of the
   * reverse of the second sequence.
   */
  bool flipped() const;

 private:
  double _value;
  int _loc1;
  int _loc2;
  bool _flipped;
};


/**
 * Prints loc1 <space> loc2 <space> flipped <space> value
 * to out with no newline.
 * 1 is used to represent true, 0 false.
 */
std::ostream& operator<<(std::ostream& out, const ValueLoc& v);

#endif
