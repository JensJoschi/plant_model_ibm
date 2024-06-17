# Version

This readme concerns model version 1.1.0. Please make sure the documentation matches the version you would like to use. 

# Overview

This repository hosts the IBM-Plant model (name to be decided)

The folder /doc contains figures relevant for this documentation. 
The folder /include contains dependencies: nlohmann::json, easylogging++, and a few files from the ECOLOPES ECOLOGICAL MODEL.
The folder /out contains the binary files constructed by this repo.
The folder /rscripts contains a few rscripts to generate inputs or analyse outputs. More like scribbled notes than anything useful.
The folder /src contains all source files of the model, and a CMake file that compiles the model.
The folder /tests includes unit tests, a CMake file to build them, and all inputs for a test run
root further contains a master CMake file that compiles the binaries (executable and tests), a doxyfile for documentation of the code(click on the download button ->documentation, or go to CICD->pipelines and download there) and a logger configuration file (see easylogging help). Root further contains this README file and a separrate USER GUIDE. 

# COPYRIGHT notice

> Copyright (C)  2022 - present  Studio Animal-Aided Design
> Permission is granted to copy, distribute and/or modify this document  
> under the terms of the GNU Free Documentation License, Version 1.3  
> or any later version published by the Free Software Foundation;  
> with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.  
> A copy of the license is included in the section entitled "GNU  
> Free Documentation License".  

This readme file has been written by Jens Joschinski. Parts of it are derived from the ECOLOPES PLANT MODEL readme, https://gitlab.com/ecolopes-team/plantmodel .

# Authorship  

The precursor of this model is the ECOLOPES PLANT MODEL, https://gitlab.com/ecolopes-team/plantmodel. To add plant Individuals and to revise the calculation of light, the majority of classes and code was rewritten. The following classes(plus their tests) have been written from scratch:  
Traits; SoilRequirements; Soil; Seed Pool; Seed Biology; ResourceAlloc; PlantShape; PlantResource; PlantGrowth; LifeHistory; Individual; HabSuit; Community; Voxel; Illumination; SeedDistribution  
The only classes which are imported from Plantmodel (but strongly altered) are: GSP_PLANTS; Data_PLANTS; PlantModel; rng (no changes here). This amounts to about 1240 out of 6073 lines of code.

The "Authorship" section of individual files mentions the main contributor(s) to each file. If there is only a sole author or author group mentioned, then this person/group developed almost all (>90%) of the code. If there are further parties mentioned, their contributions are listed in detail in mostly chronological order. If an authorship statement is missing, all content was written by Jens Joschinski. 

The include/jointModel folder includes classes that were originally written for the joint model (plantModel and the other ECOLOPES models derive from them). The base and container classes were modified in the IBM model.

# 0 Preface

The model description *loosely* follows the ODD (Overview, Design concepts, Details) protocol for describing individual- and
agent-based models (Grimm et al. 2006), as updated by Grimm et al. (2020). 

# 1 Purpose and Patterns

The model aims to predict the presence and temporal change of plant communities in 3dimensional environments. It tracks the biomass and number of plant individuals from multiple species or types. It improves upon the ECOLOPES PLANT MODEL and includes Individuals, each with their own state including a resource pool. 
The outcome of the model outcome should reflect the community succession of plants, as affected by competition for light and space. In the worst case, the model should provide a phenomenological description by producing outcomes that can be used for relative comparisons among species, largely tracking the onset and demise of individual species in the correct order. With reasonable parametrization, the model should however also produce correct results in absolute terms, i.e. abundances and biomasses and their vertical distribution that can be verified with real-world data. 
The model is not expected to perform well in highly stochastic environments (e.g. erratic herbivore pressure) or in environments under soil nutrient limitation, as these processes are not included in the model.

# 2 Entities, state variables, and scales 

## Spatial and temporal resolution  

