#include "QListWidgetEx.h"
#include <QMouseEvent>

//=======================================================================
//=======================================================================
QListWidgetEx::QListWidgetEx(QWidget* parent) : QListWidget(parent),
  _lastBtnClick(Qt::NoButton)
{
    this->setMovement(QListView::Static);
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem *)));
}

//=======================================================================
//=======================================================================
void QListWidgetEx::mousePressEvent(QMouseEvent *event)
{
    _lastBtnClick = event->button();

    QListWidget::mousePressEvent(event);
}

//=======================================================================
//=======================================================================
void QListWidgetEx::slotItemClicked(QListWidgetItem *item)
{
    if (_lastBtnClick == Qt::LeftButton)
    {
        emit signalItemLeftClicked(item);
    }
    else if (_lastBtnClick == Qt::RightButton)
    {
        emit signalItemRightClicked(item);
    }
}
