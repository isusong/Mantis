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

#ifndef __MYDEBUG_H__
#define __MYDEBUG_H__

#include <memory>
#include <string>
/* Define Maverick's debug option. 
 * If below line, ie, #define MYDEBUG, are commented out, some debug code would not be compiled.
 */

/*
 * An example of using this macro:
 * #IFDEF MYDEBUG  
 * cout << "a=" << a << "\n"; 
 * #ENDIF 
 */ 


#define MYDEBUG

/////////////////////////////////////////////////////////
/* other functions used for finding error when running 
 * not necessarily only for the debuging
 */

/* Call strerror_r and get the full error message. Allocate memory for the
 * entire string with malloc. Return a pointer to string. 
 * If malloc fails, return a pointer to an empty string
 * 
 * this function replace strerror, so that there is no warning 
 * during compiling. 
 * ---added by Maverick 
 */
std::auto_ptr<std::string> all_strerror(int n); 

#endif
