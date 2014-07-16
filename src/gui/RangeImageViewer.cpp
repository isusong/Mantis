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

#include "RangeImageViewer.h"
#include <QIcon>
#include <QColorDialog>

RangeImageViewer::RangeImageViewer(RangeImage* rangeImage, 
	int flatDimension, int w, int h, QWidget* parent):
	QWidget(parent)
{
	width = w;
	height = h;
	mainGrid = new QGridLayout(this);
	graphics = new GraphicsWidget(this);
	renderer = new RangeImageRenderer(rangeImage, this);
	viewTools = new QToolBar(this);
	actionGroup = new QActionGroup(viewTools);
	shaded = new QAction(this);
	wireframe = new QAction(this);
	textured = new QAction(this);
	heightMapped = new QAction(this);
	signalMapper = new QSignalMapper(this);
	localCsys = new QAction(this);
	worldCsys = new QAction(this);
	background = new QAction(this);

	//Adjust for flat dimension.
	setFlatDimension(flatDimension);

	initActions();
	makeConnections();
	assemble();

	//Some start up defaults.
	graphics->setDrawCS(false);
	renderer->setDrawCS(false);
	renderer->setDrawMode(2); //start in textured.
	graphics->updateGL();
}

RangeImageViewer::~RangeImageViewer()
{
	//Everything is parented.
}

void
RangeImageViewer::initActions()
{
	//Set up the actions.
	//Shaded button.
	shaded->setCheckable(true);
	shaded->setActionGroup(actionGroup);
	shaded->setIcon(QIcon(":/shading_modes/Icons/smooth.png"));
	shaded->setToolTip(tr("Shaded view"));
	signalMapper->setMapping(shaded, 0);

	//Wireframe button
	wireframe->setCheckable(true);
	wireframe->setActionGroup(actionGroup);
	wireframe->setIcon(QIcon(":/shading_modes/Icons/wire.png"));
	wireframe->setToolTip(tr("Wireframe view"));
	signalMapper->setMapping(wireframe, 1);
	
	//Textured button.
	textured->setCheckable(true);
	textured->setActionGroup(actionGroup);
	textured->setIcon(QIcon(":/shading_modes/Icons/textures.png"));
	textured->setToolTip(tr("Textured view"));
	signalMapper->setMapping(textured, 2);
	textured->setChecked(true);

	//Height map button.
	heightMapped->setCheckable(true);
	heightMapped->setActionGroup(actionGroup);
	heightMapped->setIcon(QIcon(":/shading_modes/Icons/color.png"));
	heightMapped->setToolTip(tr("Color-mapped view"));
	signalMapper->setMapping(heightMapped, 3);

	//Local Csys button.
	localCsys->setCheckable(true);
	localCsys->setIcon(QIcon(":/controls/Icons/local-csys.png"));
	localCsys->setToolTip(tr("Local coordinate system on/off"));
	localCsys->setChecked(false);

	//World Csys button.
	worldCsys->setCheckable(true);
	worldCsys->setIcon(QIcon(":/controls/Icons/world-csys.png"));
	worldCsys->setToolTip(tr("World coordinate system on/off"));
	worldCsys->setChecked(false);

	//Background selection button
	background->setIcon(QIcon(":/controls/Icons/color-wheel.png"));
	background->setToolTip(tr("Change background color"));
}

void
RangeImageViewer::makeConnections()
{
	connect(shaded, SIGNAL(triggered()), signalMapper,
		SLOT(map()));
	connect(wireframe, SIGNAL(triggered()), signalMapper,
		SLOT(map()));
	connect(textured, SIGNAL(triggered()), signalMapper,
		SLOT(map()));
	connect(heightMapped, SIGNAL(triggered()), signalMapper,
		SLOT(map()));
	connect(localCsys, SIGNAL(toggled(bool)), renderer,
		SLOT(setDrawCS(bool)));
	connect(worldCsys, SIGNAL(toggled(bool)), graphics,
		SLOT(setDrawCS(bool)));
	connect(background, SIGNAL(triggered()), this,
		SLOT(selectBackgroundColor()));
	connect(renderer, SIGNAL(updateGL()), graphics, SLOT(updateGL()));
	connect(signalMapper, SIGNAL(mapped(int)), 
		renderer, SLOT(setDrawMode(int)));
	connect(graphics, SIGNAL(doubleClicked(int, int)),
		this, SIGNAL(doubleClicked(int, int)));
}

void
RangeImageViewer::assemble()
{
	viewTools->setOrientation(Qt::Vertical);
	viewTools->addAction(shaded);
	viewTools->addAction(wireframe);
	viewTools->addAction(textured);
	viewTools->addAction(heightMapped);
	viewTools->addSeparator();
	viewTools->addAction(localCsys);
	viewTools->addAction(worldCsys);
	viewTools->addSeparator();
	viewTools->addAction(background);
	graphics->setModel(renderer);
	graphics->setSizePolicy(QSizePolicy::MinimumExpanding,
		QSizePolicy::MinimumExpanding);
	mainGrid->addWidget(viewTools, 0, 0);
	mainGrid->addWidget(graphics, 0, 1);
	this->setLayout(mainGrid);
	this->resize(width, height);
}

void
RangeImageViewer::selectBackgroundColor()
{
	QColor selection = QColorDialog::getColor(
		"black", this, tr("Select Background Color"));
	if (selection.isValid())
		graphics->setBackgroundColor(selection);
}

void
RangeImageViewer::setFlatDimension(int flatDimension)
{
	//Adjust for flat dimension.
	switch (flatDimension)
	{
		case 0:
			graphics->setMouseMultipliers(QVector3D(6, 1, 1));
			break;
		case 1:
			graphics->setMouseMultipliers(QVector3D(1, 6, 1));
			break;
		case 2:
			graphics->setMouseMultipliers(QVector3D(1, 1, 6));
	}
}

void
RangeImageViewer::rotate(float xAxis, float yAxis, float zAxis)
{
	renderer->setTransformMatrix(QMatrix4x4());
	renderer->rotate(zAxis, QVector3D(0, 0, 1)); //z roll
	renderer->rotate(yAxis, QVector3D(0, 1, 0)); //y yaw
	renderer->rotate(xAxis, QVector3D(1, 0, 0)); //x pitch
	graphics->updateGL();
}
