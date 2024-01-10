# COPYRIGHT notice

> **The current version of the model is for internal use only, and in contrast to the licensing statements in individual files, any distribution (propagating or conveying) is forbidden.**

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
This file has been written by Jens Joschinski.

**This file requires an update, to include instructions to compile as standalone or as executable.**

# Installation notes and settings

The plant model requires at least c++ 20 to compile. It is guaranteed to compile on clang, and MinGW’s GNU compiler for windows (“gcc”). The program is best compiled with CMake, a CMakelists.txt is available (building with cmake . -G "MinGW Makefiles"   should help under windows). The repository on Gitlab contains compiled windows and linux executables: go to the Menu CI/CD->pipelines to find a download button for an up-to-date version. 

If building from source, here are the full command line instructions to install all programs and compile under linux: 

```
apt-get update –yes 

apt-get install --yes cmake 

cd path/to/work_directory 

cmake -B build 

cmake --build build  

cd build 

./model 
```
 

Full command line instructions to install all programs and compile on MacOS: 

```
Brew upgrade 

Brew install cmake 

Brew install llvm 

cd path/to/work_directory 

cmake -B build -DCMAKE_CXX_COMPILER=/usr/local/cellar/llvm/16.0.4/bin/clang++ 

cmake --build build  

cd build 

./model 
```
 

  

Full command line instructions to install all programs and compile under Windows: 

```
choco install -y cmake 

$env:Path += ';C:\Program Files\CMake\bin' 

choco install mingw –y 

cd path/to/work_directory 

cmake -B build -G "MinGW Makefiles" 

cmake --build build  

model.exe 
```
 

Running 

CMake should resolve the current work directory and use that as a default when the model searches for inputs. If this does not work, running the model with the command line option 

model.exe -w path/to/dir  

should work. Please make sure that your path/to/dir contains a filenames.json, as well as the subfolders written into filenames.json (by default, path/to/dir/0-INPUT and path/to/dir/0-RESULTS)  

  

# Example 

In the following I assume that the program has been successfully built and that the root contains the file “test.json”. The json files can be opened with any text editor, but opening with VSCode or Mozilla Firefox ensures that the files are displayed in a pretty format. The description is written for version 1.3 but something equivalent should also work with 2.4/2.5. 

The test.json contains “GlobSimulFile”: "test.json", and “DATA”: “test.json” This means that both the “GlobSimulFile” (a file with global simulation parameters) and the data file (inputs) are the same file as the one currently open. The file does NOT contain a “PlantInputs”/”SoilInputs”/”AnimalInputs” entry, so these are set to “test.json” as well (a warning will be issued). This file hence contains all data entries (general and submodels), and all configuration entries (general and submodels). It includes, among other variables, information about the simulation duration (10 iterations) and the number of PFGs. The next lines  specify the input directory “999-test” and the output directory (which may not yet exist under root/). This information is followed by information about the functional groups that are present in the region (currently these are simply all groups that will also be defined by the submodels, but they may in the future also be only a subset of all possible groups). If attempting to change this information, make sure that the names given here match exactly with the information the submodels give, otherwise an error occurs. The file further contains the filenames of the microenvironemntal parameters, soil, shading and soil depth. The soil file is straightforward, go to root/999-test/ninesoil.json to have a look. You will find nine coordinate-triplets, each with a soil class associated. The shading and depth files are coded differently in this example. Go to root/999-test/nine.json to check the file content. There are also 9 coordinate keys in the file, but each key is associated with two values (called SHADING and DEPTH). Hence, the entry in test.json has two fields per microenvironmental variable: a file name (nine.json in both cases) and a field name (“SHADING” or “DEPTH”). The last two important entries in test.json are the PFG and soil definitions. The PFG definitions (root/999-test/fullPFG.json) is rather complex. It starts with the entry “aquatic_10” which is approximately 1 page long, folled by “aquatic_11”. In total there are 107 such PFG definitions in this file, each with the same fields. See table 2 for details what the individual entries mean. The last file is the soil transition matrix (root/999-test/unchanging_soil.json). This file first shows the transition probabilities of each soil class, starting with “Lo_l_2”. The class Lo_l_2 has a probability of 1 to remain the same, and zero probability to transition into any other soil class. A separate R script to help wirting a transition matrix is available (root/Rscripts/transition_matrix.r). All other entries of the test.json belong to the other submodels and are explained in the respecitve sections. 

Having read the input information, we know that the model will run with 107 PFGs and 42 soil classes, for 10 iterations and with all submodels enabled, on a 3x3m ecolope. We now hit  

“model.exe -f test.json” (if running under linux or MacOS: “./model -f test.json”). The model will finish within one second, but relevant information has been printed on the terminal (and, from 2.4 upwards, also in separate log files). This includes the reading and content of major files, warnings encountered while trying to read, and information regarding the time steps. The model saves various information while running, and concatenates the information into json files at the end. The results can be explored unter root/testRESULTS, which is the output directory that was written into test.json. The model currently saves the plant biomasses as seen by the animal model as a .txt file for each time step (e.g. Resource_aquatic_6_T0.txt). Each of these files contains 9 values, corresponding to the 9 grid cells of the site. The animal resources as seen by the animal model are saved accordingly ( e.g. Resource_Weasel_T7.txt). The model also saves the plant biomass, animals and soil classes as json files for each time step (t_0aquatic_6_plantbiomass.json), and further produces a joint animal file (animalbiomass.json), a joint plant file (plantbiomass.json) and a joint soilclass file (soil.json). Separate R scripts to visualize these results should be available (not all done yet) 