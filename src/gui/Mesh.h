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
 * Author: Brian Bailey (bbailey@code-hammer.com)
 */

#ifndef MESH_H
#define MESH_H

#include <QGLBuffer>
#include "../core/box.h"

/**
 * Class for displaying a GenericModel object in a QT widget
 * using OpenGL.
 *
 * @author Brian Bailey (bbailey@code-hammer.com)
 */

class Mesh
{
public:
    typedef std::tr1::shared_ptr<QGLBuffer> PQGLBuffer;

public:
    Mesh();
    virtual ~Mesh();

    bool created() { if (!_vbo) return false; return true; }
    int primitives() { return _primCount; }
    int sizeVbo();
    int sizeNbo();
    int sizeCbo();
    int sizeTbo();
    int sizeIbo();

    virtual bool draw();

    // TODO: flush out more types
    bool createTrisNT(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &tbo); // creates indexed triangles, with normals and texture coords
    bool createTrisNC(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &cbo); // creates indexed triangles, with normals and color
    bool createQuadsNC(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &cbo); // creates indexed quads, with normals and color
    bool createQuadsN(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo); // creates indexed quads, with normals

    void destroy();

    void setBox(const box3f &box) { _box = box; }
    const box3f& getBox() const { return _box; }

protected:
    bool create(const std::vector<GLuint> &ibo, const std::vector<GLfloat> *vbo, const std::vector<GLfloat> *nbo=NULL, const std::vector<GLfloat> *cbo=NULL, const std::vector<GLfloat> *tbo=NULL);

protected:

    int _primCount;
    int _primVertCount; // verts per each primitive (dependes on _primType, 3 for GL_TRIANGLES)
    GLenum _primType;
    PQGLBuffer _ibo; // index buffer
    PQGLBuffer _vbo; // vertex buffer
    PQGLBuffer _cbo; // color buffer
    PQGLBuffer _nbo; // normal buffer
    PQGLBuffer _tbo; // texture coords buffer
    box3f _box;
};

typedef std::tr1::shared_ptr<Mesh> PMesh;

#endif
