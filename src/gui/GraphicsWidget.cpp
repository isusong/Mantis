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
#include "GraphicsWidget.h"
#include "qmath.h"
#include "../QtBoxesDemo/QGLExtensionWrangler/glextensions.h"
#include "../core/logger.h"
#include "../core/utlqt.h"

#define FOVY 45.0f
#define NEARCLIP 0.1f
#define FARCLIP 100000.0f

//=======================================================================
//=======================================================================
GraphicsWidget::GraphicsWidget(WidgetLoc loc, QWidget* parent):
	QGLWidget(parent)
{
    _showWindowSelected = false;
    _wloc = loc;

	//Set timer to updateGL() every 20 msecs.
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
    _timer->start(20);

	//By default, only multiply mouse movements by 1.
    _mouseMult = QVector3D(1, 1, 1);
    _drawCS = true;

    _rot = QVector3D(0,0,0);
    _trans = QVector3D(0,0,0);
}

//=======================================================================
//=======================================================================
GraphicsWidget::~GraphicsWidget()
{

}

//=======================================================================
//=======================================================================
void GraphicsWidget::setWindowSelected(bool sel)
{
    if (_showWindowSelected == sel) return;

    _showWindowSelected = sel;
    updateGL();
}


//=======================================================================
//=======================================================================
double GraphicsWidget::getTransX()
{
    return _trans.x();
}

//=======================================================================
//=======================================================================
double GraphicsWidget::getTransY()
{
    return _trans.y();
}

//=======================================================================
//=======================================================================
double GraphicsWidget::getTransZ()
{
    return _trans.z();
}

//=======================================================================
// in degrees
//=======================================================================
double GraphicsWidget::getYaw()
{
    return _rot.y();
}

//=======================================================================
// in degrees
//=======================================================================
double GraphicsWidget::getPitch()
{
    return _rot.z();
}

