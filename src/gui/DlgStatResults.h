#ifndef DLGSTATRESULTS_H
#define DLGSTATRESULTS_H

#include <QDialog>
#include "../core/StatResults.h"

namespace Ui {
class DlgStatResults;
}

class DlgStatResults : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgStatResults(QWidget *parent = 0);
    ~DlgStatResults();

    void updateStats(StatResults *stats);
    
public slots:
    void slotOnBtnClose();

private:
    Ui::DlgStatResults *ui;
};

#endif // DLGSTATRESULTS_H
