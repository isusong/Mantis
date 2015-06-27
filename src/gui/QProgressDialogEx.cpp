#include "QProgressDialogEx.h"

//=======================================================================
//=======================================================================
QProgressDialogEx::QProgressDialogEx(const QString &labelText, const QString &cancelBtnText, QWidget *parent) :
    QProgressDialog(labelText, cancelBtnText, 0, 100, parent)
{
}

//=======================================================================
//=======================================================================
void QProgressDialogEx::slotStart()
{
    setValue(0);
}

//=======================================================================
//=======================================================================
void QProgressDialogEx::slotProgress(float percent)
{
    setValue((int)(percent * 100.0f));
}

//=======================================================================
//=======================================================================
void QProgressDialogEx::slotMsg(QString msg)
{
    setLabelText(msg);
}
