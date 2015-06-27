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
#include "../core/VirtualTip.h"
#include <QVector3D>
#include <QGLBuffer>
#include <QMatrix4x4>
#include <QGLShaderProgram>
#include "Selection.h"
#include "Mesh.h"
#include "../core/box.h"

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

public:
    struct LightInfo
    {
        ///Origin of light.
        QVector3D org;
        QVector3D amb;
        QVector3D dif;
        QVector3D spe;
        float shi;

        LightInfo()
        {
            org = QVector3D(-10, -5, 10);
            amb = QVector3D(1,1,1);
            dif = QVector3D(1,1,1);
            spe = QVector3D(1,1,1);
            shi = 5;
        }
    };

    struct SearchBox
    {
        int y;
        int height;
        int dataLen;
        bool draw;
        float color[4];

        SearchBox()
        {
            y = 0;
            height = 0;
            dataLen = 0;
            draw = true;

            // purple
            color[0] = .6078f;
            color[1] = 0;
            color[2] = 1;
            color[3] = .6f;
        }
    };

    enum DrawModes
    {
        SHADED = 0,
        WIREFRAME = 1,
        TEXTURED = 2,
        COLORMAPPED = 3,
        UNPROJECT = 4,
        TIP_COLORMAPPED = 5
    };

protected:
    struct TipData
    {
        PVirtualTip vtip;
        bool updatePolys;
        bool draw;
        PMesh mesh;
        GLfloat depthMin;
        GLfloat depthMax;
        GLfloat depthMean;
        GLfloat depthStdDev;
        PProfile profile;
        int shader;
        int shaderPrev;

        TipData()
        {
            updatePolys = false;
            draw = false;
            depthMin = 0;
            depthMax = 0;
            depthMean = 0;
            depthStdDev = 0;
            shader = TIP_COLORMAPPED;
            shaderPrev = TEXTURED;
        }
    };

public:
	///Create a renderer for newModel; doesn't take ownership of newModel.
    RangeImageRenderer(PRangeImage newModel, QWidget *parent = 0);
	virtual ~RangeImageRenderer();

    void setModel(PRangeImage newModel);
    PRangeImage getModel() { return _model; }

    bool setIsTip(bool isTip);
    bool getIsTip();
    bool setDrawMark(bool draw);
    bool getDrawMark();

    int getProfilePlateCol();



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
    virtual void draw(QGLWidget* scene);

    virtual void updateStatsSelection(QGLWidget* scene, int winx, int winy);

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
    inline const QMatrix4x4& getTransformMatrix() { return _transform; }
	///Set the transform matrix.
    inline void setTransformMatrix(const QMatrix4x4& newTrans) { _transform = newTrans; }
	
    ///Update the lighting origin.
    ///TODO: MAKE THREAD SAFE
    void setLightOrigin(const QVector3D& newOrigin);
    void setLightAmb(const QVector3D& v);
    void setLightDif(const QVector3D& v);
    void setLightSpe(const QVector3D& v);
    void setLightShine(float s);
    const LightInfo& getLightInfo() { return _lightInfo; }
    void setLightInfo(const LightInfo &info);

    ///Schedule an update to the internal selection object by "unprojection."
    virtual void scheduleSelectionUpdate(int x, int y);
    //Pass throughs for Selection object.
    virtual QPointF getBasis() { return _selection->getBasis(); }
    ///Use with spin box update.  Only schedules a selection redraw.
    virtual void redrawSelection(float x, float y) { _selection->redrawSelection(x, y); }
    virtual void setSelectionEnabled(bool status) { _selection->setEnabled(status); }
    virtual void setSelectionMode(Selection::drawModes mode) { _selection->setDrawMode(mode); }
    virtual void setSelectionMultiplier(int mult) { _selection->setMultiplier(mult); }

    Profile* getProfile();
    PProfile getProfilePlate();
    PProfile getProfileTip(float rotx, float roty, float rotz);
    void setProfileTip(PProfile profile);

    virtual void setSearchBox(int y, int height, int dataLen, bool draw=true);
    virtual void setSearchBoxColor(float r, float g, float b, float a);

    virtual void logInfo();

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



  //signals:
	//void statusMessage(QString msg); Laura: Won't work with GL!

protected:
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
    virtual void passDataToShaders(QGLWidget* scene, int shaderidx);
    ///Set up internal buffer objects.
    void initBuffers();
    void destroyBuffers();

    bool initMesh();
    bool initTipMesh();

    ///Helper function for computing normals for the nbo.
    ///WARNING: This modifies the vector reference "normals"!
    void computeTriangleNormal(std::vector<GLfloat> &normals, const std::vector<GLfloat> &xyz, int centerIdx,  int leftIdx, int rightIdx,
                               box3f *pbox, box3f *pboxtrans, const QMatrix4x4 *mat);

    ///Does the actual drawing for draw so draw can update selection.
    void internalRender(QGLWidget* scene);

    void drawSearchBox();
    void drawSearchBoxMark(Mesh *mesh);

    int getCurShaderId();
    QGLShaderProgram* getCurShader();
    QGLShaderProgram* getShader(int idx);

protected:
  //Range image data.
  ///Pointer to the range image file data.
  PRangeImage _model;

  TipData _tipData;
  /*
  VirtualTip *_vtip;
  bool _tipUpdatePolys;
  bool _tipDraw;
  PMesh _meshTip;
  GLfloat _tipDepthMin;
  GLfloat _tipDepthMax;
  GLfloat _tipDepthMean;
  GLfloat _tipDepthStdDev;
  PProfile _tipProfile;
  */


  //Cached from model.
  int _width; ///< width from model
  int _height; ///< height from model
  float _pixelSizeX; ///< pixelSizeX from model.
  float _pixelSizeY; ///< pixelSizeY from model.
  ///Depth from model (implicitly shared).
  QVector<float> _depth;
  ///Mask from model (implicitly shared).
  QBitArray _mask;
  ///Scaled QImage for texture shader.
  QImage _scaledTexture;

  //Drawing buffer variables.
  ///Matrix defining model transformations.
  QMatrix4x4 _transform;
  ///Downsample parameter. Default = 6.
  int _skip;

  /*
  ///Vertex buffer object.
  QGLBuffer _vbo;
  ///Index buffer object for storing the mesh.
  QGLBuffer _ibo;
  ///Number of triangles in model
  int _nTriangles;
  ///Normal buffer object.
  QGLBuffer _nbo;
  ///Texture coordinates buffer object.
  QGLBuffer _tbo;
    */

  Mesh _mesh;
  box3f _meshBox;
  box3f _meshBoxTrans;

  //Drawing mode variables.
  ///Index of the current shader program.
  int _currentShaderProgram;
  ///Shader program vector.
  /**
   * By default,
   * 0 is shaded program, 1 is wireframe program,
   * 2 is textured program, 3 is colorMappedProgram.
   */

  QVector<QGLShaderProgram*> _shaderPrograms;
  ///Companion vector describing shader polygon modes.
  QVector<GLenum> _polyModes;
  ///Should the coordinate system be turned on?
  bool _drawCS;

  ///Light Info
  LightInfo _lightInfo;

  ///Object for drawing double-click selection.
  Selection* _selection;
  ///Do we need to update the selection this draw loop?
  bool _needSelectionUpdate;
  ///Window x coordinate for currently selected point.
  int _winX;
  ///Window y coordinate for currently selected point.
  int _winY;

  SearchBox _searchBox;
};

#endif //!defined __RANGEIMAGERENDERER_H__
