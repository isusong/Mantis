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

RangeImageRenderer::RangeImageRenderer(RangeImage* newModel, 
	QWidget *parent):
	GenericModel(parent)
{
	//Set the model.
	model = newModel;
	width = model->getWidth();
	height = model->getHeight();
	pixelSizeX = model->getPixelSizeX();
	pixelSizeY = model->getPixelSizeY();
	depth = model->getDepth();
	mask = model->getMask();
	computeBoundingBox();

	//Default downsample.
	skip = 6;

	//Downsample the texture.
	QImage cachedTexture = model->getTexture();
	int scaleFactor = skip/3;
	if (1 > scaleFactor) scaleFactor = 1;
	int scaledHeight = cachedTexture.height()/scaleFactor;
	scaledTexture = 
		cachedTexture.scaledToHeight(scaledHeight);

	//Default drawing mode.
	currentShaderProgram = 0;
	drawCS = true;
	lightOrigin.setX(-1000);
	lightOrigin.setY(-500);
	lightOrigin.setZ(1000);
	selection = new Selection(bbMin, bbMax, this);
	selection->setEnabled(false);
	connect(selection, SIGNAL(updateGL()), this,
		SIGNAL(updateGL()));
	needSelectionUpdate = false;
	winX = 0;
	winY = 0;
}

RangeImageRenderer::~RangeImageRenderer()
{
	//model belongs to someone else.
	delete selection;
}

void
RangeImageRenderer::computeBoundingBox()
{
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;

	for (int i = 0; i < width*height; ++i)
	{
		if (mask.testBit(i))
		{
			float currentX = (i%width)*pixelSizeX;
			float currentY = (i/width)*pixelSizeY;
			float currentZ = depth[i];
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
	bbMin = QVector3D(minX, minY, minZ);
	bbMax = QVector3D(maxX, maxY, maxZ);
}

void
RangeImageRenderer::initShaders()
{
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
	shaderPrograms.push_back(shadedProgram);
	polyModes.push_back(GL_FILL);
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
	shaderPrograms.push_back(wireframeProgram);
	polyModes.push_back(GL_LINE);
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
	shaderPrograms.push_back(texturedProgram);
	polyModes.push_back(GL_FILL);
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
	shaderPrograms.push_back(colorMappedProgram);
	polyModes.push_back(GL_FILL);
	//unproject program
	QGLShaderProgram* unprojectProgram = new QGLShaderProgram(this);
	unprojectProgram->addShaderFromSourceFile(
        QGLShader::Vertex, ":/glsl/glsl/colorlevels.vert");
	unprojectProgram->addShaderFromSourceFile(
        QGLShader::Fragment, ":/glsl/glsl/colorlevels.frag");
    unprojectProgram->link();
	shaderPrograms.push_back(unprojectProgram);
	polyModes.push_back(GL_FILL);
}

void
RangeImageRenderer::passDataToShaders(GraphicsWidget* scene)
{
	if (4 != currentShaderProgram)	
	{
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"lightOrigin", lightOrigin);
	}
	if (3 == currentShaderProgram)
	{
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"minDepth", (GLfloat) bbMin.z());
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"maxDepth", (GLfloat) bbMax.z());
	}
	if (2 == currentShaderProgram)
	{
		scene->bindTexture(scaledTexture,
			GL_TEXTURE_2D, GL_RGBA,
			QGLContext::LinearFilteringBindOption);
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"modelTexture", (GLuint) 0);
	}
	if (4 == currentShaderProgram)
	{
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"width", width);
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"height", height);
		shaderPrograms[currentShaderProgram]->setUniformValue(
			"skip", skip);
	}
} 

