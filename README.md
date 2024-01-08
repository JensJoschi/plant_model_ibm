

# INTERNAL USE ONLY, NO PROPAGATION, CONVEYING OR DISTRIBUTION WITHOUT EXPLICIT PERMISSSION

# Overview

This repository hosts the ECOLOPES Plant model. It is meant for use within the computational workflow (WP3) of ECOLOPES, but we may in the future publish it as stand-alone model for research use as well. 

The folder /src contains all source files of the model; a "tests.cpp" that allows conducting unit tests (currently for testing the test strategy only); and a CMake file that compiles the model both as a library (.dylib/.dll) and as executable for unit tests. 
The folder /include contains dependencies: nlohmann::json, easylogging++, and a few files from the ECOLOPES ECOLOGICAL MODEL
The folder /999-test includes files to run a simple test case of the plant model. 

root contains "maintest.cpp" with a main() function, and another CMake file which compiles maintest.cpp and links it with the compiled plant model library (currently for testing only). Root also contains a doxyfile for documentation of the code(click on the download button ->documentation, or go to CICD->pipelines and download there). Lastly, root contains test.json, a configuration file telling the plant model global simulation parameters and where to find which data. Root further contains this README file and a separrate USER GUIDE. 

# COPYRIGHT notice

> **The current version of the model is for internal use only, and in contrast to the licensing statements in individual files, any distribution (propagating or conveying) is forbidden.**>

I (Jens Joschinski) have not received clearance by the Project management board or any direct supervisor yet, thus any copyright I assume is contestable 
and may lie with TU Munich or Studio Animal-Aided-Design. This would invalidate any license statements I made and restore the license to 
"All Rights Reserved" 

Note that usage and distribution may infringe the copyright of the FATE authors, if the program is distributed without GPL licenses.
Additionally, Plant model is not yet licensed to use the ECOLOGICAL MODEL files, and distribution would similarly infringe the copyright of the ECOLOGICAL MODEL authors (Jens Joschinski, Victoria Culshaw, possibly TUM or SAAD). 
All of above is not legal advice but my own layman's interpretation.

In the future this document (README.md) will contain a GNU FDL license:
> Copyright (C)  2023  Jens Joschinski.  
> Permission is granted to copy, distribute and/or modify this document  
> under the terms of the GNU Free Documentation License, Version 1.3  
> or any later version published by the Free Software Foundation;  
> with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.  
> A copy of the license is included in the section entitled "GNU  
> Free Documentation License".  

# Authorship  

This readme file has been written by Jens Joschinski, integrating information from the general document, D4.1. (specifically the ecological modelling parts and the FG characterisation ) or comments in the FATE code.

The plant model is derived from Fate/RFate, but has been heavily modified. Some individual files still contain remnants of the original model or built on their ideas.  
The "Authorship" section of individual files mentions the main contributor(s) to each file. If there is only a sole author or author group mentioned, then this person/group developed almost all (>90%) of the code. If there are further parties mentioned, their contributions are listed in detail in mostly chronological order. If an authorship statement is missing, all content was written by Jens Joschinski. 

Currently some files are copied from the ECOLOGICAL MODEL. clear separation / movement into external library folder is required.

# 0 Preface

The ECOLOPES ECOLOGICAL MODE is a spatial-explicit model that models the interdependent spatial and temporal dynamics of the soil, microbiota, plants, and animals, as response to the regional species pool, the local abiotic conditions, the geometry of the building, the substrate used to design the ecolope, and the management. The biological units of the model are plant (PFG) and animal (AFG) functional groups and generic soil classes, which makes the model generalizable to all sets of conditions. The model is based on a multiscalar approach. The regional model determines which FGs of the species pool have a reasonable chance to colonize the ecolope according to its location in the city. The local model applies a second filter on these species based on the abiotic and biotic conditions delivered by the ecolope, and simulates demographic processes to predict the outcome of inter-and intra-specific interactions. The output of the models is a temporal sequence of plant-animal-soil community development.  The Plant Model is part of the larger ECOLOPES ECOLOGICAL MODEL, though it can also be used as a stand-alone tool.

The model description *loosely* follows the ODD (Overview, Design concepts, Details) protocol for describing individual- and
agent-based models (Grimm et al. 2006), as updated by Grimm et al. (2020). A proper and independent ODD-style description of the plant model will be written later.

