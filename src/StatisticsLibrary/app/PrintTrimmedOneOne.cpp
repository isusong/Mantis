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

#include "PrintTrimmedOneOne.h"
#include "../base/flipcorrelation.h"
#include "../base/FlippableCorLoc.h"
#include "../base/intcorrelation.h"
#include "../base/intnolev_functors.h"
#include "../base/random.h"
//#include "../base/timing.h"
#include "../io/readtrace.h"
#include "../base/stats.h"
#include <iostream>
//#include "MarkName.h"
#include <memory>
#include <string>
#include <vector>
using namespace std;

/* 
// commented out by Maverick 
auto_ptr<vector<int> > readTrace(const string& dataDir, const MarkName& m, int traceNum)
{
  return readTrace(dataDir.c_str(), m.tip(), m.side(), m.angle(), m.specimen(), traceNum);
}
*/
void trimVector(vector<int>& v, int trim)
{
     //XXX! There's no particular reason for this not to be generic
     //except perhaps to make the executable smaller.
    
     const int finalLength = v.size() - 2 * trim;
     v.erase(v.begin(), v.begin() + trim);
     v.resize(finalLength);
}

PrintTrimmedOneOne::PrintTrimmedOneOne(int trim) : _trim(trim) {}

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
void PrintTrimmedOneOne::printComparison(const std::string& dataDir,
                                         const MarkName& m1,
                                         const MarkName& m2,
                                         int searchWindow,
                                         int valWindow,
                                         int numPairs,
                                         std::ostream& out) const
{
  RandomUpTo randUpTo;
  const int traceNum1 = 2 + randUpTo(4); //junk now
  const int traceNum2 = 2 + randUpTo(4); //junk now
  auto_ptr<vector<int> > trace1 = readTrace(dataDir, m1, traceNum1);
  auto_ptr<vector<int> > trace2 = readTrace(dataDir, m2, traceNum2);

  trimVector(*trace1, _trim);  //remove trim from beginning and end
  trimVector(*trace2, _trim);

  const int length = trace1->size(); //might want for trace2

  //XXX! We're currently running without checking for flips.
  FlippableCorLoc c = maxCorWithFlips(trace1->begin(),
                            trace2->begin(),
                            length, //add length for trace2
                            searchWindow,
                            false);

  IntRigidCorSampExcludeSearch compRigidCor;
  auto_ptr<vector<double> > rigidCor
    = compRigidCor(*trace1, *trace2, c.loc1(), c.loc2(),
               searchWindow, numPairs, valWindow);

  //The following was taken from PrintManyMany.cpp.
  //I'm using it temporarily while we stop checking for
  //flips... once we put that code back, we can go
  //back to using the functor from
  std::vector<double> nonrigidCor(numPairs);
    const int maxShiftPlus1 = length - valWindow + 1;
  for (int i = 0; i != numPairs; ++i) {
      std::vector<int>::iterator y1Begin = trace1->begin() + randUpTo(maxShiftPlus1);
      const int shift2 = randUpTo(maxShiftPlus1);
      //XXX! We're currently running without looking for flips.
      nonrigidCor[i] = intCompCorr(y1Begin, trace2->begin() + shift2, valWindow);
  }
  out << m1 << ' ' << traceNum1 << ' '
      << m2 << ' ' << traceNum2 << ' ' << c << ' '
      << t1Statistic(*rigidCor, nonrigidCor);

}
*/

/* added by maverick to replace printComparison
 *
 */
