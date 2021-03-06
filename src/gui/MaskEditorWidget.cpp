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

#include "MaskEditorWidget.h"
#include <QApplication>
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QLinearGradient>
#include <QRect>
#include "App.h"
#include "DlgClean.h"
#include "../core/CleaningCode/Clean.h"
#include "../core/logger.h"
#include "../core/IProgress.h"
#include "../core/UtlQt.h"
#include "QProgressDialogEx.h"
#include "ThreadCleaner.h"

using std::cout;
using std::endl;

//Color for masked regions. (Transparent coral.)
#define REDVAL 237
#define GREENVAL 108
#define BLUEVAL 108
#define ALPHAVAL 100

//=======================================================================
//=======================================================================
MaskEditorWidget::MaskEditorWidget(QWidget* parent) :
	QWidget(parent)
{
    //Default - no file yet.
    _vt = VT_Draw;
    _drawing = false;
    _drawMode = false;
    _eraseMode = false;
    _modified = false;
    _maskColor = QColor (REDVAL, GREENVAL, BLUEVAL, ALPHAVAL);
    _emptyColor = QColor (0, 0, 0, 0);

    refreshSettings();

    setCursor(Qt::CrossCursor);
}

//=======================================================================
//=======================================================================
MaskEditorWidget::~MaskEditorWidget()
{
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::paintEvent(QPaintEvent* event)
{
    QMutexLocker lock(&_imgLock);

	QPainter painter (this);
	QRect drawRect = event->rect();
	
	//Draw background.
	QLinearGradient background (drawRect.topLeft(), drawRect.bottomRight());
	background.setColorAt(0.0, Qt::darkGray);
	background.setColorAt(1.0, Qt::black);
	painter.fillRect(drawRect, background);

	//Draw texture and mask (if loaded)
    if ((!_texture.isNull()) && (!_maskImg.isNull()))
	{
		QImage scaledTexture;
		QImage scaledMask;

		float winAspect = ((float) width())/((float) height());
        float textAspect= ((float) _textureWidth)/((float) _textureHeight);
		if (textAspect > winAspect) //texture is wider than window.
		{
            scaledTexture = _texture.scaledToWidth(width());
            scaledMask = _maskImg.scaledToWidth(width());
            _toWidth = true;
		}
		else
		{
            scaledTexture = _texture.scaledToHeight(height());
            scaledMask = _maskImg.scaledToHeight(height());
            _toWidth = false;
		}

        //Draw texture and then _maskImg.
		painter.drawImage(drawRect,	scaledTexture, drawRect);
		painter.drawImage(drawRect, scaledMask, drawRect);
	}
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::drawTo(const QPoint endpoint)
{
    if (_maskImg.isNull()) return; //Do not draw if no image.

	//Calculate the scale of the image for coordinate transformation.
	float scale;
    if (_toWidth)
	{
        scale = ((float) _textureWidth)/((float) width());
	}
	else
	{
        scale = ((float) _textureHeight)/((float) height());
	}

    //Open the painter on the _maskImg.
    QPainter painter (&_maskImg);
	//Allows us to paint without overlapping and to erase.
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	//Setup the pen.
	QPen toolPen;
    if (_vt == VT_Draw)
	{
        toolPen.setColor(_maskColor);
	}
	else
	{
        toolPen.setColor(_emptyColor);
	}
    toolPen.setWidth(brushSize()*scale);
	toolPen.setCapStyle(Qt::RoundCap);
	toolPen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(toolPen);
	
	//Draw the line.
    painter.drawLine(_capturedPos*scale, endpoint*scale);
    _modified = true;

	//Update the view.
	update();
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::mousePressEvent(QMouseEvent* event)
{
    if (_rngImg.isNull()) return;

    if (Qt::LeftButton == event->button())
	{
        _capturedPos = event->pos();
        _drawing = true;
	}
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (_drawing && (Qt::LeftButton & event->buttons()))
	{
		drawTo(event->pos());
        _capturedPos = event->pos();
	}
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (_drawing && (Qt::LeftButton & event->buttons()))
	{
		drawTo(event->pos());
        _drawing = false;
	}
}

//=======================================================================
//=======================================================================
bool MaskEditorWidget::isModified()
{
    if (_rngImg.isNull()) return false;

    return _modified;
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::setModified(bool m)
{
    _modified = m;
}

//=======================================================================
//=======================================================================
float MaskEditorWidget::brushSize()
{
    return App::settings()->mask().szBrush;
}

//=======================================================================
// 3 prog steps
//=======================================================================
void MaskEditorWidget::setImg(PRangeImage img, IProgress *prog)
{
    QMutexLocker lock(&_imgLock);

    _modified = false;

    _rngImg = img;
    if (_rngImg.isNull())
    {
        _texture = QImage();
        _maskImg = QImage();
        _textureWidth = 0;
        _textureHeight = 0;

        if (!prog) update();  //Refresh widget (don't refresh if calling from non-gui thread)

        // prog update
        if (prog) prog->progStep(4);

        return;
    }

    if (!prog) QApplication::setOverrideCursor(Qt::WaitCursor);

    QBitArray mask; //to hold the mask temporarily.

    _texture = _rngImg->getTexture();
    mask = _rngImg->getMask();

    //Load texture width and height.
    _textureWidth = _texture.width();
    _textureHeight = _texture.height();

    //Encode mask to QImage.
    //This will render the masked portion as transparent coral for now.
    if (_maskImg.isNull() || _maskImg.width() != _textureWidth || _maskImg.height() != _textureHeight)
    {
        _maskImg = QImage (_textureWidth, _textureHeight, QImage::Format_ARGB32); //_Premultiplied);
    }

    // prog update
    if (prog) prog->progStep();

    for (int i = 0; i < _textureHeight; ++i)
    {
        for (int j = 0; j < _textureWidth; ++j)
        {
            //Masked region is 0
            //Kept (non-masked) region is 255
            if (!mask.testBit((_textureWidth*i) + j)) //If mask is 0.
            {
                _maskImg.setPixel(j, i, _maskColor.rgba());
            }
            else
            {
                _maskImg.setPixel(j, i, _emptyColor.rgba());
            }
        }
    }

    // prog update
    if (prog) prog->progStep();

    //Rotate the images for easier viewing.
    QMatrix rot;
    rot.rotate(90);
    _maskImg = _maskImg.transformed(rot);
    _texture = _texture.transformed(rot);
    int temp = _textureWidth;
    _textureWidth = _textureHeight;
    _textureHeight = temp;

    // prog update
    if (prog) prog->progStep();

    if (!prog) update();  //Refresh widget (don't refresh if calling from non-gui thread)

    if (!prog) QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
PRangeImage MaskEditorWidget::getImgCopy()
{
    QMutexLocker lock(&_imgLock);

    if (!_rngImg) return PRangeImage();

    return PRangeImage( new RangeImage(*_rngImg.data()));
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::maskClear()
{
    if (_rngImg.isNull()) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for (int i = 0; i < _textureHeight; ++i)
    {
        for (int j = 0; j < _textureWidth; ++j)
        {
            _maskImg.setPixel(j, i, _emptyColor.rgba());
        }
    }

    _modified = true; // data has been modified

    update();  //Refresh widget.

    QApplication::restoreOverrideCursor();
}

//=======================================================================
//=======================================================================
int MaskEditorWidget::computeProgCount()
{
    if (!_dlgClean || !_rngImg) return 0;

    int count = 0;

    // save mask
    count += 3; // 1 for mask creation, 1 for mask image rotation, 1 for image creation

    if (_dlgClean->getDataIslands())
    {
        count += 4; // 4 for algoritm
    }
    if (_dlgClean->getTipSpike())
    {
        count += 8 + 24; // 32 for algoritm
    }
    if (_dlgClean->getTipCoordSys() && _rngImg->isTip())
    {
        count += 2; // 2 for algoritm
    }
    if (_dlgClean->getPltDetrend())
    {
        count += 3; // 3 for algoritm
    }
    if (_dlgClean->getPltCoordSys() && _rngImg->isPlate())
    {
        count += 2; // 2 for algoritm
    }
    if (_dlgClean->getThresholdRun())
    {
        count += 10; // 10 for algoritm
    }

    count += 3; // 3 steps for final set image call

    return count;
}

/*
//=======================================================================
//=======================================================================
void MaskEditorWidget::clean()
{
    if (_rngImg.isNull())
    {
        QMessageBox::warning(this, "Mask Editor Clean", "You must load or import data first to then clean it.");
        return;
    }

    //Perform any desired cleaning functions
    _dlgClean.reset(new DlgClean(_rngImg->getImgType()));
    if (_dlgClean->exec() != QDialog::Accepted || !_dlgClean->haveMod())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // save the mask to use the current modified mask data during cleaning
    saveMask();

    //Run connected components to avoid floating data islands.
    Clean cleaner;
    RangeImage *imgNew;

    if (_dlgClean->getDataIslands())
    {
        imgNew = cleaner.connectedComponents(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (_dlgClean->getTipSpike())
    {
        LogTrace("Removing spikes and filling holes... ");
        // This redoes the conncomps if necessary.

        imgNew = cleaner.spikeRemovalHoleFilling(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (_dlgClean->getTipCoordSys() && _rngImg->isTip())
    {
        LogTrace("Computing a tip coordinate system.");
        imgNew = cleaner.coordinateSystem4Tip(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (_dlgClean->getPltDetrend())
    {
        LogTrace("Detrending....");
        imgNew = cleaner.detrend(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (_dlgClean->getPltCoordSys() && _rngImg->isPlate())
    {
        LogTrace("Computing a centroid coordinate system.");
        imgNew = cleaner.coordinateSystem2Centroid(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }


    // reset the image to get everything up to date
    setImg(_rngImg);
    _modified = true; // data has been modified

    QApplication::restoreOverrideCursor();
}
*/

//=======================================================================
//=======================================================================
void MaskEditorWidget::cleanWithProgress()
{
    if (_rngImg.isNull())
    {
        QMessageBox::warning(this, "Mask Editor Clean", "You must load or import data first to then clean it.");
        return;
    }

    //Perform any desired cleaning functions
    QString fpath = UtlQt::filePath(_rngImg->getFileName());
    _dlgClean.reset(new DlgClean(_rngImg->getImgType(), fpath));
    if (_dlgClean->exec() != QDialog::Accepted || !_dlgClean->haveMod())
    {
        return;
    }

    QProgressDialogEx progress("Cleaning..", "Abort", this);
    progress.setWindowModality(Qt::WindowModal);

    std::tr1::shared_ptr<ThreadCleaner> threadCleaner;
    threadCleaner.reset(new ThreadCleaner(this));


    bool res = true;
    res = connect(threadCleaner.get(), SIGNAL(signalStart()), &progress, SLOT(slotStart()));
    res = connect(threadCleaner.get(), SIGNAL(signalProgress(float)), &progress, SLOT(slotProgress(float)));
    res = connect(threadCleaner.get(), SIGNAL(signalMsg(QString)), &progress, SLOT(slotMsg(QString)));
    res = connect(&progress, SIGNAL(canceled()), threadCleaner.get(), SLOT(slotCancel()));

    progress.setValue(0);
    progress.show();


    threadCleaner->startThread();
    while (threadCleaner->isRunning())
    {
        QApplication::processEvents();
    }

    update(); // refresh widget
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::clean()
{
    if (_rngImg.isNull())
    {
        QMessageBox::warning(this, "Mask Editor Clean", "You must load or import data first to then clean it.");
        return;
    }

    //Perform any desired cleaning functions
    QString fpath = UtlQt::filePath(_rngImg->getFileName());
    DlgClean options(_rngImg->getImgType(), fpath);
    if (options.exec() != QDialog::Accepted || !options.haveMod())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // save the mask to use the current modified mask data during cleaning
    saveMask();

    //Run connected components to avoid floating data islands.
    Clean cleaner;
    RangeImage *imgNew;

    if (options.getDataIslands())
    {
        imgNew = cleaner.connectedComponents(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (options.getTipSpike())
    {
        LogTrace("Removing spikes and filling holes... ");
        // This redoes the conncomps if necessary.

        imgNew = cleaner.spikeRemovalHoleFilling(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (options.getTipCoordSys() && _rngImg->isTip())
    {
        LogTrace("Computing a tip coordinate system.");
        imgNew = cleaner.coordinateSystem4Tip(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (options.getPltDetrend())
    {
        LogTrace("Detrending....");
        imgNew = cleaner.detrend(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }
    if (options.getPltCoordSys() && _rngImg->isPlate())
    {
        LogTrace("Computing a centroid coordinate system.");
        imgNew = cleaner.coordinateSystem2Centroid(_rngImg.data());
        imgNew->setFileName(_rngImg->getFileName());
        _rngImg = PRangeImage(imgNew);
    }


    // reset the image to get everything up to date
    setImg(_rngImg);
    _modified = true; // data has been modified

    QApplication::restoreOverrideCursor();
}

//=======================================================================
// 3 prog steps
//=======================================================================
bool MaskEditorWidget::saveMask(IProgress *prog)
{
    QMutexLocker lock(&_imgLock);

    if (_rngImg.isNull()) return false;

    if (!prog)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    _modified = true;

    //Decode mask from _maskImg.
    //Undo the rotation.
    QMatrix rot;
    rot.rotate(-90);
    _maskImg = _maskImg.transformed(rot);
    int temp = _textureWidth;
    _textureWidth = _textureHeight;
    _textureHeight = temp;
    //Create mask, initialized to false.
    QBitArray mask (_textureWidth*_textureHeight, false);
    for (int i = 0; i < _textureHeight; ++i)
    {
        for (int j = 0; j < _textureWidth; ++j)
        {
            if (_maskImg.pixel(j, i) == _emptyColor.rgba())
            {
                mask.setBit((_textureWidth*i) + j);
            }
        }
    }

    // prog update
    if (prog) prog->progStep();

    //Redo the rotation.
    rot = QMatrix();
    rot.rotate(90);
    _maskImg = _maskImg.transformed(rot) ;
    temp = _textureWidth;
    _textureWidth = _textureHeight;
    _textureHeight = temp;

    // prog update
    if (prog) prog->progStep();

    //Create new range image and delete old one.
    //Make mutable copies of the depth, texture, csys.
    QVector<float> newDepth (_rngImg->getDepth());
    QImage newTexture (_rngImg->getTexture());
    QMatrix4x4 newCsys (_rngImg->getCoordinateSystemMatrix());

    //Make the new data structure.
    RangeImage* newImg = new RangeImage(
        _rngImg->getWidth(),
        _rngImg->getHeight(),
        _rngImg->getPixelSizeX(),
        _rngImg->getPixelSizeY(),
        newDepth,
        newTexture,
        mask,
        newCsys,
        _rngImg->getImgType(),
        _rngImg->getFileName());

    _rngImg = PRangeImage(newImg);

    // prog update
    if (prog) prog->progStep();

    if (!prog)
    {
        QApplication::restoreOverrideCursor();
    }

    return true;
}

//=======================================================================
//=======================================================================
bool MaskEditorWidget::saveFile(const QString &filename)
{
    if (_rngImg.isNull())
    {
        QMessageBox::warning(this, "Mask Editor Save", "You must load or import data first.");
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    LogTrace("Saving file %s...", filename.toStdString().c_str());

    saveMask();
    int success = _rngImg->save(filename);

    QApplication::restoreOverrideCursor();

    if (!success)
    {
        QMessageBox::critical(this, "Mask Editor Clean", "Failed to save data to file " + filename);
        return false;
    }

    //If we got here, the save was successful.
    _rngImg->setFileName(filename);
    _modified = false;
    return true;

}

//=======================================================================
//=======================================================================
void MaskEditorWidget::setViewTool(EViewTool vt)
{
    _vt = vt;
}

//=======================================================================
//=======================================================================
void MaskEditorWidget::refreshSettings()
{
    //SettingsStore::loadMaskEditor(&_settings);
}
