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

public:
    enum WidgetLoc
    {
        None,
        Left,
        Right
    };

public:
    ///Create an empty scene.
    GraphicsWidget(WidgetLoc loc=None, QWidget* parent = 0);
	virtual ~GraphicsWidget();
	///Add the model to the scene. Does not take ownership of the pointer.
	void setModel(GenericModel* newModel);

    void setWidgetLoc(WidgetLoc loc) { _wloc = loc; }

	//Mouse functions
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent * event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void setMouseMultipliers(QVector3D newMult);

    void setWindowSelected(bool sel);
    bool getWindowSelected() { return _showWindowSelected; }

    const QVector3D& getTrans() { return _trans; }
    const QVector3D& getRot() { return _rot; }
    double getTransX();
    double getTransY();
    double getTransZ();
    double getYaw();
    double getPitch();
    double getRoll();
    void setTrans(const QVector3D &trans);
    void setRot(const QVector3D &rot);
    void setTransX(double t);
    void setTransY(double t);
    void setTransZ(double t);
    void setYaw(double deg);
    void setPitch(double deg);
    void setRoll(double deg);

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
    void onLButtonDown(int x, int y);
    void onChangedTranslation(QVector3D trans);
    void onChangedRotation(QVector3D rot);

protected:
    //Protected member functions

    virtual void keyPressEvent(QKeyEvent * event);

    ///Perform init functions.
    void initializeGL();
    ///Do the actual rendering.
    void paintGL();
    ///Convert back from screen to normalized device coordinates.
    QPointF screenToNDC(const QPoint& point);

    void drawWindowSelected();

    void setModelView();

protected:

  //Member variables.
  bool _showWindowSelected;
  WidgetLoc _wloc;

  ///QT Pointer to model object.
  QPointer<GenericModel> _model;
  ///Cached bounding box dimensions from model.
  float _bbX, _bbY, _bbZ;
  ///Where to put the camera to see the whole model.
  float _zRecommendedView;
  ///Whether or not I should draw the world coordinate system.
  bool _drawCS;

  //Mouse members
  ///Mouse position as of most recent click.
  QPoint _capturedPos;
  ///Optional multipliers for mouse movements
  QVector3D _mouseMult;
  ///Matrix for scene translations.
  //QMatrix4x4 _sceneTranslation;
  ///Matrix for scene rotations.
  //QMatrix4x4 _sceneRotation;
  ///TrackBall object for scene rotations.
  TrackBall _sphere;
  ///Identity quaternion for passing into TrackBall.
  //QQuaternion _quat;
  QQuaternion _quatRotation;

  QVector3D _rot;
  QVector3D _trans;
  ///Timer for updating openGL regularly.
  QTimer* _timer;

};

#endif //! defined __GRAPHICSWIDGET_H__
