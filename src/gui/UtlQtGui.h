#ifndef UTLQTGUI_H
#define UTLQTGUI_H

#include <QMessageBox>
class QLineEdit;
class QSlider;

class UtlQtGui
{
public:

    static int showMsg(const QString &title, const QString &msg, int btns = QMessageBox::NoButton);
    static int showLongMsg(const QString &title, const QString &msg, int btns = QMessageBox::NoButton);

    static void setColorBtnStyle(QPushButton *btn, const QColor &c);
    static void setColorBtnStyle(QPushButton *btn, const char *rgb);
    static void setColorBtnStyle(QPushButton *btn, const QString &rgb);

    static void updateEdit(QLineEdit *edit, double d);
    static void updateEdit(QLineEdit *edit, int i);
    static void updateEdit(QLineEdit *edit, const char *v);

    static void updateSlider(QSlider *slider, int pos);
};

#endif
