#ifndef DLGMARKOPTSETTINGS_H
#define DLGMARKOPTSETTINGS_H

#include <QDialog>

namespace Ui {
class DlgMarkOptSettings;
}

class DlgMarkOptSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgMarkOptSettings(QWidget *parent = 0);
    ~DlgMarkOptSettings();
    
public slots:
    void onOk();

private:
    Ui::DlgMarkOptSettings *ui;
};

#endif // DLGMARKOPTSETTINGS_H
