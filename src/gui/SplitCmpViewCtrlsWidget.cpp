/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 *
 * This computer software was prepared by The Ames
 * Laboratory, hereinafter the Contractor, under
 * Interagency Agreement number 2009-DN-R-119 between
 * the National Institute of Justice (NIJ) and the
 * Department of Energy (DOE). All rights in the computer
 * software are reserved by NIJ/DOE on behalf of the
 * United States Government and the Contractor as provided
 * in its Contract, DE-AC02-07CH11358.  You are authorized
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY,
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE
 * OF THIS SOFTWARE.
 *
 * This notice including this sentence
 * must appear on any copies of this computer software.
 *
 * Author: Brian Bailey (bbailey@code-hammer.com)
 */

#include "SplitCmpViewCtrlsWidget.h"
#include "ui_SplitCmpViewCtrlsWidget.h"
#include "../core/logger.h"
#include "QMdiSplitCmpWnd.h"
#include "Investigator.h"

double getNormalizedDeg(double deg)
{
    double div = deg / 360.0;
    if (div <= 1) return deg;

    int imul = (int)div;
    return deg - 360*imul;
}

double clip(double v, double min=0, double max=1)
{
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

//=======================================================================
//=======================================================================
SplitCmpViewCtrlsWidget::SplitCmpViewCtrlsWidget(Investigator *pi, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplitCmpViewCtrlsWidget),
    _pi(pi),
    _boxTrans(vec3d(-8000, -8000, -9500), vec3d(8000, 8000, 9500))
{             
    ui->setupUi(this);

    ui->horizontalSliderYaw->setMinimum(0);
    ui->horizontalSliderYaw->setMaximum(100);
    ui->verticalSliderPitch->setMinimum(0);
    ui->verticalSliderPitch->setMaximum(100);
    ui->horizontalSliderPanH->setMinimum(0);
    ui->horizontalSliderPanH->setMaximum(100);
    ui->verticalSliderPanV->setMinimum(0);
    ui->verticalSliderPanV->setMaximum(100);
    ui->verticalSliderZoom->setMinimum(0);
    ui->verticalSliderZoom->setMaximum(100);

    setLightingDefault(false);

    makeConnections();
}

//=======================================================================
//=======================================================================
SplitCmpViewCtrlsWidget::~SplitCmpViewCtrlsWidget()
{
    delete ui;
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::connectSplitCmp(QMdiSplitCmpWnd* wnd)
{
    connect(wnd, SIGNAL(onChangedTranslation(int, QVector3D)), this, SLOT(onChangedTranslation(int, QVector3D)));
    connect(wnd, SIGNAL(onChangedRotation(int, QVector3D)), this, SLOT(onChangedRotation(int, QVector3D)));
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::makeConnections()
{
    connect(_pi->getMdiArea(), SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(onSubWndActivated(QMdiSubWindow*)));

    connect(ui->pushButtonDefaultView, SIGNAL(clicked()), this, SLOT(onClickDefaultView()));
    connect(ui->checkBoxLinkViews, SIGNAL(clicked()), this, SLOT(onLinkViewsClicked()));

    connect(ui->horizontalSliderYaw, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedYaw(int)));
    connect(ui->verticalSliderPitch, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPitch(int)));
    //connect(ui->horizontalSliderRoll, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedRoll(int)));
    connect(ui->horizontalSliderPanH, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPanH(int)));
    connect(ui->verticalSliderPanV, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPanV(int)));
    connect(ui->verticalSliderZoom, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedZoom(int)));

    connect(ui->editLightDirX, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirX(QString)));
    connect(ui->editLightDirY, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirY(QString)));
    connect(ui->editLightDirZ, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirZ(QString)));
    connect(ui->editLightShine, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightShine(QString)));
    connect(ui->btnLightAmb, SIGNAL(clicked()), this, SLOT(onClickLightAmb()));
    connect(ui->btnLightDif, SIGNAL(clicked()), this, SLOT(onClickLightDif()));
    connect(ui->btnLightSpe, SIGNAL(clicked()), this, SLOT(onClickLightSpe()));
    connect(ui->btnLightDefaults, SIGNAL(clicked()), this, SLOT(onClickLightDef()));
}


