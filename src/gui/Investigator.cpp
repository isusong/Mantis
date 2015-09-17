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
#include <QMdiSubWindow>
#include <QMessageBox>

#include "../core/logger.h"
#include "../core/RangeImage.h"
#include "../core/UtlMtFiles.h"
#include "../core/UtlQt.h"

#include "App.h"

#include "TipWidget.h"
#include "PlateWidget.h"
#include "QListWidgetItemEx.h"
#include "QMdiSplitCmpWnd.h"
#include "UtlQtGui.h"
#include "QProgressDialogEx.h"
#include "GuiSettings.h"
#include "DlgLighting.h"

#include "QMdiMaskEditor.h"

//=======================================================================
//=======================================================================
Investigator::Investigator(QWidget* parent): QMainWindow(parent),
    _win1(NULL),
    _win2(NULL),
    _winSplit(NULL),
    _dlgStatResults(NULL)
{
	//Create the data objects.
    //_lastDirectory = QDir::currentPath();
    _slotFilled_1 = false;
    _slotFilled_2 = false;

	//Create the gui objects.
    _center = new QWidget(this);
    _mainLayout = new QVBoxLayout(_center);
    _area = new QMdiArea(this);
    _stat = new StatisticsWidget(App::settings()->inv().showStatPlots, App::settings()->inv().autoUpdateStatsRT, this);
    _fileMenu = new QMenu(tr("&File"), this);
    _openTipAction = new QAction(this);
    _openPlateAction = new QAction(this);
    _openMaskEditorAction = new QAction(this);
    _importTipAction = new QAction(this);
    _importPlateAction = new QAction(this);
    _importKnifeAction = new QAction(this);
    _importBulletAction = new QAction(this);
    _updateMtFiles = new QAction(this);
    _viewMenu = new QMenu(tr("&View"), this);
    _toolsMenu = new QMenu(tr("&Tools"), this);
    _tileAction = new QAction(this);
    _cascadeAction = new QAction(this);
    _actionViewLigthing = new QAction(this);
    _actionViewShowStartupDlg = new QAction(this);
    _statSettingsAction = new QAction(this);
    _showSplitCompareAction = new QAction(this);
    _setProjectFolderAction = new QAction(this);
    _showStatPlotsAction = new QAction(this);
    _autoUpdateStatsRTAction = new QAction(this);
    _showMaskEditorAction = new QAction(this);

	//Title the actions.
    _openTipAction->setText(tr("Open &Tip"));
    _openPlateAction->setText(tr("Open Test &Mark"));
    _openMaskEditorAction->setText(tr("Open Mask Editor and Cleaner"));
    _importTipAction->setText(tr("Import Tip"));
    _importPlateAction->setText(tr("Import Plate"));
    _importKnifeAction->setText(tr("Import Knife"));
    _importBulletAction->setText(tr("Import Bullet"));
    _updateMtFiles->setText(tr("Update MT Files to Latest &Version"));


    _tileAction->setText(tr("&Tile Windows"));
    _cascadeAction->setText(tr("&Cascade Windows"));
    _actionViewLigthing->setText(tr("Lighting Settings"));
    _actionViewShowStartupDlg->setText(tr("Show Startup Dialog"));

    _statSettingsAction->setText(tr("&Statistics Settings"));
    _showSplitCompareAction->setText(tr("Show Split Compare"));
    _setProjectFolderAction->setText(tr("Set Split Compare Project Folder"));
    _showStatPlotsAction->setText(tr("Show Statistic Plots"));
    _autoUpdateStatsRTAction->setText(tr("Auto &Update Stats RT"));
    _showMaskEditorAction->setText(tr("Show Mask Editor"));


	//Init tile and cascade off
    _tileAction->setEnabled(false);
    _cascadeAction->setEnabled(false);

    _dockCmpViewCtrls = NULL;
    _splitCmpViewCtrls = NULL;
    _dockCmpThumbLoader = NULL;
    _splitCmpThumbLoader = NULL;

	//Set up.
    initStyles();
	makeConnections();
	assemble();
}

//=======================================================================
//=======================================================================
Investigator::~Investigator()
{
	//everything is parented.
    //App::settings()->saveAll();
    //SettingsStore::saveInvestigator(_settings);
}

//=======================================================================
//=======================================================================
void Investigator::closeEvent(QCloseEvent *closeEvent)
{
    QMdiMaskEditor *maskEditor = getTopMaskEditor();
    if (maskEditor && !maskEditor->onDestroyModified())
    {
        // user canceled and has more editing to do, so abort the close
        closeEvent->ignore();
        return;
    }

    SettingsStore::saveDocStates(this, _dockCmpViewCtrls);
    SettingsStore::saveDocStates(this, _dockCmpThumbLoader);
    //App::settings()->saveAll();

    QMainWindow::closeEvent(closeEvent);

    //QApplication::quit();
}

//=======================================================================
//=======================================================================
void Investigator::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if (!App::settings()->inv().lastDirSet || !UtlQt::validateDir(App::settings()->inv().lastDir, false))
    {
        slotSetSplitCmpProjFolder();
    }
}

//=======================================================================
//=======================================================================
void Investigator::initStyles()
{
    QString splitStyle = "QSplitter::handle { ";
    splitStyle += "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,";
    splitStyle += "stop:0 rgba(255, 255, 255, 0),";
    splitStyle += "stop:0.407273 rgba(200, 200, 200, 255),";
    splitStyle += "stop:0.4825 rgba(101, 104, 113, 235),";
    splitStyle += "stop:0.6 rgba(255, 255, 255, 0));";
    splitStyle += "image: url(:/images/splitter.png);";
    splitStyle += "}";
    this->setStyleSheet(splitStyle);
}

