#ifndef SPLITCMPVIEWCTRLSWIDGET_H
#define SPLITCMPVIEWCTRLSWIDGET_H

#include <QWidget>
#include <QUuid>
#include <QVector3D>
#include "../core/box.h"

class QMdiSubWindow;
class QMdiSplitCmpWnd;
class Investigator;
class QSlider;
class QPushButton;
class QLineEdit;

namespace Ui {
class SplitCmpViewCtrlsWidget;
}

class SplitCmpViewCtrlsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SplitCmpViewCtrlsWidget(Investigator *pi, QWidget *parent = 0);
    ~SplitCmpViewCtrlsWidget();

    void connectSplitCmp(QMdiSplitCmpWnd* wnd);
    void refreshGui(QMdiSplitCmpWnd* wnd);

public slots:
    void onSubWndActivated(QMdiSubWindow *wnd);

    void onClickDefaultView();
    void onLinkViewsClicked();

    void onValueChangedYaw(int value);
    void onValueChangedPitch(int value);
    void onValueChangedRoll(int value);
    void onValueChangedPanH(int value);
    void onValueChangedPanV(int value);
    void onValueChangedZoom(int value);

    void onChangedTranslation(int viewer, QVector3D trans);
    void onChangedRotation(int viewer, QVector3D rot);

    void onTextEditedLightDirX(const QString &text);
    void onTextEditedLightDirY(const QString &text);
    void onTextEditedLightDirZ(const QString &text);
    void onTextEditedLightShine(const QString &text);
    void onClickLightAmb();
    void onClickLightDif();
    void onClickLightSpe();
    void onClickLightDef();

protected:
    void makeConnections();
    bool getViewsLinked();

    void setLightingDefault(bool updateRenderer);
    void setColorBtnStyle(QPushButton *btn, const QColor &c);
    void setColorBtnStyle(QPushButton *btn, const char *rgb);
    void setColorBtnStyle(QPushButton *btn, const QString &rgb);
    void updateEdit(QLineEdit *edit, double d);
    void updateEdit(QLineEdit *edit, const char *v);

    void updateSlider(QSlider *slider, int pos);
    void updateYawSlider(double yaw);
    void updatePitchSlider(double pitch);
    void updatePanAndZoomSlider(const QVector3D &trans);

    double getPercentRot(double deg);
    double getPercentPanH(double pan, double min, double max);
    double getPercentPanV(double pan, double min, double max);
    double getPercentZoom(double zoom, double min, double max);

    QVector3D getColor(const QColor &c);
    QColor getColor(const QVector3D &v);
    
protected:
    Investigator *_pi;
    box3d _boxTrans;

private:
    Ui::SplitCmpViewCtrlsWidget *ui;

};

#endif // SPLITCMPVIEWCTRLSWIDGET_H
