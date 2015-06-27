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

#include "TipWidget.h"

TipWidget::TipWidget(PRangeImage rangeImage, QWidget *parent):
	InvestigatorSubWidget(rangeImage, parent)
{
	//Create the data objects.
    tip = new VirtualTip(rangeImage.data(), NULL, NULL, this);

    //Create the gui objects.
	controlsLayout = new QGridLayout(controls);
	xlabel = new QLabel(tr("x-Axis"), controls);
	ylabel = new QLabel(tr("y-Axis"), controls);
	zlabel = new QLabel(tr("z-Axis"), controls);
	xbox = new QDoubleSpinBox(controls);
	ybox = new QDoubleSpinBox(controls);
	zbox = new QDoubleSpinBox(controls);
	markButton = new QToolButton(controls);

	//Controls setup.
	controls->setTitle(tr("Virtual Mark Controls"));
	//x box
	xbox->setValue(0);
	xbox->setDecimals(1);
	xbox->setMinimum(-90);
	xbox->setMaximum(90);
	xbox->setSingleStep(10);
	//y box
	ybox->setValue(0);
	ybox->setDecimals(1);
	ybox->setMinimum(-90);
	ybox->setMaximum(90);
	ybox->setSingleStep(10);
	//z box
	zbox->setValue(0);
	zbox->setDecimals(1);
	zbox->setMinimum(-90);
	zbox->setMaximum(90);
	zbox->setSingleStep(10);
	//mark button
	markButton->setIcon(QIcon(":/controls/Icons/makeMark.png"));
	markButton->setToolTip(tr("Make mark"));
	markButton->setIconSize(QSize(24, 24));

	makeConnections();
	assemble();
}

TipWidget::~TipWidget()
{
	//everything is parented and will get auto-deleted.
}

void
TipWidget::makeConnections()
{
	connect(markButton, SIGNAL(clicked()), this, SLOT(mark()));
	connect(zbox, SIGNAL(valueChanged(double)), this, SLOT(rotateModel()));
	connect(ybox, SIGNAL(valueChanged(double)), this, SLOT(rotateModel()));
	connect(xbox, SIGNAL(valueChanged(double)), this, SLOT(rotateModel()));
}

void
TipWidget::assemble()
{
	//Assemble controls.
	controlsLayout->addWidget(xlabel, 0, 0);
	controlsLayout->addWidget(xbox, 0, 1);
	controlsLayout->addWidget(ylabel, 0, 2);
	controlsLayout->addWidget(ybox, 0, 3);
	controlsLayout->addWidget(zlabel, 0, 4);
	controlsLayout->addWidget(zbox, 0, 5);
	controlsLayout->addWidget(markButton, 0, 6);
	controlsLayout->addWidget(flipButton, 0, 7);
	QWidget* hspacer = new QWidget(this);
	hspacer->resize(0, 0);
	hspacer->setSizePolicy(QSizePolicy::MinimumExpanding,
		QSizePolicy::Maximum);
	controlsLayout->addWidget(hspacer, 0, 8);
	QWidget* vspacer = new QWidget(this);
	vspacer->resize(0, 0);
	vspacer->setSizePolicy(QSizePolicy::Maximum, 
		QSizePolicy::MinimumExpanding);
    controlsLayout->addWidget(vspacer, 1, 0, 1, -1);
	controls->setLayout(controlsLayout);
}

void
TipWidget::mark()
{
	emit statusMessage("Marking.  This may take some time....");
	stalePlots();
    //delete profile;
    Profile *profile = tip->mark(xbox->value(), ybox->value(), zbox->value());
    _profile.reset(profile);
	plot->setEnabled(true);
    plot->setProfile(_profile);
	flipButton->setEnabled(true);
	emit statusMessage("");
}

void
TipWidget::rotateModel()
{
	graphics->rotate(xbox->value(), ybox->value(), zbox->value());
}
