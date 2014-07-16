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

#ifndef __LABELED_H__
#define __LABELED_H__

/**
 * Functions for reading (and validating)
 * labeled input.
 */

//These first appeared in testlevmaxcor2/readtestfile.h

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "../base/mydebug.h"


/**
 * Attempt to read the given string from in.
 * If the attempt fails, print an error message
 * and exit.
 */
void readLabel(std::istream& in, const char* expected)
{
  std::string label;
  in >> label;
  if (!in) {
    std::ostringstream what;
    what << "Got error while looking for: " << expected << std::endl;
    what << (*all_strerror(errno));
    throw std::runtime_error(what.str());
  }
  else if (label != expected) {
    std::ostringstream what;
    what << "Expected: " << expected << "; got: " << label << "\n";
    throw std::runtime_error(what.str());
  }
}


/**
 * Attempt to read a T from in. If successful, store the result
 * in result. Otherwise, print an error message and exit.
 * in -- stream to read from
 * description -- description of the object to be read
 * result -- place to store the result
 */
template<typename T>
void readValue(std::istream& in, const char* description, T& result)
{
  in >> result;
  if (!in) {
    std::ostringstream what;
    what << "Problem reading " << description << "\n";
    what << (*::all_strerror(errno));
    throw std::runtime_error(what.str());
  }
}

template<typename T>
void readLabeledValue(std::istream& in, const char* label, T& result)
{

  readLabel(in, label);
  readValue(in, label, result);
}

/**
 * Attempts to read result.size() T from in, storing each
 * in the corresponding index of result.
 */
template<typename T>
void readLabeledVector(std::istream& in, const char* label,
		       std::vector<T>& result)
{
  readLabel(in, label);
  const size_t n = result.size();
  for (size_t i = 0; i != n; ++i) {
    readValue(in, label, result[i]);
  }
}

template<typename T>
void appendLabeledVector(std::istream& in, const char* label,
			 std::vector<T>& result)
{
  readLabel(in, label);
  std::string line;
  std::getline(in, line);
  std::istringstream strIn(line);
  while (!strIn.eof()) {
    T value;
    strIn >> value;
    if (!strIn) {
      std::ostringstream what;
      what << "Problem reading " << label << " " << std::strerror(errno) << '\n'
	   << "got: <<" << line << ">>\n";
      throw std::runtime_error(what.str());
    }
    result.push_back(value);
  }
}

/**
 * label is expected _not_ to end with a trailing space.
 * Prints a trailing newline on out.
 */
template<typename T>
void printLabeledVector(std::ostream& out,
			const char* label,
			const std::vector<T>& values)
{
  out << label;
  size_t n = values.size();
  for (size_t i = 0; i != n; ++i) {
    out << ' ' << values[i];
  }
  out << '\n';
}

#endif
