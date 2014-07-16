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

#include "ManipulatePlate.h"

ManipulatePlate::ManipulatePlate(RangeImage* newPlate, float* plateDepth)
{
	//Cache some data.
	width = newPlate->getWidth();
	height = newPlate->getHeight();
	pixelSizeX = newPlate->getPixelSizeX();
	pixelSizeY = newPlate->getPixelSizeY();
	depth = newPlate->getDepth();
	mask = newPlate->getMask();

	//Fit a plane to the plate data.
	QVector<int> idx;
	//Go in a diagonal line across the plate.
	//Take downsampled points (40 of them).
	int skip = height/40;
	for (int i = 0; i < height - skip; i += skip)
	{
		//Left diagonal.
		int j = (int) i * ((float) width/(float) height);
		int id = i*width + j;
		if (mask.testBit(id))
		{
			idx.push_back(id);
		}

		//Right diagonal.
		j = width - j;
		id = i*width + j;
		if (mask.testBit(id))
		{
			idx.push_back(id);
		}
	}
	QVector4D planeCoeffs = fitPlane(idx);
	float nx = planeCoeffs.x();
	float ny = planeCoeffs.y();
	float nz = planeCoeffs.z();
	float p =  planeCoeffs.w();

	//Subtract off the plane.
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			float x = j*pixelSizeX;
			float y = i*pixelSizeY;
			plateDepth[i*width + j] 
				-= (-1/nz)*(p + (nx*x) + (ny*y));
		}
	}
}

ManipulatePlate::~ManipulatePlate()
{
	//Nothing belongs to ManipulatePlate.
}

QVector4D
ManipulatePlate::fitPlane(const QVector<int>& idx)
{
	//Here we solve the least squares problem
	//to determine a, b, & c in the equation
	//z= ax + by + c.
	//We can format this as so:
	//+       + + +    +  +
	//|x0 y0 1| |a|  = |z0|
	//|x1 y1 1| |b|    |z1|
	//|x2 y2 1| |c|    |z2|
	//|x3 y3 1| + +    |z3|
	//|.   . .|        | .|
	//|.   . .|        | .|
	//|.   . .|        | .|
	//|xn yn 1|        |zn|
	//+       +        +  +
	//Which we can write as Xv = z.
	//Then we can take (X^T)Xv = (X^T)z.
	//Then we solve v = ((X^T)X)^(-1)*(X^T)z.
	//Reference: http://tutorial.math.lamar.edu/Classes/LinAlg/LeastSquares.aspx

	QVector<float> X, z;
	for (int i = 0; i < idx.size(); ++i)
	{
		//Load a row xi yi 1 into X
		X.push_back((idx[i]%width)*pixelSizeX); //x
		X.push_back((idx[i]/width)*pixelSizeY); //y
        X.push_back(1.0f);

		//Load a depth value into z vector.
		z.push_back(depth[idx[i]]);
	}

	//(X^T)X
	QMatrix4x4 XTX; //Identity matrix.
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			XTX(i, j) = 0;
			for (int k = 0; k < idx.size(); ++k)
			{
				//This makes sense if you look at
				//doing (X^T)X by hand.
				XTX(i, j) += X[k*3 + i]*X[k*3 + j];
			}
		}
	}

	//(X^T)z
	QVector4D XTz; //zero vector.
	for (int k = 0; k < idx.size(); ++k)
	{
		XTz += QVector4D(X[k*3]*z[k], //xk*zk
						 X[k*3 + 1]*z[k], //yk*zk
						 z[k],   //zk
						  0);  //w, has no effect
	}

	//Solve for v
	//A note on ((X^T)X)^(-1):
	//Using the properties of block matrices, it is simple to
	//show that
	//+     ++		  +
	//|A   0||A^(-1) 0|  is the identity matrix,
	//|0^T 1||0^T    1|
	//+     ++        +
	//where the 0's are the appropriate column vectors of 0's.
	//Therefore, we can use the QMatrix4x4 object to do
	//the inversion and multiplication.
	//Reference: http://mathworld.wolfram.com/BlockMatrix.html
	//The QVector3D constructor drops the meaningless w coordinate.
	QVector3D v (XTX.inverted()*XTz); 

	//v = [a, b, c]^T; z = ax + by + c
	//Put into plane form to find normal:
	//-ax - by + 1z - c = 0
	//Therefore normal is [-a -b 1]^T normalized.
	QVector3D n = -v;
	float negC = n.z();
	n.setZ(1);
	negC /= n.length();
	n.normalize();

	return QVector4D(n.x(), n.y(), n.z(), negC);
}

