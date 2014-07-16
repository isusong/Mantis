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

#ifndef __AL3D_FILE_H__
#define __AL3D_FILE_H__

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtGui/QImage>
#include <QtCore/QSize>
#include <QBitArray>

/**
 * Class for reading in an .al3d file.
 * Special thanks to Jason Ekstrand for mostly writing
 * this one. I added extra functions, comments, and
 * readied it for use.
 *
 * @author Jason & Laura Ekstrand
 */

class AL3DFile
{
	//Private member variables.
    bool null; ///< Is this a null file?
	///Stores tags and values
    QHash<QString, QString> tags;
	///User-defined comment
    QString comment;
	///Size of file.
    QSize image_size;
	///Z depth values as @f$ Z_1Z_2Z_3 @f$....
    float * depth_data;
	///File Icon
    QImage icon;
	///File image planes.
    QList<QImage> image_planes;
	///File mask.
	QBitArray mask;
	///Pixel Size in X
	float pixelSizeX;
	///Pixel Size in Y
	float pixelSizeY;

	//Private member functions.
	///Empty the file object of all of its contents.
    void clear();
	///Load data from QFile
    void load(QFile * file);
	///Read a tag.
    bool read_tag(QFile * file);
	///Read in the file icon.
    bool read_icon(QFile * file);
	///Read the depth data
    bool read_depth_data(QFile * file);
	///Read in one grayscale image plane.
    bool read_image_plane(QFile * file);

  public:
	///Construct an empty file object.
    AL3DFile();
	///Construct a file object containing filename.
    AL3DFile(const QString& filename);
    virtual ~AL3DFile();

	//Structure for holding al3d tags.
	struct al3d_tag_data
	{
		char key[20];
		char value[30];
		char end_of_line[2];
	};

	///Load filename into the file object.
    void load(const QString& filename);

	//Getters and setters
	///Determine if AL3DFile is valid
	bool isNull();
	///Retrieve tag from QHash table, returning null QVariant if not found.
    QVariant get_tag(const QString& tag);
	///Grab a particular image, e.g. "TexturePtr" image.
    QImage get_image_by_tag(const QString& tag);
	///Returns file comment (probably nothing).
	QString getComment();
	///Get the data width. If null=true, this will be garbage.
	int getWidth();
	///Get the data height. If null=true, this will be garbage.
	int getHeight();
	///Get the x pixel size. If null=true, this will be garbage.
	float getPixelSizeX();
	///Get the y pixel size. If null=true, this will be garbage.
	float getPixelSizeY();
	///Get the depth data. Returns NULL if no depth data.
	/**
	 * FYI: The pointer you get is still owned by this object. Do not delete it.
	 */
	const float * getDepth();
	///Get icon. Returns null QImage if there is no icon.
	QImage getIcon();
	///Get mask. Returns empty QBitArray if there is no depth data or InvalidPixelValue.
	QBitArray getMask(); //It's ok to pass this around - it's copy on write.
};

#endif // ! defined __AL3D_FILE_H__

