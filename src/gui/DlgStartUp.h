#ifndef DLGSTARTUP_H
#define DLGSTARTUP_H

#include <QDialog>

namespace Ui {
class DlgStartUp;
}

class DlgStartUp : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgStartUp(QWidget *parent = 0);
    ~DlgStartUp();

protected slots:
    void slotOnMaskEditor();
    void slotOnSplitCmp();
    void slotOnShow();
    
private:
    Ui::DlgStartUp *ui;
};

#endif // DLGSTARTUP_H