//=======================================================================
//=======================================================================
void Investigator::makeConnections()
{
    bool result;
    result = connect(_actionViewLigthing, SIGNAL(triggered()), this, SLOT(slotActivateLightingSettings()));
    result = connect(_actionViewShowStartupDlg, SIGNAL(triggered()), this, SLOT(slotToggleShowStartupDlg()));


    result = connect(_openTipAction, SIGNAL(triggered()), this, SLOT(openTip()));
    result = connect(_openPlateAction, SIGNAL(triggered()), this, SLOT(openPlate()));
    result = connect(_openMaskEditorAction, SIGNAL(triggered()), this, SLOT(openMaskEditor()));
    result = connect(_importTipAction, SIGNAL(triggered()), this, SLOT(importTip()));
    result = connect(_importPlateAction, SIGNAL(triggered()), this, SLOT(importPlate()));
    result = connect(_importKnifeAction, SIGNAL(triggered()), this, SLOT(importKnife()));
    result = connect(_importBulletAction, SIGNAL(triggered()), this, SLOT(importBullet()));
    result = connect(_updateMtFiles, SIGNAL(triggered()), this, SLOT(updateMtFiles()));

    result = connect(_tileAction, SIGNAL(triggered()), _area, SLOT(tileSubWindows()));
    result = connect(_cascadeAction, SIGNAL(triggered()), _area, SLOT(cascadeSubWindows()));
    result = connect(_area, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slotOnSubWndActivated(QMdiSubWindow*)));

    result = connect(_statSettingsAction, SIGNAL(triggered()), this, SLOT(activateStatSettings()));
    result = connect(_showSplitCompareAction, SIGNAL(triggered()), this, SLOT(showSplitComparison()));
    result = connect(_setProjectFolderAction, SIGNAL(triggered()), this, SLOT(slotSetSplitCmpProjFolder()));

    result = connect(_showStatPlotsAction, SIGNAL(triggered()), this, SLOT(slotToggleShowStatPlotsAction()));
    result = connect(_autoUpdateStatsRTAction, SIGNAL(triggered()), this, SLOT(slotToggleAutoUpdateStatsRTAction()));

    result = connect(_showMaskEditorAction, SIGNAL(triggered()), this, SLOT(slotShowMaskEditor()));


    result = connect(_stat, SIGNAL(showOptGraphResults()), this, SLOT(slotShowOptGraphResults()));


}

//=======================================================================
//=======================================================================
void Investigator::assemble()
{
    _fileMenu->addAction(_openTipAction);
    _fileMenu->addAction(_openPlateAction);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_openMaskEditorAction);
    _fileMenu->addAction(_importTipAction);
    _fileMenu->addAction(_importPlateAction);
    _fileMenu->addAction(_importKnifeAction);
    _fileMenu->addAction(_importBulletAction);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_updateMtFiles);

    _viewMenu->addAction(_actionViewShowStartupDlg);
    _actionViewShowStartupDlg->setCheckable(true);
    _actionViewShowStartupDlg->setChecked(App::settings()->inv().showStartupDlg);
    _viewMenu->addSeparator();
    _viewMenu->addAction(_actionViewLigthing);

    //_toolsMenu->addAction(_tileAction);
    //_toolsMenu->addAction(_cascadeAction);
    //_toolsMenu->addSeparator();
    _toolsMenu->addAction(_showMaskEditorAction);
    _toolsMenu->addAction(_showSplitCompareAction);
    _toolsMenu->addAction(_setProjectFolderAction);

    _toolsMenu->addSeparator();

    _toolsMenu->addAction(_statSettingsAction);
    _toolsMenu->addAction(_showStatPlotsAction);
    _showStatPlotsAction->setCheckable(true);
    _showStatPlotsAction->setChecked(App::settings()->inv().showStatPlots);
    _toolsMenu->addAction(_autoUpdateStatsRTAction);
    _autoUpdateStatsRTAction->setCheckable(true);
    _autoUpdateStatsRTAction->setChecked(App::settings()->inv().autoUpdateStatsRT);


    menuBar()->addMenu(_fileMenu);
    menuBar()->addMenu(_viewMenu);
    menuBar()->addMenu(_toolsMenu);

    _mainLayout->addWidget(_area);
    _mainLayout->addWidget(_stat);
    _center->setLayout(_mainLayout);
    setCentralWidget(_center);

    createSplitCompareDockWindows();
    showSplitComparison();

    if (App::settings()->inv().startupMode == SettingsStore::StartUp_MaskEditor)
    {
        slotShowMaskEditor(); // show the mask editor interface on top
    }
}

