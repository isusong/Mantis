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
#include "mainWindow.h"
#include "settingsDialog.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	//QSettings set up.
	QCoreApplication::setOrganizationName("ISU Forensics Research Group");
	QCoreApplication::setOrganizationDomain("vrac.iastate.edu/~song/projects.htm");
	QCoreApplication::setApplicationName("Mask Editor");

	//Main window setup
	mainWindow mwindow;
	mwindow.show();

	//Settings dialog setup
	settingsDialog sd;
	QObject::connect(&mwindow, SIGNAL(needSettingsDialog()), &sd, SLOT(show()));
	QObject::connect(&mwindow, SIGNAL(needSettingsDialog()), &sd, SLOT(readSettings()));
	QObject::connect(&sd, SIGNAL(settingsChanged()), &mwindow, SLOT(updateSettings()));

	return app.exec();
}
