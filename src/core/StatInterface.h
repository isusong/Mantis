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
 * Authors: Max Morris, Laura Ekstrand (ldmil@iastate.edu)
 */

#ifndef __STATINTERFACE_H__
#define __STATINTERFACE_H__
#include <QObject>
#include <QVector>
#include "Profile.h"
#include <QScriptable>
#include <QScriptValue>

/**
 * Class that communicates with the statistics package to
 * perform the statistics comparison.
 *
 * @author Laura Ekstrand, though large portions of compare were 
 * copied from PrintTrimmedOneOne in the stat package.
 */

class StatInterface: public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(int searchWindow READ getSearchWindow WRITE setSearchWindow)
	Q_PROPERTY(int validWindow READ getValidWindow WRITE setValidWindow)
	Q_PROPERTY(int numRigidPairs READ getNumRigidPairs WRITE setNumRigidPairs)
	Q_PROPERTY(int numRandomPairs READ getNumRandomPairs WRITE setNumRandomPairs)
	Q_PROPERTY(float maxShiftPercentage READ getMaxShiftPercentage WRITE setMaxShiftPercentage)
	Q_PROPERTY(int T_sample_size READ getTSampleSize WRITE setTSampleSize)
	Q_PROPERTY(double rValue READ getRValue)
	Q_PROPERTY(double tValue READ getTValue)
	Q_PROPERTY(int loc1 READ getLoc1)
	Q_PROPERTY(int loc2 READ getLoc2)

  public:
    struct StatConfig
    {
        int searchWindow;
        int validWindow;
        int numRigidPairs;
        int numRandomPairs;
        float maxShiftPercentage;
        int tSampleSize;
    };

  public:
	///Create a parented object.
	StatInterface(QObject *parent = 0);
	virtual ~StatInterface();

	///Performs the statistical comparison.
	/**
	 * May throw std::range_error or std::runtime_error.
	 */
	void compare(QVector<float> data1, QVector<float> data2);
	///Wraps the other compare so that Profile objects can be used.
	/**
	 * Does not delete either pointer.
	 * May throw std::range_error or std::runtime_error.
	 */
    void compare(Profile *data1, Profile *data2);

    StatConfig getConfig();

  public slots:
	///Wraps the comparison for scripting.
	/**
	 * Should take two Profile objects for comparison.
	 * Will throw Qt Script exceptions.
	 */
	QScriptValue compare();

	//Get inputs.
	inline int getSearchWindow() {return searchWindow;}
	inline int getValidWindow() {return validWindow;}
	inline int getNumRigidPairs() {return numRigidPairs;}
	inline int getNumRandomPairs() {return numRandomPairs;}
	inline float getMaxShiftPercentage() {return maxShiftPercentage;}
	inline int getTSampleSize() {return T_sample_size;}

	//Get outputs.
	inline double getTValue() {return tValue;}
	inline double getRValue() {return rValue;}
	inline int getLoc1() {return loc1;}
	inline int getLoc2() {return loc2;}
    inline int getDataLen1() { return _dataLen1; }
    inline int getDataLen2() { return _dataLen2; }

	//Set inputs.
	///Set the size of the search window.
	void setSearchWindow(int width);
	///Set the size of the validation window.
	void setValidWindow(int width);
	///Set the number of rigid pairs.
	void setNumRigidPairs(int num);
	///Set the number of random pairs.
	void setNumRandomPairs(int num);
	///Set the number of T value samples.
	void setTSampleSize(int num);
	///Set the maximum shift percentage (the "leash")
	void setMaxShiftPercentage(double num);

protected:
  //Input settings.
  int searchWindow; ///<Size of search window.
  int validWindow; ///<Size of validation window.
  int numRigidPairs;
  int numRandomPairs;
  /**
   * A value between 0 and 1 to multiply the maximum shift
   * possible between search windows by during the location of
   * the region of maximum correlation. This is an attempt to
   * solve the opposite-end problem.
   */
  float maxShiftPercentage;
  ///How many samples of T do you want (for an averaged T)?
  int T_sample_size;

  //Outputs.
  double rValue, tValue;
  int loc1, loc2;
  int _dataLen1, _dataLen2;

  //Private functions
  ///Convert a QVector<float> to a QVector<double>
  QVector<double> toDouble(const QVector<float> data);
  ///Trim the masked ends off of a Profile and return it as a QVector<float>.
  /**
   * Masked points in the middle of the Profile will be included in the
   * QVector<float>.  This is because the stat package does not understand
   * the concept of masked points.
   *
   * Does not delete pointer.
   */
  QVector<float> trimProfileEnds(Profile *data);
};

Q_DECLARE_METATYPE(StatInterface*)

typedef std::tr1::shared_ptr<StatInterface> PStatInterface;

#endif //!defined __STATINTERFACE_H__
