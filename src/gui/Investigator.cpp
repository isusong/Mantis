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

#include "TipWidget.h"
#include "PlateWidget.h"
#include "QListWidgetItemEx.h"
#include "QMdiSplitCmpWnd.h"

//=======================================================================
//=======================================================================
Investigator::Investigator(QWidget* parent):
    QMainWindow(parent)
{
    // init persistent settings
    QSettings settings;
    settings.beginGroup("investigator");
    _lastDirectory = settings.value("lastDir", QDir::currentPath()).toString();
    settings.endGroup();


	//Create the data objects.
    //_lastDirectory = QDir::currentPath();
    _slotFilled_1 = false;
    _slotFilled_2 = false;

	//Create the gui objects.
    _center = new QWidget(this);
    _mainLayout = new QVBoxLayout(_center);
    _area = new QMdiArea(this);
    _stat = new StatisticsWidget(this);
    _fileMenu = new QMenu(tr("&File"), this);
    _openTipAction = new QAction(this);
    _openPlateAction = new QAction(this);
    _viewMenu = new QMenu(tr("&View"), this);
    _toolsMenu = new QMenu(tr("&Tools"), this);
    _tileAction = new QAction(this);
    _cascadeAction = new QAction(this);
    _statSettingsAction = new QAction(this);
    _showSplitCompareAction = new QAction(this);

	//Title the actions.
    _openTipAction->setText(tr("Open &Tip"));
    _openPlateAction->setText(tr("Open Test &Mark"));
    _tileAction->setText(tr("&Tile Windows"));
    _cascadeAction->setText(tr("&Cascade Windows"));
    _statSettingsAction->setText(tr("&Statistics Settings"));
    _showSplitCompareAction->setText(tr("Show Split Compare"));


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

    QSettings settings;
    settings.beginGroup("investigator");
    settings.setValue("lastDir", _lastDirectory);
    settings.endGroup();


    //saveDocStates(_dockCmpViewCtrls, "dockCmpViewCtrls");
    //saveDocStates(_dockCmpThumbLoader, "dockCmpThumbLoader");
}

//=======================================================================
//=======================================================================
void Investigator::closeEvent(QCloseEvent *closeEvent)
{
    saveDocStates(_dockCmpViewCtrls);
    saveDocStates(_dockCmpThumbLoader);

    QMainWindow::closeEvent(closeEvent);
}

