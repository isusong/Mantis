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
#include "../core/UtlQt3d.h"

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
bool RangeImageRenderer::setIsTip(bool isTip)
{
    if (_tipData.vtip)
    {
        _tipData.vtip.reset();
    }

    if (!isTip) return true;

    if (_model.isNull()) return false;

    _tipData.vtip.reset(new VirtualTip(_model.data(), NULL, NULL, this));
    return true;
}

//=======================================================================
//=======================================================================
bool RangeImageRenderer::getIsTip()
{
    if (_tipData.vtip) return true;

    return false;
}

//=======================================================================
//=======================================================================
int RangeImageRenderer::getImgType()
{
    if (_model.isNull()) return RangeImage::ImgType_Unk;

    return _model->getImgType();
}

//=======================================================================
//=======================================================================
bool RangeImageRenderer::setDrawMark(bool draw)
{
    if (!_tipData.vtip) return false;
    _tipData.draw = draw;
    return true;
}

//=======================================================================
//=======================================================================
bool RangeImageRenderer::getDrawMark()
{
    if (!_tipData.vtip) return false;
    return _tipData.draw;
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
    shadedProgram->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/farAwayLight.vert");
    shadedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
    shadedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/hologram.frag");
    shadedProgram->link();
    _shaderPrograms.push_back(shadedProgram);
    _polyModes.push_back(GL_FILL);

	//wireframe program
    QGLShaderProgram* wireframeProgram = new QGLShaderProgram(this);
    wireframeProgram->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/farAwayLight.vert");
    wireframeProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
    wireframeProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/hello.frag");
	wireframeProgram->link();
    _shaderPrograms.push_back(wireframeProgram);
    _polyModes.push_back(GL_LINE);

	//Textured program
    QGLShaderProgram* texturedProgram = new QGLShaderProgram(this);
    texturedProgram->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/texturing.vert");
    texturedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
    texturedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/texturing.frag");
	texturedProgram->link();
    _shaderPrograms.push_back(texturedProgram);
    _polyModes.push_back(GL_FILL);

	//colorMapped program
    QGLShaderProgram* colorMappedProgram = new QGLShaderProgram(this);
    colorMappedProgram->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/colorMap.vert");
    colorMappedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
    colorMappedProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/colorMap.frag");
	colorMappedProgram->link();
    _shaderPrograms.push_back(colorMappedProgram);
    _polyModes.push_back(GL_FILL);

	//unproject program
	QGLShaderProgram* unprojectProgram = new QGLShaderProgram(this);
    unprojectProgram->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/colorlevels.vert");
    unprojectProgram->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/colorlevels.frag");
    unprojectProgram->link();
    _shaderPrograms.push_back(unprojectProgram);
    _polyModes.push_back(GL_FILL);

    //tip colorMapped program
    QGLShaderProgram* tipProgColor =  new QGLShaderProgram(this);
    tipProgColor->addShaderFromSourceFile(QGLShader::Vertex, ":/glsl/glsl/colorMapTip.vert");
    tipProgColor->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/lighting.frag");
    tipProgColor->addShaderFromSourceFile(QGLShader::Fragment, ":/glsl/glsl/colorMapTip.frag");
    tipProgColor->link();
    _shaderPrograms.push_back(tipProgColor);
    _polyModes.push_back(GL_FILL);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::passDataToShaders(QGLWidget* scene, int shaderid)
{
    if (!_model) return;

    //int curProg = getCurShaderId();
    //QGLShaderProgram* shdr = getCurShader();
    QGLShaderProgram* shdr = getShader(shaderid);
    if (!shdr) return;

    if (UNPROJECT != shaderid)
	{
        shdr->setUniformValue("lightOrigin",_lightInfo.org);
        shdr->setUniformValue("lightAmb", _lightInfo.amb);
        shdr->setUniformValue("lightDif", _lightInfo.dif);
        shdr->setUniformValue("lightSpe", _lightInfo.spe);
        shdr->setUniformValue("lightShi", _lightInfo.shi);
	}

    if (TIP_COLORMAPPED == shaderid)
    {
        shdr->setUniformValue("minDepth", (GLfloat) _tipData.depthMin);
        shdr->setUniformValue("maxDepth", (GLfloat) _tipData.depthMax);
    }
    else if (COLORMAPPED == shaderid)
    {
        shdr->setUniformValue("minDepth", (GLfloat) _bbMin.z());
        shdr->setUniformValue("maxDepth", (GLfloat) _bbMax.z());
	}
    else if (TEXTURED == shaderid)
	{
        scene->bindTexture(_scaledTexture, GL_TEXTURE_2D, GL_RGBA, QGLContext::LinearFilteringBindOption);
        shdr->setUniformValue("modelTexture", (GLuint) 0);
	}
    else if (UNPROJECT == shaderid)
	{
        shdr->setUniformValue("width", _width);
        shdr->setUniformValue("height", _height);
        shdr->setUniformValue("skip", _skip);
	}
} 

//=======================================================================
//=======================================================================
bool RangeImageRenderer::initMesh()
{
    const char *func = "RangeImageRenderer::initBuffers() -";

   const QMatrix4x4 &coordSysMat = _model->getCoordinateSystemMatrix();
    _meshBox.reset();
    _meshBoxTrans.reset();

    //Downsampled width & height.
    int dWidth = _width/_skip;
    int dHeight = _height/_skip;

    //Populate vertices & texture coordinates.
    std::vector<GLfloat> vbo(3*dWidth*dHeight);
    std::vector<GLfloat> tbo(2*dWidth*dHeight);
    for(int i = 0; i < dHeight; ++i)
    {
        for(int j = 0; j < dWidth; ++j)
        {
            int idx = dWidth*i + j;
            vbo[3*idx + 0] = (GLfloat) j*_skip*_pixelSizeX;
            vbo[3*idx + 1] = (GLfloat) i*_skip*_pixelSizeY;
            vbo[3*idx + 2] = (GLfloat) _depth[_width*i*_skip + j*_skip];
            tbo[2*idx] = (1/((float) dWidth)) * (j + 0.5f);
            tbo[2*idx + 1] = (1/((float) dHeight)) * (i + 0.5f);
        }
    }

    //Do meshing (populate indices and normals).
    std::vector<GLuint> ibo;
    std::vector<GLfloat> nbo(3*dWidth*dHeight, 0);
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

            if (vertID0 >= (int)vbo.size() || vertID1 >= (int)vbo.size() || vertID2 >= (int)vbo.size() || vertID3 >= (int)vbo.size())
            {
                LogError("%s - invalid index into the vbo", func);
            }

            //Can I connect point 1 to point 2?
            if (_mask.testBit(index1) & _mask.testBit(index2))
            {
                // I use the default / method to divide the
                //triangles.
                if(_mask.testBit(index0)) //Can I connect 0 to 1 and 2?
                {
                    //If so, I can make a triangle out of them.
                    ibo.push_back(vertID0);
                    ibo.push_back(vertID1);
                    ibo.push_back(vertID2);
                    computeTriangleNormal(nbo, vbo, vertID2, vertID0, vertID1, &_meshBox, &_meshBoxTrans, &coordSysMat);
                }
                if(_mask.testBit(index3)) //Can I connect 3 to 1 and 2?
                {
                    ibo.push_back(vertID2);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID1);
                    computeTriangleNormal(nbo, vbo, vertID2, vertID1, vertID3, &_meshBox, &_meshBoxTrans, &coordSysMat);
                }
            }
            else if (_mask.testBit(index0) & _mask.testBit(index3)) //Can I connect 0 and 3?
            {
                //I use the \ method the divide the triangles.
                if(_mask.testBit(index1)) //Can I connect 1 to 0 and 3?
                {
                    ibo.push_back(vertID1);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID0);
                    computeTriangleNormal(nbo, vbo, vertID3, vertID0, vertID1, &_meshBox, &_meshBoxTrans, &coordSysMat);
                }
                if(_mask.testBit(index2)) //Can I connect 2 to 0 and 3?
                {
                    ibo.push_back(vertID2);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID0);
                    computeTriangleNormal(nbo, vbo, vertID3, vertID2, vertID0, &_meshBox, &_meshBoxTrans, &coordSysMat);
                }
            }
        }
    }

    return _mesh.createTrisNT(ibo, vbo, nbo, tbo);
}

