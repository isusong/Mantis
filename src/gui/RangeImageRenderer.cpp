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
#include "RangeImageRenderer.h"
#include "GraphicsWidget.h"
#include <cfloat>
#include "../core/logger.h"

//=======================================================================
//=======================================================================
RangeImageRenderer::RangeImageRenderer(PRangeImage newModel,
	QWidget *parent):
    GenericModel(parent),
    _selection (NULL)
{
	//Default downsample.
    _skip = 6;

    //Set the model.
    _width = 0;
    _height = 0;
    _pixelSizeX = 0;
    _pixelSizeY = 0;
    //_depth
    setModel(newModel);

	//Default drawing mode.
    _currentShaderProgram = 0;
    _drawCS = true;

    //_lightOrigin = QVector3D(-1000, -500, 1000);
    _lightInfo.org = QVector3D(-10, -5, 10);
    _lightInfo.amb = QVector3D(1,1,1);
    _lightInfo.dif = QVector3D(1,1,1);
    _lightInfo.spe = QVector3D(1,1,1);
    _lightInfo.shi = 5;


    _selection = new Selection(_bbMin, _bbMax, this);
    _selection->setEnabled(false);
    connect(_selection, SIGNAL(updateGL()), this, SIGNAL(updateGL()));
    _needSelectionUpdate = false;
    _winX = 0;
    _winY = 0;
}