void
RangeImageRenderer::initBuffers()
{
	//Downsampled width & height.
	int dWidth = width/skip;
	int dHeight = height/skip;

	//Populate vertices & texture coordinates.
	GLfloat* xyz = 
		new GLfloat[3*dWidth*dHeight];
	GLfloat* texCoords = new GLfloat[2*dWidth*dHeight];
	for(int i = 0; i < dHeight; ++i)
	{
		for(int j = 0; j < dWidth; ++j)
		{
			int idx = dWidth*i + j;
			xyz[3*idx + 0] = (GLfloat) j*skip*pixelSizeX;
			xyz[3*idx + 1] = (GLfloat) i*skip*pixelSizeY;
			xyz[3*idx + 2] = (GLfloat) 
				depth[width*i*skip + j*skip];
			texCoords[2*idx] = (1/((float) dWidth)) * (j + 0.5f);
			texCoords[2*idx + 1] = (1/((float) dHeight)) 
				* (i + 0.5f);
		}
	}
	//Init VBO
	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QGLBuffer::StaticDraw);
	vbo.allocate(xyz, 3*dWidth*dHeight*sizeof(GLfloat));
	//Init TBO
	tbo.create();
	tbo.bind();
	tbo.setUsagePattern(QGLBuffer::StaticDraw);
	tbo.allocate(texCoords, sizeof(GLfloat)*2*dWidth*dHeight);
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
			int index0 = width*i*skip + j*skip;
			int index1 = index0 + skip;
			int index2 = index0 + width*skip;
			int index3 = index2 + skip;
			//Indices into the vertices.
			int vertID0 = dWidth*i + j;
			int vertID1 = vertID0 + 1;
			int vertID2 = vertID0 + dWidth;
			int vertID3 = vertID2 + 1;

			//Can I connect point 1 to point 2?
			if (mask.testBit(index1) & mask.testBit(index2))
			{
				// I use the default / method to divide the 
				//triangles.
				if(mask.testBit(index0)) //Can I connect 0 to 1 and 2?
				{
					//If so, I can make a triangle out of them.
					indices.push_back(vertID0);
					indices.push_back(vertID1);
					indices.push_back(vertID2);
					computeTriangleNormal(normals, xyz, vertID2, 
						vertID0, vertID1);
				}
				if(mask.testBit(index3)) //Can I connect 3 to 1 and 2?
				{
					indices.push_back(vertID2);
					indices.push_back(vertID3);
					indices.push_back(vertID1);
					computeTriangleNormal(normals, xyz, vertID2, 
						vertID1, vertID3);
				}
			}
			else if (mask.testBit(index0) & mask.testBit(index3)) //Can I connect 0 and 3?
			{
				//I use the \ method the divide the triangles.
				if(mask.testBit(index1)) //Can I connect 1 to 0 and 3?
				{
					indices.push_back(vertID1);
					indices.push_back(vertID3);
					indices.push_back(vertID0);
					computeTriangleNormal(normals, xyz, vertID3, 
						vertID0, vertID1);
				}
				if(mask.testBit(index2)) //Can I connect 2 to 0 and 3?
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
	ibo = QGLBuffer(QGLBuffer::IndexBuffer);
	ibo.create();
	ibo.bind();
	ibo.setUsagePattern(QGLBuffer::StaticDraw);
	ibo.allocate(indices.data(), sizeof(GLuint)*indices.size());
	nTriangles = indices.size()/3;

	//Allocate the nbo.
	nbo.create();
	nbo.bind();
	nbo.setUsagePattern(QGLBuffer::StaticDraw);
	nbo.allocate(normals.data(), sizeof(GLfloat)*normals.size());

	delete [] xyz;
}

void
RangeImageRenderer::computeTriangleNormal(QVector<GLfloat>& 
	normals, const float* xyz, int centerIdx, 
	int leftIdx, int rightIdx)
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

