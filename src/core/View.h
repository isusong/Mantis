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
 *
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#ifndef __VIEW_H__
#define __VIEW_H__
#include <QObject>
#include "Profile.h"
#include "RangeImage.h"

/**
 * Connect this class via signals and slots to a WindowManager to
 * ask the GUI thread to show windows.
 *
 * @author Laura Ekstrand
 */

class View: public QObject
{
	Q_OBJECT

  public:
	///Create an empty view interface.
	View(QObject* parent = 0);
	virtual ~View();

  public slots:
	///Asks for a profile plot.
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
	QString getProfilePlot(const QString& name, Profile* profile,
		const QString& penColor = "red", 
		bool mask = true, int width = 600, int height = 300);

	///Asks for a 3D viewer for a Range Image.
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
		RangeImage* rangeImage, int flatDimension, 
		int width = 500, int height = 500);

	///Asks for a 3D viewer for a Range Image.
	/**
	 * Same as other getRangeImageWindow, except that, for your
	 * convenience, you can pass in objectType = 
	 * "plate", "Plate", or "PLATE" to get the flat dimension = 2 (z direction).
	 * "default" has no mouse mulitipliers; it assumes the 3D structure is
	 * roughly cubic.
	 *
	 * "plate" will speed up the zoom since the plate is thin in the z direction.
	 */
	QString getRangeImageWindow(const QString& name,
		RangeImage* rangeImage, const QString& objectType = "default", 
		int width = 500, int height = 500);

	///Asks to add a search window to an existing profile plot.
	/**
	 * If the window with ID name does not exist or is not a profile plot,
	 * this will silently fail.
	 *
	 * If you want to clear off an existing search window, pass a width
	 * of 0.
	 */
	void addSearchWindow(const QString& name, int location, int width);

	///Asks for an Investigator GUI.
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

	///Ask to delete the named window.
	QString deleteWindow(const QString& name);

  signals:
	void needProfilePlot(const QString&, Profile*, 
		const QString&, bool, int, int);
	void needToAddSearchWindow(const QString&, int, int);
	void needRangeImageWindow(const QString&, RangeImage*, int, int, int);
	void needInvestigatorGUI(const QString&);
	void needToDeleteWindow(const QString&);
};

#endif //!defined __VIEW_H__
