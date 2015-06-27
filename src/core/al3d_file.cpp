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
 * Authors: Jason Ekstrand, Laura Ekstrand (ldmil@iastate.edu)
 */

#include "al3d_file.h"
#include <QtDebug>
#include <cfloat>
#include <QtCore/QStringList>
#include <cstring>
#include <QDataStream>
#include "logger.h"

AL3DFile::AL3DFile():
    null(true), depth_data(NULL)
{ }

AL3DFile::AL3DFile(const QString& filename):
    null(true), depth_data(NULL)
{
    load(filename);
}

AL3DFile::~AL3DFile()
{
    delete [] depth_data;
}

void
AL3DFile::load(QFile * file)
{
	//Reset object.
    clear();

	//Grab type string to make sure it is an al3d file.
    char type[17];
    qint64 bytesRead = file->read(type, 17);
	if (17 != bytesRead) //Fatal error.
	{
		qDebug() << file->fileName() <<
			": Error reading file identification header.";
		return;
	}
    if (! strncmp(type, "AliconaImaging\r\n", 17)) {
        // Wrong file type
		qDebug() << file->fileName() <<
			" is not an .al3D file.";
        return;
    }
	
	//Read tags.
    //Read the first two tags to get the Version
	//and number of tags.
	bool success;
    for (int i = 0; i < 2; ++i)
        success = read_tag(file);

	if (!success) //Fatal error.
	{
		qDebug() << file->fileName() <<
			": Error reading file identification header.";
		return;
	}

    int tag_count = get_tag("TagCount").toInt();

    for (int i = 0; i < tag_count; ++i) {
        success = read_tag(file);
    }

	if (!success)
	{
		qDebug() << file->fileName() <<
			": File is missing information.";
	}

    //Get the user-defined comment.
    comment = QString(file->read(256));
    
	//Get the icon if there is one.
    QVariant icon_offset = get_tag("IconOffset");
    if ((! icon_offset.isNull()) && (icon_offset.toLongLong() != 0)
		&& (file->pos() <= icon_offset.toLongLong())) {
        
        file->seek(icon_offset.toLongLong());
        success = read_icon(file);
		if (!success)
		{
			qDebug() << file->fileName() <<
				": Icon failed to read properly.";
			icon = QImage();
		}
    }

	//Retrieve the size of the data.
	QVariant cols = get_tag("Cols");
	QVariant rows = get_tag("Rows");
    image_size = QSize(cols.toInt(), rows.toInt());
	if ((cols.isNull()) || (rows.isNull()) ||
		(image_size.width() <= 0) || (image_size.height() <= 0))
	{
		qDebug() << file->fileName()
			<< ": Invalid data width or height.";
		return;
	}

	//Retrieve the pixel size.
	QVariant pixX = get_tag("PixelSizeXMeter");
	QVariant pixY = get_tag("PixelSizeYMeter");
	pixelSizeX = pixX.toFloat();
	pixelSizeY = pixY.toFloat();
	if ((pixX.isNull()) || (pixY.isNull()) ||
		(pixelSizeX < FLT_MIN) || (pixelSizeY < FLT_MIN))
	{
		qDebug() << file->fileName()
			<< ": Invalid pixel sizes.";
		return;
	}

	//Get the depth data and mask.
    QVariant depth_data_offset = get_tag("DepthImageOffset");
    if ((! depth_data_offset.isNull()) && (depth_data_offset.toLongLong() != 0)) {
        file->seek(depth_data_offset.toLongLong());
        success = read_depth_data(file);
		if (!success) //fatal error
		{
			qDebug() << file->fileName() <<
				": Depth data failed to read properly.";
			delete [] depth_data;
			depth_data = NULL;
		}
		else
		{
			QVariant maskedValue = get_tag("InvalidPixelValue");
			if (!maskedValue.isNull())
			{
				//Populate mask.
				//Init mask to size of image and fill with false.
				int numel = image_size.width()
					*image_size.height();	
				mask = QBitArray(numel);
				//Set valid data points to true
				for(int i = 0; i < numel; ++i)
				{
					if (maskedValue.toFloat() !=
						depth_data[i])
					{
						mask.setBit(i);
					}
				}
			}
		}
    }

	//Get the texture data.
    QVariant texture_offset = get_tag("TextureImageOffset");
    QVariant planes = get_tag("NumberOfPlanes");
    if ((!texture_offset.isNull()) && (texture_offset.toLongLong() != 0)
            && (!planes.isNull()) && (planes.toInt() > 0)) {
        file->seek(texture_offset.toLongLong());
        for (int i = 0; i < planes.toInt(); ++i) {
            success = read_image_plane(file);
			if (!success)
			{
				qDebug() << file->fileName() <<
					": Texture data plane " << i
					<< " failed to read properly";
			}
        }
    }

	//Successful read.
    null = false;
}

