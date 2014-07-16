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

#include "FlippableCorLoc.h"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iostream>
using namespace std;

FlippableCorLoc::FlippableCorLoc(double cor, int loc1, int loc2, bool flipped)
  : ValueLoc(cor, loc1, loc2, flipped)
{
   //XXX! I'm unsure of whether to use
   //domain_error, invalid_argument, or range_error.
   //XXX! It might be better to switch to using the fpclassify
   //macro to check for infinite or NaN values.
   //if (std::isnan(cor) || std::isinf(cor) || cor < -1.0 || cor > 1.0) {
   if ( cor < -1.0 || cor > 1.0) {
        std::ostringstream what;
        what << "cor not in [-1,1]: " << cor;
        throw std::range_error(what.str());
    }
   if (loc1 < 0) {
        std::ostringstream what;
        what << "loc1 < 0: " << loc1;
        throw std::range_error(what.str());
   }
   if (loc2 < 0) {
        std::ostringstream what;
        what << "loc2 < 0: " << loc2;
       throw std::range_error(what.str());
   }
}

double FlippableCorLoc::cor() const
{
    return value();
}

/**
 * A header describing the output produced by the print method.
 * Suitable for use as a header in a file meant to be read as
 * a data frame in R.
 */
std::string FlippableCorLoc::header()
{
    return "loc1 loc2 flipped cor";
}
