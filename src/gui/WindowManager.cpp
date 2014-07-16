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

#include "WindowManager.h"
#include "qwt-plots/statPlot.h"
#include <exception>
#include "RangeImageViewer.h"
#include "Investigator.h"

WindowManager::WindowManager(QObject* parent):
	QObject(parent)
{

}

WindowManager::~WindowManager()
{
	//Laura: It seems that QT is doing the cleanup,
	//so this is commented out.
	//Unfortunately, not all windows could be
	//parented. Therefore, manual cleanup.
	//if (!hashMap.empty())
	//{
	//	QHash<QString, QObject*>::const_iterator i;
	//	for (i = hashMap.constBegin(); i != hashMap.constEnd(); ++i)
	//		delete i.value();
	//}
}

QObject*
WindowManager::getWindowByName(const QString& name)
{
	if (hashMap.contains(name))
		return hashMap.value(name);
	else
		return NULL;
}


QString
WindowManager::getProfilePlot(const QString& name,
	Profile* profile, const QString& penColor, bool mask,
	int width, int height)
{
	//Set up the window.
	statPlot* plot = new statPlot();
	plot->setProfile(profile, mask);
	QColor color;
	color.setNamedColor(penColor);
	plot->setProfilePen(color);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->setObjectName(name);
	plot->resize(width, height);
	plot->setWindowTitle(name);
	plot->show();

	//Delete the old window with that name (if it exists).
	if (hashMap.contains(name))
		delete hashMap.value(name);

	//Store the new window.
	hashMap.insert(name, plot);

	//Make sure the window is deleted out of the hash map
	//when closed.
	connect(plot, SIGNAL(destroyed(QObject*)), this,
		SLOT(deleteWindowEntry(QObject*)));

	return name;
}

void
WindowManager::addSearchWindow(const QString& name, int location,
	int width)
{
	if (hashMap.contains(name))
	{
		QObject* window = hashMap.value(name);
		try
		{
			statPlot* statwindow = dynamic_cast<statPlot*>(window);
			if (NULL != statwindow)
				statwindow->setSearchWindow(location, width);
		}
		catch (std::exception& e) {};
	}
}

QString
WindowManager::getRangeImageWindow(const QString& name,
    PRangeImage rangeImage, int flatDimension,
	int width, int height)
{
	RangeImageViewer* viewer = new RangeImageViewer(rangeImage,
		flatDimension, width, height);
	viewer->setAttribute(Qt::WA_DeleteOnClose);
	viewer->setObjectName(name);
	viewer->setWindowTitle(name);
	viewer->show();

	//Delete the old window with that name (if it exists).
	if (hashMap.contains(name))
		delete hashMap.value(name);

	//Store the new window.
	hashMap.insert(name, viewer);

	//Make sure the window is deleted out of the hash map
	//when closed.
	connect(viewer, SIGNAL(destroyed(QObject*)), this,
		SLOT(deleteWindowEntry(QObject*)));

	return name;
}

QString
WindowManager::getInvestigatorGUI(const QString& name)
{
	Investigator* gui = new Investigator();
	gui->setAttribute(Qt::WA_DeleteOnClose);
	gui->setObjectName(name);
	gui->setWindowTitle(tr("Mantis Investigator"));
	gui->show();

	//Delete the old window with that name (if it exists).
	if (hashMap.contains(name))
		delete hashMap.value(name);

	//Store the new window.
	hashMap.insert(name, gui);

	//Make sure the window is deleted out of the hash map
	//when closed.
	connect(gui, SIGNAL(destroyed(QObject*)), this,
		SLOT(deleteWindowEntry(QObject*)));

	return name;
}

QString
WindowManager::deleteWindow(const QString& name)
{
	delete hashMap.value(name);
	hashMap.remove(name);
	return name;
}

void
WindowManager::deleteWindowEntry(QObject* obj)
{
	//FIXME: Does the name actually persist this far?
	hashMap.remove(obj->objectName());
}
