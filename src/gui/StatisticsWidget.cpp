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

#include "StatisticsWidget.h"
#include <QMessageBox>
#include <stdexcept>

#define TSAMPLES_DEFAULT 200 //Ru He's number.
#define SEARCH_DEFAULT 300 //Amy's default.
#define VALID_DEFAULT 50 //Amy's default.
#define RIGID_DEFAULT 50
#define RANDOM_DEFAULT 50
#define MAXSHIFT_DEFAULT 1.00f

StatisticsWidget::StatisticsWidget(QWidget *parent):
	QWidget(parent)
{
	ui.setupUi(this);	
	statObject = new StatInterface(this);
	data1 = NULL;
	data2 = NULL;
	statObject->setTSampleSize(TSAMPLES_DEFAULT); 

	ui.calculateButton->setEnabled(false);
	ui.rValue->setEnabled(false);
	ui.tValue->setEnabled(false);
	ui.sWindowSpinBox->setValue(SEARCH_DEFAULT); 
	ui.vWindowSpinBox->setValue(VALID_DEFAULT); 

	connect(ui.calculateButton, SIGNAL(clicked()),
		this, SLOT(compare()));
	connect(ui.sWindowSpinBox, SIGNAL(valueChanged(int)),
		statObject, SLOT(setSearchWindow(int)));
	connect(ui.vWindowSpinBox, SIGNAL(valueChanged(int)),
		statObject, SLOT(setValidWindow(int)));

	//Settings dialog stuff
	settings = new StatisticsSettingsDialog(RIGID_DEFAULT,
		RANDOM_DEFAULT, MAXSHIFT_DEFAULT, TSAMPLES_DEFAULT, this);
	settings->hide();
	connect(settings, SIGNAL(rigidPairsUpdated(int)),
		statObject, SLOT(setNumRigidPairs(int)));
	connect(settings, SIGNAL(randomPairsUpdated(int)),
		statObject, SLOT(setNumRandomPairs(int)));
	connect(settings, SIGNAL(maxShiftUpdated(double)),
		statObject, SLOT(setMaxShiftPercentage(double)));
	connect(settings, SIGNAL(tSamplesUpdated(int)),
		statObject, SLOT(setTSampleSize(int)));
}

StatisticsWidget::~StatisticsWidget()
{
	//statObject is parented to this.
	//settings is parented to this.
}

void
StatisticsWidget::updateTValue(QString tValue)
{
	ui.tValue->setText(tValue);
}

void
StatisticsWidget::updateRValue(QString rValue)
{
	ui.rValue->setText(rValue);
}

void
StatisticsWidget::setPlotData_1(Profile* data)
{
	data1 = data;
	ui.tValue->setEnabled(false);
	ui.rValue->setEnabled(false);
	//Stale search windows.
	emit searchWindowUpdated_1(0, 0); 
	emit searchWindowUpdated_2(0, 0); 

	if ((NULL == data1) || (NULL == data2))
	{
		ui.calculateButton->setEnabled(false);
	}
	else 
	{
		ui.calculateButton->setEnabled(true);
	}
}

void
StatisticsWidget::setPlotData_2(Profile* data)
{
	data2 = data;
	ui.tValue->setEnabled(false);
	ui.rValue->setEnabled(false);
	//Stale search windows.
	emit searchWindowUpdated_1(0, 0); 
	emit searchWindowUpdated_2(0, 0); 

	if ((NULL == data1) || (NULL == data2))
	{
		ui.calculateButton->setEnabled(false);
	}
	else 
	{
		ui.calculateButton->setEnabled(true);
	}
}

void
StatisticsWidget::compare()
{
	if ((NULL != data1) && (NULL != data2))
	{
		try 
		{
			statObject->compare(data1, data2);
		}
		catch (std::exception err)
		{
			reportStatPackageError(err.what());
			updateTValue("####");
			updateRValue("####");

			//Stale search windows.
			emit searchWindowUpdated_1(0, 0); 
			emit searchWindowUpdated_2(0, 0); 
			return;
		}

		//Update GUI with results.
		updateTValue(QString::number(statObject->getTValue()));
		updateRValue(QString::number(statObject->getRValue()));
		ui.tValue->setEnabled(true);
		ui.rValue->setEnabled(true);
		emit searchWindowUpdated_1(statObject->getLoc1(), 
			statObject->getSearchWindow());
		emit searchWindowUpdated_2(statObject->getLoc2(), 
			statObject->getSearchWindow());
	}
}

void
StatisticsWidget::reportStatPackageError(QString msg)
{
	QMessageBox::critical(this, tr("Statistical Package Error"), msg);
}

void
StatisticsWidget::setSettingsVisibility(bool visible)
{
	settings->setVisible(visible);
	settings->setDefaultButtonDefault(false);
}
