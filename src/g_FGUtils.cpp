/*------------------------------------------------------------------------------
Copyright (C) 2023 - present Jens Joschinski

This file is part of the ECOLOPES PLANT MODEL.

ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // RFate team
 // JJ: removed GDAL and boost related things. 
 // --------------------------------------------------------------------------

#include "g_FGUtils.h"


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Categorical to scalar and scalar to categorical conversion functions                            */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

Fract getLeavingFract(Fract f1, Fract f2){
	return DoubleToFract( 1.0 - ( FractToDouble(f1) + FractToDouble(f2) ) );
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

double FractToDouble(Fract fract){
	const double Fract_Real [ Fcount ] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
	return Fract_Real[static_cast<int>(fract)];
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

double FractToDouble(Fract2 fract){
	const double Fract_Real [ F2count ] = {0.0,0.1,0.5,0.9,1.0};
	return Fract_Real[static_cast<int>(fract)];
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

Fract DoubleToFract(double d){
	if (d <= 0.0) return Fract(0);
	else if( d > 0.0 && d <= 0.15 ) return Fract(1);
	else if( d > 0.15 && d <= 0.25 ) return Fract(2);
	else if( d > 0.25 && d <= 0.35 ) return Fract(3);
	else if( d > 0.35 && d <= 0.45 ) return Fract(4);
	else if( d > 0.45 && d <= 0.55 ) return Fract(5);
	else if( d > 0.55 && d <= 0.65 ) return Fract(6);
	else if( d > 0.65 && d <= 0.75 ) return Fract(7);
	else if( d > 0.75 && d <= 0.85 ) return Fract(8);
	else if( d > 0.85 && d < 1.0 ) return Fract(9);
	else return Fract(10);
}


