#ifndef SPLITCMPTHUMBLOADERWIDGET_H
#define SPLITCMPTHUMBLOADERWIDGET_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class SplitCmpThumbLoaderWidget;
}

class SplitCmpThumbLoaderWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SplitCmpThumbLoaderWidget(QWidget *parent = 0);
    ~SplitCmpThumbLoaderWidget();

    bool setProjectFolder(const QString &dirdirPath);

    QListWidget* getListWidgetThumbs();

protected:
    struct FileItem
    {
        QString folderName;
        QString fullPathIcon;
        QString fullPathMt;
    };
    typedef QSharedPointer<FileItem> PFileItem;
    typedef std::vector<PFileItem> FileItemList;


    void findFiles(const QString &dirPath, FileItemList *plist);

private:
    Ui::SplitCmpThumbLoaderWidget *ui;
};

#endif // SPLITCMPTHUMBLOADERWIDGET_H
