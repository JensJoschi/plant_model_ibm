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
 * \file p_Individual.h
 * \brief single plant individual
 * \details 
 */

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "Traits.h"
#include "PlantResource.h"
#include "LifeHistory.h"
#include "SoilRequirements.h"

/** @cond */
#include <string>
/** @endcond */

//to move in sep file
class Soil{
    public:
    Soil(int capacity, int depth, const std::string& name): m_capacity(capacity), m_depth(depth), m_name(name){};
    void updateSoilClass(const std::string& name){m_name = name;};
    const int m_capacity;
    const int m_depth;
    private:
    std::string m_name;
};


/*!
 * \class Individual
 * \brief single plant individual
 * \details 
 * Objectives:
 * This class simulates an individual plant. A plant consists of (above-ground) biomass and a (belowground) resource storage:
 * - The biomass is used to collect light and convert it into resources. Biomass has a certain two-dimensional distribution, and 
 *   the distribution is important, because competing individuals in the same community can shade each other. Generally speaking, it is 
 *   beneficial to place as much biomass as high as possible, to reduce the risk of shading, but it also represents considerable investment. 
 * - Resources are chiefly carbohydrates (converted from light), though soil nutrients can also affect the ability to grow. 
 *   The resource can be spent on biomass (for more resource collection), growth (competitive advantage), reproduction 
 *    (seeds that will create copies of self), and maintenance (upkeep costs of biomass). 
 * The individual may be further disturbed by environmental factors, depleting either the plant's biomass or its resources.
 * The lifespan of the plant is limited, and it will die if it runs out of resources or reaches its maximum age. 

 * There is no single optimal strategy for resource allocation and spatial distribution of biomass, rather the optimal strategy depends on the
 * environment and the other individuals in the community. Hence, different trait combinations (life history strategies) have evolved. 
 
 * Implementation details:
 * The trait combination that makes up the plant's strategy (representing e.g, species, functional group or functional type) is encoded 
 * in the Traits class rather than using class inheritance patterns. Having a separate class allows traits to 
 * be read from file and easily updated (or used in an evolutionary model, though this is currently not planned). The traits class in turn 
 * is made up of the classes "ResourceAllocation", "SoilRequirements", "LifeHistory" and "SeedBiology". Changes to either functionality 
 * (aging, resource allocation...) only requires changes to the according Trait subclass (possibly using inheritance patterns).
 * Currently this class (Individual) handles aging and soil use, but Seed Biology and Resource allocation each have their own class. 
 * This separates the biologically informed calculations from the rest of the code and allows for easy replacement and updating.
 
 * In this model, plant shape is simplified to a rectangular shape, and biomass is homogenously distributed from 0 to height.
 * The rectangle may grow in height, according to the annual investment in growth (and resurce availability), and the biomass may 
 * increase according to the annual investment in biomass (and resource availability). More biomass per m height means a wider rectangle.
 * The allocation of resources into growth and biomass is defined in the traits class (same for all Individuals of the same type).
 * For now, the plant's growth in height is independent of resources or biomass and uses a logistic growth function, with the inflection point
 * being the age of maturity (when plants invest into seeds, their growth decelerates); the slope is arbitrarily set to 1.0.
 * \note In RFate, a slightly different formula was used, as the plants reached Immsize*MaxStratum at maturity/2. The curve apparently is not sigmoidal, 
 * but in the beginning a straight line, which then decelearates (no slow start). Immsize is hard to guess from data anyway so it was not used here.
 
 * \note In this model there is only a single form of biomass, but woody plants contain both structural (dead) 
 * and metabolically active biomass. A slower height growth and a lower allocation to (metabolically active) biomass gain, or a low light conversion
 * rate, can be used to simulate woody plants. In a pure herbal plant, on the other hand, the conversion rate and the height growth are faster. 
 * (there is however no way yet to have herbivores or mowing reduce the height, only the biomass or resources.)
 
 * \note There are at least three different ways to disturb a plant: 
    1) an insect may consume leaves or directly tap into the resources (aphids). This does not really affect the standing biomass, plant height, 
       or the ability to shade competitors; but it reduces the resource pool and possibly the growth ability in the next year.
    2) Pruning may remove standing biomass, which affects the ability to shade competitors in this year, and the biomass will take a while to grow back; 
       the resources are not directly affected, however. Height is also unaffected.
    3) a large browsing mammal removes enough leaves to affect shading, but the plant may grow back in the course of the same year. This does not affect
       the standing biomass of next year (or height), but temporarily affect shading. If the lost biomass is recovered in the same year, 
       it imposes a cost to resources.
    Cases 1 and 2 are implemented (disturb() and depleteResources()), but case 3 is not yet implemented. It may require additional trait data.
    In theory one could also reduce a plant's height, not only the biomass. This is also not implemented.
* \note this individual class has a rectangular base shape. One may derive a class with a different shape (lollipop, triangular) and override the 
* getBiomass(from, to) function. 
*/
class Individual{
    friend class IndividualTest_builds_Test; //lets unit test "IndividualTest::builds" access this class
    friend class IndividualTest_getBiomass_Test;
    friend class IndividualTest_doesItDie_Test;
    friend class IndividualTest_disturbWithoutRange_Test;
    friend class IndividualTest_disturbWithRange_Test;
    friend class IndividualTest_age_Test;