//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::setColorBtnStyle(QPushButton *btn, const QColor &c)
{
    char acfrmt[256];
    sprintf(acfrmt, "rgb(%d,%d,%d)", c.red(), c.green(), c.blue());
    QString rgb = acfrmt;

    setColorBtnStyle(btn, rgb);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::setColorBtnStyle(QPushButton *btn, const char *rgb)
{
    setColorBtnStyle(btn, QString(rgb));
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::setColorBtnStyle(QPushButton *btn, const QString &rgb)
{
    QString style =  "QPushButton  { border: 2px solid #8f8f91; border-radius: 6px;";
    style += "background-color: ";
    style += rgb;
    style += ";}";

    style += "QPushButton:pressed   {background-color: ";
    style += rgb;
    style += ";";
    style += "border: 2px solid #707070;";
    style += "}";

    btn->setStyleSheet(style);

    //min-width: 80px;
    // regular then pressed
    // background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);
    // background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::setLightingDefault(bool updateRenderer)
{
    updateEdit(ui->editLightDirX, "-10");
    updateEdit(ui->editLightDirY, "-5");
    updateEdit(ui->editLightDirZ, "10");
    updateEdit(ui->editLightShine, "5.0");


    setColorBtnStyle(ui->btnLightAmb, "rgb(255,255,255)");
    setColorBtnStyle(ui->btnLightDif, "rgb(255,255,255)");
    setColorBtnStyle(ui->btnLightSpe, "rgb(255,255,255)");

    if (!updateRenderer) return;

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    wnd->setDefaultLight(getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::refreshGui(QMdiSplitCmpWnd* wnd)
{
    if (!wnd) return;

    GraphicsWidget *gw =  wnd->getSelectedGraphics();
    if (!gw) return;

    updateYawSlider(gw->getYaw());
    updatePitchSlider(gw->getPitch());
    updatePanAndZoomSlider(gw->getTrans());

}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateEdit(QLineEdit *edit, double d)
{
    char ac[32];
    sprintf(ac, "%.2f", d);

    updateEdit(edit, ac);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateEdit(QLineEdit *edit, const char *v)
{
    bool b = edit->blockSignals(true);
    edit->setText(v);
    edit->blockSignals(b);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateSlider(QSlider *slider, int pos)
{
    bool b = slider->blockSignals(true);
    slider->setSliderPosition(pos);
    slider->blockSignals(b);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateYawSlider(double yaw)
{
    yaw = getNormalizedDeg(yaw);
    int yawp = (int)(getPercentRot(yaw) * 100.0);

    updateSlider(ui->horizontalSliderYaw, yawp);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updatePitchSlider(double pitch)
{
    pitch = getNormalizedDeg(pitch);
    int pitp = (int)(getPercentRot(pitch) * 100.0);

    updateSlider(ui->verticalSliderPitch, pitp);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updatePanAndZoomSlider(const QVector3D &trans)
{
    int panh = (int)(getPercentPanH(trans.x(), _boxTrans.vmin.x, _boxTrans.vmax.x) * 100.0);
    int panv = (int)(getPercentPanV(trans.y(), _boxTrans.vmin.y, _boxTrans.vmax.y) * 100.0);
    int zoom = (int)(getPercentZoom(trans.z(), _boxTrans.vmin.z, _boxTrans.vmax.z) * 100.0);

    updateSlider(ui->horizontalSliderPanH, panh);
    updateSlider(ui->verticalSliderPanV, panv);
    updateSlider(ui->verticalSliderZoom, zoom);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSubWndActivated(QMdiSubWindow *wnd)
{
    QMdiSplitCmpWnd *subwin = dynamic_cast<QMdiSplitCmpWnd*>(wnd);
    if (!subwin)return;


}

//=======================================================================
//=======================================================================
bool SplitCmpViewCtrlsWidget::getViewsLinked()
{
    return ui->checkBoxLinkViews->isChecked();
}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentRot(double deg)
{
    return getNormalizedDeg(deg) / 360.0;
}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentPanH(double v, double min, double max)
{
    double lent = qAbs(max - min);
    double len = qAbs(v - min);
    double per = len/lent;
    return clip(per);

}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentPanV(double v, double min, double max)
{
    return getPercentPanH(v, min, max);
}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentZoom(double v, double min, double max)
{
    return getPercentPanH(v, min, max);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickDefaultView()
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setDefaultView(getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onLinkViewsClicked()
{
    if (!getViewsLinked()) return;

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->linkViews();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedYaw(int value)
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double yaw = ((double)value / 100.0) * 360;
    wnd->setYaw(yaw, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPitch(int value)
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double pitch = ((double)value / 100.0) * 360;
    wnd->setPitch(pitch, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedRoll(int value)
{

}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPanH(int value)
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.x + ((double)value / 100.0) * _boxTrans.getWidth();
    wnd->setPanH(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPanV(int value)
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.y + ((double)value / 100.0) * _boxTrans.getHeight();
    wnd->setPanV(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedZoom(int value)
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.z + ((double)value / 100.0) * _boxTrans.getDepth();
    wnd->setZoom(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onChangedTranslation(int viewer, QVector3D trans)
{
    //LogTrace("onChangedTrans: %.2f, %.2f, %.2f", trans.x(), trans.y(), trans.z());

    if (getViewsLinked())
    {
        QMdiSplitCmpWnd *wnd = dynamic_cast<QMdiSplitCmpWnd*>(sender());
        if (wnd)
        {
            int lnkview = 1;
            if (viewer > 0) lnkview = 0;
            wnd->getGraphics(lnkview)->setTrans(trans);
        }
    }

    updatePanAndZoomSlider(trans);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onChangedRotation(int viewer, QVector3D rot)
{
   // LogTrace("onChangedRot: %.2f, %.2f, %.2f", rot.x(), rot.y(), rot.z());

    if (getViewsLinked())
    {
        QMdiSplitCmpWnd *wnd = dynamic_cast<QMdiSplitCmpWnd*>(sender());
        if (wnd)
        {
            int lnkview = 1;
            if (viewer > 0) lnkview = 0;
            wnd->getGraphics(lnkview)->setRot(rot);
        }
    }

    updateYawSlider(rot.y());
    updatePitchSlider(rot.x());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedLightDirX(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().org;
    v.setX(f);
    wnd->setLightOrigin(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedLightDirY(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().org;
    v.setY(f);
    wnd->setLightOrigin(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedLightDirZ(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().org;
    v.setZ(f);
    wnd->setLightOrigin(v, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedLightShine(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setLightShine(f, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightAmb()
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D amb = wnd->getSelectedRenderer()->getLightInfo().amb;
    QColor c = QColorDialog::getColor(getColor(amb));

    amb = getColor(c);
    wnd->setLightAmb(amb, getViewsLinked());

    setColorBtnStyle(ui->btnLightAmb, c);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightDif()
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().dif;
    QColor c = QColorDialog::getColor(getColor(v));

    v = getColor(c);
    wnd->setLightDif(v, getViewsLinked());

    setColorBtnStyle(ui->btnLightDif, c);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightSpe()
{
    QMdiSplitCmpWnd *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().spe;
    QColor c = QColorDialog::getColor(getColor(v));

    v = getColor(c);
    wnd->setLightSpe(v, getViewsLinked());

    setColorBtnStyle(ui->btnLightSpe, c);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightDef()
{
    setLightingDefault(true);
}

//=======================================================================
//=======================================================================
QVector3D SplitCmpViewCtrlsWidget::getColor(const QColor &c)
{
    return QVector3D((double)c.red()/255.0, (double)c.green()/255.0, (double)c.blue()/255.0);
}

//=======================================================================
//=======================================================================
QColor SplitCmpViewCtrlsWidget::getColor(const QVector3D &v)
{
    double x = clip(v.x(), 0, 1);
    double y = clip(v.x(), 0, 1);
    double z = clip(v.y(), 0, 1);


    return QColor((int)(x*255.0), (int)(y*255.0), (int)(z*255.0), 255);
}
