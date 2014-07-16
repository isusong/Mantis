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
 *
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#include "RangeImage.h"
#include <QFile>
#include <QDataStream>
#include "al3d_file.h"
#include <cstring>
#include <cfloat>
#include <QFileInfo>
#include <QTextStream>
#include <cmath>

#define CONVERT 1E6 ///Convert al3d from meters to um

RangeImage::RangeImage(const QString& fname, QObject* parent):
	QObject(parent)
{
	//Init members
	null = true;
	
	//Report status as loading.
	QString status ("Loading ");
	status.append(fname);
	qDebug() << status;
	emit statusMessage(status);

	//Open the file
	QFile file (fname);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning;
		emit warningMessage(warning);
		return;
	}

	//Serializer object for QT classes.
	QDataStream fileReader(&file);
	fileReader.setVersion(QDataStream::Qt_4_8);
	fileReader.setFloatingPointPrecision(QDataStream::
		SinglePrecision);

	//Check fileID and version.
	QString fileID;
	fileReader >> fileID;
	if (QString("Mantis Tip File") != fileID)
	{
		QString warning (fname);
		warning.append(": Not a Mantis Tip File.");
		qDebug() << warning;
		emit warningMessage(warning);
		return;
	}
	qint32 version;
	fileReader >> fileID; //get rid of the "Version: " string.
	fileReader >> version;
	if ((1 != version) && (2 != version))
	{
		QString versionWarning ("This file is .mt version ");
		versionWarning.append(QString::number((int) version));
		versionWarning.append(". Only versions 1 and 2 are supported.");
		qDebug() << versionWarning;
		emit warningMessage(versionWarning);
		return;
	}

	//Read in the data.
	//Version 1 assumes pixelSizeX = pixelSizeY.
	//Version 2 does not make this assumption.
	if (1 == version)
	{
		float pixelSize;
		fileReader >> width >> height >> pixelSize;
		pixelSizeX = pixelSize;
		pixelSizeY = pixelSize;
	}
	else
	{
		fileReader >> width >> height >> 
			pixelSizeX >> pixelSizeY;
	}
	fileReader >> depth;
	fileReader >> texture;
	fileReader >> mask;
	fileReader >> coordinateSystem;

	file.close();

	//Check to see if loaded data is consistent.
	null = !isConsistent();
}

RangeImage::RangeImage(int w, int h, float pixX,
	float pixY, QVector<float> zdata, QImage tex2D, 
	QBitArray maskdata, QMatrix4x4& csys, QObject* parent):
	QObject(parent)
{
	//Data assignment
	width = w;
	height = h;
	pixelSizeX = pixX;
	pixelSizeY = pixY;
	depth = zdata;
	texture = tex2D;
	mask = maskdata;
	coordinateSystem = csys;

	//Member assignment
	null = !isConsistent();
}

RangeImage::RangeImage(const RangeImage& other,
	QObject *parent):
	QObject(parent)
{
	//Data assignment
	width = other.getWidth();
	height = other.getHeight();
	pixelSizeX = other.getPixelSizeX();
	pixelSizeY = other.getPixelSizeY();
	depth = other.getDepth();
	texture = other.getTexture();
	mask = other.getMask();
	coordinateSystem = other.getCoordinateSystemMatrix();

	//Member assignment
	null = !isConsistent();
}

RangeImage::~RangeImage()
{

}

bool
RangeImage::isConsistent()
{
	//Is the data valid?
	if ((depth.size() != width*height) ||
		(texture.width() != width) ||
		(texture.height() != height) ||
		(mask.size() != width*height))
	{
		QString warning ("Data loaded is invalid.");
		warning.append(" Width and height");
		warning.append(" are inconsistent with the depth, texture");
		warning.append(" and/or mask.");
		qDebug() << warning;
		emit warningMessage(warning);
		return false;
	}
	else return true;
}

bool
RangeImage::save(const QString& fname) const
{
	//Report status.
	QString status ("Saving ");
	status.append(fname);
	qDebug() << status;

	//Open the file.
	QFile file (fname);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning;
		return false;
	}

	//Serializer object for QT classes.
	QDataStream fileWriter(&file);
	fileWriter.setVersion(QDataStream::Qt_4_8);
	fileWriter.setFloatingPointPrecision(QDataStream::
		SinglePrecision);

	//File header.
	fileWriter << QString("Mantis Tip File"); 
	fileWriter << QString("Version: ") << (qint32) 2; 
	fileWriter << (qint32) width << 
		(qint32) height << pixelSizeX << pixelSizeY;

	//File body.
	fileWriter << depth;
	fileWriter << texture;
	fileWriter << mask;
	fileWriter << coordinateSystem;

	file.close();

	return true;
}

