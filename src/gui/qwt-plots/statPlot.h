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

#ifndef __STATPLOT_H__
#define __STATPLOT_H__

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtGui/QPen>
#include "../core/Profile.h"

/**
 * Provides a plot of a profile and its 
 * associated statistics search window.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Laura Ekstrand
 */

class statPlot: public QwtPlot
{
	Q_OBJECT

	QwtPlotCurve* profileCurve;
	QPen curvePen;
	QwtPlotCurve* searchWindow;
	QwtPlotGrid* gridLines;

	public:
		statPlot(QWidget *parent = 0);
		virtual ~statPlot();
	
	public slots:
		void setProfileCurve(const QVector<QPointF>& profilePts);
		///Display Profile. mask indicates whether to mask data.
		/**
		 * Note: If masked = true, the masked-out points simply
		 * won't be added to the plot data. Therefore,
		 * instead of seeing a gap there, you will get interpolation
		 * between the two nearest "on" points.
		 */
		void setProfile(Profile* profile, bool masked = true);
		void setSearchWindow(int searchLoc, int searchWidth);
		void setProfilePen(QColor penColor);
	
	signals:
		///Announce that the profile has changed.
		void profileChanged(Profile* profile, bool masked);
};
#endif // ! defined __STATPLOT_H__
