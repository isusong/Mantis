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
//  Author: Brian Bailey(bbailey@code-hammer.com)
//

#version 120

uniform float minDepth;
uniform float maxDepth;
uniform bool useLighting = false;
varying float vertexDepth;

vec3 lighting(vec3, vec3, vec3);

void main()
{
    float normalizedDepth = (vertexDepth - minDepth)/(maxDepth - minDepth);
    
    if (useLighting)
    {
        vec3 c = vec3(normalizedDepth, normalizedDepth, normalizedDepth);
        gl_FragColor = vec4(lighting(c,c,c), 1.0f);
    }
    else
    {
        gl_FragColor = vec4(normalizedDepth, normalizedDepth, normalizedDepth, 1.0f);
    }
}
