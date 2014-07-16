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

//Import from al3d, clean, and save a screwdriver plate.
//Author: Laura Ekstrand (ldmil@iastate.edu)

//Put your own hard-coded file path here:
var path = "../../new_screwdrivers/";
var filenames = [
	["2A3", "35"],
	["2A6", "30"],
	["2B6", "35"],
	["2B8", "30"],
	["3A7", "30"],
	["3B4", "30"],
	["4A1", "30"],
	["4A10", "30"],
	["5A6", "35"],
	["5B2", "35"]
	];

for (var i = 0; i < filenames.length; ++i)
{
	//Import the al3d file.
	gc();
	var filepath = path + "M" + filenames[i][0] + "at" + filenames[i][1]
		+ "/";
	var file = importRangeImage(filepath + "dem.al3d", filepath + "texture.bmp"); 

	//Display it.
	//view.getRangeImageWindow(filepath, file, "plate");
	//wait();
	//view.deleteWindow(filepath);

	//Save as mt file.
	file.save(path + "M" + filenames[i][0] + "at" + filenames[i][1] + ".mt");
}
