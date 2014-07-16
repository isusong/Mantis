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
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#include "heightMap.h"
#include <qwt_plot_canvas.h>
#include <qwt_color_map.h>
#include <iostream>

using std::cout;
using std::endl;

heightMap::heightMap(QWidget* parent):
	QwtPlot(parent)
{
	//Prepare plot
	//Init spectrogram.
	profileCurve = new QwtPlotSpectrogram();	
	profileCurve->setColorMap(new QwtLinearColorMap(Qt::black, Qt::white));
	profileCurve->attach(this);

	//Init default data.
	heightData = new QwtMatrixRasterData();
	QVector<double> profilePts;
	profilePts.push_back(2.7); //A wonderful grayed-out color.
	heightData->setValueMatrix(profilePts, profilePts.size());
	//Apparently you need these intervals, even though the XAxis and YAxis calls do nothing in our case.
	heightData->setInterval( Qt::XAxis, QwtInterval(5, 7));
	heightData->setInterval( Qt::YAxis, QwtInterval(-1, 4));
	//Height interval (very much needed for the map coloration and the color bar).
	zInterval.setMinValue(1);
	zInterval.setMaxValue(3);
	heightData->setInterval( Qt::ZAxis, zInterval);
	profileCurve->setData(heightData);
	//Do bilinear interpolation
	heightData->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

	//Turn off axes.
	enableAxis(xBottom, false);
	enableAxis(yLeft, false);

	//Turn off frame.
	canvas()->setFrameShape(QFrame::NoFrame);

	//Create color bar.
	rightAxis = axisWidget(QwtPlot::yRight); //Grab the right axis.
	rightAxis->setColorBarEnabled(true);
	rightAxis->setColorBarWidth(40);
    rightAxis->setColorMap(zInterval, new QwtLinearColorMap(Qt::black, Qt::white)); //Give color map the appropriate color limits.

	//Give the color bar an axis.
    setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
	enableAxis(yRight, true);

}

heightMap::~heightMap()
{
	delete profileCurve;
	delete rightAxis;
	//delete heightData; //Don't delete - its parent deletes it.
}

void
heightMap::setProfileCurve(const QVector<QPointF>& profilePts, double zMin, double zMax)
{
	//It doesn't look like setData will delete the previous pointer for you.
	//Store the y-coordinates in a QVector<double>.
	//Make a QwtMatrixRasterData*, call setValueMatrix on it, then set
	//intervals.
	
	//Vector of y values from the data
	QVector<double> yPoints;
	for (int i = 0; i < profilePts.size(); ++i)
	{
		yPoints.push_back(profilePts[i].y());
	}

	//Set heightData to new data
	heightData->setValueMatrix(yPoints, yPoints.size());
	zInterval.setMinValue(zMin);
	zInterval.setMaxValue(zMax);
	heightData->setInterval(Qt::ZAxis, zInterval);

	//Set color bar limits
    rightAxis->setColorMap(zInterval, new QwtLinearColorMap(Qt::black, Qt::white)); //Give color map the appropriate color limits.
    setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
	//Try ScaleDiv ticks()

	replot();
}
