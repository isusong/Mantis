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
 *
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#include "VirtualTip.h"
#include "../QtBoxesDemo/QGLExtensionWrangler/glextensions.h"
#include <cfloat>
#include <limits>
#include <GL/glu.h>
#include <iostream>
#include "CleaningCode/CleanVirtualMark.h"
#include <QGLPixelBuffer>
#include <QGLWidget>
#include "logger.h"

#ifndef GL_DEPTH_COMPONENT32F
#define GL_DEPTH_COMPONENT32F 0x8CAC
#endif

using std::cout;
using std::endl;

#define NEARCLIP 0.1f
#define FARCLIP 100000.0f
#define EPS 1e-5
#define LEFT -2
#define RIGHT 2
#define CAMERAZ -5000.0
//Number of triangles to fill the buffer to about 30 MB, 
//assuming 4 bytes/GLfloat.  If a GLfloat is something larger,
//you probably have more GPU memory.
#define NTRIS 655360 
#define PBUFWIDTH 512
#define PBUFHEIGHT 512

///Global to this file.  Used to get OpenGL context
static QGLPixelBuffer* s_pbuffer = NULL;
///Global to this file.  Used to get OpenGL context
static QGLWidget* s_widget = NULL;

//=======================================================================
//=======================================================================
VirtualTip::VirtualTip(RangeImage *newTip, QGLContext* newContext, IProgress *prog, QObject* parent):
    QObject(parent),
    _progress(prog)
{
    _tip = newTip;
    _depth = _tip->getDepth(); //implicitly shared
    _mask = _tip->getMask(); //implicitly shared
	computeBoundingBox();

	//Now, we can get a context.
	if (newContext != NULL)
        _context = newContext;
	else
        _context = getOpenGLContext();

    // LogTrace("VirtualTip::VirtualTip() activating opengl context in thead: %d", QThread::currentThreadId());
    _context->makeCurrent();  //Activate the context for its init.
    if (!getGLExtensionFunctions().resolve(_context))
	{
        LogError("Missing support for any of the following:");
        LogError("- buffers (required for the 3D displays)");
        LogError("- framebuffers, renderbuffers (for virtual marking)");
        LogError("  or 3D texture images (for extension wrangler)");
        LogError("  The program may fail catastrophically!");
	}

	//Link the shader program.
    bool ret = true;
    _prog = new QGLShaderProgram(this);
    ret = _prog->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/mark.vert");
    ret = _prog->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/mark.frag");
    ret = _prog->link();

	//put camera a good way back
	//for orthographic projection.
    _camera.translate(0, 0, CAMERAZ);

	//init resolution with maximum pixel spacing.
    float pixX = _tip->getPixelSizeX();
    float pixY = _tip->getPixelSizeY();
	if (pixX > pixY)
        _resDefault = pixX;
	else
        _resDefault = pixY;
    _resolution = _resDefault;

	//Create the stream buffer.
    _sbuffer = new StreamBuffer(3*NTRIS);
    LogTrace("VirtualTip - stream buffer size: %.2f mb", StreamBuffer::toMB(_sbuffer->getCapacity()));

	//Init IDs.
	//Make framebuffer. The "renderbuffer" 
	//attached to this will
	//hold the depth data from OpenGL.
    glGenFramebuffersEXT(1, &_fboID);
	//Tell glDeleteTextures to ignore rboID the first time.
    _rboID = 0;
}

//=======================================================================
//=======================================================================
VirtualTip::~VirtualTip()
{
	//tip belongs to someone else
	//context belongs to someone else
	//prog belongs to QT.
    delete _sbuffer;
	//pbuffer and widget cannot be deleted here
	//they need to persist to other instances.
}

//=======================================================================
//=======================================================================
void VirtualTip::computeBoundingBox()
{
	//Cache some things.
    int width = _tip->getWidth();
    int height = _tip->getHeight();
    float pixelSizeX = _tip->getPixelSizeX();
    float pixelSizeY = _tip->getPixelSizeY();

	//Initialize.
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;

	for (int i = 0; i < width*height; ++i)
	{
        if (_mask.testBit(i))
		{
			float currentX = (i%width)*pixelSizeX;
			float currentY = (i/width)*pixelSizeY;
            float currentZ = _depth[i];
			if (currentX < minX)
				minX = currentX;
			if (currentX > maxX)
				maxX = currentX;
			if (currentY < minY)
				minY = currentY;
			if (currentY > maxY)
				maxY = currentY;
			if (currentZ < minZ)
				minZ = currentZ;
			if (currentZ > maxZ)
				maxZ = currentZ;
		}
	}
    _bbMin = QVector3D(minX, minY, minZ);
    _bbMax = QVector3D(maxX, maxY, maxZ);
}

