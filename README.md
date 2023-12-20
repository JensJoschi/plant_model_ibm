
# INTERNAL USE ONLY, NO PROPAGATION, CONVEYING OR DISTRIBUTION WITHOUT EXPLICIT PERMISSSION

This repository hosts the ECOLOPES Plant model. It is meant for use within the computational workflow (WP3) of ECOLOPES, but we may in the future publish it as stand-alone model for research use as well. 
The folder /src contains all source files of the model; a "tests.cpp" that allows conducting unit tests (currently testing only); and a CMake file that compiles the model both as a library (.dylib/.dll) and as executable for unit tests. 
The folder /include contains dependencies: nlohmann::json, easylogging++, and a few files from the ECOLOPES ECOLOGICAL MODEL
The folder /999-test includes a few files to run a simple test case of the plant model. 
root contains "maintest.cpp" with a main() function, and another CMAke file which compiles maintest.cpp and links it with the compiled plant model library (currently for testing only). Root also contains a doxyfile for documentation of the code(click on the download button ->documentation, or go to CICD->pipelines and download there). Lastly, root contains test.json, a configuration file telling the plant model global simulation parameters and where to find which data.

# important caveats
Currently the folder src includes some files that are dependencies (coming from ECOLOGICAL MODEL or easylogging++). They should be removed or moved to the include folder. 
The unit tests and the main do not contain much yet, were only used to test the build system. 
Some files belong to a new version of plant model and are not being used yet. 
PlantModel currently requires a vector<string> keys, which needs to be removed. 
Logging seems to work, but is ugly. Requires thorough cleanup.
It is yet to test whether the Plantlib.dylib file can be incorporated by an "external" project that is compiles elsewhere (e.g. ECOLOPES ECOLOGICAL MODEL). Also, we require builing it for all platforms.
A new gitlab-CI.yaml file needs to be written. 

# COPYRIGHT notice
*The current version of the model is for internal use only, and in contrast to the licensing statements in individual files, 
any distribution (propagating or conveying) is forbidden. *
I (Jens Joschinski) have not received clearance by the Project management board or any direct supervisor yet, thus any copyright I assume is contestable 
and may lie with TU Munich or Studio Animal-Aided-Design. This would invalidate any license statements I made and restore the license to 
"All Rights Reserved" 
Note that usage and distribution may infringe the copyright of the FATE authors, if the program is distributed without GPL licenses.
Additionally, Plant model is not yet licensed to use the ECOLOGICAL MODEL files, and distribution would similarly infringe the copyright of the ECOLOGICAL MODEL authors (Jens Joschinski, Victoria Culshaw, possibly TUM or SAAD). 
All of above is not legal advice but my own layman's interpretation.

# Authorship  
This readme file has been written by Jens Joschinski, integrating information from the general document, D4.1. (specifically the ecological modelling parts and the FG characterisation ) or comments in the FATE code.

The plant model is derived from Fate/RFate, but has been heavily modified. Some individual files still contain remnants of the original model or built on their ideas. 
The "Authorship" section of individual files mentions the main contributor(s) to each file. If there is only a sole author or author group mentioned, then this person/group developed almost all (>90%) of the code. If there are further parties mentioned, their contributions are listed in detail in mostly chronological order. If an authorship statement is missing, all content was written by Jens Joschinski. 

Currently some files are copied from the ECOLOGICAL MODEL. clear separation / movement into external library folder is required.

# 1 Abstract
To write

# 2 Introduction  

# Installation notes and settings:

outdated, requires an update. include compiling as either library or executable.

The ecological model requires at least c++ 20 to compile. It is guaranteed to compile on clang, appleClang and MinGW’s GNU compiler for windows (“gcc”). The program is best compiled with CMake, a CMakelists.txt is available (building with cmake . -G "MinGW Makefiles"   should help under windows). The repository on Gitlab contains compiled windows and linux executables: go to the Menu CI/CD->pipelines to find a download button for an up-to-date version. 

If building from source, here are the full command line instructions to install all programs and compile under linux: 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
apt-get update –yes 

apt-get install --yes cmake 

cd path/to/work_directory 

cmake -B build 

cmake --build build  

cd build 

./model 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
Full command line instructions to install all programs and compile on MacOS: 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Brew upgrade 

Brew install cmake 

Brew install llvm 

cd path/to/work_directory 

cmake -B build -DCMAKE_CXX_COMPILER=/usr/local/cellar/llvm/16.0.4/bin/clang++ 

cmake --build build  

cd build 

./model 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The LLVM compiler is needed, because AppleClang does not support some modern functions yet.
If the compiler is not available, follow the compile messages and change the offending lines directly in the code.

Full command line instructions to install all programs and compile under Windows: 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

choco install -y cmake 

$env:Path += ';C:\Program Files\CMake\bin' 

choco install mingw –y 

cd path/to/work_directory 

cmake -B build -G "MinGW Makefiles" 

cmake --build build  

model.exe 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CMake should resolve the current work directory and use that as a default when the model searches for inputs. If this does not work, running the model with the command line option 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
model.exe path/to/dir  
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

should work. Please make sure that your path/to/dir contains a filenames.json, as well as the subfolders written into filenames.json (by default, path/to/dir/0-INPUT and path/to/dir/0-RESULTS) If parallelization is required, follow the instructions in the CMakeLists.txt, and make sure to not use the apple Clang compiler. Note that currently no parallelization is used in the models. 

