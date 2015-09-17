#ifndef UTLQT_H
#define UTLQT_H

#include <QString>

class UtlQt
{
public:

    static bool fileExists(const char *path);
    static bool fileExists(const QString &path);
    static QString fileName(const char *fullpath);
    static QString fileName(const QString &fullpath);
    static QString fileNameWithExt(const QString &fullpath);
    static QString filePath(const QString &fullpath);
    static bool validateDir(const char *dir, bool autoCreate=true);
    static bool validateDir(const QString &dir, bool autoCreate=true);
    static QString pathCombine(const QString &path1, const QString &path2);

    static void delay(int secs);
};

#endif
