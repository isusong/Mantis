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
 * Author: Song Zhang (song@iastate.edu)
 */

#pragma once
#include<iostream>
using namespace std;

class CFilterImage
{
public:
	CFilterImage(int width, int height);
	~CFilterImage(void);
public:
	void MedianFilterFloat(float *input, float *output, const int fsize, unsigned char *mask = NULL);
	void MedianFilterU8(unsigned char *input, unsigned char *output, const int fsize, unsigned char *mask = NULL);
	void GaussianFilterFloat(float *input, float *output, const int fsize, unsigned char *mask = NULL);
	void GaussianFilterU8(unsigned char *input, unsigned char *output, const int fsize, unsigned char *mask = NULL);
	void FillHolesFloat(float *zData, unsigned char *maskData, const int hsize = 10);
private:
	void ComputeGaussianFilter(float *&filter, int hsize);

	const int m_imageWidth, m_imageHeight, m_imageSize;
};
