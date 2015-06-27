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

#include "DlgCleaning.h"

//=======================================================================
//=======================================================================
DlgCleaning::DlgCleaning(QWidget* parent) :
	QDialog(parent)
{
	ui.setupUi(this);
	
	//Optimize functionality.
	connect(ui.testMark, SIGNAL(clicked()), 
		this, SLOT(optimizeForMark()));
	connect(ui.tip, SIGNAL(clicked()), 
		this, SLOT(optimizeForTip()));
}

//=======================================================================
//=======================================================================
DlgCleaning::~DlgCleaning()
{

}

//=======================================================================
//=======================================================================
bool DlgCleaning::haveMod()
{
    if (getSpike()) return true;
    if (getDetrend()) return true;
    if (getComputeCsys()) return true;

    return false;
}

//=======================================================================
//=======================================================================
bool DlgCleaning::getDetrend()
{
	Qt::CheckState state = ui.detrend->checkState();
	if (Qt::Unchecked == state)
		return false;
	else
		return true;
}

//=======================================================================
//=======================================================================
bool DlgCleaning::getComputeCsys()
{
	return ui.csys->isChecked();
}

//=======================================================================
//=======================================================================
bool DlgCleaning::getCentroid()
{
	return ui.centroid->isChecked();
}

//=======================================================================
//=======================================================================
bool DlgCleaning::getFlatHeadScrewdriver()
{
	return ui.flatHead->isChecked();
}

//=======================================================================
//=======================================================================
bool DlgCleaning::getSpike()
{
	Qt::CheckState state = ui.spike->checkState();
	if (Qt::Unchecked == state)
		return false;
	else
		return true;
}

//=======================================================================
//=======================================================================
void DlgCleaning::optimizeForMark()
{
	ui.detrend->setCheckState(Qt::Checked);
	ui.csys->setChecked(true);
	ui.centroid->setChecked(true);
	ui.flatHead->setChecked(false);
	ui.spike->setCheckState(Qt::Unchecked);
}

//=======================================================================
//=======================================================================
void DlgCleaning::optimizeForTip()
{
	ui.detrend->setCheckState(Qt::Unchecked);
	ui.csys->setChecked(true);
	ui.centroid->setChecked(false);
	ui.flatHead->setChecked(true);
	ui.spike->setCheckState(Qt::Checked);
}
