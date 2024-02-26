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
#include "PlantGrowth.h"
#include "PlantResource.h"
#include "HabSuit.h"

/** @cond */
#include <string>
#include <memory>
#include <utility>
/** @endcond */


/*!
 * \class Individual
 * \brief single plant individual
 * \details 
 * Objectives:
 * This class simulates an individual plant. The individual has the following attributes:
- it lives: it has a geometrical shape, it grows, reproduces and dies.
- it uses light: it has a metabolically active (above-ground) biomass, which is used to collect light. Light resources are 
   needed for growth and reproduction.
- it needs a place to live: the soil determines whether a plant can grow at all (habitat is suitable). Soil 
   (or space) can be seen as a limiting resource.
- it can be disturbed by the environment: Stressors can affect each of the above properties,
   i.e. cause mortality, deplete resources or affec the soil.

The four attributes (life, light, soil, disturbance) are implemented as separate classes. They are highly interdependent,
however, and the Individual class takes care of their interaction. The interdependences are currently kept to a minimum, but should
be extended in the future. In particular:
- Age and other life-history parameters do not affect seed biology (except onset of fecundity) or resource allocation 
  (except for height affecting the location of biomass), and are neither affected by soil nor by resources
  or seed attributes.
- Resources do not affect heigth growth or seed attributes, and are not affected by soil.
- Soil is not conditioned in any way by the plants or seeds living in it, and it does not affect resource use, life history 
or ungerminated seeds
- disturbance is not fully implemented, and will anyway only affect resources and biomass, but not lifespan (height or 
mortality), seeds or soil 


 * Implementation details:
 * The trait combination that makes up the plant's strategy (representing e.g, species, functional group or functional type) is encoded 
 * in the Traits class rather than using class inheritance patterns. Having a separate class allows traits to 
 * be read from file and easily updated (or used in an evolutionary model, though this is currently not planned). The traits class in turn 
 * is made up of the classes "ResourceAllocation", "SoilRequirements", "LifeHistory" and "SeedBiology". Changes to either functionality 
 * (aging, resource allocation...) only requires changes to the according Trait subclass (possibly using inheritance patterns).
 
 //TO MOVE TO DISTURBANCE class:
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
*/
class Individual{
   friend class IndividualTest_builds_Test; //give unit test IndividualTest::builds access to private members
      public:
      /**
       * \brief create an individual (factory function)
       * \details creates an individual, but only if the soil is suitable. Otherwise returns a nullptr.
       * \param traits life history, resource allocation, disturbance and soil traits
       * \param soil soil on which Individual shall live
       */
    static std::unique_ptr<Individual> create(const Traits* traits, std::shared_ptr<Soil> soil){
      HabSuit habSuit(traits->soilReqs, soil);
      if (habSuit.isSuitable()){
           return std::unique_ptr<Individual>(new Individual(traits, soil));
      }
      else{
           return nullptr;
      }
   }  
private: 
      /**
       * \brief constructor
       * \details Individuals need to be created through Individual::create, which only constructs
       * Individual if soil is suitable.
       */
    Individual(const Traits* traits, std::shared_ptr<Soil> soil);

      public:
    /**
     * \brief turn light into a resource
     * \details light is forwarded to the ResourcePool, which converts the light into resources
     * Resources can only be used if the soil is suitable.
     * \param light amount of light (in lux)
     */
    void feed(const int light);

    /**
     * \brief let plant be disturbed
     * \details Disturb the plant's resources. Depending on type of disturbance, 
     * it may have no effect, remove biomass, or deplete resources.
     * If the plant is smaller than the disturbance, the remaining 
     * amount of disturbance is returned
     * \param disturabnces name and amount of disturbances 
     * @return non-consumed disturbance
     * \note not yet implemented
     */
    std::map<std::string, float> disturb(const std::map<std::string, float>& disturbances);

    /**
     * \brief let plant age
     * \details when aging, the plant allocates resources into growth, reproduction and maintenances
     * \return bool does the plant survive, int number of seeds produced (which may be zero)
     */
    std::pair<bool, int> age();

      /**
      * \brief Get the current biomass
      * \details Return biomass between two heigths; if shadingCorrected is true, it returns instead the shade 
      * provided by the biomass
      * \param from lower bound of the window
      * \param to upper bound of the window
      * @return int biomass in the window
      */
    int getBiomass(int from, int to, bool shadingCorrected = false) const;


    private:
   std::unique_ptr<PlantResource> m_resource_ptr;
   std::unique_ptr<PlantGrowth> m_growth_ptr;
   std::unique_ptr<HabSuit> m_habSuit_ptr;

};

#endif //INDIVIDUAL_H

