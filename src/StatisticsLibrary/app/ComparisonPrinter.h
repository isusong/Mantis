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

#ifndef __ComparisonPrinter_h__
#define __ComparisonPrinter_h__

#include <iosfwd>
//#include "MarkName.h"
#include <string>

class ComparisonPrinter {
      public:
      virtual ~ComparisonPrinter() {}

      /**
       * The following is to be printed:
       * (m1) <trace1> (m2) <trace2> (FlippableCorLoc) <t1>
       * where trace1 and trace2 are the traces in the
       * respective marks in which the best match
       * was found, FlippableCorLoc is the print
       * representation of the object representing the match,
       * and t1 is the T1 stat computed in the validation
       * step.
       *    Additional info may be printed on lines preceding
       * the above provided each line of it is preceded by
       * a hash mark -- #.
       */
      
      /*
      virtual void printComparison(const std::string& dataDir,
                                  const MarkName& m1,
                                  const MarkName& m2,
                                  int searchWindow,
                                  int numPairs,
                                  int valWindow,
                                  std::ostream& out) const = 0; */

      // this function is added by maverick
      // the original function `printComparison' is deleted.  
      // The function would compute a statistic for the 
      // two traces stored in file1 and file2 respectively in folder dataDir. 
      // The output to the output file is in format 
      // file1~file2~  <spalce> maxlocal_info <spalce>  test_statistic 
      // maxlocal_info includes three items: 
      //   loc1 <spalce> loca2  <spalce> flipped <space> max_corr 
      // as defined in file `ValueLoc.cpp' 
      virtual void doPairComparison(const std::string& dataDir,
                               const std::string& file1,
                               const std::string& file2,
                               int searchWindow,
                               int valWindow,
                               int numRigidPairs, 
                               int numRandomPairs, 
                               std::ostream& out) const = 0; 
                  
      virtual std::string name() const = 0;
};

#endif
