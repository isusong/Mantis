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

    bool setProjectFolder(const QString &dirPath);

    QListWidget* getListWidgetThumbs();

    bool validateInsert(const QString &filepath);

public slots:

signals:

protected:
    void loadItem(const QString &fileName, const QString &fullpathMt);

protected:
    QString _dirPath;

private:
    Ui::SplitCmpThumbLoaderWidget *ui;
};

#endif // SPLITCMPTHUMBLOADERWIDGET_H
