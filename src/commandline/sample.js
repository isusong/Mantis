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

//Find out some simple things about a RangeImage file with javascripting.
//Author: Laura Ekstrand (ldmil@iastate.edu)

//Put your own hard-coded file path here:
var path = "../../CleanIFMTip_data/thesis_data/tips/Tip 4B45 10x low res 2$3D/";

////Open a file
//var file = importRangeImage(path + "dem.al3d", path + "texture.png"); 
//
////Print out some information.
//wait();
//print("Is file null? " + file.isNull())
//print("File width = " + file.width + " pixels")
//print("File height = " + file.height + " pixels")
//print("File x resolution = " + file.pixelSizeX + " um")
//print("File y resolution = " + file.pixelSizeY + " um")
//
//wait();
var plate = RangeImage(path + "../../plates/P4B-85-4$3D/P4B-85-4$3D.mt");
var dplate = plate.downsample(6);
dplate.exportToOBJ("test.obj");
view.getRangeImageWindow("P4B-85", plate, "plate");
//var colNo = Math.floor(plate.width/2);
//print("Plate 4B-85 middle col: " + colNo);
//var profile = plate.getColumn(colNo);
//var winname = view.getProfilePlot("Plate 4B-85 col " + colNo, profile, "lightseagreen");
//view.addSearchWindow(winname, 100, 300);
////var winname2 = view.getProfilePlot("Plate 4B-85 col " + colNo + "_2", profile, "mediumslateblue");
//wait();
//view.deleteWindow(winname);
wait();

var file = RangeImage(path + "Tip 4B45 10x low res 2.mt");
view.getRangeImageWindow("Tip 4B", file);
wait();
