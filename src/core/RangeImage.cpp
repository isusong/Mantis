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
#include <QPainter>
#include <cmath>
#include "logger.h"

#define CONVERT 1E6 ///Convert al3d from meters to um

//=======================================================================
//=======================================================================
RangeImage::RangeImage(const QString& fname, QObject* parent):
    QObject(parent),
    _dataNull(true)
{
    loadFile(fname);
}

//=======================================================================
//=======================================================================
RangeImage::RangeImage(const QString& fname, bool loadIconOnly, QObject* parent):
    QObject(parent),
    _dataNull(true)
{
    loadFile(fname, loadIconOnly);
}

//=======================================================================
//=======================================================================
RangeImage::RangeImage(int w, int h, float pixX,
	float pixY, QVector<float> zdata, QImage tex2D, 
    QBitArray maskdata, QMatrix4x4& csys, EImgType imgType, QString fileName, QObject* parent):
	QObject(parent)
{
	//Data assignment
    _imgType = imgType;
    _fileName = fileName;
    _width = w;
    _height = h;
    _pixelSizeX = pixX;
    _pixelSizeY = pixY;
    _depth = zdata;
    _texture = tex2D;
    _mask = maskdata;
    _coordinateSystem = csys;

	//Member assignment
    _dataNull = !isConsistent();
}

//=======================================================================
//=======================================================================
RangeImage::RangeImage(const RangeImage& other,
	QObject *parent):
	QObject(parent)
{
	//Data assignment
    _imgType = ImgType_Unk;
    _width = other.getWidth();
    _height = other.getHeight();
    _pixelSizeX = other.getPixelSizeX();
    _pixelSizeY = other.getPixelSizeY();
    _icon = other.getIcon();
    _depth = other.getDepth();
    _texture = other.getTexture();
    _mask = other.getMask();
    _coordinateSystem = other.getCoordinateSystemMatrix();

	//Member assignment
    _dataNull = !isConsistent();
}

//=======================================================================
//=======================================================================
RangeImage::~RangeImage()
{

}

//=======================================================================
//=======================================================================
bool RangeImage::loadFile(const QString& fname, bool iconOnly)
{
    //Init members
    _dataNull = true;
    _imgType = ImgType_Unk;

    //Report status as loading.
    QString status ("Loading ");
    status.append(fname);
    LogInfo("%s", status.toStdString().c_str());
    emit statusMessage(status);

    //Open the file
    QFile file (fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        QString warning ("Error opening file ");
        warning.append(fname);
        LogError("%s", warning.toStdString().c_str());
        emit warningMessage(warning);
        return false;
    }

    //Serializer object for QT classes.
    QDataStream fileReader(&file);
    fileReader.setVersion(QDataStream::Qt_4_8);
    fileReader.setFloatingPointPrecision(QDataStream::
        SinglePrecision);

    int version = readFileVersion(fileReader, fname);
    if (version <= 0) return false;

    //Read in the data.
    //Version 1 assumes pixelSizeX = pixelSizeY.
    //Version 2 does not make this assumption.
    if (version == 1)
    {
        float pixelSize;
        fileReader >> _width >> _height >> pixelSize;
        _pixelSizeX = pixelSize;
        _pixelSizeY = pixelSize;
        guessImgType(fname);
    }
    else if (version <= 3)
    {
        fileReader >> _width >> _height >> _pixelSizeX >> _pixelSizeY;
        guessImgType(fname);
    }
    else
    {
        fileReader >> _width >> _height >> _pixelSizeX >> _pixelSizeY >> _imgType;
        if (_imgType != ImgType_Tip && _imgType != ImgType_Plt)
        {
            LogError("Error loading file: %s, unsupported image type: %d", fname.toStdString().c_str(), _imgType);
            guessImgType(fname);
        }
    }

    // read in the data
    readFileData(fileReader, fname, version, iconOnly);
    file.close();

    if (iconOnly) return true;

    //Check to see if loaded data is consistent.
    _dataNull = !isConsistent();

    _fileName = fname;

    return true;
}

//=======================================================================
//=======================================================================
void RangeImage::guessImgType(const QString& fname)
{
    _imgType = ImgType_Tip;
    if (fname.size() <= 0) return;
    if (fname.at(0) == 'p')
    {
        _imgType = ImgType_Plt;
    }
}

