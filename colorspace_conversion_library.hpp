/*
Copyright (c) 2010, Alex Kuhl, http://www.alexkuhl.org
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of the author nor the names of other
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* A small, easy-to-understand C++ library written to convert colorspaces
 * Will convert RGB to HSV or HSL and back to RGB
 * Note on the colors:
 * R,G,B are ints [0,255]
 * S,V,L are floats [0,1]
 * H is a float [0,360)
 *
 * Requires the CImg library header file (testing code used version 1.3.1)
 * http://cimg.sourceforge.net/
 */

#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>
#include "CImg.h"

using namespace std ;
using namespace cimg_library ;

void rgb_to_hsl( int r, int g, int b, float& h, float& s, float& l )
{
	float fr = r/255.0, fg = g/255.0, fb = b/255.0 ;
	int imax = max( max( r, g ), b ) ;
	int imin = min( min( r, g ), b ) ;
	float fmax = imax/255.0 ;
	float fmin = imin/255.0 ;
	float multiplier = ( imin == imax ) ? 0.0 : 60/( fmax - fmin ) ;
		
	if( r == imax ) 	// red is dominant
	{
		h = ( multiplier*( fg - fb ) + 360 ) ;
		if( h >= 360 ) h -= 360 ;	// take quick modulus, % doesn't work with floats
	}
	else if( g == imax )// green is dominant
		h = multiplier*( fb - fr ) + 120 ;
	else				// blue is dominant
		h = multiplier*( fr - fg ) + 240 ;	
	l = .5*( fmax + fmin ) ;
	if( imax == imin )
		s = 0.0 ;
	else if( l <= .5 )
		s = ( fmax - fmin )/( 2*l ) ;
	else
		s = ( fmax - fmin )/( 2 - 2*l ) ;
}

void hsl_to_rgb( float h, float s, float l, int& r, int& g, int& b )
{
	float q ;
	if( l < .5 )
		q = l*( 1 + s ) ;
	else
		q = l + s - ( l*s ) ;
	float p = 2*l - q ;
	h /= 360 ;
	// t holds r, g, and b values
	float t[ 3 ] = { h + 1/3.0, h, h - 1/3.0 } ;
	for( int i = 0 ; i < 3 ; ++i )
	{
		if( t[ i ] < 0 )
			t[ i ] += 1 ;
		else if( t[ i ] > 1 )
			t[ i ] -= 1 ;
		// calculate the color
		if( t[ i ] < 1.0/6.0 )
			t[ i ] = p + ( ( q - p )*6*t[ i ] ) ;
		else if( 1.0/6.0 <= t[ i ] && t[ i ] < .5 )
			t[ i ] = q ;
		else if( .5 <= t[ i ] && t[ i ] < 2.0/3.0 )
			t[ i ] = p + ( ( q - p )*6*( 2.0/3.0 - t[ i ] ) ) ;
		else
			t[ i ] = p ;
	}	
	r = round( t[ 0 ]*255 ) ;
	g = round( t[ 1 ]*255 ) ;
	b = round( t[ 2 ]*255 ) ;
}

void rgb_to_hsv( int r, int g, int b, float& h, float& s, float& v )
{
	float fr = r/255.0, fg = g/255.0, fb = b/255.0 ;
	int imax = max( max( r, g ), b ) ;
	int imin = min( min( r, g ), b ) ;
	float fmax = imax/255.0 ;
	float fmin = imin/255.0 ;
	float multiplier = ( imin == imax ) ? 0.0 : 60/( fmax - fmin ) ;
		
	if( r == imax ) 	// red is dominant
	{
		h = ( multiplier*( fg - fb ) + 360 ) ;
		if( h >= 360 ) h -= 360 ;	// take quick modulus, % doesn't work with floats
	}
	else if( g == imax )// green is dominant
		h = multiplier*( fb - fr ) + 120 ;
	else				// blue is dominant
		h = multiplier*( fr - fg ) + 240 ;	
	if( imax == 0 )
		s = 0 ;
	else
		s = 1 - ( fmin/fmax ) ;
	v = fmax ;
}
 
void hsv_to_rgb( float h, float s, float v, int& r, int& g, int& b )
{
	h /= 60 ;
	int hi = (int)h ;
	float f = h - hi ;
	// all the following *255 are to move from [0,1] to [0,255] domains
	// because rgb are assumed [0,255] integers
	int p = round( v*( 1 - s )*255 ) ;
	int q = round( v*( 1 - f*s )*255 ) ;
	int t = round( v*( 1 - ( 1 - f )*s )*255 ) ;
	int iv = round( v*255 ) ;
	switch( hi )
	{
		case 0:
			r = iv ; g = t ; b = p ;
			break ;
		case 1:
			r = q ; g = iv ; b = p ;
			break ;
		case 2:
			r = p ; g = iv ; b = t ;
			break ;
		case 3:
			r = p ; g = q ; b = iv ;
			break ;
		case 4:
			r = t ; g = p ; b = iv ;
			break ;
		case 5:
			r = iv ; g = p ; b = q ;
			break ;
	}
}

