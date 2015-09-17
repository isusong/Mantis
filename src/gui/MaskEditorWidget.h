/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#ifndef MASKEDITORWIDGET_H
#define MASKEDITORWIDGET_H
#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPaintEvent>
#include <QColor>
#include <QString>
#include <QPoint>
#include <QSettings>
#include <QMutex>
#include "SettingsStore.h"
#include "../core/RangeImage.h"
#include "DlgClean.h"

class IProgress;

class MaskEditorWidget : public QWidget
{
	Q_OBJECT

	//Inspired by the QT Scribble Example:
	//http://qt-project.org/doc/qt-4.8/widgets-scribble.html
public:
    enum EViewTool
    {
        VT_Draw = 1,
        VT_Erase = 2
    };

public:
    MaskEditorWidget(QWidget* parent=NULL);
    virtual ~MaskEditorWidget();

    void setImg(PRangeImage img, IProgress *prog=NULL);
    PRangeImage getImg() { return _rngImg; }
    PRangeImage getImgCopy();
    void clean();
    void cleanWithProgress();
    bool saveFile(const QString &file);

    void maskClear();

    //Mouse Capture functions
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    //Screen Capture Function
    bool isModified(); //Was the file modified?
    void setModified(bool m);

    float brushSize();

    void setViewTool(EViewTool vt);
    void refreshSettings();

    int computeProgCount();

    DlgClean* getCleanOptions() { return _dlgClean.get(); }
    bool saveMask(IProgress *prog=NULL);

    QMutex* getImgLock(){ return &_imgLock; }
public slots:

protected:
    void paintEvent(QPaintEvent* event); //Draw the scene
    void drawTo(const QPoint endpoint); //Function for drawing into mask.

protected:

    std::tr1::shared_ptr<DlgClean> _dlgClean;
    PRangeImage _rngImg;
    QImage _maskImg;
    QImage _texture;
    int _textureWidth;
    int _textureHeight;
    bool _toWidth; //Is the view scaled to width or to height?
    //float _toolSize; //size of brush/eraser.
    QPoint _capturedPos; //position of the mouse when pressed last.
    bool _drawing; //Are they drawing?
    QColor _maskColor; //Pen color for masked areas.
    QColor _emptyColor; //Pen color for kept (non-masked) areas;
    bool _drawMode; //Using the brush?
    bool _eraseMode; //Using the eraser?
    EViewTool _vt;

    bool _modified; //modified?

    QMutex _imgLock;
};


#endif //! defined __GRAPHICSWIDGET_H__
