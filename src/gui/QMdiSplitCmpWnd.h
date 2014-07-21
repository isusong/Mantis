#ifndef QMDISPLITCMPWND_H
#define QMDISPLITCMPWND_H

#include <QMdiSubWindow>
#include <QSplitter>
#include "RangeImageViewer.h"

class SplitCmpViewCtrlsWidget;

class QMdiSplitCmpWnd : public QMdiSubWindow
{
   Q_OBJECT

public:
    QMdiSplitCmpWnd(SplitCmpViewCtrlsWidget *pvc, QWidget * parent = 0, Qt::WindowFlags flags = 0);

    void init();

    bool loadRangeImg(const QString &filename);
    bool loadRangeImg(const QString &filename, int viewer);
    void setRangeImg(PRangeImage img, int viewer);

    const QUuid& getGuid() { return _guid; }

    int getSelectedView();
    GraphicsWidget* getSelectedGraphics();
    GraphicsWidget* getGraphics(int iviewer=0);
    RangeImageRenderer* getSelectedRenderer();
    RangeImageRenderer* getRenderer(int iviewer=0);

    void linkViews();
    void setDefaultView(bool linkViews);
    void setYaw(double deg, bool linkViews);
    void setPitch(double deg, bool linkViews);
    void setPanH(double v, bool linkViews);
    void setPanV(double v, bool linkViews);
    void setZoom(double v, bool linkViews);

    void setDefaultLight(bool linkViews);
    void setLightOrigin(const QVector3D& newOrigin, bool linkViews);
    void setLightAmb(const QVector3D& v, bool linkViews);
    void setLightDif(const QVector3D& v, bool linkViews);
    void setLightSpe(const QVector3D& v, bool linkViews);
    void setLightShine(float s, bool linkViews);


public slots:
    void onLButtonDownViewer0(int x, int y);
    void onLButtonDownViewer1(int x, int y);

    void onTranslationViewer0(QVector3D trans);
    void onRotationViewer0(QVector3D rot);
    void onTranslationViewer1(QVector3D trans);
    void onRotationViewer1(QVector3D rot);

signals:
    void onChangedTranslation(int viewer, QVector3D trans);
    void onChangedRotation(int viewer, QVector3D rot);
    void onClosed();


protected:

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void closeEvent(QCloseEvent *closeEvent);
    void makeConnections();

protected:

    QUuid _guid;
    QSplitter* _splitter;
    RangeImageViewer* _imgViewer0;
    RangeImageViewer* _imgViewer1;

    SplitCmpViewCtrlsWidget *_pvc;

};

#endif // QMDISPLITCMPWND_H