//=======================================================================
//=======================================================================
void Investigator::saveDocStates(QDockWidget *doc)
{
    if (!doc) return;
    if (!doc->isVisible()) return;

    QSettings settings;
    settings.beginGroup(doc->windowTitle());
    settings.setValue("pos", doc->pos());
    settings.setValue("floating", doc->isFloating());
    settings.setValue("docWidgetArea", dockWidgetArea(doc));
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void Investigator::loadDocStates(QDockWidget *doc)
{
    if (!doc) return;

    QSettings settings;
    settings.beginGroup(doc->windowTitle());

    bool floating = settings.value("floating", doc->isFloating()).toBool();
    doc->setFloating(floating);

    if (floating)
    {
        QPoint pt = settings.value("pos", doc->pos()).toPoint();
        doc->move(pt);
    }
    else
    {
        Qt::DockWidgetArea docArea = (Qt::DockWidgetArea)settings.value("docWidgetArea", Qt::NoDockWidgetArea).toInt();
        addDockWidget(docArea, doc);
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
    connect(_openTipAction, SIGNAL(triggered()), this, SLOT(openTip()));
    connect(_openPlateAction, SIGNAL(triggered()), this, SLOT(openPlate()));
    connect(_tileAction, SIGNAL(triggered()), _area, SLOT(tileSubWindows()));
    connect(_cascadeAction, SIGNAL(triggered()), _area, SLOT(cascadeSubWindows()));
    connect(_statSettingsAction, SIGNAL(triggered()), this, SLOT(activateStatSettings()));
    connect(_showSplitCompareAction, SIGNAL(triggered()), this, SLOT(showSplitComparison()));
}

//=======================================================================
//=======================================================================
void Investigator::assemble()
{
    _fileMenu->addAction(_openTipAction);
    _fileMenu->addAction(_openPlateAction);
    _toolsMenu->addAction(_tileAction);
    _toolsMenu->addAction(_cascadeAction);
    _toolsMenu->addSeparator();
    _toolsMenu->addAction(_statSettingsAction);
    _toolsMenu->addAction(_showSplitCompareAction);
    menuBar()->addMenu(_fileMenu);
    menuBar()->addMenu(_viewMenu);
    menuBar()->addMenu(_toolsMenu);
    _mainLayout->addWidget(_area);
    _mainLayout->addWidget(_stat);
    _center->setLayout(_mainLayout);
    setCentralWidget(_center);

    createSplitCompareDockWindows();
}

//=======================================================================
//=======================================================================
void Investigator::createSplitCompareDockWindows()
{
    if (_dockCmpViewCtrls) return;

    _dockCmpViewCtrls = new QDockWidget(tr("Split Comparison View Ctrls"), this);
    _dockCmpViewCtrls->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _splitCmpViewCtrls = new SplitCmpViewCtrlsWidget(this, _dockCmpViewCtrls);
    _dockCmpViewCtrls->setWidget(_splitCmpViewCtrls);
    _dockCmpViewCtrls->setMinimumSize(_splitCmpViewCtrls->size().width(), _splitCmpViewCtrls->size().height());

    addDockWidget(Qt::RightDockWidgetArea, _dockCmpViewCtrls);
    _dockCmpViewCtrls->setFloating(true);
    _viewMenu->addAction(_dockCmpViewCtrls->toggleViewAction());

    _dockCmpViewCtrls->hide();



    _dockCmpThumbLoader = new QDockWidget(tr("Split Comparison Thumb Loader"), this);
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

    connect(_splitCmpThumbLoader->getListWidgetThumbs(), SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotSplitItemClicked(QListWidgetItem*)));
}

//=======================================================================
//=======================================================================
void Investigator::showSplitComparison()
{
    QMdiSplitCmpWnd *wnd = getTopSplitCmp();
    if (wnd)
    {
        QMessageBox::warning(this, "Split Comparison", "Split comparision window already opened"); // TODO: FIGURE OUT HOW TO GREY OUT THE MENU ITEM UNTIL WINDOW IS CLOSED
        return;
    }


    // do we have any windows open
    std::vector<InvestigatorSubWidget*> subs;
    getCurrentWindows(&subs);

    // if there are no subwindows already open then ask for a project folder
    if (subs.size() <= 0)
    {
        QString dir = QFileDialog::getExistingDirectory(NULL, "Select Project Folder", _lastDirectory, QFileDialog::ShowDirsOnly);
        if (dir == "")
        {
            // user canceled
            return;
        }
        _lastDirectory = dir;
    }
    else
    {
        // TODO: is _lastDirectory the correct one, or do we need to go up one directory to get to the project folder
    }


    if (!_splitCmpThumbLoader->setProjectFolder(_lastDirectory))
    {
        QMessageBox::critical(NULL, "Project Folder Error", "No icon.png with associated .mt files were found.");
        return;
    }

    //Create the new window
    QMdiSplitCmpWnd* window = new QMdiSplitCmpWnd(_splitCmpViewCtrls, _area);
    window->init();

    _area->addSubWindow(window);
    //splitter->show();
    _area->tileSubWindows();

    // lets load up any existing range inmages
    for (unsigned int i=0; i<subs.size(); i++)
    {
        PRangeImage rimg = subs[i]->getRangeImage();
        window->setRangeImg(rimg, i);
    }

    _splitCmpViewCtrls->connectSplitCmp(window);
    _splitCmpViewCtrls->refreshGui(window);

    loadDocStates(_dockCmpViewCtrls);
    loadDocStates(_dockCmpThumbLoader);
    _dockCmpViewCtrls->show();
    _dockCmpThumbLoader->show();

    connect(window, SIGNAL(onClosed()), this, SLOT(slotSplitCmpWndClosed()));
}

//=======================================================================
//=======================================================================
void Investigator::openTip()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
        tr("Open .mt file"), _lastDirectory,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
    _fileInfo.setFile(filename);
    _lastDirectory = _fileInfo.absolutePath();

	//Create the new widget.
    QMdiSubWindow* window = new QMdiSubWindow(_area);
	window->setAttribute(Qt::WA_DeleteOnClose);
    //PRangeImage tip(new RangeImage(filename, window));
    PRangeImage tip(new RangeImage(filename));
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
    window->setWindowTitle(_fileInfo.fileName());
    _area->addSubWindow(window);
	tipWidget->show();
    _area->tileSubWindows();
	assignSlot(tipWidget);
}

