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

#ifndef __MANIPULATEPLATE_H_
#define __MANIPULATEPLATE_H_
#include "../RangeImage.h"
#include <QPointer>
#include <QVector4D>

/**
 * @author Song Zhang, but Laura Ekstrand
 * has made major changes to the interface
 * and implementation.
 */

class ManipulatePlate
{
  private:
	//Member variables.
	int width, height; //cached from plate.
	float pixelSizeX; //cached from plate.
	float pixelSizeY; //cached from plate.
	QVector<float> depth; //implicitly shared.
	QBitArray mask; //implicitly shared.

	///Fit a plane to the points described by idx and return the coefficients.
	/**
	 * Yes, I was naughty and copied this from ComputeCS.
	 * No, I didn't change anything.
     *
	 * @return [nx ny nz p] for n*[x;y;z] + p = 0 plane form.
	 * [nx ny nz] is the normal vector for the plane.
	 */
	QVector4D fitPlane(const QVector<int>& idx);

  public:
	ManipulatePlate(RangeImage* newPlate, float* plateDepth);
	~ManipulatePlate();
};

#endif // __MANIPULATEPLATE_H__
