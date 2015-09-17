#include "DlgClean.h"
#include "ui_DlgClean.h"
#include "../core/RangeImage.h"
#include "../core/UtlQt.h"
#include "App.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>

//=======================================================================
//=======================================================================
DlgClean::DlgClean(int dataType, const QString &imgFolder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgClean),
    _dataType(dataType),
    _imgFolder(imgFolder)
{
    ui->setupUi(this);

    if (_dataType == RangeImage::ImgType_Tip || _dataType == RangeImage::ImgType_Plt)
    {
        ui->checkBoxDataIslands->setChecked(true);
    }

    if (_dataType == RangeImage::ImgType_Tip)
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

    // look for quality map
    QString filenames[] = {QString("qualitymap.bmp"), QString("qualitymap.png")};
    for (int i=0; i<2; i++)
    {
        QString qmap;
        qmap = UtlQt::pathCombine(_imgFolder, filenames[i]);
        if (UtlQt::fileExists(qmap))
        {
            ui->lineEditThreshQMap->setText(qmap);
        }
    }

    makeConnections();
}

//=======================================================================
//=======================================================================
DlgClean::~DlgClean()
{
    delete ui;
}


//=======================================================================
//=======================================================================
void DlgClean::makeConnections()
{
    bool result;
    result = connect(ui->btnBrowseQMap, SIGNAL(clicked()), this, SLOT(slotOnBtnBrowseQltMap()));
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
    if (getThresholdRun()) return true;

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


//=======================================================================
//=======================================================================
bool DlgClean::getThresholdRun()
{
    return (ui->checkBoxThreshRun->checkState() == Qt::Checked);
}


//=======================================================================
//=======================================================================
QSharedPointer<QImage> DlgClean::getQualityMap()
{
    return _qualityMap;
}

//=======================================================================
//=======================================================================
int DlgClean::getThresholdTx()
{
    return ui->spinBoxThreshMinTx->value();
}

//=======================================================================
//=======================================================================
int DlgClean::getThresholdQlt()
{
    return ui->spinBoxThreshMinQl->value();
}

//=======================================================================
//=======================================================================
void DlgClean::slotOnBtnBrowseQltMap()
{
    QString filename = QFileDialog::getOpenFileName(App::mainWnd(), tr("Select Quality Map"), _imgFolder,
        tr("Image Files (*.jpg *.jpeg *.png *.bmp *.tif *.tiff);;BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg *.jpeg);;All files (*.*)" ));

    if (filename.isNull()) return;

    QImage qmap;
    if (!qmap.load(filename))
    {
        QMessageBox msgBox;
        msgBox.setText("Quality map image file is not valid.");
        msgBox.setInformativeText(QString("To run a threshold you need to select a valid quality map image.\nFailed to load file ") + filename + QString("."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    ui->lineEditThreshQMap->setText(filename);
}

//=======================================================================
//=======================================================================
void DlgClean::accept()
{
    if (!getThresholdRun())
    {
        QDialog::accept();
        return;
    }

    _qualityMap.clear();

    QString file = ui->lineEditThreshQMap->text();
    if (file.isNull())
    {
        QMessageBox msgBox;
        msgBox.setText("Quality map image file is not valid.");
        msgBox.setInformativeText("To run a threshold you need to select a valid quality map image.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    _qualityMap = QSharedPointer<QImage>(new QImage());
    if (!_qualityMap->load(file))
    {
        QMessageBox msgBox;
        msgBox.setText("Quality map image file is not valid.");
        msgBox.setInformativeText(QString("To run a threshold you need to select a valid quality map image.\nFailed to load file ") + file + QString("."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

