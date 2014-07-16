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

#ifndef __GRAPHICSWIDGET_H__
#define __GRAPHICSWIDGET_H__
#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPaintEvent>
#include <QColor>
#include <QString>
#include <QPoint>
#include <QSettings>
#include "../core/RangeImage.h"

class graphicsWidget : public QWidget
{
	Q_OBJECT

	//Inspired by the QT Scribble Example:
	//http://qt-project.org/doc/qt-4.8/widgets-scribble.html

	//Member variables.
	RangeImage* fileContents; 
	QImage maskImage;
	QImage texture;	
	int textureWidth;
	int textureHeight;
	bool toWidth; //Is the view scaled to width or to height?
	QSettings settings; //settings object.
	float toolSize; //size of brush/eraser.
	QPoint capturedPos; //position of the mouse when pressed last.
	bool drawing; //Are they drawing?
	QColor maskColor; //Pen color for masked areas.
	QColor emptyColor; //Pen color for kept (non-masked) areas;
	bool drawMode; //Using the brush?
	bool eraseMode; //Using the eraser?
	bool modified; //modified?

	//Private Functions
	void paintEvent(QPaintEvent* event); //Draw the scene
	void drawTo(const QPoint endpoint); //Function for drawing into mask.

	public:
		graphicsWidget(QWidget* parent);
		~graphicsWidget();

		//Mouse Capture functions
		void mousePressEvent(QMouseEvent * event);
		void mouseMoveEvent(QMouseEvent * event);
		void mouseReleaseEvent(QMouseEvent * event);

		//Screen Capture Function
		bool isModified(); //Was the file modified?

	public slots:
		void setFile(QString filename); //Load a file.
		bool saveFile(QString filename);
		void toggleDraw();
		void toggleErase();
		void readSettings(); //Reads the settings from the settings file.
};


#endif //! defined __GRAPHICSWIDGET_H__
