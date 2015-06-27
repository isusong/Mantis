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
 * Author: Brian Bailey (www.code-hammer.com)
 */

#include "StatPlotAngT.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <QPalette>

#define LINEWIDTH 2

//=======================================================================
//=======================================================================
StatPlotAngT::StatPlotAngT(QWidget* parent) : QwtPlot(parent),
    _overrideSizeHints(false)
{
    init();
}

//=======================================================================
//=======================================================================
StatPlotAngT::StatPlotAngT(const QSize &szHint, const QSize &szHintMin, QWidget *parent) : QwtPlot(parent),
    _overrideSizeHints(true),
    _szHint(szHint),
    _szHintMin(szHintMin)
{
    init();
}

//=======================================================================
//=======================================================================
StatPlotAngT::~StatPlotAngT()
{
}

//=======================================================================
//=======================================================================
void StatPlotAngT::init()
{
    _scaleDrawX = new StatScaleDraw();
    setAxisScaleDraw(xBottom, _scaleDrawX);


    //Plot-wide settings.
    //Init panning and magnifying.
    //new QwtPlotPanner( canvas() ); //Inherits from QWidget.
    //new QwtPlotMagnifier( canvas() ); //Inherits from QObject.

    //Style the plot box
    QFrame *frame = qobject_cast<QFrame *>(canvas());
    if (frame)
    {
        frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        frame->setObjectName("plotFrame");
    }

    //Title the axes.
    setAxisTitle(xBottom, tr("Angle"));
    setAxisTitle(yLeft, tr("T1"));
    //Make grid lines.
    _gridLines.reset(new QwtPlotGrid());
    QPen gridPen (Qt::lightGray);
    gridPen.setStyle(Qt::DashLine);
    _gridLines->setPen(gridPen);
    _gridLines->attach(this);

    //Set up profile plotting.
    _plot.reset( new QwtPlotCurve("Angle Vs T1 Curve"));
    _plot->setRenderHint(QwtPlotItem::RenderAntialiased);
    _plotPen.setColor(Qt::red);
    _plotPen.setWidth(LINEWIDTH);
    _plot->setPen(_plotPen);
    _plot->attach(this);
}

//=======================================================================
//=======================================================================
QSize StatPlotAngT::sizeHint() const
{
    if (_overrideSizeHints)
    {
        return _szHint;
    }
    else
    {
        return QwtPlot::sizeHint();
    }
}

//=======================================================================
//=======================================================================
QSize StatPlotAngT::minimumSizeHint() const
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
}

//=======================================================================
//=======================================================================
void StatPlotAngT::setPlotPts(const QVector<QPointF>& profilePts)
{
    _plot->setData(new QwtPointSeriesData(profilePts));
    replot();
}

//=======================================================================
//=======================================================================
void StatPlotAngT::setPlotPen(const QColor &penColor)
{
    _plotPen.setColor(penColor);
    _plot->setPen(_plotPen);
    replot();
}
