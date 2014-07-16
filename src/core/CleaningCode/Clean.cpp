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

#include "Clean.h"
#include "FastConnectComponent.h"
#include "SpikeRemoval.h"
#include "FilterImage.h"
#include <cmath>
#include "ComputeFlatScrewdriverTipCsys.h"
#include "FindValidMarkRange.h"
#include "ManipulatePlate.h"

Clean::Clean(QObject* parent): QObject(parent)
{
}

Clean::~Clean()
{
}

//members:

//Returns the maximum color component in the QRgb.
int
Clean::maxGray(QRgb color)
{
	int maxRet = qRed(color);	
	if (qBlue(color) > maxRet)
		maxRet = qBlue(color);
	if (qGreen(color) > maxRet)
		maxRet = qGreen(color);
	return maxRet;
}

//Threshold based on the quality map and the texture.
//Does not delete tip pointer.
QBitArray
Clean::threshold(RangeImage* tip, const QImage& qualityMap, 
	int quality_threshold, int texture_threshold)
{
	//Cache some items from the tip.
	int width = tip->getWidth();
	int height = tip->getHeight();
	QBitArray mask (tip->getMask()); //Make a copy of the mask.
	QImage texture = tip->getTexture(); 
	int qPixel, tPixel;
	
	//Modify the mask copy.
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (mask.testBit(width*i + j)) //If the pixel is on:
			{
				//Grab the grayscale value of the pixel quality.
				//TODO: qGray solution
				qPixel = qGray(qualityMap.pixel(j, i)); 

				//Grab the grayscale value of the pixel texture.
				//TODO: Dr. Zhang's solution
				tPixel = maxGray(texture.pixel(j, i)); 
				
				//If the pixel doesn't meet the threshold,
				//turn it off.
				if ((tPixel < texture_threshold) ||
					(qPixel > quality_threshold))
					mask.clearBit(width*i + j);
			}
		}
	}

	return mask;
}

//Finds the border of the unmasked regions and chips away at it.
/*
 * @param lx Number of pixels to be chipped away on left boundary.
 * @param rx Number of pixels to be chipped away on right boundary.
 *
 * @param ty Number of pixels to be chipped away on top boundary.
 * @param by Number of pixels to be chipped away on bottom boundary.
 */
void 
Clean::removeAdditionalBoundary(unsigned char* maskData, 
	int imageWidth, int imageHeight, int lx, int rx, int ty, int by)
{
	// left and right
	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth - lx; j++)
		{
			int id = i * imageWidth + j;
			if (maskData[id])
			{
				
				memset(&maskData[id], 0, sizeof(maskData[0])*lx);
				j = imageWidth+1;
			}
		}

		for (int j = imageWidth-1; j > rx; j--)
		{
			int id = i * imageWidth + j;
			if (maskData[id])
			{
				
				memset(&maskData[id-rx+1], 0, sizeof(maskData[0])*rx);
				j = -1;
			}
		}
	}

	// top and bottom
	for (int i = 0; i < imageWidth; i++)
	{
		for (int j = 0; j < imageHeight - ty; j++)
		{
			int id = j * imageWidth + i;
			if (maskData[id])
			{
				for (int k = 0; k < ty; k++)
				{
					maskData[(j+k)*imageWidth + i] = 0;	
				}
				j = imageHeight;
			}
		}

		for (int j = imageHeight-1; j > by; j--)
		{
			int id = j * imageWidth + i;
			if (maskData[id])
			{
				for (int k = 0; k < by; k++)
				{
					maskData[(j-by+k+1)*imageWidth + i] = 0;
				}
				j = by;
			}
		}
	}
}

//slots:

RangeImage*
Clean::cleanFlatScrewdriverTip(RangeImage* tip, 
	const QImage& qualityMap, int quality_threshold, 
	int texture_threshold)
{
	//Threshold the tip's mask with texture and quality.
	QBitArray mask = threshold(tip, qualityMap, 
		quality_threshold, texture_threshold);

	//Cache some data.
	int width = tip->getWidth();
	int height = tip->getHeight();
	float pixelSizeX = tip->getPixelSizeX();
	float pixelSizeY = tip->getPixelSizeY();
	QImage texture = tip->getTexture();
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(tip->getDepth());

	//Copy the tip mask into an unsigned char array with 255 as "on."
	QVector<unsigned char> ucharMask;
	for (int i = 0; i < mask.size(); ++i)
		ucharMask.push_back(255*mask.testBit(i));

	//Use connected components to eliminate all but
	//the largest unmasked area.
	CFastConnectComponent conComp(width, height);
	QVector<unsigned char> ucharMaskCopy (ucharMask);
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;
	
	//Laura - Not sure about this.
	//If zData is large spike in positive or 
	//negative direction, set mask and zData there to 0.
	//for (int i = 0; i < mask.size(); ++i)
	//{
	//	if (fabs(depth[i]) > 1E9)
	//	{
	//		depth[i] = 0;
	//		ucharMask[i] = 0;
	//	}
	//}

	//// remove spikes based on thresholding
	//For now, this doesn't modify anything but
	//the mask you pass in.
	QVector<float> xData, yData;
	for (int j = 0; j < width; ++j)
		xData.push_back(j*pixelSizeX);
	for (int i = 0; i < height; ++i)
		yData.push_back(i * pixelSizeY);
	CSpikeRemoval spikeRemover;
	//If the last arg is false, this may modify the depth.
	spikeRemover.PolyLineRemoval(xData.data(), yData.data(),
		depth.data(), ucharMask.data(), 100, 100, 7,
		width, height, true);

	//// determine the fast component again
	ucharMaskCopy = ucharMask;
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;

	//Fill holes in depth data.
	//Only fills holes with width less than or equal
	//to the last argument to FillHolesFloat.
	//FYI: Modifies depth, of course.
	CFilterImage filter(width, height);
	filter.FillHolesFloat(depth.data(), ucharMask.data(), 20);

	// eat in additional few points from outside
	removeAdditionalBoundary(ucharMask.data(), 
		width, height, 20, 20, 20, 20);

	//Convert results to QBitArray and make new tip.
	//If the bit is on in the mask and off in ucharMask,
	//turn it off in the mask.
	//Moreover, if the bit is off in the mask and on in
	//ucharMask, it means that a hole has been filled.
	//Turn it on in the mask, too.
	for(int i = 0; i < ucharMask.size(); ++i)
	{
		if ((mask.testBit(i)) && (0 == ucharMask[i]))
		{
			mask.clearBit(i);
		}
		if ((!mask.testBit(i)) && (255 == ucharMask[i]))
		{
			mask.setBit(i);
		}
	}

	//Compute coordinate system matrix.
	//Assume scan at 45 degrees for now.
	//May change in the future.
	ComputeFlatScrewdriverTipCsys computeCsys (width, height,
		pixelSizeX, pixelSizeY, depth, mask);
	QMatrix4x4 csys = computeCsys.getCoordinateSystemMatrix();

	return new RangeImage(width, height, pixelSizeX, pixelSizeY,
		depth, texture, mask, csys);
}

RangeImage*
Clean::cleanFlatScrewdriverTip(RangeImage* tip, 
	QString qualityMapFilename, int quality_threshold,
	int texture_threshold)
{
	QImage qualityMap;
	if (!qualityMap.load(qualityMapFilename))
		return NULL;
	return cleanFlatScrewdriverTip(tip, qualityMap, quality_threshold,
		texture_threshold);
}

