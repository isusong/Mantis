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
 * Authors: Song Zhang (song@iastate.edu), Yuanzheng Gong
 */

#include "FindValidMarkRange.h"
#include "FilterImage.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include <cfloat>
using namespace std;

CFindValidMarkRange::CFindValidMarkRange(int width, int height):
m_imageWidth(width), m_imageHeight(height), m_imageSize(width * height)
{
        m_maskData = NULL;
}

CFindValidMarkRange::~CFindValidMarkRange(void)
{
}

void CFindValidMarkRange::LocateMarkRegion(unsigned char *image, unsigned char *qualityData, unsigned char *mask, float &markAngle)
{
        m_maskData = mask;

        CFilterImage filter(m_imageWidth, m_imageHeight);
        float *nimage = new float [m_imageSize];

        memset(mask, 255, sizeof(mask[0])*m_imageSize);        
        NormalizeImageU8(image, nimage, mask);
        ComputeMask(nimage, qualityData, 0.05f, 200);

        float *Fx = new float [m_imageSize];
        float *Fy = new float [m_imageSize];
        float mFx = ComputeGradientX(nimage, Fx);
        float mFy = ComputeGradientY(nimage, Fy);
        markAngle = atan2(-mFy, mFx);

        // Get absolute value of the gradient map
        AbsoluteImage(Fx, Fx);
        AbsoluteImage(Fy, Fy);

        // binarize the image
        BinarizeImageFT(Fx, Fx, mask, 0.04f); // 0.04 originally (try 2 if things don't work out)
        BinarizeImageFT(Fy, Fy, mask, 0.04f); // 0.04 originally

        // apply median filter
        filter.MedianFilterFloat(Fx, Fx, 3, mask);
        filter.MedianFilterFloat(Fy, Fy, 3, mask);

        // compute absolute difference
        float * diff = new float [m_imageSize];
        for (int i = 0; i < m_imageSize; i++)
        {
                diff[i] = fabs(Fx[i] - Fy[i]);
        }

        // binarize image
        BinarizeImageFT(diff, diff, mask, 0.15f);

        // median filtering it again
        filter.MedianFilterFloat(diff, diff, 3, mask);
        filter.MedianFilterFloat(diff, diff, 3, mask);

        // Remove outside boundary by making mask value to be zeros;
        RemoveBoundary(diff, mask);

        delete [] diff;
        delete [] Fx;
        delete [] Fy;
}
// void

void CFindValidMarkRange::ComputeMask(float *textureData, unsigned char *qualityData, float threshold_t, unsigned char threshold_q)
{
        memset(m_maskData, 255, sizeof(m_maskData[0]) * m_imageSize);
        if (qualityData)
        {
                for (int i = 0; i < m_imageSize; i++)
                {
                        if (textureData[i] < threshold_t || qualityData[i] > threshold_q)
                        {
                                m_maskData[i] = 0;
                        }
                }
        }
        else
        {
                for (int i = 0; i < m_imageSize; i++)
                {
                        if (textureData[i] < threshold_t)
                        {
                                m_maskData[i] = 0;
                        }
                }
        }
}

//
void CFindValidMarkRange::RemoveBoundary(float *diff, unsigned char *mask)
{
        // Count how many valid pixels per row
        vector<int>vcont;
        vcont.clear();
        for (int i = 0; i < m_imageHeight; i++)
        {
                float cnt = 0;
                for (int j = 0; j < m_imageWidth; j++)
                {
                        cnt += diff[i*m_imageWidth + j];
                }
                //printf("%.2f\t", cnt);
                int t = cnt > m_imageWidth /2 ? 1 : 0; //less than half valid, the line is not stripe
                vcont.push_back(t);
        }

        int startID = 10;
        int endID = m_imageHeight - 10;
        // Find the top boundary
        for (size_t i = 10; i < vcont.size() - 10; i++)
        {
                startID = i;
                if (vcont[i]) break;

        }

        for (size_t i = vcont.size() - 10; i > 10; i--)
        {
                endID = i;
                if (vcont[i]) break;
        }
        vcont.clear();

        //printf("start = %d\t end = %d\n", startID, endID);
        memset(mask, 0, sizeof(mask[0])* startID * m_imageWidth);
        memset(&mask[endID * m_imageWidth], 0, sizeof(mask[0])* (m_imageHeight - endID) * m_imageWidth);
}

