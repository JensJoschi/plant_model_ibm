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

 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski (IBM)
 // ----------------------------------------------------------------------------


/*!
 * \file Community.h
 * \brief Plant community in a cell
 * \details Contains all individuals that live in a habitat(usually a voxel cell), sorted by type (species, functional group)
 */

#ifndef COMMUNITY_H
#define COMMUNITY_H

#include "Individual.h"
#include "SeedPool.h"

/** @cond */
#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <nlohmann/json.hpp>
/** @endcond */


/*!
 * \class Community
 * \brief Plant community in a cell
 * \details 
 * A plant community consists of multiple individuals as well as their seeds. Individuals (and seeds) may be of various
 * types (species, functional group), defining their growth characteristics, reosource use, seed biology etc. 
 * The community is responsible for managing the individuals and seeds, and for updating the community state.
 * The community lives on a soil - it has a max capacity and (in the future) may be depleted
 * */
class Community{
    friend class CommunityTest_rainSeeds_Test; //typical workflow of community is rainseeds() => age() => getBiomass of adult individuals; 
    //private access to SeedPool makes it easier to test that rainseeds provides the correct inputs.

    public:
    /**
     * \brief Construct a new Community object
     * \details creates a Community of plants
     * The constructor creates a number of suitable individuals, and seeds. 
     * The traits define which types of Individuals may be created in this community.
     * The community is filled with individuals of random types (species etc., see Traits class), but only those for which 
     * the habitat is suitable. The number of individuals is maxIndividuals, unless soil::capacity is reached before; or zero
     * if no suitable individuals are found. 
     * The seed pools of each suitable type have a capacity of 100 * soil::capacity seeds each (soil::capacity is the capacity 
     * for individuals, but it is assumed that many more seeds than individuals can be stored), and are initially filled with 
     * maxSeeds seeds (unless this exceeds SeedPool::capacity).
     * Soil is - biologically speaking - a shared resource, i.e., all individuals have access and may modify/deplete the soil.
     * \note community and the individuals have a weak_ptr to soil, as they are not responsible for the lifetime or memory 
     * allocation (biological resource sharing does not imply shared responsibility for memory management). Only the Voxel 
     * owns the soil and is responsible for its deletion.
     * \param traits traits define the types of individuals that may be created
     * \param maxIndividuals maximum number of individuals that will be created
     * \param initialseeds maximum number of seeds that will be created
     */
     Community(std::weak_ptr<Soil> soil, 
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds);
     
    ~Community();
    /**
     * \brief json-based community constructor
     * \details see other constructor for more details.
     * This constructor additionally contains a json with all neccessary information to create an individual
     * (e.g., age, biomass, but not life span or resource allocation stratgies)
     */
    Community(std::weak_ptr<Soil> soil,
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
        const nlohmann::json& individuals);
    
    /**
     * \brief add seeds to Pool
     * \details Additional seeds are deposited to the seedPool. These seeds may have dispersed from neighbouring cells, 
     * for example. Seeds of unknown type will cause an error
     * \param seeds list of seeds, named by type
     */
    void rainSeeds(const std::map<std::string_view, int>& seeds);

    /**
     * \brief let the community age
     * \details When the community ages by a time step, the following actions happen:
     * - Individuals age. Individuals that die are removed.
     * - The individuals allocate resource into growth and reproduction. This leads to the production of seeds.
     * The new seeds will be returned, not placed in the seed Pool. They may be dispersed elsewhere, eaten, or placed in
     * the pool with rainSeeds().
     * - new individuals are recruited from the seedpool (not from the new seeds). 
     * \note does not contain disturbance yet
     * @return std::map<std::string, int> Newly produced seeds of each type
     */
    std::map<std::string, int> age();

    /**
     * \brief Get the number of individuals of a certain type
     * \details If type is empty, returns the total number of individuals in the community.
     */
    int getCount(std::string_view type = "") const;

    /**
        * \brief Get the number of individuals of a certain type in a certain height range
        * \details If type is empty, returns the total number of individuals in the height range
        * This function is based only on indiviudals with a age>0 and an area (seedlings may have no biomass yet)
        * \param from lower bound of height section
        * \param to upper bound of height section
        * \param type type of individual
    */
    int getCount (int from, int to, std::string_view type = "") const;
    
    float getBiomass(std::string_view type = "") const; //age 0 excluded
   
   std::vector<std::weak_ptr<Individual>> getIndividuals() const;

    private:
    std::multimap<std::string_view, std::shared_ptr<Individual>> m_individuals; //sorts the individuals in the community by type
    std::map<std::string_view, std::unique_ptr<SeedPool>>  m_seeds; //sorts the seeds in the community by type
    std::map<std::string_view, const Traits*> m_traits;             //traits of all types (species, PFGs etc) that may occur in this community
    std::weak_ptr<Soil> m_soil; //soil that the community lives on (biologically a shared resource; but not computationally. See notes)
    /**
     * \brief create individuals from seeds
     */
    void recruit();

    /**
     * \brief attempts to create an individual. 
     * \details If successful, the individual is added to the community and (if necessary) a seed pool is created; 
     * otherwise returns false.
     * \param traits 
     * \param name type (species, PFG etc) name
     * \param soil 
     * \param maxSeeds 
     */
    bool createIndividualAndSeedPool(const Traits* traits, std::string_view name, int initialSeeds);
};

#endif // COMMUNITY_H