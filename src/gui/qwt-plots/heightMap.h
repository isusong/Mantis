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

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_matrix_raster_data.h>
#include <QtCore/QPointF>
#include <QtCore/QVector>

/**
 * Provides a plot of a profile's height values
 * as indices into a grayscale colormap. This
 * is to simulate viewing a mark with a 
 * confocal microscope.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Laura Ekstrand
 */

class heightMap : public QwtPlot
{
    Q_OBJECT

	QwtPlotSpectrogram* profileCurve;
	QwtScaleWidget* rightAxis;
	QwtMatrixRasterData* heightData;
	QwtInterval zInterval;

public:
    heightMap(QWidget* parent = 0);
    ~heightMap();

public slots:
	void setProfileCurve(const QVector<QPointF>& profilePts, double zMin, double zMax);
};

#endif // HEIGHTMAP_H
