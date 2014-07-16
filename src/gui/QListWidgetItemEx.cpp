#include "QListWidgetItemEx.h"

//=======================================================================
//=======================================================================
QListWidgetItemEx::QListWidgetItemEx(const QIcon &icon, const QString &displayName, const QString &file)
    : QListWidgetItem(icon, displayName),
      _file(file)
{
}
