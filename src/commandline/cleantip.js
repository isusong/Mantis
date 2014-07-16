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

//Import from al3d, clean, and save a screwdriver tip.
//Author: Laura Ekstrand (ldmil@iastate.edu)

//Put your own hard-coded file path here:
var path = "../../new_screwdrivers/T8A/";

//Import the al3d file.
var file = importRangeImage(path + "dem.al3d", path + "texture.bmp"); 

////Clean the tip.
//var cleaner = Clean(); //Make the cleaner object.
//print("Cleaning....");
//var cleanfile = cleaner.cleanFlatScrewdriverTip(file, path + "qualitymap.png");

//Save the file
file.save(path + "T8A.mt")
