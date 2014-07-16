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

#include "FilterImage.h"
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

CFilterImage::CFilterImage(int width, int height):
m_imageWidth(width), m_imageHeight(height), m_imageSize(width * height)
{
}

CFilterImage::~CFilterImage(void)
{
}
//////////////////////////////////////////////////////////////////////////////////////////
//	Description:
//		Fill the holes of the phase map 
//
//	Returns:
//		
//	Parameters:
//
//////////////////////////////////////////////////////////////////////////////////////////

void CFilterImage::FillHolesFloat(float *image, unsigned char *maskData, const int hsize)
{

	if (hsize < 1) return; // don't need fill holes

	int i, j, k, k1, k2;
	float ax, bx;

	// Copy in data to out data
	//memcpy(unwrappedPhaseIn, unwrappedPhaseOut, sizeof(unwrappedPhaseOut[0]) * m_imageSize);
	
	// The falling edges in a line will be saved in hole_start
	// The rising edges in a line will be saved in hole_end
	// A hole exist between the falling edge and the rising edge following it
	vector<int>  hole_start;
	vector<int>  hole_end;


	// fill holes along y direction
	// A line in y direction is a colume in the image
	// We will go around all the colume and find and fille hols for each colume
	for ( j = 0; j < m_imageWidth - 1; j++)
	{
		// Neither falling edge nor rising edge is found 
		hole_start.clear();
		hole_end.clear();
	
		// Seach for the first valid pixel
		// For colume j, we search from the first row to the last row
		i = 0;
		while( i < m_imageHeight - 1)
		{
			if(maskData[ i * m_imageWidth + j ] == 255)
			{
				break;
			}
			i ++;
		}

		// If all the point on this colume is invalid, continue to serach for another colume
		if( i == m_imageHeight - 1) continue;
		
		// Now start to search for a hole
		// in a line of image, we have data  1110000011000
		// When data change from 1 to 0, there is a falling edge
		// When data change from 0 to 1, there is a rising edge
		// A hole exists between a following edge and the rising edge after it

		// I will start from 1 ( 255 ), to search for falling edge first
		// Here we use 255 instead of 1
		int former_state = 255;

		// Search from current point to the end of the line
		for( ; i < m_imageHeight - 1; i ++)
		{
			if( maskData[ i * m_imageWidth + j ] != 255 && former_state == 255)
			{
				// The current point is invalid, and the points before the current are valid
				// A falling edge 
				// A new possible hole starts, save the point before j
				hole_start.push_back( i - 1);
				former_state = 0;
			}
			else if( maskData[ i * m_imageWidth + j] == 255 && former_state != 255)
			{
				// The current point is a valid point, and the points 
				hole_end.push_back( i );
				former_state = 255;
			}
		}
		//		ASSERT( hole_start.size() >= hole_end.size());
		// No hole found in this line
		if( hole_end.size() < 1) continue;

		// Go through all the possible holes
		for( size_t kk = 0; kk < hole_end.size(); kk++)
		{
			int start = hole_start[kk];
			int end   = hole_end[kk];

			// Only fill the holes that has a size less than or equal to holeSize
			if( end - start < hsize + 1)
			{
				// Get the geometry of the point before the hole
				k1 = (start * m_imageWidth + j);
				ax = image[k1];
				// Get the geometry of the point after the hole
				k2 = (end * m_imageWidth + j);
				bx = image[k2];					
				for( int kkk = start + 1; kkk < end; kkk ++)
				{
					// Fill the hole by linear interpolation
					k = (kkk * m_imageWidth + j);
					float t = (float)( kkk - start) / (end - start);
					image[k] = ax * (1-t) + bx * t;
					maskData[k] = 255;
				}
			}
		}
	}
	// Fill the holes in horizontal line
	for ( i = 0; i < m_imageHeight; i++)
	{
		// No hole found in this line
		hole_start.clear();
		hole_end.clear();

		// Get the index of the first pixel of this line
		int lineindex = i * m_imageWidth;
	
		// Seach for the first valid pixel
		j = 0;
		while( j < m_imageWidth - 1)
		{
			if( maskData[ lineindex + j ] == 255)
			{
				break;
			}
			j ++;
		}
		if( j == m_imageWidth - 1) continue;
		
		// Now start to search for a hole
		// in a line of image, we have data  1110000011000
		// When data change from 1 to 0, there is a falling edge
		// When data change from 0 to 1, there is a rising edge
		// A hole exists between a folling and the rising edge after it

		// I will start from 1, to search for falling edge first
		// Here we use 255 instead of 1
		int former_state = 255;

		// Search from current point to the end of the line
		for( ; j < m_imageWidth - 1; j ++)
		{
			if( maskData[ lineindex + j ] != 255 && former_state == 255)
			{
				// The state of the current point is not the same as the former point
				// and the former point is of value 255
				// A falling edge is found.

				// A new possible hole starts, save the point before j
				hole_start.push_back( j - 1);
				former_state = 0;
			}
			else if( maskData[ lineindex + j] == 255 && former_state != 255)
			{
				// A rising edge is found
				hole_end.push_back( j );
				former_state = 255;
			}
		}
//		ASSERT( hole_start.size() >= hole_end.size());
		// If no hole is found, continue to search the next line
		if( hole_end.size() < 1) continue;

		// Go through all the possible holes
		for( size_t kk = 0; kk < hole_end.size(); kk++)
		{
			int start = hole_start[kk];
			int end   = hole_end[kk];
			// Fill the hole if its size is less than or equal to holeSize
			if( end - start < hsize + 1)
			{
				// Get the geometry of the point before the hole
				k1 = (lineindex + start);
				ax = image[k1];
				// Get the geometry of the point after the hole
				k2 = (lineindex + end);
				bx = image[k2];	

				// Fill the hole by linear interpolation
				for( int kkk = start + 1; kkk < end; kkk ++)
				{
					k = (lineindex + kkk);
					float t = (float)( kkk - start) / (end - start);
					image[ k] = ax * (1-t) + bx * t;
					maskData[k] = 255;
				}
			}
		}
	}

	hole_start.clear();
	hole_end.clear();
	
}
//Median filter the image
void CFilterImage::MedianFilterFloat(float *input, float *output, const int fsize, unsigned char *mask)
{
	int hsize = fsize/2;
	bool isMask = true;
	if (mask == NULL) 
	{
		isMask = false;
		mask = new unsigned char [m_imageSize];
		memset(mask, 255, sizeof(mask[0]) * m_imageSize);
	}
	//memset(output, 255, sizeof(output[0]) * m_imageSize);
	vector<float> vt;
	for (int i = hsize; i < m_imageHeight-hsize; i++)
	{
		for (int j = hsize; j < m_imageWidth-hsize; j++)
		{
			int id0 = i * m_imageWidth + j;
			if (mask[id0])
			{
				vt.clear();
				for (int m = -hsize; m < hsize+1; m++)
				{
					for (int n = -hsize; n < hsize+1; n++)
					{
						int idx = (i+m)*m_imageWidth + j + n;
						if (mask[idx])
							vt.push_back(input[idx]);
					}
				}
				
				sort(vt.begin(), vt.end());
				output[id0] = vt[vt.size()/2];
				vt.clear();
			}
			else 
			{
				output[id0] = 0;
			}
		}
	}
	if (!isMask)
	{
		delete [] mask;
		mask = NULL;
	}
	vt.clear();
	
}
//Median filter the image
void CFilterImage::MedianFilterU8(unsigned char *input, unsigned char *output, const int fsize, unsigned char *mask)
{
	int hsize = fsize/2;
	bool isMask = true;
	if (mask == NULL) 
	{
		isMask = false;
		mask = new unsigned char [m_imageSize];
		memset(mask, 255, sizeof(mask[0]) * m_imageSize);
	}
	memset(output, 0, sizeof(output[0]) * m_imageSize);
	vector<unsigned char> vt;
	for (int i = hsize; i < m_imageHeight-hsize; i++)
	{
		for (int j = hsize; j < m_imageWidth-hsize; j++)
		{
			int id0 = i * m_imageWidth + j;
			if (mask[id0])
			{
				vt.clear();
				for (int m = -hsize; m < hsize+1; m++)
				{
					for (int n = -hsize; n < hsize+1; n++)
					{
						int idx = (i+m)*m_imageWidth + j + n;
						if (mask[idx])
							vt.push_back(input[idx]);
					}
				}

				sort(vt.begin(), vt.end());
				output[id0] = vt[vt.size()/2];
				vt.clear();
			}
			else
			{
				output[id0] = 0;
			}
		}
	}
	if (!isMask)
	{
		delete [] mask;
		mask = NULL;
	}
	vt.clear();
}
//Gaussian smooth the image
void CFilterImage::GaussianFilterFloat(float *input, float *output, const int fsize, unsigned char *mask)
{
	int hsize = fsize/2;
	float * F = NULL;
	ComputeGaussianFilter(F, hsize);

	bool isMask = true;
	if (mask == NULL) 
	{
		isMask = false;
		mask = new unsigned char [m_imageSize];
		memset(mask, 255, sizeof(mask[0]) * m_imageSize);
	}

	for (int j = 0; j < m_imageWidth; j++)
	{
		for (int i = hsize; i < m_imageHeight-hsize; i++)
		{
			int idx0 = i*m_imageWidth + j;
			if (mask[idx0])
			{
				float tmpSum = 0.0f;
				float fSum = 0.0f;
				for (int m = -hsize; m < hsize+1; m++)
				{
					int idx = (i+m)*m_imageWidth + j;
					if (mask[idx])
					{
						tmpSum += input[idx] * F[m+hsize];
						fSum += F[m+hsize];
					}

				}
				output[idx0] = tmpSum/fSum;
			}
		}
	}

	for (int i = 0; i < m_imageHeight; i++)
	{
		for (int j = hsize; j < m_imageWidth - hsize; j++)
		{
			int idx0 = i*m_imageWidth + j;
			if (mask[idx0])
			{
				float tmpSum = 0.0f;
				float fSum = 0.0f;
				for (int m = -hsize; m < hsize+1; m++)
				{
					int idx = i * m_imageWidth + j+m;
					if (mask[idx])
					{
						tmpSum += input[idx] * F[m+hsize];
						fSum += F[m+hsize];
					}

				}
				output[idx0] = tmpSum/fSum;
			}
		}
	}
	if (!isMask)
	{
		delete [] mask;
		mask = NULL;
	}
}
//Gaussian smooth the image
void CFilterImage::GaussianFilterU8(unsigned char *input, unsigned char *output, const int fsize, unsigned char *mask)
{
	int hsize = fsize/2;
	float * F = NULL;
	ComputeGaussianFilter(F, hsize);
	memcpy(output, input, sizeof(output[0])*m_imageSize);
	
	bool isMask = true;
	if (mask == NULL) 
	{
		isMask = false;
		mask = new unsigned char [m_imageSize];
		memset(mask, 255, sizeof(mask[0]) * m_imageSize);
	}
	for (int j = 0; j < m_imageWidth; j++)
	{
		for (int i = hsize; i < m_imageHeight-hsize; i++)
		{
			int idx0 = i*m_imageWidth + j;
			if (mask[idx0])
			{
				float tmpSum = 0.0f;
				float fSum = 0.0f;
				for (int m = -hsize; m < hsize+1; m++)
				{
					int idx = (i+m)*m_imageWidth + j;
					if (mask[idx])
					{
						tmpSum += input[idx] * F[m+hsize];
						fSum += F[m+hsize];
					}

				}
				output[idx0] = (int)(tmpSum/fSum);
			}
		}
	}

	for (int i = 0; i < m_imageHeight; i++)
	{
		for (int j = hsize; j < m_imageWidth - hsize; j++)
		{
			int idx0 = i*m_imageWidth + j;
			if (mask[idx0])
			{
				float tmpSum = 0.0f;
				float fSum = 0.0f;
				for (int m = -hsize; m < hsize+1; m++)
				{
					int idx = i * m_imageWidth + j+m;
					if (mask[idx])
					{
						tmpSum += input[idx] * F[m+hsize];
						fSum += F[m+hsize];
					}

				}
				output[idx0] = (int)(tmpSum/fSum);
			}
		}
	}
	if (!isMask)
	{
		delete [] mask;
		mask = NULL;
	}
}
// compute Gaussian filter
void CFilterImage::ComputeGaussianFilter(float *&filter, int hsize)
{
	if (hsize < 1) return;

	if (filter) delete [] filter;
	int filterSize = hsize*2+1;
	filter = new float[filterSize];

	float thegma2 = (float)((hsize/1.5)*(hsize/1.5));

	float sum = 0.0;
	for (int i = 0; i < filterSize; i++)
	{
		filter[i] = exp(-(i-hsize)*(i-hsize)/thegma2);
		sum += filter[i];
	}
	sum = 1.0f / sum;
	for (int i = 0; i < filterSize; i++) 
	{
		filter[i] *= sum;
	}
}
