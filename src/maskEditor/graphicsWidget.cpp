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

#include "graphicsWidget.h"
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QLinearGradient>
#include <QRect>
#include "../core/CleaningCode/Clean.h"
#include "cleaningDialog.h"

using std::cout;
using std::endl;

//Color for masked regions. (Transparent coral.)
#define REDVAL 237
#define GREENVAL 108
#define BLUEVAL 108
#define ALPHAVAL 100

graphicsWidget::graphicsWidget(QWidget* parent) :
	QWidget(parent)
{
	//Default - no file yet.
	fileContents = NULL;
	toolSize = 1.0f;
	drawing = false;
	drawMode = false;
	eraseMode = false;
	modified = false;
	maskColor = QColor (REDVAL, GREENVAL, BLUEVAL, ALPHAVAL);
	emptyColor = QColor (0, 0, 0, 0);

	readSettings();

	//Write settings in case of new system
	settings.setValue("toolSize", toolSize);
}

graphicsWidget::~graphicsWidget()
{
	delete fileContents;
}

void
graphicsWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter (this);
	QRect drawRect = event->rect();
	
	//Draw background.
	QLinearGradient background (drawRect.topLeft(), drawRect.bottomRight());
	background.setColorAt(0.0, Qt::darkGray);
	background.setColorAt(1.0, Qt::black);
	painter.fillRect(drawRect, background);

	//Draw texture and mask (if loaded)
	if ((!texture.isNull()) && (!maskImage.isNull()))
	{
		QImage scaledTexture;
		QImage scaledMask;

		float winAspect = ((float) width())/((float) height());
		float textAspect= ((float) textureWidth)/((float) textureHeight);
		if (textAspect > winAspect) //texture is wider than window.
		{
			scaledTexture = texture.scaledToWidth(width());
			scaledMask = maskImage.scaledToWidth(width());
			toWidth = true;
		}
		else
		{
			scaledTexture = texture.scaledToHeight(height());
			scaledMask = maskImage.scaledToHeight(height());
			toWidth = false;
		}

		//Draw texture and then maskImage.
		painter.drawImage(drawRect,	scaledTexture, drawRect);
		painter.drawImage(drawRect, scaledMask, drawRect);
	}
}

void
graphicsWidget::drawTo(const QPoint endpoint)
{
	if (maskImage.isNull()) return; //Do not draw if no image.

	//Calculate the scale of the image for coordinate transformation.
	float scale;
	if (toWidth)
	{
		scale = ((float) textureWidth)/((float) width()); 
	}
	else
	{
		scale = ((float) textureHeight)/((float) height());
	}

	//Open the painter on the maskImage.
	QPainter painter (&maskImage);
	//Allows us to paint without overlapping and to erase.
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	//Setup the pen.
	QPen toolPen;
	if (drawMode)
	{
		toolPen.setColor(maskColor);
	}
	else
	{
		toolPen.setColor(emptyColor);	
	}
	toolPen.setWidth(toolSize*scale);
	toolPen.setCapStyle(Qt::RoundCap);
	toolPen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(toolPen);
	
	//Draw the line.
	painter.drawLine(capturedPos*scale, endpoint*scale);
	modified = true;

	//Update the view.
	update();
}

void
graphicsWidget::mousePressEvent(QMouseEvent* event)
{
	if ((drawMode || eraseMode) && Qt::LeftButton == event->button())
	{
		capturedPos = event->pos();
		drawing = true;	
	}
}

void
graphicsWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (drawing && (Qt::LeftButton & event->buttons()))
	{
		drawTo(event->pos());
		capturedPos = event->pos();
	}
}

void
graphicsWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (drawing && (Qt::LeftButton & event->buttons()))
	{
		drawTo(event->pos());
		drawing = false;
	}
}

bool
graphicsWidget::isModified()
{
	return modified;
}

