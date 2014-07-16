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

#ifndef __COMPARISONS_BASE_H__
#define __COMPARISONS_BASE_H__

#include <memory>
class ComparisonPrinter;

/**
 * numArgs == 2
 * args[0] == name of application
 * args[1] == location of parameter file
 * The parameter file is expected to have the following fields:
 *        data.dir: <location of the data files>\n"
 *        search.window: <search window width to use>\n"
 *        val.window: <validation window width>\n"
 *        num.rigidpairs: <number of pairs to pick with the same shift for the pair>\n"
 *        num.randompairs: <number of pairs to pick with different shift for the pair> \n"
 *        output.file: <file in which to save results>\n"; 
 * --- comments modified by Maverick 
 */
void comparisonsMain(int numArgs,
                     char** args,
                     std::auto_ptr<ComparisonPrinter> printComp,
                     unsigned long seed = 337);

#endif


