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

#ifndef __PROFILE_H__
#define __PROFILE_H__
#include <QObject>
#include <QString>
#include <QVector>
#include <QBitArray>
#include <QMetaType>

/**
 * A class for reading in profiles (.csv), 
 * storing their data,
 * and presenting a common interface for accessing that data.
 * Profiles consist of a regularly spaced 1D sequence of 
 * depth values, an associated pixel size (ie. resolution) and a mask.
 *
 * Note: This class is immutable.
 *
 * @author Laura Ekstrand
 */

class Profile: public QObject
{
	Q_OBJECT
	Q_PROPERTY(float pixelSize READ getPixelSize)

  protected:
	//Member variables.
	bool null;  ///< True if internal data is inconsistent or uninitialized.

	//File members.
	float pixelSize; ///< Pixel size (sampling resolution)
	///The Z point data in the order @f$ Z_1Z_2Z_3 @f$....
	QVector<float> depth;
	///The mask point data in the order @f$ M_1M_2M_3 @f$....
	QBitArray mask;

	//Check to make sure the data is consistent.
	bool isConsistent();

  public:
	///Load from .csv file.
	Profile(const QString& fname, QObject *parent = 0);
	///Load straight from prepared data.
    Profile(float pix, QVector<float> zdata, QBitArray maskdata,
		QObject* parent = 0);
	///Copy constructor.
	Profile(const Profile& other, QObject* parent = 0);
	virtual ~Profile();

	//Getters.
	///Get depth. (Implicitly shared.)
    const QVector<float>& getDepth() const;
	///Get the mask. (Implicitly shared.)
    const QBitArray& getMask() const;

  public slots:
	//Some functions are here so that you can script them.
	///Save to specially formatted .csv file.
	bool save(const QString& fname) const;
	///Get a flipped copy of this Profile.
	Profile* flip();

	//Getters.
	///Is the file null?
	inline bool isNull() {return null;}
	///Get pixel size (resolution) in um.
	inline float getPixelSize() const {return pixelSize;}
};

Q_DECLARE_METATYPE(Profile*)

typedef std::tr1::shared_ptr<Profile> PProfile;

#endif //!defined __PROFILE_H__
