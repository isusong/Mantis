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

#ifndef GRAPHICSWIDGET2_H
#define GRAPHICSWIDGET2_H
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
#include "../core/Profile.h"

/**
 * Class for displaying a GenericModel object in a QT widget
 * using OpenGL.
 *
 * @author Laura Ekstrand
 */

class GraphicsWidget2: public QGLWidget
{
    Q_OBJECT

public:
    enum WidgetLoc
    {
        None,
        Left,
        Right
    };

    enum LinkView
    {
        LinkNone = 0,
        LinkZoom = 1,
        LinkAngles = 2,
        LinkPosition = 4
    };

    struct ModelData
    {
        int id;
        ///QT Pointer to model object.
        PGenericModel model;
        ///Cached bounding box dimensions from model.
        float bbX, bbY, bbZ;
        ///Where to put the camera to see the whole model.
        float zRecommendedView;

        QVector3D rot;
        QVector3D trans;

        bool selected;

        WidgetLoc loc;
        float perWidth;

        ModelData()
        {
            id = 0;
            bbX = 0;
            bbY = 0;
            bbZ = 0;
            zRecommendedView = 0;
            rot = QVector3D(0,0,0);
            trans = QVector3D(0,0,0);
            selected = false;
            loc = Left;
            perWidth = .5;
        }

        bool isValid()
        {
            if (!model.get()) return false;
            return model->isValid();
        }
    };

public:
    ///Create an empty scene.
    GraphicsWidget2(const QGLFormat &format, bool splitMode=false, WidgetLoc loc=None, QWidget* parent = 0);
    virtual ~GraphicsWidget2();

    bool screenShot(const QString &file);

    ///Add the model to the scene. Does not take ownership of the pointer.
    void setModel(PGenericModel newModel, int mnum=0);

    void setWidgetLoc(WidgetLoc loc) { _wloc = loc; } // Not Needed

    //Mouse functions
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent * event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void setMouseMultipliers(QVector3D newMult);

    void setViewLinkOn(bool on);
    bool getViewLinkOn() { return _viewLinkOn; }

    void setViewLinkMode(int mode);
    int getViewLinkMode() { return _viewLinkMode; }
    void enableViewLink(LinkView type);
    void disableViewLink(LinkView type);

    void setWindowSelected(bool sel, int mnum=0);
    bool isWindowSelected(int defnum=0);
    int getSelectedWindow();
    void toggleSelectedWindow();

    void setDefaultView();
    const QVector3D& getTrans(int mnum=0);
    const QVector3D& getRot(int mnum=0);
    double getTransX(int mnum=0);
    double getTransY(int mnum=0);
    double getTransZ(int mnum=0);
    double getYaw(int mnum=0);
    double getPitch(int mnum=0);
    double getRoll(int mnum=0);
    void setTrans(const QVector3D &trans, int mnum=0);
    void setRot(const QVector3D &rot, int mnum=0);
    void setTransX(double t, int mnum=0);
    void setTransY(double t, int mnum=0);
    void setTransZ(double t, int mnum=0);
    void setYaw(double deg, int mnum=0);
    void setPitch(double deg, int mnum=0);
    void setRoll(double deg, int mnum=0);
    void setLinkedTrans(const QVector3D &t);
    void setLinkedTransX(double t);
    void setLinkedTransY(double t);
    void setLinkedTransZ(double t);
    void setLinkedRot(const QVector3D &deg);
    void setLinkedYaw(double deg);
    void setLinkedPitch(double deg);
    void setLinkedRoll(double deg);

    ModelData* getModelData(int num=0);
    ModelData* getModelDataSel(int defnum=0);
    ModelData* getModelDataLnk();
    int getModelCount();
    int getModelNum(GenericModel *pModel); // 0 or 1, or -1 if not found

    int getSplitterX();
    void setSplitterX(int x);
    void updateSplitStatsSelection() { _runSplitStatsSelection = true; }

    virtual void resizeEvent(QResizeEvent *event);

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
    void onChangedTranslationMouse(int mnum, const QVector3D &trans);
    void onChangedRotationMouse(int mnum, const QVector3D &rot);
    void onWindowSelChange();
    void onSplitterMoved(int x, int y);
    void onSplitStatSelectionUpdated();

protected:
    //Protected member functions

    virtual void keyPressEvent(QKeyEvent * event);

    ///Perform init functions.
    void initializeGL();
    ///Do the actual rendering.
    void paintGL();
    ///Convert back from screen to normalized device coordinates.
    QPointF screenToNDC(const QPoint& point);

    void setModelView(ModelData *md, bool drawingMark=false);

    void drawWindowSelected(ModelData *md);
    void drawSplitter();

    void runWindowSel(const QPoint &pt);
    void moveSplitter(const QPoint &pt, bool fireSignal=true);
    bool onSplitter(const QPoint &pt);
    void getWindows(int vpl[4], int vpr[4], int *splitterX=NULL);

    void runSplitStatsSelection();
    void updateSplitStatSelection();

    bool linkOnZoom();
    bool linkOnAng();
    bool linkOnPos();

    float computeZ(ModelData *md1, ModelData *md2, int *mdUsed);

protected:

  //Member variables.
  WidgetLoc _wloc;

  bool _splitMode;
  int _splitterW;
  bool _splitterSel;
  bool _allowSplitterGui; // allow the splitter to be manipulated with the mouse

  ModelData _md0;
  ModelData _md1;

  bool _viewLinkOn;
  int _viewLinkMode;

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


  ///Timer for updating openGL regularly.
  QTimer* _timer;

  bool _runSplitStatsSelection;

};

#endif //! defined __GRAPHICSWIDGET_H__
