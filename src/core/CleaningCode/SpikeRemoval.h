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
 *
 * Authors: Song Zhang (song@iastate.edu), Laura Ekstrand (ldmil@iastate.edu)
 */

#pragma once
#include <iostream>
#include <vector>

using namespace std;

/**
 * Class for using polynomial fitting
 * to remove spikes. Modifies the
 * mask and a small sample of the depth values.
 *
 * Laura: Made LeastSquareFit publically available.
 *
 * @author Song Zhang
 */

class IProgress;

class CSpikeRemoval
{
public:
	CSpikeRemoval(void);
	~CSpikeRemoval(void);
public:
	///isVerify=false modifies depth values, isVerify=true does not.
	void PolyLineRemoval(const float *xData, const float *yData, float *zData, 
		unsigned char *maskData, float thresh, float thresh2, int poly_order, 
        int width, int height, bool isVerify = true, IProgress *prog=NULL);

	///Quickly perform linear regression up to 19th order.
	/**
	 * Coef will be of the form:
	 * coef[0] = a0, coef[1] = a1, coef[2] = a2, etc. for
	 * y_hat = a0 + a1*x + a2*x^2 + .... fitting.
	 *
	 * Warning: coef must be empty when passed in for correct results.
	 */
	void LeastSquareFit(const vector<float> x, const vector<float> y, const int m, vector<float> &coef);

private:
	int m_imageWidth;
	int m_imageHeight;
};
