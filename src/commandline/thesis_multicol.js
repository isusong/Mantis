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

//Re-perform the comparisons in my master's thesis
//Author: Laura Ekstrand (ldmil@iastate.edu)

//Paths
var path = "../../CleanIFMTip_data/thesis_data/";
var vmarkfolder = "vmarks/";
var platesfolder = "plates/quick-select-links/";
var tablesfolder = "tables/";

//Ranges
var tips = ["2A", "2B", "3A", "3B", "4A", "4B", "5A", "5B", "8A", "8B", "44A", "44B"];
var vangles = [];
for (var i = 6; i < 19; ++i)
{
	vangles.push(i*5);
}
var rangles = [45, 60, 85];

//Where to put results for R.
var nm45 = [];
var nm60 = [];
var nm85 = [];
var km45 = [];
var km60 = [];
var km85 = [];

//For detected matches.
var matches = [];
var matchthresh = 2.5;
function arrayMax(array)
{
	var max = -Number.MAX_VALUE;
	var idx = -1;
	for (var i = 0; i < array.length; ++i)
	{
		if (array[i] > max)
		{
			max = array[i];
			idx = i;
		}
	}
	return [max, idx];
}

//Make the stat object.
var stat = StatInterface();
stat.maxShiftPercentage = 0.5;
function compare(stat, profile1, profile2)
{
	try
	{
		//Perform the comparison.
		stat.compare(profile1, profile2);
		return stat.tValue;
	}
	catch (err)
	{
		return "####";
	}
}
//Number of comparisons to make each time
//multicompare runs.
var nmulti = 3; 
//Take the average of the top how many?
var averageOfTop = 2; 
function contains(array, string)
{
	for (var i = 0; i < array.length; ++i)
	{
		if (string === array[i])
			return true
	}
	return false;
}
function multicompare(stat, vprofile, r3D)
{
	//Perform the comparisons.
	var comps = []; //where to store T1s.
	for (var i = 0; i < nmulti; ++i)
	{
		var col = Math.round((i + 1)*(r3D.width/(nmulti+1)));
		print("col: " + col);
		comps.push(compare(stat, vprofile, r3D.getColumn(col)));
	}

	if (contains(comps, "####"))
		return "####";
	else
	{
		var sortedcomps = comps.sort();
		if (averageOfTop > nmulti)
			averageOfTop = nmulti;
		if (averageOfTop < 1)
			averageOfTop = 1;
		var sum = 0;
		for (var i = nmulti - averageOfTop; i < nmulti; ++i)
		{
			sum += sortedcomps[i];
		}
		return sum/averageOfTop;
	}
}

//Object constructors
function vmark(tip, angle)
{
	this.tip = tip;
	this.angle = angle;
	this.profile = Profile(path + vmarkfolder + "V" +
		this.tip + "-" + this.angle +
		".csv");
}
function rmark(tip, angle)
{
	this.tip = tip;
	this.angle = angle;
}

//Construct the vmarks and rmarks.
var vmarks = [];
var rmarks = [];
for (var i = 0; i < tips.length; ++i)
{
	//Construct vmarks.
	var subvmarks = []
	for (var j = 0; j < vangles.length; ++j)
	{
		subvmarks.push(new vmark(tips[i], vangles[j]));
	}
	vmarks.push(subvmarks);

	//Construct rmarks.
	var subrmarks = [];
	for (var j = 0; j < rangles.length; ++j)
	{
		if (("5B" == tips[i]) && (45 == rangles[j]))
			continue; //skip this one, it doesn't exist.

		if (("44A" == tips[i]) && (45 == rangles[j]))
			continue; //skip this one, it doesn't exist.

		subrmarks.push(new rmark(tips[i], rangles[j]));
	}
	rmarks.push(subrmarks);
}

