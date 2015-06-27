#include "UtlQtGl.h"
#include "logger.h"
#include <QGLContext>

//QOpenGLContext* createContext(QObject *parent=NULL)

/*
//=======================================================================
//=======================================================================
QGLPixelBuffer* UtlQtGl::createPixelBuffer(int width, int height)
{
    if (!QGLPixelBuffer::hasOpenGLPbuffers()) return NULL;

    QGLPixelBuffer *pbuffer = new QGLPixelBuffer(width, height);
    if (!pbuffer->isValid())
    {
        delete pbuffer;
        return NULL;
    }

    pbuffer->makeCurrent();
        //return const_cast<QGLContext*>(QGLContext::currentContext());
    return pbuffer;
}
*/

//=======================================================================
//=======================================================================
void UtlQtGl::logModelView(const char *msg)
{
    GLfloat matrix[16];
    glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
    if (msg) LogTrace("%s", msg);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[0], matrix[1], matrix[2], matrix[3]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[4], matrix[5], matrix[6], matrix[7]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[8], matrix[9], matrix[10], matrix[11]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[12], matrix[13], matrix[14], matrix[15]);
}
