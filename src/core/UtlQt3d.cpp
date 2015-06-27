#include "UtlQt3d.h"
#include "logger.h"

const float PI = 3.14159265358979323846264f;

//=======================================================================
//=======================================================================
QVector3D UtlQt3d::getVert(const std::vector<GLfloat> &v, int idxstart)
{
    if (idxstart + 2 >= (int)v.size())
    {
        return QVector3D(0,0,0);
    }

    return QVector3D(v[idxstart], v[idxstart+1], v[idxstart+2]);
}

//=======================================================================
//=======================================================================
QVector3D UtlQt3d::normal(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3)
{
    QVector3D n = QVector3D::normal(v2 - v1, v3 - v2);
    return n;
}

//=======================================================================
// ibo index is index into the vertex array as if each vertex where a single item, but the v vector has 3 items per vertex
//=======================================================================
QVector3D UtlQt3d::getNormalFromIbo(const std::vector<GLfloat> &v, int ibo1, int ibo2, int ibo3)
{
    return UtlQt3d::getNormalFromIdx(v, ibo1*3, ibo2*3, ibo3*3);
}

//=======================================================================
//=======================================================================
QVector3D UtlQt3d::getNormalFromIdx(const std::vector<GLfloat> &v, int idx1, int idx2, int idx3)
{
    QVector3D v1 = getVert(v, idx1);
    QVector3D v2 = getVert(v, idx2);
    QVector3D v3 = getVert(v, idx3);
    return UtlQt3d::normal(v1, v2, v3);
}

//=======================================================================
//=======================================================================
void UtlQt3d::getColor(float value, float min, float max, std::vector<GLfloat> *pv)
{
    if (value < min) value = min;
    if (value > max) value = max;

    GLfloat per = (value - min) / (max-min);

    pv->push_back(per); // r
    pv->push_back(per); // g
    pv->push_back(per); // b
    pv->push_back(1);   // a
}

//=======================================================================
//=======================================================================
bool UtlQt3d::closeEnough(const float& a, const float& b, const float& epsilon)
{
    return (epsilon > std::abs(a - b));
}

//=======================================================================
// QMatrix is in row-major
//=======================================================================
QVector3D UtlQt3d::eulerAngles(const QMatrix4x4& m)
{
    const qreal *p = m.constData();
    qreal R[4][4];

    // row major order
    for (int r=0; r<4; r++)
    {
        for (int c=0; c<4; c++)
        {
            R[r][c] = *p;
            p++;
        }
    }

    //check for gimbal lock
    if (closeEnough(R[0][2], -1.0f)) {
        float x = 0; //gimbal lock, value of x doesn't matter
        float y = PI / 2;
        float z = x + atan2(R[1][0], R[2][0]);
        return QVector3D( x, y, z );
    } else if (closeEnough(R[0][2], 1.0f)) {
        float x = 0;
        float y = -PI / 2;
        float z = -x + atan2(-R[1][0], -R[2][0]);
        return QVector3D( x, y, z );
    } else { //two solutions exist
        float x1 = -asin(R[0][2]);
        float x2 = PI - x1;

        float y1 = atan2(R[1][2] / cos(x1), R[2][2] / cos(x1));
        float y2 = atan2(R[1][2] / cos(x2), R[2][2] / cos(x2));

        float z2 = atan2(R[0][1] / cos(x2), R[0][0] / cos(x2));
        float z1 = atan2(R[0][1] / cos(x1), R[0][0] / cos(x1));

        Q_UNUSED(y2);
        Q_UNUSED(z2);
        return QVector3D( x1, y1, z1 );

        //choose one solution to return
        //for example the "shortest" rotation
        /*
        if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2))) {
            return { x1, y1, z1 };
        } else {
            return { x2, y2, z2 };
        }
        */
    }
}

/*
//=======================================================================
// from quaternion
//=======================================================================
QVector3D UtlQt::eulerAngles(float q0, float q1, float q2, float q3)
{
    return QVector3D(   atan2(2 * (q0*q1 + q2*q3), 1 - 2 * (q1*q1 + q2*q2)),
                        asin( 2 * (q0*q2 - q3*q1)),
                        atan2(2 * (q0*q3 + q1*q2), 1 - 2 * (q2*q2 + q3*q3));

}
*/

/*
//=======================================================================
// from quaternion
//=======================================================================
QVector3D UtlQt::eulerAngles(double x, double y, double z, double w)
{
    double roll  = Mathf.Atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z);
    double pitch = Mathf.Atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z);
    double yaw   =  Mathf.Asin(2*x*y + 2*z*w);

    return QVector3D(pitch, yaw, roll);
}
*/

//=======================================================================
//=======================================================================
bool UtlQt3d::debugDumpQuads(const char *filepath, const std::vector<GLuint> &ibo, const std::vector<GLfloat> &vbo)
{
    const char *func = "UtlQt3d::debugDumpQuads() -";

    //Open the file.
    FILE *fp = NULL;
    fopen_s(&fp, filepath, "wt");

    if (!fp)
    {
        LogError("%s failed to open the file: %s", func, filepath);
        return false;
    }

    bool baddata = false;
    int i = 0;
    int quads = ibo.size() / 4;
    fprintf(fp, "quads: %d \n\n", quads);
    for (int q=0; q<quads; q++)
    {
        fprintf(fp, "%d\n", q);
        for (int ib=0; ib<4; ib++)
        {
            if (i >= (int)ibo.size())
            {
                fprintf(fp, "unexpected: ibo index out ouf range: %d, ibo size: %d\n", i, ibo.size());
                baddata = true;
                break;
            }
            int idx = ibo[i]; i++;
            idx *= 3;

            if (idx + 2 >= (int)vbo.size())
            {
                fprintf(fp, "unexpected: vbo index out ouf range: %d, vbo size: %d\n", idx, vbo.size());
                baddata = true;
                break;
            }

            float x = vbo[idx];
            float y = vbo[idx+1];
            float z = vbo[idx+2];

            fprintf(fp, "%.2f, %.2f, %.2f\n", x, y, z);
        }

        fprintf(fp, "\n");

        if (baddata) break;
    }

    fclose(fp);
    return true;

}

//=======================================================================
//=======================================================================
void UtlQt3d::logMatrix(const char *msg, const QMatrix4x4 &mat)
{
    const qreal *matrix = mat.constData();
    if (msg) LogTrace("%s", msg);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[0], matrix[1], matrix[2], matrix[3]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[4], matrix[5], matrix[6], matrix[7]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[8], matrix[9], matrix[10], matrix[11]);
    LogTrace("%.2f, %.2f, %.2f, %.2f", matrix[12], matrix[13], matrix[14], matrix[15]);
}
