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

#include "settingsDialog.h"
#include "ui_settingsDialog.h"
#include <iostream>

using std::cout;
using std::endl;

settingsDialog::settingsDialog(QWidget* parent) :
	QDialog(parent), sdui(new Ui::settingsDialog)
{
	sdui->setupUi(this);
	readSettings();
}

settingsDialog::~settingsDialog()
{
	delete sdui;
}

void
settingsDialog::writeSettings()
{
	settings.setValue("toolSize", sdui->toolSizeSpinBox->value());
}

void
settingsDialog::readSettings()
{
	//Read settings from settings file.
	//If the setting does not exist, initialize it.
	sdui->toolSizeSpinBox->setValue(
		settings.value("toolSize", 1.00).toDouble());
}

void
settingsDialog::on_buttonBox_accepted()
{
	writeSettings();
	emit settingsChanged();
}
