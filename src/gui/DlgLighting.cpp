#include "DlgLighting.h"
#include "ui_DlgLighting.h"
#include "../core/logger.h"
#include "QMdiSplitCmpWnd.h"
#include "Investigator.h"
#include "UtlQtGui.h"
#include "../core/UtlMath.h"

//=======================================================================
//=======================================================================
DlgLighting::DlgLighting(Investigator *p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgLighting),
    _pi(p)
{
    ui->setupUi(this);

    setLightingDefault(false);

    makeConnections();
}

//=======================================================================
//=======================================================================
DlgLighting::~DlgLighting()
{
    delete ui;
}


//=======================================================================
//=======================================================================
void DlgLighting::makeConnections()
{
    bool ret;
    ret = connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(onClose()));
    ret = connect(ui->editLightDirX, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirX(QString)));
    ret = connect(ui->editLightDirY, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirY(QString)));
    ret = connect(ui->editLightDirZ, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightDirZ(QString)));
    ret = connect(ui->editLightShine, SIGNAL(textEdited(QString)), this, SLOT(onTextEditedLightShine(QString)));
    ret = connect(ui->btnLightAmb, SIGNAL(clicked()), this, SLOT(onClickLightAmb()));
    ret = connect(ui->btnLightDif, SIGNAL(clicked()), this, SLOT(onClickLightDif()));
    ret = connect(ui->btnLightSpe, SIGNAL(clicked()), this, SLOT(onClickLightSpe()));
    ret = connect(ui->btnLightDefaults, SIGNAL(clicked()), this, SLOT(onClickLightDef()));
}

//=======================================================================
//=======================================================================
void DlgLighting::onClose()
{
    close();
}

//=======================================================================
//=======================================================================
void DlgLighting::onTextEditedLightDirX(const QString &text)
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
void DlgLighting::onTextEditedLightDirY(const QString &text)
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
void DlgLighting::onTextEditedLightDirZ(const QString &text)
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
void DlgLighting::onTextEditedLightShine(const QString &text)
{
    float f = atof(text.toStdString().c_str());

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    wnd->setLightShine(f, getViewsLinked());
}

//=======================================================================
//=======================================================================
void DlgLighting::onClickLightAmb()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D amb = wnd->getSelectedRenderer()->getLightInfo().amb;
    QColor c = QColorDialog::getColor(getColor(amb));
    if (!c.isValid()) return;

    amb = getColor(c);
    wnd->setLightAmb(amb, getViewsLinked());

    UtlQtGui::setColorBtnStyle(ui->btnLightAmb, c);
}

//=======================================================================
//=======================================================================
void DlgLighting::onClickLightDif()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().dif;
    QColor c = QColorDialog::getColor(getColor(v));
    if (!c.isValid()) return;

    v = getColor(c);
    wnd->setLightDif(v, getViewsLinked());

    UtlQtGui::setColorBtnStyle(ui->btnLightDif, c);
}

//=======================================================================
//=======================================================================
void DlgLighting::onClickLightSpe()
{
    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;

    QVector3D v = wnd->getSelectedRenderer()->getLightInfo().spe;
    QColor c = QColorDialog::getColor(getColor(v));
    if (!c.isValid()) return;

    v = getColor(c);
    wnd->setLightSpe(v, getViewsLinked());

    UtlQtGui::setColorBtnStyle(ui->btnLightSpe, c);
}

//=======================================================================
//=======================================================================
void DlgLighting::onClickLightDef()
{
    setLightingDefault(true);
}

//=======================================================================
//=======================================================================
void DlgLighting::setLightingDefault(bool updateRenderer)
{
    UtlQtGui::updateEdit(ui->editLightDirX, "-10");
    UtlQtGui::updateEdit(ui->editLightDirY, "-5");
    UtlQtGui::updateEdit(ui->editLightDirZ, "10");
    UtlQtGui::updateEdit(ui->editLightShine, "5.0");


    UtlQtGui::setColorBtnStyle(ui->btnLightAmb, "rgb(255,255,255)");
    UtlQtGui::setColorBtnStyle(ui->btnLightDif, "rgb(255,255,255)");
    UtlQtGui::setColorBtnStyle(ui->btnLightSpe, "rgb(255,255,255)");

    if (!updateRenderer) return;

    QMdiSplitCmpWnd2 *wnd = _pi->getTopSplitCmp();
    if (!wnd) return;
    wnd->setDefaultLight(getViewsLinked());
}

//=======================================================================
//=======================================================================
QVector3D DlgLighting::getColor(const QColor &c)
{
    return QVector3D((double)c.red()/255.0, (double)c.green()/255.0, (double)c.blue()/255.0);
}

//=======================================================================
//=======================================================================
QColor DlgLighting::getColor(const QVector3D &v)
{
    double x = UtlMath::clip(v.x(), 0, 1);
    double y = UtlMath::clip(v.x(), 0, 1);
    double z = UtlMath::clip(v.y(), 0, 1);


    return QColor((int)(x*255.0), (int)(y*255.0), (int)(z*255.0), 255);
}

//=======================================================================
//=======================================================================
bool DlgLighting::getViewsLinked()
{
    SplitCmpViewCtrlsWidget *viewCtrls = _pi->getSplitCmpCtrls();
    if (!viewCtrls) return false;

    return viewCtrls->getViewsLinked();
}
