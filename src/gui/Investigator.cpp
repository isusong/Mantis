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

#include "Investigator.h"
#include <QFileDialog>
#include "../core/RangeImage.h"
#include <QMdiSubWindow>
#include "TipWidget.h"
#include "PlateWidget.h"
#include <QMessageBox>

Investigator::Investigator(QWidget* parent):
    QMainWindow(parent)
{
	//Create the data objects.
	lastDirectory = QDir::currentPath();
	slotFilled_1 = false;
	slotFilled_2 = false;

	//Create the gui objects.
	center = new QWidget(this);
	mainLayout = new QVBoxLayout(center);
	area = new QMdiArea(this);
	stat = new StatisticsWidget(this);
	fileMenu = new QMenu(tr("&File"), this);
	openTipAction = new QAction(this);
	openPlateAction = new QAction(this);
	toolsMenu = new QMenu(tr("&Tools"), this);
	tileAction = new QAction(this);
	cascadeAction = new QAction(this);
	statSettingsAction = new QAction(this);

	//Title the actions.
	openTipAction->setText(tr("Open &Tip"));
	openPlateAction->setText(tr("Open Test &Mark"));
	tileAction->setText(tr("&Tile Windows"));
	cascadeAction->setText(tr("&Cascade Windows"));
	statSettingsAction->setText(tr("&Statistics Settings"));

	//Init tile and cascade off
	tileAction->setEnabled(false);
	cascadeAction->setEnabled(false);

	//Set up.
	makeConnections();
	assemble();
}

Investigator::~Investigator()
{
	//everything is parented.
}

void
Investigator::makeConnections()
{
	connect(openTipAction, SIGNAL(triggered()), this, SLOT(openTip()));
	connect(openPlateAction, SIGNAL(triggered()), this, SLOT(openPlate()));
	connect(tileAction, SIGNAL(triggered()), area, SLOT(tileSubWindows()));
	connect(cascadeAction, SIGNAL(triggered()), area, SLOT(cascadeSubWindows()));
	connect(statSettingsAction, SIGNAL(triggered()), this, 
		SLOT(activateStatSettings()));
}

void
Investigator::assemble()
{
	fileMenu->addAction(openTipAction);
	fileMenu->addAction(openPlateAction);
	toolsMenu->addAction(tileAction);
	toolsMenu->addAction(cascadeAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(statSettingsAction);
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(toolsMenu);
	mainLayout->addWidget(area);
	mainLayout->addWidget(stat);
	center->setLayout(mainLayout);
	setCentralWidget(center);
}

void
Investigator::openTip()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open .mt file"), lastDirectory,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
	fileInfo.setFile(filename);
	lastDirectory = fileInfo.absolutePath();

	//Create the new widget.
	QMdiSubWindow* window = new QMdiSubWindow(area);
	window->setAttribute(Qt::WA_DeleteOnClose);
	RangeImage* tip = new RangeImage(filename, window);
	if (tip->isNull())
	{
		//Deletes the window and the tip object.
		window->close();
		QMessageBox::warning(this, tr("Error"), 
			tr("File failed to open."));
		return;
	}
	TipWidget* tipWidget = new TipWidget(tip, window);
	window->setWidget(tipWidget);
	window->setWindowTitle(fileInfo.fileName());
	area->addSubWindow(window);
	tipWidget->show();
	area->tileSubWindows();
	assignSlot(tipWidget);
}

void
Investigator::openPlate()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open .mt file"), lastDirectory,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
	fileInfo.setFile(filename);
	lastDirectory = fileInfo.absolutePath();

	//Create the new widget.
	QMdiSubWindow* window = new QMdiSubWindow(area);
	window->setAttribute(Qt::WA_DeleteOnClose);
	RangeImage* plate = new RangeImage(filename, window);
	if (plate->isNull())
	{
		//Deletes the window and the plate object.
		window->close();
		QMessageBox::warning(this, tr("Error"), 
			tr("File failed to open."));
		return;
	}
	PlateWidget* plateWidget = new PlateWidget(plate, window);
	window->setWidget(plateWidget);
	window->setWindowTitle(fileInfo.fileName());
	area->addSubWindow(window);
	plateWidget->show();
	area->tileSubWindows();
	assignSlot(plateWidget);
}

void
Investigator::assignSlot(InvestigatorSubWidget* window)
{
	if (!slotFilled_1)
	{
		slotFilled_1 = true;
		connect(window, SIGNAL(profileChanged(Profile*, bool)),
			stat, SLOT(setPlotData_1(Profile*)));
		connect(stat, SIGNAL(searchWindowUpdated_1(int, int)),
			window, SLOT(setSearchWindow(int, int)));
		connect(window, SIGNAL(destroyed()),
			this, SLOT(emptySlot_1()));
	}
	else if (!slotFilled_2)
	{
		slotFilled_2 = true;
		connect(window, SIGNAL(profileChanged(Profile*, bool)),
			stat, SLOT(setPlotData_2(Profile*)));
		connect(stat, SIGNAL(searchWindowUpdated_2(int, int)),
			window, SLOT(setSearchWindow(int, int)));
		connect(window, SIGNAL(destroyed()),
			this, SLOT(emptySlot_2()));
	}
	connect(window, SIGNAL(statusMessage(QString)), 
		this, SLOT(postStatusMessage(QString)));
	updateEnabledStatus();
}

void
Investigator::updateEnabledStatus()
{
	//If two windows are open, you can't open any new tips or marks
	//unless you close a tip or a plate and free up the slot.
	if (slotFilled_1 && slotFilled_2)
	{
		openTipAction->setEnabled(false);
		openPlateAction->setEnabled(false);
	}
	else
	{
		openTipAction->setEnabled(true);
		openPlateAction->setEnabled(true);
	}

	//You can't tile or cascade windows unless you have some.
	if (slotFilled_1 || slotFilled_2)
	{
		tileAction->setEnabled(true);
		cascadeAction->setEnabled(true);
	}
	else
	{
		tileAction->setEnabled(false);
		cascadeAction->setEnabled(false);
	}
}

void
Investigator::emptySlot_1()
{
	slotFilled_1 = false;
	stat->setPlotData_1(NULL);
	updateEnabledStatus();
}

void
Investigator::emptySlot_2()
{
	slotFilled_2 = false;
	stat->setPlotData_2(NULL);
	updateEnabledStatus();
}

void
Investigator::postStatusMessage(QString msg)
{
	statusBar()->showMessage(msg);
	QCoreApplication::processEvents();
}

void
Investigator::activateStatSettings()
{
	stat->setSettingsVisibility(true);
}
