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

#include "StatPlot.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_scale_widget.h>
#include <QPalette>

#define LINEWIDTH 2

//=======================================================================
//=======================================================================
StatPlot::StatPlot(QWidget* parent) : QwtPlot(parent),
    _overrideSizeHints(false)
{
    init();
}

//=======================================================================
//=======================================================================
StatPlot::StatPlot(const QSize &szHint, const QSize &szHintMin, QWidget *parent) : QwtPlot(parent),
    _overrideSizeHints(true),
    _szHint(szHint),
    _szHintMin(szHintMin)
{
    init();
}

//=======================================================================
//=======================================================================
StatPlot::~StatPlot()
{
}

//=======================================================================
//=======================================================================
void StatPlot::init()
{
    /*
    QSize cshint = canvas()->sizeHint();
    QSize csize = canvas()->size();
    QSize cmsizehint = canvas()->minimumSizeHint();
    QSize cminsize = canvas()->minimumSize();
    canvas()->setMinimumSize(cminsize.width() * .5, cminsize.height() *.5);
    canvas()->resize(cminsize.width() * .5, cminsize.height() *.5);


    QSize shint = this->sizeHint();
    QSize size = this->size();
    QSize msizehint = this->minimumSizeHint();
    QSize minsize = this->minimumSize();
    canvas()->setMinimumSize(minsize.width() * .5, minsize.height() *.5);
    canvas()->resize(minsize.width() * .5, minsize.height() *.5);
    */

    _scaleDrawX = new StatScaleDraw();
    setAxisScaleDraw(xBottom, _scaleDrawX);


    //Plot-wide settings.
    //Init panning and magnifying.
    new QwtPlotPanner( canvas() ); //Inherits from QWidget.
    new QwtPlotMagnifier( canvas() ); //Inherits from QObject.

    //Style the plot box
    QFrame *frame = qobject_cast<QFrame *>(canvas());
    if (frame)
    {
        frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        frame->setObjectName("plotFrame");
        //frame->setStyleSheet("#plotFrame { border: 1px solid red; }");
        //QPalette* palette = new QPalette();
        //palette->setColor(QPalette::Foreground, Qt::green);
        //frame->setPalette(*palette);
    }

    //Title the axes.
    setAxisTitle(xBottom, tr("x"));
    setAxisTitle(yLeft, tr("z"));
    //Make grid lines.
    _gridLines.reset(new QwtPlotGrid());
    QPen gridPen (Qt::lightGray);
    gridPen.setStyle(Qt::DashLine);
    _gridLines->setPen(gridPen);
    _gridLines->attach(this);

    //Set up profile plotting.
    _profileCurve.reset( new QwtPlotCurve("Mark Profile Curve"));
    _profileCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    _curvePen.setColor(Qt::red); //Default pen color is red.
    _curvePen.setWidth(LINEWIDTH);
    _profileCurve->setPen(_curvePen);
    _profileCurve->attach(this); //Associate the curve with the plot.

    //Set up search window plotting.
    _searchWindow.reset(new QwtPlotCurve("Search Window"));
    _searchPen.setColor(Qt::darkMagenta);
    _searchPen.setWidth(LINEWIDTH);
    _searchPen.setJoinStyle(Qt::MiterJoin);
    _searchWindow->setPen(_searchPen);
    _searchWindow->attach(this);
}

//=======================================================================
//=======================================================================
QSize StatPlot::sizeHint() const
{
    // original values
    //return QSize(443, 433);

    if (_overrideSizeHints)
    {
        return _szHint;
    }
    else
    {
        return QwtPlot::sizeHint();
    }

    //return QSize(358, 268);
}

//=======================================================================
//=======================================================================
QSize StatPlot::minimumSizeHint() const
{
    // original values
    //return QSize(258, 168);
    if (_overrideSizeHints)
    {
        return _szHintMin;
    }
    else
    {
        return QwtPlot::minimumSizeHint();
    }

    //return QSize(358, 268);
}

//=======================================================================
//=======================================================================
void StatPlot::setProfileCurve(const QVector<QPointF>& profilePts)
{
	//This is capable of being safely called multiple times.
	//setData is inherited from QwtPlotSeriesItem<T>,
	//which deletes the pointer when done with it.
    _profileCurve->setData(new QwtPointSeriesData(profilePts));
	replot();
}

//=======================================================================
//=======================================================================
void StatPlot::setProfile(PProfile profile, bool masked)
{
	if ((NULL == profile) || (profile->isNull()))
	{
		setProfileCurve(QVector<QPointF> ());
		setSearchWindow(0, 0);
        emit profileChanged(PProfile(), true);
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

//=======================================================================
//=======================================================================
void StatPlot::setSearchWindow(int searchLoc, int searchWidth)
{
    if (_profileCurve->boundingRect().isValid() &&
		(searchWidth > 0)) //If profileCurve has data.
	{
		//Make and plot box.

		//Ensure window is not out of bounds.
        int n = _profileCurve->data()->size();
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
            (_profileCurve->data()->sample(startSample).x(), _profileCurve->minYValue());
		QPointF tempLowerRight
            (_profileCurve->data()->sample(endSample).x(), _profileCurve->minYValue());
		QPointF tempUpperRight = tempLowerRight;
		QPointF tempUpperLeft = tempLowerLeft;
        tempUpperRight.setY(_profileCurve->maxYValue());
        tempUpperLeft.setY(_profileCurve->maxYValue());
		QVector<QPointF> boxPts;
		boxPts.push_back(tempLowerLeft);
		boxPts.push_back(tempLowerRight);
		boxPts.push_back(tempUpperRight);
		boxPts.push_back(tempUpperLeft);
		boxPts.push_back(tempLowerLeft);
        _searchWindow->setData(new QwtPointSeriesData(boxPts));
		replot();
	}
	else
	{
        _searchWindow->setData(new QwtPointSeriesData(QVector<QPointF> ()));
		replot();
	}
}

//=======================================================================
//=======================================================================
void StatPlot::setProfilePen(const QColor &penColor)
{
    _curvePen.setColor(penColor);
    _profileCurve->setPen(_curvePen);
	replot();
}

//=======================================================================
//=======================================================================
void StatPlot::setSearchPen(const QColor &penColor)
{
    _searchPen.setColor(penColor);
    _searchWindow->setPen(_searchPen);
    replot();
}

//=======================================================================
//=======================================================================
void StatPlot::setAxisTextX(const QString &text)
{
    setAxisTitle(xBottom, text);
}

//=======================================================================
//=======================================================================
void StatPlot::setAxisColor(const QColor &c)
{
    setAxisColorX(c);
    setAxisColorY(c);
}

//=======================================================================
//=======================================================================
void StatPlot::setAxisColorX(const QColor &c)
{
    setAxisColor(xBottom, c);
}

//=======================================================================
//=======================================================================
void StatPlot::setAxisColorY(const QColor &c)
{
    setAxisColor(yLeft, c);
}

//=======================================================================
//=======================================================================
void StatPlot::setAxisColor(int axis, const QColor &c)
{
    // color the axis
    QPalette pal = axisWidget(axis)->palette();
    pal.setColor(QPalette::Foreground, c);
    axisWidget(axis)->setPalette(pal);
}
