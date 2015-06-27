#ifndef QLISTWIDGETEX_H
#define QLISTWIDGETEX_H

#include <QListWidget>

class QListWidgetEx : public QListWidget
{
    Q_OBJECT

public:
    QListWidgetEx(QWidget* parent = 0);


public slots:
    void slotItemClicked(QListWidgetItem *item);

signals:
    void signalItemLeftClicked(QListWidgetItem *item);
    void signalItemRightClicked(QListWidgetItem *item);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

protected:
    Qt::MouseButton _lastBtnClick;
};

#endif
