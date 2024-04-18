# Version

This readme concerns model version 1.0.4. Please make sure the documentation matches the version you would like to use. 

# Overview

This repository hosts the IBM-Plant model (name to be decided)

The folder /doc contains figures relevant for this documentation. 
The folder /include contains dependencies: nlohmann::json, easylogging++, and a few files from the ECOLOPES ECOLOGICAL MODEL. It also includes the public header file of the model library (the output of this repo).
The folder /out contains the binary files (including the library) constructed by this repo.
The folder /rscripts contains a few rscripts to generate inputs or analyse outputs. More like scribbled notes than anything useful.
The folder /src contains all source files of the model, and a CMake file that compiles the model as executable.  
The folder /tests includes unit tests, a CMake file to build them, and all inputs for a test run
root further contains a master CMake file that compiles the binaries (library, executable and tests), a doxyfile for documentation of the code(click on the download button ->documentation, or go to CICD->pipelines and download there) and a logger configuration file (see easylogging help). Root further contains this README file and a separrate USER GUIDE. 

# COPYRIGHT notice

> Copyright (C)  2022 - present  Studio Animal-Aided Design
> Permission is granted to copy, distribute and/or modify this document  
> under the terms of the GNU Free Documentation License, Version 1.3  
> or any later version published by the Free Software Foundation;  
> with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.  
> A copy of the license is included in the section entitled "GNU  
> Free Documentation License".  

# Authorship  

This readme file has been written by Jens Joschinski. It is derived from the ECOLOPES PLANT MODEL readme.

The IBM-Plant model is derived from the ECOLOPES PLANT MODEL, which in turn is derived from Fate/RFate.
The "Authorship" section of individual files mentions the main contributor(s) to each file. If there is only a sole author or author group mentioned, then this person/group developed almost all (>90%) of the code. If there are further parties mentioned, their contributions are listed in detail in mostly chronological order. If an authorship statement is missing, all content was written by Jens Joschinski. 
The following classes have been written from scratch for the individual-based-model:  
Traits  
SoilRequirements  
Soil  
Seed Pool  
Seed Biology  
ResourceAlloc  
PlantShape  
PlantResource  
PlantGrowth  
LifeHistory  
Individual  
HabSuit  
Community  
Illumination  

The following classes are imported from PlantModel and have been altered:  
none  

The following classes are imported from PlantModel and are used without alteration:  
none  

The following classes are imported from PlantModel but are not used (they will eventually be deleted or used with or without alteration):  
rng  
PropPool  
plants  
PlantModel  
PFGDisturbance  
PfgDefs  
PFG  
main  
GSP_PLANTS  
FuncGroup  
FGUtils  
dist  
Data_PLANTS  
cohort  
Cell  
Of these classes, the following are used with minor/no alteration from Fate: PropPool , FGUtils, FuncGroup  
The following were written from scratch for the Ecolopes Plant Model: rng, plants, PFGDisturbance, Data_Plants, GSP_Plants, main, PFGDefs  

# 0 Preface

The model description *loosely* follows the ODD (Overview, Design concepts, Details) protocol for describing individual- and
agent-based models (Grimm et al. 2006), as updated by Grimm et al. (2020). 

# 1 Purpose and Patterns

The plant model aims to predict the presence and abundance of plant individuals from multiple species or types. In contrast to the ECOLOPES PLANT MODEL, it includes Individuals, each with their specific resource and particular needs.

# 2 Entities, state variables, and scales 

## Spatial and temporal resolution  

FATE did not constrain spatial extent, temporal extent, or spatial resolution in any way, but was designed as a landscape scale model with annual time scale.
The ECOLOPES PLANT MODEL was originally built for use in urban environments, and it simulates community dynamics over the typical life time of a building envelope (annual time steps, 10-200 years, 1 m3 spatial resolution, 100x100x10 m spatial extent).
This model does not constrain spatial extent, temporal extent or spatial resolution in any way, but it does not contain any functions to model seasonal change. 

## Entities
| Entity | Description |
| ---    | --- |
| Environment | (urban) habitat containing voxel cells, global simulation parameters |
| voxel cell | a voxel cell containing a community of one or more plant individuals of different types (e.g. species); light; soil; and disturbances.|
| Individual | composed of an (aboveground) biomass and a (belowground) resource. Biomass converts light into resource, resource is spent on growth (height and biomass) and reproduction. Indidividuals cast shade, and can be disturbed. Individuals of different types (e.g. species) differ in traits.
| Traits | Each type of individual (e.g. species) contains a set of traits defining 1) Life history parameters, 2) resource allocation strategies, 3) seed biology, 4) disturbance response, 5) soil requirements and 6) shape.