//=======================================================================
//=======================================================================
bool AL3DFile::loadIconOnly(const QString& filename)
{
    const char *funcname = "AL3DFile::loadIconOnly() -";
    std::string sfile = filename.toStdString();
    QFile qfile(filename);
    if (!qfile.open(QIODevice::ReadOnly))
    {
        LogError("%s failed to open the file %s", funcname, sfile.c_str());
        return false;
    }

    QFile *file = &qfile;



    //Reset object.
    clear();

    //Grab type string to make sure it is an al3d file.
    char type[17];
    qint64 bytesRead = file->read(type, 17);
    if (17 != bytesRead) //Fatal error.
    {
        LogError("%s Error reading file identification header for file %s", funcname, sfile.c_str());
        return false;
    }
    if (! strncmp(type, "AliconaImaging\r\n", 17))
    {
        // Wrong file type
        LogError("%s %s is not an .al3D file.", funcname, sfile.c_str());
        return false;
    }

    //Read tags.
    //Read the first two tags to get the Version
    //and number of tags.
    bool success;
    for (int i = 0; i < 2; ++i)
    {
        success = read_tag(file);
    }

    if (!success) //Fatal error.
    {
        LogError("%s Error reading file identification header for file %s", funcname, sfile.c_str());
        return false;
    }

    int tag_count = get_tag("TagCount").toInt();

    for (int i = 0; i < tag_count; ++i)
    {
        success = read_tag(file);
    }

    if (!success)
    {
        LogError("%s Error missing information for file %s", funcname, sfile.c_str());
    }

    //Get the user-defined comment.
    comment = QString(file->read(256));

    //Get the icon if there is one.
    QVariant icon_offset = get_tag("IconOffset");
    if ((! icon_offset.isNull()) && (icon_offset.toLongLong() != 0)
        && (file->pos() <= icon_offset.toLongLong()))
    {

        file->seek(icon_offset.toLongLong());
        success = read_icon(file);
        if (!success)
        {
            LogError("%s %s: Icon failed to read properly", funcname, sfile.c_str());
            return false;
        }

        return true;
    }

    LogError("%s %s: No Icon found", funcname, sfile.c_str());
    return false;
}

void
AL3DFile::load(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    load(&file);
}

bool
AL3DFile::isNull()
{
	return null;
}

QVariant
AL3DFile::get_tag(const QString& tag)
{
	return tags.value(tag, QVariant().toString());
}

QImage
AL3DFile::get_image_by_tag(const QString& tag)
{
	//Check tag
    QVariant tag_val = get_tag(tag);
    // If we don't have that tag, return a null image
    if (tag_val.isNull())
        return QImage();
    QString ptr_str = tag_val.toString();
    // If we have an empty tag, return a null image
    if (ptr_str.isEmpty())
        return QImage();

	//Valid tag, split it.
    QStringList strings = ptr_str.split(";");
	//Do what the tag says
    if (strings.size() == 0) { //If no planes
        return QImage(); //return null image
    }
	else if (strings.size() == 1) { //If one plane
		//Return grayscale image.
        // TODO: Check for valid int
        return image_planes.at(strings.at(0).toInt());
    } 
	else if (strings.size() == 3) { //If 3 planes

		//Assemble an RGB image
        int planes[3];
        for (int i = 0; i < 3; ++i) {
            planes[i] = strings.at(i).toInt();
            // TODO: Check for valid int
        }

        // Create a new image
        QImage image(image_size, QImage::Format_RGB888);
        uchar * image_data = image.bits();

        // Interlace the image data
        int width = image_size.width();
        int height = image_size.height();
		int stride = image.bytesPerLine();
        for (int p = 0; p < 3; ++p) {
            QImage plane = image_planes.at(planes[p]);
			int plane_stride = plane.bytesPerLine();
            const uchar * plane_data = plane.bits();

            for (int r = 0; r < height; ++r) {
                for (int c = 0; c < width; ++c) {
                    image_data[r * stride + 3*c + p] =
                            plane_data[r * plane_stride + c];
                }
            }
        }

        return image;
    } 
	else { //Something is very wrong.
		qDebug() << tag <<
			" has an invalid number of image planes.\n"
			<< "Returning null QImage.";
        return QImage();
    }
}

