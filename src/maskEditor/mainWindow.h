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

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileInfo>

namespace Ui {
	class mainWindowForm;
}

class mainWindow: public QMainWindow
{
	Q_OBJECT

	Ui::mainWindowForm* ui;
	QString lastDirectory; ///< Stores last directory of navigation.
	QFileInfo fileInfo;

	void closeEvent(QCloseEvent* event);
	bool discardQuestion();

	private slots:
		void on_actionOpenFile_triggered();
		void on_actionSave_triggered();
		void on_actionSettings_triggered();
		void on_actionAbout_triggered();
		void on_actionDraw_toggled();
		void on_actionErase_toggled();
		void updateSettings();

	public:
		mainWindow(QWidget *parent = 0);		
		virtual ~mainWindow();

	signals:
		void needSettingsDialog();
};
