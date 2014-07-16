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


#include <algorithm>
#include "../base/FlippableCorLoc.h"
#include "../base/getcurrenttime.h"
#include "../base/random.h"
#include "comparisons_base.h"
#include "ComparisonPrinter.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "../io/labeled.h"
#include <iostream>
//#include "samplecomparisons.h"
#include <set>
#include <utility>
#include "../io/dirlist.h"
#include "../base/mydebug.h"

using namespace std;

/* actually, this is the mail function in a sense it is almost the only function 
 * called by function main. 
 */
void comparisonsMain(int numArgs,
             char** args,
             auto_ptr<ComparisonPrinter> printComp,
             unsigned long seed)
{

    if (numArgs != 2) {
		// cerr << "Time:" << *(::getCurrentTime()) <<  endl;
		cerr << "Usage: " << args[0] << " <param file>\n";		
        cerr << " The parameter file is expected to have the following fields:\n"
                "  data.dir: <location of the data files>\n"
                //"  num.matches: <number of matches to collect>\n"
                //"  num.nonmatches: <number of nonmatches to collect>\n"
                "  search.window: <search window width to use>\n"
                "  val.window: <validation window width>\n"
                "  num.rigidpairs: <number of pairs to pick with the same shift for the pair>\n"
                "  num.randompairs: <number of pairs to pick with different shift for the pair> \n"
                "  output.file: <file in which to save results>\n";
#ifdef MYDEBUG
       perror("Any key to quit.\n");
       system("pause"); 
#endif
       exit(1);
    }

    ifstream param(args[1]);
    if (!param) {
       cerr << "Failed to open param file: " << args[1] << '\n';
       perror("io-error");
#ifdef MYDEBUG
       perror("Any key to quit.\n");
       system("pause"); 
#endif
       exit(1);
    }
    /*
   string dataDir("P:\\LINKS\\PROJECTS\\Forensics\\yatrackcorr\\testraw_data");
   // server for debug by Maverick 

    */
    string dataDir;
    int searchWindow; //= 100;
    int valWindow; //= 50;
    int numRigidPairs; // = 10; 
    int numRandomPairs; // = 12 ;
    string outputFile;

    try {
    
        // cout << "here\n";  // used for debug by maverick
        readLabeledValue(param, "data.dir:", dataDir);
  
        // cout << dataDir << '\n';
  
        /*
        int numMatches;
        readLabeledValue(param, "num.matches:", numMatches);
        int numNonmatches;
        readLabeledValue(param, "num.nonmatches:", numNonmatches);
        */
        
        readLabeledValue(param, "search.window:", searchWindow);
        readLabeledValue(param, "val.window:", valWindow);
        
        readLabeledValue(param, "num.rigidpairs:", numRigidPairs);
        readLabeledValue(param, "num.randompairs:", numRandomPairs);
        
        readLabeledValue(param, "output.file:", outputFile);
        param.close();
    } catch (runtime_error err) {
        cout << err.what() << "\n";
#ifdef MYDEBUG
        perror("ERROR IN READING THE PARAMETER FILE.\n");
        perror("Any key to quit.\n");
        system("pause"); 
        exit(1);
#endif
    }
    ofstream out(outputFile.c_str()); 
    /* 
    ofstream out("testout.txt"); // serve for debug
    */
    /*
    // commented out by Maverick 
    out << MarkName::header(1) << " trace1 "
        << MarkName::header(2) << " trace2 "
        << FlippableCorLoc::header()
        << " t1\n";
     */
    out << "# OUTPUT file by program " << args[0] << " with parameter file " << args[1] << "\n\n";
	out << "#start: " << *(::getCurrentTime()) << endl;
    out << "#data.dir: " << dataDir << endl;
    /*
    out << "#num.matches: " << numMatches << '\n';
    out << "#num.nonmatches: " << numNonmatches << '\n';
    */
    out << "#search.window: " << searchWindow << '\n';
    out << "#val.window: " << valWindow << '\n';
    out << "#num.rigidpairs: " << numRigidPairs << '\n';
    out << "#num.randompairs: " << numRandomPairs << '\n';
    out << "#alg.name: " << printComp->name() << '\n';
    out << "#seed: " << seed << "\n\n";

    setSeed(seed);

    out.precision(16); //setting decimal precision for all relevant output (r and T1)

	//Ru He update:
	//For (1) the better output format, (2) add mean of T and var of T
    //out << "file1, file2, loc1, loc2, flipped maxcorr, test_stat \n"; 
	out << "file1        \t file2                        \t loc1 \t loc2 \t flipped_maxcorr \t T_sample_size: \t T_mean            \t T_var \n"; 
  

    /*
    //We have to grab both of these at the same time, after setting the seed
    //and prior to making any other calls to the RNG to ensure the same
    //set of comparisons are chosen for both the many-many and one-one
    //algs since otherwise there'd be a different number of calls
    //to the RNG between hence different state for each.
    
    // commented out by Maverick 

    vector<ComparisonId> matches; // = sampleMatchesByAngle(numMatches);
    vector<ComparisonId> nonmatches;// = sampleNonmatchesByAngle(numNonmatches);
    */

   // To read all trace files for which we want to do pairwise comparison 
   // In the newly implemented codes, we only read trace files in a folder specified by
   // the parameter configuration file. 
   // ---comments added by Maverick 
  
   vector<string> alltracefiles;

   int total = listDirfun(dataDir.c_str(), &alltracefiles);
   for (int i = 0; i < total - 1; i++)  {
       for (int j=i+1; j < total; j++) {           
           printComp -> doPairComparison(dataDir, alltracefiles[i], alltracefiles[j], 
                                         searchWindow, valWindow, numRigidPairs, numRandomPairs, out);
       }
   }

   /*
   // commented out by Maverick 
   for (int i = 0; i != numMatches; ++i) {
       ComparisonId id = matches[i];
       printComp->printComparison(dataDir,
                                  id.first,
                                 id.second,
                                 searchWindow,
                                 numPairs,
                                 valWindow,
                                 out);
       out << '\n';
   }
 
   out << "\n#nonmatches:\n";
   for (int i = 0; i != numNonmatches; ++i) {
        ComparisonId id = nonmatches[i];
    
        printComp->printComparison(dataDir,
                                  id.first,
                                  id.second,
                                  searchWindow,
                                  numPairs,
                                  valWindow,
                                  out);
        out << '\n';
   }
   */

   out << "\n#finish: " << *(::getCurrentTime()) << endl;

   system("pause");
}
