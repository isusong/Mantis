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

#include "ValueLoc.h"
#include <iostream>

ValueLoc::ValueLoc(double value, int loc1, int loc2, bool flipped)
  : _value(value), _loc1(loc1), _loc2(loc2), _flipped(flipped) 
{}

/**
 * The value of the function of the two sequences at the given
 * point.
 */
double ValueLoc::value() const
{
  return _value;
}

/**
 * Index into the first sequence.
 */
int ValueLoc::loc1() const
{
  return _loc1;
}

/**
 * Index into the second sequence.
 * If flipped() is true, is an index into
 * the reverse of the second sequence. Ie,
 * for a sequence of length n, if flipped() == true,
 * loc2() == 0 corresponds to index n-1.
 */
int ValueLoc::loc2() const
{
  return _loc2;
}

/**
 * Indicates whether loc2 is in the coordinates of the
 * reverse of the second sequence.
 */
bool ValueLoc::flipped() const
{
  return _flipped;
}

/**
 * Prints loc1 <space> loc2 <space> flipped <space> value
 * to out with no newline.
 */
std::ostream& operator<<(std::ostream& out, const ValueLoc& v)
{
  using namespace std;
  out << v.loc1() << ' ' << v.loc2() << ' '
      << (v.flipped() ? 1 : 0) << ' ' << v.value();
  return out;
}