//=======================================================================
//=======================================================================
int RangeImage::getFileVersion(const QString& fname)
{
    //Open the file
    QFile file (fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        LogError("Error opening file: %s", fname.toStdString().c_str());
        return 0;
    }

    //Serializer object for QT classes.
    QDataStream fileReader(&file);
    fileReader.setVersion(QDataStream::Qt_4_8);
    fileReader.setFloatingPointPrecision(QDataStream:: SinglePrecision);

    return readFileVersion(fileReader, fname);
}

//=======================================================================
//=======================================================================
bool RangeImage::readFileData(QDataStream &fileReader, const QString& fname, int version, bool iconOnly)
{
    if (version == 1 || version == 2)
    {
        fileReader >> _depth;
        fileReader >> _texture;
        createIcon();

        if (iconOnly) return true;

        fileReader >> _mask;
        fileReader >> _coordinateSystem;
        return true;
    }
    else if (version >= 3)
    {
        fileReader >> _icon;
        if (iconOnly) return true;

        fileReader >> _depth;
        fileReader >> _texture;
        fileReader >> _mask;
        fileReader >> _coordinateSystem;
        return true;
    }

    LogError("Error loading file: %s, unsupported version: %d", fname.toStdString().c_str(), version);
    return false;
}

//=======================================================================
//=======================================================================
bool RangeImage::createIcon()
{
    if (_texture.width() <=  0 || _texture.height() <= 0)
    {
        _icon = QImage(); // null image
        return false;
    }

    //int w = 150, h = 150;
    int w = 500, h = 500;
    _icon = QImage(w, h, QImage::Format_RGB32);

    QPainter p(&_icon);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.fillRect(0, 0, w, h, Qt::black);

    // compute scale
    QRectF rcDst;
    if (_texture.width() > _texture.height())
    {
        float scale = (float)_texture.height() / (float)_texture.width();
        float dstH = scale *(float)_icon.height();
        float top = ((float)_icon.height() - dstH) / 2.0f;
        rcDst = QRectF(0, top, _icon.width(), dstH);
    }
    else
    {
        float scale = (float)_texture.width() / (float)_texture.height();
        float dstW = scale *(float)_icon.width();
        float left = ((float)_icon.width() - dstW) / 2.0f;
        rcDst = QRectF(left, 0, dstW, _icon.height());
    }

    p.drawImage(rcDst, _texture);
    return true;
}

//=======================================================================
//=======================================================================
int RangeImage::readFileVersion(QDataStream &fileReader, const QString& fname)
{
    //Check fileID and version.
    QString fileID;
    fileReader >> fileID;
    if (QString("Mantis Tip File") != fileID)
    {
        LogError("%s : Not a Mantis Tip File.", fname.toStdString().c_str());
        return 0;
    }

    qint32 version;
    fileReader >> fileID; //get rid of the "Version: " string.
    fileReader >> version;
    if (version < 1 || version > 4)
    {
        LogError("This file is .mt version %d. Only versions 1, 2, 3, 4 are supported.", version);
        return 0;
    }

    return version;
}

//=======================================================================
//=======================================================================
bool RangeImage::isConsistent()
{
	//Is the data valid?
    if ((_depth.size() != _width*_height) ||
        (_texture.width() != _width) ||
        (_texture.height() != _height) ||
        (_mask.size() != _width*_height))
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
//=======================================================================
//=======================================================================
bool RangeImage::save(const QString& fname)
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
    fileWriter.setFloatingPointPrecision(QDataStream::SinglePrecision);

	//File header.
    qint32 version = getCurrentFileVersion();
	fileWriter << QString("Mantis Tip File"); 
    fileWriter << QString("Version: ") << version;
    fileWriter << (qint32) _width << (qint32) _height << _pixelSizeX << _pixelSizeY << (qint32)_imgType;

    // create an icon if we don't have one
    if (_icon.isNull()) createIcon();

	//File body.
    fileWriter << _icon;
    fileWriter << _depth;
    fileWriter << _texture;
    fileWriter << _mask;
    fileWriter << _coordinateSystem;

	file.close();

    _fileName = fname;

	return true;
}