RangeImage* 
Clean::cleanStriatedLeadMark(RangeImage* plate, const QImage& qualityMap)
{
	//Cache some data.
	int width = plate->getWidth();
	int height = plate->getHeight();
	float pixelSizeX = plate->getPixelSizeX();
	float pixelSizeY = plate->getPixelSizeY();
	QImage texture = plate->getTexture();
	QBitArray mask (plate->getMask()); //Make a copy of the mask.
	//Make a copy of the coordinate system.
	QMatrix4x4 coordinateSystem (plate->getCoordinateSystemMatrix());
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(plate->getDepth());

	//Make a grayscale texture copy.
	QVector<unsigned char> grayTexture;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//Should be row major, according to PngFileIO.cpp.
			//Int output from maxGray should be between 0 and 255.
			int grayValue = maxGray(texture.pixel(j, i));
			if ((255 < grayValue) || (0 > grayValue))
			{
				qDebug() << "Plate texture was not in range" <<
					" [0, 255].";
			}
			grayTexture.push_back((unsigned char) grayValue);
		}
	}

	//Copy the mask into an unsigned char array with 255 as "on."
	QVector<unsigned char> ucharMask;
	for (int i = 0; i < mask.size(); ++i)
	{
		ucharMask.push_back(255*mask.testBit(i));
	}

	//Make a grayscale quality map copy.
	QVector<unsigned char> qualityData;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//Should be row major, according to PngFileIO.cpp.
			//Int output from maxGray should be between 0 and 255.
			int grayValue = maxGray(qualityMap.pixel(j, i));
			if ((255 < grayValue) || (0 > grayValue))
			{
				qDebug() << "Plate quality map was not in range" <<
					" [0, 255].";
			}
			qualityData.push_back((unsigned char) grayValue);
		}
	}

	//Find the marked area, mask off the rest.
	CFindValidMarkRange markRange(width, height);
	float markAngle = 0;
	markRange.LocateMarkRegion(grayTexture.data(), 
		qualityData.data(), ucharMask.data(), markAngle);
	
	//Median filter the data.
	CFilterImage filter(width, height);
	//Make a copy of the mask.
	QVector<unsigned char> ucharMaskCopy (ucharMask);
	filter.MedianFilterU8(ucharMask.data(), 
		ucharMaskCopy.data(), 5);
	//Since we median filtered the mask, we need
	//to make sure that no bad points became classified
	//as good points by mistake.
	for (int i = 0; i < width*height; ++i)
	{
		if(255 != ucharMask[i])
			ucharMaskCopy[i] = 0;
	}

	//Find the connected components.
	qDebug() << "Finding connected components....";
	CFastConnectComponent conComp(width, height);
	conComp.FastConnectAlg(ucharMaskCopy.data(),
		ucharMask.data(), 0, 255);
	
	//Remove spikes. WARNING: This modifies depth.
	qDebug() << "Median filtering the z data....";
	filter.MedianFilterFloat(depth.data(), depth.data(),
		5, ucharMask.data());

	//Fill holes.  WARNING: This modifies depth.
	qDebug() << "Filling holes....";
	filter.FillHolesFloat(depth.data(), ucharMask.data(), 50);

	//Translate changes to the mask.
	//If the bit is on in the mask and off in ucharMask,
	//turn it off in the mask.
	//Moreover, if the bit is off in the mask and on in
	//ucharMask, it means that a hole has been filled.
	//Turn it on in the mask, too.
	for(int i = 0; i < ucharMask.size(); ++i)
	{
		if ((mask.testBit(i)) && (0 == ucharMask[i]))
		{
			mask.clearBit(i);
		}
		if ((!mask.testBit(i)) && (255 == ucharMask[i]))
		{
			mask.setBit(i);
		}
	}

	//Detrending operation.
	//Fit a plane to the data, and then subtract it from
	//the depth.  WARNING: This modifies depth, but it
	//is for statistical purposes.
	qDebug() << "Removing plane slope....";
	ManipulatePlate maniPlate(plate, depth.data());

	//Compute the coordinate system.
	qDebug() << "Computing coordinate system....";
	//Center at centroid.
	QVector3D centroid;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int id = i*width + j;
			if (mask.testBit(id))
			{
				centroid += QVector3D(
					j*pixelSizeX,
					i*pixelSizeY,
					depth[id]);
			}
		}
	}
	//Number added in was number of "on" points.
	centroid /= mask.count(true);
	coordinateSystem.translate(-centroid);
	
	return new RangeImage(width, height, pixelSizeX, pixelSizeY, 
		depth, texture, mask, coordinateSystem);
}

