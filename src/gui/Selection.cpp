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
#include "Selection.h"
#include <QGLContext>
#include <GL/glu.h>
#include <cmath>

#define EPS 1

Selection::Selection(const QVector3D& bb0, const QVector3D& bb1,
	QObject* parent):
	QObject(parent)
{
	//Initialize
	bbMin = bb0;
	bbMax = bb1;
	enabled = true;
	mode = colPlane;
	multiplier = 1;
}

Selection::~Selection()
{

}

void
Selection::drawColPlane()
{
	//Make transparent plane.
	//Disable writing into the depth buffer.
	//This is a trick that prevents "stitching" in the
	//highlight line. Since the depth of the plane
	//is not written to the depth buffer (only used to
	//draw the plane correctly), it can't interfere
	//with the depth values for depth testing of the
	//highlight line.
	glDepthMask(GL_FALSE); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 0.0f, 0.0f, 0.4f); //red
	glBegin(GL_POLYGON);
		glVertex3f(basis.x(), bbMin.y(), 
			multiplier*bbMin.z());
		glVertex3f(basis.x(), bbMax.y(), 
			multiplier*bbMin.z());
		glVertex3f(basis.x(), bbMax.y(), 
			multiplier*bbMax.z());
		glVertex3f(basis.x(), bbMin.y(), 
			multiplier*bbMax.z());
	glEnd();
	glBlendFunc(GL_ONE, GL_ZERO); //make non-transparent.
	glDepthMask(GL_TRUE); //Re-enable writing into the depth buffer.

	//Make highlighting line around plane.
	glColor3f(1.0f, 0.0f, 0.0f); //red.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GLfloat currentLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
	glLineWidth(3.0f);
	glBegin(GL_POLYGON);
		glVertex3f(basis.x(), bbMin.y(), 
			multiplier*bbMin.z());
		glVertex3f(basis.x(), bbMax.y(), 
			multiplier*bbMin.z());
		glVertex3f(basis.x(), bbMax.y(), 
			multiplier*bbMax.z());
		glVertex3f(basis.x(), bbMin.y(), 
			multiplier*bbMax.z());
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(currentLineWidth);
}

void
Selection::drawRowPlane()
{
	//Make transparent plane.
	//Disable writing into the depth buffer.
	//This is a trick that prevents "stitching" in the
	//highlight line. Since the depth of the plane
	//is not written to the depth buffer (only used to
	//draw the plane correctly), it can't interfere
	//with the depth values for depth testing of the
	//highlight line.
	glDepthMask(GL_FALSE); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 0.0f, 0.0f, 0.4f); //red
	glBegin(GL_POLYGON);
		glVertex3f(bbMin.x(), basis.y(), 
			multiplier*bbMin.z());
		glVertex3f(bbMax.x(), basis.y(), 
			multiplier*bbMin.z());
		glVertex3f(bbMax.x(), basis.y(), 
			multiplier*bbMax.z());
		glVertex3f(bbMin.x(), basis.y(), 
			multiplier*bbMax.z());
	glEnd();
	glBlendFunc(GL_ONE, GL_ZERO); //make non-transparent.
	glDepthMask(GL_TRUE); //Re-enable writing into the depth buffer.

	//Make highlighting line around plane.
	glColor3f(1.0f, 0.0f, 0.0f); //red.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GLfloat currentLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
	glLineWidth(3.0f);
	glBegin(GL_POLYGON);
		glVertex3f(bbMin.x(), basis.y(), 
			multiplier*bbMin.z());
		glVertex3f(bbMax.x(), basis.y(), 
			multiplier*bbMin.z());
		glVertex3f(bbMax.x(), basis.y(), 
			multiplier*bbMax.z());
		glVertex3f(bbMin.x(), basis.y(), 
			multiplier*bbMax.z());
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(currentLineWidth);
}

void
Selection::draw()
{
	if (colPlane == mode)
		drawColPlane();
	else if (rowPlane == mode)
		drawRowPlane();
}

void
Selection::updateSelectionBasis(float x, float y)
{
	basis.setX(x);
	basis.setY(y);
}

void
Selection::redrawSelection(float x, float y)
{
	float basisX = basis.x();
	float basisY = basis.x();
	float xdiff = fabs(basisX - x);
	float ydiff = fabs(basisY - y);

	//This is verbosely coded to make it clear to read.
	if ((colPlane == mode) && (xdiff > EPS))
	{
		updateSelectionBasis(x, y);
		emit updateGL();
	}
	else if ((rowPlane == mode) && (ydiff > EPS))
	{
		updateSelectionBasis(x, y);
		emit updateGL();
	}
}
