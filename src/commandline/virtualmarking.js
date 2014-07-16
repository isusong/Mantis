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

//Make virtual marks for the thesis dataset and save them.
//Author: Laura Ekstrand (ldmil@iastate.edu)

var path = "../../CleanIFMTip_data/thesis_data/";
var tipinfo = [
	[2,"A",""],
	[2,"B",""],
	[3,"A",""],
	[3,"B",""],
	[4,"A",""],
	[4,"B"," 2"],
	[5,"A",""],
	[5,"B",""],
	[8,"A",""],
	[8,"B",""],
	[44,"A",""],
	[44,"B",""]
];
var angles = [];
for (var i = 6; i < 19; ++i)
{
	angles.push(i*5);
}

for (var i = 0; i < tipinfo.length; ++i)
{
	gc();
	
	//Load a file and make it into a tip.
	var name = "Tip " + tipinfo[i][0]
		+ tipinfo[i][1] + "45 10x low res" + tipinfo[i][2];
	var file = RangeImage(path + "tips/" + name + "$3D/"
		+ name + ".mt");
	var tip = createVirtualTip(file);

	//Make virtual marks and save them.
	for (var j = 0; j < angles.length; ++j)
	{
		var profile = tip.mark(0, angles[j], 0);
		profile.save(path + "vmarks/V" + tipinfo[i][0] +
			tipinfo[i][1] + "-" + angles[j] + ".csv");
	}
}
