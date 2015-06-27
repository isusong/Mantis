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
#include "DlgMarkOptSettings.h"
#include "UtlQtGui.h"
#include "../core/UtlMath.h"

#include <QMessageBox>


//=======================================================================
//=======================================================================
SplitCmpViewCtrlsWidget::SplitCmpViewCtrlsWidget(Investigator *pi, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplitCmpViewCtrlsWidget),
    _pi(pi),
    _boxTrans(vec3d(-8000, -8000, -9500), vec3d(8000, 8000, 9500))
{             
    ui->setupUi(this);

    ui->horizontalSliderYaw->setMinimum(-180);
    ui->horizontalSliderYaw->setMaximum(180);
    ui->verticalSliderPitch->setMinimum(-180);
    ui->verticalSliderPitch->setMaximum(180);
    ui->horizontalSliderRoll->setMinimum(-180);
    ui->horizontalSliderRoll->setMaximum(180);

    ui->lineEditYaw->setValidator( new QIntValidator(-180, 180, this) );
    ui->lineEditPitch->setValidator( new QIntValidator(-180, 180, this) );
    ui->lineEditRoll->setValidator( new QIntValidator(-180, 180, this) );

    ui->lineEditYaw->setText(QString("0"));
    ui->lineEditPitch->setText(QString("0"));
    ui->lineEditRoll->setText(QString("0"));

    ui->horizontalSliderPanH->setMinimum(0);
    ui->horizontalSliderPanH->setMaximum(100);
    ui->verticalSliderPanV->setMinimum(0);
    ui->verticalSliderPanV->setMaximum(100);
    ui->verticalSliderZoom->setMinimum(0);
    ui->verticalSliderZoom->setMaximum(100);

    //initialize link view menu options
    QMenu *menu = new QMenu();

    _linkViewOptZoom = new QAction("Zoom", this);
    _linkViewOptZoom->setCheckable(true);
    _linkViewOptZoom->setChecked(true);
    menu->addAction(_linkViewOptZoom);

    _linkViewOptAngles = new QAction("Angles", this);
    _linkViewOptAngles->setCheckable(true);
    _linkViewOptAngles->setChecked(true);
    menu->addAction(_linkViewOptAngles);

    _linkViewOptPos = new QAction("Position", this);
    _linkViewOptPos->setCheckable(true);
    _linkViewOptPos->setChecked(true);
    menu->addAction(_linkViewOptPos);

    ui->toolBtnLinkViews->setMenu(menu);
    ui->toolBtnLinkViews->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBtnLinkViews->setCheckable(true);
    ui->toolBtnLinkViews->setChecked(true);
    ui->toolBtnLinkViews->setText("Linked Views On");


    // init mark stuff
    ui->labelInfo->setText("Mark Mode Off");

    menu = new QMenu();
    _markOptSettings = new QAction("settings", this);
    menu->addAction(_markOptSettings);
    ui->toolBtnOptimizeMark->setMenu(menu);
    ui->toolBtnOptimizeMark->setPopupMode(QToolButton::MenuButtonPopup);


    //setLightingDefault(false);

    makeConnections();
}

//=======================================================================
//=======================================================================
SplitCmpViewCtrlsWidget::~SplitCmpViewCtrlsWidget()
{
    delete ui;
}

