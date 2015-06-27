#include "UtlQtGui.h"
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>

//=======================================================================
//=======================================================================
int UtlQtGui::showMsg(const QString &title, const QString &msg, int btns)
{
    QMessageBox box(QMessageBox::Information, title, msg, (QMessageBox::StandardButtons)btns);
    return box.exec();
}

//=======================================================================
//=======================================================================
int UtlQtGui::showLongMsg(const QString &title, const QString &msg, int btns)
{
    QMessageBox box(QMessageBox::Information, title, msg, (QMessageBox::StandardButtons)btns);
    QGridLayout *layout = qobject_cast<QGridLayout *>(box.layout());
    if (layout)
    {
        QTextEdit *edit = new QTextEdit(msg);
        edit->setReadOnly(true);
        layout->addWidget(edit, 0, 1);
    }

    return box.exec();
}

//=======================================================================
//=======================================================================
void UtlQtGui::setColorBtnStyle(QPushButton *btn, const QColor &c)
{
    char acfrmt[256];
    sprintf_s(acfrmt, "rgb(%d,%d,%d)", c.red(), c.green(), c.blue());
    QString rgb = acfrmt;

    setColorBtnStyle(btn, rgb);
}

//=======================================================================
//=======================================================================
void UtlQtGui::setColorBtnStyle(QPushButton *btn, const char *rgb)
{
    setColorBtnStyle(btn, QString(rgb));
}

//=======================================================================
//=======================================================================
void UtlQtGui::setColorBtnStyle(QPushButton *btn, const QString &rgb)
{
    QString style =  "QPushButton  { border: 2px solid #8f8f91; border-radius: 6px;";
    style += "background-color: ";
    style += rgb;
    style += ";}";

    style += "QPushButton:pressed   {background-color: ";
    style += rgb;
    style += ";";
    style += "border: 2px solid #707070;";
    style += "}";

    btn->setStyleSheet(style);

    //min-width: 80px;
    // regular then pressed
    // background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);
    // background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);
}

//=======================================================================
//=======================================================================
void UtlQtGui::updateEdit(QLineEdit *edit, double d)
{
    char ac[32];
    sprintf_s(ac, "%.2f", d);

    updateEdit(edit, ac);
}

//=======================================================================
//=======================================================================
void UtlQtGui::updateEdit(QLineEdit *edit, int i)
{
    char ac[32];
    sprintf_s(ac, "%d", i);

    updateEdit(edit, ac);
}


//=======================================================================
//=======================================================================
void UtlQtGui::updateEdit(QLineEdit *edit, const char *v)
{
    bool b = edit->blockSignals(true);
    edit->setText(v);
    edit->blockSignals(b);
}

//=======================================================================
//=======================================================================
void UtlQtGui::updateSlider(QSlider *slider, int pos)
{
    bool b = slider->blockSignals(true);
    slider->setSliderPosition(pos);
    slider->blockSignals(b);
}
