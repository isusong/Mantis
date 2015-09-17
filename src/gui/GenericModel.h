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
#include "Selection.h"

/**
 * Abstract base class defining an
 * object that GraphicsWidget can
 * display.
 *
 * @author Laura Ekstrand
 */

class GraphicsWidget2;

class GenericModel: public QObject
{
	Q_OBJECT

public:
	///Create a parented model
	GenericModel(QWidget *parent = 0);
	virtual ~GenericModel();

	///Draw the model.
    virtual void draw(QGLWidget* scene) = 0;

    virtual void updateStatsSelection(QGLWidget* scene, int winx, int winy) = 0;

	//Getters & setters.
	///Get minimum corner of bounding box.
    const QVector3D& getBbMin() const {return _bbMin;}
	///Get maximum corner of bounding box.
    const QVector3D& getBbMax() const {return _bbMax;}
    QVector3D getBbSize() const { return _bbMax - _bbMin; }
    QVector3D getBbSizeHalf() const { return (getBbSize() * .5f); }
    QVector3D getBbCenter() const { return _bbMin + getBbSizeHalf(); }



    virtual void scheduleSelectionUpdate(int x, int y) { Q_UNUSED(x); Q_UNUSED(y); }
    virtual QPointF getBasis() { return QPointF(0,0); }
    virtual void redrawSelection(float x, float y) { Q_UNUSED(x); Q_UNUSED(y); }
    virtual void setSelectionEnabled(bool status) { Q_UNUSED(status); }
    virtual void setSelectionMode(Selection::drawModes mode) { Q_UNUSED(mode); }
    virtual void setSelectionMultiplier(int mult) { Q_UNUSED(mult); }


    void setBbMin(const QVector3D &v) { _bbMin = v; }
    void setBbMax(const QVector3D &v) { _bbMax = v; }

    virtual void setSearchBox(int y, int height, int dataLen, bool draw=true) { Q_UNUSED(y); Q_UNUSED(height); Q_UNUSED(dataLen); Q_UNUSED(draw);}
    virtual void setSearchBoxColor(float r, float g, float b, float a) { Q_UNUSED(r); Q_UNUSED(g); Q_UNUSED(b); Q_UNUSED(a); }

    virtual bool isValid() { return true; }
    virtual void logInfo();

  signals:
	///Tell rendering class it needs to update.
	void updateGL();


protected:
  //Minimum bounding box corner.
  QVector3D _bbMin;
  ///Maximum bounding box corner.
  QVector3D _bbMax;
};

typedef std::tr1::shared_ptr<GenericModel> PGenericModel;

#endif //!defined __GENERICMODEL_H__
