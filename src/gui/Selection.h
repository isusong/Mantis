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

#ifndef __SELECTION_H__
#define __SELECTION_H__
#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPointF>

/**
 * A class for drawing selections (e.g. planes)
 * on objects with a bounding box.
 *
 * @author Laura Ekstrand
 */

class Selection: public QObject
{
	Q_OBJECT 

  public:
	///Draw type enum
	enum drawModes {colPlane, rowPlane};

  public:
	///Create a selection object.
	/**
	 * Object has bounding box with bb0 as the minimum corner
	 * and bb1 as the maximum corner.
	 */
	Selection(const QVector3D& bb0, const QVector3D& bb1,
		QObject* parent = 0);
	virtual ~Selection();

	///Draw the selection.
	virtual void draw();

    void setBbMin(const QVector3D &v) { bbMin = v; }
    void setBbMax(const QVector3D &v) { bbMax = v; }

    static void drawSearchBox(float l, float r, float t, float b, float z, float color[4], float linewidth=1);

    static void drawColPlane(float x, float ymin, float ymax, float zmin, float zmax, float color[4], int zmul=1);
    static void drawRowPlane(float xmin, float xmax, float y, float zmin, float zmax, float color[4], int zmul=1);

  public slots:
	///Update the basis point.
	void updateSelectionBasis(float x, float y);
	///Ask for a redraw of the selection
	/**
	 * To avoid signal loops, this ensures that the 
	 * basis point has truly been updated.
	 */
	void redrawSelection(float x, float y);

	//Getters and setters.
    inline QPointF getBasis() {return basis;}
	inline bool isEnabled() {return enabled;}
	inline void setEnabled(bool status) {enabled = status;}
	inline void setDrawMode(drawModes newMode)
		{mode = newMode;}
	inline drawModes getDrawMode() {return mode;}
	inline void setMultiplier(int mult) {multiplier = mult;}
	inline int getMultiplier() {return multiplier;}

  signals:
	void updateGL();  ///< Ask for an OpenGL update.

protected:
    void drawColPlane();
    void drawRowPlane();

protected:
  //Member variables.
  QPointF basis; ///< The selected point.
  QVector3D bbMin; ///< Minimum bounding box corner for object.
  QVector3D bbMax; ///< Maximum bounding box corner for object.
  bool enabled; ///< Is selection currently enabled?
  drawModes mode; ///< Which type of selection to draw.
  ///Selection strength (i.e. height of plane, diameter of circle, etc.)
  int multiplier;

};

#endif //!defined __SELECTION_H__
