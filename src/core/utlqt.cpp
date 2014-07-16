#include "utlqt.h"
#include <QDir>

#include <QMatrix4x4>
#include <QVector3D>
#include <array>
#include <limits>

const float PI = 3.14159265358979323846264f;

//=======================================================================
//=======================================================================
bool UtlQt::fileExists(const char *path)
{
    QFile file(path);
    if (file.exists()) return true;

    return false;
}

//=======================================================================
//=======================================================================
bool UtlQt::validateDir(const char *dirpath, bool autoCreate)
{
    QDir dir(dirpath);
    if (dir.exists()) return true;

    if (!autoCreate) return false;

    return dir.mkpath(dirpath);
}

//=======================================================================
//=======================================================================
QString UtlQt::pathCombine(const QString &path1, const QString &path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

//=======================================================================
//=======================================================================
bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon())
{
    return (epsilon > std::abs(a - b));
}

//=======================================================================
// QMatrix is in row-major
//=======================================================================
QVector3D UtlQt::eulerAngles(const QMatrix4x4& m)
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

