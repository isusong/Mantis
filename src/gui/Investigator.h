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

#ifndef __INVESTIGATOR_H_
#define __INVESTIGATOR_H_
#include <QMainWindow>
#include <QString>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QMdiArea>
#include <QDockWidget>
#include <QAction>
#include <QSettings>

#include "StatisticsWidget.h"
#include "InvestigatorSubWidget.h"
#include "SplitCmpViewCtrlsWidget.h"
#include "SplitCmpThumbLoaderWidget.h"

class Investigator: public QMainWindow
{
    Q_OBJECT

public:
    Investigator(QWidget* parent = 0);
    virtual ~Investigator();

    QMdiSplitCmpWnd* getTopSplitCmp();
    QMdiArea* getMdiArea() { return _area; }

public slots:
    void showSplitComparison();
	void openTip();
	void openPlate();
	void assignSlot(InvestigatorSubWidget* window); 
	void updateEnabledStatus(); ///< Enable/Disable actions.
	void emptySlot_1(); ///< Update slot 1 to empty status.
	void emptySlot_2(); ///< Update slot 2 to empty status.
	void postStatusMessage(QString msg);
	void activateStatSettings();
    void slotSplitItemClicked(QListWidgetItem * item);
    void slotSplitCmpWndClosed();

protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
    void saveDocStates(QDockWidget *doc);
    void loadDocStates(QDockWidget *doc);
    void initStyles();
    void makeConnections();
    void assemble();
    void createSplitCompareDockWindows();
    unsigned int getCurrentWindows(std::vector<InvestigatorSubWidget*> *pv);

protected:
    //Data Objects.
    QString _lastDirectory; ///< Stores last directory of navigation.
    QFileInfo _fileInfo;
    bool _slotFilled_1; ///< Is the first window slot filled?
    bool _slotFilled_2; ///< Is the second window slot filled?

    //Gui Objects.
    QWidget* _center; ///< Placeholder for central widget.
    QVBoxLayout* _mainLayout;
    QMdiArea* _area;
    StatisticsWidget* _stat;
    QMenu* _fileMenu;
    QAction* _openTipAction;
    QAction* _openPlateAction;
    QMenu* _viewMenu;
    QMenu* _toolsMenu;
    QAction* _tileAction;
    QAction* _cascadeAction;
    QAction* _statSettingsAction;
    QAction* _showSplitCompareAction;

    QDockWidget* _dockCmpViewCtrls;
    SplitCmpViewCtrlsWidget* _splitCmpViewCtrls;

    QDockWidget* _dockCmpThumbLoader;
    SplitCmpThumbLoaderWidget* _splitCmpThumbLoader;
};

#endif //!defined __INVESTIGATOR_H__