bool
RangeImage::saveMask(const QString& fname) const
{
	QImage testMask (width, height, QImage::Format_Indexed8);
	testMask.setColorCount(2);
	testMask.setColor(0, qRgb(0, 0, 0));
	testMask.setColor(1, qRgb(255,255, 255));
	testMask.fill(0); //Start with black.

	//Turn on the appropriate white pixels.
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (mask.testBit(width*i + j))
			{
				testMask.setPixel(j, i, 1);
			}
		}
	}

	return testMask.save(fname);
}

bool
RangeImage::saveDepth(const QString& fname) const
{
	QImage graydepth (width, height, QImage::Format_Indexed8);
	graydepth.setColorCount(256);
	for (int i = 0; i < 256; ++i)
		graydepth.setColor(i, qRgb(i, i, i));
	graydepth.fill(0);

	//Find the depth range.
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;
	for (int i = 0; i < width*height; ++i)
	{
		if (mask.testBit(i))
		{
			float currentZ = depth[i];
			if (currentZ < minZ)
				minZ = currentZ;
			if (currentZ > maxZ)
				maxZ = currentZ;
		}
	}

	//Turn on the appropriate white pixels.
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (mask.testBit(width*i + j))
			{
				int colorIdx = 
					floor(255*((depth[width*i + j] - minZ)/(maxZ - minZ)));
				graydepth.setPixel(j, i, colorIdx);
			}
		}
	}

	return graydepth.save(fname);
}

const QVector<float>
RangeImage::getDepth() const
{
	return depth;
}

const QImage
RangeImage::getTexture() const
{
	return texture;
}

const QBitArray
RangeImage::getMask() const
{
	return mask;
}

RangeImage*
RangeImage::import(const QString& fname, const QString& texfname)
{
	QFileInfo info (fname);
	QString suffix = info.suffix();
	if (0 == suffix.localeAwareCompare("mt"))
	{
		RangeImage* ret = new RangeImage(fname);
		if (ret->isNull())
		{
			delete ret;
			return NULL;
		}
		else
			return ret;
	}
	else if (0 == suffix.localeAwareCompare("al3d"))
		return importFromAl3d(fname, texfname);
	else if (0 == suffix.localeAwareCompare("txyz"))
		return importFromTxyz(fname);
	else
	{
		qDebug() << "Unrecognized file format: " << suffix;
		return NULL;
	}
}

QScriptValue
RangeImage::scriptableImport(QScriptContext* context,
	QScriptEngine* engine)
{
	int argc = context->argumentCount();
	if (argc < 1)
	{
		qDebug() << "Incorrect number of arguments to import.";
		qDebug() << "Correct number of arguments is 1 or 2.";
		qDebug() << "Returning a null object.";
		return QScriptValue();
	}

	QString argument0 = context->argument(0).toString();
	//If there exists an argument 1:
	if (2 == argc)
	{
		QString argument1 = context->argument(1).toString();
		return engine->newQObject(import(argument0, argument1), 
			QScriptEngine::AutoOwnership, 
			QScriptEngine::AutoCreateDynamicProperties);
	}
	else
	{
		return engine->newQObject(import(argument0), 
			QScriptEngine::AutoOwnership, 
			QScriptEngine::AutoCreateDynamicProperties);
	}
}

