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

#include "PlateWidget.h"
#include <cmath>

#define EPS 1

PlateWidget::PlateWidget(PRangeImage rangeImage, QWidget *parent):
	InvestigatorSubWidget(rangeImage, parent)
{
	plate = rangeImage;

	//Create the gui objects.
	controlsLayout = new QGridLayout(controls);
	label = new QLabel(tr("Selected Col:"), controls);
	spinBox = new QSpinBox(controls);

	//RangeImageViewer setup.
	graphics->setFlatDimension(2);
	graphics->setSelectionMode(Selection::colPlane);
	graphics->setSelectionMultiplier(5);
	graphics->setSelectionEnabled(false);

	//Controls setup.
	controls->setTitle(tr("Test Mark Controls"));
	spinBox->setValue(0);
	spinBox->setMinimum(0);
	spinBox->setMaximum(rangeImage->getWidth());
	spinBox->setSingleStep(1);

	makeConnections();
	assemble();
}

PlateWidget::~PlateWidget()
{
	//everything is parented and will get auto-deleted.
}

void
PlateWidget::makeConnections()
{
	connect(graphics, SIGNAL(doubleClicked(int, int)), 
		this, SLOT(activateSelection()));
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(activateSelection()));
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePlot()));
	connect(graphics, SIGNAL(doubleClicked(int, int)), graphics,
		SLOT(updateSelection(int, int)));
	connect(graphics, SIGNAL(doubleClicked(int, int)), this,
		SLOT(updateSpinBox()));
	connect(spinBox, SIGNAL(valueChanged(int)), this, 
		SLOT(redrawSelection(int)));
}

void
PlateWidget::assemble()
{
	//Assemble controls.
	controlsLayout->addWidget(label, 0, 0);
	controlsLayout->addWidget(spinBox, 0, 1);
	controlsLayout->addWidget(flipButton, 0, 2);
	QWidget* hspacer = new QWidget(this);
	hspacer->resize(0, 0);
	hspacer->setSizePolicy(QSizePolicy::MinimumExpanding,
		QSizePolicy::Maximum);
	controlsLayout->addWidget(hspacer, 0, 3);
	QWidget* vspacer = new QWidget(this);
	vspacer->resize(0, 0);
	vspacer->setSizePolicy(QSizePolicy::Maximum, 
		QSizePolicy::MinimumExpanding);
	controlsLayout->addWidget(vspacer, 1, 0, 1, -1);
	controls->setLayout(controlsLayout);
}

void
PlateWidget::updatePlot()
{
	stalePlots();
    //delete profile;
    Profile *profile = plate->getColumn(spinBox->value());
    _profile.reset(profile);
	plot->setEnabled(true);
    plot->setProfile(_profile);
	flipButton->setEnabled(true);
}

void
PlateWidget::updateSpinBox()
{
	QPointF selection = graphics->getBasis();
	spinBox->setValue(selection.x()/plate->getPixelSizeX());
}

void 
PlateWidget::redrawSelection(int j)
{
	QPointF selection = graphics->getBasis();

	//Only redraw if significantly different than current x.
	if (fabs(j*plate->getPixelSizeX() - selection.x()) > EPS)
	{
		graphics->redrawSelection(j*plate->getPixelSizeX(), 
			selection.y());
	}
}

void
PlateWidget::activateSelection()
{
	//Turn on selection.
	graphics->setSelectionEnabled(true);
	//Make sure this is never called again.
	disconnect(graphics, SIGNAL(doubleClicked(int, int)), 
		this, SLOT(activateSelection()));
	disconnect(spinBox, SIGNAL(valueChanged(int)), 
		this, SLOT(activateSelection()));
}
