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
 // Jens Joschinski (IBM)
 // ----------------------------------------------------------------------------


/*!
 * \file SeedPool.h
 * \brief Seed pool class
 * \details models behavior of seeds. Seeds can turn into Individuals, or die.
 */

#ifndef SEEDS_H
#define SEEDS_H
#include "Traits.h"
#include "SeedBiology.h"
/*!
 * \class SeedPool
 * \brief Seed pool class
 * \details A seed is of a specific type (e.g. species or pant functional group). Individual seeds can lay dormant 
   or they can be active. There is a certain chance that seeds die (different for germinating and dormant seeds) every time step,
   and a certain chance that they turn into Individuals (of the same type). The attributes describing the seeds (mortality etc)
   are defined in the SeedBiology class, which is part of the Traits of the species or plant functional group.
   This class is expected to be part of a Cell class and contain all seeds of a specific type in a cell, independent of the identity of their parent 
   \note if this class were moved to become composite of Individual, it could be used for evolutionary simulations.
   \note one could also posit polyphenism, i.e. have two distinct seed pools
 */
class SeedPool{
    friend class SeedPoolTest_builds_Test; ////lets unit test "SeedPoolTest::builds" access this class
    friend class SeedPoolTest_increaseDormant_Test;
    friend class SeedPoolTest_increaseActive_Test;

    friend class SeedPoolTest_germinate_Test; //tests for private classes, will be removed eventually
    friend class SeedPoolTest_decrease_Test;
    friend class SeedPoolTest_decreaseOnlyActive_Test;

    public:
    /**
     * \brief constructor
     * \details creates a seed pool with a certain capacity
     * \param traits pointer to the traits of the species or plant functional group
     * \param c capacity of the seed pool
     */
    explicit SeedPool(const Traits* traits, const int c);
    /**
     * \brief constructor
     * \details creates a seed pool with a certain capacity
     * \param SeedBiology pointer to the seed biology of the species or plant functional group
     * \param c capacity of the seed pool
     */
    explicit SeedPool(const SeedBiology* seedBiology, const int c);

    /**
     * \brief increase seed pool by n seeds
     * \details put new (dormant) seeds in the pool
     * \param n number of new seeds
     */
    void increase(int n, bool active = true);

    /**
     * \brief let the seedPool age for one time step
     * \details includes general mortality and specific disturbance, activation of dormant seeds, and germination. 
     * \brief distMagnitude magnitude of the disturbance
     * @return int number of germinated seeds
     */
    int age(int distMagnitude);

//============================================================================
    private:
    /**
     * \brief decrease seed pool by n seeds
     * \details remove a specific number of seeds from the pool. Normally the environment does not distinguish between
     * active and dormant seeds and draws them randomly.
     * If there are less than n seeds in the pool, only n seeds are removed.
     * \param n number of seeds to remove
     * \param onlyActive if set to true, only active seeds are removed
     * \return number of seeds that were removed (<= n)
     */
    int decrease(int n, bool onlyActive = false);

    /**
     * \brief disturb the seed pool
     * \details reduce the number of seeds in the pool by a certain amount. The amount is determined by the 
     *  disturbance type-specific effect size (e.g. herbivore: 0.2) times the amount of disturbance (4). 
     * Effect size is written into the DisturbanceTrait class. 
     * \param disturbance name and amount of the disturbance (e.g. "herbivore": 0.2)
     * @return number of seeds that were removed by each disturbance
     * \note unimplemented.
     */
    std::map<std::string, int> disturb(const std::map<std::string, float>& disturbance, bool onlyActive = false);


    /**
     * \brief germinate seeds
     * \details reduces active seeds of the seed pool. The number of seeds that germinate 
     * is determined by the germination rate
     * \note although this is a getter function, it should almost never be called on its own. 
     * Usually we also want to activate seeds and calculate mortalities. Made this function private 
     * for now and only leave the wrapper function age() public.
     * \return number of seeds that germinated
     */
    int germinate();


    /**
     * \brief activates a fraction of dormant seeds
     * \details moves a fraction of dormant seeds to the active pool. The amount is determined by the activation rate (dormancy break rate)
     * \note could add environment-induced activation (e.g. fire, temperature) here
     */
    void activate();

    /**
     * \brief a certain fraction of dormant seeds dies
     * \details reduces the number of seeds in the dormant pool by a certain fraction. The fraction is determined by 
     * the seed mortality rate
     */
    void killDormant();

    const int m_capacity;
    int m_activeSeeds;
    int m_dormantSeeds;
    const SeedBiology* const m_seedBiology_ptr;
};
#endif // SEEDS_H