RangeImage*
Clean::cleanStriatedLeadMark(RangeImage* plate,
	QString qualityMapFilename)
{
	QImage qualityMap;
	if (!qualityMap.load(qualityMapFilename))
		return NULL;
	
	return cleanStriatedLeadMark(plate, qualityMap);
}

RangeImage* 
Clean::cleanSlipJointPliersMark(RangeImage* mark, 
	const QImage& qualityMap, int quality_threshold,
	int texture_threshold)
{
	//Threshold the mark's mask with texture and quality.
	QBitArray mask = threshold(mark, qualityMap, 
		quality_threshold, texture_threshold);

	//Cache some data.
	int width = mark->getWidth();
	int height = mark->getHeight();
	float pixelSizeX = mark->getPixelSizeX();
	float pixelSizeY = mark->getPixelSizeY();
	QImage texture = mark->getTexture();
	//Make a copy of the coordinate system.
	QMatrix4x4 coordinateSystem (mark->getCoordinateSystemMatrix());
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(mark->getDepth());

	//Copy the mark mask into an unsigned char array with 255 as "on."
	QVector<unsigned char> ucharMask;
	for (int i = 0; i < mask.size(); ++i)
		ucharMask.push_back(255*mask.testBit(i));

	//Use connected components to eliminate all but
	//the largest unmasked area.
	CFastConnectComponent conComp(width, height);
	QVector<unsigned char> ucharMaskCopy (ucharMask);
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;
	
	//Laura - Not sure about this, this was on in original code.
	//If zData is large spike in positive or 
	//negative direction, set mask and zData there to 0.
	//for (int i = 0; i < mask.size(); ++i)
	//{
	//	if (fabs(depth[i]) > 1E9)
	//	{
	//		depth[i] = 0;
	//		ucharMask[i] = 0;
	//	}
	//}

	//// remove spikes based on thresholding
	//For now, this doesn't modify anything but
	//the mask you pass in.
	QVector<float> xData, yData;
	for (int j = 0; j < width; ++j)
		xData.push_back(j*pixelSizeX);
	for (int i = 0; i < height; ++i)
		yData.push_back(i * pixelSizeY);
	CSpikeRemoval spikeRemover;
	//If the last arg is false, this may modify the depth.
	spikeRemover.PolyLineRemoval(xData.data(), yData.data(),
		depth.data(), ucharMask.data(), 100, 100, 7,
		width, height, true);

	//// determine the fast component again
	ucharMaskCopy = ucharMask;
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;

	//Fill holes in depth data.
	//Only fills holes with width less than or equal
	//to the last argument to FillHolesFloat.
	//FYI: Modifies depth, of course.
	CFilterImage filter(width, height);
	filter.FillHolesFloat(depth.data(), ucharMask.data(), 20);

	// eat in additional few points from outside
	removeAdditionalBoundary(ucharMask.data(), 
		width, height, 10, 10, 10, 10);

	//Convert results to QBitArray and make new mark.
	//If the bit is on in the mask and off in ucharMask,
	//turn it off in the mask.
	//Moreover, if the bit is off in the mask and on in
	//ucharMask, it means that a hole has been filled.
	//Turn it on in the mask, too.
	for(int i = 0; i < ucharMask.size(); ++i)
	{
		if ((mask.testBit(i)) && (0 == ucharMask[i]))
		{
			mask.clearBit(i);
		}
		if ((!mask.testBit(i)) && (255 == ucharMask[i]))
		{
			mask.setBit(i);
		}
	}

	//Center at centroid.
	QVector3D centroid;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int id = i*width + j;
			if (mask.testBit(id))
			{
				centroid += QVector3D(
					j*pixelSizeX,
					i*pixelSizeY,
					depth[id]);
			}
		}
	}
	//Number added in was number of "on" points.
	centroid /= mask.count(true);
	coordinateSystem.translate(-centroid);

	return new RangeImage(width, height, pixelSizeX, pixelSizeY,
		depth, texture, mask, coordinateSystem);
}

