//
//  Copyright 2008-2014 Iowa State University
// 
//  This file is part of Mantis.
//  
//  Mantis is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  Mantis is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with Mantis.  If not, see <http://www.gnu.org/licenses/>.
// 
//  Author: Laura Ekstrand (ldmil@iastate.edu)
//

//Compare plier marks.
//Author: Laura Ekstrand (ldmil@iastate.edu)

//Some initialization.
//The path from here to the folder with the data in it.
var path = "../../../../courses/ME556X/cu_trimmed_pliers/"; 
var width = 3; //# of columns in test (i.e. S-V pairs)
var height = 2; //# of rows in test. (i.e. pairs of plier marks)
var column = 700; //The column being compared.
//The object that makes comparisons for us. Front-end for stat package.
var stat = StatInterface();
//The table that we will stash the results in.
var table = CsvTable(width, height);
//The S-V pairs.
var svpairs = [
	[500, 250],
	[1000, 500],
	[2000, 1000]
];
//The top headers.
for (var i = 1; i <= width; ++i)
{
	table.setTopHeader(i, new String(svpairs[i-1][0]) 
		+ "-" + new String(svpairs[i-1][1]));
}

//Perform the comparisons and stash the results.
for (var i = 0; i < height; ++i)
{
	var name1 = "P" + (i + 1) + "A18 trimmed.mt";
	var name2 = "P" + (i + 1) + "A20 trimmed.mt";
	table.setSideHeader(i, name1 + " - " + name2 + ": Row " + column);
	var file1 = RangeImage(path + name1);
	var file2 = RangeImage(path + name2);
	var profile1 = file1.getColumn(column);
	var profile2 = file2.getColumn(column);

	for (var j = 0; j < width; ++j)
	{
		stat.searchWindow = svpairs[j][0];
		stat.validWindow = svpairs[j][1];
		print("search window = " + stat.searchWindow);
		print("validation window = " + stat.validWindow);
		try
		{
			//Perform the comparison.
			stat.compare(profile1, profile2);
			print("tValue = " + stat.tValue);
			//Stash the T value.
			table.setValue(i, j, stat.tValue);
		}
		catch (err)
		{
			print("tValue = ####");
			table.setValue(i, j, "####");
		}
	}
}

table.note = "Top headers are in this format: (length of search window) - (length of validation window)";
table.save("thistest.csv");
