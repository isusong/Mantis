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

#define GL_GLEXT_PROTOTYPES
#include "GraphicsWidget2.h"
#include "qmath.h"
#include "../QtBoxesDemo/QGLExtensionWrangler/glextensions.h"
#include "../core/logger.h"
#include "../core/utlqt.h"
#include "RangeImageRenderer.h"
#include "GuiSettings.h"

#define FOVY 45.0f
#define NEARCLIP 0.1f
#define FARCLIP 100000.0f

#ifndef GL_MULTISAMPLE
    #define GL_MULTISAMPLE  0x809D
#endif

//=======================================================================
//=======================================================================
GraphicsWidget2::GraphicsWidget2(const QGLFormat &format, bool splitMode, WidgetLoc loc, QWidget* parent):
    QGLWidget(format, parent),
    _viewLinkOn(false),
    _viewLinkMode(LinkNone),
    _splitMode(splitMode),
    _wloc(loc),
    _splitterW(1),
    _splitterSel(false),
    _allowSplitterGui(false),
    _runSplitStatsSelection(false)
{
    //Set timer to updateGL() every 20 msecs.
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
    _timer->start(20);

    //By default, only multiply mouse movements by 1.
    _mouseMult = QVector3D(1, 1, 1);
    _drawCS = true;

    _md0.id = 0;
    _md0.loc = Left;
    _md0.perWidth = .5f;
    _md1.id = 1;
    _md1.loc = Right;
    _md0.perWidth = .5f;

    if (!_splitMode)
    {
         _md0.loc = None;
         _md0.perWidth = 1;
         _md1.perWidth = 0;
    }
    else
    {
        setWindowSelected(true, 0);
        LogTrace("GraphicsWidget2 - split mode on");
    }

    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
}

//=======================================================================
//=======================================================================
GraphicsWidget2::~GraphicsWidget2()
{

}

