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

#ifndef __STATISTICSWIDGET_H__
#define __STATISTICSWIDGET_H__
#include <QWidget>
#include <ui_StatisticsWidget.h>
#include "../core/StatInterface.h"
#include "../core/Profile.h"
#include "StatisticsSettingsDialog.h"

class StatisticsWidget: public QWidget
{
	Q_OBJECT

public:
    StatisticsWidget(bool showStatPlots, bool autoUpdateRT, QWidget *parent = 0);
	virtual ~StatisticsWidget();

    StatInterface::StatConfig getStatConfig() { return statObject->getConfig(); }

    void setMag1(float mag);
    void setMag2(float mag);
    void setZoom1(float zoom);
    void setZoom2(float zoom);

    void setOptAng(float ang);
    void setOptR(float r);
    void setOptT(float t);

    void setOptHaveResults(bool have);

public slots:
	///Updates the T statistic.
	void updateTValue(QString tValue);
	///Updates the max correlation value.
	void updateRValue(QString rValue);
	///Receive data set 1.
    void setPlotData_1(PProfile data);
	///Receive data set 2.
    void setPlotData_2(PProfile data);
	///Performs the comparison.
	void compare();
	///Report statistics package error to the user.
	void reportStatPackageError(QString msg);
	///Show or hide the advanced settings dialog
	void setSettingsVisibility(bool visible);

    void onShowStatsPlotClicked();
    void onAutoUpdateRTClicked();

    void onOptGraphResultsClicked();

signals:
	///Reports the search window for the 1st data set.
    void searchWindowUpdated_1(int loc, int searchWidth, int dataLen);
	///Reports the search window for the 2nd data set.
    void searchWindowUpdated_2(int loc, int searchWidth, int dataLen);

    void showStatPlots(bool show);
    void autoUpdateRT(bool update);

    void showOptGraphResults();

private:
    ///GUI (from Qt Designer)
    Ui::StatisticsWidget ui;
    ///Interface to the statistics package.
    StatInterface* statObject;
    ///Data set 1 (owned by the InvestigatorSubWidget)
    PProfile _data1;
    ///Data set 2 (owned by the InvestigatorSubWidget)
    PProfile _data2;
    ///Settings dialog.
    StatisticsSettingsDialog* settings;

};
#endif //!defined __STATISTICSWIDGET_H__
