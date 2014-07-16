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

#include "StatisticsSettingsDialog.h"

StatisticsSettingsDialog::StatisticsSettingsDialog(int rigid,
	int random, float maxShift, int tSamples, QWidget *parent):
	QDialog(parent)
{
	ui.setupUi(this);	

	//Set the defaults.
	rigidPairsDefault = rigid;
	randomPairsDefault = random;
	maxShiftDefault = maxShift;
	tSamplesDefault = tSamples;
	ui.rigidPairsSpinBox->setValue(rigidPairsDefault);
	ui.randomPairsSpinBox->setValue(randomPairsDefault);
	ui.maxShiftSpinBox->setValue(maxShiftDefault);
	ui.tSampleSizeSpinBox->setValue(tSamplesDefault);

	//Init defaults button.
	connect(ui.defaultsButton, SIGNAL(pressed()), this,
		SLOT(resetToDefaults()));

	//Foward the signals.
	connect(ui.rigidPairsSpinBox, SIGNAL(valueChanged(int)),
		this, SIGNAL(rigidPairsUpdated(int)));
	connect(ui.randomPairsSpinBox, SIGNAL(valueChanged(int)),
		this, SIGNAL(randomPairsUpdated(int)));
	connect(ui.maxShiftSpinBox, SIGNAL(valueChanged(double)),
		this, SIGNAL(maxShiftUpdated(double)));
	connect(ui.tSampleSizeSpinBox, SIGNAL(valueChanged(int)),
		this, SIGNAL(tSamplesUpdated(int)));
}

StatisticsSettingsDialog::~StatisticsSettingsDialog()
{

}

void
StatisticsSettingsDialog::resetToDefaults()
{
	ui.rigidPairsSpinBox->setValue(rigidPairsDefault);
	ui.randomPairsSpinBox->setValue(randomPairsDefault);
	ui.maxShiftSpinBox->setValue(maxShiftDefault);
	ui.tSampleSizeSpinBox->setValue(tSamplesDefault);
}