void
graphicsWidget::setFile(QString filename)
{
	QBitArray mask; //to hold the mask temporarily.

	//Load file.
	if (filename.endsWith("mt",
		Qt::CaseInsensitive)) //file ext is MT or mt
	{
		//Delete any already loaded files.
		delete fileContents;

		//Load new file
		fileContents = RangeImage::import(filename);
		if (NULL == fileContents)
		{
			QMessageBox::critical(this, tr("Error"),
				tr("MT File failed to load."));
			return;
		}
		texture = fileContents->getTexture();
		mask = fileContents->getMask();
	}
	else
	{
		QMessageBox::warning(this, tr("Error"),
			tr("Invalid file format.\n"
			"Format must be .mt."));
		return;
	}

	//Load texture width and height.
	textureWidth = texture.width();
	textureHeight = texture.height();

	//Encode mask to QImage.
	//This will render the masked portion as transparent coral for now.
	maskImage = QImage (textureWidth, textureHeight,
		QImage::Format_ARGB32); //_Premultiplied);
	for (int i = 0; i < textureHeight; ++i)
	{
		for (int j = 0; j < textureWidth; ++j)
		{
			//Masked region is 0
			//Kept (non-masked) region is 255
			if (!mask.testBit((textureWidth*i) + j)) //If mask is 0.
			{
				maskImage.setPixel(j, i, maskColor.rgba());
			}
			else
			{
				maskImage.setPixel(j, i, emptyColor.rgba());
			}
		}
	}

	//Rotate the images for easier viewing.
	QMatrix rot;
	rot.rotate(90);
	maskImage = maskImage.transformed(rot);
	texture = texture.transformed(rot);
	int temp = textureWidth;
	textureWidth = textureHeight;
	textureHeight = temp;

    update();  //Refresh widget.
}

bool
graphicsWidget::saveFile(QString filename)
{
    if (NULL == fileContents) return false;  

	//Decode mask from maskImage.
	//Undo the rotation.
	QMatrix rot;
	rot.rotate(-90);
	maskImage = maskImage.transformed(rot);
	int temp = textureWidth;
	textureWidth = textureHeight;
	textureHeight = temp;
	//Create mask, initialized to false.
	QBitArray mask (textureWidth*textureHeight, false); 
	for (int i = 0; i < textureHeight; ++i)
	{
		for (int j = 0; j < textureWidth; ++j)
		{
			if (maskImage.pixel(j, i) == emptyColor.rgba())
			{
				mask.setBit((textureWidth*i) + j);
			}
		}
	}
	//Redo the rotation.
	rot = QMatrix();
	rot.rotate(90);
	maskImage = maskImage.transformed(rot);
	temp = textureWidth;
	textureWidth = textureHeight;
	textureHeight = temp;

	//Create new range image and delete old one.
	//Make mutable copies of the depth, texture, csys.
	QVector<float> newDepth (fileContents->getDepth());
	QImage newTexture (fileContents->getTexture());
	QMatrix4x4 newCsys (fileContents->getCoordinateSystemMatrix());

	//Make the new data structure.
	RangeImage* newContents = new RangeImage(
		fileContents->getWidth(),
		fileContents->getHeight(),
		fileContents->getPixelSizeX(),
		fileContents->getPixelSizeY(),
		newDepth,
		newTexture,
		mask,
		newCsys);
	delete fileContents;
	fileContents = newContents;

	//Run connected components to avoid floating data islands.
	Clean cleaner;
	newContents = cleaner.connectedComponents(fileContents);
	delete fileContents;
	fileContents = newContents;

	//Perform any desired cleaning functions prior to save.
	cleaningDialog options;
	options.exec();
	if (options.getSpike())
	{
		qDebug() << "Removing spikes and filling holes.";
		//This redoes the conncomps if necessary.
		newContents = cleaner.spikeRemovalHoleFilling(
			fileContents);
		delete fileContents;
		fileContents = newContents;
	}
	if (options.getDetrend())
	{
		qDebug() << "Detrending....";
		newContents = cleaner.detrend(fileContents);
		delete fileContents;
		fileContents = newContents;
	}
	if (options.getComputeCsys())
	{
		if (options.getCentroid())
		{
			qDebug() << "Computing a centroid coordinate system.";
			newContents = cleaner.coordinateSystem2Centroid(
				fileContents);
			delete fileContents;
			fileContents = newContents;
		}
		else
		{
			qDebug() << "Computing a tip coordinate system.";
			newContents = cleaner.coordinateSystem4Tip(
				fileContents);
			delete fileContents;
			fileContents = newContents;
		}
	}

	//Save
	int success = fileContents->save(filename);
	if (!success) return false;

	//If we got here, the save was successful.
	modified = false;
	return true;
}

void
graphicsWidget::toggleDraw()
{
	drawMode = !drawMode;
}

void
graphicsWidget::toggleErase()
{
	eraseMode = !eraseMode;
}

void
graphicsWidget::readSettings()
{
	//Read settings from settings file.
	//If the setting does not exist, initialize it.
	toolSize = settings.value("toolSize", 1.00).toDouble();
}
