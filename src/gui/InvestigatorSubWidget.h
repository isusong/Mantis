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

#ifndef __INVESTIGATORSUBWIDGET_H__
#define __INVESTIGATORSUBWIDGET_H__
#include <QWidget>
#include "../core/RangeImage.h"
#include "../core/Profile.h"
#include <QGridLayout>
#include <QSplitter>
#include "RangeImageViewer.h"
#include "qwt-plots/statPlot.h"
#include <QGroupBox>
#include <QToolButton>

/**
 * A base class widget for interfacing with tips or marks.
 *
 * This class uses Qwt (http://qwt.sourceforge.net/).
 *
 * @author Laura Ekstrand
 */

class InvestigatorSubWidget: public QWidget
{
	Q_OBJECT

public:
    InvestigatorSubWidget(PRangeImage rangeImage, QWidget *parent = 0);
	virtual ~InvestigatorSubWidget();

    virtual void setRangeImage(PRangeImage rangeImage) {}
    virtual PRangeImage getRangeImage();

public slots:
	void flip(); ///< Flips the profile
	///Pass-thru for adding the search window.
	void setSearchWindow(int loc, int width);

signals:
	///Give the GUI a status message.
	void statusMessage(QString msg);
	///Announce that the profile has changed.
	void profileChanged(Profile* profile, bool masked);

protected:
    //Data objects.
    Profile* profile; ///< The virtual mark or cross-section.

    //Gui Objects.
    QGridLayout* grid;
    QSplitter* horizontalSplitter; ///< Main splitter.
    RangeImageViewer* graphics; ///< 3D view
    QSplitter* verticalSplitter; ///< Right-hand splitter.
    statPlot* plot; ///< Profile plot
    QGroupBox* controls; ///< Holds the relevant controls
    QToolButton* flipButton; ///< Flips the profile.

    //Private member functions
    ///Make the GUI connections.
    virtual void makeConnections();
    ///Assemble the GUI.
    virtual void assemble();
    ///Make the plots go stale.
    void stalePlots();
};
#endif //!defined __INVESTIGATORSUBWIDGET_H__
