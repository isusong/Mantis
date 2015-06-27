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
 * Author: Brian Bailey (bbailey@code-hammer.com)
 */

#ifndef STATSCALEDRAW_H
#define STATSCALEDRAW_H

#define QWT_DLL

#include <qwt_scale_draw.h>
class StatScaleDraw: public QwtScaleDraw
{
public:
    StatScaleDraw();


    virtual QwtText label(double value)	const;

};

#endif