RangeImage* RangeImage::importFromTxyz(const QString& fname)
{
	//Report status
	QString status ("Importing ");
	status.append(fname);
	status.append(" from Txyz");
	qDebug() << status;
	//emit statusMessage(status);

	//This is code inspired by Song Zhang's TxyzFileIO class.
	FILE* fp;
	fp = fopen(fname.toStdString().c_str(), "rb");
	if (NULL == fp) //If filestream is bad
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning;
		//emit warningMessage(warning);
		return NULL;
	}

	//Parameters that are needed from the file.
	int width, height;
	float pixelSize;
	QVector<float> depth;
	QImage texture;
	QBitArray mask;
	QMatrix4x4 coordinateSystem;

	//Grab size of file and pixel size.
	fscanf(fp, "image size width x height x pixelSize = %d x %d x %f",
		&width, &height, &pixelSize);
	int imageSize = width * height;

	//Read bw texture data and convert to QImage.
	unsigned char * bwTexture = new unsigned char [imageSize];
	fread(bwTexture, sizeof(unsigned char), imageSize, fp);
	QImage temp = QImage (bwTexture, width, height, 
		sizeof(unsigned char) * width, QImage::Format_Indexed8);
	//Need to copy here because temp is only valid as long
	//as bwTexture exists, and I want to delete bwTexture soon.
	texture = temp.copy();
	//Set the color table to B/W.
	texture.setColorCount(256);
	for (int i = 0; i < 256; ++i) //set the color table.
		texture.setColor(i, qRgb(i, i, i));

	//Read depth data.
	float * depthData = new float [imageSize];
	fread(depthData, sizeof(float), imageSize, fp);
	for (int i = 0; i < imageSize; ++i)
		depth.push_back(depthData[i]);
	delete [] depthData;

	//Interpret the mask from the texture.
	mask.fill(false, imageSize);
	for (int i = 0; i < imageSize; ++i)
	{
		if (bwTexture[i]) //If the texture is nonzero.
			mask.setBit(i); //Set the mask there to true.
	}
	delete [] bwTexture;

	//Clean up.
	fclose(fp);
	
	return new RangeImage(width, height, pixelSize, pixelSize,
		depth, texture, mask, coordinateSystem);
}

RangeImage*
RangeImage::importFromAl3d(const QString& fname, 
	const QString& texfname)
{
	//Report status.
	QString status ("Importing ");
	status.append(fname);
	status.append(" from al3d.");
	qDebug() << status;
	//emit statusMessage(status);

	//Parameters that are needed from the file.
	int width, height;
	float pixelSizeX, pixelSizeY;
	QVector<float> depth;
	QImage texture;
	QBitArray mask;
	QMatrix4x4 coordinateSystem;

	//Create an al3d object for the file.
	AL3DFile al3d (fname);
	if (al3d.isNull()) 
	{
		QString warning ("Error importing al3d file ");
		warning.append(fname);
		qDebug() << warning;
		//emit warningMessage(warning);
		return NULL;
	}

	//Get width, height, pixelSize.
	width = al3d.getWidth();
	height = al3d.getHeight();
	pixelSizeX = al3d.getPixelSizeX()*CONVERT;
	pixelSizeY = al3d.getPixelSizeY()*CONVERT;
	if (pixelSizeX != pixelSizeY)
	{
		qDebug() << fname << ": pixelSizeX != pixelSizeY.";
	}

	//Get depth.
	//al3d object owns the depthData pointer and deletes it.
	const float * depthData = al3d.getDepth();
	if (!depthData)
	{
		QString warning (fname);
		warning.append(" does not contain depth data.");
		qDebug() << warning;
		//emit warningMessage(warning);
		return NULL;
	}
	for (int i = 0; i < width*height; ++i)
		depth.push_back(depthData[i]*CONVERT);

	//Get texture
	if (texfname.isEmpty()) //I haven't specified an external texture.
	{
		//Grab the texture from the file.
		if (!al3d.get_image_by_tag("TexturePtr").isNull())
			texture = al3d.get_image_by_tag("TexturePtr");
		else //error, no texture info.
		{
			QString warning (fname);
			warning.append(" does contain 2D texture");
			warning.append(" and no additional texture file was specified.");
			qDebug() << warning;
			//emit warningMessage(warning);
			return NULL;
		}
	}
	else
	{
		//Load texture from external file.
		if (!texture.load(texfname)) 
		{
			QString msg (texfname);
			msg.append(": Invalid file or filename.");
			qDebug() << msg;
			//emit warningMessage(msg);
			return NULL;
		}
	}

	//Get mask.
	mask = al3d.getMask();
	if (mask.isEmpty())
		mask = QBitArray(width*height, true);

	//Success
	return new RangeImage(width, height, pixelSizeX, pixelSizeY,
		depth, texture, mask, coordinateSystem);
}

