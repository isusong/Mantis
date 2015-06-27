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

#ifndef __STATPLOTANGT_H__
#define __STATPLOTANGT_H__

#define QWT_DLL

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtGui/QPen>
#include "StatScaleDraw.h"
#include <memory>

/**
 * Provides a plot of a profile and its
 * associated statistics search window.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Brian Bailey www.code-hammer.com
 */

class StatPlotAngT: public QwtPlot
{
    Q_OBJECT

    public:
        StatPlotAngT(QWidget *parent = 0);
        StatPlotAngT(const QSize &szHint, const QSize &szHintMin, QWidget *parent = 0);
        virtual ~StatPlotAngT();

        virtual QSize sizeHint() const;
        virtual QSize minimumSizeHint() const;

        void setPlotPts(const QVector<QPointF>& profilePts);
        void setPlotPen(const QColor &penColor);

    public slots:

    signals:

    protected:
        void init();

    protected:
        bool _overrideSizeHints;
        QSize _szHint;
        QSize _szHintMin;
        StatScaleDraw *_scaleDrawX;

        std::tr1::shared_ptr<QwtPlotCurve> _plot;
        QPen _plotPen;
        std::tr1::shared_ptr<QwtPlotGrid> _gridLines;

};
#endif