//=======================================================================
//=======================================================================
void Investigator::createSplitCompareDockWindows()
{
    if (_dockCmpViewCtrls) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    _dockCmpViewCtrls = new QDockWidget(tr("Split Comparison View Ctrls"), this);
    _dockCmpViewCtrls->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _splitCmpViewCtrls = new SplitCmpViewCtrlsWidget(this, _dockCmpViewCtrls);
    _dockCmpViewCtrls->setWidget(_splitCmpViewCtrls);
    _dockCmpViewCtrls->setMinimumSize(_splitCmpViewCtrls->size().width(), _splitCmpViewCtrls->size().height());

    addDockWidget(Qt::RightDockWidgetArea, _dockCmpViewCtrls);
    _dockCmpViewCtrls->setFloating(true);
    _viewMenu->addAction(_dockCmpViewCtrls->toggleViewAction());

    _dockCmpViewCtrls->hide();



    _dockCmpThumbLoader = new QDockWidget(tr("Split Comparison Loader"), this);
    _dockCmpThumbLoader->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    _splitCmpThumbLoader = new SplitCmpThumbLoaderWidget(_dockCmpThumbLoader);
    _splitCmpThumbLoader->setMinimumSize(900, 220);
    _splitCmpThumbLoader->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    _dockCmpThumbLoader->setWidget(_splitCmpThumbLoader);
    _dockCmpThumbLoader->setMinimumSize(_splitCmpThumbLoader->size().width(), _splitCmpThumbLoader->size().height());
    _splitCmpThumbLoader->setMaximumSize(9999, 9999);

    addDockWidget(Qt::BottomDockWidgetArea, _dockCmpThumbLoader);
    _dockCmpThumbLoader->setFloating(true);
    _viewMenu->addAction(_dockCmpThumbLoader->toggleViewAction());

    _dockCmpThumbLoader->hide();

    if (App::settings()->inv().lastDirSet)
    {
        setSplitCmpProjFolder(App::settings()->inv().lastDir);
    }

    bool res;
    res = connect(_splitCmpThumbLoader->getListWidgetThumbs(), SIGNAL(signalItemLeftClicked(QListWidgetItem*)), this, SLOT(slotSplitItemClicked(QListWidgetItem*)));
    res = connect(_splitCmpThumbLoader, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotSplitCmpContextMenuThumbLoader(const QPoint&)));
    res = connect(_splitCmpViewCtrls, SIGNAL(updateMark()), this, SLOT(slotSplitCmpUpdateMark()));
    res = connect(_splitCmpViewCtrls, SIGNAL(onZoomUpdated(int,double)), this, SLOT(slotZoomUpdated(int,double)));
    res = connect(_splitCmpViewCtrls, SIGNAL(onDefaultView()), this, SLOT(slotOnDefaultView()));

    QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
bool Investigator::pickSplitCmpProjFolder(QString *pfolder)
{
    QString dir = QFileDialog::getExistingDirectory(NULL, "Select Project Folder", App::settings()->inv().lastDir, QFileDialog::ShowDirsOnly);
    if (dir == "")
    {
        // user canceled
        return false;
    }

    if (pfolder) *pfolder = dir;
    return true;
}

//=======================================================================
//=======================================================================
bool Investigator::setSplitCmpProjFolder(const QString& folder)
{
    if (_splitCmpThumbLoader == NULL) return false;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!_splitCmpThumbLoader->setProjectFolder(folder))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(NULL, "Project Folder Error", "Failed to set the project folder, no mt files found.");
        return false;
    }

    App::settings()->inv().lastDir = folder;
    App::settings()->inv().lastDirSet = true;
    QApplication::restoreOverrideCursor();
    return true;
}

