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

#ifndef __RANGEIMAGE_H__
#define __RANGEIMAGE_H__
#include <QObject>
#include <QString>
#include <QVector>
#include <QImage>
#include <QBitArray>
#include <QMatrix4x4>
#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptContext>
#include "Profile.h"

/**
 * A class for reading in range image files (*.mt, .al3d, .txyz), 
 * storing their data,
 * and presenting a common interface for accessing that data.
 * Range image files have a regularly spaced, rectangular grid of 
 * depth values, associated pixel sizes in X and Y, a mask,
 * a texture, and a coordinate system.
 *
 * Note: This class is immutable.
 *
 * @author Laura Ekstrand
 */

class RangeImage: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth)
	Q_PROPERTY(int height READ getHeight)
	Q_PROPERTY(float pixelSizeX READ getPixelSizeX)
	Q_PROPERTY(float pixelSizeY READ getPixelSizeY)

  protected:
	//Member variables.
	bool null;  ///< True if internal data is inconsistent or uninitialized.

	//File members.
	int width; ///< Width of reconstructed 3D data
	int height; ///< Height of reconstructed 3D data
	float pixelSizeX; ///< Pixel size in the width
	float pixelSizeY; ///< Pixel size in the height
	///The Z point data in the order @f$ Z_1Z_2Z_3 @f$....
	QVector<float> depth;
	///The texture.
	QImage texture;
	///The mask point data in the order @f$ M_1M_2M_3 @f$....
	QBitArray mask;
	///Matrix defining the object coordinate system.
	QMatrix4x4 coordinateSystem;

	//Check to make sure the data is consistent.
	bool isConsistent();

  public:
	///Load from .mt file.
	RangeImage(const QString& fname, QObject *parent = 0);
	///Load straight from prepared data.
	RangeImage(int w, int h, float pixX, float pixY, 
		QVector<float> zdata, QImage tex2D, QBitArray maskdata,
		QMatrix4x4& csys, QObject* parent = 0);
	///Copy constructor.
	RangeImage(const RangeImage& other, QObject *parent = 0);
	virtual ~RangeImage();

	//Public static import functions.
	///Import data from TXYZ, AL3D, or MT
	/**
	 * For your convenience.
	 *
	 * This function automatically determines the file type 
	 * based on the 
	 * file extension for the QString fname.
	 * If the file is a TXYZ or MT, 
	 * just leave off the texfname parameter when calling
	 * this function. AL3D requires the separate texture file name.
	 */
	static RangeImage* import(const QString& fname, 
		const QString& texfname = "");
	///Wraps import, allows import to be scripted.
	static QScriptValue scriptableImport(QScriptContext* context, 
		QScriptEngine* engine);
	///Import data from a TXYZ file. You own the returned pointer.
	static RangeImage* importFromTxyz(const QString& fname);
	///Import data from an al3d file. You own the returned pointer.
	/**
	 * FYI: This function converts the depth data and 
	 * pixel size from the al3d file from meters to
	 * micrometers.
	 *
	 * WARNING: This may return a null texture.
	 * If no texture is expected inside the file, use
	 * the texfname string to specify an external
	 * texture file to load.
	 *
	 * @param fname AL3D filename.
	 * @param texfname Filename of the external texture image.
	 */
	static RangeImage* importFromAl3d(const QString& fname, 
		const QString& texfname = "");

	//Getters.
	///Get depth. (Implicitly shared.)
	const QVector<float> getDepth() const;
	///Get the texture. (Implicitly shared.)
	const QImage getTexture() const;
	///Get the mask. (Implicitly shared.)
	const QBitArray getMask() const;
	///Get the coordinateSystem matrix.
	inline const QMatrix4x4& getCoordinateSystemMatrix() const
		{return coordinateSystem;}

  public slots:
	//Some functions are here so that you can script them.
	///Save to Mantis Tip File (*.mt).
	bool save(const QString& fname) const;
	///Export the texture (pass through for QImage save).
	inline bool exportTexture(const QString& fname)
		{return texture.save(fname);}
	///Export to OBJ file format.
	bool exportToOBJ(const QString& fname);
	///Downsample by skipping skip rows and columns.
	/**
	 * The returned object is a new downsampled version
	 * of this.  You own the returned object.
	 */
	RangeImage* downsample(int skip);
	///For debugging: saves mask.
	bool saveMask(const QString& fname) const;
	///For debugging: saves depth?
	bool saveDepth(const QString& fname) const;
	
	//Getters.
	///Is the file null?
	inline bool isNull() const {return null;}
	///Get 3D data width.
	inline int getWidth() const {return width;}
	///Get 3D data height.
	inline int getHeight() const {return height;}
	///Get 3D data X pixel size in um.
	inline float getPixelSizeX() const {return pixelSizeX;}
	///Get 3D data Y pixel size in um.
	inline float getPixelSizeY() const {return pixelSizeY;}

	//Retrieve cross-sections.
	///Retrieve a row as a Profile. You are responsible for the pointer.
	/**
	 * Returns NULL if idx is out of range.
	 */
	Profile* getRow(int idx);
	///Retrieve a column as a Profile. You are responsible for the pointer.
	/**
	 * Returns NULL if idx is out of range.
	 */
	Profile* getColumn(int idx);

  signals:
	//Give the GUI a status message.
	void statusMessage(QString msg);
	//Alert the GUI to a problem.
	void warningMessage(QString msg);
};

Q_DECLARE_METATYPE(RangeImage*)

typedef QSharedPointer<RangeImage> PRangeImage;

#endif //!defined __RANGEIMAGE_H__
