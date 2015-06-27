#ifndef UTLQT3D_H
#define UTLQT3D_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QtOpenGL>
#include <limits>

class UtlQt3d
{
public:

    static QVector3D getVert(const std::vector<GLfloat> &v, int idxstart);
    static QVector3D normal(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3);
    static QVector3D getNormalFromIbo(const std::vector<GLfloat> &v, int ibo1, int ibo2, int ibo3);
    static QVector3D getNormalFromIdx(const std::vector<GLfloat> &v, int idx1, int idx2, int idx3);

    static void getColor(float value, float min, float max, std::vector<GLfloat> *pv);

    static QVector3D eulerAngles(const QMatrix4x4 &m);

    static bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());


    static bool debugDumpQuads(const char *filepath, const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo);

    static void logMatrix(const char *msg, const QMatrix4x4 &mat);
};

#endif // UTLQT3D_H
