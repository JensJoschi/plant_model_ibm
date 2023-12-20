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


/*!
 * \file g_FGUtils.h
 * \brief a bunch of enums and general functions used by plant model
 * \details  several numerical values are turned into categorical data within FATE. These are coded as enums within this file. 
 * \note the use of Fracts does not make much sense, the fract enums are stored as ints. simple conversion of double->int would be better
 * \note the fracts should probably be their own class, or at least namespaced together with the fract-functions.
 * \author Damien Georges, Maya Gueguen
 * \version 1.0
 * \date 2022
 */



#ifndef FGUTILS_H
#define FGUTILS_H
/** @cond */
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <vector>
/** @endcond */




/*!
 * \enum Abund
 * \brief Factorial PFGs Abundances
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(Abund::Acount)
 */
enum Abund{
	ANone,
	ALow, 
	AMedium,
	AHigh, 
	Acount
};

/*!
 * \enum PoolType
 * \brief Different Propagules Pool type
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(PoolType::PTcount)
 */
enum PoolType{
	DormantP, /*!< Dormant Seed Pool */
	ActiveP, /*!< Active Seed Pool */
	PTcount
};

/*!
 * \enum Resource
 * \brief Factorial Light Resources
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(Resource::Rcount)
 */
enum Resource{
	RLow, /*!< Shaded area */
	RMedium, /*!< Semi-shaded area */
	RHigh, /*!< Lighted area*/
	Rcount
};

/*!
 * \enum LifeStage
 * \brief PFGs individuals possible Life Stages
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(LifeStage::LScount)
 */
enum LifeStage{
	Propagule, /*!< Propagule Stage (= Seed) */
	Germinant, /*!< 1st state of Plants */
	Immature, /*!< Immature Stage */
	Mature, /*!< Mature Stage */
	LScount
};

/*!
 * \enum Fract
 * \brief Numerical fraction used in Modelling
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(Fract::Fcount)
 * \note I guess that the Fracts are supposed to increase performance. However, enums are translated into 
 * integers in the assembly code. One could just as well use integers from 0 -100 without any performance loss.
 */
enum Fract{
	PC00, /*!< 0% */
	PC10, /*!< 10% */
	PC20, /*!< 20% */
	PC30, /*!< 30% */
	PC40, /*!< 40% */
	PC50, /*!< 50% */
	PC60, /*!< 60% */
	PC70, /*!< 70% */
	PC80, /*!< 80% */
	PC90, /*!< 90% */
	PC100, /*!< 100% */
	Fcount
};

/*!
 * \enum Fract2
 * \brief Factorial fraction used in Modeling
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(Fract2::F2count)
 */
enum Fract2{
	F2None, /*!< = 0% */
	F2Low, /*!< = 10% */
	F2Medium, /*!< = 50% */
	F2High, /*!< = 90% */
	F2All, /*!< = 100% */
	F2count
};

/*!
 * \enum DistFate
 * \brief PFG behaviour in response to Disturbances
 * \details last level tracks how many factors there are. Get the number with 
 * static_cast<int>(DistFate::DFcount)
 */
enum DistFate{
	Kill, /*!< Plants die */
	Unaff, /*!< Plants are not affected */
	Respr, /*!< Plants resprout (a parameter define resprouting age */
	DFcount
};

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/*-------------------------------------------*/
/* Routine functions definition -------------*/
/*-------------------------------------------*/

/*!
 *	\brief Calculate the complement from a given fraction to 100%
 *
 *	This function returns the complement of one or two given fractions to reach
 *	100%. All involved fraction are categorical (input and output).
 *
 *	\param f1 : the first fraction to be completed
 *	\param f2 : an optional second fraction
 *	\return categorical fraction corresponding to 100% - (f1 + f2)
 */
Fract getLeavingFract(Fract f1, Fract f2 = Fract(0));

/*!
 *	\brief Convert a categorical Fract fraction into scalar one
 *
 *	This function returns a number between 0 and 1 corresponding to a given
 *	categorical fraction.
 *
 *	\param fract : a categorical fraction of class Fract
 *	\return a number between 0 and 1
 */
double FractToDouble(Fract fract);

/*!
 *	\brief Convert a categorical Fract2 fraction into scalar one
 *
 *	This function returns a number between 0 and 1 corresponding to a given
 *	categorical fraction.
 *
 *	\param fract : a categorical fraction of class Fract2
 *	\return a number between 0 and 1
 */
double FractToDouble(Fract2 fract);

/*!
 *	\brief Convert a scalar fraction fraction into categorical one
 *
 *	This function returns the closest categorical fraction to a given number
 * between 0 and 1.
 *
 *	\param d : a 0-1 scalar number
 *	\return a categorical fraction of class Fract
 */
Fract DoubleToFract(double d);



#endif //FGUTILS_H_INCLUDED
