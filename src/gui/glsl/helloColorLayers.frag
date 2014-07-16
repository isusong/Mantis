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

//Hello world fragment shader
void
main()
{
	if (gl_FragCoord.z > 0.994f)
	{
		gl_FragColor = vec4(1.0f, 0, 1.0f, 1.0f); //Magenta
	}
	else if (gl_FragCoord.z > 0.99f)
	{
		gl_FragColor = vec4(0, 0, 1.0f, 1.0f); //Blue
	}
	else if (gl_FragCoord.z > 0.988f)
	{
		gl_FragColor = vec4(0, 1.0f, 0, 1.0f); //Green
	}
	else
	{
		gl_FragColor = vec4(1.0f, 0, 0, 1.0f); //Red
	}
}