bool
RangeImage::exportToOBJ(const QString& fname)
{
	//Report status.
	QString status ("Exporting to obj file ");
	status.append(fname);
	qDebug() << status;

	//Open the file.
	QFile file (fname);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning;
		return false;
	}

	//Do meshing (populate indices).
	QVector<int> indices;
	//Go through the spaces between the rows.
	for (int i = 0; i < height - 1; ++i)
	{
		//Go through the spaces between the columns.
		for (int j = 0; j < width - 1; ++j)
		{
			//Zoom in on a point 0 at i, j and its
			//neighbor on the right, point 1, its
			//neighbor below, point 2, and its
			//neighbor below and to the right, point 3.
			//Like this:
			// 0  1
			// 2  3
			int index0 = width*i + j;
			int index1 = index0 + 1;
			int index2 = index0 + width;
			int index3 = index2 + 1;

			//Can I connect point 1 to point 2?
			if (mask.testBit(index1) & mask.testBit(index2))
			{
				// I use the default / method to divide the 
				//triangles.
				if(mask.testBit(index0)) //Can I connect 0 to 1 and 2?
				{
					//If so, I can make a triangle out of them.
					indices.push_back(index0);
					indices.push_back(index1);
					indices.push_back(index2);
				}
				if(mask.testBit(index3)) //Can I connect 3 to 1 and 2?
				{
					indices.push_back(index2);
					indices.push_back(index3);
					indices.push_back(index1);
				}
			}
			else if (mask.testBit(index0) & mask.testBit(index3)) //Can I connect 0 and 3?
			{
				//I use the \ method the divide the triangles.
				if(mask.testBit(index1)) //Can I connect 1 to 0 and 3?
				{
					indices.push_back(index1);
					indices.push_back(index3);
					indices.push_back(index0);
				}
				if(mask.testBit(index2)) //Can I connect 2 to 0 and 3?
				{
					indices.push_back(index2);
					indices.push_back(index3);
					indices.push_back(index0);
				}
			}
		}
	}

	//Serializer object for QT classes.
	QTextStream fileWriter(&file);
	//File body.
	//vertices and texture coordinates
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			fileWriter << "v " 
				<< j*pixelSizeX << " " 
				<< i*pixelSizeY << " " 
				<< depth[width*i + j] << " "
				<< "\n";
			fileWriter << "vt " 
				<< ((1/((float) width)) * (j + 0.5f)) //s
				<< " "
				<< ((1/((float) height))* (i + 0.5f)) //t
				<< " \n";
		}
	}
	//faces
	int ntri = indices.size()/3;
	for (int i = 0; i < ntri; ++i)
	{
		int index1 = indices[(3*i) + 0] + 1;
		int index2 = indices[(3*i) + 1] + 1;
		int index3 = indices[(3*i) + 2] + 1;
		fileWriter << "f " << index1 << " "
			<< index2 << " "
			<< index3 << " \n";
	}

	file.close();

	return true;
}

RangeImage*
RangeImage::downsample(int skip)
{
	int dWidth = width/skip;
	int dHeight = height/skip;
	QVector<float> dDepth;
	QImage dTexture (dWidth, dHeight, texture.format());
	QBitArray dMask (dWidth*dHeight, true);

	for (int i = 0; i < dHeight; ++i)
	{
		for (int j = 0; j < dWidth; ++j)
		{
			int id = width*i*skip + j*skip;
			dDepth.push_back(depth[id]);
			//slow, but precise and simple
			dTexture.setPixel(j, i, texture.pixel(j*skip, i*skip));
			if (!mask.testBit(id)) dMask.clearBit(dWidth*i + j);
		}
	}
	
	return new RangeImage(dWidth, dHeight, skip*pixelSizeX,
		skip*pixelSizeY, dDepth, dTexture, dMask,
		coordinateSystem);
}

Profile*
RangeImage::getRow(int idx)
{
	if ((idx < 0) || (idx > height - 1))
		return NULL;

	//Populate depth and mask.
	QVector<float> row;
	QBitArray rowMask (width, true);
	for (int j = 0; j < width; ++j)
	{
		row.push_back(depth[width*idx + j]);
		if (!mask.testBit(width*idx + j))
			rowMask.clearBit(j);
	}

	return new Profile(pixelSizeX, row, rowMask);
}

Profile*
RangeImage::getColumn(int idx)
{
	if ((idx < 0) || (idx > width - 1))
		return NULL;
	
	//Populate depth and mask.
	QVector<float> column;
	QBitArray colMask (height, true);
	for (int i = 0; i < height; ++i)
	{
		column.push_back(depth[width*i + idx]);
		if (!mask.testBit(width*i + idx))
			colMask.clearBit(i);
	}

	return new Profile(pixelSizeY, column, colMask);
}
