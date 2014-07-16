#ifndef QLISTWIDGETITEMEX_H
#define QLISTWIDGETITEMEX_H

#include <QListWidgetItem>

class QListWidgetItemEx : public QListWidgetItem
{
public:
    QListWidgetItemEx(const QIcon &icon, const QString &displayName, const QString &file);

    void setFile(const QString &file) { _file = file; }
    QString getFile() { return _file; }

protected:
    QString _file;
};

#endif // QLISTWIDGETITEMEX_H