### Individuals, seeds and traits

The principle agent is an individual plant. The plant has two life stages: a seed stage and the actual plant individual. 

The individual has the following attributes:
- it lives: it grows, reproduces and dies.
- it is photoautotrophic: it converts light into a (carbohydrate) resource; it lives from these resources
- it has a shape: it has a geometric representation, being able to e.g. cast a shade 
- it needs a place to live: the soil determines whether a plant can grow at all (habitat is suitable). Soil 
   (or space) is a limiting resource.
- it interacts with the environment: Stressors can affect each of the above properties,
   i.e. cause mortality, deplete resources or affect the soil.

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
 1) Life history parameters. Life history determines general change of plant shape (e.g. height growth) and demographics (maturity onset, lifespan).
 2) resource allocation strategies. Resource allocation determines the relative investment into seeds and biomass gain (e.g. light conversion efficiency, fecundity).
 3) soil requirements. Plants can only grow on certain types of soil, and the soil needs to meet some minimum requirements (minimum depth).
 4) disturbance response. Disturbance response defines which types of disturbance ("rabbit", "fire") affect the plant, and how strongly.
 5) seed biology. The seed biology determines the mortality of seeds, dormancy attributes, and germination rates.  

 In addition, the plant has a certain shape; the shape determines the amount of biomass at a specific height. 
 Shape does not change over time (a rectangular plant cannot become a lollipop tree), but its extent does (height, width)

There are a few implementation-specific notes affecting the general logic of the model:
- Plant shape is simplified to a rectangular shape. 
- Soil: soil has a capacity to simulate competition for space, but no other attributes that may be depleted. Hence, plants of different types compete for the same singular resource.
- disturbance: limited to depleting either the plant's biomass or its resources.  


### Voxel Cell
A voxel contains a soil, a plant community, and light.  
Soil is a shared resource, used by the Individuals (see above). It has a limited capacity to host plant individuals (individuals may differ in the space they occupy), a depth, and an identity. Depth is a fixed attribute, assigned at initialization of the simulation, while soil identity (soil class) is initially assigned but may change over the simulation.  
For the purpose of light calculations, the voxel can be conceptualized as a stratified cuboid. The dimensions and number of strata, as well as the abiotic shade provided in each stratum (e.g. by buildings, cliffs) are fixed at model initialization. Light beams that hit the community at an angle of 90 degrees first hit the uppermost strata and plants therein. The light is then distributed to all plant individuals living in it, according to the surface area they cover. Any light that is not absorbed by the plants in the uppermost strata is passed on to the next strata, and so on. If there is abiotic shading, the light entering each stratum is reduced by a percentage. 
The logic of light calculation follows RFate and the ECOLOPES PLANT MODEL, but with some improvements:
- like in ECOLOPES (but not RFate) the built environment reduces available light
- light is an integer and not a factor, this allows converting watt/m2 into resources in joule (at least in principle; parametrization is required)
- plants on the same stratum do compete for light; 
- plants still do shade themselves, but as the upper and lower  parts share resources, they do not kill themselves. 