# 1 Purpose and Patterns

The plant model aims to predict the presence and abundance on the ecolope of different generic Plant Functional Groups (PFGs) with different maturity stages (height). For extensive discussion of the predecessor of the Plant Model, please refer to the Fate-HD documentation (paper and appendices). In short, Fate HD is a landscape model dealing with forest and non-forests that: 

1. includes spatiotemporal dynamics 

2. considers multiple species or species groups in interactions; 

3. accounts for the processes shaping biodiversity distribution 
 
Fate - HD was not intended to predict plant abundances in absolute numbers, but instead provides guidance on relative shifts in community composition. 

**The plant model repurposes FATE with the following aims in mind:**

1. draw general correlations between geometry/environment and ecological functions. This allows creating general design rules, such as the (trivial) statment that "sloped surfaces do not support tree-dominated plant communities"; 
2. evaluate a specific building design and return plant community information to the (architect) user. The user is expected to visualize and evaluate the results and draw conclusions (likely helped by computer programs and statistical / ML approaches), potentially using information from use case 1.

**Throughout the remaining text, changes in comparison to FATE-HD are either highlighted in bold or placed in a blockquote**
>>> 
Within ECOLOPES, the model is expected to deliver "good enough" predictions on how building geometry alters community structure, such that the model can be relied upon to provide suggestions for improving building designs. The output of the plant model is also used as input for an allometric animal home range formation, which requires "reasonably accurate" biomass inputs. Thus, the model is required to return absolute numbers. It is, however, expected that an experienced human user group (architects and ecologists) will evaluate any design suggestion provided by the ECOLOPES toolchain, so a certain margin of error that is currently expected from the models can be accommodated. Furthermore, the current main aim of the model is to suggest a novel way of design, and to create and demonstrate the general applicability of a model-informed design workflow. Hence, not the results, but the pipelines matter. Accordingly, the by far largest changes in the plant model regard the technical implementation, not the concepts or theory. The model architecture was revised to achieve stronger encapsulation (classes no longer access data that is outside of their scope), modularization and information management. Major changes were also done to the way inputs are read and stored (separate Inputs class, runtime environment, no more use of GDAL) and exchanged (new data container). The model further contains more assertions and modified error checks.

Major conceptual changes include connections to other models, including: shading by external inputs and by neighbouring cells (competition); soil depth and soil classes (habitat suitability); a revision of how disturbances are read and saved; and simplified dispersal.
>>>

# 2 Entities, state variables, and scales 

## Spatial and temporal resolution  

FATE did not constrain spatial extent, temporal extent, or spatial resolution in any way, but was designed as a landscape scale model with annual time scale.

**The Plant Model was originally built for use in urban environments, and it simulates community dynamics over the typical life time of a building envelope. We assume that the simulation time ranges from 10 to 200 years, the spatial extent is in the order of 100 x 100 x 10 m, and the resolution are voxel cells of 1 m3.**
| Dimension | Value range |
| ---------- | ----------- |
| Spatial extent | **~100 x 100 m** |
| Temporal extent | **~25-100 years** | 
| Spatial resolution | **1 m x 1 m** |
| Temporal resolution | 1 year |

>>>
The model simulates 1x1x1 m voxels in a 3D landscape.  Not all cells need to be present (e.g. building interior, air between buildings), and undercuts or balconies can also be modelled. The maximum extent in x-, y-and z- direction is user-defined. Each voxel is a cube in which a plant community can thrive. The voxel itself is stratified into 4 layers. Plants differ in maximum sizes and growth patterns, but will generally grow thorugh those four layers. Any 3D dynamics (competition for light) are expected to happen within the 1 m3 voxel, among voxels of the same horizontal plane, or among all voxels but without considering space (random dispersal of plants), i.e. there is no interaction among stacked voxels.>

Despite the expectations mentioned above, one can nevertheless use the model outside urban context, and one can change both the simulation duration and the spatial extent. Regarding the resolution, the model itself does not incorporate it into any calculations. If assuming a different resolution, one might have to choose different values in the PFG parameters and global parameters though. Also, thinking that all seeds disperse freely across the site becomes quite a stretch. Additionally, the plant model will be at some point parametrized for a square meter resolution, and the output is then based on these expectations. 
If the model resolution is much smaller than the size of one PFG, the results become very awkward as well, because the model does not provision for the case that plants grow out of their voxel cell.
>>>