//=======================================================================
//=======================================================================
RangeImageRenderer::~RangeImageRenderer()
{
	//model belongs to someone else.
    delete _selection;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setModel(PRangeImage newModel)
{
    destroyBuffers();

    _model = newModel;
    if (!_model) return;

    _width = _model->getWidth();
    _height = _model->getHeight();
    _pixelSizeX = _model->getPixelSizeX();
    _pixelSizeY = _model->getPixelSizeY();
    _depth = _model->getDepth();
    _mask = _model->getMask();
    computeBoundingBox();

    //Downsample the texture.
    QImage cachedTexture = _model->getTexture();
    int scaleFactor = _skip/3;
    if (1 > scaleFactor) scaleFactor = 1;
    int scaledHeight = cachedTexture.height()/scaleFactor;
    _scaledTexture = cachedTexture.scaledToHeight(scaledHeight);

    if (_selection)
    {
         _selection->setBbMin(_bbMin);
         _selection->setBbMax(_bbMax);
    }

    //_scaledTexture.save("D:/dev/mantis/mantis/data/test.png");
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::computeBoundingBox()
{
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;

    for (int i = 0; i < _width*_height; ++i)
	{
        if (_mask.testBit(i))
		{
            float currentX = (i%_width)*_pixelSizeX;
            float currentY = (i/_width)*_pixelSizeY;
            float currentZ = _depth[i];
			if (currentX < minX)
			{
				minX = currentX;
			}
			if (currentX > maxX)
			{
				maxX = currentX;
			}
			if (currentY < minY)
			{
				minY = currentY;
			}
			if (currentY > maxY)
			{
				maxY = currentY;
			}
			if (currentZ < minZ)
			{
				minZ = currentZ;
			}
			if (currentZ > maxZ)
			{
				maxZ = currentZ;
			}
		}
	}

    _bbMin = QVector3D(minX, minY, minZ);
    _bbMax = QVector3D(maxX, maxY, maxZ);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::initShaders()
{
    if (_shaderPrograms.size() > 0) return; // already initialized

	//Shader programs are parented to this object.
	//They will get deleted automatically by QT.

	//Link the shader programs.
	//shaded program
	QGLShaderProgram* shadedProgram = new QGLShaderProgram(this);
	shadedProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/farAwayLight.vert");
	shadedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
	shadedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/hologram.frag");
    shadedProgram->link();
    _shaderPrograms.push_back(shadedProgram);
    _polyModes.push_back(GL_FILL);
	//wireframe program
	QGLShaderProgram* wireframeProgram = 
		new QGLShaderProgram(this);
	wireframeProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/farAwayLight.vert");
	wireframeProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
	wireframeProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/hello.frag");
	wireframeProgram->link();
    _shaderPrograms.push_back(wireframeProgram);
    _polyModes.push_back(GL_LINE);

	//Textured program
	QGLShaderProgram* texturedProgram = 
		new QGLShaderProgram(this);
	texturedProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/texturing.vert");
	texturedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
	texturedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/texturing.frag");
	texturedProgram->link();
    _shaderPrograms.push_back(texturedProgram);
    _polyModes.push_back(GL_FILL);

	//colorMapped program
	QGLShaderProgram* colorMappedProgram = 
		new QGLShaderProgram(this);
	colorMappedProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/colorMap.vert");
	colorMappedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
	colorMappedProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/colorMap.frag");
	colorMappedProgram->link();
    _shaderPrograms.push_back(colorMappedProgram);
    _polyModes.push_back(GL_FILL);
	//unproject program
	QGLShaderProgram* unprojectProgram = new QGLShaderProgram(this);
	unprojectProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/colorlevels.vert");
	unprojectProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/colorlevels.frag");
    unprojectProgram->link();
    _shaderPrograms.push_back(unprojectProgram);
    _polyModes.push_back(GL_FILL);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::passDataToShaders(GraphicsWidget* scene)
{
    if (!_model) return;

    int curProg = getCurShaderId();
    QGLShaderProgram* shdr = getCurShader();
    if (!shdr) return;

    if (4 != curProg)
	{
        shdr->setUniformValue("lightOrigin",_lightInfo.org);
        shdr->setUniformValue("lightAmb", _lightInfo.amb);
        shdr->setUniformValue("lightDif", _lightInfo.dif);
        shdr->setUniformValue("lightSpe", _lightInfo.spe);
        shdr->setUniformValue("lightShi", _lightInfo.shi);
	}
    if (3 == curProg)
	{
        shdr->setUniformValue("minDepth", (GLfloat) _bbMin.z());
        shdr->setUniformValue("maxDepth", (GLfloat) _bbMax.z());
	}
    if (2 == curProg)
	{
        scene->bindTexture(_scaledTexture,
			GL_TEXTURE_2D, GL_RGBA,
			QGLContext::LinearFilteringBindOption);
        shdr->setUniformValue("modelTexture", (GLuint) 0);
	}
    if (4 == curProg)
	{
        shdr->setUniformValue("width", _width);
        shdr->setUniformValue("height", _height);
        shdr->setUniformValue("skip", _skip);
	}
} 

//=======================================================================
//=======================================================================
void RangeImageRenderer::initBuffers()
{
	//Downsampled width & height.
    int dWidth = _width/_skip;
    int dHeight = _height/_skip;

	//Populate vertices & texture coordinates.
	GLfloat* xyz = 
		new GLfloat[3*dWidth*dHeight];
	GLfloat* texCoords = new GLfloat[2*dWidth*dHeight];
	for(int i = 0; i < dHeight; ++i)
	{
		for(int j = 0; j < dWidth; ++j)
		{
			int idx = dWidth*i + j;
            xyz[3*idx + 0] = (GLfloat) j*_skip*_pixelSizeX;
            xyz[3*idx + 1] = (GLfloat) i*_skip*_pixelSizeY;
			xyz[3*idx + 2] = (GLfloat) 
                _depth[_width*i*_skip + j*_skip];
			texCoords[2*idx] = (1/((float) dWidth)) * (j + 0.5f);
			texCoords[2*idx + 1] = (1/((float) dHeight)) 
				* (i + 0.5f);
		}
	}
	//Init VBO
    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QGLBuffer::StaticDraw);
    _vbo.allocate(xyz, 3*dWidth*dHeight*sizeof(GLfloat));
	//Init TBO
    _tbo.create();
    _tbo.bind();
    _tbo.setUsagePattern(QGLBuffer::StaticDraw);
    _tbo.allocate(texCoords, sizeof(GLfloat)*2*dWidth*dHeight);
	delete [] texCoords;

	//Do meshing (populate indices and normals).
	QVector<GLuint> indices;
	QVector<GLfloat> normals (3*dWidth*dHeight, 0);
	//Generate mesh
	//Go through the spaces between the rows.
	for (int i = 0; i < dHeight - 1; ++i)
	{
		//Go through the spaces between the columns.
		for (int j = 0; j < dWidth - 1; ++j)
		{
			//Zoom in on a point 0 at i*skip, j*skip and its
			//downsampled neighbor on the right, point 1, its
			//downsampled neighbor below, point 2, and its
			//downsampled neighbor below and to the right, point 3.
			//Like this:
			// 0  1
			// 2  3
			//Indices into the mask.
            int index0 = _width*i*_skip + j*_skip;
            int index1 = index0 + _skip;
            int index2 = index0 + _width*_skip;
            int index3 = index2 + _skip;
			//Indices into the vertices.
			int vertID0 = dWidth*i + j;
			int vertID1 = vertID0 + 1;
			int vertID2 = vertID0 + dWidth;
			int vertID3 = vertID2 + 1;

			//Can I connect point 1 to point 2?
            if (_mask.testBit(index1) & _mask.testBit(index2))
			{
				// I use the default / method to divide the 
				//triangles.
                if(_mask.testBit(index0)) //Can I connect 0 to 1 and 2?
				{
					//If so, I can make a triangle out of them.
					indices.push_back(vertID0);
					indices.push_back(vertID1);
					indices.push_back(vertID2);
					computeTriangleNormal(normals, xyz, vertID2, 
						vertID0, vertID1);
				}
                if(_mask.testBit(index3)) //Can I connect 3 to 1 and 2?
				{
					indices.push_back(vertID2);
					indices.push_back(vertID3);
					indices.push_back(vertID1);
					computeTriangleNormal(normals, xyz, vertID2, 
						vertID1, vertID3);
				}
			}
            else if (_mask.testBit(index0) & _mask.testBit(index3)) //Can I connect 0 and 3?
			{
				//I use the \ method the divide the triangles.
                if(_mask.testBit(index1)) //Can I connect 1 to 0 and 3?
				{
					indices.push_back(vertID1);
					indices.push_back(vertID3);
					indices.push_back(vertID0);
					computeTriangleNormal(normals, xyz, vertID3, 
						vertID0, vertID1);
				}
                if(_mask.testBit(index2)) //Can I connect 2 to 0 and 3?
				{
					indices.push_back(vertID2);
					indices.push_back(vertID3);
					indices.push_back(vertID0);
					computeTriangleNormal(normals, xyz, vertID3, 
						vertID2, vertID0);
				}
			}
		}
	}

	//Allocate the ibo.
    _ibo = QGLBuffer(QGLBuffer::IndexBuffer);
    _ibo.create();
    _ibo.bind();
    _ibo.setUsagePattern(QGLBuffer::StaticDraw);
    _ibo.allocate(indices.data(), sizeof(GLuint)*indices.size());
    _nTriangles = indices.size()/3;

	//Allocate the nbo.
    _nbo.create();
    _nbo.bind();
    _nbo.setUsagePattern(QGLBuffer::StaticDraw);
    _nbo.allocate(normals.data(), sizeof(GLfloat)*normals.size());

	delete [] xyz;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::destroyBuffers()
{
    _vbo.destroy();
    _ibo.destroy();
    _nbo.destroy();
    _tbo.destroy();

    _nTriangles = 0;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::computeTriangleNormal(QVector<GLfloat>&  normals, const float* xyz, int centerIdx,  int leftIdx, int rightIdx)
{
	//WARNING: This modifies the normals QVector.
	//Doing this to avoid having to store the normals on
	//both CPU and GPU.
	QVector3D centerPt (xyz[3*centerIdx],
		xyz[3*centerIdx + 1], xyz[3*centerIdx + 2]);
	QVector3D leftPt (xyz[3*leftIdx],
		xyz[3*leftIdx + 1], xyz[3*leftIdx + 2]);
	QVector3D rightPt (xyz[3*rightIdx],
		xyz[3*rightIdx + 1], xyz[3*rightIdx + 2]);
	QVector3D normalVec = QVector3D::normal(
		rightPt - centerPt, leftPt - centerPt);
	
	
	//Add normalVec for the triangle to the normals
	//for each participating vertex in the triangle.
	//This will generate the "average" normal vector
	//of all adjacent faces to each vertex.
	//It will have the proper direction but need to be normalized.
	normals[3*centerIdx] += normalVec.x();
	normals[3*centerIdx + 1] += normalVec.y();
	normals[3*centerIdx + 2] += normalVec.z();
	normals[3*leftIdx] += normalVec.x();
	normals[3*leftIdx + 1] += normalVec.y();
	normals[3*leftIdx + 2] += normalVec.z();
	normals[3*rightIdx] += normalVec.x();
	normals[3*rightIdx + 1] += normalVec.y();
	normals[3*rightIdx + 2] += normalVec.z();
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::internalRender(GraphicsWidget* scene)
{
    const char *func = "RangeImageRenderer::internalRenderer() - ";

    if (!_model) return;

	//Init buffers & shaders if this is first draw
	//Need context to do this.
    if (!_vbo.isCreated())
	{
		qDebug() << "Drawing for the first time....";
        initShaders();
		//Turn on the vertex array capability.
		glEnableClientState(GL_VERTEX_ARRAY);
		//Turn on the normal array capability.
		glEnableClientState(GL_NORMAL_ARRAY);
		//Turn on the texture coordinate array capability.
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		initBuffers();	
		glEnable(GL_BLEND); //for the selection plane.
	}

    QGLShaderProgram *shdr = getCurShader();

	//Bind current shader.
    if (!shdr)
	{
		//In case you've done something like pull all
        //the programs out of _shaderPrograms:
		qDebug() << "You have no shader programs!" <<
			"Drawing with 1.0 pipeline.";
		glColor3f(0, 0, 1.0f); //blue pen
		glPolygonMode(GL_FRONT_AND_BACK,
			GL_FILL);
	}
	else 
	{
        if (!shdr->bind())
        {
            LogError("%s Failed to bind shader", func);

        }
		//Provide shaders with any needed data/settings.
		passDataToShaders(scene);
        //If _currentShaderProgram is valid, then polyMode
		//will have an entry for it (see setDrawMode).
        glPolygonMode(GL_FRONT_AND_BACK, _polyModes[getCurShaderId()]);
	}

	//Set up matrix stack.
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

    // TODO: RESTORE
    glMultMatrixd(_transform.constData());
    glMultMatrixd(_model->getCoordinateSystemMatrix().constData());


	//Draw model data.
    if (!_vbo.bind())
    {
        LogError("%s Failed to bind vbo", func);
    }

	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glVertexPointer(3, GL_FLOAT, 0, NULL);

    if (!_nbo.bind())
    {
        LogError("%s Failed to bind nbo", func);
    }
	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glNormalPointer(GL_FLOAT, 0, NULL);

    if (!_tbo.bind())
    {
        LogError("%s Failed to bind tbo", func);
    }
	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    if (!_ibo.bind())
    {
        LogError("%s Failed to bind ibo", func);
    }
	//Draw the buffers in the order given in ibo.
	//NULL says "use the currently bound buffer" as the indices.
    glDrawElements(GL_TRIANGLES, _nTriangles*3, GL_UNSIGNED_INT, NULL);

    //Unbind current shader program.
    if (shdr) shdr->release();

	//Draw selection, if enabled.
    if (_selection->isEnabled()) _selection->draw();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    //Draw coordinate system.
    if (_drawCS)
    {
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
        glMultMatrixd(_transform.constData());
		float lineLength = 4e3;
		glLineWidth(2.0f);
		glBegin(GL_LINES);
			glColor3f(1.0f, 0, 0); //red pen
			glVertex3f(0, 0, 0);
			glVertex3f(lineLength, 0, 0); //x
			glColor3f(0, 1.0f, 0); //green pen
			glVertex3f(0, 0, 0);
			glVertex3f(0, lineLength, 0); //y
			glColor3f(0, 0, 1.0f); //blue pen
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, lineLength);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::draw(GraphicsWidget* scene)
{
    if (!_model) return;

    if (_needSelectionUpdate)
	{
		//Save state.
        int oldProgram = getCurShaderId();
        bool oldDrawCS = _drawCS;
        bool oldSelectionEnabled = _selection->isEnabled();

		//Draw for identifying window coordinates.
        _currentShaderProgram = 4;
        _drawCS = false;
        _selection->setEnabled(false);
		internalRender(scene);

		//Read back pixel color.
		unsigned char color[3];
		//Must flip y - Qt and OpenGL are different.
        glReadPixels(_winX,  scene->height() - _winY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

		//Determine i and j values from pixel color.
		int id = (65536*color[0]) + (256*color[1]) + color[2];
        int i = id/_width;
        int j = id%_width;

		//Update selection.
        _selection->updateSelectionBasis(_pixelSizeX*j, _pixelSizeY*i);

		//Restore state.
        _currentShaderProgram = oldProgram;
        _drawCS = oldDrawCS;
        _selection->setEnabled(oldSelectionEnabled);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//We have updated the selection.
        _needSelectionUpdate = false;
	}
	internalRender(scene);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::translate(const QVector3D& vector)
{
	QMatrix4x4 temp;
	temp.translate(vector);
	applyLeftTransform(temp);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::rotate(float angle, const QVector3D& axis)
{
	QMatrix4x4 temp;
	temp.rotate(angle, axis);
	applyRightTransform(temp);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::applyLeftTransform(const QMatrix4x4& other)
{
    _transform = other * _transform;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::applyRightTransform(const QMatrix4x4& other)
{
    _transform = _transform * other;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setSkip(int newSkip)
{
    _skip = newSkip;

    if (!_model) return;

    if (1 != _skip)
	{
        QImage cachedTexture = _model->getTexture();
        int scaleFactor = _skip/3;
		if (1 > scaleFactor) scaleFactor = 1;
		int scaledHeight = cachedTexture.height()/scaleFactor;
        _scaledTexture = cachedTexture.scaledToHeight(scaledHeight);
	}
	else
        _scaledTexture = _model->getTexture();
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setDrawMode(int idx)
{
    if (_currentShaderProgram == idx) return;

    // if shaderPrograms is not filled yet, we still need to store the idx for later
    _currentShaderProgram = idx;

    //If idx is a valid index into _shaderPrograms, use it.
    if ((0 <= idx) && (idx < _shaderPrograms.size()))
    {
		//If not enough polyModes:
        if (idx >= _polyModes.size())
		{
            int diff = idx - _polyModes.size() + 1;
			for (int i = 0; i < diff; ++i)
			{
                _polyModes.push_back(GL_FILL);
			}
		}


       emit updateGL();
	}
}

//=======================================================================
//=======================================================================
int RangeImageRenderer::getCurShaderId()
{
    if ((0 <= _currentShaderProgram) && (_currentShaderProgram < _shaderPrograms.size()))
    {
        return _currentShaderProgram;
    }

    return 0;
}

//=======================================================================
//=======================================================================
QGLShaderProgram* RangeImageRenderer::getCurShader()
{
    if (_shaderPrograms.size() <= 0) return NULL;

    int idx = getCurShaderId();
    return _shaderPrograms[idx];
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setDrawCS(bool enable)
{
    _drawCS = enable;
	emit updateGL();
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightOrigin(const QVector3D& newOrigin)
{
    _lightInfo.org = newOrigin;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightAmb(const QVector3D& v)
{
    _lightInfo.amb = v;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightDif(const QVector3D& v)
{
    _lightInfo.dif = v;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightSpe(const QVector3D& v)
{
    _lightInfo.spe = v;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightShine(float s)
{
    _lightInfo.shi = s;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setLightInfo(const LightInfo &info)
{
    _lightInfo = info;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::scheduleSelectionUpdate(int x, int y)
{
    if (_selection->isEnabled())
	{
        _needSelectionUpdate = true;
        _winX = x;
        _winY = y;
		emit updateGL();
	}
}
