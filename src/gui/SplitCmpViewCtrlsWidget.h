#ifndef SPLITCMPVIEWCTRLSWIDGET_H
#define SPLITCMPVIEWCTRLSWIDGET_H

#include <QWidget>
#include <QUuid>
#include <QVector3D>
#include "../core/box.h"

class QMdiSubWindow;
class QMdiSplitCmpWnd2;
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

    void refreshGui(QMdiSplitCmpWnd2* wnd);

    bool getViewsLinked();
    bool linkedZoom(bool viewsLinked=true);
    bool linkedAng(bool viewsLinked=true);
    bool linkedPos(bool viewsLinked=true);

public slots:
    void onSubWndActivated(QMdiSubWindow *wnd);



    void slotChangedTranslationMouse(int viewer, const QVector3D &v);
    void slotChangedRotationMouse(int viewer, const QVector3D &v);

    /*
    void onTextEditedLightDirX(const QString &text);
    void onTextEditedLightDirY(const QString &text);
    void onTextEditedLightDirZ(const QString &text);
    void onTextEditedLightShine(const QString &text);
    void onClickLightAmb();
    void onClickLightDif();
    void onClickLightSpe();
    void onClickLightDef();
    */

protected slots:
    void onLinkViewsClicked();
    void onLinkViewOptToggledZoom(bool checked);
    void onLinkViewOptToggledAngles(bool checked);
    void onLinkViewOptToggledPos(bool checked);
    void onClickDefaultView();

    void onClickScreenShot();
    void onClickMark();
    void onClickMarkOptimize();
    void onClickMarkSettings();
    void onClickProfileUpdate();

    void onValueChangedYaw(int value);
    void onValueChangedPitch(int value);
    void onValueChangedRoll(int value);

    void onTextEditedYaw(const QString &text);
    void onTextEditedPitch(const QString &text);
    void onTextEditedRoll(const QString &text);

    void onSliderReleasedYaw();
    void onSliderReleasedPitch();
    void onSliderReleasedRoll();

    void onValueChangedPanH(int value);
    void onValueChangedPanV(int value);
    void onValueChangedZoom(int value);

signals:
    void updateMark();
    void onZoomUpdated(int viewer, double zoom);
    void onDefaultView();

protected:
    void makeConnections();

    void refreshMarkStatus(QMdiSplitCmpWnd2 *wnd);

    void onEmitUpdateMark();
    void onSliderReleasedRotation();

    /*
    void setLightingDefault(bool updateRenderer);
    void setColorBtnStyle(QPushButton *btn, const QColor &c);
    void setColorBtnStyle(QPushButton *btn, const char *rgb);
    void setColorBtnStyle(QPushButton *btn, const QString &rgb);
    void updateEdit(QLineEdit *edit, double d);
    void updateEdit(QLineEdit *edit, int i);
    void updateEdit(QLineEdit *edit, const char *v);
    */

    void updateYawSlider(double yaw);
    void updatePitchSlider(double pitch);
    void updateRollSlider(double roll);
    void updatePanAndZoomSlider(const QVector3D &trans);

    void updateEditYawPitchRoll(QLineEdit *edit, double yaw);

    double getPercentRot(double deg);
    double getPercentPanH(double pan, double min, double max);
    double getPercentPanV(double pan, double min, double max);
    double getPercentZoom(double zoom, double min, double max);

    QVector3D getColor(const QColor &c);
    QColor getColor(const QVector3D &v);

    void setViewLinkMode(int mode, bool enable, QMdiSplitCmpWnd2 *wnd=false);
    
protected:
    Investigator *_pi;
    box3d _boxTrans;

private:
    Ui::SplitCmpViewCtrlsWidget *ui;

    QAction *_linkViewOptZoom;
    QAction *_linkViewOptAngles;
    QAction *_linkViewOptPos;

    QAction *_markOptSettings;

};

#endif // SPLITCMPVIEWCTRLSWIDGET_H