RangeImage*
Clean::cleanSlipJointPliersMark(RangeImage* mark, 
	QString qualityMapFilename, int quality_threshold,
	int texture_threshold)
{
	QImage qualityMap;
	if (!qualityMap.load(qualityMapFilename))
		return NULL;
	
	return cleanSlipJointPliersMark(mark, qualityMap,
		quality_threshold, texture_threshold);
}

RangeImage*
Clean::detrend(RangeImage* mark)
{
	//Cache some data.
	int width = mark->getWidth();
	int height = mark->getHeight();
	float pixelSizeX = mark->getPixelSizeX();
	float pixelSizeY = mark->getPixelSizeY();
	QImage texture = mark->getTexture();
	//Make a copy of the mask.
	QBitArray mask (mark->getMask());
	//Make a copy of the coordinate system.
	QMatrix4x4 coordinateSystem (mark->getCoordinateSystemMatrix());
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(mark->getDepth());

	//Detrending operation.
	//Fit a plane to the data, and then subtract it from
	//the depth.  WARNING: This modifies depth, but it
	//is for statistical purposes.
	qDebug() << "Removing plane slope....";
	ManipulatePlate maniPlate(mark, depth.data());

	return new RangeImage(width, height, pixelSizeX, pixelSizeY, 
		depth, texture, mask, coordinateSystem);
}

RangeImage*
Clean::coordinateSystem2Centroid(RangeImage* mark)
{
	//Cache some data.
	int width = mark->getWidth();
	int height = mark->getHeight();
	float pixelSizeX = mark->getPixelSizeX();
	float pixelSizeY = mark->getPixelSizeY();
	QImage texture = mark->getTexture();
	//Make a copy of the mask.
	QBitArray mask (mark->getMask());
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(mark->getDepth());

	//Compute the coordinate system.
	qDebug() << "Computing coordinate system....";
	//Center at centroid.
	QVector3D centroid;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int id = i*width + j;
			if (mask.testBit(id))
			{
				centroid += QVector3D(
					j*pixelSizeX,
					i*pixelSizeY,
					depth[id]);
			}
		}
	}
	//Number added in was number of "on" points.
	centroid /= mask.count(true);
	QMatrix4x4 coordinateSystem;
	coordinateSystem.translate(-centroid);
	
	return new RangeImage(width, height, pixelSizeX, pixelSizeY, 
		depth, texture, mask, coordinateSystem);
}

RangeImage*
Clean::coordinateSystem4Tip(RangeImage* tip)
{
	//Cache some data.
	int width = tip->getWidth();
	int height = tip->getHeight();
	float pixelSizeX = tip->getPixelSizeX();
	float pixelSizeY = tip->getPixelSizeY();
	QBitArray mask (tip->getMask());
	QImage texture = tip->getTexture();
	QVector<float> depth (tip->getDepth());

	//Compute coordinate system matrix.
	//Assume scan at 45 degrees for now.
	//May change in the future.
	ComputeFlatScrewdriverTipCsys computeCsys (width, height,
		pixelSizeX, pixelSizeY, depth, mask);
	QMatrix4x4 csys = computeCsys.getCoordinateSystemMatrix();

	return new RangeImage(width, height, pixelSizeX, pixelSizeY, 
		depth, texture, mask, csys);
}

