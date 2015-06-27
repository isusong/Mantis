#include "DlgStartUp.h"
#include "ui_DlgStartUp.h"
#include "App.h"
#include "SettingsStore.h"

//=======================================================================
//=======================================================================
DlgStartUp::DlgStartUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStartUp)
{
    ui->setupUi(this);

    connect(ui->pushBtnMaskEdit, SIGNAL(clicked()), SLOT(slotOnMaskEditor()));
    connect(ui->pushBtnSplitCmp, SIGNAL(clicked()), SLOT(slotOnSplitCmp()));
    connect(ui->checkBoxShow, SIGNAL(clicked()), SLOT(slotOnShow()));
    ui->checkBoxShow->setChecked(App::settings()->inv().showStartupDlg);
}

//=======================================================================
//=======================================================================
DlgStartUp::~DlgStartUp()
{
    delete ui;
}


//=======================================================================
//=======================================================================
void DlgStartUp::slotOnMaskEditor()
{
    App::settings()->inv().startupMode = SettingsStore::StartUp_MaskEditor;

    done(Accepted);
}

//=======================================================================
//=======================================================================
void DlgStartUp::slotOnSplitCmp()
{
    App::settings()->inv().startupMode = SettingsStore::StartUp_SplitCmp;
    done(Accepted);
}

//=======================================================================
//=======================================================================
void DlgStartUp::slotOnShow()
{
    App::settings()->inv().showStartupDlg = ui->checkBoxShow->isChecked();
}
