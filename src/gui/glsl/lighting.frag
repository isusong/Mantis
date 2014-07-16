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

//White illumination from afar.
varying vec3 surfaceNormal;
varying vec4 eyeDistance;
uniform vec3 lightOrigin;

vec3
lighting(vec3 ambient, vec3 diffuse, vec3 specular, float shine)
{
	vec3 normal = normalize(surfaceNormal);

	//Light source properties
	vec3 source = vec3(1.0f); //white light source.
	vec3 ambSource = vec3(1.0f);

	//Diffuse reflection.
	float lambert = max(dot(normalize(lightOrigin), normal), 0);

	//Specular reflection.
	vec3 r = normalize(reflect(lightOrigin, normal));
	vec3 v = normalize(vec3(eyeDistance));
	float phong = pow(max(dot(r, v), 0), shine);

	////2nd light.
	//lightOrigin = vec3(1000.0f, 500.0f, 1000.0f);

	////Diffuse reflection.
	//lambert += max(dot(normalize(lightOrigin), normal), 0);

	////Specular reflection.
	//r = normalize(reflect(lightOrigin, normal));
	//phong += pow(max(dot(r, v), 0), shine);

	//Combine results.
	return (ambSource * ambient) +
		(lambert * source * diffuse) +
		(phong * source * specular);
}
