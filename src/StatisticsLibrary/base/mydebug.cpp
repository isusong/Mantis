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

#include "mydebug.h"
#include <cstdlib>
#include <cstring>

using std::strerror;

/* Call strerror_r and get the full error message. Allocate memory for the
 * entire string with malloc. Return a pointer to string. 
 * If malloc fails, return a pointer to an empty string
 * 
 * this function replace strerror, so that there is no warning 
 * during compiling. 
 * ---added by Maverick 
 */

std::auto_ptr<std::string> all_strerror(int n) {
	//Laura:  This isn't cross platform, so we will just let Visual Studio whine about strerror.
	//It'll be just fine, since std::string will guarantee a copy of the string.
    //char *s;
    //size_t size;
    //size = 1024;
    //s = (char *) malloc(size);
    //if (s == NULL)  return std::auto_ptr<std::string>(new std::string());

    //while (strerror_s(s, size, n) == 0 && errno == ERANGE) {
    //    size += 2;
    //    s = (char * ) realloc(s, size);
    //    if (s == NULL) return std::auto_ptr<std::string>(new std::string());
    //}
	//std::auto_ptr<std::string> perrstr(new std::string(s));
	//free(s);
	std::auto_ptr<std::string> perrstr(new std::string(strerror(n)));
	return perrstr;
}
