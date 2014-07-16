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

#include "View.h"

View::View(QObject* parent):
	QObject(parent)
{

}

View::~View()
{

}

QString
View::getProfilePlot(const QString& name,
	Profile* profile, const QString& penColor, bool mask, 
	int width, int height)
{
	emit needProfilePlot(name, profile, penColor, mask, 
		width, height);
	return name;
}

void
View::addSearchWindow(const QString& name, int location,
	int width)
{
	emit needToAddSearchWindow(name, location, width);
}

QString
View::getRangeImageWindow(const QString& name,
	RangeImage* rangeImage, int flatDimension, 
	int width, int height)
{
	emit needRangeImageWindow(name, rangeImage, 
		flatDimension, width, height);
	return name;
}

QString
View::getRangeImageWindow(const QString& name,
	RangeImage* rangeImage, const QString& objectType,
	int width, int height)
{
	int flatDimension = -1;
	if (("plate" == objectType) ||
		("Plate" == objectType) ||
		("PLATE" == objectType))
	{
		flatDimension = 2;
	}

	emit needRangeImageWindow(name, rangeImage, 
		flatDimension, width, height);
	return name;
}

QString
View::getInvestigatorGUI(const QString& name)
{
	emit needInvestigatorGUI(name);
	return name;
}

QString
View::deleteWindow(const QString& name)
{
	emit needToDeleteWindow(name);
	return name;
}
