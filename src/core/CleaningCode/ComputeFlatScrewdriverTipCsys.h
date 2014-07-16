/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * Mantis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mantis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mantis.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Song Zhang (song@iastate.edu), Laura Ekstrand (ldmil@iastate.edu)
 */

#ifndef __COMPUTEFLATSCREWDRIVERTIPCSYS_H__
#define __COMPUTEFLATSCREWDRIVERTIPCSYS_H__
#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QVector>
#include <QMatrix4x4>
#include <QVector3D>
#include <QBitArray> 
#include <QVector4D>

/**
 * A class for computing the coordinate system of a 
 * screwdriver tip.
 *
 * @author Song Zhang, but Laura Ekstrand has made 
 * very major changes to the interface and implementation.
 */

class ComputeFlatScrewdriverTipCsys: public QObject
{
	Q_OBJECT

  protected:
	//Member variables.
	///Keeps track of indices that appear to belong to tip edge points.
	QVector<int> edgeIdx;
	///Keeps track of the centroid position.
	QVector3D centroid;
	///Translation to the centroid of the tip edge estimate.
	QMatrix4x4 translationMatrix;
	///Vector representing the x axis.
	QVector3D xAxisVector;
	///Vector representing the y axis.
	QVector3D yAxisVector;
	///Vector representing the z axis.
	QVector3D zAxisVector;
	///Calculated coordinate system.
	QMatrix4x4 coordinateSystem;

	//Cached stuff from tip.
	QBitArray mask; ///< mask from tip. Implicitly shared.
	QVector<float> depth; ///< depth from tip. Implicitly shared.
	int width; ///< width from tip.
	int height; ///< height from tip.
	float pixelSizeX; ///< pixelSizeX from tip.
	float pixelSizeY; ///< pixelSizeY from tip.

	//Member functions.
	///Find points that likely lie along the edge of the tip.
	void findEdgeIndices();
	///Use the estimated edge to generate a y axis for the coordinate sys.
	void generateYAxis();

  public:
	///Compute the coordinate system. Pass in the y-angle of the tip when it was scanned.
	ComputeFlatScrewdriverTipCsys(int w, int h, float pixX, float pixY,
		const QVector<float>& zData, const QBitArray& maskData,
		float yScanAngle = 45.0f, 
		QWidget *parent = 0);
	virtual ~ComputeFlatScrewdriverTipCsys();

	///Get the computed translation matrix.
	inline const QMatrix4x4& getTranslationMatrix()
		{return translationMatrix;}
	///Get the computed coordinate system matrix.
	inline const QMatrix4x4& getCoordinateSystemMatrix()
		{return coordinateSystem;}
	///Get the x axis estimate.
	inline const QVector3D& getXAxisVector()
		{return xAxisVector;}
	///Get the y axis estimate.
	inline const QVector3D& getYAxisVector()
		{return yAxisVector;}
	///Get the z axis estimate.
	inline const QVector3D& getZAxisVector()
		{return zAxisVector;}
};

#endif //!defined __COMPUTEFLATSCREWDRIVERTIPCSYS_H__