This model simulates plant communities in urban areas, in particular on building envelopes. While it does not constrain spatial extent, temporal extent or spatial resolution in any way, it is assumed that settings are around 10-200 time steps, 1m3 spatial resolution and 100 m x 100m spatial extent. Shading and plant competition might become unrealistic if lower spatial resolutions (coarser) are used. With much longer temporal extent, the model will require longer to run and create larger amounts of data, and very large spatial extents may cause memory issues.   
A time step could in principle be a year or a week, depending on the parametrization (growth rates, light conversion, units of light inputs). The model does, however, not contain any functions to model seasonal change, nor does it include leaf senescence (this is assumed to be part of maintenance costs). When assuming annual time steps, please mind that these make no sense for annual plants. The transition from seed to young plant becomes also difficult - how big should the plant be 1 year after germinating? The starting size is currently hardcoded in PlantResource constructor.

## Entities
| Entity | Description |
| ---    | --- |
| Environment | (urban) habitat containing voxel cells; and seeds that are not (yet) settled in voxels|
| voxel cell | a voxel cell containing a community of one or more plant individuals of different types (e.g. species); light; soil; and disturbances.|
| Individual | composed of an (aboveground) biomass and a (belowground) resource. Biomass converts light into resource, resource is spent on growth (height and biomass) and reproduction. Indidividuals cast shade, and can be disturbed. Individuals of different types (e.g. species) differ in traits.
| Traits | Each type of individual (e.g. species) contains a set of traits defining 1) Life history parameters, 2) resource allocation strategies, 3) seed biology, 4) disturbance response, 5) soil requirements and 6) shape.

### Individuals, seeds and traits

The principle agent is an individual plant. The plant has two life stages: a seed stage and the actual plant individual. 

The individual has the following attributes:
- it lives: it grows, reproduces and dies.
- it is photoautotrophic: it converts light into a (carbohydrate) resource; it lives from these resources and produces green (photosynthetically active) biomass
- it has a shape: it has a geometric representation, being able to e.g. cast a shade 
- it needs a place to live: the soil determines whether a plant can grow at all (habitat is suitable). Soil (or space) is a limiting resource.
- it interacts with the environment: Stressors can affect each of the above properties, i.e. cause mortality, deplete resources or affect the soil.

The seeds have the following attributes:
- they live: they can die, or they can be converted into individuals
- they can form seed banks: Seeds produced in autumn may germinate the next year, or they can stay dormant and be buried in the soil for many years. The mortality of dormant seeds is lower than the mortality of active seeds that attempt to germinate (germination rate)
- they need space: the soil can accomodate a potentially large, but not infinite number of seeds. 

The attributes are highly interdependent. For example, light is often a limiting resource and competing individuals in the same community can shade each other, so the spatial positioning of the biomass is important. The interdependences are currently kept to a minimum, but should be extended in the future. In particular:
- Age and other life-history parameters do not affect seed biology (except of course fecundity only starting at maturity) or resource allocation (except for height affectinh the location of biomass), and are neither affected by soil nor by resources or seed attributes.
- Resources do not affect heigth growth or seed attributes, and are not affected by soil.
- Soil is not conditioned in any way by the plants or seeds living in it, and it does not affect resource use, life history or ungerminated seeds
- disturbance is not fully implemented, and will anyway only affect resources and biomass, but not lifespan (height or mortality), seeds or soil 
- The plant shape is, however affected by height and biomass.

There is no single optimal strategy for life history, resource allocation and other attributes, rather the optimal strategy depends on the environment and the other individuals in the community. Hence, different trait combinations (strategies) have evolved. In this model, each individual belongs to a certain strategy type with fixed traits, i.e. there is no intraspecific trait variation, no evolution and no phenotypic plasticity. The trait types may be envisioned as species, but can also be genera, distinct subpopulations, functional groups or functional types.
 According to the attributes given above, traits that define an individual fall into 5 categories: 
 1) Life history parameters. Life history determines key demographics (maturity onset, lifespan) and height growth.
 2) resource allocation strategies. Resource allocation determines the relative investment into seeds and biomass gain (e.g. light conversion efficiency, fecundity).
 3) soil requirements. Plants can only grow on certain types of soil, and the soil needs to meet some minimum requirements (minimum depth).
 4) disturbance response. Disturbance response defines which types of disturbance ("rabbit", "fire") affect the plant, and how strongly.
 5) seed biology. The seed biology determines the mortality of seeds, dormancy attributes, and germination rates.  

 In addition, the plant has a certain shape; the shape determines the amount of biomass growing at a specific height. Knowing the plant's general shape, its current fresh biomass (g), and the mean density of the plant(g/cm2), one can infer the plant's leaf surface area(cm2) within a certain height section. This variable is important for the calculation of light in a voxel cell (see below)
 Shape does not change over time (a rectangular plant cannot become a lollipop tree), but its extent does (height, width). 

