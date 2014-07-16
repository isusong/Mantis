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

#ifndef __CORLOC_H__
#define __CORLOC_H__

#include <iostream>
#include <cstddef>

/**
 * The max correlation found and its location.
 */
class CorLoc {
   public:
    CorLoc() : cor(-1000000.0), loc1(1000000), loc2(1000000) {}
  
    CorLoc(double cor, std::size_t loc1, std::size_t loc2)
      : cor(cor), loc1(loc1), loc2(loc2) {}
    
    /**
      * Maximum correlation found.
      */
    double cor;
      
    /**
     * Index of the max correlation in the first sample.
     */
    std::size_t loc1;
  
    /**
     * Index of the max correlation in the second sample.
     */
    std::size_t loc2;
};

bool operator==(const CorLoc& c1, const CorLoc& c2) {
    return c1.cor == c2.cor && c1.loc1 == c2.loc1 && c1.loc2 == c2.loc2;
}

std::ostream& operator<<(std::ostream& out, const CorLoc& c) {
    out << "( " << c.cor << " " << c.loc1 << " " << c.loc2 << " )";
    return out;
}

#endif

