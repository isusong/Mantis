#ifndef UTLMTFILES_H
#define UTLMTFILES_H

#include <QString>
#include <vector>
#include <QSharedPointer>

class UtlMtFiles
{
public:
    struct FileItem
    {
        QString fileName;
        QString folderName;
        QString fullPathMt;
    };
    typedef QSharedPointer<FileItem> PFileItem;
    typedef std::vector<PFileItem> FileItemList;

public:
    UtlMtFiles() {}

    static void findFiles(const QString &dirPath, FileItemList *plist);
    static bool updateFile(const QString &filePath, QString *pErr);

    static bool isTipFile(const QString &filePath);
};

#endif
