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


#ifndef __CONVERTTRACETOINT_H__
#define __CONVERTTRACETOINT_H__

#include <cmath>
#include <memory>
#include <vector>

/**
 * A functor that takes a vector of doubles,
 * multiplies each by 100, then rounds
 * each to the nearest integer. Ie,
 * result[i] = round(100 * trace[i]).
 *
 * This seems to correspond to converting
 * the elements of a trace as represented
 * in ASCII back to the original format
 * used by the profilometer in its PIP
 * format.
 */
class ConvertTraceToInt {
 public:

  std::auto_ptr<std::vector<int> >
    operator()(const std::vector<double>& trace)
    {
      const size_t n = trace.size();
      std::auto_ptr<std::vector<int> > result(new std::vector<int>(n));
      for (size_t i = 0; i != n; ++i) {
	(*result)[i] = static_cast<int>(int(100 * trace[i] + 0.5));
      }
      return result;
    }
};

#endif

