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

#include "Mesh.h"
#include "../core/logger.h"

//=======================================================================
//=======================================================================
Mesh::Mesh() :
    _primCount(0),
    _primVertCount(3),
    _primType(GL_TRIANGLES)
{
}

//=======================================================================
//=======================================================================
Mesh::~Mesh()
{
    destroy();
}

//=======================================================================
//=======================================================================
int sizeBuf(QGLBuffer *p)
{
    if (!p) return 0;
    return p->size();
}

//=======================================================================
//=======================================================================
int Mesh::sizeVbo()
{
    return sizeBuf(_vbo.get());
}

//=======================================================================
//=======================================================================
int Mesh::sizeNbo()
{
    return sizeBuf(_nbo.get());
}

//=======================================================================
//=======================================================================
int Mesh::sizeCbo()
{
    return sizeBuf(_cbo.get());
}

//=======================================================================
//=======================================================================
int Mesh::sizeTbo()
{
    return sizeBuf(_tbo.get());
}

//=======================================================================
//=======================================================================
int Mesh::sizeIbo()
{
    return sizeBuf(_ibo.get());
}

//=======================================================================
// returns true if the state was modified
//=======================================================================
GLboolean setClientState(GLenum state, GLboolean curState, GLboolean newState)
{
    if (curState == newState) return false;

    if (newState == GL_TRUE)
    {
        glEnableClientState(state);
    }
    else
    {
        glDisableClientState(state);
    }

    return true;
}

//=======================================================================
//=======================================================================
bool Mesh::draw()
{
    const char *func = "Mesh::draw() -";

    // at a minimum, need primitives, vbo and ibo
    if (_primCount <= 0) return false;
    if (_vbo == NULL) return false;
    if (_ibo == NULL) return false;

    // NOTE: Should keep cash of our state and avoid these calls as they can be slow
    GLboolean onVbo = glIsEnabled(GL_VERTEX_ARRAY);
    GLboolean onNbo = glIsEnabled(GL_NORMAL_ARRAY);
    GLboolean onCbo = glIsEnabled(GL_COLOR_ARRAY);
    GLboolean onTbo = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
    GLboolean restoreVbo = false;
    GLboolean restoreNbo = false;
    GLboolean restoreCbo = false;
    GLboolean restoreTbo = false;


    // vertexes
    restoreVbo = setClientState(GL_VERTEX_ARRAY, onVbo, true);
    if (!_vbo->bind())
    {
        LogError("%s Failed to bind vbo", func);
        return false;
    }
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // normals
    if (_nbo)
    {
        restoreNbo = setClientState(GL_NORMAL_ARRAY, onNbo, true);
        if (!_nbo->bind())
        {
            LogError("%s Failed to bind nbo", func);
        }
        glNormalPointer(GL_FLOAT, 0, NULL);
    }
    else
    {
        restoreNbo = setClientState(GL_NORMAL_ARRAY, onNbo, false);
    }

    // texture coords
    if (_tbo)
    {
        restoreTbo = setClientState(GL_TEXTURE_COORD_ARRAY, onTbo, true);
        if (!_tbo->bind())
        {
            LogError("%s Failed to bind tbo", func);
        }
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    }
    else
    {
        restoreTbo = setClientState(GL_TEXTURE_COORD_ARRAY, onTbo, false);
    }

    // color
    if (_cbo)
    {
        restoreCbo = setClientState(GL_COLOR_ARRAY, onCbo, true);
        if (!_cbo->bind())
        {
            LogError("%s Failed to bind cbo", func);
        }

        glColorPointer(4, GL_FLOAT, 0, NULL);
    }
    else
    {
        restoreCbo = setClientState(GL_COLOR_ARRAY, onCbo, false);
    }

    if (!_ibo->bind())
    {
        LogError("%s Failed to bind ibo", func);
        return false;
    }

    // Draw the buffers in the order given in ibo.
    glDrawElements(_primType, _primCount*_primVertCount, GL_UNSIGNED_INT, NULL);
    //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // restore states
    if (restoreVbo) setClientState(GL_VERTEX_ARRAY, true, false);
    if (restoreNbo) setClientState(GL_NORMAL_ARRAY, true, false);
    if (restoreTbo) setClientState(GL_TEXTURE_COORD_ARRAY, true, false);
    if (restoreCbo) setClientState(GL_COLOR_ARRAY, true, false);


    return true;
}

