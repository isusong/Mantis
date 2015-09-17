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
#include "QMdiSplitCmpWnd2.h"
#include "ThreadStatMarkOpt.h"
#include "SettingsStore.h"
#include "../core/StatResults.h"
#include "DlgStatResults.h"

#include "QMdiMaskEditor.h"

class Investigator: public QMainWindow
{
    Q_OBJECT

public:
    Investigator(QWidget* parent = 0);
    virtual ~Investigator();

    QMdiSplitCmpWnd2* getTopSplitCmp();
    QMdiMaskEditor* getTopMaskEditor();
    QMdiArea* getMdiArea() { return _area; }
    SplitCmpViewCtrlsWidget* getSplitCmpCtrls() { return _splitCmpViewCtrls; }

    bool autoUpdateStatsRT();

    void runMarkOptimization(RangeImageRenderer *tipModel, PProfile plateProfile);

public slots:
    void slotActivateLightingSettings();
    void slotToggleShowStartupDlg();

    void showSplitComparison();
	void openTip();
	void openPlate();
    void openMaskEditor();
    void importTip();
    void importPlate();
    void importKnife();
    void importBullet();
    void updateMtFiles();
	void assignSlot(InvestigatorSubWidget* window); 
	void updateEnabledStatus(); ///< Enable/Disable actions.
	void emptySlot_1(); ///< Update slot 1 to empty status.
	void emptySlot_2(); ///< Update slot 2 to empty status.
	void postStatusMessage(QString msg);
	void activateStatSettings();

    void slotSplitCmpContextMenuThumbLoader(const QPoint &pos);
    void slotSetSplitCmpProjFolder();
    void slotSplitItemClicked(QListWidgetItem *item);
    void slotSplitCmpWndClosed();
    void slotSplitCmpProfileUpdated(PProfile p1, PProfile p2);
    void slotSplitCmpUpdateStatsRT();
    void slotToggleShowStatPlotsAction();
    void slotToggleAutoUpdateStatsRTAction();
    void slotShowStatPlots(bool show);
    void slotAutoUpdateStatsRT(bool update);
    void slotSplitCmpUpdateMark();
    void slotSplitWindowSelChange();
    void slotShowOptGraphResults();

    void slotShowMaskEditor();

    void slotZoomUpdated(int viewer, double zoom);
    void slotChangedTranslationMouse(int viewer, const QVector3D &v);
    void slotOnDefaultView();

    void slotOnSubWndActivated(QMdiSubWindow *wnd);

    void slotOnMaskEditorFileSaved(QString file);


protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void closeEvent(QCloseEvent *closeEvent);
    virtual void showEvent(QShowEvent *event);
    //void saveDocStates(QDockWidget *doc);
    //void loadDocStates(QDockWidget *doc);
    void initStyles();
    void makeConnections();
    void assemble();
    void createSplitCompareDockWindows();
    bool pickSplitCmpProjFolder(QString *pfolder=NULL);
    bool setSplitCmpProjFolder(const QString &folder);
    unsigned int getCurrentWindows(std::vector<InvestigatorSubWidget*> *pv);

    void refreshThumbLoaderSelection();
    void refreshMagAndZoom();

    void import(RangeImage::EImgType type);
    QMdiMaskEditor* loadMaskEditor(PRangeImage img);

protected:
    //Data Objects.
    /*
    QString _lastDirectory; ///< Stores last directory of navigation.
    bool _lastDirSet;
    bool _autoUpdateStatsRT;
    bool _showStatPlots;
    */


    //SettingsStore::InvSettings _settings;


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
    QAction* _openMaskEditorAction;
    QAction* _importTipAction;
    QAction* _importPlateAction;
    QAction* _importKnifeAction;
    QAction* _importBulletAction;
    QAction* _updateMtFiles;
    QMenu* _viewMenu;
    QAction* _actionViewLigthing;
    QAction *_actionViewShowStartupDlg;
    QMenu* _toolsMenu;
    QAction* _tileAction;
    QAction* _cascadeAction;
    QAction* _statSettingsAction;
    QAction* _setProjectFolderAction;
    QAction* _showSplitCompareAction;
    QAction* _showMaskEditorAction;
    QAction* _showStatPlotsAction;
    QAction* _autoUpdateStatsRTAction;

    QDockWidget* _dockCmpViewCtrls;
    SplitCmpViewCtrlsWidget* _splitCmpViewCtrls;

    QDockWidget* _dockCmpThumbLoader;
    SplitCmpThumbLoaderWidget* _splitCmpThumbLoader;

    QWidget* _win1;
    QWidget* _win2;
    QWidget* _winSplit;

    DlgStatResults *_dlgStatResults;
    PStatResults _statOptResults;
};

#endif //!defined __INVESTIGATOR_H__
