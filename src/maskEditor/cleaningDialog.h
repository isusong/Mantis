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

#ifndef __CLEANINGDIALOG_H__
#define __CLEANINGDIALOG_H__
#include <QDialog>
#include "ui_cleaningDialog.h"

class cleaningDialog: public QDialog
{
	Q_OBJECT

	//Member variables.
	Ui::cleaningDialog ui;

	public:
		cleaningDialog(QWidget* parent = 0);
		virtual ~cleaningDialog();
	
	public slots:
		bool getDetrend();
		bool getComputeCsys();
		bool getCentroid();
		bool getFlatHeadScrewdriver();
		bool getSpike();

		void optimizeForMark();
		void optimizeForTip();
};
#endif //!defined __CLEANINGDIALOG_H__
