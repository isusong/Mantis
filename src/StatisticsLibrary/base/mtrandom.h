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
 * Author: Max Morris (mmorris@iastate.edu)
 */

#ifndef __MTRANDOM_H__
#define __MTRANDOM_H__

//Header for some useful functions from the
//standard MT implementation.

/**
 * A state of the Mersenne Twister Random Number Generator.
 */
class MTState
{
 public:
  /**
   * Capture the current state of the RNG
   * and return a copy. The caller is responsible
   * for deleting the returned object.
   */
  static MTState* captureState();

  ~MTState();
  
  /**
   * Set the current state of the RNG to that
   * captured in this object.
   */
  void setState() const;

 private:
  MTState();

  unsigned long *_mt;
  unsigned long _mag01[2];
  int _mti;
};

/**
 * Initialize the Mersenne Twister RNG with
 * a seed that's a function of s.
 */
void init_genrand(unsigned long s);

/**
 *  Generates a random number on [0,0xffffffff]-interval
 */
unsigned long genrand_int32(void);

/**
 * Generates a random number on [0, 1)-real-interval.
 */
double genrand_real2(void);

/**
 * Generates a random number on (0,1)-real-interval
 */
double genrand_real3(void);

#endif