//=======================================================================
//=======================================================================
void Investigator::openPlate()
{
	//Get the filename.
	QString filename = QFileDialog::getOpenFileName(this,
        tr("Open .mt file"), _lastDirectory,
		tr("Mantis File (*.mt)"));
	if (filename.isNull()) return; //user canceled.

	//Update the last directory.
    _fileInfo.setFile(filename);
    _lastDirectory = _fileInfo.absolutePath();

	//Create the new widget.
    QMdiSubWindow* window = new QMdiSubWindow(_area);
	window->setAttribute(Qt::WA_DeleteOnClose);
    //PRangeImage plate(new RangeImage(filename, window));
    PRangeImage plate(new RangeImage(filename));
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
    window->setWindowTitle(_fileInfo.fileName());
    _area->addSubWindow(window);
	plateWidget->show();
    _area->tileSubWindows();
	assignSlot(plateWidget);
}

//=======================================================================
//=======================================================================
void Investigator::assignSlot(InvestigatorSubWidget* window)
{
    if (!_slotFilled_1)
	{
        _slotFilled_1 = true;
        connect(window, SIGNAL(profileChanged(Profile*, bool)), _stat, SLOT(setPlotData_1(Profile*)));
        connect(_stat, SIGNAL(searchWindowUpdated_1(int, int)), window, SLOT(setSearchWindow(int, int)));
        connect(window, SIGNAL(destroyed()), this, SLOT(emptySlot_1()));
	}
    else if (!_slotFilled_2)
	{
        _slotFilled_2 = true;
        connect(window, SIGNAL(profileChanged(Profile*, bool)), _stat, SLOT(setPlotData_2(Profile*)));
        connect(_stat, SIGNAL(searchWindowUpdated_2(int, int)), window, SLOT(setSearchWindow(int, int)));
		connect(window, SIGNAL(destroyed()),
			this, SLOT(emptySlot_2()));
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
    _stat->setPlotData_1(NULL);
	updateEnabledStatus();
}

//=======================================================================
//=======================================================================
void Investigator::emptySlot_2()
{
    _slotFilled_2 = false;
    _stat->setPlotData_2(NULL);
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
QMdiSplitCmpWnd* Investigator::getTopSplitCmp()
{
    QList<QMdiSubWindow*> winlist = _area->subWindowList(QMdiArea::StackingOrder); //	The windows are returned in the order in which they are stacked, with the top-most window being last in the list.)
    QMdiSplitCmpWnd *subwin = NULL;

    // find top split compare window
    for (int i=winlist.size()-1; i >= 0; i--)
    {
        QMdiSubWindow *sub = winlist[i];
        if (sub->windowTitle() == QString("Split Comparison") )
        {
            subwin =  dynamic_cast<QMdiSplitCmpWnd*>(sub);
            break;
        }
    }

    return subwin;
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitItemClicked(QListWidgetItem * item)
{
    QListWidgetItemEx *ex = dynamic_cast<QListWidgetItemEx *>(item);
    if (!ex) return;

    QMdiSplitCmpWnd *subwin = getTopSplitCmp();
    if (!subwin)
    {
        LogError("Failed to find split comparison window.");
        return;
    }

    subwin->loadRangeImg(ex->getFile());
    if (_splitCmpViewCtrls)
    {
        _splitCmpViewCtrls->refreshGui(subwin);
    }
}

//=======================================================================
//=======================================================================
void Investigator::slotSplitCmpWndClosed()
{
    saveDocStates(_dockCmpViewCtrls);
    saveDocStates(_dockCmpThumbLoader);

    if (_dockCmpViewCtrls) _dockCmpViewCtrls->hide();
    if (_dockCmpThumbLoader) _dockCmpThumbLoader->hide();
}

//=======================================================================
//=======================================================================
void Investigator::keyPressEvent(QKeyEvent* event)
{
    QMainWindow::keyPressEvent(event);
}
