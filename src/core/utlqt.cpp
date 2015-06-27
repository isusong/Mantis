#include "UtlQt.h"
#include <QDir>

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
QString UtlQt::fileName(const char *fullpath)
{
    return fileName(QString(fullpath));
}

//=======================================================================
//=======================================================================
QString UtlQt::fileName(const QString &fullpath)
{
    QFileInfo fi(fullpath);
    return fi.baseName();
}

//=======================================================================
//=======================================================================
bool UtlQt::validateDir(const char *dirpath, bool autoCreate)
{
    return validateDir(QString(dirpath), autoCreate);
}

//=======================================================================
//=======================================================================
bool UtlQt::validateDir(const QString &dirpath, bool autoCreate)
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

