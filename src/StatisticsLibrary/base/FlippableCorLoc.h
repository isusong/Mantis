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

#ifndef __FLIPPABLECORLOC_H__
#define __FLIPPABLECORLOC_H__

#include "ValueLoc.h"
#include <iosfwd>
#include <string>

/**
 * Class representing the result of code that
 * searches through two sequences for the max
 * correlation, also examining correlations
 * between the first and the reverse of the
 * second.
 */
class FlippableCorLoc : public ValueLoc {
 public:

  FlippableCorLoc(double cor, int loc1, int loc2, bool flipped=false);

  double cor() const;

  /**
   * A header describing the output produced by the print method.
   * Suitable for use as a header in a file meant to be read as
   * a data frame in R.
   */
  static std::string header();
};

#endif