For details on the state variables, see section 7 ("submodels") or UML diagram.

There are a few implementation-specific notes affecting the general logic of the model:
- The model is written with extensibility in mind. Replacing one section (e.g. resource calculation) should have minimal effects on the functionality of the remaining model
- Plant shape is currently simplified to a rectangular shape. Further shapes can be added later
- Soil: soil has a capacity to simulate competition for space, but no other attributes that may be depleted. Hence, plants of different types compete for the same singular resource.
- disturbance: limited to depleting either the plant's biomass or its resources. Mostly not implemented yet.

### Voxel Cell
A voxel is a three-dimensional object that is embedded in a 3D-landscape. In our model it contains a soil, a plant community, and light (and disturbance, to add). 
*Soil*  
Soil is a shared resource, used by the Individuals (see above). It has a limited capacity to host plant individuals and its state may change over time. Some attributes are set at initialization and cannot change throughout the simulation run.  
Soil is not represented as 2- or 3Dimensional object, but as an object with one-dimensional properties (depth, type, capacity); stratification of soil types is  not yet included.

*Light*  
For the purpose of light calculations, the voxel can be conceptualized as a stratified (2-Dimensional) rectangle. Light beams that hit the community at an angle of 90 degrees first hit the uppermost strata and plants therein. The light is then distributed to all plant individuals living in this stratum. Any light that is not absorbed by the plants in the uppermost strata is passed on to the next strata, and so on.
The logic of light calculation follows RFate and the ECOLOPES PLANT MODEL, but with some improvements:
- light is an integer and not a factor, this allows converting watt/m2 into resources in joule (at least in principle; parametrization is required)
- plants on the same stratum do compete for light; 
- plants still do shade themselves, but as the upper and lower  parts share resources, they do not kill themselves. 

There is obviously a mismatch between the 2-Dimensional representation of plants and shading, and the 3-Dimensional nature of voxels.
For this reason the voxel is cut into very thin strata (e.g. 5 cm high); the surface area of all plants growing in the stratum is used to calculate how much light each plant (segment) receives, and how much light is passed on to the next stratum.  
Example: with 5 cm strata, a 1m x 1m x 1m voxel can harbour light for 20 * 100cm * 100cm = 200 000 cm2 surface area of plant material.
see issue #10.   

