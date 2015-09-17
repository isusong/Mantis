#include "UtlQt.h"
#include <QDir>
#include <QTime>
#include <QCoreApplication>

//=======================================================================
//=======================================================================
bool UtlQt::fileExists(const char *path)
{
    return fileExists(QString(path));
}

//=======================================================================
//=======================================================================
bool UtlQt::fileExists(const QString &path)
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
QString UtlQt::fileNameWithExt(const QString &fullpath)
{
    QFileInfo fi(fullpath);
    return fi.fileName();
}

//=======================================================================
//=======================================================================
QString UtlQt::filePath(const QString &fullpath)
{
    QFileInfo fi(fullpath);
    return fi.dir().path();
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

//=======================================================================
//=======================================================================
void UtlQt::delay(int secs)
{
    QTime dieTime= QTime::currentTime().addSecs(secs);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