//=======================================================================
//=======================================================================
bool RangeImage::saveMask(const QString& fname) const
{
    QImage testMask (_width, _height, QImage::Format_Indexed8);
    testMask.setColorCount(2);
	testMask.setColor(0, qRgb(0, 0, 0));
	testMask.setColor(1, qRgb(255,255, 255));
	testMask.fill(0); //Start with black.

	//Turn on the appropriate white pixels.
    for (int i = 0; i < _height; ++i)
	{
        for (int j = 0; j < _width; ++j)
		{
            if (_mask.testBit(_width*i + j))
			{
				testMask.setPixel(j, i, 1);
			}
		}
	}

	return testMask.save(fname);
}

//=======================================================================
//=======================================================================
bool RangeImage::saveDepth(const QString& fname) const
{
    QImage graydepth (_width, _height, QImage::Format_Indexed8);
	graydepth.setColorCount(256);
	for (int i = 0; i < 256; ++i)
		graydepth.setColor(i, qRgb(i, i, i));
	graydepth.fill(0);

	//Find the depth range.
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;
    for (int i = 0; i < _width*_height; ++i)
	{
        if (_mask.testBit(i))
		{
            float currentZ = _depth[i];
			if (currentZ < minZ)
				minZ = currentZ;
			if (currentZ > maxZ)
				maxZ = currentZ;
		}
	}

	//Turn on the appropriate white pixels.
    for (int i = 0; i < _height; ++i)
	{
        for (int j = 0; j < _width; ++j)
		{
            if (_mask.testBit(_width*i + j))
			{
				int colorIdx = 
                    floor(255*((_depth[_width*i + j] - minZ)/(maxZ - minZ)));
				graydepth.setPixel(j, i, colorIdx);
			}
		}
	}

	return graydepth.save(fname);
}

//=======================================================================
//=======================================================================
const QVector<float>& RangeImage::getDepth() const
{
    return _depth;
}

//=======================================================================
//=======================================================================
const QImage& RangeImage::getTexture() const
{
    return _texture;
}

//=======================================================================
//=======================================================================
const QBitArray& RangeImage::getMask() const
{
    return _mask;
}

//=======================================================================
//=======================================================================
bool RangeImage::isIconValid() const
{
    return !_icon.isNull();
}
//=======================================================================
//=======================================================================
bool RangeImage::isTextureValid() const
{
    if (_texture.width() <= 0) return false;
    if (_texture.height() <= 0) return false;

    return true;
}

//=======================================================================
//=======================================================================
bool RangeImage::isTip() const
{
    return (_imgType == ImgType_Tip);
}

//=======================================================================
//=======================================================================
bool RangeImage::isPlate() const
{
    return (_imgType == ImgType_Plt);
}

//=======================================================================
//=======================================================================
void RangeImage::setImgType(EImgType type)
{
    _imgType = type;
}

//=======================================================================
//=======================================================================
RangeImage::EImgType RangeImage::getImgType()
{
    return (RangeImage::EImgType)_imgType;
}

//=======================================================================
//=======================================================================
RangeImage* RangeImage::import(const QString& fname, const QString& texfname)
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
        LogError("Unrecognized file format: %s", suffix.toStdString().c_str());
		return NULL;
	}
}

