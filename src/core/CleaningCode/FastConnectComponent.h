/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 * Author: Song Zhang (song@iastate.edu)
 */

// FastConnectComponent.h: interface of the CFastConnectComponent class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
using namespace std;

class CFastConnectComponent 
{
public:
	CFastConnectComponent(int width, int height);
	virtual ~CFastConnectComponent();

public:
	void FastConnectFindStartPoint(int &startx, int &starty);
	int  FastConnectAlg( const unsigned char *input, unsigned char *output,
			const int &background, const int&valid);

	void FastConnectBottomRight(const int &xstart, const int &ystart);
	void FastConnectTopRight(const int &xstart, const int &ystart);
	void FastConnectBottomLeft(const int &xstart, const int &ystart);
	void FastConnectTopLeft(const int &xstart, const int &ystart);

	const unsigned char *m_ucmask;
	unsigned char *m_uclocalmask;

	const int  m_iwidth;
	const int  m_iheight;
	int  m_valid;
	int  m_background;
	vector<int> m_ipossible;
};
