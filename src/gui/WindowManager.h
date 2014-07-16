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

#ifndef __WINDOWMANAGER_H__
#define __WINDOWMANAGER_H__
#include <QObject>
#include "../core/Profile.h"
#include "../core/RangeImage.h"
#include <QHash>

class WindowManager: public QObject
{
	Q_OBJECT

	///Holds on to your windows by a string name.
	QHash<QString, QObject*> hashMap;

	//Private methods.
	///Retrieve a pointer to your window.
	/**
	 * FYI: If there is no window by that name,
	 * this will return NULL.
	 */
	QObject* getWindowByName(const QString& name);

  public:
	///Create an empty window manager to fill.
	WindowManager(QObject* parent = 0);
	virtual ~WindowManager();

  public slots:
	///Creates and shows a profile plot.
	/**
	 * @param name will be your key for retrieving your window.
	 * It is returned for you as well.
	 * If you give the same name as a previous window, the previous
	 * window will be deleted.
	 * @param profile is your profile object
	 * @param mask specifies true if you want it masked, false if not.
	 * @param penColor is what color you want the line (default: red)
	 * Colors are given at http://www.w3.org/TR/SVG/types.html#ColorKeywords
	 */
	QString getProfilePlot(const QString& name, 
		Profile* profile, const QString& penColor = "red", 
		bool mask = true, int width = 600, int height = 300);

	///Adds a search window to an existing profile plot.
	/**
	 * If the window with ID name does not exist 
	 * or is not a profile plot,
	 * this will silently fail.
	 *
	 * If you want to clear off an existing search window, pass a width
	 * of 0.
	 */
	void addSearchWindow(const QString& name, int location, int width);

	///Creates and shows a 3D viewer for a Range Image.
	/**
	 * @param name will be your key for retrieving your window.
	 * It is returned for you as well.
	 * If you give the same name as a previous window, the previous
	 * window will be deleted.
	 *
	 * flatDimension specifies if a particular dimension of the object
	 * is known to be very thin.
	 * For instance, for a plate, z is very small.  In this
	 * case, pass flatDimension = 2; this will make navigation speed up
	 * in the z direction so zoom in does not take forever.
	 * If no dimensions are flat, pass in -1 or a value that is not
	 * 0, 1, or 2.
	 */
	QString getRangeImageWindow(const QString& name,
		RangeImage* rangeImage, int flatDimension = -1, 
		int width = 500, int height = 500);

	///Creates and shows an Investigator GUI.
	/**
	 * This is a monolithic-style graphical user interface that allows
	 * you to open tips and marks and compare them interactively.
	 *
	 * @param name will be your key for retrieving your window.
	 * It is returned for you as well.
	 * If you give the same name as a previous window, the previous
	 * window will be deleted.
	 */
	QString getInvestigatorGUI(const QString& name);

	///Delete the window object and entry in the hash map with @param name.
	QString deleteWindow(const QString& name);

	///Delete the window entry in the hash map belonging to obj.
	/**
	 * This will not destroy obj, only remove its entry.
	 */
	void deleteWindowEntry(QObject* obj);
};

#endif //!defined __WINDOWMANAGER_H__