//=======================================================================
//=======================================================================
void VirtualTip::computeProjection(const QMatrix4x4& transform, int* rboHeight,
	int* partitions, float* yMin, float* yDelta)
{
	//Determine orientation of tip.
	
	//Bounding box points indexed like this:
	//Looking the down the negative z, we
	//first see the plane
	//		  1   0 <-(bbMax)
	// 		  2   3
	//Then we see
	//		    5   4
	//(bbMin)-> 6   7
	QVector<QVector3D> boundingBox (8);
	/////////////////////////////////////////////Front plane:
    boundingBox[0] = QVector3D(_bbMax);
    boundingBox[1] = QVector3D(_bbMin.x(), _bbMax.y(), _bbMax.z());
    boundingBox[2] = QVector3D(_bbMin.x(), _bbMin.y(), _bbMax.z());
    boundingBox[3] = QVector3D(_bbMax.x(), _bbMin.y(), _bbMax.z());
	/////////////////////////////////////////////Back plane:
    boundingBox[4] = QVector3D(_bbMax.x(), _bbMax.y(), _bbMin.z());
    boundingBox[5] = QVector3D(_bbMin.x(), _bbMax.y(), _bbMin.z());
    boundingBox[6] = QVector3D(_bbMin);
    boundingBox[7] = QVector3D(_bbMax.x(), _bbMin.y(), _bbMin.z());

	//Manipulate the bounding box to find 
	//global x and y maxes and mins.
    QMatrix4x4 bbTransform = transform * _tip->getCoordinateSystemMatrix();
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	for (int i = 0; i < 8; ++i)
	{
		boundingBox[i] = bbTransform.map(
			boundingBox[i]);

		if (minY > boundingBox[i].y())
			minY = boundingBox[i].y();
		if (maxY < boundingBox[i].y())
			maxY = boundingBox[i].y();
	}
	*yMin = minY; //Store ymin.

	//Auto calculate number of partitions.
	bool openGLError = true;
	*partitions = 1;
	while (openGLError && (*partitions < 10))
	{
		//Compute rboHeight for the correct resolution.
		//We add one to round up to the nearest pixel.
        *rboHeight = (maxY - minY)/((*partitions)*_resolution) + 1;
		//Compute yDelta to yield the correct resolution.
        *yDelta = (*rboHeight)*_resolution;

		//Set up "renderbuffer"
		//I was using a renderbuffer before to hold the depth,
		//but certain OpenGL implementations (ATI, for example)
		//make the assumption that all framebuffers will have at
		//least one texture object attached to them.
		//Therefore, I needed to make this a texture for 
		//completeness.
		//I referred to: 
	  //developer.amd.com/media/gpu_assets/FramebufferObjects.pdf
        glDeleteTextures(1, &_rboID);
		//Make and bind a new renderbuffer for holding the depth.
		//Here's the gameplan: we make the renderbuffer 1 pixel
		//wide and rboHeight tall.  We then set glViewport to
		//make the window representation of the data
		//the same size.  In the rasterization step,
		//OpenGL will render the object
		//into the nearest pixels available.
        glGenTextures(1, &_rboID);
        glBindTexture(GL_TEXTURE_2D, _rboID);
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_DEPTH_COMPONENT32F, //Floating pt. format.
			1, *rboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);			  //Allocate it.
		// From Nik:  This checks for any openGL errors.
		GLenum errCode;
		if ((errCode = glGetError()) != GL_NO_ERROR) 
		{
			cout << "OpenGL Error: " << 
				gluErrorString(errCode) << endl;
			(*partitions)++; //Increase the number of partitions.
			//openGLError stays true.
		}
		else
		{
			//Break the loop.
			openGLError = false;
		}
	}

    //LogTrace("VirtualTip::ComputeProjection() - number of partitions: %d", *partitions);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
        GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D,
        _rboID, 0); //Attach to fbo.

	//Need to tell the driver that we're not using color, only
	//depth.  Otherwise, it complains that the fbo is incomplete.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

	//Is the fbo complete?
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        LogError("VirtualTip::mark() - framebuffer status error: 0x%X", status);
    }

	//Set viewport to appropriate size.
	glViewport(0, 0, 1, *rboHeight);
}

