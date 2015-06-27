#include "DlgMarkOptSettings.h"
#include "ui_DlgMarkOptSettings.h"
#include "SettingsStore.h"
#include "App.h"

DlgMarkOptSettings::DlgMarkOptSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgMarkOptSettings)
{
    ui->setupUi(this);

    setWindowTitle(QString("Mark Optimization Settings"));

    ui->spinBoxYawStart->setMinimum(0);
    ui->spinBoxYawStart->setMaximum(359);
    ui->spinBoxYawStart->setValue(App::settings()->mark().yawMin);
    ui->spinBoxYawEnd->setMinimum(0);
    ui->spinBoxYawEnd->setMaximum(359);
    ui->spinBoxYawEnd->setValue(App::settings()->mark().yawMax);
    ui->spinBoxYawInc->setMinimum(1);
    ui->spinBoxYawInc->setMaximum(30);
    ui->spinBoxYawInc->setValue(App::settings()->mark().yawInc);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
}

DlgMarkOptSettings::~DlgMarkOptSettings()
{
    delete ui;
}

void DlgMarkOptSettings::onOk()
{
    int yawStart = ui->spinBoxYawStart->value();
    int yawEnd = ui->spinBoxYawEnd->value();
    int yawInc = ui->spinBoxYawInc->value();

    App::settings()->mark().yawMin = yawStart;
    App::settings()->mark().yawMax = yawEnd;
    App::settings()->mark().yawInc = yawInc;
    if (yawStart > yawEnd)
    {
        App::settings()->mark().yawMin = yawEnd;
        App::settings()->mark().yawMax = yawStart;
    }
}