*The plant community*  
The plant community is made up of the Individuals and seeds. Although the plant community and all its individuals have their home in the Voxel Cell, they can outgrow it, i.e. the total amount of plant material in a stratum can become larger than a voxel cell's (or rather, stratum's) space. Plant biomass that spills into neighbouring voxels will reduce the neighbor's light levels, while still collecting light for the individual at home. Neighbour cells in this sense are the 2-dimensional neighbour cells of the "home" voxel cell. Vertical outgrowth of a voxel cell is not supported yet.  
In contrast to the ECOLOPES plant model, light no longer can pass through neighbouring voxel cells (see issue #8).  

*Disturbance* to add  

### Environment
The environment contains voxel cells in some specified spatial configuration; and seeds that are not yet settled in voxels. In each time step all individuals in each voxel are given the opportunity to produce seeds. These seeds do not immediately sink to the ground of the voxel cell, but are released into the environment. Here they are reshuffled and then distributed (randomly and uniformly) to voxel cells.  There is potentially additional seed input from the region in which the site is embedded
The spatial configuration of the environment determines where (in which cell) plants end up when they outgrow their home cell. Plants currently grow only into their home voxel's immediate 2D neighbour cells.

# 3. Process overview and scheduling
- create PlantModel (environment): in this step all input data, configurations and plant trait definitions is read in. It is checked that all input is consistent, and voxel cells are created in the correct spatial configuration and filled with the correct input data. Plant communities are created according to input data and configurations. If a regional model was provided, that only individuals of the fitting plant groups can occur in each voxel cell.  
- run time step:
  - add seeds from the region. If enabled, a config::seedRain * number_of_voxels seeds of each plant type are added to the environment.
  - add seeds to Voxels. The seeds that are in the environment settle in the assigned voxel cells.
  - distribute biomass. Calculates the plant material that outgrows a home voxel and registeres its biomass in the neighbour cells.
  - provide resources to voxels. Light is provided to each voxel cell, the amount being specified in the input data (light). The individuals living (partially) in the cells receive the light resource.
  - aging in each voxel. Lets each individual age, die (e.g. if too old or resources are insufficient) and potentially produce seeds. Also recruits new individuals from the current seed pool (but not from newly produced seeds). New seeds then are given to the environment.
  - Dispersal: seeds in the environment are reshuffled (random, uniform).
- save data. Total plant abundance, biomass, or abundance and biomass of certain groups is retrieved and saved. There are options to only save certain height intervals.  

The order ensures that new seeds may land in a cell and germinate in the same time step; but that the plant cannot additionally grow the same time step. It is also ensured that newly produced seeds disperse, but cannot germinate in the same time step.

# 4. Design concepts

## Basic principles
The model combines habitat filtering approaches with process-based modelling, with particular focus on competition for light. The soil and nutrient balance as well as disturbance are on purpose left as simple and general as possible. The model is, however, written in a way that allows later extension.

FATE follows field-specific software engineering principles of ecological models. See (Scheller et al., 2010; Vedder et al., 2021) for an overview of common approaches, potential drawbacks and risks.
The plant model improves on this and provides a platform prototype that is planned to be further extended in the future. It is, however, neither accurate nor correct ,as it inherits conceptual decisions from RFATE which may not work very well on a very high resolution.  
This implementation adds extensive testing, proper inheritance structures and similar software engineering approahces, making the model more maintainable in the future. Moreover, it adds Individuals, which is an improtant step towards higher accuracy and realism.


## Emergence and interaction 

The key outcome of the simulation is the spatiotemporal change of community patterns, which emerges from the interaction of individuals within each voxel cell (particularly competition for light). On a landscape scale, plant material outgrowing a voxel (i.e., large trees) can also cause very localized patterning. Abiotic factors (soil depth, soil class) further impose a general community structure via habitat filtering. The abiotic factors (shading, in particular) further affect some competition outcomes, leading to the emergence of complex environment-community relationships.

## Adaptation

The  model does not contain any direct objective-seeking mechanisms, i.e. the agents do not possess the ability to choose among two or more alternative behaviors e.g. via learning or condition-dependent rules (phenotypic plasticity).

## Objectives

The model does not simulate adaptive behavior (fitness objectives). The rules by which agents respond to the environment (the "genotypes" or traits) are instead provided by the user.

## Interaction

Individual plants interact with one another, directly or indirectly, especially in the competition for light, which is explained in detail in the submodel section. Briefly, growth and death of plants continually changes light conditions, and, subject to light tolerances, plants may be outcompeted and die. This leads to turnover of plant communities. 
There is further simple competition for soil as a shared resource.

## Stochasticity 

The model contains the following stochastic processes:
1) at initialization, plant individuals are picked randomly from the pool of suitable types, until each cell's capacity is reached. Their initial biomass is fixed (non-random).
2) when the total plant biomass exceeds a voxel's capacity, a random set of plant material is chooosen to lie "on top" and receive the light. The granularity of the draw (how much plant material to choose per draw/number of draws) can be adjusted by the user.
3) At the end of the time step, all plant seeds are pooled and randomly redistributed over the simulation environment, together with the seeds that enter from the region (seed rain). 

