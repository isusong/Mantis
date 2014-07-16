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

#include "CleanVirtualMark.h"
#include "FilterImage.h"
#include "../../StatisticsLibrary/base/stats.h"
#include <cmath>

QVector<float>
CleanVirtualMark::unmask(const QVector<float> data, 
	float maskValue)
{
	//Trim the masked data off the ends.
	int dataLength = data.size();

	//Find the edges of the valid data.
	int startIndex, endIndex;
	bool startFound = false;
	bool endFound = false;
	for (int i = 0; i < dataLength; ++i)
	{
		if (!startFound && (maskValue != data[i]))
		{
			startIndex = i;
			startFound = true;
		}

		if (!endFound && (maskValue != data[dataLength - 1 - i]))
		{
			endIndex = dataLength - 1 - i;
			endFound = true;
		}
	}

	//Trim the masked data off the ends, leaving only the middle.
	QVector<float> trimmedData;
	for (int i = startIndex; i < endIndex + 1; ++i)
	{
		trimmedData.push_back(data[i]);
	}

	return trimmedData;
}

void
CleanVirtualMark::depthToUm(QVector<float>& data, float nearClip,
	float farClip, float cameraZ)
{
	//Convert data to um.
	for (int i = 0; i < data.size(); ++i)
	{
		//This first undoes the action of glViewport,
		//and then unprojects the data.
		data[i] = (data[i]*(nearClip - farClip)) - nearClip;

		//Undo camera.
		data[i] = data[i] - cameraZ;
	}
}

QPoint
CleanVirtualMark::findMarkEdges(const QVector<float> data)
{
	//Approximate the derivative with partial differences.
	QVector<float> diff;
	for (int i = 1; i < data.size(); ++i)
	{
		diff.push_back(data[i] - data[i-1]);
	}

	//Filter the derivative to remove gross noise.
	QVector<float> diffCopy (diff);
	CFilterImage filter(diff.size(), 1); //1D derivative.
	filter.GaussianFilterFloat(diff.data(), diffCopy.data(), 15);

	//Convert the filtered derivative to a vector of doubles
	//for the statistics calculation.
	QVector<double> diffDouble;
	for (int i = 0; i < diffCopy.size(); ++i)
		diffDouble.push_back((double) diffCopy[i]);
	//Don't need these anymore.
	diff.clear();
	diffCopy.clear();

	//Locate the edges of the mark using an iterative approach.
	int border = 20; //Minimum to get cut off on each end.
	int startIndex = 0;
	int endIndex = diffDouble.size() - 1;
	for (int j = 0; j < 2; ++j) //Repeat
	{
		//Compute the mean and standard deviation of the filtered
		//derivative using stats.h in the statistics library:
		double mean, stddev;
		//Operate on trimmed copy.
		QVector<double> diffDoubleTrimmed;
		for (int i = startIndex; i < endIndex + 1; ++i)
			diffDoubleTrimmed.push_back(diffDouble[i]);
		meanAndVar(diffDoubleTrimmed.toStdVector(), mean, stddev);
		stddev = sqrt(stddev); //Variance -> stddev.

		//Look for ends where the absolute value of (the 
		//derivative - mean) is greater than stddev.
		bool startFound = false;
		bool endFound = false;
		int oldStartIndex = startIndex;
		int oldEndIndex = endIndex;
		int diffSize = diffDouble.size();
		for (int i = 0; i < diffSize; ++i)
		{
			if (!startFound && 
				(abs(diffDouble[i] - mean) > stddev))
			{
				startIndex = i + 1;
			}
			else if (i > border)
				startFound = true;

			if (!endFound && 
				(abs(diffDouble[diffSize-1-i] - mean) > stddev))
			{
				endIndex = diffSize - 1 - i - 1;
			}
			else if (i > border)
				endFound = true;
		}

		//If the new start and end points were 
		//never found, return original start and end points.
		if (!startFound || !endFound) //something's wrong
		{
			int startIndex = oldStartIndex;
			int endIndex = oldEndIndex;
			break; //don't repeat.
		}

	}

	return QPoint(startIndex, endIndex);
}


