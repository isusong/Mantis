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

#ifndef __STREAMBUFFER_H__
#define __STREAMBUFFER_H__
#include <QGLBuffer>

/**
 * Class for streaming vertices to the GPU using a buffer object.
 * This speeds code performance and avoids the problem of OpenGL 
 * reserving upwards of 70% of the CPU memory for a buffer for
 * glVertex4f commands.
 *
 * You need to have a valid OpenGL context before creating 
 * a StreamBuffer. The StreamBuffer will piggyback on the current context.
 *
 * @author Laura Ekstrand
 */

class StreamBuffer
{
  protected:
	///Number of vertices currently in buffer.
	int size;
	///Number of vertices the buffer can hold.
	int capacity;
	///Drawing mode for the vertices.  Default is GL_TRIANGLES.
	GLenum drawMode;
	///Vertex buffer object for streaming vertex data.
	QGLBuffer vbo;
	///Client side back up for streaming vertex data.
	/**
	 * This is a client-side back up for the mapped buffer 
	 * pointer. In case the buffer doesn't unmap correctly 
	 * and the GPU vertices are lost before they are drawn.
	 */
	GLfloat* vertices;

  public:
	StreamBuffer(int maxVertices, GLenum vertexDrawMode = GL_TRIANGLES);
	virtual ~StreamBuffer();

	///Pass through for QGLBuffer::bind()
	/**
	 * You should do this before adding vertices with addVertex().
	 */
	inline void bind() {vbo.bind();}
	///Place a vertex in the stream buffer object.
	void addVertex(float x, float y, float z, float w);
	///Flush the buffer to the graphics card.
	/**
	 * Do this when you're done to flush any leftover vertices
	 * that didn't manage to fill the whole buffer
	 */
	void flush();
	///Returns the internal capacity (maximum number of vertices).
	inline int getCapacity() {return capacity;}
	///Tells you how large the stream buffer will be in MegaBytes.
	static float toMB(int nVertices);
};

#endif //! defined __STREAMBUFFER_H__
