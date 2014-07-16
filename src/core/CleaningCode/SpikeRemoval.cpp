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

#include "SpikeRemoval.h"
#include <cstdio>
#include <cstring>
#include <cmath>

#include <iostream>
using std::cout;
using std::endl;

CSpikeRemoval::CSpikeRemoval(void)
{
}

CSpikeRemoval::~CSpikeRemoval(void)
{
}

void CSpikeRemoval::PolyLineRemoval(const float *xData, const float *yData,
	float *zData, unsigned char *maskData,
	float thresh1, float thresh2, int poly_order, 
	int width, int height, bool isVerfiy)
{
	m_imageWidth = width;
	m_imageHeight = height;
	unsigned char *lmask = new unsigned char[width * height];
	memcpy(lmask, maskData, sizeof(lmask[0])*width * height);

	vector<float>x,z;
	vector<float>coeff;

	// fit the data points line by line using polynomials 
	// remove those points beyond the threholds
	
	//FILE *fp;
	//fopen_s(&fp, "col2000.txt", "w");
	//for (int i = 0; i < m_imageHeight; i++)
	//{
	//	int id = i*m_imageWidth + 1999;
	//	if (maskData[id])
	//	{
	//		fprintf(fp, "%.2f\t%.2f\n", xyzData[id * 3+1],xyzData[id * 3 + 2]); 
	//	}
	//}
	//fclose (fp);

	// vertical lines
	for (int i = 0;  i <  m_imageWidth; i++)
	{
		x.clear();
		z.clear();
		coeff.clear();
		for (int j = 0; j < m_imageHeight; j++)
		{
			int id = j * m_imageWidth + i;
			if (maskData[id])// put y and z data into a vector
			{
				x.push_back(yData[j]);
				z.push_back(zData[id]);
			}
		}
		if (x.size() > 20)
		{
			LeastSquareFit(x, z, poly_order, coeff);// fit the polynomial curve

			int kk = 0;
			for (int j = 0; j < m_imageHeight; j++)
			{
				int id = j * m_imageWidth + i;			
				if (maskData[id])
				{
					
					//float x1 = x[kk];
					//float fz = 0; //coeff[0];
					//for (size_t k = 0; k < coeff.size(); k++)
					//{
					//	fz += coeff[k] * pow(x1, (float)k); // compute the data using the fitted polynomials
					//}

					float x1 = x[kk];
					float fz = coeff[0];
					float xx = x1;
					for (size_t k = 1; k < coeff.size(); k++)
					{
						fz += coeff[k] * xx;
						xx *= x1;
					}

					if (fabs(fz - z[kk]) > thresh2) // if the data point is bad, mask it out
					{
						maskData[id] = 0;
					}
					kk ++;
				}
			}			
		}
		else // if the data points are too small, get rid of all of them
		{
			for (int j = 0; j < m_imageHeight; j++)
			{
				int id = j * m_imageWidth + i;
				maskData[id] = 0;
			}
		}
	}


	
// Horizontal lines
	for (int i = 0;  i <  m_imageHeight; i++)
	{
		x.clear();
		z.clear();
		coeff.clear();
		for (int j = 0; j < m_imageWidth; j++)
		{
			int id = i * m_imageWidth + j;
			if (maskData[id]) // put x and z data into a vector
			{
				x.push_back(xData[j]);
				z.push_back(zData[id]);
			}
		}
		if (x.size() > 20)
		{
			LeastSquareFit(x, z, poly_order, coeff); // fit the polynomial curve
			int kk = 0;
			for (int j = 0; j < m_imageWidth; j++)
			{
				int id = i * m_imageWidth + j;			
				if (maskData[id])
				{
					//float x1 = x[kk];
					//float fz = 0;
					//for (size_t k = 0; k < coeff.size(); k++)
					//{
					//	fz += coeff[k] * pow(x1, (float)k); // compute the data using the fitted polynomials
					//}
					float x1 = x[kk];
					float fz = coeff[0];
					float xx = x1;
					for (size_t k = 1; k < coeff.size(); k++)
					{
						fz += coeff[k] * xx;
						xx *= x1;
					}
					
					if (fabs(fz - z[kk]) > thresh1) // if the data point is bad, mask them out
					{
						maskData[id] = 0;
					}
					kk ++;
				}
			}			
		}
		else // if the data points are too small, get rid of all of them
		{
			//memset(&lmask[i*m_imageWidth], 0, sizeof(lmask[0])*m_imageHeight);
			for (int j = 0; j < m_imageWidth; j++)
			{
				int id = i * m_imageWidth + j;
				maskData[id] = 0;
			}
		}
	}
	

	// fill in those removed points vertically if not for testing
	if (!isVerfiy)
	{
		// vertical lines
		for (int i = 0;  i <  m_imageWidth; i++)
		{
			x.clear();
			z.clear();
			coeff.clear();
			for (int j = 0; j < m_imageHeight; j++)
			{
				int id = j * m_imageWidth + i;
				if (maskData[id])// put y and z data into a vector
				{
					x.push_back(yData[j]);
					z.push_back(zData[id]);
				}
			}
			if (x.size() > 20)
			{
				LeastSquareFit(x, z, 5, coeff);// fit the polynomial curve

				for (int j = 0; j < m_imageHeight; j++)
				{
					int id = j * m_imageWidth + i;			
					if (lmask[id])
					{
						float x = yData[j];
						float xx = x;
						float fz = coeff[0];
						for (size_t k = 1; k < coeff.size(); k++)
						{
							fz += coeff[k] * xx;
							xx *= x;
						}
	
						if (maskData[id] == 0)
						{
							cout << "Z" << id << " changed." << endl;
							zData[id] = fz;
							maskData[id] = 255;
						}
					}
				}			
			}
			else // if the data points are too small, get rid of all of them
			{
				for (int j = 0; j < m_imageHeight; j++)
				{
					int id = j * m_imageWidth + i;
					maskData[id] = 0;
				}
			}
		}


		for (int i = 0;  i <  m_imageHeight; i++)
		{
			x.clear();
			z.clear();
			coeff.clear();
			for (int j = 0; j < m_imageWidth; j++)
			{
				int id = i * m_imageWidth + j;
				if (maskData[id]) // put x and z data into a vector
				{
					x.push_back(xData[j]);
					z.push_back(zData[id]);
				}
			}
			if (x.size() > 20)
			{
				LeastSquareFit(x, z, poly_order, coeff); // fit the polynomial curve
				int kk = 0;
				for (int j = 0; j < m_imageWidth; j++)
				{
					int id = i * m_imageWidth + j;			
					if (maskData[id])
					{
						//float x1 = x[kk];
						//float fz = 0;
						//for (size_t k = 0; k < coeff.size(); k++)
						//{
						//	fz += coeff[k] * pow(x1, (float)k); // compute the data using the fitted polynomials
						//}	
						float x1 = x[kk];
						float fz = coeff[0];
						float xx = x1;
						for (size_t k = 1; k < coeff.size(); k++)
						{
							fz += coeff[k] * xx;
							xx *= x1;
						}
						if (fabs(fz - z[kk]) > thresh1) // if the data point is bad, replaced it with the predicted data point
						{
							cout << "Z" << id << " changed." << endl;
							zData[id] = fz;
						}
						kk ++;
					}
				}			
			}
			else // if the data points are too small, get rid of all of them
			{
				//Laura: Fixed it - used m_imageHeight instead of m_imageWidth
				memset(&lmask[i*m_imageWidth], 0, sizeof(lmask[0])*m_imageWidth);
			}
		}
	}

	coeff.clear();
	z.clear();
	x.clear();
}
/////////////////////////////////////////////////////////////////////
// Fit the curve using a polynomial for the function y = f(x). The 
// polynomial order is determined by the value m.  
//
// Input:
//		x = x sampled values 
//		y = y sampled values
//		m = order of polynomials to be fitted
//		coef = coefficients of the polynomials
// Return:
//
//////////////////////////////////////////////////////////////////////
void CSpikeRemoval::LeastSquareFit(const vector<float> x, const vector<float> y, const int m, vector<float> &coef)
{
	int mp1, mp2, m2, i, j, k, l;
	double w1, w2, w3, pivot, aik;
	double a[21][22], w[42];
	int n = (int)(x.size());

	// This algorithm only works till 19th order
	if (m >= n || m < 1 || m > 20) 
	{
		printf("Either the order number > data number\n");
		printf("Or order number > 19\n");
		return;
	}

	mp1 = m + 1;
	mp2 = m + 2;
	m2 = 2 * m;

	for (i = 0; i < m2; i++)
	{
		w1 = 0.0;
		for ( j = 0; j < n; j++)
		{	
			w3 = x[j];
			w2 = w3;
			for ( k = 0; k < i; k++)
				w2 *= w3;
			w1 += w2;
		}
		w[i] = w1;
	}

	for ( i = 0; i < mp1; i++)
	{
		for ( j = 0; j < mp1; j++)
		{
			if (i != 0 || j != 0) // add by song
			{
				l = i + j - 1;
				a[i][j] = w[l];
			}
		}
	}

	a[0][0] = n;	
	w1 = 0.0;
	for ( i = 0; i < n; i++)
		w1 += y[i];
	a[0][mp1] = w1;

	for ( i = 0; i < m; i++)
	{
		w1 = 0.0;
		for ( j = 0; j < n; j++)
		{
			w3 = x[j];
			w2 = w3;
			for ( k = 0; k < i; k++)
				w2 *= w3;

			w1 += y[j] * w2;
		}
		a[i + 1][mp1] = w1;
		
	}

	for ( k = 0; k < mp1; k++)
	{
		pivot = a[k][k];
		
		for ( j = k; j < mp2; j++)
			a[k][j] /= pivot;
		for ( i = 0; i < mp1; i++)
		{
			if ( i != k)
			{
				aik = a[i][k];
				for ( j = k; j < mp2; j++)
					a[i][j] -= aik * a[k][j];
			}
		}
	}

	for ( i = 0; i < mp1; i++)
	{
		coef.push_back((float)a[i][mp1]);
	}
}