QString
AL3DFile::getComment()
{
	return comment;
}

int
AL3DFile::getWidth()
{
	return image_size.width();
}

int
AL3DFile::getHeight()
{
	return image_size.height();
}

float
AL3DFile::getPixelSizeX()
{
	return pixelSizeX;
}

float
AL3DFile::getPixelSizeY()
{
	return pixelSizeY;
}

const float *
AL3DFile::getDepth()
{
	return depth_data;
}

QImage
AL3DFile::getIcon()
{
	return icon;
}

QBitArray
AL3DFile::getMask()
{
	return mask;
}

void
AL3DFile::clear()
{
    delete [] depth_data;
    depth_data = NULL;
	mask.clear();

    tags.clear();
    icon = QImage();
    image_planes.clear();
}

bool
AL3DFile::read_tag(QFile * file)
{
	//Read tag from file.
    al3d_tag_data tag_data;
    qint64 ret = file->read((char *)&tag_data, 52);
    if (ret < 52)
	{
		qDebug() << file->fileName() << ": Tag misread.";
        return false; //not much else we can do at this point.
	}

	//Insert successfully read tag into hash table.
    tags.insert(QString(tag_data.key), QString(tag_data.value));
	return true;
}

bool
AL3DFile::read_icon(QFile * file)
{
	int iconSize = 150;
	int iconSize8 = 152; //152 for 8-byte alignment.

    icon = QImage(QSize(iconSize, iconSize), QImage::Format_RGB888);
    uchar * icon_data = icon.bits();
	int stride = icon.bytesPerLine();
    for (int p = 0; p < 3; ++p) { //3 planes.
        for (int r = 0; r < iconSize; ++r) { //number of rows
            // Read a row
            char* row = new char[iconSize8];
            qint64 bytesRead = file->read(row, iconSize8);
			if (bytesRead != iconSize8)
			{
				delete [] row;
				return false; //failure to properly read icon
			}
            // Fill the row
            for (int c = 0; c < iconSize; ++c) { //150 cols.
                icon_data[r * stride + (3*c + p)] = row[c];
            }
			delete [] row;
        }
    }

	return true; //success
}

bool
AL3DFile::read_depth_data(QFile * file)
{
	//Make a datastream to ensure correct endianness and precision.
	QDataStream stream (file);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream.setFloatingPointPrecision(
		QDataStream::SinglePrecision);

    // Cache these
    int width = image_size.width();
    int height = image_size.height();

	//8 byte alignment.
    int line_width = width;
    // Padd to 8 bytes
    if (line_width % 2) //If odd
        ++line_width; //Make it even.

    // re-allocate the depth data
    depth_data = new float[width * height];

	//Read in the depth data.
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < line_width; ++j)
		{
			if (j < width)
			{
				stream >> depth_data[i*width + j];
			}
			else
			{
				float junk;
				stream >> junk;
			}
		}
	}

	if (stream.status() != QDataStream::Ok)
	{
		qDebug() << file->fileName() 
			<< ": Either read past end or"
			<< " read corrupt data.";
		return false;
	}
	else
	{	
		return true; //success
	}
}

bool
AL3DFile::read_image_plane(QFile * file)
{
    // Cache these
    int width = image_size.width();
    int height = image_size.height();

    int line_width = width;
    // Pad to 8 bytes
    if (line_width % 8)
        line_width = line_width + (8 - (line_width % 8));

    // Create a new QImage
    QImage image(image_size, QImage::Format_Indexed8);
    image.setColorCount(256);
    // Set the color table
    for (int i = 0; i < 256; ++i)
        image.setColor(i, qRgb(i, i, i));
	//Get pointer to image data in QImage.
    uchar * image_data = image.bits();
	int stride = image.bytesPerLine();

    // Create storage space
    char * buffer = new char[line_width];

    for (int r = 0; r < height; ++r) {
        qint64 bytesRead = file->read(buffer, line_width);
		if (line_width != bytesRead)
		{
			return false; //problem reading data.
		}

        // I could use memcpy here, but I'm not sure what it would do with
        // alignment, so I'll leave it alone.
		memcpy(image_data + (r * stride), buffer, line_width);
    }

    delete [] buffer;

    // Add it to the list
    image_planes.append(image);

	return true;
}

