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

#define FOVY 45.0f
#define NEARCLIP 0.1f
#define FARCLIP 100000.0f

GraphicsWidget::GraphicsWidget(QWidget* parent):
	QGLWidget(parent)
{
	//Set timer to updateGL() every 20 msecs.
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateScene()));
	timer->start(20);

	//By default, only multiply mouse movements by 1.
	mouseMult = QVector3D(1, 1, 1);
	drawCS = true;
}

GraphicsWidget::~GraphicsWidget()
{

}

void
GraphicsWidget::initializeGL()
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

void
GraphicsWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (model) 
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(sceneTranslation.constData());
		glMultMatrixd(sceneRotation.constData());

		model->draw(this);

        //Draw world coordinate system.
        //The model is in charge of detaching its shader programs, if any.
		if (drawCS)
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
}

void
GraphicsWidget::resizeGL(int width, int height)
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

void
GraphicsWidget::setDrawCS(bool status)
{
	drawCS = status;
	updateGL();
}

void
GraphicsWidget::setBackgroundColor(QColor color)
{
	glClearColor(color.redF(), color.greenF(),
		color.blueF(), 0.0f);
	updateGL();
}

QPointF
GraphicsWidget::screenToNDC(const QPoint& point)
{
	QPointF ret;
	ret.setX(((2/(float) width())*
		((float) point.x())) - 1);
	ret.setY(((2/(float) height())*
		((float) point.y())) - 1);
	ret.setY(-ret.y()); //Correct for window coordinates.
	return ret;
}

void
GraphicsWidget::setModel(GenericModel* newModel)
{
	//QPointer now points to the same thing as newModel.
	model = newModel;

	//Cache bounding box.
	QVector3D bbExtent = model->getBbMax() - model->getBbMin();
	bbX = bbExtent.x();
	bbY = bbExtent.y();
	bbZ = bbExtent.z();

	//Set up camera.
	sceneRotation = QMatrix4x4();
	sceneTranslation = QMatrix4x4();
	zRecommendedView = 1.5*bbY/(2*qTan(FOVY/2));
	sceneTranslation.translate(0, 0, -zRecommendedView);
}

void
GraphicsWidget::mousePressEvent(QMouseEvent* event)
{
	if (!model) return;
	capturedPos = event->pos();
	sphere.push(screenToNDC(event->pos()), quat);
}

void
GraphicsWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!model) return;
	if (Qt::LeftButton & event->buttons())
	{
		QVector3D delta = QVector3D(capturedPos - event->pos());
		delta.setX(-delta.x()); //Correct for window coordinates.
		//Convert to world coordinates -ish.
		delta *= (mouseMult.y()*bbY)/height();
		//Correct for zoom -ish.
		delta *= qAbs(sceneTranslation(2, 3))/zRecommendedView;
		sceneTranslation.translate(delta);
	}
	if (Qt::RightButton & event->buttons())
	{
		sphere.move(screenToNDC(event->pos()), quat);
	}
	capturedPos = event->pos();
	updateGL();
}

void
GraphicsWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (!model) return;

	if (Qt::RightButton == event->button())
	{
		sphere.release(screenToNDC(event->pos()), quat);
	}
}

void
GraphicsWidget::wheelEvent(QWheelEvent * event)
{
	if (!model) return;
	QVector3D temp = QVector3D(0, 0, (float) event->delta());
	temp.setZ((temp.z()/(360))*mouseMult.z()*bbZ);
	sceneTranslation.translate(temp);
	updateGL();
}

void
GraphicsWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit doubleClicked(event->pos().x(), event->pos().y());
}

void
GraphicsWidget::setMouseMultipliers(QVector3D newMult)
{
	mouseMult = newMult;
}

void
GraphicsWidget::updateScene()
{
	QMatrix4x4 temp;
	temp.rotate(sphere.rotation());
	if (!(qFuzzyCompare(temp, sceneRotation)))
	{
		sceneRotation = temp;
		updateGL();
	}
}
