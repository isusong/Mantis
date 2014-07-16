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
#include <QGLWidget>
#include <QPointer>
#include "GenericModel.h"
#include <QMatrix4x4>
#include <QPoint>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector4D>
#include "../QtBoxesDemo/QTrackBall/trackball.h"
#include <QPointF>
#include <QTimer>

/**
 * Class for displaying a GenericModel object in a QT widget 
 * using OpenGL.
 *
 * @author Laura Ekstrand
 */
 
class GraphicsWidget: public QGLWidget
{
	Q_OBJECT

  protected:
	///QT Pointer to model object.
	QPointer<GenericModel> model;
	///Cached bounding box dimensions from model.
	float bbX, bbY, bbZ;
	///Where to put the camera to see the whole model.
	float zRecommendedView;
	///Whether or not I should draw the world coordinate system.
	bool drawCS;

	//Mouse members
	///Mouse position as of most recent click.
	QPoint capturedPos;
	///Optional multipliers for mouse movements
	QVector3D mouseMult;
	///Matrix for scene translations.
	QMatrix4x4 sceneTranslation;
	///Matrix for scene rotations.
	QMatrix4x4 sceneRotation;
	///TrackBall object for scene rotations.
	TrackBall sphere;
	///Identity quaternion for passing into TrackBall.
	QQuaternion quat;
	///Timer for updating openGL regularly.
	QTimer* timer;

	//Protected member functions
	///Perform init functions.
    void initializeGL();
	///Do the actual rendering.
    void paintGL();
	///Convert back from screen to normalized device coordinates.
	QPointF screenToNDC(const QPoint& point);

  public:
    ///Create an empty scene.
	GraphicsWidget(QWidget* parent = 0);
	virtual ~GraphicsWidget();
	///Add the model to the scene. Does not take ownership of the pointer.
	void setModel(GenericModel* newModel);

	//Mouse functions
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent * event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void setMouseMultipliers(QVector3D newMult);

  public slots:
	///Update sceneRotation and call updateGL().
	void updateScene();
	///How to react when window is resized.
	void resizeGL(int width, int height);
	///Turn the coordinate system on or off.
	void setDrawCS(bool status);
	///Set background color.
	void setBackgroundColor(QColor color);

  signals:
	///Window coordinates where user clicked.
	void doubleClicked(int x, int y);
};

#endif //! defined __GRAPHICSWIDGET_H__
