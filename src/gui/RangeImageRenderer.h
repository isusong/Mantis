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

#ifndef __RANGEIMAGERENDERER_H__
#define __RANGEIMAGERENDERER_H__
#include "GenericModel.h"
#include <QPointer>
#include "../core/RangeImage.h"
#include <QVector3D>
#include <QGLBuffer>
#include <QMatrix4x4>
#include <QGLShaderProgram>
#include "Selection.h"

/**
 * A class for displaying the contents of a RangeImageobject
 * and its coordinate system.
 * Provides an interface for
 * translation and rotation of the model.
 * Also provides an interface for different drawing modes
 * (shaded, wireframe, textured, colorMapped).
 *
 * @author Laura Ekstrand
 */

class RangeImageRenderer: public GenericModel
{
	Q_OBJECT 

  protected:
	//Member variables.
	//Range image data.
	///Pointer to the range image file data.
	QPointer<RangeImage> model;
	//Cached from model.
	int width; ///< width from model
	int height; ///< height from model
	float pixelSizeX; ///< pixelSizeX from model.
	float pixelSizeY; ///< pixelSizeY from model.
	///Depth from model (implicitly shared).
	QVector<float> depth;
	///Mask from model (implicitly shared).
	QBitArray mask; 
	///Scaled QImage for texture shader.
	QImage scaledTexture;

	//Drawing buffer variables.
	///Matrix defining model transformations.
	QMatrix4x4 transform;
	///Downsample parameter. Default = 6.
	int skip;
	///Vertex buffer object.
	QGLBuffer vbo;
	///Index buffer object for storing the mesh.
	QGLBuffer ibo;
	///Number of triangles in model
	int nTriangles;
	///Normal buffer object.
	QGLBuffer nbo;
	///Texture coordinates buffer object.
	QGLBuffer tbo;

	//Drawing mode variables.
	///Index of the current shader program.
	int currentShaderProgram;
	///Shader program vector.
	/**
	 * By default,
	 * 0 is shaded program, 1 is wireframe program,
	 * 2 is textured program, 3 is colorMappedProgram.
	 */
	QVector<QGLShaderProgram*> shaderPrograms;
	///Companion vector describing shader polygon modes.
	QVector<GLenum> polyModes;
	///Should the coordinate system be turned on?
	bool drawCS;
	///Origin of light.
	QVector3D lightOrigin;
	///Object for drawing double-click selection.
	Selection* selection;
	///Do we need to update the selection this draw loop?
	bool needSelectionUpdate;
	///Window x coordinate for currently selected point.
	int winX;
	///Window y coordinate for currently selected point.
	int winY;

	//Protected member functions.
	///Compute bounding box.
	virtual void computeBoundingBox();
	///Set up shaders.
	/**
	 * If you overload in this a subclass to add any new shaders,
	 * call RangeImageRenderer::initShaders() first!
	 * By default,
	 * 0 is shaded program, 1 is wireframe program,
	 * 2 is textured program, 3 is colorMappedProgram.
	 * 4 is a program that paints each vertex with a unique
	 * color for unprojection (this is more or less private
	 * to this class).
	 * Also, in addition to adding to the shaderPrograms 
	 * vector, be sure to add to the polyModes vector with the
	 * polygon mode you want.  (Unless you just want GL_FILL.)
	 */
	virtual void initShaders();
	///Pass any needed data into shaders.
	virtual void passDataToShaders(GraphicsWidget* scene);
	///Set up internal buffer objects.
	void initBuffers();
	///Helper function for computing normals for the nbo.
	///WARNING: This modifies the vector reference "normals"!
	void computeTriangleNormal(QVector<GLfloat>& normals,
		const float* xyz, int centerIdx, int leftIdx, 
		int rightIdx);
	///Does the actual drawing for draw so draw can update selection.
	void internalRender(GraphicsWidget* scene);

  public:
	///Create a renderer for newModel; doesn't take ownership of newModel.
	RangeImageRenderer(RangeImage* newModel, 
		QWidget *parent = 0);
	virtual ~RangeImageRenderer();

	///Draw the model. Overloading draw() from GenericModel.
	/**
	 * This initializes the shaders with initShaders() 
	 * and buffers with initBuffers() on the first
	 * draw.
	 * 
	 * This also binds the appropriate shader, calls
	 * passDataToShaders(), and then draws the geometry.
     * Finally, it releases the appropriate shader,
     * and if the coordinate system is turned on, it
	 * then draws it.
	 */
	virtual void draw(GraphicsWidget* scene);

	//Transform operations
	///Translate the tip.
	virtual void translate(const QVector3D& vector);
	///Rotate the tip.
	virtual void rotate(float angle, const QVector3D& axis);
	///Multiply transform matrix on the left by other.
	void applyLeftTransform(const QMatrix4x4& other);
	///Multiply transform matrix on the right by other.
	void applyRightTransform(const QMatrix4x4& other);

	//Getters and setters.
	///Set the downsampling rate.
	/**
	 * This is ineffective after the first call of this->draw().
	 * So if you want something besides 6, set it on creation
	 * of this object.
	 */
	void setSkip(int newSkip);
	///Get the transform matrix.
	inline const QMatrix4x4& getTransformMatrix()
		{return transform;}
	///Set the transform matrix.
	inline void setTransformMatrix(const QMatrix4x4& newTrans)
		{transform = newTrans;}
	
  public slots:
	///Select the current shader.
	/**
	 * If idx is out of range, this function does nothing.
	 * If polyModes is shorter than shaderPrograms, it 
	 * will be extended with values of GL_FILL to fit the
	 * requested idx.
	 */
	void setDrawMode(int idx);
	///Set whether or not the coordinate system is drawn.
	void setDrawCS(bool enabled);
	///Update the lighting origin.
	inline void setLightOrigin(const QVector3D& newOrigin)
		{lightOrigin = newOrigin;}
	///Schedule an update to the internal selection object by "unprojection."
	void scheduleSelectionUpdate(int x, int y);
	//Pass throughs for Selection object.
	inline QPointF& getBasis() {return selection->getBasis();}
	///Use with spin box update.  Only schedules a selection redraw.
	inline void redrawSelection(float x, float y)
		{selection->redrawSelection(x, y);}
	inline void setSelectionEnabled(bool status)
		{selection->setEnabled(status);}
	inline void setSelectionMode(Selection::drawModes mode)
		{selection->setDrawMode(mode);}
	inline void setSelectionMultiplier(int mult)
		{selection->setMultiplier(mult);}

  //signals:
	//void statusMessage(QString msg); Laura: Won't work with GL!
};

#endif //!defined __RANGEIMAGERENDERER_H__