//=======================================================================
//=======================================================================
void VirtualTip::makeTriangles(float x0,float y0, float z0, float x1, float y1, float z1)
{
	//Make triangles like this:
	//0r  1r
	//*---*
	//|  /|
	//| / |
	//|/  |
	//*---*
	//0l  1l

    _sbuffer->addVertex(x0, y0, z0, LEFT); //0l
    _sbuffer->addVertex(x1, y1, z1, RIGHT); //1r
    _sbuffer->addVertex(x0, y0, z0, RIGHT); //0r

    _sbuffer->addVertex(x0, y0, z0, LEFT); //0l
    _sbuffer->addVertex(x1, y1, z1, LEFT); //1l
    _sbuffer->addVertex(x1, y1, z1, RIGHT); //1r
}

//=======================================================================
//=======================================================================
void VirtualTip::draw()
{
	//To ease the load on OpenGL, I'm using a StreamBuffer 
	//object to actually draw the triangles.

	//Need to use unique meshing strategy here due to
	//squish matrix.  The OpenGL polygon rasterizer won't
	//draw anything at 1 thick. See
	//http://www.opengl.org/archives/resources/faq/technical/rasterization.htm, item 14.120.
	//So we employ a "cat's cradle" strategy.
	//We mesh _between_ two instances of the data, a left and a right.
	//We set w in the point to tell the vertex shader which is which.
	//Then the vertex shader pulls the two instances apart.
	
	//cache some things.
    int width = _tip->getWidth();
    int height = _tip->getHeight();
    float pixSizeX = _tip->getPixelSizeX();
    float pixSizeY = _tip->getPixelSizeY();
    float* depthPtr = _depth.data(); //don't delete this.

    _sbuffer->bind();
	
	//Make triangles and draw them.
	for (int i = 0; i < height - 1; ++i) //Spaces between rows.
	{
		for (int j = 0; j < width-1; ++j) //Spaces between cols.
		{
            if (progCancel()) return;

			//Zoom in a point 0 at j, i, and its right
			//neighbor, neighbor below, and its 
			//neighbor below and to the right.
			//Like this:
			// 0  1
			// 2  3
			int idx0 = width*i + j;
			int idx1 = idx0 + 1;
			int idx2 = width + idx0;
			int idx3 = idx2 + 1;

			//Need x and y coordinates for each point.
			float x0 = j*pixSizeX;
			float y0 = i*pixSizeY;
			float x1 = x0 + pixSizeX;
			float y2 = y0 + pixSizeY;

            if (idx0 < 0 || idx0 >= _depth.size() ||
                idx1 < 0 || idx1 >= _depth.size() ||
                idx2 < 0 || idx2 >= _depth.size() ||
                idx3 < 0 || idx3 >= _depth.size() )

            {
                //int debug = 1;
            }

			//Can I make triangles between 0l/1l and 0r/1r?
            if (_mask.testBit(idx0) & _mask.testBit(idx1))
			{
				makeTriangles(x0, y0, depthPtr[idx0],
					x1, y0, depthPtr[idx1]);
			}
			//Can I make triangles between 1l/3l and 1r/3r?
            if ((_mask.testBit(idx1) & _mask.testBit(idx3))
				&& (j == width - 1)) //only draw at end.
			{
				makeTriangles(x1, y0, depthPtr[idx1],
					x1, y2, depthPtr[idx3]);
			}
			//Can I make triangles between 2l/3l and 2r/3r?
            if ((_mask.testBit(idx2) & _mask.testBit(idx3))
				&& (i == height - 1)) //only draw at end.
			{
				makeTriangles(x1, y2, depthPtr[idx3],
					x0, y2, depthPtr[idx2]);
			}
			//Can I make triangles between 0l/2l and 0r/2r?
            if (_mask.testBit(idx0) & _mask.testBit(idx2))
			{
				makeTriangles(x0, y2, depthPtr[idx2],
					x0, y0, depthPtr[idx0]);
			}
			//Can I make triangles between 1l/2l and 1r/2r?
            if (_mask.testBit(idx1) & _mask.testBit(idx2))
			{
				makeTriangles(x0, y2, depthPtr[idx2],
					x1, y0, depthPtr[idx1]);
			}
			//Can I make triangles between 0l/3l and 0r/3r?
            if (_mask.testBit(idx0) & _mask.testBit(idx3))
            {
				makeTriangles(x0, y0, depthPtr[idx0],
					x1, y2, depthPtr[idx3]);
			}
		}
	}

	//Draw the leftovers, if any.
    _sbuffer->flush();
}