// normalize image
void CFindValidMarkRange::NormalizeImage(float *input, float *output, unsigned char *mask)
{
        bool isMask = true;
        if (mask == NULL)
        {
                isMask = false;
                mask = new unsigned char [m_imageSize];
                memset(mask, 255, sizeof(mask[0]) * m_imageSize);
        }

        float maxV = -FLT_MAX;
        float minV = FLT_MAX;
        for(int i = 0; i < m_imageSize; i++)
        {
                if (mask[i])
                {
                        maxV = maxV > input[i] ? maxV : input[i];
                        minV = minV < input[i] ? minV : input[i];
                }
        }
        float irange = 1.0f/(maxV - minV);
        for(int i = 0; i < m_imageSize; i++)
        {
                if (mask[i])
                {
                        output[i] = ((input[i] - minV)*irange);
                }
                else output[i] = 0;
        }

        if (!isMask)
        {
                delete [] mask;
                mask = NULL;
        }
}

// normalize image
void CFindValidMarkRange::NormalizeImageU8(unsigned char *input, float *output, unsigned char *mask)
{
        float *inputFT = new float [m_imageSize];        
        for (int i = 0; i < m_imageSize; i++)
        {
                inputFT[i] = (float)input[i];
        }
        NormalizeImage(inputFT, output, mask);

        delete [] inputFT;
}

// normalize image
void CFindValidMarkRange::BinarizeImageU8(unsigned char *input, unsigned char *output, unsigned char *mask, const int threshd)
{
        bool isMask = true;
        if (mask == NULL)
        {
                isMask = false;
                mask = new unsigned char [m_imageSize];
                memset(mask, 255, sizeof(mask[0]) * m_imageSize);
        }

        for(int i = 0; i < m_imageSize; i++)
        {
                if (mask[i])
                {
                        output[i] = input[i] > threshd ? 255 : threshd;
                }
        }

        if (!isMask)
        {
                delete [] mask;
                mask = NULL;
        }
}
// normalize image
void CFindValidMarkRange::BinarizeImageFT(float *input, float *output, unsigned char *mask, const float threshd)
{
        bool isMask = true;
        if (mask == NULL)
        {
                isMask = false;
                mask = new unsigned char [m_imageSize];
                memset(mask, 255, sizeof(mask[0]) * m_imageSize);
        }
//	memset(output, 0, sizeof(output[0])*m_imageSize);

        for(int i = 0; i < m_imageSize; i++)
        {
                if (mask[i])
                {
                        output[i] = input[i] > threshd ? 1.0f : 0;
                }
        }

        if (!isMask)
        {
                delete [] mask;
                mask = NULL;
        }
}
// normalize image
void CFindValidMarkRange::AbsoluteImage(float *input, float *output)
{
        for(int i = 0; i < m_imageSize; i++)
        {
                output[i] = fabs(input[i]);
        }
}

// return mean value of x gradient
float CFindValidMarkRange::ComputeGradientX(float *image, float *Fx)
{
        float mFx = 0.0f;
        int cnt = 0;
        memset(Fx, 0, sizeof(Fx[0]) * m_imageSize);

        for (int j = 0; j < m_imageHeight; j++)
        {
                int lineId = j * m_imageWidth;
                for (int i = 1; i < m_imageWidth; i++)
                {
                        int idx = lineId + i;
                        if (m_maskData[idx] && image[idx-1])
                        {
                                float t = image[idx] - image[idx-1];
                                Fx[idx] = t;
                                mFx += t;
                                cnt++;
                        }
                }
        }
        if (!cnt) return 0.0f;
        return mFx/cnt;
}


// return mean value of y gradient
float CFindValidMarkRange::ComputeGradientY(float *image, float *Fy)
{
        float mFy = 0.0f;
        int cnt = 0;
        memset(Fy, 0, sizeof(Fy[0]) * m_imageSize);
        for (int j = 1; j < m_imageHeight; j++)
        {
                int lineId = j * m_imageWidth;
                for (int i = 0; i < m_imageWidth; i++)
                {
                        int idx = lineId + i;
                        if (m_maskData[idx] && image[idx-m_imageWidth])
                        {
                                float t = image[idx] - image[idx-m_imageWidth];
                                Fy[idx] = t;
                                mFy += t;
                                cnt++;
                        }
                }
        }
        if (!cnt) return 0.0f;
        return mFy/cnt;
}

