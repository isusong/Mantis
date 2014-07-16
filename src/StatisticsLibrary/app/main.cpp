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

/*
 * This file is created by Maverick, serving as the program file including the 
 * main function. So the program starts here. 
 * The only change compared with the corresponding program file is 
 * that the seed parameter by function comparisonsMain is set by time
 * instead of using the default one (337, declared in comparisons_base.h). 
 */
#include "comparisons_base.h"
#include "../base/random.h"

#include <memory>
#include "PrintOneOne.h"
using namespace std;

// #include <iostream>

// #include <vector>

int main(int numArgs, char** args)
{
   unsigned long seed = 123; 
   seed = getSeedByTime();
   //seed = 123;
   

   comparisonsMain(numArgs, args,
                   auto_ptr<ComparisonPrinter>(new PrintOneOne), seed); 
   
}