//=================== ====================================================
//=======================================================================
bool GraphicsWidget2::screenShot(const QString &file)
{
    paintGL();
    QImage img = QGLWidget::grabFrameBuffer(false);
    return img.save(file);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setViewLinkOn(bool on)
{
    _viewLinkOn = on;

    if (!_viewLinkOn) return;

    // They don't want the views to change when this is set,
    // only that both views will change my the same amount from here on

    /*
    ModelData *mdsel = getModelDataSel();
    ModelData *mdlnk = getModelDataLnk();

    mdlnk->rot = mdsel->rot;
    mdlnk->trans.setZ(mdsel->trans.z()); // match zoom

    RangeImageRenderer *prlnk = dynamic_cast<RangeImageRenderer *>(mdlnk->model.data());
    RangeImageRenderer *prsel = dynamic_cast<RangeImageRenderer *>(mdsel->model.data());
    if (prlnk && prsel)
    prlnk->setLightInfo(prsel->getLightInfo());
    */
}


//=======================================================================
//=======================================================================
void GraphicsWidget2::setViewLinkMode(int mode)
{
    _viewLinkMode = mode;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::enableViewLink(LinkView type)
{
    if (type == LinkNone)
    {
        _viewLinkMode = 0;
    }
    else
    {
        _viewLinkMode |= (int)type;
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::disableViewLink(LinkView type)
{
    _viewLinkMode &= ~(int)type;
}

//=======================================================================
//=======================================================================
bool GraphicsWidget2::linkOnZoom()
{
    return (_viewLinkOn && (_viewLinkMode & LinkZoom));
}

//=======================================================================
//=======================================================================
bool GraphicsWidget2::linkOnAng()
{
    return (_viewLinkOn && (_viewLinkMode & LinkAngles));
}

//=======================================================================
//=======================================================================
bool GraphicsWidget2::linkOnPos()
{
    return (_viewLinkOn && (_viewLinkMode & LinkPosition));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setDefaultView()
{
    ModelData *mdsel = getModelDataSel();

    mdsel->rot = QVector3D(0,0,0);
    mdsel->trans = QVector3D(0,0,0);

    if (!_viewLinkOn)
    {
        LogTrace("View Link not on");
        return;
    }

     ModelData *mdlnk = getModelDataLnk();


     if (linkOnAng())
     {
        mdlnk->rot = QVector3D(0,0,0);
     }

     if (linkOnPos())
     {
        mdlnk->trans.setX(0);
        mdlnk->trans.setY(0);
     }

     if (linkOnZoom())
     {
        mdlnk->trans.setZ(0);
     }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setWindowSelected(bool sel, int mnum)
{
    if (getModelData(mnum)->selected == sel) return;

    getModelData(mnum)->selected = sel;
    //updateGL();
}

//=======================================================================
//=======================================================================
bool GraphicsWidget2::isWindowSelected(int mnum)
{
    return getModelData(mnum)->selected;
}

//=======================================================================
//=======================================================================
const QVector3D& GraphicsWidget2::getTrans(int mnum)
{
   return getModelData(mnum)->trans;
}

//=======================================================================
//=======================================================================
const QVector3D& GraphicsWidget2::getRot(int mnum)
{
   return getModelData(mnum)->rot;
}

//=======================================================================
//=======================================================================
double GraphicsWidget2::getTransX(int mnum)
{
    return getModelData(mnum)->trans.x();
}

//=======================================================================
//=======================================================================
double GraphicsWidget2::getTransY(int mnum)
{
    return getModelData(mnum)->trans.y();
}

//=======================================================================
//=======================================================================
double GraphicsWidget2::getTransZ(int mnum)
{
    return getModelData(mnum)->trans.z();
}

//=======================================================================
// in degrees
//=======================================================================
double GraphicsWidget2::getYaw(int mnum)
{
    return getModelData(mnum)->rot.y();
}

//=======================================================================
// in degrees
//=======================================================================
double GraphicsWidget2::getPitch(int mnum)
{
    return getModelData(mnum)->rot.z();
}

//=======================================================================
//=======================================================================
double GraphicsWidget2::getRoll(int mnum)
{
    return getModelData(mnum)->rot.z();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setTrans(const QVector3D &trans, int mnum)
{
    getModelData(mnum)->trans = trans;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setRot(const QVector3D &rot, int mnum)
{
    getModelData(mnum)->rot = rot;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setTransX(double t, int mnum)
{
    getModelData(mnum)->trans.setX(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setTransY(double t, int mnum)
{
    getModelData(mnum)->trans.setY(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setTransZ(double t, int mnum)
{
    getModelData(mnum)->trans.setZ(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setYaw(double deg, int mnum)
{
    getModelData(mnum)->rot.setY(deg);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setPitch(double deg, int mnum)
{
    getModelData(mnum)->rot.setX(deg);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setRoll(double deg, int mnum)
{
    getModelData(mnum)->rot.setZ(deg);
}

//=======================================================================
// For linked views they don't watch the views to match,
// but when linked they want both views to update by the same amount.
// So if the both views started off the same they will be the same, otherwise they will be different.
// This is so you may need to tweek a model slightly to get it to match up as a good starting point with the other model
// then move together.
//=======================================================================
void GraphicsWidget2::setLinkedTrans(const QVector3D &t)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    QVector3D dif(0,0,0);
    dif = t - md->trans;
    md->trans = t;

    ModelData *mdlnk = getModelDataLnk();
    if (!mdlnk) return;

    mdlnk->trans += dif;
}


//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedTransX(double t)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedTrans(QVector3D(t, md->trans.y(), md->trans.z()));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedTransY(double t)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedTrans(QVector3D(md->trans.x(), t, md->trans.z()));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedTransZ(double t)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedTrans(QVector3D(md->trans.x(), md->trans.y(), t));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedRot(const QVector3D &deg)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    QVector3D dif(0,0,0);
    dif = deg - md->rot;
    md->rot = deg;

    ModelData *mdlnk = getModelDataLnk();
    if (!mdlnk) return;

    mdlnk->rot += dif;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedYaw(double deg)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedRot(QVector3D(md->rot.x(), deg, md->rot.z()));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedPitch(double deg)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedRot(QVector3D(deg, md->rot.y(), md->rot.z()));
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setLinkedRoll(double deg)
{
    ModelData *md = getModelDataSel();
    if (!md) return;

    setLinkedRot(QVector3D(md->rot.x(), md->rot.y(), deg));
}

//=======================================================================
//=======================================================================
GraphicsWidget2::ModelData* GraphicsWidget2::getModelData(int mnum)
{
    if (mnum == 1)
        return &_md1;
    else
        return &_md0;
}

//=======================================================================
//=======================================================================
GraphicsWidget2::ModelData* GraphicsWidget2::getModelDataSel(int defnum)
{
    if (_md0.selected) return &_md0;
    if (_md1.selected) return &_md1;

    if (defnum == -1) return NULL;
    return getModelData(defnum);
}

//=======================================================================
//=======================================================================
GraphicsWidget2::ModelData* GraphicsWidget2::getModelDataLnk()
{
    ModelData *mdsel = getModelDataSel();
    ModelData *mdlnk = NULL;
    if (mdsel->id == 0) mdlnk = getModelData(1);
    else mdlnk = getModelData(0);

    return mdlnk;
}

//=======================================================================
//=======================================================================
int GraphicsWidget2::getModelCount()
{
    int count = 0;
    if (_md0.model.get() != NULL) count++;
    if (_md1.model.get() != NULL) count++;

    return count;
}

//=======================================================================
// 0 or 1, or -1 if not found
//=======================================================================
int GraphicsWidget2::getModelNum(GenericModel *pModel)
{
    if (_md0.model.get() != NULL && _md0.model.get() == pModel) return 0;
    if (_md1.model.get() != NULL && _md1.model.get() == pModel) return 1;
    return -1;
}

//=======================================================================
//=======================================================================
int GraphicsWidget2::getSelectedWindow()
{
    if (_md0.selected) return 0;
    if (_md1.selected) return 1;

    return 0;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::toggleSelectedWindow()
{
    if (_md0.selected)
    {
        _md0.selected = false;
        _md1.selected = true;
    }
    else if (_md1.selected)
    {
        _md0.selected = true;
        _md1.selected = false;
    }
    else
    {
        _md0.selected = false;
        _md1.selected = true;
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::initializeGL()
{
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    //Set clear color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(0.5, 0.5, 0.5, 1.0);
    glLineStipple(1, 0x00FF);

    //Init extensions.
    bool success = getGLExtensionFunctions().resolve(this->context());
    if (!success)
    {
        QMessageBox::critical(this, "Missing Extensions",
            "You are missing support for any of the following:\n\n"
            "Buffers (required for the 3D displays).\n"
            "Framebuffers, renderbuffers (required for virtual mark generation).\n"
            "3D texture images (required by the extension wrangler class).\n\n"
            "The program may fail catastrophically.");
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::paintGL()
{
    if (_runSplitStatsSelection)
    {
        runSplitStatsSelection();
        _runSplitStatsSelection = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int vpl[4], vpr[4];

    if (_splitMode)
    {
        getWindows(vpl, vpr);
        glEnable(GL_SCISSOR_TEST);
    }

    for (int i=0; i<2; i++)
    {
        ModelData *md = getModelData(i);
        if (!md->model) continue;

        bool drawingMark = ((RangeImageRenderer *)md->model.get())->getDrawMark();
        setModelView(md, drawingMark);

        if (_splitMode)
        {
            if (md->loc == Left) glScissor(vpl[0], vpl[1], vpl[2], vpl[3]);
            else if (md->loc == Right) glScissor(vpr[0], vpr[1], vpr[2], vpr[3]);
        }

        md->model->draw(this);

       //Draw world coordinate system.
        //The model is in charge of detaching its shader programs, if any.
        if (!_drawCS) continue;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_LINE_STIPPLE); //dashed line
        float lineLength = 4e3;
        glBegin(GL_LINES);
          glColor3f(0.6f, 0, 0); //red pen
          glVertex3f(0, 0, 0);
          glVertex3f(lineLength, 0, 0); //x
          glColor3f(0, 0.6f, 0); //green pen
          glVertex3f(0, 0, 0);
          glVertex3f(0, lineLength, 0); //y
          glColor3f(0, 0, 0.6f); //blue pen
          glVertex3f(0, 0, 0);
          glVertex3f(0, 0, lineLength);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
        glEnable(GL_DEPTH_TEST);
    }

    if (_splitMode) glDisable(GL_SCISSOR_TEST);

    for (int i=0; i<2; i++)
    {
        ModelData *md = getModelData(i);

        if (md->selected)
        {
            drawWindowSelected(md);
        }
    }

    drawSplitter();

    glFlush();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::runSplitStatsSelection()
{
    if (!_splitMode) return;

    int vpl[4], vpr[4], winx=0, winy=0;
    getWindows(vpl, vpr, &winx);
    winy = vpl[1] + vpl[3] / 2;

    for (int i=0; i<2; i++)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ModelData *md = getModelData(i);
        if (md->model.get() == NULL) continue;

        setModelView(md);
        md->model->updateStatsSelection(this, winx, winy);
    }

    emit onSplitStatSelectionUpdated();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setModelView(ModelData *md, bool drawingMark)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    if (!md || !md->model) return;

    // -_zRecommendedView is the normalized 0 z for the model.
    // its better to not include it in _trans so outside modifiers can link view togethers without
    // worrying about the difference in this value between models.

    // pan and zoom
    glTranslatef(md->trans.x(), md->trans.y(), md->trans.z() - md->zRecommendedView);

    // now rotate the object   
    if (!drawingMark)
    {
        glRotatef(md->rot.x(), 1.0f, 0.0f, 0.0f);
        glRotatef(md->rot.y(), 0.0f, 1.0f, 0.0f);
        glRotatef(md->rot.z(), 0.0f, 0.0f, 1.0f);
    }
    else
    {
        glRotatef(0, 1.0f, 0.0f, 0.0f);
        glRotatef(0, 0.0f, 1.0f, 0.0f);
        glRotatef(0, 0.0f, 0.0f, 1.0f);
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::resizeGL(int width, int height)
{
    //Compute new perspective matrix.
    float aspectRatio = ((float) width)/((float) height);
    QMatrix4x4 perspectiveMatrix;
    perspectiveMatrix.perspective(FOVY, aspectRatio, NEARCLIP, FARCLIP);

    //Load perspective matrix into OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(perspectiveMatrix.constData());

    glViewport(0, 0, width, height);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setDrawCS(bool status)
{
    _drawCS = status;
    updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setBackgroundColor(QColor color)
{
    glClearColor(color.redF(), color.greenF(),
        color.blueF(), 0.0f);
    updateGL();
}

//=======================================================================
//=======================================================================
QPointF GraphicsWidget2::screenToNDC(const QPoint& point)
{
    QPointF ret;
    ret.setX(((2/(float) width())*
        ((float) point.x())) - 1);
    ret.setY(((2/(float) height())*
        ((float) point.y())) - 1);
    ret.setY(-ret.y()); //Correct for window coordinates.
    return ret;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setModel(PGenericModel newModel, int mnum)
{
    ModelData *md = getModelData(mnum);
    md->model = newModel;

    //Cache bounding box.
    QVector3D bbExtent = md->model->getBbMax() - md->model->getBbMin();
    md->bbX = bbExtent.x();
    md->bbY = bbExtent.y();
    md->bbZ = bbExtent.z();

    //Set up camera.
    md->rot = QVector3D(0,0,0);
    md->trans = QVector3D(0,0,0);


    //_sceneRotation = QMatrix4x4();
    //_sceneTranslation = QMatrix4x4();
    md->zRecommendedView = 1.5*md->bbY/(2*qTan(FOVY/2));
    //_sceneTranslation.translate(0, 0, -_zRecommendedView);
    //_trans.setZ(-_zRecommendedView);

    emit onChangedTranslationMouse(md->id, md->trans);
    emit onChangedRotationMouse(md->id, md->rot);
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::mousePressEvent(QMouseEvent* event)
{
    //if (!_model) return;
    _capturedPos = event->pos();
    //_sphere.push(screenToNDC(event->pos()), QQuaternion());

    if (_splitMode)
    {
        if (_allowSplitterGui)
        {
            if (onSplitter(event->pos()))
            {
                _splitterSel = true;
                setCursor(Qt::SplitHCursor);
                return;
            }

            _splitterSel = false;
        }

        runWindowSel(event->pos());
    }

    if (event->button() == Qt::LeftButton)
    {
        emit onLButtonDown(event->pos().x(), event->pos().y());
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::mouseMoveEvent(QMouseEvent* event)
{
    if (_splitterSel)
    {
        // move splitter
        moveSplitter(event->pos());
        return;
    }

     // splitter highlight
    if (_splitMode && _allowSplitterGui && !(Qt::LeftButton & event->buttons()) && !(Qt::RightButton & event->buttons()))
    {
        if (onSplitter(event->pos()))
        {
            setCursor(Qt::SplitHCursor);
        }
        else
        {
            setCursor(Qt::CrossCursor);
        }
    }

    ModelData *md = getModelDataSel();
    if (!md || !md->model) return;

    if (Qt::LeftButton & event->buttons())
    {
        QVector3D delta = QVector3D(_capturedPos - event->pos());
        delta.setX(-delta.x()); //Correct for window coordinates.
        //Convert to world coordinates -ish.
        delta *= (_mouseMult.y()*md->bbY)/height();
        //Correct for zoom -ish.
        delta *= qAbs(md->trans.z() - md->zRecommendedView)/md->zRecommendedView;
        //delta *= qAbs(_sceneTranslation(2, 3))/_zRecommendedView;
        //_sceneTranslation.translate(delta);

        md->trans += delta;
        if (linkOnPos())
        {
            ModelData *mdlnk = getModelDataLnk();
            //mdlnk->trans.setZ(md->trans.z());
            mdlnk->trans += delta;
            emit onChangedTranslationMouse(mdlnk->id, md->trans);
        }

        emit onChangedTranslationMouse(md->id, md->trans);
    }
    if (Qt::RightButton & event->buttons())
    {
       // _sphere.move(screenToNDC(event->pos()), QQuaternion());

        float viewspeed = .1f;
        float rotXAdd = (event->pos().y() - _capturedPos.y()) * viewspeed;
        float rotYAdd = (event->pos().x() - _capturedPos.x()) * viewspeed;
        //md->rot.setX( md->rot.x() + (event->pos().y() - _capturedPos.y()) * viewspeed); // pitch
        //md->rot.setY( md->rot.y() + (event->pos().x() - _capturedPos.x()) * viewspeed); // yaw;
        md->rot.setX( md->rot.x() + rotXAdd); // pitch
        md->rot.setY( md->rot.y() + rotYAdd); // yaw;
        if (linkOnAng())
        {
            ModelData *mdlnk = getModelDataLnk();
                   // LogTrace("%s main: %d, link: %d, rot before: %.2f, %.2f, %.2f, after: %.2f, %.2f, %.2f", func,
                   //          md->id, mdlnk->id,
                   //          mdlnk->rot.x(), mdlnk->rot.y(), mdlnk->rot.z(),
                   //          md->rot.x(), md->rot.y(), md->rot.z());
            // mdlnk->rot = md->rot;
            mdlnk->rot.setX( mdlnk->rot.x() + rotXAdd); // pitch
            mdlnk->rot.setY( mdlnk->rot.y() + rotYAdd); // yaw;
        }

        emit onChangedRotationMouse(md->id, md->rot);
    }
    _capturedPos = event->pos();
    updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::mouseReleaseEvent(QMouseEvent* event)
{
    if (_splitterSel)
    {
        _splitterSel = false;
        setCursor(Qt::CrossCursor);
    }

    //if (!_model) return;

    if (Qt::RightButton == event->button())
    {
        //_sphere.release(screenToNDC(event->pos()), QQuaternion());
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::wheelEvent(QWheelEvent * event)
{
    ModelData *md = getModelDataSel(0);
    if (!md || !md->model) return;

    QVector3D temp = QVector3D(0, 0, (float) event->delta());
    temp.setZ((temp.z()/(360))*_mouseMult.z()*md->bbZ);
    //_sceneTranslation.translate(temp);
    md->trans += temp;

    if (linkOnZoom())
    {
        ModelData *mdlnk = getModelDataLnk();
        //mdlnk->trans.setZ(md->trans.z());
        mdlnk->trans += temp;
        emit onChangedTranslationMouse(mdlnk->id, md->trans);
    }

    emit onChangedTranslationMouse(md->id, md->trans);

    updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked(event->pos().x(), event->pos().y());
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setMouseMultipliers(QVector3D newMult)
{
    _mouseMult = newMult;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::updateScene()
{
    /*
    QMatrix4x4 temp;
    temp.rotate(_sphere.rotation());
    if (!(qFuzzyCompare(temp, _sceneRotation)))
    {
        _sceneRotation = temp;
        updateGL();
    }
    */

    updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::drawWindowSelected(ModelData *md)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable( GL_LINE_SMOOTH );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // draw border line around the window

    // set up the color
    float colrPrev[4];
    float colr[4];
    QColor qc = GuiSettings::colorSelectWinL();
    if (md->loc == Right) qc = GuiSettings::colorSelectWinR();


    colr[0] = qc.redF();
    colr[1] = qc.greenF();
    colr[2] = qc.blueF();
    colr[3] = qc.alphaF();
    glGetFloatv(GL_CURRENT_COLOR, colrPrev);
    glColor4fv(colr);

    float lineWidthPrev = 1;
    glGetFloatv(GL_LINE_WIDTH, &lineWidthPrev);
    glLineWidth(8);

    // window coords should be -1 to 1
    float lofs = -.9999f, rofs = .9999f, t = .9999f, b = -.9999f, l=-1.0f, r=1.0f;
    float w = (r - l) * md->perWidth;

    if (md->loc == Left)
    {
        r = l + w;
        rofs = lofs + w;
    }
    else if (md->loc == Right)
    {
        l = r - w;
        lofs = rofs - w;
    }

    // draw box polyline
    glBegin(GL_LINES);

        // horizontal top
        glVertex2f(l, t);
        glVertex2f(r, t);

        // horizontal bottom
        glVertex2f(l, b);
        glVertex2f(r, b);

        // vertical left
        if (md->loc != Right)
        {
            glVertex2f(lofs, t);
            glVertex2f(lofs, b);
        }

        // vertical right
        if (md->loc != Left)
        {
            glVertex2f(rofs, t);
            glVertex2f(rofs, b);
        }

    glEnd();


    // restore
    glColor4fv(colrPrev);
    glLineWidth(lineWidthPrev);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND); // blend suppose to be enabled // TODO: get previous state and restore

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::drawSplitter()
{
    if (!_splitMode) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable( GL_LINE_SMOOTH );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // set up the color
    float colrPrev[4];
    float colr[4];

    if (_splitterSel)
    {
        colr[0] = 1;
        colr[1] = 1;
        colr[2] = 0;
    }
    else
    {
        colr[0] = 150.0f/255.0f;
        colr[1] = 150.0f/255.0f;
        colr[2] = 150.0f/255.0f;
    }
    colr[3] = 1;
    glGetFloatv(GL_CURRENT_COLOR, colrPrev);
    glColor4fv(colr);

    // set up line width
    float lineWidthPrev = 1;
    glGetFloatv(GL_LINE_WIDTH, &lineWidthPrev);
    glLineWidth(_splitterW);

    float sx = -1 + 2.0f * getModelData(0)->perWidth;

    // draw box polyline
    glBegin(GL_LINES);
        // draw splitter line
        glVertex2f(sx, 1);
        glVertex2f(sx, -1);
    glEnd();


    // restore
    glColor4fv(colrPrev);
    glLineWidth(lineWidthPrev);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND); // blend suppose to be enabled // TODO: get previous state and restore

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::keyPressEvent(QKeyEvent * event)
{
    QGLWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)
    {
        setWindowSelected(false);
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::runWindowSel(const QPoint &pt)
{
    const char *func = "GraphicsWidget2::runWindowSel() - ";
    int vpl[4], vpr[4];
    getWindows(vpl, vpr);

    //LogTrace("%s left box (%d, %d), right box (%d, %d), Mouse X: %d", func, leftx, leftx+leftw, rightx, rightx+rightw, pt.x());

    if (pt.x() >= vpl[0] && pt.x() <= (vpl[0] + vpl[2]))
    {
        if (getModelData(0)->selected == true) return;

        getModelData(0)->selected = true;
        getModelData(1)->selected = false;
        LogTrace("%s left selected", func);
        emit onWindowSelChange();
    }
    else if (pt.x() >= vpr[0] && pt.x() <= (vpr[0] + vpr[2]))
    {
        if (getModelData(1)->selected == true) return;

        getModelData(1)->selected = true;
        getModelData(0)->selected = false;
        LogTrace("%s right selected", func);
        emit onWindowSelChange();
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::moveSplitter(const QPoint &pt, bool emitSignal)
{
   if (!_splitMode) return;

   float perLeft = (float)pt.x() / (float)width();
   if (perLeft <= 0) perLeft = 1.0f / (float)width();
   if (perLeft >= 1) perLeft =  ((float)width() - 1.0f)/ (float)width();

   getModelData(0)->perWidth = perLeft;
   getModelData(1)->perWidth = 1.0f - perLeft;

   updateSplitStatSelection();

   if (emitSignal)
   {
       emit onSplitterMoved(pt.x(), pt.y());
   }
}

//=======================================================================
//=======================================================================
bool GraphicsWidget2::onSplitter(const QPoint &pt)
{
    if (!_splitMode) return false;

    int vpl[4], vpr[4], sx;
    getWindows(vpl, vpr, &sx);
    int halfSplitW = _splitterW / 2;
    if (halfSplitW <= 0) halfSplitW = 1;

    if (pt.x() >= sx-halfSplitW && pt.x() <= sx+halfSplitW) return true;
    return false;
}


//=======================================================================
//=======================================================================
void GraphicsWidget2::getWindows(int vpl[4], int vpr[4], int *splitterX)
{
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    int leftx = vp[0];
    int leftw = (int)((float)vp[2] * getModelData(0)->perWidth);
    int rightx = leftx + leftw + _splitterW;
    int rightw = (int)((float)vp[2] * getModelData(1)->perWidth);

    vpl[0] = leftx;
    vpl[1] = 0;
    vpl[2] = leftw;
    vpl[3] = vp[3];

    vpr[0] = rightx;
    vpr[1] = 0;
    vpr[2] = rightw;
    vpr[3] = vp[3];

    if (splitterX) *splitterX = vpl[0] + vpl[2];

}

//=======================================================================
//=======================================================================
void GraphicsWidget2::setSplitterX(int x)
{
    moveSplitter(QPoint(x,0), false);
}

//=======================================================================
//=======================================================================
int GraphicsWidget2::getSplitterX()
{
    int vpl[4], vpr[4], splitterX = 0;
    getWindows(vpl, vpr, &splitterX);
    return splitterX;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::updateSplitStatSelection()
{
    if (!getModelData(0)->model || !getModelData(1)->model)
    {
        return;
    }

    _runSplitStatsSelection = true;
}

//=======================================================================
//=======================================================================
void GraphicsWidget2::resizeEvent(QResizeEvent *event)
{
    QGLWidget::resizeEvent(event);

    if (!_splitMode) return;

    int x = getSplitterX();
    emit onSplitterMoved(x, 0);
}