    public:
    explicit Individual(const Traits* traits, std::shared_ptr<Soil> soil);
    ~Individual();
    /**
     * \brief turn light into a resource
     * \details light is forwarded to the ResourcePool, which converts the light into resources
     * Resources can only be used if the soil is suitable.
     * \param light amount of light (in lux)
     * \note soil is currently ignored
     */
    void feed(const int light);

    /**
     * \brief reduce individual biomass by a certain amount
     * \details removes a certain amount of biomass. If the plant is smaller than the disturbance, the remaining 
     * amount of disturbance is returned
     * \param amount disturbance 
     * @return non-consumed disturbance
     */
    int disturb(int amount);

//this overload checks first whether / how much the disturbance affects the plant. to write
   //  int disturb(int amount, const std::string& name); 

    /**
     * \brief reduce individual biomass by a certain amount
     * \details attacks a plant at a specific height window (between "from" and "to") and removes a certain amount of biomass. 
     * If the biomass in the window is smaller than the disturbance, the remaining 
     * amount of disturbance is returned
     * \param amount disturbance 
     * \param from lower bound of the window
     * \param to upper bound of the window
     * @return non-consumed disturbance
     */
   int disturb(int amount, int from, int to);

    /**
     * \brief reduce saved resources by a certain amount
     * \details removes a certain amount of resources. If the plant has fewer resources than required, the remaining amount
     * of depletion is returned. 
     * \param amount of resource depleted 
     * @return int amount of resource that could not be depleted
     */
    int depleteResources(int amount); 

    /**
     * \brief Check whether conditions for death are met
     * \details Plant dies if there is critical lack of resources or lifespan exceeded
     * \note soil is currently ignored
     */
    bool doesItDie() const;

    /**
     * \brief Get the current biomass
     */
    int getBiomass() const;

    /**
     * \brief Get the Biomass between two heights
     * \details The biomass is distributed homogenously throughout the plant height. 
     * This function returns the biomass in a window between "from" and "to".
     * \param from lower bound of the window
     * \param to upper bound of the window
     * @return int biomass in the window
     */
    int getBiomass(int from, int to) const;

      /**
      * \brief Get the current shading
      * \details The shading is calculated from the biomass and the plant's shading factor. 
      * The shading is the amount of light that is blocked by the plant. 
      * \param from lower bound of the window
      * \param to upper bound of the window
      * @return int shading in the window
      */
    int getShading(int from, int to) const;

   /**
   * \brief Get the current plant height
   */
    int getHeight() const;


    /**
     * \brief let plant age
     * \details when aging, the plant allocates resources into growth, reproduction and maintenances
     * \return int number of seeds produced
     */
    int age();


    private:
    const LifeHistory* const m_LifeHist_ptr;
    const SoilRequirements* const m_SoilReq_ptr;
    
    std::unique_ptr<PlantResource> m_resPool_ptr;
    int m_age;
    float m_height;
    int m_biomass;
    std::shared_ptr<Soil> const m_soil_ptr;

    /**
     * \brief allocate resources
     * \details a certain amount of resources is reserved for maintenance; some is used for growth (height or biomass gain), some 
     * for reproduction (seeds), and some is saved. The proportions are defined in the Traits::ResourceAlloc attribute
     * @return int number of seeds produced. Has side-effect of increasing biomass (usually) and depleting resources
     * \note soil is currently ignored
     */
    int useResources();

};

#endif //INDIVIDUAL_H