/*
//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::connectSplitCmp(QMdiSplitCmpWnd* wnd)
{
    connect(wnd, SIGNAL(onChangedTranslation(int, QVector3D)), this, SLOT(onChangedTranslation(int, QVector3D)));
    connect(wnd, SIGNAL(onChangedRotation(int, QVector3D)), this, SLOT(onChangedRotation(int, QVector3D)));
}
*/
//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::makeConnections()
{
    bool ret = false;
    ret = connect(_pi->getMdiArea(), SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(onSubWndActivated(QMdiSubWindow*)));


    ret = connect(ui->toolBtnLinkViews, SIGNAL(clicked()), this, SLOT(onLinkViewsClicked()));
    ret = connect(_linkViewOptZoom, SIGNAL(toggled(bool)), this, SLOT(onLinkViewOptToggledZoom(bool)));
    ret = connect(_linkViewOptAngles, SIGNAL(toggled(bool)), this, SLOT(onLinkViewOptToggledAngles(bool)));
    ret = connect(_linkViewOptPos, SIGNAL(toggled(bool)), this, SLOT(onLinkViewOptToggledPos(bool)));


    //ret = connect(ui->checkBoxLinkZoom, SIGNAL(clicked()), this, SLOT(onLinkZoomClicked()));
    ret = connect(ui->pushButtonDefaultView, SIGNAL(clicked()), this, SLOT(onClickDefaultView()));
    ret = connect(ui->pushButtonScreenShot, SIGNAL(clicked()), this, SLOT(onClickScreenShot()));
    ret = connect(ui->pushButtonMark, SIGNAL(clicked()), this, SLOT(onClickMark()));
    ret = connect(ui->toolBtnOptimizeMark, SIGNAL(clicked()), this, SLOT(onClickMarkOptimize()));
    ret = connect(_markOptSettings, SIGNAL(triggered()), this, SLOT(onClickMarkSettings()));
    ret = connect(ui->pushButtonProfileUpdate, SIGNAL(clicked()), this, SLOT(onClickProfileUpdate()));


    ret = connect(ui->horizontalSliderYaw, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedYaw(int)));
    ret = connect(ui->verticalSliderPitch, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPitch(int)));
    ret = connect(ui->horizontalSliderRoll, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedRoll(int)));

    ret = connect(ui->lineEditYaw, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedYaw(QString)));
    ret = connect(ui->lineEditPitch, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedPitch(QString)));
    ret = connect(ui->lineEditRoll, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedRoll(QString)));

    ret = connect(ui->horizontalSliderYaw, SIGNAL(sliderReleased()), this, SLOT(onSliderReleasedYaw()));
    ret = connect(ui->verticalSliderPitch, SIGNAL(sliderReleased()), this, SLOT(onSliderReleasedPitch()));
    ret = connect(ui->horizontalSliderRoll, SIGNAL(sliderReleased()), this, SLOT(onSliderReleasedRoll()));


    ret = connect(ui->horizontalSliderPanH, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPanH(int)));
    ret = connect(ui->verticalSliderPanV, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedPanV(int)));
    ret = connect(ui->verticalSliderZoom, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedZoom(int)));

    /*
    ret = connect(ui->editLightDirX, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirX(QString)));
    ret = connect(ui->editLightDirY, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirY(QString)));
    ret = connect(ui->editLightDirZ, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirZ(QString)));
    ret = connect(ui->editLightShine, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightShine(QString)));
    ret = connect(ui->btnLightAmb, SIGNAL(clicked()), this, SLOT(onClickLightAmb()));
    ret = connect(ui->btnLightDif, SIGNAL(clicked()), this, SLOT(onClickLightDif()));
    ret = connect(ui->btnLightSpe, SIGNAL(clicked()), this, SLOT(onClickLightSpe()));
    ret = connect(ui->btnLightDefaults, SIGNAL(clicked()), this, SLOT(onClickLightDef()));
    */
}

