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

#ifndef __CSVTABLE_H__
#define __CSVTABLE_H__
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * Class that presents a simple interface to a CSV table.
 * Note that placing "," or "\n" in a string will modify
 * the table when saved.
 *
 * @author Laura Ekstrand
 */

class CsvTable: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth)
	Q_PROPERTY(int height READ getHeight)
	Q_PROPERTY(QString note READ getNote WRITE setNote)

  protected:
	//Member variables.
	int width; ///< The width of the table. Immutable.
	int height; ///< The height of the table. Immutable.
	bool printFileHeader; ///< Whether or not you want "Mantis..." and note at top of file
	QString note; ///< A user-defined note for the top of the file.
	///The header cells that go above the table columns.
	/**
	 * The first of these is the top left corner. This is idx = 0.
	 * The last entry is idx = width.
	 * The length of this is immutable.
	 */
	QStringList topHeaders;
	///The header cells that go to the left of the table rows.
	/**
	 * These start with the second row because the top left corner 
	 * belongs to topHeaders. Therefore, the second row is idx = 0.
	 * The last entry is idx = height - 1.
	 * The length of this is immutable.
	 */
	QStringList sideHeaders;
	///The data cells, in row major order.
	/**
	 * This is zero-indexed. Therefore, the first data cell is (0, 0) and the 
	 * last one is (height - 1, width - 1).
	 * The size of this is immutable.
	 */
	QStringList data;

  public:
	///Creates a table with width w and height h.
	CsvTable(int w, int h, bool pHeader=true, QObject* parent = 0);
	virtual ~CsvTable();

  public slots:
	//These are here so that they can be scriptable.
	///Save contents to a .csv file.
	bool save(const QString& fname) const;
	///For your convenience: uses QFileInfo to strip the path off a filename.
	QString getFileName(const QString& fname) const;
	
	//Getters.
	///Get the width.
	inline int getWidth() {return width;}
	///Get the height.
	inline int getHeight() {return height;}
	///Get the note.
	inline QString getNote() {return note;}
	///Get the contents of a cell in the top header.
	QString getTopHeader(int idx);
	///Get the contents of a cell in the side header.
	QString getSideHeader(int idx);
	///Get the contents of a data cell.
	/**
	 * Here, i is the row index and j is the column index.
	 * This can be confusing if you are thinking in terms of x and y.
	 * i roughly corresponds to -y and a j to x.
	 */
	QString getValue(int i, int j);

	//Setters.
	///Set the note.
	inline void setNote(QString text) {note = text;}
	///Set the contents of a cell in the top header.
	void setTopHeader(int idx, QString text);
	///Set the contents of a cell in the side header.
	void setSideHeader(int idx, QString text);
	///Set the contents of a data cell.
	/**
	 * Here, i is the row index and j is the column index.
	 * This can be confusing if you are thinking in terms of x and y.
	 * i roughly corresponds to -y and a j to x.
	 */
	void setValue(int i, int j, QString text);
};

#endif //!defined  __CSVTABLE_H__