//=======================================================================
//=======================================================================
QGLContext* VirtualTip::getOpenGLContext()
{
	//Create a context. This gets interesting....
	//const_cast isn't the best solution, but they
	//use it all the time inside QtOpenGL

	//If pbuffer or widget already exists, use that context.
    if (s_pbuffer != NULL)
	{
		//Return the pbuffer's context.
        s_pbuffer->makeCurrent();
		return const_cast<QGLContext*>(QGLContext::currentContext());
	}
    if (s_widget != NULL)
	{
		//Return the widget's context.
        return const_cast<QGLContext*>(s_widget->context());
	}

	//Otherwise, make a pbuffer or a widget.

	//Try pbuffers solution.
	if (QGLPixelBuffer::hasOpenGLPbuffers())
	{
        s_pbuffer = new QGLPixelBuffer(PBUFWIDTH, PBUFHEIGHT);
        if (s_pbuffer->isValid())
		{
            s_pbuffer->makeCurrent();
			return const_cast<QGLContext*>(QGLContext::currentContext());
		}
		else
		{
            delete s_pbuffer;
            s_pbuffer = NULL;
		}
	}

	//If we made it here, make and use a widget.
    s_widget = new QGLWidget();
    s_widget->isValid();
    s_widget->makeCurrent();
    s_widget->updateGL();
    s_widget->show();
    s_widget->setWindowTitle("Virtual Mark Window. DO NOT CLOSE");
    return const_cast<QGLContext*>(s_widget->context());
}

//=======================================================================
//=======================================================================
void VirtualTip::destroyOpenGLContext()
{
	//Note: delete silently ignores null pointers.
	//so we can safely do this.
    if (s_pbuffer)
    {
        delete s_pbuffer;
        s_pbuffer = NULL;
    }

    if (s_widget)
    {
        delete s_widget;
        s_widget = NULL;
    }
}