//=======================================================================
//=======================================================================
QScriptValue RangeImage::scriptableImport(QScriptContext* context,
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

//=======================================================================
//=======================================================================
RangeImage* RangeImage::importFromTxyz(const QString& fname)
{
	//Report status
	QString status ("Importing ");
	status.append(fname);
	status.append(" from Txyz");
	qDebug() << status;
	//emit statusMessage(status);

	//This is code inspired by Song Zhang's TxyzFileIO class.
    FILE* fp = NULL;
    fopen_s(&fp, fname.toStdString().c_str(), "rb");
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
    fscanf_s(fp, "image size width x height x pixelSize = %d x %d x %f",
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
	
    return new RangeImage(width, height, pixelSize, pixelSize, depth, texture, mask, coordinateSystem);
}

//=======================================================================
//=======================================================================
RangeImage* RangeImage::importFromAl3d(const QString& fname, const QString& texfname)
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
    return new RangeImage(width, height, pixelSizeX, pixelSizeY, depth, texture, mask, coordinateSystem);
}

//=======================================================================
//=======================================================================
bool RangeImage::exportToOBJ(const QString& fname)
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
    for (int i = 0; i < _height - 1; ++i)
	{
		//Go through the spaces between the columns.
        for (int j = 0; j < _width - 1; ++j)
		{
			//Zoom in on a point 0 at i, j and its
			//neighbor on the right, point 1, its
			//neighbor below, point 2, and its
			//neighbor below and to the right, point 3.
			//Like this:
			// 0  1
			// 2  3
            int index0 = _width*i + j;
			int index1 = index0 + 1;
            int index2 = index0 + _width;
			int index3 = index2 + 1;

			//Can I connect point 1 to point 2?
            if (_mask.testBit(index1) & _mask.testBit(index2))
			{
				// I use the default / method to divide the 
				//triangles.
                if(_mask.testBit(index0)) //Can I connect 0 to 1 and 2?
				{
					//If so, I can make a triangle out of them.
					indices.push_back(index0);
					indices.push_back(index1);
					indices.push_back(index2);
				}
                if (_mask.testBit(index3)) //Can I connect 3 to 1 and 2?
				{
					indices.push_back(index2);
					indices.push_back(index3);
					indices.push_back(index1);
				}
			}
            else if (_mask.testBit(index0) & _mask.testBit(index3)) //Can I connect 0 and 3?
			{
				//I use the \ method the divide the triangles.
                if(_mask.testBit(index1)) //Can I connect 1 to 0 and 3?
				{
					indices.push_back(index1);
					indices.push_back(index3);
					indices.push_back(index0);
				}
                if(_mask.testBit(index2)) //Can I connect 2 to 0 and 3?
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
    for (int i = 0; i < _height; ++i)
	{
        for (int j = 0; j < _width; ++j)
		{
			fileWriter << "v " 
                << j*_pixelSizeX << " "
                << i*_pixelSizeY << " "
                << _depth[_width*i + j] << " "
				<< "\n";
			fileWriter << "vt " 
                << ((1/((float) _width)) * (j + 0.5f)) //s
				<< " "
                << ((1/((float) _height))* (i + 0.5f)) //t
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

//=======================================================================
//=======================================================================
RangeImage* RangeImage::downsample(int skip)
{
    int dWidth = _width/skip;
    int dHeight = _height/skip;
	QVector<float> dDepth;
    QImage dTexture (dWidth, dHeight, _texture.format());
	QBitArray dMask (dWidth*dHeight, true);

	for (int i = 0; i < dHeight; ++i)
	{
		for (int j = 0; j < dWidth; ++j)
		{
            int id = _width*i*skip + j*skip;
            dDepth.push_back(_depth[id]);
			//slow, but precise and simple
            dTexture.setPixel(j, i, _texture.pixel(j*skip, i*skip));
            if (!_mask.testBit(id)) dMask.clearBit(dWidth*i + j);
		}
	}
	
    return new RangeImage(dWidth, dHeight, skip*_pixelSizeX, skip*_pixelSizeY, dDepth, dTexture, dMask, _coordinateSystem, getImgType());
}

//=======================================================================
//=======================================================================
Profile* RangeImage::getRow(int idx)
{
    if ((idx < 0) || (idx > _height - 1))
		return NULL;

	//Populate depth and mask.
	QVector<float> row;
    QBitArray rowMask (_width, true);
    for (int j = 0; j < _width; ++j)
	{
        row.push_back(_depth[_width*idx + j]);
        if (!_mask.testBit(_width*idx + j))
			rowMask.clearBit(j);
	}

    return new Profile(_pixelSizeX, row, rowMask);
}

//=======================================================================
//=======================================================================
Profile* RangeImage::getColumn(int idx)
{
    if ((idx < 0) || (idx > _width - 1))
		return NULL;
	
	//Populate depth and mask.
	QVector<float> column;
    QBitArray colMask (_height, true);
    for (int i = 0; i < _height; ++i)
	{
        column.push_back(_depth[_width*i + idx]);
        if (!_mask.testBit(_width*i + idx))
			colMask.clearBit(i);
	}

    return new Profile(_pixelSizeY, column, colMask);
}

//=======================================================================
//=======================================================================
void RangeImage::logInfo()
{
    LogTrace("RangeImage - width: %d, height: %d, pixelSizeX: %.2f, pixelSizeY: %.2f", _width, _height, _pixelSizeX, _pixelSizeY);
}
