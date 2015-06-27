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

#include "InvestigatorSubWidget.h"
#include <qwt_text.h>

//=======================================================================
//=======================================================================
InvestigatorSubWidget::InvestigatorSubWidget(PRangeImage rangeImage,
	QWidget *parent):
	QWidget(parent)
{
	//Create the data objects.
    //profile = NULL; //no virtual mark or cross-section yet.

	//Create the gui objects.
	grid = new QGridLayout(this);
	horizontalSplitter = new QSplitter(this);
    graphics = new RangeImageViewer(rangeImage, false, true, -1, 330, 250, this);
	verticalSplitter = new QSplitter(Qt::Vertical, this);
    plot = new StatPlot(this);
	controls = new QGroupBox(this);
	flipButton = new QToolButton(controls);

	//Graphics setup.
	graphics->setSizePolicy(QSizePolicy::MinimumExpanding,
		QSizePolicy::MinimumExpanding);

	//Splitters setup.
	horizontalSplitter->setChildrenCollapsible(false);
	verticalSplitter->setChildrenCollapsible(false);

	//Plot setup.
	//This keeps the axis labels from running into each other.
	plot->setAxisMaxMajor(QwtPlot::xBottom, 5);
	//Title the axes.
	plot->setAxisTitle(QwtPlot::xBottom, 
		QwtText("x (&mu;m)", QwtText::RichText));
	plot->setAxisTitle(QwtPlot::yLeft, 
		QwtText("z (&mu;m)", QwtText::RichText));
	plot->setSizePolicy(QSizePolicy::MinimumExpanding,
		QSizePolicy::Preferred);
	plot->setEnabled(false); //gray it out. No data yet.

	//Controls setup.
	//flip button
	flipButton->setIcon(QIcon(":/controls/Icons/fliplr.png"));
	flipButton->setToolTip(tr("Flip mark"));
	flipButton->setIconSize(QSize(24, 24));
	flipButton->setEnabled(false);
	controls->setSizePolicy(QSizePolicy::Maximum,
		QSizePolicy::Maximum);

	makeConnections();
	assemble();
}

//=======================================================================
//=======================================================================
InvestigatorSubWidget::~InvestigatorSubWidget()
{
    //delete profile;
	//everything else is parented and will get auto-deleted.
}

//=======================================================================
//=======================================================================
PRangeImage InvestigatorSubWidget::getRangeImage()
{
    if (!graphics) return PRangeImage();
    if (!graphics->getRenderer()) return PRangeImage();

    return graphics->getRenderer()->getModel();
}

//=======================================================================
//=======================================================================
void InvestigatorSubWidget::makeConnections()
{
	connect(flipButton, SIGNAL(pressed()), this, SLOT(flip()));

	//Forward plot changes.
    connect(plot, SIGNAL(profileChanged(PProfile, bool)), this, SIGNAL(profileChanged(PProfile, bool)));
}

//=======================================================================
//=======================================================================
void InvestigatorSubWidget::assemble()
{
	//Assemble widget.
	verticalSplitter->addWidget(controls);
	verticalSplitter->addWidget(plot);
	QWidget* spacer = new QWidget(this);
	spacer->resize(20, 40);
	verticalSplitter->addWidget(spacer);
	horizontalSplitter->addWidget(graphics);
	horizontalSplitter->addWidget(verticalSplitter);
	grid->addWidget(horizontalSplitter);
	setLayout(grid);
}

//=======================================================================
//=======================================================================
void InvestigatorSubWidget::stalePlots()
{
    //plot->setProfile(NULL);
    plot->setProfile(PProfile());
	plot->setSearchWindow(0, 0);
	flipButton->setEnabled(false);
	plot->setEnabled(false);
}

//=======================================================================
//=======================================================================
void InvestigatorSubWidget::setSearchWindow(int loc, int width, int dataLen)
{
    Q_UNUSED(dataLen);

    if (NULL != _profile)
    {
		plot->setSearchWindow(loc, width);
    }
}

//=======================================================================
//=======================================================================
void InvestigatorSubWidget::flip()
{
    if (NULL != _profile)
	{
        Profile* flipped = _profile->flip();
        //delete profile;
        //profile = flipped;
        _profile.reset(flipped);
		plot->setSearchWindow(0, 0);
        plot->setProfile(_profile);
	}
}
