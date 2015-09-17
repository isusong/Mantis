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

#include <QApplication>
#include <iostream>
#include <cstdlib>
#include <QFile>
#include <QTextStream>
#include <QSplashScreen>
#include "../core/RangeImage.h"
#include "../core/CleaningCode/Clean.h"
#include "../core/ScriptInterface.h"
#include "../core/Profile.h"
#include "../core/StatInterface.h"
#include "../core/CsvTable.h"
#include "../core/VirtualTip.h"
#include "../core/Logger.h"
#include "../gui/WindowManager.h"
#include "../core/View.h"
#include "../gui/Investigator.h"
#include "../gui/App.h"
#include "../gui/DlgStartUp.h"
#include "../core/UtlQt.h"
#include <QThread>
#include <cstdio>

using std::cout;
using std::endl;

QScriptValue wait(QScriptContext*, QScriptEngine*);
int runInvestigator(int argc, char** argv);
void initCoreApp();

//=======================================================================
//=======================================================================
int main(int argc, char** argv)
{
    Log::logToFile("log.txt");

    // no args, then just run investigator
    if (argc == 1)
    {
        LogInfo("Running Investigator...");
        return runInvestigator(argc, argv);
    }

    // Input processing.
    if (2 != argc)
    {
        LogError("Usage:  mantis <javascript filename>");
        exit(-1);
    }

    LogInfo("Running Script %s...", argv[1]);


	//Need these for scripting language.
	QApplication app (argc, argv);
	app.setQuitOnLastWindowClosed(false);
    ScriptInterface scripter;

    // for QSettings
    initCoreApp();

	//Allow user to create instances of objects.
    scripter.AddObjectType<RangeImage, QString>("RangeImage");
    scripter.AddObjectType<Clean>("Clean");
    scripter.AddObjectType<Profile, QString>("Profile");
    scripter.AddObjectType<StatInterface>("StatInterface");
    scripter.AddObjectType<CsvTable, int, int, bool>("CsvTable");

	//Register special RangeImage scripting interface.
	//Allow RangeImage* to be passed back as a return arg.
    scripter.RegisterMetaObjectType<RangeImage>();
	//Register the factory function for importing RangeImage.
    scripter.RegisterStaticFunction("importRangeImage", RangeImage::scriptableImport);

	//Register special Profile scripting interface.
	//Allow Profile* to be passed back as a return arg.
    scripter.RegisterMetaObjectType<Profile>();

	//Register special VirtualTip scripting interface.
	//Allow VirtualTip* to be passed back as a return arg.
    scripter.RegisterMetaObjectType<VirtualTip>();
	//Register the factory function for constructing VirtualTip.
    scripter.RegisterStaticFunction("createVirtualTip", VirtualTip::scriptableCreate);


	//Enable data viewing.
	WindowManager windowManager;
	View view;
    QObject::connect(&view, SIGNAL(needProfilePlot(const QString&, Profile*, const QString&, bool, int, int)), &windowManager, SLOT(getProfilePlot(const QString&, Profile*, const QString&, bool, int, int)));
    QObject::connect(&view, SIGNAL(needRangeImageWindow(const QString&, RangeImage*, int, int, int)), &windowManager, SLOT(getRangeImageWindow(const QString&, RangeImage*, int, int, int)));
    QObject::connect(&view, SIGNAL(needToAddSearchWindow(const QString&, int, int)), &windowManager, SLOT(addSearchWindow(const QString&, int, int)));
    QObject::connect(&view, SIGNAL(needInvestigatorGUI(const QString&)),&windowManager, SLOT(getInvestigatorGUI(const QString&)));
    QObject::connect(&view, SIGNAL(needToDeleteWindow(const QString&)), &windowManager, SLOT(deleteWindow(const QString&)));
    scripter.RegisterExistingQObject("view", &view);


	//Enable the wait command.
    scripter.RegisterStaticFunction("wait", wait);

	//Put the scripting in a separate thread.
	//This will be deleted at program termination.
    scripter.setScriptFilename(QString(argv[1]));
	QThread* thread = new QThread();
    QObject::connect(thread, SIGNAL(started()), &scripter, SLOT(Run()));
    QObject::connect(&scripter, SIGNAL(scriptFinished()), thread, SLOT(quit()));
    QObject::connect(thread, SIGNAL(finished()), &app, SLOT(quit()));
    scripter.moveToThread(thread);
	thread->start();


	//Start the GUI.
    int ret = app.exec();


    LogInfo("Mantis Finished %d", ret);
    Log::shutdown();
    return ret;
}

//=======================================================================
//=======================================================================
QScriptValue wait(QScriptContext* context, QScriptEngine* engine)
{
    Q_UNUSED(context);
    Q_UNUSED(engine);

	cout << "Waiting. Press any key to continue." << endl;
    getchar();
	return QScriptValue();
}

//=======================================================================
//=======================================================================
int runInvestigator(int argc, char** argv)
{
    QApplication app(argc, argv);
    initCoreApp();

    // show splash screen
    QPixmap pixmap(":/general/Icons/MantisSplash.png");
    if (pixmap.isNull())
    {
        LogError("Failed to load splash image");
    }
    //QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    UtlQt::delay(5);


    // init persistent settings
    SettingsStore settings;
    settings.loadAll();
    App::settings(&settings);

    if (App::settings()->inv().showStartupDlg)
    {
        DlgStartUp dlg;
        dlg.exec();
    }

    Investigator *gui = new Investigator();
    App::mainWnd(gui);
    gui->setAttribute(Qt::WA_DeleteOnClose);
    gui->setObjectName("gui");
    gui->setWindowTitle("Mantis Investigator");
    gui->show();

    splash.finish(gui); // kill the splash
    int ret = app.exec();

    settings.saveAll();
    LogInfo("Mantis Investigator Finished - exit code: %d", ret);
    return ret;
}

//=======================================================================
//=======================================================================
void initCoreApp()
{
    // for QSettings
    QCoreApplication::setOrganizationName("Iowa State University");
    QCoreApplication::setOrganizationDomain("iastate.edu");
    QCoreApplication::setApplicationName("Mantis");
}