The model does not incorporate seasonality and does not include fine-scaled modelling of growth processes within a year, so a temporal resolution below 1 year is impossible.

## Entities
| Entity | Description |
| ---    | --- |
| Environment | (urban) habitat containing grid cells, global simulation parameters **and regional model information** |
| Grid cells | a 1x1 m cell with **a soil class, soil depth,** disturbances, and 4 layers. Each layer contains a different amount of light, and the abundances of different Plant Functional Groups. **Soil depth and soil class** impose constraints on which PFG can live here, and the disturbances can affect their growth. |
| Functional Group definitions | ~600, a data input. Defines habitat suitability, growth, competition and disturbance attributes. |

### Individuals, grid cells, and functional groups 

The principle agent is not an individual plant but a functional group that lives with other functional groups in a grid cell (A functional group is a group of species with similar traits, see section Design Concepts). The grid cell contains information about the biomass of multiple Plant Functional Groups in multiple layers. 

It is expected that the grid size (spatial resolution) is much larger than the diameter of the largest Plant Functional Group, as the shading provided by plant biomass does not cross grid cell boundaries. When this expectation is violated, and the environment is highly heterogenous, larger plants would be modelled to occupy only a single m2 patch. 

The plant biomass of the higher levels will additionally shade lower levels, and when the grid size is not much larger than the PFG size this causes an unrealistic inter-individual competition for light. See issue #21 and #29.

### Environment: microenvironment and management plans 

>>>
The topographic features of the environment are very important in a rugged urban environment. We argue that topography affects the community in two fundamentally different ways: 

1. by limiting dispersal, and 
2. by altering the microenvironment. 

Microenvironmental changes are captured via shading and soil depth variables, which map the effect of a 3D building geometry to individual cells (Shading in cell A is affected by the high rise building covering cells B and C). The ecological model uses these cell-wise representations, making the ecological communities respond to 3D topography indirectly. Future model developments will include more direct 3D effects, such as non-horizontal surfaces, or natural daylighting. They will also allow for further microenvironmental influences; the dispersal aspect will also be solved in a future version of the model.   