void
RangeImageRenderer::internalRender(GraphicsWidget* scene)
{
	//Init buffers & shaders if this is first draw
	//Need context to do this.
	if (!vbo.isCreated())
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

	//Bind current shader.
	if (shaderPrograms.isEmpty())
	{
		//In case you've done something like pull all
		//the programs out of shaderPrograms:
		qDebug() << "You have no shader programs!" <<
			"Drawing with 1.0 pipeline.";
		glColor3f(0, 0, 1.0f); //blue pen
		glPolygonMode(GL_FRONT_AND_BACK,
			GL_FILL);
	}
	else 
	{
		shaderPrograms[currentShaderProgram]->bind();
		//Provide shaders with any needed data/settings.
		passDataToShaders(scene);
		//If currentShaderProgram is valid, then polyMode
		//will have an entry for it (see setDrawMode).
		glPolygonMode(GL_FRONT_AND_BACK,
			polyModes[currentShaderProgram]);
	}

	//Set up matrix stack.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(transform.constData());
	glMultMatrixd(model->getCoordinateSystemMatrix().constData());

	//Draw model data.
	vbo.bind();
	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	nbo.bind();
	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glNormalPointer(GL_FLOAT, 0, NULL);

	tbo.bind();
	//Put buffer contents in Vertex Array.
	//NULL says "use the currently bound buffer"
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	ibo.bind();
	//Draw the buffers in the order given in ibo.
	//NULL says "use the currently bound buffer" as the indices.
	glDrawElements(GL_TRIANGLES, nTriangles*3, 
		GL_UNSIGNED_INT, NULL);

    //Unbind current shader program.
    if (!shaderPrograms.isEmpty())
          shaderPrograms[currentShaderProgram]->release();

	//Draw selection, if enabled.
	if (selection->isEnabled())
		selection->draw();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    //Draw coordinate system.
	if (drawCS)
    {
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixd(transform.constData());
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

void
RangeImageRenderer::draw(GraphicsWidget* scene)
{
	if (needSelectionUpdate)
	{
		//Save state.
		int oldProgram = currentShaderProgram;
		bool oldDrawCS = drawCS;
		bool oldSelectionEnabled = selection->isEnabled();

		//Draw for identifying window coordinates.
		currentShaderProgram = 4;
		drawCS = false;
		selection->setEnabled(false);
		internalRender(scene);

		//Read back pixel color.
		unsigned char color[3];
		//Must flip y - Qt and OpenGL are different.
		glReadPixels(winX, 
			scene->height() - winY, 1, 1,
			GL_RGB, GL_UNSIGNED_BYTE, color);

		//Determine i and j values from pixel color.
		int id = (65536*color[0]) + (256*color[1]) + color[2];
		int i = id/width;
		int j = id%width;

		//Update selection.
		selection->updateSelectionBasis(pixelSizeX*j, pixelSizeY*i);

		//Restore state.
		currentShaderProgram = oldProgram;
		drawCS = oldDrawCS;
		selection->setEnabled(oldSelectionEnabled);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//We have updated the selection.
		needSelectionUpdate = false;
	}
	internalRender(scene);
}

void
RangeImageRenderer::translate(const QVector3D& vector)
{
	QMatrix4x4 temp;
	temp.translate(vector);
	applyLeftTransform(temp);
}

void 
RangeImageRenderer::rotate(float angle, const QVector3D& axis)
{
	QMatrix4x4 temp;
	temp.rotate(angle, axis);
	applyRightTransform(temp);
}

void
RangeImageRenderer::applyLeftTransform(const QMatrix4x4& other)
{
	transform = other * transform;
}

void
RangeImageRenderer::applyRightTransform(const QMatrix4x4& other)
{
	transform = transform * other;
}

void
RangeImageRenderer::setSkip(int newSkip)
{
	skip = newSkip;

	if (1 != skip)
	{
		QImage cachedTexture = model->getTexture();
		int scaleFactor = skip/3;
		if (1 > scaleFactor) scaleFactor = 1;
		int scaledHeight = cachedTexture.height()/scaleFactor;
		scaledTexture = 
			cachedTexture.scaledToHeight(scaledHeight);
	}
	else
		scaledTexture = model->getTexture();
}

void
RangeImageRenderer::setDrawMode(int idx)
{
	//If idx is a valid index into shaderPrograms, use it.
	if ((0 <= idx) && (idx < shaderPrograms.size()))
	{
		currentShaderProgram = idx;

		//If not enough polyModes:
		if (idx >= polyModes.size())
		{
			int diff = idx - polyModes.size() + 1;
			for (int i = 0; i < diff; ++i)
			{
				polyModes.push_back(GL_FILL);
			}
		}

		emit updateGL();
	}
}

void
RangeImageRenderer::setDrawCS(bool enable)
{
	drawCS = enable;
	emit updateGL();
}

void
RangeImageRenderer::scheduleSelectionUpdate(int x, int y)
{
	if (selection->isEnabled())
	{
		needSelectionUpdate = true;
		winX = x;
		winY = y;
		emit updateGL();
	}
}