/*
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

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    wnd->setDefaultLight(getViewsLinked());
}
*/

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::refreshGui(QMdiSplitCmpWnd2* wnd)
{
    if (!wnd) return;

    wnd->linkViews(getViewsLinked());
    setViewLinkMode(GraphicsWidget2::LinkZoom, linkedZoom(false), wnd);
    setViewLinkMode(GraphicsWidget2::LinkAngles, linkedAng(false), wnd);
    setViewLinkMode(GraphicsWidget2::LinkPosition, linkedPos(false), wnd);

    GraphicsWidget2 *gw = wnd->getGraphics();
    if (!gw) return;
    if (!gw->getModelDataSel()->model) return;

    int m = gw->getSelectedWindow();
    updateYawSlider(gw->getYaw(m));
    updatePitchSlider(gw->getPitch(m));
    updateRollSlider(gw->getRoll(m));
    updatePanAndZoomSlider(gw->getTrans(m));
    updateEditYawPitchRoll(ui->lineEditYaw, gw->getYaw(m));
    updateEditYawPitchRoll(ui->lineEditPitch, gw->getPitch(m));
    updateEditYawPitchRoll(ui->lineEditRoll, gw->getRoll(m));

    refreshMarkStatus(wnd);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::refreshMarkStatus(QMdiSplitCmpWnd2 *wnd)
{
    if (wnd->markAvailable())
    {
        ui->pushButtonMark->setEnabled(true);
        ui->toolBtnOptimizeMark->setEnabled(true);
    }
    else
    {
        ui->pushButtonMark->setEnabled(false);
        ui->toolBtnOptimizeMark->setEnabled(false);
    }

    if (wnd->getMarkMode())
    {
        ui->labelInfo->setText("Mark Mode On");
        ui->pushButtonMark->setText("Mark Off");
    }
    else
    {
        ui->labelInfo->setText("Mark Mode Off");
        ui->pushButtonMark->setText("Mark On");
    }
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateYawSlider(double yaw)
{
    /*
    yaw = getNormalizedDeg(yaw);
    int yawp = (int)(getPercentRot(yaw) * 100.0);
    updateSlider(ui->horizontalSliderYaw, yawp);
    */

    int degp = (int)UtlMath::getNormalizedDeg180(yaw);
    UtlQtGui::updateSlider(ui->horizontalSliderYaw, degp);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updatePitchSlider(double pitch)
{
    /*
    pitch = getNormalizedDeg(pitch);
    int pitp = (int)(getPercentRot(pitch) * 100.0);
    updateSlider(ui->verticalSliderPitch, pitp);
    */

    int degp = (int)UtlMath::getNormalizedDeg180(pitch);
    UtlQtGui::updateSlider(ui->verticalSliderPitch, degp);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateRollSlider(double roll)
{
    /*
    roll = getNormalizedDeg(roll);
    int rollp = (int)(getPercentRot(roll) * 100.0);
    updateSlider(ui->horizontalSliderRoll, rollp);
    */

    int degp = (int)UtlMath::getNormalizedDeg180(roll);
    UtlQtGui::updateSlider(ui->horizontalSliderRoll, degp);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updateEditYawPitchRoll(QLineEdit *edit, double deg)
{
    int ideg = (int)UtlMath::getNormalizedDeg180(deg);
    UtlQtGui::updateEdit(edit, ideg);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onEmitUpdateMark()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    if (!wnd->getMarkMode()) return;

    emit updateMark();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSliderReleasedRotation()
{
    if (!_pi->autoUpdateStatsRT()) return;

    onEmitUpdateMark();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSliderReleasedYaw()
{
    onSliderReleasedRotation();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSliderReleasedPitch()
{
   onSliderReleasedRotation();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSliderReleasedRoll()
{
    onSliderReleasedRotation();
}


//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::updatePanAndZoomSlider(const QVector3D &trans)
{
    int panh = (int)(getPercentPanH(trans.x(), _boxTrans.vmin.x, _boxTrans.vmax.x) * 100.0);
    int panv = (int)(getPercentPanV(trans.y(), _boxTrans.vmin.y, _boxTrans.vmax.y) * 100.0);
    int zoom = (int)(getPercentZoom(trans.z(), _boxTrans.vmin.z, _boxTrans.vmax.z) * 100.0);

    UtlQtGui::updateSlider(ui->horizontalSliderPanH, panh);
    UtlQtGui::updateSlider(ui->verticalSliderPanV, panv);
    UtlQtGui::updateSlider(ui->verticalSliderZoom, zoom);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onSubWndActivated(QMdiSubWindow *wnd)
{
    QMdiSplitCmpWnd2 *subwin = dynamic_cast<QMdiSplitCmpWnd2*>(wnd);
    if (!subwin)return;


}

//=======================================================================
//=======================================================================
bool SplitCmpViewCtrlsWidget::getViewsLinked()
{
    return ui->toolBtnLinkViews->isChecked();
}

//=======================================================================
//=======================================================================
bool SplitCmpViewCtrlsWidget::linkedZoom(bool viewsLinked)
{
    if (viewsLinked && !getViewsLinked()) return false;
    return _linkViewOptZoom->isChecked();
}

//=======================================================================
//=======================================================================
bool SplitCmpViewCtrlsWidget::linkedAng(bool viewsLinked)
{
    if (viewsLinked && !getViewsLinked()) return false;
    return _linkViewOptAngles->isChecked();
}

//=======================================================================
//=======================================================================
bool SplitCmpViewCtrlsWidget::linkedPos(bool viewsLinked)
{
    if (viewsLinked && !getViewsLinked()) return false;
    return _linkViewOptPos->isChecked();
}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentRot(double deg)
{
    return UtlMath::getNormalizedDeg(deg) / 360.0;
}

//=======================================================================
//=======================================================================
double SplitCmpViewCtrlsWidget::getPercentPanH(double v, double min, double max)
{
    v = UtlMath::clip(v, min, max);
    double lent = qAbs(max - min);
    double len = qAbs(v - min);
    double per = len/lent;
    return UtlMath::clip(per);
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
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setDefaultView();
    emit onDefaultView();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickScreenShot()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd)
    {
        QMessageBox::warning(this, tr("Error"), tr("No Split Compare Window."));
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Screen Shot"), "", tr("Png (*.png);;Jpg (*.jpg)"));
    if (filename.isEmpty()) return;

    if (!wnd->getGraphics()->screenShot(filename))
    {
        QMessageBox::warning(this, tr("Error"), tr("Failed to save screen shot to: ") + filename);
    }
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickMark()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    if (!wnd->markAvailable()) return;

    if (wnd->getMarkMode())
    {
        wnd->setMarkMode(false);
    }
    else
    {
        wnd->setMarkMode(true);
    }

    refreshMarkStatus(wnd);


    //onEmitUpdateMark();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickMarkOptimize()
{
    RangeImageRenderer *tip = NULL;
    RangeImageRenderer *plate = NULL;
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    if (!wnd->markAvailable(&tip, &plate)) return;
    if (!tip || !plate) return;

    PProfile proPlate = plate->getProfilePlate();
    if (!proPlate)
    {
        QMessageBox::warning(this, tr("Plate Error"), tr("No Profile for the current plate. Try moving the splitter."));
        return;
    }

    _pi->runMarkOptimization(tip, proPlate);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickMarkSettings()
{
    DlgMarkOptSettings settings;
    settings.exec();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickProfileUpdate()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->updateProfilesAndStats();
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onLinkViewsClicked()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    bool linkedViews = getViewsLinked();
    wnd->linkViews(linkedViews);

    if (linkedViews)
    {
        ui->toolBtnLinkViews->setText(tr("Linked Views On"));
    }
    else
    {
        ui->toolBtnLinkViews->setText(tr("Linked Views Off"));
    }
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::setViewLinkMode(int mode, bool enable, QMdiSplitCmpWnd2 *wnd)
{
    if (!wnd) wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    GraphicsWidget2::LinkView lvmode = (GraphicsWidget2::LinkView)mode;
    if (enable)
    {
        wnd->enableViewLink(lvmode);
    }
    else
    {
        wnd->disableViewLink(lvmode);
    }
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onLinkViewOptToggledZoom(bool checked)
{
    Q_UNUSED(checked);
    setViewLinkMode(GraphicsWidget2::LinkZoom, _linkViewOptZoom->isChecked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onLinkViewOptToggledAngles(bool checked)
{
    Q_UNUSED(checked);
    setViewLinkMode(GraphicsWidget2::LinkAngles, _linkViewOptAngles->isChecked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onLinkViewOptToggledPos(bool checked)
{
    Q_UNUSED(checked);
    setViewLinkMode(GraphicsWidget2::LinkPosition, _linkViewOptPos->isChecked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedYaw(int value)
{
    UtlQtGui::updateEdit(ui->lineEditYaw, value);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setYaw((double)value, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPitch(int value)
{
    UtlQtGui::updateEdit(ui->lineEditPitch, value);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setPitch((double)value, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedRoll(int value)
{
    UtlQtGui::updateEdit(ui->lineEditRoll, value);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    wnd->setRoll((double)value, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedYaw(const QString &text)
{
    double deg = (double)atoi(text.toStdString().c_str());
    updateYawSlider(deg);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setYaw(deg, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedPitch(const QString &text)
{
    double deg = (double)atoi(text.toStdString().c_str());
    updatePitchSlider(deg);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setPitch(deg, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onTextEditedRoll(const QString &text)
{
    double deg = (double)atoi(text.toStdString().c_str());
    updateRollSlider(deg);

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setRoll(deg, linkedAng());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPanH(int value)
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.x + ((double)value / 100.0) * _boxTrans.getWidth();
    wnd->setPanH(v, linkedPos());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedPanV(int value)
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.y + ((double)value / 100.0) * _boxTrans.getHeight();
    wnd->setPanV(v, linkedPos());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onValueChangedZoom(int value)
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    double v = _boxTrans.vmin.z + ((double)value / 100.0) * _boxTrans.getDepth();
    wnd->setZoom(v, linkedZoom());

    if (linkedZoom())
    {
        emit onZoomUpdated(QMdiSplitCmpWnd2::ViewLeft, v);
        emit onZoomUpdated(QMdiSplitCmpWnd2::ViewRight, v);
    }
    else
    {
        emit onZoomUpdated(wnd->getSelectedView(), v);
    }
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::slotChangedTranslationMouse(int viewer, const QVector3D &v)
{
    Q_UNUSED(viewer);
    updatePanAndZoomSlider(v);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::slotChangedRotationMouse(int viewer, const QVector3D &v)
{
    Q_UNUSED(viewer);

    updateYawSlider(v.y());
    updatePitchSlider(v.x());
    updateRollSlider(v.z());
    updateEditYawPitchRoll(ui->lineEditYaw, v.y());
    updateEditYawPitchRoll(ui->lineEditPitch, v.x());
    updateEditYawPitchRoll(ui->lineEditRoll, v.z());
}

/*
//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onChangedTranslation(int viewer, QVector3D trans)
{
    //LogTrace("onChangedTrans: %.2f, %.2f, %.2f", trans.x(), trans.y(), trans.z());

    if (getViewsLinked())
    {
        QMdiSplitCmpWnd2 *wnd = dynamic_cast<QMdiSplitCmpWnd2*>(sender());
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
void SplitCmpViewCtrlsWidget::onTextEditedLightDirX(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
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

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
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

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
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

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setLightShine(f, getViewsLinked());
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightAmb()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D amb = wnd->getSelectedRenderer()->getLightInfo().amb;
    QColor c = QColorDialog::getColor(getColor(amb));
    if (!c.isValid()) return;

    amb = getColor(c);
    wnd->setLightAmb(amb, getViewsLinked());

    setColorBtnStyle(ui->btnLightAmb, c);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightDif()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().dif;
    QColor c = QColorDialog::getColor(getColor(v));
    if (!c.isValid()) return;

    v = getColor(c);
    wnd->setLightDif(v, getViewsLinked());

    setColorBtnStyle(ui->btnLightDif, c);
}

//=======================================================================
//=======================================================================
void SplitCmpViewCtrlsWidget::onClickLightSpe()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().spe;
    QColor c = QColorDialog::getColor(getColor(v));
    if (!c.isValid()) return;

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
    double x = UtlMath::clip(v.x(), 0, 1);
    double y = UtlMath::clip(v.x(), 0, 1);
    double z = UtlMath::clip(v.y(), 0, 1);


    return QColor((int)(x*255.0), (int)(y*255.0), (int)(z*255.0), 255);
}
*/
