#include "DlgClean.h"
#include "ui_DlgClean.h"

//=======================================================================
//=======================================================================
DlgClean::DlgClean(bool isTip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgClean),
    _isTip(isTip)
{
    ui->setupUi(this);

    ui->checkBoxDataIslands->setChecked(true);

    if (isTip)
    {
        ui->checkBoxTipCoordSys->setChecked(true);
        ui->checkBoxTipSpike->setChecked(true);
        ui->checkBoxPltCoordSys->setChecked(false);
        ui->checkBoxPltDetrend->setChecked(false);
    }
    else
    {
        ui->checkBoxPltCoordSys->setChecked(true);
        ui->checkBoxPltDetrend->setChecked(true);
        ui->checkBoxTipCoordSys->setChecked(false);
        ui->checkBoxTipSpike->setChecked(false);
    }
}

//=======================================================================
//=======================================================================
DlgClean::~DlgClean()
{
    delete ui;
}

//=======================================================================
//=======================================================================
bool DlgClean::haveMod()
{
    if (getDataIslands()) return true;
    if (getTipCoordSys()) return true;
    if (getTipSpike()) return true;
    if (getPltCoordSys()) return true;
    if (getPltDetrend()) return true;

    return false;
}

//=======================================================================
//=======================================================================
bool DlgClean::getDataIslands()
{
    return (ui->checkBoxDataIslands->checkState() == Qt::Checked);
}

//=======================================================================
//=======================================================================
bool DlgClean::getTipCoordSys()
{
    return (ui->checkBoxTipCoordSys->checkState() == Qt::Checked);
}

//=======================================================================
//=======================================================================
bool DlgClean::getTipSpike()
{
    return (ui->checkBoxTipSpike->checkState() == Qt::Checked);
}

//=======================================================================
//=======================================================================
bool DlgClean::getPltCoordSys()
{
    return (ui->checkBoxPltCoordSys->checkState() == Qt::Checked);
}

//=======================================================================
//=======================================================================
bool DlgClean::getPltDetrend()
{
    return (ui->checkBoxPltDetrend->checkState() == Qt::Checked);
}

