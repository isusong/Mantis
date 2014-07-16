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

#include <io.h>
#include "dirlist.h"
#include <string>

/* list only files in a directory withought;
 * folders are ignored.
 * the codes are copied from the internet and then modfied by maverick.
 *
 * sdir: the folder name we want to list.
 * p_vectData: pointer to a vector in which we would holder the file names
 * return: total number of files found.
 */
int listDirfun(const char * sdir, vector<string> *p_vectData) { 
    string str, strDir;
    struct _finddata_t c_file;
    long hFile;
	int  total;
	total = 0;

    // Find first file in current directory
    str = sdir + (string) "\\*";
    if ( (hFile = _findfirst( str.c_str(), &c_file )) == -1L ){
        fprintf(stderr, "Error opening %s!\n", str.c_str());
        return -1;
    }

    do {
        //skip if find . and ..
        if ((strcmp(c_file.name, ".") == 0 ||  strcmp(c_file.name, "..") == 0)) {
            continue;
        }

		//also skip if it is a folder.
		if (c_file.attrib & _A_SUBDIR) continue;

        //str = "";
 
		/* the following is commented out because here we only concern files */
		/*
        if (c_file.attrib & _A_SUBDIR){
            strDir = sdir + (string) "\\" + (string) c_file.name;
            str += "D: " + strDir;
            m_vectData.push_back(str);
            listDir(strDir.c_str(), count+1);
            continue;
        } */
        //join given path and file name
        // str += (string) sdir + (string) "\\" + (string) c_file.name;
        p_vectData -> push_back((string) c_file.name); 
		total += 1;
    } while(_findnext( hFile, &c_file ) == 0);

   _findclose( hFile );
   return total;
}