//=======================================================================
//=======================================================================
void Investigator::slotSetSplitCmpProjFolder()
{
    // update the folder
    QString folder;
    if (!pickSplitCmpProjFolder(&folder)) return;
    setSplitCmpProjFolder(folder);
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpContextMenuThumbLoader(const QPoint& pos)
{
    QPoint globalPos = _splitCmpThumbLoader->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Set Project Folder");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (!selectedItem)
    {
        return;
    }

    slotSetSplitCmpProjFolder();
}

//=======================================================================
//=======================================================================
void Investigator::showSplitComparison()
{
    QMdiSplitCmpWnd2 *wnd = getTopSplitCmp();
    if (wnd)
    {
        _area->setActiveSubWindow(wnd);
        //QMessageBox::warning(this, "Split Comparison", "Split comparision window already opened"); // TODO: FIGURE OUT HOW TO GREY OUT THE MENU ITEM UNTIL WINDOW IS CLOSED
        return;
    }


    // do we have any windows open
    std::vector<InvestigatorSubWidget*> subs;
    getCurrentWindows(&subs);

    // if there are no subwindows already open then ask for a project folder
    if (subs.size() <= 0)
    {
        if (!App::settings()->inv().lastDirSet)
        {
            QString folder;
            if (!pickSplitCmpProjFolder(&folder)) return;
            setSplitCmpProjFolder(folder);
        }
    }
    else
    {
        // TODO: is _lastDirectory the correct one, or do we need to go up one directory to get to the project folder
    }



    /*
    if (!_splitCmpThumbLoader->setProjectFolder(_lastDirectory))
    {
        QMessageBox::critical(NULL, "Project Folder Error", "Failed to set the project folder, no mt files found.");
        return;
    }
    */

    //Create the new window
    QMdiSplitCmpWnd2* window = new QMdiSplitCmpWnd2(_area);//QMdiSplitCmpWnd2(_splitCmpViewCtrls, _area);
    window->init();
    window->showPlots(App::settings()->inv().showStatPlots);

    _area->addSubWindow(window);
    window->showMaximized();
    // _area->tileSubWindows();

    // lets load up any existing range inmages
    for (unsigned int i=0; i<subs.size(); i++)
    {
        bool isTip = false;
        TipWidget* tipWidget = dynamic_cast<TipWidget *>(subs[i]);
        if (tipWidget)
        {
            isTip = true;
        }

        PRangeImage rimg = subs[i]->getRangeImage();
        if (rimg->isUnkType())
        {
            rimg->setImgType(RangeImage::ImgType_Tip);
        }
        window->setRangeImg(rimg, i);
    }

    //_splitCmpViewCtrls->connectSplitCmp(window);
    if (_splitCmpViewCtrls)
    {
        _splitCmpViewCtrls->refreshGui(window);
    }

    refreshThumbLoaderSelection();

    SettingsStore::loadDocStates(this, _dockCmpViewCtrls);
    SettingsStore::loadDocStates(this, _dockCmpThumbLoader);
    _dockCmpViewCtrls->show();
    _dockCmpThumbLoader->show();

    bool result;

    // TODO: searchWindowUpdated_1 signal may already be mapped to a tip or mark widgets statPlot, need to figure out how
    // the SplitCmpWnd can exist with these, because the setSearchWindows can be called improperly when SplitCmpWnd is showing along with a tip or mark widget
    //
    result = connect(window, SIGNAL(onClosed()), this, SLOT(slotSplitCmpWndClosed()));
    result = connect(window, SIGNAL(onStatsProfileUpdated(PProfile, PProfile)), this, SLOT(slotSplitCmpProfileUpdated(PProfile, PProfile)));
    //result = connect(window, SIGNAL(onRangeImageLoaded()), this, SLOT(slotSplitCmpUpdateStatsRT()));
    result = connect(window, SIGNAL(updateStatsRT()), this, SLOT(slotSplitCmpUpdateStatsRT()));
    result = connect(window->getGraphics(), SIGNAL(onWindowSelChange()), this, SLOT(slotSplitWindowSelChange()));

    result = connect(window, SIGNAL(onChangedTranslationMouse(int,QVector3D)), _splitCmpViewCtrls, SLOT(slotChangedTranslationMouse(int, const QVector3D&)));
    result = connect(window, SIGNAL(onChangedRotationMouse(int,QVector3D)), _splitCmpViewCtrls, SLOT(slotChangedRotationMouse(int, const QVector3D&)));
    result = connect(window, SIGNAL(onChangedTranslationMouse(int,QVector3D)), this, SLOT(slotChangedTranslationMouse(int, const QVector3D&)));

    result = connect(_stat, SIGNAL(showStatPlots(bool)), this, SLOT(slotShowStatPlots(bool)));
    result = connect(_stat, SIGNAL(autoUpdateRT(bool)), this, SLOT(slotAutoUpdateStatsRT(bool)));


    result = connect(_stat, SIGNAL(searchWindowUpdated_1(int, int, int)), window, SLOT(slotSetSearchWindow1(int, int, int)));
    result = connect(_stat, SIGNAL(searchWindowUpdated_2(int, int, int)), window, SLOT(slotSetSearchWindow2(int, int, int)));
}

//=======================================================================
//=======================================================================
void Investigator::slotShowMaskEditor()
{
    QMdiMaskEditor *wnd = getTopMaskEditor();
    if (wnd)
    {
        _area->setActiveSubWindow(wnd);
        return;
    }

    loadMaskEditor(PRangeImage());
}

//=======================================================================
//=======================================================================
void Investigator::openTip()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
        tr("Open .mt file"), App::settings()->inv().lastDir,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
    _fileInfo.setFile(filename);
    App::settings()->inv().lastDir = _fileInfo.absolutePath();

    QApplication::setOverrideCursor(Qt::WaitCursor);

	//Create the new widget.
    QMdiSubWindow* window = new QMdiSubWindow(_area);
	window->setAttribute(Qt::WA_DeleteOnClose);
    //PRangeImage tip(new RangeImage(filename, window));
    PRangeImage tip(new RangeImage(filename));
	if (tip->isNull())
	{
        QApplication::restoreOverrideCursor();

		//Deletes the window and the tip object.
		window->close();
		QMessageBox::warning(this, tr("Error"), 
			tr("File failed to open."));
		return;
	}
	TipWidget* tipWidget = new TipWidget(tip, window);
	window->setWidget(tipWidget);
    window->setWindowTitle(_fileInfo.fileName());
    _area->addSubWindow(window);
	tipWidget->show();


   //  _area->tileSubWindows();

    if (!_slotFilled_1) _win1 = window;
    else _win2 = window;

	assignSlot(tipWidget);

    QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
void Investigator::openPlate()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
        tr("Open .mt file"), App::settings()->inv().lastDir,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
    _fileInfo.setFile(filename);
    App::settings()->inv().lastDir = _fileInfo.absolutePath();

     QApplication::setOverrideCursor(Qt::WaitCursor);

	//Create the new widget.
    QMdiSubWindow* window = new QMdiSubWindow(_area);
	window->setAttribute(Qt::WA_DeleteOnClose);
    //PRangeImage plate(new RangeImage(filename, window));
    PRangeImage plate(new RangeImage(filename));
	if (plate->isNull())
	{
        QApplication::restoreOverrideCursor();

		//Deletes the window and the plate object.
		window->close();
		QMessageBox::warning(this, tr("Error"), 
			tr("File failed to open."));
		return;
	}
	PlateWidget* plateWidget = new PlateWidget(plate, window);
	window->setWidget(plateWidget);
    window->setWindowTitle(_fileInfo.fileName());
    _area->addSubWindow(window);
	plateWidget->show();


    //_area->tileSubWindows();

    if (!_slotFilled_1) _win1 = window;
    else _win2 = window;

	assignSlot(plateWidget);

    QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
void Investigator::openMaskEditor()
{
    PRangeImage img = QMdiMaskEditor::loadMt(getTopMaskEditor());
    if (img.isNull()) return;

    loadMaskEditor(img);
}

//=======================================================================
//=======================================================================
void Investigator::importTip()
{
    import(RangeImage::ImgType_Tip);
}

//=======================================================================
//=======================================================================
void Investigator::importPlate()
{
    import(RangeImage::ImgType_Plt);;
}

//=======================================================================
//=======================================================================
void Investigator::importKnife()
{
    import(RangeImage::ImgType_Knf);
}

//=======================================================================
//=======================================================================
void Investigator::importBullet()
{
    import(RangeImage::ImgType_Bul);
}

//=======================================================================
//=======================================================================
void Investigator::import(RangeImage::EImgType type)
{
    QMdiMaskEditor *wnd = loadMaskEditor(PRangeImage());
    wnd->import(type);
}

//=======================================================================
//=======================================================================
QMdiMaskEditor* Investigator::loadMaskEditor(PRangeImage img)
{
    QMdiMaskEditor *wnd = getTopMaskEditor();
    if (wnd == NULL)
    {
        wnd = new QMdiMaskEditor(_area);
        wnd->init(img);
        _area->addSubWindow(wnd);
        _area->setActiveSubWindow(wnd);

        connect(wnd, SIGNAL(onFileSaved(QString)), this, SLOT(slotOnMaskEditorFileSaved(QString)));
        return wnd;
    }

    // TODO: need to make sure no data has been modified, if so ask to save.

    return wnd;
}

//=======================================================================
//=======================================================================
void Investigator::updateMtFiles()
{
    // if there are no subwindows already open then ask for a project folder
    QString dir = QFileDialog::getExistingDirectory(NULL, "Select Folder To Convert All Files", "", QFileDialog::ShowDirsOnly);
    if (dir == "")
    {
        // user canceled
        return;
    }

    UtlMtFiles::FileItemList fileItems, fileItemsUpdate;
    UtlMtFiles::findFiles(dir, &fileItems);

    int vcur = RangeImage::getCurrentFileVersion();
    LogTrace("Current MT file version is %d", vcur);

    for (unsigned int i=0; i<fileItems.size(); i++)
    {
        UtlMtFiles::PFileItem item = fileItems[i];
        std::string path =item->fullPathMt.toStdString();

        int ver = RangeImage::getFileVersion(item->fullPathMt);
        if (ver < vcur)
        {
            fileItemsUpdate.push_back(item);
            LogTrace("Old MT file version %d found %s", ver, path.c_str());
        }
        else if (ver > vcur)
        {
            LogError("UnExpected: newer version MT file version %d found %s.", ver, path.c_str());
        }
        else
        {
            LogTrace("Current MT file version found %s", path);
        }
    }

    if (fileItemsUpdate.size() <= 0)
    {
        UtlQtGui::showMsg("Mt File Update", "No .mt files were found.");
        return;
    }

    QString fileNamesMsg = "Files will be converted and original file will be saved as name.mt(oldversion).<br/>";
    fileNamesMsg += "Do you wish to proceed?<br/><br/>";
    fileNamesMsg += "Files Found:<br/>";
    fileNamesMsg += "--------------------<br/>";
    for (unsigned int i=0; i<fileItemsUpdate.size(); i++)
    {
        fileNamesMsg += fileItemsUpdate[i]->fullPathMt;
        fileNamesMsg += "<br/>";
    }

    int res = UtlQtGui::showLongMsg("Update Mt Files to Current Version", fileNamesMsg, QMessageBox::Ok | QMessageBox::Cancel);
    if (res != QMessageBox::Ok)
    {
        return;
    }

    // update files and show progress dialog so they can cancel
    QProgressDialog progress("Updating files...", "Cancel Update", 0, fileItemsUpdate.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue(0);
    qApp->processEvents();

    QString err = "";
    for (unsigned int i=0; i<fileItemsUpdate.size(); i++)
    {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;

        UtlMtFiles::PFileItem item = fileItemsUpdate[i];
        UtlMtFiles::updateFile(item->fullPathMt, &err);
        qApp->processEvents();
    }
    progress.setValue(fileItemsUpdate.size());

    if (err.length() > 0)
    {
        UtlQtGui::showLongMsg("Update Mt Files to Current Version", err);
    }
}

//=======================================================================
//=======================================================================
void Investigator::assignSlot(InvestigatorSubWidget* window)
{
    // the profile changed and searchWindowUpdated are only used by the split compare window now.
    return;



    if (!_slotFilled_1)
	{
        _slotFilled_1 = true;
        connect(window, SIGNAL(profileChanged(Profile*, bool)), _stat, SLOT(setPlotData_1(Profile*)));
        connect(_stat, SIGNAL(searchWindowUpdated_1(int, int, int)), window, SLOT(setSearchWindow(int, int, int)));
        connect(window, SIGNAL(destroyed()), this, SLOT(emptySlot_1()));
	}
    else if (!_slotFilled_2)
	{
        _slotFilled_2 = true;
        connect(window, SIGNAL(profileChanged(Profile*, bool)), _stat, SLOT(setPlotData_2(Profile*)));
        connect(_stat, SIGNAL(searchWindowUpdated_2(int, int, int)), window, SLOT(setSearchWindow(int, int, int)));
        connect(window, SIGNAL(destroyed()), this, SLOT(emptySlot_2()));
	}

    connect(window, SIGNAL(statusMessage(QString)),  this, SLOT(postStatusMessage(QString)));
	updateEnabledStatus();
}

//=======================================================================
//=======================================================================
void Investigator::updateEnabledStatus()
{
	//If two windows are open, you can't open any new tips or marks
	//unless you close a tip or a plate and free up the slot.
    if (_slotFilled_1 && _slotFilled_2)
	{
        _openTipAction->setEnabled(false);
        _openPlateAction->setEnabled(false);
	}
	else
	{
        _openTipAction->setEnabled(true);
        _openPlateAction->setEnabled(true);
	}

	//You can't tile or cascade windows unless you have some.
    if (_slotFilled_1 || _slotFilled_2)
	{
        _tileAction->setEnabled(true);
        _cascadeAction->setEnabled(true);
	}
	else
	{
        _tileAction->setEnabled(false);
        _cascadeAction->setEnabled(false);
	}
}

//=======================================================================
//=======================================================================
void Investigator::emptySlot_1()
{
    _slotFilled_1 = false;
    _stat->setPlotData_1(PProfile());
	updateEnabledStatus();
}

//=======================================================================
//=======================================================================
void Investigator::emptySlot_2()
{
    _slotFilled_2 = false;
    _stat->setPlotData_2(PProfile());
	updateEnabledStatus();
}

//=======================================================================
//=======================================================================
void Investigator::postStatusMessage(QString msg)
{
	statusBar()->showMessage(msg);
	QCoreApplication::processEvents();
}

//=======================================================================
//=======================================================================
void Investigator::activateStatSettings()
{
    _stat->setSettingsVisibility(true);
}

//=======================================================================
//=======================================================================
unsigned int Investigator::getCurrentWindows(std::vector<InvestigatorSubWidget*> *pv)
{
    QList<QMdiSubWindow*> winlist = _area->subWindowList(QMdiArea::StackingOrder); //	The windows are returned in the order in which they are stacked, with the top-most window being last in the list.)

    // find top split compare window
    for (int i=winlist.size()-1; i >= 0; i--)
    {
        QMdiSubWindow *sub = winlist[i];
        InvestigatorSubWidget *subw = dynamic_cast<InvestigatorSubWidget*>(sub->widget());
        if (subw) pv->push_back(subw);
    }

    return pv->size();
}

//=======================================================================
//=======================================================================
QMdiSplitCmpWnd2* Investigator::getTopSplitCmp()
{
    QList<QMdiSubWindow*> winlist = _area->subWindowList(QMdiArea::StackingOrder); //	The windows are returned in the order in which they are stacked, with the top-most window being last in the list.)
    QMdiSplitCmpWnd2 *subwin = NULL;

    // find top split compare window
    for (int i=winlist.size()-1; i >= 0; i--)
    {
        QMdiSubWindow *sub = winlist[i];
        if (sub->windowTitle() == QString("Split Comparison") )
        {
            subwin =  dynamic_cast<QMdiSplitCmpWnd2*>(sub);
            break;
        }
    }

    return subwin;
}

//=======================================================================
//=======================================================================
QMdiMaskEditor* Investigator::getTopMaskEditor()
{
    QList<QMdiSubWindow*> winlist = _area->subWindowList(QMdiArea::StackingOrder); //	The windows are returned in the order in which they are stacked, with the top-most window being last in the list.)
    QMdiMaskEditor *subwin = NULL;

    // find top split compare window
    for (int i=winlist.size()-1; i >= 0; i--)
    {
        QMdiSubWindow *sub = winlist[i];
        subwin =  dynamic_cast<QMdiMaskEditor*>(sub);
        if (subwin) break;
    }

    return subwin;
}

//=======================================================================
//=======================================================================
bool Investigator::autoUpdateStatsRT()
{
    return App::settings()->inv().autoUpdateStatsRT;
}

//=======================================================================
//=======================================================================
void Investigator::refreshThumbLoaderSelection()
{
    if (!_splitCmpThumbLoader) return;
    QListWidget *list = _splitCmpThumbLoader->getListWidgetThumbs();
    if (!list) return;

    bool haveL = false;
    bool haveR = false;
    QString fileL = "";
    QString fileR = "";
    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (subwin)
    {
        PRangeImage imgl = subwin->getModel(QMdiSplitCmpWnd2::ViewLeft);
        PRangeImage imgr = subwin->getModel(QMdiSplitCmpWnd2::ViewRight);
        if (!imgl.isNull())
        {
            haveL = true;
            fileL = imgl->getFileName();
        }
        if (!imgr.isNull())
        {
            haveR = true;
            fileR = imgr->getFileName();
        }
    }

    for (int i=0; i<list->count(); i++)
    {
        QListWidgetItem *item = list->item(i);
        if (!item)continue;

        item->setBackgroundColor(GuiSettings::colorListWidgetBg());

        QListWidgetItemEx *ex = dynamic_cast<QListWidgetItemEx *>(item);
        if (!ex) continue;

        if (haveL)
        {
            if (!QString::compare(fileL, ex->getFile(), Qt::CaseInsensitive))
            {
                ex->setBackgroundColor(GuiSettings::colorSelectWinL());
            }
        }

        if (haveR)
        {
            if (!QString::compare(fileR, ex->getFile(), Qt::CaseInsensitive))
            {
                ex->setBackgroundColor(GuiSettings::colorSelectWinR());
            }
        }
    }
}

//=======================================================================
//=======================================================================
void Investigator::refreshMagAndZoom()
{
    slotOnDefaultView();
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitItemClicked(QListWidgetItem * item)
{
    QListWidgetItemEx *ex = dynamic_cast<QListWidgetItemEx *>(item);
    if (!ex) return;

    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin)
    {
        LogError("Failed to find split comparison window.");
        return;
    }

    if (subwin->loadRangeImg(ex->getFile()))
    {
        // LETS NOT TOGGLE THE SELECTED VIEW
        //subwin->toggleSelectedView();
    }

    if (_splitCmpViewCtrls)
    {
        _splitCmpViewCtrls->refreshGui(subwin);
    }

    refreshThumbLoaderSelection();
    refreshMagAndZoom();
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpWndClosed()
{
    SettingsStore::saveDocStates(this, _dockCmpViewCtrls);
    SettingsStore::saveDocStates(this, _dockCmpThumbLoader);

    if (_dockCmpViewCtrls) _dockCmpViewCtrls->hide();
    if (_dockCmpThumbLoader) _dockCmpThumbLoader->hide();

    _stat->setPlotData_1(PProfile());
    _stat->setPlotData_2(PProfile());
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpProfileUpdated(PProfile p1, PProfile p2)
{
    if (!_stat) return;

    _stat->setPlotData_1(p1);
    _stat->setPlotData_2(p2);
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpUpdateStatsRT()
{
    if (!App::settings()->inv().autoUpdateStatsRT) return;
    if (!_stat) return;
    _stat->compare();
}

//=======================================================================
//=======================================================================
void Investigator::slotToggleShowStatPlotsAction()
{
    App::settings()->inv().showStatPlots = !App::settings()->inv().showStatPlots;
    _showStatPlotsAction->setChecked(App::settings()->inv().showStatPlots);

    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin)
    {
        return;
    }

    subwin->showPlots(App::settings()->inv().showStatPlots);
}

//=======================================================================
//=======================================================================
void Investigator::slotToggleAutoUpdateStatsRTAction()
{
    App::settings()->inv().autoUpdateStatsRT = !App::settings()->inv().autoUpdateStatsRT;
    _autoUpdateStatsRTAction->setChecked(App::settings()->inv().autoUpdateStatsRT);
    slotSplitCmpUpdateStatsRT();
}

//=======================================================================
//=======================================================================
void Investigator::slotShowStatPlots(bool show)
{
    App::settings()->inv().showStatPlots = show;
    _showStatPlotsAction->setChecked(App::settings()->inv().showStatPlots);

    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin)
    {
        return;
    }

    subwin->showPlots(App::settings()->inv().showStatPlots);
}

//=======================================================================
//=======================================================================
void Investigator::slotAutoUpdateStatsRT(bool update)
{
    App::settings()->inv().autoUpdateStatsRT = update;
    _autoUpdateStatsRTAction->setChecked(App::settings()->inv().autoUpdateStatsRT);
    slotSplitCmpUpdateStatsRT();
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpUpdateMark()
{
    /*
    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin) return;

    if (!subwin->isMarkMode()) return;

    subwin->updateProfiles();
    */
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitWindowSelChange()
{
    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin) return;

    if (_splitCmpViewCtrls)
    {
        _splitCmpViewCtrls->refreshGui(subwin);
    }
}

//=======================================================================
//=======================================================================
void Investigator::slotShowOptGraphResults()
{
    if (!_dlgStatResults)
    {
        _dlgStatResults = new DlgStatResults(this);
        _dlgStatResults->setModal(false);
    }

    _dlgStatResults->updateStats(_statOptResults.get());

    if (!_dlgStatResults->isVisible())
    {
        _dlgStatResults->show();
    }
}

//=======================================================================
//=======================================================================
void Investigator::slotZoomUpdated(int viewer, double zoom)
{
    if (viewer == QMdiSplitCmpWnd2::ViewLeft)
    {
        _stat->setZoom1(-1*zoom);
    }
    else
    {
       _stat->setZoom2(-1*zoom);
    }
}

//=======================================================================
//=======================================================================
void Investigator::slotChangedTranslationMouse(int viewer, const QVector3D &v)
{
    if (viewer == QMdiSplitCmpWnd2::ViewLeft)
    {
        _stat->setZoom1(-1*v.z());
    }
    else
    {
       _stat->setZoom2(-1*v.z());
    }
}

//=======================================================================
//=======================================================================
void Investigator::slotOnDefaultView()
{
    QMdiSplitCmpWnd2 *subwin = getTopSplitCmp();
    if (!subwin) return;
    GraphicsWidget2 *gfx = subwin->getGraphics();
    if (!gfx) return;

    float mag1 = 0, mag2 = 0, zoom1=0, zoom2=0;

    GraphicsWidget2::ModelData *md = gfx->getModelData(0);
    if (md)
    {
        mag1 = md->zRecommendedView;
        zoom1 = md->trans.z();
    }

    md = gfx->getModelData(1);
    if (md)
    {
        mag2 = md->zRecommendedView;
        zoom2 = md->trans.z();
    }

    _stat->setMag1(mag1);
    _stat->setMag2(mag2);
    _stat->setZoom1(-1*zoom1);
    _stat->setZoom2(-1*zoom2);
}

//=======================================================================
//=======================================================================
void Investigator::slotOnSubWndActivated(QMdiSubWindow *wnd)
{
    if (wnd == NULL) return;

    bool showCmpWnds = true;
    QMdiMaskEditor *maskEditor = dynamic_cast<QMdiMaskEditor *>(wnd);
    if (maskEditor != NULL)
    {
        showCmpWnds = false;
    }

    // show split comparison gui
    if (showCmpWnds)
    {
        //if (!_stat->isVisible())
        //{
            _stat->show();
        //}

        //if (!_dockCmpViewCtrls->isVisible())
        //{
            _dockCmpViewCtrls->show();
        //}

        //if (!_dockCmpThumbLoader->isVisible())
        //{
            _dockCmpThumbLoader->show();
        //}
    }
    else // hide split comparison gui when mask editor is top windwo
    {
        // need to store the visibility in settings.
        //if (_stat->isVisible())
        //{
            _stat->hide();
        //}

        //if (_dockCmpViewCtrls->isVisible())
        //{
            _dockCmpViewCtrls->hide();
        //}

        //if (_dockCmpThumbLoader->isVisible())
        //{
            _dockCmpThumbLoader->hide();
        //}
    }
}

//=======================================================================
//=======================================================================
void Investigator::keyPressEvent(QKeyEvent* event)
{
    QMainWindow::keyPressEvent(event);
}

//=======================================================================
//=======================================================================
void Investigator::runMarkOptimization(RangeImageRenderer *tipModel, PProfile plateProfile)
{
    if (!tipModel)
    {
        LogError("UnExpected Error: tipModel is not valid.");
        return;
    }
    if (!plateProfile)
    {
        LogError("UnExpected Error: plate profile is not valid.");
        return;
    }

    QProgressDialogEx progress("Mark Optimimization..", "Abort", this);
    progress.setWindowModality(Qt::WindowModal);

    std::tr1::shared_ptr<ThreadStatMarkOpt> threadMarkOpt;
    threadMarkOpt.reset(new ThreadStatMarkOpt(_stat->getStatConfig(), plateProfile, tipModel->getModel(), App::settings()->mark().yawInc, App::settings()->mark().yawMin, App::settings()->mark().yawMax));


    bool res = true;
    res = connect(threadMarkOpt.get(), SIGNAL(signalStart()), &progress, SLOT(slotStart()));
    res = connect(threadMarkOpt.get(), SIGNAL(signalProgress(float)), &progress, SLOT(slotProgress(float)));
    res = connect(threadMarkOpt.get(), SIGNAL(signalMsg(QString)), &progress, SLOT(slotMsg(QString)));
    res = connect(&progress, SIGNAL(canceled()), threadMarkOpt.get(), SLOT(slotCancel()));

    progress.setValue(0);
    progress.show();

    threadMarkOpt->startThread();
    while (threadMarkOpt->isRunning())
    {
        QApplication::processEvents();
    }

    //progress.close();
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    //QApplication::processEvents();

    // set stats
    _statOptResults = threadMarkOpt->getResults();
    if (_stat)
    {
        float ang = threadMarkOpt->getResultMaxT().yaw;
        float r = threadMarkOpt->getResultMaxT().r;
        float t = threadMarkOpt->getResultMaxT().t;
        _stat->setOptAng(ang);
        _stat->setOptR(r);
        _stat->setOptT(t);
        _stat->setOptHaveResults(true);
    }

    // update graph
    if (_dlgStatResults)
    {
        _dlgStatResults->updateStats(_statOptResults.get());
    }


    // update the mark
    PProfile tipProfile = threadMarkOpt->getProfileMaxT();
    if (tipProfile)
    {
        // set the tips rotation to match
        QMdiSplitCmpWnd2 *splitwin = getTopSplitCmp();
        if (splitwin)
        {
            // set the tips rotation
            int mnum = splitwin->getGraphics()->getModelNum(tipModel);
            if (mnum != -1)
            {
                splitwin->getGraphics()->setYaw(threadMarkOpt->getMaxYaw(), mnum);
                splitwin->setMarkMode(true);
                splitwin->getRenderer(mnum)->setProfileTip(tipProfile);
                splitwin->setProfile(mnum, tipProfile);
            }

            // update the gui with new rotation value
            if (_splitCmpViewCtrls)
            {
                _splitCmpViewCtrls->refreshGui(splitwin);
            }
        }

        slotSplitCmpProfileUpdated(tipProfile, plateProfile);
    }

    //QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
void Investigator::slotActivateLightingSettings()
{
    if (!getTopSplitCmp())
    {
        UtlQtGui::showMsg("Lighting Settings", "Split Compare Window must be visible to change lighting settings.");
        return;
    }

    DlgLighting dlg(this, this);
    dlg.exec();
}

//=======================================================================
//=======================================================================
void Investigator::slotToggleShowStartupDlg()
{
    App::settings()->inv().showStartupDlg = !App::settings()->inv().showStartupDlg;
    _actionViewShowStartupDlg->setChecked(App::settings()->inv().showStartupDlg);
}

//=======================================================================
//=======================================================================
void Investigator::slotOnMaskEditorFileSaved(QString file)
{
    if (_splitCmpThumbLoader == NULL) return;

    _splitCmpThumbLoader->validateInsert(file);
}
