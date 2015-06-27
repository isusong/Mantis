#ifndef QPROGRESSDIALOGEX_H
#define QPROGRESSDIALOGEX_H

#include <QProgressDialog>

class QProgressDialogEx : public QProgressDialog
{
    Q_OBJECT
public:
    explicit QProgressDialogEx::QProgressDialogEx(const QString &labelText, const QString &cancelBtnText, QWidget *parent = NULL);
    
signals:
    
public slots:
    //void slotInitFailed(QString reason);
    void slotStart();
    void slotProgress(float percent);
    void slotMsg(QString msg);
    //void slotEnd();
    
};

#endif // QPROGRESSDIALOGEX_H