//=======================================================================
//=======================================================================
double GraphicsWidget::getRoll()
{
    return _rot.z();
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setTrans(const QVector3D &trans)
{
    _trans = trans;
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setRot(const QVector3D &rot)
{
    _rot = rot;
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setTransX(double t)
{
    _trans.setX(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setTransY(double t)
{
    _trans.setY(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setTransZ(double t)
{
    _trans.setZ(t);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setYaw(double deg)
{
    _rot.setY(deg);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setPitch(double deg)
{
    _rot.setX(deg);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setRoll(double deg)
{
    _rot.setZ(deg);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	//Set clear color to black
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glLineStipple(1, 0x00FF);

	//Init extensions.
	bool success = 
		getGLExtensionFunctions().resolve(this->context());
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
void GraphicsWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (_model)
	{
        /*
		glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(_sceneTranslation.constData());
        glMultMatrixd(_sceneRotation.constData());
        */
        setModelView();

        _model->draw(this);

        //Draw world coordinate system.
        //The model is in charge of detaching its shader programs, if any.
        if (_drawCS)
		{
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
    }

    if (_showWindowSelected)
    {
        drawWindowSelected();
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setModelView()
{
    /*
      // original code
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(_sceneTranslation.constData());
    glMultMatrixd(_sceneRotation.constData());
    */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    if (!_model) return;

    // -_zRecommendedView is the normalized 0 z for the model.
    // its better to not include it in _trans so outside modifiers can link view togethers without
    // worrying about the difference in this value between models.

    // pan and zoom
    glTranslatef(_trans.x(), _trans.y(), _trans.z() - _zRecommendedView);

    // now rotate the object
    glRotatef(_rot.x(), 1.0f, 0.0f, 0.0f);
    glRotatef(_rot.y(), 0.0f, 1.0f, 0.0f);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::resizeGL(int width, int height)
{
	//Compute new perspective matrix.
	float aspectRatio = ((float) width)/((float) height);
	QMatrix4x4 perspectiveMatrix;
	perspectiveMatrix.perspective(FOVY, aspectRatio, 
		NEARCLIP, FARCLIP);

	//Load perspective matrix into OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(perspectiveMatrix.constData());

	glViewport(0, 0, width, height);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setDrawCS(bool status)
{
    _drawCS = status;
	updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setBackgroundColor(QColor color)
{
	glClearColor(color.redF(), color.greenF(),
		color.blueF(), 0.0f);
	updateGL();
}

//=======================================================================
//=======================================================================
QPointF GraphicsWidget::screenToNDC(const QPoint& point)
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
void GraphicsWidget::setModel(GenericModel* newModel)
{
	//QPointer now points to the same thing as newModel.
    _model = newModel;

	//Cache bounding box.
    QVector3D bbExtent = _model->getBbMax() - _model->getBbMin();
    _bbX = bbExtent.x();
    _bbY = bbExtent.y();
    _bbZ = bbExtent.z();

	//Set up camera.
    _rot = QVector3D(0,0,0);
    _trans = QVector3D(0,0,0);


    //_sceneRotation = QMatrix4x4();
    //_sceneTranslation = QMatrix4x4();
    _zRecommendedView = 1.5*_bbY/(2*qTan(FOVY/2));
    //_sceneTranslation.translate(0, 0, -_zRecommendedView);
    //_trans.setZ(-_zRecommendedView);

    emit onChangedTranslation(_trans);
    emit onChangedRotation(_rot);
}

//=======================================================================
//=======================================================================
void GraphicsWidget::mousePressEvent(QMouseEvent* event)
{
    if (!_model) return;
    _capturedPos = event->pos();
    //_sphere.push(screenToNDC(event->pos()), QQuaternion());

    if (event->button() == Qt::LeftButton)
    {
        emit onLButtonDown(event->pos().x(), event->pos().y());
    }
}

//=======================================================================
//=======================================================================
void GraphicsWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!_model) return;

	if (Qt::LeftButton & event->buttons())
	{
        QVector3D delta = QVector3D(_capturedPos - event->pos());
		delta.setX(-delta.x()); //Correct for window coordinates.
		//Convert to world coordinates -ish.
        delta *= (_mouseMult.y()*_bbY)/height();
		//Correct for zoom -ish.
        delta *= qAbs(_trans.z() - _zRecommendedView)/_zRecommendedView;
        //delta *= qAbs(_sceneTranslation(2, 3))/_zRecommendedView;
        //_sceneTranslation.translate(delta);

        _trans += delta;
        emit onChangedTranslation(_trans);
	}
	if (Qt::RightButton & event->buttons())
	{
       // _sphere.move(screenToNDC(event->pos()), QQuaternion());

        float viewspeed = .1f;
        _rot.setX( _rot.x() + (event->pos().y() - _capturedPos.y()) * viewspeed); // pitch
        _rot.setY( _rot.y() + (event->pos().x() - _capturedPos.x()) * viewspeed); // yaw;
        emit onChangedRotation(_rot);
	}
    _capturedPos = event->pos();
	updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (!_model) return;

	if (Qt::RightButton == event->button())
	{
        //_sphere.release(screenToNDC(event->pos()), QQuaternion());
	}
}

//=======================================================================
//=======================================================================
void GraphicsWidget::wheelEvent(QWheelEvent * event)
{
    if (!_model) return;

	QVector3D temp = QVector3D(0, 0, (float) event->delta());
    temp.setZ((temp.z()/(360))*_mouseMult.z()*_bbZ);
    //_sceneTranslation.translate(temp);
    _trans += temp;
    emit onChangedTranslation(_trans);

	updateGL();
}

//=======================================================================
//=======================================================================
void GraphicsWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit doubleClicked(event->pos().x(), event->pos().y());
}

//=======================================================================
//=======================================================================
void GraphicsWidget::setMouseMultipliers(QVector3D newMult)
{
    _mouseMult = newMult;
}

//=======================================================================
//=======================================================================
void GraphicsWidget::updateScene()
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
void GraphicsWidget::drawWindowSelected()
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
    colr[0] = 1;
    colr[1] = 150.0f/255.0f;
    colr[2] = 0;
    colr[3] = 1;
    glGetFloatv(GL_CURRENT_COLOR, colrPrev);
    glColor4fv(colr);

    float lineWidthPrev = 1;
    glGetFloatv(GL_LINE_WIDTH, &lineWidthPrev);
    glLineWidth(8);

    // window coords should be -1 to 1
    float l = -.9999f, r = .9999f, t = .9999f, b = -.9999f;

    // draw box polyline
    glBegin(GL_LINES);

        // horizontal top
        glVertex2f(-1, t);
        glVertex2f(1, t);

        // horizontal bottom
        glVertex2f(-1, b);
        glVertex2f(1, b);

        // vertical left
        if (_wloc != Right)
        {
            glVertex2f(l, t);
            glVertex2f(l, b);
        }

        // vertical right
        if (_wloc != Left)
        {
            glVertex2f(r, t);
            glVertex2f(r, b);
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
void GraphicsWidget::keyPressEvent(QKeyEvent * event)
{
    QGLWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)
    {
        setWindowSelected(false);
    }
}
