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

#ifndef __GETCURRENTTIME_H__
#define __GETCURRENTTIME_H__

#include <time.h>
#include <memory> /* for auto_ptr */

/**
 * Returns a pointer to a string representing the local time.
 * Intended here to serve as a time stamp.
 */


std::auto_ptr<std::string> getCurrentTime() {
	//Laura: Converted from Visual Studio to g++
	//char buf[50];
	time_t t;
	time(&t);
	//ctime_s(buf, 50, &t);
	//return std::auto_ptr<std::string>(new std::string(buf)); 
	return std::auto_ptr<std::string>(new std::string(ctime(&t))); 
}

#endif