RangeImage*
Clean::connectedComponents(RangeImage* data)
{
	//Cache some data.
	int width = data->getWidth();
	int height = data->getHeight();
	float pixelSizeX = data->getPixelSizeX();
	float pixelSizeY = data->getPixelSizeY();
	QImage texture = data->getTexture();
	QVector<float> depth (data->getDepth());
	QMatrix4x4 csys (data->getCoordinateSystemMatrix());
	//Make a mutable copy of the mask.
	QBitArray mask (data->getMask());

	//Copy the mask into an unsigned char array with 255 as "on."
	QVector<unsigned char> ucharMask;
	for (int i = 0; i < mask.size(); ++i)
		ucharMask.push_back(255*mask.testBit(i));

	//Use connected components to eliminate all but
	//the largest unmasked area.
	CFastConnectComponent conComp(width, height);
	QVector<unsigned char> ucharMaskCopy (ucharMask);
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;
	
	//Convert results to QBitArray.
	//If the bit is on in the mask and off in ucharMask,
	//turn it off in the mask.
	//Moreover, if the bit is off in the mask and on in
	//ucharMask, it means that a hole has been filled.
	//Turn it on in the mask, too.
	for(int i = 0; i < ucharMask.size(); ++i)
	{
		if ((mask.testBit(i)) && (0 == ucharMask[i]))
		{
			mask.clearBit(i);
		}
		if ((!mask.testBit(i)) && (255 == ucharMask[i]))
		{
			mask.setBit(i);
		}
	}

	return new RangeImage(width, height, pixelSizeX, pixelSizeY, 
		depth, texture, mask, csys);
}

RangeImage*
Clean::spikeRemovalHoleFilling(RangeImage* tip)
{
	//Cache some data.
	int width = tip->getWidth();
	int height = tip->getHeight();
	float pixelSizeX = tip->getPixelSizeX();
	float pixelSizeY = tip->getPixelSizeY();
	QImage texture = tip->getTexture();
	//Copy the depth into a mutable vector.
	QVector<float> depth = QVector<float>(tip->getDepth());
	//Copy the mask into a mutable bit array.
	QBitArray mask (tip->getMask());
	//Copy the coordinate system.
	QMatrix4x4 csys (tip->getCoordinateSystemMatrix());

	//Copy the tip mask into an unsigned char array with 255 as "on."
	QVector<unsigned char> ucharMask;
	for (int i = 0; i < mask.size(); ++i)
		ucharMask.push_back(255*mask.testBit(i));

	//// remove spikes based on thresholding
	//For now, this doesn't modify anything but
	//the mask you pass in.
	QVector<float> xData, yData;
	for (int j = 0; j < width; ++j)
		xData.push_back(j*pixelSizeX);
	for (int i = 0; i < height; ++i)
		yData.push_back(i * pixelSizeY);
	CSpikeRemoval spikeRemover;
	//If the last arg is false, this may modify the depth.
	spikeRemover.PolyLineRemoval(xData.data(), yData.data(),
		depth.data(), ucharMask.data(), 100, 100, 7,
		width, height, true);

	//// determine the fast component again
	CFastConnectComponent conComp(width, height);
	QVector<unsigned char> ucharMaskCopy (ucharMask);
	conComp.FastConnectAlg(ucharMask.data(),
		ucharMaskCopy.data(), 0, 255);
	ucharMask = ucharMaskCopy;

	//Fill holes in depth data.
	//Only fills holes with width less than or equal
	//to the last argument to FillHolesFloat.
	//FYI: Modifies depth, of course.
	CFilterImage filter(width, height);
	filter.FillHolesFloat(depth.data(), ucharMask.data(), 20);

	// eat in additional few points from outside
	removeAdditionalBoundary(ucharMask.data(), 
		width, height, 20, 20, 20, 20);

	//Convert results to QBitArray and make new tip.
	//If the bit is on in the mask and off in ucharMask,
	//turn it off in the mask.
	//Moreover, if the bit is off in the mask and on in
	//ucharMask, it means that a hole has been filled.
	//Turn it on in the mask, too.
	for(int i = 0; i < ucharMask.size(); ++i)
	{
		if ((mask.testBit(i)) && (0 == ucharMask[i]))
		{
			mask.clearBit(i);
		}
		if ((!mask.testBit(i)) && (255 == ucharMask[i]))
		{
			mask.setBit(i);
		}
	}

	return new RangeImage(width, height, pixelSizeX, pixelSizeY,
		depth, texture, mask, csys);
}
