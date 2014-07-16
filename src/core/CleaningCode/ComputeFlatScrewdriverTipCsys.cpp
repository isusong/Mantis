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
 * Authors: Song Zhang (song@iastate.edu), Laura Ekstrand (ldmil@iastate.edu)
 */

#include "ComputeFlatScrewdriverTipCsys.h"
#include <cfloat>
#include <QPointF>
#include <QGenericMatrix>

ComputeFlatScrewdriverTipCsys::ComputeFlatScrewdriverTipCsys(
	int w, int h, float pixX, float pixY,
	const QVector<float>& zData, const QBitArray& maskData, 
	float yScanAngle, QWidget *parent):
	QObject(parent)
{
	//Do some caching.
	mask = maskData;
	depth = zData;
	width = w;
	height = h;
	pixelSizeX = pixX;
	pixelSizeY = pixY;

	//Make an estimate of the tip edge.
	findEdgeIndices();

	//Store a translation to the centroid of the tip edge estimate
	for (int i = 0; i < edgeIdx.size(); ++i)
	{
		float xi = (edgeIdx[i]%width)*pixelSizeX;
		float yi = (edgeIdx[i]/width)*pixelSizeY;
		centroid += QVector3D(xi, yi, depth[edgeIdx[i]]);
	}
	centroid /= edgeIdx.size();
	translationMatrix.translate(-centroid);

	//Estimate the axes
	//y axis.
	generateYAxis();
	yAxisVector = -yAxisVector;
	//x axis.
	QMatrix4x4 rotateXAxis;
	rotateXAxis.rotate(yScanAngle, QVector3D(0, 1, 0));
	xAxisVector = rotateXAxis * QVector3D(0, 0, 1);
	xAxisVector = -xAxisVector;
	//z axis.
	zAxisVector = QVector3D::crossProduct(
		xAxisVector, yAxisVector);
	zAxisVector.normalize();

	// The following operation is to ensure that 
	// the new coordinate system is orthonormal
	xAxisVector = QVector3D::crossProduct(
		yAxisVector, zAxisVector);
	xAxisVector.normalize();

	//Construct final coordinate system.
	QMatrix4x4 basisChange; //identity matrix
	//QVector4D constructor sets w to 0
	basisChange.setColumn(0, QVector4D(xAxisVector));
	basisChange.setColumn(1, QVector4D(yAxisVector));
	basisChange.setColumn(2, QVector4D(zAxisVector));
	coordinateSystem = basisChange.inverted() * translationMatrix;
}

ComputeFlatScrewdriverTipCsys::~ComputeFlatScrewdriverTipCsys()
{

}

// find the edge point horizontally
// assume edge is the largest depth value and find roughly 300 points
// on that edge (NOTE: a number makes sense)
void
ComputeFlatScrewdriverTipCsys::findEdgeIndices()
{
	edgeIdx.clear();  //Get rid of any lingering data.

	//Amount to downsample in the row direction.
	//If 1, take every row; if 2, take every other row, etc.
	int skip = height/300 + 1;

	//Find maxes (unless they are too close to the edge).
	for (int i = 0; i < height; i += skip)
	{
		 //Go down row & find max point.
		 int lineId = i * width;
		 float tmp = -FLT_MAX;
		 int maxIdx = 0;
		 for (int j = 0; j < width; j++)
		 {
				 int idx = lineId + j;
				 if (mask.testBit(idx))
				 {
						 if (tmp < depth[idx])
						 {
								 maxIdx = idx;
								 tmp = depth[idx];
						 }
				 }
		 }
		 //If the maxIdx is valid and does not occur near the edge,
		 //store it.
		 if ( maxIdx > 0 && 
				 ((maxIdx % width) > 10) && 
				 ((maxIdx % width) < width - 10)
				 )
		 {
				 edgeIdx.push_back(maxIdx);
		 }
	 }
}

// define y direction by 3D line fitting through linear regression
// use the method described in Wiki: http://en.wikipedia.org/wiki/User:Vossman/3D_Line_Regression
void
ComputeFlatScrewdriverTipCsys::generateYAxis()
{
	QMatrix4x4 A; //identity matrix.
	A.fill(0); //Set to all zeros.
	QVector4D B; //zero vector.
	int edgeLength = edgeIdx.size();
	
	for (int i = 0; i < edgeLength; ++i)
	{
		// swap (x0 y0 z0) to (y z x) to make sure that the line
		// direction (close to y0 direction) pass through x-y plane,
		// change y to z. 
		float y = (edgeIdx[i]%width)*pixelSizeX - centroid.x();//x
		float z = (edgeIdx[i]/width)*pixelSizeY - centroid.y();//y
		float x = depth[edgeIdx[i]] - centroid.z(); //z

		A(0,0) += z*z / edgeLength;
		A(0,2) += z / edgeLength;
		B += QVector4D(x*z / edgeLength,
		               x / edgeLength,
		               y*z / edgeLength,
                       y / edgeLength);
	}

	A(1, 0) = A(0, 2);
	A(2, 1) = A(0, 0);
	A(2, 3) = A(0, 2);
	A(3, 1) = A(0, 2);
	A(1, 2) = 1;
	A(3, 3) = 1;

	QVector4D v0;
	v0 = A.inverted()*B;

	// deterine y vector obtain the vector, and
	// swap back to original coordinate system
	yAxisVector = QVector3D(v0.y()-v0.w(), 1.0f, v0.x()-v0.z());
	yAxisVector.normalize();
}
