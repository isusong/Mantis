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

#include "StreamBuffer.h"
#include <cstring>
#include <QDebug>
#include "logger.h"
#include <GL/glu.h>

StreamBuffer::StreamBuffer(int maxVertices, 
	GLenum vertexDrawMode)
{
	size = 0;
	capacity = maxVertices;
	drawMode = vertexDrawMode;
	vertices = new GLfloat[4*capacity];

	//Set up the vbo.
    bool res = vbo.create();
    res = vbo.bind();
	//Tells OpenGL that the buffer will be overwritten after almost
	//every draw.
	vbo.setUsagePattern(QGLBuffer::StreamDraw);
	//Set aside enough graphics memory to hold the elements.
	vbo.allocate(4*capacity*sizeof(GLfloat));
}

StreamBuffer::~StreamBuffer()
{
	delete vertices;
}

void
StreamBuffer::addVertex(float x, float y, float z, float w)
{
	//If full, go ahead and send the vertices to the GPU.
	if (size == capacity) flush();

    if ((4*size + 3) >= 4*capacity)
    {
        //int idebug = 1;
    }

	//Add the new vertex.
	vertices[4*size + 0] = x;
	vertices[4*size + 1] = y;
	vertices[4*size + 2] = z;
	vertices[4*size + 3] = w;
	size++;
}

void
StreamBuffer::flush()
{
	//qDebug() << "flushing stream buffer.";
	//qDebug() << "vertices pointer: " << vertices;
	//Number of tries for mapping and unmapping before giving up.
	int maxTries = 10; //Beyond this doesn't make sense.

	//Get the buffer pointer from the GPU.
	GLfloat* bufferMap;
	for (int i = 0; i < maxTries; ++i)
	{
		bufferMap = (GLfloat*) vbo.map(QGLBuffer::WriteOnly);
		if (bufferMap != NULL) break;
	}

	if (bufferMap == NULL)
	{
		qDebug() << "Buffer couldn't be mapped; " <<
			"mark will be incomplete.";
	}
	else
	{
		bool unmapSuccess;
		for (int i = 0; i < maxTries; ++i)
		{
			std::memcpy(bufferMap, vertices, 4*size*sizeof(GLfloat));
			unmapSuccess = vbo.unmap();
			if (unmapSuccess) break;

            LogError("StreamBuffer::flush() - failed to unmap vbo, trying again...");
		}

		if (unmapSuccess)
		{
			//Draw the buffer.
			//Put buffer contents in Vertex Array.
			//NULL says "use the currently bound buffer"
			//qDebug() << "Putting buffer contents in vertex array.";
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(4, GL_FLOAT, 0, NULL);
			//qDebug() << "Done putting.";
			//qDebug() << "size = " << size;
			//qDebug() << "capacity = " << capacity;
            glDrawArrays(drawMode, 0, size);
            //qDebug() << "Drew arrays.";

            /*
            GLenum errCode;
            if ((errCode = glGetError()) != GL_NO_ERROR)
            {
                LogError("StreamBuffer::flush() - OpenGL Error: %s", gluErrorString(errCode));
                //openGLError stays true.
            }
            */

		}
		else
		{
			qDebug() << "Buffer couldn't be unmapped;" <<
				"mark will be incomplete.";
		}
	}

	//Start over-writing vertices.
	size = 0;
	//qDebug() << "Done flushing stream buffer.";
}

float
StreamBuffer::toMB(int nElements)
{
	return (sizeof(GLfloat)*4*nElements)/(1024*1024);
}