//=======================================================================
//=======================================================================
Profile* VirtualTip::mark(float xAxis, float yAxis, float zAxis)
{
    //Declare/initialize some things.
    int rboHeight; //Number of pixels in 1D "renderbuffer."
    int partitions; //Number of scene partitions.
    float yMin; //Bottom edge of the overall scene.
    float yDelta; //Length of a partition in um.
    //Create the transform matrix.
    QMatrix4x4 transform;
    //Transforms in PYR order.
    //This causes it to rotate around the fixed world
    //coordinate system.
    transform.rotate(zAxis, QVector3D(0, 0, 1)); //z-roll
    transform.rotate(yAxis, QVector3D(0, 1, 0)); //y-yaw
    transform.rotate(xAxis, QVector3D(1, 0, 0)); //x-pitch

    //Activate the context.
    //LogTrace("VirtualTip::mark() activating opengl context in thead: %d", QThread::currentThreadId());
    _context->makeCurrent();
    glEnable(GL_DEPTH_TEST); //so OpenGL will use a depth buffer.

    //Bind framebuffer for reading and writing.
    //Now OpenGL draws into this
    //instead of the screen.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fboID);

    //Create the rbo and determine the correct projection.
    computeProjection(transform, &rboHeight, &partitions, &yMin, &yDelta);

    if (!progStep()) // 1
    {
        //Clean up.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return NULL;
    }

    //Set up the modelview stack.
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(_camera.constData());
    QMatrix4x4 squish;
    squish(0, 0) = 0;
    glMultMatrixd(squish.constData()); //squish in x.
    glMultMatrixd(transform.constData());
    glMultMatrixd(_tip->getCoordinateSystemMatrix().constData());

    //Bind the correct shader program.
    bool res = _prog->bind();
    if (!res)
    {
        LogError("VirtualTip::mark - failed to bind shader!");
    }

    //Capture each mark partition and collect them.
    QVector<float> rawMarkData; //to store the mark.
    float bottomClip = yMin;
    float topClip = yMin + yDelta;
    for (int i = 0; i < partitions; ++i)
    {
        //Clear fbo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Compute new projection matrix.
        QMatrix4x4 projectionMatrix;
        projectionMatrix.ortho(-0.5, 0.5, bottomClip, topClip, NEARCLIP, FARCLIP);

        //Load perspective matrix into OpenGL
        glMatrixMode(GL_PROJECTION);
        //Overwrite the current projection matrix.
        glLoadMatrixd(projectionMatrix.constData());

        //Draw the tip.
        draw();
        if (progCancel())
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            return NULL;
        }

        // From Nik:  This checks for any openGL errors.
        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR)
        {
            cout << "OpenGL Error: " <<
                gluErrorString(errCode) << endl;
        }

        //Pull depth out of the framebuffer object.
        GLfloat* data = new GLfloat [rboHeight];
        glReadPixels(0, 0, 1, rboHeight, GL_DEPTH_COMPONENT, GL_FLOAT, data);
        //stash it.
        for (int j = 0; j < rboHeight; ++j)
        {
            rawMarkData.push_back(data[j]);
        }
        delete [] data;

        //Update the clip planes for the next run.
        bottomClip += yDelta;
        topClip += yDelta;
    }

    if (!progStep()) // 2
    {
        //Clean up.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return NULL;
    }

    //Remove the masked data from the ends.
    float maskValue;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &maskValue);
    QVector<float> markData = CleanVirtualMark::unmask(rawMarkData, maskValue);

    //Convert data to um.
    CleanVirtualMark::depthToUm(markData, NEARCLIP, FARCLIP, CAMERAZ);

    //Flip the mark data up/down.
    //This needs to happen because the mark is an impression.
    for (int i = 0; i < markData.size(); ++i)
    {
        markData[i] = -markData[i];
    }
    //Flip the mark left/right.
    QVector<float> markDataCopy(markData);
    int markDataSize = markData.size();
    for (int i = 0; i < markDataSize; ++i)
    {
        markData[i] = markDataCopy[markDataSize - i - 1];
    }

    if (!progStep()) // 3
    {
        //Clean up.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return NULL; // 3
    }

    //Find the mark edges for trimming.
    QPoint edges = CleanVirtualMark::findMarkEdges(markData);

    //Make Profile.
    QBitArray profileMask (markDataSize, true);
    for (int i = 0; i < edges.x(); ++i)
        profileMask.clearBit(i);
    for (int i = edges.y() + 1; i < markDataSize; ++i)
        profileMask.clearBit(i);
    Profile* ret = new Profile(_resolution, markData, profileMask);

    //Clean up.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    if (!progStep()) // 4
    {
        return NULL;
    }
    return ret;
}

