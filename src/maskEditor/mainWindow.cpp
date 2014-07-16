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

#include "mainWindow.h"
#include "ui_mainWindowForm.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QDir>

mainWindow::mainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::mainWindowForm)
{
	ui->setupUi(this);	
	lastDirectory = QDir::currentPath();
}

mainWindow::~mainWindow()
{
	delete ui;
}

void
mainWindow::closeEvent(QCloseEvent* event) //See QT Application example
{
	if (discardQuestion())
	{
		event->accept();	
	}
	else
	{
		event->ignore();
	}
}

bool
mainWindow::discardQuestion() //See QT application example.
{
	if (ui->centralwidget->isModified())
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::question(this, tr("Confirm discard"),
			tr("The file was modified.\n"
				"Are you sure you want to discard it?"),
			QMessageBox::Ok | QMessageBox::Cancel,
			QMessageBox::Cancel);
		if (QMessageBox::Ok == ret)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void
mainWindow::on_actionOpenFile_triggered()
{
	if (discardQuestion())
	{
		QString filename = QFileDialog::getOpenFileName(this,
			tr("Open .mt file"), lastDirectory,
			tr("Mantis file (*.mt)"));
		if (!filename.isNull())
		{
			ui->centralwidget->setFile(filename);
			lastDirectory = fileInfo.absolutePath();
		}
	}
}

void
mainWindow::on_actionSave_triggered()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save .mt file"), lastDirectory,
		tr("Mantis file (*.mt)"));
	if (!filename.isNull())
	{
		bool saveSuccess = ui->centralwidget->saveFile(filename);
		lastDirectory = fileInfo.absolutePath();
		if (!saveSuccess)
		{
			QMessageBox::warning(this, tr("Error"),
				tr("Save was unsuccessful.\n"));
		}
	}
}

void
mainWindow::on_actionSettings_triggered()
{
	emit needSettingsDialog();
}

void
mainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("About Mask Editor"),
	tr("Mask Editor - Copyright 2012, 2013 Iowa State University\n\n"
	"Created by Laura Ekstrand (ldmil@iastate.edu, laura@jlekstrand.net).\n\n"
	"QT 4.8 program for manual editing of RangeImage file masks.\n\nThis work was funded by Iowa State University through "
	"U.S. Department of Energy Ames Laboratory.  This work was supported by Award No. 2009-DN-R-119 from the National "
	"Institute of Justice, Office of Justice Programs, U.S. Department of Justice."
	"\n\nOpen-source licenced under the GPL v3: http://www.gnu.org/licenses/\n"
    "This program comes with ABSOLUTELY NO WARRANTY."
	));

}

void 
mainWindow::on_actionDraw_toggled()
{
	ui->centralwidget->toggleDraw();
	if(ui->actionDraw->isChecked()) 
	{
		if (ui->actionErase->isChecked())
		{
			//Turn erase off.
			ui->actionErase->toggle();
		}
	}
}

void 
mainWindow::on_actionErase_toggled()
{
	ui->centralwidget->toggleErase();
	if(ui->actionErase->isChecked()) 
	{
		if (ui->actionDraw->isChecked())
		{
			//Turn draw off.
			ui->actionDraw->toggle();
		}
	}
}

void
mainWindow::updateSettings()
{
	ui->centralwidget->readSettings();
}
