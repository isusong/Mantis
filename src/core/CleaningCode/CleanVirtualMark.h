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

#ifndef __CLEANVIRTUALMARK_H__
#define __CLEANVIRTUALMARK_H__
#include <QVector>
#include <QPoint>
#include <QGLContext> //for GLfloat

/**
 * Consolidates static functions for
 * cleaning a newly generated virtual mark
 * made by a VirtualMark object.
 *
 * @author Laura Ekstrand
 */

class CleanVirtualMark
{
  public:
	///Trim the masked data off the ends, leaving only the middle.
	/**
	 * I can pass QVector<float> around like this because it is
	 * implicitly shared.
	 *
	 * @return The processed data.
	 */
	static QVector<float> unmask(const QVector<float> data, 
		float maskValue);
	///Convert depthbuffer to um values. WARNING: Modifies data.
	/**
	 * This assumes the mark was made with an orthogonal
	 * projection (as in VirtualMark.cpp) and uses the 
	 * location of the near and far clip planes (nearClip and
	 * farClip, respectively) to unproject the data.
	 * Then, assuming a camera located at (0, 0, cameraZ) and
	 * looking down the negative z-axis (as in VirtualMark.cpp),
	 * it subtracts the camera distance, cameraZ.
	 */
	static void depthToUm(QVector<float>& data, float nearClip,
		float farClip, float cameraZ);
	///Find where to trim the tip edges off the mark.
	/**
	 * These edges are sharp drop-offs at either end of the data.
	 * @return The indices of the mark
	 * edges (sides of the screwdriver) as x and y in a QPoint.
	 * x = startIndex, y = endIndex.
	 */
	static QPoint findMarkEdges(const QVector<float> data);
};

#endif //! defined __CLEANVIRTUALMARK_H__
