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
 // JJ: cleanup and removal of unused code
 // ----------------------------------------------------------------------------

#include "p_PropPool.h"
/** @cond */
#include <cmath>
#include "easylogging++.h"
/** @endcond */


PropPool::PropPool(int size, bool declining, int dTime) : m_Size(size), m_Declining(declining), m_DTime(dTime){}

/*----------------------------------------------------------------------------*/

void PropPool::PutSeedInPool(int Inp){
	if (Inp < m_Size){return;}
	m_Size = Inp;
	m_Declining = false;
	m_DTime = 0;
}

void PropPool::EmptyPool(){
	m_Size = 0;
	m_Declining = false;
	m_DTime = 0;
}

void PropPool::AgePool1(int pl){
	if (m_Size<=0){return;}

	/* Seed mortality rate follow a linear relationship as a function of seed life */
	/* size (n+1) = size (n) - size(n) * (1 / (pl + 1)) */

	double decRate = 1.0 / static_cast<double>( pl + 1.0 ); // calculate decreasing rate
	m_Declining = true; // new seeds, so the pool is declining
	m_DTime = m_DTime + 1; // increase age of youngest seeds
	m_Size = floor(m_Size - decRate * m_Size);

	if (m_Size == 0){
		m_Declining = false;
		m_DTime = 0;
	}
}

/*----------------------------------------------------------------------------*/

void PropPool::show() const{
	// logg.debug("Seed Pool : size = ", m_Size, ", declining = ", m_Declining,
	// 					 ", age = ", m_DTime);
}

int  PropPool::getSize() const { return m_Size; }