# 5. Input and Output Data
Largely copied from the Ecolopes Plant Model, only adjusted contents.
- classes are GSP_Plants, Data_PLANTS, Traits. Voxel data on landscape scale is saved in container class Landscape   

# 6. Initialization
At initalization each cell is filled with the according input data and with a starting Community. The Community created by randomly drawing from a list of suitable types until the capacity (or starting population) size is reached; an according seed pool is also created. Individuals are created with a fixed initial biomass, often 5.0 or 100.0 (see PlantResource.cpp, contstructor)

# 7. Submodels

## Plant growth
This submodel lets the plant age, mature and grow in height. The life history traits defining the plant's growth are:  
	- int MatAge: Age at which the plant reaches maturity and can start reproducing.   
	- int LifeSpan; Maximum age that a plant can reach, if not dying of other causes. The age is entirely deterministic (no random deviations in life span).  
	- int HMax; Maxumum height a plant can reach.  
The plant's age is incremented in every growth cycle; if it exceeds the life span, it dies.  
The growth in height is resource-independent and uses a logistic growth function, with the inflection point being the age of maturity (when plants invest into seeds, their growth decelerates), the slope is arbitrarily set to 1.0. The height defines only the potential positioning of biomass, but does not imply a certain biomass or shape. Biomass growth is on purpose entirely height-agnostic and has its own submodel (Resource use).

## Habitat suitability 

The habitat suitability submodel checks for each plant type in which cells it can theoretically occur (habitat filtering).
The soil requirements defining the habitat suitability are:  
- int minDepth; minimum depth that the soil must have to be considered suitable  
- std::map<std::string, bool> acceptedSoils; list of soils on which this plant can grow  
- int size determines how much space the individual takes (there is only a maximum number of "seats" available in each soil)
While seeds may be deposited on any soil, and also germinate, they only are recruited (converted into Individuals) if the soil is at least as deep as minDepth, and one of the acceptedSoils of the plant.
Already established plants will not be destroyed if a habitat becomes unsuitable. This may, however, affect efficient resource use or storage (in future versions) 

## Disturbance  

The disturbance model allows to provide a list of disturbances. The disturbances can destroy biomass, deplete the resource pool, or remove seeds.
Disturbance is not yet implemented.

## Shape
The plant's shape describes the location of biomass, which affects the light that can be collected and the shade the plant casts. 
The most important function of the shape submodel is the calculation of the plant surface area, either of the total plant, or (more useful) of certain height section. The surface area determines how much light a plant can intercept, and is thus used to calculate the light household of a voxel cell.
Currently all plants have a rectangular shape, and the surface area is homogenous along all the plant's height. 
The traits the shape submodel uses are: 
-    float density; the density of the plant describes how compact the biomass is distributed. Like Specific Leaf Area (SLA), it is expressed in m2/kg. However, in contrast to SLA, the density in this model describes the **area per fresh aboveground biomass**. Density in this model can be calculated from SLA, water content and woodiness (possibly just SLA / 1000 or a similar universal scaling factor)  
-    Height is determined by the Plant Growth model, and biomass is calculated by the Resource model.  
The calculation of specific leaf area is simply: (height section/total height) * biomass/density  
Example: A 10 m high tree has a biomass of 1000 kg, so there are 200 kg of plant material in the section between 4 and 6 m. With a density of 0.1m2/kg, the 200 kg cover a surface area of 20 m2.  
Note: density is the inverse of SLA. The variable will eveentually be renamed to specificArea, and the calculations will be inverted. For now, the variable put into the model should be "density": {scaling factor * 1/SLA} 

