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

#ifndef __GENERICMODEL_H__
#define __GENERICMODEL_H__
#include <QObject>
#include <QVector3D>
#include <QImage>
#include <QGLContext>

/**
 * Abstract base class defining an
 * object that GraphicsWidget can
 * display.
 *
 * @author Laura Ekstrand
 */

class GraphicsWidget;

class GenericModel: public QObject
{
	Q_OBJECT

  protected:
	//Minimum bounding box corner.
	QVector3D bbMin;
	///Maximum bounding box corner.
	QVector3D bbMax;

  public:
	///Create a parented model
	GenericModel(QWidget *parent = 0);
	virtual ~GenericModel();

	///Draw the model.
	virtual void draw(GraphicsWidget* scene) = 0;

	//Getters & setters.
	///Get minimum corner of bounding box.
	inline const QVector3D& getBbMin()
		{return bbMin;}
	///Get maximum corner of bounding box.
	inline const QVector3D& getBbMax()
		{return bbMax;}

  signals:
	///Tell rendering class it needs to update.
	void updateGL();
};

#endif //!defined __GENERICMODEL_H__
