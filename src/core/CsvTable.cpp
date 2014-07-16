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

#include "CsvTable.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

CsvTable::CsvTable(int w, int h, bool pHeader, QObject* parent):
	QObject(parent)
{
	//Initialize internal data to match the size of the table.
	width = w;
	height = h;
	printFileHeader = pHeader;
	note = "";

	for (int i = 0; i <= width; ++i)
		topHeaders.push_back("");
	
	for (int i = 0; i < height; ++i)
		sideHeaders.push_back("");
	
	for (int i = 0; i < width*height; ++i)
		data.push_back("");
}

CsvTable::~CsvTable()
{

}

bool
CsvTable::save(const QString& fname) const
{
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
	if (printFileHeader)
	{
		fileWriter << "Mantis CSV Table File" << endl; 
		fileWriter << "Version,1" << endl; 
		fileWriter << note << endl;
		fileWriter << endl; //Empty line for separator.
	}

	//File body.
	//Check to see whether top headers or side headers are used.
	bool thused = false;
	for (int i = 0; i <= width; ++i)
	{
		if (topHeaders[i] != "") 
		{
			thused = true;
			break;
		}
	}
	bool shused = false;
	for (int i = 0; i < height; ++i)
	{
		if (sideHeaders[i] != "") 
		{
			shused = true;
			break;
		}
	}

	//Print the top headers.
	if (thused)
	{
		for (int i = 0; i <= width; ++i)
			fileWriter << topHeaders[i] << ",";
		fileWriter << endl;
	}

	//Print the rows.
	for (int i = 0; i < height; ++i)
	{
		if (shused)
			fileWriter << sideHeaders[i] << ",";

		for (int j = 0; j < width; ++j)
			fileWriter << data[width*i + j] << ",";

		fileWriter << endl;
	}

	file.close();

	return true;
}

QString
CsvTable::getFileName(const QString& fname) const
{
	QFileInfo fileInfo (fname);
	return fileInfo.fileName();
}

QString
CsvTable::getTopHeader(int idx)
{
	if ((0 > idx) || (width < idx))
	{
		qDebug() << "Top Header index " << idx << " out of range.";
		qDebug() << "Returning QString()";
		return QString();
	}

	return topHeaders[idx];
}

QString
CsvTable::getSideHeader(int idx)
{
	if ((0 > idx) || (height - 1 < idx))
	{
		qDebug() << "Side Header index " << idx << " out of range.";
		qDebug() << "Returning QString()";
		return QString();
	}

	return sideHeaders[idx];
}

QString
CsvTable::getValue(int i, int j)
{
	if ((0 > i) || (height - 1 < i))
	{
		qDebug() << "Data value index i=" << i << " out of range.";
		qDebug() << "Returning QString()";
		return QString();
	}

	if ((0 > j) || (width - 1 < j))
	{
		qDebug() << "Data value index j=" << j << " out of range.";
		qDebug() << "Returning QString()";
		return QString();
	}

	return data[i*width + j];
}

void
CsvTable::setTopHeader(int idx, QString text)
{
	if ((0 > idx) || (width < idx))
	{
		qDebug() << "Top Header index " << idx << " out of range.";
		qDebug() << "Nothing was set.";
		return;
	}

	topHeaders[idx] = text;
}

void
CsvTable::setSideHeader(int idx, QString text)
{
	if ((0 > idx) || (height - 1 < idx))
	{
		qDebug() << "Side Header index " << idx << " out of range.";
		qDebug() << "Nothing was set.";
		return;
	}

	sideHeaders[idx] = text;
}

void
CsvTable::setValue(int i, int j, QString text)
{
	if ((0 > i) || (height - 1 < i))
	{
		qDebug() << "Data value index i=" << i << " out of range.";
		qDebug() << "Nothing was set.";
		return;
	}

	if ((0 > j) || (width - 1 < j))
	{
		qDebug() << "Data value index j=" << j << " out of range.";
		qDebug() << "Nothing was set.";
		return;
	}

	data[i*width + j] = text;
}
