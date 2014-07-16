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
//Lighting from infinitely far away.

varying vec3 surfaceNormal;
varying vec4 eyeDistance;

void
main()
{
	//Surface properties
	vec3 normal = normalize(surfaceNormal);
	vec3 ambient = vec3(0.11f, 0.18f, 0.20f);
	vec3 diffuse = vec3(0.58f, 0.82f, 1.00f);
	vec3 specular = vec3(1.00f, 1.00f, 1.00f);
	float shine = 13.0f;

	//Light source properties
	vec3 source = vec3(1.0f); //white light source.
	vec3 ambSource = vec3(1.0f);
	vec3 lightOrigin = vec3(-1000.0f, 0, 1000.0f);

	//Diffuse reflection.
	float lambert = max(dot(normalize(lightOrigin), normal), 0);

	//Specular reflection.
	vec3 r = normalize(reflect(lightOrigin, normal));
	vec3 v = normalize(vec3(eyeDistance));
	float phong = pow(max(dot(r, v), 0), shine);

	//Combine results.
	vec3 colors = (ambSource * ambient) +
		(lambert * source * diffuse) +
		(phong * source * specular);

	//Output color
	gl_FragColor = vec4(colors, 1.0f);
	//gl_FragColor = vec4(normal, 1.0f); //normal map
}
