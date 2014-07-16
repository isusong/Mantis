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

#ifndef __VIRTUALTIP_H__
#define __VIRTUALTIP_H__
#include <QObject>
#include "RangeImage.h"
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QVector3D>
#include "StreamBuffer.h"
#include <QGLContext>
#include "Profile.h"
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptEngine>

/**
 * Class for making a virtual mark with a RangeImage object.
 * The virtual mark is generated using the mark function and 
 * returned as Profile object.
 *
 * Note: The typical RangeImage may contain up to 10 million vertices.
 * A stream buffer object is used to send these to the GPU
 * memory in manageable chunks of 30 MB. If your system is weird
 * and has GLfloats larger than 4 bytes, 
 * this chunk will be larger.
 *
 * Note: Call destroyOpenGLContext() at application termination to ensure
 * proper final destruction of all Virtual Tip objects.
 *
 * @author Laura Ekstrand
 */

class VirtualTip: public QObject
{
	Q_OBJECT
	Q_PROPERTY(float resolution READ getResolution WRITE setResolution)
	Q_PROPERTY(float resDefault READ getDefaultResolution)

  protected:
	//Tip Variables.
	RangeImage* tip; ///< Pointer to tip object. Not owned by this class.
	QVector3D bbMin; ///< Tip bounding box min pt.
	QVector3D bbMax; ///< Tip bounding box max pt.
	///Cached depth from tip (implicitly shared).
	QVector<float> depth;
	///Cached mask from tip (implicitly shared).
	QBitArray mask;

	//Marking variables.
	QGLContext* context; ///< The OpenGL context. Not owned by this class.
	QGLShaderProgram* prog;///< Shader program
	QMatrix4x4 camera; ///< camera.
	///Fixed data resolution the virtual mark should have.
	float resolution;
	///Default value for resolution. Max pixel size of tip.
	float resDefault;
	///Stream buffer for sending lots of vertices to the GPU.
	StreamBuffer* sbuffer;
	GLuint fboID; ///< Framebuffer object id.
	///"Renderbuffer" object id. (Actually, it's now a texture.)
	GLuint rboID;

	//Protected member functions
	///Get a persisting OpenGL context for constructing your VirtualTip.
	/**
	 * Every tip constructed with this function will get the same pointer.
	 * This will internally create a pbuffer or a widget that owns the context.
	 *
	 * Note: Call destroyOpenGLContext() on application termination to ensure
	 * that the internal pbuffer or widget is destroyed properly.
	 */
	static QGLContext* getOpenGLContext();
	///Compute the bounding box.
	void computeBoundingBox();
	///Determine the correct projection matrix parameters.
	/**
	 * This also sets up the "renderbuffer," the texture that
	 * the depth gets rendered to.
	 *
	 * Needs the transformation matrix from mark and pointers for
	 * rboHeight, partitions, yMin, and yDelta.
	 */
	void computeProjection(const QMatrix4x4& transform, int* rboHeight,
		int* partitions, float* yMin, float* yDelta);
	///Helper function for making and drawing two triangles.
	void makeTriangles(float x0, 
		float y0, float z0, float x1, float y1, float z1);
	///Draws the tip.
	void draw();

  public:
	///Create a virtual tip from a RangeImage; won't delete the RangeImage*.
	/**
	 * newContext is a pointer to the OpenGL context.
	 * If you omit newContext, this class will generate an internal context
	 * using either a QGLPixelBuffer or a QGLWidget.  This internal context
	 * will be shared across all instances of this class by default.
	 *
	 * If you pass in a different newContext, this instance will use that 
	 * context instead.  This allows you to piggyback off of a GUI OpenGL
	 * widget if you would like. Of course, you own the passed-in newContext.
	 */
	VirtualTip(RangeImage* newTip, QGLContext* newContext = NULL, 
		QObject* parent = 0);
	virtual ~VirtualTip();

	///Call this at program termination to deallocate persisting context.
	static void destroyOpenGLContext();

  public slots:
	///Make the mark.
	/**
	 * xAxis = rotation about x axis (degrees)
	 * yAxis = rotation about y axis (degrees)
	 * zAxis = rotation about z axis (degrees)
	 * 
	 * These are applied in PYR order, meaning z first, then y, then x.
	 *
	 * This makes a Profile that you are responsible for deleting later.
	 */
	Profile* mark(float xAxis, float yAxis, float zAxis);

	///Wraps creation of Virtual Tip so you get an OpenGL context while scripting.
	static QScriptValue scriptableCreate(QScriptContext* scriptContext, 
		QScriptEngine* engine);

	///Set the mark resolution.
	/*
	 * The default resolution is max tip pixel size.
	 * If resolution is set lower than the default, it will 
	 * return false, otherwise true.
	 * Note: Either way, the resolution is set to newRes.
	 */
	bool setResolution(float newRes);
	///Return the default resolution of the data (max pix size).
	inline float getDefaultResolution() {return resDefault;}
	///Return actual resolution of the data.
	inline float getResolution() {return resolution;}
};

Q_DECLARE_METATYPE(VirtualTip*)

#endif //! defined __VIRTUALTIP_H__
