#include "UtlMtFiles.h"
#include "logger.h"
#include "UtlQt.h"
#include "RangeImage.h"
#include <QDir>
#include <QFile>

//=======================================================================
//=======================================================================
void UtlMtFiles::findFiles(const QString &dirPath, FileItemList *plist)
{
    QDir dir(dirPath);
    QStringList mtFiles = dir.entryList(QStringList("*.mt"));

    if (mtFiles.size() > 0)
    {
        // add all files found
        for (int i=0; i<mtFiles.size(); i++)
        {
            PFileItem item(new FileItem);
            item->folderName = dir.dirName();
            item->fileName = mtFiles[i];
            item->fullPathMt = UtlQt::pathCombine(dir.absolutePath(), mtFiles[i]);

            plist->push_back(item);
        }
    }

    QString curDir = QDir::cleanPath(dirPath);

    // find any sub directories and recurse
    QFileInfoList subDirs = dir.entryInfoList(QDir::AllDirs);
    for (int i=0; i<subDirs.size(); i++)
    {
        if (!subDirs[i].isDir()) continue;

        QString name = subDirs[i].fileName();
        if (name == "." || name == "..")
        {
            continue;
        }

        QString subDir = subDirs[i].absoluteFilePath();
        if (QDir::cleanPath(subDir) == curDir)
        {
            continue;
        }

        findFiles(subDir, plist);
    }
}

//=======================================================================
//=======================================================================
bool UtlMtFiles::updateFile(const QString &filePath, QString *pErr)
{
    const char *func = "UtlMtFiles::updateFile() -";
    QString err;

    int ver = RangeImage::getFileVersion(filePath);
    if (ver == RangeImage::getCurrentFileVersion())
    {
        err = filePath + " already at current version";
        LogError("%s %s", func, err.toStdString().c_str());
        if (pErr) *pErr += err;

        return false;
    }

    if (ver > RangeImage::getCurrentFileVersion())
    {
        err = filePath + " already at current version";
        LogError("%s %s", func, err.toStdString().c_str());
        if (pErr) *pErr += err;

        return false;
    }

    RangeImage ri(filePath);
    if (ri.isNull())
    {
        err = filePath + " failed to load.";
        LogError("%s %s", func, err.toStdString().c_str());
        if (pErr) *pErr += err;

        return false;
    }

    // save a backup of old file version
    QString sver = QString::number(ver);
    QString filePathBak = filePath + sver;
    if (!QFile::copy(filePath, filePathBak))
    {
        err = filePath + QString(" failed to save backup to ") + filePathBak;
        LogError("%s %s", func, err.toStdString().c_str());
        if (pErr) *pErr += err;

        return false;
    }

    // save the new version
    if (!ri.save(filePath))
    {
        err = filePath + " failed to save to new version!";
        LogError("%s %s", func, err.toStdString().c_str());
        if (pErr) *pErr += err;

        return false;
    }

    return true;
}

//=======================================================================
//=======================================================================
bool UtlMtFiles::isTipFile(const QString &filePath)
{
    QFileInfo fi(filePath);
    QString filename = fi.fileName();
    if (filename.length() <= 0) return false;
    if (filename.at(0) == 't' || filename.at(0) == 'T') return true;

    return false;
}