/*
//=======================================================================
// this is not working
//=======================================================================
bool RangeImageRenderer::initMesh()
{
    const char *func = "RangeImageRenderer::initMesh() -";

    //Downsampled width & height.
    int dWidth = _width/_skip;
    int dHeight = _height/_skip;

    //Populate vertices & texture coordinates.
    std::vector<GLfloat> vbo(3*dWidth*dHeight);
    std::vector<GLfloat> tbo(2*dWidth*dHeight);
    int v = 0;
    int t = 0;
    for(int i = 0; i < dHeight; i++)
    {
        for(int j = 0; j < dWidth; j++)
        {
            int idd = _width*i*_skip + j*_skip;
            if (idd >= _depth.size())
            {
                LogError("%s bad depth lookup: %d, vector size: %d", func, idd, _depth.size());
            }


            //int idx = dWidth*i + j;
            //vbo[3*idx + 0] = (GLfloat) j*_skip*_pixelSizeX;
            //vbo[3*idx + 1] = (GLfloat) i*_skip*_pixelSizeY;
            //vbo[3*idx + 2] = (GLfloat) _depth[_width*i*_skip + j*_skip];
            //tbo[2*idx] = (1/((float) dWidth)) * (j + 0.5f);
            //tbo[2*idx + 1] = (1/((float) dHeight)) * (i + 0.5f);



            //vbo[v] = (GLfloat) j*_skip*_pixelSizeX; v++;
            //vbo[v] = (GLfloat) i*_skip*_pixelSizeY; v++;
            //vbo[v] = (GLfloat) _depth[_width*i*_skip + j*_skip]; v++;
            //tbo[t] = (1/((float) dWidth)) * (j + 0.5f); t++;
            //tbo[t] = (1/((float) dHeight)) * (i + 0.5f); t++;


            float x = (GLfloat) j*_skip*_pixelSizeX;
            float y = (GLfloat) i*_skip*_pixelSizeY;
            float z = (GLfloat) _depth[_width*i*_skip + j*_skip];
            if (x > 9999999 || y > 9999999 || z > 9999999)
            {
                //LogError("%s bad vert data when constructing vbo", func);
            }

            vbo[v] = x; v++;
            vbo[v] = y; v++;
            vbo[v] = x; v++;
            tbo[t] = (1/((float) dWidth)) * (j + 0.5f); t++;
            tbo[t] = (1/((float) dHeight)) * (i + 0.5f); t++;

        }
    }


    //Init VBO
    //_vbo.create();
    //_vbo.bind();
    //_vbo.setUsagePattern(QGLBuffer::StaticDraw);
    //_vbo.allocate(xyz, 3*dWidth*dHeight*sizeof(GLfloat));
    //Init TBO
    //_tbo.create();
    //_tbo.bind();
    //_tbo.setUsagePattern(QGLBuffer::StaticDraw);
    //_tbo.allocate(texCoords, sizeof(GLfloat)*2*dWidth*dHeight);
    //delete [] texCoords;


    //Do meshing (populate indices and normals).
    std::vector<GLuint> ibo;
    std::vector<GLfloat> nbo(3*dWidth*dHeight, 0);
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

            if (vertID0 >= vbo.size() || vertID1 >= vbo.size() || vertID2 >= vbo.size() || vertID3 >= vbo.size())
            {
                LogError("%s - invalid index into the vbo", func);
            }

            //Can I connect point 1 to point 2?
            if (_mask.testBit(index1) & _mask.testBit(index2))
            {
                // I use the default / method to divide the
                //triangles.
                if(_mask.testBit(index0)) //Can I connect 0 to 1 and 2?
                {
                    //If so, I can make a triangle out of them.
                    ibo.push_back(vertID0);
                    ibo.push_back(vertID1);
                    ibo.push_back(vertID2);
                    computeTriangleNormal(nbo, vbo, vertID2, vertID0, vertID1);
                }
                if(_mask.testBit(index3)) //Can I connect 3 to 1 and 2?
                {
                    ibo.push_back(vertID2);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID1);
                    computeTriangleNormal(nbo, vbo, vertID2, vertID1, vertID3);
                }
            }
            else if (_mask.testBit(index0) & _mask.testBit(index3)) //Can I connect 0 and 3?
            {
                //I use the \ method the divide the triangles.
                if(_mask.testBit(index1)) //Can I connect 1 to 0 and 3?
                {
                    ibo.push_back(vertID1);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID0);
                    computeTriangleNormal(nbo, vbo, vertID3, vertID0, vertID1);
                }
                if(_mask.testBit(index2)) //Can I connect 2 to 0 and 3?
                {
                    ibo.push_back(vertID2);
                    ibo.push_back(vertID3);
                    ibo.push_back(vertID0);
                    computeTriangleNormal(nbo, vbo, vertID3, vertID2, vertID0);
                }
            }
        }
    }

    _meshBox.reset();

    int i = 0;
    while (i < ibo.size())
    {
        int idx = ibo[i]; i++;
        idx *= 3;


        if (idx + 2 >= vbo.size()|| idx < 0)
        {
            LogError("%s ibo index out of bounds", func);
            continue; // bad vert
        }

        float x = vbo[idx];
        float y = vbo[idx+1];
        float z = vbo[idx+2];

        if (x > 99999999 || y > 99999999 || z > 99999999)
        {
            //LogError("%s bad vert data: %.2f, %.2f, %.2f", func, x, y, z);
            continue; // bad data ?
        }

        _meshBox.updateBox(x, y, z);
    }


    //Allocate the ibo.
    //_ibo = QGLBuffer(QGLBuffer::IndexBuffer);
    //_ibo.create();
    //_ibo.bind();
    //_ibo.setUsagePattern(QGLBuffer::StaticDraw);
    //_ibo.allocate(indices.data(), sizeof(GLuint)*indices.size());
    //_nTriangles = indices.size()/3;

    //Allocate the nbo.
    //_nbo.create();
    //_nbo.bind();
    //_nbo.setUsagePattern(QGLBuffer::StaticDraw);
    //_nbo.allocate(normals.data(), sizeof(GLfloat)*normals.size());

    //delete [] xyz;


    return _mesh.createTrisNT(ibo, vbo, nbo, tbo);
}
*/

