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

#include "Profile.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>

Profile::Profile(const QString& fname, QObject* parent):
	QObject(parent)
{
	//Init members
	null = true;
	
	//Report status as loading.
	QString status ("Loading ");
	status.append(fname);
	qDebug() << status.toStdString().c_str();

	//Open the file
	QFile file (fname);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning.toStdString().c_str();
		return;
	}

	//Serializer object for QT classes.
	QTextStream fileReader(&file);

	//Read in the data line by line.
	QString line;
	QStringList lineList;
	//Check for correct file ID.
	line = fileReader.readLine();
	lineList = line.split(",");
	if (QString("Mantis Profile File") != lineList[0])
	{
		QString warning (fname);
		warning.append(": Not a Mantis Profile File.");
		qDebug() << warning.toStdString().c_str();
		return;
	}
	//Check for correct version number.
	line = fileReader.readLine();
	lineList = line.split(",");
	if ((lineList.size() < 2) || (1 != lineList[1].toInt()))
	{
		QString versionWarning ("This file is not Mantis Profile File version 1");
		versionWarning.append(". Only version 1 is supported.");
		qDebug() << versionWarning.toStdString().c_str();
		return;
	}

	//Read in the pixel size.
	line = fileReader.readLine();
	lineList = line.split(",");
	if (lineList.size() < 2)
	{
		QString pixelSizeWarning ("There is no pixel size (resolution) data.");
		qDebug() << pixelSizeWarning.toStdString().c_str();
		return;
	} 
	pixelSize = lineList[1].toFloat();

	//Read in the data.
	QVector<int> tempMask;  //temporary storage for the mask.
	while (!fileReader.atEnd())
	{
		line = fileReader.readLine();
		lineList = line.split(",");
		if ("Depth" == lineList[0]) continue;  //skip headers.
		depth.push_back(lineList[0].toFloat());
		if (lineList.size() > 1)
			tempMask.push_back(lineList[1].toInt());
		else
			tempMask.push_back(1); //Assume on.
	}

	file.close();

	//Fill the actual mask.
	mask = QBitArray (tempMask.size(), true);
	for (int i = 0; i < tempMask.size(); ++i)
	{
		if (0 == tempMask[i])
			mask.clearBit(i);
	}

	//Check to see if loaded data is consistent.
	null = !isConsistent();
}

Profile::Profile(float pix, QVector<float> zdata,
	QBitArray maskdata, QObject* parent):
	QObject(parent)
{
	//Data assignment
	pixelSize = pix;
	depth = zdata;
	mask = maskdata;

	//Member assignment
	null = !isConsistent();
}

Profile::Profile(const Profile& other, QObject* parent):
	QObject(parent)
{
	//Data assignment
	pixelSize = other.getPixelSize();
	depth = other.getDepth();
	mask = other.getMask();

	//Member assignment
	null = !isConsistent();
}

Profile::~Profile()
{

}

bool Profile::isConsistent()
{
	//Is the data valid?
	if (depth.size() != mask.size())
	{
		QString warning ("Data loaded is invalid.");
		warning.append("Depth size does not match mask size.");
		qDebug() << warning.toStdString().c_str();
		return false;
	}
	else return true;
}

const QVector<float>& Profile::getDepth() const
{
	return depth;
}

const QBitArray& Profile::getMask() const
{
	return mask;
}

bool Profile::save(const QString& fname) const
{
	//Report status.
	QString status ("Saving ");
	status.append(fname);
	qDebug() << status.toStdString().c_str();

	if (null)
	{
		qDebug() << "Cannot save, profile is null.";
		return false;
	}

	//Open the file.
	QFile file (fname);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QString warning ("Error opening file ");
		warning.append(fname);
		qDebug() << warning.toStdString().c_str();
		return false;
	}

	//Serializer object for QT classes.
	QTextStream fileWriter(&file);

	//File header.
	fileWriter << "Mantis Profile File" << endl; 
	fileWriter << "Version,1" << endl; 
	fileWriter << "Pixel size," << pixelSize << endl;
	fileWriter << "Depth,Mask" << endl;

	//Make copy of mask that we can easily write out.
	QVector<int> tempMask;
	for (int i = 0; i < mask.size(); ++i)
	{
		if (mask.testBit(i))
			tempMask.push_back(1);
		else
			tempMask.push_back(0);
	}

	//File body.
	for (int i = 0; i < depth.size(); ++i)
		fileWriter << depth[i] << "," << tempMask[i] << endl;

	file.close();

	return true;
}

//=======================================================================
//=======================================================================
Profile* Profile::flip()
{
	//If internally inconsistent, we can't flip.
	if (null) return NULL;
	
	//Do the flipping part.
	QVector<float> flipDepth;
	int size = depth.size();
	QBitArray flipMask (size, true);
	for (int i = 0; i < size; ++i)
	{
		flipDepth.push_back(depth[size - 1 - i]);	
		if (!mask.testBit(size - 1 - i))
			flipMask.clearBit(i);
	}

	//Make the new object.
	return new Profile(pixelSize, flipDepth, flipMask);
}
