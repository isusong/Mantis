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

//Load the plate files from my thesis and save the central columns.
//Author: Laura Ekstrand (ldmil@iastate.edu)

var path = "../../CleanIFMTip_data/thesis_data/";
var plateinfo = [ //Last col indicates 1 for second attempt
	[2, "A", 45, 7, 0],
	[2, "A", 60, 2, 0],
	[2, "A", 85, 2, 0],
	[2, "B", 45, 1, 0],
	[2, "B", 60, 2, 0],
	[2, "B", 85, 4, 1],
	[3, "A", 45, 3, 1],
	[3, "A", 60, 4, 1],
	[3, "A", 85, 4, 1],
	[3, "B", 45, 3, 0],
	[3, "B", 60, 4, 0],
	[3, "B", 85, 4, 0],
	[4, "A", 45, 2, 0],
	[4, "A", 60, 4, 0],
	[4, "A", 85, 4, 0],
	[4, "B", 45, 2, 0],
	[4, "B", 60, 4, 0],
	[4, "B", 85, 4, 0],
	[5, "A", 45, 1, 0],
	[5, "A", 60, 4, 0],
	[5, "A", 85, 4, 0],
	[5, "B", 60, 4, 0],
	[5, "B", 85, 4, 0],
	[8, "A", 45, 2, 0],
	[8, "A", 60, 4, 0],
	[8, "A", 85, 4, 0],
	[8, "B", 45, 2, 0],
	[8, "B", 60, 4, 0],
	[8, "B", 85, 4, 0],
	[44, "A", 60, 4, 0],
	[44, "A", 85, 4, 0],
	[44, "B", 45, 1, 0],
	[44, "B", 60, 3, 0],
	[44, "B", 85, 3, 0]
];

for (var i = 0; i < plateinfo.length; ++i)
{
	gc();
	
	//Load a file.
	var basename = "P" + plateinfo[i][0] + plateinfo[i][1] +
		"-" + plateinfo[i][2] + "-" + plateinfo[i][3];
	var foldername = basename;
	if (plateinfo[i][4])
		foldername += " second attempt";
	foldername += "$3D";
	var filename = basename;
	if (plateinfo[i][4])
		filename += "-second-attempt.mt";
	else
		filename += "$3D.mt";
	var file = RangeImage(path + "plates/" + foldername + "/" + filename);

	//Ask for the center column and save it.
	var col = Math.floor(file.width/2);
	var profile = file.getColumn(col);
	profile.save(path + "centralplatecols/P" + plateinfo[i][0] +
		plateinfo[i][1] + "-" + plateinfo[i][2] + "-Col" + col + ".csv");
}
