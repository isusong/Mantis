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

#include "GenericModel.h"
#include "../core/logger.h"

//=======================================================================
//=======================================================================
GenericModel::GenericModel(
	QWidget *parent):
	QObject(parent)
{

}

//=======================================================================
//=======================================================================
GenericModel::~GenericModel()
{

}

//=======================================================================
//=======================================================================
void GenericModel::logInfo()
{
    LogTrace("Model: box min: (%.2f, %.2f, %.2f), max: (%.2f, %.2f, %.2f)", _bbMin.x(), _bbMin.y(), _bbMin.z(), _bbMax.x(), _bbMax.y(), _bbMax.z());
}
