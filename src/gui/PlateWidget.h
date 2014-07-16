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

#ifndef __PLATEWIDGET_H__
#define __PLATEWIDGET_H__
#include "InvestigatorSubWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

/**
 * A Qt gui widget for interfacing with test mark (plate) files.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Laura Ekstrand
 */

class PlateWidget: public InvestigatorSubWidget
{
	Q_OBJECT
	
	//Data Objects.
	///Pointer to 3D data.  Not owned by this class.
    PRangeImage plate;

	//Gui Objects.
	QGridLayout* controlsLayout;
	QLabel* label;
	QSpinBox* spinBox;

	//Private member functions
	///Make the GUI connections.
	void makeConnections();
	///Assemble the GUI.
	void assemble();

public:
    PlateWidget(PRangeImage rangeImage, QWidget *parent = 0);
	virtual ~PlateWidget();

public slots:
	void updatePlot();
	void updateSpinBox();
	///This redraws the selection when the user changes the spin box.
	/**
	 * To avoid signal loops, this ensures that the selected column
	 * is different from the currently selected column.
	 */
	void redrawSelection(int j);
	///Turn on the selection for the first double click.
	void activateSelection();
};
#endif //!defined __PLATEWIDGET_H__