//=======================================================================
//=======================================================================
bool Mesh::createTrisNT(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &tbo)
{
    _primVertCount = 3;
    _primType = GL_TRIANGLES;
    return create(ibo, &vbo, &nbo, NULL, &tbo);
}

//=======================================================================
//=======================================================================
bool Mesh::createTrisNC(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &cbo)
{
    _primVertCount = 3;
    _primType = GL_TRIANGLES;
    return create(ibo, &vbo, &nbo, &cbo);
}

//=======================================================================
//=======================================================================
bool Mesh::createQuadsNC(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo, const std::vector<GLfloat> &cbo)
{
    _primVertCount = 4;
    _primType = GL_QUADS;
    return create(ibo, &vbo, &nbo, &cbo);
}

//=======================================================================
//=======================================================================
bool Mesh::createQuadsN(const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo, const std::vector<GLfloat> &nbo)
{
    _primVertCount = 4;
    _primType = GL_QUADS;
    return create(ibo, &vbo, &nbo);
}

//=======================================================================
//=======================================================================
bool Mesh::create(const std::vector<GLuint> &ibo, const std::vector<GLfloat> *vbo, const std::vector<GLfloat> *nbo, const std::vector<GLfloat> *cbo, const std::vector<GLfloat> *tbo)
{
    const char *func = "Mesh::create() - ";

    destroy();

    _primCount = ibo.size() / _primVertCount;
    if (_primCount <= 0)
    {
        return false;
    }

    int bytes = 0;
    std::string buftype;
    QGLBuffer *pglbuf;
    const void *data = NULL;

    for (int i=0; i<5; i++)
    {
        if (i == 0)
        {
            if (vbo == NULL) continue;

            _vbo.reset(new QGLBuffer);
            pglbuf = _vbo.get();
            data = &(*vbo)[0];
            bytes = sizeof(GLfloat) * vbo->size();
            buftype = "vbo";
        }
        else if (i == 1)
        {
            if (nbo == NULL) continue;

            _nbo.reset(new QGLBuffer);
            pglbuf = _nbo.get();
            data = &(*nbo)[0];
            bytes = sizeof(GLfloat) * nbo->size();
            buftype = "nbo";
        }
        else if (i == 2)
        {
            if (cbo == NULL) continue;

            _cbo.reset(new QGLBuffer);
            pglbuf = _cbo.get();
            data = &(*cbo)[0];
            bytes = sizeof(GLfloat) * cbo->size();
            buftype = "cbo";
        }
        else if (i == 3)
        {
            if (tbo == NULL) continue;

            _tbo.reset(new QGLBuffer);
            pglbuf = _tbo.get();
            data = &(*tbo)[0];
            bytes = sizeof(GLfloat) * tbo->size();
            buftype = "tbo";
        }
        else if (i == 4)
        {
            _ibo.reset(new QGLBuffer(QGLBuffer::IndexBuffer)); // need to create an index buffer
            pglbuf = _ibo.get();
            data = &ibo[0];
            bytes = sizeof(GLuint) * ibo.size();
            buftype = "ibo";
        }

        if (!pglbuf->create())
        {
            LogError("%s failed to create the opengl buffer object %s", func, buftype.c_str());
            return false;
        }

        if (!pglbuf->bind())
        {
            LogError("%s failed to bind the opengl buffer object %s", func, buftype.c_str());
            return false;
        }

        pglbuf->setUsagePattern(QGLBuffer::StaticDraw);
        pglbuf->allocate(data, bytes);
    }

    return true;
}

//=======================================================================
// note: opengl context that was current when creating buffer object, must be current now
//=======================================================================
void Mesh::destroy()
{
    _primCount = 0;

    if (_ibo)
    {
        _ibo->release();
        _ibo.reset();
    }

    if (_vbo)
    {
        _vbo->release();
        _vbo.reset();
    }

    if (_nbo)
    {
        _nbo->release();
        _nbo.reset();
    }

    if (_cbo)
    {
        _cbo->release();
        _cbo.reset();
    }

    if (_tbo)
    {
        _tbo->release();
        _tbo.reset();
    }
}
