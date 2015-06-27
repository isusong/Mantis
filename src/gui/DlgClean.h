#ifndef DLGCLEAN_H
#define DLGCLEAN_H

#include <QDialog>

namespace Ui {
class DlgClean;
}

class DlgClean : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgClean(bool isTip, QWidget *parent = 0);
    ~DlgClean();
    
    bool haveMod();

    bool getDataIslands();

    bool getTipCoordSys();
    bool getTipSpike();

    bool getPltCoordSys();
    bool getPltDetrend();



private:
    Ui::DlgClean *ui;
    bool _isTip;
};

#endif // DLGCLEAN_H