//Perform the comparisons.
//For each tip.
for (var i = 0; i < tips.length; ++i)
{
	//For each plate.
	for (var j = 0; j < rmarks.length; ++j)
	{
		//For each possible plate angle.
		for (var k = 0; k < rmarks[j].length; ++k)
		{
			print("Comparing Tip " + tips[i] + " to P" + rmarks[j][k].tip +
				"-" + rmarks[j][k].angle);

			//Each plate.
			gc(); //collect the garbage
			//Open the plate file
			var r3D = RangeImage(path + platesfolder + "P" + rmarks[j][k].tip +
				"-" + rmarks[j][k].angle + ".mt");
			var set = [];
			var setsum = 0;
			var flippedset = [];
			var fsetsum = 0;
			for (var l = 0; l < vmarks[i].length; ++l)
			{
				//Perform forward comparison
				print("Forward " + vmarks[i][l].angle);
				set.push(multicompare(stat, vmarks[i][l].profile, r3D));
				//Perform flipped comparison
				print("Flipped " + vmarks[i][l].angle);
				flippedset.push(multicompare(stat, vmarks[i][l].profile.flip(), r3D));
				setsum += set[l];
				fsetsum += flippedset[l];
			}
			print("Done comparing");

			//Log matches.
			var max = arrayMax(set);
			if (max[0] > matchthresh)
			{
				matches.push([("V" + tips[i] + "-" + vangles[max[1]]), 
					("P" + rmarks[j][k].tip + "-" + rmarks[j][k].angle), max[0]]);
			}
			var flippedmax = arrayMax(flippedset);
			if (flippedmax[0] > matchthresh)
			{
				matches.push([("V" + tips[i] + "-" + vangles[flippedmax[1]] + "-f"), 
					("P" + rmarks[j][k].tip + "-" + rmarks[j][k].angle),
					flippedmax[0]]);
			}
			
			//Is the plate flipped?
			var flipped;
			if (setsum > fsetsum)
				flipped = false;
			else
				flipped = true;

			//Is this a known match comparison?
			var knownmatch;
			if (tips[i] == rmarks[j][k].tip)
				knownmatch = true;
			else
				knownmatch = false;

			//Store the results.
			for (var l = 0; l < vmarks[i].length; ++l)
			{
				if (45 == rmarks[j][k].angle)
				{
					if (knownmatch && (!flipped))
					{
						km45.push([("45-"+ vmarks[i][l].angle), set[l]]);
						nm45.push([("45-"+ vmarks[i][l].angle), flippedset[l]]);
					}
					else if (knownmatch && flipped)
					{
						km45.push([("45-"+ vmarks[i][l].angle), flippedset[l]]);
						nm45.push([("45-"+ vmarks[i][l].angle), set[l]]);
					}
					else
					{
						nm45.push([("45-"+ vmarks[i][l].angle), set[l]]);
						nm45.push([("45-"+ vmarks[i][l].angle), flippedset[l]]);
					}
				}

				if (60 == rmarks[j][k].angle)
				{
					if (knownmatch && (!flipped))
					{
						km60.push([("60-"+ vmarks[i][l].angle), set[l]]);
						nm60.push([("60-"+ vmarks[i][l].angle), flippedset[l]]);
					}
					else if (knownmatch && flipped)
					{
						km60.push([("60-"+ vmarks[i][l].angle), flippedset[l]]);
						nm60.push([("60-"+ vmarks[i][l].angle), set[l]]);
					}
					else
					{
						nm60.push([("60-"+ vmarks[i][l].angle), set[l]]);
						nm60.push([("60-"+ vmarks[i][l].angle), flippedset[l]]);
					}
				}

				if (85 == rmarks[j][k].angle)
				{
					if (knownmatch && (!flipped))
					{
						km85.push([("85-"+ vmarks[i][l].angle), set[l]]);
						nm85.push([("85-"+ vmarks[i][l].angle), flippedset[l]]);
					}
					else if (knownmatch && flipped)
					{
						km85.push([("85-"+ vmarks[i][l].angle), flippedset[l]]);
						nm85.push([("85-"+ vmarks[i][l].angle), set[l]]);
					}
					else
					{
						nm85.push([("85-"+ vmarks[i][l].angle), set[l]]);
						nm85.push([("85-"+ vmarks[i][l].angle), flippedset[l]]);
					}
				}
			}
		}
	}
}

//Copy the results into csv tables.
var table_matches = new CsvTable(3, matches.length, false);
table_matches.setTopHeader(0, "Tip");
table_matches.setTopHeader(1, "Plate");
table_matches.setTopHeader(2, "T1 Value");
for (var i = 0; i < matches.length; ++i)
{
	for (var j = 0; j < 3; ++j)
	{
		table_matches.setValue(i, j, matches[i][j]);
	}
}
table_matches.save(path + tablesfolder + "matches-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_nm45 = new CsvTable(2, nm45.length, false);
table_nm45.setTopHeader(0, "Plate Angle - VMark Angle");
table_nm45.setTopHeader(1, "T1 Value");
for (var i = 0; i < nm45.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_nm45.setValue(i, j, nm45[i][j]);
	}
}
table_nm45.save(path + tablesfolder + "45-non-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_nm60 = new CsvTable(2, nm60.length, false);
table_nm60.setTopHeader(0, "Plate Angle - VMark Angle");
table_nm60.setTopHeader(1, "T1 Value");
for (var i = 0; i < nm60.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_nm60.setValue(i, j, nm60[i][j]);
	}
}
table_nm60.save(path + tablesfolder + "60-non-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_nm85 = new CsvTable(2, nm85.length, false);
table_nm85.setTopHeader(0, "Plate Angle - VMark Angle");
table_nm85.setTopHeader(1, "T1 Value");
for (var i = 0; i < nm85.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_nm85.setValue(i, j, nm85[i][j]);
	}
}
table_nm85.save(path + tablesfolder + "85-non-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_km45 = new CsvTable(2, km45.length, false);
table_km45.setTopHeader(0, "Plate Angle - VMark Angle");
table_km45.setTopHeader(1, "T1 Value");
for (var i = 0; i < km45.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_km45.setValue(i, j, km45[i][j]);
	}
}
table_km45.save(path + tablesfolder + "45-known-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_km60 = new CsvTable(2, km60.length, false);
table_km60.setTopHeader(0, "Plate Angle - VMark Angle");
table_km60.setTopHeader(1, "T1 Value");
for (var i = 0; i < km60.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_km60.setValue(i, j, km60[i][j]);
	}
}
table_km60.save(path + tablesfolder + "60-known-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");

var table_km85 = new CsvTable(2, km85.length, false);
table_km85.setTopHeader(0, "Plate Angle - VMark Angle");
table_km85.setTopHeader(1, "T1 Value");
for (var i = 0; i < km85.length; ++i)
{
	for (var j = 0; j < 2; ++j)
	{
		table_km85.setValue(i, j, km85[i][j]);
	}
}
table_km85.save(path + tablesfolder + "85-known-" +
	Math.round(stat.maxShiftPercentage*100) + ".csv");