## Resource use
The resource submodel takes care of collecting light, converting it into resources, and spending resources on growth and reproduction.
The resource allocation traits that determine Resource use are:
 -   float conversionEfficiency; The photosynthetic efficiency of the plant. Typical values are in the order of 0.005 (https://en.wikipedia.org/wiki/Photosynthetic_efficiency;
 -   float maintenanceCosts;
 -   float seedAllocation; proportion of the resources that is available for allocation into biomass in each growth cycle
 -   float biomassAllocation; proportion of the resources that is available for allocation into biomass in each growth cycle
 -   int maxBiomass;  Maximum biomass that can be reached

The plant converts light into a resource. One light unit (in watt/m2) is converted into conversionEfficency resource units (e.g. carbohydrates in g). Typically, the photosynthetic efficiency is in the order of 5% but this includes direct above-and belowground biomass gain; storing as starch incurs further cost, so a reasonable estimate is approx 1% or less.  
The plant needs to pay an annual maintenance cost of maintenanceCosts * biomass  
The plant can allocate the fraction seedAllocation of the resources to reproduction and up to biomassAllocation to biomass. Seed production is only resource-limited  
Biomass growth follows a sqrt function, i.e. the growth rate decelerates as the biomass approaches maxBiomass; this limits biomass even if enough resources are available. If less resources are available, the growth is limited accordingly; The amount of available resources is not the whole full resource pool, but only resource pool * biomassAllocation.  
If the plant runs out of resources, it dies.

## Seed Pool
The seed pool submodel simulates the life of seeds before they are recruited and converted into individuals. 
The seed biology definings seeds are:  
-    bool dormancy;   
-    float germinationSuccess;  
-    float mortalityDormant;  
-    float dormancyBreakRate;  

The seed pool contains up to c seeds of a plant type (species, PFG etc.); the cell and its soil are responsible for determining c.  
Seeds may be dormant or active; if dormant, they have a mortality of mortalityDormant in each growth cycle, if active they suffer germinatSuccess mortality upon attempting to turn into an Individual. Active seeds cannot turn into dormant seeds, but dormant seeds are converted at a rate of dormancyBreakRate into active seeds in each growth cycle.  
Seeds may be disturbed (affecting only acitve or in equal proportions active and dormant seeds), and new seeds (active or dormant) may be added to the seed pool.  

## Dispersal  
random uniform redistribution of any newly produced seeds + input from region.

## Illumination
The voxel is represented as a stack of rectangles with the same area. Light hitting a voxel will first hit the uppermost stratum. It will be distributed to all plants inhabiting the voxel at the height. The remaining light leaving a stratum is diffused and homogenized over the full area of the next stratum. This ensures that the plant shades not only itself but also its competitors. Exampöe:
If a plant covers 2/3 of the uppermost stratum's area, and 1/3 of the area in the next stratum, it receives 66% of the light in the upper stratum. The remaining third of the light gets passed on to the next stratum. There, the plant receives 1 third of the remaining light (11%). The remainder(22%) may be distributed to other plants, or passes through the stratum without hitting the plant. 
Without the homogenization the uppermost part would receive 66%, but the lower part would be entirely shaded and receive nothing; and 33% of the light would be lost. 
The illumination submodel includes both the plants originating in a voxel, and material spilling over from neighrbour voxels (see "Plant Community") When there is too much material in the voxel (both from plants growing here and spilling over from neighbours), a top layer  of plant material is randomly selected and receives light. The number of draws is a parameter (lightDistributionFactor).

Disturbance to add



# 8. Implementation

>>>
| concept | detail |
| --- | --- |
| Operating system | Portable to any common OS |
| Programming language | c++ 20 (or newer) |
| Precursors | FATE-HD, RFate; ECOLOPES PLANT MODEL; some code (data containers, inputs) shared with joint ECOLOPES models |


Because this is work in progress, it currently only works on macOS and linux.

## Dependencies and requirements

All dependencies are included in the /include folder (ECOLOPES joint model for data containers, nlohmann::json and easylogging). The model has no specific requirements to run (< 1 GB hard disc space, < 2GB RAM, any modern CPU), but compilation requires cmake and a c++20 compiler (it is tested llvm and MinGW's GNU compiler, but should also work on AppleClang). See USER_GUIDE.md for installation notes.


## Implementation overview

![UML] (doc/UML.jpg)

## Ideas for the future 


*addition of other shapes* 
