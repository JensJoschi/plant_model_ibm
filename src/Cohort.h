/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

This file is part of the INDIVIDUAL-BASED PLANT MODEL.

INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL.
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* INDIVIDUAL-BASED PLANT MODEL is derived from the ECOLOPES PLANT MODEL, Copyright (C) 2022-present Studio Animal-Aided Design.

 * ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // RFate team
 // JJ: simplification to a struct
 // --------------------------------------------------------------------------


/*============================================================================*/
/*                                Cohort Class                                */
/*============================================================================*/

/*!
 * \file p_Cohort.h
 * \brief Struct to store Plants abundance
 * \author Jens Joschinski
 * \version 1.0
 * \date 2023/03/01
 */


#ifndef COHORT_H
#define COHORT_H



/*!
 * \struct Cohort
 * \brief Basal Structure to store PFG abundances
 *  \details
 * A cohort is the formal unity for storing plant abundances in optimal way.
 * It is represented by individuals of different age but of the same abundance.
 * Thus, a cohort is defined by the ages of youngest and oldest individuals, and
 * by an integer representing the abundance of each age.
 * For example, a cohort defined by (100, 2, 4) contains 300 individuals aged
 * from 2 to 4 years.
 * \note JJ: not sure this way of storing information saves much memory (only if 
 * demography is quite homogenous, i.e. abundance of several age classes is the same);
 * but it increases computation time - see comment in FuncGroup.h
 */
struct Cohort{
	int CSize; /*!< Abundance of each age of the Cohort */
	int Ay; /*!< Age of the Youngest individuals */
	int Ao; /*!< Age of the Oldest individuals */
};
#endif
