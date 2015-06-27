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

//=======================================================================
//=======================================================================
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

//=======================================================================
//=======================================================================
Selection::~Selection()
{

}

//=======================================================================
//=======================================================================
void Selection::drawSearchBox(float  left, float right, float top, float bottom, float z, float color[4], float linewidth)
{
    float widthprev = 1;
    glGetFloatv(GL_LINE_WIDTH, &widthprev);
    glLineWidth(linewidth);

    GLboolean depthon = GL_TRUE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthon);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColor4fv(color);

    glBegin(GL_LINE_LOOP);
        glVertex3f(left, bottom, z);
        glVertex3f(right, bottom, z);
        glVertex3f(right, top, z);
        glVertex3f(left, top, z);
    glEnd();

    // restore states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(depthon);
    glLineWidth(widthprev);
}

//=======================================================================
//=======================================================================
void Selection::drawColPlane(float x, float ymin, float ymax, float zmin, float zmax, float color[4], int zmul)
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
    //glColor4f(1.0f, 0.0f, 0.0f, 0.4f); //red
    glColor4fv(color);
    glBegin(GL_POLYGON);
        glVertex3f(x, ymin, zmul*zmin);
        glVertex3f(x, ymax, zmul*zmin);
        glVertex3f(x, ymax, zmul*zmax);
        glVertex3f(x, ymin, zmul*zmax);
    glEnd();
    glBlendFunc(GL_ONE, GL_ZERO); //make non-transparent.
    glDepthMask(GL_TRUE); //Re-enable writing into the depth buffer.

    //Make highlighting line around plane.
    //glColor3f(1.0f, 0.0f, 0.0f); //red.
    glColor3f(color[0], color[1], color[2]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLfloat currentLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
    glLineWidth(3.0f);
    glBegin(GL_POLYGON);
        glVertex3f(x, ymin, zmul*zmin);
        glVertex3f(x, ymax, zmul*zmin);
        glVertex3f(x, ymax, zmul*zmax);
        glVertex3f(x, ymin, zmul*zmax);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(currentLineWidth);
}

//=======================================================================
//=======================================================================
void Selection::drawRowPlane(float xmin, float xmax, float y, float zmin, float zmax, float color[4], int zmul)
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
    //glColor4f(1.0f, 0.0f, 0.0f, 0.4f); //red
    glColor4fv(color);
    glBegin(GL_POLYGON);
        glVertex3f(xmin, y, zmul*zmin);
        glVertex3f(xmax, y, zmul*zmin);
        glVertex3f(xmax, y, zmul*zmax);
        glVertex3f(xmin, y, zmul*zmax);
    glEnd();
    glBlendFunc(GL_ONE, GL_ZERO); //make non-transparent.
    glDepthMask(GL_TRUE); //Re-enable writing into the depth buffer.

    //Make highlighting line around plane.
    //glColor3f(1.0f, 0.0f, 0.0f); //red.
    glColor3f(color[0], color[1], color[2]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLfloat currentLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
    glLineWidth(3.0f);
    glBegin(GL_POLYGON);
        glVertex3f(xmin, y, zmul*zmin);
        glVertex3f(xmax, y, zmul*zmin);
        glVertex3f(xmax, y, zmul*zmax);
        glVertex3f(xmin, y, zmul*zmax);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(currentLineWidth);
}

//=======================================================================
//=======================================================================
void Selection::drawColPlane()
{
    float color[4] = {1,0,0,.4f};
    drawColPlane(basis.x(), bbMin.y(),  bbMax.y(), bbMin.z(), bbMax.z(), color, multiplier);
}

//=======================================================================
//=======================================================================
void Selection::drawRowPlane()
{
    float color[4] = {1,0,0,.4f};

    drawRowPlane(bbMin.x(), bbMax.x(), basis.y(), bbMin.z(), bbMax.z(), color, multiplier);
}

//=======================================================================
//=======================================================================
void Selection::draw()
{
	if (colPlane == mode)
		drawColPlane();
	else if (rowPlane == mode)
		drawRowPlane();
}

//=======================================================================
//=======================================================================
void Selection::updateSelectionBasis(float x, float y)
{
	basis.setX(x);
	basis.setY(y);
}

//=======================================================================
//=======================================================================
void Selection::redrawSelection(float x, float y)
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
