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

#include "statPlot.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>

#define LINEWIDTH 2

statPlot::statPlot(QWidget* parent):
	QwtPlot(parent)
{
	//Plot-wide settings.
	//Init panning and magnifying.
    (void) new QwtPlotPanner( canvas() ); //Inherits from QWidget.
    (void) new QwtPlotMagnifier( canvas() ); //Inherits from QObject.
	//Style the plot box
    QFrame *frame = qobject_cast<QFrame *>(canvas());
    if (frame) frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	//Title the axes.
	setAxisTitle(xBottom, tr("x"));
	setAxisTitle(yLeft, tr("z"));
	//Make grid lines.
	gridLines = new QwtPlotGrid();
	QPen gridPen (Qt::lightGray);
	gridPen.setStyle(Qt::DashLine);
	gridLines->setPen(gridPen);
	gridLines->attach(this);

	//Set up profile plotting.
	profileCurve = new QwtPlotCurve("Mark Profile Curve");
	profileCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
	curvePen.setColor(Qt::red); //Default pen color is red.
	curvePen.setWidth(LINEWIDTH);
	profileCurve->setPen(curvePen);
	profileCurve->attach(this); //Associate the curve with the plot.
	
	//Set up search window plotting.
	searchWindow = new QwtPlotCurve("Search Window");
	QPen windowPen (Qt::darkMagenta);
	windowPen.setWidth(LINEWIDTH);
	windowPen.setJoinStyle(Qt::MiterJoin);
	searchWindow->setPen(windowPen);
	searchWindow->attach(this);
}

statPlot::~statPlot()
{
	delete profileCurve;
	delete searchWindow;
	delete gridLines;
}

void
statPlot::setProfileCurve(const QVector<QPointF>& profilePts)
{
	//This is capable of being safely called multiple times.
	//setData is inherited from QwtPlotSeriesItem<T>,
	//which deletes the pointer when done with it.
	profileCurve->setData(new QwtPointSeriesData(profilePts));
	replot();
}

void
statPlot::setProfile(Profile* profile, bool masked)
{
	if ((NULL == profile) || (profile->isNull()))
	{
		setProfileCurve(QVector<QPointF> ());
		setSearchWindow(0, 0);
		emit profileChanged(NULL, true);
	}
	else
	{
		QVector<float> depth = profile->getDepth();
		QBitArray mask = profile->getMask();
		QVector<QPointF> profilePts;
		float res = profile->getPixelSize();

		for (int i = 0; i < depth.size(); ++i)
		{
			if (masked)
			{
				if (mask.testBit(i))
					profilePts.push_back(QPointF(i*res, depth[i]));
			}
			else
				profilePts.push_back(QPointF(i*res, depth[i]));
		}

		setProfileCurve(profilePts);
		emit profileChanged(profile, masked);
	}
}

void
statPlot::setSearchWindow(int searchLoc, int searchWidth)
{
	if (profileCurve->boundingRect().isValid() &&
		(searchWidth > 0)) //If profileCurve has data.
	{
		//Make and plot box.

		//Ensure window is not out of bounds.
		int n = profileCurve->data()->size();
		int startSample;
		int endSample;
		if (searchLoc < 0)
		{
			startSample = 0;
		}
		else if (searchLoc >= n)
		{
			startSample = n - 1;
		}
		else
		{
			startSample = searchLoc;
		}

		if ((searchLoc + searchWidth) > n)
		{
			endSample = n - 1;
		}
		else
		{
			endSample = searchLoc + searchWidth - 1;
		}

		QPointF tempLowerLeft
			(profileCurve->data()->sample(startSample).x(), profileCurve->minYValue());
		QPointF tempLowerRight
			(profileCurve->data()->sample(endSample).x(), profileCurve->minYValue());
		QPointF tempUpperRight = tempLowerRight;
		QPointF tempUpperLeft = tempLowerLeft;
		tempUpperRight.setY(profileCurve->maxYValue());
		tempUpperLeft.setY(profileCurve->maxYValue());
		QVector<QPointF> boxPts;
		boxPts.push_back(tempLowerLeft);
		boxPts.push_back(tempLowerRight);
		boxPts.push_back(tempUpperRight);
		boxPts.push_back(tempUpperLeft);
		boxPts.push_back(tempLowerLeft);
		searchWindow->setData(new QwtPointSeriesData(boxPts));
		replot();
	}
	else
	{
		searchWindow->setData(new QwtPointSeriesData(QVector<QPointF> ()));
		replot();
	}
}

void
statPlot::setProfilePen(QColor penColor)
{
	curvePen.setColor(penColor);
	profileCurve->setPen(curvePen); //Reset the curve color.
	replot();
}
