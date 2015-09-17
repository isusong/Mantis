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

#ifndef __CLEAN_H__
#define __CLEAN_H__
#include "../RangeImage.h"
#include <QObject>

class IProgress;

/**
 * This class forms a fake namespace that you can use with Qt Script.
 * These functions take in RangeImage files, clean them up, and affix
 * appropriate coordinate systems to them. 
 * They do not delete the pointer passed in.
 *
 * @author Laura Ekstrand
 */

class Clean: public QObject
{
	Q_OBJECT

  public:
	///Make a cleaner.
	Clean(QObject* parent = 0);
	virtual ~Clean();

  public slots:
	//These are slots so you can script them.

	//Higher-level functions.

	///For tips. You are responsible for the returned pointer.
	RangeImage* cleanFlatScrewdriverTip(RangeImage* tip, 
		const QImage& qualityMap, int quality_threshold = 200,
		int texture_threshold = 20);
	///Calls cleanFlatScrewdriverTip after reading qualityMapFilename
	/**
	 * For your convenience.
	 * Throws NULL if qualityMapFilename cannot be opened.
	 */
	RangeImage* cleanFlatScrewdriverTip(RangeImage* tip, 
		QString qualityMapFilename, int quality_threshold = 200,
		int texture_threshold = 20);

	///For plates. You are responsible for the returned pointer.
	RangeImage* cleanStriatedLeadMark(RangeImage* plate, const QImage& qualityMap);
	///Calls cleanStriatedLeadMark after reading qualityMapFilename
	/**
	 * For your convenience.
	 * Throws NULL if qualityMapFilename cannot be opened.
	 */
	RangeImage* cleanStriatedLeadMark(RangeImage* plate, 
		QString qualityMapFilename);
	
	///For slip-joint pliers marks. You are responsible for the returned pointer.
	RangeImage* cleanSlipJointPliersMark(RangeImage* mark, 
		const QImage& qualityMap, int quality_threshold = 200,
		int texture_threshold = 10);
	///Calls cleanSlipJointPliersMark after reading qualityMapFilename
	/**
	 * For your convenience.
	 * Throws NULL if qualityMapFilename cannot be opened.
	 */
	RangeImage* cleanSlipJointPliersMark(RangeImage* mark, 
		QString qualityMapFilename, int quality_threshold = 200,
		int texture_threshold = 10);

	//Lower-level functions.

	///Detrending operation.
	/**
	 * Fit a plane to the data, and then subtract it from
	 * the depth.  WARNING: This modifies depth, but it
	 * is for statistical purposes.
	 */
    RangeImage* detrend(RangeImage* mark, IProgress *prog=NULL);

	///Move the coordinate system to the centroid
    RangeImage* coordinateSystem2Centroid(RangeImage* mark, IProgress *prog=NULL);

	///Compute the coordinate system for a tip.
    RangeImage* coordinateSystem4Tip(RangeImage* tip, IProgress *prog=NULL);

	///Perform connected components.
    RangeImage* connectedComponents(RangeImage* data, IProgress *prog=NULL);

	///Perform seventh-order spike removal and hole-filling.
    RangeImage* spikeRemovalHoleFilling(RangeImage* tip, IProgress *prog=NULL);

    int thresholdMask(RangeImage* ri, const QImage *qualityMap, int quality_threshold, int texture_threshold, IProgress *prog=NULL);

protected:
    //Member helper functions.
    ///Returns the maximum color component in the QRgb.
    int maxGray(QRgb color);


    ///Threshold based on quality map and texture.
    /**
     * Does not delete tip pointer.
     */
    QBitArray threshold(RangeImage* tip, const QImage& qualityMap, int quality_threshold, int texture_threshold, int *pixelsTurnedOff=NULL, IProgress *prog=NULL);



    ///Finds the border of the unmasked regions and chips away at it.
    /**
     * @param lx Number of pixels to be chipped away on left boundary.
     * @param rx Number of pixels to be chipped away on right boundary.
     *
     * @param ty Number of pixels to be chipped away on top boundary.
     * @param by Number of pixels to be chipped away on bottom boundary
     */
    void removeAdditionalBoundary(unsigned char* maskData,
        int imageWidth, int imageHeight, int lx, int rx,
        int ty, int by);
};

#endif //!defined __CLEAN_H__
