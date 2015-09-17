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
    explicit DlgClean(int dataType, const QString &imgFolder, QWidget *parent = 0);
    ~DlgClean();
    
    bool haveMod();

    bool getDataIslands();

    bool getTipCoordSys();
    bool getTipSpike();

    bool getPltCoordSys();
    bool getPltDetrend();

    bool getThresholdRun();
    QSharedPointer<QImage> getQualityMap();
    int getThresholdTx();
    int getThresholdQlt();

public slots:
    virtual void accept();

    void slotOnBtnBrowseQltMap();

protected:
    void makeConnections();
private:
    Ui::DlgClean *ui;
    int _dataType;
    QString _imgFolder;
    QSharedPointer<QImage> _qualityMap;
};

#endif // DLGCLEAN_H
