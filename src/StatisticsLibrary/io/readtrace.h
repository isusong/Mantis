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

#ifndef __READTRACE_H__
#define __READTRACE_H__

#include "converttracetoint.h"
#include <cstring>
#include <iostream>
#include <memory>
#include "readinto.h"
#include <string>
#include <sstream>
#include <vector>

std::auto_ptr<std::vector<int> > readTrace(const std::string& file) {
  ConvertTraceToInt trans;
  
  std::vector<double> y;
  y.reserve(9600);

  readFromFileInto(file.c_str(), y);

  //Ru He comments:
  //change double array into int array
  return trans(y);
}

// this funciton is added by maverick 
// but it calls another function written before 
std::auto_ptr<std::vector<int> > readTrace(const char* dataDir, const char *fname) {
	std::ostringstream file;
	int len = strlen(dataDir);
	const char* sep = "";
	// since now (oct 22, 2010), we run the code on windows,
	// the seprator file file path is assumed to be '\'
	// 
	// if there is not a '\' at the end of path, put one
	// when creating the full path for the file
	if (len > 0 && dataDir[len - 1] != '\\')  { sep = "\\"; }
		
    file << dataDir << sep << fname;
    return readTrace(file.str());
}

/**
 * prefix is to be of the form: <tip><side><angle><specimen>
 */

std::auto_ptr<std::vector<int> > readTrace(const char* dataDir,
					   const char* prefix,
					   int trace,
					   const char* suffix = "_y.txt")
{

  std::ostringstream file;

  //XXX! This code depends on "/" being the
  //directory separator.
  int len = strlen(dataDir);
  const char* sep = "";
  if (len > 0 && dataDir[len - 1] != '/') {
    sep = "/";
  }

  file << dataDir << sep << prefix << trace << suffix;
  return readTrace(file.str());

  /*
    if (y.size() != 9600) {
    std::cerr << "Warning: Expected 9600 points in " << file.str()
    << "; found " << y.size() << '\n';
    }
  */

}


std::auto_ptr<std::vector<int> > readTrace(const char* dataDir,
					   int tip,
					   char side,
					   int angle,
					   int specimen,
					   int trace,
					   const char* suffix = "_y.txt")
{

  std::ostringstream prefix;
  prefix << tip << side << angle << specimen;
  return readTrace(dataDir, prefix.str().c_str(), trace, suffix);
}

#endif
