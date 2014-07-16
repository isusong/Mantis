//
// Copyright 2008-2014 Iowa State University
//
// This file is part of Mantis.
// 
// This computer software was prepared by The Ames 
// Laboratory, hereinafter the Contractor, under 
// Interagency Agreement number 2009-DN-R-119 between 
// the National Institute of Justice (NIJ) and the 
// Department of Energy (DOE). All rights in the computer 
// software are reserved by NIJ/DOE on behalf of the 
// United States Government and the Contractor as provided 
// in its Contract, DE-AC02-07CH11358.  You are authorized 
// to use this computer software for Governmental purposes
// but it is not to be released or distributed to the public.  
// NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
// OF THIS SOFTWARE.  
//
// This notice including this sentence 
// must appear on any copies of this computer software.
// 
//  Author: Laura Ekstrand (ldmil@iastate.edu)
//

#version 120
//Model where each vertex has a unique 
//color up to 16777216 vertices

uniform int width; 
uniform int height; 
uniform int skip;

void
main()
{
	//Retrieve i and j.
	int iValue = int((height * gl_TexCoord[0].t) - (float(skip)/2));
	int jValue = int((width * gl_TexCoord[0].s) - (float(skip)/2));

	//Retrieve "vertex id"
	int id = (width*iValue) + jValue;
	
	//Map vertex id to r,g,b color
	float red = float(id/65536);
	float blue = float(mod(id, 256));
	float green = float((id - (65536*red) - blue)/256);

	red /= 255;
	blue /= 255;
	green /= 255;

	gl_FragColor = vec4(red, green, blue, 1.0f);
}