The plant community can (still) outgrow the voxel cell. Plant biomass that spills into neighbouring voxels is considered lost (does not provide shade, does not collect light)
In contrast to the ECOLOPES plant model, light no longer can pass through neighbouring voxel cells (see issue #8).
For the correct setting of number of strata, see issue #10.
Disturbance to add

### Environment: microenvironment and management plans

# 3. Process overview and scheduling

# 4. Design concepts

## Basic principles

### Robust software engineering approach 

FATE follows field-specific software engineering principles of ecological models. See (Scheller et al., 2010; Vedder et al., 2021) for an overview of common approaches, potential drawbacks and risks.
The plant model improves on this and provides a platform prototype that is planned to be further extended in the future. It is, however, neither accurate nor correct ,as it inherits conceptual decisions from RFATE which may not work very well on a very high resolution.
>>>
This implementation adds extensive testing, proper inheritance structures and similar software engineering approahces, making the model more maintainable in the future. Moreover, it adds Individuals, which is an improtant step towards higher accuracy and realism.
>>>


## Emergence and interaction 

The key outcome of the simulation is the spatiotemporal change of community patterns, which emerges from the interaction of plants within each grid cell (particularly competition for light). On a landscape scale, abiotic factors (soil depth, soil class) impose a general community structure via habitat filtering, and these abiotic factors may be static (inputs) or dynamically changing (passed on from another model). The abiotic factors (shading, in particular) further affect some competition outcomes, leading to the emergence of complex environment-community relationships, but we consider these indirect effects secondary and weaker than those imposed by habitat filtering.*

## Adaptation 

The plant model does not contain any direct objective-seeking mechanisms, i.e. the agents do not possess the ability to choose among two or more alternative behaviors e.g. via learning or condition-dependent rules (phenotypic plasticity). The rules by which agents respond to the environment (the "genotypes" of the plant types) are instead provided by the user (trait definitions).

## Objectives

The plant model does not simulate adaptive behavior (fitness objectives). 

>>>
Nevertheless, indices at higher organizational structure (richness, mean biomass of grid cells) show how good or bad a community copes with its surrounding. Such indices can be used as fitness objectives in evolutionary solvers (e.g. Wallacei), allowing to optimize environments or building designs (phenotypes) such that one or multiple community indicators are maximized. Note that this procedure optimizes the "fitness" of building designs; the use of evolutionary algorithms in this case has nothing to do with evolution in the biological sense (apart from copying algorithms). More generally, the model outcomes can be used to assess how "fit" a plant community is for a particular purpose, and the results can be used to adapt/manage the landscape with a particular objective in mind.
>>>

## Interaction

Individual plants interact with one another, directly or indirectly, especially in the competition for light, which is explained in detail in the submodel section. Briefly, growth and death of plants continually changes light conditions, and, subject to light tolerances, plants may be outcompeted and die. This leads to turnover of plant communities. 

## Stochasticity 

In FATE, dispersal is random within "dispersal discs" that indicate how far seeds can travel. The radius of the dispersal discs (short-and long-distance dispersal) was an input parameter, and several distributions (neg exponential, uniform) can be used for the random dispersal.

>>>
The ECOLOPES PLANT MODEL contains two sources of stochasticity:  

At the end of the time step, all plant seeds are pooled and randomly redistributed over the simulation environment, together with the seeds that enter from the region (seed rain). This simplified form of dispersal was chosen over the pre-existing disc-based dispersal from FATE-HD, because the spatial extent of the site is expected to be smaller than the usual dispersal kernel of all PFGs. 

The initialization seeds a random number of plants (aged 0-1 years) to each cell where the habitat is generally suitable. The number of plants per age group follows a random uniform distribution between 0 and 100. 
>>>

stochasticity of this model to write.

# 5. Input and Output Data

Rewrite

# 6. Initialization

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
to do


# 8. Implementation

>>>
| concept | detail |
| --- | --- |
| Operating system | Portable to any common OS |
| Programming language | c++ 20 (or newer) |
| Precursors | FATE-HD, RFate; ECOLOPES PLANT MODEL; some code (data containers, inputs) shared with joint ECOLOPES models |


## Compiling as library or executable

The model can be compiled as a library (.dll or .dylib), so that it can easily be #include'd into another program. This requires copying the library file and the header file "plantmodel.h" to the target directory (or its "include" folder) and linking against it. The folder src contains all plant model source files, as well a CMake file that helps building the library.  The CMake file in the root directory (together with the main in root) provide a demonstration (**to do**).

There is furthermore a "tests" subfolder, in which unit tests according to the GoogleTest framework are conducted. The test procedure is also included in the cmake files. 
Alternatively, one can compile the source files together with a main.cpp (in src folder) to create a full executable. 

Because this is work in progress, it currently only works on macOS and linux.

## Dependencies and requirements

All dependencies are included in the /include folder (ECOLOPES joint model for data containers, nlohmann::json and easylogging). The model has no specific requirements to run (< 1 GB hard disc space, < 2GB RAM, any modern CPU), but compilation requires cmake and a c++20 compiler (it is tested llvm and MinGW's GNU compiler, but should also work on AppleClang). See USER_GUIDE.md for installation notes.


## Implementation overview

## Ideas for the future 

*Light calculation and 3D in the plant model* 

The plant model is, strictly speaking, only correct when all cells are horizontal and light enters at a 90° angle. This is unrealistic, in reality light comes in at a variety of angles (changing over the course of the day), and the distribution of angles is latitude dependent. When the cell is tilted, the distribution of angles is strongly affected by the aspect (north/east/south/west) of the cell. When light enters at an angle, it does not pass through all strata of a cell, but partly through strata of a neighbouring cell.
A solution to above problem is raytracing, I.e. following the sun rays and checking with which cells they collide. 

*addition of other shapes* 
