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

#ifndef __RANGEIMAGEVIEWER_H__
#define __RANGEIMAGEVIEWER_H__
#include <QWidget>
#include <QGridLayout>
#include "GraphicsWidget.h"
#include "RangeImageRenderer.h"
#include <QToolBar>
#include <QActionGroup>
#include <QAction>
#include <QSignalMapper>
#include "../core/RangeImage.h"

class RangeImageViewer: public QWidget
{
	Q_OBJECT

	int width;
	int height;
	QGridLayout* mainGrid; 
	GraphicsWidget* graphics;
	RangeImageRenderer* renderer;
	QToolBar* viewTools;

	//Shading modes group
	QActionGroup* actionGroup;
	QAction* shaded;
	QAction* wireframe;
	QAction* textured;
	QAction* heightMapped;
	QSignalMapper* signalMapper;

	//Csys buttons.
	QAction* localCsys;
	QAction* worldCsys;

	//Background color button.
	QAction* background;

	void initActions();
	void makeConnections();
	void assemble();

  public:
	///Create a viewer for a RangeImage.
	/**
	 * Will not delete rangeImage.
	 *
	 * flatDimension specifies if a particular dimension of the object
	 * is known to be very thin.
	 * For instance, for a plate, z is very small.  In this
	 * case, pass flatDimension = 2; this will make navigation speed up
	 * in the z direction so zoom in does not take forever.
	 * If no dimensions are flat, pass in -1 or a value that is not
	 * 0, 1, or 2.
	 */
	RangeImageViewer(RangeImage* rangeImage, int flatDimension = -1, 
		int w = 500, int h = 500, QWidget* parent = 0);
	virtual ~RangeImageViewer();

  public slots:
	///Change the background color for the render.
	void selectBackgroundColor();
	///Change the flat dimension.
	/**
	 * flatDimension specifies if a particular dimension of the object
	 * is known to be very thin.
	 * For instance, for a plate, z is very small.  In this
	 * case, pass flatDimension = 2; this will make navigation speed up
	 * in the z direction so zoom in does not take forever.
	 * If no dimensions are flat, pass in -1 or a value that is not
	 * 0, 1, or 2.
	 */
	void setFlatDimension(int flatDimension);
	///Rotate the Range Image relative to the world in the display.
	/**
	 * xAxis, yAxis, zAxis are rotations about the respective axes
	 * in degrees. These are applied in PYR order: z, then y, then x.
	 */
	void rotate(float xAxis, float yAxis, float zAxis);
	//Pass throughs for Selection object.
	inline QPointF& getBasis() {return renderer->getBasis();}
	///Use with double clicked signal. Schedules an "unprojection."
	inline void updateSelection(int winX, int winY)
		{renderer->scheduleSelectionUpdate(winX, winY);}
	///Use with spin box update.  Only schedules a selection redraw.
	inline void redrawSelection(float x, float y)
		{renderer->redrawSelection(x, y);}
	inline void setSelectionEnabled(bool status)
		{renderer->setSelectionEnabled(status);}
	inline void setSelectionMode(Selection::drawModes mode)
		{renderer->setSelectionMode(mode);}
	inline void setSelectionMultiplier(int mult)
		{renderer->setSelectionMultiplier(mult);}

  signals:
	///Coordinates where user clicked on 3D window.
	void doubleClicked(int x, int y);
};

#endif //!defined __RANGEIMAGEVIEWER_H__
