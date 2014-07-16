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

#include "StatInterface.h"
#include "../StatisticsLibrary/io/converttracetoint.h"
#include <memory>
#include <vector>
#include <stdexcept>
#include <QDebug>
#include "../StatisticsLibrary/base/flipcorrelation.h"
#include "../StatisticsLibrary/base/FlippableCorLoc.h"
#include "../StatisticsLibrary/base/intnolev_functors.h"
#include "../StatisticsLibrary/base/stats.h"
#include <QScriptContext>
#include <QScriptEngine>

using std::vector;
using std::auto_ptr;
using std::runtime_error;

StatInterface::StatInterface(QObject *parent):
	QObject(parent)
{
	//Default values (from Amy's mytest.param).
	T_sample_size = 200; 
	searchWindow = 300;
	validWindow = 50;
	numRigidPairs = 50;
	numRandomPairs = 50;
	maxShiftPercentage = 1.0f;
}

StatInterface::~StatInterface()
{

}

//This works without deep copies because 
//QVector is implicitly shared.
QVector<double>
StatInterface::toDouble(const QVector<float> data)
{
	QVector<double> ret;
	for (int i = 0; i < data.size(); ++i)
	{
		ret.push_back((double) data[i]);
	}
	return ret;
}

QVector<float>
StatInterface::trimProfileEnds(Profile* data)
{
	//Cache some things.
	QVector<float> depth = data->getDepth();
	QBitArray mask = data->getMask();

	//Find the ends to trim off.
	int dataLength = depth.size();
	int startIdx, endIdx;
	for (int i = 0; i < dataLength; ++i)
	{
		if (mask.testBit(i))
		{
			startIdx = i;
			break;
		}
	}

	for (int i = 0; i < dataLength; ++i)
	{
		if (mask.testBit(dataLength - 1 - i))
		{
			endIdx = dataLength - 1 - i;
			break;
		}
	}

	//Trim it.
	QVector<float> ret;
	for (int i = startIdx; i < endIdx + 1; ++i)
		ret.push_back(depth[i]);

	return ret;
}

void
StatInterface::compare(QVector<float> data1, QVector<float> data2)
{
	//Vector prep.
	ConvertTraceToInt intConverter;
	auto_ptr<vector<int> > trace1 = 
		intConverter(toDouble(data1).toStdVector());
	auto_ptr<vector<int> > trace2 = 
		intConverter(toDouble(data2).toStdVector());
	
	//Run without checking for flips.
	//maxCorWithFlips is in flipcorrelation.h.
	//Ru He says, "Once searchWindow is fixed, the 
	//returned c.loc1(), c.loc2() in c object should be unique."
	const int length1 = trace1->size();
	const int length2 = trace2->size();
	FlippableCorLoc c (0, 0, 0, true); //"default" FCL object.
	try
	{
		c = maxCorWithFlips(trace1->begin(),
							trace2->begin(),
							length1,
							length2,
							searchWindow,
							maxShiftPercentage,
							false);
	} catch (runtime_error err) {
		qDebug() << "There was a runtime error in the" <<
			"stat package:";
		qDebug() << err.what();
		throw err; //Throw it up to the next level.
		return; //exit from this function. r and t and loc1 and loc2 will stale.
	}

	//Calculate T value (and average if T_sample_size is > 1).
	QVector<double> T_vector;
	for (int i = 0; i < T_sample_size; ++i)
	{
		//Maverick says, "[R]igid pairs correlation.
		//In other words, the correlation for two windows
		//that has [sic] the same shifts with respect to the
		//position of maximum correlation."
		//IntRigidCorSampExcludeSearch is in intnolev_functors.h
		IntRigidCorSampExcludeSearch compRigidCor;
		auto_ptr<vector<double> > rigidCor = 
			compRigidCor(*trace1, *trace2, c.loc1(), c.loc2(),
					  searchWindow, numRigidPairs, validWindow);
		if (0 == rigidCor->size())
		{
			//Ran out of space for the validation window.
			QString what (tr("The rigid-shift "
				"validation window "
				"did not fit inside one of the trace data sets. "
				"Bear in mind that the validation window "
				"cannot lie inside the search window.\n\n"
				"Try a smaller validation and/or search window."));
			qDebug() << what;
			throw std::range_error(what.toStdString());
			return; //exit from this function.
		}

		//Maverick says, "For the random pairs correlation,
		//the shifts of two pairs are different w.r.t. to [sic]
		//the position of maximum correlation.
		//IntRandomCorSampExcludeSearch is in intnolev_functors.h
		IntRandomCorSampExcludeSearch compRandomCor;
		auto_ptr<vector<double> > randomCor =
			compRandomCor(*trace1, *trace2, c.loc1(), c.loc2(),
				searchWindow, numRandomPairs, validWindow);
		if (0 == randomCor->size())
		{
			//Ran out of space for the validation window.
			QString whatnow (tr("The random-shift "
				"validation window "
				"did not fit inside one of the trace data sets. "
				"Bear in mind that the validation window "
				"cannot lie inside the search window.\n\n"
				"Try a smaller validation and/or search window."));
			qDebug() << whatnow;
			throw std::range_error(whatnow.toStdString());
			return; //exit from this function.
		}

		//Compute T value
		T_vector.push_back(t1Statistic(*rigidCor, *randomCor));
	}
	//Average T values.
	//If we made it this far, there were no errors.
	double T_mean, T_var;
	meanAndVar(T_vector.toStdVector(), T_mean, T_var);

	//Store the results.
	tValue = T_mean;
	rValue = c.cor();
	loc1 = c.loc1();
	loc2 = c.loc2();
}

void
StatInterface::compare(Profile* data1, Profile* data2)
{
	if (data1->getPixelSize() != data2->getPixelSize())
	{
		QString what (tr("The Profiles do not have matching pixel sizes."
			"It does not make sense to compare them."
			"TValue and other outputs not updated."));
		qDebug() << what.toStdString().c_str();
		throw std::range_error(what.toStdString());
		return;
	}

	compare(trimProfileEnds(data1), trimProfileEnds(data2));
}

QScriptValue
StatInterface::compare()
{
	int argc = argumentCount();
	if (argc < 2)
	{
		qDebug() << "Incorrect number of arguments to compare.";
		qDebug() << "Correct number of arguments is 2.";
		qDebug() << "Returning a null object.";
		qDebug() << "Outputs will stale. Do not trust the tValue, etc.";
		return QScriptValue();
	}

	Profile* argument0 = qscriptvalue_cast<Profile*>( argument(0) );
	Profile* argument1 = qscriptvalue_cast<Profile*>( argument(1) );

	try
	{
		compare(argument0, argument1);
	}
	catch (std::exception err)
	{
		return context()->throwError(QScriptContext::RangeError, 
			QString(err.what()));
	}

	return QScriptValue(); //~ void
}

void
StatInterface::setSearchWindow(int width)
{
	searchWindow = width;
}

void
StatInterface::setValidWindow(int width)
{
	validWindow = width;
}

void
StatInterface::setNumRigidPairs(int num)
{
	numRigidPairs = num;
}

void
StatInterface::setNumRandomPairs(int num)
{
	numRandomPairs = num;
}

void
StatInterface::setTSampleSize(int num)
{
	T_sample_size = num;
}

void
StatInterface::setMaxShiftPercentage(double num)
{
	maxShiftPercentage = (float) num;
}

