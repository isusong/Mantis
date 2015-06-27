#include "StatScaleDraw.h"

//=======================================================================
//=======================================================================
StatScaleDraw::StatScaleDraw()
{
}

//=======================================================================
//=======================================================================
QwtText StatScaleDraw::label(double value)	const
{
    QwtText text;
    if (value >= 1000 || value <= -1000)
    {
        value = value / 1000.0;
        text = QwtScaleDraw::label(value);
        QString s = text.text();
        s = s + "k";
        text.setText(s);
    }
    else
    {
        text = QwtScaleDraw::label(value);
    }

    return text;
}
