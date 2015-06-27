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

//=======================================================================
//=======================================================================
StatisticsWidget::StatisticsWidget(bool showStatPlots, bool autoUpdateRT, QWidget *parent):
	QWidget(parent)
{
	ui.setupUi(this);	
	statObject = new StatInterface(this);
    //data1 = NULL;
    //data2 = NULL;
	statObject->setTSampleSize(TSAMPLES_DEFAULT); 

	ui.calculateButton->setEnabled(false);
	ui.rValue->setEnabled(false);
	ui.tValue->setEnabled(false);
	ui.sWindowSpinBox->setValue(SEARCH_DEFAULT); 
    ui.vWindowSpinBox->setValue(VALID_DEFAULT);

    ui.checkBoxShowStatPlots->setChecked(showStatPlots);
    ui.checkBoxAutoUpdateRT->setChecked(autoUpdateRT);
    ui.labelMag1->setText(QString("0"));
    ui.labelMag2->setText(QString("0"));
    ui.labelZoom1->setText(QString("0.00"));
    ui.labelZoom2->setText(QString("0.00"));

    bool ret;
    ret = connect(ui.calculateButton, SIGNAL(clicked()),
		this, SLOT(compare()));
    ret = connect(ui.sWindowSpinBox, SIGNAL(valueChanged(int)),
		statObject, SLOT(setSearchWindow(int)));
    ret = connect(ui.vWindowSpinBox, SIGNAL(valueChanged(int)),
		statObject, SLOT(setValidWindow(int)));

	//Settings dialog stuff
	settings = new StatisticsSettingsDialog(RIGID_DEFAULT,
		RANDOM_DEFAULT, MAXSHIFT_DEFAULT, TSAMPLES_DEFAULT, this);
	settings->hide();
    ret = connect(settings, SIGNAL(rigidPairsUpdated(int)),
		statObject, SLOT(setNumRigidPairs(int)));
    ret = connect(settings, SIGNAL(randomPairsUpdated(int)),
		statObject, SLOT(setNumRandomPairs(int)));
    ret = connect(settings, SIGNAL(maxShiftUpdated(double)),
		statObject, SLOT(setMaxShiftPercentage(double)));
    ret = connect(settings, SIGNAL(tSamplesUpdated(int)),
		statObject, SLOT(setTSampleSize(int)));

    // split compare stuff
    ret = connect(ui.checkBoxShowStatPlots, SIGNAL(clicked()), this, SLOT(onShowStatsPlotClicked()));
    ret = connect(ui.checkBoxAutoUpdateRT, SIGNAL(clicked()), this, SLOT(onAutoUpdateRTClicked()));

    ret = connect(ui.btnOptGraphResults, SIGNAL(clicked()), this, SLOT(onOptGraphResultsClicked()));
    setOptHaveResults(false);
}

//=======================================================================
//=======================================================================
StatisticsWidget::~StatisticsWidget()
{
	//statObject is parented to this.
	//settings is parented to this.
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setMag1(float mag)
{
    ui.labelMag1->setText(QString("%1").arg(mag, 0, 'f', 0));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setMag2(float mag)
{
    ui.labelMag2->setText(QString("%1").arg(mag, 0, 'f', 0));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setZoom1(float zoom)
{
    ui.labelZoom1->setText(QString("%1").arg(zoom, 0, 'f', 0));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setZoom2(float zoom)
{
    ui.labelZoom2->setText(QString("%1").arg(zoom, 0, 'f', 0));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setOptAng(float ang)
{
    ui.labelOptAngle->setText(QString("%1").arg(ang, 0, 'f', 0));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setOptR(float r)
{
    ui.labelOptR->setText(QString::number(r));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setOptT(float t)
{
    ui.labelOptT->setText(QString::number(t));
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setOptHaveResults(bool have)
{
    ui.btnOptGraphResults->setEnabled(have);
}

//=======================================================================
//=======================================================================
void StatisticsWidget::updateTValue(QString tValue)
{
	ui.tValue->setText(tValue);
}

//=======================================================================
//=======================================================================
void StatisticsWidget::updateRValue(QString rValue)
{
	ui.rValue->setText(rValue);
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setPlotData_1(PProfile data)
{
    _data1 = data;
	ui.tValue->setEnabled(false);
	ui.rValue->setEnabled(false);
	//Stale search windows.
    emit searchWindowUpdated_1(0, 0, 0);
    emit searchWindowUpdated_2(0, 0, 0);

    if ((NULL == _data1) || (NULL == _data2))
	{
		ui.calculateButton->setEnabled(false);
	}
	else 
	{
		ui.calculateButton->setEnabled(true);
	}
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setPlotData_2(PProfile data)
{
    _data2 = data;
	ui.tValue->setEnabled(false);
	ui.rValue->setEnabled(false);
	//Stale search windows.
    emit searchWindowUpdated_1(0, 0, 0);
    emit searchWindowUpdated_2(0, 0, 0);

    if ((NULL == _data1) || (NULL == _data2))
	{
		ui.calculateButton->setEnabled(false);
	}
	else 
	{
		ui.calculateButton->setEnabled(true);
	}
}

//=======================================================================
//=======================================================================
void StatisticsWidget::compare()
{

    if ((NULL == _data1) || (NULL == _data2)) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    try
    {
        statObject->compare(_data1.get(), _data2.get());
    }
    catch (std::exception err)
    {
        reportStatPackageError(err.what());
        updateTValue("####");
        updateRValue("####");

        //Stale search windows.
        emit searchWindowUpdated_1(0, 0, 0);
        emit searchWindowUpdated_2(0, 0, 0);

        QApplication::restoreOverrideCursor();
        return;
    }

    //Update GUI with results.
    updateTValue(QString::number(statObject->getTValue()));
    updateRValue(QString::number(statObject->getRValue()));
    ui.tValue->setEnabled(true);
    ui.rValue->setEnabled(true);

    emit searchWindowUpdated_1(statObject->getLoc1(), statObject->getSearchWindow(), statObject->getDataLen1());
    emit searchWindowUpdated_2(statObject->getLoc2(), statObject->getSearchWindow(), statObject->getDataLen2());

    QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
void StatisticsWidget::reportStatPackageError(QString msg)
{
    QString outmsg = tr("Stat Error: ");
    outmsg += msg;
    QMessageBox::critical(this, tr("Statistical Package Error"), outmsg);
}

//=======================================================================
//=======================================================================
void StatisticsWidget::setSettingsVisibility(bool visible)
{
	settings->setVisible(visible);
	settings->setDefaultButtonDefault(false);
}

//=======================================================================
//=======================================================================
void StatisticsWidget::onShowStatsPlotClicked()
{
    emit showStatPlots(ui.checkBoxShowStatPlots->isChecked());
}

//=======================================================================
//=======================================================================
void StatisticsWidget::onAutoUpdateRTClicked()
{
    emit autoUpdateRT(ui.checkBoxAutoUpdateRT->isChecked());
}

//=======================================================================
//=======================================================================
void StatisticsWidget::onOptGraphResultsClicked()
{
    emit showOptGraphResults();
}