//=======================================================================
//=======================================================================
bool RangeImageRenderer::initTipMesh()
{
    const char *func = "RangeImageRenderer::initTipMesh() -";
    if (!_tipData.vtip)
    {
        LogError("%s failed to init: no tip object.", func);
        return false;
    }

    if (!_tipData.profile)
    {
        LogError("%s failed to init: no tip profile.", func);
        return false;
    }

    int depthCount = _tipData.profile->getDepth().size();
    if (depthCount < 4)
    {
        LogError("%s failed to init: tip profile depth buffer too small: %d", func, depthCount);
        return false;
    }

    // skip min 2% and max 2%
    int trim = int((float)depthCount * .02f);
    int depthCountTrim = depthCount - 2*trim;
    if (depthCountTrim < 4)
    {
        LogError("%s failed to init: tip profile trimmed depth buffer too small: %d", func, depthCountTrim);
        return false;
    }

    box3f box;
    std::vector<GLfloat> vbo, nbo, cbo;
    std::vector<GLuint> ibo;

    // need to figure out how to make the mesh box the same size as the visible model,
    // it is always much taller

    float boxw = _meshBoxTrans.getSize().x;
    float xl = _meshBoxTrans.getCenter().x - (boxw/2.0f);
    float xr = xl + boxw;
    float boxh = _meshBoxTrans.getSize().y;
    float ycur = _meshBoxTrans.getTopLeft().y;
    float ylerp = boxh / (float)depthCountTrim;

    /*
    float width = getBbSize().x(); //* .10f;
    float xl = getBbCenter().x() - (width/2.0f);
    float xr = xl + width;
    float ycur = getBbMax().y();
    float ylerp = getBbSize().y() / (float)depthCountTrim;
    */

    float dmin = 999999999.0f;
    float dmax = -999999999.0f;
    float dtotal = 0;
    float dmean = 0;
    float dstd = 0;
    for (int i=trim; i < (depthCount - trim); i++)
    {
        float d = _tipData.profile->getDepth()[i];

        if (d < dmin) dmin = d;
        if (d > dmax) dmax = d;
        dtotal += d;

        // 2 side by side pts
        vbo.push_back(xl);
        vbo.push_back(ycur);
        vbo.push_back(d);
        box.updateBox(xl, ycur, d);

        vbo.push_back(xr);
        vbo.push_back(ycur);
        vbo.push_back(d);
        box.updateBox(xr, ycur, d);

        ycur -= ylerp;
    }

    dmean = dtotal / depthCountTrim;

    // compute standard deviation
    dtotal = 0;
    for (int i=trim; i < (depthCount - trim); i++)
    {
        float d = _tipData.profile->getDepth()[i];
        float dv = d - dmean;
        dtotal += dv * dv;
    }
    dstd = dtotal / depthCountTrim;

    // for now use min and max depths for color scale..
    // but should probably switch to 2 standard deviation above and below mean for min/max scale
    int idxvertrow = 0;
    for (int i=trim; i < (depthCount - trim); i++)
    {
        float d = _tipData.profile->getDepth()[i];

        // compute color and set for left and right vert
        UtlQt3d::getColor(d, dmin, dmax, &cbo);
        UtlQt3d::getColor(d, dmin, dmax, &cbo);

        // compute normal and set for left and right vert
        QVector3D navg(0,0,0);
        int ncount = 0;
        int vertsperrow = 2;
        int vstart = idxvertrow * vertsperrow; // * 2 each row has 2 verts

        // do we have a previous row to compute normal with
        if (vstart > 0)
        {
            int vstartprev = (idxvertrow - 1) * vertsperrow;
            navg += UtlQt3d::getNormalFromIbo(vbo, vstartprev, vstart, vstart+1);
            ncount++;
        }

        // do we have a next row to compute normal with
        if ((vstart + vertsperrow + 1)*3 < (int)vbo.size())
        {
            int vstartnext = (idxvertrow + 1) * vertsperrow;
            navg += UtlQt3d::getNormalFromIbo(vbo, vstart, vstartnext, vstartnext+1);
            ncount++;
        }

        navg /= ncount;
        navg.normalize();

        // fill nbo with normal for 2 row verts
        nbo.push_back(navg.x());
        nbo.push_back(navg.y());
        nbo.push_back(navg.z());
        nbo.push_back(navg.x());
        nbo.push_back(navg.y());
        nbo.push_back(navg.z());

        // make a quad per row (accept last row will get no quad)
        int idxvert = idxvertrow * vertsperrow;

        if ((idxvert + 3)*3 < (int)vbo.size())
        {
            ibo.push_back(idxvert);
            ibo.push_back(idxvert + 2);
            ibo.push_back(idxvert + 3);
            ibo.push_back(idxvert + 1);
        }
        else
        {
            // should be last row
        }
        idxvertrow++;
    }

    /*
    int idxvertrow = 0;
    for (int i=trim; i < (depthCount - trim); i++)
    {
        float d = _tipData.profile->getDepth()[i];

        // compute color and set for left and right vert
        UtlQt3d::getColor(d, dmin, dmax, &cbo);
        UtlQt3d::getColor(d, dmin, dmax, &cbo);

        // compute normal and set for left and right vert
        QVector3D navg(0,0,0);
        int ncount = 0;
        int vstart = idxvertrow * 6; // * 6 each row has 2 verts with 3 values each

        // do we have a previous row to compute normal with
        if (vstart > 0)
        {
            int vstartprev = (idxvertrow - 1) * 6;
            navg += UtlQt3d::getNormal(vbo, vstartprev, vstart, vstart+3);
            ncount++;
        }

        // do we have a next row to compute normal with
        if (vstart + 9 < vbo.size())
        {
            int vstartnext = (idxvertrow + 1) * 6;
            navg += UtlQt3d::getNormal(vbo, vstart, vstartnext, vstartnext+3);
            ncount++;
        }

        navg /= ncount;
        navg.normalize();

        // make a quad per row (accept last row will get no quad)
        int idxvert = idxvertrow * 2;

        if (idxvert + 3*3 < vbo.size())
        {
            ibo.push_back(idxvert);
            ibo.push_back(idxvert + 2);
            ibo.push_back(idxvert + 3);
            ibo.push_back(idxvert + 1);

            // fill nbo with normal for 2 row verts
            nbo.push_back(navg.x());
            nbo.push_back(navg.y());
            nbo.push_back(navg.z());
            nbo.push_back(navg.x());
            nbo.push_back(navg.y());
            nbo.push_back(navg.z());
        }
        else
        {
            // should be last row
            int idebug = 1;
        }
        idxvertrow++;
    }
    */

    /*
    if (cbo.size() != vbo.size())
    {
        LogError("%s unexpected error cbo size %d doesn't match vbo size %d", func, cbo.size(), vbo.size());
    }

    if (nbo.size() != vbo.size())
    {
        LogError("%s unexpected error nbo size %d doesn't match vbo size %d", func, nbo.size(), vbo.size());
    }
    */

    // UtlQt3d::debugDumpQuads("D:\\dev\\mantis_dev\\mantis\\data\\debug_markquads.txt", ibo, vbo);

    _tipData.depthMin = dmin;
    _tipData.depthMax = dmax;
    _tipData.depthMean = dmean;
    _tipData.depthStdDev = dstd;

    if (!_tipData.mesh)
    {
        _tipData.mesh.reset(new Mesh());
    }

    _tipData.mesh->setBox(box);
    return _tipData.mesh->createQuadsN(ibo, vbo, nbo); // compute the color in the shader
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::destroyBuffers()
{
    _mesh.destroy();
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::computeTriangleNormal(std::vector<GLfloat>&  normals, const std::vector<GLfloat> &xyz,
                                               int centerIdx,  int leftIdx, int rightIdx,
                                               box3f *pbox, box3f *pboxtrans, const QMatrix4x4 *mat)
{
    //WARNING: This modifies the normals QVector.
    //Doing this to avoid having to store the normals on
    //both CPU and GPU.
    QVector3D centerPt (xyz[3*centerIdx], xyz[3*centerIdx + 1], xyz[3*centerIdx + 2]);
    QVector3D leftPt (xyz[3*leftIdx], xyz[3*leftIdx + 1], xyz[3*leftIdx + 2]);
    QVector3D rightPt (xyz[3*rightIdx], xyz[3*rightIdx + 1], xyz[3*rightIdx + 2]);
    QVector3D normalVec = QVector3D::normal(rightPt - centerPt, leftPt - centerPt);

    // update the bounding box
    if (pbox)
    {
        pbox->updateBox(centerPt.x(), centerPt.y(), centerPt.z());
        pbox->updateBox(leftPt.x(), leftPt.y(), leftPt.z());
        pbox->updateBox(rightPt.x(), rightPt.y(), rightPt.z());
    }

    if (pboxtrans)
    {
        if (mat)
        {
            centerPt = *mat * centerPt;
            leftPt = *mat * leftPt;
            rightPt = *mat * rightPt;
        }

        pboxtrans->updateBox(centerPt.x(), centerPt.y(), centerPt.z());
        pboxtrans->updateBox(leftPt.x(), leftPt.y(), leftPt.z());
        pboxtrans->updateBox(rightPt.x(), rightPt.y(), rightPt.z());
    }


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

/*
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
*/

//=======================================================================
//=======================================================================
void RangeImageRenderer::internalRender(QGLWidget* scene)
{
    const char *func = "RangeImageRenderer::internalRenderer() - ";

    if (!_model) return;

    scene->makeCurrent();

    Mesh *pmesh = &_mesh;
    int shaderid = _currentShaderProgram;
    const qreal *matTransform = _transform.constData();
    const qreal *matCoordSys = _model->getCoordinateSystemMatrix().constData();

    //Init buffers & shaders if this is first draw
    //Need context to do this.
    if (!_mesh.created())
    {
        LogTrace("Drawing for the first time....");
        initShaders();
        //Turn on the vertex array capability.
        glEnableClientState(GL_VERTEX_ARRAY);
        //Turn on the normal array capability.
        glEnableClientState(GL_NORMAL_ARRAY);
        //Turn on the texture coordinate array capability.
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //initBuffers();
        initMesh();
        glEnable(GL_BLEND); //for the selection plane.
    }

    if (_tipData.updatePolys)
    {
        initTipMesh();
        _tipData.updatePolys = false;
    }

    bool drawingMark = false;
    if (_tipData.draw && _tipData.vtip)
    {
        //UtlQt3d::logMatrix("internal transform", _transform);
        //UtlQt3d::logMatrix("internal coorninate system", _model->getCoordinateSystemMatrix());
        pmesh = _tipData.mesh.get();
        shaderid = _tipData.shader;
        matTransform = NULL;
        matCoordSys = NULL;
        drawingMark = true;
    }

    if (!pmesh) return;



    QGLShaderProgram *shdr = getShader(shaderid);

    //Bind current shader.
    if (!shdr)
    {
        //In case you've done something like pull all
        //the programs out of _shaderPrograms:
        LogError("You have no shader programs! Drawing with 1.0 pipeline.");
        glColor3f(0, 0, 1.0f); //blue pen
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        if (!shdr->bind())
        {
            LogError("%s Failed to bind shader", func);
        }
        //Provide shaders with any needed data/settings.
        passDataToShaders(scene, shaderid);
        //If _currentShaderProgram is valid, then polyMode
        //will have an entry for it (see setDrawMode).
        glPolygonMode(GL_FRONT_AND_BACK, _polyModes[shaderid]);
    }

    //Set up matrix stack.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    if (matTransform)
    {
        glMultMatrixd(matTransform);
    }
    if (matCoordSys)
    {
        glMultMatrixd(matCoordSys);
    }

    pmesh->draw();

    //Unbind current shader program.
    if (shdr) shdr->release();

    //Draw selection, if enabled.
    if (_selection->isEnabled()) _selection->draw();

    // draw search box
    if (drawingMark)
    {
        drawSearchBoxMark(pmesh);
    }
    else
    {
        drawSearchBox();
    }

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
void RangeImageRenderer::drawSearchBox()
{
    if (_model.isNull()) return;
    if (!_searchBox.draw) return;
    if (_searchBox.height <= 0) return;

    //float color[4] = {.6078f, 0, 1, .6f};
    //float zmul = 5;
    float ymin = _searchBox.y;
    float ymax = _searchBox.y + _searchBox.height;
    //float zmax = _bbMax.z() + 1;
    //float zmin = _bbMin.z() - 1;
    float z = _bbMin.z() + (_bbMax.z() - _bbMin.z())/2.0f;

    Selection::drawSearchBox(_bbMin.x(), _bbMax.x(), ymax, ymin, z, _searchBox.color, 5);
    /*
    // draw the box, 2 row lines, 2 col lines
    Selection::drawRowPlane(_bbMin.x(), _bbMax.x(), ymin, _bbMin.z(), _bbMax.z(), color, zmul);
    Selection::drawRowPlane(_bbMin.x(), _bbMax.x(), ymax, _bbMin.z(), _bbMax.z(), color, zmul);
    Selection::drawColPlane(_bbMin.x(), ymin, ymax, _bbMin.z(), _bbMax.z(), color, zmul);
    Selection::drawColPlane(_bbMax.x(), ymin, ymax, _bbMin.z(), _bbMax.z(), color, zmul);
    */
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::drawSearchBoxMark(Mesh *mesh)
{
    if (_model.isNull()) return;
    if (!mesh) return;
    if (!_searchBox.draw) return;
    if (_searchBox.height <= 0) return;
    if (_searchBox.dataLen == 0) return;

    const box3f &box = mesh->getBox();

    float perminy = (float)_searchBox.y / (float)_searchBox.dataLen;
    float permaxy = (float)(_searchBox.y + _searchBox.height) / (float)_searchBox.dataLen;

    float mdlh = box.getHeight();
    float ymin = box.vmin.y + mdlh * perminy;
    float ymax = box.vmin.y + mdlh * permaxy;

    float z = box.vmin.z + (box.vmax.z - box.vmin.z)/2.0f;

    Selection::drawSearchBox(box.vmin.x, box.vmax.x, ymax, ymin, z, _searchBox.color, 5);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setSearchBox(int y, int height, int dataLen, bool draw)
{
    _searchBox.y = y;
    _searchBox.height = height;
    _searchBox.dataLen = dataLen;
    _searchBox.draw = draw;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::setSearchBoxColor(float r, float g, float b, float a)
{
    _searchBox.color[0] = r;
    _searchBox.color[1] = g;
    _searchBox.color[2] = b;
    _searchBox.color[3] = a;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::draw(QGLWidget* scene)
{
    if (!_model) return;

    if (_needSelectionUpdate)
	{
        updateStatsSelection(scene,_winX, _winY);
        _needSelectionUpdate = false;
	}

	internalRender(scene);
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::updateStatsSelection(QGLWidget* scene, int winx, int winy)
{
    if (!_model) return;

    // do we need to initialize
    //if (!_vbo.isCreated())
    if (!_mesh.created())
    {
        internalRender(scene);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

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
    glReadPixels(winx,  scene->height() - winy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

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
QGLShaderProgram* RangeImageRenderer::getShader(int idx)
{
    if (_shaderPrograms.size() <= 0) return NULL;
    if (idx < 0 || idx >= _shaderPrograms.size()) return NULL;
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

//=======================================================================
//=======================================================================
int RangeImageRenderer::getProfilePlateCol()
{
    if (_model.isNull())
    {
        return 0;
    }

    QPointF sel = getBasis();
    int col = sel.x()/_model->getPixelSizeX();
    return col;
}

//=======================================================================
//=======================================================================
Profile* RangeImageRenderer::getProfile()
{
    if (_model.isNull())
    {
        return NULL;
    }

    int col = getProfilePlateCol();
    Profile *profile = _model->getColumn(col);
    _tipData.draw = false;

    return profile;
}

//=======================================================================
//=======================================================================
PProfile RangeImageRenderer::getProfilePlate()
{
    Profile *profile = getProfile();
    if (!profile) return PProfile();

    return PProfile(profile);
}

//=======================================================================
//=======================================================================
PProfile RangeImageRenderer::getProfileTip(float rotx, float roty, float rotz)
{
    if (_tipData.vtip == NULL)
    {
        return PProfile();
    }

    if (_model.isNull())
    {
        return PProfile();
    }


    Profile *profile = _tipData.vtip->mark(rotx, roty, rotz);
    _tipData.updatePolys = true;
    //_tipData.draw = true;
    _tipData.profile.reset(profile);
    _tipData.shaderPrev = _currentShaderProgram;
    return _tipData.profile;
}


//=======================================================================
//=======================================================================
void RangeImageRenderer::setProfileTip(PProfile profile)
{
    _tipData.updatePolys = true;
    //_tipData.draw = true;
    _tipData.profile = profile;
    _tipData.shaderPrev = _currentShaderProgram;
}

//=======================================================================
//=======================================================================
bool RangeImageRenderer::isValid()
{
    if (_model.isNull()) return false;
    return true;
}

//=======================================================================
//=======================================================================
void RangeImageRenderer::logInfo()
{
    GenericModel::logInfo();

    if (_model.isNull()) return;
    _model->logInfo();
}

