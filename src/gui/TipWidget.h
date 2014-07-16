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

#ifndef __TIPWIDGET_H__
#define __TIPWIDGET_H__
#include "InvestigatorSubWidget.h"
#include "../core/VirtualTip.h"
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>

/**
 * A Qt gui widget for interfacing with tool tip
 * files.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Laura Ekstrand
 */

class TipWidget: public InvestigatorSubWidget
{
	Q_OBJECT

	//Data objects.
	VirtualTip* tip; ///< Tip for making virtual marks.

	//Gui Objects.
	QGridLayout* controlsLayout;
	QLabel* xlabel;
	QLabel* ylabel;
	QLabel* zlabel;
	QDoubleSpinBox* xbox;
	QDoubleSpinBox* ybox;
	QDoubleSpinBox* zbox;
	QToolButton* markButton; ///< Makes virtual mark.

	//Private member functions
	///Make the GUI connections.
	void makeConnections();
	///Assemble the GUI.
	void assemble();

public:
    TipWidget(PRangeImage rangeImage, QWidget *parent = 0);
	virtual ~TipWidget();

public slots:
	void mark(); ///< Makes the virtual mark.
	void rotateModel(); ///< Updates the model rotation.
};
#endif //!defined __TIPWIDGET_H__
