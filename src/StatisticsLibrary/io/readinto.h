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

#ifndef __READINTO_H__
#define __READINTO_H__

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "../base/mydebug.h"


/**
 * Reads values of type T from in until the stream is exhausted.
 * Appends each value to the end of values.
 *
 * in -- the stream from which to read
 * name -- name of the stream, used in error messages.
 * values -- vector to append to
 */
template<typename T>
void readFromInto(std::istream& in, const char* name, std::vector<T>& values)
{  
	using namespace std;
	T x;
	int lineNum = 1;
	while (!in.eof()) {
		//Skip space characters
		while (in && !in.eof() && isspace(in.peek())) {
			if (in.get() == '\n') { //This would miss lines on Classic Mac OS
				++lineNum;
			}
		}
	
		if (in.eof() || EOF == in.peek()) { //Allow a final empty line/trailing newline
			//XXX! Why the hell is it not always the case that in.eof() <=> EOF == in.peek()?
			break;
		}
		else if (!in) {
			std::ostringstream what;
			what << name << ", line " << lineNum << " (skipping space): "
				<< *(::all_strerror(errno)); 
			throw std::runtime_error(what.str());
		}

		in >> x;
		//std::cout << x << "  "; 
		if (!in) {
			std::ostringstream what;
			what << name << ", line " << lineNum << " (reading value): "
				<< *(::all_strerror(errno)); 
			throw std::runtime_error(what.str());
		}
		values.push_back(x);
	}
	
	//XXX! Should we add code to ensure that in is closed, even if an exception is thrown?
}

/**
 * Reads values of type T from the file named filename
 * until that file is exhausted. Each value is appended
 * to the end of values.
 */
template<typename T>
void readFromFileInto(const char* filename, std::vector<T>& values)
{
  using namespace std;
  std::ifstream in(filename);
  if (!in.is_open()) {
    std::ostringstream what;
	what << "Problem opening " << filename << ": " << *(::all_strerror(errno)); 
    throw std::runtime_error(what.str());
  }
  
  readFromInto(in, filename, values);
  in.close();
}


#endif