void PrintTrimmedOneOne::doPairComparison(const std::string& dataDir,
                                          const std::string& file1,
                                          const std::string& file2,
                                          int searchWindow,
                                          int valWindow,
                                          int numRigidPairs, 
                                          int numRandomPairs, 
                                          std::ostream& out) const { 
     

    //Ru He update:
    //out << file1 << "," << file2 << ",  ";
	out << file1 << " \t " << file2 << "             \t ";
	//

	//cout << "test";

	//system("pause");

    auto_ptr<vector<int> > trace1 = readTrace(dataDir.c_str(), file1.c_str()); 
    auto_ptr<vector<int> > trace2 = readTrace(dataDir.c_str(), file2.c_str());

    trimVector(*trace1, _trim);  //remove trim from beginning and end 
    trimVector(*trace2, _trim);

    const int length1 = trace1->size(); 
    const int length2 = trace2->size();

	int tmp = 0;
	//cout << "length1=" << length1 << endl; 
	//cout << "length2=" << length2 << endl;

    //XXX! We're currently running without checking for flips.
	//Ru He comments:
	//Once searchWindow is fixed, the returned c.loc1(), c.loc2() in c object should be unique
	//Ru He Test:
	//ofstream out_debug_maxCorWithFlips("T:\\debug_maxCorWithFlips.txt");
	//out_debug_maxCorWithFlips << "loc1" << " \t " << "loc2" << " \t " << "max_corr" << endl; 
	FlippableCorLoc c = maxCorWithFlips(trace1->begin(),
                                        trace2->begin(),
                                        length1, 
                                        length2,
                                        searchWindow,
                                        false);
	//out_debug_maxCorWithFlips << c.loc1() << " \t " << c.loc2() << " \t " << c.cor() << endl; 

	/*
	for(int i = 0; i < 10; i++){
		c = maxCorWithFlips(trace1->begin(),
                                        trace2->begin(),
                                        length1, 
                                        length2,
                                        searchWindow,
                                        false);
		out_debug_maxCorWithFlips << c.loc1() << " \t " << c.loc2() << " \t " << c.cor() << endl; 


	}
	*/	
	//

#if 0  // another way to comment out some code 
    cout << c.loc1() << "-" << c.loc2() << endl;
    cout << "dddd\n";
#endif
  /*
   * comments add by Maverick 
   * In the following code, we first get rigid pairs correlation.  
   * In other words, the correlation for two windows that has the same shifts with
   * respect to the position of maximum correlation. 
   * For the random pairs correlation, the shifts of two pairs are different  
   * w.r.t. to the position of maximum correlation. 
   */

       
    //Ru He comments:
	//In order to get sample mean and standard devation of T for any fixed vector (searchWindow, valWindow, numRigidPairs, numRandomPairs), 
	//we could add the loop in the following:
	//Ru He update:
	//For calculate the sample mean and var of T and output them
	//
	const int T_sample_size = 200;
	vector<double> T_vector;
	double temp_T;
	for(int i = 0; i < T_sample_size; i++){
	//


   IntRigidCorSampExcludeSearch compRigidCor;
   auto_ptr<vector<double> > rigidCor = compRigidCor(*trace1, *trace2, c.loc1(), c.loc2(),
                                                    searchWindow, numRigidPairs, valWindow);

   //Ru He comments:
   //The original code is not correct since it can allow the overlap of search window and random window, (which is confirmed from Amy).
   //Comments out the original code.
   
   /*
   //The following was taken from PrintManyMany.cpp.
   //I'm using it temporarily while we stop checking for
   //flips... once we put that code back, we can go
   //back to using the functor from
#if 0
   for (int it = 0; it < 5; it++) {
#endif
   std::vector<double> RandomCor(numRandomPairs);

   const int maxShiftPlus1 = length1 - valWindow + 1;
   const int maxShiftPlus2 = length2 - valWindow + 1;
   for (int i = 0; i != numRandomPairs; ++i) {
	   //Ru He comments:
	   //trace1->begin() + randomUpTo(maxShiftPlus1) seems different from the description of algo
	   //Yes. confirmed from Amy that the original code is not correct since it can have overlap of search window and random window

       std::vector<int>::iterator y1Begin = trace1->begin() + randomUpTo(maxShiftPlus1);
       const int shift2 = randomUpTo(maxShiftPlus2);
       //XXX! We're currently running without looking for flips.
       RandomCor[i] = intCompCorr(y1Begin, trace2->begin() + shift2, valWindow);
   }
   */

   //Ru He updates:
   IntRandomCorSampExcludeSearch compRandomCor;
   auto_ptr<vector<double> > randomCor = compRandomCor(*trace1, *trace2, c.loc1(), c.loc2(),
                                                    searchWindow, numRandomPairs, valWindow);
   //

   //Ru He update:
   //out << c.loc1() << "	" << c.loc2() << "	" << "	" << c.cor() << "	" << t1Statistic(*rigidCor, RandomCor) << "\n";
   temp_T = t1Statistic(*rigidCor, *randomCor);
   T_vector.push_back(temp_T);
   //

#if 0
   }
#endif
   /*
   // commented out by Maverick 
   out << m1 << ' ' << traceNum1 << ' '
      << m2 << ' ' << traceNum2 << ' ' << c << ' '
      << t1Statistic(*rigidCor, RandomCor);*/

    //Ru He update:
	}//for(int i = 0; i < T_sample_size; i++)

	double T_mean, T_var;
	meanAndVar(T_vector, T_mean, T_var);
	T_vector.clear();
	out << c.loc1() << " \t " << c.loc2() << " \t " << c.cor() << " \t " 
		<< T_sample_size << "                   \t " << T_mean  << " \t " << T_var << "\n";
	//

}

std::string PrintTrimmedOneOne::name() const
{
    return "PrintTrimmedOneOne";
}