/*
Profile* VirtualTip::mark(float xAxis, float yAxis, float zAxis)
{
	//Declare/initialize some things.
	int rboHeight; //Number of pixels in 1D "renderbuffer."
	int partitions; //Number of scene partitions.
	float yMin; //Bottom edge of the overall scene.
	float yDelta; //Length of a partition in um.
	//Create the transform matrix.
	QMatrix4x4 transform;
	//Transforms in PYR order.
	//This causes it to rotate around the fixed world 
	//coordinate system.
	transform.rotate(zAxis, QVector3D(0, 0, 1)); //z-roll
	transform.rotate(yAxis, QVector3D(0, 1, 0)); //y-yaw
	transform.rotate(xAxis, QVector3D(1, 0, 0)); //x-pitch

	//Activate the context.
    _context->makeCurrent();
	glEnable(GL_DEPTH_TEST); //so OpenGL will use a depth buffer.

	//Bind framebuffer for reading and writing. 
	//Now OpenGL draws into this
	//instead of the screen.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fboID);

	//Create the rbo and determine the correct projection.
	computeProjection(transform, &rboHeight, &partitions, &yMin, &yDelta);

	//Set up the modelview stack.
	glMatrixMode(GL_MODELVIEW); 
    glLoadMatrixd(_camera.constData());
	QMatrix4x4 squish;
	squish(0, 0) = 0;
	glMultMatrixd(squish.constData()); //squish in x.
	glMultMatrixd(transform.constData());
    glMultMatrixd(_tip->getCoordinateSystemMatrix().constData());

	//Bind the correct shader program.
    bool res = _prog->bind();

	//Capture each mark partition and collect them.
	QVector<float> rawMarkData; //to store the mark.
	float bottomClip = yMin;
	float topClip = yMin + yDelta;
	for (int i = 0; i < partitions; ++i)
	{
		//Clear fbo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Compute new projection matrix.
		QMatrix4x4 projectionMatrix;
        projectionMatrix.ortho(-0.5, 0.5, bottomClip, topClip, NEARCLIP, FARCLIP);

		//Load perspective matrix into OpenGL
		glMatrixMode(GL_PROJECTION);
		//Overwrite the current projection matrix.
		glLoadMatrixd(projectionMatrix.constData());

		//Draw the tip.
		draw();

		// From Nik:  This checks for any openGL errors.
		GLenum errCode;
		if ((errCode = glGetError()) != GL_NO_ERROR) 
		{
			cout << "OpenGL Error: " << 
				gluErrorString(errCode) << endl;
		}

		//Pull depth out of the framebuffer object.
		GLfloat* data = new GLfloat [rboHeight];
        glReadPixels(0, 0, 1, rboHeight, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		//stash it.
		for (int j = 0; j < rboHeight; ++j)
		{
			rawMarkData.push_back(data[j]);
		}
		delete [] data;

		//Update the clip planes for the next run.
		bottomClip += yDelta;
		topClip += yDelta;
	}

	//Remove the masked data from the ends.
	float maskValue;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &maskValue);
	QVector<float> markData = 
		CleanVirtualMark::unmask(rawMarkData, maskValue);
	
	//Convert data to um.
	CleanVirtualMark::depthToUm(markData, NEARCLIP,
		FARCLIP, CAMERAZ);

	//Flip the mark data up/down.
	//This needs to happen because the mark is an impression.
	for (int i = 0; i < markData.size(); ++i)
	{
		markData[i] = -markData[i];
	}
	//Flip the mark left/right.
	QVector<float> markDataCopy (markData);
	int markDataSize = markData.size();
	for (int i = 0; i < markDataSize; ++i)
	{
		markData[i] = markDataCopy[markDataSize - i - 1];
	}

	//Find the mark edges for trimming.
	QPoint edges = CleanVirtualMark::findMarkEdges(markData);

	//Make Profile.
	QBitArray profileMask (markDataSize, true);
	for (int i = 0; i < edges.x(); ++i)
		profileMask.clearBit(i);
	for (int i = edges.y() + 1; i < markDataSize; ++i)
		profileMask.clearBit(i);
    Profile* ret = new Profile(_resolution, markData, profileMask);

	//Clean up.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    return ret;
}
*/

//=======================================================================
//=======================================================================
QScriptValue VirtualTip::scriptableCreate(QScriptContext* scriptContext, QScriptEngine* engine)
{
	int argc = scriptContext->argumentCount();
	if (argc < 1)
	{
		qDebug() << "Incorrect number of arguments to create.";
		qDebug() << "Correct number of arguments is 1.";
		qDebug() << "Returning a null object.";
		return QScriptValue();
	}

    // TODO: FIGURE OUT HOW TO WORK WITH SMART POINTER IN VirtualTip

	RangeImage* argument0 = qscriptvalue_cast<RangeImage*>(
		scriptContext->argument(0) );
	return engine->newQObject(
		new VirtualTip(argument0), 
		QScriptEngine::AutoOwnership, 
		QScriptEngine::AutoCreateDynamicProperties);
}

//=======================================================================
//=======================================================================
bool VirtualTip::setResolution(float newRes)
{
    _resolution = newRes;
    if (newRes < _resDefault)
	{
		//Warn the user of possible interpolation.
		qDebug() << "Input resolution is smaller than the" <<
			"maximum tip data set resolution.\n" <<
			"The virtual mark may contain data obtained via" <<
			"interpolation.\n" <<
			"If this is undesireable, call" <<
			"setResolution(getDefaultResolution()) to use" <<
			"the maximum tip data set resolution.";
		return false;
	}
	else return true;
}

//=======================================================================
//=======================================================================
bool VirtualTip::progStep(const char *msg)
{
    if (!_progress) return true;

    _progress->progStep(1);
    if (msg != NULL)
    {
        _progress->progMsg(msg);
    }

    if (_progress->progCancel())
    {
        return false; // abort
    }

    return true;
}

//=======================================================================
//=======================================================================
bool VirtualTip::progCancel()
{
    if (!_progress) return false;
    return _progress->progCancel();
}

