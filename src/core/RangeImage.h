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

public:
    enum EImgType
    {
        ImgType_Unk = 0,
        ImgType_Min = 1,
        ImgType_Tip = 1,
        ImgType_Plt = 2,
        ImgType_Knf = 3,
        ImgType_Bul = 4,
        ImgType_Max = 4
    };

public:
	///Load from .mt file.
    RangeImage(const QString& fname, QObject *parent = 0);
    RangeImage(const QString& fname, bool iconOnly, QObject *parent = 0);
	///Load straight from prepared data.
    RangeImage(int w, int h, float pixX, float pixY, QVector<float> zdata, QImage tex2D, QBitArray maskdata, QMatrix4x4& csys, EImgType imgType = ImgType_Unk, QString fileName="", QObject* parent = 0);
    ///Copy constructor.
	RangeImage(const RangeImage& other, QObject *parent = 0);
	virtual ~RangeImage();

    bool loadFile(const QString& fname, bool iconOnly=false);
    QString getFileName() const { return _fileName; }
    void setFileName(const QString &fname) { _fileName = fname; }
    static int getFileVersion(const QString& fname);
    static int getCurrentFileVersion() { return 4; }

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
    static RangeImage* importFromAl3d(const QString& fname, const QString& texfname = "");

    ///Get iocn.
    const QImage& getIcon() const { return _icon; }
	///Get depth. (Implicitly shared.)
    const QVector<float>& getDepth() const;
	///Get the texture. (Implicitly shared.)
    const QImage& getTexture() const;
	///Get the mask. (Implicitly shared.)
    void setMask(const QBitArray &ba);
    const QBitArray& getMask() const;
	///Get the coordinateSystem matrix.
    inline const QMatrix4x4& getCoordinateSystemMatrix() const {return _coordinateSystem;}

    bool isIconValid() const;
    bool isTextureValid() const;

    bool isTip() const;
    bool isPlate() const;
    bool isKnife() const;
    bool isBullet() const;
    bool isUnkType() const;

    void setImgType(EImgType type);
    EImgType getImgType() const;
    QString getQualityMapFile() const { return _qualityMapFile; }
    void setQualityMapFile(const QString &file) { _qualityMapFile = file; }

public slots:
	//Some functions are here so that you can script them.
	///Save to Mantis Tip File (*.mt).
    bool save(const QString& fname);
	///Export the texture (pass through for QImage save).
	inline bool exportTexture(const QString& fname)
        {return _texture.save(fname);}
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
    inline bool isNull() const {return _dataNull;}
	///Get 3D data width.
    inline int getWidth() const {return _width;}
	///Get 3D data height.
    inline int getHeight() const {return _height;}
	///Get 3D data X pixel size in um.
    inline float getPixelSizeX() const {return _pixelSizeX;}
	///Get 3D data Y pixel size in um.
    inline float getPixelSizeY() const {return _pixelSizeY;}

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

    virtual void logInfo();

signals:
	//Give the GUI a status message.
	void statusMessage(QString msg);
	//Alert the GUI to a problem.
	void warningMessage(QString msg);

protected:
    static int readFileVersion(QDataStream &fileReader, const QString& fname);
    bool readFileData(QDataStream &fileReader, const QString& fname, int version, bool iconOnly);
    void guessImgType(const QString& fname);
    bool createIcon();

    //Check to make sure the data is consistent.
    bool isConsistent();

protected:
  //Member variables.
  bool _dataNull;  ///< True if internal data is inconsistent or uninitialized.

  //File members.
  QString _fileName;
  int _width; ///< Width of reconstructed 3D data
  int _height; ///< Height of reconstructed 3D data
  float _pixelSizeX; ///< Pixel size in the width
  float _pixelSizeY; ///< Pixel size in the height
  QImage _icon;
  ///The Z point data in the order @f$ Z_1Z_2Z_3 @f$....
  QVector<float> _depth;
  ///The texture.
  QImage _texture;
  ///The mask point data in the order @f$ M_1M_2M_3 @f$....
  QBitArray _mask;
  ///Matrix defining the object coordinate system.
  QMatrix4x4 _coordinateSystem;

  int _imgType;

  QString _qualityMapFile;
};

Q_DECLARE_METATYPE(RangeImage*)

typedef QSharedPointer<RangeImage> PRangeImage;

#endif //!defined __RANGEIMAGE_H__
