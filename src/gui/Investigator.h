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

#ifndef __INVESTIGATOR_H_
#define __INVESTIGATOR_H_
#include <QMainWindow>
#include <QString>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QMdiArea>
#include "StatisticsWidget.h"
#include <QAction>
#include "InvestigatorSubWidget.h"

class Investigator: public QMainWindow
{
    Q_OBJECT

	//Data Objects.
	QString lastDirectory; ///< Stores last directory of navigation.
	QFileInfo fileInfo;
	bool slotFilled_1; ///< Is the first window slot filled?
	bool slotFilled_2; ///< Is the second window slot filled?

	//Gui Objects.
	QWidget* center; ///< Placeholder for central widget.
	QVBoxLayout* mainLayout;
	QMdiArea* area;
	StatisticsWidget* stat;
	QMenu* fileMenu;
	QAction* openTipAction;
	QAction* openPlateAction;
	QMenu* toolsMenu;
	QAction* tileAction;
	QAction* cascadeAction;
	QAction* statSettingsAction;

	//Private functions.
	void makeConnections();
	void assemble();
    
  public:
    Investigator(QWidget* parent = 0);
    virtual ~Investigator();

  public slots:
	void openTip();
	void openPlate();
	void assignSlot(InvestigatorSubWidget* window); 
	void updateEnabledStatus(); ///< Enable/Disable actions.
	void emptySlot_1(); ///< Update slot 1 to empty status.
	void emptySlot_2(); ///< Update slot 2 to empty status.
	void postStatusMessage(QString msg);
	void activateStatSettings();
};

#endif //!defined __INVESTIGATOR_H__
