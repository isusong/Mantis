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
 * Authors: Song Zhang (song@iastate.edu), Yuanzheng Gong
 */

#ifndef FINDVALIDMARKRANGE_H
#define FINDVALIDMARKRANGE_H

#include<iostream>
using namespace std;

class CFindValidMarkRange
{
public:
        CFindValidMarkRange(int width, int height);
        ~CFindValidMarkRange(void);
public:
        void LocateMarkRegion(unsigned char *textureData, unsigned char *qualityMap, unsigned char *mask, float &markAngle);

private:
        float ComputeGradientX(float *image, float *Fx);
        float ComputeGradientY(float *image, float *Fy);
        void NormalizeImage(float *input, float *output, unsigned char *mask = NULL);
        void NormalizeImageU8(unsigned char *input, float *output, unsigned char *mask = NULL);
        void AbsoluteImage(float *input, float *output);
        void BinarizeImageU8(unsigned char *input, unsigned char *output, unsigned char *mask = NULL, const int threshd = 10);
        void BinarizeImageFT(float *input, float *output, unsigned char *mask, const float threshd);
        void RemoveBoundary(float *diff, unsigned char *mask);
        void ComputeMask( float *textureData, unsigned char *qualityData,
                float threshold_t = 0.05f, unsigned char threshold_q = 200);

private:


        unsigned char *m_maskData;
        const int m_imageWidth, m_imageHeight, m_imageSize;

};

#endif // FINDVALIDMARKRANGE_H