Another feature of urban environments is the strong influence of human use and human management plans. Management plans can, for example, target specific functional groups (or groups of functional groups) and remove their biomass (e.g., tree pruning, mowing), or kill individuals. Such management masks can in principle also be used to simulate human disturbance (e.g. trampling disturbing herb functional groups), and they can even be used to include further effects of the environment that are currently not part of the model (e.g. water runoffs causing waterlogging, which affects PFGs differently; see issue #24). A protoype version of management is included in the model. 

**The inheritance pattern of the inputs classes needs to be clarified (Plant Model should inherit from BASE, and joint model should inherit from BASE; currently BASE includes joint model stuff instead). Further information regarding the environment will be placed here afterwards.**
>>>

# 3. Process overview and scheduling

! [Process overview and schduling] (doc/process_overview.png)  
*Process overview (draft). Notes: 1. the order doesn’t seem to make sense, but is copied from RFate; 2. processes match function names of code(see UML diagram). Or they should at least, I keep changing them.*

# 4. Design concepts

## Basic principles

### Modelling framework

FATE, from which the Plant Model is derived, combines phenomenological and process-based approaches: it first considers abiotic environmental filters 
to determine where each agent can in principle live, and then simulates their growth and demographic structure, including competition among agents. 

**The Plant Model adds flexibility to the phenomenological approach, and allows users to derive habitat suitabilities instead based on other (process-based) models. It further lets the processes be influenced by observable inputs. For example, competition outcomes are now influecned by the total amount of light/shade that is provided in a grid cell.**

FATE is an agent-based model, but not implemented as a traditional ecological individual-based model. In agent-based models, simple rules are set up for individual entities (agents), and the interaction among agents causes complex behaviors to emerge. In Ecology the agents are usually individual cells or individuals of a population, which may for example grow, compete, reproduce and die. Individual agents are nested in habitats, populations or environments (multiple levels of nesting may occur), and the nesting is usually reflected in a Object-Oriented Programming (OOP) approach: individual instances are created (birth) or deleted (death), and may be reassigned to other instances of higher-order objects (dispersal). The modular structure of the OOP approach allows modification of agent rules easily. In FATE, however, the principal agent is the Plant Functional Group (similar to a plant species, see below), which is nested within a plant community of a grid cell, which in turn is nested in an environment. The Plant Functional Group is altered by the environment and changes its demographic composition according to simple rules, but the Plant Functional Group itself does neither move, nor be born or die. Accordingly, there is a single instance of each Functional Group  within each grid cell, which is constructed at initialization and only deleted at program termination. 

This approach is computationally more feasible, but comes at the cost of model resolution and accuracy (only the community composition, but not individual plant development can be tracked). The biomass is hence not meant to represent an accurate absolute value, but shall be interpreted as a probability to find a certain plant (for relative comparisons among functional groups). **A future version of the plant model will implement Individuals, thus alleviating some of the current constraints and limitations (see issue #29).**

### Functional Groups

The model does not simulate individual species but functional groups. A functional group can be defined as a group of species that has a similar functional niche owing to a convergent ecological strategy. In this, the model results will allow generalisations based on the sensitivities of multiple species to different environmental conditions (Blaum et al. 2011). Furthermore, assessments of FG richness instead of simply measuring species richness in a single species approach is favourable since FG are directly related to traits rather than to an evolutionary based taxonomic classification, which may not relate to the capacity of a particular species to provide a specific ecosystem service (Tilman et al. 1997). 

>>>
Current research practices define functional groups ad-hoc and in a non-systematic, case-specific way. We do instead provide a default set of very general PFGs (Plant Functional Groups), which is embedded in a set of matching Animal Functional Groups, PFGs and soil classes. PFGs, their attributes, and interactions with other modelling entities (soil classes, disturbances) can easily be changed by the user, enabling for example the modelling of any trophic structure and multiple external pressures (e.g. management). Nevertheless, we generally caution against changing PFG attributes in most use cases, because it requires reparametrization and validation. *Shortening* and reducing the number of PFGs and available soil classes is possible, and often advisable (currently by deleting some PFG definitions, later by using a regional model).
>>>

### Robust software engineering approach 

FATE follows field-specific software engineering principles of ecological models. See (Scheller et al., 2010; Vedder et al., 2021) for an overview of common approaches, potential drawbacks and risks.

>>>
The Plant Model is specifically designed as a platform prototype that is planned to be further extended in the future. It is thus neither accurate nor neccessarily correct, but built to be consistent, maintainable and extensible. The model thus employs robust software engineering principles that are not commonplace in ecological research (Scheller et al., 2010; Vedder et al., 2021), including, e.g. free and open code, modularization according to OOP best practices, a CI/CD system with automated unit tests, and up-to-date documentation of the code with doxygen. Conversion of the code is not finished, however (see issues #25-#31)
>>>


## Emergence and interaction 

The key outcome of the simulation is the spatiotemporal change of community patterns, which emerges from the interaction of plant functional groups within each grid cell (particularly competition for light). On a landscape scale, abiotic factors (soil depth, soil class) impose a general community structure via habitat filtering, **and these abiotic factors may be static (inputs) or dynamically changing (passed on from another model). The abiotic factors (shading, in particular) further affect some competition outcomes, leading to the emergence of complex environment-community relationships, but we consider these indirect effects secondary and weaker than those imposed by habitat filtering.**

## Adaptation 

The plant model does not contain any direct objective-seeking mechanisms, i.e. the agents do not possess the ability to choose among two or more alternative behaviors e.g. via learning or condition-dependent rules (phenotypic plasticity). Two grid cells with initially equal biomasses of each PFG will always develop exactly the same community, if no new seeds land via dispersal (**Dispersal is also entirely random and uninformed**). The rules by which agents respond to the environment (the "genotypes" of the Functional Groups) are instead provided by the user (PFG Definitions).

## Objectives

The plant model does not simulate adaptive behavior (fitness objectives). 

>>>
Nevertheless, indices at higher organizational structure (richness, mean biomass of grid cells) show how good or bad a community copes with its surrounding. Such indices can be used as fitness objectives in evolutionary solvers (e.g. Wallacei), allowing to optimize environments or building designs (phenotypes) such that one or multiple community indicators are maximized. Note that this procedure optimizes the "fitness" of building designs; the use of evolutionary algorithms in this case has nothing to do with evolution in the biological sense (apart from copying algorithms). More generally, the model outcomes can be used to assess how "fit" a PFG community is for a particular purpose, and the results can be used to adapt/manage the landscape with a particular objective in mind.
>>>

## Interaction

Currently, individual agents (PFGs) do not interact with one another, directly or indirectly, except in the competition for light, which is explained in detail in the submodel section. Briefly, growth and death of plants continually changes light conditions, and, subject to light tolerances, plants may be outcompeted and die. This leads to turnover of plant communities. 

## Stochasticity 

In FATE, dispersal is random within "dispersal discs" that indicate how far seeds can travel. The radius of the dispersal discs (short-and long-distance dispersal) was an input parameter, and several distributions (neg exponential, uniform) can be used for the random dispersal.

>>>
The plant model contains two sources of stochasticity:  

At the end of the time step, all plant seeds are pooled and randomly redistributed over the simulation environment, together with the seeds that enter from the region (seed rain). This simplified form of dispersal was chosen over the pre-existing disc-based dispersal from FATE-HD, because the spatial extent of the site is expected to be smaller than the usual dispersal kernel of all PFGs. 

The initialization seeds a random number of plants (aged 0-1 years) to each cell where the habitat is generally suitable. The number of plants per age group follows a random uniform distribution between 0 and 100. 
>>>

# 5. Input and Output Data
Plant inputs and outputs were revised in comparison to FATE-HD.

>>>
The plant model requires the following information to run:
- configuration parameters (e.g. how many years to simulate)
- Definition of Functional Groups, including their response to disturbances (e.g. management, animals) and soil (soil classes, depth requirements)
- microenvironmental information, currently the amount of shading and the soil depth in each cell. 

The output of the model is the biomass of each PFG in each grid cell, for each time step that was specified. A list of time steps to output can be provided via the configuration parameters.

See USER_GUIDE.md for details on input and output data formats and their use.
>>>

# 6. Initialization

>>>
Upon startup, the model reads all inputs, and then reserves memory and creates all PFGs in all grid cells. 
First, the model reads the input file, which contains the file name of the global simulation parameters(“GlobSimulFile”), and the file name of the data file (“DATA”). The model then reads and checks the information in those two files as well. Using the information in the global simulation parameters and in the DATA file, the model checks the data content, and builds and initializes the grid cells and their Functional Groups. 

Building the Functional groups includes seeding a random number of plants (aged 0-1 years) to each cell where the habitat is generally suitable. The number of plants per age group follows a random uniform distribution between 0 and 100.

During initialization, the plant model runs 5 iterations. Running the model for several iterations ensures a buildup of a reasonable plant community, as the grid cells are initially seeded with only 0-1 year old plant material.
>>>

# 7. Submodels


**The modelling approach contains three main processes: habitat suitability, succession, and dispersal. (note that this description of processes contradicts FATE, which sees disturbance and light competition as separate modules.)**

## Habitat suitability 

The habitat suitability module checks for each PFG in which cells it can theoretically occur (habitat filtering), based on a static user input.

>>>
The habitat suitability module now uses a soil class that can be provided as a static input by the user or as a dynamically changing property provided by a different model. The soil class of each cell is compared with the soil class requirements of each PFG (each PFG can potentially live on multiple soil classes), and it also checks whether the soil depth is deeper than the minimum requirement of each PFG. The outcome of this comparison indicates the suitability of the cell for each PFG.
>>>

The suitability of a habitat impacts whether seeds can germinate. Already established plants will not be destroyed if a habitat becomes unsuitable.  

## Succession (*to be renamed!*)

Succession describes the growth processes that happen within a year inside a single grid cell. It consists (in this order) of disturbance, death due to insufficient light, aging, calculation of current light resources, germination and recruitment, and reproduction. 

The processes largely describe processes affecting individuals, yet the model's smallest unit are Functional Groups, i.e. the processes work on a demographic level.

**The implementation of succession was revised, but the general calculations have not changed (yet). See workflow diagrams for sequence of events. Long-term Seed dormancy is disabled, however.**

### Disturbance  

The disturbance model allows to provide a map (for one disturbance) or several maps (several disturbances) for each PFG and each stage (juvenile, mature, senescent) that can either cause a given % of mortality or a given % of seed production. The lost material may, however ,grow back in the following years, depending on the type of disturbance. 

>>>
We make two kinds of disturbance possible: 
1. the user may provide a constant disturbance map that works the same way as in the original version, and 
2. the user may provide a new map every time step. The dynamic maps are not read from files, but passed on from another model as c++-internal objects. 

In the PFG definitions one can define the effect each kind of disturbance has on each PFG. The constant maps represent management decisions such as annual tree pruning, while the dynamic maps are, for example, annually changing animal biomasses. Currently only one of the two disturbances can be used at a time. 
>>>

### Death due to insufficient light

This function works on the light conditions of the preceding year. Light is expressed as a vector of factors, classifying the light in each stratum as low, intermediate or high. It is tested for each PFG in each growth stage (seed, germinant, juveniles, matures)  whether the light tolerance matches the current light conditions (the 12 light tolerance parameters, I.e., low, med and high for each growth stage, are provided in the PFG definitions). Plant material that does not tolerate the current light conditions dies.

### Aging

The description of the demography of the functional group is updated, according to the LifeSpan and maturity attributes of the PFG. Material that exceeds the life span of a PFG is removed. 


### calculation of current light resources

Conceptually, the light is calculated for the uppermost stratum according to the biomass of the PFGs living in it. The light that is not consumed is passed on to the next stratum, being reduced in the same fashion until it reaches zero or arrives at ground level (stratum 0). Consumption of light depends on a shading factor **(not to be confused with the input “shading”)** that represents leaf and growth form, maturity of the plant, and on the biomass that the plant reaches (the maximum biomass that a plant can reach differs between small, medium and large plants, and the abundance class is another PFG attribute). The amount of light in each stratum is converted into a factor (low, medium and high light conditions). 

Technically, a ray of shading passes through the cell. It collects any (leaf-form and maturity- corrected) plant abundance it encounters. In each level the shading ray is compared against a light threshold parameter, and if smaller than the "low" or "medium" threshold, the light resource declines to the according level

>>>
In contrast to the original Rfate/Fate HD implementation, the threshold against which the shading ray is compared is corrected by a shading index (a proportion ranging from 0 = full sun to 1 = full shade) in our model. This ensures that shaded grid cells fall earlier to "medium" or "low" light conditions, though the uppermost stratum can never be converted into lower light levels (see issue #20). Furthermore, light may fall at an angle below 90°. If this is the case, if will partially pass through the neighbouring cell and is accordingly reduced by the neighbour’s plant abundance. The calculation is as following:

The modelled grid cell is actually a box (voxel) that is stratified. Light will enter into stratum i, get depleted by the plant biomass, and the remaining light enters stratum i-1 etc. (Figure, left). When light enters at an angle (figure, right), a proportion bypasses the upper layer. Knowing the height of the stratum (a) and the angle at which light enters(ß), we can calculate the length at which light hits the ground as x = a/tan(ß) (x is actually an area, not a line, but if light enters exactly from south/east/etc., the calculation does not change). With a grid cell size of 1m, the area not covered by light is y= 1-x. The light falling on the ground is thus y * (light from above) + x * (light from above in neighbouring cell). We can repeat this calculation for every layer. The angle ß is a simulation parameter that can be changed, but the direction of incoming light is fixed at  x  = - inf.*
>>>

### germination and recruitment

To do

### reproduction

To do


## Dispersal  

In Fate-HD the dispersal model was based on three parameters: d50 that is the maximum distance within which 50% of the seeds are dispersed, d99 is the maximum distance within which 99% of the seeds are found in total, and ldd is the maximum long-distance dispersal. Within each dispersal disc, the seeds could be distributed uniformly or decreasing exponentially with distance from the plant. 

**Plant dispersal has been simplified in comparison to Fate: all seeds produced in a year are pooled, and then redistributed randomly across the site at the end of the year.**


# 8. Implementation

>>>
Operating system | Portable to any common OS 
Programming language | c++ 20 (or newer) 
Precursors | FATE-HD, RFate. The model was previously developed in a joint ECOLOPES repository; as a result of joint development and intended interaction, 
some components of the Plant Model (e.g. data containers) are shared with/inherited from the ECOLOPES joint model (written by the same author(s)).
>>>

## Dependencies and requirements

## Compiling as library or executable

## Warnings and limitations

The model is not fully parametrized and not validated. Inputs are assumed to be coarse approximations or simplifcations, and the model accordingly only suggests relative differences among FGs. The model is not yet thoroughly tested and currently meant to be used only for academic purposes. 