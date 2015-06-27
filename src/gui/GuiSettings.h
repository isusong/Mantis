#ifndef GUISETTINGS_H
#define GUISETTINGS_H

#include <QColor>

class GuiSettings
{
public:
    static QColor colorSearchBoxPlotLeft() { return Qt::darkMagenta; }
    static QColor colorSearchBoxPlotRight() { return Qt::blue; }

    static QColor colorSearchBoxLeft() { QColor c = colorSearchBoxPlotLeft(); c.setAlpha(153); return c; }
    static QColor colorSearchBoxRight() { QColor c = colorSearchBoxPlotRight(); c.setAlpha(153); return c; }

    static QColor colorSelectWinL() { return QColor(200,0,200,255); }
    static QColor colorSelectWinR() { return QColor(0,150,255,255); }
    //static QColor colorSelectWinL() { return QColor(255,150,0,255); }
    //static QColor colorSelectWinR() { return QColor(0,150,255,255); }

    static QColor colorListWidgetBg() { return QColor(255,255,255,255); }
};

#endif
