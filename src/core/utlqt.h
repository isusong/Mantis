#ifndef UTLQT_H
#define UTLQT_H

#include <QString>
#include <QMatrix4x4>

class UtlQt
{
public:

    static bool fileExists(const char *path);
    static bool validateDir(const char *dir, bool autoCreate=true);
    static QString pathCombine(const QString &path1, const QString &path2);

    static QVector3D eulerAngles(const QMatrix4x4 &m);
};

#endif
